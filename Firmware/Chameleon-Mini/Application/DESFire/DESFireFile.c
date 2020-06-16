/* 
 * DESFireFile.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "DESFireFile.h"
#include "DESFirePICCControl.h"
#include "DESFireStatusCodes.h"
#include "DESFireMemoryOperations.h"
#include "DESFireInstructions.h"
#include "DESFireApplicationDirectory.h"
#include "../MifareDESFire.h"

uint16_t GetFileSize(DESFireFileTypeSettings *File) {
     if(File == NULL) {
          return 0x0000;
     }
     switch(File->FileType) {
          case DESFIRE_FILE_STANDARD_DATA:
               return File->StandardFile.FileSize;
          case DESFIRE_FILE_BACKUP_DATA:
               return File->BackupFile.FileSize;
          case DESFIRE_FILE_VALUE_DATA:
               return sizeof(int32_t); // 4
          case DESFIRE_FILE_LINEAR_RECORDS:
               return (File->LinearRecordFile.BlockCount) * DESFIRE_EEPROM_BLOCK_SIZE;
          case DESFIRE_FILE_CIRCULAR_RECORDS:
          default:
               break;
     }
     return 0x0000;
}

/*
 * File management: creation, deletion, and misc routines
 */

uint16_t GetFileDataAreaBlockId(uint8_t FileNum) {
     uint8_t FileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
     if(FileIndex >= DESFIRE_MAX_FILES) {
          return 0x0000;
     }
     DESFireFileTypeSettings fileSettings = ReadFileSettings(SelectedApp.Slot, FileIndex);
     return fileSettings.FileDataBlockId;
}

void WriteFileDataAreaBlockId(uint8_t FileNum, uint16_t DataBlockId) {
     uint8_t FileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
     if(FileIndex >= DESFIRE_MAX_FILES) {
          return;
     }
     DESFireFileTypeSettings fileSettings = ReadFileSettings(SelectedApp.Slot, FileIndex);
     fileSettings.FileDataBlockId = DataBlockId;
     WriteFileSettings(SelectedApp.Slot, FileIndex, &fileSettings);
}

uint8_t ReadFileControlBlock(uint8_t FileNum, DESFireFileTypeSettings *File) {
     if(File == NULL) {
          return STATUS_PARAMETER_ERROR;
     }
     uint8_t FileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
     if(FileIndex >= DESFIRE_MAX_FILES) {
          return STATUS_FILE_NOT_FOUND;
     }
     DESFireFileTypeSettings fileSettings = ReadFileSettings(SelectedApp.Slot, FileIndex);
     memcpy(File, &fileSettings, sizeof(DESFireFileTypeSettings));
     return STATUS_OPERATION_OK;
}

uint8_t WriteFileControlBlock(uint8_t FileNum, DESFireFileTypeSettings *File) {
     if(File == NULL) {
          return STATUS_PARAMETER_ERROR;
     }
     uint8_t FileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
     if(FileIndex >= DESFIRE_MAX_FILES) {
          return STATUS_FILE_NOT_FOUND;
     }
     WriteFileSettings(SelectedApp.Slot, FileIndex, File);
     return STATUS_OPERATION_OK;
}

uint16_t AllocateFileStorage(uint8_t FileNum, uint8_t BlockCount) {
     if(BlockCount == 0) {
          return STATUS_PARAMETER_ERROR;
     }
     uint8_t FileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum); 
     if(FileIndex >= DESFIRE_MAX_FILES) {
          return STATUS_FILE_NOT_FOUND;
     }
     DESFireFileTypeSettings fileSettings = ReadFileSettings(SelectedApp.Slot, FileIndex);
     if(fileSettings.FileDataBlockId != 0) {
          return STATUS_DUPLICATE_ERROR; // already allocated file storage space for this file 
     }
     fileSettings.FileDataBlockId = AllocateBlocks(BlockCount);
     if(fileSettings.FileDataBlockId == 0) {
          return STATUS_OUT_OF_EEPROM_ERROR;
     }
     WriteFileSettings(SelectedApp.Slot, FileIndex, &fileSettings);
     return STATUS_OPERATION_OK;
}

