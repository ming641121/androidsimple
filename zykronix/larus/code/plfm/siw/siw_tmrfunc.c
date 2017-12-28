/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_tmrfunc.c
 *
 *System Interface Wrapper code for Timer Function
 *
 *Note:
 *
 *Author(s):
 * 
 */

#include <stdio.h>
#include <time.h>
#include <lstLib.h>
#include <errnoLib.h>
#include <intLib.h>
#include <axss_types.h>
#include <siw_if.h>
#include <sys_os.h>
#include <sys_if.h>
#include "siw_trace.h"
#include "siw_event.h"

#define TMR_ACTIVE               1
#define TMR_STOP                 2
#define TMR_RUN                  3
#define TMR_IDLE                 4
#define TMR_DELETE               5
#define TMR_PRIVATE              1
#define TMR_PUBLIC               2
#define TIMER_RELTIME            2
#define DEFAULT_TMR_TIMEOUT      10

typedef struct {
 NODE           node;
 UINT32         lapseTime;
 UINT32         residualTime;
 SIW_TMR_MQ_t   *tmrMsgQPtr;
 SIW_TMR_FUNC_t *tmrFuncPtr;
} TMR_ELEM_t;

BOOL_t tmrDebug = FALSE;

#if AHOYA
STATUS_t TmDispatcher(void);
#endif

STATUS_t TmrSetTimer(SYS_TMR_ENTRY_t *pTable, SIW_TMRID_t timerId);
void     TmrTmrFunc(timer_t timerid, int siwTimerId);
void     TmrDebugEnable(void);
void     TmrDebugDisable(void);
void     SiwTimerShow(SIW_TMRID_t timerId);

LOCAL void  TmrFreeNodes(LIST *pList);

/***********************************************************************
 *  Function:    SiwTimerCreate()
 *  Description: This function creates a timer that can be used to
 *               activate a timer function.  The new created timer is
 *               defaulted to active.  Each task is allowed to create
 *               16 timer only.
 *
 *  In Parms:    None
 *  Out Parms:   SIW_TMRID_t *pTimerId - where to return the timer ID
 *  Returns:     OK or ERROR
 ***********************************************************************/
STATUS_t
SiwTimerCreate(SIW_TMRID_t *pTimerId)
{
    STATUS_t         rc;
    BOOL_t           found = FALSE;
    U32_t            count = 0;
    S32_t            lockKey;
    WIND_TCB        *tcbPtr = NULL;
    SYS_TMR_ENTRY_t *tmrTblPtr = NULL;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTimerCreate, pTimerId, 0, 0, 0, 0);

    /* Get semaphore to access TMR Table */
    rc = semTake(sysTmrTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwTimerCreate");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "Timer",
                        sysTmrTblSem, errno);
        }
        return(ERROR);
    }

    /* search TMR Table for available entry */
    while ( found != TRUE && count < MAX_TMR_ENTRIES )
    {
        /* Get next entry pointer */
        tmrTblPtr = &sysTmrTable[sysNextTmrid];

        /* Check if the entry is available */
        if ( tmrTblPtr->siwTimerId == SIW_INVALID_TMRID )
        {
            found = TRUE;
            *pTimerId = tmrTblPtr->siwTimerId = (SIW_TMRID_t)sysNextTmrid;
        }

        /* Increment next available timerId and count checked */
        sysNextTmrid++;
        count++;

        /* Check if at the end of TMR Table */
        if ( sysNextTmrid >=  MAX_TMR_ENTRIES )
        {
            /* reset the index */
            sysNextTmrid = 0;
        }
    }

    if ( found != TRUE )
    {
        /* Release table semaphore */
        semGive(sysTmrTblSem);

        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMERTBLFULL), MAX_TMR_ENTRIES);

        return(ERROR);
    }

    /* create timer ID */
    if (timer_create(CLOCK_REALTIME, NULL, &tmrTblPtr->timerId) == ERROR)
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRCREATEFAIL), errno);

        /* invalid the entry */
        tmrTblPtr->siwTimerId = SIW_INVALID_TMRID;

        return (ERROR);
    }

    /* keep the SIW_TID of the calling task */
    tmrTblPtr->taskId = SiwTaskIdSelf();

    /* setup the init condition for the timer */
    tmrTblPtr->state = TMR_ACTIVE;
    tmrTblPtr->status = TMR_IDLE;
    tmrTblPtr->option = TMR_PRIVATE;

    /* init the list for chaining timer function */
    lstInit(&tmrTblPtr->tmrList);

    /* free semaphore */
    semGive(sysTmrTblSem);

    /* save timer creation into TCB */
    lockKey = intLock();
    tcbPtr = (WIND_TCB *)taskIdSelf();
    tcbPtr->spare2 = 1;
    intUnlock(lockKey);

    return (OK);
}

