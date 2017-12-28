/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_mq.c
 *
 *System Interface Wrapper code for Message Queue
 *
 *Note:
 *
 *Author(s):
 * 
 */

#include <stdio.h>
#include <intLib.h>
#include <objLib.h>
#include <memLib.h>
#include <msgQLib.h>
#include <string.h>
#include <taskLib.h>
#include <time.h>
#include <sysSymTbl.h>
#include <axss_types.h>
#include <siw_if.h>
#include <sys_os.h>
#include <sys_if.h>
#include "siw_trace.h"
#include "siw_event.h"

/***********************************************************************
 *  Define used for invalid status valud
 **********************************************************************/
#define NO_EXIT_STATUS       0xBADBEEF

/***********************************************************************
 *  Local static function declaration
 **********************************************************************/
LOCAL S32_t SysMsgQFlush(MSG_Q_ID msgQId);

#ifndef SIM
extern U32_t SysClkTickGet(void);
#endif

/***********************************************************************
 *  Function:    SiwMsgQCreate()
 *  Description: This function creates and initializes a message queue
 *               that has the maximum message size. The message queue
 *               is associated with the specified owning task id and
 *               has the specified queue name.
 *
 *  In Parms:    S32_t maxMsgs - specifies max number of message in queue
 *               S32_t maxMsgLength - specfies max message size in queue
 *               S32_t option - message queue option
 *               SIW_TID_t ownerTaskId - task id which owns queue
 *               char *qName - unique name for message queue
 *  Out Parms:   None
 *  Returns:     A valid SIW_MQID_t or SIW_INVALID_MQID
 **********************************************************************/
SIW_MQID_t
SiwMsgQCreate(S32_t     maxMsgs,
              S32_t     maxMsgLength,
              S32_t     option,
              char *    queueName)
{
    SIW_TID_t        ownerTaskId;
    STATUS_t         rc;
    BOOL_t           found = FALSE;
    S32_t            count = 0;
    SIW_MQID_t       mqId = SIW_INVALID_MQID;
    SYS_MSGQ_ENTRY_t *mqTblPtr = NULL;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQCreate, maxMsgs, maxMsgLength, option, queueName, 0);

    /* Check if ownerTaskId of self passed */
    ownerTaskId = SiwTaskIdSelf();

    /* Get semaphore to search table for available entry */
    rc = semTake(sysMsgQTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwMsgQCreate");
        } 
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "MsgQ",
                        sysMsgQTblSem, errno);
        } 
        return(SIW_INVALID_MQID);
    } 

    /* Search message queue table for available entry */
    while ( found != TRUE && count < MAX_MSGQ_ENTRIES )
    {
        /* Get next entry pointer */
        mqTblPtr = &sysMsgQTable[sysNextMqid];
        
        /* Check if next entry is available */
        if ( mqTblPtr->siwMqId == SIW_INVALID_MQID )
        {
            found = TRUE;
            mqId = (SIW_MQID_t)sysNextMqid;
            mqTblPtr->siwMqId = mqId;
        }

        /* Increment next available msgqid and count checked */
        sysNextMqid++;
        count++;

        /* Check if at end of message queue table */
        if ( sysNextMqid >= MAX_MSGQ_ENTRIES )
        {
            sysNextMqid = SYS_MQID_START_VALUE;
        }
    }

    /* Check if not found */
    if ( found != TRUE )
    {
        /* Release table semaphore */
        semGive(sysMsgQTblSem);

        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQTBLFULL), MAX_MSGQ_ENTRIES);

        return(SIW_INVALID_MQID);
    }

    /* Initialize message queue table with parameters */
    mqTblPtr->mqPara.maxMsgs = maxMsgs;
    mqTblPtr->mqPara.maxMsgLength = maxMsgLength;
    mqTblPtr->mqPara.ownerTaskId = ownerTaskId;

    if ( queueName != NULL )
    {
        strncpy(&mqTblPtr->mqPara.queueName[0], queueName,
                SIW_MQ_NAME_SIZE - 1);
        mqTblPtr->mqPara.queueName[SIW_MQ_NAME_SIZE - 1] = '\0';
    }
    else
    {
        mqTblPtr->mqPara.queueName[0] = '\0';
    }

    /* Call SysMsgQCreate with id and caller location */
    mqId = SysMsgQCreate(mqId, option);

    /* Assign result of create to entry */
    mqTblPtr->siwMqId = mqId;

    /* Release table semaphore */
    semGive(sysMsgQTblSem);
    
    /* Return mqId value returned */
    return(mqId);
}

