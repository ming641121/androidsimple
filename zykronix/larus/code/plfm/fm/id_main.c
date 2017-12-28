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
#include "ioLib.h"
#include "string.h" /* bzero(), etc */
#include "stdio.h"  /* for printf */
#include "lstLib.h"  /* for linked list */
#include "stat.h"  /* for fstat() */
#include "mrpc_if.h"
#include "fm_if.h"
#include "fm.h"
#include "sm_if.h"
#include "cm_if.h"
#include "dbm_if.h"
#include "id_if.h"
#include "id.h"
#include "id_events.h"


/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */

#if 0 /* System Services creates the ID now */
SIW_MQID_t    idMsgQId;
#endif  /* System Services creates the ID now */

FR_t flashRec[MAX_SLOTS];
S32_t idSubTaskCnt;


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

#if 1  /* SSSSSS */

void startId()
{
    U32_t stat;

    /* This routine is to spwan the MRPC task */
    stat =  
       taskSpawn("idTask", 100, 0, 4096, (FUNCPTR)IdMain, 0,0,0,0,0,0,0,0,0,0);
    printf (" ID Task io spawned status = %d\n ", stat);
}

#endif   /* SSSSSS */



/****************************************************************************
 * Function:    initId()
 * Descripion:  This routine initializes the internal structures of Image 
 *              downloader and resets the state of ID.
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t
initId()
{
    RET_t ret;

    SIW_FUNC_TRACE(ID, initId, 0, 0, 0, 0, 0);

#if 0 /* System Services creates the ID now */
    /* create the message Queue for ID task to receive messages */
    /* SSSS */
    idMsgQId = SiwMsgQCreate(MAX_ID_IN_MESSAGES, sizeof(U32_t), 
				    SIW_MSG_Q_PRIORITY, "flfm:id");
    if (idMsgQId == NULL) {
	return(ID_ERROR);
    }

    printf("The message Q Id for ID task is %x\n", idMsgQId);
#endif  /* System Services creates the ID now */

    /* Here publish all the services for the remote users of Image Downloader */

    /* Publish IdWriteFlashNextBlock() */
    ret = 
      MrpcAddPortFunc(ID_MOD, ID_WRITE_FLASH_NEXT_BLOCK, IdWriteFlashNextBlock);
    if ((ret != MRPC_OK) && (ret != MRPC_SERVICE_ALREADY_DEFINED)) 
	return(ID_ERROR);

    return (ID_OK);
}



/****************************************************************************
 * Function:    getSrcImageFileName()
 * Descripion:  This routine gets the name for image file for a given card
 *              as it pertains to the where the image lives on the SCC for
 * 	        a given card.
 * In Parms:    U32_t    card; 
 * Out Parms:   Char     **fName; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t
getSrcImageFileName(U32_t card, char **fName) 
{
#ifndef FLASH_BOOT

    if (VALID_SLOT(card)) {
	strcpy(*fName, (char *)shelf.slot[card].cCfg.srcImageFileName);
	return (ID_OK);
    }
    return (ID_ERROR);
#endif /* FLASH_BOOT */
	return (ID_OK);
}


/****************************************************************************
 * Function:    getDstImageFileName()
 * Descripion:  This routine gets the name for image file for a given card.
 *              This name pertains to the file name under which the image
 * 	        lives on the destination card under question.
 * In Parms:    U32_t    card; 
 * Out Parms:   Char     **fName; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t
getDstImageFileName(U32_t card, char **fName) 
{
#ifndef FLASH_BOOT

    if (VALID_SLOT(card)) {
	strcpy(*fName, (char *)shelf.slot[card].cCfg.dstImageFileName);
	return (ID_OK);
    }
    return (ID_ERROR);
#endif /* FLASH_BOOT */
	return (ID_OK);
}


/****************************************************************************
 * Function:    getDstFlashFileName()
 * Descripion:  This routine gets the name for Flash file for a given card.
 *              This name pertains to the file name under which the image
 * 	        lives on the destination card under question.
 * In Parms:    U32_t    card; 
 * Out Parms:   Char     **fName; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t
getDstFlashFileName(U32_t card, char **fName) 
{
#ifndef FLASH_BOOT

    if (VALID_SLOT(card)) {
	strcpy(*fName, (char *)shelf.slot[card].cCfg.dstFlashFileName);
	return (ID_OK);
    }
    return (ID_ERROR);
#endif /* FLASH_BOOT */
	return (ID_OK);
}



/****************************************************************************
 * Function:    openLocalFile()
 * Descripion:  This routine Opens a local file and verifies the FDP to be
 *              valid. otherwise it makes the FDP = NULL.
 * In Parms:    char    *name; 
 * Out Parms:   FD_t     **fdp; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
openLocalFile(char *srcName, FD_t **fdp)
{
    /* Open the file for reading */
    *fdp = FmOpen(srcName, READ_ONLY); 

    if (!VALID_FDP(*fdp)) {
	/* Opsss... we got a problem opening local file 
	 * We can not do much here; Retunn an error back to the
	 * caller of this message
	 */
	*fdp= NULL;
	return(ID_COULD_NOT_OPEN_SRC_FILE);
    }
    return(ID_OK);
}

