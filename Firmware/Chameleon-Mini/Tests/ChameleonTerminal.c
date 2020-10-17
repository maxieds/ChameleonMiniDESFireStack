/* ChameleonTerminal.c */

#ifdef ENABLE_RUNTESTS_TERMINAL_COMMAND

#include "ChameleonTerminal.h"
#include "CryptoTests.h"

CommandStatusIdType CommandRunTests(char *OutParam) {
     const ChameleonTestType testCases[] = {
          #ifdef ENABLE_CRYPTO_TESTS
          &CryptoTDEATestCase1, 
          &CryptoTDEATestCase2, 
          &CryptoAESTestCase1, 
          &CryptoAESTestCase2, 
          &CryptoAESTestCase3,
          #endif
     };
     uint8_t t;
     uint16_t maxOutputChars = TERMINAL_BUFFER_SIZE, charCount;
     bool statusPassed = true;
     for(t = 0; t < sizeof(testCases); t++) {
          if(!testCases[t]()) {
               charCount = snprintf_P(OutParam, maxOutputChars, PSTR("> Test #% 2d ... [X]\r\n"), t + 1);
               statusPassed = false;
          }
          maxOutputChars = MIN(0, maxOutputChars - charCount);
          OutParam += charCount;
     }
     if(t == 0) {
          sprintf_P(OutParam, "No tests to run.\r\n");
     }
     else if(statusPassed) {
          snprintf_P(OutParam, maxOutputChars, PSTR("All tests passed.\r\n"));
     }
     else {
          snprintf_P(OutParam, maxOutputChars, PSTR("Tests failed.\r\n"));
     }
     return COMMAND_INFO_OK_WITH_TEXT;
}

#endif /* ENABLE_RUNTESTS_TERMINAL_COMMAND */
