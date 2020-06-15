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
#include "DESFireUtils.h"

const BYTE DEFAULT_SELECT_DESFIRE_AID[] = { 
     0xd2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x00 
};
const BYTE DEFAULT_SELECT_ISO7816_AID[] = { 
     0xa0, 0x00, 0x00, 0x00, 0x03, 0x96 
};

DesfireApplicationDataType SelectedAppData = { 0 };

/*
 * Global card structure support routines
 */

void SynchronizeAppDir(void) {
    WriteBlockBytes(&AppDir, DESFIRE_APP_DIR_BLOCK_ID, sizeof(DESFireAppDirType));
}

SIZET GetAppProperty(DesfireCardLayout propId, BYTE AppSlot) {
     if(AppSlot >= DESFIRE_MAX_SLOTS) {
          return 0x00;
     }
     SelectedAppCacheType appCache;
     ReadBlockBytes(&appCache, AppSlot * SELECTED_APP_CACHE_TYPE_BLOCK_SIZE, sizeof(SelectedAppCacheType));
     switch(propId) {
          case DESFIRE_APP_KEY_COUNT_BLOCK_ID:
               return appCache.KeyCount;
          case DESFIRE_APP_FILE_COUNT_BLOCK_ID:
               return appCache.FileCount;
          case DESFIRE_APP_CRYPTO_COMM_STANDARD:
               return appCache.CryptoCommStandard;
          case DESFIRE_APP_KEY_SETTINGS_BLOCK_ID:
               return appCache.KeySettings;
          case DESFIRE_APP_FILE_NUMBER_ARRAY_MAP_BLOCK_ID:
               return appCache.FileNumbersArrayMap;
          case DESFIRE_APP_FILE_COMM_SETTINGS_BLOCK_ID:
               return appCache.FileCommSettings;
          case DESFIRE_APP_FILE_ACCESS_RIGHTS_BLOCK_ID:
               return appCache.FileAccessRights;
          case DESFIRE_APP_KEY_VERSIONS_ARRAY_BLOCK_ID:
               return appCache.KeyVersionsArray;
          case DESFIRE_APP_FILES_PTR_BLOCK_ID:
               return appCache.FilesAddress;
          case DESFIRE_APP_KEYS_PTR_BLOCK_ID:
               return appCache.KeyAddress;
          default:
               return 0x00; 
     }
}

void SetAppProperty(DesfireCardLayout propId, BYTE AppSlot, SIZET Value) {
     if(AppSlot >= DESFIRE_MAX_SLOTS) {
          return;
     }
     SelectedAppCacheType appCache;
     ReadBlockBytes(&appCache, AppSlot * SELECTED_APP_CACHE_TYPE_BLOCK_SIZE, sizeof(SelectedAppCacheType));
     switch(propId) {
          case DESFIRE_APP_KEY_COUNT:
               appCache.KeyCount = ExtractLSBBE(Value);
               break;
          case DESFIRE_APP_FILE_COUNT:
               appCache.FileCount = ExtractLSBBE(Value);
               break;
          case DESFIRE_APP_CRYPTO_COMM_STANDARD: 
               appCache.CryptoCommStandard = ExtractLSBBE(Value);
               break;
          case DESFIRE_APP_KEY_SETTINGS_BLOCK_ID:
               appCache.KeySettings = Value;
               break;
          case DESFIRE_APP_FILE_NUMBER_ARRAY_MAP_BLOCK_ID:
               appCache.FileNumbersArrayMap = Value;
               break;
          case DESFIRE_APP_FILE_COMM_SETTINGS_BLOCK_ID:
               appCache.FileCommSettings = Value;
               break;
          case DESFIRE_APP_FILE_ACCESS_RIGHTS_BLOCK_ID:
               appCache.FileAccessRights = Value;
               break;
          case DESFIRE_APP_KEY_VERSIONS_ARRAY_BLOCK_ID:
               appCache.KeyVersionsArray = Value;
               break;
          case DESFIRE_APP_FILES_PTR_BLOCK_ID:
               appCache.FilesAddress = Value;
               break;
          case DESFIRE_APP_KEYS_PTR_BLOCK_ID:
               appCache.KeyAddress = Value;
               break;
          default:
               return; 
     }
     WriteBlockBytes(&appCache, AppSlot * SELECTED_APP_CACHE_TYPE_BLOCK_SIZE, sizeof(SelectedAppCacheType));
}

/*
 * Application key management
 */

BYTE ReadKeyCount(uint8_t AppSlot) {
     return (BYTE) GetAppProperty(DESFIRE_APP_KEY_COUNT_BLOCK_ID, AppSlot);
}

