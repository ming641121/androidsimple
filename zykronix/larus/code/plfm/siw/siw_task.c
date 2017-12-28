/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_task.c
 *
 *System Interface Wrapper code for Tasks
 *
 *Note:
 *
 *Author(s):
 * 
 */

#include <stdio.h>
#include <string.h>
#include <taskLib.h>
#include <intLib.h>
#include <objLib.h>
#include <memLib.h>
#include <axss_types.h>
#include <siw_if.h>
#include <sys_os.h>
#include <sys_if.h>
/*#include <sys_piodefs.h> */
#include "siw_events.h"
#include "siw_trace.h"
#include "siw_event.h"

#define SIW_TASK_DELETE    0x7F7F7F7F

/***********************************************************************
 *  Local static function declaration
 **********************************************************************/
BOOL_t siwTaskDebug = FALSE;
int    sysLockTaskId = 0;
int    siwLockTaskId = 0;
int    siwLockKey;
char   *siwLockTaskName = NULL;

LOCAL int siwTaskSpawnOpt = 0;   /* taskSpawn() options parameter */

LOCAL STATUS_t SysTaskSetup(SIW_TID_t, FUNCPTR, S32_t, S32_t, S32_t, S32_t,
                            S32_t, S32_t, S32_t, S32_t);
extern SIW_SEMID_t SysWaitSemIdGet();

/***********************************************************************
 *  Function:    SiwTaskSpawn()
 *  Description: This function spawns a task with the indicated name,
 *               priority, start information, and monitor actions.
 *               This function will wait for an acknowledgement from
 *               the spawned task.  The initialization status of the
 *               child spawned is returned in the child status output
 *               parameter. If the child status is ERROR, then the child
 *               is suspended. If the child status is TIMEOUT, then the
 *               waiting for init acknowledge timed out. The value returned
 *               is an SIW_TID_t or SIW_INVALID_TID if a failure occurs.
 *
 *  In Parms:    char *name - upto 10 chars
 *               SIW_PRIORITY_t *taskPriority - define application id, 
 *                                     priority group, and priority within
 *                                     group
 *               SIW_TASKINFO_t *taskInfo - define stacksize, entry point,
 *                                     and arguments
 *               SIW_RECV_ACTION_t suspendAction - action taken by task 
 *                                     monitor when it detects that task has
 *                                     suspended or disappeared
 *               SIW_RECV_ACTION_t failAction - action taken by task monitor 
 *                                     if the suspend action fails
 *               SIW_RECVFUNC_t recvFunc - recovery function
 *  Out Parms:   STATUS_t *childStatus - status value returned by
 *                                     initialization of child if wait for
 *                                     task ack init
 *  Returns:     A SIW_TID_t or SIW_INVALID_TID on error
 **********************************************************************/
SIW_TID_t
SiwTaskSpawn(char             *name,
             SIW_PRIORITY_t   *taskPriority,
             SIW_TASKINFO_t   *taskInfo,
             SIW_RECV_ACTION_t suspendAction,
             SIW_RECV_ACTION_t failAction,
             SIW_RECVFUNC_t    recvFunc,
             STATUS_t          *childStatus)
{
    SIW_TID_t        taskId = SIW_INVALID_TID;
    BOOL_t           found  = FALSE;
    S32_t            count  = 0;
    SYS_TASK_ENTRY_t *taskTblPtr = NULL;
    STATUS_t         rc;
    int              i;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskSpawn, 0, 0, 0, 0, 0);
    
    /* Get semaphore to search table for available entry */
    rc = semTake(sysTaskTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwTaskSpawn");
        } 
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "Task",
                        sysTaskTblSem, errno);
        } 
        return(SIW_INVALID_TID);
    }

    for (i=0; i<10; i++)
    {
        /* Search task table for available entry */
        while ( found != TRUE && count < MAX_TASK_ENTRIES )
        {
            /* Get next entry pointer */
            taskTblPtr = &sysTaskTable[sysNextTid];
        
            /* Check if next entry is available */
            if ( taskTblPtr->siwTID == SIW_INVALID_TID )
            {
                found = TRUE;
                taskId = (SIW_TID_t)sysNextTid;
                taskTblPtr->siwTID = taskId;
            } 

            /* Increment next available taskid and count checked */
            sysNextTid++;
            count++;

            /* Check if at end of task table */
            if ( sysNextTid >= MAX_TASK_ENTRIES )
            {
                sysNextTid = SYS_TID_START_VALUE;
            } 
        }
        if ( found == TRUE )
        {
            break;
        }
        taskDelay(2);
    } 

    /* Check if not found */
    if ( found != TRUE )
    {
        /* Release table semaphore */
        semGive(sysTaskTblSem);

        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKTBLFULL), MAX_TASK_ENTRIES);

        return(SIW_INVALID_TID);
    }

    /* Initialize taskTable with parameters */
    strncpy(&taskTblPtr->taskPara.name[0], name, 11);
    taskTblPtr->taskPara.name[11] = '\0';
    memcpy(&taskTblPtr->taskPara.taskPriority, taskPriority,
           sizeof(SIW_PRIORITY_t));
    memcpy(&taskTblPtr->taskPara.taskInfo, taskInfo,
           sizeof(SIW_TASKINFO_t));
    taskTblPtr->taskPara.suspendAction = suspendAction;
    taskTblPtr->taskPara.failAction = failAction;
    taskTblPtr->taskPara.recvFunc = recvFunc;
    taskTblPtr->tmrCntr = 0;

    /* Call SysTaskSpawn with id */
    taskId = SysTaskSpawn(taskId);

    /* Assign result to entry to be reused */
    taskTblPtr->siwTID = taskId;

    /* Release table semaphore */
    semGive(sysTaskTblSem);

    /* Check if task was spawned */
    if ( taskId == SIW_INVALID_TID )
    {
        return(taskId);
    } 
    
    /* Wait for task init */
    rc = SysTaskWaitAck(taskId);

    /* Return child status */
    if ( childStatus != NULL )
    {
        *childStatus = rc;
    }
    
    /* Return taskId value returned */
    return(taskId);
}

