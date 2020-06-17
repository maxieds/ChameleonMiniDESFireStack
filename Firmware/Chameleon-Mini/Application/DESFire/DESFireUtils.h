/* 
 * DESFireUtils.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_UTILS_H__
#define __DESFIRE_UTILS_H__

#include <stdarg.h>

#include "DESFireFirmwareSettings.h"

#define MAX2(x, y)      ((x) > (y) ? (x) : (y))
#define MIN2(x, y)      ((x) < (y) ? (x) : (y))

BOOL PadBuffer(BYTE *buf, SIZET startIdx, SIZET endIdx, BYTE fillByte);
BOOL RightPadBuffer(BYTE *buf, SIZET origBufSize, SIZET padBytes, BYTE fillByte);
BOOL LeftPadBuffer(BYTE *buf, SIZET padBytes, BYTE fillByte);
BOOL ReverseBuffer(BYTE *buf, SIZET bufSize);

void RotateArrayRight(BYTE *srcBuf, BYTE *destBuf, SIZET bufSize);
void RotateArrayLeft(BYTE *srcBuf, BYTE *destBuf, SIZET bufSize);
void ConcatByteArrays(BYTE *arrA, SIZET arrASize, BYTE *arrB, SIZET arrBSize, BYTE *destArr);

BOOL ShortToByteArray(SIZET shortValue, BYTE *outputArr);
BOOL IntegerToByteArray(UINT intValue, BYTE *outputArr);
BOOL CopySubByteArray(BYTE *inputData, SIZET startIdx, SIZET byteSizeToCopy, BYTE *destArr);

BOOL ByteToBinaryString(BYTE byteValue, char *destStrBuf, SIZET maxDestSize);
BOOL ByteArrayToBinaryString(BYTE *byteBuf, SIZET byteBufSize, char *destStrBuf, SIZET maxDestSize);
BOOL ByteArrayToAsciiString(BYTE *byteBuf, SIZET byteBufSize, char *destStrBuf, SIZET maxDestSize);
BOOL StringArrayJoin(char **strArray, SIZET arrSize, char joinDelim, char *destStrBuf, SIZET maxDestSize);

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
