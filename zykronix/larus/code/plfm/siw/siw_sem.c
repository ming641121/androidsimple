/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_sem.c
 *
 *System Interface Wrapper code for Semaphore
 *
 *Note:
 *
 *Author(s):
 * 
 */

#include <objLib.h>
#include <intLib.h>
#include <memLib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <sysSymTbl.h>
#include <axss_types.h>
#include <axss_if.h>
#include <siw_if.h>
#include <sys_os.h>
#include <sys_if.h>
#include "siw_events.h"
#include "siw_trace.h"
#include "siw_event.h"

typedef struct {
  int   taskId;
  int   type;
  U32_t tick;
  int   recursive;
  STATUS_t status;
} SEM_TRACE_t;

#define SEM_TRACE_SIZE  5000
SEM_TRACE_t semTraceTable[SEM_TRACE_SIZE];
BOOL_t semTraceEnable = TRUE;
int    semTraceId = 0x21;
int    semTraceTableIndex = 0;
S32_t  semTraceCount = 0x7fffffff;
U32_t  debugSemId = MAX_SEM_ENTRIES + 1;
BOOL_t semDebugEnable = FALSE;

void SiwSemTableShow(void);
void SiwSemIdSet(U32_t siwSemId);
void SiwSemIdReset();

void
prtSemTrace (S32_t idx, S32_t cnt)
{
    S32_t i;
    SEM_TRACE_t *ptr = NULL;

    for (i=0; i < cnt; i++) {
        ptr = &semTraceTable[idx];
	printf("%d TaskName[%s] Time[%d] recursive[%d] stats[%d] type[%d]\n", idx, 
			taskName(ptr->taskId), ptr->tick, ptr->recursive, ptr->status, ptr->type);
        idx++;
        if (idx >= SEM_TRACE_SIZE)
            idx = 0;
    }
}

/***********************************************************************
 *  Function:    SiwSemCreate()
 *  Description: This function creates a semaphore of the specified
 *               type, options, and initial state. There are three
 *               different types of semaphores - binary, counting,
 *               and mutual exclusion.  The option parameter for
 *               binary and counting semaphores specify how the
 *               waiting tasks are queued by either FIFO or priority.
 *               The option parameter for mutual exclusion semaphores
 *               can also specify if the owning task is deletion safe
 *               or if priority inversion safe. The initState parameter
 *               indicates the initial state of a binary or counting
 *               semaphore. The deadlockAction is used to specify the
 *               action to be taken if a deadlock has been detected in
 *               trying to take a semaphore. The function also takes
 *               an optional semName which must be a stable string
 *               buffer and not a string created on the stack. If no
 *               name is desired then NULL should be passed for semName.
 *               The SIW_SEMID_t is returned for the created semaphore
 *               or SIW_INVALID_SEMID if resources are not available
 *               for the semaphore.
 *
 *  In Parms     char * semName - unique name for the semaphore
 *               SIW_SEM_TYPE_t semType - semaphore type to create
 *               SIW_SEM_OPTION_t option - options to set on semaphore
 *               S32_t initState - initial state of binary or counting
 *                                 semaphore
 *               SIW_RECV_ACTION_t deadlockAction - action to be taken when
 *                                        deadlock condition is detected
 *  Out Parms:   None
 *  Returns:     A valid SIW_SEMID_t or SIW_INVALID_SEMID
 **********************************************************************/
