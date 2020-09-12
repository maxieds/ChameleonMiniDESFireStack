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
