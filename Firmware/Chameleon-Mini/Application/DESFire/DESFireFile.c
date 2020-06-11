/* 
 * DESFireFile.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "DESFireFile.h"
#include "DESFirePICCControl.h"
#include "DESFireStatusCodes.h"
#include "DESFireMemoryOperations.h"

/*
 * File management: creation, deletion, and misc routines
 */

uint8_t GetFileControlBlockId(uint8_t FileNum) {
    uint8_t FileIndexBlock;
    DesfireFileIndexType FileIndex;

    /* Read in the file index */
    FileIndexBlock = GetAppFileIndexBlockId(SelectedApp.Slot);
    ReadBlockBytes(&FileIndex, FileIndexBlock, sizeof(FileIndex));
    return FileIndex[FileNum];
}

uint8_t GetFileDataAreaBlockId(uint8_t FileNum) {
    return GetFileControlBlockId(FileNum) + 1;
}

uint8_t ReadFileControlBlock(uint8_t FileNum, DESFireFileTypeSettings* File) {
    uint8_t BlockId;

    /* Check whether the file exists */
    BlockId = GetFileControlBlockId(FileNum);
    if (!BlockId) {
        return STATUS_FILE_NOT_FOUND;
    }
    /* Read the file control block */
    ReadBlockBytes(File, BlockId, sizeof(File));
    return STATUS_OPERATION_OK;
}

uint8_t WriteFileControlBlock(uint8_t FileNum, DESFireFileTypeSettings* File) {
    uint8_t BlockId;

    /* Check whether the file exists */
    BlockId = GetFileControlBlockId(FileNum);
    if (!BlockId) {
        return STATUS_FILE_NOT_FOUND;
    }
    /* Write the file control block */
    WriteBlockBytes(File, BlockId, sizeof(File));
    return STATUS_OPERATION_OK;
}

uint8_t AllocateFileStorage(uint8_t FileNum, uint8_t BlockCount, uint8_t* BlockIdPtr) {
    uint8_t FileIndexBlock;
    uint8_t FileIndex[DESFIRE_MAX_FILES];
    uint8_t BlockId;

    /* Read in the file index */
    FileIndexBlock = GetAppFileIndexBlockId(SelectedApp.Slot);
    ReadBlockBytes(&FileIndex, FileIndexBlock, sizeof(FileIndex));
    /* Check if the file already exists */
    if (FileIndex[FileNum]) {
        return STATUS_DUPLICATE_ERROR;
    }
    /* Allocate blocks for the file */
    BlockId = AllocateBlocks(1 + BlockCount);
    if (BlockId) {
        /* Write the file index */
        FileIndex[FileNum] = BlockId;
        WriteBlockBytes(&FileIndex, FileIndexBlock, sizeof(FileIndex));
        /* Write the output value */
        *BlockIdPtr = BlockId;
        return STATUS_OPERATION_OK;
    }
    return STATUS_OUT_OF_EEPROM_ERROR;
}

uint8_t CreateStandardFile(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights, uint16_t FileSize) {
    uint8_t Status;
    uint8_t BlockId;

    /* Grab storage */
    Status = AllocateFileStorage(FileNum, 1 + DESFIRE_BYTES_TO_BLOCKS(FileSize), &BlockId);
    if (Status == STATUS_OPERATION_OK) {
        /* Fill in the control structure and write it */
        memset(&SelectedFile, 0, sizeof(SelectedFile));
        SelectedFile.File.Type = DESFIRE_FILE_STANDARD_DATA;
        SelectedFile.File.CommSettings = CommSettings;
        SelectedFile.File.AccessRights = AccessRights;
        SelectedFile.File.StandardFile.FileSize = FileSize;
        WriteBlockBytes(&SelectedFile.File, BlockId, sizeof(SelectedFile.File));
    }
    /* Done */
    return Status;
}

