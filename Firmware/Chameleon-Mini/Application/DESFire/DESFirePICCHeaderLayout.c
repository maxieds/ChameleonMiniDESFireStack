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

This source code is only licensed for 
redistribution under for non-commercial users. 
All commerical use or inclusion of this 
software requires express written consent of the author (MDS). 
This restriction pertains to any binary distributions which 
are derivative works of this software.

The author is free to revoke or modify this license for future 
versions of the code at free will.
*/

/* 
 * DESFirePICCHeaderLayout.c
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "DESFirePICCHeaderLayout.h"
#include "DESFirePICCControl.h"
#include "DESFireLogging.h"

const const char PPRINT_INDENT_LEVELS[][3] = {
     "", 
     "   ", 
     "      ",
};
BYTE PPrintIndentLevel = 0;

SIZET PrettyPrintPICCHeaderData(BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
     SIZET charsWritten = 0x00;
     charsWritten  = snprintf_P(outputBuffer, maxLength, 
                                PSTR("%s(UID) %s\r\n"), 
                                PPRINT_INDENT_LEVELS[PPrintIndentLevel],
                                GetHexBytesString(Picc.Uid, DESFIRE_UID_SIZE));
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("%s(VERSION) HW=%02x.%02x, SW=%02x.%02x\r\n"), 
                                PPRINT_INDENT_LEVELS[PPrintIndentLevel],
                                Picc.HwVersionMajor, Picc.HwVersionMinor, 
                                Picc.SwVersionMajor, Picc.SwVersionMinor);
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("%s(BATCH) %s\r\n"), 
                                PPRINT_INDENT_LEVELS[PPrintIndentLevel],
                                GetHexBytesString(Picc.BatchNumber, 5));
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("%s(DATE) %02x/%02x\r\n"), 
                                PPRINT_INDENT_LEVELS[PPrintIndentLevel],
                                Picc.ProductionWeek, Picc.ProductionYear);
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
                                    PSTR("%s-> %02x [Comm=%s -- %s]\r\n"), 
                                    PPRINT_INDENT_LEVELS[PPrintIndentLevel], 
                                    fileNumber, 
                                    GetCommSettingsDesc(fileCommSettings), 
                                    GetFileAccessPermissionsDesc(fileAccessRights));
    }
    else {
         charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                    PSTR("%s-> %02x\r\n"), 
                                    PPRINT_INDENT_LEVELS[PPrintIndentLevel], 
                                    fileNumber);
    }
    return charsWritten;
}

SIZET PrettyPrintPICCFilesFull(SelectedAppCacheType *appData, BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    SIZET charsWritten = 0x00;
    BYTE fileIndex;
    charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                               PSTR("%s[Files -- %d total]\r\n"), 
                               PPRINT_INDENT_LEVELS[PPrintIndentLevel], appData->FileCount);
    for(fileIndex = 0; fileIndex < DESFIRE_MAX_FILES; fileIndex++) {
         ++PPrintIndentLevel;
         charsWritten += PrettyPrintPICCFile(appData, fileIndex, outputBuffer + charsWritten, 
                                             maxLength - charsWritten, verbose);
         --PPrintIndentLevel;
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
                               PSTR("%s-> %s [v%02d"), 
                               PPRINT_INDENT_LEVELS[PPrintIndentLevel], 
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
                               PSTR("%s[Keys -- %d total]\r\n"), 
                               PPRINT_INDENT_LEVELS[PPrintIndentLevel], appData->KeyCount);
    for(keyIndex = 0; keyIndex < DESFIRE_MAX_KEYS; keyIndex++) {
         ++PPrintIndentLevel;
         charsWritten += PrettyPrintPICCKey(appData, keyIndex, outputBuffer + charsWritten, 
                                            maxLength - charsWritten, verbose);
         --PPrintIndentLevel;
    }
    return charsWritten;
}

SIZET PrettyPrintPICCAppDir(uint8_t appIndex, BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    SIZET charsWritten = 0x00;
    BYTE keyIndex, fileIndex; 
    SelectedAppCacheType appData;
    GetAppData(appIndex, &appData);
    charsWritten += PrettyPrintPICCKeysFull(&appData, outputBuffer + charsWritten, 
                                            maxLength - charsWritten, verbose);
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
         if((curAID[0] | curAID[1] | curAID[2]) == 0x00) {
              continue;
         }
         charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                    PSTR("%s== AID 0x%02x%02x%02x\r\n"), 
                                    PPRINT_INDENT_LEVELS[PPrintIndentLevel], 
                                    curAID[0], curAID[1], curAID[2]);
         ++PPrintIndentLevel;
         charsWritten += PrettyPrintPICCAppDir(appDirIndex, outputBuffer + charsWritten, 
                                                maxLength - charsWritten, verbose);
         PPrintIndentLevel--;
    }
    return charsWritten;
}

SIZET PrettyPrintPICCImageData(BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    BYTE charsWritten = 0x00;
    charsWritten += snprintf_P(outputBuffer, maxLength, 
                               PSTR("**** DESFIRE HEADER DATA ****\r\n"));
    ++PPrintIndentLevel;
    charsWritten += PrettyPrintPICCHeaderData(outputBuffer + charsWritten, maxLength - charsWritten, verbose);
    PPrintIndentLevel--;
    charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                               PSTR("**** DESFIRE INTERNAL STORAGE ****\r\n"));
    ++PPrintIndentLevel;
    charsWritten += PrettyPrintPICCAppDirsFull(outputBuffer + charsWritten, maxLength - charsWritten, verbose);
    return charsWritten;
}
