/* DESFireCrypto.h 
 * maxieds@gmail.com
 */

#ifndef __DESFIRE_CRYPTO_H__
#define __DESFIRE_CRYPTO_H__

/* Cryptography related definitions */

#define CRYPTO_DES         (0x00)
#define CRYPTO_TDES        (0x1A)
#define CRYPTO_TKTDES      (0x1A)
#define CRYPTO_AES         ((uint8_t) 0xAA)

/* Key sizes, in bytes */

#define CRYPTO_DES_KEY_SIZE         8 /* Bytes */
#define CRYPTO_2KTDEA_KEY_SIZE      (CRYPTO_DES_KEY_SIZE * 2)
#define CRYPTO_3KTDEA_KEY_SIZE      (CRYPTO_DES_KEY_SIZE * 3)
#define CRYPTO_DES_BLOCK_SIZE       8 /* Bytes */

#define DESFIRE_CRYPTO_IV_SIZE             (CRYPTO_DES_BLOCK_SIZE)
#define DESFIRE_CRYPTO_SESSION_KEY_SIZE    (CRYPTO_3KTDEA_KEY_SIZE)

typedef uint8_t Desfire2KTDEAKeyType[CRYPTO_2KTDEA_KEY_SIZE];
typedef uint8_t Desfire3KTDEAKeyType[CRYPTO_3KTDEA_KEY_SIZE];

extern uint8_t _64_BIT[8];
extern uint8_t _128_BIT[16];
extern uint8_t _192_BIT[24];

typedef struct {
     uint32_t keySize;
     uint32_t randomBlockSize;
     uint32_t blockSize;
     uint8_t cryptoMethod;
     uint8_t *keyData;
} CryptoKey;

CryptoKey InitDESFireKey(uint32_t ksize, uint32_t rbsize, uint32_t bsize, uint8_t cmethod);
uint8_t[] GetDefaultKeyBuffer(uint8_t keyType);
void FreeKey(CryptoKey *key);

CryptoKey CreateNewSessionKey(uint8_t *arrA, uint8_t *arrB, uint8_t keyType);
uint16_t BuildSessionKey(CryptoKey *keyData, uint8_t *arrA, uint8_t *arrB);

uint8_t * CRC16(uint8_t *inputData, uint16_t dataLength);
uint8_t * CRC32(uint8_t *inputData, uint16_t dataLength);

#endif
