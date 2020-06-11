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
uint16_t MifareDesfireProcessIdle(uint8_t *Buffer, uint16_t ByteCount);
uint16_t MifareDesfireProcessCommand(uint8_t *Buffer, uint16_t ByteCount);
uint16_t MifareDesfireProcess(uint8_t *Buffer, uint16_t ByteCount);
void MifareDesfireReset(void);

#endif /* MIFAREDESFIRE_H_ */