/***********************************************************************
 *  Function:    SiwMsgQDelete()
 *  Description: This function deletes the specified message queue and
 *               frees the resources used by it. Any pended tasks on 
 *               this message queue will unblock and retun ERROR.
 *
 *  In Parms:    SIW_MQID_t msgQId - message queue to delete
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwMsgQDelete(SIW_MQID_t msgQId)
{
    STATUS_t         rc;
    S32_t            flushCount;
    U32_t            mqIndex = (U32_t)msgQId;
    U32_t            taskIndex;
    S32_t            lockKey;
    MSG_Q_ID         vxMsgQId;
    SYS_MSGQ_ENTRY_t **mqChainPtr;
    SYS_MSGQ_ENTRY_t *mqTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQDelete, msgQId, 0, 0, 0, 0);
    
    /* Check that msgQId is in range */
    if ( mqIndex >= MAX_MSGQ_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);

        return(ERROR);
    }

    /* Get table entry pointer */
    mqTblPtr = &sysMsgQTable[mqIndex];

    /* Check that msgQId has correct tag value */
    if ( msgQId != mqTblPtr->siwMqId )
    {
        /* Check if could be deleted siwMqId */
        if ( (U32_t)msgQId < sysNextMqid )
        {
            /* Log warning event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDDELETED), msgQId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        }

        return(ERROR);
    }

    /* Get semaphore to modify table */
    rc = semTake(sysMsgQTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE ) 
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwMsgQDelete");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "MsgQ",
                        sysMsgQTblSem, errno);
        }

        return(ERROR);
    }

    /* Remove msgQ from task msgQ chain */
    taskIndex = (U32_t)mqTblPtr->mqPara.ownerTaskId;
    mqChainPtr = &sysTaskTable[taskIndex].mqChain;
    while ( *mqChainPtr != NULL )
    {
        /* Check if right list entry */
        if ( (*mqChainPtr) == mqTblPtr )
        {
            /* Found entry in list - remove from list */
            *mqChainPtr = mqTblPtr->mqNext;
            break;
        }

        /* Go to next list entry */
        mqChainPtr = &((*mqChainPtr)->mqNext);
    }

    /* Invalidate siwMqId and VxWorks MSG_Q_ID */
    vxMsgQId = mqTblPtr->msgQId;
    mqTblPtr->siwMqId = SIW_INVALID_MQID;
    mqTblPtr->msgQId = NULL;
    
    /* Check if binary semaphore should be deleted */
    if ( sysTaskTable[taskIndex].mqChain == NULL )
    {
        /* Delete binary semaphore */
        rc = semDelete(sysTaskTable[taskIndex].sembId);
        if ( rc == ERROR )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQSEMDELFAIL),
                     sysTaskTable[taskIndex].sembId, msgQId);
        }
        sysTaskTable[taskIndex].sembId = NULL;
    }

    /* Flush message queue */
    flushCount = SysMsgQFlush(vxMsgQId);

    /* Update stats for msgQ if flushCount > 0 */
    if ( flushCount > 0 )
    {
        lockKey = intLock();
        mqTblPtr->msgRcvd += flushCount;
        mqTblPtr->qDelay += flushCount * SysHrTimeGet();
        intUnlock(lockKey);
    }

    /* Delete message queue */
    rc = msgQDelete(vxMsgQId);

    /* Clean up msgQ table entry */
    mqTblPtr->mqNext = NULL;
    mqTblPtr->cTask = SIW_INVALID_TID;
    mqTblPtr->fTask = SIW_INVALID_TID;

    /* Check delete status */
    if ( rc == ERROR )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQDELFAIL), vxMsgQId, msgQId);
    }

    /* Release table semaphore */
    semGive(sysMsgQTblSem);

    /* Return delete status */
    return(rc);
}

/***********************************************************************
 *  Function:    SiwMsgQFlush()
 *  Description: This function flushes all messages out of the
 *               specified message queue and frees the resources used
 *               by the messages being discarded.
 *
 *  In Parms:    SIW_MQID_t msgQId - message queue to flush
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwMsgQFlush(SIW_MQID_t msgQId)
{
    S32_t            flushCount;
    U32_t            mqIndex = (U32_t)msgQId;
    S32_t            lockKey;
    SYS_MSGQ_ENTRY_t *mqTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQFlush, msgQId, 0, 0, 0, 0);

    /* Check that msgQId is in range */
    if ( mqIndex >= MAX_MSGQ_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        return(ERROR);
    }

    /* Get table entry pointer */
    mqTblPtr = &sysMsgQTable[mqIndex];

    /* Check that msgQId has correct tag value */
    if ( msgQId != mqTblPtr->siwMqId )
    {
        /* Check if could be deleted siwMqId */
        if ( (U32_t)msgQId < sysNextMqid )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDDELETED), msgQId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        }

        return(ERROR);
    }

    /* Call sysMsgQFlush to flush msgQ */
    flushCount = SysMsgQFlush(mqTblPtr->msgQId);
    
    /* Update stats for msgQ if flushCount > 0 */
    if ( flushCount > 0 )
    {
        lockKey = intLock();
        mqTblPtr->msgRcvd += flushCount;
        mqTblPtr->qDelay += flushCount * SysHrTimeGet();
        intUnlock(lockKey);
    }

    /* Return status of flush */
    else if ( flushCount == ERROR )
    {
        return(ERROR);
    }
    
    return(OK);
}

/***********************************************************************
 *  Function:    SiwMsgQSend()
 *  Description: This function sends a message to a message queue. If
 *               the message queue is full a TIMEOUT is returned.
 *
 *  In Parms:    SIW_MQID_t msgQId  - message queue to receive sent
 *                                    message
 *               MSG_BUF_HDR_t *bufPtr - pointer to message buffer
 *               U32_t    bufSize - size of message being sent
 *               S32_t    timeout - amount of time in clock ticks
 *                                  to wait
 *               S32_t    urgent  - priority to send message
 *  Out Parms:   None
 *  Returns:     OK, ERROR, or TIMEOUT
 **********************************************************************/
