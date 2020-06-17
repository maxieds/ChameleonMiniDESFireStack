/* 
 * DESFireCrypto.h 
 * Maxie D. Schmidt (github.com/maxieds) 
 */

#ifndef __DESFIRE_CRYPTO_H__
#define __DESFIRE_CRYPTO_H__

#include "../../Common.h"
#include "DESFireFirmwareSettings.h"
#include "DESFireAPDU.h"

#include "ExternalCryptoLib/AVRCryptoLib/aes/aes.h"
#include "ExternalCryptoLib/AVRCryptoLib/bcal/bcal-aes.h"
#include "ExternalCryptoLib/AVRCryptoLib/bcal/bcal-cmac.h"
#include "ExternalCryptoLib/AVRCryptoLib/des/des.h"

/* Communication modes: 
 * Define the modes of communication over the RFID channel
 * 
 * Note there is also an AES wrapped COMMS variant called 
 * LRP Secure Messaging detailed starting on page 37 
 * (Section 7.2) of 
 * https://www.nxp.com/docs/en/application-note/AN12343.pdf
 */
#define DESFIRE_COMMS_PLAINTEXT            (0x00)
#define DESFIRE_COMMS_PLAINTEXT_MAC        (0x01)
#define DESFIRE_COMMS_CIPHERTEXT_DES       (0x03)
#define DESFIRE_COMMS_CIPHERTEXT_AES128    (0x04)
#define DESFIRE_COMMS_CIPHERTEXT_AES192    (0x14)
#define DESFIRE_COMMS_CIPHERTEXT_AES256    (0x24)
#define DESFIRE_DEFAULT_COMMS_STANDARD     (DESFIRE_COMMS_PLAINTEXT)

#define CRYPTO_TYPE_ANY         (0x00)
#define CRYPTO_TYPE_DES         (0x01)
#define CRYPTO_TYPE_2K3DES      (0x0A)
#define CRYPTO_TYPE_3K3DES      (0x1A)
#define CRYPTO_TYPE_AES128      (0x4A) /* AES key size determined by the initial auth buffer size */
#define CRYPTO_TYPE_AES192      (0x5A)
#define CRYPTO_TYPE_AES256      (0x6A)

#define CryptoTypeAES(ct) \
    ((ct == CRYPTO_TYPE_AES128) || (ct == CRYPTO_TYPE_AES192) || (ct == CRYPTO_TYPE_AES256))

/* Key sizes, block sizes (in bytes): */
#define CRYPTO_DES_KEY_SIZE                  (8)
#define CRYPTO_2KTDEA_KEY_SIZE               (2 * CRYPTO_DES_KEY_SIZE)
#define CRYPTO_3KTDEA_KEY_SIZE               (3 * CRYPTO_DES_KEY_SIZE)
#define CRYPTO_AES_KEY_SIZE                  (16)
#define CRYPTO_MAX_KEY_SIZE                  (32)
#define CRYPTO_DES_BLOCK_SIZE                (8) 
#define CRYPTO_3KTDEA_BLOCK_SIZE             (CRYPTO_DES_BLOCK_SIZE)
#define CRYPTO_AES_BLOCK_SIZE                (16)
#define CRYPTO_MAX_BLOCK_SIZE                (16)
#define DESFIRE_DES_IV_SIZE                  (CRYPTO_DES_BLOCK_SIZE)
#define DESFIRE_AES_IV_SIZE                  (CRYPTO_AES_BLOCK_SIZE)
#define DESFIRE_SESSION_KEY_SIZE             (CRYPTO_3KTDEA_KEY_SIZE)
#define CRYPTO_CHALLENGE_RESPONSE_BYTES      (8)

typedef BYTE Crypto2KTDEAKeyType[CRYPTO_2KTDEA_KEY_SIZE];
typedef BYTE Crypto3KTDEAKeyType[CRYPTO_3KTDEA_KEY_SIZE];
typedef BYTE CryptoKeyBufferType[CRYPTO_MAX_KEY_SIZE];
typedef BYTE CryptoIVBufferType[CRYPTO_MAX_BLOCK_SIZE];

