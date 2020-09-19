#### Makefile for the libnfc-based tests in C
#### These tests are compiled for the local host system, not for AVR platforms

CC=gcc-10
CFLAGS= -ILocalInclude -ISource -DDRIVER_PN53X_USB_ENABLED \
		-g -O0 -Wall -pedantic -Wextra -std=c99 -Du_int8_t=uint8_t -Du_int16_t=uint16_t \
		`pkg-config libnfc --cflags` -I/usr/local/opt/openssl/include \
		-DLIBNFC_LOGLEVEL=3 -DHOST_BUILD -DCRYPTO_AES_DEFAULT=1\
		-I../ExternalCryptoLib
LD=gcc-10
LDFLAGS= -lc `pkg-config libnfc --libs` \
		 -L/usr/local/opt/openssl/lib -lssl -lcrypto

BINDIR=./Bin
BINEXT=exe
OBJDIR=./Obj
OBJEXT=o

FILE_BASENAMES=NFCAntiCollisionMod \
			   TestAuthenticateAES128 \
			   TestAuthenticateIso \
			   TestAuthenticateLegacy \
			   TestGeneralCommands \
			   TestKeyManagementCommands \
			   TestApplicationManagementCommands \
			   TestFileManagementCommandsSupport \
			   TestDataManipulationCommandsSupport \
			   TestDataManipulationCommandsSupport2
OBJFILES=$(addprefix $(OBJDIR)/, $(addsuffix .$(OBJEXT), $(basename $(FILE_BASENAMES))))
BINOUTS=$(addprefix $(BINDIR)/, $(addsuffix .$(BINEXT), $(basename $(FILE_BASENAMES))))

.SECONDARY: $(OBJFILES)
.PRECIOUS: $(OBJFILES)

all: clean default

default: $(BINOUTS)

$(OBJDIR)/%.$(OBJEXT): Source/%.c
	$(CC) $(CFLAGS) $< -c -o $@
	@/bin/bash ./Scripts/LibNFCObjectDefsRename.sh $@

$(BINDIR)/%.$(BINEXT): $(OBJDIR)/%.$(OBJEXT)
	$(LD) $(LDFLAGS) $< -o $@

clean:
	@rm -f $(OBJDIR)/* $(BINDIR)/*