#if 0
/****************************************************************************
 * Function:    openRemFile()
 * Descripion:  This routine Opens a local file and verifies the FDP to be
 *              valid. otherwise it makes the FDP = NULL.
 * In Parms:    char    *name; 
 * Out Parms:   FD_t     **fdp; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
openRemFile(S32_t targetCard, char *dstName, FD_t **fdp)
{
    FM_HDR_t *fmHdrP;
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    FD_t *fd;
    RET_t ret;

    SIW_FUNC_TRACE(ID, openRemFile, targetCard, dstName, fdp, 0, 0);
    /*  
     * At this time need to  open the remote file to write 
     * But, before we do that we need to allocate a message
     * buffer, fill up the FM Header message for the remote
     * File Manager task so that it knows what to do with it
     */
    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("openRemFile(): ########  WARNING!!!!! ######\n");
	return(ID_ILLEGAL_ADDRESS);
    }
    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
    strcpy(&outMsg->data[sizeof(FM_HDR_t)], dstName);
    fmHdrP->arg1 = sizeof(FM_HDR_t); /* Offset */
    fmHdrP->arg2 = WRITE_ONLY;

    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + strlen(dstName) + 1);

    { U32_t i;
	FM_DEBUG("Header Info for FmRemOpen:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", outMsg->data[i]);
	FM_DEBUG("\n");
    }
    ret = MrpcSync(targetCard, FM_MOD, FM_OPEN_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
    FM_DEBUG("openRemFile(): First Time Return code from Mrpcsync is %d\n", ret);
    if (ret < 0) {
#if 0 /* SSSSS */
	/* We had problem opening the destination file */
	/* 
	 * We most likly timed out on the MRPC.
	 * Make one more attempt to open it and see if we are 
	 * able to open it second time.
	 */
	SiwFreeMem(replyMsg);
	outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
        /* Set the Data Length */
        SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + strlen(dstName) + 1);
	if (!VALID_ADDRESS(outMsg)) {
	    /* Scream!!!!! We just ran out of buffers */
	    FM_DEBUG("openRemFile():222 ########  WARNING!!!!! ######\n");
	    return(ID_ILLEGAL_ADDRESS);
	}
	fmHdrP = (FM_HDR_t *)&outMsg->data[0];
	strcpy(&outMsg->data[sizeof(FM_HDR_t)], dstName);
	fmHdrP->arg1 = sizeof(FM_HDR_t); /* Offset */
	fmHdrP->arg2 = WRITE_ONLY;
	ret = MrpcSync(targetCard, FM_MOD, FM_OPEN_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
	FM_DEBUG("openRemFile():2nd Time Return code from Mrpcsync is %d\n", ret);
#endif /* 0  SSSSS */

	if (ret < 0) {
	    /* Failed to open second time; so give up */
	    *fdp = NULL;
	    if (replyMsg != NULL)
		SiwFreeMem(replyMsg);
	    return(ret);
	}
    }
    /* Just make sure that the file got opened OK, if so, get
     * the dest file descriptor poiner, as it is needed for
     * writing to the destination
     */
    if (((FM_HDR_t *)&(replyMsg->data[0]))->err != FM_OK) {
	/* Opsss, we got an error opening the remote file */
	/* Need to take recovery action */
	SiwFreeMem(replyMsg);
	*fdp = NULL;
	return(ID_COULD_NOT_OPEN_DST_FILE);
    } 

    bcopy(
       (char *)&replyMsg->data[sizeof(FM_HDR_t)], (char *)&fd, sizeof(U32_t));
    SiwFreeMem(replyMsg);

    if (fd == NULL) {
	/* There must have been an error on the way */
	/* need to handle this case and do not proceed from
	 * here on, as we are not working with a valid File
	 * descriptor. The user needs to be informed that 
	 * things did not go OK and therefore a recoverty 
	 * mechanism needs to kick in at this time
	 */
	*fdp = NULL;
	return(ID_COULD_NOT_OPEN_DST_FILE);
    }
    *fdp = fd;
    return(ID_OK);
}
#endif 



/****************************************************************************
 * Function:    createRemFile()
 * Descripion:  This routine Opens a local file and verifies the FDP to be
 *              valid. otherwise it makes the FDP = NULL.
 * In Parms:    char    *name; 
 * Out Parms:   none 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
createRemFile(S32_t targetCard, char *dstName)
{
    FM_HDR_t *fmHdrP;
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    RET_t ret;

    SIW_FUNC_TRACE(ID, createRemFile, targetCard, dstName, 0, 0, 0);
    /*  
     * At this time need to  create the remote file to write 
     * But, before we do that we need to allocate a message
     * buffer, fill up the FM Header message for the remote
     * File Manager task so that it knows what to do with it
     */
    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("createRemFile(): ########  WARNING!!!!! ######\n");
	return(ID_ILLEGAL_ADDRESS);
    }
    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
    strcpy(&outMsg->data[sizeof(FM_HDR_t)], dstName);
    fmHdrP->arg1 = sizeof(FM_HDR_t); /* Offset */
    fmHdrP->arg2 = READ_WRITE | CREATE;
    fmHdrP->arg3 = RAM;
    fmHdrP->arg4 = MAX_FILE_LENGTH;
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + strlen(dstName) + 1);
    { U32_t i;
	FM_DEBUG("Header Info for FmRemCreate:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", outMsg->data[i]);
	FM_DEBUG("\n");
    }
    ret = MrpcSync(targetCard, FM_MOD, FM_CREATE_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
    FM_DEBUG("createRemFile(): First Time Return code from Mrpcsync is %d\n", ret);
    if (ret < 0) {
#if 0 /* SSSSS */
	/* We had problem opening the destination file */
	/* 
	 * We most likly timed out on the MRPC.
	 * Make one more attempt to open it and see if we are 
	 * able to open it second time.
	 */
	SiwFreeMem(replyMsg);
	outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
	if (!VALID_ADDRESS(outMsg)) {
	    /* Scream!!!!! We just ran out of buffers */
	    FM_DEBUG("createRemFile(): 2222 ########  WARNING!!!!! ######\n");
	    return(ID_ILLEGAL_ADDRESS);
	}
	fmHdrP = (FM_HDR_t *)&outMsg->data[0];
	strcpy(&outMsg->data[sizeof(FM_HDR_t)], dstName);
	fmHdrP->arg1 = sizeof(FM_HDR_t); /* Offset */
	fmHdrP->arg2 = READ_WRITE | CREATE;
	fmHdrP->arg3 = RAM;
	fmHdrP->arg4 = MAX_FILE_LENGTH;
        /* Set the Data Length */
        SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + strlen(dstName) + 1);
	ret = MrpcSync(targetCard, FM_MOD, FM_CREATE_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
	FM_DEBUG("createRemFile():2nd Return code from Mrpcsync is %d\n", ret);
#endif /* 0  SSSSS */

	if (ret < 0) {
	    /* Failed to open second time; so give up */
	    if (replyMsg != NULL)
		SiwFreeMem(replyMsg);
	    return(ret);
	}
    }
    /* Just make sure that the file got opened OK, if so, get
     * the dest file descriptor poiner, as it is needed for
     * writing to the destination
     */
    if (((FM_HDR_t *)&(replyMsg->data[0]))->err <  0) {
	/* Opsss, we got an error creating the remote file */
	/* Need to take recovery action */
	SiwFreeMem(replyMsg);
	return(ID_COULD_NOT_CREATE_DST_FILE);
    } 

    SiwFreeMem(replyMsg);

    return(ID_OK);
}


#if 0

