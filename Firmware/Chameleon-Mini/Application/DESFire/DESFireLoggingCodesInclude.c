/* 
 * DESFireLoggingCodesInclude.c : 
 * Maxie D. Schmidt (github.com/maxieds)
 */ 

#ifndef __DESFIRE_LOGGING_CODES_INCLUDE_C__ 
#define __DESFIRE_LOGGING_CODES_INCLUDE_C__

     /* Intended to log all routine, complete transaction records (verbose output), and 
        to source a list of debugging messages and TODO items in the 
        developing DESFire emulation support within the firmware sources. 
        These codes should almost immediately be supported for use of the Chameleon Mini 
        devices with the Chameleon Mini Live Debugger logger application for Android: */
     LOG_ERR_DESFIRE_GENERIC_ERROR                = 0xE0, 
     LOG_INFO_DESFIRE_STATUS_INFO                 = 0xE1, 
     LOG_INFO_DESFIRE_DEBUGGING_OUTPUT            = 0xE2, 
     LOG_INFO_DESFIRE_INCOMING_DATA               = 0xE3, 
     LOG_INFO_DESFIRE_INCOMING_DATA_VERBOSE       = 0xE4, 
     LOG_INFO_DESFIRE_OUTGOING_DATA               = 0xE5, 
     LOG_INFO_DESFIRE_OUTGOING_DATA_VERBOSE       = 0xE6, 
     LOG_INFO_DESFIRE_NATIVE_COMMAND              = 0xE7, 
     LOG_INFO_DESFIRE_ISO1443_COMMAND             = 0xE8, 
     LOG_INFO_DESFIRE_ISO7816_COMMAND             = 0xE9, 
     LOG_INFO_DESFIRE_PICC_RESET                  = 0xEA, 
     LOG_INFO_DESFIRE_PICC_RESET_FROM_MEMORY      = 0XEB, 
     LOG_INFO_DESFIRE_PROTECTED_DATA_SET          = 0xEC, 
     LOG_INFO_DESFIRE_PROTECTED_DATA_SET_VERBOSE  = 0xED, 

#endif
