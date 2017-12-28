/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This header file contains the API for the File Manager. All APIs
 *    are prototyped in this file.
 *
 * Note:
 *
 * Author(s):
 *
 *  
 */


#ifndef FM_IF_H
#define FM_IF_H

/*
#include "tcg_if.h"
*/
/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */

/* 
 * Following are the services published and offered by the File Manager
 * Please see the definition (prototypes) of the functions associated
 * with these services. All clients should use these service numbers 
 * to request services from File Manager remotely.
 */
#define  FM_CREATE_SERVICE    0
#define  FM_DELETE_SERVICE    1
#define  FM_OPEN_SERVICE      2
#define  FM_CLOSE_SERVICE     3
#define  FM_READ_SERVICE      4
#define  FM_WRITE_SERVICE     5
#define  FM_RENAME_SERVICE    6
#define  FM_GET_IMAGE_FILE_CHECKSUM_SERVICE  7
#define  FM_SET_ANMT_FILE_FLAG_SERVICE       8
#define  FM_UPDATE_ANMT_FILE_TABLE_SERVICE   9 
#define  FM_UPDATE_TONE_PLAN_FILE_TABLE_SERVICE   10 
#define  FM_SET_TONE_TIMEOUT_SERVICE         11

/* 
 * Following  is the list of all possible error codes returned by the
 * fm module API. 
 */
#define FM_OK                                    0
#define FM_EQUAL_CHKSUM                          1
#define FM_EQUAL_REC                             2
#define FM_ERROR                                -1
#define FM_ILLEGAL_FILE_TYPE                    -2
#define FM_ILLEGAL_FILE_SIZE                    -3
#define FM_ILLEGAL_FILE_FLAGS                   -4
#define FM_ILLEGAL_FILE_NAME                    -5
#define FM_ILLEGAL_FILE_DESCR                   -6
#define FM_ILLEGAL_FILE_OFFSET                  -7
#define FM_ILLEGAL_ADDRESS                      -8
#define FM_ILLEGAL_LENGTH                       -9
#define FM_ILLEGAL_MEM_TYPE_READ                -10
#define FM_ILLEGAL_MEM_TYPE_WRITE               -11
#define FM_FILE_ALREADY_CREATED                 -12
#define FM_FILE_SYSTEM_FULL                     -13
#define FM_OUT_OF_FDS                           -14
#define FM_NO_SUPPORT_FOR_NVRAM                 -15
#define FM_FILE_DOES_NOT_EXIST                  -16
#define FM_FILE_NOT_OPEN                        -17
#define FM_FILE_NOT_OPEN_FOR_READ               -18
#define FM_FILE_NOT_OPEN_FOR_WRITE              -19
#define FM_ILLEGAL_INDEX                        -20
#define FM_ILLEGAL_SLOT                         -21
#define FM_COULD_NOT_OPEN_DST_FILE              -22
#define FM_CB_FILE_WR_FAIL                      -23
#define FM_CB_FILE_RD_FAIL                      -24
#define FM_CB_FILE_OPEN_FAIL                    -25
#define FM_ANMT_PLAY_ON_GOING                   -26
#define FM_ANMT_IS_DELETING                     -27
#define FM_MRPC_TIMEOUT                         -28
#define FM_ILLEGAL_TONE_PLAN_ID                 -29
#define FM_TONE_PLAN_NONEXIST                   -30
#define FM_STANDBY_SCC_IN_BOOT                  -31
#define FM_RM_FAILS_ON_STANDBY_SCC              -32
#define FM_RM_FAILS_ON_ACTIVE_SCC               -33
#define FM_TONE_DEF_NOT_FOUND                   -34
#define FM_ILLEGAL_TONE_TYPE_ID                 -35
#define FM_TONE_PLAN_FILE_OPEN_FAIL             -36
#define FM_TONE_PLAN_FILE_RD_FAIL               -37
#define FM_TONE_PLAN_FILE_WR_FAIL               -38
#define FM_TONE_DEF_TYPE_NOT_FOUND              -39

#ifdef FM_CB_FILE 
/* FM Control block file names */
#define FM_FILE_NAME              "C:/FM_CB" /* Name of the root file */
#define FM_SAVE_FILE_NAME         "C:/FM_CB.SAV" /* Name of Saved root file */
#define FM_SUSPECT_FILE_NAME      "C:/FM_CB.SUS" /* Name of Suspect root file*/
#endif /* FM_CB_FILE  */

