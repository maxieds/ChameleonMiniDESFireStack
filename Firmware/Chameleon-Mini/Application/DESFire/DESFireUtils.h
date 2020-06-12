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
BOOL RotateLeft(BYTE *buf, SIZET bufSize, SIZET rotateBy);
BOOL RotateRight(BYTE *buf, SIZET bufSize, SIZET rotateBy);
BOOL XorByteArray(BYTE *arrA, BYTE *arrB, SIZET arrSize);

BOOL GetConstantArray(BYTE *buf, SIZET byteSizeToCopy, BYTE fillByte);
BOOL GetZeroArray(BYTE *destArr, SIZET byteSizeToCopy);

BOOL ShortToByteArray(SIZET shortValue, BYTE *outputArr);
BOOL IntegerToByteArray(UINT intValue, BYTE *outputArr);
BOOL ConcatByteArrays(BYTE *arrA, SIZET arrASize, BYTE *arrB, SIZET arrBSize, BYTE *destArr);
BOOL CopySubByteArray(BYTE *inputData, SIZET startIdx, SIZET byteSizeToCopy, BYTE *destArr);

SIZET ByteArrayCompare(BYTE *arrA, SIZET arrASize, BYTE *arrB, SIZET arrBSize);

BOOL ByteToBinaryString(BYTE byteValue, char *destStrBuf, SIZET maxDestSize);
BOOL ByteArrayToBinaryString(BYTE *byteBuf, SIZET byteBufSize, char *destStrBuf, SIZET maxDestSize);
BOOL ByteArrayToAsciiString(BYTE *byteBuf, SIZET byteBufSize, char *destStrBuf, SIZET maxDestSize);
BOOL StringArrayJoin(char **strArray, SIZET arrSize, char joinDelim, char *destStrBuf, SIZET maxDestSize);

// TODO: Need to account for endianness: 
BYTE ExtractByteN(UINT fullValue, BYTE byteNumber);

void DebugPrintP(const char *fmt, ...);

#define DEBUG_PRINT(fmt, ...) \
    DebugPrintP(PSTR(fmt), ##__VA_ARGS__)

SIZET RoundBlockSize(SIZET byteSize, SIZET blockSize);

#endif