STATUS_t
SiwMsgQSend(SIW_MQID_t     msgQId,
            MSG_BUF_HDR_t *msgPtr,
            U32_t          msgSize,
            S32_t          timeout,
            S32_t          urgent)
{
    SIW_MQID_t       mqId = NULL;
    SYS_MSGQ_ENTRY_t *mqTblPtr = NULL;
    U32_t            mqIndex = (U32_t)msgQId;
    S32_t            lockKey;
    BOOL_t           isIntContext;
    STATUS_t         rc;
    S32_t            numMsgs;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQSend, mqId, msgPtr, msgSize, timeout, urgent);

    mqId = (SIW_MQID_t)(mqIndex & SIW_MSGQ_UNRELIABLE_MASK);
    mqIndex = (mqIndex & SIW_MSGQ_UNRELIABLE_MASK);
 
    /* Check if SIW MSGQ ID is in range */
    if ( mqIndex >= MAX_MSGQ_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), mqId);
        return(ERROR);
    }

    /* check msg buffer pointer */
    if ((msgPtr == NULL) || (msgSize != sizeof(MSG_BUF_HDR_t)))
    {
        return (ERROR);
    }

    /* Get table entry pointer */
    mqTblPtr = &sysMsgQTable[mqIndex];

    /* Check mqId */
    if ( mqId != mqTblPtr->siwMqId )
    {
        /* Check if could be deleted siwMqId */
        if ( (U32_t)mqId < sysNextMqid )
        {
            /* Log warning event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDDELETED), mqId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), mqId);
        }

        return(ERROR);
    }

    /* Get interrupt context */
    isIntContext = intContext();

    /* Tasklock if not interrupt context */
    if ( isIntContext == FALSE )
    {
        /* lock task from swabed */
        taskLock();

        /* fill in srcTaskId */
        msgPtr->srcTaskId = SiwTaskIdSelf();
    }
    else
    {
        /* fill in srcTaskId */
        msgPtr->srcTaskId = NULL;
#if 0
        if (((workQWriteIx - workQReadIx) & 0xff) > 230)
            return(ERROR);
#endif
    }

#ifdef SIM
    msgPtr->spare2 = (U32_t)tickGet();
#else
    msgPtr->spare2 = (U16_t)SysClkTickGet();
#endif

#if 0
    while (SiwMsgQNumMsgs(mqId) >= mqTblPtr->mqPara.maxMsgs) 
    {
        if ((mqIndex & SIW_MSGQ_UNRELIABLE) == SIW_MSGQ_UNRELIABLE)
        {
            /* taskUnlock if not interrupt context */
            if ( isIntContext == FALSE )
            {
                taskUnlock();
                /* Set failing task */
                mqTblPtr->fTask = SiwTaskIdSelf();
            }
            else
            {
                mqTblPtr->fTask = (SIW_TID_t) NULL;
            }

            return(TIMEOUT);
        }
     
        if (timeout != WAIT_FOREVER)
        {
            if (toCnt == 0)
            {
                /* taskUnlock if not interrupt context */
                if ( isIntContext == FALSE )
                {
                    taskUnlock();

                    /* Set failing task */
                    mqTblPtr->fTask = SiwTaskIdSelf();
                }
                else
                {
                    mqTblPtr->fTask = (SIW_TID_t) NULL;
                }

                return(TIMEOUT);
            }
            else
            {
                toCnt--;
            }
        }
		SiwTaskDelay(1);
    }
#endif

    /* Send message to Vxworks msgQ */
    rc = msgQSend(mqTblPtr->msgQId, (char *)msgPtr, msgSize, timeout,
                  (urgent ? MSG_PRI_URGENT : MSG_PRI_NORMAL));

    if ( rc == ERROR )
    {
        /* Update failing stats */
        mqTblPtr->sfCount++;
        if ( isIntContext == FALSE )
        {
            /* Unlock tasklock */
            taskUnlock();
            /* Set failing task */
            mqTblPtr->fTask = SiwTaskIdSelf();
        }
        else
        {
            mqTblPtr->fTask = (SIW_TID_t) NULL;
        }
        mqTblPtr->fErrno = errno;

        if ( errno == S_objLib_OBJ_UNAVAILABLE )
        {
            return(TIMEOUT);
        }

        if ( errno == S_msgQLib_INVALID_MSG_LENGTH )
        {
            /* Log error message */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQSENDLENINV), msgSize, mqId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQSENDFAIL), mqTblPtr->msgQId,
                     mqId);
        }
            
        /* Return error status */
        return(ERROR);
    }

    /* Update Xmt stats */
    numMsgs = SiwMsgQNumMsgs(mqId);
    lockKey = intLock();
    mqTblPtr->msgXmtd++;
    mqTblPtr->qDelay -= SysHrTimeGet();
    if (numMsgs > (S32_t)mqTblPtr->maxQDepth)
    {
           mqTblPtr->maxQDepth = numMsgs;
    }
    intUnlock(lockKey);

#if 0
    /* Get owning task entry */
    taskIndex = (U32_t)mqTblPtr->mqPara.ownerTaskId;

    /* Give task binary semaphore */
    rc = semGive(sysTaskTable[taskIndex].sembId);
    if ( rc == ERROR )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQSEMGIVEFAIL),
                 sysTaskTable[taskIndex].sembId, msgQId);
    }
#endif

    /* TaskUnlock if not interrupt context */
    if ( isIntContext == FALSE )
    {
        taskUnlock();
    }

    return(OK);
}

