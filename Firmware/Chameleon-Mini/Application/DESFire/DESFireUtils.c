/* 
 * DESFireUtils.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "../../Terminal/Terminal.h"
#include "DESFireUtils.h" 

void RotateArrayRight(BYTE *srcBuf, BYTE *destBuf, SIZET bufSize) {
     destBuf[bufSize - 1] = srcBuf[0];
     for(int bidx = 0; bidx < bufSize - 1; bidx++) {
          destBuf[bidx] = srcBuf[bidx + 1];
     }
}

void RotateArrayLeft(BYTE *srcBuf, BYTE *destBuf, SIZET bufSize) {
     for(int bidx = 1; bidx < bufSize; bidx++) {
          destBuf[bidx] = srcBuf[bidx - 1];
     }
     destBuf[0] = srcBuf[bufSize - 1];
}

void ConcatByteArrays(BYTE *arrA, SIZET arrASize, BYTE *arrB, SIZET arrBSize, BYTE *destArr) {
     memcpy(destArr, arrA, arrASize);
     memcpy(destArr + arrASize, arrB, arrBSize);
}

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

