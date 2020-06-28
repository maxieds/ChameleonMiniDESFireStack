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

/* The core functions used outside of this implementation 
 * to describe the DESFire emulation to the Chameleon firmware: 
 */
void MifareDesfireEV0AppInit(void);
void MifareDesfire2kEV1AppInit(void);
void MifareDesfire4kEV1AppInit(void);
void MifareDesfire8kEV1AppInit(void);
void MifareDesfireAppReset(void);
void MifareDesfireAppTask(void);

uint16_t MifareDesfireAppProcess(uint8_t* Buffer, uint16_t BitCount);

void MifareDesfireGetUid(ConfigurationUidType Uid);
void MifareDesfireSetUid(ConfigurationUidType Uid);

/* Helper function definitions since we need them 
 * elsewhere in the backend, and so we do not need to 
 * declare them as static in the source. 
 */

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    DESFIRE_HALT,
    DESFIRE_IDLE,
    DESFIRE_CMD_READY,
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
extern bool DesfireFromHalt;
extern BYTE DesfireCmdCLA;

void ResetLocalStructureData(void);
uint16_t MifareDesfireProcessCommand(uint8_t *Buffer, uint16_t ByteCount);
void MifareDesfireReset(void);

#endif /* MIFAREDESFIRE_H_ */