/***********************************************************************
 *  Function:    SiwMsgQReceive()
 *  Description: This function receives a message from the specified
 *               message queue. If there are no messages in the queue
 *               it will wait for up to the clock tick of timeout value
 *               before returning a TIMEOUT. The function returns the
 *               size of the message or ERROR on an error condition or
 *               TIMEOUT when a timeout occurs.
 *
 *  In Parms:    SIW_MQID_t msgQId - message queue from which to receive
 *                                   message
 *               MSG_BUF_HDR_t *bufPtr - pointer to a buffer to receive message
 *               U32_t msgSize  - size of passed message buffer
 *               S32_t timeout   - amount of time in clock ticks to wait
 *  Out Parms:   None
 *  Returns:     Size of received message, ERROR, or TIMEOUT
 **********************************************************************/
S32_t
SiwMsgQReceive(SIW_MQID_t     msgQId,
               MSG_BUF_HDR_t *msgBufPtr,
               U32_t          msgBufSize,
               S32_t          timeout)
{
    S32_t            len;
    U32_t            mqIndex = (U32_t)msgQId;
    S32_t            lockKey;
    SYS_MSGQ_ENTRY_t *mqTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQReceive, msgQId, msgBufPtr, msgBufSize, timeout, 0);

    /* Check that msgQId is in range */
    if ( mqIndex >= MAX_MSGQ_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        return(ERROR);
    }

    /* Get table entry pointer */
    mqTblPtr = &sysMsgQTable[mqIndex];

    /* Check that msgQId has correct tag value */
    if ( msgQId != mqTblPtr->siwMqId )
    {
        /* Check if could be deleted siwMqId */
        if ( (U32_t)msgQId < sysNextMqid )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDDELETED), msgQId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        }

        return(ERROR);
    }

    /* Receive from msgQ */
    len = msgQReceive(mqTblPtr->msgQId, (char *)msgBufPtr, msgBufSize,
                      timeout);

    /* Check if error received */
    if ( len == ERROR )
    {
        if ( errno == S_objLib_OBJ_TIMEOUT ||
             errno == S_objLib_OBJ_UNAVAILABLE )
        {
            return(TIMEOUT);
        }

        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
#if 0
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwMsgQReceive");
#endif
        }
        else if ( errno == S_msgQLib_INVALID_MSG_LENGTH )
        {
#if 0
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQRECVLENINV), msgBufSize, msgQId);
#endif
        }
        else
        {
#if 0
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQRECVFAIL), mqTblPtr->msgQId,
                     msgQId);
#endif
        }
        
        /* Return error status */
        return(ERROR);
    }

    /* Update statistics */
    lockKey = intLock();
    mqTblPtr->msgRcvd++;
    mqTblPtr->qDelay += SysHrTimeGet();
    intUnlock(lockKey);
    
    return(len);
}

/***********************************************************************
 *  Function:    SiwMsgQNameToId()
 *  Description: This function looks for a message queue of the
 *               specified queue name and returns a message queue
 *               id if found.
 *
 *  In Parms:    char * queueName - name of message queue to locate
 *  Out Parms:   None
 *  Returns:     A valid SIW_MQID_t or SIW_INVALID_MQID
 **********************************************************************/
SIW_MQID_t
SiwMsgQNameToId(char *queueName)
{
    STATUS_t rc;
    U32_t    mqIndex = 1;
    char    *strPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQNameToId, queueName, 0, 0, 0, 0);
 
    /* Get semaphore to search table for available entry */
    rc = semTake(sysMsgQTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwMsgQNameToId");
        } /* end if errno == S_intLib_NOT_ISR_CALLABLE */
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "MsgQ",
                        sysMsgQTblSem, errno);
        } /* end else */
        return(SIW_INVALID_MQID);
    } /* end if rc == ERROR */

    /* Search through semaphore table */
    while ( mqIndex < MAX_MSGQ_ENTRIES )
    {
        /* Get string pointer */
        strPtr = sysMsgQTable[mqIndex].mqPara.queueName;

        /* Check if entry is valid and matches */
        if ( sysMsgQTable[mqIndex].siwMqId != SIW_INVALID_MQID &&
             strPtr != NULL && strcmp(strPtr, queueName) == 0 )
        {
            /* Release table semaphore */
            semGive(sysMsgQTblSem);

            return(sysMsgQTable[mqIndex].siwMqId);
        }

        /* Increment index pointer */
        mqIndex++;
    }

    /* Release table semaphore */
    semGive(sysMsgQTblSem);

    return(SIW_INVALID_MQID);
} 


/***********************************************************************
 *  Function:    SiwMsgQNumMsgs()
 *  Description: This function is used to get the number of messages
 *               in the specified message queue.
 *
 *  In Parms:    SIW_MQID_t msgQId - message queue to examine
 *  Out Parms:   None
 *  Returns:     The number of messages queued or ERROR
 **********************************************************************/
S32_t
SiwMsgQNumMsgs(SIW_MQID_t msgQId)
{
    S32_t            len;
    U32_t            mqIndex = (U32_t)msgQId;
    SYS_MSGQ_ENTRY_t *mqTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQNumMsgs, msgQId, 0, 0, 0, 0);

    /* Check that msgQId is in range */
    if ( mqIndex >= MAX_MSGQ_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        return(ERROR);
    }

    /* Get table entry pointer */
    mqTblPtr = &sysMsgQTable[mqIndex];

    /* Check that msgQId has correct tag value */
    if ( msgQId != mqTblPtr->siwMqId )
    {
        /* Check if could be deleted siwMqId */
        if ( (U32_t)msgQId < sysNextMqid )
        {
            /* Log warning event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDDELETED), msgQId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        }

        return(ERROR);
    }

    /* Get number of msgs in queue */
    len = msgQNumMsgs(mqTblPtr->msgQId);

    /* Check if error returned */
    if ( len == ERROR )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQNUMMSGFAIL), mqTblPtr->msgQId,
                 msgQId);
    }

    return(len);
}

