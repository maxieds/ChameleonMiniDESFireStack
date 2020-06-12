/* 
 * DESFireInstructions.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "DESFireInstructions.h"
#include "DESFirePICCControl.h"
#include "DESFireStatusCodes.h"
#include "DESFireLogging.h"

#include "../../Configuration.h"
#include "../../Memory.h"
#include "../../Random.h"

const BYTE VERSION1[] = { 
     0x04, 0x01, 0x01, 0x01, 0x00, 0x1a, 0x05 
};
const BYTE VERSION2[] = { 
     0x04, 0x01, 0x01, 0x01, 0x03, 0x1a, 0x05 
};
const BYTE VERSION3[] = { 
    // Expected Response: 00  04  91  3a  29  93  26  80  00  00  00  00  00  39  08  91  00
    0x04, (BYTE) 0x91, 0x3a, 0x29, (BYTE) 0x93, 
    0x26, (BYTE) 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x08 
};

DesfireSavedCommandStateType DesfireCommandState = { 0 };

static uint16_t ExitWithStatus(uint8_t *Buffer, uint8_t StatusCode, uint16_t DefaultReturnValue) {
     Buffer[0] = StatusCode;
     return DefaultReturnValue;
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
    return VERSION1_BYTES_PROCESSED;
}

uint16_t EV0CmdGetVersion2(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_ADDITIONAL_FRAME;
    Buffer[1] = DESFIRE_MANUFACTURER_ID;
    Buffer[2] = DESFIRE_TYPE;
    Buffer[3] = DESFIRE_SUBTYPE;
    GetPiccSoftwareVersionInfo(&Buffer[4]);
    Buffer[7] = DESFIRE_SW_PROTOCOL_TYPE;
    DesfireState = DESFIRE_GET_VERSION3;
    return VERSION2_BYTES_PROCESSED;
}

uint16_t EV0CmdGetVersion3(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = STATUS_OPERATION_OK;
    GetPiccManufactureInfo(&Buffer[1]);
    DesfireState = DESFIRE_IDLE;
    return VERSION3_BYTES_PROCESSED;
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
    if (AuthenticatedWithKey != DESFIRE_MASTER_KEY_ID) {
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

uint16_t EV0CmdAuthenticate2KTDEA1(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t KeyId;
    Desfire2KTDEAKeyType Key;

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
    ReadSelectedAppKey(KeyId, Key);
    LogEntry(LOG_APP_AUTH_KEY, Key, sizeof(Key));
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
    DesfireState = DESFIRE_AUTHENTICATE2;
    Buffer[0] = STATUS_ADDITIONAL_FRAME;
    return DESFIRE_STATUS_RESPONSE_SIZE + DESFIRE_2KTDEA_NONCE_SIZE;
}

uint16_t EV0CmdAuthenticate2KTDEA2(uint8_t* Buffer, uint16_t ByteCount) {
    Desfire2KTDEAKeyType Key;
    DesfireState = DESFIRE_IDLE;
    ActiveAuthType = DESFIRE_AUTH_LEGACY;

    /* Validate command length */
    if (ByteCount != 1 + 2 * CRYPTO_DES_BLOCK_SIZE) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* Fetch the key */
    ReadSelectedAppKey(DesfireCommandState.Authenticate.KeyId, Key);
    LogEntry(LOG_APP_AUTH_KEY, Key, sizeof(Key));
    /* Encipher to obtain plain text; zero IV = no CBC */
    memset(ExtractIVBufferData(ActiveAuthType, &SessionIV), 0, sizeof(ExtractIVBufferData(ActiveAuthType, &SessionIV)));
    CryptoEncrypt2KTDEA_CBCReceive(2, &Buffer[1], &Buffer[1], ExtractIVBufferData(ActiveAuthType, &SessionIV), Key);
    LogEntry(LOG_APP_NONCE_AB, &Buffer[1], 2 * DESFIRE_2KTDEA_NONCE_SIZE);
    /* Now, RndA is at Buffer[1], RndB' is at Buffer[9] */
    if (memcmp(&Buffer[9], &DesfireCommandState.Authenticate.RndB[1], DESFIRE_2KTDEA_NONCE_SIZE - 1) || 
        (Buffer[16] != DesfireCommandState.Authenticate.RndB[0])) {
        /* Scrub the key */
        memset(&Key, 0, sizeof(Key));
        Buffer[0] = STATUS_AUTHENTICATION_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Compose the session key */
    BYTE *SessionKeyData = ExtractSessionKeyData(ActiveAuthType, &SessionKey);
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
    memset(&Key, 0, sizeof(Key)); // TODO: Check legacy auth? 
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
    KeySettings = GetSelectedAppKeySettings();
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
    memset(ExtractIVBufferData(ActiveAuthType, &SessionIV), 0, sizeof(ExtractIVBufferData(ActiveAuthType, &SessionIV)));
    CryptoEncrypt2KTDEA_CBCReceive(3, &Buffer[2], &Buffer[2], 
         ExtractIVBufferData(ActiveAuthType, &SessionIV), ExtractSessionKeyData(ActiveAuthType, &SessionKey));
    /* Verify the checksum first */
    if (!ISO14443ACheckCRCA(&Buffer[2], sizeof(Desfire2KTDEAKeyType))) {
        Buffer[0] = STATUS_INTEGRITY_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* The PCD generates data differently based on whether AuthKeyId == ChangeKeyId */
    if (KeyId != AuthenticatedWithKey) {
        Desfire2KTDEAKeyType OldKey;
        uint8_t i;

        /* NewKey^OldKey | CRC(NewKey^OldKey) | CRC(NewKey) | Padding */
        ReadSelectedAppKey(KeyId, OldKey);
        for (i = 0; i < sizeof(OldKey); ++i) {
            Buffer[2 + i] ^= OldKey[i];
            OldKey[i] = 0;
        }
        /* Overwrite the old CRC */
        Buffer[2 + 16] = Buffer[2 + 18];
        Buffer[2 + 17] = Buffer[2 + 19];
        /* Verify the checksum again */
        if (!ISO14443ACheckCRCA(&Buffer[2], sizeof(Desfire2KTDEAKeyType))) {
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
    WriteSelectedAppKey(KeyId, &Buffer[2]);
    memset(&Buffer[2], 0, sizeof(Desfire2KTDEAKeyType));

    /* Done */
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t EV0CmdGetKeySettings(uint8_t* Buffer, uint16_t ByteCount) {
    /* Validate command length */
    if (ByteCount != 1) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    Buffer[1] = GetSelectedAppKeySettings();
    Buffer[2] = DESFIRE_MAX_KEYS - 1;

    /* Done */
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE + 2;
}

uint16_t EV0CmdChangeKeySettings(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t NewSettings;

    /* Validate command length */
    if (ByteCount != 1 + CRYPTO_DES_BLOCK_SIZE) {
        Buffer[0] = STATUS_LENGTH_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Verify whether settings are changeable */
    if (!(GetSelectedAppKeySettings() & DESFIRE_ALLOW_CONFIG_CHANGE)) {
        Buffer[0] = STATUS_PERMISSION_DENIED;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
    /* Verify the master key has been authenticated with */
    if (AuthenticatedWithKey != DESFIRE_MASTER_KEY_ID) {
        Buffer[0] = STATUS_PERMISSION_DENIED;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    /* Encipher to obtain plaintext */
    CryptoEncrypt2KTDEA(&Buffer[2], &Buffer[2], ExtractSessionKeyData(ActiveAuthType, &SessionKey));
    /* Verify the checksum first */
    if (!ISO14443ACheckCRCA(&Buffer[2], sizeof(Desfire2KTDEAKeyType))) {
        Buffer[0] = STATUS_INTEGRITY_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    NewSettings = Buffer[1];
    if (IsPiccAppSelected()) {
        NewSettings &= 0x0F;
    }
    SetSelectedAppKeySettings(NewSettings);

    /* Done */
    Buffer[0] = STATUS_OPERATION_OK;
    return DESFIRE_STATUS_RESPONSE_SIZE + 2;
}

/*
 * DESFire application management commands
 */

uint16_t EV0CmdGetApplicationIds1(uint8_t* Buffer, uint16_t ByteCount) {
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
    if (!(GetSelectedAppKeySettings() & DESFIRE_FREE_DIRECTORY_LIST) && 
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
    /* Verify authentication settings */
    if (!(GetSelectedAppKeySettings() & DESFIRE_FREE_CREATE_DELETE) && 
        AuthenticatedWithKey != DESFIRE_MASTER_KEY_ID) {
        /* PICC master key authentication is required */
        Status = STATUS_AUTHENTICATION_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    /* Validate number of keys: less than max */
    KeyCount = Buffer[5];
    if (KeyCount > DESFIRE_MAX_KEYS) {
        Status = STATUS_PARAMETER_ERROR;
        return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
    }
    KeySettings = Buffer[4];
    /* Done */
    Status = CreateApp(Aid, KeyCount, KeySettings);
    return ExitWithStatus(Buffer, Status, DESFIRE_STATUS_RESPONSE_SIZE);
}

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
    if (!(GetSelectedAppKeySettings() & DESFIRE_FREE_CREATE_DELETE) && AuthenticatedWithKey != DESFIRE_MASTER_KEY_ID) {
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
            VALIDATE_ACCESS_READWRITE|VALIDATE_ACCESS_READ|VALIDATE_ACCESS_WRITE)) {
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
    Status = ReadValueFileSetup(CommSettings);
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


