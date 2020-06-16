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

#define DESFIRE_PICC_APP_SLOT          0x00
#define DESFIRE_MASTER_KEY_ID          0x00

/* Storage allocation constants */
#define DESFIRE_EEPROM_BLOCK_SIZE      32 /* Bytes */
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
#define ID_PHILIPS_NXP                  0x04
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

/* Other HW product types for DESFire tags: See page 7 of 
 * https://www.nxp.com/docs/en/application-note/AN12343.pdf 
 */
// typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
//     NATIVEIC_PHYS_CARD                 = 0x01, 
//     LIGHT_NATIVEIC_PHYS_CARD           = 0x08, 
//     MICROCONTROLLER_PHYS_CARDI         = 0x81, 
//     MICROCONTROLLER_PHYS_CARDII        = 0x83, 
//     JAVACARD_SECURE_ELEMENT_PHYS_CARD  = 0x91, 
//     HCE_MIFARE_2GO                     = 0xa1, 
// } DESFireHWProductCodes;
//
// const BYTE DefaultJCOPDESFireATS[] = {
//     0x06, 0x75, 0xf7, 0xb1, 0x02, 0x80
// };
// 
// const BYTE VERSION1[] = {
//     0x04, 0x01, 0x01, 0x01, 0x00, 0x1a, 0x05
// };
// const BYTE VERSION2[] = {
//     0x04, 0x01, 0x01, 0x01, 0x03, 0x1a, 0x05
// };
// const BYTE VERSION3[] = {
//    // Expected Response: 00  04  91  3a  29  93  26  80  00  00  00  00  00  39  08  91  00
//    0x04, (BYTE) 0x91, 0x3a, 0x29, (BYTE) 0x93,
//    0x26, (BYTE) 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x08
// };

#define DESFIRE_STORAGE_SIZE_2K  0x16
#define DESFIRE_STORAGE_SIZE_4K  0x18
#define DESFIRE_STORAGE_SIZE_8K  0x1A

/* 
 * Defines the global PICC configuration.
 * This is located in the very first block on the card.
 */
#define PICC_FORMAT_BYTE                   (0xff)
#define PICC_EMPTY_BYTE                    (0x00)

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
   bit 3: PICC master key settings frozen = 0 (WARNING - this is irreversible); 
          PICC master key settings changeable when authenticated with PICC master key = 1
   bit 2: PICC master key authentication required for creating or deleting applications = 0; 
          Authentication not required = 1
   bit 1: PICC master key authentication required for listing of applications or 
          reading key settings = 0; 
          Free listing of applications and reading key settings = 1
   bit 0: PICC master key frozen (reversible with configuration change or when formatting card) = 0; 
          PICC master key changeable = 1
*/
#define DESFIRE_ALLOW_MASTER_KEY_CHANGE  (1 << 0)
#define DESFIRE_FREE_DIRECTORY_LIST      (1 << 1)
#define DESFIRE_FREE_CREATE_DELETE       (1 << 2)
#define DESFIRE_ALLOW_CONFIG_CHANGE      (1 << 3)
#define DESFIRE_USE_TARGET_KEY           0xE
#define DESFIRE_ALL_KEYS_FROZEN          0xF

typedef struct {
    BYTE  Slot;
    BYTE  KeyCount;
    BYTE  FileCount;
    BYTE  CryptoCommStandard;
    SIZET KeySettings;            /* Block offset in EEPROM */
    SIZET FileNumbersArrayMap;    /* Block offset in EEPROM */ 
    SIZET FileCommSettings;       /* Block offset in EEPROM */
    SIZET FileAccessRights;       /* Block offset in EEPROM */
    SIZET KeyVersionsArray;       /* Block offset in EEPROM */
    SIZET KeyTypesArray;          /* Block offset in EEPROM */
    SIZET FilesAddress;           /* Block offset in EEPROM */
    SIZET KeyAddress;             /* Block offset in EEPROM */
    UINT  DirtyFlags;
} SelectedAppCacheType;

extern BYTE SELECTED_APP_CACHE_TYPE_BLOCK_SIZE;
extern BYTE APP_CACHE_KEY_SETTINGS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_FILE_NUMBERS_HASHMAP_BLOCK_SIZE;
extern BYTE APP_CACHE_FILE_COMM_SETTINGS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_FILE_ACCESS_RIGHTS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_KEY_VERSIONS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_KEY_TYPES_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_KEY_BLOCKIDS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_FILE_BLOCKIDS_ARRAY_BLOCK_SIZE;
extern BYTE APP_CACHE_MAX_KEY_BLOCK_SIZE;

extern SIZET DESFIRE_PICC_INFO_BLOCK_ID;
extern SIZET DESFIRE_APP_DIR_BLOCK_ID;
extern SIZET DESFIRE_APP_CACHE_DATA_ARRAY_BLOCK_ID;
extern SIZET DESFIRE_INITIAL_FIRST_FREE_BLOCK_ID;
extern SIZET DESFIRE_FIRST_FREE_BLOCK_ID;
extern SIZET CardCapacityBlocks;

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    /* AppData keeping track how many keys each app has */
    DESFIRE_APP_KEY_COUNT,
    /* AppData active file count */
    DESFIRE_APP_FILE_COUNT,
    /* AppData keep track of default crypto comm standard */
    DESFIRE_APP_CRYPTO_COMM_STANDARD, 
    /* AppData keeping track of apps key settings */
    DESFIRE_APP_KEY_SETTINGS_BLOCK_ID,
    /* AppData hash-like unsorted array mapping file indices to their labeled numbers */
    DESFIRE_APP_FILE_NUMBER_ARRAY_MAP_BLOCK_ID,
    /* AppData communication settings (crypto transfer protocols) per file */
    DESFIRE_APP_FILE_COMM_SETTINGS_BLOCK_ID,
    /* AppData file access rights */
    DESFIRE_APP_FILE_ACCESS_RIGHTS_BLOCK_ID, 
    /* AppData keep track of newer EVx revisions key versioning schemes */
    DESFIRE_APP_KEY_VERSIONS_ARRAY_BLOCK_ID,
    /* AppData keep track of the key types (and hence, byte sizes) by crypto method */
    DESFIRE_APP_KEY_TYPES_ARRAY_BLOCK_ID,
    /* AppData keeping track of apps file index blocks */
    DESFIRE_APP_FILES_PTR_BLOCK_ID,
    /* AppData keeping track of apps key locations */
    DESFIRE_APP_KEYS_PTR_BLOCK_ID,
} DesfireCardLayout;

// TODO: 
BOOL PrettyPrintPICCHeaderData(uint8_t slotNumber, BYTE *outputBuffer, SIZET maxLength);
BOOL PrettyPrintPICCImageData(uint8_t slotNumber, BYTE *outputBuffer, SIZET maxLength);
BOOL PrettyPrintPICCApplicationDir(uint8_t slotNumber, BYTE *outputBuffer, SIZET maxLength);
BOOL PrettyPrintPICCFile(uint8_t slotNumber, uint8_t fileIndex, BYTE *outputBuffer, SIZET maxLength);

#endif
