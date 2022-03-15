NOTES ABOUT THE RECENCY OF THIS REPOSITORY -- AND WHERE TO FIND UP-TO-DATE DESFIRE EMULATION SUPPORT FOR THE CHAMELEON (REVG):
==============================================================================================================================

This repository now reflects the historical modifications and commit history made by **@maxieds** in 2020 to update 
and finalize the DESFire emulation support started by **@dev-zzo** in approximately 2017. Its contents have since 
been merged into the main Chameleon (RevG) firmware sources by [pull request (PR) #287](https://github.com/emsec/ChameleonMini/pull/287). 
More changes and better reader support are in the works over at [my fork of the main (emsec) source code](https://github.com/maxieds/ChameleonMini/). 
Please consult the most recent additions and sub-branches there if you want the most up-to-date 
DESFire support in active development. (For example, see [this DESFire-AuthISO-Patch](https://github.com/maxieds/ChameleonMini/tree/DESFire-AuthISO-Patch) 
which is still being tested and verified as working with the PM3 hardware -- 
It still fixes a lot of small bugs aside from the PM3 compatibility issues getting worked out right now.) 
Finalized, and well tested, known working versions 
of the forked development code will in due time (eventually) get submitted as PRs to the main firmware sources. 
At this point, I am going to archive this repository to keep it around and preserved for posterity's sake.  🙂

Chameleon Mini Mifare DESFire tag emulation support
===================================================

This project aims to bring a full featured Mifare DESFire tag emulation support to the standard sources for the Chameleon Mini RevG firmware.

* [Latest releases with firmware binaries](https://github.com/maxieds/ChameleonMiniDESFireStack/releases)
* [Primary source code modifications](https://github.com/maxieds/ChameleonMiniDESFireStack/tree/master/Firmware/Chameleon-Mini/Application/DESFire)
* [Current features support tracking table](https://github.com/maxieds/ChameleonMiniDESFireStack/blob/master/Firmware/Chameleon-Mini/Application/DESFire/Docs/DESFireSupportReadme.md#tables-of-tested-support-for-active-commands)
* [DESFire specific Chameleon Terminal commands](https://github.com/maxieds/ChameleonMiniDESFireStack/blob/master/Firmware/Chameleon-Mini/Application/DESFire/Docs/DESFireSupportReadme.md#chameleon-mini-terminal-addons-to-support-configmf_desfire-modes)
* [In-depth libnfc-based testing code in C (host side)](https://github.com/maxieds/ChameleonMiniDESFireStack/tree/master/Firmware/Chameleon-Mini/Application/DESFire/Testing) -- also can be compared with [sample outputs of the native command set](https://github.com/maxieds/ChameleonMiniDESFireStack/tree/master/Firmware/Chameleon-Mini/Application/DESFire/Testing/SampleOutputDumps)
* [Credits for sources and past code](https://github.com/maxieds/ChameleonMiniDESFireStack/blob/master/Firmware/Chameleon-Mini/Application/DESFire/Docs/DESFireSupportReadme.md#credits)
