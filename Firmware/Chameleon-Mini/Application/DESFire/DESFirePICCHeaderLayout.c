/*
The DESFire stack portion of this firmware source 
is free software written by Maxie Dion Schmidt (@maxieds): 
You can redistribute it and/or modify
it under the terms of this license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

The complete source distribution of  
this firmware is available at the following link:
https://github.com/maxieds/ChameleonMiniFirmwareDESFireStack.

Based in part on the original DESFire code created by  
@dev-zzo (GitHub handle) [Dmitry Janushkevich] available at  
https://github.com/dev-zzo/ChameleonMini/tree/desfire.

This notice must be retained at the top of all source files where indicated. 
*/

/* 
 * DESFirePICCHeaderLayout.c
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "../../Common.h"
#include "DESFirePICCHeaderLayout.h"
#include "DESFirePICCControl.h"
#include "DESFireLogging.h"

SIZET PrettyPrintPICCHeaderData(BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
     SIZET charsWritten = 0x00;
     charsWritten  = snprintf_P(outputBuffer, maxLength, 
                                PSTR("(UID) %s\r\n"), 
                                GetHexBytesString(Picc.Uid, DESFIRE_UID_SIZE));
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("(VERSION) HW=%02x.%02x, SW=%02x.%02x\r\n"), 
                                Picc.HwVersionMajor, Picc.HwVersionMinor, 
                                Picc.SwVersionMajor, Picc.SwVersionMinor);
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("(BATCH) %s\r\n"), 
                                GetHexBytesString(Picc.BatchNumber, 5));
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("(DATE) %02x/%02x\r\n"), 
                                Picc.ProductionWeek, Picc.ProductionYear);
     BYTE atsBytes[6];
     memcpy(&atsBytes[0], Picc.ATSBytes, 5);
     atsBytes[5] = 0x80;
     BufferToHexString(__InternalStringBuffer, STRING_BUFFER_SIZE, atsBytes, 6);
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("(ATS) %s\r\n"), __InternalStringBuffer);
     return charsWritten;
}

SIZET PrettyPrintPICCFile(SelectedAppCacheType *appData, uint8_t fileIndex, 
                          BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    BYTE charsWritten = 0x00;
    BYTE fileNumber = LookupFileNumberByIndex(appData->Slot, fileIndex);
    if(fileNumber >= DESFIRE_MAX_FILES) {
         return charsWritten;
    }
    if(verbose) {
         BYTE fileCommSettings = ReadFileCommSettings(appData->Slot, fileIndex);
         SIZET fileAccessRights = ReadFileAccessRights(appData->Slot, fileIndex);
         charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                    PSTR("  -> %02x [Comm=%s -- %s]\r\n"), 
                                    fileNumber, 
                                    GetCommSettingsDesc(fileCommSettings), 
                                    GetFileAccessPermissionsDesc(fileAccessRights));
    }
    else {
         charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                    PSTR("  -> %02x\r\n"), 
                                    fileNumber);
    }
    return charsWritten;
}

SIZET PrettyPrintPICCFilesFull(SelectedAppCacheType *appData, BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    SIZET charsWritten = 0x00;
    BYTE fileIndex;
    charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                               PSTR(" [Files -- %d total of %d possible]\r\n"), 
                               appData->FileCount, DESFIRE_MAX_FILES);
    uint8_t fileNumberIndexMap[DESFIRE_MAX_FILES];
    ReadBlockBytes(&fileNumberIndexMap, appData->FileNumbersArrayMap, DESFIRE_MAX_FILES);
    for(fileIndex = 0; fileIndex < DESFIRE_MAX_FILES; fileIndex++) {
         if(fileNumberIndexMap[fileIndex] == DESFIRE_FILE_NOFILE_INDEX) {
             continue;
         }
         charsWritten += PrettyPrintPICCFile(appData, fileIndex, outputBuffer + charsWritten, 
                                             maxLength - charsWritten, verbose);
    }
    return charsWritten;
}

SIZET PrettyPrintPICCKey(SelectedAppCacheType *appData, uint8_t keyIndex, 
                         BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    if(!KeyIdValid(appData->Slot, keyIndex)) {
         return 0x00;
    }
    BYTE charsWritten = 0x00;
    BYTE keySettings = ReadKeySettings(appData->Slot, keyIndex);
    BYTE keyVersion = ReadKeyVersion(appData->Slot, keyIndex);
    BYTE keyType = ReadKeyCryptoType(appData->Slot, keyIndex);
    charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                               PSTR("  -> %s [v%02d"), 
                               GetCryptoMethodDesc(keyType), keyVersion);
    if((appData->Slot == DESFIRE_PICC_APP_SLOT) && (keyIndex == DESFIRE_MASTER_KEY_ID)) {
         charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                    PSTR(" -- PMK"));
         if(verbose) {
              charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                         PSTR(" : %02x"), keySettings);
         }
    }
    else if(keyIndex == DESFIRE_MASTER_KEY_ID) {
         charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                    PSTR(" -- AMK"));
         if(verbose) {
              charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                         PSTR(" : %02x"), keySettings);
         }
    }
    else if(verbose) {
         charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                    PSTR(" -- %02d"), keySettings);
    }
    charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                               PSTR("]\r\n"));
    return charsWritten;
}

SIZET PrettyPrintPICCKeysFull(SelectedAppCacheType *appData, BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    SIZET charsWritten = 0x00;
    BYTE keyIndex;
    charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                               PSTR(" [Keys -- %d of %d total]\r\n"), 
                               appData->KeyCount, appData->MaxKeyCount);
    uint16_t keyDataAddresses[DESFIRE_MAX_KEYS];
    ReadBlockBytes(&keyDataAddresses, ReadKeyStorageAddress(appData->Slot), 2 * DESFIRE_MAX_KEYS);
    for(keyIndex = 0; keyIndex < DESFIRE_MAX_KEYS; keyIndex++) {
         if(keyDataAddresses[keyIndex] == 0) {
             continue;
         }
         charsWritten += PrettyPrintPICCKey(appData, keyIndex, outputBuffer + charsWritten, 
                                            maxLength - charsWritten, verbose);
    }
    return charsWritten;
}

SIZET PrettyPrintPICCAppDir(uint8_t appIndex, BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    SIZET charsWritten = 0x00;
    BYTE keyIndex, fileIndex; 
    SelectedAppCacheType appData;
    if(!GetAppData(appIndex, &appData)) {
        return charsWritten;
    }
    appData.Slot = appIndex;
    charsWritten += PrettyPrintPICCKeysFull(&appData, outputBuffer + charsWritten, 
                                            maxLength - charsWritten, verbose);
    if(appIndex == 0) { // master
        return charsWritten;
    }
    charsWritten += PrettyPrintPICCFilesFull(&appData, outputBuffer + charsWritten, 
                                             maxLength - charsWritten, verbose);
    return charsWritten;
}

SIZET PrettyPrintPICCAppDirsFull(BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    SIZET charsWritten = 0x00;
    BYTE appDirIndex;
    for(appDirIndex = 0; appDirIndex < DESFIRE_MAX_SLOTS; appDirIndex++) {
         DESFireAidType curAID;
         memcpy(curAID, AppDir.AppIds[appDirIndex], MAX_AID_SIZE);
         if((curAID[0] | curAID[1] | curAID[2]) == 0x00 && appDirIndex > 0) {
              continue;
         }
         charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                    PSTR("%s== AID 0x%02x%02x%02x\r\n"), 
                                    curAID[0], curAID[1], curAID[2]);
         charsWritten += PrettyPrintPICCAppDir(appDirIndex, outputBuffer + charsWritten, 
                                               maxLength - charsWritten, verbose);
    }
    return charsWritten;
}

SIZET PrettyPrintPICCImageData(BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    BYTE charsWritten = 0x00;
    //charsWritten += snprintf_P(outputBuffer, maxLength, 
    //                           PSTR("**** DESFIRE HEADER DATA ****\r\n"));
    charsWritten += PrettyPrintPICCHeaderData(outputBuffer + charsWritten, maxLength - charsWritten, verbose);
    //charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
    //                           PSTR("**** DESFIRE INTERNAL STORAGE ****\r\n"));
    charsWritten += PrettyPrintPICCAppDirsFull(outputBuffer + charsWritten, maxLength - charsWritten, verbose);
    outputBuffer[maxLength - 1] = '\0';
    return charsWritten;
}
