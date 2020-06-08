/* 
 * DESFireAPDU.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_APDU_H__
#define __DESFIRE_APDU_H__

#include "DESFireFirmwareSettings.h" 
#include "DESFireCommands.h"

#define DESFIRE_MAX_PAYLOAD_SIZE            59 /* Bytes */

typedef struct DESFIRE_FIRMWARE_PACKING {
     BYTE  cla;
     BYTE  ins;
     BYTE  p1;
     BYTE  p2;
     BYTE  lc;
     BYTE  data[DESFIRE_MAX_PAYLOAD_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
     SIZET dataSize;
     BYTE  le;
} APDUCommand;

typedef struct DESFIRE_FIRMWARE_PACKING {
     BYTE  data[DESFIRE_MAX_PAYLOAD_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
     SIZET dataSize;
     BYTE  sw1;
     BYTE  sw2;
} APDUResponse;

extern APDUCommand  DESFireInternalAPDUCommand;
extern APDUResponse DESFireInternalAPDUResponse;

BOOL ByteBufferToAPDUCommand(BYTE *buf, SIZET bufSize, APDUCommand *apduCmd);
BOOL ByteBufferToAPDUResponse(BYTE *buf, SIZET bufSize, APDUResponse *apduResp);
BOOL APDUCommandToByteBuffer(APDUCommand *apduCmd, BYTE *destBuf, SIZET maxSize);
BOOL APDUResponseToByteBuffer(APDUResponse *apduResp, BYTE *destBuf, SUZET maxSize);

BOOL APDUCommandToPPrintString(APDUCommand *apduCmd, BYTE *destBuf, SIZET maxSize);
BOOL APDUResponseToPPrintString(APDUResponse *apduResp, BYTE *destBuf, SIZET maxSize);

DESFireCommandSet GetAPDUCommandInstructionSet(APDUCommand *apduCmd);

#endif
