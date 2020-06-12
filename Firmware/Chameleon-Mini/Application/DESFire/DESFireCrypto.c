/* 
 * DESFireCrypto.c
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include <string.h>

#include "DESFireCrypto.h"
#include "DESFireInstructions.h"
#include "DESFirePICCControl.h"
#include "DESFireISO14443Support.h"

BYTE ActveCommMode = DESFIRE_COMMS_PLAINTEXT;

const BYTE InitialMasterKeyDataDES[CRYPTO_DES_KEY_SIZE] = { 
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};
const BYTE InitialMasterKeyDataAES[CRYPTO_AES_KEY_SIZE] = { 
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};
const BYTE InitialMasterKeyData3KTDEA[CRYPTO_3KTDEA_KEY_SIZE] = { 
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

BYTE NO_KEY_AUTHENTICATED = 0xff;
BYTE CHECKSUM_IV[4] = { 
     0x00, 0x00, 0x00, 0x00 
};

DesfireAuthType ActiveAuthType = DESFIRE_AUTH_LEGACY;

CryptoSessionKey SessionKey = { 0 };
CryptoIVBuffer SessionIV = { 0 };

BYTE GetCryptoMethodKeySize(uint8_t cryptoType) {
     switch(cryptoType) {
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

BYTE * ExtractSessionKeyData(DesfireAuthType authType, CryptoSessionKey *skey) {
     switch(authType) {
          case DESFIRE_AUTH_LEGACY:
               return skey->LegacyTransfer;
          case DESFIRE_AUTH_AES:
               return skey->AESTransfer;
          default:
               return skey->IsoTransfer;
     }
}

BYTE * ExtractIVBufferData(DesfireAuthType authType, CryptoIVBuffer *ivBuf) {
     switch(authType) {
          case DESFIRE_AUTH_LEGACY:
               return ivBuf->LegacyTransferIV;
          case DESFIRE_AUTH_AES:
               return ivBuf->AESTransferIV;
          default:
               return ivBuf->IsoTransferIV;
     }
}

BYTE * GetDefaultKeyBuffer(BYTE keyType) {
     switch(keyType) {
          case CRYPTO_TYPE_2K3DES:
               return (BYTE *) InitialMasterKeyDataAES;
          case CRYPTO_TYPE_3K3DES:
               return (BYTE *) InitialMasterKeyData3KTDEA;
          case CRYPTO_TYPE_AES:
               return (BYTE *) InitialMasterKeyDataAES;
          default:
               return (BYTE *) InitialMasterKeyDataDES;
     }
}

BYTE GetCryptoKeyTypeFromAuthenticateMethod(BYTE authCmdMethod) {
     switch(authCmdMethod) {
          case CMD_AUTHENTICATE_AES:
          case CMD_AUTHENTICATE_EV2_FIRST:
          case CMD_AUTHENTICATE_EV2_NONFIRST:
               return CRYPTO_TYPE_AES;
          case CMD_AUTHENTICATE_ISO:
               return CRYPTO_TYPE_3K3DES;
          case CMD_AUTHENTICATE:
          case CMD_ISO7816_EXTERNAL_AUTHENTICATE:
          case CMD_ISO7816_INTERNAL_AUTHENTICATE:
          default:
               return CRYPTO_TYPE_2K3DES;
     }
}

/* Checksum routines */

void TransferChecksumUpdateCRCA(const uint8_t* Buffer, uint8_t Count) {
    TransferState.Checksums.CRCA = ISO14443AUpdateCRCA(Buffer, Count, TransferState.Checksums.CRCA);
}

uint8_t TransferChecksumFinalCRCA(uint8_t* Buffer) {
    /* Copy the checksum to destination */
    memcpy(Buffer, &TransferState.Checksums.CRCA, 2);
    /* Return the checksum size */
    return 2;
}

