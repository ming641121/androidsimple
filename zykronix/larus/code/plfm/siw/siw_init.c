/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_init.c
 *
 *System Interface Wrapper code for initialization
 *
 *Note:
 *
 *Author(s):
 *
 */

#define DEFINE_SYS_GLOBALS

#include <vxWorks.h>
#include <taskHookLib.h>
#include <axss_types.h>
#include <siw_if.h>
#include <siw_trace.h>
#include <sys_os.h>
#include <sys_if.h>
#include <sys_memmap.h>

/***********************************************************************
 *  Function:     SiwInit()
 *  Description:  This function is used to initialize the System 
 *                Interface Wrapper environment before any SIW
 *                function are called. This function msut be called
 *                early in the boot process of the platform before any
 *                SIW objects are created.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      OK or ERROR if unable to init.
 **********************************************************************/
STATUS_t
SiwInit(void)
{
    U32_t  index;

    /* disable SIW Functional Trace */
    TraceDebugSet("SIW", TRACE_LEVEL_1, 0, FALSE);
   
    /* Create semaphore to protect task table */
    sysTaskTblSem = semMCreate(SEM_DELETE_SAFE);
    if ( sysTaskTblSem == NULL )
    {
        return(ERROR);
    }

    /* Initialize fields of Task table necessary */
    for ( index=0; index<MAX_TASK_ENTRIES; index++ )
    {
        sysTaskTable[index].siwTID = SIW_INVALID_TID;
        sysTaskTable[index].parent = SIW_INVALID_TID;
    } /* end for index */

    /* Create semaphore to protect msgQ table */
    sysMsgQTblSem = semMCreate(SEM_DELETE_SAFE);
    if ( sysMsgQTblSem == NULL )
    {
        return(ERROR);
    }

    /* Initialize fields of MsgQ table necessary */
    for ( index=0; index<MAX_MSGQ_ENTRIES; index++ )
    {
        sysMsgQTable[index].siwMqId = SIW_INVALID_MQID;
    } /* end for index */

    /* Create semaphore to protect semaphore table */
    sysSemTblSem = semMCreate(SEM_DELETE_SAFE);
    if ( sysSemTblSem == NULL )
    {
        return(ERROR);
    }

    /* Initialize fields of semaphore table necessary */
    for ( index=0; index<MAX_SEM_ENTRIES; index++ )
    {
        sysSemTable[index].siwSemId = SIW_INVALID_SEMID;
    } /* end for index */

    /* Create semaphore to protect timer table */
    sysTmrTblSem = semMCreate(SEM_DELETE_SAFE);
    if ( sysTmrTblSem == NULL )
    {
        return(ERROR);
    }

    for ( index=0; index<MAX_TMR_ENTRIES; index++ )
    {
        sysTmrTable[index].siwTimerId = SIW_INVALID_TMRID;
    }

    /* Add task delete hook */
    if ( taskDeleteHookAdd(SysTaskDelete) == ERROR )
    {
        return(ERROR);
    } /* end if taskDeleteHookAdd(sysTaskDelete) == ERROR */

    return(OK);
}