void WriteKeyCount(uint8_t AppSlot, BYTE KeyCount) {
     SetAppProperty(DESFIRE_APP_KEY_COUNT_BLOCK_ID, AppSlot, (SIZET) KeyCount);
}

BYTE ReadKeySettings(uint8_t AppSlot, uint8_t KeyId) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || KeyId >= DESFIRE_MAX_KEYS) {
          return 0x00;
     }
     SIZET keySettingsBlockId = GetAppProperty(DESFIRE_APP_KEY_SETTINGS_BLOCK_ID, AppSlot);
     BYTE keySettingsArray[DESFIRE_MAX_KEYS];
     ReadBlockBytes(keySettingsArray, keySettingsBlockId, DESFIRE_MAX_KEYS);
     return keySettingsArray[KeyId];
}

void WriteKeySettings(uint8_t AppSlot, uint8_t KeyId, BYTE Value) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || KeyId >= DESFIRE_MAX_KEYS) {
          return;
     }
     SIZET keySettingsBlockId = GetAppProperty(DESFIRE_APP_KEY_SETTINGS_BLOCK_ID, AppSlot);
     BYTE keySettingsArray[DESFIRE_MAX_KEYS];
     ReadBlockBytes(keySettingsArray, keySettingsBlockId, DESFIRE_MAX_KEYS);
     keySettingsArray[KeyId] = Value;
     WriteBlockBytes(keySettingsArray, keySettingsBlockId, DESFIRE_MAX_KEYS);
}

BYTE ReadKeyVersion(uint8_t AppSlot, uint8_t KeyId) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || KeyId >= DESFIRE_MAX_KEYS) {
          return 0x00;
     }
     SIZET keyVersionsBlockId = GetAppProperty(DESFIRE_APP_KEY_VERSIONS_ARRAY, AppSlot);
     BYTE keyVersionsArray[DESFIRE_MAX_KEYS];
     ReadBlockBytes(keyVersionsArray, keyVersionsBlockId, DESFIRE_MAX_KEYS);
     return keyVersionsArray[KeyId];
}

void WriteKeyVersion(uint8_t AppSlot, uint8_t KeyId, BYTE Value) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || KeyId >= DESFIRE_MAX_KEYS) {
          return;
     }
     SIZET keyVersionsBlockId = GetAppProperty(DESFIRE_APP_KEY_VERSIONS_ARRAY, AppSlot);
     BYTE keyVersionsArray[DESFIRE_MAX_KEYS];
     ReadBlockBytes(keyVersionsArray, keyVersionsBlockId, DESFIRE_MAX_KEYS);
     keyVersionsArray[KeyId] = Value;
     WriteBlockBytes(keyVersionsArray, keyVersionsBlockId, DESFIRE_MAX_KEYS);
}

SIZET ReadKeyStorageAddress(uint8_t AppSlot) {
     return GetAppProperty(DESFIRE_APP_KEYS_PTR_BLOCK_ID, AppSlot);
}

void WriteKeyStorageAddress(uint8_t AppSlot, SIZET Value) {
     SetAppProperty(DESFIRE_APP_KEYS_PTR_BLOCK_ID, AppSlot, Value);
}

void ReadAppKey(uint8_t AppSlot, uint8_t KeyId, uint8_t *Key, SIZET KeySize) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || KeyId >= DESFIRE_MAX_KEYS) {
          return;
     }
     else if(KeySize > APP_CACHE_MAX_KEY_SIZE) {
          return;
     }
     SIZET keyStorageArrayBlockId = ReadKeyStorageAddress(AppSlot);
     SIZET keyStorageArray[DESFIRE_MAX_KEYS];
     ReadBlockBytes(keyStorageArray, keyStorageArrayBlockId, 2 * DESFIRE_MAX_KEYS);
     ReadBlockBytes(Key, keyStorageArray[KeyId], KeySize);
}

void WriteAppKey(uint8_t AppSlot, uint8_t KeyId, const uint8_t *Key, SIZE KeySize) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || KeyId >= DESFIRE_MAX_KEYS) {
          return;
     }
     else if(KeySize > APP_CACHE_MAX_KEY_SIZE) {
          return;
     }
     SIZET keyStorageArrayBlockId = ReadKeyStorageAddress(AppSlot);
     SIZET keyStorageArray[DESFIRE_MAX_KEYS];
     ReadBlockBytes(keyStorageArray, keyStorageArrayBlockId, 2 * DESFIRE_MAX_KEYS);
     WriteBlockBytes(Key, keyStorageArray[KeyId], KeySize);
}

/*
 * Application file management
 */

BYTE ReadFileCount(uint8_t AppSlot) {
     return (BYTE) GetAppProperty(DESFIRE_APP_FILE_COUNT, AppSlot);
}

