# This makefile uses sh in order to build the project.
# Note when using AVR-Toolchain, you have to install cygwin and 
# append cygwin's bin directory to the PATH environment variable 
# because Atmel does not ship sh.exe anymore with the toolchain. 
SHELL = /bin/bash

#Supported configurations
SETTINGS    += -DCONFIG_MF_CLASSIC_1K_SUPPORT
SETTINGS    += -DCONFIG_MF_CLASSIC_1K_7B_SUPPORT
SETTINGS    += -DCONFIG_MF_CLASSIC_4K_SUPPORT
SETTINGS    += -DCONFIG_MF_CLASSIC_4K_7B_SUPPORT
SETTINGS    += -DCONFIG_MF_ULTRALIGHT_SUPPORT 
SETTINGS    += -DCONFIG_MF_DESFIRE_SUPPORT
SETTINGS    += -DCONFIG_ISO14443A_SNIFF_SUPPORT
SETTINGS    += -DCONFIG_ISO14443A_READER_SUPPORT

#Support magic mode on mifare classic configuration
SETTINGS	+= -DSUPPORT_MF_CLASSIC_MAGIC_MODE

#Don't touch manufacturer byte with BUTTON_ACTION_UID_LEFT_(DE/IN)CREMENT
SETTINGS	+= -DSUPPORT_UID7_FIX_MANUFACTURER_BYTE

#Support activating firmware upgrade mode through command-line
SETTINGS	+= -DSUPPORT_FIRMWARE_UPGRADE

#Default configuration
#SETTINGS	+= -DDEFAULT_CONFIGURATION=CONFIG_MF_CLASSIC_1K
#SETTINGS	+= -DDEFAULT_CONFIGURATION=CONFIG_MF_CLASSIC_4K
#SETTINGS	+= -DDEFAULT_CONFIGURATION=CONFIG_MF_ULTRALIGHT
SETTINGS	+= -DDEFAULT_CONFIGURATION=CONFIG_NONE
#SETTINGS	+= -DDEFAULT_CONFIGURATION=CONFIG_ISO14443A_READER
#SETTINGS       += -DDEFAULT_CONFIGURATION=CONFIG_MF_DESFIRE
#SETTINGS       += -DDEFAULT_CONFIGURATION=CONFIG_ISO14443A_SNIFF

#Default button actions
#SETTINGS	+= -DDEFAULT_RBUTTON_ACTION=BUTTON_ACTION_UID_RANDOM
#SETTINGS	+= -DDEFAULT_RBUTTON_ACTION=BUTTON_ACTION_UID_LEFT_INCREMENT
#SETTINGS	+= -DDEFAULT_RBUTTON_ACTION=BUTTON_ACTION_UID_RIGHT_INCREMENT
#SETTINGS	+= -DDEFAULT_RBUTTON_ACTION=BUTTON_ACTION_UID_LEFT_DECREMENT
#SETTINGS	+= -DDEFAULT_RBUTTON_ACTION=BUTTON_ACTION_UID_RIGHT_DECREMENT
SETTINGS	+= -DDEFAULT_RBUTTON_ACTION=BUTTON_ACTION_CYCLE_SETTINGS
#SETTINGS	+= -DDEFAULT_RBUTTON_ACTION=BUTTON_ACTION_STORE_MEM
#SETTINGS	+= -DDEFAULT_LBUTTON_ACTION=BUTTON_ACTION_RECALL_MEM
SETTINGS	+= -DDEFAULT_LBUTTON_ACTION=BUTTON_ACTION_PRINT_STATE

#Define if button action setting should be independent of active setting 
SETTINGS	+= -DBUTTON_SETTING_GLOBAL

#Default LED functions
SETTINGS	+= -DDEFAULT_RED_LED_ACTION=LED_SETTING_CHANGE
SETTINGS	+= -DDEFAULT_GREEN_LED_ACTION=LED_POWERED

#Define if LED function setting should be independent of active setting
SETTINGS	+= -DLED_SETTING_GLOBAL

#Default logging mode
#SETTINGS	+= -DDEFAULT_LOG_MODE=LOG_MODE_OFF
#SETTINGS	+= -DDEFAULT_LOG_MODE=LOG_MODE_MEMORY
SETTINGS	+= -DDEFAULT_LOG_MODE=LOG_MODE_LIVE