/************************************************************************
 *  Function:    SiwTimerCancel()
 *  Description: This function removes a previously created timer.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
STATUS_t
SiwTimerCancel(SIW_TMRID_t timerId)
{
    SYS_TMR_ENTRY_t *tmrTblPtr = &sysTmrTable[(U32_t)timerId];
    LIST            *pList = &tmrTblPtr->tmrList;
    STATUS          rc;
    U32_t           timerIndex = (U32_t)timerId;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTimerCancel, timerId, 0, 0, 0, 0);

    /* check if the timerId is invalid, and caller is valid user */
    if ((timerIndex >= MAX_TMR_ENTRIES) ||
        (tmrTblPtr->siwTimerId != timerId) ||
        (tmrTblPtr->taskId != SiwTaskIdSelf() && 
         tmrTblPtr->option == TMR_PRIVATE))
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRIDINVALID), timerId, "SiwTimerCancel");

        return (ERROR);
    }

    /* free all nodes from the list */
    TmrFreeNodes(pList);

    /* cancel the timer */
    timer_delete(tmrTblPtr->timerId);

    /* Get semaphore to access TMR Table */
    rc = semTake(sysTmrTblSem, WAIT_FOREVER);
    if ( rc == ERROR )
    {
        if ( errno == S_intLib_NOT_ISR_CALLABLE )
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, ISRNOTCALLABLE), "SiwTimerCreate");
        }
        else
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TBLSEMTAKEFAIL), "Timer",
                        sysTmrTblSem, errno);
        }
        return(ERROR);
    }

    /* invalid the entry */
    tmrTblPtr->siwTimerId = SIW_INVALID_TMRID;

    /* free semaphore */
    semGive(sysTmrTblSem);
    return (OK);
}

/*************************************************************************
 *  Function:    SiwTimerFuncAdd()
 *  Description: This function adds a timer function to a list that
 *               is under monitoring by the specified timer.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *               U32_t lapseTime - amount of time in ticks to lapse 
 *                                  before to activate the timer function
 *               SIW_TMR_MQ_t *tmrMsgQPtr - notify task through message queue
 *               SIW_TMR_FUNC_t *tmrFuncPtr - notify task through callback
 *                                            timer function 
 *  Out Parms:   None
 *  Returns:     SIW_TMRFUNCID_t or SIW_INVALID_TMRFUNCID
 ************************************************************************/
SIW_TMRFUNCID_t
SiwTimerFuncAdd(SIW_TMRID_t    timerId,
                U32_t          lapseTime,
                SIW_TMR_MQ_t   *tmrMsgQPtr,
                SIW_TMR_FUNC_t *tmrFuncPtr)
{
    TMR_ELEM_t       *memPtr = NULL;
    U32_t            timerIndex = (U32_t)timerId;
    SYS_TMR_ENTRY_t  *tmrTblPtr = &sysTmrTable[timerIndex];

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTimerFuncAdd, timerId, lapseTime, tmrMsgQPtr, 
                   tmrFuncPtr, 0);

    /* check if the timerId is invalid, and caller is valid user */
    if ((timerIndex >= MAX_TMR_ENTRIES) ||
        (tmrTblPtr->siwTimerId != timerId) ||
        (tmrTblPtr->taskId != SiwTaskIdSelf() && 
         tmrTblPtr->option == TMR_PRIVATE))
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRIDINVALID), timerId, "SiwTimerFuncAdd");

        return (SIW_INVALID_TMRFUNCID);
    }

    /* get a memory block */
    if ((memPtr = (TMR_ELEM_t *)SiwAllocMem(SIW_DYNAMIC, sizeof(TMR_ELEM_t), 
                                          DEFAULT_TMR_TIMEOUT)) == NULL)
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRNOMEM));

        return (SIW_INVALID_TMRFUNCID);
    }

    /* store info into the memory block */
    memPtr->tmrMsgQPtr = tmrMsgQPtr;
    memPtr->tmrFuncPtr = tmrFuncPtr;
    memPtr->lapseTime = lapseTime;
    memPtr->residualTime = lapseTime;

    /* add node to the list */
    lstAdd(&tmrTblPtr->tmrList, &memPtr->node);

    /* check status of the Timer */
    if ((tmrTblPtr->status == TMR_IDLE) &&
        (tmrTblPtr->state == TMR_ACTIVE))
    {
        /* set timer */
        if (TmrSetTimer(tmrTblPtr, timerId) == ERROR)
        {
            SiwFreeMem(memPtr);
            return (SIW_INVALID_TMRFUNCID);
        }
    }

    return ((SIW_TMRFUNCID_t)memPtr);
}

