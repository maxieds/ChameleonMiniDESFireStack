/* 
 * DESFireCrypto.h 
 * Maxie D. Schmidt (github.com/maxieds) 
 */

#ifndef __DESFIRE_CRYPTO_H__
#define __DESFIRE_CRYPTO_H__

#include "../../Common.h"

#include "DESFireFirmwareSettings.h"
#include "DESFireAPDU.h"

/** Cryptography related definitions **/

/* Communication modes */
#define DESFIRE_COMMS_PLAINTEXT         0
#define DESFIRE_COMMS_PLAINTEXT_MAC     1
#define DESFIRE_COMMS_CIPHERTEXT_DES    3

/* Note there is also an AES wrapped COMMS variant called 
 * LRP Secure Messaging detailed starting on page 37 
 * (Section 7.2) of 
 * https://www.nxp.com/docs/en/application-note/AN12343.pdf
 */

/* Define the modes of communication over the RFID channel: */
extern BYTE ActiveCommMode;

#define CRYPTO_TYPE_DES         (0x0A)
#define CRYPTO_TYPE_2K3DES      (0x0A)
#define CRYPTO_TYPE_3K3DES      (0x1A)
#define CRYPTO_TYPE_AES         ((BYTE) 0xAA)

/* Key sizes, block sizes (in bytes): */
#define CRYPTO_DES_KEY_SIZE         (8)
#define CRYPTO_2KTDEA_KEY_SIZE      (2 * CRYPTO_DES_KEY_SIZE)
#define CRYPTO_3KTDEA_KEY_SIZE      (3 * CRYPTO_DES_KEY_SIZE)
#define CRYPTO_AES_KEY_SIZE         (16)
#define CRYPTO_DES_BLOCK_SIZE       (1) // ???: 8
#define CRYPTO_3KTDEA_BLOCK_SIZE    (CRYPTO_DES_BLOCK_SIZE) // ???
#define CRYPTO_AES_BLOCK_SIZE       (4) // ???

#define DESFIRE_DES_IV_SIZE                (CRYPTO_DES_BLOCK_SIZE)
#define DESFIRE_AES_IV_SIZE                (CRYPTO_AES_BLOCK_SIZE)
#define DESFIRE_CRYPTO_SESSION_KEY_SIZE    (CRYPTO_3KTDEA_KEY_SIZE)

typedef uint8_t Desfire2KTDEAKeyType[CRYPTO_2KTDEA_KEY_SIZE];
typedef uint8_t Desfire3KTDEAKeyType[CRYPTO_3KTDEA_KEY_SIZE];

INLINE BYTE GetCryptoMethodKeySize(uint8_t cryptoType) {
     switch(cryptoType) {
          //case CRYPTO_TYPE_DES:
          case CRYPTO_TYPE_2K3DES:
               return CRYPTO_2KTDEA_KEY_SIZE;
          case CRYPTO_TYPE_3K3DES:
               return CRYPTO_3KTDEA_KEY_SIZE;
          case CRYPTO_TYPE_AES:
               return CRYPTO_AES_KEY_SIZE;
          default:
               return 0;
     }
}

#define MAC_LENGTH          4
#define CMAC_LENGTH         8

/* Authentication types support */
#define DESFIRE_2KTDEA_NONCE_SIZE CRYPTO_DES_BLOCK_SIZE

/* Authentication status */
#define DESFIRE_MASTER_KEY_ID     0
#define DESFIRE_NOT_AUTHENTICATED 0xFF

typedef enum DESFIRE_FIRMWARE_ENUM_PACKING {
    DESFIRE_AUTH_LEGACY,
    DESFIRE_AUTH_ISO_2KTDEA,
    DESFIRE_AUTH_ISO_3KTDEA,
    DESFIRE_AUTH_AES,
} DesfireAuthType;

extern DesfireAuthType ActiveAuthType;

extern const BYTE InitialMasterKeyDataDES[CRYPTO_DES_KEY_SIZE]; 
extern const BYTE InitialMasterKeyDataAES[CRYPTO_AES_KEY_SIZE]; 
extern const BYTE InitialMasterKeyData3KTDEA[CRYPTO_3KTDEA_KEY_SIZE]; 

extern BYTE NO_KEY_AUTHENTICATED;
extern BYTE CHECKSUM_IV[4];

// TODO: Remove this???
typedef struct DESFIRE_FIRMWARE_PACKING {
     SIZET keySize;
     SIZET randomBlockSize;
     SIZET blockSize;
     BYTE  cryptoMethod;
     BYTE  *keyData;
} CryptoKey;
extern CryptoKey SessionCryptoKeyData;

