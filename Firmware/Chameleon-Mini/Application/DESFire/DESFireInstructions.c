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
 * DESFireInstructions.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include <avr/pgmspace.h>
#include <string.h>

#include "../../Configuration.h"
#include "../../Memory.h"
#include "../../Common.h"
#include "../../Random.h"

#include "DESFireInstructions.h"
#include "DESFirePICCControl.h"
#include "DESFireCrypto.h"
#include "DESFireStatusCodes.h"
#include "DESFireLogging.h"
#include "DESFireUtils.h"
#include "DESFireMemoryOperations.h"
#include "../MifareDESFire.h"

DesfireSavedCommandStateType DesfireCommandState = { 0 };

const __flash DESFireCommand DESFireCommandSet[] = {
     {
          .insCode = CMD_AUTHENTICATE, 
          .insDesc = (const __flash char[]) { "Authenicate_Legacy" }, 
          .insFunc = &EV0CmdAuthenticate2KTDEA1
     },
     {
          .insCode = CMD_AUTHENTICATE_ISO, 
          .insDesc = (const __flash char[]) { "Authenticate_ISO" },
          .insFunc = &DesfireCmdAuthenticate3KTDEA1
     },
     {
          .insCode = CMD_AUTHENTICATE_AES, 
          .insDesc = (const __flash char[]) { "Authenticate_AES" },
          .insFunc = &DesfireCmdAuthenticateAES1
     },
     {
          .insCode = CMD_AUTHENTICATE_EV2_FIRST, 
          .insDesc = (const __flash char[]) { "Authenticate_AES_EV2_First" },
          .insFunc = NULL
     },
     {
          .insCode = CMD_AUTHENTICATE_EV2_NONFIRST, 
          .insDesc = (const __flash char[]) { "Authenticate_AES_EV2_NonFirst" },
          .insFunc = NULL
     },
     {
          .insCode = CMD_CHANGE_KEY_SETTINGS, 
          .insDesc = (const __flash char[]) { "Change_Key_Settings" }, 
          .insFunc = &EV0CmdGetKeySettings
     },
     {
          .insCode = CMD_SET_CONFIGURATION, 
          .insDesc = (const __flash char[]) { "Set_Configuration" },
          .insFunc = &DesfireCmdSetConfiguration
     },
     {
          .insCode = CMD_CHANGE_KEY, 
          .insDesc = (const __flash char[]) { "Change_Key" },
          .insFunc = &EV0CmdChangeKey
     },
     {
          .insCode = CMD_GET_KEY_VERSION, 
          .insDesc = (const __flash char[]) { "Get_Key_Version" },
          .insFunc = &DesfireCmdGetKeyVersion
     },
     {
          .insCode = CMD_CREATE_APPLICATION, 
          .insDesc = (const __flash char[]) { "Create_Application" },
          .insFunc = &EV0CmdCreateApplication
     },
     {
          .insCode = CMD_DELETE_APPLICATION, 
          .insDesc = (const __flash char[]) { "Delete_Application" },
          .insFunc = &EV0CmdDeleteApplication
     },
     {
          .insCode = CMD_GET_APPLICATION_IDS, 
          .insDesc = (const __flash char[]) { "Get_Application_IDs" },
          .insFunc = &EV0CmdGetApplicationIds1
     },
     {
          .insCode = CMD_FREE_MEMORY, 
          .insDesc = (const __flash char[]) { "Free_Memory" },
          .insFunc = &DesfireCmdFreeMemory
     },
     {
          .insCode = CMD_GET_DF_NAMES, 
          .insDesc = (const __flash char[]) { "Get_DF_Names" },
          .insFunc = &DesfireCmdGetDFNames
     },
     {
          .insCode = CMD_GET_KEY_SETTINGS, 
          .insDesc = (const __flash char[]) { "Get_Key_Settings" },
          .insFunc = &EV0CmdGetKeySettings
     },
     {
          .insCode = CMD_SELECT_APPLICATION, 
          .insDesc = (const __flash char[]) { "Select_Application" },
          .insFunc = &EV0CmdSelectApplication
     },
     {
          .insCode = CMD_FORMAT_PICC, 
          .insDesc = (const __flash char[]) { "Format_PICC" },
          .insFunc = &EV0CmdFormatPicc
     },
     {
          .insCode = CMD_GET_VERSION, 
          .insDesc = (const __flash char[]) { "Get_Version" },
          .insFunc = &EV0CmdGetVersion1
     },
     {
          .insCode = CMD_GET_CARD_UID, 
          .insDesc = (const __flash char[]) { "Get_Card_UID" },
          .insFunc = &DesfireCmdGetCardUID
     },
     {
          .insCode = CMD_GET_FILE_IDS, 
          .insDesc = (const __flash char[]) { "Get_File_IDs" },
          .insFunc = &EV0CmdGetFileIds
     },
     {
          .insCode = CMD_GET_FILE_SETTINGS, 
          .insDesc = (const __flash char[]) { "Get_File_Settings" },
          .insFunc = &EV0CmdGetFileSettings
     },
     {
          .insCode = CMD_CHANGE_FILE_SETTINGS, 
          .insDesc = (const __flash char[]) { "Change_File_Settings" },
          .insFunc = &EV0CmdChangeFileSettings
     },
     {
          .insCode = CMD_CREATE_STDDATA_FILE, 
          .insDesc = (const __flash char[]) { "Create_Data_File" },
          .insFunc = &EV0CmdCreateStandardDataFile
     },
     {
          .insCode = CMD_CREATE_BACKUPDATA_FILE, 
          .insDesc = (const __flash char[]) { "Create_Backup_File" },
          .insFunc = &EV0CmdCreateBackupDataFile
     },
     {
          .insCode = CMD_CREATE_VALUE_FILE, 
          .insDesc = (const __flash char[]) { "Create_Value_File" },
          .insFunc = &EV0CmdCreateValueFile
     },
     {
          .insCode = CMD_CREATE_LINEAR_RECORD_FILE, 
          .insDesc = (const __flash char[]) { "Create_Linear_Record_File" },
          .insFunc = &EV0CmdCreateLinearRecordFile
     },
     {
          .insCode = CMD_CREATE_CYCLIC_RECORD_FILE, 
          .insDesc = (const __flash char[]) { "Create_Cyclic_Record_File" },
          .insFunc = &EV0CmdCreateCyclicRecordFile
     },
     {
          .insCode = CMD_DELETE_FILE, 
          .insDesc = (const __flash char[]) { "Delete_File" },
          .insFunc = &EV0CmdDeleteFile
     },
     {
          .insCode = CMD_GET_ISO_FILE_IDS, 
          .insDesc = (const __flash char[]) { "Get_ISO_File_IDs" },
          .insFunc = &EV0CmdGetFileIds
     },
     {
          .insCode = CMD_READ_DATA, 
          .insDesc = (const __flash char[]) { "Read_Data" },
          .insFunc = &EV0CmdReadData
     },
     {
          .insCode = CMD_WRITE_DATA, 
          .insDesc = (const __flash char[]) { "Write_Data" },
          .insFunc = &EV0CmdWriteData
     },
     {
          .insCode = CMD_GET_VALUE, 
          .insDesc = (const __flash char[]) { "Get_Value" },
          .insFunc = &EV0CmdGetValue
     },
     {
          .insCode = CMD_CREDIT, 
          .insDesc = (const __flash char[]) { "Credit" },
          .insFunc = &EV0CmdCredit
     },
     {
          .insCode = CMD_DEBIT, 
          .insDesc = (const __flash char[]) { "Debit" },
          .insFunc = &EV0CmdDebit
     },
     {
          .insCode = CMD_LIMITED_CREDIT, 
          .insDesc = (const __flash char[]) { "Limited_Credit" },
          .insFunc = &EV0CmdLimitedCredit
     },
     {
          .insCode = CMD_WRITE_RECORD, 
          .insDesc = (const __flash char[]) { "Write_Record" },
          .insFunc = &EV0CmdWriteRecord
     },
     {
          .insCode = CMD_READ_RECORDS, 
          .insDesc = (const __flash char[]) { "Read_Records" },
          .insFunc = &EV0CmdReadRecords
     },
     {
          .insCode = CMD_CLEAR_RECORD_FILE, 
          .insDesc = (const __flash char[]) { "Clear_Record_File" },
          .insFunc = &EV0CmdClearRecords
     },
     {
          .insCode = CMD_COMMIT_TRANSACTION, 
          .insDesc = (const __flash char[]) { "Commit_Transaction" },
          .insFunc = &EV0CmdCommitTransaction
     },
     {
          .insCode = CMD_ABORT_TRANSACTION, 
          .insDesc = (const __flash char[]) { "Abort_Transaction" },
          .insFunc = &EV0CmdAbortTransaction
     },
     {
          .insCode = CMD_ISO7816_SELECT, 
          .insDesc = (const __flash char[]) { "ISO7816_Select" },
          .insFunc = &ISO7816CmdSelect
     },
     {
          .insCode = CMD_ISO7816_GET_CHALLENGE, 
          .insDesc = (const __flash char[]) { "ISO7816_Get_Challenge" },
          .insFunc = &ISO7816CmdGetChallenge
     },
     {
          .insCode = CMD_ISO7816_EXTERNAL_AUTHENTICATE, 
          .insDesc = (const __flash char[]) { "ISO7816_External_Authenticate" },
          .insFunc = &ISO7816CmdExternalAuthenticate
     },
     {
          .insCode = CMD_ISO7816_INTERNAL_AUTHENTICATE, 
          .insDesc = (const __flash char[]) { "ISO7816_Internal_Authenticate" },
          .insFunc = &ISO7816CmdInternalAuthenticate
     },
     {
          .insCode = CMD_ISO7816_READ_BINARY, 
          .insDesc = (const __flash char[]) { "ISO7816_Read_Binary" },
          .insFunc = &ISO7816CmdReadBinary
     },
     {
          .insCode = CMD_ISO7816_UPDATE_BINARY, 
          .insDesc = (const __flash char[]) { "ISO7816_Update_Binary" },
          .insFunc = &ISO7816CmdUpdateBinary
     },
     {
          .insCode = CMD_ISO7816_READ_RECORDS, 
          .insDesc = (const __flash char[]) { "ISO7816_Read_Records" },
          .insFunc = &ISO7816CmdReadRecords
     },
     {
          .insCode = CMD_ISO7816_APPEND_RECORD, 
          .insDesc = (const __flash char[]) { "ISO7816_Append_Record" },
          .insFunc = &ISO7816CmdAppendRecord
     }
};