/***********************************************************************
 *  Function:    SiwTaskInitAck()
 *  Description: This function is used to acknowlegde to the task
 *               spawner the status of the initialization of the task.
 *               If status of ERROR is reported then the task will be
 *               suspended after sending the init status.
 *
 *  In Parms:    STATUS_t status - initialization status of OK or ERROR
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwTaskInitAck(STATUS_t status)
{
    STATUS_t         rc;
    U32_t            taskIndex;
    SYS_TASK_ENTRY_t *taskTblPtr;
    
    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskInitAck, status, 0, 0, 0, 0);

    /* Get table entry */
    taskIndex = (U32_t)SiwTaskIdSelf();
    taskTblPtr = &sysTaskTable[taskIndex];

    /* Save init status */
    taskTblPtr->iStatus = status;

    /* Check if sync semaphore still exists */
    if ( taskTblPtr->spawnSem != NULL )
    {
        rc = semGive(taskTblPtr->spawnSem);

        /* Check if give failed */
        if ( rc == ERROR )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKSEMGIVEFAIL),
                     taskTblPtr->spawnSem, SiwTaskIdSelf());
        }
    }
    else
    {
        /* Parent timed out wait - deleted semaphore */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKINITPTO), SiwTaskIdSelf());
        rc = TIMEOUT;
    }

    /* If error status - suspend task */
    if ( status == ERROR )
    {
        /* Log warning event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKINITFAIL), SiwTaskIdSelf());

        /* Suspend task */
        taskSuspend(0);
    } /* end if status == ERROR */

#if 0
    /* give the CPU to parent */
    taskDelay(1);
#endif

    /* Return rc */
    return(rc);
}

/***********************************************************************
 *  Function:    SiwTaskRegRecv()
 *  Description: This function is used to register a recovery function
 *               for the specified task when it is suspended. The recovery
 *               function can return the action it wants the task monitor
 *               to take for the task after it has analyzed or performed
 *               some actions. Only one recv function can be registered
 *               for a task.
 *
 *  In Parms:    SIW_TID_t - task on which to register recovery function
 *               SIW_RECVFUNC_t - recovery function pointer
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwTaskRegRecv(SIW_TID_t taskId, SIW_RECVFUNC_t recvFunc)
{
    U32_t            taskIndex = 0;
    SYS_TASK_ENTRY_t *taskTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskRegRecv, taskId, recvFunc, 0, 0, 0);
    
    /* Check if taskId of self passed */
    if ( taskId == SIW_TID_SELF )
    {
        taskIndex = (UINT32)SiwTaskIdSelf();
    }

    /* Get table entry pointer */
    taskTblPtr = &sysTaskTable[taskIndex];

    /* Check that taskId has correct tag value */
    if ( taskId != taskTblPtr->siwTID )
    {
        /* Check if could be deleted siwTID */
        if ( (U32_t)taskId < sysNextTid )
        {
            /* Log error event */
            /* SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDDELETED), taskId); */
        } 
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), taskId);
        } 

        return(ERROR);
    } 

    /* Set recovery function */
    taskTblPtr->taskPara.recvFunc = recvFunc;

    /* Return OK */
    return(OK);
}

/***********************************************************************
 *  Function:    SiwTaskDelete()
 *  Description: This function is used to delete a task. A value of
 *               SIW_TID_SELF can be passed to have a task delete itself.
 *               This function will not be able to delete a suspended task
 *               which is safe from deletion. When this condition occurs,
 *               an ERROR is returned rather than hanging the caller because
 *               the suspended task will not exit from being safe from
 *               deletion.
 *
 *  In Parms:    SIW_TID_t siwTID - SIW task ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwTaskDelete(SIW_TID_t siwTID)
{
    STATUS_t         rc;
    U32_t            taskIndex = 0;
    SYS_TASK_ENTRY_t *taskTblPtr;
    WIND_TCB         *tcbPtr;
    SIW_TID_t        taskId = siwTID;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskDelete, taskId, 0, 0, 0, 0);

    /* Check if taskId of self passed */
    if ( taskId == SIW_TID_SELF )
    {
        taskId = SiwTaskIdSelf();
    }
    taskIndex = (U32_t)taskId;

    /* Get table entry pointer */
    taskTblPtr = &sysTaskTable[taskIndex];

    /* Check that taskId has correct tag value */
    if ( taskId != taskTblPtr->siwTID )
    {
        /* Check if could be deleted siwTID */
        if (taskIndex < sysNextTid)
        {
            /* Log error event */
            /* SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDDELETED), taskId); */
        }
        else
        {
            /* Log error event */
            /* SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), taskId); */
        } 

        return(ERROR);
    } 

    /* Get Task TCB */
    tcbPtr = taskTcb(taskTblPtr->tid);

    /* Check if task is not valid */
    if ( tcbPtr == NULL )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKDELFAIL), taskTblPtr->tid,
                 taskId);
        return(ERROR);
    } 

    /* Check if taskDelete will hang */
    if ( taskIsSuspended(taskTblPtr->tid) == TRUE &&
         tcbPtr->safeCnt > 0 )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKDELSAFE), taskId);
        return(ERROR);
    }

    /* Delete the VxWorks Task */
    rc = taskDelete(taskTblPtr->tid);

    /* Check if error */
    if ( rc == ERROR )
    {
        /* Check if not ISR Callable */
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE),"SiwTaskDelete");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKDELFAIL), taskTblPtr->tid,
                     taskId);
        } 
    } 

    return(rc);
} 

