/* 
 * DESFireISO14443Support.c
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "DESFireISO14443Support.h"

/*
 * ISO/IEC 14443-4 implementation
 */
Iso144434StateType Iso144434State = ISO14443_4_STATE_EXPECT_RATS;
uint8_t Iso144434BlockNumber = 0x00;
uint8_t Iso144434CardID = 0x00;
uint8_t Iso144434LastBlockLength = 0x00;
uint8_t Iso144434LastBlock[CODEC_BUFFER_SIZE];

void ISO144434SwitchState(Iso144434StateType NewState) {
    Iso144434State = NewState;
    LogEntry(LOG_ISO14443_4_STATE, &Iso144434State, 1);
}

void ISO144434Reset(void) {
    /* No logging -- spams the log */
    Iso144434State = ISO14443_4_STATE_EXPECT_RATS;
    Iso144434BlockNumber = 1;
}

uint16_t ISO144434ProcessBlock(uint8_t* Buffer, uint16_t ByteCount) {
    uint8_t PCB = Buffer[0];
    uint8_t MyBlockNumber = Iso144434BlockNumber;
    uint8_t PrologueLength;
    uint8_t HaveCID, HaveNAD;

    /* Verify the block's length: at the very least PCB + CRCA */
    if (ByteCount < (1 + ISO14443A_CRCA_SIZE)) {
        /* Huh? Broken frame? */
        /* TODO: LOG ME */
        //DEBUG_PRINT("\r\nISO14443-4: length fail");
        return ISO14443A_APP_NO_RESPONSE;
    }
    ByteCount -= 2;

    /* Verify the checksum; fail if doesn't match */
    if (!ISO14443ACheckCRCA(Buffer, ByteCount)) {
        LogEntry(LOG_ERR_APP_CHECKSUM_FAIL, Buffer, ByteCount);
        /* ISO/IEC 14443-4, clause 7.5.5. The PICC does not attempt any error recovery. */
        //DEBUG_PRINT("\r\nISO14443-4: CRC fail; %04X vs %04X", *(uint16_t*)&Buffer[ByteCount], 
        //            ISO14443AComputeCRCA(Buffer, ByteCount));
        return ISO14443A_APP_NO_RESPONSE;
    }

    switch (Iso144434State) {
    case ISO14443_4_STATE_EXPECT_RATS:
        /* See: ISO/IEC 14443-4, clause 5.6.1.2 */
        if (Buffer[0] != ISO14443A_CMD_RATS) {
            /* Ignore blocks other than RATS and HLTA */
            return ISO14443A_APP_NO_RESPONSE;
        }
        /* Process RATS.
         * NOTE: ATS bytes are tailored to Chameleon implementation and differ from DESFire spec.
         * NOTE: Some PCD implementations do a memcmp() over ATS bytes, which is completely wrong.
         */
        Iso144434CardID = Buffer[1] & 0x0F;

        Buffer[0] = DESFIRE_EV0_ATS_TL_BYTE;
        Buffer[1] = DESFIRE_EV0_ATS_T0_BYTE;
        Buffer[2] = DESFIRE_EV0_ATS_TA_BYTE;
        Buffer[3] = DESFIRE_EV0_ATS_TB_BYTE;
        Buffer[4] = DESFIRE_EV0_ATS_TC_BYTE;
        Buffer[5] = 0x80; /* T1: dummy value for historical bytes */

        ISO144434SwitchState(ISO14443_4_STATE_ACTIVE);
        ByteCount = Buffer[0];
        break;

    case ISO14443_4_STATE_ACTIVE:
        /* See: ISO/IEC 14443-4; 7.1 Block format */

        /* Parse the prologue */
        PrologueLength = 1;
        HaveCID = PCB & ISO14443_PCB_HAS_CID_MASK;
        if (HaveCID) {
            PrologueLength++;
            /* Verify the card ID */
            if (Buffer[1] != Iso144434CardID) {
                /* Different card ID => the frame is ignored */
                return ISO14443A_APP_NO_RESPONSE;
            }
        }

        switch (PCB & ISO14443_PCB_BLOCK_TYPE_MASK) {
        case ISO14443_PCB_I_BLOCK:
            HaveNAD = PCB & ISO14443_PCB_HAS_NAD_MASK;
            if (HaveNAD) {
                PrologueLength++;
                /* Not currently supported => the frame is ignored */
                /* TODO: LOG ME */
                return ISO14443A_APP_NO_RESPONSE;
            }
            /* 7.5.3.2, rule D: toggle on each I-block */
            Iso144434BlockNumber = MyBlockNumber = !MyBlockNumber;
            if (PCB & ISO14443_PCB_I_BLOCK_CHAINING_MASK) {
                /* Currently not supported => the frame is ignored */
                /* TODO: LOG ME */
                return ISO14443A_APP_NO_RESPONSE;
            }

            /* Build the prologue for the response */
            /* NOTE: According to the std, incoming/outgoing prologue lengths are equal at all times */
            PCB = ISO14443_PCB_I_BLOCK_STATIC | MyBlockNumber;
            if (HaveCID) {
                PCB |= ISO14443_PCB_HAS_CID_MASK;
                Buffer[1] = Iso144434CardID;
            }
            Buffer[0] = PCB;
            /* Let the DESFire application code process the input data */
            ByteCount = MifareDesfireProcess(Buffer + PrologueLength, ByteCount - PrologueLength);
            /* Short-circuit in case the app decides not to respond at all */
            if (ByteCount == ISO14443A_APP_NO_RESPONSE) {
                return ISO14443A_APP_NO_RESPONSE;
            }
            ByteCount += PrologueLength;
            break;

        case ISO14443_PCB_R_BLOCK:
            /* 7.5.4.3, rule 11 */
            if ((PCB & ISO14443_PCB_BLOCK_NUMBER_MASK) == MyBlockNumber) {
                /* NOTE: This already includes the CRC */
                memmove(&Buffer[0], &Iso144434LastBlock[0], Iso144434LastBlockLength);
                return Iso144434LastBlockLength;
            }
            if (PCB & ISO14443_PCB_R_BLOCK_ACKNAK_MASK) {
                /* 7.5.4.3, rule 12 */
                /* This is a NAK. Send an ACK back */
                Buffer[0] = ISO14443_PCB_R_BLOCK_STATIC | ISO14443_PCB_R_BLOCK_ACK | MyBlockNumber;
                ByteCount = 1;
            } else {
                /* This is an ACK */
                /* NOTE: Chaining is not supported yet. */
                return ISO14443A_APP_NO_RESPONSE;
            }
            break;

        case ISO14443_PCB_S_BLOCK:
            if (PCB == ISO14443_PCB_S_DESELECT) {
                /* Reset our state */
                ISO144434Reset();
                LogEntry(LOG_ISO14443_4_STATE, &Iso144434State, 1);
                /* Transition to HALT */
                ISO144433AHalt();
                /* Answer with S(DESELECT) -- just send the copy of the message */
                ByteCount = PrologueLength;
                break;
            }
            return ISO14443A_APP_NO_RESPONSE;
        }
        break;
    }

    ISO14443AAppendCRCA(Buffer, ByteCount);
    /* Stash the block for possible retransmissions */
    ByteCount += ISO14443A_CRCA_SIZE;
    Iso144434LastBlockLength = ByteCount;
    memmove(&Iso144434LastBlock[0], &Buffer[0], ByteCount);
    return ByteCount;
}

