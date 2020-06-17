/* 
 * DESFireChameleonTerminal.c
 * Maxie D. Schmidt (github.com/maxieds)
 */

#include "../../Terminal/Terminal.h"
#include "../../Terminal/Commands.h"

#include "DESFireChameleonTerminal.h"
#include "DESFireFirmwareSettings.h"
#include "DESFirePICCControl.h"

static CommandStatusIdType CommandNotYetImplemented(char *OutMessage) {
     snprintf_P(OutMessage, TERMINAL_BUFFER_SIZE, PSTR("(!!!) DESFire Terminal Command NOT YET IMPLEMENTED."));
     return COMMAND_ERR_INVALID_USAGE_ID;
}

CommandStatusIdType CommandDESFireGetHeaderProperty(char *OutParam) {
     snprintf_P(OutParam, TERMINAL_BUFFER_SIZE, 
                PSTR("%s <HardwareVersion-2|SoftwareVersion-2|BatchNumber-5|ProductionDate-2> <HexBytes-N>"), 
                DFCOMMAND_SET_HEADER);
     return COMMAND_INFO_OK_WITH_TEXT_ID;
}

CommandStatusIdType CommandDESFireSetHeaderProperty(char *OutParam, const char *InParams) {
     char hdrPropSpecStr[24];
     char propSpecBytesStr[16];
     BYTE propSpecBytes[16];
     SIZET dataByteCount = 0x00;
     BYTE StatusError = 0x00;
     if(!sscanf_P(InParams, PSTR("%24s %12s"), hdrPropSpecStr, propSpecBytesStr)) {
          CommandDESFireGetHeaderProperty(OutParam);
          return COMMAND_ERR_INVALID_PARAM_ID;
     }
     hdrPropSpecStr[23] = propSpecBytesStr[15] = '\0';
     dataByteCount = HexStringToBuffer(propSpecBytes, 16, propSpecBytesStr);
     if(!strcmp_P(hdrPropSpecStr, PSTR("HardwareVersion"))) {
          if(dataByteCount != 2) {
               StatusError = 0x01;
          }
          else {
               Picc.HwVersionMajor = propSpecBytes[0];
               Picc.HwVersionMinor = propSpecBytes[1];
          }
     }
     else if(!strcmp_P(hdrPropSpecStr, PSTR("SoftwareVersion"))) {
          if(dataByteCount != 2) {
               StatusError = 0x01;
          }
          else {
               Picc.SwVersionMajor = propSpecBytes[0];
               Picc.SwVersionMinor = propSpecBytes[1];
          }
     }
     else if(!strcmp_P(hdrPropSpecStr, PSTR("BatchNumber"))) {
          if(dataByteCount != 5) {
               StatusError = 0x01;
          }
          else {
               memcpy(Picc.BatchNumber, propSpecBytes, 5);
          }
     }
     else if(!strcmp_P(hdrPropSpecStr, PSTR("ProductionDate"))) {
         if(dataByteCount != 2) {
               StatusError = 0x01;
          }
          else {
               Picc.ProductionWeek = propSpecBytes[0];
               Picc.ProductionYear = propSpecBytes[1];
          }
     }
     else {
          StatusError = 0x01;
     }
     if(StatusError) {
          CommandDESFireGetHeaderProperty(OutParam);
          return COMMAND_ERR_INVALID_PARAM_ID;
     }
     return COMMAND_INFO_OK_ID;
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

CommandStatusIdType CommandDESFireGetTestingMode(char *OutParam) {
     return CommandNotYetImplemented(OutParam);
}

CommandStatusIdType CommandDESFireSetTestingMode(char *OutParam, const char *InParams) {
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
