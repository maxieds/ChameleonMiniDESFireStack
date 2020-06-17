/* 
 * DESFireLogging.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_LOGGING_UTILS_H__
#define __DESFIRE_LOGGING_UTILS_H__

#include "../../Log.h"
#include "DESFireFirmwareSettings.h"
#include "DESFireMemoryOperations.h"

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
 * (e.g., RndB) and IV salt vectors should default back to 
 * predictable constant values for testing purposes. 
 */
extern BYTE LocalTestingMode; 

void DESFireLogErrorMessage(char *fmtMsg, ...);
void DESFireLogStatus(BYTE *bufMsg, SIZET bufSize);
void DESFireLogDebuggingMessage(char *fmtMsg, ...);
void DESFireLogSourceCodeTODO(char *implNoteMsg, char *srcFileLoggingData);
void DESFireLogIncomingData(BYTE *byteBuf, SIZET bufLength);
void DESFireLogOutgoingData(BYTE *byteBuf, SIZET bufLength);
void DESFireLogNativeCommand(BYTE *Buffer, SIZET ByteCount);
void DESFireLogISO1443Command(BYTE *Buffer, SIZET ByteCount);
void DESFireLogISO7816Command(BYTE *Buffer, SIZET ByteCount);
void DESFireLogSetProtectedData(BYTE *pdataBuf, SIZET byteBufSize);
void DESFireLogPICCHardReset(BYTE *strBuf, SIZET strLength);
void DESFireLogPICCSoftReset(BYTE *strBuf, SIZET strLength);

#define GetSourceFileLoggingData()                           ({ \
        char *strBuffer;                                        \
        do {                                                    \
		snprintf_P(__InternalStringBuffer, STRING_BUFFER_SIZE,  \
                   PSTR("@@ LINE #%d in *%s of \"%s\" @@"),     \
			       __LINE__, __FILE__, __FUNCTION__);           \
	    __InternalStringBuffer[STRING_BUFFER_SIZE - 1] = '\0';  \
        } while(0);                                             \
        strBuffer = __InternalStringBuffer;                     \
        strBuffer;                                              \
        })

#define GetSymbolNameString(symbolName)                      ({ \
        char *strBuffer;                                        \
        do {                                                    \
        snprintf_P(__InternalStringBuffer2, STRING_BUFFER_SIZE, \
                   PSTR("%s"),                                  \
                   PSTR(#symbolName));                          \
        } while(0);                                             \
        strBuffer = __InternalStringBuffer2;                    \
        strBuffer;                                              \
        })

#define GetHexBytesString(byteArray, arrSize)                ({ \
    char *strBuffer;                                            \
    do {                                                        \
        BufferToHexString(__InternalStringBuffer,               \
                          STRING_BUFFER_SIZE,                   \
                          byteArray, arrSize);                  \
        __InternalStringBuffer[STRING_BUFFER_SIZE - 1] = '\0';  \
    } while(0);                                                 \
    strBuffer = __InternalStringBuffer;                         \
    strBuffer;                                                  \
    })

#endif