typedef union DESFIRE_FIRMWARE_PACKING {
     BYTE LegacyTransfer[DESFIRE_CRYPTO_SESSION_KEY_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
     BYTE IsoTransfer[DESFIRE_CRYPTO_SESSION_KEY_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
     BYTE AESTransfer[CRYPTO_AES_KEY_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
} CryptoSessionKey;

INLINE BYTE* ExtractSessionKeyData(DesfireAuthType authType, CryptoSessionKey *skey) {
     switch(authType) {
          case DESFIRE_AUTH_LEGACY:
               return skey->LegacyTransfer;
          case DESFIRE_AUTH_AES:
               return skey->AESTransfer;
          default:
               return skey->IsoTransfer;
     }
}

typedef union DESFIRE_FIRMWARE_PACKING {
     BYTE LegacyTransferIV[DESFIRE_DES_IV_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
     BYTE IsoTransferIV[DESFIRE_DES_IV_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
     BYTE AESTransferIV[DESFIRE_AES_IV_SIZE] DESFIRE_FIRMWARE_ARRAY_ALIGNAT;
} CryptoIVBuffer;

INLINE BYTE* ExtractIVBufferData(DesfireAuthType authType, CryptoIVBuffer *ivBuf) {
     switch(authType) {
          case DESFIRE_AUTH_LEGACY:
               return ivBuf->LegacyTransferIV;
          case DESFIRE_AUTH_AES:
               return ivBuf->AESTransferIV;
          default:
               return ivBuf->IsoTransferIV;
     }
}

extern CryptoSessionKey SessionKey;
extern CryptoIVBuffer SessionIV;

BOOL InitDESFireKey(CryptoKey *ckey, SIZET ksize, SIZET rbsize, SIZET bsize, BYTE cmethod);
BYTE * GetDefaultKeyBuffer(BYTE keyType);

// TODO: See LibFreefare implementation ... 
// TODO: Split implementation into calling separate methods by key type ... 
//BOOL CreateNewSessionKey(CryptoKey *ckey, BYTE *arrA, BYTE *arrB, BYTE keyType);
//SIZET BuildSessionKey(CryptoKey *keyData, BYTE *arrA, BYTE *arrB);

SIZET CRC16(BYTE *inputData, SIZET dataLength);
UINT  CRC32(BYTE *inputData, SIZET dataLength);

BYTE GetCryptoKeyTypeFromAuthenticateMethod(BYTE authCmdMethod);

typedef enum {
    MCD_SEND,
    MCD_RECEIVE
} DESFireCryptoDirection;

/* Checksum routines: */
// TODO: Need AES equivalents? 
void TransferChecksumUpdateCRCA(const uint8_t *Buffer, uint8_t Count);
uint8_t TransferChecksumFinalCRCA(uint8_t *Buffer);
void TransferChecksumUpdateMACTDEA(const uint8_t *Buffer, uint8_t Count);
uint8_t TransferChecksumFinalMACTDEA(uint8_t *Buffer);

/* Encryption routines */
// TODO: Need AES equivalents? 
#define DESFIRE_MAX_PAYLOAD_TDEA_BLOCKS (DESFIRE_MAX_PAYLOAD_SIZE / CRYPTO_DES_BLOCK_SIZE)

uint8_t TransferEncryptTDEASend(uint8_t *Buffer, uint8_t Count);
uint8_t TransferEncryptTDEAReceive(uint8_t *Buffer, uint8_t Count);

/** The following is modified from devzzo's DESFire firmware implementation sources: **/
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
typedef void (*CryptoTDEACBCFuncType)(uint16_t Count, const void* Plaintext, void* Ciphertext, void *IV, const uint8_t* Keys);

/** Performs the Triple DEA enciphering in ECB mode (single block)
 *
 * \param Plaintext     Source buffer with plaintext
 * \param Ciphertext    Destination buffer to contain ciphertext
 * \param Keys          Key block pointer (CRYPTO_2KTDEA_KEY_SIZE)
 */
void CryptoEncrypt2KTDEA(const void* Plaintext, void* Ciphertext, const uint8_t* Keys);

/** Performs the 2-key Triple DES en/deciphering in the CBC "send" mode (xor-then-crypt)
 *
 * \param Count         Block count, expected to be >= 1
 * \param Plaintext     Source buffer with plaintext
 * \param Ciphertext    Destination buffer to contain ciphertext
 * \param IV            Initialization vector buffer, will be updated
 * \param Keys          Key block pointer (CRYPTO_2KTDEA_KEY_SIZE)
 */
void CryptoEncrypt2KTDEA_CBCSend(uint16_t Count, const void* Plaintext, void* Ciphertext, void *IV, const uint8_t* Keys);
void CryptoDecrypt2KTDEA_CBCSend(uint16_t Count, const void* Plaintext, void* Ciphertext, void *IV, const uint8_t* Keys);

/** Performs the 2-key Triple DES en/deciphering in the CBC "receive" mode (crypt-then-xor)
 *
 * \param Count         Block count, expected to be >= 1
 * \param Plaintext     Source buffer with plaintext
 * \param Ciphertext    Destination buffer to contain ciphertext
 * \param IV            Initialization vector buffer, will be updated
 * \param Keys          Key block pointer (CRYPTO_2KTDEA_KEY_SIZE)
 */
void CryptoEncrypt2KTDEA_CBCReceive(uint16_t Count, const void* Plaintext, void* Ciphertext, void *IV, const uint8_t* Keys);
void CryptoDecrypt2KTDEA_CBCReceive(uint16_t Count, const void* Plaintext, void* Ciphertext, void *IV, const uint8_t* Keys);

/** Performs the 3-key Triple DES en/deciphering in the CBC "send" or "receive" mode
 *
 * \param Count         Block count, expected to be >= 1
 * \param Plaintext     Source buffer with plaintext
 * \param Ciphertext    Destination buffer to contain ciphertext
 * \param IV            Initialization vector buffer, will be updated
 * \param Keys          Key block pointer (CRYPTO_3KTDEA_KEY_SIZE)
 */
void CryptoEncrypt3KTDEA_CBCSend(uint16_t Count, const void* Plaintext, void* Ciphertext, void *IV, const uint8_t* Keys);
void CryptoDecrypt3KTDEA_CBCReceive(uint16_t Count, const void* Plaintext, void* Ciphertext, void *IV, const uint8_t* Keys);

/** Applies padding to the data within the buffer
 *
 * \param Buffer 		Data buffer to pad
 * \param BytesInBuffer	How much data there is in the buffer already
 * \param FirstPaddingBitSet Whether the very first bit (MSB) will be set or not
 */
void CryptoPaddingTDEA(uint8_t* Buffer, uint8_t BytesInBuffer, bool FirstPaddingBitSet);

#endif
