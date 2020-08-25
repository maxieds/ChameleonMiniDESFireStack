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
 * DESFireLogging.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_LOGGING_UTILS_H__
#define __DESFIRE_LOGGING_UTILS_H__

#include "../../Log.h"
#include "../../Common.h"
#include "DESFireFirmwareSettings.h"
#include "DESFireMemoryOperations.h"

#ifndef DESFIRE_MIN_INCOMING_LOGSIZE
     #define DESFIRE_MIN_INCOMING_LOGSIZE       (1)
#endif
#ifndef DESFIRE_MIN_OUTGOING_LOGSIZE
     #define DESFIRE_MIN_OUTGOING_LOGSIZE       (1)
#endif

INLINE void DesfireLogEntry(LogEntryEnum LogCode, void *LogDataBuffer, uint16_t BufSize) {
     if(DESFIRE_MIN_OUTGOING_LOGSIZE <= BufSize) {
          LogEntry(LogCode, LogDataBuffer, BufSize);
     }
}

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

#define DEBUG_PRINT_P(fmtStr, ...)                           ({ \
    uint8_t logLength = 0;                                      \
    do {                                                        \
        snprintf_P(__InternalStringBuffer, STRING_BUFFER_SIZE,  \
		   fmtStr, ##__VA_ARGS__);                      \
	logLength = StringLength(__InternalStringBuffer,        \
			         STRING_BUFFER_SIZE);           \
	DesfireLogEntry(LOG_ERR_DESFIRE_GENERIC_ERROR,          \
			__InternalStringBuffer, logLength);     \
    } while(0);                                                 \
    })

#define GetSourceFileLoggingData()                               ({ \
        char *strBuffer;                                            \
        do {                                                        \
	    snprintf_P(__InternalStringBuffer, STRING_BUFFER_SIZE,  \
                       PSTR("@@ LINE #%d in *%s @@"),               \
			    __LINE__, __FILE__);                    \
	    __InternalStringBuffer[STRING_BUFFER_SIZE - 1] = '\0';  \
        } while(0);                                                 \
        strBuffer = __InternalStringBuffer;                         \
        strBuffer;                                                  \
        })

#define GetSymbolNameString(symbolName)                          ({ \
        char *strBuffer;                                            \
        do {                                                        \
        strncpy_P(__InternalStringBuffer2, PSTR(#symbolName),       \
                  DATA_BUFFER_SIZE_SMALL);                          \
        } while(0);                                                 \
        strBuffer = __InternalStringBuffer2;                        \
        strBuffer;                                                  \
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

#define LogDebuggingMsg(msg)                                                         ({ \
    char *strBuffer;                                                                    \
    do {                                                                                \
         strncpy_P(__InternalStringBuffer, msg, STRING_BUFFER_SIZE);                    \
         uint8_t sbufLength = StringLength(__InternalStringBuffer, STRING_BUFFER_SIZE); \
         LogEntry(LOG_INFO_DESFIRE_OUTGOING_DATA, (void *) __InternalStringBuffer,      \
                  sbufLength);                                                          \
    } while(0);                                                                         \
    strBuffer = __InternalStringBuffer;                                                 \
    strBuffer;                                                                          \
    })

#define DesfireLogIncoming(incBuf, bitCount)                                             ({ \
    uint8_t logLength = 0;                                                                  \
    do {                                                                                    \
        logLength = BufferToHexString(__InternalStringBuffer, STRING_BUFFER_SIZE,           \
			              incBuf, (bitCount + 7) / 8);                          \
        snprintf_P(__InternalStringBuffer + logLength, PSTR(" [#=%d] <-- IN"),              \
		   bitCount);                                                               \
	logLength = StringLength(__InternalStringBuffer, STRING_BUFFER_SIZE);               \
	DesfireLogEntry(LOG_INFO_DESFIRE_INCOMING_DATA, __InternalStringBuffer, logLength); \
    } while(0);                                                                             \
    })

#define DesfireLogOutgoing(incBuf, bitCount)                                             ({ \
    uint8_t logLength = 0;                                                                  \
    do {                                                                                    \
        logLength = BufferToHexString(__InternalStringBuffer, STRING_BUFFER_SIZE,           \
			              incBuf, (bitCount + 7) / 8);                          \
        snprintf_P(__InternalStringBuffer + logLength, PSTR(" [#=%d] --> OUT"),             \
		   bitCount);                                                               \
	logLength = StringLength(__InternalStringBuffer, STRING_BUFFER_SIZE);               \
	DesfireLogEntry(LOG_INFO_DESFIRE_OUTGOING_DATA, __InternalStringBuffer, logLength); \
    } while(0);                                                                             \
    })

#define DesfireLogISOStateChange(state, logCode)                                         ({ \
    const char *stateSymbName = NULL;                                                       \
    uint8_t logLength = 0x00;                                                               \
    do {                                                                                    \
        switch(state) {                                                                     \
            case ISO14443_3A_STATE_IDLE:                                                    \
	        stateSymbName = PSTR("ISO14443_3A_STATE_IDLE");                             \
		break;                                                                      \
	    case ISO14443_3A_STATE_READY1:                                                  \
	        stateSymbName = PSTR("ISO14443_3A_STATE_READY1");                           \
		break;                                                                      \
	    case ISO14443_3A_STATE_READY2:                                                  \
	        stateSymbName = PSTR("ISO14443_3A_STATE_READY2");                           \
		break;                                                                      \
	    case ISO14443_3A_STATE_ACTIVE:                                                  \
	        stateSymbName = PSTR("ISO14443_3A_STATE_ACTIVE");                           \
		break;                                                                      \
	    case ISO14443_3A_STATE_HALT:                                                    \
	        stateSymbName = PSTR("ISO14443_3A_STATE_HALT");                             \
		break;                                                                      \
	    case ISO14443_4_STATE_EXPECT_RATS:                                              \
		stateSymbName = PSTR("ISO14443_4_STATE_EXPECT_RATS");                       \
		break;                                                                      \
	    case ISO14443_4_STATE_ACTIVE:                                                   \
		stateSymbName = PSTR("ISO14443_4_STATE_ACTIVE");                            \
		break;                                                                      \
	    default:                                                                        \
	        stateSymbName = PSTR("UNKNOWN_STATE");                                      \
		break;                                                                      \
        }                                                                                   \
        snprintf_P(__InternalStringBuffer, STRING_BUFFER_SIZE, PSTR(" => "));               \
	strcat_P(__InternalStringBuffer, stateSymbName);                                    \
        logLength = StringLength(__InternalStringBuffer, STRING_BUFFER_SIZE);               \
	DesfireLogEntry(logCode, __InternalStringBuffer, logLength);                        \
    } while(0);                                                                             \
    })

#endif
