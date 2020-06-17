/* 
 * DESFireChameleonTerminal.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */ 

#ifndef __DESFIRE_CHAMELEON_TERMINAL_H__
#define __DESFIRE_CHAMELEON_TERMINAL_H__

#include "../../Terminal/Commands.h"
#include "../../Terminal/CommandLine.h"

// Version, manufacturer, serial number ... 
#define DFCOMMAND_SET_HEADER                  "DF_SETHDR"
CommandStatusIdType CommandDESFireGetHeaderProperty(char *OutParam);
CommandStatusIdType CommandDESFireSetHeaderProperty(char *OutMessage, const char *InParams);

#define DFCOMMAND_LAYOUT_PPRINT               "DF_PPRINT_PICC"
CommandStatusIdType CommandDESFireLayoutPPrint(char *OutParam);

#define DFCOMMAND_FIRMWARE_INFO               "DF_FWINFO"
CommandStatusIdType CommandDESFireFirmwareInfo(char *OutParam);

#define DFCOMMAND_LOGGING_MODE                "DF_LOGMODE"
CommandStatusIdType CommandDESFireGetLoggingMode(char *OutParam);
CommandStatusIdType CommandDESFireSetLoggingMode(char *OutMessage, const char *InParams);

#define DFCOMMAND_TESTING_MODE                "DF_TESTMODE"
CommandStatusIdType CommandDESFireGetTestingMode(char *OutParam);
CommandStatusIdType CommandDESFireSetTestingMode(char *OutMessage, const char *InParams);

#endif