/****************************************************************************
 * Function:    closeBothFiles()
 * Descripion:  This routine Opens a local file and verifies the FDP to be
 *              valid. otherwise it makes the FDP = NULL.
 * In Parms:    char    *name; 
 * Out Parms:   FD_t     **fdp; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
closeBothFiles(S32_t targetCard, FD_t *srcFdp, FD_t *destFdp)
{
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    FM_HDR_t *fmHdrP;
    RET_t ret;

    SIW_FUNC_TRACE(ID, closeBothFiles, targetCard, srcFdp, destFdp, 0, 0);

    /* Now close both local and remote files */
    FmClose(srcFdp);

    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("closeBothfiles(): ########  WARNING!!!!! ######\n");
	return(ID_ILLEGAL_ADDRESS);
    }
    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
    fmHdrP->arg1 = (U32_t)destFdp;
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t));
    { U32_t i;
	FM_DEBUG("Header Info for FmRemClose:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", outMsg->data[i]);
	FM_DEBUG("\n");
    }
    ret = MrpcSync(targetCard, FM_MOD, FM_CLOSE_SERVICE, outMsg, LO, 
					WAIT_FOREVER, &replyMsg);
    if (ret < 0) {
#if 0 /* SSSSSS */
	/* Failed on MRPC Message; Give it one more shot and see if
	 * we can close it the second time
	 */
	outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
	if (!VALID_ADDRESS(outMsg)) {
	    /* Scream!!!!! We just ran out of buffers */
	    FM_DEBUG("closeBothfiles(): 2222 ########  WARNING!!!!! ######\n");
	    return(ID_ILLEGAL_ADDRESS);
	}
	fmHdrP = (FM_HDR_t *)&outMsg->data[0];
	fmHdrP->arg1 = (U32_t)destFdp;
        /* Set the Data Length */
        SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t));
	ret = MrpcSync(targetCard, FM_MOD, FM_CLOSE_SERVICE, outMsg, LO, 
					    WAIT_FOREVER, &replyMsg);
#endif /* 0  SSSSSS */
	if (ret < 0) {
	    /* Failed again. This is not good; Well, we have already
	     * tried twice. So give up
	     */
	    if (replyMsg != NULL)
		SiwFreeMem(replyMsg);
        /* added this to distinguish MRPC timeouts from appl errors */
        if (ret == MRPC_TIMEOUT)
            return(ID_MRPC_TIMEOUT);
        else
            return(ID_ERROR);
	}
    }

    FM_DEBUG("Closed the local and remote files \n");

    /* we are done with the inbuffer, so free it up now */
    SiwFreeMem(replyMsg);

    return (ID_OK);
}
#endif

/****************************************************************************
 * Function:    sendDataToRemCard()
 * Descripion:  This routine will send data to remote card using MrpcSync()
 *              If a failure is returned, it re-tires the same message
 * In Parms:    S32_t   targetCard 
 *              S32_t   mod        - Target module number
 *              S32_t   service    - Target service number
 *              MSG_t   *msg       - the message to be sent
 * Out Parms:   FD_t     **fdp; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
sendDataToRemCard(S32_t targetCard, S32_t mod, S32_t service, MSG_t *msg)
{
    RET_t ret;
    MSG_t *replyMsg = NULL;

    SIW_FUNC_TRACE(ID, sendDataToRemCard, targetCard, mod, service, msg, 0);
    { U32_t i;
	FM_DEBUG("Header Info for FmRemwrite:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", msg->data[i]);
	FM_DEBUG("\n");
	FM_DEBUG("Data To Be written:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", msg->data[sizeof(FM_HDR_t)+i]);
	FM_DEBUG("\n");
    }
    ret = MrpcSync(targetCard, mod, service, msg, LO, WAIT_FOREVER, &replyMsg);
    FM_DEBUG("sendDataToRemCard()1st Time Return code from Mrpcsync is %d\n", ret);
    if (ret < 0) {
#if 0 /* SSSSS */
	/* Failed on MRPC Message; Give it one more shot and see if
	 * we can close it the second time
	 */
	ret = MrpcSync(targetCard, mod, service, msg, LO, WAIT_FOREVER, 
								 &replyMsg);
	FM_DEBUG("sendDataToRemCard() 2nd Time Return code from Mrpcsync is %d\n",
		      ret);
#endif  /*0  SSSSS */

	if (ret < 0) {
	    /* Failed again. This is not good; Well, we have already
	     * tried twice. So give up
	     */
	    if (replyMsg != NULL)
		SiwFreeMem(replyMsg);
	    return(ID_ERROR);
	}
    }

    /* we are done with the inbuffer, so free it up now */
    SiwFreeMem(replyMsg);

    return (ID_OK);
}



/****************************************************************************
 * Function:    processDownloadReq()
 * Descripion:  This routine gets the name for image file for a given card.
 *              This name pertains to the file name under which the image
 * 	        lives on the destination card junder question.
 * In Parms:    U32_t    card; 
 * Out Parms:   Char     **fName; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
processDownloadReq(S32_t targetCard, char *srcName, char *dstName)
{
    FD_t  *srcFdp, *destFdp;
    FM_HDR_t *fmHdrP;
    S32_t hdrLen, blockLen, fileLen;
    S32_t i, numBlocks;
    MSG_t *outMsg;
    RET_t ret;
    U32_t addr;
    struct stat fileStat;

    SIW_FUNC_TRACE(ID, processDownloadReq, targetCard, srcName, dstName, 0, 0);

    /* If the card is not present, no need to proceed any further;
     * this was callin SiwCardState earlier; changed it to 
     * GetCardState bcos we should be looking at the physical card
     * state instead of logical card state;
     */
    if (CARD_REMOVED == GetCardState(targetCard)) {
        printf("processDownloadReq: Download to card(%d) aborted\n",targetCard);
        return(ID_ILLEGAL_SLOT);
    }
    ret = openLocalFile(srcName, &srcFdp);
    if (ret) 
	return (ret);
    ret = openRemFile(targetCard, dstName, &destFdp);
    if (ret) {
	closeBothFiles(targetCard, srcFdp, destFdp);
	return (ret);
    }

    /* The size of max payload that can be moved per message */
    blockLen = MAX_BUFF_LEN - sizeof (FM_HDR_t);

    /* Now read a buffer full of data including the
     * header of the image file to see how big
     * the file is that needs to be downloaded
     */
    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg))  {
	FM_DEBUG("processDownloadReq(): 2222 ########  WARNING!!!!! ######\n");
	return (FM_ILLEGAL_ADDRESS);
    }
    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
    hdrLen = FmRead( srcFdp, &outMsg->data[sizeof(FM_HDR_t)], 0, 
						    blockLen);
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, MAX_BUFF_LEN);

    if (hdrLen <= 0 ) {
	/* Oppsss... we have a problem reading the file.....
	 */
	SiwFreeMem(outMsg);
	FM_DEBUG("processDownloadReq():Could not read file (%d)\n",hdrLen);
	closeBothFiles(targetCard, srcFdp, destFdp);
	return(ID_NO_DATA_READ);
    }
    FM_DEBUG("Read %d bytes out of %d from src \n",hdrLen,blockLen);

    FM_DEBUG("Data Read:  ");
    for(i=0; i < 20; i++)
	FM_DEBUG("%x ", outMsg->data[sizeof(FM_HDR_t)+i]);
    FM_DEBUG("\n");

#if 0  /* SSSSSS */
    fileLen = 
       ((IMAGE_FILE_HDR_t *)(&outMsg->data[sizeof(FM_HDR_t)]+16))->len;
