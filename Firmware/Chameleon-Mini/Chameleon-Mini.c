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
            LEDTick(); // this has to be the first function called here, since it is time-critical - the functions below may have non-negligible runtimes!
            RandomTick();
            TerminalTick();
            ButtonTick();
            LogTick();
            if(GlobalSettings.ActiveSettingPtr->LogMode == LOG_MODE_LIVE) {
                AtomicLiveLogTick();
            }
            CommandLineTick();
            AntennaLevelTick();
	    ApplicationTick();
            LEDHook(LED_POWERED, LED_ON);
        }

        TerminalTask();
        LogTask();
        ApplicationTask();
        CodecTask();
    }
}

