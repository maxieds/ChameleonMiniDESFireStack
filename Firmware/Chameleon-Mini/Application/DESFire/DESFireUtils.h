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
