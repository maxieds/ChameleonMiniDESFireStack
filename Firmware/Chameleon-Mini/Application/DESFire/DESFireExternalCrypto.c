/* 
 * DESFireExternalCrypto.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "DESFireExternalCrypto.h"
#include "DESFireCrypto.h"
#include "DESFireStatusCodes.h"

AESCryptoContext AESCryptoContext = { 0 };

void InitAESCryptoContext(DesfireAESCryptoContext *cryptoCtx) {
     memset(cryptoCtx, 0x00, sizeof(DesfireAESCryptoContext));
}

void InitAESCryptoKeyData(DesfireAESCryptoKey *cryptoKeyData) {
     memset(cryptoKeyData, 0x00, sizeof(DesfireAESCryptoKey));
}

uint8_t * ExtractAESKeyBuffer(DesfireAESCryptoKey *cryptoKey, DesfireAESCryptoContext *cryptoCtx) {
     if(cryptoKey == NULL || cryptoCtx == NULL) {
          return NULL;
     }
     switch(cryptoCtx->keySizeBytes) {
          case 16:
               return (uint8_t *) cryptoKey->aes128Key;
          case 24:
               return (uint8_t *) cryptoKey->aes192Key;
          case 32:
               return (uint8_t *) cryptoKey->aes256Key;
          default: 
               return NULL;
     }
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
     if(bufSize == 0 || bufSize > 32) {
          return STATUS_PARAMETER_ERROR;
     }
     uint8_t keySize;
     uint8_t *paddedKeyBuf;
     DesfireAESCryptoKey cryptoKeyBuf = { 0 };
     aes_genctx_t *aesCryptoContextType;
     if(bufSize > 24 && bufSize <= 32) {
          keySize = 32;
          paddedKeyBuf = (uint8_t *) cryptoKeyBuf.aes256Key;
          aesCryptoContextType = &(cryptoCtx->aes256Context);
     }
     else if(bufSize > 16 && bufSize <= 24) {
          keySize = 24;
          paddedKeyBuf = (uint8_t *) cryptoKeyBuf.aes192Key;
          aesCryptoContextType = &(cryptoCtx->aes192Context);
     }
     else {
          keySize = 16;
          paddedKeyBuf = (uint8_t *) cryptoKeyBuf.aes128Key;
          aesCryptoContextType = &(cryptoCtx->aes128Context);
     }
     memcpy(paddedKeyBuf, initKeyBuffer, bufSize);
     aes_init(paddedKeyBuf, keySize, aesCryptoContext);
     cryptoCtx->keySizeBytes = keySize;
     return STATUS_OPERATION_OK;
}

uint8_t DesfireAESEncryptBuffer(DesfireAESCryptoContext *cryptoCtx, uint8_t *plainSrcBuf, 
                                uint8_t *encDestBuf, uint16_t bufSize) {
     if(cryptoCtx == NULL || plainSrcBuf == NULL || encDestBuf == NULL) {
          return STATUS_PARAMETER_ERROR;
     }
     else if((bufSize % CRYPTO_AES_BLOCK_SIZE) != 0) {
          return STATUS_PARAMETER_ERROR;
     }
     uint8_t encBlockData[CRYPTO_AES_BLOCK_SIZE];
     for(int blockOffset = 0; blockOffset < bufSize / CRYPTO_AES_BLOCK_SIZE; blockOffset += CRYPTO_AES_BLOCK_SIZE) {
          memcpy(encBlockData, plainSrcBuf + blockOffset, CRYPTO_AES_BLOCK_SIZE);
          switch(cryptoCtx->keySizeBytes) {
               case 16:
                    aes128_enc(encBlockData, cryptoCtx->aes128Context);
                    break;
               case 24:
                    aes192_enc(encBlockData, cryptoCtx->aes192Context);
                    break;
               case 32:
                    aes256_enc(encBlockData, cryptoCtx->aes256Context);
                    break;
               default:
                    return STATUS_NO_SUCH_KEY;
          }
          memcpy(encDestBuf + blockOffset, encBlockData, CRYPTOAES_BLOCK_SIZE);
     }
     return STATUS_OPERATION_OK;
}

uint8_t DesfireAESDecryptBuffer(DesfireAESCryptoContext *cryptoCtx, 
                                uint8_t *encSrcBuf, uint8_t *plainDestBuf, uint16_t bufSize);



