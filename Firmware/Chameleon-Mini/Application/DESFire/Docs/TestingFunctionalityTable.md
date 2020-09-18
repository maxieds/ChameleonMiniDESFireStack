# Quick checklist for indicating which instructions / DESFire functionality has been well tested

## Native DESFire instruction set (some parts inferred from public application notes) 

| Instruction | Cmd Byte | Description | Testing Status | Implementation Notes |
| :---        |   :----: |     :----:  |    :----:      | :--                  |
| CMD_AUTHENTICATE | 0x0A | Authenticate legacy | :question: :grey_question: :interrobang: | |
| CMD_AUTHENTICATE_ISO | 0x1A | ISO / 3DES auth | :x: | |
| CMD_AUTHENTICATE_AES | 0xAA | Standard AES auth | :ballot_box_with_check: | |
| CMD_AUTHENTICATE_EV2_FIRST | 0x71 | Newer spec auth variant | :x: | |    
| CMD_AUTHENTICATE_EV2_NONFIRST | 0x77 | Newer spec auth variant | :x: | See page 32 of AN12343.pdf | 
| CMD_CHANGE_KEY_SETTINGS | 0x54 | | :ballot_box_with_check: | |
| CMD_SET_CONFIGURATION |  0x5C | | :x: | |
| CMD_CHANGE_KEY |  0xC4 | | :ballot_box_with_check: | |
| CMD_GET_KEY_VERSION | 0x64 | | :ballot_box_with_check: | |
| CMD_CREATE_APPLICATION |  0xCA | | :ballot_box_with_check: | |
| CMD_DELETE_APPLICATION |  0xDA | | :ballot_box_with_check: | |
| CMD_GET_APPLICATION_IDS | 0x6A | | :ballot_box_with_check: | |
| CMD_FREE_MEMORY | 0x6E | | :ballot_box_with_check: | |
| CMD_GET_DF_NAMES | 0x6D | | :x: | *Need docs for what this command does!* |
| CMD_GET_KEY_SETTINGS | 0x45 | | :ballot_box_with_check: | |
| CMD_SELECT_APPLICATION |  0x5A | | :ballot_box_with_check: | |
| CMD_FORMAT_PICC |  0xFC | | :ballot_box_with_check: | |
| CMD_GET_VERSION | 0x60 | | :ballot_box_with_check: | |
| CMD_GET_CARD_UID | 0x51 | | :ballot_box_with_check: | |
| CMD_GET_FILE_IDS |  0x6F | | | |
| CMD_GET_FILE_SETTINGS | 0xF5 | | | |
| CMD_CHANGE_FILE_SETTINGS | 0x5F | | | |
| CMD_CREATE_STDDATA_FILE |  0xCD | | | |
| CMD_CREATE_BACKUPDATA_FILE |  0xCB | | | |
| CMD_CREATE_VALUE_FILE |  0xCC | | | |
| CMD_CREATE_LINEAR_RECORD_FILE | 0xC1 | | | |
| CMD_CREATE_CYCLIC_RECORD_FILE | 0xC0 | | | |
| CMD_DELETE_FILE | 0xDF | | | |
| CMD_GET_ISO_FILE_IDS | 0x61 | | | |
| CMD_READ_DATA |  0x8D | | | |
| CMD_WRITE_DATA |  0x3D | | | |
| CMD_GET_VALUE | 0x6C | | | |
| CMD_CREDIT | 0x0C | | | |
| CMD_DEBIT | 0xDC | | | |
| CMD_LIMITED_CREDIT | 0x1C | | | |
| CMD_WRITE_RECORD | 0x3B | | | |
| CMD_READ_RECORDS | 0xBB | | | |
| CMD_CLEAR_RECORD_FILE | 0xEB | | | |
| CMD_COMMIT_TRANSACTION | 0xC7 | | | |
| CMD_ABORT_TRANSACTION | 0xA7 | | | |               |

## ISO7816 instruction and subcommands support 

| Instruction | Cmd Byte | Description | Testing Status | Implementation Notes |
| :---        |   :----: |     :----:  |    :----:      | :--                  |
| CMD_ISO7816_SELECT | 0xa4 | | | |
| CMD_ISO7816_GET_CHALLENGE | 0x84 | | | |
| CMD_ISO7816_EXTERNAL_AUTHENTICATE | 0x82 | | | |
| CMD_ISO7816_INTERNAL_AUTHENTICATE | 0x88 | | | |
| CMD_ISO7816_READ_BINARY | 0xb0 | | | |
| CMD_ISO7816_UPDATE_BINARY | 0xd6 | | | |
| CMD_ISO7816_READ_RECORDS | 0xb2 | | | |
| CMD_ISO7816_APPEND_RECORD | 0xe2 | | | |
