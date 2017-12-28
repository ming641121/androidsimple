/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file contains the main MRPC Task (i.e. MrpcMain()) and a lot
 *    supporting functions.
 *
 * Author(s):
 *
 *    
 */


/*
 *====================================================================
 *=                            include                               =
 *====================================================================
 */
#include "axss_types.h"
#include "axss_if.h"
#include "sys_if.h"
#include "siw_if.h"
#include "taskLib.h"
#include "msgQLib.h"
#include "sysLib.h"
#if 1  /* SSSSSS */
#include "ioLib.h"
#endif   /* SSSSSS */
#include "string.h" /* bzero(), etc */
#include "stdio.h"  /* for printf */
#include "lstLib.h"  /* for linked list */
#include "mrpc_if.h"
#include "sm_if.h"
#include "cm_if.h"
#include "fm_if.h"
#include "nwg.h"
#include "fm.h"
#include "fm_events.h"


/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */

#if 0 /* System Services creates the ID now */
SIW_MQID_t    fmMsgQId;
#endif  /* System Services creates the ID now */
FLASH_MEM_t   flashMemory;
SIW_SEMID_t   fmHDImageTbSem;
SIW_SEMID_t   fmHDAnmtTbSem;
SIW_SEMID_t   fmCrmSyncSem;

/*
 *====================================================================
 *=                         external variables                       =
 *====================================================================
 */

extern FT_t   fileTable[MAX_FILES];
extern LIST   freeFdList;
extern LIST   imageFtList;
extern LIST   anmtFtList;
extern SIW_SEMID_t imageFileTableSem;
extern SIW_SEMID_t anmtFileTableSem;
extern SIW_SEMID_t tonePlanFileTableSem;
extern SIW_SEMID_t nscAnmtTableSem;
extern SIW_SEMID_t tonePlanSem;
extern S32_t  anmtMibIndex[MAX_ANNOUNCEMENT_FILES];
extern S32_t  nscAnmtTable[MAX_ANNOUNCEMENT_FILES]; 


