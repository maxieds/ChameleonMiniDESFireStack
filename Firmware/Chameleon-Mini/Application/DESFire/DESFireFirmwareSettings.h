/* 
 * DESFireFirmwareSettings.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_FIRMWARE_SETTINGS_H__
#define __DESFIRE_FIRMWARE_SETTINGS_H__

#define DESFIRE_FIRMWARE_BUILD_TIMESTAMP            (BUILD_DATE)
#define DESFIRE_FIRMWARE_GIT_COMMIT_ID              (COMMIT_ID)
#define DESFIRE_FIRMWARE_REVISION                   ("0.0.1")

#define DESFIRE_PICC_LAYOUT_REVISION                (0x01)
#define DESFIRE_PICC_STRUCT_PACKING                 __attribute__((packed))
#define DESFIRE_FIRMWARE_PACKING                    __attribute__((packed))
#define DESFIRE_PICC_ARRAY_ALIGNAT                  __attribute__((aligned(1))
#define DESFIRE_FIRMWARE_ARRAY_ALIGNAT              __attribute__((aligned(1))

/* Some standard boolean interpreted and other values for types and return values: */
typedef int      BOOL;
typedef uint8_t  BYTE;
typedef uint16_t NIBBLE;
typedef uint16_t SIZET;
typedef uint32_t UINT;

inline const BOOL TRUE  = 1;
inline const BOOL FALSE = 0;

#define IsTrue(rcond)                                (rcond == TRUE)
#define IsFalse(rcond)                               (rcond != TRUE)

/* Allow users to modify typically reserved and protected read-only data on the tag 
   like the manufacturer bytes and the serial number (set in Makefile)? */
#ifdef ENABLE_PERMISSIVE_DESFIRE_SETTINGS
     #define DESFIRE_ALLOW_PROTECTED_MODIFY         (1)
#else
     #define DESFIRE_ALLOW_PROTECTED_MODIFY         (0)
#endif

/* Whether to auto select application ID and file before the user (input system) 
   even invokes an ISO SELECT APPLICATION [0x00 0xa4 0x04] command? */
#define DESFIRE_LEGACY_SUPPORT                      (1)

/* Allow multiple default AID byte strings, and even provide for users to "spoof" another 
   one of their own choosing while still emulating a DESFire tag: */

inline const BYTE DEFAULT_DESFIRE_AID[] = { 0xd2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x00 };
inline const BYTE DEFAULT_ISO7816_AID[] = { 0xa0, 0x00, 0x00, 0x00, 0x03, 0x96 };

#define MAX_AID_SIZE                                (12)
extern BYTE CUSTOM_DESFIRE_AID[MAX_AID_SIZE];

/* Define the operating protocols under which we emulate the tag: */
typedef enum {
     DFPROTO_NATIVE         = 0, 
     DFPROTO_WRAPPED_NATIVE = 1, 
     DFPROTO_ISO            = 2, 
} DESFireProtocol;

#define DEFAULT_DESFIRE_OPERATING_PROTOCOL          (DFPROTO_NATIVE)

/* Define the modes of communication over the RFID channel: */ 
typedef enum {
     COMMTYPE_PLAIN      = 0, 
     COMMTYPE_MAC        = 1, 
     COMMTYPE_ENCIPHERED = 3,
} DESFireCommType;

#define DEFAULT_DESFIRE_COMMTYPE                    (COMMTYPE_PLAIN)

#endif 
