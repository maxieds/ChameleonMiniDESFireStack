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

#define DESFIRE_MAX_FILES_EV0             16
#define DESFIRE_MAX_FILES_EV1             32

#ifdef DESFIRE_USE_FACTORY_SIZES
     #undef  DESFIRE_CUSTOM_MAX_APPS
     #define DESFIRE_CUSTOM_MAX_APPS                (28)
     #undef  DESFIRE_CUSTOM_MAX_KEYS
     #define DESFIRE_CUSTOM_MAX_KEYS                (14)
     #undef  DESFIRE_CUSTOM_MAX_FILES
     #define DESFIRE_CUSTOM_MAX_FILES               (DESFIRE_MAX_FILES_EV1)
#elif defined(DESFIRE_MAXIMIZE_SIZES_FOR_STORAGE)
     #undef  DESFIRE_CUSTOM_MAX_APPS
     #define DESFIRE_CUSTOM_MAX_APPS                (DESFIRE_EEPROM_BLOCK_SIZE - 1)
     #undef  DESFIRE_CUSTOM_MAX_KEYS
     #define DESFIRE_CUSTOM_MAX_KEYS                (DESFIRE_EEPROM_BLOCK_SIZE)
     #undef  DESFIRE_CUSTOM_MAX_FILES
     #define DESFIRE_CUSTOM_MAX_FILES               (DESFIRE_EEPROM_BLOCK_SIZE)
#endif


#if defined(DESFIRE_MEMORY_LIMITED_TESTING) && !defined(DESFIRE_CUSTOM_MAX_APPS)
     #define DESFIRE_MAX_APPS                       (6)
#elif defined(DESFIRE_CUSTOM_MAX_APPS)
     #define DESFIRE_MAX_APPS                       (DESFIRE_CUSTOM_MAX_APPS)
#else
     #define DESFIRE_MAX_APPS                       (28)
#endif 

#define DESFIRE_MAX_SLOTS                           (DESFIRE_MAX_APPS + 1)

#if defined(DESFIRE_MEMORY_LIMITED_TESTING) && !defined(DESFIRE_CUSTOM_MAX_FILES)
     #define DESFIRE_MAX_FILES                      (4)
#elif defined(DESFIRE_CUSTOM_MAX_FILES)
     #define DESFIRE_MAX_FILES                      (DESFIRE_CUSTOM_MAX_FILES)
#else
     #define DESFIRE_MAX_FILES                      (DESFIRE_MAX_FILES_EV1)
#endif

#if defined(DESFIRE_MEMORY_LIMITED_TESTING) && !defined(DESFIRE_CUSTOM_MAX_KEYS)
     #define DESFIRE_MAX_KEYS                       (4)
#elif defined(DESFIRE_CUSTOM_MAX_KEYS)
     #define DESFIRE_MAX_KEYS                       (DESFIRE_CUSTOM_MAX_KEYS)
#else
     #define DESFIRE_MAX_KEYS                       (14)
#endif

/* Mifare DESFire EV1 Application crypto operations */
#define APPLICATION_CRYPTO_DES    0x00
#define APPLICATION_CRYPTO_3K3DES 0x40
#define APPLICATION_CRYPTO_AES    0x80

/* Define application directory identifiers: */
#define MAX_AID_SIZE                                (12)
#define DESFIRE_AID_SIZE                            (3)

typedef BYTE DESFireAidType[DESFIRE_AID_SIZE];

extern const BYTE DEFAULT_SELECT_DESFIRE_AID[]; 
extern const BYTE DEFAULT_SELECT_ISO7816_AID[]; 

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
    DESFireAidType AppIds[DESFIRE_MAX_SLOTS] DESFIRE_FIRMWARE_ARRAY_ALIGNAT; 
} DESFireAppDirType;

#define DESFIRE_APP_DIR_BLOCKS      DESFIRE_BYTES_TO_BLOCKS(sizeof(DESFireAppDirType))

/* Global card structure support routines */
void SynchronizeAppDir(void);
void SynchronizePICCInfo(void);

/* PICC master key (PMK) settings for application creation / deletion 
 * (see page 34 of the datasheet)
 */
BYTE PMKConfigurationChangeable(void);
BYTE PMKRequiredForAppCreateDelete(void);
BYTE PMKFreeDirectoryListing(void);
BYTE PMKAllowChangingKey(void);

/* Application master key (AMK) 
 * (see page 35 of the datasheet)
 */
BYTE AMKConfigurationChangeable(void);
BYTE AMKRequiredForFileCreateDelete(void);
BYTE AMKFreeDirectoryListing(void);
BYTE AMKAllowChangingKey(void);
BYTE AMKRequiredToChangeKeys(void);
BYTE AMKGetRequiredKeyToChangeKeys(void);
BYTE AMKRequireCurrentKeyToChangeKey(void);
BYTE AMKAllKeysFrozen(void);

/* Application data management */
SIZET GetAppProperty(DesfireCardLayout propId, BYTE AppSlot);
void SetAppProperty(DesfireCardLayout propId, BYTE AppSlot, SIZET Value);

/* Application key management */
BYTE ReadKeyCount(uint8_t AppSlot);
void WriteKeyCount(uint8_t AppSlot, BYTE KeyCount);
BYTE ReadKeySettings(uint8_t AppSlot, uint8_t KeyId);
void WriteKeySettings(uint8_t AppSlot, uint8_t KeyId, BYTE Value);
BYTE ReadKeyVersion(uint8_t AppSlot, uint8_t KeyId);
void WriteKeyVersion(uint8_t AppSlot, uint8_t KeyId, BYTE Value);
SIZET ReadKeyStorageAddress(uint8_t AppSlot);
void WriteKeyStorageAddress(uint8_t AppSlot, SIZET Value);
void ReadAppKey(uint8_t AppSlot, uint8_t KeyId, uint8_t *Key, SIZET KeySize);
void WriteAppKey(uint8_t AppSlot, uint8_t KeyId, const uint8_t *Key, SIZET KeySize);

/* Application file management */
BYTE ReadFileCount(uint8_t AppSlot);
void WriteFileCount(uint8_t AppSlot, BYTE FileCount);
BYTE LookupFileNumberIndex(uint8_t AppSlot, BYTE FileNumber);
void WriteFileNumberAtIndex(uint8_t AppSlot, uint8_t FileIndex, BYTE FileNumber);
BYTE ReadFileCommSettings(uint8_t AppSlot, uint8_t FileIndex);
void WriteFileCommSettings(uint8_t AppSlot, uint8_t FileIndex, BYTE CommSettings);
SIZET ReadFileAccessRights(uint8_t AppSlot, uint8_t FileIndex);
void WriteFileAccessRights(uint8_t AppSlot, uint8_t FileIndex, SIZET AccessRights);
DESFireFileTypeSettings ReadFileSettings(uint8_t AppSlot, uint8_t FileIndex);
void WriteFileSettings(uint8_t AppSlot, uint8_t FileIndex, DESFireFileTypeSettings *FileSettings);

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
TransferStatus GetApplicationIdsTransfer(uint8_t *Buffer);
uint16_t GetApplicationIdsIterator(uint8_t *Buffer, uint16_t ByteCount);

#endif
