/* 
 * DESFirePICCControl.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */ 

#ifndef __DESFIRE_PICC_CONTROL_H__
#define __DESFIRE_PICC_CONTROL_H__

#include "DESFireFirmwareSettings.h" 
#include "DESFirePICCHeaderLayout.h"
#include "DESFireFile.h"

/*
 * Internal state variables: 
 */

typedef struct DESFIRE_FIRMWARE_PACKING {
    BYTE  Slot;
    BYTE  KeySettings;
    BYTE  KeyCount;
    SIZET FilesAddress; /* in FRAM */
    SIZET KeyAddress;   /* in FRAM */
    UINT  DirtyFlags;
} SelectedAppCacheType;

typedef struct DESFIRE_FIRMWARE_PACKING {
    BYTE Num;
    DesfireFileType File;
} SelectedFileCacheType;

extern SIZET CardCapacityBlocks;

/* Cached data: flush to FRAM if changed */
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
    struct DESFIRE_FIRMWARE_PACKING {
        BYTE NextIndex;
    } GetApplicationIds;
    struct DESFIRE_FIRMWARE_PACKING {
        TransferChecksumUpdateFuncType UpdateFunc;
        TransferChecksumFinalFuncType FinalFunc;
        BYTE AvailablePlaintext;
        union {
            struct {
                CryptoTDEACBCFuncType MACFunc;
                BYTE BlockBuffer[CRYPTO_DES_BLOCK_SIZE];
            } MAC;
            SIZET CRCA;
        };
    } Checksums;
    struct DESFIRE_FIRMWARE_PACKING {
        SIZET BytesLeft;
        struct DESFIRE_FIRMWARE_PACKING {
            TransferSourceFuncType Func;
            SIZET Pointer; /* in FRAM */
        } Source;
        struct DESFIRE_FIRMWARE_PACKING {
            BOOL FirstPaddingBitSet;
            TransferEncryptFuncType Func;
            BYTE AvailablePlaintext;
            union {
                struct {
                    BYTE BlockBuffer[CRYPTO_DES_BLOCK_SIZE];
                } TDEA;
            };
        } Encryption; // TODO: More types of encrypted comms to be added
    } ReadData;
    struct DESFIRE_FIRMWARE_PACKING {
        SIZET BytesLeft;
        struct DESFIRE_FIRMWARE_PACKING {
            TransferSinkFuncType Func;
            SIZET Pointer; /* in FRAM */
        } Sink;
        struct DESFIRE_FIRMWARE_PACKING {
            TransferEncryptFuncType Func;
            BYTE AvailablePlaintext;
            union {
                struct {
                    BYTE BlockBuffer[CRYPTO_DES_BLOCK_SIZE];
                } TDEA;
            };
        } Encryption;
    } WriteData;
} TransferStateType;

extern TransferStateType TransferState;

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    DESFIRE_IDLE,
    DESFIRE_GET_VERSION2,
    DESFIRE_GET_VERSION3,
    DESFIRE_GET_APPLICATION_IDS2,
    DESFIRE_AUTHENTICATE2,
    DESFIRE_READ_DATA_FILE,
    DESFIRE_WRITE_DATA_FILE,
} DesfireStateType;

extern DesfireStateType DesfireState;
extern uint8_t AuthenticatedWithKey;

static void SyncronizePICCInfo(void);

BOOL SetProtectedHeaderData(PICCHeaderField hfield, BYTE *byteBuf, SIZET bufSize);
BOOL ResetProtectedHeaderData(PICCHeaderField hfield); 
BOOL ResetAndZeroFillPICCImage(int slotNumber); 
BOOL WriteByteArrayToPICC(int slotNum, SIZET piccOffset, BYTE *byteBuf, SIZET bufSize);

/* Transfer routines */
static TransferStatus PiccToPcdTransfer(uint8_t *Buffer);
static uint8_t PcdToPiccTransfer(uint8_t *Buffer, uint8_t Count);

/* Setup routines */
static uint8_t ReadDataFilterSetup(uint8_t CommSettings);
static uint8_t WriteDataFilterSetup(uint8_t CommSettings);

/* PICC management */
static void InitialisePiccBackendEV0(uint8_t StorageSize);
static void InitialisePiccBackendEV1(uint8_t StorageSize);
static void ResetPiccBackend(void);
static bool IsEmulatingEV1(void);
static void GetPiccHardwareVersionInfo(uint8_t* Buffer);
static void GetPiccSoftwareVersionInfo(uint8_t* Buffer);
static void GetPiccManufactureInfo(uint8_t* Buffer);
static uint8_t GetPiccKeySettings(void);
static void FormatPicc(void);
static void CreatePiccApp(void);
static void FactoryFormatPiccEV0(void);
static void FactoryFormatPiccEV1(uint8_t StorageSize);
static void GetPiccUid(ConfigurationUidType Uid);
static void SetPiccUid(ConfigurationUidType Uid);

#endif
