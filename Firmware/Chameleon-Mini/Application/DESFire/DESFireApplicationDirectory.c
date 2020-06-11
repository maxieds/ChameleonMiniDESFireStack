/* 
 * DESFireApplicationDirectory.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "DESFireApplicationDirectory.h"
#include "DESFirePICCControl.h"
#include "DESFireCrypto.h"
#include "DESFireStatusCodes.h"
#include "DESFireInstructions.h"
#include "DESFireMemoryOperations.h"

const BYTE DEFAULT_DESFIRE_AID[] = { 
     0xd2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x00 
};
const BYTE DEFAULT_ISO7816_AID[] = { 
     0xa0, 0x00, 0x00, 0x00, 0x03, 0x96 
};

/*
 * Global card structure support routines
 */

void SynchronizeAppDir(void) {
    WriteBlockBytes(&AppDir, DESFIRE_APP_DIR_BLOCK_ID, sizeof(DESFireAppDirType));
}

void SynchronizePICCInfo(void) {
    WriteBlockBytes(&Picc, DESFIRE_PICC_INFO_BLOCK_ID, sizeof(DESFirePICCInfoType));
}

uint8_t GetAppProperty(uint8_t BlockId, uint8_t AppSlot) {
    DesfireApplicationDataType Data;
    ReadBlockBytes(&Data, BlockId, sizeof(Data));
    return Data.AppData[AppSlot];
}

void SetAppProperty(uint8_t BlockId, uint8_t AppSlot, uint8_t Value) {
    DesfireApplicationDataType Data;
    ReadBlockBytes(&Data, BlockId, sizeof(Data));
    Data.AppData[AppSlot] = Value;
    WriteBlockBytes(&Data, BlockId, sizeof(Data));
}

void SetAppKeyCount(uint8_t AppSlot, uint8_t Value) {
    SetAppProperty(DESFIRE_APP_KEY_COUNT_BLOCK_ID, AppSlot, Value);
}

uint8_t GetAppKeySettings(uint8_t AppSlot) {
    return GetAppProperty(DESFIRE_APP_KEY_SETTINGS_BLOCK_ID, AppSlot);
}

void SetAppKeySettings(uint8_t AppSlot, uint8_t Value) {
    SetAppProperty(DESFIRE_APP_KEY_SETTINGS_BLOCK_ID, AppSlot, Value);
}

void SetAppKeyStorageBlockId(uint8_t AppSlot, uint8_t Value) {
    SetAppProperty(DESFIRE_APP_KEYS_PTR_BLOCK_ID, AppSlot, Value);
}

uint8_t GetAppFileIndexBlockId(uint8_t AppSlot) {
    return GetAppProperty(DESFIRE_APP_FILES_PTR_BLOCK_ID, AppSlot);
}

void SetAppFileIndexBlockId(uint8_t AppSlot, uint8_t Value) {
    SetAppProperty(DESFIRE_APP_FILES_PTR_BLOCK_ID, AppSlot, Value);
}

/*
 * Application key management
 */

uint8_t GetSelectedAppKeyCount(void) {
    return SelectedApp.KeyCount;
}

uint8_t GetSelectedAppKeySettings(void) {
    return SelectedApp.KeySettings;
}

void SetSelectedAppKeySettings(uint8_t KeySettings) {
    SelectedApp.KeySettings = KeySettings;
    SetAppKeySettings(SelectedApp.Slot, KeySettings);
}

void ReadSelectedAppKey(uint8_t KeyId, uint8_t* Key) {
    MemoryReadBlock(Key, SelectedApp.KeyAddress + KeyId * sizeof(Desfire2KTDEAKeyType), sizeof(Desfire2KTDEAKeyType));
}

void WriteSelectedAppKey(uint8_t KeyId, const uint8_t* Key) {
    MemoryWriteBlock(Key, SelectedApp.KeyAddress + KeyId * sizeof(Desfire2KTDEAKeyType), sizeof(Desfire2KTDEAKeyType));
}

/*
 * Application selection
 */

uint8_t LookupAppSlot(const DESFireAidType Aid) {
    uint8_t Slot;

    for (Slot = 0; Slot < DESFIRE_MAX_SLOTS; ++Slot) {
        if (!memcmp(AppDir.AppIds[Slot], Aid, DESFIRE_AID_SIZE))
            break;
    }

    return Slot;
}

void SelectAppBySlot(uint8_t AppSlot) {
    /* TODO: verify this behaviour */
    AbortTransaction();

    SelectedApp.Slot = AppSlot;

    SelectedApp.KeySettings = GetAppProperty(DESFIRE_APP_KEY_SETTINGS_BLOCK_ID, AppSlot);
    SelectedApp.KeyCount = GetAppProperty(DESFIRE_APP_KEY_COUNT_BLOCK_ID, AppSlot);
    SelectedApp.KeyAddress = GetAppProperty(DESFIRE_APP_KEYS_PTR_BLOCK_ID, AppSlot) * DESFIRE_EEPROM_BLOCK_SIZE;
    SelectedApp.FilesAddress = GetAppProperty(DESFIRE_APP_FILES_PTR_BLOCK_ID, AppSlot) * DESFIRE_EEPROM_BLOCK_SIZE;
}