void TransferChecksumUpdateMACTDEA(const uint8_t* Buffer, uint8_t Count) {
    uint8_t AvailablePlaintext = TransferState.Checksums.AvailablePlaintext;
    uint8_t TempBuffer[CRYPTO_DES_BLOCK_SIZE];

    if (AvailablePlaintext) {
        uint8_t TempBytes;
        /* Fill the partial block */
        TempBytes = CRYPTO_DES_BLOCK_SIZE - AvailablePlaintext;
        if (TempBytes > Count)
            TempBytes = Count;
        memcpy(&TransferState.Checksums.MAC.BlockBuffer[AvailablePlaintext], &Buffer[0], TempBytes);
        Count -= TempBytes;
        Buffer += TempBytes;
        /* MAC the partial block */
        TransferState.Checksums.MAC.MACFunc(1, &TransferState.Checksums.MAC.BlockBuffer[0], 
                                            &TempBuffer[0], SessionIV.LegacyTransferIV, 
                                            SessionKey.LegacyTransfer);
    }
    /* MAC complete blocks in the buffer */
    while (Count >= CRYPTO_DES_BLOCK_SIZE) {
        /* NOTE: This is block-by-block, hence slow. 
         *       See if it's better to just allocate a temp buffer large enough (64 bytes). */
        TransferState.Checksums.MAC.MACFunc(1, &Buffer[0], &TempBuffer[0], 
                                            SessionIV.LegacyTransferIV, 
                                            SessionKey.LegacyTransfer);
        Count -= CRYPTO_DES_BLOCK_SIZE;
        Buffer += CRYPTO_DES_BLOCK_SIZE;
    }
    /* Copy the new partial block */
    if (Count) {
        memcpy(&TransferState.Checksums.MAC.BlockBuffer[0], &Buffer[0], Count);
    }
    TransferState.Checksums.AvailablePlaintext = Count;
}

uint8_t TransferChecksumFinalMACTDEA(uint8_t* Buffer) {
    uint8_t AvailablePlaintext = TransferState.Checksums.AvailablePlaintext;
    uint8_t TempBuffer[CRYPTO_DES_BLOCK_SIZE];

    if (AvailablePlaintext) {
        /* Apply padding */
        CryptoPaddingTDEA(&TransferState.Checksums.MAC.BlockBuffer[0], AvailablePlaintext, false);
        /* MAC the partial block */
        TransferState.Checksums.MAC.MACFunc(1, &TransferState.Checksums.MAC.BlockBuffer[0], 
                                            &TempBuffer[0], SessionIV.LegacyTransferIV, 
                                            SessionKey.LegacyTransfer);
        TransferState.Checksums.AvailablePlaintext = 0;
    }
    /* Copy the checksum to destination */
    memcpy(Buffer, SessionIV.LegacyTransferIV, 4);
    /* Return the checksum size */
    return 4;
}

/* Encryption routines */

uint8_t TransferEncryptTDEASend(uint8_t* Buffer, uint8_t Count) {
    uint8_t AvailablePlaintext = TransferState.ReadData.Encryption.AvailablePlaintext;
    uint8_t TempBuffer[(DESFIRE_MAX_PAYLOAD_TDEA_BLOCKS + 1) * CRYPTO_DES_BLOCK_SIZE];
    uint8_t BlockCount;

    if (AvailablePlaintext) {
        /* Fill the partial block */
        memcpy(&TempBuffer[0], &TransferState.ReadData.Encryption.TDEA.BlockBuffer[0], AvailablePlaintext);
    }
    /* Copy fresh plaintext to the temp buffer */
    memcpy(&TempBuffer[AvailablePlaintext], Buffer, Count);
    Count += AvailablePlaintext;
    BlockCount = Count / CRYPTO_DES_BLOCK_SIZE;
    /* Stash extra plaintext for later */
    AvailablePlaintext = Count - BlockCount * CRYPTO_DES_BLOCK_SIZE;
    if (AvailablePlaintext) {
        memcpy(&TransferState.ReadData.Encryption.TDEA.BlockBuffer[0], &Buffer[BlockCount * CRYPTO_DES_BLOCK_SIZE], AvailablePlaintext);
    }
    TransferState.ReadData.Encryption.AvailablePlaintext = AvailablePlaintext;
    /* Encrypt complete blocks in the buffer */
    CryptoEncrypt2KTDEA_CBCSend(BlockCount, &TempBuffer[0], &Buffer[0], 
                                SessionIV.LegacyTransferIV, 
                                SessionKey.LegacyTransfer);
    /* Return byte count to transfer */
    return BlockCount * CRYPTO_DES_BLOCK_SIZE;
}

uint8_t TransferEncryptTDEAReceive(uint8_t* Buffer, uint8_t Count) {
     // TODO: Add logging ... 
     return 0;
}

void CryptoPaddingTDEA(uint8_t* Buffer, uint8_t BytesInBuffer, bool FirstPaddingBitSet)
{
    uint8_t PaddingByte = FirstPaddingBitSet << 7;
    uint8_t i;

    for (i = BytesInBuffer; i < CRYPTO_DES_BLOCK_SIZE; ++i) {
        Buffer[i] = PaddingByte;
        PaddingByte = 0x00;
    }
}

