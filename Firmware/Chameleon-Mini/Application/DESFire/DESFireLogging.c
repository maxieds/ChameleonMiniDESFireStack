/* 
 * DESFireLogging.c 
 * Maxie D. Schmidt (github.com/maxieds) 
 */

#include "../../Log.h"
#include "DESFireLogging.h" 

#ifdef DESFIRE_DEFAULT_DEBUGGING_MODE
     DESFireLoggingMode LocalLoggingMode = DESFIRE_DEFAULT_LOGGING_MODE;
#else
     DESFireLoggingMode LocalLoggingMode = DEBUGGING;
#endif

#ifdef DESFIRE_DEFAULT_TESTING_MODE
     BYTE LocalTestingMode = DESFIRE_DEFAULT_TESTING_MODE;
#else
     BYTE LocalTestingMode = 0x00;
#endif

void DESFireLogErrorMessage(char *fmtMsg, ...) {
    char Format[80];
    char Buffer[80];
    va_list args;
    strcpy_P(Format, fmtMsg);
    va_start(args, fmtMsg);
    vsnprintf(Buffer, sizeof(Buffer), Format, args);
    va_end(args);
    LogEntry(LOG_ERR_DESFIRE_GENERIC_ERROR, Buffer, StringLength(Buffer, 80) + 1);
}

void DESFireLogSourceCodeTODO(char *implNoteMsg, char *srcFileLoggingData) {
     char *bigDataBuffer = (char *) __InternalDataBuffer;
     snprintf_P(bigDataBuffer, MAX_DATA_BUFFER_SIZE, PSTR("%s: %s"), 
                implNoteMsg, srcFileLoggingData);
     SIZET logMsgBufferSize = StringLength(bigDataBuffer, MAX_DATA_BUFFER_SIZE);
     LogEntry(LOG_INFO_DESFIRE_DEBUGGING_OUTPUT, bigDataBuffer, logMsgBufferSize + 1);
}

