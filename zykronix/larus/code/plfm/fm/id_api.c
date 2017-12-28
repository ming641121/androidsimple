/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file contains all routines associated with Image Downloader  
 *    services offered by AXSS-1600 infrastructure.
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
#include "mrpc_if.h"
#include "stdio.h"  /* for printf */
#include "sysLib.h"
#include "fm_if.h"
#include "fm.h"
#include "id_if.h"
#include "id.h"
#include "id_events.h"


/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */



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


/****************************************************************************
 * Function:    IdImageDownload()
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
IdImageDownload(IN S32_t slot, ID_MEM_TYPE_t  mem)
{
    RET_t    ret;
    ID_HDR_t *pHdr;
    MSG_t    *outMsg;
    MSG_BUF_HDR_t itMsg;

    SIW_FUNC_TRACE(ID, IdImageDownload,slot, mem, 0, 0, 0);

    if (!VALID_SLOT(slot))  {
	return (ID_ILLEGAL_SLOT);
    }
    if (!VALID_MEM_TYPE(mem))  {
	return (ID_ILLEGAL_MEM_TYPE);
    }

    /* This download request needs to be forwarded to the ID Task.
     * Therefore, prepare a message for the ID Task and send it over.
     * Also embed enough information in the message so that ID task can
     * send a response back to the caller of this routine.
     */

    /* first of all get a message buffer */
    outMsg = (MSG_t *)SiwAllocMem(SIW_STATIC, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg))  {
	return (FM_ILLEGAL_ADDRESS);
    }
    pHdr = (ID_HDR_t *)&outMsg->data[0];
    if (mem == RAM_MEM) {
	pHdr->msg = ID_CMD_DOWNLOAD_IMAGE_TO_RAM;
    } else {
	if (mem == FLASH_MEM) {
	    pHdr->msg = ID_CMD_DOWNLOAD_IMAGE_TO_FLASH;
	} else {
	    pHdr->msg = NULL;
	}
    }
    pHdr->srcSlot  = SiwMyPhySlotNo(); 
    pHdr->destSlot = slot;
    pHdr->srcMsgQId  = MSGQ2SM; /* Shelf Manaer Q Id */
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





/****************************************************************************
 * Function:    IdWriteFlashNextBlock()
 * Descripion:  This routine is called by the File Manager via MRPC Async call
 *              to let the ID manager know that it has written a block of
 *              of data into the Flash, so that ID task can send the next
 *              block of data. This function is used as a synchronization
 *              mechanism between the remote FM task and local ID task.
 * In Parms:    MSG_t *inMsg      - pointer to message buffer
 * Out Parms:   MSG_t **outMsg    - pointer to reply buffer
 * Return Values:
 *              ID_OK      - If all works out OK
 *              ID_ERROR
 */
RET_t 
IdWriteFlashNextBlock(IN MSG_t *inMsg, OUT MSG_t **replyMsg)
{
    RET_t    ret;
    FM_HDR_t *pFHdr, *pOutHdr;
    ID_HDR_t *pIHdr;
    S32_t    slot;
    MSG_t    *outMsg;
    MSG_BUF_HDR_t itMsg;

    SIW_FUNC_TRACE(ID, IdWriteFlashNextBlock, inMsg, replyMsg, 0, 0, 0);

    if (!VALID_ADDRESS(inMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to 
	 */
	*replyMsg = NULL;
	SiwFreeMem(inMsg);
	return (ID_ILLEGAL_ADDRESS);
    }

    /* get FM header address; so that we can see what's up */
    pFHdr = FM_HDR_PTR(inMsg);

    /* Get the slot number */
    slot = pFHdr->srcSlot;

    if (!VALID_SLOT(slot))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to 
	 */
	*replyMsg = NULL;
	SiwFreeMem(inMsg);
	return (ID_ILLEGAL_SLOT);
    }

    /* See if the remote side is reporting an error */
    if (pFHdr->err != FM_OK) {
	/* Since the last block had an error, so adjust the read pointer
	 * and number of processed blocks so that previous block can be
	 * re-sent
	 */
	flashRec[slot].nextAddrToRead -= flashRec[slot].thisBlockLen;
	flashRec[slot].nextAddrToWrite -= flashRec[slot].thisBlockLen;
	flashRec[slot].blocksProcessed--;
	flashRec[slot].blocksRemaining++;
	FM_DEBUG(">>>>>>>>Error Block Reported by Rem side %x\n", pFHdr->err);
    }

    FM_DEBUG("IdWriteFlashNextBlock(): got reply target card = %d\n", slot);
    /* first of all get a message buffer */
    outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    pIHdr = (ID_HDR_t *)&outMsg->data[0];
    pIHdr->msg = ID_CMD_DOWNLOAD_NEXT_FLASH_BLOCK;
    pIHdr->srcSlot  = SiwMyPhySlotNo();
    pIHdr->destSlot = slot;
    pIHdr->srcMsgQId  = MSGQ2ID; /* ID task Q Id */
    pIHdr->srcTid   = SiwTaskIdSelf();
    pIHdr->err      = 0;
    
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

    /* Now get a response buffer */
    *replyMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    pOutHdr = FM_HDR_PTR(*replyMsg);

    if (ret < 0) {
	PACK_FM_ERR_MSG(pOutHdr, ret);
    } else {
	PACK_FM_ACK_MSG(pOutHdr);
    }

    return (ret);
}