#Define if log settings should be global
SETTINGS	+= -DLOG_SETTING_GLOBAL

#Default setting
SETTINGS	+= -DDEFAULT_SETTING=SETTINGS_FIRST

#Default pending task timeout
SETTINGS	+= -DDEFAULT_PENDING_TASK_TIMEOUT=50 #* 100ms

#Default reader threshold
SETTINGS	+= -DDEFAULT_READER_THRESHOLD=400

#Use EEPROM to store settings
SETTINGS	+= -DENABLE_EEPROM_SETTINGS

#Whether or not to customize the USB identifier settings in the firmware:
SETTINGS	+= -DENABLE_LUFAUSB_CUSTOM_VERSIONS

#Whether or not to allow users Chameleon terminal access to change the DESFire configuration's 
#sensitive settings like manufacturer, serial number, etc.
SETTINGS	+= -DENABLE_PERMISSIVE_DESFIRE_SETTINGS
SETTINGS	+= -DALLOW_DESFIRE_TERMINAL_COMMANDS

#Set a default logging mode for debugging with the DESFire 
#emulation code:
SETTINGS	+= -DDESFIRE_DEFAULT_LOGGING_MODE=DEBUGGING
#SETTINGS	+= -DDESFIRE_DEFAULT_LOGGING_MODE=OFF

#Set a default testing mode setting (0 = OFF, non-NULL = ON):
SETTINGS	+= -DDESFIRE_DEFAULT_TESTING_MODE=1

#Feature: Use randomized UIDs that mask the actual secret UID until 
#the tag has been issued a successful authentication sequence: 
SETTINGS	+= -DDESFIRE_RANDOMIZE_UIDS_PREAUTH

#Anticipating that the implementation overhead is high with the 
#maximum storage allocations for the number of possible keys per 
#application directory, and/or the total number of AID numbered 
#directory slots, the following options will tweak this limitation: 
# -> Set DESFIRE_MEMORY_LIMITED_TESTING to shrink the defaults 
# -> Or explicitly define DESFIRE_CUSTOM_MAX_KEYS=##UINT## (per AID), 
# -> And/Or define DESFIRE_CUSTOM_MAX_APPS=##UINT## 
#    (total number of AID spaces available, not including the master 0x00)
SETTINGS	+= -DDESFIRE_MEMORY_LIMITED_TESTING
#SETTINGS	+= -DDESFIRE_CUSTOM_MAX_APPS=8
#SETTINGS	+= -DDESFIRE_CUSTOM_MAX_KEYS=6
#SETTINGS   += -DDESFIRE_CUSTOM_MAX_FILES=6
#SETTINGS	+= -DDESFIRE_USE_FACTORY_SIZES
#SETTINGS	+= -DDESFIRE_MAXIMIZE_SIZES_FOR_STORAGE

#Set a minimum incoming/outgoing log size so we do not spam the 
#Chameleon Mini logs to much by logging everything:
SETTINGS	+= -DDESFIRE_MIN_INCOMING_LOGSIZE=2
SETTINGS	+= -DDESFIRE_MIN_OUTGOING_LOGSIZE=2

#If we are planning on logging mostly to a live debugging console at runtime, 
#there is *a lot* of data space to be saved by minimizing the LogMem 
#buffer size. Define the following to optimize this space in the event of 
#live logging sessions:
SETTINGS	+= -DDESFIRE_OPTIMIZE_LIVE_LOGGING_SPACE

#Enable printing of crypto tests when a new DESFire emulation instance is started:
#SETTINGS	+= -DDESFIRE_RUN_CRYPTO_TESTING_PROCEDURE

#Memory definitions and objcopy flags to include sections in binaries
FLASH_DATA_ADDR = 0x01000 #Start of data section in flash
FLASH_DATA_SIZE = 0x0FFF0 #Size of data section in flash
FLASH_DATA_OBJCOPY = --set-section-flags=.flashdata="alloc,load"
SPM_HELPER_ADDR = 0x21FE0 #Start of SPM helper section. Should be last 32Byte in bootloader section
SPM_HELPER_OBJCOPY = --set-section-flags=.spmhelper="alloc,load"

