
/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:This file contains common constants/typedefs needed by NWG tasks. 
 *
 *
 *Note:
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: nwg_if.h,v 1.3 2006/10/03 04:35:52 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/nwg_if.h,v $
 *====================================================================
 * $Log: nwg_if.h,v $
 * Revision 1.3  2006/10/03 04:35:52  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.2  2006/10/03 04:24:30  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/

#ifndef NWG_IF_H
#define NWG_IF_H



/***********************************************************************
 *  Error Codes
  ***********************************************************************/
#define NWG_EXCEED_SYSTEM_LIMIT       -3
#define NWG_ILLEGAL_FILE_NAME         -5
#define NWG_SET_REM_FLAG_FAIL         -9
#define NWG_SET_FLAG_FAIL             -10

/***********************************************************************
 *  Function Definitions
  ***********************************************************************/

/************************************************************
 *  Function: convert
 *  Description:  convert the .au and .wav file format to 
 *                Ahoya format
 *
 *  Input:    filename     - input file name  (e.g., test.au)
 *
 *  Output:   OK - success; ERROR - fail
 ************************************************************/
extern int convert (char *filename);
/*
 * Firmware image file Header Structure
 */
typedef struct fileHdr 
{
    S32_t   len;
    U32_t   offset;
    U32_t   cardType;
    U16_t   slot;
    U16_t   ckSum;
    char    version[MAX_VERSION_LENGTH];
    char    fileName[MAX_FILE_NAME_LENGTH];
    U16_t   pad;   /* pad to 68 byte boundary */
}FILE_HDR_t;

#endif /* NWG_IF_H */

