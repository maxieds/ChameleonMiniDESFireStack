/* ArduinoCryptoLib-SingleSource.c : Include all sources in one file. */

#ifndef __ARDUINO_CRYPTO_LIB_SINGLE_SOURCE_C__
#define __ARDUINO_CRYPTO_LIB_SINGLE_SOURCE_C__

#include "aes/aes-common.h"
#include "aes/aes128.h"
#include "aes/ProgMemUtil.h"

#include "aes/aes-common.c"
#include "aes/aes128.c"

#include "../AVRCryptoLib/memxor/memxor.h"
#include "aes/aes-cmac.h"
#include "aes/aes-cmac.c"

#endif