/*
 * ISO/IEC 14443-3A implementation
 */

Iso144433AStateType Iso144433AState = ISO14443_3A_STATE_IDLE;
Iso144433AStateType Iso144433AIdleState = ISO14443_3A_STATE_IDLE;

void ISO144433ASwitchState(Iso144433AStateType NewState) {
    Iso144433AState = NewState;
    LogEntry(LOG_ISO14443_3A_STATE, &Iso144433AState, 1);
}

void ISO144433AReset(void) {
    /* No logging -- spams the log */
    Iso144433AState = ISO14443_3A_STATE_IDLE;
    Iso144433AIdleState = ISO14443_3A_STATE_IDLE;
}

void ISO144433AHalt(void) {
    ISO144433ASwitchState(ISO14443_3A_STATE_HALT);
    Iso144433AIdleState = ISO14443_3A_STATE_HALT;
}

bool ISO144433AIsHalt(const uint8_t* Buffer, uint16_t BitCount) {
    return BitCount == ISO14443A_HLTA_FRAME_SIZE + ISO14443A_CRCA_SIZE * 8
        && Buffer[0] == ISO14443A_CMD_HLTA
        && Buffer[1] == 0x00
        && ISO14443ACheckCRCA(Buffer, ISO14443A_HLTA_FRAME_SIZE / 8);
}

