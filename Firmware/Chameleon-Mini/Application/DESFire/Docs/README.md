# Source for Mifare DESFire tag support in the Chameleon Mini firmware

# Resources and references 

The following detailed PDF resources are available to help with parsing the more subtle details 
to the mutifaceted Mifare DESFire tag specifications:
* [DESFire Specs and Datasheet (MF3 IC D40 -- Dated April, 2004)](https://web.archive.org/web/20170201031920/http://neteril.org/files/M075031_desfire.pdf)
* [NXP Application Note (Contains some helpful hints and common snags)](https://www.nxp.com/docs/en/application-note/AN4453.pdf)
* [ISO/IEC 7816-4 Standard](http://www.unsads.com/specs/ISO/7816/ISO7816-4.pdf)
* [LibFreeFare DESFire main source file](https://github.com/nfc-tools/libfreefare/blob/5459806659d5f6aa0e7705b88c48775ea6c861a6/libfreefare/mifare_desfire.c)
* [LibFreeFare main header and typedefs](https://github.com/nfc-tools/libfreefare/blob/5459806659d5f6aa0e7705b88c48775ea6c861a6/libfreefare/freefare.h)
* [Crypto Authenticate examples](https://hack.cert.pl/files/desfire-9f122c71e0057d4f747d2ee295b0f5f6eef8ac32.html)
* [LibFreeFare DESFire crypto source](https://github.com/nfc-tools/libfreefare/blob/5459806659d5f6aa0e7705b88c48775ea6c861a6/libfreefare/mifare_desfire_crypto.c)
* [LibFreeFare Crypto Key Source](https://github.com/nfc-tools/libfreefare/blob/5459806659d5f6aa0e7705b88c48775ea6c861a6/libfreefare/mifare_desfire_key.c)
* [Random UIDs](https://stackoverflow.com/questions/29819356/apdu-for-getting-uid-from-mifare-desfire)

Additionally, the following weblinks and posts clarify some common reference points:

* [StackOverflow post on emulating DESFire on Android](https://stackoverflow.com/a/20068329)
* [StackOverflow post on Android NFC support for DESFire](https://stackoverflow.com/a/26775311)
* [Helpful source examples and hints on AES authenticate](https://www.linkedin.com/pulse/mifare-desfire-introduction-david-coelho)
* [Explanation of send/receive padding in encrypted transfer modes (with diagrams)](https://github.com/nceruchalu/easypay/blob/master/mifare/mifare_crypto.c#L707)

## DESFire communication examples

* [AES crypto](https://stackoverflow.com/questions/38283998/desfire-ev1-communication-examples)
* [AES crypto pitfalls (with detailed examples)](https://hack.cert.pl/files/desfire-9f122c71e0057d4f747d2ee295b0f5f6eef8ac32.html)
* [DESFire examples in Python Gist](https://gist.github.com/dev-zzo/87e0947f3ca0bb6d6baf78dd4d0ecb9c) 
* [Proxmark response notes](https://github.com/zhovner/proxmark3-1/blob/master/common/protocols.h)

## ISO7816 Command set implementation hints:

* [External Authenticate](https://stackoverflow.com/questions/34425494/external-authentication-in-desfire-card-with-iso-7816-4-apdus)
* [Overview of command structure](https://www.informit.com/articles/article.aspx?p=29265&seqNum=6)
* [Detailed Docs](https://cardwerk.com/smart-card-standard-iso7816-4-section-6-basic-interindustry-commands/)

# TODO

## Short list 

* Add in logging and debugging print statements **everywhere** 
* grep through the "TODO" marked parts in the source files 
* Read though the old DESFire EV0 datasheet to make sure are correctly handling all of the SW response code cases 
* If ``LocalTestingMode != 0``, then need to implement a solution so can test with predictable 
  (non-randomized) crypto vectors. In particular, ``RndA`` (random number associated with the authenticate 
  session) and the value of the ``SessionIV`` crypto salt should be fixed to facilitate debugging.
* **All** the crypto needs to be re-routed through the well tested AVR board crypto library. 
  This is going to require writing wrappers around their excellent existing GPL routines. 
  Then can consult with the LibFreeFare implementation for DESFire tags to glue this together with the 
  Send / Receive protocol exchanges with MAC/CRC checksums going on.
* Need to document the reasons for the obscured file system layout that has to 
  put in-use accounting arrays into EEPROM and fetch them every time a request is made: 
  The ATXMega128 avr chip (even, for upgraded RevG boards) has severely limited ``.data`` segment 
  space. To the point that even a window of 1024bytes is the difference between colliding with other 
  critical data at runtime. The slower EEPROM, on the other hand, is substantially more plentiful. 
  And with a moderately small number of requests for small-ish files going through the tag at 
  any given time, this solution is the only way to maintain all the runtime data for actively selected 
  applications in a somewhat state-of-art pushing standard that is the ever spec-wise unknown DESFire stack. 
* See what data segment space can be saved by setting ``__attribute__((...))`` packed on big union 
  structures like ``TransferStateType`` versus aligned at one for the component structs?
* May need to allocate the ``SelectedAppCacheType`` structures on an as-needed basis.
* Where does the default ``CommMode`` get invoked? For all transfers, or just 
  for sensitive file and key data transfers? 
* Need to store info about key type and byte size? 
* Need a general function for transferring data based on the ``ActiveCommMode``.
* Try to add extended support for the ``CLONE`` command when DESFire emulation is on the 
  active slot.
* See all the example functionality in [libfreefare](https://github.com/nfc-tools/libfreefare/tree/master/examples).

## Feature requests (for debugging) 
* Have an action where a (long) push of a button allows for 
     1. A dump of the stored internal logging data to get written LIVE-style to the serial USB 
     2. A dump of the pretty-printed DESFire tag layout to get written to the serial USB 
* Store some compile-time randomized system bits (from openssl) to get stored to a special 
  small enough segment within the EEPROM for reference (sort of like a secret UID data, or even 
  unique serial number that should get reprogrammed everytime the firmware is re-compiled ... 
  1. ``#define EEPROM_ATTR __attribute__ ((section (".eeprom")))``

## Instructions implementation and testing table (TODO)
