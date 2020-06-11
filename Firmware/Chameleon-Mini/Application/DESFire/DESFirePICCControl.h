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
    DESFireFileTypeSettings File;
} SelectedFileCacheType;

extern const SIZET DESFIRE_PICC_INFO_BLOCK_ID;
extern const SIZET DESFIRE_APP_DIR_BLOCK_ID;
extern const SIZET DESFIRE_FIRST_FREE_BLOCK_ID;

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
            struct DESFIRE_FIRMWARE_PACKING {
                CryptoTDEACBCFuncType MACFunc;
                BYTE BlockBuffer[CRYPTO_DES_BLOCK_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
            } MAC;
            struct DESFIRE_FIRMWARE_PACKING {
                CryptoTDEACBCFuncType MACFunc;
                BYTE BlockBuffer[CRYPTO_DES_BLOCK_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
            } CipherTextTransferMAC;
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
                struct DESFIRE_FIRMWARE_PACKING {
                    BYTE BlockBuffer[CRYPTO_DES_BLOCK_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
                } TDEA;
                struct DESFIRE_FIRMWARE_PACKING {
                    BYTE BlockBuffer[CRYPTO_3KTDEA_BLOCK_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
                } IsoTransferTDEA;
                struct DESFIRE_FIRMWARE_PACKING {
                    BYTE BlockBuffer[CRYPTO_DES_BLOCK_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
                } AESTransferTDEA;
            };
        } Encryption; 
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
                struct DESFIRE_FIRMWARE_PACKING {
                    BYTE BlockBuffer[CRYPTO_DES_BLOCK_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
                } TDEA;
                struct DESFIRE_FIRMWARE_PACKING {
                    BYTE BlockBuffer[CRYPTO_3KTDEA_BLOCK_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
                } CipherTextTransferTDEA;
            };
        } Encryption;
    } WriteData;
} TransferStateType;

#define ExtractTransferMACData(commMode, tstate) \
	(commMode == DESFIRE_COMMS_CIPHERTEXT) ? \
	tstate.Checksums.CipherTextTransferMAC : \
	tstate.Checksums.MAC
#define ExtractTransferTDEAData(commMode, tstate) \
	(commMode == DESFIRE_COMMS_CIPHERTEXT) ? \
tstate.Encryption.CipherTextTransferTDEA : \
	tstate.Encryption.TDEA

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

void SyncronizePICCInfo(void);

BOOL SetProtectedHeaderData(PICCHeaderField hfield, BYTE *byteBuf, SIZET bufSize);
BOOL ResetProtectedHeaderData(PICCHeaderField hfield); 
BOOL ResetAndZeroFillPICCImage(int slotNumber); 
BOOL WriteByteArrayToPICC(int slotNum, SIZET piccOffset, BYTE *byteBuf, SIZET bufSize);

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
