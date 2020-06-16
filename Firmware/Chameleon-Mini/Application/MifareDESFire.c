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
#include "DESFire/DESFireLogging.h"

DesfireStateType DesfireState = DESFIRE_IDLE;
BYTE DesfireCmdCLA = 0x90;

/* Dispatching routines */

uint16_t MifareDesfireProcessIdle(uint8_t* Buffer, uint16_t ByteCount) {
    if(ByteCount > 0 && DesfireCmdCLA == 0x90) {
        return ProcessNativeDESFireCommand(Buffer, ByteCount);
    }
    else if(ByteCount > 0 && DesfireCmdCLA == 0x00) {
        return ProcessISO7816Command(Buffer, ByteCount);
    }
    else if(ByteCount == 0) {
         Buffer[0] = STATUS_PARAMETER_ERROR;
         return DESFIRE_STATUS_RESPONSE_SIZE;
    }
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
    case DESFIRE_LEGACY_AUTHENTICATE2:
        return EV0CmdAuthenticate2KTDEA2(Buffer, ByteCount);
    case DESFIRE_ISO_AUTHENTICATE2:
        return CmdNotImplemented(Buffer, ByteCount);
    case DESFIRE_AES_AUTHENTICATE2:
        return CmdNotImplemented(Buffer, ByteCount);
    case DESFIRE_READ_DATA_FILE:
        return ReadDataFileIterator(Buffer, ByteCount);
    default:
        /* Should not happen. */
        Buffer[0] = STATUS_PICC_INTEGRITY_ERROR;
        return DESFIRE_STATUS_RESPONSE_SIZE;
    }
}

uint16_t MifareDesfireProcess(uint8_t* Buffer, uint16_t ByteCount) {
    LogEntry(LOG_INFO_DESFIRE_INCOMING_DATA, Buffer, ByteCount);
    if(ByteCount < 6) {
        return CmdNotImplemented(Buffer, ByteCount);
    }
    /* TODO: Properly detect ISO 7816-4 PDUs and switch modes to avoid doing this all the time */
    if (Buffer[0] == 0x90 && Buffer[2] == 0x00 && Buffer[3] == 0x00 && Buffer[4] == ByteCount - 6) {
        DesfireCmdCLA = Buffer[0];
        /* Unwrap the PDU from ISO 7816-4 */
        ByteCount = Buffer[4];
        Buffer[0] = Buffer[1];
        memmove(&Buffer[1], &Buffer[5], ByteCount);
        /* Process the command */
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
        DesfireCmdCLA = Buffer[0];
        return MifareDesfireProcessCommand(Buffer, ByteCount);
    }
}

void MifareDesfireReset(void) {
    ResetLocalStructureData();
    ResetPiccBackend();
}

void MifareDesfireEV0AppInit(void) {
    /* Init lower layers: nothing for now */
    InitialisePiccBackendEV0(DESFIRE_STORAGE_SIZE_4K);
    /* The rest is handled in reset */
}

static void MifareDesfireEV1AppInit(uint8_t StorageSize) {
    /* Init lower layers: nothing for now */
    InitialisePiccBackendEV1(StorageSize);
    /* The rest is handled in reset */
}

void MifareDesfire2kEV1AppInit(void) {
    MifareDesfireEV1AppInit(DESFIRE_STORAGE_SIZE_2K);
}

void MifareDesfire4kEV1AppInit(void) {
    MifareDesfireEV1AppInit(DESFIRE_STORAGE_SIZE_4K);
}

void MifareDesfire8kEV1AppInit(void) {
    MifareDesfireEV1AppInit(DESFIRE_STORAGE_SIZE_8K);
}

void MifareDesfireAppReset(void) {
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
     InvalidateAuthState(0x00);
}

uint16_t MifareDesfireAppProcess(uint8_t* Buffer, uint16_t BitCount) {
    LogEntry(LOG_INFO_DESFIRE_OUTGOING_DATA, Buffer, BitCount);
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
