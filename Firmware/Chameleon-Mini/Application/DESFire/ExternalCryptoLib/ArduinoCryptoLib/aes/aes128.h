/*
The DESFire stack portion of this firmware source 
is free software written by Maxie Dion Schmidt: 
you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

The complete license provided with source distributions of this library is available at the following link:
https://github.com/maxieds/ChameleonMiniFirmwareDESFireStack

This notice must be retained at the top of all source files in the repository. 

This source code is only licensed for 
redistribution under the above GPL clause for 
non-commercial users. All commerical use or inclusion of this 
software requires express written consent of the author (MDS). 
This restriction pertains to any binary distributions which 
are derivative works of this software.
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
