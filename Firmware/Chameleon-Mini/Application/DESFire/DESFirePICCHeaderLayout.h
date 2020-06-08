/* 
 * DESFirePICCHeaderLayout.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_PICC_HDRLAYOUT_H__
#define __DESFIRE_PICC_HDRLAYOUT_H__

#include "DESFireFirmwareSettings.h"

static inline const BYTE PICC_FORMATTED_MARKER[] = { 0xf0, 0x12, 0x34 };
static inline const BYTE DefaultDESFireATS[] = { 0x06, 0x75, 0x77, 0x81, 0x02, 0x80 };

#define DESFIRE_PICC_APP_SLOT 0

/* Storage allocation constants */
#define DESFIRE_EEPROM_BLOCK_SIZE 32 /* Bytes */
#define DESFIRE_BYTES_TO_BLOCKS(x) \
    ( ((x) + DESFIRE_EEPROM_BLOCK_SIZE - 1) / DESFIRE_EEPROM_BLOCK_SIZE )

#define DESFIRE_UID_SIZE     ISO14443A_UID_SIZE_DOUBLE

/*
 * Definitions pertaining to on-card data
 */

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

typedef enum {
     PICCHDR_SERIALNO     = 1,
     // TODO
} PICCHeaderField;

// TODO: Insist on defaults (or zero fill of bytes ... )
// TODO: Validate lengths on these fields (set defines) ...

/** Defines the global PICC configuration.
 * This is located in the very first block on the card.
 */
#pragma pack (push)
#pragma pack (1)
typedef struct {
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
} DesfirePiccInfoType;
#pragma pack (pop)

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
BOOL PrettyPrintPICCImageDate(int slotNumber, BYTE outputBuffer, SIZET, maxLength, BOOL includeHdrInfo);

#endif
