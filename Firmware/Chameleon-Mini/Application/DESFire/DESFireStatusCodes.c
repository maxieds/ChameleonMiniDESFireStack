/* 
 * DESFireStatusCodes.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "DESFireStatusCodes.h"

BYTE FRAME_CONTINUE[] = { 
     (BYTE) 0x91, (BYTE) 0xaf 
};
BYTE OPERATION_OK[] = { 
     (BYTE) 0x91, 0x00 
};
BYTE OK[] = { 
     (BYTE) 0x90, 0x00 
};
BYTE INIT[] = {
     0x00, 0x00 
};

