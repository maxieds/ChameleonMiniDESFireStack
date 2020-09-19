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

This source code is only licensed for 
redistribution under for non-commercial users. 
All commerical use or inclusion of this 
software requires express written consent of the author (MDS). 
This restriction pertains to any binary distributions which 
are derivative works of this software.

The author is free to revoke or modify this license for future 
versions of the code at free will.
*/

/* 
 * DESFireUtils.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_UTILS_H__
#define __DESFIRE_UTILS_H__

#include <stdarg.h>

#include "DESFireFirmwareSettings.h"

void PadBuffer(BYTE *buf, SIZET startIdx, SIZET endIdx, BYTE fillByte);
void RightPadBuffer(BYTE *buf, SIZET origBufSize, SIZET padBytes, BYTE fillByte);
void LeftPadBuffer(BYTE *buf, SIZET padBytes, BYTE fillByte);

void RotateArrayRight(BYTE *srcBuf, BYTE *destBuf, SIZET bufSize);
void RotateArrayLeft(BYTE *srcBuf, BYTE *destBuf, SIZET bufSize);
void ConcatByteArrays(BYTE *arrA, SIZET arrASize, BYTE *arrB, SIZET arrBSize, BYTE *destArr);

void ShortToByteArray(SIZET shortValue, BYTE *outputArr);
void IntegerToByteArray(UINT intValue, BYTE *outputArr);
void Int32ToByteBuffer(uint8_t *byteBuffer, int32_t int32Value);
void Int24ToByteBuffer(uint8_t *byteBuffer, uint32_t int24Value);

void ByteToBinaryString(BYTE byteValue, char *destStrBuf, SIZET maxDestSize);
void ByteArrayToBinaryString(BYTE *byteBuf, SIZET byteBufSize, char *destStrBuf, SIZET maxDestSize);
void ByteArrayToAsciiString(BYTE *byteBuf, SIZET byteBufSize, char *destStrBuf, SIZET maxDestSize);
void StringArrayJoin(char **strArray, SIZET arrSize, char joinDelim, char *destStrBuf, SIZET maxDestSize);

void DebugPrintP(const char *fmt, ...);

#define DEBUG_PRINT(fmt, ...) \
    DebugPrintP(PSTR(fmt), ##__VA_ARGS__)

SIZET RoundBlockSize(SIZET byteSize, SIZET blockSize);

#define UnsignedTypeToUINT(typeValue) \
    ((UINT) typeValue)
#define ExtractLSBLE(ui) \
    ((BYTE) (((UnsignedTypeToUINT(ui) & 0xff000000) >> 24) & 0x000000ff))
#define ExtractLSBBE(ui) \
    ((BYTE) (UnsignedTypeToUINT(ui) & 0x000000ff))

#endif
