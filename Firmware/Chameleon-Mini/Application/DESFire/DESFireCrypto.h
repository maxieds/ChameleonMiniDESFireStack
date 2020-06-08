/* DESFireCrypto.h 
 * maxieds@gmail.com
 */

#ifndef __DESFIRE_CRYPTO_H__
#define __DESFIRE_CRYPTO_H__

/** Cryptography related definitions **/

/* Communication modes */
#define DESFIRE_COMMS_PLAINTEXT         0
#define DESFIRE_COMMS_PLAINTEXT_MAC     1
#define DESFIRE_COMMS_CIPHERTEXT_DES    3

extern BYTE ActiveCommMode;

#define CRYPTO_TYPE_DES         (0x00)
#define CRYPTO_TYPE_TDES        (0x1A)
#define CRYPTO_TYPE_TKTDEA      (0x1A)
#define CRYPTO_TYPE_AES         ((uint8_t) 0xAA)

/* Key sizes, in bytes */

#define CRYPTO_DES_KEY_SIZE         8 /* Bytes */
#define CRYPTO_2KTDEA_KEY_SIZE      (CRYPTO_DES_KEY_SIZE * 2)
#define CRYPTO_3KTDEA_KEY_SIZE      (CRYPTO_DES_KEY_SIZE * 3)
#define CRYPTO_AES_KEY_SIZE         (16)
#define CRYPTO_DES_BLOCK_SIZE       8 /* Bytes */

#define DESFIRE_CRYPTO_IV_SIZE             (CRYPTO_DES_BLOCK_SIZE)
#define DESFIRE_CRYPTO_SESSION_KEY_SIZE    (CRYPTO_3KTDEA_KEY_SIZE)

static inline BYTE InitialMasterKeyDataDES[CRYPTO_DES_KEY_SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static inline BYTE InitialMasterKeyDataAES[CRYPTO_AES_KEY_SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static inline BYTE InitialMasterKeyData3KTDEA[CRYPTO_AES_KEY_SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

extern uint8_t _64_BIT[8];
extern uint8_t _128_BIT[16];
extern uint8_t _192_BIT[24];

typedef struct {
     uint32_t keySize;
     uint32_t randomBlockSize;
     uint32_t blockSize;
     uint8_t  cryptoMethod;
     uint8_t *keyData;
} CryptoKey;

CryptoKey InitDESFireKey(uint32_t ksize, uint32_t rbsize, uint32_t bsize, uint8_t cmethod);
uint8_t[] GetDefaultKeyBuffer(uint8_t keyType);
void FreeKey(CryptoKey *key);

CryptoKey CreateNewSessionKey(uint8_t *arrA, uint8_t *arrB, uint8_t keyType);
uint16_t BuildSessionKey(CryptoKey *keyData, uint8_t *arrA, uint8_t *arrB);

uint8_t * CRC16(uint8_t *inputData, uint16_t dataLength);
uint8_t * CRC32(uint8_t *inputData, uint16_t dataLength);

/*
int
mifare_desfire_authenticate(FreefareTag tag, uint8_t key_no, MifareDESFireKey key)
{
    switch (key->type) {
    case MIFARE_KEY_DES:
    case MIFARE_KEY_2K3DES:
	return authenticate(tag, AUTHENTICATE_LEGACY, key_no, key);
	break;
    case MIFARE_KEY_3K3DES:
	return authenticate(tag, AUTHENTICATE_ISO, key_no, key);
	break;
    case MIFARE_KEY_AES128:
	return authenticate(tag, AUTHENTICATE_AES, key_no, key);
	break;
    }

    return -1; /* NOTREACHED */
}
*/


#endif
