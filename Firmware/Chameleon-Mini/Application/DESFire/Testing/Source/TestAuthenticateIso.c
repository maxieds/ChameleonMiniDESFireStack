/* TestAuthenticateIso.c */

#include "LibNFCUtils.h"
#include "LibNFCWrapper.h"
#include "DesfireUtils.h"
#include "CryptoUtils.h"

int main(int argc, char **argv) {

    nfc_context *nfcCtxt;
    nfc_device  *nfcPnd = GetNFCDeviceDriver(&nfcCtxt);
    if(nfcPnd == NULL) {
         return EXIT_FAILURE;
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;

    // Select AID application 0x000000:
    if(SelectApplication(nfcPnd, MASTER_APPLICATION_AID, APPLICATION_AID_LENGTH)) {
        return EXIT_FAILURE;
    }

    // Start ISO authentication (default key, blank setting of all zeros):
    if(AuthenticateIso(nfcPnd, DESFIRE_CRYPTO_AUTHTYPE_AES128,
                       MASTER_KEY_INDEX, ZERO_KEY)) {
        return EXIT_FAILURE;
    }

    FreeRxDataStruct(rxDataStorage, true);
    FreeNFCDeviceDriver(&nfcCtxt, &nfcPnd);
    return EXIT_SUCCESS;

}
