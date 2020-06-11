/* 
 * DESFireEndianUtil.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */ 

/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __DESFIRE_ENDIAN_UTIL_H__
#define __DESFIRE_ENDIAN_UTIL_H__

#include <inttypes.h>
#include <endian.h>

/* 
 * Chameleon RevG Board FYI: *ATXMEGA128* is *LITTLE ENDIAN* for the byte order. 
 * CPU is assumed to be little endian. Edit this file if you
 * need to port this library to a big endian CPU.
 */
#if   __BYTE_ORDER == __BIG_ENDIAN 
     #define CRYPTO_LITTLE_ENDIAN        (0)
#elif __BYTE_ORDER == __LITTLE_ENDIAN
     #define CRYPTO_LITTLE_ENDIAN        (1)
#else 
     #include "DESFireFirmwareSettings.h"
     #define CRYPTO_LITTLE_ENDIAN        (DESFIRE_LITTLE_ENDIAN)
#endif

#if !defined(le32toh) && defined(letoh32)
    #define le32toh(x) letoh32(x)
    #define be32toh(x) betoh32(x)
#endif

#if !defined(le16toh) && defined(letoh16)
    #define le16toh(x) letoh16(x)
    #define be16toh(x) betoh16(x)
#endif

#if defined(CRYPTO_LITTLE_ENDIAN) && CRYPTO_LITTLE_ENDIAN == 1
     #define htole16(x)  (x)
     #define le16toh(x)  (x)
     #define htobe16(x)  \
         (__extension__ ({ \
             uint16_t _temp = (x); \
             ((_temp >> 8) & 0x00FF) | \
             ((_temp << 8) & 0xFF00); \
         }))
     #define be16toh(x)  (htobe16((x)))
     #define htole32(x)  (x)
     #define le32toh(x)  (x)
     #define htobe32(x)  \
         (__extension__ ({ \
             uint32_t _temp = (x); \
             ((_temp >> 24) & 0x000000FF) | \
             ((_temp >>  8) & 0x0000FF00) | \
             ((_temp <<  8) & 0x00FF0000) | \
             ((_temp << 24) & 0xFF000000); \
         }))
     #define be32toh(x)  (htobe32((x)))
     #define htole64(x)  (x)
     #define le64toh(x)  (x)
     #define htobe64(x)  \
         (__extension__ ({ \
             uint64_t __temp = (x); \
             uint32_t __low = htobe32((uint32_t)__temp); \
             uint32_t __high = htobe32((uint32_t)(__temp >> 32)); \
             (((uint64_t)__low) << 32) | __high; \
         }))
     #define be64toh(x)  (htobe64((x)))
#else 
    /* Note: Code borrowed from libfreefare 
     *       (https://github.com/nfc-tools/libfreefare/blob/master/libfreefare/freefare_internal.h) 
     */
     #include <byteswap.h>
     #if !defined(le32toh) && defined(bswap_32)
          #define be32toh(x) (x)
	  #define htobe32(x) (x)
	  #define le32toh(x) bswap_32(x)
	  #define htole32(x) bswap_32(x)
     #endif
     #if !defined(htole16) && defined(bswap_16)
          #define be16toh(x) (x)
	  #define htobe16(x) (x)
	  #define htole16(x) (bswap_16(x))
	  #define le16toh(x) (bswap_16(x))
     #endif
#endif

#endif 
