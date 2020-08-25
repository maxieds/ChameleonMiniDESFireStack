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

This source code is only licensed for 
redistribution under the above GPL clause for 
non-commercial users. All commerical use or inclusion of this 
software requires express written consent of the author (MDS). 
This restriction pertains to any binary distributions which 
are derivative works of this software.
*/

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
     LOG_INFO_DESFIRE_INCOMING_DATA_ENC           = 0xE4, 
     LOG_INFO_DESFIRE_OUTGOING_DATA               = 0xE5, 
     LOG_INFO_DESFIRE_OUTGOING_DATA_ENC           = 0xE6, 
     LOG_INFO_DESFIRE_NATIVE_COMMAND              = 0xE7, 
     LOG_INFO_DESFIRE_ISO1443_COMMAND             = 0xE8, 
     LOG_INFO_DESFIRE_ISO7816_COMMAND             = 0xE9, 
     LOG_INFO_DESFIRE_PICC_RESET                  = 0xEA, 
     LOG_INFO_DESFIRE_PICC_RESET_FROM_MEMORY      = 0XEB, 
     LOG_INFO_DESFIRE_PROTECTED_DATA_SET          = 0xEC, 
     LOG_INFO_DESFIRE_PROTECTED_DATA_SET_VERBOSE  = 0xED, 

     /* DESFire app */
     LOG_APP_AUTH_KEY            = 0xD0, ///< The key used for authentication
     LOG_APP_NONCE_B             = 0xD1, ///< Nonce B's value (generated)
     LOG_APP_NONCE_AB            = 0xD2, ///< Nonces A and B values (received)
     
     /* ISO 14443 related entries */
     LOG_ISO14443_3A_STATE       = 0x53,
     LOG_ISO14443_4_STATE        = 0x54,

#endif
