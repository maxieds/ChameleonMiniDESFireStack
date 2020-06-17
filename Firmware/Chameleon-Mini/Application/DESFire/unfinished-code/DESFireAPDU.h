/* 
 * DESFireAPDU.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_APDU_H__
#define __DESFIRE_APDU_H__

#include "DESFireFirmwareSettings.h" 

#define CLA_PROTECTED_APDU           (0x0c)
#define CLA_ISO7816                  (0x00)
#define P1_DF                        (0x04)
#define P2_SELECT                    (0x0C)
#define LE_MAX                       ((uint16_t) 256)

#define DO87_START                   ((uint8_t) 0x87)
#define DO87_TYPE                    ((uint32_t) 0x00000080)
#define DO87_BYTENO                  ((uint8_t) 0x7F)
#define DO87_END                     ((uint8_t) 0x01)

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
BOOL APDUResponseToByteBuffer(APDUResponse *apduResp, BYTE *destBuf, SIZET maxSize);

BOOL APDUCommandToPPrintString(APDUCommand *apduCmd, BYTE *destBuf, SIZET maxSize);
BOOL APDUResponseToPPrintString(APDUResponse *apduResp, BYTE *destBuf, SIZET maxSize);

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
     DFCMD_SPEC_UNKNOWN   = 0,
     DFCMD_SPEC_NATIVE    = 1,
     DFCMD_SPEC_ISO1443   = 2,
     DFCMD_SPEC_ISO7816   = 3,
} DESFireCommandSet;

DESFireCommandSet GetAPDUCommandInstructionSet(APDUCommand *apduCmd);

#endif
