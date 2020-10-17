/* Utils.h */

#ifndef __LOCAL_UTILS_H__
#define __LOCAL_UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <nfc/nfc.h>

#include "ErrorHandling.h"
#include "Config.h"
#include "CryptoUtils.h"
#include "LibNFCUtils.h"
#include "GeneralUtils.h"

static inline int AuthenticateAES128(nfc_device *nfcConnDev, uint8_t keyIndex, const uint8_t *keyData) {
    
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
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }

    // Now need to decrypt the challenge response sent back as rndB (8 bytes), 
    // rotate it left, generate a random 8 byte rndA, concat rndA+rotatedRndB, 
    // encrypt this 16 byte result, and send it forth to the PICC:
    uint8_t encryptedRndB[16], plainTextRndB[16], rotatedRndB[8];
    uint8_t rndA[8], challengeResponse[16], challengeResponseCipherText[16];
    int8_t IVBuf[16];
    memcpy(encryptedRndB, rxDataStorage->rxDataBuf, 16);
    CryptoData_t aesCryptoData = { 0 };
    aesCryptoData.keySize = 16;
    aesCryptoData.keyData = keyData;
    aesCryptoData.ivSize = 16;
    DecryptAES128(encryptedRndB, 16, plainTextRndB, aesCryptoData);
    RotateArrayLeft(plainTextRndB, rotatedRndB, 8);
    //memcpy(IVBuf, rxDataStorage->rxDataBuf, 8);
    memset(IVBuf, 0x00, 16);
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
        fprintf(stdout, "    -- RNDA = "); print_hex(rndA, 8);
        fprintf(stdout, "    -- RNDB = "); print_hex(plainTextRndB, 8);
        fprintf(stdout, "    -- CHAL = "); print_hex(challengeResponse, 16);
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
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }

    // Finally, to finish up the auth process: 
    // decrypt rndA sent by PICC, compare it to our original randomized rndA computed above, 
    // and report back whether they match: 
    uint8_t decryptedRndAFromPICCRotated[16], decryptedRndA[16];
    DecryptAES128(rxDataStorage->rxDataBuf, 16, decryptedRndAFromPICCRotated, aesCryptoData);
    RotateArrayRight(decryptedRndAFromPICCRotated, decryptedRndA, 8);
    if(!memcmp(rndA, decryptedRndA, 8)) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "       ... AUTH OK! :)\n\n");
        }
        AUTHENTICATED = true;
        AUTHENTICATED_PROTO = DESFIRE_CRYPTO_AUTHTYPE_AES128;
        memcpy(CRYPTO_RNDB_STATE, plainTextRndB, 8);
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "       ... AUTH FAILED -- X; :(\n\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
}

