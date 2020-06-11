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

extern DESFireLoggingMode localSlotLoggingMode;

DESFireLoggingMode StringToLoggingMode(BYTE *strParamBuf, SIZET strLength);
DESFireLoggingMode GetActiveDESFireSlotLoggingMode(int slotIndex);
BOOL StoreActiveDESFireSlotLoggingMode(int slotIndex, DESFireLoggingMode nextLogMode);
BOOL SetActiveDESFireSlotLoggingMode(int slotIndex, DESFireLoggingMode nextLogMode);

BOOL DESFireLogError(BYTE[2] swRespCodes, BYTE *bufMsg, SIZET bufSize);
BOOL DESFireLogError(BYTE *bufMsg, SIZET bufSize);
BOOL DESFireLogError(BYTE *fmtMsg, SIZET fmtLength, ...);
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
