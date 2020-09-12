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
 * DESFireCrypto.c
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include <string.h>

#include "../../Log.h"

#include "DESFireCrypto.h"
#include "DESFireInstructions.h"
#include "DESFirePICCControl.h"
#include "DESFireISO14443Support.h"
#include "DESFireStatusCodes.h"
#include "DESFireLogging.h"

CryptoKeyBufferType SessionKey = { 0 };
CryptoIVBufferType SessionIV = { 0 };
BYTE SessionIVByteSize = { 0 };

DesfireAESCryptoContext AESCryptoContext = { 0 };
uint16_t AESCryptoKeySizeBytes = 0;
DesfireAESCryptoKey AESCryptoSessionKey = { 0 };
DesfireAESCryptoKey AESCryptoIVBuffer = { 0 };
DesfireAESCryptoCMACContext AESCryptoChecksumContext = { 0 };

uint8_t Authenticated = 0x00;
uint8_t AuthenticatedWithKey = 0x00;
uint8_t AuthenticatedWithPICCMasterKey = 0x00;
uint8_t CryptoAuthMethod = CRYPTO_TYPE_ANY;
uint8_t ActiveCommMode = DESFIRE_DEFAULT_COMMS_STANDARD;

void InvalidateAuthState(BYTE keepPICCAuthData) {
     if(!keepPICCAuthData) {
          AuthenticatedWithPICCMasterKey = DESFIRE_NOT_AUTHENTICATED;
     }
     Authenticated = DESFIRE_NOT_AUTHENTICATED;
     AuthenticatedWithKey = DESFIRE_NOT_AUTHENTICATED;
     CryptoAuthMethod = CRYPTO_TYPE_ANY;
     ActiveCommMode = DESFIRE_DEFAULT_COMMS_STANDARD;
}

BYTE GetDefaultCryptoMethodKeySize(uint8_t cryptoType) {
     switch(cryptoType) {
          case CRYPTO_TYPE_2KTDEA:
               return CRYPTO_2KTDEA_KEY_SIZE;
          case CRYPTO_TYPE_3K3DES:
               return CRYPTO_3KTDEA_KEY_SIZE;
          case CRYPTO_TYPE_AES128:
               return 16;
          default:
               return 0;
     }
}

const char * GetCryptoMethodDesc(uint8_t cryptoType) {
     switch(cryptoType) {
          case CRYPTO_TYPE_2KTDEA:
               return PSTR("2KTDEA");
          case CRYPTO_TYPE_3K3DES:
               return PSTR("3K3DES");
          case CRYPTO_TYPE_AES128:
               return PSTR("AES128");
          default:
               return PSTR("");
     }
}

BYTE GetCryptoMethodCommSettings(uint8_t cryptoType) {
     switch(cryptoType) {
          case CRYPTO_TYPE_2KTDEA:
               return DESFIRE_COMMS_PLAINTEXT_MAC;
          case CRYPTO_TYPE_3K3DES:
               return DESFIRE_COMMS_CIPHERTEXT_DES;
          case CRYPTO_TYPE_AES128:
               return DESFIRE_COMMS_CIPHERTEXT_AES128;
          default:
               return DESFIRE_COMMS_PLAINTEXT;
     }
}

const char * GetCommSettingsDesc(uint8_t cryptoType) {
     switch(cryptoType) {
          case CRYPTO_TYPE_2KTDEA:
               return PSTR("PTEXT-MAC");
          case CRYPTO_TYPE_3K3DES:
               return PSTR("CTEXT-DES");
          case CRYPTO_TYPE_AES128:
               return PSTR("CTEXT-AES128-CMAC");
          default:
               return PSTR("PTEXT-DEFAULT");
     }
}


BYTE GetCryptoKeyTypeFromAuthenticateMethod(BYTE authCmdMethod) {
     switch(authCmdMethod) {
          case CMD_AUTHENTICATE_AES:
          case CMD_AUTHENTICATE_EV2_FIRST:
          case CMD_AUTHENTICATE_EV2_NONFIRST:
               return CRYPTO_TYPE_AES128;
          case CMD_AUTHENTICATE_ISO:
               return CRYPTO_TYPE_3K3DES;
          case CMD_AUTHENTICATE:
          case CMD_ISO7816_EXTERNAL_AUTHENTICATE:
          case CMD_ISO7816_INTERNAL_AUTHENTICATE:
          default:
               return CRYPTO_TYPE_2KTDEA;
     }
}

