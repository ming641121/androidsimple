/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file is a local header file for Image Downloader
 *
 * Author(s):
 *
 *    
 */


#ifndef ID_H
#define ID_H

/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */

#ifdef  TRUE
#undef  TRUE
#define TRUE    1
#endif

#ifdef  FALSE
#undef  FALSE
#define FALSE   0
#endif

#ifdef  NULL
#undef  NULL
#define NULL   0
#endif

#define MAX_ID_IN_MESSAGES      32

#define DB_HD_FILE_NAME                          "DbHd"
/* these are fake file names. Used as flags in our legancy code. */ 
#define DB_BSC_RAM_AND_HD_FILE_NAME             "DbBscRamAndHd"
#define DB_NSC_DOWNLOAD_PRI_HD_RAM_FILE_NAME    "DbNscDownloadPriHdRam"
#define DB_NSC_DOWNLOAD_SEC_HD_RAM_FILE_NAME    "DbNscDownloadSecHdRam"
#define DB_NSC_REBUILD_RAM_HD_FILE_NAME         "DbNscRebuildHdRam"
#define DB_UPGD_RAM_HD_FILE_NAME                "DbUpgdHdRam"
#define DB_SYNCBACK_RAM_HD_FILE_NAME            "DbNscSyncBackHdRam"
#define DB_NSC_DNLD_SYNC_HD_FILE_NAME           "DbNscDnldSyncHd"

/*
 *====================================================================
 *=                             Typedefs                           =
 *====================================================================
 */

typedef struct idhdr
{
    S32_t         msg;        
    S32_t         srcSlot;
    S32_t         destSlot;
    SIW_MQID_t    srcMsgQId;
    SIW_TID_t     srcTid;
    S32_t         err;
} ID_HDR_t;

typedef struct fr
{
    S32_t totalBlocks;
    S32_t blocksProcessed;
    S32_t blocksRemaining;
    S32_t thisBlock;
    S32_t thisBlockLen;
    MSG_t *replyBuffer;
    FD_t  *srcFdp;
    FD_t  *dstFdp;
    U32_t nextAddrToRead;
    U32_t nextAddrToWrite;
}FR_t;

/*
 *====================================================================
 *=                             Macros                           =
 *====================================================================
 */

#define VALID_MEM_TYPE(t)  \
	((((t) == RAM_MEM) || ((t) == FLASH_MEM)) ? TRUE : FALSE)
#define SECONDS(s) (sysClkRateGet() * (s))

#define ID_HDR_PTR(i) ((ID_HDR_t *)(&(i)->data[0]))

/*
 *====================================================================
 *=                             Externs                           =
 *====================================================================
 */
#if 0 /* System Services creates the ID now */
extern SIW_MQID_t    idMsgQId;
#endif  /* System Services creates the ID now */
RET_t IdMain(int a0, int a1, int a2, int a3, int a4, 
	 int a5, int a6, int a7, int a8, int a9);
extern FR_t flashRec[MAX_SLOTS];

#endif  /* ID_H */
