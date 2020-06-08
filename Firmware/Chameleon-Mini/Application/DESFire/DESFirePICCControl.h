/* 
 * DESFirePICCControl.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */ 

#ifndef __DESFIRE_PICC_CONTROL_H__
#define __DESFIRE_PICC_CONTROL_H__

#include "DESFireFirmwareSettings.h" 

typedef enum {
     PICCHDR_SERIALNO     = 1, 
     // TODO 
} PICCHeaderField;

// TODO: Insist on defaults (or zero fill of bytes ... ) 
// TODO: Validate lengths on these fields (set defines) ... 

BOOL SetProtectedHeaderData(PICCHeaderField hfield, BYTE *byteBuf, SIZET bufSize);
BOOL ResetProtectedHeaderData(PICCHeaderField hfield); 

BOOL ResetAndZeroFillPICCImage(int slotNumber); 

#endif