/*************************************************************************
 *  Function:    SiwTimerFuncDelete()
 *  Description: This function deletes the specified timer function
 *               from a list that is under monitoring by the specified
 *               timer.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *               SIW_TMRFUNCID_t tmrFuncId - timer function ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ************************************************************************/
STATUS_t
SiwTimerFuncDelete(SIW_TMRID_t timerId, SIW_TMRFUNCID_t tmrFuncId)
{
    U32_t           timerIndex = (U32_t)timerId;
    SYS_TMR_ENTRY_t *tmrTblPtr = &sysTmrTable[timerIndex];
    TMR_ELEM_t      *pNode = NULL;
    BOOL_t          found = FALSE;
    int             i;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTimerFuncDelete, timerId, tmrFuncId, 0, 0, 0);

    /* check if the timerId is invalid, and caller is valid user */
    if ((timerIndex >= MAX_TMR_ENTRIES) ||
        (tmrTblPtr->siwTimerId != timerId) ||
        (tmrTblPtr->taskId != SiwTaskIdSelf() && 
         tmrTblPtr->option == TMR_PRIVATE))
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRIDINVALID), timerId, "SiwTimerFuncDelete");

        return (ERROR);
    }

    /* search the node for deleting */
    for (pNode = (TMR_ELEM_t *)lstFirst(&tmrTblPtr->tmrList), i = 0;
         pNode != NULL;
         pNode = (TMR_ELEM_t *)lstNext(&pNode->node), i++)
    {
        if (pNode == (TMR_ELEM_t *)tmrFuncId)
        {
            found = TRUE;

            /* delete the node from the list */
            lstDelete(&tmrTblPtr->tmrList, &pNode->node);

            /* free the memory */
            SiwFreeMem(pNode);
            break;
        }
    }

    if (found != TRUE)
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRFUNCIDINV), timerId, tmrFuncId);

        return (ERROR);
    }

    return (OK);
}

/************************************************************************
 *  Function:    SiwTimerStart()
 *  Description: This function starts to activate a timer for a list of
 *               timer function.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
STATUS_t
SiwTimerStart(SIW_TMRID_t timerId)
{
    U32_t timerIndex = (U32_t)timerId;
    SYS_TMR_ENTRY_t *tmrTblPtr = &sysTmrTable[timerIndex];

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTimerStart, timerId, 0, 0, 0, 0);

    /* check if the timerId is invalid, and caller is valid user */
    if ((timerIndex >= MAX_TMR_ENTRIES) ||
        (tmrTblPtr->siwTimerId != timerId) ||
        (tmrTblPtr->taskId != SiwTaskIdSelf() && 
         tmrTblPtr->option == TMR_PRIVATE))
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRIDINVALID), timerId, "SiwTimerStart");

        return (ERROR);
    }

    /* set timer to ACTIVE */
    tmrTblPtr->state = TMR_ACTIVE;

    if (tmrDebug)
    {
        printf("start Timer: %p\n", timerId);
    }

    /* check status of the Timer */
    if ((tmrTblPtr->status == TMR_IDLE) &&
        (tmrTblPtr->state == TMR_ACTIVE))
    {
        /* set timer */
        return (TmrSetTimer(tmrTblPtr, timerId));
    }

    return (ERROR);
}