#define HD_IMAGE_FILE_TABLE       "C:/HD_IMAGE_FILE_TB" /* name of the 
                                                          HD Image file table */  
#define NEW_HD_IMAGE_FILE_TABLE       "C:/HD_IMAGE_FILE_TB.new" /* name of the 
                                                          new HD Image file table */  
#define HD_ANMT_FILE_TABLE       "C:/hdAnmtFileTb" /* name of the 
                                                          HD Image file table */  
#define NEW_HD_ANMT_FILE_TABLE       "C:/hdAnmtFileTb.new" /* name of the 
                                                          new HD ANMT file table */  
#define IMAGE_FILE_NAME_PREFIX  "C:/IMAGE/" 
#define ANMT_FILE_NAME_PREFIX  "C:/IMAGE/Announce" 
#define UNUSED_ENTRY   "XXXXX"
#define MISC_FILE_DIR           "C:/IMAGE/misc"

#define TONE_PLAN_FILE_NAME_SUFFIX        ".tone"
#define TONE_PLAN_ERROR_FILE_NAME_SUFFIX  ".out"
#define TONE_PLAN_FILE_NAME_SUFFIX_LENGTH 5
#define TONE_PLAN_ERROR_FILE_NAME_SUFFIX_LENGTH  4
/* 
 * following are to be used as file types only 
 */
typedef enum fileType
{
    RAM  = 1, 
    NVRAM,
    HD,
    FLASH
} FILE_TYPE_t;

/* 
 * following are to be used as file Flags only 
 */
#define READ_ONLY       O_RDONLY
#define WRITE_ONLY      O_WRONLY
#define READ_WRITE      O_RDWR
#define CREATE          O_CREAT

/* 
 * following structure encapsulates the header information for FM task 
 * messaging. For remote services to use, one has to fill in the following
 * structure. 
 */
 /* MEANING OF THE ELEMENTS OF THE FOLLOWING STRUCTURE:
  * msg    -  This is request Message. Currently use service number as a message
  *           e.g. FM_WRITE_SERVICE, FM_OPEN_SERVICE, etc..
  * srcSlot-  Card number (or Slot Number) of the client card.
  * modNum -  Module number of the Client (Not Server). i.e. your application's
  *           module number. This is module who will receive the acknowldgement
  *           back from the remote FM task
  * servNum-  This is the service number for Client (not server). This is the
  *           service which will be invoked in order to report the results back
  *           to client from the remote FM Task
  * err    -  If an error occured on the remote side, the FM Task will report
  *           error number here (see the definitions of all Errors above)
  * arg1 - arg5 - These are the 5 IN arguments which are packaged by the client
  *           before sending the request to remote FM task. Please note that
  *           all 5 arguments do not need to be set always. The arguments should
  *           be set based upon the need. For example, if FmRemoteOpen() service
  *           is being used, only two arguments needs to be filled in. Others
  *           are not used. 
  */
typedef struct fmhdr
{
    S32_t    msg;          /* Currently use Service number */ 
    S32_t    srcSlot;
    S32_t    srcModNum;	   /* Src Mod num; send reply msg here */
    S32_t    srcServNum;   /* Src service num; send reply msg using this srv */
    S32_t    err;          /* report any errors here */
    U32_t    arg1;
    U32_t    arg2;
    U32_t    arg3;
    U32_t    arg4;
    U32_t    arg5;
} FM_HDR_t;

/*
 * In addition to above defined FM Services types as messages, additional
 * messages are defined as follows:
 */
#define FM_ACK        10
#define FM_NACK       11
#define FM_RESP       12



/*
 * following structure used by FM for its internal house keeping
 */

typedef struct fd
{
    NODE     node;
    U32_t    fdSignature; /* Signatures to validate FD;Don't trust if not mtch*/
    U32_t    fStart;   /* Start Address of file */
    S32_t    fMode;    /* Open for READ, WRITE, or Both */
    SIW_SEMID_t   fSemId;   /* Semaphore to prvent multiple writes */
    BOOL_t   semStatus; /* TRUE if sem was taken FALSE if sem was not taken */
    S32_t    fType;    /* What type of file, i.e. RAM, NVRAM, HD, FLASH*/
    S32_t    fLen;     /* Max. Length of the file */
    U32_t    fNextWr;    /* Next location where to write (append)  */
    S32_t    vxWFd;    /* This is VxWorks FD */
    S32_t    fStatus;  /* open/close count */
    LIST     tlist;    /* list of the tasks using this FD */
    char     fName[MAX_FILE_NAME_LENGTH];
} FD_t;