/***********************************************************************
 *  Function:    SiwTaskDelay()
 *  Description: This function is used to delay a task from executing
 *               for the specified delay time in ticks.
 *
 *  In Parms:    S32_t delay - time to delay in ticks
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwTaskDelay(S32_t delaytime)
{
    STATUS_t   rc;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskDelay, delaytime, 0, 0, 0, 0);
    
    /* Delay using VxWorks taskDelay */
    rc = taskDelay(delaytime);

    /* Check if error */
    if ( rc == ERROR )
    {
        /* Check if not ISR Callable */
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwTaskDelay");
        } 
    } 

    return(rc);
}

/***********************************************************************
 *  Function:    SiwTaskIdSelf()
 *  Description: This function is used to get the task id of the
 *               currently executing task. If called from an interrupt
 *               service routine the result are indeterminate. If called
 *               by a task which was not spawned as a SIW task then a
 *               SIW_TID is assigned to the task and returned.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     SIW_TID_t of calling task
 **********************************************************************/
SIW_TID_t
SiwTaskIdSelf(void)
{
    WIND_TCB         *tcbPtr;
    SIW_TID_t        taskId;
    STATUS_t         rc;
    BOOL_t           found = FALSE;
    S32_t            count = 0;
    S32_t            priority;
    SYS_TASK_ENTRY_t *taskTblPtr = NULL;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskIdSelf, 0, 0, 0, 0, 0);
    
    /* Get TCB entry of current task */
    tcbPtr = (WIND_TCB *)taskIdSelf();

    /* Get siwTID from spare field */
    taskId = (SIW_TID_t)tcbPtr->spare1;

    /* Check if siwTID is zero */
    if ( taskId == (SIW_TID_t)0 && intContext() == FALSE )
    {
        /* Initialize for search */
        found = FALSE;
        count = 0;
        
        /* Get semaphore to search table for available entry */
        rc = semTake(sysTaskTblSem, WAIT_FOREVER);
        if ( rc == ERROR )
        {
            if ( errno == S_intLib_NOT_ISR_CALLABLE ) {
                /* Log error event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwTaskIdSelf");
            } 
            else
            {
                /* Log error event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "Task",
                            sysTaskTblSem, errno);
            } 

            return((SIW_TID_t)0);
        } 

        /* Search task table for available entry */
        while ( found != TRUE && count < MAX_TASK_ENTRIES )
        {
            /* Get next entry pointer */
            taskTblPtr = &sysTaskTable[sysNextTid];
        
            /* Check if next entry is available */
            if ( taskTblPtr->siwTID == SIW_INVALID_TID )
            {
                found = TRUE;
                taskId = (SIW_TID_t)sysNextTid;
                taskTblPtr->siwTID = taskId;
            } 

            /* Increment next available taskid and count checked */
            sysNextTid++;
            count++;

            /* Check if at end of task table */
            if ( sysNextTid >= MAX_TASK_ENTRIES )
            {
                sysNextTid = SYS_TID_START_VALUE;
            }
        } 

        /* Check if not found */
        if ( found != TRUE )
        {
            /* Release table semaphore */
            semGive(sysTaskTblSem);

            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKTBLFULL), MAX_TASK_ENTRIES);
            return((SIW_TID_t)0);
        } 

        /* Set Tcb spare field with SIW TID */
        tcbPtr->spare1 = (S32_t)taskId;
        tcbPtr->spare2 = 0;
        
        /* Initialize taskTable entry with task info */
        taskTblPtr->tid = (S32_t)tcbPtr;
        strncpy(&taskTblPtr->taskPara.name[0],
                taskName(taskTblPtr->tid), 11);
        taskTblPtr->taskPara.name[11] = '\0';
        taskTblPtr->taskPara.taskPriority.applId = SIW_PLATFORM_SW;
        taskPriorityGet(taskTblPtr->tid, &priority);
        
        if ( priority < SYS_PLFM_NORMAL_BASE )
        {
            taskTblPtr->taskPara.taskPriority.prioGroup =
                SIW_PRI_GRP_HIGH;
            taskTblPtr->taskPara.taskPriority.priority = priority;
        } 
        else if ( priority < SYS_BACKGROUND_BASE )
        {
            taskTblPtr->taskPara.taskPriority.prioGroup =
                SIW_PRI_GRP_NORMAL;
            taskTblPtr->taskPara.taskPriority.priority =
                priority - SYS_PLFM_NORMAL_BASE;
        } 
        else
        {
            taskTblPtr->taskPara.taskPriority.prioGroup =
                SIW_PRI_GRP_BACKGROUND;
            taskTblPtr->taskPara.taskPriority.priority =
                priority - SYS_BACKGROUND_BASE;
        }

        taskTblPtr->taskPara.taskInfo.stackSize =
            tcbPtr->pStackBase - tcbPtr->pStackEnd;
        taskTblPtr->taskPara.taskInfo.entryPt = tcbPtr->entry;
        taskTblPtr->taskPara.taskInfo.arg1 = 0;
        taskTblPtr->taskPara.taskInfo.arg2 = 0;
        taskTblPtr->taskPara.taskInfo.arg3 = 0;
        taskTblPtr->taskPara.taskInfo.arg4 = 0;
        taskTblPtr->taskPara.taskInfo.waitAck = 0;
        taskTblPtr->taskPara.suspendAction = SIW_RECV_NO_ACTION;
        taskTblPtr->taskPara.failAction = SIW_RECV_NO_ACTION;

        taskTblPtr->parent = SIW_INVALID_TID;
        taskTblPtr->priority = priority;
        taskTblPtr->spawnSem = NULL;
        taskTblPtr->iStatus = OK;
        taskTblPtr->taskPara.recvFunc = NULL;
        taskTblPtr->procEvt = FALSE;
        taskTblPtr->errorLog = NULL;

        /* Trace addition to task table */
        /*SIW_TRACE(SIW, TRACE_NORMAL, 4,
                  "Added SIW Task %p for Vx Tid=%p Priority=%d Stksize=%d.",
                  taskId, tcbPtr, priority,
                  taskTblPtr->taskPara.taskInfo.stackSize);*/
        
        /* Release table semaphore */
        semGive(sysTaskTblSem);
    } 
    
    return(taskId);
} 

