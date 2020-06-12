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

#define DFAID_SIZE                    (3)
#define DF_DATA_CHUNK_SIZE            (32)
#define DF_DATA_CHUNK_ALIGNAT         __attribute__((align(DF_DATA_CHUNK_SIZE)))

#define MAX_AID_SIZE                                (12)
#define DESFIRE_AID_SIZE                            (3)

#if defined(DESFIRE_MEMORY_LIMITED_TESTING) && !defined(DESFIRE_CUSTOM_MAX_APPS)
     #define DESFIRE_MAX_APPS                       (6)
#elif defined(DESFIRE_CUSTOM_MAX_APPS)
     #define DESFIRE_MAX_APPS                       (DESFIRE_CUSTOM_MAX_APPS)
#else
     #define DESFIRE_MAX_APPS                            (28)
#endif 

#define DESFIRE_MAX_SLOTS                           (DESFIRE_MAX_APPS + 1)

#if defined(DESFIRE_MEMORY_LIMITED_TESTING) && !defined(DESFIRE_CUSTOM_MAX_KEYS)
     #define DESFIRE_MAX_KEYS                            (4)
#elif defined(DESFIRE_CUSTOM_MAX_KEYS)
     #define DESFIRE_MAX_KEYS                            (DESFIRE_CUSTOM_MAX_KEYS)
#else
     #define DESFIRE_MAX_KEYS                            (14)
#endif

typedef BYTE DESFireAidType[DESFIRE_AID_SIZE];

extern const BYTE DEFAULT_DESFIRE_AID[]; 
extern const BYTE DEFAULT_ISO7816_AID[]; 

/** Data about applications is kept in these structures.
 * The application is represented as a collection of one-byte "properties":
 * key settings, key count, keystore block ID, file index block ID.
 * Since pre-EV2 cards have a fixed maximum amount of applications (1 PICC + 28 user),
 * using a fixed structure for this makes sense.
 */
typedef struct DESFIRE_FIRMWARE_PACKING {
    //BYTE AppData[DESFIRE_MAX_SLOTS] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    BYTE IsValid;
    BYTE Spare0;
    SIZET Checksum; /* Not actually used atm */
    BYTE FirstFreeFileSlot;
    BYTE FileNumbersArrayMap[DESFIRE_MAX_FILES] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    SIZET FilePiccBlockOffsets[DESFIRE_MAX_FILES] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    BYTE KeyData[DESFIRE_MAX_KEYS - 1][CRYPTO_3KTDEA_KEY_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    BYTE KeyCryptoMethodTypes[DESFIRE_MAX_KEYS - 1] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    BYTE KeyVersions[DESFIRE_MAX_KEYS - 1] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    BYTE AppCryptoStandard;
} DesfireApplicationDataType;

extern DesfireApplicationDataType SelectedAppData;

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
    BYTE Spare[8];
    DESFireAidType AppIds[DESFIRE_MAX_SLOTS] DESFIRE_FIRMWARE_ARRAY_ALIGNAT; /* 84 */
    SIZET AppIdPiccBlockOffsets[DESFIRE_MAX_SLOTS] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    BYTE AppMasterKey[CRYPTO_3KTDEA_KEY_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
    BYTE AppMasterKeyCryptoMethod;
    BYTE AppKeySettings[DESFIRE_MAX_SLOTS] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
} DESFireAppDirType;

/** Defines the block ID of each application's file on the card. */
typedef uint8_t DesfireFileIndexType[DESFIRE_MAX_FILES];

/* This resolves to 1 */
#define DESFIRE_FILE_INDEX_BLOCKS DESFIRE_BYTES_TO_BLOCKS(sizeof(DesfireFileIndexType))

/* This resolves to 4 */
#define DESFIRE_APP_DIR_BLOCKS DESFIRE_BYTES_TO_BLOCKS(sizeof(DESFireAppDirType))

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

/* Global card structure support routines */
void SynchronizeAppDir(void);
void SynchronizePICCInfo(void);
uint8_t GetApplicationData(uint8_t AppSlot, DesfireApplicationDataType *appData);

/* Application key management */
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
