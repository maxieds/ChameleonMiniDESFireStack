/* 
 * DESFireLogging.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_LOGGING_UTILS_H__
#define __DESFIRE_LOGGING_UTILS_H__

#include "../../Log.h"
#include "DESFireFirmwareSettings.h"
#include "DESFireAPDU.h"

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
DESFireLoggingMode GetActiveDESFireSlotLoggingMode(int slotIndex);
BOOL StoreActiveDESFireSlotLoggingMode(int slotIndex, DESFireLoggingMode nextLogMode);
BOOL SetActiveDESFireSlotLoggingMode(int slotIndex, DESFireLoggingMode nextLogMode);

BOOL DESFireLogErrorMessage(BYTE *fmtMsg, SIZET fmtLength, ...);
BOOL DESFireLogError(SIZET swRespCodes, BYTE *bufMsg, SIZET bufSize);
BOOL DESFireLogStatus(BYTE *bufMsg, SIZET bufSize);
BOOL DESFireLogDebuggingMessage(BYTE *fmtMsg, SIZET fmtLength, ...);
BOOL DESFireLogIncomingData(BYTE *byteBuf, SIZET bufLength);
BOOL DESFireLogOutgoingData(BYTE *byteBuf, SIZET bufLength);
BOOL DESFireLogNativeCommand(APDUCommand *apduCmd);
BOOL DESFireLogISO1443Command(APDUCommand *apduCmd);
BOOL DESFireLogISO7816Command(APDUCommand *apduCmd);
BOOL DESFireLogSetProtectedData(BYTE *pdataBuf, SIZET byteBufSize);
BOOL DESFireLogPICCHardReset(BYTE *strBuf, SIZET strLength);
BOOL DESFireLogPICCSoftReset(BYTE *strBuf, SIZET strLength);

#endif