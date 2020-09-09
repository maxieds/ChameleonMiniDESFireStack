/* ANSIStringPrinting.h */

#ifndef __ANSI_STRING_FMTPRINTING_H__
#define __ANSI_STRING_FMTPRINTING_H__

#include <stdlib.h>
#include <string.h>

typedef const char * ANSIPrintFmtCode;

const ANSIPrintFmtCode __BLACK           = "\x1b[0;30m";
const ANSIPrintFmtCode __RED             = "\x1b[0;31m";
const ANSIPrintFmtCode __GREEN           = "\x1b[0;32m";
const ANSIPrintFmtCode __BROWN           = "\x1b[0;33m";
const ANSIPrintFmtCode __BLUE            = "\x1b[0;34m";
const ANSIPrintFmtCode __PURPLE          = "\x1b[0;35m";
const ANSIPrintFmtCode __CYAN            = "\x1b[0;36m";
const ANSIPrintFmtCode __LIGHT_GRAY      = "\x1b[0;37m";
const ANSIPrintFmtCode __DARK_GRAY       = "\x1b[1;30m";
const ANSIPrintFmtCode __LIGHT_RED       = "\x1b[1;31m";
const ANSIPrintFmtCode __LIGHT_GREEN     = "\x1b[1;32m";
const ANSIPrintFmtCode __YELLOW          = "\x1b[1;33m";
const ANSIPrintFmtCode __LIGHT_BLUE      = "\x1b[1;34m";
const ANSIPrintFmtCode __LIGHT_PURPLE    = "\x1b[1;35m";
const ANSIPrintFmtCode __LIGHT_CYAN      = "\x1b[1;36m";
const ANSIPrintFmtCode __LIGHT_WHITE     = "\x1b[1;37m";
const ANSIPrintFmtCode __BOLD            = "\x1b[1m";
const ANSIPrintFmtCode __FAINT           = "\x1b[2m";
const ANSIPrintFmtCode __ITALIC          = "\x1b[3m";
const ANSIPrintFmtCode __UNDERLINE       = "\x1b[4m";
const ANSIPrintFmtCode __BLINK           = "\x1b[5m";
const ANSIPrintFmtCode __NEGATIVE        = "\x1b[7m";
const ANSIPrintFmtCode __CROSSED         = "\x1b[9m";
const ANSIPrintFmtCode __END             = "\x1b[0m";

#endif