SIW_SEMID_t
SiwSemCreate(char *            semName,
             SIW_SEM_TYPE_t    semType,
             SIW_SEM_OPTION_t  option,
             S32_t             initState,
             SIW_RECV_ACTION_t deadlockAction)
{
    STATUS_t        rc;
    BOOL_t          found = FALSE;
    S32_t           count = 0;
    SIW_SEMID_t     semId = SIW_INVALID_SEMID;
    SYS_SEM_ENTRY_t *semTblPtr = NULL;
    
    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwSemCreate, semName, semType, option, initState,
                   deadlockAction);
        
    /* Get semaphore to search table for available entry */
    rc = semTake(sysSemTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwSemCreate");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "Sem",
                        sysSemTblSem, errno);
        }
        return(SIW_INVALID_SEMID);
    }

    /* Search semaphore table for available entry */
    while ( found != TRUE && count < MAX_SEM_ENTRIES )
    {
        /* Get next entry pointer */
        semTblPtr = &sysSemTable[sysNextSemid];
        
        /* Check if next entry is available */
        if ( semTblPtr->siwSemId == SIW_INVALID_SEMID )
        {
            found = TRUE;
            semId = (SIW_SEMID_t)sysNextSemid;
            semTblPtr->siwSemId = semId;
        }

        /* Increment next available semid and count checked */
        sysNextSemid++;
        count++;

        /* Check if at end of semaphore table */
        if ( sysNextSemid >= MAX_SEM_ENTRIES )
        {
            sysNextSemid = SYS_SEMID_START_VALUE;
        }
    }

    /* Check if not found */
    if ( found != TRUE )
    {
        /* Release table semaphore */
        semGive(sysSemTblSem);

        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMTBLFULL), MAX_SEM_ENTRIES);
        return(SIW_INVALID_SEMID);
    }

    /* Initialize semTable with parameters */
    if ( semName != NULL )
    {
        strncpy(&semTblPtr->semPara.semName[0], semName,
                SIW_SEM_NAME_SIZE - 1);
        semTblPtr->semPara.semName[SIW_SEM_NAME_SIZE - 1] = '\0';
    }
    else
    {
        semTblPtr->semPara.semName[0] = '\0';
    }
    semTblPtr->semPara.semType = semType;
    semTblPtr->semPara.option = option;
    semTblPtr->semPara.initState = initState;
    semTblPtr->semPara.deadlockAction = deadlockAction;
    
    semId = SysSemCreate(semId);

    /* Assign result to entry to be reused */
    semTblPtr->siwSemId = semId;

    /* Release table semaphore */
    semGive(sysSemTblSem);
    
    return(semId);
}

/***********************************************************************
 *  Function:    SiwSemDelete()
 *  Description: This function deletes the specified semaphore and
 *               frees the resources used by it. Any pended tasks on the
 *               semaphore will unblock and return ERROR.
 *
 *  In Parms:    SIW_SEMID_t semId - semaphore to delete
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwSemDelete(SIW_SEMID_t semId)
{
    STATUS_t        rc;
    U32_t           semIndex = (U32_t)semId;
    SEM_ID          vxSemId;
    SYS_SEM_ENTRY_t *semTblPtr;

    /* Check that semId is in range */
    if ( semIndex >= MAX_SEM_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        return(ERROR);
    }

    /* Get table entry pointer */
    semTblPtr = &sysSemTable[semIndex];
    
    /* Check that semId has correct tag value */
    if ( semId != semTblPtr->siwSemId )
    {
        /* Check if could be deleted siwSemId */
        if ( (U32_t)semId < sysNextSemid )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDDELETED), semId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        }

        return(ERROR);
    }

    /* Get semaphore to delete entry */
    rc = semTake(sysSemTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwSemDelete");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "Sem",
                        sysSemTblSem, errno);
        }
        return(ERROR);
    }

    /* Invalidate siwSemId */
    semTblPtr->siwSemId = SIW_INVALID_SEMID;
   
#if 0 
    /* Check if owned by deleting task */
    if ( semTblPtr->tTask != SiwTaskIdSelf() &&
         semTblPtr->tTask != SIW_INVALID_TID &&
         (semTblPtr->semPara.semType != SIW_SEM_BINARY ||
          semTblPtr->semPara.initState != SIW_SEM_EMPTY))
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMDELUNOWN), semId, semTblPtr->tTask);
    }
#endif

    /* Clean up semaphore table entry */
    vxSemId = semTblPtr->semId;
    semTblPtr->semId = NULL;
    semTblPtr->cTask = SIW_INVALID_TID;
    semTblPtr->tTask = SIW_INVALID_TID;
    semTblPtr->ltTask = SIW_INVALID_TID;
    strncpy(&semTblPtr->semPara.semName[0], taskName(taskIdSelf()), SIW_SEM_NAME_SIZE - 1);

    /* Release table semaphore */
    semGive(sysSemTblSem);

    /* Delete VxWorks semaphore */
    if (vxSemId != NULL)
    {
        rc = semDelete(vxSemId);
    }

    /* Check delete status */
    if ( rc == ERROR )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMDELFAIL), vxSemId, semId, errno);
    }
    
    return(rc);
}

