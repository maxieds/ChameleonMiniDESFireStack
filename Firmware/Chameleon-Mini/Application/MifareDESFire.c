/*
The DESFire stack portion of this firmware source 
is free software written by Maxie Dion Schmidt (@maxieds): 
You can redistribute it and/or modify
it under the terms of this license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

The complete source distribution of  
this firmware is available at the following link:
https://github.com/maxieds/ChameleonMiniFirmwareDESFireStack.

Based in part on the original DESFire code created by  
@dev-zzo (GitHub handle) [Dmitry Janushkevich] available at  
https://github.com/dev-zzo/ChameleonMini/tree/desfire.

This notice must be retained at the top of all source files where indicated. 
*/

/*
 * MifareDesfire.c
 * MIFARE DESFire frontend
 *
 * Created on: 14.10.2016
 * Author: dev_zzo
 */

#ifdef CONFIG_MF_DESFIRE_SUPPORT

#include "MifareDESFire.h"
#include "DESFire/DESFireFirmwareSettings.h"
#include "DESFire/DESFireInstructions.h"
#include "DESFire/DESFirePICCControl.h"
#include "DESFire/DESFireCrypto.h"
#include "DESFire/DESFireISO14443Support.h"
#include "DESFire/DESFireStatusCodes.h"
#include "DESFire/DESFireLogging.h"
#include "Reader14443A.h"

DesfireStateType DesfireState = DESFIRE_HALT;
DesfireStateType DesfirePreviousState = DESFIRE_IDLE;
bool DesfireFromHalt = false;
BYTE DesfireCmdCLA = 0x90;

Iso7816WrappedParams_t Iso7816P1Data = ISO7816_NO_DATA;
Iso7816WrappedParams_t Iso7816P2Data = ISO7816_NO_DATA;
bool Iso7816FileSelected = false;
uint8_t Iso7816FileOffset = 0x00;
uint8_t Iso7816EfIdNumber = 0xff;

/* Dispatching routines */
void MifareDesfireReset(void) {}

void MifareDesfireEV0AppInit(void) {
    /* Init lower layers: nothing for now */
    ResetLocalStructureData();
    DesfireState = DESFIRE_IDLE;
    DesfireFromHalt = false;
    InitialisePiccBackendEV0(DESFIRE_STORAGE_SIZE_4K);
    /* The rest is handled in reset */
}

static void MifareDesfireEV1AppInit(uint8_t StorageSize) {
    /* Init lower layers: nothing for now */
    ResetLocalStructureData();
    DesfireState = DESFIRE_IDLE;
    DesfireFromHalt = false;
    InitialisePiccBackendEV1(StorageSize);
    /* The rest is handled in reset */
}

void MifareDesfire2kEV1AppInit(void) {
    ResetLocalStructureData();
    DesfireState = DESFIRE_IDLE;
    DesfireFromHalt = false;
    MifareDesfireEV1AppInit(DESFIRE_STORAGE_SIZE_2K);
}

void MifareDesfire4kEV1AppInit(void) {
    ResetLocalStructureData();
    DesfireState = DESFIRE_IDLE;
    DesfireFromHalt = false;
    MifareDesfireEV1AppInit(DESFIRE_STORAGE_SIZE_4K);
}

void MifareDesfire8kEV1AppInit(void) {
    ResetLocalStructureData();
    DesfireState = DESFIRE_IDLE;
    DesfireFromHalt = false;
    MifareDesfireEV1AppInit(DESFIRE_STORAGE_SIZE_8K);
}

void MifareDesfireAppReset(void) {
    /* This is called repeatedly, so limit the amount of work done */
    ISO144433AReset();
    ISO144434Reset();
    MifareDesfireReset();
}

void MifareDesfireAppTick(void)
{
    if(!CheckStateRetryCount2(false, false)) {
        MifareDesfireAppReset();
    }
    /* Empty */
}

void MifareDesfireAppTask(void)
{
    /* Empty */
}

