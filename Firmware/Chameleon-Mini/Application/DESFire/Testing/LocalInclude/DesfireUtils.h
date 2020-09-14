/* Utils.h */

#ifndef __LOCAL_UTILS_H__
#define __LOCAL_UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <nfc/nfc.h>

#include "ErrorHandling.h"
#include "CryptoUtils.h"
#include "LibNFCUtils.h"

#define MAX_FRAME_LENGTH             (264)
#define APPLICATION_AID_LENGTH       (3)

static const inline uint8_t MASTER_APPLICATION_AID[] = {
    0x00, 0x00, 0x00
};

static const inline uint8_t MASTER_KEY_INDEX = 0x00;

static const inline uint8_t SELECT_APPLICATION_CMD[]   = { 
    0x90, 0x5a, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00 
};

static inline int SelectApplication(nfc_device *nfcConnDev, uint8_t *aidBytes, size_t aidLength) {
    if(nfcConnDev == NULL || aidBytes == NULL || aidLength < APPLICATION_AID_LENGTH) {
        return INVALID_PARAMS_ERROR;
    }
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> Select Application By AID:\n");
        fprintf(stdout, "    -> ");
        print_hex(SELECT_APPLICATION_CMD, sizeof(SELECT_APPLICATION_CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, SELECT_APPLICATION_CMD, sizeof(SELECT_APPLICATION_CMD), rxDataStorage);
    if(rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    <- ");
            print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
            fprintf(stdout, "\n");
        }
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
            fprintf(stdout, "\n");
        }
        return EXIT_FAILURE;
    }
}

static const inline uint8_t GET_APPLICATION_AID_LIST_CMD[] = { 
    0x90, 0x6a, 0x00, 0x00, 0x00, 0x00 
};

static inline int GetApplicationIds(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> Get AID List From Device:\n");
        fprintf(stdout, "    -> ");
        print_hex(GET_APPLICATION_AID_LIST_CMD, sizeof(GET_APPLICATION_AID_LIST_CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;   
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, GET_APPLICATION_AID_LIST_CMD, 
                                       sizeof(GET_APPLICATION_AID_LIST_CMD), rxDataStorage);
    if(rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    <- ");
            print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
            fprintf(stdout, "\n");
        }
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
            fprintf(stdout, "\n");
        }
        return EXIT_FAILURE;
    }
}

static inline int AuthenticateAES128(nfc_device *nfcConnDev, int authType, uint8_t keyIndex, const uint8_t *keyData) {
    
    if(nfcConnDev == NULL || keyData == NULL) {
        return INVALID_PARAMS_ERROR;
    }

    // Start AES authentication (default key, blank setting of all zeros):
    uint8_t AUTHENTICATE_AES_CMD[] = {
        0x90, 0xaa, 0x00, 0x00, 0x01, 0x00, 0x00
    };
    AUTHENTICATE_AES_CMD[5] = keyIndex;
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> Start AES Authenticate:\n");
        fprintf(stdout, "    -> ");
        print_hex(AUTHENTICATE_AES_CMD, sizeof(AUTHENTICATE_AES_CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, AUTHENTICATE_AES_CMD, sizeof(AUTHENTICATE_AES_CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        return EXIT_FAILURE;
    }

    // Now need to decrypt the challenge response sent back as rndB (8 bytes), 
    // rotate it left, generate a random 8 byte rndA, concat rndA+rotatedRndB, 
    // encrypt this 16 byte result, and send it forth to the PICC:
    uint8_t encryptedRndB[16], plainTextRndB[16], rotatedRndB[8];
    uint8_t rndA[8], challengeResponse[16], challengeResponseCipherText[16];
    uint8_t IVBuf[16];
    memcpy(encryptedRndB, rxDataStorage->rxDataBuf, 16);
    CryptoData_t aesCryptoData = { 0 };
    aesCryptoData.keySize = 16;
    aesCryptoData.keyData = keyData;
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

    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    -> ");
        print_hex(sendBytesBuf, sizeof(sendBytesBuf));
    }
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, sendBytesBuf, 22, rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        return EXIT_FAILURE;
    }

    // Finally, to finish up the auth process: 
    // decrypt rndA sent by PICC, compare it to our original randomized rndA computed above, 
    // and report back whether they match: 
    uint8_t decryptedRndAFromPICC[16];
    DecryptAES128(rxDataStorage->rxDataBuf, 16, decryptedRndAFromPICC, aesCryptoData);
    if(memcmp(rndA, decryptedRndAFromPICC, 8)) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "       ... AUTH OK! :)\n\n");
        }
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "       ... AUTH FAILED -- X; :(\n\n");
        }
        return EXIT_FAILURE;
    }
}