/***********************************************************************
 *  Function:    SiwTaskIdVerify()
 *  Description: This function is used to verify the existance of the
 *               specified task id passed. It returns OK if the SIW_TID
 *               is valid or ERROR if the SIW_TID is invalid.
 *
 *  In Parms:    SIW_TID_t taskId - task id to verify existance
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwTaskIdVerify(SIW_TID_t taskId)
{
    STATUS_t         rc;
    U32_t            taskIndex;
    SYS_TASK_ENTRY_t *taskTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskIdVerify, taskId, 0, 0, 0, 0);
    
    /* Get task table index */
    taskIndex = (U32_t)taskId;
    
    /* Check that taskId is in range */
    if ( taskIndex >= MAX_TASK_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), taskId);
        return(ERROR);
    }

    /* Get table entry pointer */
    taskTblPtr = &sysTaskTable[taskIndex];

    /* Check that taskId is valid */
    if ( taskId != taskTblPtr->siwTID )
    {
        /* Check if could be deleted siwTID */
        if ( (U32_t)taskId < sysNextTid )
        {
            /* Log error event */
            /* SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDDELETED), taskId); */
        } 
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), taskId);
        } 
        return(ERROR);
    } 

    /* Get status of VxWorks tid verify */
    rc = taskIdVerify(taskTblPtr->tid);

    /* Check status */
    if ( rc == ERROR )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), taskTblPtr->tid);
    } 
    
    /* Return status */
    return(rc);
} 

/***********************************************************************
 *  Function:    SiwTaskLock()
 *  Description: This function disables task from switching. The task
 *               calling this routine will be the only one allowed to
 *               execute unless it explicitly blocks.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwTaskLock(void)
{
    STATUS_t  rc;
    
    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskLock, 0, 0, 0, 0, 0);
    
    /* Call Vxworks taskLock */
    rc = taskLock();

    /* Check if error */
    if ( rc == ERROR )
    {
        /* Check if not ISR Callable */
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwTaskLock");
        }
    } 

    siwLockKey = rc;
    siwLockTaskId = taskIdSelf();
    siwLockTaskName = taskName(siwLockTaskId);

    return(rc);
} 

/***********************************************************************
 *  Function:    SiwReleaseTaskLock()
 *  Description: This function releases a task from locking the CPU.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 **********************************************************************/
void SiwReleaseTaskLock()
{
    WIND_TCB *wTcb;
    
    if (siwLockTaskId != 0)
    {
        wTcb = taskTcb(siwLockTaskId);
        wTcb->lockCnt = 0;
        sysLockTaskId = siwLockTaskId;
        siwLockTaskId = 0;
    }
    else if (sysLockTaskId == 0)
    {
        sysLockTaskId = taskIdSelf();
        wTcb = taskTcb(sysLockTaskId);

        if (wTcb->lockCnt != 0)
        {
            wTcb->lockCnt = 0;
            siwLockTaskName = taskName(sysLockTaskId);
        }
    }
}
 
/***********************************************************************
 *  Function:    SiwTaskUnlock()
 *  Description: This function enables task rescheduling after a
 *               SiwTaskLock has been done.  This function cannot be
 *               called from an interrupt service routine.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwTaskUnlock(void)
{
    STATUS_t  rc;
    
    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskUnlock, 0, 0, 0, 0, 0);

    /* Call VxWorks taskUnlock */
    rc = taskUnlock();
    
    /* Check if error */
    if ( rc == ERROR )
    {
        /* Check if not ISR Callable */
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE),"SiwTaskUnlock");
        }
    }

    siwLockTaskId = 0;

    return(rc);
}

