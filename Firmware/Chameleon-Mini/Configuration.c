/*
 * Standards.c
 *
 *  Created on: 15.02.2013
 *      Author: skuser
 */

#include <avr/pgmspace.h>

#include "Configuration.h"
#include "Settings.h"
#include "AntennaLevel.h"
#include "LEDHook.h"

#ifdef CONFIG_MF_DESFIRE_SUPPORT
     #include "Application/MifareDESFire.h"
#endif

#if defined(CONFIG_MF_DESFIRE_SUPPORT) || defined(CONFIG_ISO1593_SNIFF_SUPPORT)
     #include "Application/ISO15693-A.h"
     #include "Codec/ISO15693.h"
#endif

/* Map IDs to text */
static const MapEntryType PROGMEM ConfigurationMap[] = {
    { .Id = CONFIG_NONE, 			        .Text = "NONE" },
#ifdef CONFIG_MF_ULTRALIGHT_SUPPORT
    { .Id = CONFIG_MF_ULTRALIGHT, 	             .Text = "MF_ULTRALIGHT" },
    { .Id = CONFIG_MF_ULTRALIGHT_EV1_80B,       .Text = "MF_ULTRALIGHT_EV1_80B" },
    { .Id = CONFIG_MF_ULTRALIGHT_EV1_164B,      .Text = "MF_ULTRALIGHT_EV1_164B" },
#endif
#ifdef CONFIG_MF_CLASSIC_1K_SUPPORT
    { .Id = CONFIG_MF_CLASSIC_1K, 	             .Text = "MF_CLASSIC_1K" },
#endif
#ifdef CONFIG_MF_CLASSIC_1K_7B_SUPPORT
    { .Id = CONFIG_MF_CLASSIC_1K_7B, 	        .Text = "MF_CLASSIC_1K_7B" },
#endif
#ifdef CONFIG_MF_CLASSIC_4K_SUPPORT
    { .Id = CONFIG_MF_CLASSIC_4K, 	             .Text = "MF_CLASSIC_4K" },
#endif
#ifdef CONFIG_MF_CLASSIC_4K_7B_SUPPORT
    { .Id = CONFIG_MF_CLASSIC_4K_7B, 	        .Text = "MF_CLASSIC_4K_7B" },
#endif
#ifdef CONFIG_MF_DESFIRE_SUPPORT
	{ .Id = CONFIG_MF_DESFIRE,		        .Text = "MF_DESFIRE" },
#endif
#ifdef CONFIG_ISO14443A_SNIFF_SUPPORT
    { .Id = CONFIG_ISO14443A_SNIFF,	        .Text = "ISO14443A_SNIFF" },
#endif
#ifdef CONFIG_ISO15693_SNIFF_SUPPORT
    { .Id = CONFIG_ISO15693_SNIFF,	             .Text = "ISO15693_SNIFF" },
#endif
#ifdef CONFIG_ISO14443A_READER_SUPPORT
    { .Id = CONFIG_ISO14443A_READER,	        .Text = "ISO14443A_READER" },
#endif
};

/* Include all Codecs and Applications */
#include "Codec/Codec.h"
#include "Application/Application.h"

static void CodecInitDummy(void) { }
static void CodecDeInitDummy(void) { }
static void CodecTaskDummy(void) { }
static void ApplicationInitDummy(void) {}
static void ApplicationResetDummy(void) {}
static void ApplicationTaskDummy(void) {}
static void ApplicationTickDummy(void) {}
static uint16_t ApplicationProcessDummy(uint8_t* ByteBuffer, uint16_t ByteCount) { return 0; }
static void ApplicationGetUidDummy(ConfigurationUidType Uid) { }
static void ApplicationSetUidDummy(ConfigurationUidType Uid) { }


