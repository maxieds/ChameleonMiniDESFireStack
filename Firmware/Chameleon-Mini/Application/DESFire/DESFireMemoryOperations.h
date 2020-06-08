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
#define MAX_DATA_BUFFER_SIZE                (256)
#define MAX_STRING_BUFFER_SIZE              (256)

extern BYTE DESFireInternalDataBuffer[MAX_DATA_BUFFER_SIZE];
extern BYTE DESFireInternalStringBuffer[MAX_STRING_BUFFER_SIZE];

BOOL WriteDataToPICCImage(int slotNumber, UINT destAddr, const BYTE *dataBuf, SIZET dataSize);
BOOL ReadDataFromPICCImage(int slotNumber, UINT srcAddr, BYTE *destBuf, SIZET maxLength);

#endif
