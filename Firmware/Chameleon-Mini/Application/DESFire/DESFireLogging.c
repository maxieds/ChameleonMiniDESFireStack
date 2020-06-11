/* 
 * DESFireLogging.c 
 * Maxie D. Schmidt (github.com/maxieds) 
 */

#include "DESFireLogging.h" 

#ifdef DESFIRE_DEFAULT_DEBUGGING_MODE
     DESFireLoggingMode LocalLoggingMode = DESFIRE_DEFAULT_LOGGING_MODE;
#else
     DESFireLoggingMode LocalLoggingMode = DEBUGGING;
#endif

#ifdef DESFIRE_DEFAULT_TESTING_MODE
     BYTE LocalTestingMode = DESFIRE_DEFAULT_TESTING_MODE;
#else
     BYTE LocalTestingMode = 0x01;
#endif

