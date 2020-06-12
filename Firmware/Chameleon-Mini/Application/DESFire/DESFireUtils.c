/* 
 * DESFireUtils.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "../../Terminal/Terminal.h"
#include "DESFireUtils.h" 

void DebugPrintP(const char *fmt, ...) {
    char Format[80];
    char Buffer[80];
    va_list args;
    strcpy_P(Format, fmt);
    va_start(args, fmt);
    vsnprintf(Buffer, sizeof(Buffer), Format, args);
    va_end(args);
    TerminalSendString(Buffer);
}

SIZET RoundBlockSize(SIZET byteSize, SIZET blockSize) {
     if(blockSize == 0) {
          return 0;
     }
     SIZET baseBlockSize = byteSize / blockSize;
     if((byteSize % blockSize) != 0) {
          baseBlockSize += 1;
     }
     return baseBlockSize;
}


