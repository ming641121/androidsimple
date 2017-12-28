/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *    This header file contains the API for the Image Downloader. All APIs
 *    are prototyped in this file.
 *
 *
 *Note:
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: id_if.h,v 1.2 2006/10/03 04:24:30 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/id_if.h,v $
 *====================================================================
 * $Log: id_if.h,v $
 * Revision 1.2  2006/10/03 04:24:30  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/

#ifndef ID_IF_H
#define ID_IF_H
/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */

/* 
 * Following are the services published and offered by the ID Manager
 */
/* NOTE: The following service is offered only for Download of Flash 
 *       and move the state machine of the ID task to the next block.
 */
#define  ID_WRITE_FLASH_NEXT_BLOCK    1
#define  SC_ENHANCEMENT               1 /* after this version of software
                                           is deployed, this flag should
                                           be turned off */


/* 
 * Following  is the list of all possible error codes returned by the
 * fm module API. 
 */
#define ID_OK                                    0
#define ID_ERROR                                -1
#define ID_ILLEGAL_MEM_TYPE                     -2
#define ID_ILLEGAL_SLOT                         -3
#define ID_COULD_NOT_OPEN_SRC_FILE              -4
#define ID_COULD_NOT_OPEN_DST_FILE              -5
#define ID_NO_DATA_READ                         -6
#define ID_LEN_OF_FILE_IS_BAD                   -7
#define ID_ILLEGAL_ADDRESS                      -8
#define ID_COULD_NOT_CREATE_DST_FILE            -9
#define ID_COULD_NOT_SPAWN_SUB_TASK             -10
#define ID_MRPC_TIMEOUT                         -11

/*
 * following are the messages for the ID Task
 */
#define ID_CMD_DOWNLOAD_IMAGE_TO_RAM                       1
#define ID_CMD_DOWNLOAD_IMAGE_TO_FLASH                     2
#define ID_CMD_DOWNLOAD_NEXT_FLASH_BLOCK                   3
#define ID_RESP                                            4
#define ID_ACK                                             5
#define ID_NACK                                            6
#define ID_CMD_DOWNLOAD_CFG_FILE                           7

/* keep this one as same as before. This constant is used
  * when a BSC and NSC comes up as an active card
  */
#define ID_CMD_DNLD_HD_REBLD_DB                              8

/* When a primary BSC comes up as a standby card, download 
 * sync and rebuild RAM and HD DB
 */ 
#define ID_CMD_ACTSYNC2STD_REBLD_BSC_DB           9

/* when a secondary NSC is at the preparing upgrade stage,
 * after the card gets reset, download and rebuild
 * the HD and RAM DB to this secondary card
 */ 
#define ID_CMD_DNLD_HD_RAM_REBLD_UPGD                             10

/* when a primary NSC card comes up as a standby card,
 * download and no rebuild the RAM and HD DB from the 
 * active secondary card. 
 */
#define ID_CMD_ACTSEC_SYNC_2STDPRI_NSC_DB             11

/* when a secondary NSC comes up as a standby card,
 * download and no-rebuild RAM and HD DB from all
 * primary cards.  
 */
#define ID_CMD_ACTPRIMGRP_SYNC_2STDSEC_NSC_DB             12

/* When a NSC witch-over or switch-back happens, rebuild
 * DBs 
 */
#define ID_CMD_REBLD_NSC_DB                     13

/* when a NSC card becomes active, if the secondary card
 * is in standby state already , then download the HD DB
 * for this primary card to standby NSC card.
 */
#define ID_CMD_DNLD_HD_ACTPRIM_SYNC_2STDSEC_NSC_DB                         14 

/* when a active secondary card is reset (by issueing
 * resetcd) sync DB back from the standby primary 
 * card has to be done  
 */
#define ID_CMD_STDSYNC2ACT_REBLD_DB       15 

/* 
 * following are to be used as file types only 
 */
typedef enum memType
{
    RAM_MEM  = 1, 
    FLASH_MEM
} ID_MEM_TYPE_t;



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
 * Descripion:  This routine is used to download Firmware image to a remote
 *              card. The image can either be downloaded into RAM or FLASH.
 * In Parms:    S32_t  slot    - The slot to be downloaded.
 *              MEM_TYPE_t mem - type of memory to be downloaded, i.e.
 *                                   RAM_MEMORY, FLASH_MEMORY
 * Out Parms:   None 
 * Return Values:
 *              ID_OK     - If all works out OK
 *              ID_ERROR
 *              ID_ILLEGAL_SLOT
 *              ID_ILLEGAL_MEM_TYPE
 *              or any of the error codes defined at the top of this file
 */
extern 
RET_t IdImageDownload(IN S32_t slot, ID_MEM_TYPE_t mem );


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
extern 
RET_t IdWriteFlashNextBlock(IN MSG_t *inMsg, OUT MSG_t **outMsg);

/****************************************************************************
 * Function:    IdCfgDownload()
 * Descripion:  This routine is used to download config file to a remote
 *              card.
 * In Parms:    S32_t  slot    - The slot to be downloaded.
 * Out Parms:   None 
 * Return Values:
 *              ID_OK     - If all works out OK
 *              ID_ERROR
 *              ID_ILLEGAL_SLOT
 *              or any of the error codes defined at the top of this file
 */
extern 
RET_t IdCfgDownload(IN S32_t slot);
extern 
RET_t IdDbmDownloadDb(IN S32_t slot, U32_t ramSync);


#endif /* ID_IF_H */
