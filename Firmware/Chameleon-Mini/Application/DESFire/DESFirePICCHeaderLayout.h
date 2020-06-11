/* 
 * DESFirePICCHeaderLayout.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_PICC_HDRLAYOUT_H__
#define __DESFIRE_PICC_HDRLAYOUT_H__

#include "DESFireFirmwareSettings.h"
#include "DESFireISO14443Support.h"

extern const BYTE PICC_FORMATTED_MARKER[]; 
extern const BYTE DefaultDESFireATS[]; 
extern const BYTE DefaultJCOPDESFireATS[]; 

#define DESFIRE_PICC_APP_SLOT 0

/* Storage allocation constants */
#define DESFIRE_EEPROM_BLOCK_SIZE 32 /* Bytes */
#define DESFIRE_BYTES_TO_BLOCKS(x) \
    ( ((x) + DESFIRE_EEPROM_BLOCK_SIZE - 1) / DESFIRE_EEPROM_BLOCK_SIZE )

#define DESFIRE_UID_SIZE     ISO14443A_UID_SIZE_DOUBLE

/*
 * Definitions pertaining to on-card data
 */

/* Anticollision parameters */
#define ATQA_VALUE              0x0344
#define SAK_CL1_VALUE           (ISO14443A_SAK_COMPLETE_COMPLIANT | ISO14443A_SAK_INCOMPLETE)
#define SAK_CL2_VALUE           (ISO14443A_SAK_COMPLETE_COMPLIANT)

#define STATUS_FRAME_SIZE           (1 * 8) /* Bits */

#define DESFIRE_EV0_ATS_TL_BYTE 0x06 /* TL: ATS length, 6 bytes */
#define DESFIRE_EV0_ATS_T0_BYTE 0x75 /* T0: TA, TB, TC present; max accepted frame is 64 bytes */
#define DESFIRE_EV0_ATS_TA_BYTE 0x00 /* TA: Only the lowest bit rate is supported */
#define DESFIRE_EV0_ATS_TB_BYTE 0x81 /* TB: taken from the DESFire spec */
#define DESFIRE_EV0_ATS_TC_BYTE 0x02 /* TC: taken from the DESFire spec */

#define GET_LE16(p)     (*(uint16_t*)&(p)[0])
#define GET_LE24(p)     (*(__uint24*)&(p)[0])

/* Defines for GetVersion */
#define ID_PHILIPS_NXP           0x04
#define DESFIRE_MANUFACTURER_ID         ID_PHILIPS_NXP

/* These do not change */
#define DESFIRE_TYPE                    0x01
#define DESFIRE_SUBTYPE                 0x01
#define DESFIRE_HW_PROTOCOL_TYPE        0x05
#define DESFIRE_SW_PROTOCOL_TYPE        0x05

/** Source: http://www.proxmark.org/forum/viewtopic.php?id=2982 **/
/* DESFire EV0 versions */
#define DESFIRE_HW_MAJOR_EV0     0x00
#define DESFIRE_HW_MINOR_EV0     0x01
#define DESFIRE_SW_MAJOR_EV0     0x00
#define DESFIRE_SW_MINOR_EV0     0x01

/* DESFire EV1 versions */
#define DESFIRE_HW_MAJOR_EV1     0x01
#define DESFIRE_HW_MINOR_EV1     0x01
#define DESFIRE_SW_MAJOR_EV1     0x01
#define DESFIRE_SW_MINOR_EV1     0x01

/* DESFire EV2 versions */
#define DESFIRE_HW_MAJOR_EV2     0x12
#define DESFIRE_HW_MINOR_EV2     0x01
#define DESFIRE_SW_MAJOR_EV2     0x12
#define DESFIRE_SW_MINOR_EV2     0x01

#define DESFIRE_STORAGE_SIZE_2K  0x16
#define DESFIRE_STORAGE_SIZE_4K  0x18
#define DESFIRE_STORAGE_SIZE_8K  0x1A

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
     PICCHDR_SERIALNO     = 1,
     // TODO
} PICCHeaderField;

// TODO: Insist on defaults (or zero fill of bytes ... )
// TODO: Validate lengths on these fields (set defines) ...

/** Defines the global PICC configuration.
 * This is located in the very first block on the card.
 */
typedef struct DESFIRE_FIRMWARE_PACKING {
    /* Static data: does not change during the PICC's lifetime */
    uint8_t Uid[DESFIRE_UID_SIZE];
    uint8_t StorageSize;
    uint8_t HwVersionMajor;
    uint8_t HwVersionMinor;
    uint8_t SwVersionMajor;
    uint8_t SwVersionMinor;
    uint8_t BatchNumber[5];
    uint8_t ProductionWeek;
    uint8_t ProductionYear;
    /* Dynamic data: changes during the PICC's lifetime */
    uint8_t FirstFreeBlock;
    uint8_t TransactionStarted;
    uint8_t Spare[9];
} DESFirePICCInfoType;

// TODO: Decode PICC master key settings (see datasheet, pp. 34-35) ... 

/* PICC / Application master key settings */
/* Mifare DESFire master key settings
bit 7 - 4: Always 0.
bit 3: PICC master key settings frozen = 0 (WARNING - this is irreversible); PICC master key settings changeable when authenticated with PICC master key = 1
bit 2: PICC master key authentication required for creating or deleting applications = 0; Authentication not required = 1
bit 1: PICC master key authentication required for listing of applications or reading key settings = 0; Free listing of applications and reading key settings = 1
bit 0: PICC master key frozen (reversible with configuration change or when formatting card) = 0; PICC master key changeable = 1
*/
#define DESFIRE_ALLOW_MASTER_KEY_CHANGE  (1 << 0)
#define DESFIRE_FREE_DIRECTORY_LIST      (1 << 1)
#define DESFIRE_FREE_CREATE_DELETE       (1 << 2)
#define DESFIRE_ALLOW_CONFIG_CHANGE      (1 << 3)
#define DESFIRE_USE_TARGET_KEY           0xE
#define DESFIRE_ALL_KEYS_FROZEN          0xF

BOOL IsPICCImageValid(int slotNumber);
BOOL PrettyPrintPICCHeaderData(int slotNumber, BYTE outputBuffer, SIZET maxLength);
BOOL PrettyPrintPICCImageData(int slotNumber, BYTE outputBuffer, SIZET maxLength, BOOL includeHdrInfo);

#endif