/***********************************************************************
 *  Function:    SiwTaskName()
 *  Description: This function returns a pointer to the task name string
 *               which can up to 10 characters in length.
 *
 *  In Parms:    SIW_TID_t taskId - task ID 
 *  Out Parms:   None
 *  Returns:     A string pointer to task name or NULL if task id invalid
 **********************************************************************/
char *
SiwTaskName(SIW_TID_t taskId)
{
    U32_t            taskIndex;
    SYS_TASK_ENTRY_t *taskTblPtr;
    char             *pTaskName;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskName, taskId, 0, 0, 0, 0);
    
    /* Get task table index */
    taskIndex = (U32_t)taskId;
    
    /* Check that taskId is in range */
    if ( taskIndex >= MAX_TASK_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), taskId);
        return(NULL);
    } 

    /* Get table entry pointer */
    taskTblPtr = &sysTaskTable[taskIndex];

    /* Check that taskId has correct tag value */
    if ( taskId != taskTblPtr->siwTID )
    {
        /* Check if could be deleted siwTID */
        if ( (U32_t)taskId < sysNextTid )
        {
            /* Log error event */
            /* SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDDELETED), taskId); */
        } 
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), taskId);
        } 
        return(NULL);
    } 

    /* Get VxWorks taskName */
    pTaskName = taskName(taskTblPtr->tid);

    /* Check if no name */
    if ( pTaskName == NULL )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKNAMEFAIL), taskId);
    } 
    
    /* Return pTaskName */
    return(pTaskName);
} 

/***********************************************************************
 *  Function:    SiwTaskNameToId()
 *  Description: This function returns the SIW_TID associated with the
 *               specified task name.
 *
 *  In Parms:    char * taskName - task name to lookup to get task id
 *  Out Parms:   None
 *  Returns:     SIW_TID_t or SIW_INVALID_TID
 **********************************************************************/
SIW_TID_t
SiwTaskNameToId(char *  taskName)
{
    STATUS_t  rc;
    U32_t     taskIndex = 1;
    char      *strPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskNameToId, taskName, 0, 0, 0, 0);
    
    /* Get semaphore to search table for entry */
    rc = semTake(sysTaskTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwTaskNameToId");
        } 
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "Task",
                        sysTaskTblSem, errno);
        } 
        return(SIW_INVALID_TID);
    } 
    
    /* Search through task table */
    while ( taskIndex < MAX_TASK_ENTRIES )
    {
        /* Get string pointer */
        strPtr = sysTaskTable[taskIndex].taskPara.name;

        /* Check if entry is valid and matches */
        if ( sysTaskTable[taskIndex].siwTID != SIW_INVALID_TID &&
             strPtr != NULL && strcmp(strPtr, taskName) == 0 )
        {
            /* Release table semaphore */
            semGive(sysTaskTblSem);
            
            /* Return SIW_TID */
            return(sysTaskTable[taskIndex].siwTID);
        }    

        /* Increment index pointer */
        taskIndex++;
    } 

    /* Release table semaphore */
    semGive(sysTaskTblSem);

    /* No found - Return invalid task id */
    return(SIW_INVALID_TID);
} 

/***********************************************************************
 *  Function:    SiwIntContext()
 *  Description: This function determines if the current execution
 *               state is in interrupt context or task context.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     TRUE if interrupt context or FALSE if task context
 **********************************************************************/
BOOL_t
SiwIntContext()
{
    return(intContext());
} 

/***********************************************************************
 *  Function:    SiwIntLock()
 *  Description: This function disables interrupts. The function
 *               locks the interrupt, but not the context switch.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     S32_t key used in SiwIntUnlock()
 **********************************************************************/
S32_t
SiwIntLock()
{
    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwIntLock, 0, 0, 0, 0, 0);

    return(intLock());
} 

/***********************************************************************
 *  Function:    SiwIntUnlock()
 *  Description: This function re-enables interrupts previously
 *               disabled by SiwIntLock.
 *
 *  In Parms:    S32_t lockkey - key returned by SiwIntLock()
 *  Out Parms:   None
 *  Returns:     None
 **********************************************************************/
void
SiwIntUnlock(S32_t lockkey)
{
    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwIntUnlock, 0, 0, 0, 0, 0);
    
    intUnlock(lockkey);
    return;
} 

/***********************************************************************
 *  Function:  SysTaskSpawn()
 *
 *  Description: This function actually spawns the task that has been
 *               registered in the task table. The static parameters
 *               area of the table must be filled in before calling
 *               this function. The SIW_TID in field siwTID being used
 *               for this entry must also be set in the table entry before
 *               calling this function. The parameter passed is the SIW_TID
 *               task that is being spawned. The task table entry is
 *               initialized to be able to get init status from the child
 *               if a wait ack is specified.
 *
 *  In Parms:    SIW_TID_t tblTid - table index that is to spawn task
 *  Out Parms:   None
 *  Returns:     SIW_TID_t or SIW_INVALID_TID
 **********************************************************************/
