/* 
 * DESFireCrypto.h 
 * Maxie D. Schmidt (github.com/maxieds) 
 */

#ifndef __DESFIRE_CRYPTO_H__
#define __DESFIRE_CRYPTO_H__

#include "../../Common.h"

#include "DESFireFirmwareSettings.h"
#include "DESFireAPDU.h"

/* Communication modes: 
 * Define the modes of communication over the RFID channel
 * 
 * Note there is also an AES wrapped COMMS variant called 
 * LRP Secure Messaging detailed starting on page 37 
 * (Section 7.2) of 
 * https://www.nxp.com/docs/en/application-note/AN12343.pdf
 */

#define DESFIRE_COMMS_PLAINTEXT            0
#define DESFIRE_COMMS_PLAINTEXT_MAC        1
#define DESFIRE_COMMS_CIPHERTEXT_DES       3
#define DESFIRE_COMMS_CIPHERTEXT_AES128    4
#define DESFIRE_COMMS_CIPHERTEXT_AES192    5
#define DESFIRE_COMMS_CIPHERTEXT_AES256    6

#define CRYPTO_TYPE_DES         (0x00)
#define CRYPTO_TYPE_2K3DES      (0x0A)
#define CRYPTO_TYPE_3K3DES      (0x1A)
#define CRYPTO_TYPE_AES         ((BYTE) 0xAA) /* Key size determined by the initial auth buffer */

/* Key sizes, block sizes (in bytes): */
#define CRYPTO_DES_KEY_SIZE         (8)
#define CRYPTO_2KTDEA_KEY_SIZE      (2 * CRYPTO_DES_KEY_SIZE)
#define CRYPTO_3KTDEA_KEY_SIZE      (3 * CRYPTO_DES_KEY_SIZE)
#define CRYPTO_AES_KEY_SIZE         (16)
#define CRYPTO_MAX_KEY_SIZE         (32)
#define CRYPTO_DES_BLOCK_SIZE       (8) 
#define CRYPTO_3KTDEA_BLOCK_SIZE    (CRYPTO_DES_BLOCK_SIZE)
#define CRYPTO_AES_BLOCK_SIZE       (16)
#define CRYPTO_MAX_BLOCK_SIZE       (16)
#define DESFIRE_DES_IV_SIZE         (CRYPTO_DES_BLOCK_SIZE)
#define DESFIRE_AES_IV_SIZE         (CRYPTO_AES_BLOCK_SIZE)
#define DESFIRE_SESSION_KEY_SIZE    (CRYPTO_3KTDEA_KEY_SIZE)

typedef BYTE Crypto2KTDEAKeyType[CRYPTO_2KTDEA_KEY_SIZE];
typedef BYTE Crypto3KTDEAKeyType[CRYPTO_3KTDEA_KEY_SIZE];
typedef BYTE CryptoKeyBufferType[CRYPTO_MAX_KEY_SIZE];
typedef BYTE CryptoIVBufferType[CRYPTO_MAX_BLOCK_SIZE];

extern CryptoIVBufferType SessionIV;
extern BYTE SessionIVByteSize;

BYTE GetCryptoMethodKeySize(uint8_t cryptoType);

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

BYTE GetCryptoKeyTypeFromAuthenticateMethod(BYTE authCmdMethod, SIZET KeyBufferSize);

/* Checksum routines: */
// TODO: Need AES equivalents ... 
void TransferChecksumUpdateCRCA(const uint8_t *Buffer, uint8_t Count);
uint8_t TransferChecksumFinalCRCA(uint8_t *Buffer);
void TransferChecksumUpdateMACTDEA(const uint8_t *Buffer, uint8_t Count);
uint8_t TransferChecksumFinalMACTDEA(uint8_t *Buffer);

/* Encryption routines */
#define DESFIRE_MAX_PAYLOAD_TDEA_BLOCKS (DESFIRE_MAX_PAYLOAD_SIZE / CRYPTO_DES_BLOCK_SIZE)

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