#else  /* SSSSSS */
    stat(srcName, &fileStat);
    fileLen = fileStat.st_size;
    printf("processDownloadReq(): Size of file %s is %d\n", srcName, fileLen);

#endif  /* SSSSSS */
    FM_DEBUG("File Length of src file is %d  \n",fileLen);

    if (fileLen <= 0) {
	/* Ooppssss.... We have a problem, We just read
	 * bogus file. No need to proceed any further. 
	 */ 
	SiwFreeMem(outMsg);
	closeBothFiles(targetCard, srcFdp, destFdp);
	return(ID_LEN_OF_FILE_IS_BAD);
    }

    /* Now see how many mesasges it is going to take to ship 
     * this file over
     */
    numBlocks = 
	    ((S32_t)(fileLen - sizeof(IMAGE_FILE_HDR_t)))/blockLen;

    if (numBlocks < 0)
	numBlocks = 0;
    /* Almost certainly, this is not going to be a exatly
     * divisible number, so need to bump up the number of blocks
     */
    if ((numBlocks*blockLen) < (S32_t)(fileLen - sizeof(IMAGE_FILE_HDR_t)))
	numBlocks++;

#if 1 /* SSSSSS */
    printf("Gonna take %d messages for card(%d) \n",numBlocks, targetCard);
#else /* SSSSSS */
    FM_DEBUG("Gonna take %d messages for card(%d) \n",numBlocks, targetCard);
#endif /* SSSSSS */

    /* Now get into the loop and start shiping this data over*/
    i=0;
    addr = 0;
    do {
	fmHdrP->msg = FM_WRITE_SERVICE;
	fmHdrP->arg1 = (U32_t)destFdp;
	fmHdrP->arg2 = sizeof(FM_HDR_t); /* Offset*/
	/*
	fmHdrP->arg3 = i * hdrLen;
	*/
	fmHdrP->arg3 = addr;
	fmHdrP->arg4 = hdrLen; 
	fmHdrP->arg5 = (BOOL_t)0; 
	ret = sendDataToRemCard(targetCard, FM_MOD, FM_WRITE_SERVICE, outMsg);
	if (ret < 0) {  /* it has already been tried twice */
	    closeBothFiles(targetCard, srcFdp, destFdp);
	    return (ret);
	}
	addr += hdrLen;
	FM_DEBUG("wrote %d bytes to dest file @%x card(%d) \n",
			       hdrLen, addr, targetCard);

	if (++i < numBlocks) {
	    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
	    if (!VALID_ADDRESS(outMsg)) {
		/* Scream!!!!! We just ran out of buffers */
		FM_DEBUG("processDownloadReq():222 ######  WARNING!!!!! ######\n");
                closeBothFiles(targetCard, srcFdp, destFdp);
		return(ID_ILLEGAL_ADDRESS);
	    }
	    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
	    hdrLen = FmRead( srcFdp, 
		     &outMsg->data[sizeof(FM_HDR_t)], 0, blockLen );
	    FM_DEBUG("inLoop Read %d bytes of %d from src %d block card(%d)\n",
						hdrLen,blockLen, i, targetCard);
            if (hdrLen <=0) {
                closeBothFiles(targetCard, srcFdp, destFdp);
                PERR("processDownloadREq:Read FAILED %d bytes of %d block %d\n",
                                                hdrLen,blockLen, i);
                SiwFreeMem(outMsg);
                return (hdrLen);
            }

            /* Set the Data Length */
            SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + hdrLen);
	}
    } while(i < numBlocks);

    closeBothFiles(targetCard, srcFdp, destFdp);
    return (ID_OK);
}




/****************************************************************************
 * Function:    flashCheckStatus()
 * Descripion:  This routine is used to download Firmware image in any of the
 *              cards in AXSS-1600.
 *              This routine sends a message to ID task to perform such
 *              function. 
 * In Parms:    S32_t slot         - The slot number of the card to be dnloaded
 *              ID_MEM_TYPE_t mem  - Where to download RAM or FLASH
 * Out Parms:   None 
 * Return Values:
 *              ID_OK      - If all works out OK
 *              ID_ERROR
 */
S32_t
flashCheckStatus(U32_t slot)
{
    RET_t    ret;
    ID_HDR_t *pHdr;
    MSG_t    *outMsg;
    MSG_BUF_HDR_t itMsg;

    SIW_FUNC_TRACE(ID, flashCheckStatus, slot, 0, 0, 0, 0);

    if (!VALID_SLOT(slot))  {
	return (ID_ILLEGAL_SLOT);
    }

    /* first of all get a message buffer */
    outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("flashCheckStatus(): ########  WARNING!!!!! ######\n");
	return(ID_ILLEGAL_ADDRESS);
    }
    pHdr = (ID_HDR_t *)&outMsg->data[0];
    pHdr->msg = ID_CMD_DOWNLOAD_NEXT_FLASH_BLOCK;
    pHdr->srcSlot  = SiwMyPhySlotNo();
    pHdr->destSlot = slot;
    pHdr->srcMsgQId  = MSGQ2ID; /* ID task Q Id */
    pHdr->srcTid   = SiwTaskIdSelf();
    pHdr->err      = 0;
    
    /*
     * forward the message to ID task. ID task will directly send
     * the response to the sender of this message. Thefore, the MsgQId of
     * the sender is embedded into the message.
     */
    /* Send pointer to the data */
    itMsg.msgBufPtr = outMsg;
    itMsg.dstTaskId = TASK_ID;
    ret = SiwMsgQSend(MSGQ2ID, &itMsg, sizeof(MSG_BUF_HDR_t), 
					 WAIT_FOREVER, SIW_MSG_PRI_NORMAL);
    return (ret);
}





U32_t debTask = 1;