void InitAESCryptoContext(DesfireAESCryptoContext *cryptoCtx) {
     aes128InitContext(cryptoCtx);
}

void InitAESCryptoKeyData(DesfireAESCryptoKey *cryptoKeyData) {
     memset(cryptoKeyData, 0x00, sizeof(DesfireAESCryptoKey));
}

uint16_t GetPaddedBufferSize(uint16_t bufSize) {
     uint16_t spareBytes = (bufSize % CRYPTO_AES_BLOCK_SIZE);
     if(spareBytes == 0) {
          return bufSize;
     }
     return bufSize + CRYPTO_AES_BLOCK_SIZE - spareBytes;
}

// assumes the crypto context struct is uninitialized: 
uint8_t DesfireAESCryptoInit(uint8_t *initKeyBuffer, uint16_t bufSize, 
                             DesfireAESCryptoContext *cryptoCtx) {
     if(initKeyBuffer == NULL || cryptoCtx == NULL) {
          return STATUS_PARAMETER_ERROR;
     }
     InitAESCryptoContext(cryptoCtx);
     if(!aes128SetKey(cryptoCtx, initKeyBuffer, bufSize)) {
         return STATUS_WRONG_VALUE_ERROR;
     }
     return STATUS_OPERATION_OK;
}

void DesfireAESEncryptBlock(DesfireAESCryptoContext *cryptoCtx, uint8_t *plainSrcBuf, uint8_t *encDestBuf) {
    aes128EncryptBlock(cryptoCtx, plainSrcBuf, encDestBuf); 
}

void DesfireAESDecryptBlock(DesfireAESCryptoContext *cryptoCtx, uint8_t *encSrcBuf, uint8_t *plainDestBuf) {
     aes128DecryptBlock(cryptoCtx, encSrcBuf, plainDestBuf); 
}

BYTE DesfireAESEncryptBuffer(DesfireAESCryptoContext *cryptoCtx, uint8_t *plainSrcBuf, 
                             uint8_t *encDestBuf, size_t bufSize) {
     size_t bufBlocks = (bufSize + CRYPTO_AES_BLOCK_SIZE - 1) / CRYPTO_AES_BLOCK_SIZE;
     bool padLastBlock = (bufSize % CRYPTO_AES_BLOCK_SIZE) != 0;
     size_t lastBlockSize = bufSize % CRYPTO_MAX_BLOCK_SIZE;
     for(int blk = 0; blk < bufBlocks; blk++) {
          if(padLastBlock && blk + 1 == bufBlocks) {
               uint8_t lastBlockBuf[CRYPTO_AES_BLOCK_SIZE];
               memset(lastBlockBuf, 0x00, CRYPTO_AES_BLOCK_SIZE);
               memcpy(lastBlockBuf, plainSrcBuf + blk * CRYPTO_AES_BLOCK_SIZE, lastBlockSize);
               DesfireAESEncryptBlock(cryptoCtx, lastBlockBuf, 
                                      encDestBuf + blk * CRYPTO_AES_BLOCK_SIZE);
          }
          else {
               DesfireAESEncryptBlock(cryptoCtx, plainSrcBuf + blk * CRYPTO_AES_BLOCK_SIZE, 
                                      encDestBuf + blk * CRYPTO_AES_BLOCK_SIZE);
          }
     }
     return padLastBlock ? STATUS_BOUNDARY_ERROR : STATUS_OPERATION_OK;
}