uint16_t CallInstructionHandler(uint8_t *Buffer, uint16_t ByteCount) {
    if(ByteCount == 0) {
         Buffer[0] = STATUS_PARAMETER_ERROR;
         return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    uint8_t callingInsCode = Buffer[0];
    uint32_t insLookupTableBuf = &DESFireCommandSet[0];
    uint8_t cmdSetLength = sizeof(DESFireCommandSet) / sizeof(DESFireCommand);
    uint8_t curInsIndex = 0;
    while(curInsIndex < cmdSetLength) {
         DESFireCommand dfCmd;
         memcpy_P(&dfCmd, insLookupTableBuf + curInsIndex * sizeof(DESFireCommand), sizeof(DESFireCommand));
         if(dfCmd.insCode == callingInsCode) {
              if(dfCmd.insFunc == NULL) {
                  snprintf_P(__InternalStringBuffer, STRING_BUFFER_SIZE, PSTR("NOT IMPLEMENTED: %s!"), dfCmd.insDesc);
                  __InternalStringBuffer[STRING_BUFFER_SIZE - 1] = '\0';
                  uint8_t bufSize = StringLength(__InternalStringBuffer, STRING_BUFFER_SIZE);
                  LogEntry(LOG_INFO_DESFIRE_DEBUGGING_OUTPUT, (void *) __InternalStringBuffer, bufSize);
                  return CmdNotImplemented(Buffer, ByteCount);
              }
              return dfCmd.insFunc(Buffer, ByteCount);
         }
         curInsIndex += 1;
    }
    return ISO14443A_APP_NO_RESPONSE;
}

uint16_t ExitWithStatus(uint8_t *Buffer, uint8_t StatusCode, uint16_t DefaultReturnValue) {
     Buffer[0] = StatusCode;
     return DefaultReturnValue;
}

uint16_t CmdNotImplemented(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE;
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

/*
 * DESFire general commands
 */

uint16_t EV0CmdGetVersion1(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ADDITIONAL_FRAME;
    Buffer[1] = DESFIRE_MANUFACTURER_ID;
    Buffer[2] = DESFIRE_TYPE;
    Buffer[3] = DESFIRE_SUBTYPE;
    GetPiccHardwareVersionInfo(&Buffer[4]);
    Buffer[7] = DESFIRE_HW_PROTOCOL_TYPE;
    DesfireState = DESFIRE_GET_VERSION2;
    return DESFIRE_VERSION1_BYTES_PROCESSED;
}

uint16_t EV0CmdGetVersion2(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ADDITIONAL_FRAME;
    Buffer[1] = DESFIRE_MANUFACTURER_ID;
    Buffer[2] = DESFIRE_TYPE;
    Buffer[3] = DESFIRE_SUBTYPE;
    GetPiccSoftwareVersionInfo(&Buffer[4]);
    Buffer[7] = DESFIRE_SW_PROTOCOL_TYPE;
    DesfireState = DESFIRE_GET_VERSION3;
    return DESFIRE_VERSION2_BYTES_PROCESSED;
}

uint16_t EV0CmdGetVersion3(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_OPERATION_OK;
    GetPiccManufactureInfo(&Buffer[1]);
    DesfireState = DESFIRE_IDLE;
    return DESFIRE_VERSION3_BYTES_PROCESSED;
}

uint16_t EV0CmdFormatPicc(uint8_t* Buffer, uint16_t ByteCount) {
    /* Require the PICC app to be selected */
    if (!IsPiccAppSelected()) {
        Buffer[0] = STATUS_PERMISSION_DENIED;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Validate command length */
    if (ByteCount != 1) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Verify authentication settings */
    if(!Authenticated || (AuthenticatedWithKey != DESFIRE_MASTER_KEY_ID)) {
        /* PICC master key authentication is always required */
        Buffer[0] = STATUS_AUTHENTICATION_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    uint8_t uidBytes[ISO14443A_UID_SIZE_DOUBLE];
    memcpy(uidBytes, Picc.Uid, ISO14443A_UID_SIZE_DOUBLE);
    if(IsPiccEV0(Picc)) {
        FactoryFormatPiccEV0();
    }
    else {
        FactoryFormatPiccEV1(Picc.StorageSize);
    }
    memcpy(&Picc.Uid[0], uidBytes, ISO14443A_UID_SIZE_DOUBLE);
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t DesfireCmdGetCardUID(uint8_t *Buffer, uint16_t ByteCount) {
     memcpy(&Buffer[0], Picc.Uid, ISO14443A_UID_SIZE_DOUBLE);
     return ISO14443A_UID_SIZE_DOUBLE;
}

uint16_t DesfireCmdSetConfiguration(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t DesfireCmdFreeMemory(uint8_t *Buffer, uint16_t ByteCount) {
    // Returns the amount of free space left on the tag in bytes 
    // Note that this does not account for overhead needed to store 
    // file structures, so that if N bytes are reported, the actual 
    // practical working space is less than N:
    uint16_t cardCapacityBlocks = GetCardCapacityBlocks();
    cardCapacityBlocks -= Picc.FirstFreeBlock;
    uint16_t freeMemoryBytes = cardCapacityBlocks * DESFIRE_EEPROM_BLOCK_SIZE;
    Buffer[0] = (uint8_t) (freeMemoryBytes >> 8);
    Buffer[1] = (uint8_t) (freeMemoryBytes >> 0);
    Buffer[2] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE + 2;
}

/*
 * DESFire key management commands
 */

// TODO: Invalidate auth state and reset accordingly ... 
uint16_t EV0CmdAuthenticate2KTDEA1(uint8_t* Buffer, uint16_t ByteCount) { // TODO: Check ... 
    uint8_t KeyId;
    Crypto2KTDEAKeyType Key;

    /* Validate command length */
    if (ByteCount != 1 + 1) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    KeyId = Buffer[1];
    /* Validate number of keys: less than max */
    if (KeyId >= DESFIRE_MAX_KEYS) {
        Buffer[0] = STATUS_PARAMETER_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* Reset authentication state right away */
    AuthenticatedWithKey = DESFIRE_NOT_AUTHENTICATED;
    /* Fetch the key */
    DesfireCommandState.Authenticate.KeyId = KeyId;
    ReadAppKey(SelectedApp.Slot, KeyId, Key, CRYPTO_2KTDEA_KEY_SIZE);
    LogEntry(LOG_APP_AUTH_KEY, (const void *) Key, sizeof(Key));
    /* Generate the nonce B */
    if(LocalTestingMode != 0) {
         RandomGetBuffer(DesfireCommandState.Authenticate.RndB, DESFIRE_2KTDEA_NONCE_SIZE);
    }
    else {
         /* Fixed nonce for testing */
         DesfireCommandState.Authenticate.RndB[0] = 0xCA;
         DesfireCommandState.Authenticate.RndB[1] = 0xFE;
         DesfireCommandState.Authenticate.RndB[2] = 0xBA;
         DesfireCommandState.Authenticate.RndB[3] = 0xBE;
         DesfireCommandState.Authenticate.RndB[4] = 0x00;
         DesfireCommandState.Authenticate.RndB[5] = 0x11;
         DesfireCommandState.Authenticate.RndB[6] = 0x22;
         DesfireCommandState.Authenticate.RndB[7] = 0x33;
    }
    LogEntry(LOG_APP_NONCE_B, DesfireCommandState.Authenticate.RndB, DESFIRE_2KTDEA_NONCE_SIZE);
    /* Encipher the nonce B with the selected key; <= 8 bytes = no CBC */
    CryptoEncrypt2KTDEA(DesfireCommandState.Authenticate.RndB, &Buffer[1], Key);
    /* Scrub the key */
    memset(&Key, 0, sizeof(Key));

    /* Done */
    DesfireState = DESFIRE_LEGACY_AUTHENTICATE2;
    Buffer[0] = STATUS_ADDITIONAL_FRAME;
    return DESFIRE_STATUS_RESPONSE_SIZE + DESFIRE_2KTDEA_NONCE_SIZE;
}

// TODO: Invalidate auth state and reset accordingly ... 
uint16_t EV0CmdAuthenticate2KTDEA2(uint8_t* Buffer, uint16_t ByteCount) { // TODO: Check ... 
    Crypto2KTDEAKeyType Key;
    DesfireState = DESFIRE_IDLE;

    /* Validate command length */
    if (ByteCount != 1 + 2 * CRYPTO_DES_BLOCK_SIZE) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* Fetch the key */
    ReadAppKey(SelectedApp.Slot, DesfireCommandState.Authenticate.KeyId, Key, CRYPTO_2KTDEA_KEY_SIZE);
    LogEntry(LOG_APP_AUTH_KEY, (const void *) Key, sizeof(Key));
    /* Encipher to obtain plain text; zero IV = no CBC */
    memset(&SessionIV, 0x00, sizeof(SessionIV));
    CryptoEncrypt2KTDEA_CBCReceive(2, &Buffer[1], &Buffer[1], SessionIV, Key);
    LogEntry(LOG_APP_NONCE_AB, &Buffer[1], 2 * DESFIRE_2KTDEA_NONCE_SIZE);
    /* Now, RndA is at Buffer[1], RndB' is at Buffer[9] */
    if (memcmp(&Buffer[9], &DesfireCommandState.Authenticate.RndB[1], DESFIRE_2KTDEA_NONCE_SIZE - 1) || 
        (Buffer[16] != DesfireCommandState.Authenticate.RndB[0])) {
        /* Scrub the key */
        memset(&Key, 0x00, sizeof(Key));
        Buffer[0] = STATUS_AUTHENTICATION_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Compose the session key */
    BYTE *SessionKeyData = SessionKey;
    SessionKeyData[0] = Buffer[1];
    SessionKeyData[1] = Buffer[2];
    SessionKeyData[2] = Buffer[3];
    SessionKeyData[3] = Buffer[4];
    SessionKeyData[4] = DesfireCommandState.Authenticate.RndB[0];
    SessionKeyData[5] = DesfireCommandState.Authenticate.RndB[1];
    SessionKeyData[6] = DesfireCommandState.Authenticate.RndB[2];
    SessionKeyData[7] = DesfireCommandState.Authenticate.RndB[3];
    SessionKeyData[8] = Buffer[5];
    SessionKeyData[9] = Buffer[6];
    SessionKeyData[10] = Buffer[7];
    SessionKeyData[11] = Buffer[8];
    SessionKeyData[12] = DesfireCommandState.Authenticate.RndB[4];
    SessionKeyData[13] = DesfireCommandState.Authenticate.RndB[5];
    SessionKeyData[14] = DesfireCommandState.Authenticate.RndB[6];
    SessionKeyData[15] = DesfireCommandState.Authenticate.RndB[7];
    AuthenticatedWithKey = DesfireCommandState.Authenticate.KeyId;
    /* Rotate the nonce A left by 8 bits */
    Buffer[9] = Buffer[1];
    /* Encipher the nonce A; <= 8 bytes = no CBC */
    CryptoEncrypt2KTDEA(&Buffer[2], &Buffer[1], Key);
    /* Scrub the key */
    memset(&Key, 0x00, sizeof(Key)); 
    /* NOTE: EV0: The session IV is reset on each transfer for legacy authentication */

    /* Done */
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE + DESFIRE_2KTDEA_NONCE_SIZE;
}

uint16_t EV0CmdChangeKey(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t KeyId;
    uint8_t ChangeKeyId;
    uint8_t KeySettings;

    /* Validate command length */
    if (ByteCount != 1 + 1 + 3 * CRYPTO_DES_BLOCK_SIZE) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    KeyId = Buffer[1];
    /* Validate number of keys: less than max */
    if (KeyId >= DESFIRE_MAX_KEYS) {
        Buffer[0] = STATUS_PARAMETER_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Validate the state against change key settings */
    KeySettings = ReadKeySettings(SelectedApp.Slot, KeyId);
    ChangeKeyId = KeySettings >> 4;
    switch (ChangeKeyId) {
    case DESFIRE_ALL_KEYS_FROZEN:
        /* Only master key may be (potentially) changed */
        if (KeyId != DESFIRE_MASTER_KEY_ID || !(KeySettings & DESFIRE_ALLOW_MASTER_KEY_CHANGE)) {
            Buffer[0] = STATUS_PERMISSION_DENIED;
            return DESFIRE_STATUS_RESPONSE_SIZE;
        }
        break;
    case DESFIRE_USE_TARGET_KEY:
        /* Authentication with the target key is required */
        if (KeyId != AuthenticatedWithKey) {
            Buffer[0] = STATUS_PERMISSION_DENIED;
            return DESFIRE_STATUS_RESPONSE_SIZE;
        }
        break;
    default:
        /* Authentication with a specific key is required */
        if (KeyId != ChangeKeyId) {
            Buffer[0] = STATUS_PERMISSION_DENIED;
            return DESFIRE_STATUS_RESPONSE_SIZE;
        }
        break;
    }

    /* Encipher to obtain plaintext */
    memset(SessionIV, 0x00, sizeof(SessionIV));
    CryptoEncrypt2KTDEA_CBCReceive(3, &Buffer[2], &Buffer[2], SessionIV, SessionKey);
    /* Verify the checksum first */
    if (!ISO14443ACheckCRCA(&Buffer[2], sizeof(Crypto2KTDEAKeyType))) {
        Buffer[0] = STATUS_INTEGRITY_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* The PCD generates data differently based on whether AuthKeyId == ChangeKeyId */
    if (KeyId != AuthenticatedWithKey) {
        Crypto2KTDEAKeyType OldKey;
        uint8_t i;
        /* NewKey^OldKey | CRC(NewKey^OldKey) | CRC(NewKey) | Padding */
        ReadAppKey(SelectedApp.Slot, KeyId, OldKey, CRYPTO_2KTDEA_KEY_SIZE);
        for (i = 0; i < sizeof(OldKey); ++i) {
            Buffer[2 + i] ^= OldKey[i];
            OldKey[i] = 0;
        }
        /* Overwrite the old CRC */
        Buffer[2 + 16] = Buffer[2 + 18];
        Buffer[2 + 17] = Buffer[2 + 19];
        /* Verify the checksum again */
        if (!ISO14443ACheckCRCA(&Buffer[2], sizeof(Crypto2KTDEAKeyType))) {
            Buffer[0] = STATUS_INTEGRITY_ERROR;
            return DESFIRE_STATUS_RESPONSE_SIZE;
        }
    }
    else {
        /* NewKey | CRC(NewKey) | Padding */
        AuthenticatedWithKey = DESFIRE_NOT_AUTHENTICATED;
    }
    /* NOTE: Padding checks are skipped, because meh (TODO). */

    /* Write the key and scrub */
    WriteAppKey(SelectedApp.Slot, KeyId, &Buffer[2], CRYPTO_2KTDEA_KEY_SIZE);
    memset(&Buffer[2], 0, sizeof(Crypto2KTDEAKeyType));

    /* Done */
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdGetKeySettings(uint8_t* Buffer, uint16_t ByteCount) { // TODO: Check 
    /* Validate command length */
    if (ByteCount != 1) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    // TODO: Need to check the app master key settings to see if requires auth ... 
    Buffer[1] = ReadKeySettings(SelectedApp.Slot, AuthenticatedWithKey);
    Buffer[2] = DESFIRE_MAX_KEYS - 1;

    /* Done */
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE + 2;
}

uint16_t EV0CmdChangeKeySettings(uint8_t* Buffer, uint16_t ByteCount) { // TODO: Check 
    uint8_t NewSettings;

    // TODO: Need to check the app master key settings to see if requires auth ... 

    /* Validate command length */
    if (ByteCount != 1 + CRYPTO_DES_BLOCK_SIZE) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Verify whether settings are changeable */
    if (!(ReadKeySettings(SelectedApp.Slot, AuthenticatedWithKey) & DESFIRE_ALLOW_CONFIG_CHANGE)) {
        Buffer[0] = STATUS_PERMISSION_DENIED;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Verify the master key has been authenticated with */
    if (AuthenticatedWithKey != DESFIRE_MASTER_KEY_ID) {
        Buffer[0] = STATUS_PERMISSION_DENIED;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* Encipher to obtain plaintext */
    CryptoEncrypt2KTDEA(&Buffer[2], &Buffer[2], SessionKey);
    /* Verify the checksum first */
    if (!ISO14443ACheckCRCA(&Buffer[2], sizeof(Crypto2KTDEAKeyType))) {
        Buffer[0] = STATUS_INTEGRITY_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    NewSettings = Buffer[1];
    if (IsPiccAppSelected()) {
        NewSettings &= 0x0F;
    }
    WriteKeySettings(SelectedApp.Slot, AuthenticatedWithKey, NewSettings);

    /* Done */
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE + 2;
}

uint16_t DesfireCmdGetKeyVersion(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

/*
 * DESFire application management commands
 */

uint16_t EV0CmdGetApplicationIds1(uint8_t* Buffer, uint16_t ByteCount) { // TODO: Check 
    /* Validate command length */
    if (ByteCount != 1) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Require the PICC app to be selected */
    if (!IsPiccAppSelected()) {
        Buffer[0] = STATUS_PERMISSION_DENIED;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Verify authentication settings */
    if (!(ReadKeySettings(SelectedApp.Slot, AuthenticatedWithKey) & DESFIRE_FREE_DIRECTORY_LIST) && 
        AuthenticatedWithKey != DESFIRE_MASTER_KEY_ID) {
        /* PICC master key authentication is required */
        Buffer[0] = STATUS_AUTHENTICATION_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* Setup the job and jump to the worker routine */
    GetApplicationIdsSetup();
    return GetApplicationIdsIterator(Buffer, ByteCount);
}

uint16_t EV0CmdCreateApplication(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t Status;
    const DESFireAidType Aid = { Buffer[1], Buffer[2], Buffer[3] };
    uint8_t KeyCount;
    uint8_t KeySettings;
    /* Require the PICC app to be selected */
    if (!IsPiccAppSelected()) {
        Status = STATUS_PERMISSION_DENIED;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate command length */
    if (ByteCount != 1 + 3 + 1 + 1) {
        Status = STATUS_LENGTH_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    KeySettings = Buffer[4];
    KeyCount = Buffer[5];
    /* Validate number of keys: less than max (one for the Master Key) */
    if (KeyCount > DESFIRE_MAX_KEYS || KeyCount == 0) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    if(PMKRequiredForAppCreateDelete() && !AuthenticatedWithPICCMasterKey) {
         /* PICC master key authentication is required */
         Status = STATUS_AUTHENTICATION_ERROR;
         return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Done */
    Status = CreateApp(Aid, KeyCount, KeySettings);
    return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
}



// TODO: Working from here ... 
uint16_t EV0CmdDeleteApplication(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t Status;
    const DESFireAidType Aid = { Buffer[1], Buffer[2], Buffer[3] };
    uint8_t PiccKeySettings;
    /* Validate command length */
    if (ByteCount != 1 + 3) {
        Status = STATUS_LENGTH_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate AID: AID of all zeros cannot be deleted */
    if ((Aid[0] | Aid[1] | Aid[2]) == 0x00) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate authentication: a master key is always required */
    if (AuthenticatedWithKey != DESFIRE_MASTER_KEY_ID) {
        Status = STATUS_AUTHENTICATION_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate authentication: deletion with PICC master key is always OK, 
       but if another app is selected... */
    if (!IsPiccAppSelected()) {
        /* TODO: verify the selected application is the one being deleted */
        PiccKeySettings = GetPiccKeySettings();
        /* Check the PICC key settings whether it is OK to delete using app master key */
        if (!(PiccKeySettings & DESFIRE_FREE_CREATE_DELETE)) {
            Status = STATUS_AUTHENTICATION_ERROR;
            return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
        }
        SelectPiccApp();
        AuthenticatedWithKey = DESFIRE_NOT_AUTHENTICATED;
    }
    /* Done */
    Status = DeleteApp(Aid);
    return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
}

uint16_t EV0CmdSelectApplication(uint8_t* Buffer, uint16_t ByteCount) {
    // handle a special case with EV1
    // docs: https://stackoverflow.com/questions/38232695/m4m-mifare-desfire-ev1-which-mifare-aid-needs-to-be-added-to-nfc-routing-table
    if(ByteCount == 8) {
        const uint8_t DesfireEV1SelectPICCAid[] = { 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x00 };
        if(!memcmp(&Buffer[1], DesfireEV1SelectPICCAid, sizeof(DesfireEV1SelectPICCAid))) {
            SelectPiccApp();
            SynchronizeAppDir();
            Buffer[0] = STATUS_OPERATION_OK;
            return DESFIRE_STATUS_RESPONSE_SIZE;
        }
    }

    const DESFireAidType Aid = { Buffer[1], Buffer[2], Buffer[3] };
    /* Validate command length */
    if (ByteCount != 1 + 3) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Done */
    AuthenticatedWithKey = DESFIRE_NOT_AUTHENTICATED;
    Buffer[0] = SelectApp(Aid);
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t DesfireCmdGetDFNames(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

/*
 * DESFire application file management commands
 */

uint16_t EV0CmdCreateStandardDataFile(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t Status;
    uint8_t FileNum;
    uint8_t CommSettings;
    uint16_t AccessRights;
    __uint24 FileSize;
    /* Validate command length */
    if (ByteCount != 1 + 1 + 1 + 2 + 3) {
        Status = STATUS_LENGTH_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Common args validation */
    FileNum = Buffer[1];
    CommSettings = Buffer[2];
    AccessRights = Buffer[3] | (Buffer[4] << 8);
    Status = CreateFileCommonValidation(FileNum, CommSettings, AccessRights);
    if (Status != STATUS_OPERATION_OK) {
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate the file size */
    FileSize = GET_LE24(&Buffer[5]);
    if (FileSize > 8160) {
        Status = STATUS_OUT_OF_EEPROM_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    Status = CreateStandardFile(FileNum, CommSettings, AccessRights, (uint16_t)FileSize);
    return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
}

uint16_t EV0CmdCreateBackupDataFile(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t Status;
    uint8_t FileNum;
    uint8_t CommSettings;
    uint16_t AccessRights;
    __uint24 FileSize;

    /* Validate command length */
    if (ByteCount != 1 + 1 + 1 + 2 + 3) {
        Status = STATUS_LENGTH_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Common args validation */
    FileNum = Buffer[1];
    CommSettings = Buffer[2];
    AccessRights = Buffer[3] | (Buffer[4] << 8);
    Status = CreateFileCommonValidation(FileNum, CommSettings, AccessRights);
    if (Status != STATUS_OPERATION_OK) {
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate the file size */
    FileSize = GET_LE24(&Buffer[5]);
    if (FileSize > 4096) {
        Status = STATUS_OUT_OF_EEPROM_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    Status = CreateBackupFile(FileNum, CommSettings, AccessRights, (uint16_t)FileSize);
    return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
}

uint16_t EV0CmdCreateValueFile(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdCreateLinearRecordFile(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdCreateCyclicRecordFile(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdDeleteFile(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t Status;
    uint8_t FileNum;
    /* Validate command length */
    if (ByteCount != 1 + 1) {
        Status = STATUS_LENGTH_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    FileNum = Buffer[1];
    /* Validate file number */
    if (FileNum >= DESFIRE_MAX_FILES) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate access settings */
    if (!(ReadKeySettings(SelectedApp.Slot, AuthenticatedWithKey) & DESFIRE_FREE_CREATE_DELETE) && 
        (AuthenticatedWithKey != DESFIRE_MASTER_KEY_ID)) {
        Status = STATUS_AUTHENTICATION_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    Status = DeleteFile(FileNum);
    return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
}

uint16_t EV0CmdGetFileIds(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO 
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdGetFileSettings(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdChangeFileSettings(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

/*
 * DESFire data manipulation commands
 */

uint16_t EV0CmdReadData(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t Status;
    uint8_t FileNum;
    uint8_t CommSettings;
    __uint24 Offset;
    __uint24 Length;

    /* Validate command length */
    if (ByteCount != 1 + 1 + 3 + 3) {
        Status = STATUS_LENGTH_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    FileNum = Buffer[1];
    /* Validate file number */
    if (FileNum >= DESFIRE_MAX_FILES) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }

    Status = SelectFile(FileNum);
    if (Status != STATUS_OPERATION_OK) {
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }

    CommSettings = GetSelectedFileCommSettings();
    /* Verify authentication: read or read&write required */
    switch (ValidateAuthentication(GetSelectedFileAccessRights(), 
            VALIDATE_ACCESS_READWRITE | VALIDATE_ACCESS_READ)) {
    case VALIDATED_ACCESS_DENIED:
        Status = STATUS_AUTHENTICATION_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    case VALIDATED_ACCESS_GRANTED_PLAINTEXT:
        CommSettings = DESFIRE_COMMS_PLAINTEXT;
        /* Fall through */
    case VALIDATED_ACCESS_GRANTED:
        /* Carry on */
        break;
    }

    /* Validate the file type */
    /* TODO: Support for more file types */
    if (GetSelectedFileType() != DESFIRE_FILE_STANDARD_DATA && 
        GetSelectedFileType() != DESFIRE_FILE_BACKUP_DATA) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate offset and length (preliminary) */
    Offset = GET_LE24(&Buffer[2]);
    Length = GET_LE24(&Buffer[5]);
    if (Offset > 8192 || Length > 8192) {
        Status = STATUS_BOUNDARY_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }

    /* Setup and start the transfer */
    Status = ReadDataFileSetup(CommSettings, (uint16_t)Offset, (uint16_t)Length);
    if (Status) {
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    return ReadDataFileIterator(Buffer, 1);
}

uint16_t EV0CmdWriteData(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t Status;
    uint8_t FileNum;
    uint8_t CommSettings;
    __uint24 Offset;
    __uint24 Length;

    /* Validate command length */
    if (ByteCount < 1 + 1 + 3 + 3) {
        Status = STATUS_LENGTH_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    FileNum = Buffer[1];
    /* Validate file number */
    if (FileNum >= DESFIRE_MAX_FILES) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }

    Status = SelectFile(FileNum);
    if (Status != STATUS_OPERATION_OK) {
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }

    CommSettings = GetSelectedFileCommSettings();
    /* Verify authentication: read or read&write required */
    switch (ValidateAuthentication(GetSelectedFileAccessRights(), 
            VALIDATE_ACCESS_READWRITE|VALIDATE_ACCESS_WRITE)) {
    case VALIDATED_ACCESS_DENIED:
        Status = STATUS_AUTHENTICATION_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    case VALIDATED_ACCESS_GRANTED_PLAINTEXT:
        CommSettings = DESFIRE_COMMS_PLAINTEXT;
        /* Fall through */
    case VALIDATED_ACCESS_GRANTED:
        /* Carry on */
        break;
    }

    /* Validate the file type */
    /* TODO: Support for more file types */
    if (GetSelectedFileType() != DESFIRE_FILE_STANDARD_DATA && 
        GetSelectedFileType() != DESFIRE_FILE_BACKUP_DATA) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate offset and length (preliminary) */
    Offset = GET_LE24(&Buffer[2]);
    Length = GET_LE24(&Buffer[5]);
    if (Offset > 8192 || Length > 8192) {
        Status = STATUS_BOUNDARY_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }

    /* Setup and start the transfer */
    Status = WriteDataFileSetup(CommSettings, (uint16_t)Offset, (uint16_t)Length);
    if (Status) {
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    Status = WriteDataFileIterator(&Buffer[8], ByteCount - 8);
    return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
}

uint16_t EV0CmdGetValue(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t Status;
    uint8_t FileNum;
    uint8_t CommSettings;
    TransferStatus XferStatus;

    /* Validate command length */
    if (ByteCount != 1 + 1) {
        Status = STATUS_LENGTH_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    FileNum = Buffer[1];
    /* Validate file number */
    if (FileNum >= DESFIRE_MAX_FILES) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }

    Status = SelectFile(FileNum);
    if (Status != STATUS_OPERATION_OK) {
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }

    CommSettings = GetSelectedFileCommSettings();
    /* Verify authentication: read or read&write required */
    switch (ValidateAuthentication(GetSelectedFileAccessRights(), 
            VALIDATE_ACCESS_READWRITE | VALIDATE_ACCESS_READ | VALIDATE_ACCESS_WRITE)) {
    case VALIDATED_ACCESS_DENIED:
        Status = STATUS_AUTHENTICATION_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    case VALIDATED_ACCESS_GRANTED_PLAINTEXT:
        CommSettings = DESFIRE_COMMS_PLAINTEXT;
        /* Fall through */
    case VALIDATED_ACCESS_GRANTED:
        /* Carry on */
        break;
    }

    /* Validate the file type */
    if (GetSelectedFileType() != DESFIRE_FILE_VALUE_DATA) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }

    /* Setup and start the transfer */
    Status = ReadDataFileSetup(CommSettings, 0, 4);
    if (Status) {
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    XferStatus = ReadDataFileTransfer(&Buffer[1]);
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE + XferStatus.BytesProcessed;
}

uint16_t EV0CmdCredit(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdDebit(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdLimitedCredit(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdReadRecords(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdWriteRecord(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdClearRecords(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

/*
 * DESFire transaction handling commands
 */

uint16_t EV0CmdCommitTransaction(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO 
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdAbortTransaction(uint8_t* Buffer, uint16_t ByteCount) {
    DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

/* 
 * EV1/EV2 supported commands 
 */

uint16_t DesfireCmdAuthenticate3KTDEA1(uint8_t *Buffer, uint16_t ByteCount) {
     DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
     return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t DesfireCmdAuthenticate3KTDEA2(uint8_t *Buffer, uint16_t ByteCount) {
     DESFireLogSourceCodeTODO("", GetSourceFileLoggingData());
     return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t DesfireCmdAuthenticateAES1(uint8_t *Buffer, uint16_t ByteCount) {
    BYTE KeyId, Status;
    BYTE keySize;
    BYTE **Key, **IVBuffer;
    
    /* Reset authentication state right away */
    DESFireLogSourceCodeTODO("Minus One...", GetSourceFileLoggingData());
    InvalidateAuthState(SelectedApp.Slot == DESFIRE_PICC_APP_SLOT);    
    DESFireLogSourceCodeTODO("Zero...", GetSourceFileLoggingData());
    /* Validate command length */
    if(ByteCount != 2) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    KeyId = Buffer[1];
    /* Validate number of keys: less than max */
    if(!KeyIdValid(SelectedApp.Slot, KeyId)) {
        Buffer[0] = STATUS_PARAMETER_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    } 
    /* Make sure that this key is AES, and figure out its byte size */
    BYTE cryptoKeyType = ReadKeyCryptoType(SelectedApp.Slot, KeyId);
    if(!CryptoTypeAES(cryptoKeyType)) {
         Buffer[0] = STATUS_NO_SUCH_KEY;
         return DESFIRE_STATUS_RESPONSE_SIZE;
    }
   
    DESFireLogSourceCodeTODO("One...", GetSourceFileLoggingData());

    InitAESCryptoContext(&AESCryptoContext);
    InitAESCryptoKeyData(&AESCryptoSessionKey);
    InitAESCryptoKeyData(&AESCryptoIVBuffer);
    
    DESFireLogSourceCodeTODO("Two...", GetSourceFileLoggingData());

    keySize = GetDefaultCryptoMethodKeySize(cryptoKeyType);
    *Key = AESCryptoSessionKey;
    *IVBuffer = AESCryptoIVBuffer;

    /* Indicate that we are in AES key authentication land */
    DesfireCommandState.Authenticate.KeyId = KeyId;
    DesfireCommandState.CryptoMethodType = cryptoKeyType;
    DesfireCommandState.ActiveCommMode = GetCryptoMethodCommSettings(cryptoKeyType);

    /* Fetch the key */
    ReadAppKey(SelectedApp.Slot, KeyId, *Key, keySize);
    LogEntry(LOG_APP_AUTH_KEY, (const void *) *Key, keySize);
    Status = DesfireAESCryptoInit(*Key, keySize, &AESCryptoContext);
    if(Status != STATUS_OPERATION_OK) {
         Buffer[0] = Status;
         return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* Generate the nonce B (RndB / Challenge response) */
    if(LocalTestingMode != 0) {
         RandomGetBuffer(DesfireCommandState.Authenticate.RndB, CRYPTO_AES_KEY_SIZE);
    }
    else {
         /* Fixed nonce for testing */
         DesfireCommandState.Authenticate.RndB[0] = 0xCA;
         DesfireCommandState.Authenticate.RndB[1] = 0xFE;
         DesfireCommandState.Authenticate.RndB[2] = 0xBA;
         DesfireCommandState.Authenticate.RndB[3] = 0xBE;
         memset(DesfireCommandState.Authenticate.RndB + 4, 0xff, CRYPTO_CHALLENGE_RESPONSE_BYTES);
         DesfireCommandState.Authenticate.RndB[12] = 0x00;
         DesfireCommandState.Authenticate.RndB[13] = 0x11;
         DesfireCommandState.Authenticate.RndB[14] = 0x22;
         DesfireCommandState.Authenticate.RndB[15] = 0x33;
    }
    LogEntry(LOG_APP_NONCE_B, DesfireCommandState.Authenticate.RndB, CRYPTO_AES_KEY_SIZE);
    
    /* Encrypt RndB with the selected key and transfer it back to the PCD */
    Status = DesfireAESEncryptBuffer(&AESCryptoContext, DesfireCommandState.Authenticate.RndB, 
                                     &Buffer[1], 2 * CRYPTO_CHALLENGE_RESPONSE_BYTES);
    if(Status != STATUS_OPERATION_OK) {
         Buffer[0] = Status;
         return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    
    /* Scrub the key */
    memset(*Key, 0, keySize);

    /* Done */
    DesfireState = DESFIRE_AES_AUTHENTICATE2;
    Buffer[0] = STATUS_ADDITIONAL_FRAME;
    return DESFIRE_STATUS_RESPONSE_SIZE + 2 * CRYPTO_CHALLENGE_RESPONSE_BYTES;

}

uint16_t DesfireCmdAuthenticateAES2(uint8_t *Buffer, uint16_t ByteCount) {
    BYTE KeyId;
    BYTE cryptoKeyType, keySize;
    BYTE **Key, **IVBuffer;

    /* Set status for the next incoming command on error */
    DesfireState = DESFIRE_IDLE;
    /* Validate command length */
    if(ByteCount != CRYPTO_AES_BLOCK_SIZE + 1) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* Reset parameters for authentication from the first exchange */
    KeyId = DesfireCommandState.Authenticate.KeyId;
    cryptoKeyType = DesfireCommandState.CryptoMethodType;
    keySize = GetDefaultCryptoMethodKeySize(cryptoKeyType);
    *Key = ExtractAESKeyBuffer(&AESCryptoSessionKey, keySize);
    *IVBuffer = ExtractAESKeyBuffer(&AESCryptoIVBuffer, keySize);
    ReadAppKey(SelectedApp.Slot, KeyId, *Key, keySize);

    /* Decrypt the challenge sent back to get RndA and a shifted RndB */
    BYTE challengeRndAB[2 * CRYPTO_AES_BLOCK_SIZE];
    BYTE challengeRndA[CRYPTO_CHALLENGE_RESPONSE_BYTES];
    BYTE challengeRndB[CRYPTO_CHALLENGE_RESPONSE_BYTES];
    DesfireAESDecryptBuffer(&AESCryptoContext, &Buffer[1], challengeRndAB, 2 * CRYPTO_AES_BLOCK_SIZE);
    RotateArrayRight(challengeRndAB + CRYPTO_CHALLENGE_RESPONSE_BYTES, challengeRndB, CRYPTO_CHALLENGE_RESPONSE_BYTES);
    memcpy(challengeRndA, challengeRndAB, CRYPTO_CHALLENGE_RESPONSE_BYTES);

    /* Check that the returned RndB matches what we sent in the previous round */
    if(memcmp(DesfireCommandState.Authenticate.RndB, challengeRndB, CRYPTO_CHALLENGE_RESPONSE_BYTES)) {
         InvalidateAuthState(0x00);
         Buffer[0] = STATUS_AUTHENTICATION_ERROR;
         return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    
    /* Authenticated successfully */
    Authenticated = 0x01;
    AuthenticatedWithKey = KeyId;
    AuthenticatedWithPICCMasterKey = (SelectedApp.Slot == DESFIRE_PICC_APP_SLOT) && 
                                     (KeyId == DESFIRE_MASTER_KEY_ID);
    
    /* Encrypt and send back the once rotated RndA buffer to the PCD */
    memset(challengeRndAB, 0x00, 2 * CRYPTO_AES_BLOCK_SIZE);
    memcpy(challengeRndAB, challengeRndA, CRYPTO_CHALLENGE_RESPONSE_BYTES);
    RotateArrayLeft(challengeRndA, challengeRndAB, CRYPTO_CHALLENGE_RESPONSE_BYTES);
    DesfireAESEncryptBuffer(&AESCryptoContext, challengeRndAB, &Buffer[1], CRYPTO_AES_BLOCK_SIZE);

    // TODO: Check that the tag now has the correct key/session data to handle further operations ... 

    /* Scrub the key */
    memset(*Key, 0, keySize);

    /* Return the status on success */
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE + CRYPTO_AES_BLOCK_SIZE;
   
}

uint16_t ISO7816CmdSelect(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t ISO7816CmdGetChallenge(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t ISO7816CmdExternalAuthenticate(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t ISO7816CmdInternalAuthenticate(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t ISO7816CmdReadBinary(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t ISO7816CmdUpdateBinary(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t ISO7816CmdReadRecords(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t ISO7816CmdAppendRecord(uint8_t *Buffer, uint16_t ByteCount) {
    return CmdNotImplemented(Buffer, ByteCount);
}