void 
IdSubTask(U32_t targetCard, char *srcName, char *dstName, 
	  S32_t goodMsg, S32_t badMsg)
{
#ifndef FLASH_BOOT
    RET_t        ret;
    MSG_t        *outMsg;
    SM_MSG_HDR_t *pOHdr;
    MSG_BUF_HDR_t itMsg;

    SIW_FUNC_TRACE(ID, IdSubTask, targetCard,srcName,dstName,goodMsg,badMsg);

    FM_DEBUG("IdSubTask Got Spawned\n");

    while (debTask == 0) {
	taskDelay(500);
    }

    FM_DEBUG("IdSubTask():card(%x); srcName(%s); dstName(%s); good(%d) bad(%d)\n", 
	   targetCard, srcName, dstName, goodMsg, badMsg);

#if 0
    SiwTaskInitAck(ID_OK);
#endif

    FM_DEBUG("IdSubTask Got a good Message \n");

    /* Here check if the DB Download request or the Image
     * Download Request 
     */
    if (0 == strcmp(srcName, DB_HD_FILE_NAME)) {
        /* This is a HD DB Download and rebuild Request 
         * used by both BSC and NSC 
         */
        ret = dbmDumpDb2Sc(targetCard, ID_CMD_DNLD_HD_REBLD_DB);
        printf("IdSubTask(): DB(%s) Dnld for card(%d) Complete ret code = %d\n",
                    srcName, targetCard, ret);
    }
    else if (0 == strcmp(srcName, DB_BSC_RAM_AND_HD_FILE_NAME)) {
        /* This is a HD and RAM DB Download and rebuild Request
         * used by BSC only
         */
        ret = dbmDumpDb2Sc(targetCard, ID_CMD_ACTSYNC2STD_REBLD_BSC_DB);
        printf("IdSubTask(): DB(%s) Dnld for card(%d) Complete ret code = %d\n",
                    srcName, targetCard, ret);
    } else if (0 == strcmp(srcName, DB_NSC_DOWNLOAD_PRI_HD_RAM_FILE_NAME)) {
        /* This is a RAM and HD DB Download Request without the
         * DB rebuild used by NSC only
         */
        ret = dbmDumpDb2Sc(targetCard, ID_CMD_ACTSEC_SYNC_2STDPRI_NSC_DB);
        printf("IdSubTask(): DB(%s) Dnld for card(%d) Complete ret code = %d\n",
                    srcName, targetCard, ret);
    } else if (0 == strcmp(srcName, DB_NSC_DOWNLOAD_SEC_HD_RAM_FILE_NAME)) {
        /* This is a RAM and HD DB Download Request without the 
         * DB rebuild used by NSC only
         */
        ret = dbmDumpDb2Sc(targetCard, ID_CMD_ACTPRIMGRP_SYNC_2STDSEC_NSC_DB);
        printf("IdSubTask(): DB(%s) Dnld for card(%d) Complete ret code = %d\n",
                    srcName, targetCard, ret);
    } else if (0 == strcmp(srcName, DB_NSC_REBUILD_RAM_HD_FILE_NAME)) {
        /* This is a  DB rebuild Request when NSC switch over happens */
        ret = dbmDumpDb2Sc(targetCard, ID_CMD_REBLD_NSC_DB);
        printf("IdSubTask(): DB(%s) Dnld for card(%d) Complete ret code = %d\n",
                    srcName, targetCard, ret);
    } else if (0 == strcmp(srcName, DB_UPGD_RAM_HD_FILE_NAME)) {
        /* This is a  DB rebuild Request when NSC switch over happens */
        ret = dbmDumpDb2Sc(targetCard, ID_CMD_DNLD_HD_RAM_REBLD_UPGD);
        printf("IdSubTask(): DB(%s) Dnld for card(%d) Complete ret code = %d\n",
                    srcName, targetCard, ret);
    } else if (0 == strcmp(srcName, DB_NSC_DNLD_SYNC_HD_FILE_NAME)) {
        /* This is a  DB rebuild Request when NSC switch over happens */
        ret = dbmDumpDb2Sc(targetCard, ID_CMD_DNLD_HD_ACTPRIM_SYNC_2STDSEC_NSC_DB);
        printf("IdSubTask(): DB(%s) Dnld for card(%d) Complete ret code = %d\n",
                    srcName, targetCard, ret);
    } else if (0 == strcmp(srcName, DB_SYNCBACK_RAM_HD_FILE_NAME)) {
        /* This is a  DB rebuild Request when NSC switch over happens */
        ret = dbmDumpDb2Sc(targetCard, ID_CMD_STDSYNC2ACT_REBLD_DB);
        printf("IdSubTask(): DB(%s) Dnld for card(%d) Complete ret code = %d\n",
                    srcName, targetCard, ret);
    } 

    else {
        /* This is a non DB Download Request */
        ret = processDownloadReq(targetCard, srcName, dstName);
        printf("IdSubTask(): %s Download for card(%d) Complete ret code = %d\n",
                    srcName, targetCard, ret);
        if (ret < 0) {
            /* If failed once, let's try it again */
            ret = processDownloadReq(targetCard, srcName, dstName);
            printf("IdSubTask():%s Dnld 2nd Try for card(%d) Complete ret=%d\n",
                    srcName, targetCard,  ret);
        }
    }

    /* this is being allocated by ID task and passed to IdSubTask */
    SiwFreeMem(srcName);
    SiwFreeMem(dstName);

    /* Now send the message to the originator of the download
     * request
     */

    /* first of all get a message buffer */
    outMsg = 
	   (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("idMain(): ########  WARNING!!!!! ######\n");
	SiwTaskDelete(SIW_TID_SELF);
	exit(-1);
    }
    pOHdr = (SM_MSG_HDR_t *)&outMsg->data[0];

    if (ret < 0)
	pOHdr->msg = (U16_t)badMsg;
    else
	pOHdr->msg = (U16_t)goodMsg;
    pOHdr->srcSlot  = targetCard; /* card for which ID completed*/
    pOHdr->version = DEFAULT_VERSION;

    /* Send pointer to the data */
    itMsg.msgBufPtr = outMsg;
    itMsg.msgType = 0;
    itMsg.dstTaskId = TASK_SM;
    ret = SiwMsgQSend(MSGQ2SM, &itMsg, 
		      sizeof(MSG_BUF_HDR_t), WAIT_FOREVER, 
		      SIW_MSG_PRI_NORMAL);

    taskDelay(10);
    SiwTaskDelete(SIW_TID_SELF);
#endif /* FLASH_BOOT */
    exit(0);
}




/****************************************************************************
 * Function:    spawnIdsubTask()
 * Descripion:  This routine takes the slot number and programs the Cubit le 
 * In Parms:    MSG_t *in - Pointer to the input buffer 
 * Out Parms:   None 
 * Return Values:
 *              CM_OK      - If all works out OK
 *              CM_ERROR
 */
