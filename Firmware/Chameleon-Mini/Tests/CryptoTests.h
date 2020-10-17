/* CryptoTests.h */

#ifdef ENABLE_CRYPTO_TESTS

#ifndef __CRYPTO_TESTS_H__
#define __CRYPTO_TESTS_H__

#include "../Common.h"
#include "../Terminal/Terminal.h"
#include "../Application/CryptoTDEA.h"
#include "../Application/CryptoAES128.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

/* DES crypto test cases: */

/* Test 2KTDEA encryption, ECB mode: */
static inline bool CryptoTDEATestCase1(void) {
     const uint8_t ZeroBlock[CRYPTO_DES_BLOCK_SIZE] = {
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     };
     const uint8_t TestKey2KTDEA[CRYPTO_2KTDEA_KEY_SIZE] = {
          0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
          0xcc, 0xaa, 0xff, 0xee, 0x11, 0x33, 0x33, 0x77,
     };
     const uint8_t TestOutput2KTDEAECB[CRYPTO_DES_BLOCK_SIZE] = {
          0xcb, 0x5c, 0xfc, 0xeb, 0x00, 0x25, 0x1b, 0x60,
     };
     uint8_t Output[CRYPTO_DES_BLOCK_SIZE];
     CryptoEncrypt2KTDEA(ZeroBlock, Output, TestKey2KTDEA);
     if(memcmp(TestOutput2KTDEAECB, Output, sizeof(TestOutput2KTDEAECB))) {
          return false;
     }
     return true;
}

/* Test 2KTDEA encryption, CBC receive mode: */
static inline bool CryptoTDEATestCase2(void) {
     const uint8_t TestKey2KTDEA[CRYPTO_2KTDEA_KEY_SIZE] = {
          0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
          0xcc, 0xaa, 0xff, 0xee, 0x11, 0x33, 0x33, 0x77,
     };
     const uint8_t TestInput2KTDEACBCReceive[CRYPTO_2KTDEA_KEY_SIZE] = {
          0x27, 0xd2, 0x6c, 0x67, 0xc8, 0x49, 0xe5, 0xa5,
          0x42, 0xa6, 0x8f, 0xe6, 0x82, 0x09, 0xa1, 0x1c,
     };
     const uint8_t TestOutput2KTDEACBCReceive[CRYPTO_2KTDEA_KEY_SIZE] = {
          0x13, 0x37, 0xc0, 0xde, 0xca, 0xfe, 0xba, 0xbe,
          0xde, 0xde, 0xde, 0xde, 0xad, 0xad, 0xad, 0xad,
     };
     uint8_t Output[2 * CRYPTO_DES_BLOCK_SIZE];
     uint8_t IV[CRYPTO_DES_BLOCK_SIZE];
     memset(IV, 0x00, sizeof(IV));
     CryptoEncrypt2KTDEA_CBCReceive(2, TestInput2KTDEACBCReceive, Output, IV, TestKey2KTDEA);
     if(memcmp(TestOutput2KTDEACBCReceive, Output, sizeof(TestOutput2KTDEACBCReceive))) {
          return false;
     }
     return true;
}

/* AES-128 crypto test cases: */

/* Test AES-128 encrypt/decrypt for a single block (ECB mode): */
static inline bool CryptoAESTestCase1(char *OutParam) {
     // Key from FIPS-197: 00010203 04050607 08090A0B 0C0D0E0
     const uint8_t KeyData[CRYPTO_AES_KEY_SIZE] = {
          0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
          //0x30, 0x70, 0x97, 0x1A, 0xB7, 0xCE, 0x45, 0x06,
	     //0x3F, 0xD2, 0x57, 0x3F, 0x49, 0xF5, 0x42, 0x0D
     };
     // Plaintext from FIPS-197: 00112233 44556677 8899AABB CCDDEEFF
     const uint8_t PlainText[CRYPTO_AES_BLOCK_SIZE] = {
          0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
          //0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	     //0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
     };
     // Cipher result from FIPS-197: 69c4e0d8 6a7b0430 d8cdb780 70b4c55a
     const uint8_t CipherText[CRYPTO_AES_BLOCK_SIZE] = {
          0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
		0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
          //0x59, 0x1D, 0xA5, 0xBF, 0xEA, 0x0E, 0xD7, 0x61,
	     //0x24, 0x4E, 0x81, 0xBA, 0x1E, 0xF6, 0x24, 0xB5
     };
     uint8_t tempBlock[CRYPTO_AES_BLOCK_SIZE];
     CryptoAESConfig_t aesContext;
     CryptoAESGetConfigDefaults(&aesContext);
     aesContext.OpMode = CRYPTO_AES_ECB_MODE;
     CryptoAESInitContext(&aesContext);
     CryptoAESEncryptBlock(PlainText, tempBlock, KeyData);
     if(memcmp(tempBlock, CipherText, CRYPTO_AES_BLOCK_SIZE)) {
          strcat_P(OutParam, PSTR("> ENC: "));
          OutParam += 7;
          BufferToHexString(OutParam, TERMINAL_BUFFER_SIZE - 7, tempBlock, CRYPTO_AES_BLOCK_SIZE);
          strcat_P(OutParam, PSTR("\r\n"));
          return false;
     }
     aesContext.XorMode = AES_XOR_bm;
     CryptoAESInitContext(&aesContext);
     CryptoAESDecryptBlock(tempBlock, CipherText, KeyData);
     if(memcmp(tempBlock, PlainText, CRYPTO_AES_BLOCK_SIZE)) {
          strcat_P(OutParam, PSTR("> DEC: "));
          OutParam += 7;
          BufferToHexString(OutParam, TERMINAL_BUFFER_SIZE - 7, tempBlock, CRYPTO_AES_BLOCK_SIZE);
          strcat_P(OutParam, PSTR("\r\n"));
          return false;
     }
     return true;
}