static const PROGMEM ConfigurationType ConfigurationTable[] = {
    [CONFIG_NONE] = {
        .CodecInitFunc = CodecInitDummy,
        .CodecDeInitFunc = CodecDeInitDummy,
        .CodecTaskFunc = CodecTaskDummy,
        .ApplicationInitFunc = ApplicationInitDummy,
        .ApplicationResetFunc = ApplicationResetDummy,
        .ApplicationTaskFunc = ApplicationTaskDummy,
        .ApplicationTickFunc = ApplicationTickDummy,
        .ApplicationProcessFunc = ApplicationProcessDummy,
        .ApplicationGetUidFunc = ApplicationGetUidDummy,
        .ApplicationSetUidFunc = ApplicationSetUidDummy,
        .UidSize = 0,
        .MemorySize = 0,
        .ReadOnly = true
    },
#ifdef CONFIG_MF_ULTRALIGHT_SUPPORT
    [CONFIG_MF_ULTRALIGHT] = {
        .CodecInitFunc = ISO14443ACodecInit,
        .CodecDeInitFunc = ISO14443ACodecDeInit,
        .CodecTaskFunc = ISO14443ACodecTask,
        .ApplicationInitFunc = MifareUltralightAppInit,
        .ApplicationResetFunc = MifareUltralightAppReset,
        .ApplicationTaskFunc = MifareUltralightAppTask,
        .ApplicationTickFunc = ApplicationTickDummy,
        .ApplicationProcessFunc = MifareUltralightAppProcess,
        .ApplicationGetUidFunc = MifareUltralightGetUid,
        .ApplicationSetUidFunc = MifareUltralightSetUid,
        .UidSize = MIFARE_ULTRALIGHT_UID_SIZE,
        .MemorySize = MIFARE_ULTRALIGHT_MEM_SIZE,
        .ReadOnly = false
    },
    [CONFIG_MF_ULTRALIGHT_EV1_80B] = {
        .CodecInitFunc = ISO14443ACodecInit,
        .CodecDeInitFunc = ISO14443ACodecDeInit,
        .CodecTaskFunc = ISO14443ACodecTask,
        .ApplicationInitFunc = MifareUltralightEV11AppInit,
        .ApplicationResetFunc = MifareUltralightAppReset,
        .ApplicationTaskFunc = MifareUltralightAppTask,
        .ApplicationTickFunc = ApplicationTickDummy,
        .ApplicationProcessFunc = MifareUltralightAppProcess,
        .ApplicationGetUidFunc = MifareUltralightGetUid,
        .ApplicationSetUidFunc = MifareUltralightSetUid,
        .UidSize = MIFARE_ULTRALIGHT_UID_SIZE,
        .MemorySize = MIFARE_ULTRALIGHT_EV11_MEM_SIZE,
        .ReadOnly = false
    },
    [CONFIG_MF_ULTRALIGHT_EV1_164B] = {
        .CodecInitFunc = ISO14443ACodecInit,
        .CodecDeInitFunc = ISO14443ACodecDeInit,
        .CodecTaskFunc = ISO14443ACodecTask,
        .ApplicationInitFunc = MifareUltralightEV12AppInit,
        .ApplicationResetFunc = MifareUltralightAppReset,
        .ApplicationTaskFunc = MifareUltralightAppTask,
        .ApplicationTickFunc = ApplicationTickDummy,
        .ApplicationProcessFunc = MifareUltralightAppProcess,
        .ApplicationGetUidFunc = MifareUltralightGetUid,
        .ApplicationSetUidFunc = MifareUltralightSetUid,
        .UidSize = MIFARE_ULTRALIGHT_UID_SIZE,
        .MemorySize = MIFARE_ULTRALIGHT_EV12_MEM_SIZE,
        .ReadOnly = false
    },
#endif
#ifdef CONFIG_MF_CLASSIC_1K_SUPPORT
    [CONFIG_MF_CLASSIC_1K] = {
        .CodecInitFunc = ISO14443ACodecInit,
        .CodecDeInitFunc = ISO14443ACodecDeInit,
        .CodecTaskFunc = ISO14443ACodecTask,
        .ApplicationInitFunc = MifareClassicAppInit1K,
        .ApplicationResetFunc = MifareClassicAppReset,
        .ApplicationTaskFunc = MifareClassicAppTask,
        .ApplicationTickFunc = ApplicationTickDummy,
        .ApplicationProcessFunc = MifareClassicAppProcess,
        .ApplicationGetUidFunc = MifareClassicGetUid,
        .ApplicationSetUidFunc = MifareClassicSetUid,
        .UidSize = MIFARE_CLASSIC_UID_SIZE,
        .MemorySize = MIFARE_CLASSIC_1K_MEM_SIZE,
        .ReadOnly = false
    },
#endif
#ifdef CONFIG_MF_CLASSIC_1K_7B_SUPPORT
    [CONFIG_MF_CLASSIC_1K_7B] = {
        .CodecInitFunc = ISO14443ACodecInit,
        .CodecDeInitFunc = ISO14443ACodecDeInit,
        .CodecTaskFunc = ISO14443ACodecTask,
        .ApplicationInitFunc = MifareClassicAppInit1K7B,
        .ApplicationResetFunc = MifareClassicAppReset,
        .ApplicationTaskFunc = MifareClassicAppTask,
        .ApplicationTickFunc = ApplicationTickDummy,
        .ApplicationProcessFunc = MifareClassicAppProcess,
        .ApplicationGetUidFunc = MifareClassicGetUid,
        .ApplicationSetUidFunc = MifareClassicSetUid,
        .UidSize = ISO14443A_UID_SIZE_DOUBLE,
        .MemorySize = MIFARE_CLASSIC_1K_MEM_SIZE,
        .ReadOnly = false
    },
#endif
#ifdef CONFIG_MF_CLASSIC_4K_SUPPORT
    [CONFIG_MF_CLASSIC_4K] = {
        .CodecInitFunc = ISO14443ACodecInit,
        .CodecDeInitFunc = ISO14443ACodecDeInit,
        .CodecTaskFunc = ISO14443ACodecTask,
        .ApplicationInitFunc = MifareClassicAppInit4K,
        .ApplicationResetFunc = MifareClassicAppReset,
        .ApplicationTaskFunc = MifareClassicAppTask,
        .ApplicationTickFunc = ApplicationTickDummy,
        .ApplicationProcessFunc = MifareClassicAppProcess,
        .ApplicationGetUidFunc = MifareClassicGetUid,
        .ApplicationSetUidFunc = MifareClassicSetUid,
        .UidSize = MIFARE_CLASSIC_UID_SIZE,
        .MemorySize = MIFARE_CLASSIC_4K_MEM_SIZE,
        .ReadOnly = false
    },
#endif
#ifdef CONFIG_MF_CLASSIC_4K_7B_SUPPORT
    [CONFIG_MF_CLASSIC_4K_7B] = {
        .CodecInitFunc = ISO14443ACodecInit,
        .CodecDeInitFunc = ISO14443ACodecDeInit,
        .CodecTaskFunc = ISO14443ACodecTask,
        .ApplicationInitFunc = MifareClassicAppInit4K7B,
        .ApplicationResetFunc = MifareClassicAppReset,
        .ApplicationTaskFunc = MifareClassicAppTask,
        .ApplicationTickFunc = ApplicationTickDummy,
        .ApplicationProcessFunc = MifareClassicAppProcess,
        .ApplicationGetUidFunc = MifareClassicGetUid,
        .ApplicationSetUidFunc = MifareClassicSetUid,
        .UidSize = ISO14443A_UID_SIZE_DOUBLE,
        .MemorySize = MIFARE_CLASSIC_4K_MEM_SIZE,
        .ReadOnly = false
    },
#endif
#ifdef CONFIG_MF_DESFIRE_SUPPORT
	[CONFIG_MF_DESFIRE] = {
		.CodecInitFunc = ISO14443ACodecInit,
		.CodecDeInitFunc =ISO14443ACodecDeInit,
		.CodecTaskFunc = ISO14443ACodecTask,
		.ApplicationInitFunc = MifareDesfireEV0AppInit,
		.ApplicationResetFunc = MifareDesfireAppReset,
		.ApplicationTaskFunc = MifareDesfireAppTask,
		.ApplicationTickFunc = MifareDesfireAppTick,
		.ApplicationProcessFunc = MifareDesfireAppProcess,
		.ApplicationGetUidFunc = MifareDesfireGetUid,
		.ApplicationSetUidFunc = MifareDesfireSetUid,
		.UidSize = ISO14443A_UID_SIZE_DOUBLE,
		.MemorySize = MIFARE_CLASSIC_4K_MEM_SIZE,
		.ReadOnly = false
	},
#endif
#ifdef CONFIG_ISO14443A_SNIFF_SUPPORT
    [CONFIG_ISO14443A_SNIFF] = {
        .CodecInitFunc = Sniff14443ACodecInit,
        .CodecDeInitFunc = Sniff14443ACodecDeInit,
        .CodecTaskFunc = Sniff14443ACodecTask,
        .ApplicationInitFunc = Sniff14443AAppInit,
        .ApplicationResetFunc = Sniff14443AAppReset,
        .ApplicationTaskFunc = Sniff14443AAppTask,
        .ApplicationTickFunc = Sniff14443AAppTick,
        .ApplicationProcessFunc = Sniff14443AAppProcess,
        .ApplicationGetUidFunc = ApplicationGetUidDummy,
        .ApplicationSetUidFunc = ApplicationSetUidDummy,
        .UidSize = 0,
        .MemorySize = 0,
        .ReadOnly = true
    },
#endif
#ifdef CONFIG_ISO15693_SNIFF_SUPPORT
    [CONFIG_ISO15693_SNIFF] = {
        .CodecInitFunc = ISO15693CodecInit,
        .CodecDeInitFunc = ISO15693CodecDeInit,
        .CodecTaskFunc = ISO15693CodecTask,
        .ApplicationInitFunc = ApplicationInitDummy,
        .ApplicationResetFunc = ApplicationResetDummy,
        .ApplicationTaskFunc = ApplicationTaskDummy,
        .ApplicationTickFunc = ApplicationTickDummy,
        .ApplicationProcessFunc = ApplicationProcessDummy,
        .ApplicationGetUidFunc = ApplicationGetUidDummy,
        .ApplicationSetUidFunc = ApplicationSetUidDummy,
        .UidSize = 0,
        .MemorySize = 0,
        .ReadOnly = true,
        .TagFamily = TAG_FAMILY_NONE
    },
#endif
#ifdef CONFIG_ISO14443A_READER_SUPPORT
    [CONFIG_ISO14443A_READER] = {
        .CodecInitFunc = Reader14443ACodecInit,
        .CodecDeInitFunc = Reader14443ACodecDeInit,
        .CodecTaskFunc = Reader14443ACodecTask,
        .ApplicationInitFunc = Reader14443AAppInit,
        .ApplicationResetFunc = Reader14443AAppReset,
        .ApplicationTaskFunc = Reader14443AAppTask,
        .ApplicationTickFunc = Reader14443AAppTick,
        .ApplicationProcessFunc = Reader14443AAppProcess,
        .ApplicationGetUidFunc = ApplicationGetUidDummy,
        .ApplicationSetUidFunc = ApplicationSetUidDummy,
        .UidSize = 0,
        .MemorySize = 0,
        .ReadOnly = false
    },
#endif
};