SIW_TID_t
SysTaskSpawn(SIW_TID_t tblTid)
{
    U32_t            taskIndex = (U32_t)tblTid;
    S32_t            tid;
    S32_t            priority;
    SYS_TASK_ENTRY_t *taskTblPtr;
    SIW_PRIORITY_t   *taskPrioPtr;
    SIW_TASKINFO_t   *taskInfoPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SysTaskSpawn, tblTid, 0, 0, 0, 0);

    if ( taskIndex >= MAX_TASK_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), tblTid);
        return(SIW_INVALID_TID);
    } 

    /* Get table entry */
    taskTblPtr = &sysTaskTable[taskIndex];

    /* Setup pointers to parameter structures */
    taskPrioPtr = &taskTblPtr->taskPara.taskPriority;
    taskInfoPtr = &taskTblPtr->taskPara.taskInfo;

    /* Validate priority number range */
    priority = taskPrioPtr->priority;
    if ( priority >= SYS_MAX_PRIO_VAL )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKPRIOINVALID), priority);
        return(SIW_INVALID_TID);
    } /* end if priority >= SYS_MAX_PRIO_VAL */

    /* Calculate task priority */
    switch ( taskPrioPtr->applId ) {
        case SIW_PLATFORM_SW :
            switch ( taskPrioPtr->prioGroup ) {
                case SIW_PRI_GRP_NORMAL:
                    priority += SYS_PLFM_NORMAL_BASE;
                    break; /* case SIW_PRI_GRP_NORMAL */
                case SIW_PRI_GRP_HIGH:
                    priority += SYS_PLFM_HIGH_BASE;
                    break; /* case SIW_PRI_GRP_HIGH */
                case SIW_PRI_GRP_BACKGROUND :
                    priority += SYS_BACKGROUND_BASE;
                    break; /* case SIW_PRI_GRP_BACKGROUND */
                default :
                    /* Log error event */
                    SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKPGRPINVALID),
                             taskPrioPtr->prioGroup);
                    return(SIW_INVALID_TID); /* default */
            }  
            break; 
        case SIW_APPL_SW :
            switch ( taskPrioPtr->prioGroup ) {
                case SIW_PRI_GRP_NORMAL :
                    priority += SYS_APPL_NORMAL_BASE;
                    break; /* case SIW_PRI_GRP_NORMAL */
                case SIW_PRI_GRP_HIGH :
                    priority += SYS_APPL_HIGH_BASE;
                    break; /* case SIW_PRI_GRP_HIGH */
                case SIW_PRI_GRP_BACKGROUND :
                    priority += SYS_BACKGROUND_BASE;
                    break; /* case SIW_PRI_GRP_BACKGROUND */
                default :
                    /* Log error event */
                    SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKPGRPINVALID),
                             taskPrioPtr->prioGroup);
                    return(SIW_INVALID_TID); /* default */
            } 
            break; 
        default :
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKAPPLINVALID),
                     taskPrioPtr->applId);
            return(SIW_INVALID_TID); /* default */
    }  /* end switch taskPrioPtr->applId */

    /* Validate stacksize */
    if ( taskInfoPtr->stackSize < SYS_MIN_STACK_SIZE ||
         taskInfoPtr->stackSize > SYS_MAX_STACK_SIZE )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKSTKSIZEINV),
                 taskInfoPtr->stackSize);
        return(SIW_INVALID_TID);
    }

    /* Validate suspend action */
    switch ( taskTblPtr->taskPara.suspendAction ) {
        case SIW_RECV_NO_ACTION :
        case SIW_RECV_LOG_ERROR :
        case SIW_RECV_RESTART_TASK :
        case SIW_RECV_SOFT_RESET :
        case SIW_RECV_REG_FUNC :
            break; /* case SIW_RECV_NO_ACTION ... */
        default :
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKSUSPACTINV),
                     taskTblPtr->taskPara.suspendAction);
            return(SIW_INVALID_TID);
    }  /* end switch taskTblPtr->taskPara.suspendAction */
    
    /* Validate fail suspend action */
    switch ( taskTblPtr->taskPara.failAction ) {
        case SIW_RECV_NO_ACTION :
        case SIW_RECV_LOG_ERROR :
        case SIW_RECV_RESTART_TASK :
        case SIW_RECV_SOFT_RESET :
        case SIW_RECV_REG_FUNC :
            break; /* case SIW_RECV_NO_ACTION ... */
        default :
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKFAILACTINV),
                     taskTblPtr->taskPara.failAction);
            return(SIW_INVALID_TID);
    }  /* end switch taskTblPtr->taskPara.failAction */

    /* Initialize task entry */
    taskTblPtr->parent = SiwTaskIdSelf();
    taskTblPtr->priority = priority;
    taskTblPtr->iStatus = ERROR;
    taskTblPtr->taskPara.recvFunc = NULL;
    taskTblPtr->errorLog = NULL;

    /* If going to wait for task ack - create sync semaphore */
    if ( taskTblPtr->taskPara.taskInfo.waitAck != 0 )
    {
        taskTblPtr->spawnSem = semBCreate(SIW_SEM_Q_FIFO, SEM_EMPTY);
        /* Check creation status */
        if ( taskTblPtr->spawnSem == NULL )
        {
            if ( errno == S_memLib_NOT_ENOUGH_MEMORY ) 
            {
                /* Log error event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, OBJCREATENOMEM), "Spawn Semaphore");
            } 
            else
            {
                /* Log error event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKSEMCRTFAIL));
            } /* end else */

            return(SIW_INVALID_TID);
        } /* end if semId == NULL */
    } /* end if taskTblPtr->taskPara.taskInfo.waitAck != 0 */
    else
    {
        taskTblPtr->spawnSem = NULL;
    } /* end else */

    /* Spawn Task */

    tid = taskSpawn(taskTblPtr->taskPara.name, priority, siwTaskSpawnOpt,
                    taskInfoPtr->stackSize, SysTaskSetup, (INT32) tblTid,
                    (INT32) taskInfoPtr->entryPt, taskInfoPtr->arg1,
                    taskInfoPtr->arg2, taskInfoPtr->arg3,
                    taskInfoPtr->arg4, taskInfoPtr->arg5,
                    taskInfoPtr->arg6, taskInfoPtr->arg7,
                    taskInfoPtr->arg8);

    /* Check if error spawning */
    if ( tid == ERROR )
    {
        if ( errno == S_memLib_NOT_ENOUGH_MEMORY ) 
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, OBJCREATENOMEM), "Task");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKSPAWNFAIL));
        } 

        /* Invalidate parent entry */
        taskTblPtr->parent = SIW_INVALID_TID;
        
        /* Delete spawnSem if created */
        if ( taskTblPtr->spawnSem != NULL )
        {
            semDelete(taskTblPtr->spawnSem);
            taskTblPtr->spawnSem = NULL;
        } 
        return(SIW_INVALID_TID);
    } 

    /* Assign VxWorks task id to task entry */
    taskTblPtr->tid = tid;

    /* zero out the task recovery counter */
    taskTblPtr->recvCntr = 0;

    /* Return siwTID */
    return(tblTid);
} 

