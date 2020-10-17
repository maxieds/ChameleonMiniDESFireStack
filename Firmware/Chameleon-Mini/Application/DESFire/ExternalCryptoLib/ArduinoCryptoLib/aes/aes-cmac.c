/* bcal-cmac.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* aes-cmac.c : */

#include <stdint.h>
#include <string.h>

#include "aes-cmac.h"

static __AES128Context __LocalAES128Ctx = { 0 };

static uint8_t left_shift_be_block(void* block, uint8_t blocksize_B) {
	uint8_t c1=0, c2;
	do{
		--blocksize_B;
		c2 = (((uint8_t*)block)[blocksize_B])>>7;
		(((uint8_t*)block)[blocksize_B]) <<= 1;
		(((uint8_t*)block)[blocksize_B]) |= c1;
		c1 = c2;
	} while(blocksize_B);
	return c1;
}

static const uint8_t const_128 = 0x87;
static const uint8_t const_64  = 0x1b;

void bcal_cipher_enc(const bcal_cmac_ctx_t *ctx, void *input, void *output) {
    aes128EncryptBlock(ctx->cctx, input, output);
}

void bcal_cipher_dec(const bcal_cmac_ctx_t *ctx, void *input, void *output) {
    aes128DecryptBlock(ctx->cctx, output, input);
}

uint8_t bcal_cmac_init(bcal_cmac_ctx_t* ctx, const uint8_t *Key) {
     memset(&__LocalAES128Ctx, 0x00, sizeof(__AES128Context));
     memcpy(__LocalAES128Ctx.keyData, Key, AES128_KEY_SIZE);
     ctx->cctx = &__LocalAES128Ctx;
     ctx->blocksize_B = 16;
     /* subkey computation */
     uint8_t r = const_128;
	memset(ctx->accu, 0x00, ctx->blocksize_B);
	memset(ctx->k1, 0x00, ctx->blocksize_B);
	bcal_cipher_enc(ctx, ctx->accu, ctx->k1); // TODO
	if(left_shift_be_block(ctx->k1, ctx->blocksize_B)){
		ctx->k1[ctx->blocksize_B-1] ^= r;
	}
	memcpy(ctx->k2, ctx->k1, ctx->blocksize_B);
	if(left_shift_be_block(ctx->k2, ctx->blocksize_B)){
		ctx->k2[ctx->blocksize_B-1] ^= r;
	}
	ctx->last_set=0;
	return 0;
}

void bcal_cmac_nextBlock(bcal_cmac_ctx_t* ctx, const void* block) {
    if(ctx->last_set){
		MemoryXOR(ctx->accu, ctx->lastblock, ctx->blocksize_B);
		bcal_cipher_enc(ctx, ctx->accu, ctx->accu); // TODO
	}
	memcpy(ctx->lastblock, block, ctx->blocksize_B);
	ctx->last_set=1;
}

void bcal_cmac_lastBlock(bcal_cmac_ctx_t* ctx, const void* block, uint16_t length_b) {
    uint16_t blocksize_b;
	blocksize_b = ctx->blocksize_B*8;
	while(length_b>=blocksize_b){
		bcal_cmac_nextBlock(ctx, block);
		block = (uint8_t*)block + ctx->blocksize_B;
		length_b -= blocksize_b;
	}
	if(ctx->last_set==0){
		MemoryXOR(ctx->accu, block, (length_b+7)/8);
		MemoryXOR(ctx->accu, ctx->k2, ctx->blocksize_B);
		ctx->accu[length_b/8] ^= 0x80>>(length_b&7);
	} else{
		if(length_b==0){
			MemoryXOR(ctx->accu, ctx->lastblock, ctx->blocksize_B);
			MemoryXOR(ctx->accu, ctx->k1, ctx->blocksize_B);
		} else{
			MemoryXOR(ctx->accu, ctx->lastblock, ctx->blocksize_B);
			bcal_cipher_enc(ctx, ctx->accu, ctx->accu); // TODO
			MemoryXOR(ctx->accu, block, (length_b+7)/8);
			MemoryXOR(ctx->accu, ctx->k2, ctx->blocksize_B);
			ctx->accu[length_b/8] ^= 0x80>>(length_b&7);
		}
	}
	bcal_cipher_enc(ctx, ctx->accu, ctx->accu); // TODO
}

void bcal_cmac_ctx2mac(void* dest, uint16_t length_b, const bcal_cmac_ctx_t* ctx) {
    memcpy(dest, ctx->accu, length_b/8);
	if(length_b&7){
		((uint8_t*)dest)[length_b/8] &= 0xff>>(length_b&7);
		((uint8_t*)dest)[length_b/8] |= (0xff00>>(length_b&7))&(ctx->accu[length_b/8]);
	}
}

void bcal_cmac(void* dest, uint16_t out_length_b, const void* block, uint32_t length_b, bcal_cmac_ctx_t* ctx) {
    uint16_t blocksize_b;
	blocksize_b = ctx->blocksize_B*8;
	while(length_b>blocksize_b){
		bcal_cmac_nextBlock(ctx, block);
		block = (uint8_t*)block + ctx->blocksize_B;
		length_b -= blocksize_b;
	}
	bcal_cmac_lastBlock(ctx, block, length_b);
	bcal_cmac_ctx2mac(dest, out_length_b, ctx);
}

