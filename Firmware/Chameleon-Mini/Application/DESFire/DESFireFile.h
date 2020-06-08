/* 
 * DESFireFile.h : 
 * Maxie D. Schmidt (github.com/maxieds) 
 */

#ifndef __DESFIRE_EFFILE_H__
#define __DESFIRE_EFFILE_H__

#define EFFILE_DATA_CHUNK_SIZE            (4)
#define EFFILE_DATA_CHUNK_ALIGNAT         __attribute__((align(EFFILE_DATA_CHUNK_SIZE)))

// TODO: File types on p. 15 of the datasheet ... 

// See pages 48-49 of the datasheet for more details ... 
typedef enum {
     FTYPE_STD_DATA       = 1, 
     FTYPE_BACKUP_DATA    = 2, 
     FTYPE_VALUE          = 3,
     FTYPE_LINEAR_RECORD  = 4, 
     FTYPE_CYCLIC_RECORD  = 5,
} DESFireFileType;

// TODO: Page 57: Read file functions ... 
// TODO: Create and write file functions ... 

#endif
