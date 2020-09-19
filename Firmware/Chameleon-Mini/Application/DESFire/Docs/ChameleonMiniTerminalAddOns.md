# DESFire specific Chameleon Mini terminal commands 

To make debugging and other parts of life with a DESFire stack more fun, 
there are some enhancements to the standard Chameleon Mini terminal 
command line that can be accessed through a serial USB console, 
or for example, using open source 
[my live logging and debugging app](https://github.com/maxieds/ChameleonMiniLiveDebugger) 
that allowed me to test this software in real time.

Note that these commands are only compiled into the firmware when the compiler 
option ``-DALLOW_DESFIRE_TERMINAL_COMMANDS`` and ``-DCONFIG_MF_DESFIRE_SUPPORT`` are 
specified. If you do not at miniumum have a configuration, ``CONFIG=MF_DESFIRE``, then 
these commands will not work.

## DF_SETHDR -- Set PICC header information 

Since the UID and other precious manufacturer data are supposed to be unique to 
tags and sacred ground upon which only manufacturers shall walk, we are going to upheave 
this notion and hack our tag "roll your own" (tag) style. This means we can pick and 
choose the components used to identify the tag by calling a few Chameleon terminal 
command variants. The syntax is varied -- the numbers after the parameter name are 
indicators for the number of bytes to include as arguments: 
```bash
DF_SETHDR <HardwareVersion-2|SoftwareVersion-2|BatchNumber-5|ProductionDate-2> <HexBytes-N>
```
For example, run the following using ``minicom`` to list the tag properties currently enabled:
```bash
DF_SETHDR HardwareVersion
DF_SETHDR SoftwareVersion
DF_SETHDR BatchNumber
DF_SETHDR ProductionDate
```
Likewise, as promised, we can modify the tag header information emulated by the tag as follows:
```bash
DF_SETHDR HardwareVersion xx xx
DF_SETHDR SoftwareVersion xx xx
DF_SETHDR BatchNumber xx xx xx xx xx
DF_SETHDR ProductionDate xx xx
```
Note that the UID for the tag can be set using separate Chameleon terminal commands.

## DF_PPRINT_PICC -- Visualize tag contents

This lets users pretty print the tag layout in several different ways, and with 
a couple of options for verbosity. This helps with visualizing the landscape that 
we are programming. The syntax include: 
```bash
DF_PPRINT_PICC FullImage
DF_PPRINT_PICC HeaderData
DF_PPRINT_PICC ListDirs
```

## DF_FWINFO -- Print firmware revision information 

Self explanatory and similar to the familiar ``VERSION`` command. Syntax:
```bash 
DF_FWINFO
```

## DF_LOGMODE -- Sets the depth of (LIVE) logging messages printed at runtime

Syntax -- not guaranteeing that all of these are meaningful or distinct just yet: 
```bash
DF_LOGMODE?
DF_LOGMODE=?
DF_LOGMODE=<OFF|NORMAL|VERBOSE|DEBUGGING>
DF_LOGMODE=<0|1|TRUE|FALSE>
```

## DF_TESTMODE -- Sets whether the firmware emulation is run in testing/debugging mode

Syntax: 
```bash
DF_TESTMODE?
DF_TESTMODE=<0|1|TRUE|FALSE|OFF|ON>
```