#ifndef SIM
extern STATUS_t RemUpdateFlash (IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern STATUS_t RemEraseFlash (IN MSG_t *inMsg, OUT MSG_t **outMsg);
#ifdef SCC
extern STATUS_t RemDbRestore (IN MSG_t *inMsg, OUT MSG_t **outMsg);
#ifndef FLASH_BOOT
extern STATUS_t RemUpdateKey (IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern STATUS_t RemUpdatePassword (IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern STATUS_t RemUpdateRoute (IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern STATUS_t RemAddRoute (IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern STATUS_t RemDelRoute (IN MSG_t *inMsg, OUT MSG_t **outMsg);
#endif
#endif
#endif

/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */

#if DBG  /* SSSSSS */

/* This routine needs to be provided by the Flash Driver */
RET_t 
FlashWrite(char *src, char *dest, S32_t len) /* SSSS */
{
    bcopy(src, dest, len);
    return (0);
}

void startFm()
{
    U32_t stat;

    /* This routine is to spwan the MRPC task */
    stat =  
       taskSpawn("fmTask", 100, 0, 4096, (FUNCPTR)FmMain, 0,0,0,0,0,0,0,0,0,0);
    printf (" FM Task io spawned status = %d\n ", stat);
}


void testFm()
{
    RET_t ret;
    FD_t *srcFdp, *destFdp;
    char buf[1000];
    MSG_t *inMsg,*outMsg;
    FD_t *o, *t, *th, *f;

    o = FmOpen("C:card_6.cfg", 0);
    t = FmOpen("C:card_6.cfg", 0);
    th = FmOpen("C:card_6.cfg", 0);
    f = FmOpen("C:card_6.cfg", 0);
    ret = FmClose(o);
    ret = FmClose(th);
    o = FmOpen("C:card_6.cfg", 0);
    th = FmOpen("C:card_6.cfg", 0);
    ret = FmClose(t);
    ret = FmDelete("C:card_6.cfg");

    inMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    ret = FmRemCreate(inMsg, &outMsg);
    ret = FmRemOpen(inMsg, &outMsg);
    ret = FmRemWrite(inMsg, &outMsg);
    ret = FmRemRead(inMsg, &outMsg);
    ret = FmRemDelete(inMsg, &outMsg);

    ret = FmCreate("/test/test2", READ_WRITE, RAM, 12345);
    ret = FmCreate("/test/xyZz", READ_ONLY, RAM, 0);
    destFdp = FmOpen("/test/test2", READ_WRITE);
    srcFdp = FmOpen("C:Makefile", READ_WRITE);
    ret = FmRead(srcFdp, buf, 0, 1000);
    printf ("testFm(): Read %d bytes from %s \n", ret, "C:Makefile");
    ret = FmWrite(destFdp, buf, 0, 1000, 0);
    ret = FmWrite(destFdp, buf, 0, 1000, 1);
    printf ("testFm(): Wrote %d bytes to %s \n", ret, "/test/test2");
    ret = FmRead(destFdp, buf, 0, 1000);
    printf ("testFm(): Read %d bytes from %s \n", ret, "/test/test2");
    ret = FmClose(srcFdp);
    ret = FmClose(destFdp);
    ret = FmDelete("/test/test2");
    ret = FmDelete("C:Makefile");
    ret = FmDelete("/test/test1");
}

void bufFill (char *b, U32_t startData, U32_t len)
{
    U32_t  i;

    for (i=0; i < len; i++) {
	*b++ = startData++;
	if (startData > 0x7F)
	    startData = 0x0;
    }
}

S32_t bufCmp (char *b1, char *b2, U32_t len)
{
    U32_t  i;

    for (i=0; i < len; i++) {
	if (*b1++ != *b2++)
	    return(i);
    }
    return 0;
}

#ifndef FLASH_BOOT
void mkCfgFile(char *name, S32_t size, U32_t slot, U32_t cardType)
{
    S32_t ds;
    char *bp, *bp2;
    CFG_FILE_HDR_t *pH, *pH2;
    FD_t *fdp;
    RET_t ret;

    ds = size-sizeof(CFG_FILE_HDR_t);
    bp = malloc(size);
    bp2 = malloc(size);
    pH = (CFG_FILE_HDR_t *)bp;
    pH->len = ds;
    pH->slot= slot;
    pH->cardType= cardType;
    pH->ckSum = 0;
    bufFill(bp+sizeof(CFG_FILE_HDR_t), 0x21, ds); 
    pH->ckSum= checkSum((U32_t *)bp, size);
    fdp = FmOpen(name, READ_WRITE | CREATE);
    printf("Opened file %s of size %d\n", name, size);
    ret = FmWrite(fdp, bp, 0, size,0);
    printf("wrote %d bytes out of %d into file %s \n", ret, size, name);
    ret = FmClose(fdp);
    printf("Closed file %s \n", name);
    fdp = FmOpen(name, READ_WRITE);
    printf("Opened file %s of size %d\n", name, size);
    ret = FmRead(fdp, bp2, 0, size);
    printf("read %d bytes out of %d into file %s \n", ret, size, name);
    pH2 = (CFG_FILE_HDR_t *)bp2;
    printf("HeaderInfo: pH2->len(%d),cardType(%d),slot(%d), pH2->ckSum(%x)\n",
	      pH2->len, pH2->cardType, pH2->slot, pH2->ckSum);
    printf("checksum of file is %x\n", checkSum((U32_t*)bp2, size));
    ret = bufCmp(bp, bp2, size);
    if (ret)
	printf("File did not get build right diff @(%d)\n", ret);
    else
	printf("CFG File %s is ready to go \n",name);
    ret = FmClose(fdp);
    printf("Closed file %s \n", name);
}

void mkImageFileByName(char *name, S32_t size)
{
    S32_t ds;
    char *bp, *bp2;
    IMAGE_FILE_HDR_t *pH, *pH2;
    FD_t *fdp;
    RET_t ret;

    ds = size-sizeof(IMAGE_FILE_HDR_t);
    bp = malloc(size);
    bp2 = malloc(size);
    pH = (IMAGE_FILE_HDR_t *)bp;
    pH->len = ds;
    pH->slot= 0;
    bufFill(bp+sizeof(IMAGE_FILE_HDR_t), 0x21, ds); 
    pH->ckSum= checkSum((U32_t *)bp, size);
    fdp = FmOpen(name, READ_WRITE | CREATE);
    printf("Opened file %s of size %d\n", name, size);
    ret = FmWrite(fdp, bp, 0, size,0);
    printf("wrote %d bytes out of %d into file %s \n", ret, size, name);
    ret = FmClose(fdp);
    printf("Closed file %s \n", name);
    fdp = FmOpen(name, READ_WRITE);
    printf("Opened file %s of size %d\n", name, size);
    ret = FmRead(fdp, bp2, 0, size);
    printf("read %d bytes out of %d into file %s \n", ret, size, name);
    pH2 = (IMAGE_FILE_HDR_t *)bp2;
    printf("HeaderInfo: pH2->len(%d), pH2->ckSum(%x)\n", pH2->len, pH2->ckSum);
    printf("checksum of file is %x\n", checkSum((U32_t*)bp2, size));
    ret = bufCmp(bp, bp2, size);
    if (ret)
	printf("File did not get build right diff @(%d)\n", ret);
    else
	printf("Image File %s is ready to go \n",name);
    ret = FmClose(fdp);
    printf("Closed file %s \n", name);
}

void mkImageFile(char* name, S32_t size)
{
    S32_t ds;
    char *bp, *bp2;
    IMAGE_FILE_HDR_t *pH, *pH2;
    FD_t *fdp;
    RET_t ret;

    ds = size-sizeof(IMAGE_FILE_HDR_t);
    bp = malloc(size);
    bp2 = malloc(size);
    pH = (IMAGE_FILE_HDR_t *)bp;
    pH->len = ds;
    pH->slot= 0;
    bufFill(bp+sizeof(IMAGE_FILE_HDR_t), 0x21, ds); 
    pH->ckSum= checkSum((U32_t *)bp, size);
    fdp = FmOpen(name, READ_WRITE | CREATE);
    printf("Opened file %s of size %d\n", name, size);
    ret = FmWrite(fdp, bp, 0, size,0);
    printf("wrote %d bytes out of %d into file %s \n", ret, size, name);
    ret = FmClose(fdp);
    printf("Closed file %s \n", name);
    fdp = FmOpen(name, READ_WRITE);
    printf("Opened file %s of size %d\n", name, size);
    ret = FmRead(fdp, bp2, 0, size);
    printf("read %d bytes out of %d into file %s \n", ret, size, name);
    pH2 = (IMAGE_FILE_HDR_t *)bp2;
    printf("HeaderInfo: pH2->len(%d), pH2->ckSum(%x)\n", pH2->len, pH2->ckSum);
    printf("checksum of file is %x\n", checkSum((U32_t*)bp2, size));
    ret = bufCmp(bp, bp2, size);
    if (ret)
	printf("File did not get build right diff @(%d)\n", ret);
    else
	printf("Image File %s is ready to go \n",name);
    ret = FmClose(fdp);
    printf("Closed file %s \n", name);
}
#endif /* FLASH_BOOT */
#endif   /* SSSSSS */




/****************************************************************************
 * Function:    FmDispFt()
 * Descripion:  This function prints out the File Table
 * In  Parms:   None 
 * Out Parms:   None 
 * Return Values:
 *              None
 *            
 */
void
FmDispFt()
{
    S32_t i,j; 
    printf("\nFile Manager's File Table: \n\n");
    printf("Num   		File Name    		FDp   \n");
    printf("================================================= \n");

    for (i=0, j=0; i < MAX_FILES; i++ ) {
	if (strlen(fileTable[i].fName) != 0) {
	    j++;
	    printf ("%d\t\t %s\t\t\t  %p\n",
		     i,
		     fileTable[i].fName,
		     fileTable[i].fdp);
	}
    }
    if (!j) printf ("No entries found in FileTable\n\n");
}





/****************************************************************************
 * Function:    FmDispSubEntry()
 * Descripion:  This function prints out the Details of One file Entry
 * In  Parms:   None 
 * Out Parms:   None 
 * Return Values:
 *              None
 *            
 */
void
FmDispSubEntry(S32_t entry, FD_t *fdp, S32_t cnt)
{
    S32_t i, c;
    FD_TID_t *n;
    printf("\nFile descriptor's details: \n\n");
    printf("Num   		File Name    		FDp   \n");
    printf("================================================= \n");

    if (strlen(fileTable[entry].fName) != 0) {
	printf ("%d\t %s\t  %p - subEntry %d\n\n",
		 entry,
		 fileTable[entry].fName,
		 fdp, cnt);
	printf("fdSignature: %x\n", fdp->fdSignature);
	printf("fStatus    : %x\n", fdp->fStatus    );
	printf("fMode      : ");
	switch(fdp->fMode & ~CREATE) {
	case READ_ONLY:
	    printf("READ_ONLY");
	    break;
	case WRITE_ONLY:
	    printf("WRITE_ONLY");
	    break;
	case READ_WRITE:
	    printf("READ_WRITE");
	    break;
	default: 
	    printf("UNKNOWN");
	    break;
	}
	if (fdp->fMode & CREATE)
	    printf(" | CREATE ");
	printf("\n");
	printf("fType      : ");
	switch(fdp->fType) {
	case RAM:
	    printf("RAM");
	    break;
	case HD:
	    printf("HD");
	    break;
	case FLASH:
	    printf("FLASH");
	    break;
	default: 
	    printf("UNKNOWN");
	    break;
	}
	printf("\n");
	printf("fLen       : %x\n", fdp->fLen       );
	printf("fStart     : %x\n", fdp->fStart     );
	printf("fNextWr    : %x\n", fdp->fNextWr    );
	printf("fSemId     : %x\n", (U32_t)fdp->fSemId     );
	printf("semStatus  : %x\n", fdp->semStatus );
	printf("vxWFd      : %x\n", fdp->vxWFd      );
	/* print out the tasks which have opened on this FD */
	c = lstCount (&fdp->tlist);
	if (c > 0) {
	    printf("\n");
	    printf("Tasks which have Opened in this FD are:\n");
	    n = (FD_TID_t *)lstFirst(&fdp->tlist);
	    i=0;
	    do {
		printf("	Tid: %d       Mode: ", (S32_t)n->tid);
		switch(n->mode & ~CREATE) {
		case READ_ONLY:
		    printf("READ_ONLY");
		    break;
		case WRITE_ONLY:
		    printf("WRITE_ONLY");
		    break;
		case READ_WRITE:
		    printf("READ_WRITE");
		    break;
		default: 
		    printf("UNKNOWN");
		    break;
		}
		if (fdp->fMode & CREATE)
		    printf(" | CREATE ");
		printf("\n");
		n = (FD_TID_t *)lstNext((NODE *)n);
		i++;
	    } while (i < c);
	} else {
	    if (fdp->fType != HD)
		printf ("File is not Opened \n");
	}
    } else 
	printf ("Illegal entry %d \n\n", entry);
}


/****************************************************************************
 * Function:    FmDispOneEntry()
 * Descripion:  This function prints out the Details of One file Entry
 * In  Parms:   None 
 * Out Parms:   None 
 * Return Values:
 *              None
 *            
 */
void
FmDispOneEntry(S32_t entry)
{
    FD_t *fdp;
    S32_t i, c;
    FD_TID_t *n;
    printf("\nFile descriptor's details: \n\n");
    printf("Num   		File Name    		FDp   \n");
    printf("================================================= \n");

    if (strlen(fileTable[entry].fName) != 0) {
	fdp = fileTable[entry].fdp;
	printf ("%d\t %s\t  %p\n\n",
		 entry,
		 fileTable[entry].fName,
		 fileTable[entry].fdp);
	printf("fdSignature: %x\n", fdp->fdSignature);
	printf("fStatus    : %x\n", fdp->fStatus    );
	printf("fMode      : ");
	switch(fdp->fMode & ~CREATE) {
	case READ_ONLY:
	    printf("READ_ONLY");
	    break;
	case WRITE_ONLY:
	    printf("WRITE_ONLY");
	    break;
	case READ_WRITE:
	    printf("READ_WRITE");
	    break;
	default: 
	    printf("UNKNOWN");
	    break;
	}
	if (fdp->fMode & CREATE)
	    printf(" | CREATE ");
	printf("\n");
	printf("fType      : ");
	switch(fdp->fType) {
	case RAM:
	    printf("RAM");
	    break;
	case HD:
	    printf("HD");
	    break;
	case FLASH:
	    printf("FLASH");
	    break;
	default: 
	    printf("UNKNOWN");
	    break;
	}
	printf("\n");
	printf("fLen       : %x\n", fdp->fLen       );
	printf("fStart     : %x\n", fdp->fStart     );
	printf("fNextWr    : %x\n", fdp->fNextWr    );
	printf("fSemId     : %x\n", (U32_t)fdp->fSemId     );
	printf("semStatus  : %x\n", fdp->semStatus );
	printf("vxWFd      : %x\n", fdp->vxWFd      );
	/* print out the tasks which have opened on this FD */
	c = lstCount (&fdp->tlist);
	if (c > 0) {
	    printf("\n");
	    printf("Tasks which have Opened in this FD are:\n");
	    n = (FD_TID_t *)lstFirst(&fdp->tlist);
	    i=0;
	    do {
		printf("	Tid: %d       Mode: ", (S32_t)n->tid);
		switch(n->mode & ~CREATE) {
		case READ_ONLY:
		    printf("READ_ONLY");
		    break;
		case WRITE_ONLY:
		    printf("WRITE_ONLY");
		    break;
		case READ_WRITE:
		    printf("READ_WRITE");
		    break;
		default: 
		    printf("UNKNOWN");
		    break;
		}
		if (fdp->fMode & CREATE)
		    printf(" | CREATE ");
		printf("\n");
		n = (FD_TID_t *)lstNext((NODE *)n);
		i++;
	    } while (i < c);
	} else {
	    if (fdp->fType != HD)
		printf ("File is not Opened \n");
	}
	i=1;
	while (fdp->node.next) {
	    FmDispSubEntry(entry, (FD_t *)fdp->node.next, i);
	    fdp = (FD_t *)fdp->node.next;
	    i++;
	}
    } else 
	printf ("Illegal entry %d \n\n", entry);
}


/****************************************************************************
 * Function:    FmDispFtEntry()
 * Descripion:  This function prints out the File Table
 * In  Parms:   None 
 * Out Parms:   None 
 * Return Values:
 *              None
 *            
 */
void
FmDispFtEntry(S32_t entry)
{
    if (entry >= 0) {
	if (strlen(fileTable[entry].fName) != 0) {
	    FmDispOneEntry(entry);
	} else
	    printf ("Illegal entry %d \n\n", entry);
    } else {
	if (entry == -1) {
	    S32_t i;
	    for (i=0; i < MAX_FILES; i++) {
		if (strlen(fileTable[i].fName) != 0) {
		    FmDispOneEntry(i);
		}
	    }
	} else 
	    printf ("Illegal entry %d \n\n", entry);
    }
}





/****************************************************************************
 * Function:    initFlashRecords()
 * Descripion:  This routine initializes the internal structures for allocation 
 *              and management of Flash memory.
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
void
initFlashRecords()
{
    S32_t i;
    SIW_FUNC_TRACE(FM, initFlashRecords, 0, 0, 0, 0, 0);

#if DBG /* SSSSS For debugging only */
    flashMemory.nextAvail = 
		  (S32_t )SiwAllocMem(SIW_STATIC, 200000, WAIT_FOREVER);
    for (i=0; i < MAX_FLASH_FILES; i++) {
	/* Initialize all flash records and mark them unavailable */
	flashMemory.fRecord[i].taken = FALSE;
	flashMemory.fRecord[i].addr  = flashMemory.nextAvail;
	flashMemory.fRecord[i].len   = 100000; /*100k = 1M for fun */
	flashMemory.nextAvail += 100000;
    }
    flashMemory.nextAvail = flashMemory.fRecord[0].addr; /*Just for fun */
#else /*  DBG  SSSSS For debugging only */
    flashMemory.nextAvail = FLASH_MEM_START_ADDRESS;
    for (i=0; i < MAX_FLASH_FILES; i++) {
	/* Initialize all flash records and mark them unavailable */
	flashMemory.fRecord[i].taken = FALSE;
	flashMemory.fRecord[i].addr  = flashMemory.nextAvail;
	flashMemory.fRecord[i].len   = MAX_FLASH_FILE_SIZE;
	flashMemory.nextAvail += MAX_FLASH_FILE_SIZE;
    }
    flashMemory.nextAvail = FLASH_MEM_START_ADDRESS; /*Just for fun */
#endif /*  DBG  SSSSS For debugging only */
}



/****************************************************************************
 * Function:    allocFlashMem()
 * Descripion:  This routine allocates a block of flash memory to the caller
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              Address of Mem  - if all OK
 *              NULL            - otherwise
 */
U32_t
allocFlashMem()
{
    S32_t i;
    SIW_FUNC_TRACE(FM, allocFlashMem, 0, 0, 0, 0, 0);

    for (i=0; i < MAX_FLASH_FILES; i++) {
	if (flashMemory.fRecord[i].taken == FALSE) {
	    flashMemory.fRecord[i].taken = TRUE;
	    return (flashMemory.fRecord[i].addr);
	}
    }
    return (NULL);
}



/****************************************************************************
 * Function:    freeFlashMem()
 * Descripion:  This routine allocates a block of flash memory to the caller
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              Address of Mem  - if all OK
 *              NULL            - otherwise
 */
void 
freeFlashMem(U32_t address)
{
    S32_t i;

    SIW_FUNC_TRACE(FM, freeFlashMem, address, 0, 0, 0, 0);
    for (i=0; i < MAX_FLASH_FILES; i++) {
	if ((flashMemory.fRecord[i].taken == TRUE) && 
	    (flashMemory.fRecord[i].addr == address)) {
	    flashMemory.fRecord[i].taken = FALSE;
	}
    }
}


/****************************************************************************
 * Function:    initFm()
 * Descripion:  This routine initializes the internal structures of file 
 *              manager and resets the state of FM.
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t
initFm()
{
    FD_t  *fdp;
    S32_t i;
    RET_t ret;
#ifdef SCC
	S32_t fd;
#endif

    SIW_FUNC_TRACE(FM, initFm, 0, 0, 0, 0, 0);

#ifndef FM_CB_FILE
    /* Zero out the ServerTable */
    bzero( (char *)&fileTable[0], sizeof(fileTable));

#else /* FM_CB_FILE */

    /* See if the FM CB file exists. If yes, open it. If not, create it */
    if ((fd = open(FM_FILE_NAME, O_RDWR, 0644)) < 0) {
        /* Opps.. Could not Open, this means the file does not exist.
         * So go ahead and create it
         */
        if ((fd = open(FM_FILE_NAME, O_RDWR | O_CREAT, 0644)) < 0) {
            /* Now this is terribly bad. Need to report this one */
            FM_TRACE5(initFm,0xF000222, fd, FM_FILE_NAME, 0, 0, 0);
            FM_LOG3(FM_FILE_OPEN_FAIL, FM_FILE_NAME, fd, 0);
            PERR("initFm: File(%s) Open Fail fd(%d) \n", FM_FILE_NAME, fd);
            return(FM_ERROR);
        } else {
            /* Zero out the ServerTable */
            bzero( (char *)&fileTable[0], sizeof(fileTable));
            ret = writeRec(fd, 0, buf, sizeof(fileTable), FM_FILE_NAME);
            if (ret < 0) {
                close(fd);
                return (FM_CB_FILE_WR_FAIL);
            }
        }
    } else {
        /* We are able to open the FM file, this meas that the file 
         * exists, and therefore, go ahead and re-create all of the FM
         * Control records
         */
        ret = readRec(fd, 0, (char *)&fileTable[0], sizeof(fileTable), 
                                                 FM_FILE_NAME);
        if (ret < 0) {
            PERR("initFm:0) File=%s Read Fail fd=%d ret=%d toRd=%d Fm %d\n", 
                        FM_FILE_NAME, fd, ret, sizeof(fileTable), 0);
            close(fd);
            RETURN (FM_CB_FILE_RD_FAIL);
        }
        close(fd);
    }
#endif /* FM_CB_FILE */

    /* Here initialize and allocate the list of free file Descriptors */
    lstInit(&freeFdList);

#ifdef NSC 
       nscAnmtTableSem = SiwSemCreate("nscAnmteTable:fm",
                                         SIW_SEM_BINARY,
                                         SIW_SEM_Q_PRIORITY,
                                         SIW_SEM_FULL,
                                         SIW_RECV_NO_ACTION);

	   tonePlanSem = SiwSemCreate("nscTonePlanTable:fm",
                                         SIW_SEM_BINARY,
                                         SIW_SEM_Q_PRIORITY,
                                         SIW_SEM_FULL,
                                         SIW_RECV_NO_ACTION);
#endif /* NSC */

#ifdef SCC
    lstInit(&imageFtList);
    lstInit(&anmtFtList);
    
    /* Initialize the semaphore for the write protection of Image File Table */
       imageFileTableSem = SiwSemCreate("imagefileTable:fm",
                                         SIW_SEM_BINARY,
                                         SIW_SEM_Q_PRIORITY,
                                         SIW_SEM_FULL,
                                         SIW_RECV_NO_ACTION);

       anmtFileTableSem = SiwSemCreate("anmtfileTable:fm",
                                         SIW_SEM_BINARY,
                                         SIW_SEM_Q_PRIORITY,
                                         SIW_SEM_FULL,
                                         SIW_RECV_NO_ACTION);

	   tonePlanFileTableSem = SiwSemCreate("tonePlanFileTable:fm",
                                         SIW_SEM_BINARY,
                                         SIW_SEM_Q_PRIORITY,
                                         SIW_SEM_FULL,
                                         SIW_RECV_NO_ACTION);

       fmHDImageTbSem = SiwSemCreate("fmHDImgTb:fm",
                                         SIW_SEM_BINARY,
                                         SIW_SEM_Q_PRIORITY,
                                         SIW_SEM_FULL,
                                         SIW_RECV_NO_ACTION);

       fmHDAnmtTbSem = SiwSemCreate("fmHDAnTb:fm",
                                         SIW_SEM_BINARY,
                                         SIW_SEM_Q_PRIORITY,
                                         SIW_SEM_FULL,
                                         SIW_RECV_NO_ACTION);

       fmCrmSyncSem = SiwSemCreate("fmHDAnmtTb:fm",
                                         SIW_SEM_BINARY,
                                         SIW_SEM_Q_PRIORITY,
                                         SIW_SEM_FULL,
                                         SIW_RECV_NO_ACTION);
	   

    /* create the HD Image File Table */
    fd = open(HD_IMAGE_FILE_TABLE, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        /* Opss... we have a problem here */
        FM_TRACE5(initFm, 0xF000002, fd, 0, 0, 0, 0);
        PERR("initFm: File Open Fail fd(%d) \n", fd);
        return (FM_CB_FILE_OPEN_FAIL);
    }
    close (fd);

    /* create the new HD Image File Table */
    fd = open(NEW_HD_IMAGE_FILE_TABLE, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        /* Opss... we have a problem here */
        FM_TRACE5(initFm, 0xF000002, fd, 0, 0, 0, 0);
        PERR("initFm: File Open Fail fd(%d) \n", fd);
        return (FM_CB_FILE_OPEN_FAIL);
    }
    close (fd);

    /* create the HD Anmt File Table */
    fd = open(HD_ANMT_FILE_TABLE, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        /* Opss... we have a problem here */
        FM_TRACE5(initFm, 0xF000002, fd, 0, 0, 0, 0);
        PERR("initFm: File Open Fail fd(%d) \n", fd);
        return (FM_CB_FILE_OPEN_FAIL);
    }
    close (fd);

    /* create the new HD Anmt File Table */
    fd = open(NEW_HD_ANMT_FILE_TABLE, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        /* Opss... we have a problem here */
        FM_TRACE5(initFm, 0xF000002, fd, 0, 0, 0, 0);
        PERR("initFm: File Open Fail fd(%d) \n", fd);
        return (FM_CB_FILE_OPEN_FAIL);
    }
    close (fd);

#endif /* SCC */

    /* Allocate FDs and add to the free list */
    for (i=0; i < MAX_FILES; i++) {
	/* Allocate memory for a FD */
	fdp = SiwAllocMem(SIW_STATIC, sizeof(FD_t), WAIT_FOREVER);
	lstAdd(&freeFdList, &fdp->node);
    }
    initFlashRecords();
#if 0
     /* rebuild the RAM Image File Table */
    ret = fmRebuildImageFileTable();
    if (ret <0) {
        /* Opss... we have a problem here */
        FM_TRACE5(initFm, 0xF000002, fd, 0, 0, 0, 0);
        PERR("initFm: RAM Image File Table Rebuild Fail ret(%d) \n", ret);
#if 0
        return (FM_CB_FILE_OPEN_FAIL);
#endif
    }
#endif /* 0 */

    /* Here publish all the services for the remote users of File Manager */
    /* Publish FmRemGetImageFileChecksum() */
    ret = MrpcAddPortFunc(FM_MOD,FM_GET_IMAGE_FILE_CHECKSUM_SERVICE,FmRemGetImageFileChecksum);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000001,ret, FM_MOD,FM_GET_IMAGE_FILE_CHECKSUM_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_GET_IMAGE_FILE_CHECKSUM_SERVICE);
	return(FM_ERROR);
    }

    /* Publish FmRemUpdateAnmtFileTable() */
    ret = MrpcAddPortFunc(FM_MOD, FM_UPDATE_ANMT_FILE_TABLE_SERVICE,FmRemUpdateAnmtFileTable);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000001,ret, FM_MOD,FM_UPDATE_ANMT_FILE_TABLE_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_UPDATE_ANMT_FILE_TABLE_SERVICE);
	return(FM_ERROR);
    }

    /* Publish FmRemSetAnmtFileFlag() */
    ret = MrpcAddPortFunc(FM_MOD, FM_SET_ANMT_FILE_FLAG_SERVICE,FmRemSetAnmtFileFlag);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000001,ret, FM_MOD,FM_SET_ANMT_FILE_FLAG_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_SET_ANMT_FILE_FLAG_SERVICE);
	return(FM_ERROR);
    }

	/* Publish FmRemUpdateAnmtFileTable() */
    ret = MrpcAddPortFunc(FM_MOD, FM_UPDATE_TONE_PLAN_FILE_TABLE_SERVICE,FmRemUpdateToneFileTable);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000001,ret, FM_MOD,FM_UPDATE_TONE_PLAN_FILE_TABLE_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_UPDATE_TONE_PLAN_FILE_TABLE_SERVICE);
	return(FM_ERROR);
    }

	/* Publish fmSetRemToneTimeOut() */
    ret = MrpcAddPortFunc(FM_MOD, FM_SET_TONE_TIMEOUT_SERVICE,fmSetRemToneTimeOut);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000001,ret, FM_MOD,FM_SET_TONE_TIMEOUT_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_SET_TONE_TIMEOUT_SERVICE);
	return(FM_ERROR);
    }

    /* Publish FmRemCreate() */
    ret = MrpcAddPortFunc(FM_MOD,FM_CREATE_SERVICE,FmRemCreate);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000001,ret, FM_MOD,FM_CREATE_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_CREATE_SERVICE);
	return(FM_ERROR);
    }

    /* Publish FmRemDelete() */
    ret= MrpcAddPortFunc(FM_MOD, FM_DELETE_SERVICE, FmRemDelete);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000002,ret, FM_MOD,FM_DELETE_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_DELETE_SERVICE);
	return(FM_ERROR);
    }

    /* Publish FmRemOpen() */
    ret= MrpcAddPortFunc(FM_MOD, FM_OPEN_SERVICE, FmRemOpen);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000003,ret, FM_MOD,FM_OPEN_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_OPEN_SERVICE);
	return(FM_ERROR);
    }

    /* Publish FmRemClose() */
    ret= MrpcAddPortFunc(FM_MOD, FM_CLOSE_SERVICE, FmRemClose);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000004,ret, FM_MOD,FM_CLOSE_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_CLOSE_SERVICE);
	return(FM_ERROR);
    }

    /* Publish FmRemRead() */
    ret= MrpcAddPortFunc(FM_MOD, FM_READ_SERVICE, FmRemRead);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000005,ret, FM_MOD,FM_READ_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_READ_SERVICE);
	return(FM_ERROR);
    }

    /* Publish FmRemWrite() */
    ret= MrpcAddPortFunc(FM_MOD, FM_WRITE_SERVICE, FmRemWrite);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000006,ret, FM_MOD,FM_WRITE_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_WRITE_SERVICE);
	return(FM_ERROR);
    }

    /* Publish FmRemRename() */
    ret= MrpcAddPortFunc(FM_MOD, FM_RENAME_SERVICE, FmRemRename);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) {
	FM_TRACE5(initFm, 0xF0000007,ret, FM_MOD,FM_RENAME_SERVICE,0,0);
	FM_LOG3(ADD_PORT_FAILED, ret, FM_MOD, FM_RENAME_SERVICE);
	return(FM_ERROR);
    }