extern CryptoKeyBufferType SessionKey;
extern CryptoIVBufferType SessionIV;
extern BYTE SessionIVByteSize;

extern uint8_t Authenticated;
extern uint8_t AuthenticatedWithKey;
extern uint8_t AuthenticatedWithPICCMasterKey;
extern uint8_t CryptoAuthMethod;
extern uint8_t ActiveCommMode;

/* TODO: 
 * Need to invalidate the authentication state after:
 * 1) Selecting a new application; 
 * 2) Changing the active key used in the authentication;
 * 3) A failed authentication;
 */
void InvalidateAuthState(BYTE keepPICCAuthData);

BYTE GetDefaultCryptoMethodKeySize(uint8_t cryptoType);

#define MAC_LENGTH          4
#define CMAC_LENGTH         8

/* Authentication types support */
#define DESFIRE_2KTDEA_NONCE_SIZE CRYPTO_DES_BLOCK_SIZE

/* Authentication status */
#define DESFIRE_NOT_AUTHENTICATED 0xFF

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    DESFIRE_AUTH_LEGACY,
    DESFIRE_AUTH_ISO_2KTDEA,
    DESFIRE_AUTH_ISO_3KTDEA,
    DESFIRE_AUTH_AES,
} DesfireAuthType;

BYTE GetCryptoKeyTypeFromAuthenticateMethod(BYTE authCmdMethod);

/*********************************************************
 * AES (128/192/256) crypto routines: 
 *********************************************************/

#define CryptoBitsToBytes(cryptoBits) \
     (cryptoBits / BITS_PER_BYTE)

typedef struct DESFIRE_FIRMWARE_PACKING {
     union {
          aes128_ctx_t aes128Context;
          aes192_ctx_t aes192Context;
          aes256_ctx_t aes256Context;
     };
     uint16_t keySizeBytes;
} DesfireAESCryptoContext;
extern DesfireAESCryptoContext AESCryptoContext;

typedef struct DESFIRE_FIRMWARE_PACKING {
     union {
          uint8_t aes128Key[16];
          uint8_t aes192Key[24];
          uint8_t aes256Key[32];
     };
} DesfireAESCryptoKey;
extern DesfireAESCryptoKey AESCryptoSessionKey;
extern DesfireAESCryptoKey AESCryptoIVBuffer;

void InitAESCryptoContext(DesfireAESCryptoContext *cryptoCtx);
void InitAESCryptoKeyData(DesfireAESCryptoKey *cryptoKeyData);
uint8_t * ExtractAESKeyBuffer(DesfireAESCryptoKey *cryptoKey, BYTE keySizeBytes);
uint16_t GetPaddedBufferSize(uint16_t bufSize);
uint8_t DesfireAESCryptoInit(uint8_t *initKeyBuffer, uint16_t bufSize, DesfireAESCryptoContext *cryptoCtx);
uint8_t DesfireAESEncryptBuffer(DesfireAESCryptoContext *cryptoCtx, uint8_t *plainSrcBuf, 
                                uint8_t *encDestBuf, uint16_t bufSize);
uint8_t DesfireAESDecryptBuffer(DesfireAESCryptoContext *cryptoCtx, uint8_t *encSrcBuf, 
                                uint8_t *plainDestBuf, uint16_t bufSize);

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
     AESAUTH_STATE_IDLE = 0,
     AESAUTH_STATE_CHALLENGEI_RECV,
     AESAUTH_STATE_RNDB_SENT2PCD,
     AESAUTH_STATE_CHALLENGEII_RECV,
     AESAUTH_STATE_AUTHENTICATED,
     AESAUTH_STATE_ERROR,
     AESAUTH_STATE_HALT_RESET,
} DesfireAESAuthState;
extern DesfireAESAuthState AESAuthState;