static inline int Authenticate(nfc_device *nfcConnDev, int authType, uint8_t keyIndex, const uint8_t *keyData) {
    InvalidateAuthState();
    if(nfcConnDev == NULL || keyData == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    switch(authType) {
        case DESFIRE_CRYPTO_AUTHTYPE_AES128:
            return AuthenticateAES128(nfcConnDev, keyIndex, keyData);
        default:
            break;
    }
    return EXIT_FAILURE;
}

static inline int GetVersionCommand(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t GET_VERSION_CMD_INIT[] = {
        0x90, 0x60, 0x00, 0x00, 0x00, 0x00
    };
    uint8_t CONTINUE_CMD[] = {
        0x90, 0xaf, 0x00, 0x00, 0x00, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> GetVersion command:\n");
        fprintf(stdout, "    -> ");
        print_hex(GET_VERSION_CMD_INIT, sizeof(GET_VERSION_CMD_INIT));
    }   
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, GET_VERSION_CMD_INIT, 
                                       sizeof(GET_VERSION_CMD_INIT), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    } 
    for(int extraCmd = 0; extraCmd < 2; extraCmd++) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -> ");
            print_hex(CONTINUE_CMD, sizeof(CONTINUE_CMD));
        }   
        rxDataStatus = libnfcTransmitBytes(nfcConnDev, CONTINUE_CMD, 
                                           sizeof(CONTINUE_CMD), rxDataStorage);
        if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    <- ");
            print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
        }
        else if(!rxDataStatus) {
            if(PRINT_STATUS_EXCHANGE_MESSAGES) {
                fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
            }
            FreeRxDataStruct(rxDataStorage, true);
            return EXIT_FAILURE;
        }
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int FormatPiccCommand(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t FORMAT_PICC_CMD[] = {
        0x90, 0xfc, 0x00, 0x00, 0x00, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> FormatPICC command:\n");
        fprintf(stdout, "    -> ");
        print_hex(FORMAT_PICC_CMD, sizeof(FORMAT_PICC_CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, FORMAT_PICC_CMD, 
                                       sizeof(FORMAT_PICC_CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int GetCardUIDCommand(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0x51, 0x00, 0x00, 0x00, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> GetCardUID command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, 
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int SetConfigurationCommand(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0x5c, 0x00, 0x00, 0x00, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> SetConfiguration command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, 
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
        fprintf(stdout, "    -- !! TODO: NOT IMPLEMENTED !!\n");
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int FreeMemoryCommand(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0x6e, 0x00, 0x00, 0x00, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> FreeMemory command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, 
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int ChangeKeyCommand(nfc_device *nfcConnDev, uint8_t keyNo, const uint8_t *keyData, int keyType) {
    if(nfcConnDev == NULL || keyData == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    else if(keyType != DESFIRE_CRYPTO_AUTHTYPE_AES128 && keyType != DESFIRE_CRYPTO_AUTHTYPE_ISODES) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t keySize = keyType == DESFIRE_CRYPTO_AUTHTYPE_AES128 ? 16 : 24;
    size_t cmdBufSize = 7 + keySize;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0xc4;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = keySize + 1;
    CMD[5] = keyNo;
    memcpy(CMD + 6, keyData, keySize);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> ChangeKey command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD,
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int GetKeySettingsCommand(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0x45, 0x00, 0x00, 0x00, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> GetKeySettings command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD,
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int ChangeKeySettingsCommand(nfc_device *nfcConnDev, const uint8_t keySettingsData) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[7];
    CMD[0] = 0x90;
    CMD[1] = 0x54;
    memset(CMD + 2, 0x00, 5);
    CMD[4] = 1;
    CMD[5] = keySettingsData;
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> ChangeKeySettings command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD,
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int GetKeyVersionCommand(nfc_device *nfcConnDev, uint8_t keyNo) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0x64, 0x00, 0x00, 0x01, keyNo, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> GetKeyVersion command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD,
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int GetApplicationIds(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t GET_APPLICATION_AID_LIST_CMD[] = { 
        0x90, 0x6a, 0x00, 0x00, 0x00, 0x00 
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> GetApplicationIds command:\n");
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
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
            fprintf(stdout, "\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
}

static inline int CreateApplication(nfc_device *nfcConnDev, uint8_t *aidBytes, 
                                    uint8_t keySettings, uint8_t numKeys) {
    if(nfcConnDev == NULL || aidBytes == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[11];
    CMD[0] = 0x90;
    CMD[1] = 0xca;
    memset(CMD + 2, 0x00, 9);
    CMD[4] = 5;
    memcpy(CMD + 5, aidBytes, 3);
    CMD[8] = keySettings;
    CMD[9] = numKeys;
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> CreateApplication command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int DeleteApplication(nfc_device *nfcConnDev, uint8_t *aidBytes) {
    if(nfcConnDev == NULL || aidBytes == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[9];
    CMD[0] = 0x90;
    CMD[1] = 0xda;
    memset(CMD + 2, 0x00, 7);
    CMD[4] = 3;
    memcpy(CMD + 5, aidBytes, 3);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> DeleteApplication command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int SelectApplication(nfc_device *nfcConnDev, uint8_t *aidBytes, size_t aidLength) {
    if(nfcConnDev == NULL || aidBytes == NULL || aidLength < APPLICATION_AID_LENGTH) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + aidLength;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0x5a;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = aidLength;
    memcpy(CMD + 5, aidBytes, aidLength);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> Select Application By AID:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
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

static inline int GetDFNamesCommand(nfc_device *nfcConnDev) {
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    -- !! GetDFNames command NOT IMPLEMENTED !!\n");
    }
    return EXIT_SUCCESS;
}

static inline int CreateStandardDataFile(nfc_device *nfcConnDev, uint8_t fileNo, uint8_t commSettings, 
                                         uint16_t accessRights, uint16_t fileSize) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 1 + 2 + 3;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0xcd;
    memset(CMD + 2, 0x00, cmdBufSize - 2); 
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    CMD[6] = commSettings;
    CMD[7] = (uint8_t) (accessRights & 0x00ff);
    CMD[8] = (uint8_t) ((accessRights >> 8) & 0x00ff);
    CMD[9] = (uint8_t) (fileSize & 0x00ff);
    CMD[10] = (uint8_t) ((fileSize >> 8) & 0x00ff);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> CreateStdDataFile command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }   
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
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

static inline int CreateBackupDataFile(nfc_device *nfcConnDev, uint8_t fileNo, uint8_t commSettings,
                                         uint16_t accessRights, uint16_t fileSize) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 1 + 2 + 3;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0xcb;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    CMD[6] = commSettings;
    CMD[7] = (uint8_t) (accessRights & 0x00ff);
    CMD[8] = (uint8_t) ((accessRights >> 8) & 0x00ff);
    CMD[9] = (uint8_t) (fileSize & 0x00ff);
    CMD[10] = (uint8_t) ((fileSize >> 8) & 0x00ff);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> CreateBackupDataFile command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }   
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
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

static inline int CreateValueFile(nfc_device *nfcConnDev, uint8_t fileNo, uint8_t CommSettings, 
                                  uint16_t AccessRights, uint32_t LowerLimit, uint32_t UpperLimit, 
                                  uint32_t Value, uint8_t LimitedCreditEnabled) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 1 + 2 + 4 + 4 + 4 + 1;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0xcc;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    CMD[6] = CommSettings;
    CMD[7] = (uint8_t) (AccessRights & 0x00ff);
    CMD[8] = (uint8_t) ((AccessRights >> 8) & 0x00ff);
    Int32ToByteBuffer(CMD + 9, LowerLimit);
    Int32ToByteBuffer(CMD + 13, UpperLimit);
    Int32ToByteBuffer(CMD + 17, Value);
    CMD[21] = LimitedCreditEnabled;
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> CreateValueFile command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }   
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
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

static inline int CreateLinearRecordFile(nfc_device *nfcConnDev, uint8_t fileNo, 
                                         uint8_t commSettings, uint16_t accessRights, 
                                         uint16_t recordSize, uint16_t maxRecords) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 1 + 2 + 3 + 3;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0xc1;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    CMD[6] = commSettings;
    CMD[7] = (uint8_t) (accessRights & 0x00ff);
    CMD[8] = (uint8_t) ((accessRights >> 8) & 0x00ff);
    Int24ToByteBuffer(CMD + 9, recordSize);
    Int24ToByteBuffer(CMD + 12, maxRecords);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> CreateLinearRecordFile command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }   
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
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

static inline int CreateCyclicRecordFile(nfc_device *nfcConnDev, uint8_t fileNo, 
                                         uint8_t commSettings, uint16_t accessRights, 
                                         uint16_t recordSize, uint16_t maxRecords) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 1 + 2 + 3 + 3;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0xc0;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    CMD[6] = commSettings;
    CMD[7] = (uint8_t) (accessRights & 0x00ff);
    CMD[8] = (uint8_t) ((accessRights >> 8) & 0x00ff);
    Int24ToByteBuffer(CMD + 9, recordSize);
    Int24ToByteBuffer(CMD + 12, maxRecords);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> CreateCyclicRecordFile command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }   
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
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

static inline int DeleteFile(nfc_device *nfcConnDev, uint8_t fileNo) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0xdf, 0x00, 0x00, 0x01, fileNo, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> DeleteFile command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD,
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");    
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int GetFileIds(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0x6f, 0x00, 0x00, 0x00, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> GetFileIds command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD,
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int GetFileSettings(nfc_device *nfcConnDev, uint8_t fileNo) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }    
    uint8_t CMD[] = {
        0x90, 0xf5, 0x00, 0x00, 0x01, fileNo, 0x00 
    };   
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> GetFileSettings command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }    
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, 
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");    
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }    
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }    
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }    
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int ChangeFileSettings(nfc_device *nfcConnDev) {
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    -- !! ChangeFileSettings command NOT IMPLEMENTED !!\n");
    }
    return EXIT_FAILURE;
}

static inline int ReadDataCommand(nfc_device *nfcConnDev, uint8_t fileNo, 
                                  uint16_t fileReadOffset, uint16_t readLength) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 3 + 3;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0xbd;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    CMD[6] = (uint8_t) (fileReadOffset & 0x00ff);
    CMD[7] = (uint8_t) ((fileReadOffset >> 8) & 0x00ff);
    CMD[9] = (uint8_t) (readLength & 0x00ff);
    CMD[10] = (uint8_t) ((readLength >> 8) & 0x00ff);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> ReadData command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
    if(rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    <- ");
            print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
        }
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        return EXIT_FAILURE;
    }
}

static inline int WriteDataCommand(nfc_device *nfcConnDev, uint8_t fileNo, 
                                   uint16_t writeDataOffset, uint16_t writeDataLength, 
                                   uint8_t *writeDataBuf) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    else if(writeDataLength > 52 || writeDataOffset > 52) {
        return DATA_LENGTH_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 3 + 3 + writeDataLength;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0x3d;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    Int24ToByteBuffer(CMD + 6, writeDataOffset);
    Int24ToByteBuffer(CMD + 9, writeDataLength);
    memcpy(CMD + 12, writeDataBuf, writeDataLength);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> WriteData command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
    if(rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    <- ");
            print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
        }
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        return EXIT_FAILURE;
    }
}

static inline int GetValueCommand(nfc_device *nfcConnDev, uint8_t fileNo) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }    
    uint8_t CMD[] = {
        0x90, 0x6c, 0x00, 0x00, 0x01, fileNo, 0x00 
    };   
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> GetValue command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }    
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, 
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");    
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }    
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }    
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }    
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int CreditValueFileCommand(nfc_device *nfcConnDev, uint8_t fileNo, uint32_t creditAmount) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 4;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0x0c;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    Int32ToByteBuffer(CMD + 6, creditAmount);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> Credit(ValueFile) command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
    if(rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    <- ");
            print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
        }
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        return EXIT_FAILURE;
    }
}

