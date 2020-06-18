/* 
 * DESFireISO14443Support.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_ISO14443_SUPPORT_H__
#define __DESFIRE_ISO14443_SUPPORT_H__

#include "DESFireFirmwareSettings.h"

#include "../ISO1443-4.h"
#include "../ISO14443-3A.h"
#include "../../Codec/ISO14443-2A.h"

/*
 * ISO/IEC 14443-4 implementation
 * To support EV2 cards emulation, proper support for handling 14443-4
 * blocks will be implemented.
 * Currently NOT supported:
 * + Block chaining
 * + Frame waiting time extension
 */

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    ISO14443_4_STATE_EXPECT_RATS,
    ISO14443_4_STATE_ACTIVE,
} Iso144434StateType;

extern Iso144434StateType Iso144434State;
extern uint8_t Iso144434BlockNumber;
extern uint8_t Iso144434CardID;
extern uint8_t Iso144434LastBlockLength;
//extern uint8_t Iso144434LastBlock[CODEC_BUFFER_SIZE];
extern uint8_t Iso144434LastBlock[2];

/* Support functions */
void ISO144434SwitchState(Iso144434StateType NewState);
void ISO144434Reset(void);
uint16_t ISO144434ProcessBlock(uint8_t* Buffer, uint16_t ByteCount);

/*
 * ISO/IEC 14443-3A implementation
 */

#define ISO14443A_CRCA_INIT      ((uint16_t) 0x0000)

uint16_t ISO14443AUpdateCRCA(const uint8_t *Buffer, uint16_t ByteCount, uint16_t InitCRCA);

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    /* The card is powered up but not selected */
    ISO14443_3A_STATE_IDLE,
    /* Entered on REQA or WUPA; anticollision is being performed */
    ISO14443_3A_STATE_READY1,
    ISO14443_3A_STATE_READY2,
    /* Entered when the card has been selected */
    ISO14443_3A_STATE_ACTIVE,
    /* Something went wrong or we've received a halt command */
    ISO14443_3A_STATE_HALT,
} Iso144433AStateType;

extern Iso144433AStateType Iso144433AState;
extern Iso144433AStateType Iso144433AIdleState;

/* Support functions */
void ISO144433ASwitchState(Iso144433AStateType NewState);
void ISO144433AReset(void);
void ISO144433AHalt(void);
bool ISO144433AIsHalt(const uint8_t* Buffer, uint16_t BitCount);
uint16_t ISO144433APiccProcess(uint8_t* Buffer, uint16_t BitCount);

#endif
