/*
The DESFire stack portion of this firmware source 
is free software written by Maxie Dion Schmidt (@maxieds): 
You can redistribute it and/or modify
it under the terms of this license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

The complete source distribution of  
this firmware is available at the following link:
https://github.com/maxieds/ChameleonMiniFirmwareDESFireStack.

Based in part on the original DESFire code created by  
@dev-zzo (GitHub handle) [Dmitry Janushkevich] available at  
https://github.com/dev-zzo/ChameleonMini/tree/desfire.

This notice must be retained at the top of all source files where indicated. 
*/

/*
 * MifareDesfire.h
 * MIFARE DESFire frontend
 *
 * Created on: 14.10.2016
 * Author: dev_zzo
 */

#ifndef MIFAREDESFIRE_H_
#define MIFAREDESFIRE_H_

#include "Application.h"
#include "DESFire/DESFireFirmwareSettings.h"
#include "DESFire/DESFirePICCHeaderLayout.h"
#include "DESFire/DESFireISO14443Support.h"
#include "DESFire/DESFireInstructions.h"

#define IS_ISO14443A_4_COMPLIANT(buf)   (buf[0] & 0x20)
#define MAKE_ISO14443A_4_COMPLIANT(buf) (buf[0] |= 0x20)

/* The core functions used outside of this implementation 
 * to describe the DESFire emulation to the Chameleon firmware: 
 */
void ResetLocalStructureData(void);
void MifareDesfireReset(void);
void MifareDesfireEV0AppInit(void);
void MifareDesfire2kEV1AppInit(void);
void MifareDesfire4kEV1AppInit(void);
void MifareDesfire8kEV1AppInit(void);
void MifareDesfireAppReset(void);
void MifareDesfireAppTick(void);
void MifareDesfireAppTask(void);

uint16_t MifareDesfireProcessCommand(uint8_t *Buffer, uint16_t ByteCount);
uint16_t MifareDesfireProcess(uint8_t *Buffer, uint16_t ByteCount);
uint16_t MifareDesfireAppProcess(uint8_t* Buffer, uint16_t BitCount);

void MifareDesfireGetUid(ConfigurationUidType Uid);
void MifareDesfireSetUid(ConfigurationUidType Uid);

/* Helper function definitions since we need them 
 * elsewhere in the backend, and so we do not need to 
 * declare them as static in the source. 
 */
#define DesfireCLA(cmdCode) \
    ((cmdCode == DESFIRE_NATIVE_CLA) || (cmdCode == DESFIRE_ISO7816_CLA))
#define Iso7816CLA(cmdCode) \
    (cmdCode == DESFIRE_ISO7816_CLA)

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    DESFIRE_HALT,
    DESFIRE_IDLE,
    DESFIRE_IDLE2,
    DESFIRE_GET_VERSION2,
    DESFIRE_GET_VERSION3,
    DESFIRE_GET_APPLICATION_IDS2,
    DESFIRE_LEGACY_AUTHENTICATE,
    DESFIRE_LEGACY_AUTHENTICATE2,
    DESFIRE_ISO_AUTHENTICATE,
    DESFIRE_ISO_AUTHENTICATE2,
    DESFIRE_AES_AUTHENTICATE,
    DESFIRE_AES_AUTHENTICATE2,
    DESFIRE_READ_DATA_FILE,
    DESFIRE_WRITE_DATA_FILE,
} DesfireStateType;

extern DesfireStateType DesfireState;
extern DesfireStateType DesfirePreviousState;
extern bool DesfireFromHalt;
extern BYTE DesfireCmdCLA;

/* Some of the wrapped ISO7816 commands have extra meaning 
 * packed into the P1-P2 bytes of the APDU byte array.
 * When we support these extra modes, this is a way to keep 
 * track of the local meanings without needing extra handling 
 * functions to distinguish between the wrapped command types 
 * for the ISO7816 versus native DESFire instructions.
 */
typedef enum {
    ISO7816_NO_DATA = 0,
    ISO7816_UNSUPPORTED_MODE,
    ISO7816_SELECT_EF,
    ISO7816_SELECT_DF,
    ISO7816_FILE_FIRST_RECORD,
    ISO7816_FILE_LAST_RECORD,
    ISO7816_FILE_NEXT_RECORD,
    ISO7816_FILE_PREV_RECORD,
} Iso7816WrappedParams_t;

extern Iso7816WrappedParams_t Iso7816P1Data;
extern Iso7816WrappedParams_t Iso7816P2Data;
extern bool Iso7816FileSelected;

#define ISO7816_STATUS_RESPONSE_SIZE                 (0x02)
#define ISO7816_CMD_NO_ERROR                         (0x0000)
#define ISO7816_ERROR_SW1                            (0x6a)
#define ISO7816_SELECT_ERROR_SW2_UNSUPPORTED         (0x81)
#define ISO7816_SELECT_ERROR_SW2_NOFILE              (0x82)
#define ISO7816_GET_CHALLENGE_ERROR_SW2_UNSUPPORTED  (0x81)

#define AppendSW12Bytes(sw1, sw2)   \
    ((uint16_t)  ((sw1 << 8) | (sw2 & 0xff)))

uint16_t SetIso7816WrappedParametersType(uint8_t *Buffer, uint16_t ByteCount);

#endif /* MIFAREDESFIRE_H_ */