/***********************************************************************
 *  Function Name:  SysTaskSetup
 *
 *  Description: This function is the entry point for all SIW Tasks.
 *               It is used to wrap all SIW tasks so that the task
 *               state can be initialized properly before invoking the
 *               actual task function.  It makes sure the assigned
 *               SIW_TID_t for the task is saved in the task TCB so that
 *               it can be easily retrieved.
 *
 *  In Parms:    SIW_TID_t taskId - siw task id of task befing invoked
 *               FUNC_PTR_t entryPt - actual entry point of task
 *               S32_t arg1 - first arg to task
 *               S32_t arg2 - second arg to task
 *               S32_t arg3 - third arg to task
 *               S32_t arg4 - fourth arg to task
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
LOCAL STATUS_t
SysTaskSetup(SIW_TID_t  taskId,
             FUNC_PTR_t entryPt,
             S32_t      arg1,
             S32_t      arg2,
             S32_t      arg3,
             S32_t      arg4,
             S32_t      arg5,
             S32_t      arg6,
             S32_t      arg7,
             S32_t      arg8)
{
    STATUS_t  rc;
    WIND_TCB  *tcbPtr;
    S32_t     lockKey;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SysTaskSetup, taskId, 0, 0, 0, 0);
 
    lockKey = intLock();   
    /* Copy SIW taskId into TCB */
    tcbPtr = (WIND_TCB *)taskIdSelf();
    tcbPtr->spare1 = (S32_t)taskId;
    tcbPtr->spare2 = 0;
    tcbPtr->entry = entryPt;
    intUnlock(lockKey);

    /* Call task entryPt */
    rc = (*entryPt)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);

    return(rc);
} 

/***********************************************************************
 *  Function Name:  SysTaskWaitAck
 *
 *  Description: This function waits for the initialization
 *               acknowledgement from a child task. It waits on a binary
 *               semaphore for the child to call SiwTaskInitAck() with
 *               its status. Any error conditions detected are logged
 *               with error events. 
 *
 *  In Parms:    SIW_TID_t tblTid - task id of child task
 *  Out Parms:   None
 *  Returns:     Child status or TIMEOUT
 **********************************************************************/
STATUS_t
SysTaskWaitAck(SIW_TID_t tblTid)
{
    STATUS_t         rc;
    U32_t            taskIndex = (U32_t)tblTid;
    SYS_TASK_ENTRY_t *taskTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SysTaskWaitAck, tblTid, 0, 0, 0, 0);
    
    /* Get table entry */
    taskTblPtr = &sysTaskTable[taskIndex];

    /* Check if wait for Ack */
    if ( taskTblPtr->taskPara.taskInfo.waitAck != 0 )
    {
        /* wait here for child task to return semaphore */
        rc = semTake(taskTblPtr->spawnSem,
                     taskTblPtr->taskPara.taskInfo.waitAck);

        /* Check if take failed */
        if ( rc == ERROR )
        {
            if ( errno == S_objLib_OBJ_TIMEOUT )
            {
                /* Log warning event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKINITACKTO),
                         SiwTaskIdSelf(), tblTid, taskTblPtr->taskPara.taskInfo.waitAck);
            } 
            else
            {
                /* Log error event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKWACKFAIL), tblTid);
            }
            
            /* Delete semaphore */
            semDelete(taskTblPtr->spawnSem);
            taskTblPtr->spawnSem = NULL;

            /* Return timeout */
            return(TIMEOUT);
        }

        /* Delete semaphore */
        semDelete(taskTblPtr->spawnSem);
        taskTblPtr->spawnSem = NULL;
        
        /* Return child status */
        return(taskTblPtr->iStatus);
    }

    return(OK);
}

