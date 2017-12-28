#ifndef SD_H
#define SD_H

/*
*******************************************************************************
*        #include
*******************************************************************************
*/

//#include "devio.h"
#include "typedef.h"

/*
*******************************************************************************
*        #define CONSTANTS
*******************************************************************************
*/
#define SD_PASS                             0
#define SD_BAD_CRC7                         -1
#define SD_RES_TIMEOUT                      -2
#define SD_READ_TIMEOUT                     -3
#define SD_WRITE_TIMEOUT                    -4
#define SD_TRANSFER_FAIL                    -5
#define SD_PROGRAM_FAIL                     -6
#define SD_BAD_CRC16                        -7
#define SD_CARD_NOT_SUPPORT                 -8
#define SD_GENERAL_FAIL                     -9
#define SD_CARD_ATTACH                      -10
#define SD_CARD_REMOVE                      -11
extern DWORD	_gdwFileSize;

/*
*******************************************************************************
*        GLOBAL MACROS
*******************************************************************************
*/

/*
*******************************************************************************
*        GLOBAL DATA TYPES 
*******************************************************************************
*/

/*
*******************************************************************************
*        GLOBAL VARIABLES
*******************************************************************************
*/
/*
*******************************************************************************
*        GLOBAL FUNCTION PROTOTYPES
*******************************************************************************
*/

//void    SdInit(DVIO_DEV *, BYTE *, BYTE *);
//SBYTE   SdCardDetect(DVIO_DEV *);                                 /*this function only allow called for DvioCardDetect()*/
void    FileSystem (void);
void    Main_SDCARD(void);
BYTE    FileRead(BYTE *, void *);
void    RegBaseAddrInit(void);
BYTE    Sd_R_Exec_fend(void *, DWORD, DWORD);
BYTE    Sd_R_Exec_bend( DWORD);
void 	SdBoot(void);
void     Sd_Deselect(void);
void     Sd_Select(void);

//void    SdInsert();
//void    SdRemove();
//BOOL    SdOn();
//BOOL    SdOff();
//void    SdIsr();
//BYTE    Sd_Write(DVIO_DEV* ,DWORD , DWORD, BYTE*);
//BYTE    Sd_Read(DVIO_DEV* , DWORD , DWORD, BYTE* );
//BOOL    SdWp();
//BOOL    TflashWp();

/*
*******************************************************************************
*        #ERROR SECTION
*******************************************************************************
*/
#endif
