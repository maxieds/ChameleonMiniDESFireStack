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

/* aes128.c : */

void aes128InitContext(AES128Context *ctx) {
    ctx->rounds = AES128_CRYPTO_ROUNDS;
    memset(ctx->schedule, 0x00, AES128_CRYPTO_SCHEDULE_SIZE);
    memset(ctx->reverse, 0x00, AES128_CRYPTO_SCHEDULE_SIZE);
}

void aes128ClearContext(AES128Context *ctx) {
    cleanContext(ctx->schedule, (AES128_CRYPTO_ROUNDS + 1) * AES128_CRYPTO_SCHEDULE_SIZE);
    cleanContext(ctx->reverse, (AES128_CRYPTO_ROUNDS + 1) * AES128_CRYPTO_SCHEDULE_SIZE);
}

bool aes128SetKey(AES128Context *ctx, const uint8_t *keyData, size_t keySize) 
{
    uint8_t *schedule;
    uint8_t round;
    uint8_t temp[4];
    
    // Set the encryption key first.
    if(keySize != 16) {
        return false;
    }
    // Make a copy of the key - it will be expanded in encryptBlock().
    memcpy(schedule, keyData, 16);
    
    // Expand the key schedule up to the last round which gives
    // us the round keys to use for the final two rounds.  We can
    // then work backwards from there in decryptBlock().
    /*schedule = ctx->reverse;
    memcpy(schedule, keyData, AES128_CRYPTO_SCHEDULE_SIZE);
    for (round = 1; round <= AES128_CRYPTO_ROUNDS; ++round) {
        KCORE(round);
        KXOR(1, 0);
        KXOR(2, 1);
        KXOR(3, 2);
    }*/

    // Key is ready to go.
    memcpy(ctx->schedule, schedule, 16);
    return true;
}

void aes128EncryptBlock(AES128Context *ctx, const uint8_t *input, uint8_t *output) 
{
    uint8_t schedule[16];
    uint8_t posn;
    uint8_t round;
    uint8_t state1[16];
    uint8_t state2[16];
    uint8_t temp[4];
    
    // Start with the key in the schedule buffer.
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

    // Start with the end of the decryption schedule.
    //memcpy(schedule, ctx->reverse, 16);
    memcpy(schedule, ctx->schedule, 16);

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
