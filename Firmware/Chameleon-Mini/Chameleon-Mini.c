#include "Chameleon-Mini.h"

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
            AntennaLevelTick();
            LEDHook(LED_POWERED, LED_ON);
        }
        ApplicationTask();
        CodecTask();
        LogTask();
        TerminalTask();
    }
}

