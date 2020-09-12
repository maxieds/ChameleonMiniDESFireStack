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
