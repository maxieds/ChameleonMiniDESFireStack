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
#define MAX_DATA_BUFFER_SIZE                (128)
#define DATA_BUFFER_SIZE_SMALL              (64)
#define STRING_BUFFER_SIZE                  (64)
extern BYTE __InternalDataBuffer[MAX_DATA_BUFFER_SIZE];
extern char __InternalStringBuffer[STRING_BUFFER_SIZE];
extern char __InternalStringBuffer2[STRING_BUFFER_SIZE];

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