static inline int DebitValueFileCommand(nfc_device *nfcConnDev, uint8_t fileNo, uint32_t debitAmount) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 4;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0xdc;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    Int32ToByteBuffer(CMD + 6, debitAmount);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> Debit(ValueFile) command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
    if(rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    <- ");
            print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
        }
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        return EXIT_FAILURE;
    }
}

static inline int LimitedCreditValueFileCommand(nfc_device *nfcConnDev, uint8_t fileNo, uint32_t creditAmount) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 4;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0x1c;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    Int32ToByteBuffer(CMD + 6, creditAmount);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> LimitedCredit(ValueFile) command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
    if(rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    <- ");
            print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
        }
        return EXIT_SUCCESS;
    }
    else {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        return EXIT_FAILURE;
    }
}

static inline int CommitTransaction(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0xc7, 0x00, 0x00, 0x00, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> CommitTransaction command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD,
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");    
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int AbortTransaction(nfc_device *nfcConnDev) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0xa7, 0x00, 0x00, 0x00, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> AbortTransaction command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD,
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");    
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

static inline int ReadRecordsCommand(nfc_device *nfcConnDev, uint8_t fileNo, 
                                     uint32_t offset, uint32_t dataLength) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 3 + 3;
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0xbb;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    Int24ToByteBuffer(CMD + 6, offset);
    Int24ToByteBuffer(CMD + 9, dataLength);
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> ReadRecords command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    bool continueFrame = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
    do {
        if(continueFrame) {
            uint8_t CMDCONT[] = { 0x90, 0xaf, 0x00, 0x00, 0x00, 0x00 };
            if(PRINT_STATUS_EXCHANGE_MESSAGES) {
                fprintf(stdout, "    -> ");
                print_hex(CMDCONT, sizeof(CMDCONT));
            }       
            rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMDCONT, sizeof(CMDCONT), rxDataStorage);
        }
        if(rxDataStatus) {
            if(PRINT_STATUS_EXCHANGE_MESSAGES) {
                fprintf(stdout, "    <- ");
                print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
            }
        }
        else {
            if(PRINT_STATUS_EXCHANGE_MESSAGES) {
                fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
            }
            return EXIT_FAILURE;
        }
        continueFrame = (rxDataStorage->rxDataBuf[rxDataStorage->recvSzRx - 1] == 0xaf);
    } while(continueFrame);
    return EXIT_SUCCESS;
}