/***********************************************************************
 *  Function:    SiwMsgQInfoGet()
 *  Description: This function is used to get information about the
 *               state and contents of a message queue.
 *
 *  In Parms:    SIW_MQID_t msgQId - message queue to query
 *  Out Parmes:  SIW_MQ_INFO_t *msgQInfo - where to return message information
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwMsgQInfoGet(SIW_MQID_t    msgQId,
               SIW_MQ_INFO_t *msgQInfoPtr)
{
    STATUS_t         rc;
    U32_t            mqIndex = (U32_t)msgQId;
    MSG_Q_INFO       mqInfo;
    SYS_MSGQ_ENTRY_t *mqTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQInfoGet, msgQId, msgQInfoPtr, 0, 0, 0);

    /* Check that msgQId is in range */
    if ( mqIndex >= MAX_MSGQ_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        return(ERROR);
    }

    /* Get table entry pointer */
    mqTblPtr = &sysMsgQTable[mqIndex];

    /* Check that msgQId has correct tag value */
    if ( msgQId != mqTblPtr->siwMqId )
    {
        /* Check if could be deleted siwMqId */
        if ( (U32_t)msgQId < sysNextMqid )
        {
            /* Log warning event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDDELETED), msgQId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        }

        return(ERROR);
    }

    /* Check that info ptr is not Null */
    if ( msgQInfoPtr == NULL )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQINFOPTRINV), msgQInfoPtr,
                 msgQId);
        return(ERROR);
    }

    /* Clear mqInfo structure */
    memset(&mqInfo, 0, sizeof(mqInfo));

    /* Get msgQ info */
    rc = msgQInfoGet(mqTblPtr->msgQId, &mqInfo);

    /* Check if error returned */
    if ( rc == ERROR )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQINFOFAIL), mqTblPtr->msgQId,
                 msgQId, errno);
        return(ERROR);
    }

    /* Copy from Vxworks structure to passed siw structure */
    msgQInfoPtr->numMsgs = mqInfo.numMsgs;
    msgQInfoPtr->numTasks = mqInfo.numTasks;
    msgQInfoPtr->maxMsgs = mqInfo.maxMsgs;
    msgQInfoPtr->maxMsgLength = mqInfo.maxMsgLength;

    return(OK);
}

/***********************************************************************
 *  Function:    SiwMsgQId()
 *  Description: This function is used to get vxWorks message queue
 *               ID.
 *
 *  In Parms:    SIW_MQID_t msgQId - SIW message queue ID
 *  Out Parms:   None
 *  Returns:     MSG_Q_ID or NULL
 ***********************************************************************/
MSG_Q_ID SiwMsgQId(SIW_MQID_t msgQId)
{
    U32_t      mqIndex = (U32_t)msgQId;
    STATUS_t   rc;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQId, msgQId, 0, 0, 0, 0);

    /* Check that msgQId is in range */
    if ( mqIndex >= MAX_MSGQ_ENTRIES )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        return(NULL);
    }

    /* Get semaphore to search table for available entry */
    rc = semTake(sysMsgQTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwMsgQId");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "MsgQ",
                        sysMsgQTblSem, errno);
        } 
        return(NULL);
    } 

    /* Search through semaphore table */
    mqIndex = 1;
    while ( mqIndex < MAX_MSGQ_ENTRIES )
    {
        /* Check if entry is valid and matches */
        if ( sysMsgQTable[mqIndex].siwMqId == msgQId)
        {
            /* Release table semaphore */
            semGive(sysMsgQTblSem);

            /* Return MSG_Q_ID */
            return(sysMsgQTable[mqIndex].msgQId);
        }

        /* Increment index pointer */
        mqIndex++;
    }

    /* Release table semaphore */
    semGive(sysMsgQTblSem);

    /* No found - Return invalid msgQ id */
    return(NULL);
}

/***********************************************************************
 *  Function:    SiwTaskNameToMsgQId()
 *  Description: This function is used to get the SIW message queue
 *               ID for the specific task.
 *
 *  In Parms:    char *taskName - task name
 *  Out Parms:   None
 *  Returns:     SIW_MQID_t or SIW_INVALID_MQID
 ***********************************************************************/
SIW_MQID_t SiwTaskNameToMsgQId(char *taskName)
{
    U32_t      mqIndex;
    SIW_TID_t  taskId;
    STATUS_t   rc;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTaskNameToMsgQId, taskName, 0, 0, 0, 0);

    /* get SIW task ID */
    if ((taskId = SiwTaskNameToId(taskName)) == SIW_INVALID_TID)
    {
        return (SIW_INVALID_MQID);
    }

    /* Get semaphore to search table for available entry */
    rc = semTake(sysMsgQTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwMsgQId");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "MsgQ",
                        sysMsgQTblSem, errno);
        } 
        return(SIW_INVALID_MQID);
    } 

    /* Search through semaphore table */
    mqIndex = 1;
    while ( mqIndex < MAX_MSGQ_ENTRIES )
    {
        /* Check if entry is valid and matches */
        if ( sysMsgQTable[mqIndex].cTask == taskId)
        {
            /* Release table semaphore */
            semGive(sysMsgQTblSem);

            /* Return SIW MSGID */
            return(sysMsgQTable[mqIndex].siwMqId);
        }

        /* Increment index pointer */
        mqIndex++;
    }

    /* Release table semaphore */
    semGive(sysMsgQTblSem);

    /* No found - Return invalid msgQ id */
    return(SIW_INVALID_MQID);
}

