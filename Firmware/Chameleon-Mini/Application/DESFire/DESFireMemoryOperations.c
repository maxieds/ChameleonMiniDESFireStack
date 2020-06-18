/* 
 * DESFireMemoryOperations.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "../../Memory.h"
#include "DESFireMemoryOperations.h"
#include "DESFirePICCControl.h"
#include "DESFireFile.h"

BYTE __InternalDataBuffer[MAX_DATA_BUFFER_SIZE] = { 0 };
char __InternalStringBuffer[STRING_BUFFER_SIZE] = { 0 };
char __InternalStringBuffer2[DATA_BUFFER_SIZE_SMALL] = { 0 };

void ReadBlockBytes(void* Buffer, SIZET StartBlock, SIZET Count) {
    MemoryReadBlock(Buffer, StartBlock * DESFIRE_EEPROM_BLOCK_SIZE, Count);
}

void WriteBlockBytes(const void* Buffer, SIZET StartBlock, SIZET Count) {
    MemoryWriteBlock(Buffer, StartBlock * DESFIRE_EEPROM_BLOCK_SIZE, Count);
}

void CopyBlockBytes(SIZET DestBlock, SIZET SrcBlock, SIZET Count) {
    uint8_t Buffer[DESFIRE_EEPROM_BLOCK_SIZE];
    uint16_t SrcOffset = SrcBlock * DESFIRE_EEPROM_BLOCK_SIZE;
    uint16_t DestOffset = DestBlock * DESFIRE_EEPROM_BLOCK_SIZE;
    while(Count > 0) {
        SIZET bytesToWrite = MIN(Count, DESFIRE_EEPROM_BLOCK_SIZE);
        MemoryReadBlock(Buffer, SrcOffset, bytesToWrite);
        MemoryWriteBlock(Buffer, DestOffset, bytesToWrite);
        SrcOffset += DESFIRE_EEPROM_BLOCK_SIZE;
        DestOffset += DESFIRE_EEPROM_BLOCK_SIZE;
        Count -= DESFIRE_EEPROM_BLOCK_SIZE;
    }
}

void SetBlockBytes(SIZET DestBlock, BYTE InitByteValue, SIZET Count) {
     BYTE initValueArray[DESFIRE_EEPROM_BLOCK_SIZE];
     memset(initValueArray, InitByteValue, DESFIRE_EEPROM_BLOCK_SIZE);
     while(Count > 0) {
          SIZET bytesToWrite = MIN(Count, DESFIRE_EEPROM_BLOCK_SIZE);
          WriteBlockBytes(initValueArray, DestBlock, bytesToWrite);
          DestBlock += 1;
          Count -= DESFIRE_EEPROM_BLOCK_SIZE;
     }
}

uint8_t AllocateBlocks(uint8_t BlockCount) {
    uint8_t Block;
    /* Check if we have space */
    Block = Picc.FirstFreeBlock;
    if (Block + BlockCount < Block || Block + BlockCount > CardCapacityBlocks) {
        return 0;
    }
    Picc.FirstFreeBlock = Block + BlockCount;
    SynchronizePICCInfo();
    SetBlockBytes(Block, 0x00, BlockCount * DESFIRE_EEPROM_BLOCK_SIZE);
    return Block;
}

uint8_t GetCardCapacityBlocks(void) {
    uint8_t MaxFreeBlocks;

    switch (Picc.StorageSize) {
    case DESFIRE_STORAGE_SIZE_2K:
        MaxFreeBlocks = 0x40 - DESFIRE_FIRST_FREE_BLOCK_ID;
        break;
    case DESFIRE_STORAGE_SIZE_4K:
        MaxFreeBlocks = 0x80 - DESFIRE_FIRST_FREE_BLOCK_ID;
        break;
    case DESFIRE_STORAGE_SIZE_8K:
        MaxFreeBlocks = (BYTE) (0x100 - DESFIRE_FIRST_FREE_BLOCK_ID);
        break;
    default:
        return 0;
    }
    return MaxFreeBlocks - Picc.FirstFreeBlock;
}

void ReadDataEEPROMSource(uint8_t* Buffer, uint8_t Count) {
    MemoryReadBlock(Buffer, TransferState.ReadData.Source.Pointer, Count);
    TransferState.ReadData.Source.Pointer += DESFIRE_BYTES_TO_BLOCKS(Count);
}

void WriteDataEEPROMSink(uint8_t* Buffer, uint8_t Count) {
    MemoryWriteBlock(Buffer, TransferState.WriteData.Sink.Pointer, Count);
    TransferState.WriteData.Sink.Pointer += DESFIRE_BYTES_TO_BLOCKS(Count);
}


