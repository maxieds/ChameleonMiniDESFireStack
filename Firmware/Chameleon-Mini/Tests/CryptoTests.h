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
bool CryptoTDEATestCase1(void);

/* Test 2KTDEA encryption, CBC receive mode: */
bool CryptoTDEATestCase2(void);

/* AES-128 crypto test cases: */

/* Test AES-128 encrypt/decrypt for a single block (ECB mode): */
bool CryptoAESTestCase1(char *OutParam);

/* Test AES-128 encrypt/decrypt for a single-block buffer (ECB mode) -- Version 1: 
 * Adapted from: https://github.com/avrxml/asf/blob/master/sam/drivers/aes/example/aes_example.c
 */
bool CryptoAESTestCase2(char *OutParam);

/* Test AES-128 encrypt/decrypt for a single-block buffer (CBC mode, with an IV) -- Version 1: 
 * Adapted from: https://github.com/avrxml/asf/blob/master/sam/drivers/aes/example/aes_example.c 
 */
bool CryptoAESTestCase3(void);

#endif /* __CRYPTO_TESTS_H__ */

#endif /* ENABLE_CRYPTO_TESTS */