/***********************************************************************
 *  Function Name:  SysMsgQCreate
 *  Description: This function actually create the message queue that
 *               has been registered in the message queue table. The
 *               static parameters area of the table must be filled in
 *               before calling this function. The SIW_MQID in field
 *               siwMqId being used for this entry must also be set in
 *               the table entry before calling this function. The
 *               parameters are what SIW_MQID is being created and where
 *               the function is being called from in the code. This
 *               information is stored to indicate what code created
 *               the message queue. This function logs an error event
 *               for all failures. 
 *
 *  In Parms:    SIW_MQID_t tblMqId - table index that is to create message
 *                                    queue
                 S32_t option - message queue option
 *               ADDRS_t caller - location of caller address that is creating
 *  Out Parms:   None
 *  Returns:     A valid SIW_MQID_t or SIW_INVALID_MQID if failure
 **********************************************************************/
SIW_MQID_t
SysMsgQCreate(SIW_MQID_t  tblMqId,
              S32_t       option)
{
    U32_t            mqIndex = (U32_t)tblMqId;
    U32_t            taskIndex;
    SEM_ID           semId;
    MSG_Q_ID         msgQId;
    SYS_MSGQ_ENTRY_t **mqChainPtr;
    SYS_MSGQ_ENTRY_t *mqTblPtr;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SysMsgQCreate, tblMqId, option, 0, 0, 0);
 
    /* Get table entry */
    mqTblPtr = &sysMsgQTable[mqIndex];

    /* Check message queue size requested */
    if ( mqTblPtr->mqPara.maxMsgs <= 0 ||
         mqTblPtr->mqPara.maxMsgs > SYS_MAX_QUEUE_DEPTH )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQMAXINVALID),
                 mqTblPtr->mqPara.maxMsgs);
        return(SIW_INVALID_MQID);
    }

    /* Check message length requested */
    if ( mqTblPtr->mqPara.maxMsgLength < (int)sizeof(MSG_BUF_HDR_t) ||
         mqTblPtr->mqPara.maxMsgLength > (int)SIW_MAX_MESSAGE_SIZE )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQMLENINVALID),
                 mqTblPtr->mqPara.maxMsgLength);
        return(SIW_INVALID_MQID);
    }

    /* Check that owner task is in valid range */
    taskIndex = (U32_t)mqTblPtr->mqPara.ownerTaskId;
    if ( taskIndex > MAX_TASK_ENTRIES ||
         taskIndex == 0 )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQOWNERINVALID),
                 mqTblPtr->mqPara.ownerTaskId);
        return(SIW_INVALID_MQID);
    }

    /* Check that task is created if not static id */
    if ( taskIndex > NUM_STATIC_TASKS &&
         sysTaskTable[taskIndex].siwTID !=
         mqTblPtr->mqPara.ownerTaskId )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQOWNERNOTEXIST),
                 mqTblPtr->mqPara.ownerTaskId);
        return(SIW_INVALID_MQID);
    }
    
    /* Create VxWorks message queue */
    msgQId = msgQCreate(mqTblPtr->mqPara.maxMsgs,
                        mqTblPtr->mqPara.maxMsgLength,
                        option);

    /* Check results of message queue create */
    if ( msgQId == NULL )
    {
        if ( errno == S_memLib_NOT_ENOUGH_MEMORY )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, OBJCREATENOMEM), "Message Queue");
        }
        else
        {
            /* Log erro event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQCREATEFAIL), errno);
        }

        return(SIW_INVALID_MQID);
    }

    /* Create binary semaphore if necessary */
    if ( sysTaskTable[taskIndex].sembId == NULL )
    {
        semId = semBCreate(SIW_SEM_Q_FIFO, SEM_EMPTY);
        /* Check creation status */
        if ( semId == NULL )
        {
            if ( errno == S_memLib_NOT_ENOUGH_MEMORY )
            {
                /* Log error event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, OBJCREATENOMEM),
                         "Sync Semaphore");
            }
            else
            {
                /* Log erro event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQSEMCRTFAIL), errno);
            }
            /* Delete message queue */
            msgQDelete(msgQId);
            
            return(SIW_INVALID_MQID);
        }
        sysTaskTable[taskIndex].sembId = semId;
    }

    /* Set msgQId in table entry */
    mqTblPtr->msgQId = msgQId;

    /* Add message queue to task mq chain */
    mqChainPtr = &sysTaskTable[taskIndex].mqChain;
    while ( *mqChainPtr != NULL )
    {
        /* Compare priorities in list with new entry */
        if ( (*mqChainPtr)->mqPara.priority <
             mqTblPtr->mqPara.priority )
        {
            /* Found location to insert */
            break;
        }

        /* Go to next entry on list */
        mqChainPtr = &((*mqChainPtr)->mqNext);
    }
    
    /* Insert rest of chain onto current entry */
    mqTblPtr->mqNext = *mqChainPtr;

    /* Link current entry to previous entry or head */
    *mqChainPtr = mqTblPtr;

    /* Initialize task quote value */
    mqTblPtr->quota = SYS_MAX_QUEUE_DEPTH;
    
    /* Initialize task creator values */
    mqTblPtr->cTask = SiwTaskIdSelf();
    mqTblPtr->cTime = time(NULL);

    /* Initialize other fields */
    mqTblPtr->qProc = 0;
    mqTblPtr->msgRcvd = 0;
    mqTblPtr->msgXmtd = 0;
    mqTblPtr->fTask = SIW_INVALID_TID;
    mqTblPtr->fErrno = 0;
    mqTblPtr->qDelay = 0;
    mqTblPtr->maxQDepth = 0;

    return(tblMqId);
}

/***********************************************************************
 *  Function:    SysMsgQFlush()
 *  Description: This function flushes all the messages out of the
 *               specified VxWorks message queue and frees any resources
 *               attached to the messages. It returns a count of the
 *               number of messages flushed from the message queue or
 *               ERROR on an error condition. When ERROR is returned,
 *               an error event has already been logged.
 *
 *  In Parms:    MSG_Q_ID msgQId - VxWorks message queue id to flush
 *  Out Parms:   None
 *  Returns:     Number of flushed messages or ERROR
 **********************************************************************/
S32_t
SysMsgQFlush(MSG_Q_ID msgQId)
{
    S32_t   count = 0;
    BOOL_t  done = FALSE;
    S32_t   len;
    char    msgBuf[SIW_MAX_MESSAGE_SIZE];

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SysMsgQFlush, msgQId, 0, 0, 0, 0);
 
    /* Receive messages until empty */
    while ( done == FALSE )
    {
        len = msgQReceive(msgQId, &msgBuf[0], sizeof(msgBuf), NO_WAIT);
        /* Check if error returned */
        if ( len == ERROR )
        {
            /* Check if queue is empty */
            if ( errno == S_objLib_OBJ_UNAVAILABLE )
            {
                /* Queue is empty */
                done = TRUE;
            }
            else if ( errno == S_intLib_NOT_ISR_CALLABLE )
            {
                /* Log error event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwMsgQFlush");
            }
            else
            {
                /* Log error event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQFLSHRECVFAIL), msgQId,
                         errno);
                return(ERROR);
            }
        }
        else
        {
            /* Update flush count */
            count++;
            
            /* Free any attached buffers to message */
            SiwFreeMem(((MSG_BUF_HDR_t *)msgBuf)->msgBufPtr);
        }
    }

    return(count);
}

/***********************************************************************
 *  Function:    SiwMsgQSummary()
 *  Description: This function prints information for each message
 *               queue, including ID, name, high water mark, and size.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
LOCAL STATUS_t
SiwMsgQSummary(void)
{
    U32_t               mqIndex;
    SYS_MSGQ_ENTRY_t    *mqTblPtr;
    char                nameBuf[SIW_MQ_NAME_SIZE];
    S32_t               maxMsgsBuf;
    U32_t               maxQDepthBuf;
    U32_t               queuedBuf;
    STATUS_t            rc;
    MSG_Q_INFO          mqInfo;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQSummary, 0, 0, 0, 0, 0);
 
    printf ("%4s  %-16s  %7s  %5s  %5s\n", "msgQ", "name", "queued", "high",
            "avail");

    for (mqIndex = 0; mqIndex < MAX_MSGQ_ENTRIES; mqIndex ++)
    {
        /* Get table entry pointer */
        mqTblPtr = &sysMsgQTable[mqIndex];

        /* Check that mqIndex has correct tag value */
        if ((mqIndex != (U32_t)mqTblPtr->siwMqId) || !mqTblPtr->msgQId)
        {
            continue;
        }

        strcpy(nameBuf, mqTblPtr->mqPara.queueName);
        maxQDepthBuf = mqTblPtr->maxQDepth;
        maxMsgsBuf   = mqTblPtr->mqPara.maxMsgs;

        /* Get msgQ info */
        bzero((void *)&mqInfo, sizeof(mqInfo));
        rc = msgQInfoGet(mqTblPtr->msgQId, &mqInfo);
        queuedBuf = mqInfo.numMsgs;

        if (rc == ERROR)
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQINFOFAIL), mqTblPtr->msgQId,
                     mqIndex, errno);
            return(ERROR);
        }

        /* Double check that mqIndex has correct tag value (not deleted) */
        if (mqIndex != (U32_t)mqTblPtr->siwMqId)
        {
            continue;
        }

        printf ("%4d  %-16s  %7d  %5d  %5d\n", mqIndex, nameBuf,
                mqInfo.numMsgs, maxQDepthBuf, maxMsgsBuf);
    }
    return (OK);
}