/****************************************************************************
 * Function:    IdCfgDownload()
 * Descripion:  This routine is used to download Configuration file in any of 
 *              the cards in AXSS-1600.
 *              This routine sends a message to ID task to perform such
 *              function. 
 * In Parms:    S32_t slot         - The slot number of the card to be dnloaded
 * Out Parms:   None 
 * Return Values:
 *              ID_OK      - If all works out OK
 *              ID_ERROR
 */
S32_t
IdCfgDownload(IN S32_t slot)
{
    RET_t    ret;
    ID_HDR_t *pHdr;
    MSG_t    *outMsg;
    MSG_BUF_HDR_t itMsg;

    SIW_FUNC_TRACE(ID, IdCfgDownload, slot, 0, 0, 0, 0);

    if (!VALID_SLOT(slot))  {
	return (ID_ILLEGAL_SLOT);
    }

    /* This download request needs to be forwarded to the ID Task.
     * Therefore, prepare a message for the ID Task and send it over.
     * Also embed enough information in the message so that ID task can
     * send a response back to the caller of this routine.
     */

    /* first of all get a message buffer */
    outMsg = (MSG_t *)SiwAllocMem(SIW_STATIC, sizeof(MSG_t), WAIT_FOREVER);
    pHdr = (ID_HDR_t *)&outMsg->data[0];
    pHdr->msg = ID_CMD_DOWNLOAD_CFG_FILE;
    pHdr->srcSlot  = SiwMyPhySlotNo(); 
    pHdr->destSlot = slot;
    pHdr->srcMsgQId  = MSGQ2SM; /* Shelf Manaer Q Id */
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




/****************************************************************************
 * Function:    IdDbmDownloadDb()
 * Descripion:  This routine is used to download Configuration file in any of 
 *              the cards in AXSS-1600.
 *              This routine sends a message to ID task to perform such
 *              function. 
 * In Parms:    S32_t slot         - The slot number of the card to be dnloaded
 * Out Parms:   None 
 * Return Values:
 *              ID_OK      - If all works out OK
 *              ID_ERROR
 */
S32_t
IdDbmDownloadDb(IN S32_t slot, U32_t ramSync)
{
    RET_t    ret;
    ID_HDR_t *pHdr;
    MSG_t    *outMsg;
    MSG_BUF_HDR_t itMsg;

    SIW_FUNC_TRACE(ID, IdDbmDownloadDb, slot, ramSync, 0, 0, 0);

    if (!VALID_SLOT(slot))  {
	return (ID_ILLEGAL_SLOT);
    }

    /* This download request needs to be forwarded to the ID Task.
     * Therefore, prepare a message for the ID Task and send it over.
     * Also embed enough information in the message so that ID task can
     * send a response back to the caller of this routine.
     */

    /* first of all get a message buffer */
    outMsg = (MSG_t *)SiwAllocMem(SIW_STATIC, sizeof(MSG_t), WAIT_FOREVER);
    pHdr = (ID_HDR_t *)&outMsg->data[0];
    pHdr->msg = ramSync;
    pHdr->srcSlot  = SiwMyPhySlotNo(); 
    pHdr->destSlot = slot;
    pHdr->srcMsgQId  = MSGQ2SM; /* Shelf Manaer Q Id */
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