#Build configuration
ifeq ($(OS),Windows_NT)
BUILD_DATE   = "\"$(shell date /t)\""
else
BUILD_DATE   = $(shell date +'\"%y%m%d\"')
endif
COMMIT_ID    = $(shell git rev-parse --short HEAD)
MCU          = atxmega128a4u
ARCH         = XMEGA
BOARD        = NONE
F_CPU        = 27120000
F_USB        = 48000000
TARGET       = Chameleon-Mini
OPTIMIZATION = s
DESFIRE_MAINSRC = Application/DESFire
SRC         += $(TARGET).c \
			LUFADescriptors.c \
			System.c \
			Configuration.c \
			Random.c \
			Common.c \
			Memory.c \
			Button.c \
			Log.c \
			Settings.c \
			LED.c \
			Map.c \
			AntennaLevel.c
SRC         += Terminal/Terminal.c \
			Terminal/Commands.c \
			Terminal/XModem.c \
			Terminal/CommandLine.c
SRC         += Codec/Codec.c \
			Codec/ISO14443-2A.c \
			Codec/Reader14443-2A.c \
			Codec/SniffISO14443-2A.c 
SRC         += Application/MifareUltralight.c \
			Application/MifareClassic.c \
			Application/ISO14443-3A.c \
			Application/Crypto1.c \
			Application/Reader14443A.c \
			Application/Sniff14443A.c
SRC         += $(DESFIRE_MAINSRC)/ExternalCryptoLib/AVRCryptoLib/AVRCryptoLib-SingleSource.c \
			$(DESFIRE_MAINSRC)/ExternalCryptoLib/ArduinoCryptoLib/ArduinoCryptoLib-SingleSource.c \
			Application/MifareDESFire.c \
			$(DESFIRE_MAINSRC)/DESFireApplicationDirectory.c \
			$(DESFIRE_MAINSRC)/DESFireChameleonTerminal.c \
			$(DESFIRE_MAINSRC)/DESFireCrypto.c \
			$(DESFIRE_MAINSRC)/DESFireFile.c \
			$(DESFIRE_MAINSRC)/DESFireISO14443Support.c \
			$(DESFIRE_MAINSRC)/DESFireInstructions.c \
			$(DESFIRE_MAINSRC)/DESFireLogging.c \
			$(DESFIRE_MAINSRC)/DESFireMemoryOperations.c \
			$(DESFIRE_MAINSRC)/DESFirePICCControl.c \
			$(DESFIRE_MAINSRC)/DESFirePICCHeaderLayout.c \
			$(DESFIRE_MAINSRC)/DESFireUtils.c 
SRC         += $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
ASRC         = MemoryAsm.S \
			Codec/Reader14443-ISR.S
ASRC        += $(DESFIRE_MAINSRC)/ExternalCryptoLib/AVRCryptoLib/gf256mul/gf256mul.S 
SRC         += $(ASRC)
LUFA_PATH    = ../LUFA
CC_FLAGS     = -std=gnu99 -D__PROG_TYPES_COMPAT__ \
			   -DUSE_LUFA_CONFIG_HEADER -DFLASH_DATA_ADDR=$(FLASH_DATA_ADDR) \
			   -DFLASH_DATA_SIZE=$(FLASH_DATA_SIZE) -DSPM_HELPER_ADDR=$(SPM_HELPER_ADDR) \
			   -DBUILD_DATE=$(BUILD_DATE) -DCOMMIT_ID=\"$(COMMIT_ID)\" $(SETTINGS) \
			   -D__AVR_ATxmega128A4U__ 
LD_FLAGS     = $(CC_FLAGS) -lc \
			   -Wl,--section-start=.flashdata=$(FLASH_DATA_ADDR) \
			   -Wl,--section-start=.spmhelper=$(SPM_HELPER_ADDR) \
				#-Wl,--defsym=__malloc_heap_start=0,--defsym=__malloc_heap_end=0 \
			   -Wl,--defsym=__heap_end=0,--defsym=__heap_start=0,--defsym=__malloc_margin=0
OBJDIR       = Bin
OBJECT_FILES = 

