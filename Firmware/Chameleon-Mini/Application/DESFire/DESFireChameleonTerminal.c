/* 
 * DESFireChameleonTerminal.c
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "../../Terminal/Terminal.h"
#include "DESFireChameleonTerminal.h"

static CommandStatusIdType CommandNotYetImplemented(char *OutMessage) {
     snprintf_P(OutMessage, TERMINAL_BUFFER_SIZE, PSTR("(!!!) DESFire Terminal Command NOT YET IMPLEMENTED."));
     return COMMAND_ERR_INVALID_USAGE_ID;
}

CommandStatusIdType CommandDESFireGetVersion(char *OutParam) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireSetVersion(char *OutParam, const char *InParams) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireGetManufacturer(char *OutParam) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireSetManufacturer(char *OutParam, const char *InParams) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireGetSerialNumber(char *OutParam) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireSetSerialNumber(char *OutParam, const char *InParams) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireLayoutPPrint(char *OutParam) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireFirmwareInfo(char *OutParam) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireGetLoggingMode(char *OutParam) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireSetLoggingMode(char *OutParam, const char *InParams) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireReadAppDirectory(char *OutParam, const char *InParams) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireListAppDirectories(char *OutParam) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireReadFile(char *OutParam, const char *InParams) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireListFiles(char *OutParam, const char *InParams) {
     return CommandNotYetImplemented(OutParam);
}

/*
CommandStatusIDType CommandName(char *OutMessage, const char *InParam) {
     // Syntax: CMDNAME <AppID> <FileID> <48-Byte Data Field>
     uint32_t appID = 0;
     uint8_t  fileID = 0;
     uint32_t dataField[12];
     if(!sscanf_P(InParam, PSTR("%06x"), &appID, PSTR("%02x"), &fileID, PSTR("%048x), &dataField[0])) {
          return COMMAND_ERR_INVALID_PARAM_ID;
     }
*/
