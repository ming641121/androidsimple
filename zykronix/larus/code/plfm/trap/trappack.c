/*
 * Copyright (C) 1998, All Rights Reserved, by
 * Ahoya Communications, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file contains the routine for TRAP task
 *
 * Note:
 *    - xxxx
 *
 * Author(s):
 *
 *     Su Ming
 */
/*
 *====================================================================
 *=                            include                               =
 *====================================================================
 */

#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "ioLib.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sockLib.h>
#include "axss_types.h"
#include "axss_if.h"
#include "sys_if.h"
#include "siw_if.h"
#include "sr_conf.h"
#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#undef SR_OFFSET /* must following method.h */
#include "objectdb.h"
#include "makevb.h"
#include "traprcd.h"
#include "traptype.h"
#include "trap_if.h"
#include "trap_events.h"

S32_t  trapFd = -1;

/*
 *====================================================================
 *=                         prototype
 *====================================================================
 */
U32_t TrapPackLen(U32_t trapNum);
typedef
struct _mmssnmptraps_t {
        SR_INT32        trapGlobalSeqNum;
        SR_INT32        trapSeverity;
        SR_INT32        trapReqSeqNum;
        SR_INT32        trapReqStartSeqNum;
        OctetString     *trapEventPhySlot;
        OctetString     *trapEventLogSlot;
        OctetString     *trapEventMessage;
#ifdef U_mmssnmptraps
U_mmssnmptraps
#endif /* U_mmssnmptraps */
        char            valid[1];
} mmssnmptraps_t;

/*
 *====================================================================
 *=                         external variables                       =
 *====================================================================
 */
extern mmssnmptraps_t trapScalarDb;
extern TRAP_TPLEN_RCD_t trapTpInfo[];
extern U32_t ttlTrapCnt;
#ifdef SCC
extern BOOL_t dbgAlrm;
#endif /* SCC */

/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */
  
/****************************************************************************
 * Function:    trapcmp()
 * Descripion:  This routine compare two U32 value
 * In Parms:    None
 * Out Parms:   None
 * Return Values: > 0
 *                  0
 *                < 0
 */

S32_t
trapcmp(const void *a, const void *b)
{
    return(*(U32_t *)(a) - *(U32_t *)(b));
}

/****************************************************************************
 * Function:    TrapTpTblInit()
 * Descripion:  This routine sort th snmp trap table
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */

void
TrapTpTblInit()
{
    SIW_FUNC_TRACE(TRM, TrapTpTblInit, 0, 0, 0, 0, 0);

    qsort(trapTpInfo, ttlTrapCnt, sizeof(TRAP_TPLEN_RCD_t), trapcmp);

    if (trapFd < 0)
    {
        trapFd = socket(AF_INET, SOCK_DGRAM, 0);
        if (trapFd < 0)
        {
            SiwEventLog(NULL, SIW_EVENT_SYM(TRM,SOCK_INIT_FAIL));
            SIW_DEBUG_TRACE(TRM, TrapObjTblInit, 0xF0000002, TRACE_LEVEL_1,
                0,0,0,0,0);
            return;
        }
    }
}

