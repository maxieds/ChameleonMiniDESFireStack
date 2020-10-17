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
     for(t = 0; t < sizeof(testCases); t++) {
          bool testPassed = testCases[t]();
          if(testPassed) {
               charCount = snprintf_P(OutParam, maxOutputChars, 
                                      PSTR("Test #% 2d ... [OK]\r\n"), t + 1);
          }
          else {
               charCount = snprintf_P(OutParam, maxOutputChars, 
                                      PSTR("Test #% 2d ... [XX]"), t + 1);
          }
          maxOutputChars = MIN(0, maxOutputChars - charCount);
          OutParam += charCount;
     }
     if(t == 0) {
          sprintf_P(OutParam, "No tests to run.\r\n");
     }
     return COMMAND_INFO_OK_WITH_TEXT;
}

#endif /* ENABLE_RUNTESTS_TERMINAL_COMMAND */