/***********************************************************************
 *  Function:    SiwSemTake()
 *  Description: This function takes a semaphore if available. The
 *               state of the semaphore is decremented in binary and
 *               counting semaphores.
 *
 *  Inp Parms:   SIW_SEMID_t semId - semaphore ID to take
 *               S32_t timeout   - amount of time in ticks to wait to take
 *                                 semaphore
 *  Out Parms:   None
 *  Returns:     OK, ERROR, or TIMEOUT
 **********************************************************************/
STATUS_t
SiwSemTake(SIW_SEMID_t semId,
           S32_t       timeout)
{
#define WAIT_HERE   100000
    STATUS_t        rc;
    U32_t           semIndex = (U32_t)semId;
    SYS_SEM_ENTRY_t *semTblPtr = NULL;
    SIW_TID_t       takeTask;
    U32_t           startTime;
    U32_t           endTime;
    int             tmOut;
    
    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwSemTake, semId, timeout, 0, 0, 0);
        
    /* Check that semId is in range */
    if ( semIndex >= MAX_SEM_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        return(ERROR);
    }

    /* Get table entry pointer */
    semTblPtr = &sysSemTable[semIndex];
    
    /* Check that semId has correct tag value */
    if ( semId != semTblPtr->siwSemId )
    {
        /* Check if could be deleted siwSemId */
        if ( (U32_t)semId < sysNextSemid )
        {
            /* Log warning event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDDELETED), semId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        }

        return(ERROR);
    }

    /* Get time in ticks to take */
    startTime = CYCLES();

if (semIndex == debugSemId)
{
    printf("Task(%s) try to take Semaphore(%p)\n", taskName(taskIdSelf()), semId);
}

    /* Take semaphore */
    if (timeout == WAIT_FOREVER)
    {
        for (;;)
        {
            rc = semTake(semTblPtr->semId, WAIT_FOREVER);
            if (rc == OK)
            {
                break;
            }
            else
            {
                SiwTaskDelay (1);
                while (semTblPtr->semId == NULL)
                {
                    SiwTaskDelay (1);
                }
            }
        }
        
if ((semTraceEnable) && (semIndex == (U32_t)semTraceId) && (semTraceCount-- > 0))
{
    SEM_TRACE_t *ptr = &semTraceTable[semTraceTableIndex++];

    ptr->taskId = taskIdSelf();
    ptr->type = 1;
    ptr->tick = SiwTickGet();
    ptr->recursive = semTblPtr->tRecurse;
    ptr->status = rc;
    if (semTraceTableIndex >= SEM_TRACE_SIZE)
        semTraceTableIndex = 0;
}
    }
    else
    {
#if 0
        int tid = taskIdSelf();

        if ((taskNameToId("tDBM") == tid) ||
            (taskNameToId("tPingT") == tid) ||
            (taskNameToId("tDBMSYNC") == tid) )
#else
        WIND_TCB *tcbPtr = NULL;
        SIW_TID_t tmrFlag = 0;

        /* Get TCB entry of current task */
        tcbPtr = (WIND_TCB *)taskIdSelf();

        /* Get siwTID from spare field */
        tmrFlag = (SIW_TID_t)tcbPtr->spare2;
 
        if (!tmrFlag)
#endif  /* 0 */
        {
            rc = semTake(semTblPtr->semId, timeout);
        }
        else
        {
            tmOut = 0;
            for (;;)
            {
                if (semTblPtr->semId == NULL)
                {
                    semTblPtr->siwSemId = (SIW_SEMID_t)99999;
                    return (ERROR);
                }
                rc = semTake(semTblPtr->semId, NO_WAIT);
                if (rc == ERROR)
                {
                    if (tmOut >= timeout)
                    {
                        break;
                    }
                    else
                    {
                        tmOut++;
                        SiwTaskDelay(1);
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }

    /* Check status */
    if ( rc == ERROR )
    {

if (semIndex == debugSemId)
{
    printf("Timeout(%d) for taking semTake(%p)\n", timeout, semId);
}

        /* Check errno timeout */
        if ( errno == S_objLib_OBJ_TIMEOUT ||
             errno == S_objLib_OBJ_UNAVAILABLE )
        {
            /* the semaphore could be in deadlock */
            switch ( semTblPtr->semPara.deadlockAction ) {
              case SIW_RECV_RESTART_TASK :
                  /* Log error event */
                  SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMDLKRESTAR), 
                          semTblPtr->semId, semId);
                  break;

              case SIW_RECV_SOFT_RESET :
                  /* Log error event */
                  SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMDLKRESET), 
                          semTblPtr->semId, semId);

                  /* suspend task */
                  taskSuspend(taskIdSelf());
                  break;

              case SIW_RECV_LOG_ERROR :
              case SIW_RECV_NO_ACTION :
                  /* Log error event */
                  SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMDLKLOGERR), 
                          semTblPtr->semId, semId);
                  break;

              case SIW_SEM_RESET:
                  /* delete semaphore */
                  SiwSemDelete(semId);

                  /* create semaphore */
                  SysSemCreate(semId);

                  /* free semaphore */
                  semGive(semTblPtr->semId);

                  /* take semaphore */
                  return (semTake(semTblPtr->semId, timeout));

              default :
            }
            return(TIMEOUT);
        }

        /* Check if not ISR Callable */
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwSemTake");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMTAKEFAIL), semTblPtr->semId,
                     semId, errno);
        }
        
        return(ERROR);
    }