#ifndef SIM
    ret = MrpcAddPortFunc(NWG_MOD, NWG_FLASH_UPDATE, RemUpdateFlash);
    if ( (ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED) )
    {
       printf ("MrpcAddPortFunc: fail to register function (ErrCode=%d)\n",
                                ret);
       return(FM_ERROR);
    }

    ret = MrpcAddPortFunc(NWG_MOD, NWG_FLASH_ERASE, RemEraseFlash);
    if ( (ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED) )
    {
       printf ("MrpcAddPortFunc: fail to register function (ErrCode=%d)\n",
                                ret);
       return(FM_ERROR);
    }

#ifdef SCC
    ret = MrpcAddPortFunc(NWG_MOD, NWG_DB_RESTORE, RemDbRestore);
    if ( (ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED) )
    {
       printf ("MrpcAddPortFunc: fail to register function (ErrCode=%d)\n",
                                ret);
       return(FM_ERROR);
    }
#ifndef FLASH_BOOT
    ret = MrpcAddPortFunc(NWG_MOD, NWG_UPDATE_KEY, RemUpdateKey);
    if ( (ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED) )
    {
       printf ("MrpcAddPortFunc: fail to register function (ErrCode=%d)\n",
                                ret);
       return(FM_ERROR);
    }
    ret = MrpcAddPortFunc(NWG_MOD, NWG_UPDATE_PSWD, RemUpdatePassword);
    if ( (ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED) )
    {
       printf ("MrpcAddPortFunc: fail to register function (ErrCode=%d)\n",
                                ret);
       return(FM_ERROR);
    }
    ret = MrpcAddPortFunc(NWG_MOD, NWG_UPDATE_ROUTE, RemUpdateRoute);
    if ( (ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED) )
    {
       printf ("MrpcAddPortFunc: fail to register function (ErrCode=%d)\n",
                                ret);
       return(FM_ERROR);
    }
    ret = MrpcAddPortFunc(NWG_MOD, NWG_ADD_ROUTE, RemAddRoute);
    if ( (ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED) )
    {
       printf ("MrpcAddPortFunc: fail to register function (ErrCode=%d)\n",
                                ret);
       return(FM_ERROR);
    }
    ret = MrpcAddPortFunc(NWG_MOD, NWG_DELETE_ROUTE, RemDelRoute);
    if ( (ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED) )
    {
       printf ("MrpcAddPortFunc: fail to register function (ErrCode=%d)\n",
                                ret);
       return(FM_ERROR);
    }
