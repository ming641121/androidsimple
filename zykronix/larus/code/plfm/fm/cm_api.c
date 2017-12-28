/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file contains all API routines associated with Card Manager  
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
#include "siw_if.h"
#include "stdio.h"  /* for printf */
#include "lstLib.h" /* for linked List processing */
#include "ioLib.h"
#include "fm_if.h"
#include "id_if.h"
#include "cfg_if.h"
#include "cm_if.h"
#include "cm.h"
#include "id.h"


/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */

CFG_ENTRY_t cfg[MAX_SLOTS]; 


/*
 *====================================================================
 *=                         external variables                       =
 *====================================================================
 */
SIW_MQID_t    chmMsgQId;


/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */

/****************************************************************************
 * Function:    checkSum()
 * Descripion:  This routine computes the checksum of data starting from addr
 *              for len number of bytes
 * In Parms:    U32_t   *addr - Start Address 
 *              U32_t   len   - Lenght in Bytes (not Words!!!)
 * Out Parms:   None 
 * Return Values:
 *              CM_OK      - If all works out OK
 *              CM_ERROR
 */
U16_t 
checkSum(U32_t *addr, U32_t len)
{
    U32_t  ln = len >> 1;
    U16_t  *p = (U16_t *)addr;
    U32_t  sum = 0;
    U32_t i;

    for (i = 0; i < ln; i++) {
	sum += *p++;
    }
    while (sum & 0xffff0000)
	sum = (sum>>16) + (sum & 0xffff);
    
    return (~sum);

}

/****************************************************************************
 * Function:    CmComputeCkSumAndExecute()
 * Descripion:  This routine will compute the checksum of on the image and   
 *              if the checksum checks out to be OK, it will proceed with the
 *              the restart of the card. Otherwise, it will return error
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              CM_OK      - If all works out OK
 *              CM_ERROR
 */
void
CmComputeCkSumAndExecute()
{
    S32_t  imageLen;
    U32_t  ckSum;

    imageLen = ((IMAGE_FILE_HDR_t *)(GET_IMAGE_START_ADDR()))->len;

    ckSum = checkSum((U32_t *)GET_IMAGE_START_ADDR(), imageLen);

    if ((ckSum == 0) || (ckSum == 0xFFFF))  {
	/* 
	 * here if checksum checks out OK; Now jump to the start address and
	 * and start the Execution
	 */
	/* SSSS ; Need a macro to jump to given address */
    }
}


/****************************************************************************
 * Function:    CmSetDefaultCfg()
 * Descripion:  This routine sets up the default configration for AXSS_1600   
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              CM_OK      - If all works out OK
 *              CM_ERROR
 */
void
CmSetDefaultCfg()
{
    S32_t  i;

    /* 0 - 5 are NMC */
    for (i=0; i< 6; i++) {
	cfg[i].slotNum  = i;
	cfg[i].cardType = CARD_NMC;
	strcpy(cfg[i].srcImageFileName, "C:nmc.fw");
	strcpy(cfg[i].dstImageFileName, "fw_image");
	strcpy(cfg[i].dstFlashFileName, "fw_flash");
    }

    /* 6 - 7 are DIC */
    for (; i< 8; i++) {
	cfg[i].slotNum  = i;
	cfg[i].cardType = CARD_DIC;
	strcpy(cfg[i].srcImageFileName, "C:dic.fw");
	strcpy(cfg[i].dstImageFileName, "fw_image");
	strcpy(cfg[i].dstFlashFileName, "fw_flash");
    }

    /* 9 - 10 are SCC */
    for (; i< 10; i++) {
	cfg[i].slotNum  = i;
	cfg[i].cardType = CARD_SCC;
	strcpy(cfg[i].srcImageFileName, "C:scc.fw");
	strcpy(cfg[i].dstImageFileName, "fw_image");
	strcpy(cfg[i].dstFlashFileName, "fw_flash");
    }

    /* 11 - 15 are SCC */
    for (; i< 16; i++) {
	cfg[i].slotNum  = i;
	cfg[i].cardType = CARD_BSC;
	strcpy(cfg[i].srcImageFileName, "C:bsc.fw");
	strcpy(cfg[i].dstImageFileName, "fw_image");
	strcpy(cfg[i].dstFlashFileName, "fw_flash");
    }
}




/****************************************************************************
 * Function:    CmInitCard()
 * Descripion:  This routine Initializes a given card
 * In Parms:    U32_t card 
 * Out Parms:   None 
 * Return Values:
 *              CM_OK      - If all works out OK
 *              CM_ERROR
 */
RET_t
CmInitCard(U32_t card)
{
    RET_t ret;
    if (!VALID_SLOT(card))
	return(CM_ERROR);

    ret = FmCreate(cfg[card].dstImageFileName, READ_WRITE, RAM, 64000);
    ret |= FmCreate(cfg[card].dstFlashFileName, READ_WRITE, FLASH, 64000);
    return (ret);
}