/* Test AES-128 encrypt/decrypt for a single-block buffer (ECB mode) -- Version 1: 
 * Adapted from: https://github.com/avrxml/asf/blob/master/sam/drivers/aes/example/aes_example.c
 */
static inline bool CryptoAESTestCase2(char *OutParam) {
     const uint8_t KeyData[CRYPTO_AES_KEY_SIZE] = {
	     0x16, 0x15, 0x7e, 0x2b, 0xa6, 0xd2, 0xae, 0x28,
	     0x88, 0x15, 0xf7, 0xab, 0x3c, 0x4f, 0xcf, 0x09
     };
     const uint8_t PlainText[CRYPTO_AES_BLOCK_SIZE] = {
	     0xe2, 0xbe, 0xc1, 0x6b, 0x96, 0x9f, 0x40, 0x2e,
	     0x11, 0x7e, 0x3d, 0xe9, 0x2a, 0x17, 0x93, 0x73
     };
     const uint8_t CipherText[CRYPTO_AES_BLOCK_SIZE] = {
          0xb4, 0x7b, 0xd7, 0x3a, 0x60, 0x36, 0x7a, 0x0d,
	     0xf3, 0xca, 0x9e, 0xa8, 0x97, 0xef, 0x66, 0x24
     };
     const uint8_t tempBlock[CRYPTO_AES_BLOCK_SIZE];
     CryptoAESConfig_t aesContext;
     CryptoAESGetConfigDefaults(&aesContext);
     aesContext.OpMode = CRYPTO_AES_ECB_MODE;
     CryptoAESInitContext(&aesContext);
     CryptoAESEncryptBuffer_NoIV(CRYPTO_AES_BLOCK_SIZE, PlainText, tempBlock, KeyData);
     if(memcmp(tempBlock, CipherText, CRYPTO_AES_BLOCK_SIZE)) {
          OutParam += 7;
          BufferToHexString(OutParam, TERMINAL_BUFFER_SIZE - 7, tempBlock, CRYPTO_AES_BLOCK_SIZE);
          strcat_P(OutParam, PSTR("\r\n"));
          return false;
     }
     CryptoAESInitContext(&aesContext);
     CryptoAESDecryptBuffer_NoIV(CRYPTO_AES_BLOCK_SIZE, tempBlock, CipherText, KeyData);
     if(memcmp(tempBlock, PlainText, CRYPTO_AES_BLOCK_SIZE)) {
          strcat_P(OutParam, PSTR("> DEC: "));
          OutParam += 7;
          BufferToHexString(OutParam, TERMINAL_BUFFER_SIZE - 7, tempBlock, CRYPTO_AES_BLOCK_SIZE);
          strcat_P(OutParam, PSTR("\r\n"));
          return false;
     }
     return true;
}

/* Test AES-128 encrypt/decrypt for a single-block buffer (CBC mode, with an IV) -- Version 1: 
 * Adapted from: https://github.com/avrxml/asf/blob/master/sam/drivers/aes/example/aes_example.c 
 */
static inline bool CryptoAESTestCase3(void) {
     const uint8_t KeyData[CRYPTO_AES_KEY_SIZE] = {
	     0x16, 0x15, 0x7e, 0x2b, 0xa6, 0xd2, 0xae, 0x28,
	     0x88, 0x15, 0xf7, 0xab, 0x3c, 0x4f, 0xcf, 0x09
     };
     const uint8_t PlainText[CRYPTO_AES_BLOCK_SIZE] = {
	     0xe2, 0xbe, 0xc1, 0x6b, 0x96, 0x9f, 0x40, 0x2e,
	     0x11, 0x7e, 0x3d, 0xe9, 0x2a, 0x17, 0x93, 0x73
     };
     const uint8_t CipherText[CRYPTO_AES_BLOCK_SIZE] = {
          0xac, 0xab, 0x49, 0x76, 0x46, 0xb2, 0x19, 0x81,
	     0x9b, 0x8e, 0xe9, 0xce, 0x7d, 0x19, 0xe9, 0x12     
     };
     const uint8_t IV[CRYPTO_AES_BLOCK_SIZE] = {
          0x03, 0x02, 0x01, 0x00, 0x07, 0x06, 0x05, 0x04,
	     0x0b, 0x0a, 0x09, 0x08, 0x0f, 0x0e, 0x0d, 0x0c
     };
     const uint8_t tempBlock[CRYPTO_AES_BLOCK_SIZE];
     CryptoAESConfig_t aesContext;
     CryptoAESGetConfigDefaults(&aesContext);
     aesContext.OpMode = CRYPTO_AES_CBC_MODE;
     CryptoAESInitContext(&aesContext);
     CryptoAESEncryptBuffer(CRYPTO_AES_BLOCK_SIZE, PlainText, tempBlock, IV, KeyData);
     if(memcmp(tempBlock, CipherText, CRYPTO_AES_BLOCK_SIZE)) {
          return false;
     }
     //CryptoAESDecryptBuffer(CRYPTO_AES_BLOCK_SIZE, tempBlock, CipherText, IV, KeyData);
     //if(memcmp(tempBlock, PlainText, CRYPTO_AES_BLOCK_SIZE)) {
     //     return false;
     //}
     return true;
}

#endif /* __CRYPTO_TESTS_H__ */

#endif /* ENABLE_CRYPTO_TESTS */
