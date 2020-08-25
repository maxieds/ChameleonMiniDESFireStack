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
