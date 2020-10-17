/* CryptoAES128.c : Adds support for XMega HW accelerated 128-bit AES encryption 
 *                  in ECB and CBC modes. 
 *                  Based in part on the the source code for Microchip's ASF library 
 *                  available at https://github.com/avrxml/asf (see license below). 
 * Author: Maxie D. Schmidt (@maxieds) 
 */

/*****************************************************************************
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "CryptoAES128.h"

/* AES interrupt callback function pointer. */
static aes_callback_t __CryptoAESCallbackFunc = NULL;

/* Keep track of the last IV block data */
static CryptoAESBlock_t __CryptoAES_IVData = { 0 };

/* Set the last operation mode (ECB or CBC) init for the context */
static uint8_t __CryptoAESOpMode = CRYPTO_AES_CBC_MODE;

void aes_start(void) {
	AES.CTRL |= AES_START_bm;
}
void aes_software_reset(void) {
	AES.CTRL = AES_RESET_bm;
}

bool aes_is_busy(void) {
	return !(AES.STATUS & (AES_SRIF_bm | AES_ERROR_bm));
}

bool aes_is_error(void) {
	return (AES.STATUS & AES_ERROR_bm);
}

void aes_clear_interrupt_flag(void) {
	AES.STATUS |= AES_SRIF_bm;
}

void aes_clear_error_flag(void) {
	AES.STATUS |= AES_ERROR_bm;
}

void aes_configure(CryptoAESAuto_t auto_start, CryptoAESXor_t xor_mode) {
     AES.CTRL = ((uint8_t) auto_start | (uint8_t) xor_mode);
}

void aes_configure_encrypt(void) {
     AES.CTRL &= ~((uint8_t) CRYPTO_AES_PMODE_DECIPHER);
     AES.CTRL |= (uint8_t) CRYPTO_AES_PMODE_ENCIPHER;
}

void aes_configure_decrypt(void) {
     AES.CTRL &= ~((uint8_t) CRYPTO_AES_PMODE_ENCIPHER);
     AES.CTRL |= (uint8_t) CRYPTO_AES_PMODE_DECIPHER;
}

void aes_set_key(uint8_t *key_in) {
     uint8_t i;
	uint8_t *temp_key = key_in;
	for (i = 0; i < CRYPTO_AES_KEY_SIZE; i++) {
		AES.KEY = *(temp_key++);
	}
}

void aes_get_key(uint8_t *key_out) {
     uint8_t i;
	uint8_t *temp_key = key_out;
	for (i = 0; i < CRYPTO_AES_KEY_SIZE; i++) {
		*(temp_key++) = AES.KEY;
	}
}

void aes_write_inputdata(uint8_t *data_in) {
     uint8_t i;
	uint8_t *temp_state = data_in;
	for (i = 0; i < CRYPTO_AES_BLOCK_SIZE; i++) {
		AES.STATE = *(temp_state++);
	}
}

void aes_read_outputdata(uint8_t *data_out) {
     uint8_t i;
	uint8_t *temp_state = data_out;
	for (i = 0; i < CRYPTO_AES_BLOCK_SIZE; i++) {
		*(temp_state++) = AES.STATE;
	}
}

void aes_isr_configure(CryptoAESIntlvl_t intlvl) {
     /* Remove pending AES interrupts. */
	AES.STATUS = (AES_ERROR_bm | AES_SRIF_bm);
	AES.INTCTRL = intlvl;
}

void aes_set_callback(const aes_callback_t callback) {
     __CryptoAESCallbackFunc = callback;
}

ISR(AES_INT_vect) {
	if (__CryptoAESCallbackFunc != NULL){
		__CryptoAESCallbackFunc();
	}
}

void CryptoAESGetConfigDefaults(CryptoAESConfig_t *ctx) {
     if(ctx == NULL) {
          return;
     }
     ctx->ProcessingMode = CRYPTO_AES_PMODE_ENCIPHER;
     ctx->ProcessingDelay = 0;
     ctx->StartMode = CRYPTO_AES_START_MODE_MANUAL;
     ctx->KeySize = CRYPTO_AES_KEY_SIZE;
     ctx->OpMode = CRYPTO_AES_CBC_MODE;
     ctx->XorMode = 0;
}

void CryptoAESInitContext(CryptoAESConfig_t *ctx) { 
     if(ctx == NULL) {
          return; 
     }
     aes_software_reset();
     memset(__CryptoAES_IVData, 0x00, CRYPTO_AES_BLOCK_SIZE);
     __CryptoAESOpMode = ctx->OpMode;
     aes_configure(ctx->StartMode, ctx->XorMode);
}

uint16_t CryptoAESGetPaddedBufferSize(uint16_t bufSize) {
     uint16_t spareBytes = (bufSize % CRYPTO_AES_BLOCK_SIZE);
     if(spareBytes == 0) {
          return bufSize;
     }   
     return bufSize + CRYPTO_AES_BLOCK_SIZE - spareBytes;
}

