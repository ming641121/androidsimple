#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ioLib.h>
#include <axss_types.h>
#include <sys_if.h>
#include <siw_if.h>
#if 0
STATUS_t
TrapEventLog(U32_t trapNum, S32_t shelf, S32_t slot, S32_t port,
			 S32_t severity, U32_t *mibEntryPtr, SIW_EVENT_OBJ_t *event,
             ...);

/****************************************************************************
 * Function:    TrapEventLog()
 * Descripion:  This routine generate trap event
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
STATUS_t
TrapEventLog(U32_t trapNum,
             S32_t shelf,
			 S32_t slot,
			 S32_t port,
			 S32_t severity,
             U32_t *mibEntryPtr,
             SIW_EVENT_OBJ_t *event,
             ...)
{
    va_list ap;
#if 0
    pTRAP_TPLEN_RCD_t  trapTpRcdPtr;
    pTRAP_DATA_TP_t    dataTpPtr;
    U32_t  datOffs;
    U8_t   *datBuf;
    U32_t  *packBuf;
	U32_t   i;
	S8_t *trapMsg;
#endif
    U32_t   val;
    SIW_EVENT_OBJ_t tmp_event;

#if 0
    trapTpRcdPtr = (pTRAP_TPLEN_RCD_t)bsearch((void *)&trapNum, trapTpInfo, 
        ttlTrapCnt, sizeof(TRAP_TPLEN_RCD_t), trapcmp);

    if (trapTpRcdPtr == NULL)
        return(ERROR);

    trapMsg = SiwAllocMem(SIW_DYNAMIC, 1024, 100);

    if (trapMsg == NULL)
        return(ERROR);

    packBuf = (U32_t *)((U8_t *)trapMsg + sizeof(TRAP_HEADER_t));
    datOffs = trapTpRcdPtr->numOfEntry * sizeof(U32_t);
    datBuf = (U8_t *)(packBuf + trapTpRcdPtr->numOfEntry);

    dataTpPtr = trapTpRcdPtr->dataTpPtr;

    for (i = 0; i < trapTpRcdPtr->numOfEntry; i++, 
         dataTpPtr++, packBuf++)
    {
        val = *(mibEntryPtr + dataTpPtr->entryLoc);
        switch (dataTpPtr->type)
	{
	case TYPE_DATA_32:
            *packBuf = val;
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
    trapMsg->trapSlot = SiwMyPhySlotNo();
#ifdef SCC
    if (dbgAlrm)
        printf("\n!!!!!!!!TrapEventLog with trapSlot=%d\n", 
            trapMsg->trapSlot);
#endif /* SCC */

    trapMsg->trapLength = datOffs + sizeof(TRAP_HEADER_t);
    trapMsg->severity = trapTpRcdPtr->severity;
    trapMsg->catalog = trapTpRcdPtr->catalog;
#endif

    /* save info from event object */
    tmp_event.severity = event->severity;
    tmp_event.option = event->option;
#if 0
    event->severity = trapTpRcdPtr->severity;
#endif
    event->option = EVENT_TRAP_FLAG | trapNum;
	event->shelf = shelf;
	event->slotNo = slot;
	event->portNo = port;
	event->severity = severity;
 
    va_start(ap, event);
    val = SysEventLog(NULL, event, ap);
    va_end(ap);

#if 0
    if ((int)val == ERROR)
        SiwFreeMem(trapMsg);
#endif

    /* restore info to event object */
    event->severity = tmp_event.severity;
    event->option = tmp_event.option;
    
    return(val);
}
#endif

#ifdef BOOT
STATUS_t CsmTask()
{
    return OK;
}

STATUS_t amTask()
{
    return OK;
}
#endif