uint8_t CreateBackupFile(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights, uint16_t FileSize) {
    uint8_t Status;
    uint8_t BlockId;
    uint8_t BlockCount;

    /* Grab storage */
    BlockCount = DESFIRE_BYTES_TO_BLOCKS(FileSize);
    Status = AllocateFileStorage(FileNum, 1 + 2 * BlockCount, &BlockId);
    if (Status == STATUS_OPERATION_OK) {
        /* Fill in the control structure and write it */
        memset(&SelectedFile, 0, sizeof(SelectedFile));
        SelectedFile.File.Type = DESFIRE_FILE_BACKUP_DATA;
        SelectedFile.File.CommSettings = CommSettings;
        SelectedFile.File.AccessRights = AccessRights;
        SelectedFile.File.BackupFile.FileSize = FileSize;
        SelectedFile.File.BackupFile.BlockCount = BlockCount;
        WriteBlockBytes(&SelectedFile.File, BlockId, sizeof(SelectedFile.File));
    }
    /* Done */
    return Status;
}

uint8_t CreateValueFile(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights,
                        int32_t LowerLimit, int32_t UpperLimit, int32_t Value, bool LimitedCreditEnabled) {
    uint8_t Status;
    uint8_t BlockId;

    /* Grab storage */
    Status = AllocateFileStorage(FileNum, 1, &BlockId);
    if (Status == STATUS_OPERATION_OK) {
        /* Fill in the control structure and write it */
        memset(&SelectedFile, 0, sizeof(SelectedFile));
        SelectedFile.File.Type = DESFIRE_FILE_VALUE_DATA;
        SelectedFile.File.CommSettings = CommSettings;
        SelectedFile.File.AccessRights = AccessRights;
        SelectedFile.File.ValueFile.LowerLimit = LowerLimit;
        SelectedFile.File.ValueFile.UpperLimit = UpperLimit;
        SelectedFile.File.ValueFile.CleanValue = Value;
        SelectedFile.File.ValueFile.DirtyValue = Value;
        SelectedFile.File.ValueFile.LimitedCreditEnabled = LimitedCreditEnabled;
        SelectedFile.File.ValueFile.PreviousDebit = 0;
        WriteBlockBytes(&SelectedFile.File, BlockId, sizeof(SelectedFile.File));
    }
    /* Done */
    return Status;
}