#define DESFIRE_MAX_PAYLOAD_AES_BLOCKS        (DESFIRE_MAX_PAYLOAD_SIZE / CRYPTO_AES_BLOCK_SIZE)

typedef uint8_t (*CryptoTransferSendFunc)(uint8_t *Buffer, uint8_t Count);
typedef uint8_t (*CryptoTransferReceiveFunc)(uint8_t *Buffer, uint8_t Count);

uint8_t TransferEncryptAESCryptoSend(uint8_t *Buffer, uint8_t Count);
uint8_t TransferEncryptAESCryptoReceive(uint8_t *Buffer, uint8_t Count);

typedef void (*CryptoAESCBCFuncType)(uint16_t, const void*, void*, void*, DesfireAESCryptoContext*);

void CryptoEncryptAES_CBCSend(uint16_t Count, const void *PlainText, void *CipherText, 
                              void *IV, DesfireAESCryptoContext *AESCryptoContextData);
void CryptoDecryptAES_CBCSend(uint16_t Count, const void *PlainText, void *CipherText, 
                              void *IV, DesfireAESCryptoContext *AESCryptoContextData);
void CryptoEncryptAES_CBCReceive(uint16_t Count, const void *PlainText, void *CipherText, 
                                 void *IV, DesfireAESCryptoContext *AESCryptoContextData);
void CryptoDecryptAES_CBCReceive(uint16_t Count, const void *PlainText, void *CipherText, 
                                 void *IV, DesfireAESCryptoContext *AESCryptoContextData);

/* CMAC local implementation */
typedef bcal_cmac_ctx_t DesfireAESCryptoCMACContext;
extern DesfireAESCryptoCMACContext AESCryptoChecksumContext;

BYTE InitAESCryptoCMACContext(DesfireAESCryptoCMACContext *cmacCtx, DesfireAESCryptoContext *cryptoCtx);
void CalculateAESCryptoCMAC(BYTE *cmacDestBytes, const BYTE *srcBuf, SIZET bufSize, 
                            DesfireAESCryptoCMACContext *cmacCtx);

/* Public checksum routines: */
void TransferChecksumUpdateCMAC(const uint8_t *Buffer, uint8_t Count);
uint8_t TransferChecksumFinalCMAC(uint8_t *Buffer);

/*********************************************************
 * TripleDES crypto routines: 
 *********************************************************/

/* Checksum routines: */
void TransferChecksumUpdateCRCA(const uint8_t *Buffer, uint8_t Count);
uint8_t TransferChecksumFinalCRCA(uint8_t *Buffer);
void TransferChecksumUpdateMACTDEA(const uint8_t *Buffer, uint8_t Count);
uint8_t TransferChecksumFinalMACTDEA(uint8_t *Buffer);

#define DESFIRE_MAX_PAYLOAD_TDEA_BLOCKS (DESFIRE_MAX_PAYLOAD_SIZE / CRYPTO_DES_BLOCK_SIZE)

/* Encryption routines */
uint8_t TransferEncryptTDEASend(uint8_t *Buffer, uint8_t Count);
uint8_t TransferEncryptTDEAReceive(uint8_t *Buffer, uint8_t Count);

/* Notes on cryptography in DESFire cards

The EV0 was the first chip in the DESFire series. It makes use of the TDEA
encryption to secure the comms. It also employs CBC to make things more secure.

CBC is used in two "modes": "send mode" and "receive mode".
- Sending data uses the same structure as conventional encryption with CBC
  (XOR with the IV then apply block cipher)
- Receiving data uses the same structure as conventional decryption with CBC
  (apply block cipher then XOR with the IV)

Both operations employ TDEA encryption on the PICC's side and decryption on
PCD's side.

*/

/* Prototype the CBC function pointer in case anyone needs it */
typedef void (*CryptoTDEACBCFuncType)(uint16_t Count, const void *PlainText, 
                                      void *Ciphertext, void *IV, const uint8_t *Keys);

