/*
The DESFire stack portion of this firmware source 
is free software written by Maxie Dion Schmidt (@maxieds): 
You can redistribute it and/or modify
it under the terms of this license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

The complete source distribution of  
this firmware is available at the following link:
https://github.com/maxieds/ChameleonMiniFirmwareDESFireStack.

Based in part on the original DESFire code created by  
@dev-zzo (GitHub handle) [Dmitry Janushkevich] available at  
https://github.com/dev-zzo/ChameleonMini/tree/desfire.

This notice must be retained at the top of all source files where indicated. 
*/

/* 
 * DESFireUtils.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifdef CONFIG_MF_DESFIRE_SUPPORT

#include "../../Terminal/Terminal.h"

#include "DESFireUtils.h" 
#include "DESFirePICCControl.h"

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

void Int32ToByteBuffer(uint8_t *byteBuffer, int32_t int32Value) {
    if(byteBuffer == NULL) {
        return;
    }
    byteBuffer[0] = (uint8_t) (int32Value & 0x000000ff);
    byteBuffer[1] = (uint8_t) ((int32Value >> 8) & 0x000000ff);
    byteBuffer[2] = (uint8_t) ((int32Value >> 16) & 0x000000ff);
    byteBuffer[3] = (uint8_t) ((int32Value >> 24) & 0x000000ff);
}

void Int24ToByteBuffer(uint8_t *byteBuffer, uint32_t int24Value) {
    if(byteBuffer == NULL) {
        return;
    }
    byteBuffer[0] = (uint8_t) (int24Value & 0x0000ff);
    byteBuffer[1] = (uint8_t) ((int24Value >> 8) & 0x0000ff);
    byteBuffer[2] = (uint8_t) ((int24Value >> 16) & 0x0000ff);
}

int32_t Int32FromByteBuffer(uint8_t *byteBuffer) {
    if(byteBuffer == NULL) {
        return 0;
    }
    int32_t b0 = byteBuffer[0];
    int32_t b1 = (byteBuffer[1] << 8) & 0x0000ff00;
    int32_t b2 = (byteBuffer[2] << 16) & 0x00ff0000;
    int32_t b3 = (byteBuffer[3] << 24) & 0xff000000;
    return b0 | b1 | b2 | b3;
}

SIZET RoundBlockSize(SIZET byteSize, SIZET blockSize) {
     if(blockSize == 0) {
          return 0;
     }
     return DESFIRE_BYTES_TO_BLOCKS(byteSize);
}

#endif /* CONFIG_MF_DESFIRE_SUPPORT */