RET_t
spawnIdSubTask(U32_t targetCard, char *srcName, char *dstName, 
	       S32_t goodMsg, S32_t badMsg)
{
#ifndef FLASH_BOOT
    STATUS_t  taskStatus;
    SIW_PRIORITY_t pri;
    SIW_TASKINFO_t info;
    char name[40];

    SIW_FUNC_TRACE(ID, spawnIdSubTask, targetCard, srcName, dstName, 0,0);

    pri.applId     = SIW_PLATFORM_SW;
    pri.prioGroup  = SIW_PRI_GRP_NORMAL;
    pri.priority   = MRPC_TASK_PRI;

    info.stackSize = SIW_LARGE_STACK;
    info.entryPt = (FUNC_PTR_t)IdSubTask;
    info.arg1 = (U32_t)targetCard;
    info.arg2 = (U32_t)srcName;
    info.arg3 = (U32_t)dstName;
    info.arg4 = (U32_t)goodMsg;
    info.arg5 = (U32_t)badMsg;
    info.waitAck = 0;


    sprintf(name, "tId_%d", idSubTaskCnt++);

    if (idSubTaskCnt > 200)
	idSubTaskCnt = 0;

    FM_DEBUG("spawnIdSubTask(): task %s is getting spawned\n", name);

    shelf.slot[targetCard].cIdTid = SiwTaskSpawn(name, &pri, &info,
		       SIW_RECV_LOG_ERROR,
		       SIW_RECV_LOG_ERROR,
		       NULL,
		       &taskStatus);
    
    if (shelf.slot[targetCard].cIdTid == SIW_INVALID_TID ) {
	ID_TRACE5(spawnIdSubTask, 0xF0000001,shelf.slot[targetCard].cIdTid, taskStatus,0,0,0);
	return (ID_COULD_NOT_SPAWN_SUB_TASK);
    }
    if (taskStatus < 0) {
	SiwTaskDelete(shelf.slot[targetCard].cIdTid);
	return (ID_COULD_NOT_SPAWN_SUB_TASK);
    }
    printf("spawnIdSubTask(): TaskId for ID task for Slot[%d] is 0x%x\n", 
                  targetCard, (unsigned int)(shelf.slot[targetCard].cIdTid));
#endif /* FLASH_BOOT */
    return (ID_OK);
}



/****************************************************************************
 * Function:    IdMain()
 * Descripion:  This is the main entry point for Image Download (ID) task.
 *              This task is intended to run on SCC. It takes messages from    
 *              Chassis Manager and perform the download of an image to a card.
 * 	        It uses MRPC to send messages to File Manager to move image to 
 * 		a specified card.
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              Never Returns unless the world falls apart
 *            
 */
