/* 
 * DESFireFile.h : 
 * Maxie D. Schmidt (github.com/maxieds) 
 */

#ifndef __DESFIRE_EFFILE_H__
#define __DESFIRE_EFFILE_H__

#include "DESFireFirmwareSettings.h"
#include "DESFireInstructions.h"

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
#define DESFIRE_FILE_STANDARD_DATA      0
#define DESFIRE_FILE_BACKUP_DATA        1
#define DESFIRE_FILE_VALUE_DATA         2
#define DESFIRE_FILE_LINEAR_RECORDS     3
#define DESFIRE_FILE_CIRCULAR_RECORDS   4

/** Data about an application's file is currently kept in this structure.
 * The location of these structures is defined by the file index.
 */
typedef struct DESFIRE_FIRMWARE_PACKING {
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
    };
} DESFireFileTypeSettings;

/*
 * File management: creation, deletion, and misc routines
 */
uint8_t LookupActiveFileSlotByFileNumber(uint8_t fileNumber);
uint8_t GetAppFileIndexBlockId(uint8_t FileNum);
uint8_t AddFileToAppDataTypeStorage(uint8_t FileNum, uint8_t BlockId, uint8_t BlockCount);
uint8_t UpdateAppDirDataFileInfo(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights);

uint8_t GetFileControlBlockId(uint8_t FileNum);
uint8_t GetFileDataAreaBlockId(uint8_t FileNum);
uint8_t ReadFileControlBlock(uint8_t FileNum, DESFireFileTypeSettings *File);
uint8_t WriteFileControlBlock(uint8_t FileNum, DESFireFileTypeSettings *File);
uint8_t AllocateFileStorage(uint8_t FileNum, uint8_t BlockCount, uint8_t *BlockIdPtr);

uint8_t CreateStandardFile(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights, uint16_t FileSize);
uint8_t CreateBackupFile(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights, uint16_t FileSize);
uint8_t CreateValueFile(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights, 
                               int32_t LowerLimit, int32_t UpperLimit, int32_t Value, bool LimitedCreditEnabled);
uint8_t DeleteFile(uint8_t FileNum);

/* Transactions */
void StartTransaction(void);
void MarkFileDirty(uint8_t FileNum);
void StopTransaction(void);
void SyncronizeFileCopies(uint8_t FileNum, bool RollBack);
void FinaliseTransaction(bool RollBack);
void CommitTransaction(void);
void AbortTransaction(void);

/* File management: data transfer related routines */
uint8_t SelectFile(uint8_t FileNum);
uint8_t GetSelectedFileType(void);
uint8_t GetSelectedFileCommSettings(void);
uint16_t GetSelectedFileAccessRights(void);

/* File transfers */
TransferStatus ReadDataFileTransfer(uint8_t* Buffer);
uint8_t WriteDataFileTransfer(uint8_t* Buffer, uint8_t ByteCount);
uint8_t ReadDataFileSetup(uint8_t CommSettings, uint16_t Offset, uint16_t Length);
uint8_t WriteDataFileSetup(uint8_t CommSettings, uint16_t Offset, uint16_t Length);
uint16_t ReadDataFileIterator(uint8_t *Buffer, uint16_t ByteCount);
uint8_t WriteDataFileInternal(uint8_t *Buffer, uint16_t ByteCount);
uint16_t WriteDataFileIterator(uint8_t *Buffer, uint16_t ByteCount);
TransferStatus ReadValueFileTransfer(uint8_t* Buffer);
uint8_t ReadValueFileSetup(uint8_t CommSettings);

/* Validation routines */
#define VALIDATE_ACCESS_READWRITE          (0x000f << 0)
#define VALIDATE_ACCESS_WRITE              (0x000f << 4)
#define VALIDATE_ACCESS_READ               (0x000f << 8)
#define VALIDATED_ACCESS_DENIED            0
#define VALIDATED_ACCESS_GRANTED           1
#define VALIDATED_ACCESS_GRANTED_PLAINTEXT 2

/* 
 * [READ (4 bits) | WRITE | READ-WRITE | CHANGE] 
 * Stored in little endian format from memory:
 */
#define GetReadPermissions(AccessRights) \
	(BYTE) (AccessRights & 0x000f)
#define GetWritePermissions(AccessRights) \
        (BYTE) (((0x00f0 & AccessRights) >> 4) & 0x000f)
#define GetReadWritePermissions(AccessRights) \
        (BYTE) (((0x0f00 & AccessRights) >> 8) & 0x000f)
#define GetChangePermissions(AccessRights) \
	(BYTE) (((0xf000 & AccessRights) >> 12) & 0x000f)

/*
 * There are also command/instruction-wise
 * citations given from file's access permissions. This data is taken
 * from the table on page 21 of the NXP application note:
 * https://www.nxp.com/docs/en/application-note/AN12343.pdf
 */
uint8_t CreateFileCommonValidation(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights);
uint8_t ValidateAuthentication(uint16_t AccessRights, uint8_t CheckMask);

// TODO: Page 57: Read file functions ... 
// TODO: Create and write file functions ... 

#endif