if (semIndex == debugSemId)
{
    printf("Semaphore(%p) is taken by task(%s)\n", 
            semId, taskName(taskIdSelf()));
}
    /* Update statistics */
    endTime = CYCLES();
    takeTask = SiwTaskIdSelf();

    if ( semTblPtr->semPara.semType == SIW_SEM_MUTUAL_EXCLUSION &&
         takeTask == semTblPtr->tTask )
    {
        if ((semTblPtr->tRecurse > 0) && (semIndex == (U32_t)semTraceId))
        {
            semTraceCount = 0;
            taskSuspend(taskIdSelf());
        }
        semTblPtr->tRecurse++;
    }
    else
    {
        semTblPtr->tTask = takeTask;
        semTblPtr->tTicks = endTime;
    }

    semTblPtr->tCount++;
    if (endTime > startTime)
    {
        semTblPtr->dTicks = (semTblPtr->dTicks + endTime - startTime) >> 1;
    }

    return(OK);
}

/***********************************************************************
 *  Function:    SiwSemGive()
 *  Description: This function gives the semaphore to allow it to be
 *               taken by whatever task is waiting.
 *
 *  In Parms:    SIW_SEMID_t semId - semaphore id to give
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwSemGive(SIW_SEMID_t semId)
{
    STATUS_t        rc;
    U32_t           semIndex = (U32_t)semId;
    SYS_SEM_ENTRY_t *semTblPtr;
    U32_t cycles;


    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwSemGive, semId, 0, 0, 0, 0);
        
    /* Check that semId is in range */
    if ( semIndex >= MAX_SEM_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        return(ERROR);
    }

    /* Get table entry pointer */
    semTblPtr = &sysSemTable[semIndex];
    
    /* Check that semId has correct tag value */
    if ( semId != semTblPtr->siwSemId )
    {
        /* Check if could be deleted siwSemId */
        if ( (U32_t)semId < sysNextSemid )
        {
            /* Log warning event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDDELETED), semId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        }

        return(ERROR);
    }

    /* Update statistics */
    if ( semTblPtr->semPara.semType == SIW_SEM_MUTUAL_EXCLUSION &&
         semTblPtr->tRecurse != 0 )
    {
        semTblPtr->tRecurse--;
    }
    else
    {
        semTblPtr->ltTask = semTblPtr->tTask;
        semTblPtr->tTask = SIW_INVALID_TID;
        cycles = CYCLES();
        if (cycles > semTblPtr->tTicks)
        {
            semTblPtr->hTicks = (semTblPtr->hTicks + cycles - semTblPtr->tTicks) >> 1;
        }
    }

if (semIndex == debugSemId)
{
    printf("Semaphore(%p) is given by task(%s)\n", 
            semId, taskName(taskIdSelf()));
}

    /* Give VxWorks semaphore */
    rc = semGive(semTblPtr->semId);
if ((semTraceEnable) && (semIndex == (U32_t)semTraceId) && (semTraceCount-- > 0))
{
    SEM_TRACE_t *ptr = &semTraceTable[semTraceTableIndex++];

    ptr->taskId = taskIdSelf();
    ptr->type = 2;
    ptr->tick = SiwTickGet();
    ptr->recursive = semTblPtr->tRecurse;
    ptr->status = rc;
    if (semTraceTableIndex >= SEM_TRACE_SIZE)
        semTraceTableIndex = 0;
}
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE ) {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwSemGive");
        }
        else if ( errno == S_semLib_INVALID_OPERATION )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMGIVEUNOWN), semId,
                      semTblPtr->tTask);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMGIVEFAIL), semTblPtr->semId,
                     semId, errno);
        }
    }

    return(rc);
}

