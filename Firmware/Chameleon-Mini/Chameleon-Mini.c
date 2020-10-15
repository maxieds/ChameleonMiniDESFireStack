#include "Chameleon-Mini.h"

//int main() __attribute__((noreturn));

int main(void)
{
    SystemInit();
    SettingsLoad();
    LEDInit();
    MemoryInit();
    CodecInitCommon();
    ConfigurationInit();
    TerminalInit();
    RandomInit();
    ButtonInit();
    AntennaLevelInit();
    LogInit();
    SystemInterruptInit();

    while(1) {
        if (SystemTick100ms()) {
            LEDTick(); // this has to be the first function called here, since it is time-critical - 
                       // the functions below may have non-negligible runtimes!
            RandomTick();
            TerminalTick();
            ButtonTick();
            ApplicationTick();
            LogTick();
            if(GlobalSettings.ActiveSettingPtr->LogMode == LOG_MODE_LIVE && 
               (++LiveLogModePostTickCount % LIVE_LOGGER_POST_TICKS) == 0) {
                AtomicLiveLogTick();
            }
            CommandLineTick();
            AntennaLevelLogReaderDetectCount = (++AntennaLevelLogReaderDetectCount) % ANTENNA_LEVEL_LOG_RDRDETECT_INTERVAL;
            AntennaLevelTick();
            LEDHook(LED_POWERED, LED_ON);
        }
        ApplicationTask();
        CodecTask();
        LogTask();
        TerminalTask();
    }
}