BYTE DesfireAESDecryptBuffer(DesfireAESCryptoContext *cryptoCtx, uint8_t *encSrcBuf, 
                             uint8_t *plainDestBuf, size_t bufSize) {
     size_t bufBlocks = (bufSize + CRYPTO_AES_BLOCK_SIZE - 1) / CRYPTO_AES_BLOCK_SIZE;
     bool padLastBlock = (bufSize % CRYPTO_AES_BLOCK_SIZE) != 0;
     for(int blk = 0; blk < bufBlocks; blk++) {
          DesfireAESEncryptBlock(cryptoCtx, encSrcBuf + blk * CRYPTO_AES_BLOCK_SIZE, 
                                 plainDestBuf + blk * CRYPTO_AES_BLOCK_SIZE);
     }
     return padLastBlock ? STATUS_BOUNDARY_ERROR : STATUS_OPERATION_OK;
}

uint8_t TransferEncryptAESCryptoSend(uint8_t *Buffer, uint8_t Count) {
    uint8_t AvailablePlaintext = TransferState.ReadData.Encryption.AvailablePlaintext;
    uint8_t TempBuffer[(DESFIRE_MAX_PAYLOAD_AES_BLOCKS + 1) * CRYPTO_DES_BLOCK_SIZE];
    uint16_t tempBufSize = (DESFIRE_MAX_PAYLOAD_AES_BLOCKS + 1) * CRYPTO_DES_BLOCK_SIZE;
    uint16_t bufFillSize = MIN(tempBufSize, AvailablePlaintext), bufFillSize2;
    uint8_t *tempBufOffset;
    if(AvailablePlaintext) {
        /* Fill the partial block */
        memcpy(&TempBuffer[0], &TransferState.BlockBuffer[0], bufFillSize);
    }
    /* Copy fresh plaintext to the temp buffer */
    if(Count > bufFillSize && tempBufSize - bufFillSize > 0) {
         tempBufOffset = &TempBuffer[bufFillSize];
         bufFillSize2 = bufFillSize;
         bufFillSize = MIN(Count, tempBufSize - bufFillSize);
         memcpy(tempBufOffset, Buffer, bufFillSize);
         Count += bufFillSize2 + Count - bufFillSize;
    }
    uint8_t BlockCount = Count / CRYPTO_AES_BLOCK_SIZE;
    /* Stash extra plaintext for later */
    AvailablePlaintext = Count - BlockCount * CRYPTO_AES_BLOCK_SIZE;
    if (AvailablePlaintext) {
        memcpy(&TransferState.BlockBuffer[0],
               &Buffer[BlockCount * CRYPTO_AES_BLOCK_SIZE], AvailablePlaintext);
    }
    TransferState.ReadData.Encryption.AvailablePlaintext = AvailablePlaintext;
    /* Encrypt complete blocks in the buffer */
    CryptoEncryptAES_CBCSend(BlockCount, &TempBuffer[0], &Buffer[0], 
                             ExtractAESKeyBuffer(&AESCryptoIVBuffer, &AESCryptoContext), 
                             AESCryptoKeySizeBytes);
    /* Return byte count to transfer */
    return BlockCount * CRYPTO_AES_BLOCK_SIZE; 
}

uint8_t TransferEncryptAESCryptoReceive(uint8_t *Buffer, uint8_t Count) {
     LogEntry(LOG_INFO_DESFIRE_INCOMING_DATA_ENC, Buffer, Count);
     return STATUS_OPERATION_OK;
}

// This routine performs the CBC "send" mode chaining: C = E(P ^ IV); IV = C
void CryptoAES_CBCSend(uint16_t Count, void* Plaintext, void* Ciphertext, 
                       void *IV, DesfireAESCryptoContext *AESCryptoContextData, 
                       CryptoAES_CBCSpec CryptoSpec) {
    uint16_t numBlocks = CRYPTO_BYTES_TO_BLOCKS(Count, CryptoSpec.blockSize);
    uint16_t blockIndex = 0;
    uint8_t *ptBuf = (uint8_t *) Plaintext, *ctBuf = (uint8_t *) Ciphertext;
    uint8_t tempBlock[CryptoSpec.blockSize], ivBlock[CryptoSpec.blockSize];
    bool lastBlockPadding = false;
    if(numBlocks * CryptoSpec.blockSize > Count) {
         lastBlockPadding = true;
    }
    while(blockIndex < numBlocks) {
        if(blockIndex + 1 == numBlocks && lastBlockPadding) {
            uint8_t bytesInBuffer = Count - (numBlocks - 1) * CryptoSpec.blockSize;
            CryptoPaddingTDEA(ptBuf + blockIndex * CryptoSpec.blockSize, bytesInBuffer, false);
        }
        memcpy(tempBlock, ptBuf + blockIndex * CryptoSpec.blockSize, CryptoSpec.blockSize);
        memcpy(ivBlock, IV, CryptoSpec.blockSize);
        memxor(ivBlock, tempBlock, CryptoSpec.blockSize);
        CryptoSpec.cryptFunc(AESCryptoContextData, ivBlock, tempBlock);
        memcpy(IV + blockIndex * CryptoSpec.blockSize, tempBlock, CryptoSpec.blockSize);
        memcpy(Ciphertext + blockIndex * CryptoSpec.blockSize, tempBlock, CryptoSpec.blockSize);
        blockIndex++;
    }
}

