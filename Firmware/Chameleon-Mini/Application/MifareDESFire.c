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
#include "DESFire/DESFireAPDU.h"

DesfireStateType DesfireState = DESFIRE_IDLE;

/* Dispatching routines */

uint16_t MifareDesfireProcessIdle(uint8_t* Buffer, uint16_t ByteCount) {
    
    // TODO: Check to make sure are handling *ALL* possible commands ... 
    // TODO: LogEntry(LOG_INFO_DESFIRE_IN, Buffer, ByteCount); // TODO
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

    default:
        break;
    }

    /* Handle EV1 commands, if enabled */
    if (IsEmulatingEV1()) {
        /* TODO: To be implemented */
    }

    /* TODO: Handle EV2 commands -- in future */
    /* TODO: Support for limited ISO7816-4 command set ... */

    Buffer[0] = STATUS_ILLEGAL_COMMAND_CODE;
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint16_t MifareDesfireProcessCommand(uint8_t* Buffer, uint16_t ByteCount) {
    if (DesfireState == DESFIRE_IDLE)
        return MifareDesfireProcessIdle(Buffer, ByteCount);

    /* Expecting further data here */
    if (Buffer[0] != STATUS_ADDITIONAL_FRAME) {
        AbortTransaction();
        DesfireState = DESFIRE_IDLE;
        Buffer[0] = STATUS_COMMAND_ABORTED;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }

    switch (DesfireState) {
    case DESFIRE_GET_VERSION2:
        return EV0CmdGetVersion2(Buffer, ByteCount);
    case DESFIRE_GET_VERSION3:
        return EV0CmdGetVersion3(Buffer, ByteCount);
    case DESFIRE_GET_APPLICATION_IDS2:
        return GetApplicationIdsIterator(Buffer, ByteCount);
    case DESFIRE_AUTHENTICATE2:
        return EV0CmdAuthenticate2KTDEA2(Buffer, ByteCount);
    case DESFIRE_READ_DATA_FILE:
        return ReadDataFileIterator(Buffer, ByteCount);
    default:
        /* Should not happen. */
        Buffer[0] = STATUS_PICC_INTEGRITY_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
}

uint16_t MifareDesfireProcess(uint8_t* Buffer, uint16_t ByteCount) {
    /* TODO: Properly detect ISO 7816-4 PDUs and switch modes to avoid doing ths all the time */
    if (Buffer[0] == 0x90 && Buffer[2] == 0x00 && Buffer[3] == 0x00 && Buffer[4] == ByteCount - 6) {
        /* Unwrap the PDU from ISO 7816-4 */
        ByteCount = Buffer[4];
        Buffer[0] = Buffer[1];
        memmove(&Buffer[1], &Buffer[5], ByteCount);
        /* Process the command */
        // TODO: LogEntry(LOG_INFO_DESFIRE_OUT, Buffer, ByteCount);
        ByteCount = MifareDesfireProcessCommand(Buffer, ByteCount + 1);
        if (ByteCount) {
            /* Re-wrap into ISO 7816-4 */
            Buffer[ByteCount] = Buffer[0];
            memmove(&Buffer[0], &Buffer[1], ByteCount - 1);
            Buffer[ByteCount - 1] = 0x91;
            ByteCount++;
        }
        return ByteCount;
    }
    else {
        /* ISO/IEC 14443-4 PDUs, no extra work */
        return MifareDesfireProcessCommand(Buffer, ByteCount);
    }
}

void MifareDesfireReset(void) {
    ResetLocalStructureData();
    ResetPiccBackend();
}

void MifareDesfireEV0AppInit(void)
{
    /* Init lower layers: nothing for now */
    InitialisePiccBackendEV0(DESFIRE_STORAGE_SIZE_4K);
    /* The rest is handled in reset */
}

static void MifareDesfireEV1AppInit(uint8_t StorageSize)
{
    /* Init lower layers: nothing for now */
    InitialisePiccBackendEV1(StorageSize);
    /* The rest is handled in reset */
}

void MifareDesfire2kEV1AppInit(void)
{
    MifareDesfireEV1AppInit(DESFIRE_STORAGE_SIZE_2K);
}

void MifareDesfire4kEV1AppInit(void)
{
    MifareDesfireEV1AppInit(DESFIRE_STORAGE_SIZE_4K);
}

void MifareDesfire8kEV1AppInit(void)
{
    MifareDesfireEV1AppInit(DESFIRE_STORAGE_SIZE_8K);
}

void MifareDesfireAppReset(void)
{
    /* This is called repeatedly, so limit the amount of work done */
    ISO144433AReset();
    ISO144434Reset();
    MifareDesfireReset();
}

void MifareDesfireAppTask(void)
{
    /* Empty */
}

void ResetLocalStructureData(void) {
     memset(&DESFireInternalAPDUCommand, 0x00, sizeof(DESFireInternalAPDUCommand));
     memset(&DESFireInternalAPDUResponse, 0x00, sizeof(DESFireInternalAPDUResponse));
     memset(&SelectedApp, 0x00, sizeof(SelectedApp));
     memset(&SelectedFile, 0x00, sizeof(SelectedFile));
     memset(&TransferState, 0x00, sizeof(TransferState));
     Iso144434State = ISO14443_4_STATE_EXPECT_RATS;
     Iso144434BlockNumber = 0x00;
     Iso144434CardID = 0x00;
     Iso144434LastBlockLength = 0x00;
     memset(Iso144434LastBlock, 0x00, CODEC_BUFFER_SIZE);
     Iso144433AState = ISO14443_3A_STATE_IDLE;
     Iso144433AIdleState = ISO14443_3A_STATE_IDLE;
     memset(&SessionKey, 0x00, sizeof(CryptoKeyBufferType));
     memset(&SessionIV, 0x00, sizeof(CryptoIVBufferType));
     SessionIVByteSize = 0x00;
     memset(&AESCryptoContext, 0x00, sizeof(DesfireAESCryptoContext));
     memset(&AESCryptoKey, 0x00, sizeof(DesfireAESCryptoKey));
     memset(&AESCryptoRndB, 0x00, sizeof(DesfireAESCryptoKey));
     memset(&AESCryptoIVBuffer, 0x00, sizeof(DesfireAESCryptoKey));
     AESAuthState = AESAUTH_STATE_IDLE;
     DesfireState = DESFIRE_IDLE;
     AuthenticatedWithKey = DESFIRE_NOT_AUTHENTICATED;
}

uint16_t MifareDesfireAppProcess(uint8_t* Buffer, uint16_t BitCount)
{
    return ISO144433APiccProcess(Buffer, BitCount);
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
