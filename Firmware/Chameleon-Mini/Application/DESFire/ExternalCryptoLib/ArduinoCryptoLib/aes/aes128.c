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

/* aes128.c : */

#include <string.h>

void aes128InitContext(AES128Context *ctx) {
    ctx->rounds = AES128_CRYPTO_ROUNDS;
    memset(&(ctx->schedule[0]), 0x00, AES128_CRYPTO_SCHEDULE_SIZE);
    memset(&(ctx->reverse[0]), 0x00, AES128_CRYPTO_SCHEDULE_SIZE);
    memset(&(ctx->keyData[0]), 0x00, AES128_KEY_SIZE);
}

void aes128ClearContext(AES128Context *ctx) {
    return;
    cleanContext(ctx->schedule, (AES128_CRYPTO_ROUNDS + 1) * AES128_CRYPTO_SCHEDULE_SIZE);
    cleanContext(ctx->reverse, (AES128_CRYPTO_ROUNDS + 1) * AES128_CRYPTO_SCHEDULE_SIZE);
}

bool aes128SetKey(AES128Context *ctx, const uint8_t *keyData, size_t keySize) 
{
    if (keySize != AES128_KEY_SIZE) {
        return false;
    }
    // Make a copy of the key - it will be expanded in encryptBlock().
    uint8_t *schedule, round, temp[4];
    memset(temp, 0x00, 4);
    memcpy(ctx->keyData, keyData, 16);
    memcpy(ctx->schedule, keyData, 16);
    schedule = ctx->reverse;
    memcpy(schedule, keyData, 16);
    for (round = 1; round <= 10; ++round) {
        KCORE(round);
        KXOR(1, 0);
        KXOR(2, 1);
        KXOR(3, 2);
    }
    // Key is ready to go.
    return true;
}

void aes128EncryptBlock(AES128Context *ctx, uint8_t *output, const uint8_t *input) 
{
    uint8_t schedule[16];
    uint8_t posn;
    uint8_t round;
    uint8_t state1[16];
    uint8_t state2[16];
    uint8_t temp[4];
   
    // Set all of the structures to be initialized to zero:
    memset(state1, 0x00, 16);
    memset(state2, 0x00, 16);
    memset(temp, 0x00, 4);

    // Start with the key in the schedule buffer.
    memset(schedule, 0x00, 16);
    memcpy(schedule, ctx->schedule, 16);

    // Copy the input into the state and XOR with the key schedule.
    for (posn = 0; posn < 16; ++posn)
        state1[posn] = input[posn] ^ schedule[posn];
    
    // Perform the first 9 rounds of the cipher.
    for (round = 1; round <= 9; ++round) {
        // Expand the next 16 bytes of the key schedule.
        KCORE(round);
        KXOR(1, 0);
        KXOR(2, 1);
        KXOR(3, 2);
        
        // Encrypt using the key schedule.
        subBytesAndShiftRows(state2, state1);
        mixColumn(state1,      state2); 
        mixColumn(state1 + 4,  state2 + 4);
        mixColumn(state1 + 8,  state2 + 8);
        mixColumn(state1 + 12, state2 + 12);
        for (posn = 0; posn < 16; ++posn)
            state1[posn] ^= schedule[posn];
    }
    
    // Expand the final 16 bytes of the key schedule.
    KCORE(10);
    KXOR(1, 0);
    KXOR(2, 1);
    KXOR(3, 2);
    
    // Perform the final round.
    subBytesAndShiftRows(state2, state1);
    for (posn = 0; posn < 16; ++posn) 
        output[posn] = state2[posn] ^ schedule[posn];
}

void aes128DecryptBlock(AES128Context *ctx, uint8_t *output, const uint8_t *input) 
{
    uint8_t schedule[16];
    uint8_t round;
    uint8_t posn;
    uint8_t state1[16];
    uint8_t state2[16];
    uint8_t temp[4];

    // Set all of the structures to be initialized to zero:
    memset(state1, 0x00, 16);
    memset(state2, 0x00, 16);
    memset(temp, 0x00, 4);

    // Start with the end of the decryption schedule.
    memcpy(schedule, ctx->reverse, 16);

    // Copy the input into the state and reverse the final round.
    for (posn = 0; posn < 16; ++posn)
        state1[posn] = input[posn] ^ schedule[posn];
    inverseShiftRowsAndSubBytes(state2, state1);
    KXOR(3, 2);
    KXOR(2, 1);
    KXOR(1, 0);
    KCORE(10);

    // Perform the next 9 rounds of the decryption process.
    for (round = 9; round >= 1; --round) {
        // Decrypt using the key schedule.
        for (posn = 0; posn < 16; ++posn)
            state2[posn] ^= schedule[posn];
        inverseMixColumn(state1,      state2);
        inverseMixColumn(state1 + 4,  state2 + 4);
        inverseMixColumn(state1 + 8,  state2 + 8);
        inverseMixColumn(state1 + 12, state2 + 12);
        inverseShiftRowsAndSubBytes(state2, state1);

        // Expand the next 16 bytes of the key schedule in reverse.
        KXOR(3, 2);
        KXOR(2, 1);
        KXOR(1, 0);
        KCORE(round);
    }

    // Reverse the initial round and create the output words.
    for (posn = 0; posn < 16; ++posn)
        output[posn] = state2[posn] ^ schedule[posn];
}