// This routine performs the CBC "receive" mode chaining: C = E(P) ^ IV; IV = P
void CryptoAES_CBCRecv(uint16_t Count, void* Plaintext, void* Ciphertext,
                       void *IV, DesfireAESCryptoContext *AESCryptoContextData, 
                       CryptoAES_CBCSpec CryptoSpec) {
    uint16_t numBlocks = CRYPTO_BYTES_TO_BLOCKS(Count, CryptoSpec.blockSize);
    uint16_t blockIndex = 0;
    uint8_t *ptBuf = (uint8_t *) Plaintext, *ctBuf = (uint8_t *) Ciphertext;
    uint8_t tempBlock[CryptoSpec.blockSize], ivBlock[CryptoSpec.blockSize];
    bool lastBlockPadding = false;
    if(numBlocks * CryptoSpec.blockSize > Count) {
         lastBlockPadding = true;
    }
    while(blockIndex < numBlocks) {
        if(blockIndex + 1 == numBlocks && lastBlockPadding) {
            uint8_t bytesInBuffer = Count - (numBlocks - 1) * CryptoSpec.blockSize;
            CryptoPaddingTDEA(ptBuf + blockIndex * CryptoSpec.blockSize, bytesInBuffer, false);
        }
        memcpy(ivBlock, ptBuf + blockIndex * CryptoSpec.blockSize, CryptoSpec.blockSize);
        CryptoSpec.cryptFunc(AESCryptoContextData, ivBlock, tempBlock);
        memcpy(ivBlock, IV, CryptoSpec.blockSize);
        memxor(ivBlock, tempBlock, CryptoSpec.blockSize);
        memcpy(IV, ptBuf + blockIndex * CryptoSpec.blockSize, CryptoSpec.blockSize);
        memcpy(Ciphertext + blockIndex * CryptoSpec.blockSize, ivBlock, CryptoSpec.blockSize);
        blockIndex++;
    }
}

void CryptoEncryptAES_CBCSend(uint16_t Count, const void *PlainText, void *CipherText, 
                              void *IV, DesfireAESCryptoContext *AESCryptoContextData) {
     CryptoAES_CBCSpec CryptoSpec = {
         .cryptFunc   = &DesfireAESEncryptBlock,
         .blockSize   = CRYPTO_AES_BLOCK_SIZE
     };
     CryptoAES_CBCSend(Count, PlainText, CipherText, IV, AESCryptoContextData, CryptoSpec);
}

void CryptoDecryptAES_CBCSend(uint16_t Count, const void *PlainText, void *CipherText, 
                              void *IV, DesfireAESCryptoContext *AESCryptoContextData) {
     CryptoAES_CBCSpec CryptoSpec = {
         .cryptFunc   = &DesfireAESDecryptBlock,
         .blockSize   = CRYPTO_AES_BLOCK_SIZE
     };
     CryptoAES_CBCSend(Count, CipherText, PlainText, IV, AESCryptoContextData, CryptoSpec);
}

void CryptoEncryptAES_CBCReceive(uint16_t Count, const void *PlainText, void *CipherText, 
                                 void *IV, DesfireAESCryptoContext *AESCryptoContextData) {
     CryptoAES_CBCSpec CryptoSpec = {
         .cryptFunc   = &DesfireAESEncryptBlock,
         .blockSize   = CRYPTO_AES_BLOCK_SIZE
     };
     CryptoAES_CBCRecv(Count, PlainText, CipherText, IV, AESCryptoContextData, CryptoSpec);
}

