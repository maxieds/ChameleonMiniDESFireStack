/*
The DESFire stack portion of this firmware source 
is free software written by Maxie Dion Schmidt: 
you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

The complete license provided with source distributions of this library is available at the following link:
https://github.com/maxieds/ChameleonMiniFirmwareDESFireStack

This notice must be retained at the top of all source files in the repository. 
*/

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
    BYTE BlockBuffer[CRYPTO_MAX_BLOCK_SIZE];
    struct DESFIRE_FIRMWARE_ALIGNAT {
        TransferChecksumUpdateFuncType UpdateFunc;
        TransferChecksumFinalFuncType FinalFunc;
        BYTE AvailablePlaintext;
        struct DESFIRE_FIRMWARE_ALIGNAT {
            union DESFIRE_FIRMWARE_ALIGNAT {
                CryptoAESCBCFuncType   AESFunc;
                CryptoTDEACBCFuncType  TDEAFunc;
            } CryptoChecksumFunc;
            union {
                 SIZET CRCA;
                 UINT  CRC32;
                 BYTE  CMAC[DESFIRE_CMAC_LENGTH];
            };
        } MACData;
    } Checksums;
    struct DESFIRE_FIRMWARE_ALIGNAT {
        SIZET BytesLeft;
        struct DESFIRE_FIRMWARE_ALIGNAT {
            TransferSourceFuncType Func;
            SIZET Pointer; /* in EEPROM */
        } Source;
        struct DESFIRE_FIRMWARE_ALIGNAT {
            BOOL FirstPaddingBitSet;
            TransferEncryptFuncType Func;
            BYTE AvailablePlaintext;
        } Encryption; 
    } ReadData;
    struct DESFIRE_FIRMWARE_ALIGNAT {
        SIZET BytesLeft;
        struct DESFIRE_FIRMWARE_ALIGNAT {
            TransferSinkFuncType Func;
            SIZET Pointer; /* in EEPROM */
        } Sink;
        struct DESFIRE_FIRMWARE_ALIGNAT {
            TransferEncryptFuncType Func;
            BYTE AvailablePlaintext;
        } Encryption;
    } WriteData;
} TransferStateType;
extern TransferStateType TransferState;

/* Transfer routines */
void SyncronizePICCInfo(void);
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
