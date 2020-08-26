#### Makefile for the libnfc-based tests in C
#### These tests are compiled for the local host system, not for AVR platforms

CC=gcc
CFLAGS= -DDRIVER_PN53X_USB_ENABLED \
		-g -O2 -Wall -pedantic -Wextra -std=c99 -Du_int8_t=uint8_t -Du_int16_t=uint16_t
LD=ld
LDFLAGS= -lnfc -lusb -lc

BINDIR=./Bin
BINEXT=exe
OBJDIR=./Obj
OBJEXT=o

FILE_BASENAMES=NFCAntiCollisionMod
BINOUTS=$(addprefix $(BINDIR)/, $(addsuffix .$(BINEXT), $(basename $(FILE_BASENAMES))))

all: default

default: $(BINOUTS)

$(OBJDIR)/%.$(OBJEXT): %.c
	$(CC) $(CFLAGS) $< -c -o $@

$(BINDIR)/%.$(BINEXT): $(OBJDIR)/%.o
	$(LD) $(LDFLAGS) $< -o $@

clean:
	@rm -f $(OBJDIR)/* $(BINDIR)/*

