/* 
 * DESFireExternalCrypto.h
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_EXTERNAL_CRYPTO_H__
#define __DESFIRE_EXTERNAL_CRYPTO_H__

#include "ExternalAVRCryptoLib/aes/aes.h"
#include "ExternalAVRCryptoLib/des/des.h"

#include "../../Common.h"
#include "DESFireFirmwareSettings.h"

#define CryptoBitsToBytes(cryptoBits) \
     (cryptoBits / BITS_PER_BYTE)

typedef struct DESFIRE_FIRMWARE_PACKING {
     union {
          aes128_ctx_t aes128Context;
          aes192_ctx_t aes192Context;
          aes256_ctx_t aes256Context;
     };
     uint16_t keySizeBytes;
} DesfireAESCryptoContext;
extern DesfireAESCryptoContext AESCryptoContext;

typedef struct DESFIRE_FIRMWARE_PACKING {
     union {
          uint8_t aes128Key[16];
          uint8_t aes192Key[24];
          uint8_t aes256Key[32];
     };
} DesfireAESCryptoKey;
extern DesfireAESCryptoKey AESCryptoKey;
extern DesfireAESCryptoKey AESCryptoRndB;
extern DesfireAESCryptoKey AESCryptoIVBuffer;

void InitAESCryptoContext(DesfireAESCryptoContext *cryptoCtx);
void InitAESCryptoKeyData(DesfireAESCryptoKey *cryptoKeyData);
uint8_t * ExtractAESKeyBuffer(DesfireAESCryptoKey *cryptoKey, DesfireAESCryptoContext *cryptoCtx);
uint16_t GetPaddedBufferSize(uint16_t bufSize);
uint8_t DesfireAESCryptoInit(uint8_t *initKeyBuffer, uint16_t bufSize, DesfireAESCryptoContext *cryptoCtx);
uint8_t DesfireAESEncryptBuffer(DesfireAESCryptoContext *cryptoCtx, uint8_t *plainSrcBuf, uint8_t *encDestBuf, uint16_t bufSize);
uint8_t DesfireAESDecryptBuffer(DesfireAESCryptoContext *cryptoCtx, uint8_t *encSrcBuf, uint8_t *plainDestBuf, uint16_t bufSize);

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
     AESAUTH_STATE_IDLE = 0,
     AESAUTH_STATE_CHALLENGEI_RECV, 
     AESAUTH_STATE_RNDB_SENT2PCD, 
     AESAUTH_STATE_CHALLENGEII_RECV,
     AESAUTH_STATE_AUTHENTICATED,
     AESAUTH_STATE_ERROR, 
     AESAUTH_STATE_HALT_RESET, 
} DesfireAESAuthState;
extern DesfireAESAuthState AESAuthState;

#define DESFIRE_MAX_PAYLOAD_AES_BLOCKS        (DESFIRE_MAX_PAYLOAD_SIZE / CRYPTO_AES_BLOCK_SIZE)

uint8_t TransferEncryptAESCryptoSend(uint8_t *Buffer, uint8_t Count);
uint8_t TransferEncryptAESCryptoReceive(uint8_t *Buffer, uint8_t Count);

typedef void (*CryptoAESCryptoCBCFuncType)(uint16_t Count, const void *PlainText, void *Ciphertext, void *IV);

extern CryptoAESCryptoCBCFuncType CryptoEncryptAES_CBCSend;
extern CryptoAESCryptoCBCFuncType CryptoDecryptAES_CBCSend;
extern CryptoAESCryptoCBCFuncType CryptoEncryptAES_CBCReceive;
extern CryptoAESCryptoCBCFuncType CryptoDecryptAES_CBCReceive;

/*
 * TODO: DES: 2KTDEA, 3KTDEA methods need to be re-worked with the AVRCrypto library sources ... 
 */

#endif
