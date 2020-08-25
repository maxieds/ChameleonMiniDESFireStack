/*
The DESFire stack portion of this firmware source 
is free software written by Maxie Dion Schmidt: 
you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

The complete license provided with source distributions of this library is available at the following link:
https://github.com/maxieds/ChameleonMiniFirmwareDESFireStack

This notice must be retained at the top of all source files in the repository. 

This source code is only licensed for 
redistribution under the above GPL clause for 
non-commercial users. All commerical use or inclusion of this 
software requires express written consent of the author (MDS). 
This restriction pertains to any binary distributions which 
are derivative works of this software.
*/

/* 
 * DESFireMemoryOperations.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "../../Memory.h"
#include "DESFireMemoryOperations.h"
#include "DESFirePICCControl.h"
#include "DESFireFile.h"

volatile char __InternalStringBuffer[STRING_BUFFER_SIZE] = { 0 };
char __InternalStringBuffer2[DATA_BUFFER_SIZE_SMALL] = { 0 };

void ReadBlockBytes(void* Buffer, SIZET StartBlock, SIZET Count) {
    MemoryReadBlock(Buffer, StartBlock * DESFIRE_EEPROM_BLOCK_SIZE, Count);
}

void WriteBlockBytes(const void* Buffer, SIZET StartBlock, SIZET Count) {
    MemoryWriteBlock(Buffer, StartBlock * DESFIRE_EEPROM_BLOCK_SIZE, Count);
}

void CopyBlockBytes(SIZET DestBlock, SIZET SrcBlock, SIZET Count) {
    uint8_t Buffer[DESFIRE_EEPROM_BLOCK_SIZE];
    uint16_t SrcOffset = SrcBlock; 
    uint16_t DestOffset = DestBlock; 
    while(Count > 0) {
        SIZET bytesToWrite = MIN(Count, DESFIRE_EEPROM_BLOCK_SIZE);
        MemoryReadBlock(Buffer, SrcOffset, bytesToWrite);
        MemoryWriteBlock(Buffer, DestOffset, bytesToWrite);
        SrcOffset += 1; 
        DestOffset += 1; 
        Count -= DESFIRE_EEPROM_BLOCK_SIZE;
    }
}

void SetBlockBytes(SIZET DestBlock, BYTE InitByteValue, SIZET ByteCount) {
     BYTE initValueArray[DESFIRE_EEPROM_BLOCK_SIZE];
     memset(initValueArray, InitByteValue, DESFIRE_EEPROM_BLOCK_SIZE);
     while(ByteCount > 0) {
          SIZET bytesToWrite = MIN(ByteCount, DESFIRE_EEPROM_BLOCK_SIZE);
          WriteBlockBytes(initValueArray, DestBlock, bytesToWrite);
          DestBlock += DESFIRE_EEPROM_BLOCK_SIZE;
          ByteCount -= DESFIRE_EEPROM_BLOCK_SIZE;
     }
}

uint8_t AllocateBlocks(uint8_t BlockCount) {
    uint8_t Block;
    /* Check if we have space */
    Block = Picc.FirstFreeBlock;
    if (Block + BlockCount < Block) {
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


