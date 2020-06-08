/* 
 * DESFireApplicationDirectory.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_DFAPPDIR_H__
#define __DESFIRE_DFAPPDIR_H__

#include "DESFireFirmwareSettings.h"
#include "DESFirePICCHeaderLayout.h"

#define DFAID_SIZE                    (3)
#define DF_DATA_CHUNK_SIZE            (32)
#define DF_DATA_CHUNK_ALIGNAT         __attribute__((align(DF_DATA_CHUNK_SIZE)))

#define DESFIRE_AID_SIZE        3
#define DESFIRE_MAX_APPS        28
#define DESFIRE_MAX_KEYS        14
#define DESFIRE_MAX_SLOTS       (DESFIRE_MAX_APPS + 1)

typedef uint8_t DesfireAidType[DESFIRE_AID_SIZE];

/** Data about applications is kept in these structures.
 * The application is represented as a collection of one-byte "properties":
 * key settings, key count, keystore block ID, file index block ID.
 * Since pre-EV2 cards have a fixed maximum amount of applications (1 PICC + 28 user),
 * using a fixed structure for this makes sense.
 */
typedef struct {
    uint8_t Spare0;
    uint8_t AppData[DESFIRE_MAX_SLOTS];
    uint16_t Checksum; /* Not actually used atm */
} DesfireApplicationDataType;

/* Mifare DESFire EV1 Application crypto operations */
#define APPLICATION_CRYPTO_DES    0x00
#define APPLICATION_CRYPTO_3K3DES 0x40
#define APPLICATION_CRYPTO_AES    0x80

/** Defines the application directory contents.
 * The application directory maps AIDs to application slots:
 * the AID's index in `AppIds` is the slot number.
 */
typedef struct {
    uint8_t FirstFreeSlot;
    uint8_t Spare[8];
    DesfireAidType AppIds[DESFIRE_MAX_SLOTS]; /* 84 */
} DesfireAppDirType;

/** Defines the block ID of each application's file on the card. */
typedef uint8_t DesfireFileIndexType[DESFIRE_MAX_FILES];

/* This resolves to 1 */
#define DESFIRE_FILE_INDEX_BLOCKS DESFIRE_BYTES_TO_BLOCKS(sizeof(DesfireFileIndexType))

/* This resolves to 4 */
#define DESFIRE_APP_DIR_BLOCKS DESFIRE_BYTES_TO_BLOCKS(sizeof(DesfireAppDirType))

/* The actual layout */
typedef enum DesfireCardLayout {
    /* PICC related informaton is kept here */
    DESFIRE_PICC_INFO_BLOCK_ID = 0,
    /* Keeps the list of all applications created */
    DESFIRE_APP_DIR_BLOCK_ID,
    /* AppData keeping track of apps' key settings */
    DESFIRE_APP_KEY_SETTINGS_BLOCK_ID = DESFIRE_APP_DIR_BLOCK_ID + DESFIRE_APP_DIR_BLOCKS,
    /* AppData keeping track how many keys each app has */
    DESFIRE_APP_KEY_COUNT_BLOCK_ID,
    /* AppData keeping track of apps' key locations */
    DESFIRE_APP_KEYS_PTR_BLOCK_ID,
    /* AppData keeping track of apps' file index blocks */
    DESFIRE_APP_FILES_PTR_BLOCK_ID,
    /* Free space starts here */
    DESFIRE_FIRST_FREE_BLOCK_ID,
};

/* Mifare DESFire Application settings
 * bit 7 - 4: Number of key needed to change application keys (key 0 - 13; 0 = master key; 14 = key itself required for key change; 15 = all keys are frozen)
 * bit 3: Application configuration frozen = 0; Application configuration changeable when authenticated with application master key = 1
 * bit 2: Application master key authentication required for create/delete files = 0; Authentication not required = 1
 * bit 1: GetFileIDs, GetFileSettings and GetKeySettings behavior: Master key authentication required = 0; No authentication required = 1
 * bit 0 = Application master key frozen = 0; Application master key changeable = 1
 */

/* Special values for access key IDs */
#define DESFIRE_ACCESS_FREE     0xE
#define DESFIRE_ACCESS_DENY     0xF
#define DESFIRE_CHANGE_ACCESS_RIGHTS_SHIFT      (0*4)
#define DESFIRE_READWRITE_ACCESS_RIGHTS_SHIFT   (1*4)
#define DESFIRE_WRITE_ACCESS_RIGHTS_SHIFT       (2*4)
#define DESFIRE_READ_ACCESS_RIGHTS_SHIFT        (3*4)

/* Application management */
bool IsPiccAppSelected(void);
void SelectPiccApp(void);
uint8_t SelectApp(const DesfireAidType Aid);
uint8_t CreateApp(const DesfireAidType Aid, uint8_t KeyCount, uint8_t KeySettings);
uint8_t DeleteApp(const DesfireAidType Aid);
void GetApplicationIdsSetup(void);
TransferStatus GetApplicationIdsTransfer(uint8_t* Buffer);

/* Application key management */
uint8_t GetSelectedAppKeyCount(void);
uint8_t GetSelectedAppKeySettings(void);
void SetSelectedAppKeySettings(uint8_t KeySettings);
void ReadSelectedAppKey(uint8_t KeyId, uint8_t* Key);
void WriteSelectedAppKey(uint8_t KeyId, const uint8_t* Key);

#endif
