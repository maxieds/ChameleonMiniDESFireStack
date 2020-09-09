/* AuthUtils.h */

#ifndef __LOCAL_INIT_AUTH_UTILS_H__
#define __LOCAL_INIT_AUTH_UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "ErrorHandling.h"
#include "StatusCodes.h"
// TODO: CryptoUtils.h ???

typedef enum {
    AUTH_TYPE_NONE = 0,
    AUTH_TYPE_ISO,
    AUTH_TYPE_LEGACY,
    AUTH_TYPE_AES128,
    AUTH_TYPE_DEFAULT
} AuthProtocolType_t;

typedef ErrorType_t STATUS_CODE;

static inline STATUS_CODE PerformAuthenticationNoSelect(AuthProtocolType_t authType) {
    return NO_ERROR;
}

static inline STATUS_CODE PerformAuthentication(AuthProtocolType_t authType) {
    return NO_ERROR;
}

static inline STATUS_CODE PerformAuthentication_AES128(AuthProtocolType_t authType) {
    return NO_ERROR;
}




#endif
