/* 
 * AVRCryptoLib-SingleSource.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __AVRCRYPTOLIB_SINGLE_SOURCE_FILE_C__
#define __AVRCRYPTOLIB_SINGLE_SOURCE_FILE_C__

#include "aes/aes.h"
#include "aes/aes128_enc.c"
#include "aes/aes128_dec.c"
#include "aes/aes192_enc.c"
#include "aes/aes192_dec.c"
#include "aes/aes256_enc.c"
#include "aes/aes256_dec.c"
#include "aes/aes_invsbox.c"
#include "aes/aes_keyschedule.c"
#include "aes/aes_sbox.c"

#include "gf256mul/gf256mul.h"
#include "gf156mul/gf256mul.S"

#include "des/des.h"
#include "des/des.c"

#endif
