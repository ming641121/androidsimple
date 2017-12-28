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
 * $Id: trapobjs.h,v 1.2 2006/10/03 04:33:30 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/trap/trapobjs.h,v $
 *====================================================================
 * $Log: trapobjs.h,v $
 * Revision 1.2  2006/10/03 04:33:30  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/
#ifndef _TRAPOBJS_H_
#define _TRAPOBJS_H_

#include "traprcd.h"

typedef struct {
    OID        oid;
    ObjectInfo *objPtr;
    U8_t       keyInx;
    U8_t       objTblNum;
} TRAP_OBJ_t, *pTRAP_OBJ_t;


typedef struct {
    U32_t        trapNum;
    pTRAP_OBJ_t  objList;
    U32_t        numOfEntry;
    U8_t         *trapName;
    U8_t         resv;
} TRAP_OIDTBL_t, *pTRAP_OIDTBL_t;

#endif
