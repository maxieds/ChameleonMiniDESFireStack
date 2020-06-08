/*
 * DesfireImpl.h
 * MIFARE DESFire backend
 *
 *  Created on: 02.11.2016
 *      Author: dev_zzo
 */

#include "../ISO14443-3A.h"
#include "../CryptoTDEA.h"
#include "../../Configuration.h"
#include "../../Memory.h"
#include "../../Random.h"

#define ISO7816_4_CURRENT_EF_FILE_ID    0x0000
#define ISO7816_4_CURRENT_DF_FILE_ID    0x3FFF
#define ISO7816_4_MASTER_FILE_ID        0x3F00

/* Various limits */

#define DESFIRE_UID_SIZE     ISO14443A_UID_SIZE_DOUBLE

#define DESFIRE_AID_SIZE 3

#define DESFIRE_MAX_APPS        28
#define DESFIRE_MAX_KEYS        14
#define DESFIRE_MAX_FILES_EV0   16
#define DESFIRE_MAX_FILES_EV1   32
#define DESFIRE_MAX_FILES       DESFIRE_MAX_FILES_EV1

#define DESFIRE_MAX_PAYLOAD_SIZE 59 /* Bytes */

/* File types */

#define DESFIRE_FILE_STANDARD_DATA      0
#define DESFIRE_FILE_BACKUP_DATA        1
#define DESFIRE_FILE_VALUE_DATA         2
#define DESFIRE_FILE_LINEAR_RECORDS     3
#define DESFIRE_FILE_CIRCULAR_RECORDS   4

/** Data about an application's file is currently kept in this structure.
 * The location of these structures is defined by the file index.
 */

enum mifare_desfire_file_types {
    MDFT_STANDARD_DATA_FILE             = 0x00,
    MDFT_BACKUP_DATA_FILE               = 0x01,
    MDFT_VALUE_FILE_WITH_BACKUP         = 0x02,
    MDFT_LINEAR_RECORD_FILE_WITH_BACKUP = 0x03,
    MDFT_CYCLIC_RECORD_FILE_WITH_BACKUP = 0x04
};


/* Special values for access key IDs */

#define DESFIRE_ACCESS_FREE     0xE
#define DESFIRE_ACCESS_DENY     0xF

#define DESFIRE_CHANGE_ACCESS_RIGHTS_SHIFT      (0*4)
#define DESFIRE_READWRITE_ACCESS_RIGHTS_SHIFT   (1*4)
#define DESFIRE_WRITE_ACCESS_RIGHTS_SHIFT       (2*4)
#define DESFIRE_READ_ACCESS_RIGHTS_SHIFT        (3*4)

/* Communication mode */

#define DESFIRE_COMMS_PLAINTEXT         0
#define DESFIRE_COMMS_PLAINTEXT_MAC     1
#define DESFIRE_COMMS_CIPHERTEXT_DES    3

/* PICC / Application master key settings */
/* Mifare DESFire master key settings
bit 7 - 4: Always 0.
bit 3: PICC master key settings frozen = 0 (WARNING - this is irreversible); PICC master key settings changeable when authenticated with PICC master key = 1
bit 2: PICC master key authentication required for creating or deleting applications = 0; Authentication not required = 1
bit 1: PICC master key authentication required for listing of applications or reading key settings = 0; Free listing of applications and reading key settings = 1
bit 0: PICC master key frozen (reversible with configuration change or when formatting card) = 0; PICC master key changeable = 1
*/
#define DESFIRE_ALLOW_MASTER_KEY_CHANGE  (1 << 0)
#define DESFIRE_FREE_DIRECTORY_LIST      (1 << 1)
#define DESFIRE_FREE_CREATE_DELETE       (1 << 2)
#define DESFIRE_ALLOW_CONFIG_CHANGE      (1 << 3)
#define DESFIRE_USE_TARGET_KEY           0xE
#define DESFIRE_ALL_KEYS_FROZEN          0xF

/* Storage allocation constants */

#define DESFIRE_EEPROM_BLOCK_SIZE 32 /* Bytes */
#define DESFIRE_BYTES_TO_BLOCKS(x) \
    ( ((x) + DESFIRE_EEPROM_BLOCK_SIZE - 1) / DESFIRE_EEPROM_BLOCK_SIZE )

/* Source: http://www.proxmark.org/forum/viewtopic.php?id=2982 */
/* DESFire EV0 versions */
#define DESFIRE_HW_MAJOR_EV0     0x00
#define DESFIRE_HW_MINOR_EV0     0x01
#define DESFIRE_SW_MAJOR_EV0     0x00
#define DESFIRE_SW_MINOR_EV0     0x01

/* DESFire EV1 versions */
#define DESFIRE_HW_MAJOR_EV1     0x01
#define DESFIRE_HW_MINOR_EV1     0x01
#define DESFIRE_SW_MAJOR_EV1     0x01
#define DESFIRE_SW_MINOR_EV1     0x01

