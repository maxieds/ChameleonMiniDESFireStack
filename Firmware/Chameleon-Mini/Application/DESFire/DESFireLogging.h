/* 
 * DESFireLogging.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_LOGGING_UTILS_H__
#define __DESFIRE_LOGGING_UTILS_H__

#include "../../Log.h"
#include "DESFireFirmwareSettings.h"

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
     OFF         = 0, 
     NORMAL      = 1, 
     VERBOSE     = 2, 
     DEBUGGING   = 3, 
} DESFireLoggingMode;

extern DESFireLoggingMode LocalLoggingMode;

/* 
 * This variable can be toggled to indicated whether to employ 
 * testable pseudo-randomness in the encrypted transfers. 
 * When this value id non-zero, then any random session numbers 
 * (e.g., RndA) and IV salt vectors should default back to 
 * predictable constant values for testing purposes. 
 */
extern BYTE LocalTestingMode; 

DESFireLoggingMode StringToLoggingMode(BYTE *strParamBuf, SIZET strLength);
DESFireLoggingMode GetDESFireLoggingMode(void);
void SetDESFireLoggingMode(DESFireLoggingMode nextLogMode);

void DESFireLogErrorMessage(BYTE *fmtMsg, SIZET fmtLength, ...);
void DESFireLogError(SIZET swRespCodes, BYTE *bufMsg, SIZET bufSize);
void DESFireLogStatus(BYTE *bufMsg, SIZET bufSize);
void DESFireLogDebuggingMessage(BYTE *fmtMsg, SIZET fmtLength, ...);
void DESFireLogIncomingData(BYTE *byteBuf, SIZET bufLength);
void DESFireLogOutgoingData(BYTE *byteBuf, SIZET bufLength);
void DESFireLogNativeCommand(BYTE *Buffer, SIZET ByteCount);
void DESFireLogISO1443Command(BYTE *Buffer, SIZET ByteCount);
void DESFireLogISO7816Command(BYTE *Buffer, SIZET ByteCount);
void DESFireLogSetProtectedData(BYTE *pdataBuf, SIZET byteBufSize);
void DESFireLogPICCHardReset(BYTE *strBuf, SIZET strLength);
void DESFireLogPICCSoftReset(BYTE *strBuf, SIZET strLength);

#endif
