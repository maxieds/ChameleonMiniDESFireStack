/* 
 * DESFirePICCControl.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */ 

#ifndef __DESFIRE_PICC_CONTROL_H__
#define __DESFIRE_PICC_CONTROL_H__

#include "../../Configuration.h"

#include "DESFireFirmwareSettings.h" 
#include "DESFirePICCHeaderLayout.h"
#include "DESFireInstructions.h"
#include "DESFireApplicationDirectory.h"
#include "DESFireFile.h"
#include "DESFireCrypto.h"

/*
 * Internal state variables: 
 */

typedef struct {
    BYTE  Slot;
    BYTE  KeyCount;
    BYTE  FileCount;
    BYTE  CryptoCommStandard;
    SIZET KeySettings;            /* Block offset in EEPROM */
    SIZET FileNumbersArrayMap;    /* Block offset in EEPROM */ 
    SIZET FileCommSettings;       /* Block offset in EEPROM */
    SIZET FileAccessRights;       /* Block offset in EEPROM */
    SIZET KeyVersionsArray;       /* Block offset in EEPROM */
    SIZET FilesAddress;           /* Block offset in EEPROM */
    SIZET KeyAddress;             /* Block offset in EEPROM */
    UINT  DirtyFlags;
} SelectedAppCacheType;

extern BYTE SELECTED_APP_CACHE_TYPE_BLOCK_SIZE;
extern BYTE APP_CACHE_KEY_SETTINGS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_FILE_NUMBERS_HASHMAP_BLOCK_SIZE;
extern BYTE APP_CACHE_FILE_COMM_SETTINGS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_FILE_ACCESS_RIGHTS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_KEY_VERSIONS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_KEY_BLOCKIDS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_FILE_BLOCKIDS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_MAX_KEY_SIZE;

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    /* AppData keeping track how many keys each app has */
    DESFIRE_APP_KEY_COUNT,
    /* AppData active file count */
    DESFIRE_APP_FILE_COUNT,
    /* AppData keep track of default crypto comm standard */
    DESFIRE_APP_CRYPTO_COMM_STANDARD, 
    /* AppData keeping track of apps key settings */
    DESFIRE_APP_KEY_SETTINGS_BLOCK_ID,
    /* AppData hash-like unsorted array mapping file indices to their labeled numbers */
    DESFIRE_APP_FILE_NUMBER_ARRAY_MAP_BLOCK_ID,
    /* AppData communication settings (crypto transfer protocols) per file */
    DESFIRE_APP_FILE_COMM_SETTINGS_BLOCK_ID,
    /* AppData file access rights */
    DESFIRE_APP_FILE_ACCESS_RIGHTS_BLOCK_ID, 
    /* AppData keep track of newer EVx revisions key versioning schemes */
    DESFIRE_APP_KEY_VERSIONS_ARRAY_BLOCK_ID,
    /* AppData keeping track of apps file index blocks */
    DESFIRE_APP_FILES_PTR_BLOCK_ID,
    /* AppData keeping track of apps key locations */
    DESFIRE_APP_KEYS_PTR_BLOCK_ID,
} DesfireCardLayout;

typedef struct DESFIRE_FIRMWARE_PACKING {
    BYTE Num;
    DESFireFileTypeSettings File;
} SelectedFileCacheType;

extern SIZET DESFIRE_PICC_INFO_BLOCK_ID;
extern SIZET DESFIRE_APP_DIR_BLOCK_ID;
extern SIZET DESFIRE_APP_CACHE_DATA_ARRAY_BLOCK_ID;
extern SIZET DESFIRE_INITIAL_FIRST_FREE_BLOCK_ID;
extern SIZET DESFIRE_FIRST_FREE_BLOCK_ID;
extern SIZET CardCapacityBlocks;

/* Cached data: flush to FRAM or relevant EEPROM addresses if changed */
extern DESFirePICCInfoType Picc;
extern DESFireAppDirType AppDir;

/* Cached app data */
extern SelectedAppCacheType SelectedApp;
extern SelectedFileCacheType SelectedFile;

