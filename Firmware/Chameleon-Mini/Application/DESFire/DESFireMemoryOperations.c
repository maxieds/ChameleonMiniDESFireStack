/* 
 * DESFireMemoryOperations.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "../../Memory.h"
#include "DESFireMemoryOperations.h"
#include "DESFirePICCControl.h"
#include "DESFireFile.h"

void ReadBlockBytes(void* Buffer, uint8_t StartBlock, uint16_t Count) {
    MemoryReadBlock(Buffer, StartBlock * DESFIRE_EEPROM_BLOCK_SIZE, Count);
}

void WriteBlockBytes(const void* Buffer, uint8_t StartBlock, uint16_t Count) {
    MemoryWriteBlock(Buffer, StartBlock * DESFIRE_EEPROM_BLOCK_SIZE, Count);
}

void CopyBlockBytes(uint8_t DestBlock, uint8_t SrcBlock, uint16_t Count) {
    uint8_t Buffer[DESFIRE_EEPROM_BLOCK_SIZE];
    uint16_t SrcOffset = SrcBlock * DESFIRE_EEPROM_BLOCK_SIZE;
    uint16_t DestOffset = DestBlock * DESFIRE_EEPROM_BLOCK_SIZE;

    while (Count--) {
        MemoryReadBlock(Buffer, SrcOffset, Count);
        MemoryWriteBlock(Buffer, DestOffset, Count);
        SrcOffset += DESFIRE_EEPROM_BLOCK_SIZE;
        DestOffset += DESFIRE_EEPROM_BLOCK_SIZE;
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
    uint8_t zeroFillBuffer[BlockCount * DESFIRE_EEPROM_BLOCK_SIZE];
    memset(zeroFillBuffer, 0x00, BlockCount * DESFIRE_EEPROM_BLOCK_SIZE);
    WriteBlockBytes(zeroFillBuffer, Block * DESFIRE_EEPROM_BLOCK_SIZE, BlockCount * DESFIRE_EEPROM_BLOCK_SIZE);
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
        MaxFreeBlocks = 0 - DESFIRE_FIRST_FREE_BLOCK_ID;
        break;
    default:
        return 0;
    }

    return MaxFreeBlocks - Picc.FirstFreeBlock;
}

void ReadDataEEPROMSource(uint8_t* Buffer, uint8_t Count) {
    MemoryReadBlock(Buffer, TransferState.ReadData.Source.Pointer, Count);
    TransferState.ReadData.Source.Pointer += Count;
}

void WriteDataEEPROMSink(uint8_t* Buffer, uint8_t Count) {
    MemoryWriteBlock(Buffer, TransferState.WriteData.Sink.Pointer, Count);
    TransferState.WriteData.Sink.Pointer += Count;
}