uint8_t CreateFileHeaderData(uint8_t FileNum, DESFireFileTypeSettings *File) {
     if(File == NULL) {
          return STATUS_PARAMETER_ERROR;
     }
     else if(SelectedApp.FileCount >= DESFIRE_MAX_FILES) {
          return STATUS_APP_COUNT_ERROR;
     }
     uint8_t fileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
     if(fileIndex < DESFIRE_MAX_FILES) {
          return STATUS_DUPLICATE_ERROR;
     }
     fileIndex = SelectedApp.FileCount;
     SIZET fileSettingsBlockId = AllocateBlocks(DESFIRE_BYTES_TO_BLOCKS(sizeof(DESFireFileTypeSettings)));
     if(fileSettingsBlockId == 0) {
          return STATUS_OUT_OF_EEPROM_ERROR;
     }
     else {
          SetBlockBytes(fileSettingsBlockId, 0x00, sizeof(DESFireFileTypeSettings));
          SIZET fileAddressArrayBlockId = GetAppProperty(DESFIRE_APP_FILES_PTR_BLOCK_ID, SelectedApp.Slot);
          SIZET fileAddressArray[DESFIRE_MAX_FILES];
          ReadBlockBytes(fileAddressArray, fileAddressArrayBlockId, 2 * DESFIRE_MAX_FILES);
          fileAddressArray[fileIndex] = fileSettingsBlockId;
          WriteBlockBytes(fileAddressArray, fileAddressArrayBlockId, 2 * DESFIRE_MAX_FILES);
     }
     WriteFileNumberAtIndex(SelectedApp.Slot, fileIndex, FileNum);
     BYTE nextFileCount = ++(SelectedApp.FileCount);
     WriteFileCount(SelectedApp.Slot, nextFileCount);
     return STATUS_OPERATION_OK;
}

uint8_t CreateStandardFile(uint8_t FileNum, uint8_t CommSettings, 
                           uint16_t AccessRights, uint16_t FileSize) {
     uint8_t Status;
     memset(&SelectedFile, PICC_EMPTY_BYTE, sizeof(SelectedFile));
     SelectedFile.Num = FileNum;
     SelectedFile.File.Type = DESFIRE_FILE_STANDARD_DATA;
     SelectedFile.File.CommSettings = CommSettings;
     SelectedFile.File.AccessRights = AccessRights;
     SelectedFile.File.StandardFile.FileSize = FileSize;
     Status = CreateFileHeaderData(FileNum, &(SelectedFile.File));
     if(Status != STATUS_OPERATION_OK) {
          return Status;
     }
     Status = AllocateFileStorage(FileNum, DESFIRE_BYTES_TO_BLOCKS(FileSize));
     if(Status != STATUS_OPERATION_OK) {
          return Status;
     }
     uint8_t fileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
     if(fileIndex >= DESFIRE_MAX_FILES) {
          return STATUS_FILE_NOT_FOUND;
     }
     WriteFileCommSettings(SelectedApp.Slot, fileIndex, CommSettings);
     WriteFileAccessRights(SelectedApp.Slot, fileIndex, AccessRights);
     return STATUS_OPERATION_OK;
}

uint8_t CreateBackupFile(uint8_t FileNum, uint8_t CommSettings, 
                         uint16_t AccessRights, uint16_t FileSize) {
     uint8_t Status;
     uint8_t BlockCount = DESFIRE_BYTES_TO_BLOCKS(FileSize);
     memset(&SelectedFile, PICC_EMPTY_BYTE, sizeof(SelectedFileCacheType));
     SelectedFile.Num = FileNum;
     SelectedFile.File.Type = DESFIRE_FILE_BACKUP_DATA;
     SelectedFile.File.CommSettings = CommSettings;
     SelectedFile.File.AccessRights = AccessRights;
     SelectedFile.File.BackupFile.FileSize = FileSize;
     SelectedFile.File.BackupFile.BlockCount = BlockCount;
     Status = CreateFileHeaderData(FileNum, &(SelectedFile.File));
     if(Status != STATUS_OPERATION_OK) {
          return Status;
     }
     Status = AllocateFileStorage(FileNum, BlockCount);
     if(Status != STATUS_OPERATION_OK) {
          return Status;
     }
     uint8_t fileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
     if(fileIndex >= DESFIRE_MAX_FILES) {
          return STATUS_FILE_NOT_FOUND;
     }
     WriteFileCommSettings(SelectedApp.Slot, fileIndex, CommSettings);
     WriteFileAccessRights(SelectedApp.Slot, fileIndex, AccessRights);
     return STATUS_OPERATION_OK;    
}

