/* ErrorHandling.h */

#ifndef __LOCAL_ERROR_HANDLING_H__
#define __LOCAL_ERROR_HANDLING_H__

#include <string.h>
#include <stdint.h>
#include <stderr.h>
#include <errno.h>

#include "ANSIStringPrinting.h"

#define STRING_BUFFER_SIZE          (256)
static inline char __InternalLoggingStringBuffer[STRING_BUFFER_SIZE] = { \0 };

#define GetSourceFileLoggingData(outputDataBuf, maxBufSize)      ({    \
        char *strBuffer;                                               \
        do {                                                           \
        snprintf(outputDataBuf, maxBufSize,                            \
                 "@@ LINE #%d in *%s @@",                              \
                 __LINE__, __FILE__);                                  \
        outputDataBuf[maxBufSize - 1] = '\0';                          \
        } while(0);                                                    \
        strBuffer = outputDataBuf;                                     \
        strBuffer;                                                     \
        })

#define GetSymbolNameString(symbolName, outputDataBuf, maxBufSize)  ({ \
        char *strBuffer = NULL;                                        \
        do {                                                           \
        strncpy(outputDataBuf, #symbolName, maxBufSize);               \
        outputDataBuf[maxBufSize - 1] = '\0';                          \
        } while(0);                                                    \
        strBuffer = outputDataBuf;                                     \
        strBuffer;                                                     \
        })

#define AppendSymbolNameString(symbolName, outputBuf, maxBufSize)   ({     \
        size_t bufLength = strlen(outputBuf);                              \
        GetSymbolNameString(symbolName, outputBuf + bufLength, maxBufSize) \
        })

typedef enum {
     NO_ERROR = 0, 
     LIBC_ERROR, 
     LIBNFC_ERROR,
     GENERIC_OTHER_ERROR,
     AES_AUTH_FAILED,
     AES_AUTH_FAILED_STEP1,
     AES_AUTH_FAILED_STEP2,
     AES_AUTH_FAILED_STEP3,
     AES_AUTH_FAILED_STEP4,
     LAST_ERROR,
} ErrorType_t;

static inline const char *LOCAL_ERROR_MSGS[] {
     [NO_ERROR]                    = "No error",
     [LIBC_ERROR]                  = "Libc function error",
     [GENERIC_OTHER_ERROR]         = "Unspecified (generic) error",
     [AES_AUTH_FAILED]             = "AES auth procedure failed (generic)",
     [AES_AUTH_FAILED_STEP1]       = "AES auth failed ... reason",
     [AES_AUTH_FAILED_STEP2]       = "AES auth failed ... reason",
     [AES_AUTH_FAILED_STEP3]       = "AES auth failed ... reason",
     [AES_AUTH_FAILED_STEP4]       = "AES auth failed ... reason",
     [LAST_ERROR]                  = NULL,
};

extern bool RUNTIME_QUIET_MODE;
extern bool RUNTIME_VERBOSE_MODE;

#endif