typedef void (*TransferSourceFuncType)(BYTE *Buffer, BYTE Count);
typedef void (*TransferSinkFuncType)(BYTE *Buffer, BYTE Count);
typedef void (*TransferChecksumUpdateFuncType)(const BYTE *Buffer, BYTE Count);
typedef BYTE (*TransferChecksumFinalFuncType)(BYTE *Buffer);
typedef BYTE (*TransferEncryptFuncType)(BYTE *Buffer, BYTE Count);
typedef TransferStatus (*PiccToPcdTransferFilterFuncType)(BYTE *Buffer);
typedef BYTE (*PcdToPiccTransferFilterFuncType)(BYTE *Buffer, BYTE Count);

/* Stored transfer state for all transfers */
typedef union DESFIRE_FIRMWARE_PACKING {
    struct DESFIRE_FIRMWARE_ALIGNAT {
        BYTE NextIndex;
    } GetApplicationIds;
    struct DESFIRE_FIRMWARE_ALIGNAT {
        TransferChecksumUpdateFuncType UpdateFunc;
        TransferChecksumFinalFuncType FinalFunc;
        BYTE AvailablePlaintext;
        struct {
            BYTE                   BlockBuffer[CRYPTO_MAX_BLOCK_SIZE];
            TDEACryptoCBCFuncType  TDEAChecksumFunc;
            AESCryptoCBCFuncType   AESChecksumFunc;
            SIZET                  CRCA;
        } MACData;
    } Checksums;
    struct DESFIRE_FIRMWARE_ALIGNAT {
        SIZET BytesLeft;
        struct DESFIRE_FIRMWARE_ALIGNAT {
            TransferSourceFuncType Func;
            SIZET Pointer; /* in FRAM */
        } Source;
        struct DESFIRE_FIRMWARE_ALIGNAT {
            BOOL FirstPaddingBitSet;
            TransferEncryptFuncType Func;
            BYTE AvailablePlaintext;
            BYTE BlockBuffer[CRYPTO_MAX_KEY_SIZE];
        } Encryption; 
    } ReadData;
    struct DESFIRE_FIRMWARE_ALIGNAT {
        SIZET BytesLeft;
        struct DESFIRE_FIRMWARE_ALIGNAT {
            TransferSinkFuncType Func;
            SIZET Pointer; /* in FRAM */
        } Sink;
        struct DESFIRE_FIRMWARE_ALIGNAT {
            TransferEncryptFuncType Func;
            BYTE AvailablePlaintext;
            BYTE BlockBuffer[CRYPTO_MAX_BLOCK_SIZE];          
        } Encryption;
    } WriteData;
} TransferStateType;

extern DesfireStateType DesfireState;
extern uint8_t AuthenticatedWithKey;

void SyncronizePICCInfo(void);

/* Transfer routines */
TransferStatus PiccToPcdTransfer(uint8_t *Buffer);
uint8_t PcdToPiccTransfer(uint8_t *Buffer, uint8_t Count);

/* Setup routines */
uint8_t ReadDataFilterSetup(uint8_t CommSettings);
uint8_t WriteDataFilterSetup(uint8_t CommSettings);

/* PICC management */
void InitialisePiccBackendEV0(uint8_t StorageSize);
void InitialisePiccBackendEV1(uint8_t StorageSize);
void ResetPiccBackend(void);
bool IsEmulatingEV1(void);
void GetPiccHardwareVersionInfo(uint8_t* Buffer);
void GetPiccSoftwareVersionInfo(uint8_t* Buffer);
void GetPiccManufactureInfo(uint8_t* Buffer);
uint8_t GetPiccKeySettings(void);
void FormatPicc(void);
void CreatePiccApp(void);
void FactoryFormatPiccEV0(void);
void FactoryFormatPiccEV1(uint8_t StorageSize);
void GetPiccUid(ConfigurationUidType Uid);
void SetPiccUid(ConfigurationUidType Uid);

#endif