void CryptoDecryptAES_CBCReceive(uint16_t Count, const void *PlainText, void *CipherText, 
                                 void *IV, DesfireAESCryptoContext *AESCryptoContextData) {
     CryptoAES_CBCSpec CryptoSpec = {
         .cryptFunc   = &DesfireAESDecryptBlock,
         .blockSize   = CRYPTO_AES_BLOCK_SIZE
     };
     CryptoAES_CBCRecv(Count, CipherText, PlainText, IV, AESCryptoContextData, CryptoSpec);
}


BYTE InitAESCryptoCMACContext(DesfireAESCryptoCMACContext *cmacCtx, DesfireAESCryptoContext *cryptoCtx) {
     if(cmacCtx == NULL || cryptoCtx == NULL) {
          return STATUS_PARAMETER_ERROR;
     }
     return bcal_cmac_init(cmacCtx, cryptoCtx);
}

void CalculateAESCryptoCMAC(BYTE *cmacDestBytes, const BYTE *srcBuf, SIZET bufSize, 
                            DesfireAESCryptoCMACContext *cmacCtx) {
     if(cmacDestBytes == NULL || srcBuf == NULL || cmacCtx == NULL) {
          return;
     }
     bcal_cmac(cmacDestBytes, DESFIRE_CMAC_LENGTH, srcBuf, bufSize, cmacCtx);
}

/* Checksum routines */

void TransferChecksumUpdateCRCA(const uint8_t* Buffer, uint8_t Count) {
    TransferState.Checksums.MACData.CRCA = 
                  ISO14443AUpdateCRCA(Buffer, Count, TransferState.Checksums.MACData.CRCA);
}

uint8_t TransferChecksumFinalCRCA(uint8_t* Buffer) {
    /* Copy the checksum to destination */
    memcpy(Buffer, &TransferState.Checksums.MACData.CRCA, 2);
    /* Return the checksum size */
    return 2;
}

void TransferChecksumUpdateMACTDEA(const uint8_t* Buffer, uint8_t Count) {
    uint8_t AvailablePlaintext = TransferState.Checksums.AvailablePlaintext;
    uint8_t TempBuffer[CRYPTO_DES_BLOCK_SIZE];

    if (AvailablePlaintext) {
        uint8_t TempBytes;
        /* Fill the partial block */
        TempBytes = CRYPTO_DES_BLOCK_SIZE - AvailablePlaintext;
        if (TempBytes > Count)
            TempBytes = Count;
        memcpy(&TransferState.BlockBuffer[AvailablePlaintext], &Buffer[0], TempBytes);
        Count -= TempBytes;
        Buffer += TempBytes;
        /* MAC the partial block */
        TransferState.Checksums.MACData.CryptoChecksumFunc.TDEAFunc(1, &TransferState.BlockBuffer[0], 
                                                                    &TempBuffer[0], SessionIV, SessionKey);
    }
    /* MAC complete blocks in the buffer */
    while (Count >= CRYPTO_DES_BLOCK_SIZE) {
        /* NOTE: This is block-by-block, hence slow. 
         *       See if it's better to just allocate a temp buffer large enough (64 bytes). */
        TransferState.Checksums.MACData.CryptoChecksumFunc.TDEAFunc(1, &Buffer[0], &TempBuffer[0], 
                                                                    SessionIV, SessionKey);
        Count -= CRYPTO_DES_BLOCK_SIZE;
        Buffer += CRYPTO_DES_BLOCK_SIZE;
    }
    /* Copy the new partial block */
    if (Count) {
        memcpy(&TransferState.BlockBuffer[0], &Buffer[0], Count);
    }
    TransferState.Checksums.AvailablePlaintext = Count;
}