/***********************************************************************
 *  Function:    SiwMsgQShowReset()
 *  Description: This function resets high water marks for each message
 *               queue.  High water marks are displayed by SiwMsgQShow().
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     OK
 **********************************************************************/
STATUS_t
SiwMsgQShowReset(void)
{
    int mqIndex;

    for (mqIndex = 0; mqIndex < MAX_MSGQ_ENTRIES; mqIndex ++)
    {
        sysMsgQTable[mqIndex].maxQDepth = 0;
    }
    return (OK);
}

/***********************************************************************
 *  Function:    SiwMsgQShow()
 *  Description: This function shows the task information for the
 *               specified task to the standard output for the task.
 *               The level is used to control the amount of detail.
 *               Level 0 is used to display summary info for all queues.
 *               Level 1 is used to display detail info for specified queue.
 *
 *  In Parms:    S32_t level - level of information to show
 *
 *               SIW_MQID_t msgQId - message queue id to show information
 *                                 about
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
STATUS_t
SiwMsgQShow(S32_t       level,
            SIW_MQID_t  msgQId)
{
    STATUS_t         rc = OK;
    U32_t            mqIndex = (U32_t)msgQId;
    char             tmpstr[MAX_SYS_SYM_LEN + 1];
    char             *strPtr;
    struct tm        timeValue;
    SIW_TID_t        taskId;
    SYS_MSGQ_ENTRY_t *mqTblPtr;
    U32_t            count;
    U32_t            average;
    U64_t            queueDelay;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwMsgQShow, level, msgQId, 0, 0, 0);
 
    /* Check if summary requested */
    if (level == 0)
    {
        return (SiwMsgQSummary());
    }

    /* Check that msgQId is in range */
    if ( mqIndex >= MAX_MSGQ_ENTRIES )
    {
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        return(ERROR);
    }

    /* Get table entry pointer */
    mqTblPtr = &sysMsgQTable[mqIndex];
    
    /* Check that msgQId has correct tag value */
    if ( msgQId != mqTblPtr->siwMqId )
    {
        /* Check if could be deleted siwMqId */
        if ( (U32_t)msgQId < sysNextMqid )
        {
            /* Log warning event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDDELETED), msgQId);
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, MQIDINVALID), msgQId);
        }

        return(ERROR);
    }

    /* Call msgQShow */
    if ( level == 1 )
    {
        printf("\nSIW_MQID            : %p", msgQId);  
        rc = msgQShow(mqTblPtr->msgQId, level);
        if ( rc == ERROR )
        {
            /*errno = SIW_INTERNAL_FAILURE;*/
        }
    }

    /* Display creation and current take status */
    if ( level == 1 )
    {
        localtime_r(&mqTblPtr->cTime, &timeValue);
        strftime(&tmpstr[0], MAX_SYS_SYM_LEN, "%m/%d/%Y %H:%M:%S",
                 &timeValue);

        printf("CreationTime          FailCount   LastTask   Errno\n");
        printf("%19.19s   ", &tmpstr[0]);

        if ( mqTblPtr->sfCount > 0 )
        {
            if ( mqTblPtr->fTask != (SIW_TID_t) NULL )
            {
                taskId = mqTblPtr->fTask;
                strPtr = SiwTaskName(taskId);
            }
            else
            {
                strPtr = "ISR";
                taskId = SIW_INVALID_TID;
            }
        
            if ( strPtr != NULL )
            {
                printf("%6.6d       %-10.10s %d", 
                        mqTblPtr->sfCount, strPtr, mqTblPtr->fErrno);
            }
            else
            {
                printf("%6.6d       %10p %d", 
                        mqTblPtr->sfCount, taskId, mqTblPtr->fErrno);
            }
        }
        else
        {
            printf("%6.6d         N/A     %d\n\n", 
                    mqTblPtr->sfCount, mqTblPtr->fErrno);
        }
    }

    /* Display Performance stats */
    if ( level == 1 )
    {
        printf("Xmtd    Recvd   AveDelay\n");
    }

    if ( level == 1 || level == 2 )
    {
        count = msgQNumMsgs(mqTblPtr->msgQId);
        queueDelay = mqTblPtr->qDelay + (count * SysHrTimeGet());
        count += mqTblPtr->msgRcvd;
        if ( count != 0 )
        {
#if 0
            average = queueDelay / (count * SYS_CLOCKS_PER_USEC);
#else
            average = 0;
#endif
        }
        else
        {
            average = 0;
        }
        printf("%6.6d  %6.6d  %8.8d(uSec)\n", mqTblPtr->msgXmtd,
               mqTblPtr->msgRcvd, average);
    }

    /* Display high water mark */
    if ( level == 1 )
    {
        printf("\nMaximum Queued      : %d\n\n", mqTblPtr->maxQDepth);
    }

    return(rc);
}

void SiwMsgQPrint(SIW_MQID_t msgQId)
{
    int               nMsgs, i;
    U32_t             mqIndex = (U32_t)msgQId;
    STATUS_t          status = OK;
    MSG_BUF_HDR_t     msgBuf;

    /* Check that msgQId is in range */
    if ( mqIndex >= MAX_MSGQ_ENTRIES )
    {
        printf("invalid SIW MsgQ ID: %p\n", msgQId);
        return;
    }

    nMsgs = SiwMsgQNumMsgs(msgQId);

    for (i=0; i<nMsgs; i++)
    {
        status = SiwMsgQReceive(msgQId, &msgBuf, sizeof(MSG_BUF_HDR_t), NO_WAIT);
        if (status == sizeof(MSG_BUF_HDR_t)) 
        {
            printf("srcTaskId: %p, msgBufPtr: %p, msgType: %d, timestamp: %d\n",
                    msgBuf.srcTaskId, msgBuf.msgBufPtr, msgBuf.msgType,
                    msgBuf.spare2);
            SiwMsgQSend(msgQId, &msgBuf, sizeof(MSG_BUF_HDR_t), NO_WAIT,
                        SIW_MSG_PRI_NORMAL);
        }
    }
}