void CryptoAESEncryptBlock(uint8_t *Plaintext, uint8_t *Ciphertext, const uint8_t *Key) {
     aes_set_key(Key);
	aes_configure_encrypt();
     aes_write_inputdata(Plaintext);
	aes_start();
	do {
	     // Wait until AES is finished or an error occurs.
	} while (aes_is_busy());
	aes_read_outputdata(Ciphertext);
}

void CryptoAESDecryptBlock(uint8_t *Plaintext, uint8_t *Ciphertext, const uint8_t *Key) {
     aes_set_key(Key);
     aes_configure_decrypt();
     aes_write_inputdata(Ciphertext);
	aes_start();
	do {
	     // Wait until AES is finished or an error occurs.
	} while (aes_is_busy());
	aes_read_outputdata(Plaintext);
}

uint8_t CryptoAESEncryptBuffer_NoIV(uint16_t Count, uint8_t *Plaintext, uint8_t *Ciphertext, const uint8_t *Key) {
     memset(__CryptoAES_IVData, 0x00, CRYPTO_AES_BLOCK_SIZE);     
     return CryptoAESEncryptBuffer(Count, Plaintext, Ciphertext, __CryptoAES_IVData, Key);
}

uint8_t CryptoAESEncryptBuffer(uint16_t Count, uint8_t *Plaintext, uint8_t *Ciphertext,
                               const uint8_t *IV, const uint8_t *Key) {
     if((Count % CRYPTO_AES_BLOCK_SIZE) != 0) {
          return 0xBE;
     }
     size_t bufBlocks = (Count + CRYPTO_AES_BLOCK_SIZE - 1) / CRYPTO_AES_BLOCK_SIZE;
     for(int blk = 0; blk < bufBlocks; blk++) {
          if(__CryptoAESOpMode == CRYPTO_AES_CBC_MODE) {
               CryptoAESBlock_t inputBlock;
               if(blk == 0) {
                    memcpy(inputBlock, &Plaintext[0], CRYPTO_AES_BLOCK_SIZE);
                    CryptoMemoryXOR(IV, inputBlock, CRYPTO_AES_BLOCK_SIZE);
               }
               else {
                    memcpy(inputBlock, &Ciphertext[(blk - 1) * CRYPTO_AES_BLOCK_SIZE], CRYPTO_AES_BLOCK_SIZE);
                    CryptoMemoryXOR(&Plaintext[blk * CRYPTO_AES_BLOCK_SIZE], inputBlock, CRYPTO_AES_BLOCK_SIZE);
               }
               CryptoAESEncryptBlock(inputBlock, Ciphertext + blk * CRYPTO_AES_BLOCK_SIZE, Key);
          }
          else {
               CryptoAESEncryptBlock(Plaintext + blk * CRYPTO_AES_BLOCK_SIZE,
                                     Ciphertext + blk * CRYPTO_AES_BLOCK_SIZE, Key);
          }
     }
     return 0;
}

uint8_t CryptoAESDecryptBuffer_NoIV(uint16_t Count, uint8_t *Plaintext, uint8_t *Ciphertext, const uint8_t *Key) {
     memset(__CryptoAES_IVData, 0x00, CRYPTO_AES_BLOCK_SIZE);     
     return CryptoAESDecryptBuffer(Count, Plaintext, Ciphertext, __CryptoAES_IVData, Key);
}

uint8_t CryptoAESDecryptBuffer(uint16_t Count, uint8_t *Plaintext, uint8_t *Ciphertext, 
                               const uint8_t *IV, const uint8_t *Key) {
     if((Count % CRYPTO_AES_BLOCK_SIZE) != 0) {
          return 0xBE;
     }
     size_t bufBlocks = (Count + CRYPTO_AES_BLOCK_SIZE - 1) / CRYPTO_AES_BLOCK_SIZE;
     for(int blk = 0; blk < bufBlocks; blk++) {
          if(__CryptoAESOpMode == CRYPTO_AES_CBC_MODE) {
               CryptoAESBlock_t inputBlock;
               if(blk == 0) {
                    memcpy(inputBlock, &Ciphertext[0], CRYPTO_AES_BLOCK_SIZE);
                    CryptoMemoryXOR(IV, inputBlock, CRYPTO_AES_BLOCK_SIZE);
               }
               else {
                    memcpy(inputBlock, &Ciphertext[(blk - 1) * CRYPTO_AES_BLOCK_SIZE], CRYPTO_AES_BLOCK_SIZE);
                    CryptoMemoryXOR(&Ciphertext[blk * CRYPTO_AES_BLOCK_SIZE], inputBlock, CRYPTO_AES_BLOCK_SIZE);
               }
               CryptoAESDecryptBlock(inputBlock, Plaintext + blk * CRYPTO_AES_BLOCK_SIZE, Key);
          }
          else {
               CryptoAESDecryptBlock(Plaintext + blk * CRYPTO_AES_BLOCK_SIZE,
                                     Ciphertext + blk * CRYPTO_AES_BLOCK_SIZE, Key);
          }
     }
     return 0;
}

