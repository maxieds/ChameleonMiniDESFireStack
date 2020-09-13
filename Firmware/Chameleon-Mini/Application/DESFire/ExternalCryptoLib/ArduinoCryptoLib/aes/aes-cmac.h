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

/* aes-cmac.h : */

#ifndef __AES_CRYPTO_CMAC_H__
#define __AES_CRYPTO_CMAC_H__

#include <stdlib.h>
#include <stdint.h>
#ifndef __LocalHostInstall__
     #include <avr/pgmspace.h>
#endif

typedef struct{
	AES128Context   *cctx;
	uint8_t         accu[16];
	uint8_t         k1[16];
	uint8_t         k2[16];
	uint8_t         lastblock[16];
	uint8_t         last_set;
	uint8_t         blocksize_B;
} bcal_cmac_ctx_t;

void bcal_cipher_enc(const bcal_cmac_ctx_t *ctx, void *input, void *output);
void bcal_cipher_dec(const bcal_cmac_ctx_t *ctx, void *input, void *output);

uint8_t bcal_cmac_init(bcal_cmac_ctx_t* ctx, AES128Context *aesCtx);
void bcal_cmac_nextBlock(bcal_cmac_ctx_t* ctx, const void* block);
void bcal_cmac_lastBlock(bcal_cmac_ctx_t* ctx, const void* block, uint16_t length_b);
void bcal_cmac_ctx2mac(void* dest, uint16_t length_b, const bcal_cmac_ctx_t* ctx);
void bcal_cmac(void* dest, uint16_t out_length_b, const void* block, uint32_t length_b, bcal_cmac_ctx_t* ctx);


#endif