/****************************************************************************
 * Function:    TrapVerify(U32_t trapNum)
 * Descripion:  verified the existence of a trap number
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
STATUS_t
TrapVerify(U32_t trapNum)
{
    if (!bsearch((void *)&trapNum, trapTpInfo, 
        ttlTrapCnt, sizeof(TRAP_TPLEN_RCD_t), trapcmp))
        return(ERROR);
    return(OK);
}

/****************************************************************************
 * Function:    TrapRegTblGet(U32_t trapNum, void  **regTbl)
 * Descripion:  get the address of the tran reg table
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
STATUS_t
TrapRegTblGet(U32_t trapNum,
              void  **regTbl)
{
    pTRAP_TPLEN_RCD_t  trapTpRcdPtr;

    trapTpRcdPtr = (pTRAP_TPLEN_RCD_t)bsearch((void *)&trapNum, trapTpInfo, 
        ttlTrapCnt, sizeof(TRAP_TPLEN_RCD_t), trapcmp);

    if (trapTpRcdPtr == NULL)
    {
        return(ERROR);
    }
    *regTbl = (void *)&trapTpRcdPtr->regTbl;
    return(OK);

}


/****************************************************************************
 * Function:    TrapEventLog()
 * Descripion:  This routine generate trap event
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
STATUS_t
TrapEventLog(U32_t trapNum,
             int   trapType,
             int   shelf,
             int   slot,
             int   port,
             int   severity,
             U32_t *mibEntryPtr,
             SIW_EVENT_OBJ_t *event,
             ...)
{
    va_list ap;
    pTRAP_TPLEN_RCD_t  trapTpRcdPtr;
    pTRAP_DATA_TP_t    dataTpPtr;
    TRAP_HEADER_t *trapMsg;
    U32_t  datOffs;
    U8_t   *datBuf;
    U32_t  *packBuf;
    U32_t   i;
    U32_t   val;
    U32_t          curTime;
    SIW_EVENT_OBJ_t tmp_event;

    trapTpRcdPtr = (pTRAP_TPLEN_RCD_t)bsearch((void *)&trapNum, trapTpInfo, 
        ttlTrapCnt, sizeof(TRAP_TPLEN_RCD_t), trapcmp);

    if (trapTpRcdPtr == NULL)
        return(ERROR);

    trapMsg = SiwAllocMem(SIW_DYNAMIC, TrapPackLen(trapNum), 100);

    if (trapMsg == NULL)
        return(ERROR);


    packBuf = (U32_t *)((U8_t *)trapMsg + sizeof(TRAP_HEADER_t));
    datOffs = trapTpRcdPtr->numOfEntry * sizeof(U32_t);
    datBuf = (U8_t *)(packBuf + trapTpRcdPtr->numOfEntry);

    dataTpPtr = trapTpRcdPtr->dataTpPtr;

    for (i = 0; i < trapTpRcdPtr->numOfEntry; i++, 
         dataTpPtr++, packBuf++)
    {
        val = (mibEntryPtr + dataTpPtr->entryLoc);

        switch (dataTpPtr->type)
	    {
	      case TYPE_DATA_32:
            *packBuf = *(U32_t *)val;
            break;
	      case TYPE_STR:
            if (val)
	        {
                *packBuf = ((OctetString *)(val))->length << 16 | datOffs;
                memcpy(datBuf, ((OctetString *)(val))->octet_ptr, 
                  ((OctetString *)(val))->length);
	        }
            else
                *packBuf = datOffs;
            datOffs += ((dataTpPtr->maxLen + 3) & 0xfffffffc);
            datBuf += ((dataTpPtr->maxLen + 3) & 0xfffffffc);
            break;
	      case TYPE_OID:
            if (val)
	        {
                *packBuf = ((OID *)val)->length << 16 | datOffs;
                memcpy(datBuf, ((OID *)val)->oid_ptr, 
                    ((OID *)val)->length * sizeof(U32_t));
	        }
            else
                *packBuf = datOffs;
                datOffs += (dataTpPtr->maxLen * sizeof(U32_t));
                datBuf += (dataTpPtr->maxLen * sizeof(U32_t));
            break;
	    }
    }

    trapMsg->trapNumber = trapNum;
    trapMsg->trapSlot = slot;

    trapMsg->trapLength = datOffs + sizeof(TRAP_HEADER_t);
    trapMsg->severity = severity;
    trapMsg->catalog = trapTpRcdPtr->catalog;
    curTime = GetTimeNow();

    TrapToMgrs(severity, curTime, trapMsg); /* yshen */

    /* save info from event object */
    tmp_event.severity = event->severity;
    tmp_event.option = event->option;

    event->option = EVENT_TRAP_FLAG | trapNum;
	event->trapType = trapType;
	event->shelf = shelf;
	event->slotNo = slot;
	event->portNo = port;
	event->severity = severity;

    va_start(ap, event);
    val = SysEventLog((U8_t *)trapMsg, event, ap);
    va_end(ap);

    if ((int)val == ERROR)
        SiwFreeMem(trapMsg);

    /* restore info to event object */
    event->severity = tmp_event.severity;
    event->option = tmp_event.option;
    
    return(val);
}


/****************************************************************************
 * Function:    TrapPackLen()
 * Descripion:  This routine pack to trap to trap message
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
U32_t
TrapPackLen(U32_t trapNum)
{
    pTRAP_TPLEN_RCD_t  trapTpRcdPtr;
    pTRAP_DATA_TP_t    dataTpPtr;
    U32_t  len;
    U32_t  i;

    trapTpRcdPtr = (pTRAP_TPLEN_RCD_t)bsearch((void *)&trapNum, trapTpInfo, 
        ttlTrapCnt, sizeof(TRAP_TPLEN_RCD_t), trapcmp);

    if (trapTpRcdPtr == NULL)
        return(ERROR);

    len = trapTpRcdPtr->numOfEntry * sizeof(U32_t);

    dataTpPtr = trapTpRcdPtr->dataTpPtr;

    for (i = 0; i < trapTpRcdPtr->numOfEntry; i++, dataTpPtr++)
    {
        switch (dataTpPtr->type)
	{
	case TYPE_STR:
            len += ((dataTpPtr->maxLen + 3) & 0xfffffffc);
            break;
	case TYPE_OID:
            len += (dataTpPtr->maxLen * sizeof(U32_t));
            break;
	}
    }
    return(len+sizeof(TRAP_HEADER_t));
}