/** Performs the Triple DEA enciphering in ECB mode (single block)
 *
 * \param Plaintext     Source buffer with plaintext
 * \param Ciphertext    Destination buffer to contain ciphertext
 * \param Keys          Key block pointer (CRYPTO_2KTDEA_KEY_SIZE)
 */
void CryptoEncrypt2KTDEA(const void* Plaintext, void* Ciphertext, const uint8_t* Keys);
void CryptoDecrypt2KTDEA(const void* Plaintext, void* Ciphertext, const uint8_t* Keys);

/** Performs the 2-key Triple DES en/deciphering in the CBC "send" mode (xor-then-crypt)
 *
 * \param Count         Block count, expected to be >= 1
 * \param Plaintext     Source buffer with plaintext
 * \param Ciphertext    Destination buffer to contain ciphertext
 * \param IV            Initialization vector buffer, will be updated
 * \param Keys          Key block pointer (CRYPTO_2KTDEA_KEY_SIZE)
 */
void CryptoEncrypt2KTDEA_CBCSend(uint16_t Count, const void* Plaintext, void* Ciphertext, 
                                 void *IV, const uint8_t* Keys);
void CryptoDecrypt2KTDEA_CBCSend(uint16_t Count, const void* Plaintext, void* Ciphertext, 
                                 void *IV, const uint8_t* Keys);

/** Performs the 2-key Triple DES en/deciphering in the CBC "receive" mode (crypt-then-xor)
 *
 * \param Count         Block count, expected to be >= 1
 * \param Plaintext     Source buffer with plaintext
 * \param Ciphertext    Destination buffer to contain ciphertext
 * \param IV            Initialization vector buffer, will be updated
 * \param Keys          Key block pointer (CRYPTO_2KTDEA_KEY_SIZE)
 */
void CryptoEncrypt2KTDEA_CBCReceive(uint16_t Count, const void* Plaintext, void* Ciphertext, 
                                    void *IV, const uint8_t* Keys);
void CryptoDecrypt2KTDEA_CBCReceive(uint16_t Count, const void* Plaintext, void* Ciphertext, 
                                    void *IV, const uint8_t* Keys);

void CryptoEncrypt3KTDEA(const void* Plaintext, void* Ciphertext, const uint8_t* Keys);
void CryptoDecrypt3KTDEA(const void* Plaintext, void* Ciphertext, const uint8_t* Keys);

/** Performs the 3-key Triple DES en/deciphering in the CBC "send" or "receive" mode
 *
 * \param Count         Block count, expected to be >= 1
 * \param Plaintext     Source buffer with plaintext
 * \param Ciphertext    Destination buffer to contain ciphertext
 * \param IV            Initialization vector buffer, will be updated
 * \param Keys          Key block pointer (CRYPTO_3KTDEA_KEY_SIZE)
 */
void CryptoEncrypt3KTDEA_CBCSend(uint16_t Count, const void* Plaintext, void* Ciphertext, 
                                 void *IV, const uint8_t* Keys);
void CryptoDecrypt3KTDEA_CBCSend(uint16_t Count, const void* Plaintext, void* Ciphertext, 
                                 void *IV, const uint8_t* Keys);
void CryptoEncrypt3KTDEA_CBCSend(uint16_t Count, const void* Plaintext, void* Ciphertext, 
                                 void *IV, const uint8_t* Keys);
void CryptoDecrypt3KTDEA_CBCReceive(uint16_t Count, const void* Plaintext, void* Ciphertext, 
                                    void *IV, const uint8_t* Keys);

/** Applies padding to the data within the buffer
 *
 * \param Buffer 		Data buffer to pad
 * \param BytesInBuffer	How much data there is in the buffer already
 * \param FirstPaddingBitSet Whether the very first bit (MSB) will be set or not
 */
void CryptoPaddingTDEA(uint8_t* Buffer, uint8_t BytesInBuffer, bool FirstPaddingBitSet);

#endif