/* Following is File Table Structure */
typedef struct ft 
{
#ifdef FM_CB_FILE 
    BOOL_t   repl;
#endif /* FM_CB_FILE  */
    FD_t     *fdp;
    char     fName[MAX_FILE_NAME_LENGTH];
} FT_t;

/* Image file table structure */
typedef struct imageFt
{
    NODE     node;
    char     fName[MAX_FILE_NAME_LENGTH];
    U32_t    fCkSum;
    U32_t    idx;
} IMAGE_FT_t;

/* ANMT file table structure */
typedef struct anmtFt
{
    NODE     node;
    U32_t    idx;
    char     fName[MAX_FILE_NAME_LENGTH];
    char     fRealName[20];
    U32_t    fId;
    U32_t    fLength;
    U32_t    duration;                /*  duration of play of an Anmt file */ 
    U32_t    format;                  /*  encoding format */
    U32_t    flag;                    /*  activated or deactivated */
} ANMT_FT_t;

typedef struct anmtFileRec {
    char     fName[MAX_FILE_NAME_LENGTH];
    char     fRealName[20];
    U32_t    fId;
    U32_t    fLength;
    U32_t    duration;  
    U32_t    format;                  
    U32_t    flag; 
} ANMT_FILE_REC_t;
/*
 *====================================================================
 *=                         General Pupose Macros                    =
 *====================================================================
 */
/* 
 * file Descriptor Signatures; This is to validate an FD. If the
 * signature do not match, do not trust the FD. It may have been corrupted
 */
#define FD_SIGNATURE  0x900DBABE


#define VALID_FDP(f)    \
    ((((f) != NULL) && ((f)->fdSignature == FD_SIGNATURE)) ? TRUE : FALSE)

#define VALID_INDEX(i)    \
    ((((i) < 0) || ((i) > MAX_FILES )) ? FALSE : TRUE)

/*
 *====================================================================
 *=                         external variables                       =
 *====================================================================
 */


/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */
extern  S32_t FmReadLine(IN FD_t *fdp, OUT char *buf, S32_t offset, S32_t maxBytes);

extern BOOL_t fmIsTonePlanFileExist(U8_t toneId);
extern BOOL_t fmIsTonePlanIdExist(U8_t toneId);
extern RET_t FmDispTonePlanDef(U8_t *toneFilename);
extern RET_t fmExtractVersionCountryTonePlanId(U8_t *toneFileName, OUT U8_t *version, OUT U8_t *country, OUT U8_t *tonePlanId);
 




/****************************************************************************
 * Function:    FmCreate()
 * Descripion:  This routine is used to create a new file in any of the
 *              media such as RAM, Hard Drive, Non-Volatile RAM, and Flash.
 *              All of the data in AXSS-1600 is stored in the form a file 
 *              which can be written to or read from using the routines  
 *              provided in this interface file. A file must be created before
 *              it can be opend and read/written to/from. When a file is 
 *              created, it must specifyt he type of the file.
 * In Parms:    char *fileName     - Name of the file to be created.  
 *              U32_t        flag  - What is this file being opened for.
 *                                   READ_ONLY, WRITE_ONLY, READ_WRITE
 *              FILE_TYPE_t fileType - type of file, i.e.
 *                                   RAM, HD, NVRAM, FLASH
 *              S32_t       fileSize     - Max. Anticipated size of this file.
 *              BOOL_T      replFlag     - TRUE(1) means replicate on Standby 
 * Out Parms:   None 
 * Return Values:
 *              fileDescriptorIndex     - If all works out OK
 *              FM_ERROR
 *              FM_OUT_OF_FDS
 *              FM_OUT_OF_ROOM
 */
extern 
S32_t FmCreate(IN char         *fileName, 
	          U32_t        flag, 
	          FILE_TYPE_t  fileType, 
#ifndef FM_CB_FILE
	          S32_t        fileSize);
#else /* FM_CB_FILE */
	          S32_t        fileSize,
	          BOOL_t       replFlag);
#endif /* FM_CB_FILE */


/****************************************************************************
 * Function:    FmDelete()
 * Descripion:  This routine is used to delete a file in any of the
 *              media such as RAM, Hard Drive, Non-Volatile RAM, and Flash.
 * In Parms:    char *fileName     - Name of the file to be deleted.  
 * Out Parms:   None 
 * Return Values:
 *              FM_OK               - If all works out OK
 *              FM_ERROR
 *              FM_FILE_DOES_NOT_EXIST
 */