uint8_t SelectApp(const DESFireAidType Aid) {
    uint8_t Slot;

    /* Search for the app slot */
    Slot = LookupAppSlot(Aid);
    if (Slot == DESFIRE_MAX_SLOTS) {
        return STATUS_APP_NOT_FOUND;
    }

    SelectAppBySlot(Slot);
    return STATUS_OPERATION_OK;
}

void SelectPiccApp(void) {
    SelectAppBySlot(DESFIRE_PICC_APP_SLOT);
}

bool IsPiccAppSelected(void) {
    return SelectedApp.Slot == DESFIRE_PICC_APP_SLOT;
}

/*
 * Application management
 */

uint8_t CreateApp(const DESFireAidType Aid, uint8_t KeyCount, uint8_t KeySettings) {
    uint8_t Slot;
    uint8_t FreeSlot;
    uint8_t KeysBlockId, FilesBlockId;

    /* Verify this AID has not been allocated yet */
    if (LookupAppSlot(Aid) != DESFIRE_MAX_SLOTS) {
        return STATUS_DUPLICATE_ERROR;
    }
    /* Verify there is space */
    Slot = AppDir.FirstFreeSlot;
    if (Slot == DESFIRE_MAX_SLOTS) {
        return STATUS_APP_COUNT_ERROR;
    }
    /* Update the next free slot */
    for (FreeSlot = Slot + 1; FreeSlot < DESFIRE_MAX_SLOTS; ++FreeSlot) {
        if ((AppDir.AppIds[FreeSlot][0] | AppDir.AppIds[FreeSlot][1] | AppDir.AppIds[FreeSlot][2]) == 0)
            break;
    }

    /* Allocate storage for the application */
    KeysBlockId = AllocateBlocks(DESFIRE_BYTES_TO_BLOCKS(KeyCount * sizeof(Desfire2KTDEAKeyType)));
    if (!KeysBlockId) {
        return STATUS_OUT_OF_EEPROM_ERROR;
    }
    FilesBlockId = AllocateBlocks(DESFIRE_FILE_INDEX_BLOCKS);
    if (!FilesBlockId) {
        return STATUS_OUT_OF_EEPROM_ERROR;
    }

    /* Initialise the application */
    SetAppKeySettings(Slot, KeySettings);
    SetAppKeyCount(Slot, KeyCount);
    SetAppKeyStorageBlockId(Slot, KeysBlockId);
    SetAppFileIndexBlockId(Slot, FilesBlockId);

    /* Update the directory */
    AppDir.FirstFreeSlot = FreeSlot;
    AppDir.AppIds[Slot][0] = Aid[0];
    AppDir.AppIds[Slot][1] = Aid[1];
    AppDir.AppIds[Slot][2] = Aid[2];
    SynchronizeAppDir();

    return STATUS_OPERATION_OK;
}

uint8_t DeleteApp(const DESFireAidType Aid) {
    uint8_t Slot;

    /* Search for the app slot */
    Slot = LookupAppSlot(Aid);
    if (Slot == DESFIRE_MAX_SLOTS) {
        return STATUS_APP_NOT_FOUND;
    }
    /* Deactivate the app */
    AppDir.AppIds[Slot][0] = 0;
    AppDir.AppIds[Slot][1] = 0;
    AppDir.AppIds[Slot][2] = 0;
    if (Slot < AppDir.FirstFreeSlot) {
        AppDir.FirstFreeSlot = Slot;
    }
    SynchronizeAppDir();

    if (Slot == SelectedApp.Slot) {
        SelectAppBySlot(DESFIRE_PICC_APP_SLOT);
    }
    return STATUS_OPERATION_OK;
}

void GetApplicationIdsSetup(void) {
    /* Skip the PICC application */
    TransferState.GetApplicationIds.NextIndex = 1;
}

TransferStatus GetApplicationIdsTransfer(uint8_t* Buffer) {
    TransferStatus Status;
    uint8_t EntryIndex;

    Status.BytesProcessed = 0;
    for (EntryIndex = TransferState.GetApplicationIds.NextIndex; EntryIndex < DESFIRE_MAX_SLOTS; ++EntryIndex) {
        if ((AppDir.AppIds[EntryIndex][0] | AppDir.AppIds[EntryIndex][1] | AppDir.AppIds[EntryIndex][2]) == 0)
            continue;
        /* If won't fit -- remember and return */
        if (Status.BytesProcessed >= DESFIRE_AID_SIZE * 19) {
            TransferState.GetApplicationIds.NextIndex = EntryIndex;
            Status.IsComplete = false;
            return Status;
        }
        Buffer[Status.BytesProcessed++] = AppDir.AppIds[EntryIndex][0];
        Buffer[Status.BytesProcessed++] = AppDir.AppIds[EntryIndex][1];
        Buffer[Status.BytesProcessed++] = AppDir.AppIds[EntryIndex][2];
    }
    Status.IsComplete = true;
    return Status;
}

uint16_t GetApplicationIdsIterator(uint8_t* Buffer, uint16_t ByteCount) {
    TransferStatus Status;

    Status = GetApplicationIdsTransfer(&Buffer[1]);
    if (Status.IsComplete) {
        Buffer[0] = STATUS_OPERATION_OK;
        DesfireState = DESFIRE_IDLE;
    }
    else {
        Buffer[0] = STATUS_ADDITIONAL_FRAME;
        DesfireState = DESFIRE_GET_APPLICATION_IDS2;
    }
    return DESFIRE_STATUS_RESPONSE_SIZE + Status.BytesProcessed;
}



