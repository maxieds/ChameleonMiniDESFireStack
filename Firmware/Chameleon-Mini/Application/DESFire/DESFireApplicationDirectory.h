/* 
 * DESFireApplicationDirectory.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_DFAPPDIR_H__
#define __DESFIRE_DFAPPDIR_H__

#include "DESFireFirmwareSettings.h"
#include "DESFirePICCHeaderLayout.h"
#include "DESFireInstructions.h"
#include "DESFireFile.h"

#define MAX_AID_SIZE                                (12)
#define DESFIRE_AID_SIZE                            (3)

#ifdef DESFIRE_USE_FACTORY_SIZES
     #undef  DESFIRE_CUSTOM_MAX_APPS
     #define DESFIRE_CUSTOM_MAX_APPS                (28)
     #undef  DESFIRE_CUSTOM_MAX_KEYS
     #define DESFIRE_CUSTOM_MAX_KEYS                (14)
#endif

#if defined(DESFIRE_MEMORY_LIMITED_TESTING) && !defined(DESFIRE_CUSTOM_MAX_APPS)
     #define DESFIRE_MAX_APPS                       (6)
#elif defined(DESFIRE_CUSTOM_MAX_APPS)
     #define DESFIRE_MAX_APPS                       (DESFIRE_CUSTOM_MAX_APPS)
#else
     #define DESFIRE_MAX_APPS                       (28)
#endif 

#define DESFIRE_MAX_SLOTS                           (DESFIRE_MAX_APPS + 1)

#if defined(DESFIRE_MEMORY_LIMITED_TESTING) && !defined(DESFIRE_CUSTOM_MAX_KEYS)
     #define DESFIRE_MAX_KEYS                       (4)
#elif defined(DESFIRE_CUSTOM_MAX_KEYS)
     #define DESFIRE_MAX_KEYS                       (DESFIRE_CUSTOM_MAX_KEYS)
#else
     #define DESFIRE_MAX_KEYS                       (14)
#endif

typedef BYTE DESFireAidType[DESFIRE_AID_SIZE];

extern const BYTE DEFAULT_DESFIRE_AID[]; 
extern const BYTE DEFAULT_ISO7816_AID[]; 

/* Mifare DESFire EV1 Application crypto operations */
#define APPLICATION_CRYPTO_DES    0x00
#define APPLICATION_CRYPTO_3K3DES 0x40
#define APPLICATION_CRYPTO_AES    0x80

/* 
 * Defines the application directory contents.
 * The application directory maps AIDs to application slots:
 * the AID's index in `AppIds` is the slot number.
 * 
 * This is the "global" structure that gets stored in the header 
 * data for the directory. To see the actual byte-for-byte storage 
 * of the accounting data for each instantiated AID slot, refer to the 
 * `DesfireApplicationDataType` structure from above.
 */
typedef struct DESFIRE_FIRMWARE_PACKING {
    BYTE FirstFreeSlot;
    DESFireAidType AppIds[DESFIRE_MAX_SLOTS] DESFIRE_FIRMWARE_ARRAY_ALIGNAT; /* 84 */
} DESFireAppDirType;

/** Defines the block ID of each application's file on the card. */
typedef uint8_t DesfireFileIndexType[DESFIRE_MAX_FILES];

/* This resolves to 1 */
#define DESFIRE_FILE_INDEX_BLOCKS DESFIRE_BYTES_TO_BLOCKS(sizeof(DesfireFileIndexType))

/* This resolves to 4 */
#define DESFIRE_APP_DIR_BLOCKS DESFIRE_BYTES_TO_BLOCKS(sizeof(DESFireAppDirType))

/* Special values for access key IDs */
#define DESFIRE_ACCESS_FREE     0xE
#define DESFIRE_ACCESS_DENY     0xF

/* Global card structure support routines */
void SynchronizeAppDir(void);
void SynchronizePICCInfo(void);

/* Application key management */
BYTE GetAppProperty(DesfireCardLayout propId, BYTE AppSlot);
void SetAppProperty(DesfireCardLayout propId, BYTE AppSlot, BYTE Value);
BYTE GetSelectedAppKeySettings(void);
void SetSelectedAppKeySettings(BYTE KeySettings);
BYTE GetAppKeySettings(uint8_t Slot);
void ReadSelectedAppKey(uint8_t KeyId, uint8_t* Key);
void WriteSelectedAppKey(uint8_t KeyId, const uint8_t* Key);

/* Application selection */
uint8_t LookupAppSlot(const DESFireAidType Aid);
void SelectAppBySlot(uint8_t AppSlot);
uint16_t SelectApp(const DESFireAidType Aid);
void SelectPiccApp(void);
bool IsPiccAppSelected(void);

/* Application management */
uint16_t CreateApp(const DESFireAidType Aid, uint8_t KeyCount, uint8_t KeySettings);
uint16_t DeleteApp(const DESFireAidType Aid);
void GetApplicationIdsSetup(void);
TransferStatus GetApplicationIdsTransfer(uint8_t* Buffer);
uint16_t GetApplicationIdsIterator(uint8_t *Buffer, uint16_t ByteCount);

#endif