extern
RET_t FmDelete(IN char *fileName);


/****************************************************************************
 * Function:    FmOpen()
 * Descripion:  This routine is used to open a file which has already been
 *              created by using FmCreate(). If not done so, this routine
 *              will return an Error. Every file must be opened before it can
 *              read from or written to and every file must be Created before
 *              it can be opened.
 * In Parms:    char *fileName     - Name of the file to be opened.  
 *              U32_t        flag  - What is this file being opened for.
 *                                   READ_ONLY, WRITE_ONLY, READ_WRITE
 * Out Parms:   None
 * Return Values:
 *              fileDescriptorPointer     - If file opened successfully
 *              FM_ERROR
 *              FM_FILE_DOES_NOT_EXIST
 */
extern 
FD_t *FmOpen(IN char *fileName, U32_t flag);

extern RET_t fmUpdateHDImageFileTable(char* tbName, U32_t idx, char *fName,  U32_t checksum);
extern RET_t fmUpdateAnmtFileTable(ANMT_FILE_REC_t *);
/****************************************************************************
 * Function:    FmClose()
 * Descripion:  This routine is used to close a file which has already been
 *              opened by using FmOpen(). This routine requires the file
 *              descriptor as an input paramter (the one which was assigned by
 *              the FmOpen() routine.
 * In Parms:    FD_t *fdp           - The File Descriptor pointer
 * Out Parms:   None
 * Return Values:
 *              FM_OK               - If all works out OK
 *              FM_ERROR
 *              FM_FILE_NOT_CREATED
 */
extern 
S32_t FmClose(IN FD_t *fdp);


/****************************************************************************
 * Function:    FmRead()
 * Descripion:  This routine is used to read a previously opened file. This 
 *              routine will allow the reading of file from any offset within
 *               the file. A multiple applications can read the same file
 *              simultaneously. Also, a file can be read while it is being
 *              written to. However, two applications can not write to the 
 *              same file simultaneously.
 * In Parms:    FD_t *fdp           - The File Descriptor pointer
 *              char *buf          - pointer to buffer where to read data into
 *              S32_t offset       - The offset from the begining of the file
 *              S32_t maxBytes     - Max. number of bytes to read
 * Out Parms:   None
 * Return Values:
 *              #of bytes read     - Number of actual bytes read
 *              FM_ERROR
 */
extern 
S32_t FmRead(IN FD_t *fdp, char *buf, S32_t offset, S32_t maxBytes);


/****************************************************************************
 * Function:    FmWrite()
 * Descripion:  This routine is used to Write into a previously opened file.
 *              This routine allows writing into a file at any location. 
 *		Same file can not be written to by two different applications.
 * In Parms:    FD_t *fdp           - The File Descriptor pointer
 *              char *buf          - pointer to buffer where the data is
 *              S32_t offset       - The offset from the begining of the file
 *              S32_t maxBytes     - Max. number of bytes to read
 *              BOOL_t append      - if TRUE; append to file; else write at
 *                                   location pointed by file start + offset
 * Out Parms:   None
 * Return Values:
 *              #of bytes written  - if not equal to maxBytes, an error occured
 *              FM_ERROR
 */
extern 
S32_t FmWrite(IN FD_t *fdp, 
		 char *buf, 
		 S32_t offset, 
		 S32_t maxBytes, 
		 BOOL_t append);

/****************************************************************************
 * Function:    FmRename()
 * Descripion:  This routine is used to Rename a file which already exists. 
 * In Parms:    char *file1        - Name of Src File
 *              char *file2        - Name of destination file
 * Out Parms:   None
 * Return Values:
 *              FM_OK 
 *              FM_ERROR
 */
extern 
S32_t FmRename(IN char *file1, 
		  char *file2);

/****************************************************************************
 * Function:    FmIndex2Ptr()
 * Descripion:  This routine takes the index tro the FileTable and returns
 *              the corresponding pointer to RAM file.
 *              NOTE: This should be used ONLY for RAM Files.
 *              NOTE2: The input parameter for this routine is the index which
 *                     is returned by the FmCreate() routine. Therefore, the
 *                     use of this routine should be as follows:
 *                     index = FmCreate(.......) - only for RAM files
 *                     ptr = FmIndex2Ptr(index)  - only for RAM files.
 *
 * In Parms:    S32_t index        - File Table Index
 * Out Parms:   None 
 * Return Values:
 *              pointer to file     - If all works out OK
 *              NULL                - if error 
 */