// This routine performs the CBC "send" mode chaining: C = E(P ^ IV); IV = C
void CryptoAES_CBCSend(uint16_t Count, void* Plaintext, void* Ciphertext,
                       uint8_t *IV, uint8_t *Key,
                       CryptoAES_CBCSpec_t CryptoSpec) {
    uint16_t numBlocks = CRYPTO_BYTES_TO_BLOCKS(Count, CryptoSpec.blockSize);
    uint16_t blockIndex = 0;
    uint8_t *ptBuf = (uint8_t *) Plaintext, *ctBuf = (uint8_t *) Ciphertext;
    uint8_t tempBlock[CryptoSpec.blockSize], ivBlock[CryptoSpec.blockSize];
    bool lastBlockPadding = false;
    if(numBlocks * CryptoSpec.blockSize > Count) {
         lastBlockPadding = true;
    }
    while(blockIndex < numBlocks) {
        if(blockIndex + 1 == numBlocks && lastBlockPadding) {
            return;
        }
        memcpy(tempBlock, ptBuf + blockIndex * CryptoSpec.blockSize, CryptoSpec.blockSize);
        memcpy(ivBlock, IV, CryptoSpec.blockSize);
        CryptoMemoryXOR(ivBlock, tempBlock, CryptoSpec.blockSize);
        CryptoSpec.cryptFunc(ivBlock, tempBlock, Key);
        memcpy(IV + blockIndex * CryptoSpec.blockSize, tempBlock, CryptoSpec.blockSize);
        memcpy(Ciphertext + blockIndex * CryptoSpec.blockSize, tempBlock, CryptoSpec.blockSize);
        blockIndex++;
    }
}

// This routine performs the CBC "receive" mode chaining: C = E(P) ^ IV; IV = P
void CryptoAES_CBCRecv(uint16_t Count, void* Plaintext, void* Ciphertext,
                       uint8_t *IV, uint8_t *Key,
                       CryptoAES_CBCSpec_t CryptoSpec) {
    uint16_t numBlocks = CRYPTO_BYTES_TO_BLOCKS(Count, CryptoSpec.blockSize);
    uint16_t blockIndex = 0;
    uint8_t *ptBuf = (uint8_t *) Plaintext, *ctBuf = (uint8_t *) Ciphertext;
    uint8_t tempBlock[CryptoSpec.blockSize], ivBlock[CryptoSpec.blockSize];
    bool lastBlockPadding = false;
    if(numBlocks * CryptoSpec.blockSize > Count) {
         lastBlockPadding = true;
    }
    while(blockIndex < numBlocks) {
        if(blockIndex + 1 == numBlocks && lastBlockPadding) {
             return;
        }
        memcpy(ivBlock, ptBuf + blockIndex * CryptoSpec.blockSize, CryptoSpec.blockSize);
        CryptoSpec.cryptFunc(ivBlock, tempBlock, Key);
        memcpy(ivBlock, IV, CryptoSpec.blockSize);
        CryptoMemoryXOR(ivBlock, tempBlock, CryptoSpec.blockSize);
        memcpy(IV, ptBuf + blockIndex * CryptoSpec.blockSize, CryptoSpec.blockSize);
        memcpy(Ciphertext + blockIndex * CryptoSpec.blockSize, ivBlock, CryptoSpec.blockSize);
        blockIndex++;
    }
}

void CryptoAESEncrypt_CBCSend(uint16_t Count, uint8_t *PlainText, uint8_t *CipherText,
                              uint8_t *Key, uint8_t *IV) {
     CryptoAES_CBCSpec_t CryptoSpec = { 
         .cryptFunc   = &CryptoAESEncryptBlock,
         .blockSize   = CRYPTO_AES_BLOCK_SIZE
     };  
     CryptoAES_CBCSend(Count, PlainText, CipherText, IV, Key, CryptoSpec);
}

void CryptoAESDecrypt_CBCSend(uint16_t Count, uint8_t *PlainText, uint8_t *CipherText,
                              uint8_t *Key, uint8_t *IV) {
     CryptoAES_CBCSpec_t CryptoSpec = { 
         .cryptFunc   = &CryptoAESDecryptBlock,
         .blockSize   = CRYPTO_AES_BLOCK_SIZE
     };  
     CryptoAES_CBCSend(Count, PlainText, CipherText, IV, Key, CryptoSpec);
}

void CryptoAESEncrypt_CBCReceive(uint16_t Count, uint8_t *PlainText, uint8_t *CipherText,
                                 uint8_t *Key, uint8_t *IV) {
     CryptoAES_CBCSpec_t CryptoSpec = { 
         .cryptFunc   = &CryptoAESEncryptBlock,
         .blockSize   = CRYPTO_AES_BLOCK_SIZE
     };  
     CryptoAES_CBCRecv(Count, PlainText, CipherText, IV, Key, CryptoSpec);
}

void CryptoAESDecrypt_CBCReceive(uint16_t Count, uint8_t *PlainText, uint8_t *CipherText,
                                 uint8_t *Key, uint8_t *IV) {
     CryptoAES_CBCSpec_t CryptoSpec = { 
         .cryptFunc   = &CryptoAESDecryptBlock,
         .blockSize   = CRYPTO_AES_BLOCK_SIZE
     };  
     CryptoAES_CBCRecv(Count, PlainText, CipherText, IV, Key, CryptoSpec);
}
