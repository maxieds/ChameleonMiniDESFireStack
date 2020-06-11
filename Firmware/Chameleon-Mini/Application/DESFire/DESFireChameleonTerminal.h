/* 
 * DESFireChameleonTerminal.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */ 

#ifndef __DESFIRE_CHAMELEON_TERMINAL_H__
#define __DESFIRE_CHAMELEON_TERMINAL_H__

#include "../../Terminal/Commands.h"
#include "../../Terminal/CommandLine.h"

#define DFCOMMAND_SET_VERSION                 "DF_VERSION"
CommandStatusIdType CommandDESFireGetVersion(char *OutParam);
CommandStatusIdType CommandDESFireSetVersion(char *OutMessage, const char *InParams);

#define DFCOMMAND_SET_MANUFACTURER            "DF_MANUFACTURER"
CommandStatusIdType CommandDESFireGetManufacturer(char *OutParam);
CommandStatusIdType CommandDESFireSetManufacturer(char *OutMessage, const char *InParams);

#define DFCOMMAND_SET_SERIALNO                "DF_SERIALNO"
CommandStatusIdType CommandDESFireGetSerialNumber(char *OutParam);
CommandStatusIdType CommandDESFireSetSerialNumber(char *OutMessage, const char *InParams);

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

#define DFCOMMAND_READ_AIDDF                  "DF_READ_APPDIR"
CommandStatusIdType CommandDESFireReadAppDirectory(char *OutMessage, const char *InParams);

#define DFCOMMAND_LIST_AIDDFS                 "DF_LIST_APPDIRS"
CommandStatusIdType CommandDESFireListAppDirectories(char *OutMessage);

#define DFCOMMAND_READ_FILE                   "DF_READ_FILE"
CommandStatusIdType CommandDESFireReadFile(char *OutMessage, const char *InParams);

#define DFCOMMAND_LIST_FILES                  "DF_LIST_FILES"
CommandStatusIdType CommandDESFireListFiles(char *OutMessage, const char *InParams);

#endif