uint8_t CreateValueFile(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights,
                        int32_t LowerLimit, int32_t UpperLimit, int32_t Value, 
                        bool LimitedCreditEnabled) {
     uint8_t Status;
     uint8_t BlockCount = DESFIRE_BYTES_TO_BLOCKS(4);
     memset(&SelectedFile, PICC_EMPTY_BYTE, sizeof(SelectedFileCacheType));
     SelectedFile.Num = FileNum;
     SelectedFile.File.Type = DESFIRE_FILE_VALUE_DATA;
     SelectedFile.File.CommSettings = CommSettings;
     SelectedFile.File.AccessRights = AccessRights;
     SelectedFile.File.ValueFile.LowerLimit = LowerLimit;
     SelectedFile.File.ValueFile.UpperLimit = UpperLimit;
     SelectedFile.File.ValueFile.CleanValue = Value;
     SelectedFile.File.ValueFile.DirtyValue = Value;
     SelectedFile.File.ValueFile.LimitedCreditEnabled = LimitedCreditEnabled;
     SelectedFile.File.ValueFile.PreviousDebit = 0;
     Status = CreateFileHeaderData(FileNum, &(SelectedFile.File));
     if(Status != STATUS_OPERATION_OK) {
          return Status;
     }
     Status = AllocateFileStorage(FileNum, BlockCount);
     if(Status != STATUS_OPERATION_OK) {
          return Status;
     }
     uint8_t fileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
     if(fileIndex >= DESFIRE_MAX_FILES) {
          return STATUS_FILE_NOT_FOUND;
     }
     WriteFileCommSettings(SelectedApp.Slot, fileIndex, CommSettings);
     WriteFileAccessRights(SelectedApp.Slot, fileIndex, AccessRights);
     return STATUS_OPERATION_OK;
}

uint8_t DeleteFile(uint8_t FileNum) {
     uint8_t fileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
     if(fileIndex >= DESFIRE_MAX_FILES) {
          return STATUS_FILE_NOT_FOUND;
     }
     else if(SelectedApp.FileCount == 0x00) {
          return STATUS_APP_COUNT_ERROR;
     }
     /* Need change permissions to delete the file */
     BYTE fileChangePermissions = GetChangePermissions(ReadFileAccessRights(SelectedApp.Slot, fileIndex));
     if(fileChangePermissions == DESFIRE_ACCESS_DENY || 
       ((fileChangePermissions != DESFIRE_ACCESS_FREE) && (fileChangePermissions != AuthenticatedWithKey))) {
         return STATUS_PERMISSION_DENIED;
     }
     WriteFileNumberAtIndex(SelectedApp.Slot, fileIndex, 0x00);
     WriteFileCommSettings(SelectedApp.Slot, fileIndex, 0x00);
     WriteFileAccessRights(SelectedApp.Slot, fileIndex, 0xffff);
     DESFireFileTypeSettings fileSettings;
     memset(&fileSettings, 0x00, sizeof(DESFireFileTypeSettings));
     WriteFileSettings(SelectedApp.Slot, fileIndex, &fileSettings);
     SIZET fileAddressArray[DESFIRE_MAX_FILES];
     ReadBlockBytes(fileAddressArray, SelectedApp.FilesAddress, 2 * DESFIRE_MAX_FILES);
     fileAddressArray[fileIndex] = 0x0000;
     WriteBlockBytes(fileAddressArray, SelectedApp.FilesAddress, 2 * DESFIRE_MAX_FILES);
     WriteFileCount(SelectedApp.Slot, --(SelectedApp.FileCount));
     return STATUS_OPERATION_OK;
}

