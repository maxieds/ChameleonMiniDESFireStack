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

This source code is only licensed for 
redistribution under for non-commercial users. 
All commerical use or inclusion of this 
software requires express written consent of the author (MDS). 
This restriction pertains to any binary distributions which 
are derivative works of this software.

The author is free to revoke or modify this license for future 
versions of the code at free will.
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
bool DesfireFromHalt = false;
BYTE DesfireCmdCLA = 0x90;

/* Dispatching routines */
void MifareDesfireReset(void) {
    DesfireState = DESFIRE_IDLE;
    InvalidateAuthState(0x00);
    DesfireCmdCLA = 0x00;
}

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
    
    //const char *loggingErrorMsg = PSTR("MFDesfireProcessCmd: ByteCount -- %d");
    //DEBUG_PRINT_P(loggingErrorMsg, ByteCount);
    LogEntry(LOG_INFO_DESFIRE_INCOMING_DATA, Buffer, ByteCount);

    if(ByteCount == 0) {
         return ISO14443A_APP_NO_RESPONSE;
    } 
    else if((DesfireState == DESFIRE_IDLE) && (DesfireCmdCLA != DESFIRE_NATIVE_CLA) && 
            (DesfireCmdCLA != DESFIRE_ISO7816_CLA)) {
        return ISO14443A_APP_NO_RESPONSE;
    }
    else if(DesfireState == DESFIRE_IDLE) {
        uint16_t ReturnBytes = CallInstructionHandler(Buffer, ByteCount);
        LogEntry(LOG_INFO_DESFIRE_OUTGOING_DATA, Buffer, ReturnBytes);
        return ReturnBytes;
    }
   
    /* Expecting further data here */
    if(Buffer[0] != STATUS_ADDITIONAL_FRAME && !CheckStateRetryCount(false)) {
        AbortTransaction();
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
        ReturnBytes = EV0CmdAuthenticate2KTDEA2(Buffer, ByteCount);
        break;
    case DESFIRE_ISO_AUTHENTICATE2:
        ReturnBytes = DesfireCmdAuthenticate3KTDEA2(Buffer, ByteCount);
        break;
    case DESFIRE_AES_AUTHENTICATE2:
        ReturnBytes = DesfireCmdAuthenticateAES2(Buffer, ByteCount);
        break;
    case DESFIRE_READ_DATA_FILE:
        ReturnBytes = ReadDataFileIterator(Buffer, ByteCount);
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
    //LogEntry(LOG_INFO_DESFIRE_INCOMING_DATA, Buffer, ByteCount);
    //return 0;
    if(ByteCount >= 8 && Buffer[0] == 0x90 && Buffer[2] == 0x00 && 
       Buffer[3] == 0x00 && Buffer[4] == ByteCount - 8) { // Wrapped native command structure: 
        // Check CRC bytes appended to the buffer:
        // -- Actually, just ignore parity problems if they exist (TODO later)
        //if(!ISO14443ACheckCRCA(Buffer, ByteCount - 2)) {
        //    Buffer[0] = STATUS_INTEGRITY_ERROR;
        //    return DESFIRE_STATUS_RESPONSE_SIZE * BITS_PER_BYTE;
        //}
        /* Unwrap the PDU from ISO 7816-4 */
        DesfireState = DESFIRE_IDLE;
        DesfireCmdCLA = Buffer[0];
        ByteCount = Buffer[4]; // also removing the trailing two parity bytes
        Buffer[0] = Buffer[1];
        if(ByteCount > 1) {
            memmove(&Buffer[1], &Buffer[5], ByteCount - 1);
        }
        /* Process the command */
        /* TODO: Where are we deciphering wrapped payload data? 
         *       This should depend on the CommMode standard? 
         */
        BitCount = MifareDesfireProcessCommand(Buffer, ByteCount);
        /* TODO: Where are we re-wrapping the data according to the CommMode standards? */
        if (BitCount) {
            /* Re-wrap into ISO 7816-4 */
            Buffer[BitCount] = Buffer[0];
            memmove(&Buffer[0], &Buffer[1], BitCount - 1);
            Buffer[BitCount - 1] = 0x91;
            ISO14443AAppendCRCA(Buffer, ++BitCount);
            BitCount += 2;
        }
        return BitCount * BITS_PER_BYTE;
    }
    else {
        /* ISO/IEC 14443-4 PDUs: No extra work */
        const char *loggingErrorMsg = PSTR("Skipping the ProcessCommand method");
        DEBUG_PRINT_P(loggingErrorMsg);
        return 0; // TODO: Remove this!
        return MifareDesfireProcessCommand(Buffer, BitCount) * BITS_PER_BYTE;
    }

}

uint16_t MifareDesfireAppProcess(uint8_t* Buffer, uint16_t BitCount) {
    size_t ByteCount = BitCount / BITS_PER_BYTE;
    //LogEntry(LOG_INFO_DESFIRE_INCOMING_DATA, Buffer, ByteCount);
    if(ByteCount >= 8 && Buffer[0] == 0x90 && Buffer[2] == 0x00 &&
       Buffer[3] == 0x00 && Buffer[4] == ByteCount - 8) {
         return MifareDesfireProcess(Buffer, BitCount);
    }
    uint16_t BitCount2 = BitCount;
    const char *loggingErrorMsg = PSTR("Skipping over the internal Picc process func");
    DEBUG_PRINT_P(loggingErrorMsg, ByteCount);
    return 0;
    BitCount = ISO144433APiccProcess(Buffer, BitCount);
    if(BitCount != ISO14443A_APP_NO_RESPONSE) {
         return BitCount;
    }
    else {
        return MifareDesfireProcess(Buffer, BitCount2);
    }
}

void ResetLocalStructureData(void) {
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

#endif /* CONFIG_MF_DESFIRE_SUPPORT */