/************************************************************************
 *  Function:    SiwTimerStop()
 *  Description: This function stops a timer for a list of timer
 *               function.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
STATUS_t
SiwTimerStop(SIW_TMRID_t timerId)
{
    U32_t            timerIndex = (U32_t)timerId;
    SYS_TMR_ENTRY_t  *tmrTblPtr = &sysTmrTable[timerIndex];

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTimerStop, timerId, 0, 0, 0, 0);

    /* check if the timerId is invalid, and caller is valid user */
    if ((timerIndex >= MAX_TMR_ENTRIES) ||
        (tmrTblPtr->siwTimerId != timerId) ||
        (tmrTblPtr->taskId != SiwTaskIdSelf() && 
         tmrTblPtr->option == TMR_PRIVATE))
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRIDINVALID), timerId, "SiwTimerStop");

        return (ERROR);
    }

    if (tmrDebug)
    {
        printf("stop Timer: %p\n", timerId);
    }

    /* set timer to STOP */
    tmrTblPtr->state = TMR_STOP;
    return (OK);
}

/************************************************************************
 *  Function:    SiwTimerReset()
 *  Description: This function resets the timer for the submitted timer
 *               function.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
STATUS_t
SiwTimerReset(SIW_TMRID_t timerId)
{
    U32_t             timerIndex = (U32_t)timerId;
    SYS_TMR_ENTRY_t  *tmrTblPtr = &sysTmrTable[timerIndex];
    struct itimerspec timeValue;
    int               clkRate = 100;
 
    /* check if the timerId is invalid, and caller is valid user */
    if ((timerIndex >= MAX_TMR_ENTRIES) ||
        (tmrTblPtr->siwTimerId != timerId) ||
        (tmrTblPtr->taskId != SiwTaskIdSelf() && 
         tmrTblPtr->option == TMR_PRIVATE))
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRIDINVALID), timerId, "SiwTimerReset");

        return (ERROR);
    }

    timeValue.it_value.tv_sec = tmrTblPtr->timeSetting/clkRate;
    timeValue.it_value.tv_nsec = (tmrTblPtr->timeSetting % clkRate) * 10000000;
    timeValue.it_interval.tv_sec = 0;
    timeValue.it_interval.tv_nsec = 0;

    if (timer_settime(tmrTblPtr->timerId, TIMER_RELTIME, &timeValue, NULL) == ERROR)
    {
        return (ERROR);
    }

    return (OK); 
}

