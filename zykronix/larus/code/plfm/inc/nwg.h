/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *	This file is a local header file for Networking Group 
 *
 *Note:
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: nwg.h,v 1.2 2006/10/03 04:24:30 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/nwg.h,v $
 *====================================================================
 * $Log: nwg.h,v $
 * Revision 1.2  2006/10/03 04:24:30  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/

#ifndef _NWG_H_
#define _NWG_H_

/***********************************************************************
 *  Defines for MRPC service ID
 ***********************************************************************/
#define NWG_FLASH_UPDATE      1
#define NWG_UPDATE_KEY        2
#define NWG_FLASH_ERASE       3


/***********************************************************************
 *  Definition 
 ***********************************************************************/
#define NWG_OK                0
#define NWG_ERROR            -1

#define ACTIVATE_ANMT_FILE    1
#define DEACTIVATE_ANMT_FILE  0


#define MAX_BASE_NAME_SIZE    16  /* plus ".wav" is 20 */


/***********************************************************************
 *  Error Codes
 ***********************************************************************/
#define NWG_INVALID_PSWD              -2

#define ONE_MEG_FLASH         1024*1024
#define FLASH_BANK0_START     0xbfc00000
#define FLASH_BANK1_START     0xbfd00000


#define NWG_FLASH_UPDATE_HDR  sizeof(NWG_FLASH_UPDATE_t)

typedef struct
{
    U32_t    Slot;
    U32_t    Length;
    U32_t    Offset;
    U32_t    BankId;
} NWG_FLASH_UPDATE_t;

typedef struct
{
    U32_t    retStatus;
} NWG_FLASH_UPDATE_ACK_t;

typedef struct
{
    U32_t    BankId;
} NWG_FLASH_ERASE_t;

typedef struct
{
    U32_t    retStatus;
} NWG_FLASH_ERASE_ACK_t;

#if 0
typedef struct
{
    U8_t     Key [TFTP_KEY_SIZE];
} NWG_UPDATE_KEY_t;

typedef struct
{
    U32_t    retStatus;
} NWG_UPDATE_KEY_ACK_t;
#endif

#endif
