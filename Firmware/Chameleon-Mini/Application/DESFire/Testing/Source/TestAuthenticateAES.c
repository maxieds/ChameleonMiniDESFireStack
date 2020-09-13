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
    fprintf(stdout, "    -> ");
    print_hex(SELECT_APP_CMD, sizeof(SELECT_APP_CMD));
    rxDataStatus = libnfcTransmitBytes(nfcPnd, SELECT_APP_CMD, sizeof(SELECT_APP_CMD), rxDataStorage);
    if(rxDataStatus) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "\n");

    // Get list of application IDs:
    fprintf(stdout, ">>> Get AID List From Device:\n");
    fprintf(stdout, "    -> ");
    print_hex(GET_AID_LIST_CMD, sizeof(GET_AID_LIST_CMD));
    rxDataStatus = libnfcTransmitBytes(nfcPnd, GET_AID_LIST_CMD, sizeof(GET_AID_LIST_CMD), rxDataStorage);
    if(rxDataStatus) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "\n");

    // Start AES authentication (default key, blank setting of all zeros):
    fprintf(stdout, ">>> Start AES Authenticate:\n");
    fprintf(stdout, "    -> ");
    print_hex(AUTH_AES_CMD, sizeof(AUTH_AES_CMD));
    rxDataStatus = libnfcTransmitBytes(nfcPnd, AUTH_AES_CMD, sizeof(AUTH_AES_CMD), rxDataStorage);
    if(rxDataStatus) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        return EXIT_FAILURE;
    }

    // Now need to decrypt the challenge response sent back as rndB (8 bytes), 
    // rotate it left, generate a random 8 byte rndA, concat rndA+rotatedRndB, 
    // encrypt this 16 byte result, and send it forth to the PICC:
    uint8_t encryptedRndB[16], plainTextRndB[16], rotatedRndB[8];
    uint8_t rndA[8], challengeResponse[16], challengeResponseCipherText[16];
    uint8_t IVBuf[16], aesZeroKeyData[16];
    memcpy(encryptedRndB, rxDataStorage->rxDataBuf, 16);
    memset(aesZeroKeyData, 0x00, 16);
    AESCryptoData_t aesCryptoData = { 0 };
    aesCryptoData.keySize = 16;
    aesCryptoData.keyData = aesZeroKeyData;
    aesCryptoData.ivSize = 16;
    DecryptAES128(encryptedRndB, 16, plainTextRndB, aesCryptoData);
    RotateArrayLeft(plainTextRndB, rotatedRndB, 8);
    memcpy(IVBuf, rxDataStorage->rxDataBuf, 8);
    aesCryptoData.ivData = IVBuf;
    GenerateRandomBytes(rndA, 8);
    ConcatByteArrays(rndA, 8, rotatedRndB, 8, challengeResponse);
    EncryptAES128(challengeResponse, 16, challengeResponseCipherText, aesCryptoData);

    uint8_t sendBytesBuf[22];
    memset(sendBytesBuf, 0x00, 22);
    sendBytesBuf[0] = 0x90;
    sendBytesBuf[1] = 0xaf;
    sendBytesBuf[4] = 0x10;
    memcpy(sendBytesBuf + 5, challengeResponseCipherText, 16);

    fprintf(stdout, "    -> ");
    print_hex(sendBytesBuf, sizeof(sendBytesBuf));
    rxDataStatus = libnfcTransmitBytes(nfcPnd, sendBytesBuf, 22, rxDataStorage);
    if(rxDataStatus) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        return EXIT_FAILURE;
    }

    // Finally, to finish up the auth process: 
    // decrypt rndA sent by PICC, compare it to our original randomized rndA computed above, 
    // and report back whether they match: 
    uint8_t decryptedRndAFromPICC[16];
    DecryptAES128(rxDataStorage->rxDataBuf, 16, decryptedRndAFromPICC, aesCryptoData);
    if(memcmp(rndA, decryptedRndAFromPICC, 8)) {
        fprintf(stdout, "       ... AUTH OK! :)\n\n");
    }
    else {
        fprintf(stdout, "       ... AUTH FAILED -- X; :(\n\n");
        return EXIT_FAILURE;
    }

    // Get list of application IDs:
    fprintf(stdout, ">>> Get AID List From Device:\n");
    fprintf(stdout, "    -> ");
    print_hex(GET_AID_LIST_CMD, sizeof(GET_AID_LIST_CMD));
    rxDataStatus = libnfcTransmitBytes(nfcPnd, GET_AID_LIST_CMD, sizeof(GET_AID_LIST_CMD), rxDataStorage);
    if(rxDataStatus) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "\n");

    FreeRxDataStruct(rxDataStorage, true);
    FreeNFCDeviceDriver(&nfcCtxt, &nfcPnd);
    return EXIT_SUCCESS;

}

