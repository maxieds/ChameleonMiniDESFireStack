/* DESFireStatus.h 
 * maxieds@gmail.com
 */

#ifndef __DESFIRE_STATUS_H__
#define __DESFIRE_STATUS_H__

#include "DESFireBackEnd.h"
#include "DESFireCrypto.h"

// TODO: Needs to be reorganized into the new files ... 


extern uint8_t NO_KEY_AUTHENTICATED;
extern uint8_t MASTER_FILE_AID[3];
extern uint8_t CHECKSUM_IV[4];

extern uint8_t FRAME_CONTINUE[];
extern uint8_t OPERATION_OK[];
extern uint8_t OK[];
extern uint8_t INIT[];
extern uint8_t VERSION_FULL[];
extern uint8_t VERSION_1[];
extern uint8_t VERSION_2[];
extern uint8_t VERSION_3[];

#define CLA_PROTECTED_APDU           (0x0c)
#define OFFSET_CLA                   (0)
#define OFFSET_INS                   (1)
#define OFFSET_P1                    (2)
#define OFFSET_P2                    (3)
#define OFFSET_LC                    (4)
#define OFFSET_CDATA                 (5)
#define CLA_ISO7816                  (0)
#define INS_SELECT                   ((uint8_t) -92)
#define INS_EXTERNAL_AUTHENTICATE    ((uint8_t) -126)
#define P1_DF                        (0x04)
#define P2_SELECT                    (0x0C)


// TODO: initialize in C file source ... 
extern uint8_t securityLevel;
extern uint8_t fileSecurityLevel;
extern uint8_t SessionKey[DESFIRE_CRYPTO_SESSION_KEY_SIZE];
extern uint8_t SessionIV[DESFIRE_CRYPTO_IV_SIZE];
extern uint8_t randomNumberToAuthenticate[MAX_DATA_SIZE];
extern uint8_t selectedDirectoryFileID;
extern uint8_t selectedFileID;
extern DESFireInstruction commandToContinue;
extern uint16_t readed;
extern uint16_t offsetToContinue;
extern uint16_t bytesLeft;
extern uint8_t keyNumberToAuthenticate;
extern uint8_t authenticated;
extern uint8_t dataBuffer[MAX_DATA_SIZE];
extern uint8_t legacyMode;

#define DO87_START     ((uint8_t) 0x87)
#define DO87_TYPE      ((uint32_t) 0x00000080)
#define DO87_BYTENO    ((uint8_t) 0x7F)
#define DO87_END       ((uint8_t) 0x01);
#define LE_MAX         ((uint16_t) 256);
    
/**
  * Response status : No Error = (short)0x9000
  */
#define SW_NO_ERROR                 ((uint16_t) -28672)
    
/**
  * Response status : Response bytes remaining = 0x6100
  */
#define SW_BYTES_REMAINING_00       ((uint16_t) 24832)
    
/**
  * >Response status : Wrong length = 0x6700
  */
#define SW_WRONG_LENGTH             ((uint16_t) 26368)
    
/**
  * Response status : Security condition not satisfied = 0x6982
  */
public static final short SW_SECURITY_STATUS_NOT_SATISFIED = 27010;
    
/**
  * Response status : File invalid = 0x6983
  */
public static final short SW_FILE_INVALID = 27011;
    
/**
  * Response status : Data invalid = 0x6984
  */
public static final short SW_DATA_INVALID = 27012;
    
/**
  * Response status : Conditions of use not satisfied = 0x6985
  */
public static final short SW_CONDITIONS_NOT_SATISFIED = 27013;
    
/**
  * Response status : Command not allowed (no current EF) = 0x6986
  */
public static final short SW_COMMAND_NOT_ALLOWED = 27014;
    
/**
  * Response status : Applet selection failed = 0x6999;
  */
public static final short SW_APPLET_SELECT_FAILED = 27033;
    
/**
  * Response status : Wrong data = 0x6A80
  */
public static final short SW_WRONG_DATA = 27264;
    
/**
  * Response status : Function not supported = 0x6A81
  */
public static final short SW_FUNC_NOT_SUPPORTED = 27265;
    
/**
  * Response status : File not found = 0x6A82
  */
public static final short SW_FILE_NOT_FOUND = 27266;
    
/**
  * Response status : Record not found = 0x6A83
  */
public static final short SW_RECORD_NOT_FOUND = 27267;
    
/**
  * Response status : Incorrect parameters (P1,P2) = 0x6A86
  */
public static final short SW_INCORRECT_P1P2 = 27270;
    
/**
  * Response status : Incorrect parameters (P1,P2) = 0x6B00
  */
public static final short SW_WRONG_P1P2 = 27392;
    
/**
  * Response status : Correct Expected Length (Le) = 0x6C00
  */
public static final short SW_CORRECT_LENGTH_00 = 27648;
    
/**
  * Response status : INS value not supported = 0x6D00
  */
public static final short SW_INS_NOT_SUPPORTED = 27904;
    
/**
  * Response status : CLA value not supported = 0x6E00
  */
public static final short SW_CLA_NOT_SUPPORTED = 28160;
    
/**
  * Response status : No precise diagnosis = 0x6F00
  */
public static final short SW_UNKNOWN = 28416;
    
/**
  * Response status : Not enough memory space in the file  = 0x6A84
  */
public static final short SW_FILE_FULL = 27268;
    
/**
  * Response status : Card does not support logical channels  = 0x6881
  */
public static final short SW_LOGICAL_CHANNEL_NOT_SUPPORTED = 26753;
    
/**
  * Response status : Card does not support secure messaging = 0x6882
  */
public static final short SW_SECURE_MESSAGING_NOT_SUPPORTED = 26754;
    
/**
  * Response status : Warning, card state unchanged  = 0x6200
  */
public static final short SW_WARNING_STATE_UNCHANGED = 25088;
public static final short SW_REFERENCE_DATA_NOT_FOUND = (short) 0x6A88;	
public static final short SW_INTERNAL_ERROR = (short) 0x6d66;

#endif