/***********************************************************************
 *  Function:    SiwSemGiveForce()
 *  Description: This function gives the semaphore to allow it to be
 *               taken by whatever task is waiting.
 *
 *  In Parms:    SIW_SEMID_t semId - semaphore id to give
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwSemGiveForce(SIW_SEMID_t semId)
{
    STATUS_t        rc;
    U32_t           semIndex = (U32_t)semId;
    SYS_SEM_ENTRY_t *semTblPtr;
    U32_t cycles;


    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwSemGive, semId, 0, 0, 0, 0);
        
    /* Check that semId is in range */
    if ( semIndex >= MAX_SEM_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        return(ERROR);
    }

    /* Get table entry pointer */
    semTblPtr = &sysSemTable[semIndex];
    
    /* Check that semId has correct tag value */
    if ( semId != semTblPtr->siwSemId )
    {
        /* Check if could be deleted siwSemId */
        if ( (U32_t)semId < sysNextSemid )
        {
            /* Log warning event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDDELETED), semId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        }

        return(ERROR);
    }

    /* Update statistics */
    if ( semTblPtr->semPara.semType == SIW_SEM_MUTUAL_EXCLUSION &&
         semTblPtr->tRecurse != 0 )
    {
        semTblPtr->tRecurse--;
    }
    else
    {
        semTblPtr->ltTask = semTblPtr->tTask;
        semTblPtr->tTask = SIW_INVALID_TID;
        cycles = CYCLES();
        if (cycles > semTblPtr->tTicks)
        {
            semTblPtr->hTicks = (semTblPtr->hTicks + cycles - semTblPtr->tTicks) >> 1;
        }
    }

if (semIndex == debugSemId)
{
    printf("Semaphore(%p) is given by task(%s)\n", 
            semId, taskName(taskIdSelf()));
}

    /* Give VxWorks semaphore */
    if ( semTblPtr->semPara.semType == SIW_SEM_MUTUAL_EXCLUSION )
        rc = semMGiveForce(semTblPtr->semId);
    else
        rc = semGive(semTblPtr->semId);

if ((semTraceEnable) && (semIndex == (U32_t)semTraceId) && (semTraceCount-- > 0))
{
    SEM_TRACE_t *ptr = &semTraceTable[semTraceTableIndex++];

    ptr->taskId = taskIdSelf();
    ptr->type = 2;
    ptr->tick = SiwTickGet();
    ptr->recursive = semTblPtr->tRecurse;
    ptr->status = rc;
    if (semTraceTableIndex >= SEM_TRACE_SIZE)
        semTraceTableIndex = 0;
}
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE ) {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwSemGive");
        }
        else if ( errno == S_semLib_INVALID_OPERATION )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMGIVEUNOWN), semId,
                      semTblPtr->tTask);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMGIVEFAIL), semTblPtr->semId,
                     semId, errno);
        }
    }

    return(rc);
}

/***********************************************************************
 *  Function:    SiwSemFlush()
 *  Description: This function unblocks every task pending on the
 *               semaphore.  This function is illegal for mutual
 *               exclusion semaphores.
 *
 *  Inp Parms:   SIW_SEMID_t semId - semaphore id to flush
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwSemFlush(SIW_SEMID_t semId)
{
    STATUS_t        rc;
    U32_t           semIndex = (U32_t)semId;
    SYS_SEM_ENTRY_t *semTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwSemFlush, semId, 0, 0, 0, 0);
        
    /* Check that semId is in range */
    if ( semIndex >= MAX_SEM_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        return(ERROR);
    }

    /* Get table entry pointer */
    semTblPtr = &sysSemTable[semIndex];
    
    /* Check that semId has correct tag value */
    if ( semId != semTblPtr->siwSemId )
    {
        /* Check if could be deleted siwSemId */
        if ( (U32_t)semId < sysNextSemid )
        {
            /* Log warning event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDDELETED), semId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        }

        return(ERROR);
    }

    /* Flush VxWorks semaphore */
    rc = semFlush(semTblPtr->semId);
    if ( rc == ERROR )
    {
        if ( errno == S_semLib_INVALID_OPERATION )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMFLUSHINV), semId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMFLUSHFAIL), semTblPtr->semId,
                     semId, errno);
        }
    }

    return(rc);
}

