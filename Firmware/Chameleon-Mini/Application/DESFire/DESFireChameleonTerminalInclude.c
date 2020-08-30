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

Based in part on the original DESFire code created by 
@dev-zzo (GitHub handle) [Dmitry Janushkevich] available at 
https://github.com/dev-zzo/ChameleonMini/tree/desfire.

This notice must be retained at the top of all source files in the repository. 

This source code is only licensed for 
redistribution under the above GPL clause for 
non-commercial users. All commerical use or inclusion of this 
software requires express written consent of the author (MDS). 
This restriction pertains to any binary distributions which 
are derivative works of this software.
*/

/* 
 * DESFireChameleonTerminalInclude.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_CHAMELEON_TERMINAL_INCLUDE_C__
#define __DESFIRE_CHAMELEON_TERMINAL_INCLUDE_C__
     {
          .Command        = DFCOMMAND_SET_HEADER, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = NO_FUNCTION,
          .SetFunc        = CommandDESFireSetHeaderProperty,
          .GetFunc        = CommandDESFireGetHeaderProperty
     },
     {
          .Command        = DFCOMMAND_LAYOUT_PPRINT, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = CommandDESFireLayoutPPrint,
          .SetFunc        = NO_FUNCTION,
          .GetFunc        = NO_FUNCTION
     },
     {
          .Command        = DFCOMMAND_FIRMWARE_INFO,
          .ExecFunc       = CommandDESFireFirmwareInfo,
          .ExecParamFunc  = NO_FUNCTION,
          .SetFunc        = NO_FUNCTION,
          .GetFunc        = NO_FUNCTION
     },
     {
          .Command        = DFCOMMAND_LOGGING_MODE, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = NO_FUNCTION,
          .SetFunc        = CommandDESFireSetLoggingMode,
          .GetFunc        = CommandDESFireGetLoggingMode
     },
     {
          .Command        = DFCOMMAND_TESTING_MODE, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = NO_FUNCTION,
          .SetFunc        = CommandDESFireSetTestingMode,
          .GetFunc        = CommandDESFireGetTestingMode
     },
#endif
