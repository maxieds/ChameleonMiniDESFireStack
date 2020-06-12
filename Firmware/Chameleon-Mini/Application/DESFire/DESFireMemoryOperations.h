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
#define MAX_DATA_BUFFER_SIZE                (64)
#define MAX_STRING_BUFFER_SIZE              (64)

extern BYTE DESFireInternalDataBuffer[MAX_DATA_BUFFER_SIZE];
extern BYTE DESFireInternalStringBuffer[MAX_STRING_BUFFER_SIZE];

/*
 * EEPROM memory management routines:
 */
void ReadBlockBytes(void *Buffer, BYTE StartBlock, SIZET Count);
void WriteBlockBytes(void *Buffer, BYTE StartBlock, SIZET Count);
void CopyBlockBytes(BYTE DestBlock, BYTE SrcBlock, SIZET Count);
BYTE AllocateBlocks(BYTE BlockCount);
BYTE GetCardCapacityBlocks(void);

/* File data transfer related routines: */
void ReadDataEEPROMSource(uint8_t *Buffer, uint8_t Count);
void WriteDataEEPROMSink(uint8_t *Buffer, uint8_t Count);

#endif