void WriteFileCount(uint8_t AppSlot, BYTE FileCount) {
     SetAppProperty(DESFIRE_APP_FILE_COUNT, AppSlot, (SIZET) FileCount);
}

BYTE LookupFileNumberIndex(uint8_t AppSlot, BYTE FileNumber) {
     if(AppSlot >= DESFIRE_MAX_SLOTS) {
          return DESFIRE_MAX_FILES;
     }
     SIZET fileNumbersHashmapBlockId = GetAppProperty(DESFIRE_APP_FILE_NUMBER_ARRAY_MAP_BLOCK_ID, AppSlot);
     BYTE fileNumbersHashmap[DESFIRE_MAX_FILES];
     ReadBlockBytes(fileNumbersHashmap, fileNumbersHashmapBlockId, DESFIRE_MAX_FILES);
     BYTE fileIndex;
     for(fileIndex = 0; fileIndex < DESFIRE_MAX_FILES; fileIndex++) {
          if(fileNumbersHashmap[fileIndex] == FileNumber) {
               break;
          }
     }
     return fileIndex;
}

void WriteFileNumberAtIndex(uint8_t AppSlot, uint8_t FileIndex, BYTE FileNumber) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || FileIndex >= DESFIRE_MAX_FILES) {
          return;
     }
     SIZET fileNumbersHashmapBlockId = GetAppProperty(DESFIRE_APP_FILE_NUMBER_ARRAY_MAP_BLOCK_ID, AppSlot);
     BYTE fileNumbersHashmap[DESFIRE_MAX_FILES];
     ReadBlockBytes(fileNumbersHashmap, fileNumbersHashmapBlockId, DESFIRE_MAX_FILES);
     fileNumbersHashmap[FileIndex] = FileNumber;
     WriteBlockBytes(fileNumbersHashmap, fileNumbersHashmapBlockId, DESFIRE_MAX_FILES);
}

BYTE ReadFileCommSettings(uint8_t AppSlot, uint8_t FileIndex) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || FileIndex >= DESFIRE_MAX_FILES) {
          return;
     }
     SIZET fileCommSettingsBlockId = GetAppProperty(DESFIRE_APP_FILE_COMM_SETTINGS_BLOCK_ID, AppSlot);
     BYTE fileCommSettingsArray[DESFIRE_MAX_FILES];
     ReadBlockBytes(fileCommSettingsArray, fileCommSettingsBlockId, DESFIRE_MAX_FILES);
     return fileCommSettingsArray[FileIndex];
}

void WriteFileCommSettings(uint8_t AppSlot, uint8_t FileIndex, BYTE CommSettings) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || FileIndex >= DESFIRE_MAX_FILES) {
          return;
     }
     SIZET fileCommSettingsBlockId = GetAppProperty(DESFIRE_APP_FILE_COMM_SETTINGS_BLOCK_ID, AppSlot);
     BYTE fileCommSettingsArray[DESFIRE_MAX_FILES];
     ReadBlockBytes(fileCommSettingsArray, fileCommSettingsBlockId, DESFIRE_MAX_FILES);
     fileCommSettingsArray[FileIndex] = CommSettings;
     WriteBlockBytes(fileCommSettingsArray, fileCommSettingsBlockId, DESFIRE_MAX_FILES);
}

SIZET ReadFileAccessRights(uint8_t AppSlot, uint8_t FileIndex) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || FileIndex >= DESFIRE_MAX_FILES) {
          return 0x0000;
     }
     SIZET fileAccessRightsBlockId = GetAppProperty(DESFIRE_APP_FILE_ACCESS_RIGHTS_BLOCK_ID, AppSlot);
     SIZET fileAccessRightsArray[DESFIRE_MAX_FILES];
     ReadBlockBytes(fileAccessRightsArray, fileAccessRightsBlockId, 2 * DESFIRE_MAX_FILES);
     return fileAccessRightsArray[FileIndex];
}

void WriteFileAccessRights(uint8_t AppSlot, uint8_t FileIndex, SIZET AccessRights) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || FileIndex >= DESFIRE_MAX_FILES) {
          return;
     }
     SIZET fileAccessRightsBlockId = GetAppProperty(DESFIRE_APP_FILE_ACCESS_RIGHTS_BLOCK_ID, AppSlot);
     SIZET fileAccessRightsArray[DESFIRE_MAX_FILES];
     ReadBlockBytes(fileAccessRightsArray, fileAccessRightsBlockId, 2 * DESFIRE_MAX_FILES);
     fileAccessRightsArray[FileIndex] = AccessRights;
     WriteBlockBytes(fileAccessRightsArray, fileAccessRightsBlockId, 2 * DESFIRE_MAX_FILES);
}

