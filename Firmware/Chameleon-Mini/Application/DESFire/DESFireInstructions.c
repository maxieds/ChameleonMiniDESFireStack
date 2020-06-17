/* 
 * DESFireInstructions.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "../../Configuration.h"
#include "../../Memory.h"
#include "../../Random.h"

#include "DESFireInstructions.h"
#include "DESFirePICCControl.h"
#include "DESFireCrypto.h"
#include "DESFireStatusCodes.h"
#include "DESFireLogging.h"
#include "DESFireUtils.h"
#include "../MifareDESFire.h"

DesfireSavedCommandStateType DesfireCommandState = { 0 };

static uint16_t ExitWithStatus(uint8_t *Buffer, uint8_t StatusCode, uint16_t DefaultReturnValue) {
     Buffer[0] = StatusCode;
     return DefaultReturnValue;
}

uint16_t CmdNotImplemented(uint8_t* Buffer, uint16_t ByteCount) {
    // TODO: Logging TODO ... (__FILE__, __LINE__, __FUNC__) 
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE;
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t ProcessNativeDESFireCommand(uint8_t *Buffer, uint16_t ByteCount) {

    /* Handle EV0 commands */
    switch (Buffer[0]) {
    case CMD_GET_VERSION:
        return EV0CmdGetVersion1(Buffer, ByteCount);
    case CMD_FORMAT_PICC:
        return EV0CmdFormatPicc(Buffer, ByteCount);

    case CMD_AUTHENTICATE:
        return EV0CmdAuthenticate2KTDEA1(Buffer, ByteCount);
    case CMD_CHANGE_KEY:
        return EV0CmdChangeKey(Buffer, ByteCount);
    case CMD_GET_KEY_SETTINGS:
        return EV0CmdGetKeySettings(Buffer, ByteCount);
    case CMD_CHANGE_KEY_SETTINGS:
        return EV0CmdChangeKeySettings(Buffer, ByteCount);

    case CMD_GET_APPLICATION_IDS:
        return EV0CmdGetApplicationIds1(Buffer, ByteCount);
    case CMD_CREATE_APPLICATION:
        return EV0CmdCreateApplication(Buffer, ByteCount);
    case CMD_DELETE_APPLICATION:
        return EV0CmdDeleteApplication(Buffer, ByteCount);
    case CMD_SELECT_APPLICATION:
        return EV0CmdSelectApplication(Buffer, ByteCount);

    case CMD_CREATE_STDDATAFILE:
        return EV0CmdCreateStandardDataFile(Buffer, ByteCount);
    case CMD_CREATE_BACKUPDATAFILE:
        return EV0CmdCreateBackupDataFile(Buffer, ByteCount);
    case CMD_CREATE_VALUE_FILE:
        return EV0CmdCreateValueFile(Buffer, ByteCount);
    case CMD_CREATE_LINEAR_RECORD_FILE:
        return EV0CmdCreateLinearRecordFile(Buffer, ByteCount);
    case CMD_CREATE_CYCLIC_RECORD_FILE:
        return EV0CmdCreateCyclicRecordFile(Buffer, ByteCount);
    case CMD_DELETE_FILE:
        return EV0CmdDeleteFile(Buffer, ByteCount);
    case CMD_GET_FILE_IDS:
        return EV0CmdGetFileIds(Buffer, ByteCount);
    case CMD_GET_FILE_SETTINGS:
        return EV0CmdGetFileSettings(Buffer, ByteCount);
    case CMD_CHANGE_FILE_SETTINGS:
        return EV0CmdChangeFileSettings(Buffer, ByteCount);

    case CMD_READ_DATA:
        return EV0CmdReadData(Buffer, ByteCount);
    case CMD_WRITE_DATA:
        return EV0CmdWriteData(Buffer, ByteCount);

    case CMD_GET_VALUE:
        return EV0CmdGetValue(Buffer, ByteCount);
    case CMD_CREDIT:
        return EV0CmdCredit(Buffer, ByteCount);
    case CMD_DEBIT:
        return EV0CmdDebit(Buffer, ByteCount);
    case CMD_LIMITED_CREDIT:
        return EV0CmdLimitedCredit(Buffer, ByteCount);

    case CMD_READ_RECORDS:
        return EV0CmdReadRecords(Buffer, ByteCount);
    case CMD_WRITE_RECORD:
        return EV0CmdWriteRecord(Buffer, ByteCount);
    case CMD_CLEAR_RECORD_FILE:
        return EV0CmdClearRecords(Buffer, ByteCount);

    case CMD_COMMIT_TRANSACTION:
        return EV0CmdCommitTransaction(Buffer, ByteCount);
    case CMD_ABORT_TRANSACTION:
        return EV0CmdAbortTransaction(Buffer, ByteCount);

    case CMD_CONTINUE:
        Buffer[0] = STATUS_OPERATION_OK;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    
    default:
        break;
    }

    /* Handle EV1 commands, if enabled */
    /* Handle EV2 commands -- in future */
    if (IsEmulatingEV1()) {
         switch(Buffer[0]) {
              case CMD_AUTHENTICATE_ISO:
              case CMD_AUTHENTICATE_AES:
              case CMD_AUTHENTICATE_EV2_FIRST:
              case CMD_AUTHENTICATE_EV2_NONFIRST:
              case CMD_SET_CONFIGURATION:
              case CMD_GET_KEY_VERSION:
              case CMD_FREE_MEMORY:
              case CMD_GET_DF_NAMES:
                  break;
              case CMD_GET_CARD_UID: /* TODO: Possible randomization if #if defined(DESFIRE_RANDOMIZE_UIDS_PREAUTH) */
                   return DesfireCmdGetCardUID(Buffer, ByteCount);
              case CMD_GET_ISO_FILE_IDS:
              default:
                   break;
         }
    }

    return CmdNotImplemented(Buffer, ByteCount);

}