#AVRDUDE settings
AVRDUDE_PROGRAMMER = flip2
AVRDUDE_MCU = atxmega128a4u
AVRDUDE_PORT = usb
AVRDUDE_WRITE_APP = -U application:w:$(TARGET)-DESFire.hex
AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET)-DESFire.eep
AVRDUDE_FLAGS = -p $(AVRDUDE_MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)

#AVRDUDE settings to program the precompiled firmware
AVRDUDE_WRITE_APP_LATEST = -U application:w:Latest/Chameleon-Mini-DESFire.hex
AVRDUDE_WRITE_EEPROM_LATEST = -U eeprom:w:Latest/Chameleon-Mini-DESFire.eep

# Default target
all: desfire-dev

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
# include $(LUFA_PATH)/Build/lufa_doxygen.mk
# include $(LUFA_PATH)/Build/lufa_dfu.mk
# include $(LUFA_PATH)/Build/lufa_hid.mk
# include $(LUFA_PATH)/Build/lufa_avrdude.mk
# include $(LUFA_PATH)/Build/lufa_atprogram.mk

#Overwrite the LUFA versions of hex/bin file generation to include spmhelper and flashdata sections
%.hex: %.elf
	@echo $(MSG_OBJCPY_CMD) "(Chameleon) Extracting HEX file data from \"$<\""
	$(CROSS)-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature $(FLASH_DATA_OBJCOPY) $< $@

%.eep: %.elf
	@echo $(MSG_OBJCPY_CMD) "(Chameleon) Extracting EEP file data from \"$<\""
	$(CROSS)-objcopy -j .eeprom  --set-section-flags=.eeprom=alloc,load \
		--change-section-lma .eeprom=0  --no-change-warnings -O ihex \
		$(TARGET).elf $(TARGET).eep

%.bin: %.elf
	@echo $(MSG_OBJCPY_CMD) "(Chameleon) Extracting BIN file data from \"$<\""
	$(CROSS)-objcopy -O binary -R .eeprom -R .fuse -R .lock -R .signature $(FLASH_DATA_OBJCOPY) $< $@

# Extract SPMHelper in the last 32 Byte of the bootloader section to externally combine it with any bootloader
spmhelper: $(TARGET).elf
	@echo $(MSG_OBJCPY_CMD) "(Chameleon) Extracting SPM helper HEX file from $(TARGET).elf"
	$(CROSS)-objcopy -O ihex -j .spmhelper $(SPM_HELPER_OBJCOPY) $(TARGET).elf $(TARGET).hex
	
# Program the device using avrdude  
program: $(TARGET).hex $(TARGET).eep
	avrdude $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_APP) $(AVRDUDE_WRITE_EEPROM)

# Program the device using avrdude with the latest official firmware
program-latest:
	avrdude $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_APP_LATEST) $(AVRDUDE_WRITE_EEPROM_LATEST)

# Program the device using batchisp and the DFU bootloader
# Note that the device has to be in bootloader mode already
dfu-flip: $(TARGET).hex $(TARGET).eep
	cp $(TARGET).eep EEPROM.hex
	batchisp -hardware usb -device $(MCU) -operation erase f memory FLASH loadbuffer $(TARGET).hex \
		program verify memory EEPROM loadbuffer EEPROM.hex program verify start reset 0
	rm EEPROM.hex

# Program the device using dfu-programmer
dfu-prog: $(TARGET).hex $(TARGET).eep
	dfu-programmer $(MCU) erase
	dfu-programmer $(MCU) flash-eeprom $(TARGET).eep
	dfu-programmer $(MCU) flash $(TARGET).hex
	dfu-programmer $(MCU) reset

desfire-dev: $(TARGET).elf $(TARGET).hex $(TARGET).eep
	@cp $(TARGET).hex $(TARGET)-DESFire.hex
	@cp $(TARGET).eep $(TARGET)-DESFire.eep
	$(CROSS)-size --mcu=atmega128 -C $(TARGET).elf

desfire-dev-copy-lastest: desfire-dev
	@cp $(TARGET)-DESFire.hex $(TARGET)-DESFire.eep Latest/

local-clean:
	@rm -f $(TARGET).{elf,hex,eep,bin,lss,map} $(TARGET)-DESFire.{hex,eep}
	@rm -rf $(OBJDIR)
	@mkdir $(OBJDIR)
	@rm -f Application/DESFire/*.code