DESFireFileTypeSettings ReadFileSettings(uint8_t AppSlot, uint8_t FileIndex) {
     DESFireFileTypeSettings fileTypeSettings = { 0 };
     if(AppSlot >= DESFIRE_MAX_SLOTS || FileIndex >= DESFIRE_MAX_FILES) {
          return fileTypeSettings;
     }
     SIZET fileTypeSettingsBlockId = GetAppProperty(DESFIRE_APP_FILES_PTR_BLOCK_ID, AppSlot);
     SIZET fileTypeSettingsAddresses[DESFIRE_MAX_FILES];
     ReadBlockBytes(fileTypeSettingsAddresses, fileTypeSettingsBlockId, 2 * DESFIRE_MAX_FILES);
     ReadBlockBytes(&fileTypeSettings, fileTypeSettingsAddresses[FileIndex], sizeof(DESFireFileTypeSettings));
     return fileTypeSettings;
}

void WriteFileSettings(uint8_t AppSlot, uint8_t FileIndex, DESFireFileTypeSettings *FileSettings) {
     if(AppSlot >= DESFIRE_MAX_SLOTS || FileIndex >= DESFIRE_MAX_FILES) {
          return;
     }
     else if(FileSettings == NULL) {
          return;
     }
     SIZET fileTypeSettingsBlockId = GetAppProperty(DESFIRE_APP_FILES_PTR_BLOCK_ID, AppSlot);
     SIZET fileTypeSettingsAddresses[DESFIRE_MAX_FILES];
     ReadBlockBytes(fileTypeSettingsAddresses, fileTypeSettingsBlockId, 2 * DESFIRE_MAX_FILES);
     WriteBlockBytes(FileSettings, fileTypeSettingsAddresses[FileIndex], sizeof(DESFireFileTypeSettings));
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
    AbortTransaction(); /* TODO: verify this behaviour */
    SIZET appCacheSelectedBlockId = DESFIRE_APP_CACHE_DATA_ARRAY_BLOCK_ID + 
                                    AppSlot * SELECTED_APP_CACHE_TYPE_BLOCK_SIZE;
    ReadBlockBytes(&SelectedApp, appCacheSelectedBlockId, sizeof(SelectedAppCacheType));
    SelectedApp.Slot = AppSlot;
}

uint16_t SelectApp(const DESFireAidType Aid) {
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
 * Application management (TODO: Start here ... )
 */

uint16_t CreateApp(const DESFireAidType Aid, uint8_t KeyCount, uint8_t KeySettings) {
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
    /* Verify ar not requesting more keys than the static capacity */
    if(KeyCount >= DESFIRE_MAX_KEYS) {
         return STATUS_NO_SUCH_KEY;
    }

    /* Update the next free slot */
    for (FreeSlot = Slot + 1; FreeSlot < DESFIRE_MAX_SLOTS; ++FreeSlot) {
        if ((AppDir.AppIds[FreeSlot][0] | AppDir.AppIds[FreeSlot][1] | AppDir.AppIds[FreeSlot][2]) == 0)
            break;
    }

    /* Allocate storage for the application */
    BYTE appStorageBlockSize = RoundBlockSize(sizeof(DesfireApplicationDataType), 
                                              DESFIRE_EEPROM_BLOCK_SIZE);
    uint8_t appDirStorageBlock = AllocateBlocks(appStorageBlockSize);
    if(appDirStorageBlock == 0) {
        return STATUS_OUT_OF_EEPROM_ERROR;
    }
    // Note: Creating the block does not mean we have selected it. 
    AppDir.AppIdPiccBlockOffsets[Slot] = appDirStorageBlock;
    AppDir.AppKeySettings[Slot] = KeySettings;

    /* Update the directory */
    AppDir.FirstFreeSlot = FreeSlot;
    AppDir.AppIds[Slot][0] = Aid[0];
    AppDir.AppIds[Slot][1] = Aid[1];
    AppDir.AppIds[Slot][2] = Aid[2];
    SynchronizeAppDir();

    return STATUS_OPERATION_OK;
}

uint16_t DeleteApp(const DESFireAidType Aid) {
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
    for(EntryIndex = TransferState.GetApplicationIds.NextIndex; 
        EntryIndex < DESFIRE_MAX_SLOTS; ++EntryIndex) {
        if ((AppDir.AppIds[EntryIndex][0] | AppDir.AppIds[EntryIndex][1] | 
             AppDir.AppIds[EntryIndex][2]) == 0)
            continue;
        /* If it won't fit -- remember and return */
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