/*
 * File management: transaction related routines
 */

void StartTransaction(void) {
    Picc.TransactionStarted = SelectedApp.Slot;
    SynchronizePICCInfo();
    SelectedApp.DirtyFlags = 0;
}

void MarkFileDirty(uint8_t FileNum) {
    SelectedApp.DirtyFlags |= 1 << FileNum;
}

void StopTransaction(void) {
    Picc.TransactionStarted = 0;
    SynchronizePICCInfo();
}

void SynchronizeFileCopies(uint8_t FileNum, bool Rollback) {
    DESFireFileTypeSettings File;
    uint8_t DataAreaBlockId;

    ReadFileControlBlock(FileNum, &File);
    DataAreaBlockId = GetFileDataAreaBlockId(FileNum);

    switch (File.Type) {
    case DESFIRE_FILE_VALUE_DATA:
        if (Rollback) {
            File.ValueFile.DirtyValue = File.ValueFile.CleanValue;
        }
        else {
            File.ValueFile.CleanValue = File.ValueFile.DirtyValue;
        }
        WriteFileControlBlock(FileNum, &File);
        break;

    case DESFIRE_FILE_BACKUP_DATA:
        if (Rollback) {
            CopyBlockBytes(DataAreaBlockId, DataAreaBlockId + File.BackupFile.BlockCount, File.BackupFile.BlockCount);
        }
        else {
            CopyBlockBytes(DataAreaBlockId + File.BackupFile.BlockCount, DataAreaBlockId, File.BackupFile.BlockCount);
        }
        break;
    }
}

void FinaliseTransaction(bool Rollback) {
    uint8_t FileNum;
    uint16_t DirtyFlags = SelectedApp.DirtyFlags;
    if (!Picc.TransactionStarted) 
        return;
    for (FileNum = 0; FileNum < DESFIRE_MAX_FILES; ++FileNum) {
        if (DirtyFlags & (1 << FileNum)) {
            SynchronizeFileCopies(FileNum, Rollback);
        }
    }
    StopTransaction();
}

void CommitTransaction(void) {
    FinaliseTransaction(false);
}

void AbortTransaction(void) {
    FinaliseTransaction(true);
}

/*
 * File management: data transfer related routines
 */

uint8_t SelectFile(uint8_t FileNum) {
    SelectedFile.Num = FileNum;
    return ReadFileControlBlock(FileNum, &SelectedFile.File);
}

uint8_t GetSelectedFileType(void) {
    return SelectedFile.File.Type;
}

uint8_t GetSelectedFileCommSettings(void) {
    return SelectedFile.File.CommSettings;
}

uint16_t GetSelectedFileAccessRights(void) {
    return SelectedFile.File.AccessRights;
}

/* Exposed transfer API: standard/backup data files */

TransferStatus ReadDataFileTransfer(uint8_t* Buffer) {
    return PiccToPcdTransfer(Buffer);
}

uint8_t WriteDataFileTransfer(uint8_t* Buffer, uint8_t ByteCount) {
    return PcdToPiccTransfer(Buffer, ByteCount);
}

uint8_t ReadDataFileSetup(uint8_t CommSettings, uint16_t Offset, uint16_t Length) {
    uint16_t fileSize = GetFileSize(&(SelectedFile.File));
    /* Verify boundary conditions */
    if (Offset + Length > fileSize) {
        return STATUS_BOUNDARY_ERROR;
    }
    /* Setup data source */
    TransferState.ReadData.Source.Func = &ReadDataEEPROMSource;
    if (!Length) {
        TransferState.ReadData.Encryption.FirstPaddingBitSet = true;
        TransferState.ReadData.BytesLeft = fileSize - Offset;
    }
    else {
        TransferState.ReadData.Encryption.FirstPaddingBitSet = false;
        TransferState.ReadData.BytesLeft = Length;
    }
    /* Clean data is always located in the beginning of data area */
    TransferState.ReadData.Source.Pointer = GetFileDataAreaBlockId(SelectedFile.Num) * 
                                            DESFIRE_EEPROM_BLOCK_SIZE + Offset;
    /* Setup data filter */
    return ReadDataFilterSetup(CommSettings);
}