#endif /* FLASH_BOOT */
#endif

#endif

    return (FM_OK);
}





/****************************************************************************
 * Function:    FmMain()
 * Descripion:  This is the main entry point for File Manager (FM) task.
 *              Initially, it is meant to deal with the FLASH programmin. Later
 *              more functionality may come along.
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              Never Returns unless the world falls apart
 *            
 */
RET_t
FmMain(int a0, int a1, int a2, int a3, int a4, 
	 int a5, int a6, int a7, int a8, int a9)
{
    MSG_t     *msg;
    MSG_t     *outMsg;
    S32_t     msgLen;
    FM_HDR_t  *pHdr, *pOutHdr;
    RET_t     ret;
    MSG_BUF_HDR_t itMsg;
#ifdef SCC
	U32_t i;
#ifndef FLASH_BOOT
	struct  stat    filestat;
	FD_t            *fd;
	PASSWORD_t      passfile;
    ROUTE_t         routeEntry;
#endif /* FLASH_BOOT */
#endif /* SCC */

#ifdef NSC
	U32_t i;
#endif

    /* Initialize the internal data structures for FM */
    if (initFm()) {
	/* Error case */ 
	FM_TRACE5(FmMain, 0xF0000001,ret, 0,0,0,0);
	FM_LOG(INIT_FAIL, ret);
	FM_DEBUG ("Could not initialize FM \n");
	SiwTaskInitAck(FM_ERROR);
    }

    /* Now that all of the Internal structures for FM are initialized,
     * let the system services know of the state of FM task.
     */
    SiwTaskInitAck(FM_OK);

#ifdef NSC
    /* initialize the a nscAnmtTable */
    for ( i = 0; i < MAX_ANNOUNCEMENT_FILES; i++)
        nscAnmtTable[i] = -1;
#endif /* NSC*/
#ifdef SCC
    /* initialize the anmtMibIndex table */
    for ( i = 0; i < MAX_ANNOUNCEMENT_FILES; i++)
        anmtMibIndex[i] = -1;

    /* rebuild the RAM image file table */
    ret = fmRebuildImageFileTable();
    if (ret <0) {
        /* Opss... we have a problem here */
        FM_TRACE5(initFm, 0xF000002, 0, 0, 0, 0, 0);
        PERR("initFm: RAM Image File Table Rebuild Fail ret(%d) \n", ret);
    }

#if 1 
    /* rebuild the RAM anmt file table */
    ret = fmRebuildAnmtFileTable();
    if (ret <0) {
        /* Opss... we have a problem here */
        FM_TRACE5(initFm, 0xF000002, 0, 0, 0, 0, 0);
        PERR("initFm: RAM Anmt File Table Rebuild Fail ret(%d) \n", ret);
    }
#endif 
#ifndef FLASH_BOOT
    if (stat (PASSWORD_FILENAME, &filestat) == ERROR)
    {  /* This file does not exist */
       fd = FmOpen (PASSWORD_FILENAME, O_CREAT|O_RDWR);
       if (fd == NULL)
       {
          printf ("GetPassword: Can not open file %s\n", PASSWORD_FILENAME);
          return (ERROR);
       }
       bzero ((void *)(&passfile), sizeof (PASSWORD_t));
       strncpy ((char *)(&passfile.root), "cisco",PASSWORD_SIZE);
       strncpy ((char *)(&passfile.super),"cisco",PASSWORD_SIZE);
       FmWrite (fd, (char *)(&passfile), 0, sizeof(PASSWORD_t), TRUE);
       FmClose (fd);
    }
    else
    {
       if (filestat.st_size < sizeof(PASSWORD_t))
       {  /* The password file is old --> clear it and re-create */
          fd = FmOpen (PASSWORD_FILENAME, O_CREAT|O_RDWR);
          if (fd == NULL)
          {
             printf ("GetPassword: Can not open file %s\n", PASSWORD_FILENAME);
             return (ERROR);
          }
          bzero ((void *)(&passfile), sizeof (PASSWORD_t));
          strncpy ((char *)(&passfile.root), "cisco",PASSWORD_SIZE);
          strncpy ((char *)(&passfile.super),"cisco",PASSWORD_SIZE);
          FmWrite (fd, (char *)(&passfile), 0, sizeof(PASSWORD_t), TRUE);
          FmClose (fd);
       }
    }

    if (stat (ROUTE_FILENAME, &filestat) == ERROR)
    { 
       fd = FmOpen (ROUTE_FILENAME, O_CREAT|O_RDWR);
       if (fd == NULL)
       {
          printf ("initFm: Can not open file %s\n", ROUTE_FILENAME);
          return (ERROR);
       }
       bzero ((void *)(&routeEntry), sizeof (ROUTE_t));
       strncpy ((char *)(&routeEntry.ipAddr), "0.0.0.0", IP_ADDR_SIZE);
       FmWrite (fd, (char *)(&routeEntry), 0, sizeof(ROUTE_t), TRUE);
       FmClose (fd);
    }

#endif

#endif /* SCC*/

    while (1) {   /* Never ending loop starts here */

	/* See if we have any thing to do */
	if ( (msgLen = SiwMsgQReceive (MSGQ2FM, &itMsg, 
				sizeof(U32_t), WAIT_FOREVER)) <= 0) {
	    FM_TRACE5(FmMain, 0xF0000002,msgLen, 0,0,0,0);
	    FM_LOG(RCV_MSG_FAIL, msgLen);
	    FM_DEBUG ("FmMain: error message queue receive\n");
	    continue; 
        }

	/* We got work to do, so proceed..... */

	msg = (MSG_t *)itMsg.msgBufPtr;
	/* First sanitize the message */
	if (msg == NULL) { 
	    /* Oppsss.. we got a NULL poiner; This is bad */
	    FM_TRACE5(FmMain, 0xF0000003,msg, 0,0,0,0);
	    FM_LOG(RCV_MSG_NULL, msg);
	    FM_DEBUG("FM recevied a Message with NULL Buffer ptr\n");
	} else {
	    pHdr = FM_HDR_PTR(msg);

	    FM_TRACE1(FmMain, 0x04,pHdr->msg, pHdr->srcSlot,0,0,0);
	    FM_LOG2(INFO, pHdr->msg, pHdr->srcSlot);
	    FM_DEBUG("FmMain(): Got Message(%x); arg1(%x) arg2(%x)\n", 
				       pHdr->msg, pHdr->arg1, pHdr->arg2);
	    /* We have a valid message to process */
	    switch (pHdr->msg) {

	    case FM_WRITE_SERVICE:
		/* first of all get a response buffer */
		outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
		pOutHdr = FM_HDR_PTR(outMsg);
		bcopy( (char *)pHdr, (char *)pOutHdr, sizeof(FM_HDR_t)); 

		ret =  FmWrite(  (FD_t *)pHdr->arg1,   /* file Desc. Ptr */
				 (char *)&msg->data[pHdr->arg2],/*bufAddr*/
				 (S32_t)pHdr->arg3,         /* offset */
				 (S32_t)pHdr->arg4,         /* maxBytes */
				 (BOOL_t)pHdr->arg5        /* append */
			       );
		if (ret < 0 ) {
		    FM_TRACE5(FmMain, 0xF0000005,ret, pHdr->arg1,
				      pHdr->arg2,pHdr->arg3,pHdr->arg4);
		    FM_LOG3(FMWRITE_FAIL, ret, pHdr->arg1, pHdr->arg2);
		    PACK_FM_ERR_MSG(pOutHdr,ret)
		} else {
		    /* Here pack the response and return */
		    outMsg->data[sizeof(FM_HDR_t)] = ret;
		    PACK_FM_RESP_MSG(pOutHdr);
		}
		/* Here need to send a message back the sender to report
		 * it about the results of the write 
		 */
		pOutHdr->srcSlot = SiwMyPhySlotNo();
		ret = MrpcAsync(pHdr->srcSlot, pHdr->srcModNum, 
				pHdr->srcServNum, outMsg, GURANTEED, 
				LO, NULL, 0, NULL);
		if (ret < 0 ) {
		    /* Opsss. Do not know how to recover out of this */
		    FM_TRACE5(FmMain, 0xF0000007,ret, pHdr->srcSlot,
				  pHdr->srcModNum,pHdr->srcServNum,outMsg);
		    FM_LOG2(MRPC_ASYNC_FAIL, ret, pHdr->srcSlot);
		    FM_DEBUG("FmMain(): Could not make MrpcAsync call (%d)\n",ret);
		}
		break;

	    case FM_RESP:

	    case FM_ACK:

	    case FM_NACK:

	    default:
		/* Unknown Massage */
		FM_TRACE5(FmMain, 0xF0000006,pHdr->msg, pHdr->srcSlot,0,0,0);
		FM_LOG2(ILLEGAL_MSG, pHdr->msg, pHdr->srcSlot);
		FM_DEBUG("FmMain(): FM received an unknown Msg(%d) \n", pHdr->msg);
		/* Free up this message */
	    } /* switch () */
	} /* else */
	SiwFreeMem(msg);
    } /* whle (1) */

    return(FM_ERROR);
}