uint16_t ProcessISO7816Command(uint8_t *Buffer, uint16_t ByteCount) {
     switch(Buffer[0]) { 
          case CMD_ISO7816_SELECT:
          case CMD_ISO7816_GET_CHALLENGE:
          case CMD_ISO7816_EXTERNAL_AUTHENTICATE:
          case CMD_ISO7816_INTERNAL_AUTHENTICATE:
          case CMD_ISO7816_READ_BINARY:
          case CMD_ISO7816_UPDATE_BINARY:
          case CMD_ISO7816_READ_RECORDS:
          case CMD_ISO7816_APPEND_RECORD:
          default:
               break;
     }
     return CmdNotImplemented(Buffer, ByteCount);
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
    FormatPicc();
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE;
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
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO 
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdGetFileSettings(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdChangeFileSettings(uint8_t* Buffer, uint16_t ByteCount) {
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
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdDebit(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdLimitedCredit(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdReadRecords(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdWriteRecord(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdClearRecords(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

/*
 * DESFire transaction handling commands
 */

uint16_t EV0CmdCommitTransaction(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO 
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdAbortTransaction(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE; // TODO
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

/* 
 * EV1/EV2 supported commands 
 */

uint16_t DesfireCmdAuthenticate3KTDEA1(uint8_t *Buffer, uint16_t ByteCount) {
     return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t DesfireCmdAuthenticate3KTDEA2(uint8_t *Buffer, uint16_t ByteCount) {
     return CmdNotImplemented(Buffer, ByteCount);
}

uint16_t DesfireCmdAuthenticateAES1(uint8_t *Buffer, uint16_t ByteCount) {
    BYTE KeyId, Status;
    BYTE keySize;
    BYTE **Key, **IVBuffer;
    
    /* Reset authentication state right away */
    InvalidateAuthState(SelectedApp.Slot == DESFIRE_PICC_APP_SLOT);
    InitAESCryptoContext(&AESCryptoContext);
    InitAESCryptoKeyData(&AESCryptoSessionKey);
    InitAESCryptoKeyData(&AESCryptoIVBuffer);
    
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
    keySize = GetDefaultCryptoMethodKeySize(cryptoKeyType);
    *Key = ExtractAESKeyBuffer(&AESCryptoSessionKey, keySize);
    *IVBuffer = ExtractAESKeyBuffer(&AESCryptoIVBuffer, keySize);

    /* Indicate that we are in AES key authentication land */
    DesfireCommandState.AuthenticateAES.KeyId = KeyId;
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
         RandomGetBuffer(DesfireCommandState.Authenticate.RndB, CRYPTO_CHALLENGE_RESPONSE_BYTES);
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
    LogEntry(LOG_APP_NONCE_B, DesfireCommandState.Authenticate.RndB, CRYPTO_CHALLENGE_RESPONSE_BYTES);
    
    /* Encrypt RndB with the selected key and transfer it back to the PCD */
    Status = DesfireAESEncryptBuffer(&AESCryptoContext, DesfireCommandState.Authenticate.RndB, 
                                     &Buffer[1], CRYPTO_CHALLENGE_RESPONSE_BYTES);
    if(Status != STATUS_OPERATION_OK) {
         Buffer[0] = Status;
         return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    
    /* Scrub the key */
    memset(*Key, 0, keySize);

    /* Done */
    DesfireState = DESFIRE_AES_AUTHENTICATE2;
    Buffer[0] = STATUS_ADDITIONAL_FRAME;
    return DESFIRE_STATUS_RESPONSE_SIZE + CRYPTO_CHALLENGE_RESPONSE_BYTES;

}

// TODO: Check the procedure with one of the example links ... 
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
    KeyId = DesfireCommandState.AuthenticateAES.KeyId;
    cryptoKeyType = DesfireCommandState.CryptoMethodType;
    keySize = GetDefaultCryptoMethodKeySize(cryptoKeyType);
    *Key = ExtractAESKeyBuffer(&AESCryptoSessionKey, keySize);
    *IVBuffer = ExtractAESKeyBuffer(&AESCryptoIVBuffer, keySize);
    ReadAppKey(SelectedApp.Slot, KeyId, *Key, keySize);

    /* Decrypt the challenge sent back to get RndA and a shifted RndB */
    BYTE challengeRndAB[CRYPTO_AES_BLOCK_SIZE];
    BYTE challengeRndA[CRYPTO_CHALLENGE_RESPONSE_BYTES];
    BYTE challengeRndB[CRYPTO_CHALLENGE_RESPONSE_BYTES];
    DesfireAESDecryptBuffer(&AESCryptoContext, &Buffer[1], challengeRndAB, CRYPTO_AES_BLOCK_SIZE);
    RotateArrayRight(challengeRndAB, challengeRndA, CRYPTO_CHALLENGE_RESPONSE_BYTES);
    memcpy(challengeRndB, challengeRndAB + CRYPTO_CHALLENGE_RESPONSE_BYTES, CRYPTO_CHALLENGE_RESPONSE_BYTES);

    /* Check that the returned RndB matches what we sent in the previous round */
    if(memcmp(DesfireCommandState.AuthenticateAES.RndB, challengeRndB, CRYPTO_CHALLENGE_RESPONSE_BYTES)) {
         InvalidateAuthState(0x00);
         Buffer[0] = STATUS_AUTHENTICATION_ERROR;
         return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    
    /* Authenticated successfully */
    Authenticated = 0x01;
    AuthenticatedWithKey = KeyId;
    AuthenticatedWithPICCMasterKey = (SelectedApp.Slot == DESFIRE_PICC_APP_SLOT) && 
                                     (KeyId == DESFIRE_MASTER_KEY_ID);
    
    /* Encrypt and send back the RndA buffer to the PCD */
    memset(challengeRndAB, 0x00, CRYPTO_AES_BLOCK_SIZE);
    memcpy(challengeRndAB, challengeRndA, CRYPTO_CHALLENGE_RESPONSE_BYTES);
    DesfireAESEncryptBuffer(&AESCryptoContext, challengeRndAB, &Buffer[1], CRYPTO_AES_BLOCK_SIZE);

    /* Scrub the key */
    memset(*Key, 0, keySize);

    /* Return the status on success */
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE + CRYPTO_AES_BLOCK_SIZE;
   
}

uint16_t DesfireCmdGetCardUID(uint8_t *Buffer, uint16_t ByteCount) {
     return CmdNotImplemented(Buffer, ByteCount);
}