static inline int WriteRecordsCommand(nfc_device *nfcConnDev, uint8_t fileNo, 
                                      uint32_t offset, uint32_t dataLength, uint8_t *dataBuf) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    size_t cmdBufSize = 6 + 1 + 3 + 3 + MIN(dataLength, 52);
    uint8_t CMD[cmdBufSize];
    CMD[0] = 0x90;
    CMD[1] = 0x3b;
    memset(CMD + 2, 0x00, cmdBufSize - 2);
    CMD[4] = cmdBufSize - 6;
    CMD[5] = fileNo;
    Int24ToByteBuffer(CMD + 6, offset);
    Int24ToByteBuffer(CMD + 9, dataLength);
    memcpy(CMD + 12, dataBuf, MIN(dataLength, 52));
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> WriteRecords command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, cmdBufSize);
    }
    uint32_t remDataBytes = MAX(0, dataLength - 52);
    uint8_t *remDataBytesBuf = dataBuf + 52;
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    bool continueFrame = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD, cmdBufSize, rxDataStorage);
    do {
        if(continueFrame && remDataBytes > 0) {
            cmdBufSize = 6 + MIN(remDataBytes, 59);
            uint8_t CMDCONT[cmdBufSize]; 
            CMDCONT[0] = 0x90;
            CMDCONT[1] = 0xaf;
            memset(CMDCONT + 2, 0x00, cmdBufSize - 2);
            CMDCONT[4] = cmdBufSize - 6;
            memcpy(CMDCONT + 5, remDataBytesBuf, MIN(remDataBytes, 59));
            remDataBytes = MAX(0, remDataBytes - 59);
            remDataBytesBuf += 59;
            if(PRINT_STATUS_EXCHANGE_MESSAGES) {
                fprintf(stdout, "    -> ");
                print_hex(CMDCONT, cmdBufSize);
            }       
            rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMDCONT, sizeof(CMDCONT), rxDataStorage);
        }
        if(rxDataStatus) {
            if(PRINT_STATUS_EXCHANGE_MESSAGES) {
                fprintf(stdout, "    <- ");
                print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
            }
        }
        else {
            if(PRINT_STATUS_EXCHANGE_MESSAGES) {
                fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
            }
            return EXIT_FAILURE;
        }
        continueFrame = (rxDataStorage->rxDataBuf[rxDataStorage->recvSzRx - 1] == 0xaf);
    } while(continueFrame);
    return EXIT_SUCCESS;
}