/*************************************************************************
 *  Function:    TmrSetTimer()
 *  Description: This function sets the time until the next expiration
 *               and arm timer.  Since not using TIMER_ABSTIME, the
 *               interval is relative to the current time.
 *
 *  In Parms:    SYS_TMR_ENTRY_t *pTable - pointer to the TMR Table
 *               SIW_TMRID_t timerId - SIW timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
STATUS_t
TmrSetTimer(SYS_TMR_ENTRY_t *pTable, SIW_TMRID_t timerId)
{
#define SIW_TIMER_DEFAULT             100
    U32_t      timerIndex = (U32_t)timerId;
    SYS_TMR_ENTRY_t  *tmrTblPtr = &sysTmrTable[timerIndex];
    TMR_ELEM_t *pNode = NULL;
    LIST       *pList = &pTable->tmrList;
    U32_t      nTime = 0;
    int        i, nodeNum, clkRate;
    struct itimerspec timeValue;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, TmrSetTimer, pTable, timerId, 0, 0, 0);

    /* check state of the Timer */
    if (pTable->state == TMR_STOP)
    {
        pTable->status = TMR_IDLE;
        if (tmrDebug)
        {
            printf("Timer[%p] is stopped\n", timerId);
        }

        timer_gettime(tmrTblPtr->timerId, &timeValue);
        SIW_DEBUG_TRACE(SIW, TmrSetTimer, 0, TRACE_LEVEL_1,
                        timeValue.it_value.tv_sec,
                        timeValue.it_value.tv_nsec,
                        timeValue.it_interval.tv_sec,
                        timeValue.it_interval.tv_nsec, 0); 
        return (OK);
    }

    /* check if there is any node in the list */
    pNode = (TMR_ELEM_t *)lstFirst(pList);
    if (pNode == NULL)
    {
        /* no available node in the list */
        pTable->status = TMR_IDLE;
        return (OK);
    }

    nTime = 100000000;  /* just setup a big number */
    nodeNum = 0;

    /* search node with minimal residual time */
    for (pNode = (TMR_ELEM_t *)lstFirst(&tmrTblPtr->tmrList), i = 0;
         pNode != NULL;
         pNode = (TMR_ELEM_t *)lstNext(&pNode->node), i++)
    {
        if (pNode->residualTime < nTime)
        {
            nTime = pNode->residualTime;
            nodeNum = i+1;
        }
    }

    if (nodeNum == 0)
    {
        /* there is no any node in the list */
        return (ERROR);
    }

    /* get the node with the minimal residual time */
    pNode = (TMR_ELEM_t *)lstNth(&tmrTblPtr->tmrList, nodeNum);

    /* arm the timer in nanosecond */
    /* clkRate = sysClkRateGet(); */
    clkRate = 100;

    if (pNode->residualTime <= 0)
    {
        if (tmrTblPtr->timeSetting > 0)
        {
            pNode->residualTime = tmrTblPtr->timeSetting;
        }
        else
        { 
            pNode->residualTime = SIW_TIMER_DEFAULT;
        }
    }

    /* save the time setting */
    tmrTblPtr->timeSetting = pNode->residualTime;

    timeValue.it_value.tv_sec = pNode->residualTime/clkRate;
    timeValue.it_value.tv_nsec = (pNode->residualTime % clkRate) * 10000000;
    timeValue.it_interval.tv_sec = 0;
    timeValue.it_interval.tv_nsec = 0;

    /* update residual time for each node in the list */
    for (pNode = (TMR_ELEM_t *)lstFirst(&tmrTblPtr->tmrList);
         pNode != NULL;
         pNode = (TMR_ELEM_t *)lstNext(&pNode->node))
    {
         pNode->residualTime -= nTime;
    }

    if (pTable->status == TMR_IDLE)
    {
        /* connect the timer */
        if (timer_connect(tmrTblPtr->timerId, TmrTmrFunc,
                          (int)timerId) == ERROR)
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRCNNTFAIL), tmrTblPtr->timerId,
                        TmrTmrFunc, errno);

            /* stop the timer */
            pTable->status = TMR_IDLE;
            pTable->state = TMR_STOP;

            /* free all nodes in the list */
            TmrFreeNodes(pList);
            return (ERROR);
        }

        /* indicating timer is running */
        pTable->status = TMR_RUN;

        if (timer_settime(tmrTblPtr->timerId, TIMER_RELTIME,
                          &timeValue, NULL) == ERROR)
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRSTMFAIL), tmrTblPtr->timerId,
                        &timeValue, errno);

            /* stop the timer */
            pTable->status = TMR_IDLE;
            pTable->state = TMR_STOP;

            /* free all nodes in the list */
            TmrFreeNodes(pList);

            return (ERROR);
        }
    }

    return (OK);
}

/*************************************************************************
 *  Function:    TmrTmrFunc()
 *  Description: This function is the signal handling routine.  After
 *               timer expired, the default signal(SIGALRM) wakes up
 *               this routine, and it then calls every user timer
 *               function with expiration of lpaseTime.
 *
 *  In Parms:    timer_t timerid - timer ID
 *               int siwTimerId - SIW timer ID
 *  Out Parms:   None
 *  Returns:     None
 **************************************************************************/
void
TmrTmrFunc(timer_t timerid, int siwTimerId)
{
    SYS_TMR_ENTRY_t *tmrTblPtr = &sysTmrTable[siwTimerId];
    LIST            *pList = &tmrTblPtr->tmrList;
    TMR_ELEM_t      *pNode = NULL;
    MSG_BUF_HDR_t   msgBuf;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, TmrTmrFunc, timerid, siwTimerId, 0, 0, 0);

    if (tmrDebug)
    {
        printf("Timer[%d] expired\n", siwTimerId);
    }