/***********************************************************************
 *  Function:  SysTaskDelete()
 *
 *  Description: This function is used to actually delete a task entry
 *               from the task table. This function is called as a task
 *               delete hook by the system to delete a task. If the task
 *               is an SIW task then the corresponding task entry is
 *               cleaned up and made available for reuse.
 *
 *  In Parms:    WIND_TCB * tcbPtr - pointer to Task Control Block of task
 *                                   being deleted
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SysTaskDelete(WIND_TCB *tcbPtr)
{
    STATUS_t         rc;
    SIW_TID_t        taskId;
    U32_t            taskIndex;
    SYS_TASK_ENTRY_t *taskTblPtr;
        
    /* Get siwTID from spare field */
    taskId = (SIW_TID_t)tcbPtr->spare1;

    /* Get task table index */
    taskIndex = (U32_t)taskId;
    
    /* Check that taskIndex is in range */
    if ((taskIndex >= MAX_TASK_ENTRIES) || (taskIndex == 0))
    {
        return(OK);
    } 

    /* Get table entry pointer */
    taskTblPtr = &sysTaskTable[taskIndex];

    /* Get semaphore to delete entry from table */
    rc = semTake(sysTaskTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "Task",
                    sysTaskTblSem, errno);
    }

    /* Invalidate vxworks tid and siwTID */
    taskTblPtr->tid = -1;
    taskTblPtr->siwTID = SIW_INVALID_TID;
 
    if (taskIndex > NUM_STATIC_TASKS)
    {
        /* Clean up task table entry */
        taskTblPtr->parent = SIW_INVALID_TID;
        taskTblPtr->priority = 0;
        taskTblPtr->iStatus = OK;
        taskTblPtr->taskPara.recvFunc = NULL;
        taskTblPtr->procEvt = FALSE;
        taskTblPtr->errorLog = NULL;
    }

    /* Delete spawnSem if exists */
    if ( taskTblPtr->spawnSem != NULL )
    {
        semDelete(taskTblPtr->spawnSem);
        taskTblPtr->spawnSem = NULL;
    }

    /* Release table semaphore */
    semGive(sysTaskTblSem);

#if 0
    /* return any semaphores that took by this task */
    SysSemReturn(taskId);
#endif

    /* Delete Timer from Timer Function Table */
    SysTimerDelete(taskId);

    return(OK);
}

/***********************************************************************
 *  Function:    SiwTaskShow()
 *  Description: This function shows the task information for the
 *               specified task to the standard output for the task.
 *               The level is used to control the amount of detail.
 *               Level 0 is used to display summary information.
 *               Level 1 is used to display detail information about
 *               the task. Level 2 is used to display all tasks.
 *
 *  In Parms:    SIW_TID_t taskId - task id to show information about
 *               S32_t level - level of information to show
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwTaskShow(SIW_TID_t  taskId,
            S32_t      level)
{
    STATUS_t         rc = OK;
    U32_t            taskIndex;
    SYS_TASK_ENTRY_t *taskTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskShow, taskId, level, 0, 0, 0);

    /* Get task table index */
    taskIndex = (U32_t)taskId;
    
    /* Check that taskId is in range */
    if ( taskIndex >= MAX_TASK_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), taskId);
        return(ERROR);
    }

    /* Get table entry pointer */
    taskTblPtr = &sysTaskTable[taskIndex];

    /* Check that taskId has correct tag value */
    if ( taskId != taskTblPtr->siwTID )
    {
        /* Check if could be deleted siwTID */
        if ( (U32_t)taskId < sysNextTid )
        {
            /* Log error event */
            /* SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDDELETED), taskId); */
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TASKIDINVALID), taskId);
        }

        return(ERROR);
    }

    /* Call taskShow */
    if ( level == 0 || level == 1 || level == 2 )
    {
        rc = taskShow(taskTblPtr->tid, level);
        if ( rc == ERROR )
        {
            /* SIW INTERNAL FAILURE;*/
        }
    }

    printf("Task Recovery Counter: %x\n", taskTblPtr->recvCntr);

    return(rc);
}

void SiwTaskDebugOn()
{
    siwTaskDebug = TRUE;
}

void SiwTaskDebugOff()
{
    siwTaskDebug = FALSE;
}

int SiwTaskWaitErrs = 0;

/***********************************************************************
 *  Function:    SiwTaskWait()
 *  Description: This function is used to delay a task from executing
 *               for delay time 2ms (+-) 1ms.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 **********************************************************************/
void
SiwTaskWait()
{
    STATUS  rc;
    SEM_ID  semId;

    semId = sysSemTable[(int)SysWaitSemIdGet()].semId;

    if (semId)
    {
        rc = semTake(semId, 2);
    }
    else
    {
        rc = ERROR;
    }

    if (rc != OK)
    {
        ++ SiwTaskWaitErrs;
    }
}

/***********************************************************************
 *  Function:    SiwTaskSpawnDone()
 *  Description: Once all static tasks have been spawned, this function
 *               is called to prepare for spawning dynamic tasks.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 **********************************************************************/
void SiwTaskSpawnDone()
{
    siwTaskSpawnOpt = VX_NO_STACK_FILL;
}