static inline int ClearRecordsCommand(nfc_device *nfcConnDev, uint8_t fileNo) {
    if(nfcConnDev == NULL) {
        return INVALID_PARAMS_ERROR;
    }
    uint8_t CMD[] = {
        0x90, 0xeb, 0x00, 0x00, 0x01, fileNo, 0x00
    };
    if(PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, ">>> ClearRecords command:\n");
        fprintf(stdout, "    -> ");
        print_hex(CMD, sizeof(CMD));
    }
    RxData_t *rxDataStorage = InitRxDataStruct(MAX_FRAME_LENGTH);
    bool rxDataStatus = false;
    rxDataStatus = libnfcTransmitBytes(nfcConnDev, CMD,
                                       sizeof(CMD), rxDataStorage);
    if(rxDataStatus && PRINT_STATUS_EXCHANGE_MESSAGES) {
        fprintf(stdout, "    <- ");    
        print_hex(rxDataStorage->rxDataBuf, rxDataStorage->recvSzRx);
    }
    else if(!rxDataStatus) {
        if(PRINT_STATUS_EXCHANGE_MESSAGES) {
            fprintf(stdout, "    -- !! Unable to transfer bytes !!\n");
        }
        FreeRxDataStruct(rxDataStorage, true);
        return EXIT_FAILURE;
    }
    FreeRxDataStruct(rxDataStorage, true);
    return EXIT_SUCCESS;
}

#endif
