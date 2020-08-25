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
*/

/* 
 * DESFireFirmwareSettings.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_FIRMWARE_SETTINGS_H__
#define __DESFIRE_FIRMWARE_SETTINGS_H__

#include <stdint.h>
#include <stdbool.h>

#include "../../Common.h"

#define DESFIRE_FIRMWARE_DEBUGGING                  (1)

#define DESFIRE_FIRMWARE_BUILD_TIMESTAMP            (BUILD_DATE)
#define DESFIRE_FIRMWARE_GIT_COMMIT_ID              (COMMIT_ID)
#define DESFIRE_FIRMWARE_REVISION                   ("0.0.1")
#define DESFIRE_FIRMWARE_PICC_LAYOUT_REVISION       (0x01)

#define DESFIRE_LITTLE_ENDIAN                       (1)

#define DESFIRE_PICC_STRUCT_PACKING                 //__attribute__((aligned(1)))
#define DESFIRE_FIRMWARE_PACKING                    //__attribute__((aligned(1)))
#define DESFIRE_FIRMWARE_ALIGNAT                    //__attribute__((aligned(1)))
#define DESFIRE_PICC_ARRAY_ALIGNAT                  //__attribute__((aligned(1)))
#define DESFIRE_FIRMWARE_ARRAY_ALIGNAT              //__attribute__((aligned(1)))
#define DESFIRE_FIRMWARE_ENUM_PACKING               //__attribute__((aligned(1)))
#define DESFIRE_FIRMWARE_NOINIT                     __attribute__ ((section (".noinit")))

/* Some standard boolean interpreted and other values for types and return values: */
typedef int      BOOL;
typedef uint8_t  BYTE;
typedef uint16_t NIBBLE;
typedef uint16_t SIZET;
typedef uint32_t UINT;

#define TRUE                                         ((BOOL) 0x01)
#define FALSE                                        ((BOOL) 0x00)

#define IsTrue(rcond)                                (rcond != FALSE)
#define IsFalse(rcond)                               (rcond == FALSE)

/* Allow users to modify typically reserved and protected read-only data on the tag 
   like the manufacturer bytes and the serial number (set in Makefile)? */
#ifdef ENABLE_PERMISSIVE_DESFIRE_SETTINGS
     #define DESFIRE_ALLOW_PROTECTED_MODIFY         (1)
#else
     #define DESFIRE_ALLOW_PROTECTED_MODIFY         (0)
#endif

/* Whether to auto select application ID and file before the user (input system) 
   even invokes an ISO SELECT APPLICATION [0x00 0xa4 0x04] command? */
#define DESFIRE_LEGACY_SUPPORT                      (0)

#endif 
