/* CryptoUtils.h */

#ifndef __CRYPTO_UTILS_H__
#define __CRYPTO_UTILS_H__

#include <string.h>
#include <stdint.h>

#include <openssl/rand.h>

#include <ArduinoCryptoLib-SingleSource.c>
#include <memxor/memxor_c.c>

#define DESFIRE_CRYPTO_AUTHTYPE_AES128      (1)
#define DESFIRE_CRYPTO_AUTHTYPE_ISODES      (2)
#define DESFIRE_CRYPTO_AUTHTYPE_LEGACY      (3)

#define  AES128_BLOCK_SIZE                  (16)

static const inline uint8_t ZERO_KEY[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const inline uint8_t TEST_KEY1[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

static const inline uint8_t TEST_KEY1_INDEX = 0x01;

typedef AES128Context DesfireAESCryptoContext;

typedef struct {
     uint8_t  *keyData;
     size_t   keySize;
     uint8_t  *ivData;
     size_t   ivSize;
} AESCryptoData_t;

static inline size_t CryptAES128(bool toEncrypt, uint8_t *inputBytes, uint8_t *outputBytes, 
                                 size_t numBytes, AESCryptoData_t cdata) { 
     if((numBytes % AES128_BLOCK_SIZE) != 0) {
         return 0;
     }
     size_t numBlocks = numBytes / AES128_BLOCK_SIZE;
     size_t outputBufLength = 0, outputBufLengthTemp = 0;
     EVP_CIPHER_CTX *cipherCtx = EVP_CIPHER_CTX_new();
     EVP_CipherInit(cipherCtx, EVP_aes_128_cbc(), cdata.keyData, cdata.ivData, toEncrypt);
     for(int blk = 0; blk < numBlocks; blk++) {
         EVP_CipherUpdate(cipherCtx, outputBytes + blk * AES128_BLOCK_SIZE, 
                          &outputBufLengthTemp, inputBytes + blk * AES128_BLOCK_SIZE, 
                          AES128_BLOCK_SIZE);
         outputBufLength += outputBufLengthTemp;
     }
     EVP_CipherFinal(cipherCtx, outputBytes + outputBufLength, &outputBufLengthTemp);
     outputBufLength += outputBufLengthTemp;
     return outputBufLength;
}

static inline size_t EncryptAES128_CBC(uint8_t *inputPlainText, size_t ptSize, 
                                   uint8_t *outputCipherText, AESCryptoData_t cdata) {
    return CryptAES128(true, inputPlainText, outputCipherText, ptSize, cdata);
}

static inline size_t DecryptAES128_CBC(uint8_t *inputCipherText, size_t ctSize, 
                                   uint8_t *outputPlainText, AESCryptoData_t cdata) {
    return CryptAES128(false, inputCipherText, outputPlainText, ctSize, cdata);
}

static inline void DesfireAESCryptoInit(uint8_t *initKeyBuffer, uint16_t bufSize, 
                                        DesfireAESCryptoContext *cryptoCtx) {
     if(initKeyBuffer == NULL || cryptoCtx == NULL) {
          return;
     }   
     aes128InitContext(cryptoCtx);
     aes128SetKey(cryptoCtx, initKeyBuffer, bufSize);
}

static inline size_t EncryptAES128(uint8_t *plainSrcBuf, size_t bufSize, 
                                   uint8_t *encDestBuf, AESCryptoData_t cdata) {
     DesfireAESCryptoContext cryptoCtx;
     DesfireAESCryptoInit(cdata.keyData, cdata.keySize, &cryptoCtx);
     size_t bufBlocks = (bufSize + AES128_BLOCK_SIZE - 1) / AES128_BLOCK_SIZE;
     bool padLastBlock = (bufSize % AES128_BLOCK_SIZE) != 0;
     size_t lastBlockSize = bufSize % AES128_BLOCK_SIZE;
     for(int blk = 0; blk < bufBlocks; blk++) {
          if(padLastBlock && blk + 1 == bufBlocks) {
               uint8_t lastBlockBuf[AES128_BLOCK_SIZE];
               memset(lastBlockBuf, 0x00, AES128_BLOCK_SIZE);
               memcpy(lastBlockBuf, plainSrcBuf + blk * AES128_BLOCK_SIZE, lastBlockSize);
               aes128EncryptBlock(&cryptoCtx, lastBlockBuf, 
                                  encDestBuf + blk * AES128_BLOCK_SIZE);
          }   
          else {
               aes128EncryptBlock(&cryptoCtx, plainSrcBuf + blk * AES128_BLOCK_SIZE, 
                                  encDestBuf + blk * AES128_BLOCK_SIZE);
          }   
     }
     return bufSize;
}

static inline size_t DecryptAES128(uint8_t *encSrcBuf, size_t bufSize, 
                                   uint8_t *plainDestBuf, AESCryptoData_t cdata) {
     DesfireAESCryptoContext cryptoCtx;
     DesfireAESCryptoInit(cdata.keyData, cdata.keySize, &cryptoCtx);
     size_t bufBlocks = (bufSize + AES128_BLOCK_SIZE - 1) / AES128_BLOCK_SIZE;
     bool padLastBlock = (bufSize % AES128_BLOCK_SIZE) != 0;
     for(int blk = 0; blk < bufBlocks; blk++) {
          aes128DecryptBlock(&cryptoCtx, plainDestBuf + blk * AES128_BLOCK_SIZE,
                             encSrcBuf + blk * AES128_BLOCK_SIZE);
     }
     return bufSize;
}

static inline int GenerateRandomBytes(uint8_t *destBuf, size_t numBytes) {
     return RAND_pseudo_bytes(destBuf, numBytes);
}

static inline void RotateArrayRight(uint8_t *srcBuf, uint8_t *destBuf, size_t bufSize) {
     destBuf[bufSize - 1] = srcBuf[0];
     for(int bidx = 0; bidx < bufSize - 1; bidx++) {
          destBuf[bidx] = srcBuf[bidx + 1];
     }
}

static inline void RotateArrayLeft(uint8_t *srcBuf, uint8_t *destBuf, size_t bufSize) {
     for(int bidx = 1; bidx < bufSize; bidx++) {
          destBuf[bidx] = srcBuf[bidx - 1];
     }
     destBuf[0] = srcBuf[bufSize - 1];
}

static inline void ConcatByteArrays(uint8_t *arrA, size_t arrASize, 
                                    uint8_t *arrB, size_t arrBSize, uint8_t *destArr) {
     memcpy(destArr, arrA, arrASize);
     memcpy(destArr + arrASize, arrB, arrBSize);
}

#endif