uint8_t TransferChecksumFinalMACTDEA(uint8_t* Buffer) {
    uint8_t AvailablePlaintext = TransferState.Checksums.AvailablePlaintext;
    uint8_t TempBuffer[CRYPTO_DES_BLOCK_SIZE];

    if (AvailablePlaintext) {
        /* Apply padding */
        CryptoPaddingTDEA(&TransferState.BlockBuffer[0], AvailablePlaintext, false);
        /* MAC the partial block */
        TransferState.Checksums.MACData.CryptoChecksumFunc.TDEAFunc(1, &TransferState.BlockBuffer[0], 
                                                                    &TempBuffer[0], SessionIV, SessionKey);
        TransferState.Checksums.AvailablePlaintext = 0;
    }
    /* Copy the checksum to destination */
    memcpy(Buffer, SessionIV, 4);
    /* Return the checksum size */
    return 4;
}

/* Encryption routines */

uint8_t TransferEncryptTDEASend(uint8_t* Buffer, uint8_t Count) {
    uint8_t AvailablePlaintext = TransferState.ReadData.Encryption.AvailablePlaintext;
    uint8_t TempBuffer[(DESFIRE_MAX_PAYLOAD_TDEA_BLOCKS + 1) * CRYPTO_DES_BLOCK_SIZE];
    uint8_t BlockCount;

    if (AvailablePlaintext) {
        /* Fill the partial block */
        memcpy(&TempBuffer[0], &TransferState.BlockBuffer[0], AvailablePlaintext);
    }
    /* Copy fresh plaintext to the temp buffer */
    memcpy(&TempBuffer[AvailablePlaintext], Buffer, Count);
    Count += AvailablePlaintext;
    BlockCount = Count / CRYPTO_DES_BLOCK_SIZE;
    /* Stash extra plaintext for later */
    AvailablePlaintext = Count - BlockCount * CRYPTO_DES_BLOCK_SIZE;
    if (AvailablePlaintext) {
        memcpy(&TransferState.BlockBuffer[0], 
               &Buffer[BlockCount * CRYPTO_DES_BLOCK_SIZE], AvailablePlaintext);
    }
    TransferState.ReadData.Encryption.AvailablePlaintext = AvailablePlaintext;
    /* Encrypt complete blocks in the buffer */
    CryptoEncrypt2KTDEA_CBCSend(BlockCount, &TempBuffer[0], &Buffer[0], 
                                SessionIV, SessionKey);
    /* Return byte count to transfer */
    return BlockCount * CRYPTO_DES_BLOCK_SIZE;
}

uint8_t TransferEncryptTDEAReceive(uint8_t* Buffer, uint8_t Count) {
     LogEntry(LOG_INFO_DESFIRE_INCOMING_DATA_ENC, Buffer, Count);
     return 0;
}

void CryptoPaddingTDEA(uint8_t* Buffer, uint8_t BytesInBuffer, bool FirstPaddingBitSet)
{
    uint8_t PaddingByte = FirstPaddingBitSet << 7;
    uint8_t i;

    for (i = BytesInBuffer; i < CRYPTO_DES_BLOCK_SIZE; ++i) {
        Buffer[i] = PaddingByte;
        PaddingByte = 0x00;
    }
}

void CryptoEncrypt2KTDEA(void *Plaintext, void *Ciphertext, const uint8_t *Keys) {
    uint8_t tempBlock[CRYPTO_2KTDEA_BLOCK_SIZE]; 
    des_enc(tempBlock, Plaintext, Keys);
    des_dec(Ciphertext, tempBlock, Keys + CRYPTO_DES_KEY_SIZE);
    memcpy(tempBlock, Ciphertext, CRYPTO_2KTDEA_BLOCK_SIZE);
    des_enc(Ciphertext, tempBlock, Keys);
}

void CryptoDecrypt2KTDEA(void *Plaintext, void *Ciphertext, const uint8_t *Keys) {
    uint8_t tempBlock[CRYPTO_2KTDEA_BLOCK_SIZE]; 
    des_dec(tempBlock, Ciphertext, Keys);
    des_enc(Plaintext, tempBlock, Keys + CRYPTO_DES_KEY_SIZE);
    memcpy(tempBlock, Plaintext, CRYPTO_2KTDEA_BLOCK_SIZE);
    des_dec(Plaintext, tempBlock, Keys);
}

