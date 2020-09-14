/* TestGeneralCommands.c */

#include <stdlib.h>
#include <stdio.h>

#include <nfc/nfc.h>

#include "LibNFCUtils.h"
#include "LibNFCWrapper.h"
#include "DesfireUtils.h"


int main(int argc, char **argv) {

    nfc_context *nfcCtxt;
    nfc_device  *nfcPnd = GetNFCDeviceDriver(&nfcCtxt);
    if(nfcPnd == NULL) {
         return EXIT_FAILURE;
    }   
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;

    if(GetVersionCommand(nfcPnd)) {
        fprintf(stdout, "    -- !! GetVersion failed !!\n");
        return EXIT_FAILURE;
    }   
    else if(FormatPiccCommand(nfcPnd)) {
        fprintf(stdout, "    -- !! FormatPICC failed !!\n");
        return EXIT_FAILURE;
    }
    else if(GetCardUIDCommand(nfcPnd)) {
        fprintf(stdout, "    -- !! GetCardUID failed !!\n");
        return EXIT_FAILURE;
    }
    else if(SetConfigurationCommand(nfcPnd)) {
        fprintf(stdout, "    -- !! SetConfiguration failed !!\n");
        return EXIT_FAILURE;
    }
    else if(FreeMemoryCommand(nfcPnd)) {
        fprintf(stdout, "    -- !! FreeMemory failed !!\n");
        return EXIT_FAILURE;
    }

    FreeRxDataStruct(rxDataStorage, true);
    FreeNFCDeviceDriver(&nfcCtxt, &nfcPnd);
    return EXIT_SUCCESS;

}