#if 0
    /* check state of the Timer */
    if (tmrTblPtr->state == TMR_STOP)
    {
        /* timer is stopped, do nothing */
        tmrTblPtr->status = TMR_IDLE;
        if (tmrDebug)
        {
            printf("and Timer is stopped\n");
        }
        return;
    }
#endif


    /* find node with expired residual time in the list */
    for (pNode = (TMR_ELEM_t *)lstFirst(pList);
         pNode != NULL;
         pNode = (TMR_ELEM_t *)lstNext(&pNode->node))
    {
        if (pNode->residualTime <= 0)
        {
            if (tmrTblPtr->state == TMR_ACTIVE)
            {
                if (pNode->tmrFuncPtr != NULL)
                {
                    /* jump to the user timer function */
                    if (pNode->tmrFuncPtr->tmrFunc != NULL)

                    {
                        (*pNode->tmrFuncPtr->tmrFunc)(pNode->tmrFuncPtr->arg1,
                                                      pNode->tmrFuncPtr->arg2,
                                                      pNode->tmrFuncPtr->arg3,
                                                      pNode->tmrFuncPtr->arg4,
                                                      pNode->tmrFuncPtr->arg5);
                    }
                    else
                    {
                        /* Log error event */
                        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRFUNCINVAL)); 
                    }
                }
                else
                {
                    if (pNode->tmrMsgQPtr == NULL)
                    {
                        /* Log error event */
                        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRMSGQINVAL)); 
                    }
                    else
                    {
                        /* send message to user */
                        msgBuf.data = (U32_t)siwTimerId;
                        msgBuf.srcTaskId = SiwTaskIdSelf();
                        msgBuf.msgBufPtr = pNode->tmrMsgQPtr->dataBuf;
                        msgBuf.msgType = TIMER_FUNC_MSG;

                        /* send message through Message Queue */
                        if (SiwMsgQSend(pNode->tmrMsgQPtr->msgQId, &msgBuf,
                                  sizeof(MSG_BUF_HDR_t), DEFAULT_TMR_TIMEOUT,
                                  (pNode->tmrMsgQPtr->urgent ? MSG_PRI_URGENT :
                                  MSG_PRI_NORMAL)) == ERROR)
                        {
                            /* Log error event */
                            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRMSGSNDFAIL), 
                                        pNode->tmrMsgQPtr->msgQId);
                        }
                    }
                }
            }

            /* reset the node */
            pNode->residualTime = pNode->lapseTime;
        }
    }

    /* set Timer is in idle */
    tmrTblPtr->status = TMR_IDLE;

    /* set timer */
    TmrSetTimer(tmrTblPtr, (SIW_TMRID_t)siwTimerId);
}

/*************************************************************************
 *  Function:    TmrFreeNodes()
 *  Description: This function frees all nodes in the list, and 
 *               returns all memory blocks.
 *
 *  In Parms:    List *pList - pointer to the list
 *  Out Parms:   None
 *  Returns:     None
 *************************************************************************/
LOCAL void
TmrFreeNodes(LIST *pList)
{
    TMR_ELEM_t  *pNode = NULL;
    int         numNodes = 0;
    int         i;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, RmrFreeNodes, pList, 0, 0, 0, 0);

    /* count the number of nodes in the list */
    numNodes = lstCount(pList);

    /* free all nodes from the list */
    for (i=0; i<numNodes; i++)
    {
        /* get node from the list */
        pNode = (TMR_ELEM_t *)lstGet(pList);

        if (pNode != NULL)
        {
            /* free the node */
            SiwFreeMem(pNode);
        }
    }
}

/************************************************************************
 *  Function:    SysTimerDelete()
 *  Description: This function deletes the timer that was created
 *               by a specific task.
 *
 *  In Parms:    SIW_TID_t taskId - task ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
void
SysTimerDelete(SIW_TID_t taskId)
{
    int             count = 0;
    SYS_TMR_ENTRY_t *tmrTblPtr = NULL;
    LIST            *pList;

    /* search TMR Table for available entry */
    while ( count < MAX_TMR_ENTRIES )
    {
        /* Get next entry pointer */
        tmrTblPtr = &sysTmrTable[count];

        /* Check if the entry is hold by the specific task */
        if ( (tmrTblPtr->siwTimerId != SIW_INVALID_TMRID) &&
             (tmrTblPtr->taskId == taskId ))
        {
            /* remove this timer from Timer Function Table */
            pList = &tmrTblPtr->tmrList;

            /* free all nodes from the list */
            TmrFreeNodes(pList);

            /* cancel the timer */
            timer_delete(tmrTblPtr->timerId);

            /* invalid the entry */
            tmrTblPtr->siwTimerId = SIW_INVALID_TMRID;
        }
        count++;
    }
}

