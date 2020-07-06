/* aes128.h : Standalone C library adapted from the ArduinoCryptoLib source to 
 *            implement AES128 encryption with a small foorprint. 
 */

#ifndef __AES128_CRYPTO_H__
#define __AES128_CRYPTO_H__

#define AES128_CRYPTO_ROUNDS              (10)
#define AES128_CRYPTO_SCHEDULE_SIZE       (16)
#define AES128_BLOCK_SIZE                 (16)
#define AES128_KEY_SIZE                   (16)

typedef struct {
    uint8_t     rounds;
    uint8_t     schedule[AES128_CRYPTO_SCHEDULE_SIZE];
    uint8_t     reverse[AES128_CRYPTO_SCHEDULE_SIZE];
} AES128Context;

void aes128InitContext(AES128Context *ctx);
void aes128ClearContext(AES128Context *ctx);
bool aes128SetKey(AES128Context *ctx, const uint8_t *keyData, size_t keySize);
void aes128EncryptBlock(AES128Context *ctx, const uint8_t *ptBlockBuf, uint8_t *ctBlockBuf);
void aes128DecryptBlock(AES128Context *ctx, uint8_t *ptBlockBuf, const uint8_t *ctBlockBuf);

#endif
