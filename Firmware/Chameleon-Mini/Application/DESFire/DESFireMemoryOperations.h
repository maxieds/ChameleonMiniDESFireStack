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
 * DESFireMemoryOperations.h : 
 * Maxie D. Schmidt (github.com/maxieds) 
 */

#ifndef __DESFIRE_MEMORY_OPERATIONS_H__
#define __DESFIRE_MEMORY_OPERATIONS_H__

#include "DESFireFirmwareSettings.h"

/* Reserve some space on the stack (text / data segment) for intermediate 
   storage of strings and data we need to write so we do not have to rely 
   on a bothersome heap-based scheme for passing pointers to functions: */
#define DATA_BUFFER_SIZE_SMALL              (24)
#define STRING_BUFFER_SIZE                  (92)
extern volatile char __InternalStringBuffer[STRING_BUFFER_SIZE];
extern char __InternalStringBuffer2[DATA_BUFFER_SIZE_SMALL];

/*
 * EEPROM memory management routines:
 */
void ReadBlockBytes(void *Buffer, SIZET StartBlock, SIZET Count);
void WriteBlockBytes(const void *Buffer, SIZET StartBlock, SIZET Count);
void CopyBlockBytes(SIZET DestBlock, SIZET SrcBlock, SIZET Count);
void SetBlockBytes(SIZET DestBlock, BYTE InitByteValue, SIZET Count);

BYTE AllocateBlocks(BYTE BlockCount);
BYTE GetCardCapacityBlocks(void);

/* File data transfer related routines: */
void ReadDataEEPROMSource(uint8_t *Buffer, uint8_t Count);
void WriteDataEEPROMSink(uint8_t *Buffer, uint8_t Count);

#endif
