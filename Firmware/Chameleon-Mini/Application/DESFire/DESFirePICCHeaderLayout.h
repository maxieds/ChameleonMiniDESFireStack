/* 
 * DESFirePICCHeaderLayout.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_PICC_HDRLAYOUT_H__
#define __DESFIRE_PICC_HDRLAYOUT_H__

#include "DESFireFirmwareSettings.h"

static inline const BYTE PICC_FORMATTED_MARKER[] = { 0xf0, 0x12, 0x34 };
static inline const BYTE DefaultDESFireATS[] = { 0x06, 0x75, 0x77, 0x81, 0x02, 0x80 };

BOOL IsPICCImageValid(int slotNumber);
BOOL PrettyPrintPICCHeaderData(int slotNumber, BYTE outputBuffer, SIZET maxLength);
BOOL PrettyPrintPICCImageDate(int slotNumber, BYTE outputBuffer, SIZET, maxLength, BOOL includeHdrInfo);

// TODO: Decode PICC master key settings (see datasheet, pp. 34-35) ... 

// TODO: Add struct for this data (with alignment packed close) and define a constant for its size in bytes 


#endif