/***********************************************************************
 *  Function:    SiwTimerShow()
 *  Description: This function creates a timer that can be used to
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     None
 ***********************************************************************/
void SiwTimerShow(SIW_TMRID_t timerId)
{
    U32_t            timerIndex = (U32_t)timerId;
    SYS_TMR_ENTRY_t  *tmrTblPtr = &sysTmrTable[timerIndex];

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTimerShow, timerId, 0, 0, 0, 0);

    /* check if the timerId is invalid, and caller is valid user */
    if (timerIndex >= MAX_TMR_ENTRIES ||
        tmrTblPtr->siwTimerId != timerId)
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRIDINVALID), timerId, "SiwTimerShow");

        return;
    }

    printf("SIW TIMER ID: %p\n", timerId);
    printf("Vxworks Timer ID: 0x%8.8x\n", (U32_t)tmrTblPtr->timerId);
    printf("Task ID: %p\n", tmrTblPtr->taskId);
    printf("Option: %d\n", tmrTblPtr->option);
    printf("Timer Job List: 0x%8.8x\n", (U32_t)&tmrTblPtr->tmrList);
    printf("Timer State: %u\n", tmrTblPtr->state);
    printf("Timer Status: %u\n", tmrTblPtr->status);
    printf("Time Setting: %d\n", tmrTblPtr->timeSetting);
}

/***********************************************************************
 *  Function:    SiwTimerPublic()
 *  Description: This function make a timer to public
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ***********************************************************************/
STATUS_t SiwTimerPublic(SIW_TMRID_t timerId)
{
    U32_t            timerIndex = (U32_t)timerId;
    SYS_TMR_ENTRY_t  *tmrTblPtr = &sysTmrTable[timerIndex];

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwTimerShow, timerId, 0, 0, 0, 0);

    /* check if the timerId is invalid, and caller is valid user */
    if (timerIndex >= MAX_TMR_ENTRIES ||
        tmrTblPtr->siwTimerId != timerId)
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, TMRIDINVALID), timerId, "SiwTimerPublic");

        return (ERROR);
    }

    tmrTblPtr->option = TMR_PUBLIC;

    return (OK);
}

/************************************************************************
 *  Function:    SiwTimerIdle()
 *  Description: This function resets the timer for the submitted timer
 *               function.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
STATUS_t
SiwTimerIdle(SIW_TMRID_t timerId)
{
    U32_t             timerIndex = (U32_t)timerId;
    SYS_TMR_ENTRY_t  *tmrTblPtr = &sysTmrTable[timerIndex];

    tmrTblPtr->timeSetting = 1;
    SiwTimerReset(timerId);
    SiwTaskDelay(2);

    return (OK);
}

void TmrDebugEnable()
{
    tmrDebug = TRUE;
}

void TmrDebugDisable()
{
    tmrDebug = FALSE;
}

#if AHOYA
STATUS_t TmDispatcher()
{
    MSG_BUF_HDR_t tmBuf;
    STATUS_t      status;

    /* send ACK back to parent */
    SiwTaskInitAck(OK);

    for (;;)
    {
        /* wait for message or sleep */
        status = SiwMsgQReceive(MSGQ2TMDISP, &tmBuf,
                               sizeof(MSG_BUF_HDR_t), WAIT_FOREVER);

        if (status == sizeof(MSG_BUF_HDR_t))
        {
            if (SiwMsgQSend((SIW_MQID_t)tmBuf.dataLen, &tmBuf,
                    sizeof(MSG_BUF_HDR_t), 10, SIW_MSG_PRI_NORMAL) != OK)
            {
if (tmrDebug)
printf("Failed to forward timer message: %u\n", tmBuf.data);
            }
        }
    }
}
#endif
