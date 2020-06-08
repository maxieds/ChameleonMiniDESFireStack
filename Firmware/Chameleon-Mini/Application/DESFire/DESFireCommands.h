/* 
 * DESFireCommands.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_INS_COMMANDS_H__
#define __DESFIRE_INS_COMMANDS_H__

/*
 * DESFire backend API functions
 */

typedef struct {
    uint8_t BytesProcessed;
    bool IsComplete;
} TransferStatus;

typedef enum {
     DFCMD_SPEC_UNKNOWN   = 0, 
     DFCMD_SPEC_NATIVE    = 1, 
     DFCMD_SPEC_ISO1443   = 2, 
     DFCMD_SPEC_ISO7816   = 3, 
} DESFireCommandSet;

typedef enum {
    // CommandToContinue:
    NO_COMMAND_TO_CONTINUE = 0x00,
    AUTHENTICATE_LEGACY = 0x0A,
    AUTHENTICATE_ISO = 0x1A,
    AUTHENTICATE_AES = 0xAA,
    CHANGE_KEY_SETTINGS = 0x54,
    SET_CONFIGURATION =  0x5C,
    CHANGE_KEY =  0xC4,
    GET_KEY_VERSION = 0x64,
    CREATE_APPLICATION =  0xCA,
    DELETE_APPLICATION =  0xDA,
    GET_APPLICATION_IDS = 0x6A,
    FREE_MEMORY = 0x6E,
    GET_DF_NAMES = 0x6D,
    GET_KEY_SETTINGS = 0x45,
    SELECT_APPLICATION =  0x5A,
    FORMAT_PICC =  0xFC,
    GET_VERSION = 0x60,
    GET_CARD_UID = 0x51,
    GET_FILE_IDS =  0x6F,
    GET_FILE_SETTINGS = 0xF5,
    CHANGE_FILE_SETTINGS = 0x5F,
    CREATE_STDDATAFILE =  0xCD,
    CREATE_BACKUPDATAFILE =  0xCB,
    CREATE_VALUE_FILE =  0xCC,
    CREATE_LINEAR_RECORD_FILE = 0xC1,
    CREATE_CYCLIC_RECORD_FILE = 0xC0,
    DELETE_FILE = 0xDF,
    GET_ISO_FILE_IDS = 0x61,
    READ_DATA =  0x8D,
    WRITE_DATA =  0x3D,
    GET_VALUE = 0x6C,
    CREDIT = 0x0C,
    DEBIT = 0xDC,
    LIMITED_CREDIT = 0x1C,
    WRITE_RECORD = 0x3B,
    READ_RECORDS = 0xBB,
    CLEAR_RECORD_FILE = 0xEB,
    COMMIT_TRANSACTION = 0xC7,
    ABORT_TRANSACTION = 0xA7,
    CONTINUE =  0xAF,
} DESFireInstruction;

// TODO: Need to add in support for the ISO7816-4 commands we support

#define 