ConfigurationType ActiveConfiguration;

void ConfigurationInit(void)
{
    memcpy_P(&ActiveConfiguration,
            &ConfigurationTable[CONFIG_NONE], sizeof(ConfigurationType));

    ConfigurationSetById(GlobalSettings.ActiveSettingPtr->Configuration);
}

void ConfigurationSetById( ConfigurationEnum Configuration )
{
    CodecDeInit();
    CommandLinePendingTaskBreak(); // break possibly pending task
    
    GlobalSettings.ActiveSettingPtr->Configuration = Configuration;

    /* Copy struct from PROGMEM to RAM */
    memcpy_P(&ActiveConfiguration,
            &ConfigurationTable[Configuration], sizeof(ConfigurationType));

    CodecInit();
    ApplicationInit();

    /* Notify LED. blink according to current setting */
    LEDHook(LED_SETTING_CHANGE, LED_BLINK + Configuration);
}

void ConfigurationGetByName(char* Configuration, uint16_t BufferSize)
{
    MapIdToText(ConfigurationMap, ARRAY_COUNT(ConfigurationMap), 
                GlobalSettings.ActiveSettingPtr->Configuration, Configuration, BufferSize);
}

MapIdType ConfigurationCheckByName(const char *Configuration) {
    MapIdType Id;
    if (MapTextToId(ConfigurationMap, ARRAY_COUNT(ConfigurationMap), Configuration, &Id)) {
        return Id;
    }
    return 0xff;
}

bool ConfigurationSetByName(const char* Configuration)
{
    MapIdType Id;

    if (MapTextToId(ConfigurationMap, ARRAY_COUNT(ConfigurationMap), Configuration, &Id)) {
        ConfigurationSetById(Id);
        LogEntry(LOG_INFO_CONFIG_SET, Configuration, StringLength(Configuration, CONFIGURATION_NAME_LENGTH_MAX-1));
        return true;
    } else {
        return false;
    }
}

void ConfigurationGetList(char* List, uint16_t BufferSize)
{
    MapToString(ConfigurationMap, ARRAY_COUNT(ConfigurationMap), List, BufferSize);
}