/***********************************************************************
 *  Function Name:  SysSemCreate
 *
 *  Description: This function actually creates the semaphore that has
 *               been registered in the semaphore table. The static
 *               parameters area of the table entry must be filled in
 *               before calling this function. The SIW_SEMID in field
 *               siwSemId being used for the entry must also be set in
 *               the table entry before calling this function. The
 *               parameters are what the SIW_SEMID_t that is being created
 *               and where the function is being called from in the
 *               code. This information is stored to indicate what code
 *               created the semaphore.
 *
 *  In Parms:   SIW_SEMID_t tblSemId - table index that is to create
 *                                     semaphore
 *  Returns:    A valid SIW_SEMID_t or SIW_INVALID_SEMID if failure
 **********************************************************************/
SIW_SEMID_t
SysSemCreate(SIW_SEMID_t tblSemId)
{
    U32_t           semIndex = (U32_t)tblSemId;
    SEM_ID          semId;
    SYS_SEM_ENTRY_t *semTblPtr = NULL;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SysSemCreate, tblSemId, 0, 0, 0, 0);
        
    /* Get table entry */
    semTblPtr = &sysSemTable[semIndex];

    /* Validate Deadlock action */
    switch ( semTblPtr->semPara.deadlockAction ) {
        case SIW_RECV_NO_ACTION :
        case SIW_RECV_LOG_ERROR :
        case SIW_RECV_RESTART_TASK :
        case SIW_RECV_SOFT_RESET :
        case SIW_SEM_RESET:
            break; /* case SIW_RECV_NO_ACTION ... */
        default :
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMRECVINV),
                     semTblPtr->semPara.deadlockAction);
            return(SIW_INVALID_SEMID);
    }

    /* Switch on semaphore type */
    switch ( semTblPtr->semPara.semType ) {
        case SIW_SEM_BINARY :
            semId = semBCreate(semTblPtr->semPara.option,
                               (SEM_B_STATE)semTblPtr->semPara.initState);
            break; /* case SIW_SEM_BINARY */
        case SIW_SEM_COUNT :
            semId = semCCreate(semTblPtr->semPara.option,
                               semTblPtr->semPara.initState);
            break; /* case SIW_SEM_COUNT */
        case SIW_SEM_MUTUAL_EXCLUSION :
            semId = semMCreate(semTblPtr->semPara.option);
            break; /* case SIW_SEM_MUTUAL_EXCLUSION */
        default :
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMTYPEINVALID),
                     semTblPtr->semPara.semType);
            return(SIW_INVALID_SEMID); /* default */
    }

    /* Check results of create */
    if ( semId == NULL )
    {
        if ( errno == S_semLib_INVALID_OPTION ||
             errno == S_semLib_INVALID_QUEUE_TYPE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMOPTINVALID),
                     semTblPtr->semPara.option,
                     semTblPtr->semPara.semType);
        }
        else if ( errno == S_semLib_INVALID_STATE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMSTATEINVALID),
                     semTblPtr->semPara.initState);
        }
        else if ( errno == S_memLib_NOT_ENOUGH_MEMORY )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, OBJCREATENOMEM), "Semaphore");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMCREATEFAIL), errno);
        }
        
        return(SIW_INVALID_SEMID);
    }

    /* Set semId in table entry */
    semTblPtr->semId = semId;
    semTblPtr->siwSemId = tblSemId;
    
    /* Initialize task creator value */
    semTblPtr->cTask = SiwTaskIdSelf();
    semTblPtr->cTime = time(NULL);

    /* Initialize other table fields */
    semTblPtr->recvFunc = NULL;
    semTblPtr->tTask = SIW_INVALID_TID;
    semTblPtr->tRecurse = 0;
    semTblPtr->ltTask = SIW_INVALID_TID;
    semTblPtr->tCount = 0;
    semTblPtr->tTicks = 0;
    semTblPtr->dTicks = 0;
    semTblPtr->hTicks = 0;

    return(tblSemId);
}

