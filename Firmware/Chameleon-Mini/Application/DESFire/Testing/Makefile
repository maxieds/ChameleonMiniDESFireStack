#### Makefile for the libnfc-based tests in C
#### These tests are compiled for the local host system, not for AVR platforms

CC=g++
CFLAGS= -O0 --std=posix --std=gnu++98 -DDRIVER_PN53X_USB_ENABLED -I/usr/local/include \
		-stdlib=libc++
LD=g++
LDFLAGS= $(CFLAGS) -L/usr/local/lib /usr/local/lib/libnfc.a -lnfc -lusb -lc++

BINDIR=./Bin
BINEXT=exe
OBJDIR=./Obj
OBJEXT=o

FILE_BASENAMES=NFCAntiCollisionMod
BINOUTS=$(addprefix $(BINDIR)/, $(addsuffix .$(BINEXT), $(basename $(FILE_BASENAMES))))

all: default

default: $(BINOUTS)

$(OBJDIR)/%.$(OBJEXT): %.cpp
	$(CC) $(CFLAGS) $< -c -o $@

$(BINDIR)/%.$(BINEXT): $(OBJDIR)/%.o
	$(LD) $(LDFLAGS) $< -o $@

clean:
	@rm -f $(OBJDIR)/* $(BINDIR)/*