RET_t
IdMain(int a0, int a1, int a2, int a3, int a4, 
	 int a5, int a6, int a7, int a8, int a9)
{
#ifndef FLASH_BOOT
    MSG_t     *msg;
    S32_t     msgLen;
    ID_HDR_t  *pHdr;
    MSG_BUF_HDR_t itMsg;

    /* Initialize the internal data structures for FM */
    if (initId()) {
	/* Error case */ 
	FM_DEBUG ("Could not initialize ID \n");
	SiwTaskInitAck(ID_ERROR);
    }

    /* Now that all of the Internal structures for FM are initialized,
     * let the system services know of the state of FM task.
     */
    SiwTaskInitAck(ID_OK);

    while (1) {   /* Never ending loop starts here */

	/* See if we have any thing to do */
	if ( (msgLen = SiwMsgQReceive (MSGQ2ID, &itMsg, 
			    sizeof(MSG_BUF_HDR_t), WAIT_FOREVER)) <= 0) {
	    FM_DEBUG ("IdMain: error message queue receive\n");
	    continue; 
        }

	/* We got work to do, so proceed..... */

	msg = (MSG_t *)itMsg.msgBufPtr;
	/* First sanitize the message */
	if (msg == NULL) { 
	    /* Oppsss.. we got a NULL poiner; This is bad */
	    FM_DEBUG("IdMain():ID Task recevied a Message with NULL Buffer ptr\n");
	    /* SSSS ; What do in a situation like this ? */
	} else {
	    MSG_t  *outMsg;
	    FM_HDR_t *fmHdrP;
	    S32_t hdrLen, blockLen, fileLen;
	    S32_t i, numBlocks;
	    FD_t  *srcFdp, *destFdp;
	    S32_t targetCard;
	    RET_t ret;
	    char   *srcName, *dstName;

#if 0
	    char  srcImageFile[MAX_FILE_NAME_LENGTH];
	    char  dstImageFile[MAX_FILE_NAME_LENGTH];
#else
	    srcName=SiwAllocMem(SIW_DYNAMIC,MAX_FILE_NAME_LENGTH,WAIT_FOREVER);
	    dstName=SiwAllocMem(SIW_DYNAMIC,MAX_FILE_NAME_LENGTH,WAIT_FOREVER);
#endif
	    pHdr = ID_HDR_PTR(msg);

	    FM_DEBUG("IdMain(): Got Message(%x); srcCard(%x) srcTID(%p)\n", 
				       pHdr->msg, pHdr->srcSlot, pHdr->srcTid);
	    /* We have a valid message to process */
	    switch (pHdr->msg) {

	    case ID_CMD_DOWNLOAD_IMAGE_TO_RAM:
		/* First of all find out which card number to be downloaded */
		targetCard = pHdr->destSlot;

		/* 
		 * Now see what is the name of the image for this card, 
		 * i.e. the file name that needs to be picked up from the
		 * disc and moved down
		 */
#if 0
		srcName = &srcImageFile[0];
		dstName = &dstImageFile[0];
#endif /* 0 */
		if (getSrcImageFileName(targetCard, &srcName) != 0) {
		    /* We have a problem here. We can not even get the 
		     * file name associated with this file. This is a big
		     * No No...!!!! What to do in a case like this? 
		     * Send and error return back to the caller task
		     */
		    /* SSSSS */
		    continue;
		}
		if (getDstImageFileName(targetCard, &dstName) != 0) {
		    /* We have a problem here. We can not even get the 
		     * file name associated with this file. This is a big
		     * No No...!!!! What to do in a case like this? 
		     * Send and error return back to the caller task
		     */
		    /* SSSSS */
		    continue;
		}

		ret = spawnIdSubTask(targetCard, srcName, dstName,
		                     SM_MSG_IMAGE_DNLD_COMPLETE, 
				     SM_MSG_IMAGE_DNLD_FAILED);
		if (ret < 0) {
		    ID_TRACE5(IdMain, 0xF0000011,ret, 0,0,0,0);
		    FM_DEBUG("IdMain():Failed to spawn Id subTask RetCode(%x)\n", 
			    ret);
		} else {
		    ID_TRACE1(executeServerFunc, 0x02,ret, 0,0,0,0);
		    FM_DEBUG("IdMain(): Spawnd Id SubTask task(%x)\n", ret);
		}
		SiwFreeMem(msg);
		break;

	    case ID_CMD_DOWNLOAD_IMAGE_TO_FLASH:
		/* First of all find out which card number to be downloaded */
		targetCard = pHdr->destSlot;

		/* 
		 * Now see what is the name of the image for this card, 
		 * i.e. the file name that needs to be picked up from the
		 * disc and moved down
		 */
#if 0
		srcName = &srcImageFile[0];
		dstName = &dstImageFile[0];
#endif /* 0 */
		if (getSrcImageFileName(targetCard, &srcName) != 0) {
		    /* We have a problem here. We can not even get the 
		     * file name associated with this file. This is a big
		     * No No...!!!! What to do in a case like this? 
		     * Send and error return back to the caller task
		     */
		    /* SSSSS */
		    continue;
		}
		if (getDstFlashFileName(targetCard, &dstName) != 0) {
		    /* We have a problem here. We can not even get the 
		     * file name associated with this file. This is a big
		     * No No...!!!! What to do in a case like this? 
		     * Send and error return back to the caller task
		     */
		    /* SSSSS */
		    continue;
		}

		/* Open both local and remote files */
		ret = openLocalFile(srcName, &srcFdp);
		if (ret) 
		    continue;
		ret = openRemFile(targetCard, dstName, &destFdp);
		if (ret) 
		    continue;
		flashRec[targetCard].srcFdp = srcFdp;
		flashRec[targetCard].dstFdp = destFdp;

		/* The size of max payload that can be moved per message */
		blockLen = MAX_BUFF_LEN - sizeof (FM_HDR_t);

		/* Now read a buffer full of data including the
		 * header of the image file to see how big
		 * the file is that needs to be downloaded
		 */
		outMsg = 
		    (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
		if (!VALID_ADDRESS(outMsg)) {
		    /* Scream!!!!! We just ran out of buffers */
		    FM_DEBUG("idMain(): 2222 ########  WARNING!!!!! ######\n");
		    return(ID_ILLEGAL_ADDRESS);
		}
		fmHdrP = (FM_HDR_t *)&outMsg->data[0];
		hdrLen = FmRead( srcFdp, &outMsg->data[sizeof(FM_HDR_t)], 0, 
								blockLen);
		if (hdrLen <= 0 ) {
		    /* Oppsss... we have a problem reading the file.....
		     */
		    SiwFreeMem(outMsg);
		    return(ID_NO_DATA_READ);
		}
		flashRec[targetCard].nextAddrToRead = hdrLen;

		FM_DEBUG("Read %d bytes out of %d from src \n",hdrLen,blockLen);

		FM_DEBUG("Data Read:  ");
		for(i=0; i < 20; i++)
		    FM_DEBUG("%x ", outMsg->data[sizeof(FM_HDR_t)+i]);
		FM_DEBUG("\n");

		fileLen = 
		   ((IMAGE_FILE_HDR_t *)(&outMsg->data[sizeof(FM_HDR_t)]))->len;
		FM_DEBUG("File Length of src file is %d  \n",fileLen);

		if (fileLen <= 0) {
		    /* Ooppssss.... We have a problem, We just read
		     * bogus file. No need to proceed any further. 
		     */ 
		    SiwFreeMem(outMsg);
		    return(ID_LEN_OF_FILE_IS_BAD);
		}

		/* Now see how many mesasges it is going to take to ship 
		 * this file over
		 */
		numBlocks = 
			((S32_t)(fileLen - sizeof(IMAGE_FILE_HDR_t)))/blockLen;

		if (numBlocks < 0)
		    numBlocks = 0;
		/* Almost certainly, this is not going to be a exatly
		 * divisible number, so need to bump up the number of blocks
		 */
		if ((numBlocks*blockLen) < (S32_t)(fileLen - sizeof(IMAGE_FILE_HDR_t)))
		    numBlocks++;

		FM_DEBUG("It is going to take %d messages \n",numBlocks);
		flashRec[targetCard].totalBlocks = numBlocks;

		fmHdrP->msg = FM_WRITE_SERVICE;
		fmHdrP->srcModNum  = ID_MOD;
		fmHdrP->srcServNum = ID_WRITE_FLASH_NEXT_BLOCK;
		fmHdrP->arg1 = (U32_t)destFdp;
		fmHdrP->arg2 = sizeof(FM_HDR_t); /* Offset*/
		fmHdrP->arg3 = 0;
		fmHdrP->arg4 = hdrLen; 
		fmHdrP->arg5 = FALSE; 
#if 0 /* SSSSSS */
                SIW_SARBUFLEN_SET(outMsg, MAX_BUFF_LEN);
		ret = MrpcAsync(targetCard, FM_MOD, FM_WRITE_SERVICE, outMsg, 
			  GURANTEED, LO, 
			  (FUNC_PTR_t)flashCheckStatus, targetCard, 
			  &flashRec[targetCard].replyBuffer);
		if (ret < 0)
		    continue;
#else /* 0 SSSSSS */
                /* Set the Data Length */
                SIW_SARBUFLEN_SET(outMsg, MAX_BUFF_LEN);
		ret = MrpcAsync(targetCard, FM_MOD, FM_WRITE_SERVICE, outMsg, 
					      GURANTEED, LO, NULL, 0, NULL);
		if (ret < 0)
		    continue;
#endif /* 0 SSSSSS */

		flashRec[targetCard].nextAddrToWrite = hdrLen;
		flashRec[targetCard].blocksProcessed = 1;
		flashRec[targetCard].thisBlock = 1;
		flashRec[targetCard].blocksRemaining = numBlocks-1;

		FM_DEBUG("wrote %d bytes to dest file \n",hdrLen);
		/* SSSSS */ 
		/* Make sure that the data made it on to the other side*/

		break;

	    case ID_CMD_DOWNLOAD_NEXT_FLASH_BLOCK:

		/* Which slot are we dealing with ? */
		targetCard = pHdr->destSlot;

		/* see if we are done yet ? */
		if (flashRec[targetCard].blocksRemaining <= 0) {
		    /* We are done with the download, so now close
		     * the files 
		     */
		    closeBothFiles(targetCard, flashRec[targetCard].srcFdp, 
				   flashRec[targetCard].dstFdp);
		    break;
		}
		/* Here move on to the next block, i.e. send the next block */

		/* Read next bock */
		outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
		if (!VALID_ADDRESS(outMsg)) {
		    /* Scream!!!!! We just ran out of buffers */
		    FM_DEBUG("idMain(): 333 ########  WARNING!!!!! ######\n");
		    return(ID_ILLEGAL_ADDRESS);
		}
		fmHdrP = (FM_HDR_t *)&outMsg->data[0];
		hdrLen = FmRead(srcFdp, &outMsg->data[sizeof(FM_HDR_t)], 
			        flashRec[targetCard].nextAddrToRead, blockLen);
		if (hdrLen <= 0 ) {
		    /* Oppsss... we have a problem reading the file.....
		     */
		    SiwFreeMem(outMsg);
		    break;
		}
		flashRec[targetCard].nextAddrToRead += hdrLen;
		flashRec[targetCard].thisBlockLen = hdrLen;

		FM_DEBUG("Read %d bytes out of %d from src \n",hdrLen,blockLen);

		FM_DEBUG("Data Read:  ");
		for(i=0; i < 20; i++)
		    FM_DEBUG("%x ", outMsg->data[sizeof(FM_HDR_t)+i]);
		FM_DEBUG("\n");

		fmHdrP->msg = FM_WRITE_SERVICE;
		fmHdrP->srcModNum  = ID_MOD;
		fmHdrP->srcServNum = ID_WRITE_FLASH_NEXT_BLOCK;
		fmHdrP->arg1 = (U32_t)flashRec[targetCard].dstFdp;
		fmHdrP->arg2 = sizeof(FM_HDR_t); /* Offset*/
		fmHdrP->arg3 = flashRec[targetCard].nextAddrToWrite;
		fmHdrP->arg4 = hdrLen; 
		fmHdrP->arg5 = FALSE; 
                /* Set the Data Length */
                SIW_SARBUFLEN_SET(outMsg, MAX_BUFF_LEN);
		ret = MrpcAsync(targetCard, FM_MOD, FM_WRITE_SERVICE, outMsg, 
					      GURANTEED, LO, NULL, 0, NULL);

		if (ret < 0)
		    continue;
		flashRec[targetCard].nextAddrToWrite += hdrLen;
		flashRec[targetCard].blocksProcessed++;
		flashRec[targetCard].thisBlock++;
		flashRec[targetCard].blocksRemaining--;

		FM_DEBUG("wrote %d bytes to dest file \n",hdrLen);
		/* SSSSS */ 
		/* Make sure that the data made it on to the other side*/

		break;

	    case ID_CMD_DOWNLOAD_CFG_FILE:
		/* First of all find out which card number to be downloaded */
		targetCard = pHdr->destSlot;
		FM_DEBUG("IdMain(): Got Message to Download Cfg file to card %d\n",
				   targetCard);

		/* 
		 * Now see what is the name of the image for this card, 
		 * i.e. the file name that needs to be picked up from the
		 * disc and moved down
		 */
		strcpy((char *)srcName, 
		       (char *)shelf.slot[targetCard].cCfg.srcCfgFileName);
		sprintf(dstName, "card_%d.cfg", targetCard);

		ret = spawnIdSubTask(targetCard, srcName, dstName,
		                     SM_MSG_CFG_DOWNLOAD_DONE, 
				     SM_MSG_CFG_DOWNLOAD_FAILED);
		if (ret < 0) {
		    ID_TRACE5(IdMain, 0xF0000011,ret, 0,0,0,0);
		    FM_DEBUG("IdMain():Failed spawn Id CFG subTsk RetCode(%x)\n", 
			    ret);
		} else {
		    ID_TRACE1(executeServerFunc, 0x02,ret, 0,0,0,0);
		    FM_DEBUG("IdMain():Spawnd Id SubTsk task(%x) for CFG\n", ret);
		}
		SiwFreeMem(msg);
		break;
	    case ID_CMD_ACTSEC_SYNC_2STDPRI_NSC_DB:
	    case ID_CMD_ACTPRIMGRP_SYNC_2STDSEC_NSC_DB:
	    case ID_CMD_ACTSYNC2STD_REBLD_BSC_DB:
	    case ID_CMD_REBLD_NSC_DB:
            case ID_CMD_DNLD_HD_RAM_REBLD_UPGD:
            case ID_CMD_DNLD_HD_ACTPRIM_SYNC_2STDSEC_NSC_DB:
            case ID_CMD_STDSYNC2ACT_REBLD_DB:
	    case ID_CMD_DNLD_HD_REBLD_DB:
		/* First of all find out which card number to be downloaded */
		targetCard = pHdr->destSlot;
		FM_DEBUG("IdMain(): Got Msg to Download RAM & HD DB to card %d\n",
				   targetCard);
		/* 
		 * Now see what kind of DB download is for this card, 
                 * the srcName is a fake file name. we have to do it to 
                 * reuse the legacy code.
		 */
                if ( pHdr->msg == ID_CMD_ACTSEC_SYNC_2STDPRI_NSC_DB)  
                    strcpy((char *)srcName, DB_NSC_DOWNLOAD_PRI_HD_RAM_FILE_NAME);

                if ( pHdr->msg == ID_CMD_ACTPRIMGRP_SYNC_2STDSEC_NSC_DB)  
                    strcpy((char *)srcName, DB_NSC_DOWNLOAD_SEC_HD_RAM_FILE_NAME);

                if ( pHdr->msg == ID_CMD_ACTSYNC2STD_REBLD_BSC_DB)  
                    strcpy((char *)srcName, DB_BSC_RAM_AND_HD_FILE_NAME);

                if ( pHdr->msg == ID_CMD_REBLD_NSC_DB)
                    strcpy((char *)srcName, DB_NSC_REBUILD_RAM_HD_FILE_NAME);

                if ( pHdr->msg == ID_CMD_DNLD_HD_ACTPRIM_SYNC_2STDSEC_NSC_DB)
                    strcpy((char *)srcName, DB_NSC_DNLD_SYNC_HD_FILE_NAME);

                if ( pHdr->msg == ID_CMD_STDSYNC2ACT_REBLD_DB)
                    strcpy((char *)srcName, DB_SYNCBACK_RAM_HD_FILE_NAME);

                if ( pHdr->msg == ID_CMD_DNLD_HD_RAM_REBLD_UPGD)
                    strcpy((char *)srcName, DB_UPGD_RAM_HD_FILE_NAME);

                if ( pHdr->msg == ID_CMD_DNLD_HD_REBLD_DB)
                    strcpy((char *)srcName, DB_HD_FILE_NAME);

		dstName[0] = 0; /* To avoide unexpected memory wipeout */

		ret = spawnIdSubTask(targetCard, srcName, dstName,
		                     SM_MSG_CFG_DOWNLOAD_DONE, 
				     SM_MSG_CFG_DOWNLOAD_FAILED);
		if (ret < 0) {
		    ID_TRACE5(IdMain, 0xF0000111,ret, 0,0,0,0);
		    FM_DEBUG("IdMain():Failed spawn Id DB subTsk RetCode(%x)\n", 
			    ret);
		} else {
		    ID_TRACE1(executeServerFunc, 0x102,ret, 0,0,0,0);
		    FM_DEBUG("IdMain():Spawnd Id SubTsk task(%x) for DB\n", ret);
		}
		SiwFreeMem(msg);
		break;
	    case ID_RESP:

	    case ID_ACK:

	    case ID_NACK:

	    default:
		/* Unknown Massage */
		FM_DEBUG("IdMain(): ID received an unknown Msg(%d) \n", pHdr->msg);
		/* free up this unknown message and recover the memory */
		SiwFreeMem(msg);
	    } /* switch () */
	} /* else */
    } /* whle (1) */

    return (ID_ERROR);
#endif /* FLASH_BOOT */
	return (ID_OK);
}