extern char *FmIndex2Ptr(S32_t index);


/****************************************************************************
 * Function:    FmAnnouncementFileId2Ptr()
 * Descripion:  This API is intended only for Service cards. If run on SCC, 
 *              will return NULL. This will check if for a given ID of 
 *              Announcement file, file exixts or not. If exists, it will  
 *              return memory pointer to this file. If file does not exists,
 *              it will return NULL
 * In Parms:    S32_t fileId  - from 0 to 31 (MAX_ANNOUNCEMENT_FILES)
 * Out Parms:   None 
 * Return Values:
 *              NULL  - File Does not exists; or wrong Id spefied
 *              addr  - Address of the file
 */
char *
FmAnnouncementFileId2Ptr(S32_t id);

extern RET_t rmDir (char	*dirName);

/****************************************************************************
 * Function:    fmGetImageFileCheckSum()
 * Descripion:  This routine is used to get the checksum of the image file.
 *
 * In Parms:    char *      a Image File name 
 * Out Parms:   U32_t       checksum 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
extern 
U32_t fmGetImageFileCheckSum(char *imageFileName, U32_t *checkSum);

/****************************************************************************
 * Function:    fmUpdateImageFileTable()
 * Descripion:  This routine  is used to register an entry to the image file
 *              table or compare/update the check sum if the file has been created. 
 *
 * In Parms:    IMAGE_FT_t      a image File Entry 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_EQUAL_CHKSUM 
 *              FM_ERROR
 *              FM_ILLEGAL_FILE_NAME
 */
extern
RET_t fmUpdateImageFileTable( char *newImageFileName, U32_t newCheckSum );

/****************************************************************************
 * Function:    fmGetImageFileIndex()
 * Descripion:  This routine is used to get the table index of the image file.
 *
 * In Parms:    char *      a Image File name 
 * Out Parms:   none 
 * Return Values:
 *              table index 
 *              FM_ERROR
 */
extern 
RET_t fmGetImageFileIndex(char *imageFileName );

/***********************************************************************
 * Function: fmCalculateChecksum() 
 * Description: Calculates the checksum of a file.
 * In parms: char *file
 *           int *checksum
 * Out parms: None
 * Return values: FM_OK
 *                FM_ERROR
 */
extern
RET_t fmCalculateChecksum(U8_t *file, U32_t* checksum);

/****************************************************************************
 * Function:    fmGetImageFileCheckSum()
 * Descripion:  This routine is used to get the checksum of the image file.
 *
 * In Parms:    char *      a Image File name 
 * Out Parms:   U32_t       checksum 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
extern
U32_t fmGetImageFileCheckSum(char *imageFileName, U32_t *checkSum);

/****************************************************************************
 * Function:    FmGetAnmtFileEntryByName()
 * Descripion:  This routine is used to get the anmt file entry.
 *
 * In Parms:    char *      a Anmt File name 
 * Out Parms:   none 
 * Return Values:
 *              table index 
 *              FM_ERROR
 */
extern
RET_t FmGetAnmtFileEntryByName(char *anmtFileName, ANMT_FILE_REC_t *anmtFileEntry );

/****************************************************************************
 * Function:    FmGetAnmtFileEntry()
 * Descripion:  This routine is used to get the table index of the anmt file.
 *
 * In Parms:    char *      a ANMT file ID 
 * Out Parms:   none 
 * Return Values:
 *              table index 
 *              FM_ERROR
 */
extern
RET_t FmGetAnmtFileEntry(U32_t id, ANMT_FILE_REC_t *anmtFileEntry );

/****************************************************************************
 * Function:    FmUpdateRemAnmtFileTable()
 * Descripion:  This routine updates an entry on Remote Anmt file table on
 *              a target SCC card.
 * In Parms:    S32_t        target; 
 *              ANMT_FILE_REC_t *anmtFileEntry;
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
extern
RET_t FmUpdateRemAnmtFileTable( IN S32_t target, ANMT_FILE_REC_t *anmtFileEntry);

/****************************************************************************
 * Function:    FmIsAnmtFileExist()
 * Descripion:  This routine is used to get the anmt file entry.
 *
 * In Parms:    char *      a Anmt File name 
 * Out Parms:   none 
 * Return Values:
 *              table index 
 *              FM_ERROR
 */
extern
RET_t FmIsAnmtFileExist(char *anmtRealName);