void CryptoEncrypt3KTDEA(void *Plaintext, void *Ciphertext, const uint8_t *Keys) {
    tdes_enc(Ciphertext, Plaintext, Keys);
}

void CryptoDecrypt3KTDEA(void *Plaintext, void *Ciphertext, const uint8_t *Keys) {
    tdes_dec(Plaintext, Ciphertext, Keys);
}

// This routine performs the CBC "send" mode chaining: C = E(P ^ IV); IV = C
void CryptoTDEA_CBCSend(uint16_t Count, void* Plaintext, void* Ciphertext, 
                        void *IV, const uint8_t* Keys, CryptoTDEA_CBCSpec CryptoSpec) {
    uint16_t numBlocks = CRYPTO_BYTES_TO_BLOCKS(Count, CryptoSpec.blockSize);
    uint16_t blockIndex = 0;
    uint8_t *ptBuf = (uint8_t *) Plaintext, *ctBuf = (uint8_t *) Ciphertext;
    uint8_t tempBlock[CryptoSpec.blockSize], ivBlock[CryptoSpec.blockSize];
    bool lastBlockPadding = false;
    if(numBlocks * CryptoSpec.blockSize > Count) {
         lastBlockPadding = true;
    }
    while(blockIndex < numBlocks) {
        if(blockIndex + 1 == numBlocks && lastBlockPadding) {
            uint8_t bytesInBuffer = Count - (numBlocks - 1) * CryptoSpec.blockSize;
            CryptoPaddingTDEA(ptBuf + blockIndex * CryptoSpec.blockSize, bytesInBuffer, false);
        }
        memcpy(tempBlock, ptBuf + blockIndex * CryptoSpec.blockSize, CryptoSpec.blockSize);
        memcpy(ivBlock, IV, CryptoSpec.blockSize);
        memxor(ivBlock, tempBlock, CryptoSpec.blockSize);
        CryptoSpec.cryptFunc(ivBlock, tempBlock, Keys);
        memcpy(IV, tempBlock, CryptoSpec.blockSize);
        memcpy(Ciphertext + blockIndex * CryptoSpec.blockSize, tempBlock, CryptoSpec.blockSize);
        blockIndex++;
    }
}

// This routine performs the CBC "receive" mode chaining: C = E(P) ^ IV; IV = P
void CryptoTDEA_CBCRecv(uint16_t Count, void* Plaintext, void* Ciphertext,
                        void *IV, const uint8_t* Keys, CryptoTDEA_CBCSpec CryptoSpec) {
    uint16_t numBlocks = CRYPTO_BYTES_TO_BLOCKS(Count, CryptoSpec.blockSize);
    uint16_t blockIndex = 0;
    uint8_t *ptBuf = (uint8_t *) Plaintext, *ctBuf = (uint8_t *) Ciphertext;
    uint8_t tempBlock[CryptoSpec.blockSize], ivBlock[CryptoSpec.blockSize];
    bool lastBlockPadding = false;
    if(numBlocks * CryptoSpec.blockSize > Count) {
         lastBlockPadding = true;
    }
    while(blockIndex < numBlocks) {
        if(blockIndex + 1 == numBlocks && lastBlockPadding) {
            uint8_t bytesInBuffer = Count - (numBlocks - 1) * CryptoSpec.blockSize;
            CryptoPaddingTDEA(ptBuf + blockIndex * CryptoSpec.blockSize, bytesInBuffer, false);
        }
        memcpy(ivBlock, ptBuf + blockIndex * CryptoSpec.blockSize, CryptoSpec.blockSize);
        CryptoSpec.cryptFunc(ivBlock, tempBlock, CryptoSpec.blockSize);
        memcpy(ivBlock, IV, CryptoSpec.blockSize);
        memxor(ivBlock, tempBlock, CryptoSpec.blockSize);
        memcpy(IV, ptBuf + blockIndex * CryptoSpec.blockSize, CryptoSpec.blockSize);
        memcpy(Ciphertext + blockIndex * CryptoSpec.blockSize, ivBlock, CryptoSpec.blockSize);
        blockIndex++;
    }
}

