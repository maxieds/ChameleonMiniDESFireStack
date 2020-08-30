/* CryptoUtils.h */

#ifndef __CRYPTO_UTILS_H__
#define __CRYPTO_UTILS_H__

#include <string.h>
#include <stdint.h>

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>

typedef struct {
     uint8_t  *keyData;
     size_t   keySize;
     uint8_t  *ivData;
     size_t   ivSize;
} AESCryptoData_t;

static inline size_t EncryptAES128(uint8_t *inputPlainText, size_t ptSize, 
                                   uint8_t *outputCipherText, AESCryptoData_t cdata) {
    AES_KEY aesEncKey;
    AES_set_encrypt_key(cdata.keyData, 128, &aesEncKey);
    AES_encrypt(inputPlainText, outputCipherText, &aesEncKey);
    return ptSize;
}

static inline size_t DecryptAES128(uint8_t *inputCipherText, size_t ctSize, 
                                   uint8_t *outputPlainText, AESCryptoData_t cdata) {
    AES_KEY aesDecKey;
    AES_set_decrypt_key(cdata.keyData, 128, &aesDecKey);
    AES_decrypt(outputPlainText, inputCipherText, &aesDecKey);
    return ctSize;
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
