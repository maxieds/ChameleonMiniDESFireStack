/* 
 * DESFireChameleonTerminalInclude.c 
 * Maxie D. Schmidt (github.com/maxieds)
 */

#ifndef __DESFIRE_CHAMELEON_TERMINAL_INCLUDE_C__
#define __DESFIRE_CHAMELEON_TERMINAL_INCLUDE_C__
     {
          .Command        = DFCOMMAND_SET_VERSION, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = NO_FUNCTION,
          .SetFunc        = CommandDESFireSetVersion,
          .GetFunc        = CommandDESFireGetVersion
     },
     {
          .Command        = DFCOMMAND_SET_MANUFACTURER, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = NO_FUNCTION,
          .SetFunc        = CommandDESFireSetManufacturer,
          .GetFunc        = CommandDESFireGetManufacturer
     },
     {
          .Command        = DFCOMMAND_SET_SERIALNO, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = NO_FUNCTION,
          .SetFunc        = CommandDESFireSetSerialNumber,
          .GetFunc        = CommandDESFireGetSerialNumber
     },
     {
          .Command        = DFCOMMAND_LAYOUT_PPRINT, 
          .ExecFunc       = CommandDESFireLayoutPPrint,
          .ExecParamFunc  = NO_FUNCTION,
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
          .Command        = DFCOMMAND_READ_AIDDF, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = CommandDESFireReadAppDirectory,
          .SetFunc        = NO_FUNCTION,
          .GetFunc        = NO_FUNCTION
     },
     {
          .Command        = DFCOMMAND_LIST_AIDDFS, 
          .ExecFunc       = CommandDESFireListAppDirectories,
          .ExecParamFunc  = NO_FUNCTION,
          .SetFunc        = NO_FUNCTION,
          .GetFunc        = NO_FUNCTION
     },
     {
          .Command        = DFCOMMAND_READ_FILE, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = CommandDESFireReadFile,
          .SetFunc        = NO_FUNCTION,
          .GetFunc        = NO_FUNCTION
     },
     {
          .Command        = DFCOMMAND_LIST_FILES, 
          .ExecFunc       = NO_FUNCTION,
          .ExecParamFunc  = CommandDESFireListFiles,
          .SetFunc        = NO_FUNCTION,
          .GetFunc        = NO_FUNCTION
     },
#endif