// TODO: Need to check access permissions? 
uint8_t DeleteFile(uint8_t FileNum) {
    uint8_t FileIndexBlock;
    uint8_t FileIndex[DESFIRE_MAX_FILES];

    FileIndexBlock = GetAppFileIndexBlockId(SelectedApp.Slot);
    ReadBlockBytes(&FileIndex, FileIndexBlock, sizeof(FileIndex));
    if (FileIndex[FileNum]) {
        FileIndex[FileNum] = 0;
    }
    else {
        return STATUS_FILE_NOT_FOUND;
    }
    WriteBlockBytes(&FileIndex, FileIndexBlock, sizeof(FileIndex));
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

void SynchFileCopies(uint8_t FileNum, bool Rollback) {
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
    /* TODO: implement other file types */
}

void FinaliseTransaction(bool Rollback) {
    uint8_t FileNum;
    uint16_t DirtyFlags = SelectedApp.DirtyFlags;

    if (!Picc.TransactionStarted) 
        return;
    for (FileNum = 0; FileNum < DESFIRE_MAX_FILES; ++FileNum) {
        if (DirtyFlags & (1 << FileNum)) {
            SynchFileCopies(FileNum, Rollback);
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
    /* Verify boundary conditions */
    if (Offset + Length > SelectedFile.File.StandardFile.FileSize) {
        return STATUS_BOUNDARY_ERROR;
    }

    /* Setup data source */
    TransferState.ReadData.Source.Func = &ReadDataEEPROMSource;
    if (!Length) {
        TransferState.ReadData.Encryption.FirstPaddingBitSet = true;
        TransferState.ReadData.BytesLeft = SelectedFile.File.StandardFile.FileSize - Offset;
    }
    else {
        TransferState.ReadData.Encryption.FirstPaddingBitSet = false;
        TransferState.ReadData.BytesLeft = Length;
    }
    /* Clean data is always located in the beginning of data area */
    TransferState.ReadData.Source.Pointer = GetFileDataAreaBlockId(SelectedFile.Num) * DESFIRE_EEPROM_BLOCK_SIZE + Offset;
    /* Setup data filter */
    return ReadDataFilterSetup(CommSettings);
}

uint8_t WriteDataFileSetup(uint8_t CommSettings, uint16_t Offset, uint16_t Length) {
    /* Verify boundary conditions */
    if (Offset + Length > SelectedFile.File.StandardFile.FileSize) {
        return STATUS_BOUNDARY_ERROR;
    }

    /* Setup data sink */
    TransferState.WriteData.BytesLeft = Length;
    TransferState.WriteData.Sink.Func = &WriteDataEEPROMSink;
    /* Dirty data location depends on the file type; correct the offset as needed */
    if (GetSelectedFileType() == DESFIRE_FILE_BACKUP_DATA) {
        Offset += SelectedFile.File.BackupFile.BlockCount * DESFIRE_EEPROM_BLOCK_SIZE;
    }
    TransferState.WriteData.Sink.Pointer = GetFileDataAreaBlockId(SelectedFile.Num) * DESFIRE_EEPROM_BLOCK_SIZE + Offset;
    /* Setup data filter */
    return WriteDataFilterSetup(CommSettings);
}

uint16_t ReadDataFileIterator(uint8_t* Buffer, uint16_t ByteCount) {
    TransferStatus Status;
    /* TODO: NOTE: incoming ByteCount is not verified here for now */

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

    switch (Status) {
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

/* Exposed transfer API: value files */

TransferStatus ReadValueFileTransfer(uint8_t* Buffer) {
    return PiccToPcdTransfer(Buffer);
}

uint8_t ReadValueFileSetup(uint8_t CommSettings) {
    /* Setup data source (generic EEPROM source) */
    TransferState.ReadData.Source.Func = &ReadDataEEPROMSource;
    TransferState.ReadData.Source.Pointer = GetFileDataAreaBlockId(SelectedFile.Num) * DESFIRE_EEPROM_BLOCK_SIZE;
    TransferState.ReadData.BytesLeft = 4;
    TransferState.ReadData.Encryption.FirstPaddingBitSet = false;
    /* Setup data filter */
    return ReadDataFilterSetup(CommSettings);
}

uint8_t CreateFileCommonValidation(uint8_t FileNum, uint8_t CommSettings, uint16_t AccessRights) {
    /* Validate file number */
    if (FileNum >= DESFIRE_MAX_FILES) {
        return STATUS_PARAMETER_ERROR;
    }
    /* TODO: Validate communication settings */
    /* TODO: Validate the access rights */
    /* TODO: Make sure the file number actually corresponds to an active file */
    return STATUS_OPERATION_OK;
}

uint8_t ValidateAuthentication(uint16_t AccessRights, uint8_t CheckMask) {
    uint8_t RequiredKeyId;
    bool HaveFreeAccess = false;

    AccessRights >>= 4;
    while (CheckMask) {
        if (CheckMask & 1) {
            RequiredKeyId = AccessRights & 0x0F;
            if (AuthenticatedWithKey == RequiredKeyId) {
                return VALIDATED_ACCESS_GRANTED;
            }
            if (RequiredKeyId == DESFIRE_ACCESS_FREE) {
                HaveFreeAccess = true;
            }
        }
        CheckMask >>= 1;
        AccessRights >>= 4;
    }
    return HaveFreeAccess ? VALIDATED_ACCESS_GRANTED_PLAINTEXT : VALIDATED_ACCESS_DENIED;
}