/* DESFire EV2 versions */
#define DESFIRE_HW_MAJOR_EV2     0x12
#define DESFIRE_HW_MINOR_EV2     0x01
#define DESFIRE_SW_MAJOR_EV2     0x12
#define DESFIRE_SW_MINOR_EV2     0x01

#define DESFIRE_STORAGE_SIZE_2K  0x16
#define DESFIRE_STORAGE_SIZE_4K  0x18
#define DESFIRE_STORAGE_SIZE_8K  0x1A

typedef enum {
    STATUS_OPERATION_OK = 0x00,
    STATUS_NO_CHANGES = 0x0C,
    STATUS_OUT_OF_EEPROM_ERROR = 0x0E,
    STATUS_ILLEGAL_COMMAND_CODE = 0x1C,
    STATUS_INTEGRITY_ERROR = 0x1E,
    STATUS_NO_SUCH_KEY = 0x40,
    STATUS_LENGTH_ERROR = 0x7E,
    STATUS_PERMISSION_DENIED = 0x9D,
    STATUS_PARAMETER_ERROR = 0x9E,
    STATUS_APP_NOT_FOUND = 0xA0,
    STATUS_APP_INTEGRITY_ERROR = 0xA1,
    STATUS_AUTHENTICATION_ERROR = 0xAE,
    STATUS_ADDITIONAL_FRAME = 0xAF,
    STATUS_BOUNDARY_ERROR = 0xBE,
    STATUS_COMMAND_ABORTED = 0xCA,
    STATUS_APP_COUNT_ERROR = 0xCE,
    STATUS_DUPLICATE_ERROR = 0xDE,
    STATUS_EEPROM_ERROR = 0xEE,
    STATUS_FILE_NOT_FOUND = 0xF0,
    STATUS_PICC_INTEGRITY_ERROR = 0xC1,
    STATUS_WRONG_VALUE_ERROR = 0x6E,
} DesfireStatusCodeType;

