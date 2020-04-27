/* DESFireCrypto.c
 * maxieds@gmail.com
 */

#include <string.h>

uint8_t _64_BIT[8] = { 0x00, 0x00, 0x00, 0x00 };
uint8_t _128_BIT[16] = { 
   0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00
};
uint8_t _192_BIT[24] = {
   0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00
};

CryptoKey InitDESFireKey(uint32_t ksize, uint32_t rbsize, uint32_t bsize, uint8_t cmethod) {
     CryptoKey ckey;
     ckey.keySize = ksize;
     ckey.randomBlockSize = rbsize;
     ckey.blockSize = bsize;
     ckey.cryptoMethod = cmethod;
     ckey.keyData = (uint8_t *) malloc(ksize); // TODO
     return ckey;
}

uint8_t[] GetDefaultKeyBuffer(uint8_t keyType) {
     switch(keyType) {
          case CRYPTO_DES:
               return _64_BIT;
          case CRYPTO_AES:
               return _128_BIT;
          case CRYPTO_TDES:
               return _192_BIT;
          default:
               break;
     }
     return _192_BIT;
}

void FreeKey(CryptoKey *key) {
     if(key == NULL) {
          return;
     }
     else if(key->keyData != NULL) {
          free(key->keyData); // TODO
     }
}

CryptoKey CreateNewSessionKey(uint8_t *arrA, uint8_t *arrB, uint8_t keyType) {
     CryptoKey ckey;
     if(arrA == NULL || arrB == NULL) {
          memset(&ckey, 0x00, sizeof(CryptoKey);
          return ckey;
     }
     switch(keyType) {
          case CRYPTO_DES:
               ckey = InitDESFireKey(8, 4, CRYPTO_DES_BLOCK_SIZE, CRYPTO_DES);
               for(int i = 0; i < 4; i++) {
                    ckey->keyData[i] = arrA[i];
                    ckey->keyData[i + 4] = arrB[i];
               }
               return ckey;
          case CRYPTO_TDES:
               ckey = InitDESFireKey(16, 8, 8, CRYPTO_TDES);
               for(int i = 0; i < 4; i++) {
                    ckey->keyData[i] = arrA[i];
                    ckey->keyData[i + 4] = arrB[i];
               }
               for(int i = 4; i < 8; i++) {
                    ckey->keyData[i + 4] = arrA[i];
                    ckey->keyData[i + 8] = arrB[i];
               }
               return ckey;
          case CRYPTO_TKDES:
               ckey = InitDESFireKey(24, 16, 16, CRYPTO_TKDES);
               for(int i = 0; i < 4; i++) {
                    ckey->keyData[i] = arrA[i];
                    ckey->keyData[i + 4] = arrB[i];
               }
               for(int i = 4; i < 8; i++) {
                    ckey->keyData[i + 4] = arrA[i + 2];
                    ckey->keyData[i + 8] = arrB[i + 2];
               }
               for(int i = 12; i < 16; i++) {
                    ckey->keyData[i + 4] = arrA[i];
                    ckey->keyData[i + 8] = arrB[i];
               }
               return ckey;
          case CRYPTO_AES:
               ckey = InitDESFireKey(16, 16, 16, CRYPTO_AES);
               for(int i = 0; i < 4; i++) {
                    ckey->keyData[i] = arrA[i];
               }
               for(int i = 4; i < 8; i++) {
                    ckey->keyData[i] = arrB[i - 4];
               }
               for(int i = 8; i < 12; i++) {
                    ckey->keyData[i] = arrA[i + 4];
               }
               for(int i = 12; i < 16; i++) {
                    ckey->keyData[i] = arrB[i];
               }
               return ckey;
          default:
               break;
     }
     memset(&ckey, 0x00, sizeof(CryptoKey));
     return ckey;
}