/****************************************************************************
 * Function:    FmIsAnmtFileActive()
 * Descripion:  This routine is used to get the anmt file entry. It is used
 *               by IPDC protocol.
 *
 * In Parms:    char *      a Anmt File name 
 * Out Parms:   none 
 * Return Values:
 *               Fm_OK 
 *              FM_ERROR
 */
extern
RET_t FmIsAnmtFileActive(S32_t id);

/****************************************************************************
 * Function:    FmIsMgcpAnmtFileActive()
 * Descripion:  This routine is used to get the table index based on the  
 *              real anmt file. It is used by MGCP protocol.
 *
 * In Parms:    char *      a Anmt File real name 
 * Out Parms:   none 
 * Return Values:
 *              table index 
 *              FM_ERROR
 */
extern
RET_t FmIsMgcpAnmtFileActive(char *anmtRealName );

/****************************************************************************
* Function:    FmGetTotalAnmtPlayDuration()
* Descripion:  This API is to return the total play duration of all downloaded
*              anmt files.
*
* In Parms:    None
* Return Values:
*              U32_t  totalDuration
*/
extern
U32_t FmGetTotalAnmtPlayDuration();

/****************************************************************************
 *
 * Function:    fmRebuildImageFileTable()
 * Descripion:  This routine takes the Image File Table record from HD Image
 *              File Table and rebuild the RAM Image File Table.
 *
 * In Parms:    S32_t      idx        - The Index of File Entry 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
extern
RET_t fmRebuildImageFileTable();

/****************************************************************************
 *
 * Function:    fmRebuildAnmtFileTable()
 * Descripion:  This routine takes the Anmt File Table record from HD ANMT
 *              File Table and rebuild the RAM Anmt File Table.
 *
 * In Parms:    S32_t      idx        - The Index of File Entry 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
extern
RET_t fmRebuildAnmtFileTable();

/****************************************************************************
 * following routines are made avaialbe for the remote access of the
 * File Manager facilitities.
 * NOTE:
 *    If an application wants to access files on a local card, the APIs defined
 *    above should be used. On the other hand, if the application needs to 
 *    access files on the remote card, it should use the APIs defined below.
 */
extern RET_t FmConvertOutfileNameTo0Base(char *from1BaseFilename, char
*to0BaseFilename);
extern S32_t FmRemCreate(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t FmRemDelete(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t FmRemOpen  (IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t FmRemClose (IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t FmRemRead  (IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t FmRemWrite (IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t FmRemRename(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t FmRemGetImageFileChecksum(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t FmRemSetAnmtFileFlag(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t FmRemUpdateAnmtFileTable(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern RET_t FmRemUpdateToneFileTable(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t fmSetRemToneTimeOut(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern RET_t FmCreateRemFile(S32_t, char *, U32_t, FILE_TYPE_t, S32_t);
extern RET_t FmRenameRemFile(S32_t, char *, char *, BOOL_t);
extern RET_t FmOpenRemFile(S32_t targetCard, char *dstName,U32_t f, FD_t **fdp);

extern RET_t FmGetRemImageFileCheckSum(IN S32_t target, char *fName, U32_t *checksum);
extern RET_t FmSetRemAnmtFileFlag(IN S32_t target, char *fName, U32_t flag);

extern RET_t openRemFile(S32_t targetCard, char *dstName, FD_t **fdp);
extern RET_t closeBothFiles(S32_t targetCard, FD_t *srcFdp, FD_t
*destFdp);

extern RET_t Copy(U8_t* srcFile, U8_t* dstFile);

extern FT_t   fileTable[MAX_FILES];
/*
extern RET_t FmUpdateTonePlanFileTable(U8_t *tonePlanFileName, U8_t tonePlanId);
extern void FmMoveAllTonePlans2Target(S32_t slot);
extern RET_t FmGetToneDef(U8_t tonePlanId, U8_t toneType,  pPT_TONE_t
ptToneDef);
extern RET_t fmBuildSysTonePlanFile(U8_t *tonePlanFileName, U8_t *destFileName);
extern RET_t fmGetTonePlanId(U8_t *toneFileName, OUT U8_t *tonePlanId);
extern STATUS FmTonePlanOutfileAction (UINT16 outfileIndex, char* outfileName);
extern FmUpdateRemToneFileTable( IN S32_t target, U8_t * tonePlanFileName, 
  U8_t tonePlanId);
*/
#endif /* FM_IF_H */
