/* 
 * DESFireFile.h : 
 * Maxie D. Schmidt (github.com/maxieds) 
 */

#ifndef __DESFIRE_EFFILE_H__
#define __DESFIRE_EFFILE_H__

#define EFFILE_DATA_CHUNK_SIZE            (4)
#define EFFILE_DATA_CHUNK_ALIGNAT         __attribute__((align(EFFILE_DATA_CHUNK_SIZE)))

#define DESFIRE_MAX_FILES_EV0             16
#define DESFIRE_MAX_FILES_EV1             32
#define DESFIRE_MAX_FILES                 DESFIRE_MAX_FILES_EV1

// TODO: See type 4 tags on CC and NTAG AIDs and FIDs 

#define ISO7816_4_CURRENT_EF_FILE_ID      0x0000
#define ISO7816_4_CURRENT_DF_FILE_ID      0x3FFF
#define ISO7816_4_MASTER_FILE_ID          0x3F00

/* File types */

// See pages 48-49 of the datasheet for more details ... 
typedef enum {
     FTYPE_STD_DATA       = 0, 
     FTYPE_BACKUP_DATA    = 1, 
     FTYPE_VALUE          = 2,
     FTYPE_LINEAR_RECORD  = 3, 
     FTYPE_CYCLIC_RECORD  = 4,
} DESFireFileType;

#define DESFIRE_FILE_STANDARD_DATA      0
#define DESFIRE_FILE_BACKUP_DATA        1
#define DESFIRE_FILE_VALUE_DATA         2
#define DESFIRE_FILE_LINEAR_RECORDS     3
#define DESFIRE_FILE_CIRCULAR_RECORDS   4

/** Data about an application's file is currently kept in this structure.
 * The location of these structures is defined by the file index.
 */
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

// TODO: Page 57: Read file functions ... 
// TODO: Create and write file functions ... 

#endif
