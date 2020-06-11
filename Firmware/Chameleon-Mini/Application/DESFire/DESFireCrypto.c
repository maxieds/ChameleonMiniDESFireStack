/* 
 * DESFireCrypto.c
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include <string.h>

BYTE ActuveCommMode = DESFIRE_COMMS_PLAINTEXT;

const BYTE InitialMasterKeyDataDES[CRYPTO_DES_KEY_SIZE] = { 
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};
const BYTE InitialMasterKeyDataAES[CRYPTO_AES_KEY_SIZE] = { 
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};
const BYTE InitialMasterKeyData3KTDEA[CRYPTO_AES_KEY_SIZE] = { 
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

BYTE NO_KEY_AUTHENTICATED = 0xff;
BYTE CHECKSUM_IV[4] = { 
     0x00, 0x00, 0x00, 0x00 
};

CryptoKey DefaultCryptoKey;
BYTE SessionKey[DESFIRE_CRYPTO_SESSION_KEY_SIZE];
BYTE SessionIV[DESFIRE_CRYPTO_IV_SIZE];

BOOL InitDESFireKey(CryptoKey &ckey, SIZET ksize, SIZET rbsize, SIZET bsize, BYTE cmethod) {
     ckey.keySize = ksize;
     ckey.randomBlockSize = rbsize;
     ckey.blockSize = bsize;
     ckey.cryptoMethod = cmethod;
     ckey.keyData = GetDefaultKeyBuffer(cmethod);
     return TRUE;
}

BYTE * GetDefaultKeyBuffer(BYTE keyType) {
     switch(keyType) {
          case CRYPTO_TYPE_2K3DES:
               return (BYTE *) InitialMasterKeyDataAES;
          case CRYPTO_3K3DES:
               return (BYTE *) InitialMasterKeyData3KTDEA;
          case CRYPTO_TYPE_AES:
               return (BYTE *) InitialMasterKeyDataAES;
          case CRYPTO_TYPE_DES:
          default:
               return (BYTE *) InitialMasterKeyDataDES;
     }
}

// TODO: See LibFreefare implementation ... 
// TODO: Split implementation into calling separate methods by key type ... 
BOOL CreateNewSessionKey(CryptoKey &ckey, BYTE *arrA, BYTE *arrB, BYTE keyType) {
     if(arrA == NULL || arrB == NULL) {
          return ckey;
     }
     switch(keyType) {
          case CRYPTO_DES:
               ckey = InitDESFireKey(8, 4, CRYPTO_DES_BLOCK_SIZE, CRYPTO_DES);
               for(int i = 0; i < 4; i++) {
                    ckey->keyData[i] = arrA[i];
                    ckey->keyData[i + 4] = arrB[i];
               }
               return ckey;
          case CRYPTO_TDES:
               ckey = InitDESFireKey(16, 8, 8, CRYPTO_TDES);
               for(int i = 0; i < 4; i++) {
                    ckey->keyData[i] = arrA[i];
                    ckey->keyData[i + 4] = arrB[i];
               }
               for(int i = 4; i < 8; i++) {
                    ckey->keyData[i + 4] = arrA[i];
                    ckey->keyData[i + 8] = arrB[i];
               }
               return ckey;
          case CRYPTO_TKDES:
               ckey = InitDESFireKey(24, 16, 16, CRYPTO_TKDES);
               for(int i = 0; i < 4; i++) {
                    ckey->keyData[i] = arrA[i];
                    ckey->keyData[i + 4] = arrB[i];
               }
               for(int i = 4; i < 8; i++) {
                    ckey->keyData[i + 4] = arrA[i + 2];
                    ckey->keyData[i + 8] = arrB[i + 2];
               }
               for(int i = 12; i < 16; i++) {
                    ckey->keyData[i + 4] = arrA[i];
                    ckey->keyData[i + 8] = arrB[i];
               }
               return ckey;
          case CRYPTO_AES:
               ckey = InitDESFireKey(16, 16, 16, CRYPTO_AES);
               for(int i = 0; i < 4; i++) {
                    ckey->keyData[i] = arrA[i];
               }
               for(int i = 4; i < 8; i++) {
                    ckey->keyData[i] = arrB[i - 4];
               }
               for(int i = 8; i < 12; i++) {
                    ckey->keyData[i] = arrA[i + 4];
               }
               for(int i = 12; i < 16; i++) {
                    ckey->keyData[i] = arrB[i];
               }
               return ckey;
          default:
               break;
     }
     return TRUE;
}

BYTE GetCryptoKeyTypeFromAuthenticateMethod(BYTE authCmdMethod) {
     switch(authCmdMethod) {
          case AUTHENTICATE_AES:
               return CRYPTO_TYPE_AES;
          case AUTHENTICATE_ISO:
               return CRYPTO_TYPE_3K3DES:
          case AUTHENTICATE_LEGACY:
          default:
               return CRYPTO_TYPE_DES;
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
        TransferState.Checksums.MAC.MACFunc(1, &TransferState.Checksums.MAC.BlockBuffer[0], &TempBuffer[0], SessionIV, SessionKey);
    }
    /* MAC complete blocks in the buffer */
    while (Count >= CRYPTO_DES_BLOCK_SIZE) {
        /* NOTE: This is block-by-block, hence slow. See if it's better to just allocate a temp buffer large enough (64 bytes). */
        TransferState.Checksums.MAC.MACFunc(1, &Buffer[0], &TempBuffer[0], SessionIV, SessionKey);
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
        TransferState.Checksums.MAC.MACFunc(1, &TransferState.Checksums.MAC.BlockBuffer[0], &TempBuffer[0], SessionIV, SessionKey);
        TransferState.Checksums.AvailablePlaintext = 0;
    }
    /* Copy the checksum to destination */
    memcpy(Buffer, SessionIV, 4);
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
    CryptoEncrypt2KTDEA_CBCSend(BlockCount, &TempBuffer[0], &Buffer[0], SessionIV, SessionKey);
    /* Return byte count to transfer */
    return BlockCount * CRYPTO_DES_BLOCK_SIZE;
}

uint8_t TransferEncryptTDEAReceive(uint8_t* Buffer, uint8_t Count) {
     // TODO: Add logging ... 
     return 0;
}


