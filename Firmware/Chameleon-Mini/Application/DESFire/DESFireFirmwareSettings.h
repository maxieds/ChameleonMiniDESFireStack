/* 
 * DESFireFirmwareSettings.h : 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_FIRMWARE_SETTINGS_H__
#define __DESFIRE_FIRMWARE_SETTINGS_H__

#include <stdint.h>

#define DESFIRE_FIRMWARE_DEBUGGING                  (1)

#define DESFIRE_FIRMWARE_BUILD_TIMESTAMP            (BUILD_DATE)
#define DESFIRE_FIRMWARE_GIT_COMMIT_ID              (COMMIT_ID)
#define DESFIRE_FIRMWARE_REVISION                   ("0.0.1")

#define DESFIRE_LITTLE_ENDIAN                       (1)

#define DESFIRE_PICC_LAYOUT_REVISION                (0x01)
#define DESFIRE_PICC_STRUCT_PACKING                 __attribute__((packed))
#define DESFIRE_FIRMWARE_PACKING                    __attribute__((packed))
#define DESFIRE_PICC_ARRAY_ALIGNAT                  __attribute__((aligned(1))
#define DESFIRE_FIRMWARE_ARRAY_ALIGNAT              __attribute__((aligned(1))
#define DESFIRE_FIRMWARE_ENUM_PACKING               __attribute__((__packed__, __aligned__(1)))

/* Some standard boolean interpreted and other values for types and return values: */
typedef int      BOOL;
typedef uint8_t  BYTE;
typedef uint16_t NIBBLE;
typedef uint16_t SIZET;
typedef uint32_t UINT;

#define TRUE                                         ((BOOL) 0x01)
#define FALSE                                        ((BOOL) 0x00)

#define IsTrue(rcond)                                (rcond != FALSE)
#define IsFalse(rcond)                               (rcond == FALSE)

#define BITS_PER_BYTE                                (8)

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

#endif 