/***********************************************************************
 *  Function:    SiwSemShow()
 *  Description: This function show information about the specified
 *               semaphore. The level is used to specify the level of
 *               detail to be displayed. A level of 0 specifies that
 *               summary information is to be displayed, while level 1
 *               specifies that detailed information is to be displayed.
 *
 *  Inp Parms:   SIW_SEMID_t semId - semaphore to display information
 *               S32_t level - level of information to display
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwSemShow(SIW_SEMID_t  semId,
           S32_t        level)
{
    STATUS_t        rc = OK;
    U32_t           semIndex = (U32_t)semId;
    char            tmpstr[MAX_SYS_SYM_LEN + 1];
    char            *strPtr;
    struct tm       timeValue;
    SYS_SEM_ENTRY_t *semTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwSemShow, semId, level, 0, 0, 0);
        
    /* Check that semId is in range */
    if ( semIndex >= MAX_SEM_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        return(ERROR);
    } 

    /* Get table entry pointer */
    semTblPtr = &sysSemTable[semIndex];
    
    /* Check that semId has correct tag value */
    if ( semId != semTblPtr->siwSemId )
    {
        /* Check if could be deleted siwSemId */
        if ( (U32_t)semId < sysNextSemid )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDDELETED), semId);
        } 
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, SEMIDINVALID), semId);
        }

        return(ERROR);
    } 

    /* Call semShow */
    if ( level == 0 || level == 1 )
    {
        printf("\nSIW_SEMID           : %p", semId);  
             
        rc = semShow(semTblPtr->semId, level);
    }

    /* Display creation and current take status */
    printf("\nName            Creation Time         Task\n");
    localtime_r(&semTblPtr->cTime, &timeValue);
    strftime(&tmpstr[0], MAX_SYS_SYM_LEN, "%m/%d/%Y %H:%M:%S",
                 &timeValue);
    strPtr = SiwTaskName(semTblPtr->cTask);

    if ( strPtr != NULL )
    {
        printf("%-15.15s %19.19s   %s\n\n", 
                semTblPtr->semPara.semName, &tmpstr[0], strPtr);
    }
    else
    {
        printf("%-15.15s %19.19s   %p\n\n", 
                semTblPtr->semPara.semName, &tmpstr[0], semTblPtr->cTask);
    } 

    /* Display Performance stats */
    printf("Taken(%u)  --  AveHoldTime(%u)  --  AveWaitTime(%u)\n",
            semTblPtr->tCount, semTblPtr->hTicks, semTblPtr->dTicks);
    printf("# of Recursive Taken: %d\n", (int)semTblPtr->tRecurse);

    return(rc);
}


void SiwSemTableShow()
{
    int              i;
    SYS_SEM_ENTRY_t *semTblPtr = NULL;

    /* Search semaphore table for available entry */
    for (i=0; i< MAX_SEM_ENTRIES; i++)
    {
        /* Get next entry pointer */
        semTblPtr = &sysSemTable[i];
        
        /* Check if next entry is available */
        if ( semTblPtr->siwSemId != SIW_INVALID_SEMID )
        {
            if (SiwTaskName(semTblPtr->cTask) != NULL)
            {
                printf("Sem ID: %4.4d is taken by task: %s\n", 
                                   i, SiwTaskName(semTblPtr->cTask));
            }
            else
            {
                printf("Sem ID: %4.4d is free\n", i);
            }
        }
    }
}

void SiwSemIdSet(U32_t siwSemId)
{
    debugSemId = siwSemId;
}

void SiwSemIdReset()
{
    debugSemId = MAX_SEM_ENTRIES + 1;
}

/***********************************************************************
 *  Function:    SysSemReturn()
 *  Description: This function returns all semaphores that are taken
 *               by a deleted task.
 *
 *  In Parms:    SIW_TID_t tid - SIW Task Id
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t SysSemReturn(SIW_TID_t tid)
{
    int              i;
    int              tidIndex = (int)tid;
    SYS_SEM_ENTRY_t *semTblPtr = NULL;

    /* validation check */
    if (tid == SIW_INVALID_TID || tidIndex > MAX_TASK_ENTRIES)
    {
        return (ERROR);
    }

    /* Search semaphore table for available entry */
    for (i=1; i< MAX_SEM_ENTRIES; i++)
    {
        /* Get next entry pointer */
        semTblPtr = &sysSemTable[i];
        
        /* Check if next entry is available */
        if ( semTblPtr->siwSemId != SIW_INVALID_SEMID )
        {
            /* check if the Task own the Semaphore */
            if (semTblPtr->tTask == tid)
            {
                SiwSemDelete((SIW_SEMID_t)i);
                SysSemCreate((SIW_SEMID_t)i);
                if (semTblPtr->semId != NULL)
                {
                    semGive(semTblPtr->semId);
                }
            }
        }
    }

    return (OK);
}

