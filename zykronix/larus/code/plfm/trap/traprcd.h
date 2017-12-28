/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *
 *Note:
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: traprcd.h,v 1.2 2006/10/03 04:33:30 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/trap/traprcd.h,v $
 *====================================================================
 * $Log: traprcd.h,v $
 * Revision 1.2  2006/10/03 04:33:30  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/
#ifndef _TRAPRCD_H_
#define _TRAPRCD_H_
#include "trapdef.h"

#define TYPE_DATA_32  0
#define TYPE_STR      1
#define TYPE_OID      2

#define MAJOR        2
#define MINOR        3
#define INFO         4

typedef struct {
    U8_t  type;
    U8_t  entryLoc;
    U16_t  maxLen;
} TRAP_DATA_TP_t, *pTRAP_DATA_TP_t;


typedef struct {
    U32_t          trapNum;
    pTRAP_DATA_TP_t dataTpPtr;
    void           *regTbl;
    U32_t          numOfEntry;
    U32_t          severity; 
    U32_t          catalog;
} TRAP_TPLEN_RCD_t, *pTRAP_TPLEN_RCD_t;

#endif
