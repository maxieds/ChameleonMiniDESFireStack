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
*/

/* 
 * DESFireChameleonTerminal.h 
 * Maxie D. Schmidt (github.com/maxieds)
 */ 

#ifndef __DESFIRE_CHAMELEON_TERMINAL_H__
#define __DESFIRE_CHAMELEON_TERMINAL_H__

#include "../../Terminal/Commands.h"
#include "../../Terminal/CommandLine.h"

#define DFCOMMAND_SET_HEADER                  "DF_SETHDR"
CommandStatusIdType CommandDESFireGetHeaderProperty(char *OutParam);
CommandStatusIdType CommandDESFireSetHeaderProperty(char *OutMessage, const char *InParams);

#define DFCOMMAND_LAYOUT_PPRINT               "DF_PPRINT_PICC"
CommandStatusIdType CommandDESFireLayoutPPrint(char *OutParam, const char *InParams);

#define DFCOMMAND_FIRMWARE_INFO               "DF_FWINFO"
CommandStatusIdType CommandDESFireFirmwareInfo(char *OutParam);

#define DFCOMMAND_LOGGING_MODE                "DF_LOGMODE"
CommandStatusIdType CommandDESFireGetLoggingMode(char *OutParam);
CommandStatusIdType CommandDESFireSetLoggingMode(char *OutMessage, const char *InParams);

#define DFCOMMAND_TESTING_MODE                "DF_TESTMODE"
CommandStatusIdType CommandDESFireGetTestingMode(char *OutParam);
CommandStatusIdType CommandDESFireSetTestingMode(char *OutMessage, const char *InParams);

#endif
