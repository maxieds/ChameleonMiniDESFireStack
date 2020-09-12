/* TestAuthenticateAES.c */

#include "LibNFCUtils.h"
#include "LibNFCWrapper.h"
#include "Utils.h"
#include "CryptoUtils.h"

#define MAX_FRAME_LENGTH       (264)

/*
 * See Notes: https://stackoverflow.com/questions/52520044/desfire-ev1-communication-how-to-assign-iv
 */
uint8_t GET_AID_LIST_CMD[] = { 0x90, 0x6a, 0x00, 0x00, 0x00, 0x00 };
uint8_t SELECT_APP_CMD[]   = { 0x90, 0x5a, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00 }; 
uint8_t AUTH_AES_CMD[]     = { 0x90, 0xaa, 0x00, 0x00, 0x01, 0x00, 0x00 };

int main(int argc, char **argv) {

    nfc_context *nfcCtxt;
    nfc_device  *nfcPnd = GetNFCDeviceDriver(&nfcCtxt);
    if(nfcPnd == NULL) {
         return EXIT_FAILURE;
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;

    // Select AID application 0x000000:
    fprintf(stdout, ">>> Select Application By AID:\n");
    fprintf(stdout, "    -> [TO CHAM TAG] ");
    print_hex(SELECT_APP_CMD, sizeof(SELECT_APP_CMD));
    rxDataStatus = libnfcTransmitBytes(nfcPnd, SELECT_APP_CMD, sizeof(SELECT_APP_CMD), rxDataStorage);
    if(rxDataStatus) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        fprintf(stdout, "    !! Unable to transfer bytes !!\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "\n");

    // Get list of application IDs:
    fprintf(stdout, ">>> Get AID List From Device:\n");
    fprintf(stdout, "    -> [TO CHAM TAG] ");
    print_hex(GET_AID_LIST_CMD, sizeof(GET_AID_LIST_CMD));
    rxDataStatus = libnfcTransmitBytes(nfcPnd, GET_AID_LIST_CMD, sizeof(GET_AID_LIST_CMD), rxDataStorage);
    if(rxDataStatus) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        fprintf(stdout, "    !! Unable to transfer bytes !!\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "\n");

    // Start AES authentication (default key, blank setting of all zeros):
    fprintf(stdout, ">>> Start AES Authenticate:\n");
    fprintf(stdout, "    -> [TO CHAM TAG] ");
    print_hex(AUTH_AES_CMD, sizeof(AUTH_AES_CMD));
    rxDataStatus = libnfcTransmitBytes(nfcPnd, AUTH_AES_CMD, sizeof(AUTH_AES_CMD), rxDataStorage);
    if(rxDataStatus) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        fprintf(stdout, "    !! Unable to transfer bytes !!\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "\n");

    // Get list of application IDs:
    fprintf(stdout, ">>> Get AID List From Device:\n");
    fprintf(stdout, "    -> [TO CHAM TAG] ");
    print_hex(GET_AID_LIST_CMD, sizeof(GET_AID_LIST_CMD));
    rxDataStatus = libnfcTransmitBytes(nfcPnd, GET_AID_LIST_CMD, sizeof(GET_AID_LIST_CMD), rxDataStorage);
    if(rxDataStatus) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        fprintf(stdout, "    !! Unable to transfer bytes !!\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "\n");




    FreeRxDataStruct(rxDataStorage, true);
    FreeNFCDeviceDriver(&nfcCtxt, &nfcPnd);
    return EXIT_SUCCESS;

}