uint16_t ISO144433APiccProcess(uint8_t* Buffer, uint16_t BitCount) {
    uint8_t Cmd = Buffer[0];

    /* This implements ISO 14443-3A state machine */
    /* See: ISO/IEC 14443-3, clause 6.2 */
    switch (Iso144433AState) {
    case ISO14443_3A_STATE_HALT:
        if (Cmd != ISO14443A_CMD_WUPA) {
            break;
        }
        /* Fall-through */

    case ISO14443_3A_STATE_IDLE:
        if (Cmd != ISO14443A_CMD_REQA && Cmd != ISO14443A_CMD_WUPA) {
            break;
        }

        Iso144433AIdleState = Iso144433AState;
        ISO144433ASwitchState(ISO14443_3A_STATE_READY1);
        Buffer[0] = (ATQA_VALUE >> 0) & 0x00FF;
        Buffer[1] = (ATQA_VALUE >> 8) & 0x00FF;
        return ISO14443A_ATQA_FRAME_SIZE;

    case ISO14443_3A_STATE_READY1:
        if (Cmd == ISO14443A_CMD_SELECT_CL1) {
            /* Load UID CL1 and perform anticollision. */
            ConfigurationUidType Uid;

            ApplicationGetUid(Uid);
            if (ActiveConfiguration.UidSize >= ISO14443A_UID_SIZE_DOUBLE) {
                Uid[3] = Uid[2];
                Uid[2] = Uid[1];
                Uid[1] = Uid[0];
                Uid[0] = ISO14443A_UID0_CT;
            }
            if (ISO14443ASelect(Buffer, &BitCount, Uid, SAK_CL1_VALUE)) {
                /* CL1 stage has ended successfully */
                ISO144433ASwitchState(ISO14443_3A_STATE_READY2);
            }

            return BitCount;
        }
        break;

    case ISO14443_3A_STATE_READY2:
        if (Cmd == ISO14443A_CMD_SELECT_CL2 && ActiveConfiguration.UidSize >= ISO14443A_UID_SIZE_DOUBLE) {
            /* Load UID CL2 and perform anticollision */
            ConfigurationUidType Uid;

            ApplicationGetUid(Uid);
            if (ISO14443ASelect(Buffer, &BitCount, &Uid[3], SAK_CL2_VALUE)) {
                /* CL2 stage has ended successfully. This means
                 * our complete UID has been sent to the reader. */
                ISO144433ASwitchState(ISO14443_3A_STATE_ACTIVE);
            }

            return BitCount;
        }
        break;

    case ISO14443_3A_STATE_ACTIVE:
        /* Recognise the HLTA command */
        if (ISO144433AIsHalt(Buffer, BitCount)) {
            LogEntry(LOG_INFO_APP_CMD_HALT, NULL, 0);
            ISO144433ASwitchState(ISO14443_3A_STATE_HALT);
            //DEBUG_PRINT("\r\nISO14443-3: Got HALT");
            return ISO14443A_APP_NO_RESPONSE;
        }
        /* Forward to ISO/IEC 14443-4 processing code */
        return ISO144434ProcessBlock(Buffer, (BitCount + 7) / 8) * 8;
    }

    /* Unknown command. Reset back to idle/halt state. */
    //DEBUG_PRINT("\r\nISO14443-3: RESET TO IDLE");
    ISO144433ASwitchState(Iso144433AIdleState);
    return ISO14443A_APP_NO_RESPONSE;
}

