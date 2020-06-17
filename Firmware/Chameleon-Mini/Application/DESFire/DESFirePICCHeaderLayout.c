/* 
 * DESFirePICCHeaderLayout.c
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "DESFirePICCHeaderLayout.h"
#include "DESFirePICCControl.h"
#include "DESFireLogging.h"

const PROGMEM const char PPRINT_INDENT_LEVELS[][3] = {
     "", 
     "   ", 
     "      ",
};
BYTE PPrintIndentLevel = 0;

SIZET PrettyPrintPICCHeaderData(BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
     SIZET charsWritten = 0x00;
     charsWritten  = snprintf_P(outputBuffer, maxLength, 
                                PSTR("%s(UID) %s\n"), 
                                PPRINT_INDENT_LEVELS[PPrintIndentLevel],
                                GetHexBytesString(Picc.Uid, DESFIRE_UID_SIZE));
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("%s(VERSION) HW=%02x.%02x, SW=%02x.%02x\n"), 
                                PPRINT_INDENT_LEVELS[PPrintIndentLevel],
                                Picc.HwVersionMajor, Picc.HwVersionMinor, 
                                Picc.SwVersionMajor, Picc.SwVersionMinor);
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("%s(BATCH) %s\n"), 
                                PPRINT_INDENT_LEVELS[PPrintIndentLevel],
                                GetHexBytesString(Picc.BatchNumber, 5));
     charsWritten += snprintf_P(outputBuffer + charsWritten, maxLength - charsWritten, 
                                PSTR("%s(DATE) %02x/%02x\n"), 
                                PPRINT_INDENT_LEVELS[PPrintIndentLevel],
                                Picc.ProductionWeek, Picc.ProductionYear);
     return charsWritten;
}

SIZET PrettyPrintPICCFiles(uint8_t appIndex, uint8_t fileIndex, 
                           BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    return 0;
}

SIZET PrettyPrintPICCFilesFull(BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    return 0;
}

SIZET PrettyPrintPICCKeys(uint8_t keyIndex, 
                          BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    return 0;
}

SIZET PrettyPrintPICCKeysFull(BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    return 0;
}

SIZET PrettyPrintPICCAppDirs(uint8_t appIndex, BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    
    return 0;
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
                                    PSTR("%s== AID 0x%02x%02x%02x\n"), 
                                    PPRINT_INDENT_LEVELS[PPrintIndentLevel], 
                                    curAID[0], curAID[1], curAID[2]);
         ++PPrintIndentLevel;
         charsWritten += PrettyPrintPICCAppDirs(appDirIndex, outputBuffer + charsWritten, maxLength - charsWritten, verbose);
         PPrintIndentLevel--;
    }
    return charsWritten;
}

SIZET PrettyPrintPICCImageData(BYTE *outputBuffer, SIZET maxLength, BYTE verbose) {
    return 0;
}

