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
     char *bigDataBuffer = (char *) __InternalStringBuffer;
     snprintf_P(bigDataBuffer, STRING_BUFFER_SIZE, PSTR("%s: %s"), 
                implNoteMsg, srcFileLoggingData);
     SIZET logMsgBufferSize = StringLength(bigDataBuffer, STRING_BUFFER_SIZE);
     LogEntry(LOG_INFO_DESFIRE_DEBUGGING_OUTPUT, bigDataBuffer, logMsgBufferSize + 1);
}

