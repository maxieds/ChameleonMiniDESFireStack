# Credits: DESFire Chameleon Mini firmware implementation 

This source code has been written, adapted, and modified by [Maxie D. Schmidt](https://github.com/maxieds) to
finally add the hard firmware spec -- complete DESFire NFC tag support for the Chameleon Mini RevG based devices!

Any grateful users that benefit from this new DESFire tag
implementation for the Chameleon hardware should consider donating to the author / re-compiler (MDS)
of this C code through sponsored freelance software work, or possibly donate hardware to support my
NFC and Chameleon related [other projects](https://github.com/maxieds/ChameleonMiniLiveDebugger)
by visiting [this site](https://patreon.com/maxieds).
Much credit should also be attributed to the difficult implementations adapted from Java / Android
cited below.

## Direct funding sources for this project

The author would like to thank the following direct sources of funding and support 
for development of this project:
* Professor [Josephine Yu](http://people.math.gatech.edu/~jyu67/) and the 
  [School of Mathematics](https://math.gatech.edu) at the 
  Georgia Institute of Technology for allowing me to work on this as a secondary 
  project as a Ph.D. candidate over the Summer and Fall of 2020. 
* The [KAOS manufacturers](https://shop.kasper.it) for providing support in the form of discounted Chameleon RevG 
  devices to support my active development on the project. 

## Sources of external code and open information about the DESFire specs 

The source code for much of this implementation has been directly adapted, or modified, from mostly Java
language open source code for Android using several primary sources. Where possible, the license and credits
for the original sources for this ``avr-gcc``-compatible C language code are as specified in the next
repositories and code bases:
* [Prior DESFire Chameleon Mini Firmware Sourced (dev-zzo)](https://github.com/dev-zzo/ChameleonMini)
* [Android DESFire Host Card Emulation / HCE (jekkos)](https://github.com/jekkos/android-hce-desfire)
* [Android HCE Framework Library (kevinvalk)](https://github.com/kevinvalk/android-hce-framework)
* [AVRCryptoLib in C](https://github.com/cantora/avr-crypto-lib)
* [LibFreefare DESFire Code (mostly as a reference and check point)](https://github.com/nfc-tools/libfreefare/tree/master/libfreefare)

## Clarification: Where the local licenses apply 

The code that is not already under direct license (see below) is released according to the normal
[license for the firmware](https://github.com/emsec/ChameleonMini/blob/master/LICENSE.txt).
Additional licenses that apply only to the code used within this DESFire stack implementation,
or to the open source libraries used to derive this code,
are indicated within the local firmware directories.