uint8_t WriteDataFileSetup(uint8_t CommSettings, uint16_t Offset, uint16_t Length) {
    /* Verify boundary conditions */
    if (Offset + Length > GetFileSize(&(SelectedFile.File))) {
        return STATUS_BOUNDARY_ERROR;
    }
    /* Setup data sink */
    TransferState.WriteData.BytesLeft = Length;
    TransferState.WriteData.Sink.Func = &WriteDataEEPROMSink;
    /* TODO: Dirty data location depends on the file type: correct the offset as needed? */
    if (GetSelectedFileType() == DESFIRE_FILE_BACKUP_DATA) {
        Offset += SelectedFile.File.BackupFile.BlockCount * DESFIRE_EEPROM_BLOCK_SIZE;
    }
    TransferState.WriteData.Sink.Pointer = GetFileDataAreaBlockId(SelectedFile.Num) * 
                                           DESFIRE_EEPROM_BLOCK_SIZE + Offset;
    /* Setup data filter */
    return WriteDataFilterSetup(CommSettings);
}

uint16_t ReadDataFileIterator(uint8_t* Buffer, uint16_t ByteCount) {
    /* NOTE: incoming ByteCount is not verified here for now */
    TransferStatus Status;
    Status = ReadDataFileTransfer(&Buffer[1]);
    if (Status.IsComplete) {
        Buffer[0] = STATUS_OPERATION_OK;
        DesfireState = DESFIRE_IDLE;
    }
    else {
        Buffer[0] = STATUS_ADDITIONAL_FRAME;
        DesfireState = DESFIRE_READ_DATA_FILE;
    }
    return DESFIRE_STATUS_RESPONSE_SIZE + Status.BytesProcessed;
}

uint8_t WriteDataFileInternal(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t Status;
    Status = WriteDataFileTransfer(Buffer, ByteCount);
    switch(Status) {
    default:
        /* In case anything goes wrong, abort things */
        AbortTransaction();
        /* Fall through */
    case STATUS_OPERATION_OK:
        DesfireState = DESFIRE_IDLE;
        break;
    case STATUS_ADDITIONAL_FRAME:
        DesfireState = DESFIRE_WRITE_DATA_FILE;
        break;
    }
    return Status;
}

uint16_t WriteDataFileIterator(uint8_t* Buffer, uint16_t ByteCount) {
    Buffer[0] = WriteDataFileInternal(&Buffer[1], ByteCount - 1);
    return DESFIRE_STATUS_RESPONSE_SIZE;
}

uint8_t CreateFileCommonValidation(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights) {
    /* Validate file number */
    uint8_t fileIndex = LookupFileNumberIndex(SelectedApp.Slot, FileNum);
    if(fileIndex >= DESFIRE_MAX_FILES) {
        return STATUS_PARAMETER_ERROR;
    }
    /* Validate basic access permissions */
    uint8_t accessPermissions = ValidateAuthentication(AccessRights, 
                                                       VALIDATE_ACCESS_READWRITE | VALIDATE_ACCESS_WRITE);
    if(accessPermissions == VALIDATED_ACCESS_DENIED) {
        return STATUS_PERMISSION_DENIED;
    }
    return STATUS_OPERATION_OK;
}

uint8_t ValidateAuthentication(uint16_t AccessRights, uint16_t CheckMask) {
    uint8_t SplitPerms[] = {
         GetReadPermissions(CheckMask & AccessRights), 
         GetWritePermissions(CheckMask & AccessRights), 
         GetReadWritePermissions(CheckMask & AccessRights), 
         GetChangePermissions(CheckMask & AccessRights)
    };
    for(int bpos = 0; bpos < 4; bpos++) {
         if (SplitPerms[bpos] == AuthenticatedWithKey) {
              return VALIDATED_ACCESS_GRANTED;
         }
         else if (SplitPerms[bpos] == DESFIRE_ACCESS_FREE) {
              return VALIDATED_ACCESS_GRANTED_PLAINTEXT;
         }
    } 
    return VALIDATED_ACCESS_DENIED;
}

