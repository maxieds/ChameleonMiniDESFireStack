/* 
 * DESFireInstructions.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_INS_COMMANDS_H__
#define __DESFIRE_INS_COMMANDS_H__

#include "DESFireFirmwareSettings.h"
#include "DESFireCrypto.h"

/*
 * DESFire backend API functions
 */

typedef struct DESFIRE_FIRMWARE_PACKING {
    BYTE BytesProcessed;
    BOOL IsComplete;
} TransferStatus;

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    // CommandToContinue:
    NO_COMMAND_TO_CONTINUE = 0x00,
    CMD_AUTHENTICATE = 0x0A, /* Authenticate Legacy */
    CMD_AUTHENTICATE_ISO = 0x1A,
    CMD_AUTHENTICATE_AES = 0xAA,
    CMD_CHANGE_KEY_SETTINGS = 0x54,
    CMD_SET_CONFIGURATION =  0x5C,
    CMD_CHANGE_KEY =  0xC4,
    CMD_GET_KEY_VERSION = 0x64,
    CMD_CREATE_APPLICATION =  0xCA,
    CMD_DELETE_APPLICATION =  0xDA,
    CMD_GET_APPLICATION_IDS = 0x6A,
    CMD_FREE_MEMORY = 0x6E,
    CMD_GET_DF_NAMES = 0x6D,
    CMD_GET_KEY_SETTINGS = 0x45,
    CMD_SELECT_APPLICATION =  0x5A,
    CMD_FORMAT_PICC =  0xFC,
    CMD_GET_VERSION = 0x60,
    CMD_GET_CARD_UID = 0x51,
    CMD_GET_FILE_IDS =  0x6F,
    CMD_GET_FILE_SETTINGS = 0xF5,
    CMD_CHANGE_FILE_SETTINGS = 0x5F,
    CMD_CREATE_STDDATAFILE =  0xCD,
    CMD_CREATE_BACKUPDATAFILE =  0xCB,
    CMD_CREATE_VALUE_FILE =  0xCC,
    CMD_CREATE_LINEAR_RECORD_FILE = 0xC1,
    CMD_CREATE_CYCLIC_RECORD_FILE = 0xC0,
    CMD_DELETE_FILE = 0xDF,
    CMD_GET_ISO_FILE_IDS = 0x61,
    CMD_READ_DATA =  0x8D,
    CMD_WRITE_DATA =  0x3D,
    CMD_GET_VALUE = 0x6C,
    CMD_CREDIT = 0x0C,
    CMD_DEBIT = 0xDC,
    CMD_LIMITED_CREDIT = 0x1C,
    CMD_WRITE_RECORD = 0x3B,
    CMD_READ_RECORDS = 0xBB,
    CMD_CLEAR_RECORD_FILE = 0xEB,
    CMD_COMMIT_TRANSACTION = 0xC7,
    CMD_ABORT_TRANSACTION = 0xA7,
    CMD_CONTINUE =  0xAF,
} DESFireCommandType;

// TODO: Need to add in support for the ISO7816-4 commands we support
// TODO: See p. 72 of the data sheet for response codes ... 

#define CLA_PROTECTED_APDU           (0x0c)
#define CLA_ISO7816                  (0)
#define INS_SELECT                   ((uint8_t) -92)
#define INS_EXTERNAL_AUTHENTICATE    ((uint8_t) -126)
#define P1_DF                        (0x04)
#define P2_SELECT                    (0x0C)

#define DO87_START     ((uint8_t) 0x87)
#define DO87_TYPE      ((uint32_t) 0x00000080)
#define DO87_BYTENO    ((uint8_t) 0x7F)
#define DO87_END       ((uint8_t) 0x01);
#define LE_MAX         ((uint16_t) 256);

extern BYTE VERSION_1[];
extern BYTE VERSION_2[];
extern BYTE VERSION_3[];

typedef union DESFIRE_FIRMWARE_PACKING {
    struct DESFIRE_FIRMWARE_PACKING {
        uint8_t KeyId;
        uint8_t RndB[CRYPTO_DES_KEY_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    } Authenticate;
    struct DESFIRE_FIRMWARE_PACKING {
        uint8_t KeyId;
        uint8_t RndB[CRYPTO_3KTDEA_KEY_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    } AuthenticateIso;
    struct DESFIRE_FIRMWARE_PACKING {
        uint8_t KeyId;
        uint8_t RndB[CRYPTO_AES_KEY_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    } AuthenticateAES;
    struct DESFIRE_FIRMWARE_PACKING {
        uint8_t NextIndex;
    } GetApplicationIds;
} DesfireSavedCommandStateType;

extern DesfireSavedCommandStateType DesfireCommandState;

/*
 * The following section implements:
 * DESFire EV0 / D40 specific commands
 */

/* General commands */
uint16_t EV0CmdGetVersion1(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdGetVersion2(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdGetVersion3(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdFormatPicc(uint8_t *Buffer, uint16_t ByteCount);

/* Key management commands */
uint16_t EV0CmdAuthenticate2KTDEA1(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdAuthenticate2KTDEA2(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdChangeKey(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdGetKeySettings(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdChangeKeySettings(uint8_t *Buffer, uint16_t ByteCount);

/* Application management commands */
uint16_t EV0CmdGetApplicationIds1(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdCreateApplication(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdDeleteApplication(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdSelectApplication(uint8_t *Buffer, uint16_t ByteCount);

/* File management commands */
uint16_t EV0CmdCreateStandardDataFile(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdCreateBackupDataFile(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdCreateValueFile(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdCreateLinearRecordFile(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdCreateCyclicRecordFile(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdDeleteFile(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdGetFileIds(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdGetFileSettings(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdChangeFileSettings(uint8_t *Buffer, uint16_t ByteCount);

/* Data manipulation commands */
uint16_t EV0CmdReadData(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdWriteData(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdGetValue(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdCredit(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdDebit(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdLimitedCredit(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdReadRecords(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdWriteRecord(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdClearRecords(uint8_t *Buffer, uint16_t ByteCount);

/* Transaction handling commands */
uint16_t EV0CmdCommitTransaction(uint8_t *Buffer, uint16_t ByteCount);
uint16_t EV0CmdAbortTransaction(uint8_t *Buffer, uint16_t ByteCount);

#endif 