uint16_t MifareDesfireProcessCommand(uint8_t* Buffer, uint16_t ByteCount) {
    
    LogEntry(LOG_INFO_DESFIRE_INCOMING_DATA, Buffer, ByteCount);
    if(ByteCount == 0) {
         return ISO14443A_APP_NO_RESPONSE;
    } 
    else if((DesfireCmdCLA != DESFIRE_NATIVE_CLA) && 
            (DesfireCmdCLA != DESFIRE_ISO7816_CLA)) {
        return ISO14443A_APP_NO_RESPONSE;
    }
    else if(Buffer[0] != STATUS_ADDITIONAL_FRAME) {
        uint16_t ReturnBytes = CallInstructionHandler(Buffer, ByteCount);
        if(ReturnBytes > 0) {
            LogEntry(LOG_INFO_DESFIRE_OUTGOING_DATA, Buffer, ReturnBytes);
        }
        return ReturnBytes;
    }
   
    /* Expecting further data here */
    if(Buffer[0] != STATUS_ADDITIONAL_FRAME) {
        return ISO14443A_APP_NO_RESPONSE;
    }

    uint16_t ReturnBytes = 0;
    switch (DesfireState) {
    case DESFIRE_GET_VERSION2:
        ReturnBytes = EV0CmdGetVersion2(Buffer, ByteCount);
        break;
    case DESFIRE_GET_VERSION3:
        ReturnBytes = EV0CmdGetVersion3(Buffer, ByteCount);
        break;
    case DESFIRE_GET_APPLICATION_IDS2:
        ReturnBytes = GetApplicationIdsIterator(Buffer, ByteCount);
        break;
    case DESFIRE_LEGACY_AUTHENTICATE2:
        ReturnBytes = EV0CmdAuthenticateLegacy2(Buffer, ByteCount);
        break;
    case DESFIRE_ISO_AUTHENTICATE2:
        ReturnBytes = DesfireCmdAuthenticate3KTDEA2(Buffer, ByteCount);
        break;
    case DESFIRE_AES_AUTHENTICATE2:
        ReturnBytes = DesfireCmdAuthenticateAES2(Buffer, ByteCount);
        break;
    case DESFIRE_READ_DATA_FILE:
        ReturnBytes = ReadDataFileIterator(Buffer);
        break;
    case DESFIRE_WRITE_DATA_FILE:
        ReturnBytes = WriteDataFileInternal(&Buffer[1], ByteCount - 1);
        break;
    default:
        /* Should not happen. */
        Buffer[0] = STATUS_PICC_INTEGRITY_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    DesfireLogEntry(LOG_INFO_DESFIRE_OUTGOING_DATA, Buffer, ReturnBytes);
    return ReturnBytes;

}

uint16_t MifareDesfireProcess(uint8_t* Buffer, uint16_t BitCount) {
    size_t ByteCount = BitCount / BITS_PER_BYTE;
    if(ByteCount >= 8 && DesfireCLA(Buffer[0]) && Buffer[2] == 0x00 && 
       Buffer[3] == 0x00 && Buffer[4] == ByteCount - 8) { // Wrapped native command structure: 
        /* Unwrap the PDU from ISO 7816-4 */
        // Check CRC bytes appended to the buffer:
        // -- Actually, just ignore parity problems if they exist
        DesfireCmdCLA = Buffer[0];
        if(Iso7816CLA(DesfireCmdCLA)) {
            uint16_t iso7816ParamsStatus = SetIso7816WrappedParametersType(Buffer, ByteCount);
            if(iso7816ParamsStatus != ISO7816_CMD_NO_ERROR) {
                Buffer[0] = (uint8_t) ((iso7816ParamsStatus >> 8) & 0xff);
                Buffer[1] = (uint8_t) (iso7816ParamsStatus & 0x00ff);
                ISO14443AAppendCRCA(Buffer, 2);
                ByteCount = 2 + 2;
                return ByteCount * BITS_PER_BYTE;
            }
        }
        ByteCount = Buffer[4]; // also removing the trailing two parity bytes
        Buffer[0] = Buffer[1];
        memmove(&Buffer[1], &Buffer[5], ByteCount);
        /* Process the command */
        /* TODO: Where are we deciphering wrapped payload data? 
         *       This should depend on the CommMode standard? 
         */
        BitCount = MifareDesfireProcessCommand(Buffer, ByteCount + 1);
        /* TODO: Where are we re-wrapping the data according to the CommMode standards? */
        if((BitCount && !Iso7816CLA(DesfireCmdCLA)) || (BitCount == 1)) {
            /* Re-wrap into padded APDU form */
            Buffer[BitCount] = Buffer[0];
            memmove(&Buffer[0], &Buffer[1], BitCount - 1);
            Buffer[BitCount - 1] = 0x91;
            ISO14443AAppendCRCA(Buffer, ++BitCount);
            BitCount += 2;
        }
        else {
            /* Re-wrap into ISO 7816-4 */
            Buffer[BitCount] = Buffer[0];
            Buffer[BitCount + 1] = Buffer[1];
            memmove(&Buffer[0], &Buffer[2], BitCount - 2);
            ISO14443AAppendCRCA(Buffer, BitCount);
            BitCount += 2;
        }
        return BitCount * BITS_PER_BYTE;
    }
    else {
        /* ISO/IEC 14443-4 PDUs: No extra work */
        return MifareDesfireProcessCommand(Buffer, ByteCount) * BITS_PER_BYTE;
    }

}

uint16_t MifareDesfireAppProcess(uint8_t* Buffer, uint16_t BitCount) {
    size_t ByteCount = BitCount / BITS_PER_BYTE;
    if(ByteCount >= 8 && DesfireCLA(Buffer[0]) && Buffer[2] == 0x00 &&
       Buffer[3] == 0x00 && Buffer[4] == ByteCount - 8) {
         return MifareDesfireProcess(Buffer, BitCount);
    }
    //else if(ByteCount >= 8 && Buffer[0] == 0x93 & Buffer[2] == 0x00 && 
    //        Buffer[3] == 0x00 && Buffer[4] == ByteCount - 8) {
    //    DesfireCmdCLA = Buffer[0];
    //    ByteCount = Buffer[4]; // also removing the trailing two parity bytes
    //    memmove(&Buffer[2], &Buffer[5], ByteCount);
    //}
    uint16_t BitCount2 = BitCount;
    return ISO144433APiccProcess(Buffer, BitCount);
    //if(BitCount != ISO14443A_APP_NO_RESPONSE) {
    //     return BitCount;
    //}
    //else {
    //    return MifareDesfireProcess(Buffer, BitCount2);
    //}
}

void ResetLocalStructureData(void) {
     DesfirePreviousState = DESFIRE_IDLE;
     DesfireState = DESFIRE_HALT;
     InvalidateAuthState(0x00);
     memset(&Picc, PICC_FORMAT_BYTE, sizeof(Picc));
     memset(&SelectedApp, 0x00, sizeof(SelectedApp));
     memset(&SelectedFile, 0x00, sizeof(SelectedFile));
     memset(&TransferState, 0x00, sizeof(TransferState));
     memset(&SessionKey, 0x00, sizeof(CryptoKeyBufferType));
     memset(&SessionIV, 0x00, sizeof(CryptoIVBufferType));
     SessionIVByteSize = 0x00;
     memset(&AESCryptoSessionKey, 0x00, sizeof(DesfireAESCryptoKey));
     memset(&AESCryptoIVBuffer, 0x00, sizeof(DesfireAESCryptoKey));
}

void MifareDesfireGetUid(ConfigurationUidType Uid)
{
    GetPiccUid(Uid);
}

void MifareDesfireSetUid(ConfigurationUidType Uid)
{
    SetPiccUid(Uid);
}

uint16_t SetIso7816WrappedParametersType(uint8_t *Buffer, uint16_t ByteCount) {
    if(ByteCount < 8 || !Iso7816CLA(Buffer[0])) {
        Iso7816P1Data = ISO7816_UNSUPPORTED_MODE;
        Iso7816P2Data = ISO7816_UNSUPPORTED_MODE;
        return AppendSW12Bytes(ISO7816_ERROR_SW1, ISO7816_SELECT_ERROR_SW2_UNSUPPORTED);
    }
    else {
        Iso7816P1Data = ISO7816_NO_DATA;
        Iso7816P2Data = ISO7816_NO_DATA;
    }
    uint8_t insCode = Buffer[1];
    uint8_t P1 = Buffer[2];
    uint8_t P2 = Buffer[3];
    if(insCode == CMD_ISO7816_SELECT) {
        /* Reference: https://cardwerk.com/smart-card-standard-iso7816-4-section-6-basic-interindustry-commands/#chap6_11 */
        if((P1 & 0xfc) == 0) { // Select by file ID:
            if((P1 & 0x03) == 0 || (P1 & 0x03) == 0x01) {
                Iso7816P1Data = ISO7816_SELECT_EF;
            }
            else {
                Iso7816P1Data = ISO7816_UNSUPPORTED_MODE;
                return AppendSW12Bytes(ISO7816_ERROR_SW1, ISO7816_SELECT_ERROR_SW2_UNSUPPORTED);
            }
        }
        else if((P1 & 0xf9) == 0) { // Select by DF/AID name:
             if((P1 & 0x03) == 0) {
                 Iso7816P1Data = ISO7816_SELECT_DF;
             }
             else {
                 Iso7816P1Data = ISO7816_UNSUPPORTED_MODE;
                 return AppendSW12Bytes(ISO7816_ERROR_SW1, ISO7816_SELECT_ERROR_SW2_UNSUPPORTED);
             }
        }
        else {
             Iso7816P1Data = ISO7816_UNSUPPORTED_MODE;
             return AppendSW12Bytes(ISO7816_ERROR_SW1, ISO7816_SELECT_ERROR_SW2_UNSUPPORTED);
        }
        if((P2 & 0xf0) == 0) {
            switch(P2 & 0x03) {
                case 0x00:
                    Iso7816P2Data = ISO7816_FILE_FIRST_RECORD;
                    break;
                case 0x01:
                    Iso7816P2Data = ISO7816_FILE_LAST_RECORD;
                    break;
                case 0x02:
                    Iso7816P2Data = ISO7816_FILE_NEXT_RECORD;
                    break;
                case 0x03:
                    Iso7816P2Data = ISO7816_FILE_PREV_RECORD;
                    break;
                default:
                    Iso7816P2Data = ISO7816_UNSUPPORTED_MODE;
                    return AppendSW12Bytes(ISO7816_ERROR_SW1, ISO7816_SELECT_ERROR_SW2_UNSUPPORTED);
            }
        }
        else {
            Iso7816P2Data = ISO7816_UNSUPPORTED_MODE;
            return AppendSW12Bytes(ISO7816_ERROR_SW1, ISO7816_SELECT_ERROR_SW2_UNSUPPORTED);
        }
    }
    else if(insCode == CMD_ISO7816_GET_CHALLENGE) {
        if(P1 != 0x00 || P2 != 0x00) {
            Iso7816P1Data = ISO7816_UNSUPPORTED_MODE;
            Iso7816P2Data = ISO7816_UNSUPPORTED_MODE;
            return AppendSW12Bytes(ISO7816_ERROR_SW1, ISO7816_SELECT_ERROR_SW2_UNSUPPORTED);
        }
        Iso7816P1Data = ISO7816_NO_DATA;
        Iso7816P2Data = ISO7816_NO_DATA;
    }
    else if(insCode == CMD_ISO7816_READ_BINARY) {
         if((P1 & 0x80) != 0) { 
              if((P1 & 0x60) != 0) {
                   Iso7816P1Data = Iso7816P2Data = ISO7816_UNSUPPORTED_MODE;
                   return AppendSW12Bytes(ISO7816_ERROR_SW1, ISO7816_ERROR_SW2_INCORRECT_P1P2);
              }
              else {
                   Iso7816EfIdNumber = P1 & 0x0f;
                   Iso7816FileOffset = P2;
                   Iso7816P1Data = Iso7816P2Data = ISO7816_NO_DATA;
              }
         }
         else {
              Iso7816EfIdNumber = 0xff;
              Iso7816FileOffset = P1 | P2;
              Iso7816P1Data = Iso7816P2Data = ISO7816_NO_DATA;
         }
    }
    else {
        Iso7816P1Data = ISO7816_UNSUPPORTED_MODE;
        Iso7816P2Data = ISO7816_UNSUPPORTED_MODE;
        return AppendSW12Bytes(ISO7816_ERROR_SW1, ISO7816_SELECT_ERROR_SW2_UNSUPPORTED);
    }
    return ISO7816_CMD_NO_ERROR;
}

#endif /* CONFIG_MF_DESFIRE_SUPPORT */
