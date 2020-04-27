/* DESFireUtils.h 
 * maxieds@gmail.com
 */

#ifndef __DESFIRE_UTILS_H__
#define __DESFIRE_UTILS_H__

#define MAX(x, y)      ((x) > (y) ? (x) : (y))
#define MIN(x, y)      ((x) < (y) ? (x) : (y))

uint16_t rotateLeft(uint8_t *inputData, uint8_t *outputData, uint16_t dataLength);
uint16_t rotateRight(uint8_t *inputData, uint8_t *outputData, uint16_t dataLength);
uint16_t shortToByteArray(uint16_t shortValue, uint8_t outputArr[2]);
uint16_t byteArrayToShort(uint8_t *valueArr);
uint16_t valueByteArrayToShort(uint8_t *valueByteArray);
uint16_t concatByteArray(uint8_t *arrA, uint16_t arrALength, uint8_t *arrB, uint16_t arrBLength, uint8_t *destArr);
uint16_t preparePaddedByteArray(uint8_t *arr, uint8_t *paddedOutputArr);
uint16_t removePadding(uint8_t *paddedArr, uint8_t *origDestArr);
uint16_t switchBytes(uint8_t *byteArr, uint16_t arrLength);
uint16_t subByteArray(uint8_t *inputData, uint16_t inputInit, uint16_t inputEnd, uint8_t *destArr);
// getCData
uint16_t byteArrayCompare(uint8_t *arrA, uint16_t arrALength, uint8_t *arrB, uint16_t arrBLength);
uint16_t getZeroArray(uint16_t length, uint8_t *destArr);
uint16_t xorByteArray(uint8_t *arrA, uint8_t *arrB, uint16_t arrLength);

#endif
