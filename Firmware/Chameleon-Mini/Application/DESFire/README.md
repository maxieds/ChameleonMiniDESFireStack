# Source for Mifare DESFire tag support in the stock Chameleon Mini firmware

This source code has been written, adapted, and modified by [Maxie D. Schmidt](https://github.com/maxieds) to 
finally add the holy grail -- complete DESFire NFC tag support for the Chameleon Mini RevG based devices! 
The code that is not already under direct license (see below) is released according to the normal 
[license for the firmware](https://github.com/emsec/ChameleonMini/blob/master/LICENSE.txt). 

Any grateful users that benefit from this new DESFire tag 
implementation for the Chameleon hardware should consider donating to the author / re-compiler (MDS)
of this C code through sponsored freelance software work, or possibly donate hardware to support my 
NFC and Chameleon related [other projects](https://github.com/maxieds/ChameleonMiniLiveDebugger)  
by visiting [this site](https://patreon.com/maxieds). 
Much credit should also be attributed to the difficult implementations adapted from Java / Android 
cited below. 

# External credits

The source code for much of this implementation has been directly adapted, or modified, from mostly Java 
language open source code for Android using several primary sources. Where possible, the license and credits 
for the original sources for this ``avr-gcc``-compatible C language code are as specified in the next 
repositories and code bases:
* [https://github.com/dev-zzo/ChameleonMini](https://github.com/dev-zzo/ChameleonMini) 
* [https://github.com/jekkos/android-hce-desfire](https://github.com/jekkos/android-hce-desfire)
* [https://github.com/kevinvalk/android-hce-framework](https://github.com/kevinvalk/android-hce-framework)
* Cryptographic routines adapted from the C++ code in TODO (see also: TODO)


