/* 
 * DESFireResponseCodes.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_RESP_ERROR_CODES_H__
#define __DESFIRE_RESP_ERROR_CODES_H__

#include "DESFireFirmwareSettings.h" 

// TODO: See p. 72 of the datasheet for descriptions of these response codes ... 

BYTE[2] GetResponseTrailerFromError(BYTE ins, BYTE errorCode);
BYTE[2] GetResponseTrialerFromOK(BYTE ins, BYTE, respCode);

BOOL ResolveErrorAsString(BYTE ins, BYTE[2] swResp, BYTE *destBuf, SIZET maxSize);
BOOL ResolveResponseAsString(BYTE ins, BYTE[2] swResp, BYTE *destBuf, SIZET maxSize);







#endif 
