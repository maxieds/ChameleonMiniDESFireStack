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