static inline int AuthenticateIso(nfc_device *nfcConnDev, int authType, uint8_t keyIndex, const uint8_t *keyData) {
    
    if(nfcConnDev == NULL || keyData == NULL) {
        return INVALID_PARAMS_ERROR;
    }

    // Start AES authentication (default key, blank setting of all zeros):
    uint8_t AUTHENTICATE_ISO_CMD[] = {
        0x90, 0x1a, 0x00, 0x00, 0x01, 0x00, 0x00
    };
    AUTHENTICATE_ISO_CMD[5] = keyIndex;
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> Start ISO Authenticate:\n");
        fprintf(stdout, "    -> ");
        print_hex(AUTHENTICATE_ISO_CMD, sizeof(AUTHENTICATE_ISO_CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, AUTHENTICATE_ISO_CMD, sizeof(AUTHENTICATE_ISO_CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        return EXIT_FAILURE;
    }

    // Now need to decrypt the challenge response sent back as rndB (8 bytes), 
    // rotate it left, generate a random 8 byte rndA, concat rndA+rotatedRndB, 
    // encrypt this 16 byte result, and send it forth to the PICC:
    uint8_t encryptedRndB[16], plainTextRndB[16], rotatedRndB[8];
    uint8_t rndA[8], challengeResponse[16], challengeResponseCipherText[16];
    uint8_t IVBuf[16];
    memcpy(encryptedRndB, rxDataStorage->rxDataBuf, 16);
    Decrypt3DES(16, encryptedRndB, plainTextRndB, keyData);
    RotateArrayLeft(plainTextRndB, rotatedRndB, 8);
    memcpy(IVBuf, rxDataStorage->rxDataBuf, 8);
    GenerateRandomBytes(rndA, 8);
    ConcatByteArrays(rndA, 8, rotatedRndB, 8, challengeResponse);
    Encrypt3DES(16, challengeResponse, challengeResponseCipherText, keyData);

    uint8_t sendBytesBuf[22];
    memset(sendBytesBuf, 0x00, 22);
    sendBytesBuf[0] = 0x90;
    sendBytesBuf[1] = 0xaf;
    sendBytesBuf[4] = 0x10;
    memcpy(sendBytesBuf + 5, challengeResponseCipherText, 16);

    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    -> ");
        print_hex(sendBytesBuf, sizeof(sendBytesBuf));
    }
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, sendBytesBuf, 22, rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        return EXIT_FAILURE;
    }

    // Finally, to finish up the auth process: 
    // decrypt rndA sent by PICC, compare it to our original randomized rndA computed above, 
    // and report back whether they match: 
    uint8_t decryptedRndAFromPICC[16];
    Decrypt3DES(16, rxDataStorage->rxDataBuf, decryptedRndAFromPICC, keyData);
    if(memcmp(rndA, decryptedRndAFromPICC, 8)) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "       ... AUTH OK! :)\n\n");
        }
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "       ... AUTH FAILED -- X; :(\n\n");
        }
        return EXIT_FAILURE;
    }
}

static inline int AuthenticateLegacy(nfc_device *nfcConnDev, int authType, uint8_t keyIndex, const uint8_t *keyData) {



    return EXIT_SUCCESS;
}

static inline int Authenticate(nfc_device *nfcConnDev, int authType, uint8_t keyIndex, const uint8_t *keyData) {
    if(nfcConnDev == NULL || keyData == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    switch(authType) {
        case DESFIRE_CRYPTO_AUTHTYPE_AES128:
            return AuthenticateAES128(nfcConnDev, authType, keyIndex, keyData);
        case DESFIRE_CRYPTO_AUTHTYPE_ISODES:
            return AuthenticateISO(nfcConnDev, authType, keyIndex, keyData);
        case DESFIRE_CRYPTO_AUTHTYPE_LEGACY:
            return AuthenticateLegacy(nfcConnDev, authType, keyIndex, keyData);
        default:
            break;
    }
    return EXIT_FAILURE;
}

static inline int SetApplicationKeyData(nfc_device *nfcConnDev, uint8_t keyIndex, uint8_t *keyData) {

    return EXIT_SUCCESS;
}

#endif
