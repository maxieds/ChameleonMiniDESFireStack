/* 
 * AVRCryptoLib-SingleSource.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "gf256mul/gf256mul.h"

#include "aes/aes.h"
#include "aes/aes_enc.c"
#include "aes/aes_dec.c"
#include "aes/aes128_enc.c"
#include "aes/aes128_dec.c"
#include "aes/aes192_enc.c"
#include "aes/aes192_dec.c"
#include "aes/aes256_enc.c"
#include "aes/aes256_dec.c"
#include "aes/aes_keyschedule.c"
#include "aes/aes_sbox.c"
#include "aes/aes_invsbox.c"

#include "des/des.h"
#include "des/des.c"

#include "memxor/memxor.h"
#include "memxor/memxor_c.c"

#include "bcal/blockcipher_descriptor.h"
#include "bcal/keysize_descriptor.h"
#include "bcal/keysize_descriptor.c"

#include "bcal/bcal-aes.h"
#include "bcal/bcal-aes.c"
#include "bcal/bcal-basic.h"
#include "bcal/bcal-basic.c"
#include "bcal/bcal-cmac.h"
#include "bcal/bcal-cmac.c"