// TODO: typecast to uint8_t: 
typedef enum {
    // CommandToContinue: 
    NO_COMMAND_TO_CONTINUE = 0x00,
    // COMMAND CODES: 
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

/*
 * Definitions pertaining to on-card data
 */

typedef uint8_t DesfireAidType[DESFIRE_AID_SIZE];

#pragma pack (push)
#pragma pack (1)
typedef struct {
    uint8_t Type;
    uint8_t CommSettings;
    uint16_t AccessRights;
    union {
        struct {
            uint16_t FileSize;
        } StandardFile;
        struct {
            uint16_t FileSize;
            uint8_t BlockCount;
        } BackupFile;
        struct {
            int32_t CleanValue;
            int32_t DirtyValue;
            int32_t LowerLimit;
            int32_t UpperLimit;
            uint8_t LimitedCreditEnabled;
            int32_t PreviousDebit;
        } ValueFile;
        struct {
            uint8_t BlockCount;
            uint8_t ClearPending;
            uint8_t RecordSize[3];
            uint8_t CurrentNumRecords[3];
            uint8_t MaxRecordCount[3];
        } LinearRecordFile;
    } FileSettings;
} DESFireFileTypeSettings;
#pragma pack (pop)

// TODO: extern DESFireFileTypeSettings 

/** Defines the block ID of each application's file on the card. */
typedef uint8_t DesfireFileIndexType[DESFIRE_MAX_FILES];


#define DESFIRE_PICC_APP_SLOT 0
#define DESFIRE_MAX_SLOTS (DESFIRE_MAX_APPS + 1)

/** Data about applications is kept in these structures.
 * The application is represented as a collection of one-byte "properties":
 * key settings, key count, keystore block ID, file index block ID.
 * Since pre-EV2 cards have a fixed maximum amount of applications (1 PICC + 28 user),
 * using a fixed structure for this makes sense.
 */
typedef struct {
    uint8_t Spare0;
    uint8_t AppData[DESFIRE_MAX_SLOTS];
    uint16_t Checksum; /* Not actually used atm */
} DesfireApplicationDataType;

/* Mifare DESFire EV1 Application crypto operations */

#define APPLICATION_CRYPTO_DES    0x00
#define APPLICATION_CRYPTO_3K3DES 0x40
#define APPLICATION_CRYPTO_AES    0x80

/* Mifare DESFire Application settings
 * bit 7 - 4: Number of key needed to change application keys (key 0 - 13; 0 = master key; 14 = key itself required for key change; 15 = all keys are frozen)
 * bit 3: Application configuration frozen = 0; Application configuration changeable when authenticated with application master key = 1
 * bit 2: Application master key authentication required for create/delete files = 0; Authentication not required = 1
 * bit 1: GetFileIDs, GetFileSettings and GetKeySettings behavior: Master key authentication required = 0; No authentication required = 1
 * bit 0 = Application master key frozen = 0; Application master key changeable = 1
 */

struct mifare_desfire_df {
    uint32_t aid;
    uint16_t fid;
    uint8_t df_name[16];
    size_t df_name_len;
};


/** Defines the global PICC configuration.
 * This is located in the very first block on the card.
 */
#pragma pack (push)
#pragma pack (1)
typedef struct {
    /* Static data: does not change during the PICC's lifetime */
    uint8_t Uid[DESFIRE_UID_SIZE];
    uint8_t StorageSize;
    uint8_t HwVersionMajor;
    uint8_t HwVersionMinor;
    uint8_t SwVersionMajor;
    uint8_t SwVersionMinor;
    uint8_t BatchNumber[5];
    uint8_t ProductionWeek;
    uint8_t ProductionYear;
    /* Dynamic data: changes during the PICC's lifetime */
    uint8_t FirstFreeBlock;
    uint8_t TransactionStarted;
    uint8_t Spare[9];
} DesfirePiccInfoType;
#pragma pack (pop)

/** Defines the application directory contents.
 * The application directory maps AIDs to application slots:
 * the AID's index in `AppIds` is the slot number.
 */
typedef struct {
    uint8_t FirstFreeSlot;
    uint8_t Spare[8];
    DesfireAidType AppIds[DESFIRE_MAX_SLOTS]; /* 84 */
} DesfireAppDirType;


/* This resolves to 4 */
#define DESFIRE_APP_DIR_BLOCKS DESFIRE_BYTES_TO_BLOCKS(sizeof(DesfireAppDirType))
/* This resolves to 1 */
#define DESFIRE_FILE_INDEX_BLOCKS DESFIRE_BYTES_TO_BLOCKS(sizeof(DesfireFileIndexType))

/* The actual layout */
enum DesfireCardLayout {
    /* PICC related informaton is kept here */
    DESFIRE_PICC_INFO_BLOCK_ID = 0,
    /* Keeps the list of all applications created */
    DESFIRE_APP_DIR_BLOCK_ID,
    /* AppData keeping track of apps' key settings */
    DESFIRE_APP_KEY_SETTINGS_BLOCK_ID = DESFIRE_APP_DIR_BLOCK_ID + DESFIRE_APP_DIR_BLOCKS,
    /* AppData keeping track how many keys each app has */
    DESFIRE_APP_KEY_COUNT_BLOCK_ID,
    /* AppData keeping track of apps' key locations */
    DESFIRE_APP_KEYS_PTR_BLOCK_ID,
    /* AppData keeping track of apps' file index blocks */
    DESFIRE_APP_FILES_PTR_BLOCK_ID,
    /* Free space starts here */
    DESFIRE_FIRST_FREE_BLOCK_ID,
};

/*
 * DESFire backend API functions
 */

typedef struct {
    uint8_t BytesProcessed;
    bool IsComplete;
} TransferStatus;

/* Application management */
bool IsPiccAppSelected(void);
void SelectPiccApp(void);
uint8_t SelectApp(const DesfireAidType Aid);
uint8_t CreateApp(const DesfireAidType Aid, uint8_t KeyCount, uint8_t KeySettings);
uint8_t DeleteApp(const DesfireAidType Aid);
void GetApplicationIdsSetup(void);
TransferStatus GetApplicationIdsTransfer(uint8_t* Buffer);

/* Application key management */
uint8_t GetSelectedAppKeyCount(void);
uint8_t GetSelectedAppKeySettings(void);
void SetSelectedAppKeySettings(uint8_t KeySettings);
void ReadSelectedAppKey(uint8_t KeyId, uint8_t* Key);
void WriteSelectedAppKey(uint8_t KeyId, const uint8_t* Key);

/* File management */
uint8_t CreateStandardFile(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights, uint16_t FileSize);
uint8_t CreateBackupFile(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights, uint16_t FileSize);
uint8_t DeleteFile(uint8_t FileNum);
void CommitTransaction(void);
void AbortTransaction(void);
uint8_t SelectFile(uint8_t FileNum);
uint8_t GetSelectedFileType(void);
uint8_t GetSelectedFileCommSettings(void);
uint16_t GetSelectedFileAccessRights(void);
uint8_t ReadDataFileSetup(uint8_t CommSettings, uint16_t Offset, uint16_t Length);
TransferStatus ReadDataFileTransfer(uint8_t* Buffer);
uint8_t WriteDataFileSetup(uint8_t CommSettings, uint16_t Offset, uint16_t Length);
uint8_t WriteDataFileTransfer(uint8_t* Buffer, uint8_t ByteCount);
uint8_t ReadValueFileSetup(uint8_t CommSettings);
TransferStatus ReadValueFileTransfer(uint8_t* Buffer);

/* PICC management */
void InitialisePiccBackendEV0(uint8_t StorageSize);
void InitialisePiccBackendEV1(uint8_t StorageSize);
void ResetPiccBackend(void);
bool IsEmulatingEV1(void);
void GetPiccHardwareVersionInfo(uint8_t* Buffer);
void GetPiccSoftwareVersionInfo(uint8_t* Buffer);
void GetPiccManufactureInfo(uint8_t* Buffer);
uint8_t GetPiccKeySettings(void);
void GetPiccUid(ConfigurationUidType Uid);
void SetPiccUid(ConfigurationUidType Uid);
void FormatPicc(void);
void FactoryFormatPiccEV0(void);
void FactoryFormatPiccEV1(uint8_t StorageSize);