void CryptoEncrypt2KTDEA_CBCSend(uint16_t Count, void* Plaintext, void* Ciphertext,
                                 void *IV, const uint8_t* Keys) {
     CryptoTDEA_CBCSpec CryptoSpec = {
         .cryptFunc   = &CryptoEncrypt2KTDEA,
         .blockSize   = CRYPTO_DES_BLOCK_SIZE
     };
     CryptoTDEA_CBCSend(Count, Plaintext, Ciphertext, IV, Keys, CryptoSpec);
}

void CryptoDecrypt2KTDEA_CBCSend(uint16_t Count, void* Plaintext, void* Ciphertext,
                                 void *IV, const uint8_t* Keys) {
     CryptoTDEA_CBCSpec CryptoSpec = {
         .cryptFunc   = &CryptoDecrypt2KTDEA,
         .blockSize   = CRYPTO_DES_BLOCK_SIZE
     };
     CryptoTDEA_CBCSend(Count, Plaintext, Ciphertext, IV, Keys, CryptoSpec);
}

void CryptoEncrypt2KTDEA_CBCReceive(uint16_t Count, void* Plaintext, void* Ciphertext,
                                    void *IV, const uint8_t* Keys) {
     CryptoTDEA_CBCSpec CryptoSpec = {
         .cryptFunc   = &CryptoEncrypt2KTDEA,
         .blockSize   = CRYPTO_DES_BLOCK_SIZE
     };
     CryptoTDEA_CBCRecv(Count, Plaintext, Ciphertext, IV, Keys, CryptoSpec);
}

void CryptoDecrypt2KTDEA_CBCReceive(uint16_t Count, void* Plaintext, void* Ciphertext,
                                    void *IV, const uint8_t* Keys) {
     CryptoTDEA_CBCSpec CryptoSpec = {
         .cryptFunc   = &CryptoDecrypt2KTDEA,
         .blockSize   = CRYPTO_DES_BLOCK_SIZE
     };
     CryptoTDEA_CBCRecv(Count, Plaintext, Ciphertext, IV, Keys, CryptoSpec);
}

void CryptoEncrypt3KTDEA_CBCSend(uint16_t Count, void* Plaintext, void* Ciphertext,
                                 void *IV, const uint8_t* Keys) {
     CryptoTDEA_CBCSpec CryptoSpec = {
         .cryptFunc   = &CryptoEncrypt3KTDEA,
         .blockSize   = CRYPTO_3KTDEA_BLOCK_SIZE
     };
     CryptoTDEA_CBCSend(Count, Plaintext, Ciphertext, IV, Keys, CryptoSpec);
}

void CryptoDecrypt3KTDEA_CBCSend(uint16_t Count, void* Plaintext, void* Ciphertext,
                                 void *IV, const uint8_t* Keys) {
     CryptoTDEA_CBCSpec CryptoSpec = {
         .cryptFunc   = &CryptoDecrypt3KTDEA,
         .blockSize   = CRYPTO_3KTDEA_BLOCK_SIZE
     };
     CryptoTDEA_CBCSend(Count, Plaintext, Ciphertext, IV, Keys, CryptoSpec);
}

void CryptoEncrypt3KTDEA_CBCReceive(uint16_t Count, void* Plaintext, void* Ciphertext,
                                    void *IV, const uint8_t* Keys) {
     CryptoTDEA_CBCSpec CryptoSpec = {
         .cryptFunc   = &CryptoEncrypt3KTDEA,
         .blockSize   = CRYPTO_3KTDEA_BLOCK_SIZE
     };
     CryptoTDEA_CBCRecv(Count, Plaintext, Ciphertext, IV, Keys, CryptoSpec);
}

void CryptoDecrypt3KTDEA_CBCReceive(uint16_t Count, void* Plaintext, void* Ciphertext,
                                    void *IV, const uint8_t* Keys) {
     CryptoTDEA_CBCSpec CryptoSpec = {
         .cryptFunc   = &CryptoDecrypt3KTDEA,
         .blockSize   = CRYPTO_3KTDEA_BLOCK_SIZE
     };
     CryptoTDEA_CBCRecv(Count, Plaintext, Ciphertext, IV, Keys, CryptoSpec);
}

