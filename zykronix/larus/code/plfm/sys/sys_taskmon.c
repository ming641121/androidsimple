/*
 *Copyright(C) 1998, All Rights Reserved, by
 *Ahoya. San Jose, CA
 *
 *Abstract:
 *
 *File Name: sys_taskmon.c
 *
 *This subsystem periodically checks tasks to insure they are
 *present and not suspended.  If any of the monitored tasks are
 *missing or suspended, it will try to recover those tasks.
 *The monitor task itself is also monitored via the hardware
 *deadman timer system.
 *
 *Note:
 *
 *Author(s):
 *
 */

#include <stdio.h>
#include <string.h>
#include <sysLib.h>
#include <taskLib.h>
#include <ioLib.h>
#include <logLib.h>

#include <axss_types.h>
#include <axss_if.h>
#include <siw_if.h>
#include <led_if.h>

#define  DEFINE_TABLES
#include <sys_if.h>
#undef   DEFINE_TABLES
 
#include <siw_events.h>
#include <siw_trace.h>
#include <sys_memmap.h>
#include <sm.h>
#include "sys_events.h"

#undef  MON_PRINT
#define SYS_MIN_MEMORY        0x00200000

#ifndef BOOT

/*****************************************************
 *   Global Variables                     
 *****************************************************/
SIW_CARD_STATE_t cardState = SIW_CARD_BOOT;
S32_t            tmEnable;
char            *errBuf;
BOOL_t           sccSwitchover = FALSE;
BOOL_t           hwWatchDogEnable = TRUE;
BOOL_t           inBoot = TRUE;

/***************************************************
 *   Static Global Variables              
 ***************************************************/
static BOOL_t  systemAlive = FALSE;

/***************************************************
 *   External Global Variables              
 ***************************************************/
extern BOOL_t  monAlive;
extern BOOL_t  hwWatchDogTiggle;
extern U32_t   monTimer;
extern BOOL_t  hwWatchDebug;
extern int     sysLockTaskId;
extern int     cpuHolderTaskId;
extern BOOL_t  sysTaskCheck;
extern int     taskHoldCount;
int            snmpTaskId;
/***************************************************
 *   Functional Prototype declaration    
 ***************************************************/
static STATUS_t RecoverTask(SIW_RECV_ACTION_t action, SYS_TASK_ENTRY_t *p);
static int      RespawnTask(SIW_TID_t, SYS_TASK_ENTRY_t *);
STATUS_t        BgndTaskCheck(void);
void            TaskMonitorOn(void);
void            TaskMonitorOff(void);
void            enablemodem(void);
void            SysTaskShow(void);

extern int      initCommStr_set();
extern int      mmsShellInit(void);
extern void     SiwTaskSpawnDone();
extern void     TrapTpTblInit(void);
extern void     TrapObjTblInit(void);
extern void     MmsTelnetInit (int maxConnNum);
extern STATUS   tffsAppInit(void);
extern void     dosFsDateTimeInstall (FUNCPTR pDateTimeFunc);
extern void     axssShellInit();
extern void     snmpd_main();
extern STATUS_t parseSysConfig();
extern STATUS_t parseTermConfig();
extern STATUS_t parseTopConfig();
extern STATUS_t sysTimeStampInit();
extern STATUS_t sys1PPsEnable();
extern int      setTrapRegMgrDefaults();

STATUS sysInit()
{
    sysClkInit();
    /* install date/time function for dosFs */
    dosFsDateTimeInstall((FUNCPTR)SysDateTimeCallback);

    tffsAppInit();

    initLED();

    sysTimeStampInit();
	
	sys1PPsEnable();

    parseSysConfig();

    parseTermConfig();

    parseTopConfig();

    return OK;
}

/**************************************************************************
 *  Function:     SysTaskMonitor()
 *  Description:  This function creates all the message queues, spawn all
 *                the tasks, and monitors all the spawned task status
 * 
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      OK or ERROR                                                 
 ****************************************************************************/
STATUS_t
SysTaskMonitor()
{
    int           i;    
    int	          initFlag = OK;
	int           bgndTaskId = 0;
    U32_t         bgndScanTime = 0;
    SIW_MQID_t    msgQId = 0;
    WIND_TCB *    tcbPtr = NULL;
    int           taskIndex = 2;
	U8_t          *tmpPtr;

    sysInit();
	/* initCommStr_set(); */
    TrapTpTblInit();
	TrapObjTblInit();

    errBuf = malloc(4096);
    memset(errBuf, 0, 4096);
    
    if (initFlag == ERROR)
    {
        strcat(errBuf, "FPGA ");
        printf("FPGA download failed\n");
    }

    /* Setup SIW environment */
    if ( SiwInit() != OK ) 
    {
        strcat(errBuf, "SiwInit ");
        initFlag = ERROR;
        printf("SIW init failed\n");
    }
    inBoot = FALSE;

    /* select PPS */
	tmpPtr = (U8_t *)MAIN_FPGA_BASE_ADDR;
	*tmpPtr = 2;
    tmpPtr++;
	*tmpPtr = 0;
	*tmpPtr = 0xff;

    /* making Root Task has the highest priority */
    taskPrioritySet(taskIdSelf(), 1);

    /* Copy siw taskId into TCB */
    tcbPtr = (WIND_TCB *)taskIdSelf();
    tcbPtr->spare1 = (S32_t)TASK_ROOT;

    /*  Create Static Message Queues  */
#ifdef MON_PRINT
    printf("Create Static Message Queue: %d\n", NUM_STATIC_MSGQS);
#endif  /* MON_PRINT */
    for (i = 0; i < NUM_STATIC_MSGQS; i++)
    {
        memcpy(&sysMsgQTable[i+1].mqPara, &sysStaticMsgQTable[i],
               sizeof(SYS_MQ_STATIC_ENTRY_t));

        sysMsgQTable[i+1].siwMqId = (SIW_MQID_t)(i+1);
        if ((msgQId = SysMsgQCreate(sysMsgQTable[i+1].siwMqId, SIW_MSG_Q_PRIORITY)
            ) == SIW_INVALID_MQID)
        {   /* Failed to create a message queue */
            /* send out a trace info */
            /* SIW_TRACE(SYS, TRACE_NORMAL, 1, 
                       "[RootTask]: static sysMsgQCreate failed, SIW_QID_t: %p", 
                       sysMsgQTable[i+1].siwMqId);*/
            strcat(errBuf, sysStaticMsgQTable[i].queueName);
            strcat(errBuf, " ");
            initFlag = ERROR;
            printf("MsgQ[%s] creation failure\n", sysStaticMsgQTable[i].queueName);
        }
	    else
	    {
#ifdef MON_PRINT
    printf("MsgQ[%s] created\n", sysStaticMsgQTable[i].queueName);
#endif  /* MON_PRINT */
	    }
    }
    
    /* Create Static Semaphores */
#ifdef MON_PRINT
    printf("Create Static Semaphore: %d\n", NUM_STATIC_SEMS);
#endif  /* MON_PRINT */
    for (i=0; i<NUM_STATIC_SEMS; i++)
    {
        memcpy(&sysSemTable[i+1].semPara, &sysStaticSemTable[i],
               sizeof(SYS_SEM_STATIC_ENTRY_t));

        if (SysSemCreate((SIW_SEMID_t)(i+1)) == SIW_INVALID_SEMID)
        {
            /* send out a trace info */
            /*SIW_TRACE(SYS, TRACE_NORMAL, 1, 
                       "[RootTask]: static sysSemCreate failed, SIW_SEMID: %p", 
                       sysSemTable[i+1].siwSemId);*/
            strcat(errBuf, sysStaticSemTable[i].semName);
            strcat(errBuf, " ");
            initFlag = ERROR;
            printf("Semaphore[%s] creation failure\n", sysStaticSemTable[i].semName);
            /* Set the siwSemId to INVALID since create failed */
            sysSemTable[i+1].siwSemId = SIW_INVALID_SEMID;
        }
        else
        {
            sysSemTable[i+1].siwSemId = (SIW_SEMID_t)(i+1);
        }
    }

    /* Create Static Tasks */
    for (i = 1; i < NUM_STATIC_TASKS; i++)
    {
#ifdef MON_PRINT
printf("tasks: %d %s %d %d %x\n", i+1, sysStaticTaskTable[i].name, 
               sysStaticTaskTable[i].taskPriority.priority, 
               sysStaticTaskTable[i].taskInfo.stackSize, 
               (UINT32)sysStaticTaskTable[i].taskInfo.entryPt);
#endif  /* MON_PRINT */        
	    /*  Spawn Task  */
        memcpy(&sysTaskTable[i+1].taskPara, &sysStaticTaskTable[i],
                sizeof(SYS_TASK_STATIC_ENTRY_t));

        sysTaskTable[i+1].siwTID = (SIW_TID_t)(i+1);
        if (SysTaskSpawn(sysTaskTable[i+1].siwTID) != SIW_INVALID_TID)
        {
#ifdef MON_PRINT
printf("Task[%s] has been created\n", sysStaticTaskTable[i].name);
#endif  /* MON_PRINT */
            if ( SysTaskWaitAck(sysTaskTable[i+1].siwTID) != OK )
            {
                SiwEventLog(NULL, SIW_EVENT_SYM(SYS, INITACKFAIL), 
                                              sysStaticTaskTable[i].name); 
                /* send out a trace info */
                /*SIW_TRACE(SYS, TRACE_NORMAL, 1, 
                  "[RootTask]: sysTaskWaitAck failed, taskName: %s", 
                  sysStaticTaskTable[i].name);*/
                strcat(errBuf, sysStaticTaskTable[i].name);
                strcat(errBuf, " ");
                initFlag = ERROR;
                printf("Received ERROR return from task %s\n", 
                                      sysStaticTaskTable[i].name);
            }
        }
        else
        {
            /* send Event Log */
            SiwEventLog(NULL, SIW_EVENT_SYM(SYS, TASKSPAWNFAIL), 
                                         sysStaticTaskTable[i].name); 
            /* send out a trace info */
            /*SIW_TRACE(SYS, TRACE_NORMAL, 2, 
             "[RootTask]: sysTaskSpawn failed, SIW_TID_t: %p, taskName: %s", 
                 sysTaskTable[i+1].siwTID, sysStaticTaskTable[i].name);*/
            strcat(errBuf, sysStaticTaskTable[i].name);
            strcat(errBuf, " ");
            initFlag = ERROR;
            printf("task %s spawn fails\n", sysStaticTaskTable[i].name);
        }
    }

    /* create Background Task */
    bgndTaskId = taskSpawn("tBgndCheck", 75, 0, 0x1000, (FUNCPTR)BgndTaskCheck,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	 
    /* enable optimization for dynamic tasks */
    SiwTaskSpawnDone();

#ifdef AHOYA
    if (initFlag == ERROR)
    {
        printf("Root/Monitor Task: Stop due to failure of task creation\n");

        taskSuspend(0);
    }
#endif
   snmpTaskId = taskSpawn("tsnmpD", 51, 0, 0x4000, (FUNCPTR)snmpd_main,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    /* enable Task Monitor feature */
    TaskMonitorOn();
    setTrapRegMgrDefaults();	
    axssShellInit();
#ifdef AHOYA
    mmsShellInit();
	MmsTelnetInit(10);    

    /* Watchdog Timer shall be disable up to here */
    /* Enable watchdog */
#endif  /* AHOYA */

	systemAlive = TRUE;
	bgndScanTime = SiwTickGet();

    for (;;) 		/* main task loop */
    {
#ifdef AHOYA
        char *testPtr = NULL;

        /* To indicate whether Monitor Task is still alive */ 
        /* defined in mipsR4kTimer.c */
        if (hwWatchDogEnable)
        {
            monAlive = TRUE;
            hwWatchDebug = TRUE;
        }

        if ((testPtr = malloc(SYS_MIN_MEMORY)) == NULL)
        {
            SiwEventLog(NULL, SIW_EVENT_SYM(SYS, MEMSHORT));
        }
        else
        {
            free (testPtr);
        }
#endif  /* AHOYA */

        /* scan monitored tasks and cycles if enabled */
        if (tmEnable)
        {
            /* check the status of background task */
            if (systemAlive)
            {
#ifdef AHOYA
                systemAlive = FALSE;
#endif /* AHOYA */
                bgndScanTime = SiwTickGet();
            }
            else if (bgndScanTime > SiwTickGet())
            {
                /* Timer is wrapped around */
                bgndScanTime = SiwTickGet();
            }
            else if ((SiwTickGet() - bgndScanTime) >= (U32_t)SECONDS(60))
            {
                /* backgroung task is in trouble */
                if (taskIsReady(bgndTaskId))
                {   /* Background Task is ready, but never get a chance to run */
#ifdef AHOYA
                    sysTaskCheck = TRUE;
                    taskDelay(100);

                    if (taskHoldCount > 80)
                    {
#if 0
                        taskSuspend(cpuHolderTaskId);
#endif
                        TaskMonitorOff();
                        SiwEventLog(NULL, SIW_EVENT_SYM(SYS, CPUISHOLD), 
                                    taskName(cpuHolderTaskId));
                    }
                    taskHoldCount = 0;
                    sysTaskCheck = FALSE;
#endif /* AHOYA */
                }
                else if (taskIsSuspended(bgndTaskId))
                {   /* Background task is suspended */
                    if (taskRestart(bgndTaskId) == ERROR)
                    {
                        /* Task get suspend and can not restart */
                        SiwEventLog(NULL, SIW_EVENT_SYM(SYS, BGNDTASKSUSPEND));
                    }
                    else
                    {
                        /* Task get suspend and restart is successful */
                        SiwEventLog(NULL, SIW_EVENT_SYM(SYS, BGNDTASKRESTART));
                    }
                } 
                bgndScanTime = SiwTickGet();
            }

            /* update index to Task Table */
            if (taskIndex >= NUM_STATIC_TASKS)
            {
                taskIndex = 2;
            }

            /* scan static tasks */
            if (sysTaskTable[taskIndex].siwTID == SIW_INVALID_TID) 
            {
                struct tm  curtm;
                int tmp;

                if (sysTaskTable[taskIndex].taskPara.suspendAction == 
                                       SIW_RECV_SOFT_RESET)
                {
                    SiwEventLog(NULL, SIW_EVENT_SYM(SYS, CTASKISDEAD),
                                 sysTaskTable[taskIndex].taskPara.name);    
                    taskDelay(100000);
                }
                else if (sysTaskTable[taskIndex].taskPara.suspendAction ==
                         SIW_RECV_LOG_ERROR)
                {
                    /* suspend action is set to log error so log an event */
                    SiwEventLog(NULL, SIW_EVENT_SYM(SYS, TASKISDEAD), 
                                sysTaskTable[taskIndex].taskPara.name);    
                    /* we don't want to flood the event log so change the suspend action */
                    /* after we log it the first time. The repeating events will be filtered */
                    /* out but the filtering will cause performance loss so let's avoid it */
                    sysTaskTable[taskIndex].taskPara.suspendAction = SIW_RECV_NO_ACTION;
                }
                else if (sysTaskTable[taskIndex].taskPara.suspendAction !=
                                       SIW_RECV_NO_ACTION)
                {
                    /* Log event error */
                    SiwEventLog(NULL, SIW_EVENT_SYM(SYS, RECVTASK),
                                sysTaskTable[taskIndex].taskPara.name, "RESPAWN TASK");

#ifdef AHOYA
                    if (hwWatchDogEnable)
                    {
                        hwWatchDogTiggle = TRUE;
                    }
#endif /* AHOYA */  
                    /* respawn task */
                    sysTaskTable[taskIndex].siwTID = (SIW_TID_t)taskIndex;
                    tmp = sysTaskTable[taskIndex].recvCntr;
                    /* get the current system time */
                    SiwDateTimeGet(&curtm);
                    /* put the info of event error into the data buffer */
                    memset(sysTaskTable[taskIndex].dateStamp, 0, DATESTAMP_SIZE + 1);
                    memset(sysTaskTable[taskIndex].timeStamp, 0, TIMESTAMP_SIZE + 1);
                    strftime(sysTaskTable[taskIndex].dateStamp, 
                                      DATESTAMP_SIZE, "%m/%d/%Y", &curtm);
                    strftime(sysTaskTable[taskIndex].timeStamp, 
                                       TIMESTAMP_SIZE, "%H:%M:%S", &curtm);

                    if (SysTaskSpawn(sysTaskTable[taskIndex].siwTID) == SIW_INVALID_TID)
                    {
                        /* Log event error */
                        SiwEventLog(NULL, SIW_EVENT_SYM(SYS, TASKISDEAD),
                                    sysTaskTable[taskIndex].taskPara.name);
                        taskDelay(100000);
                    }
                    else
                    {
                        if (SysTaskWaitAck(sysTaskTable[taskIndex].siwTID) != OK )
                        {
                            SiwEventLog(NULL, SIW_EVENT_SYM(SYS, INITACKFAIL), 
                                           sysTaskTable[taskIndex].taskPara.name);
                        }
                    } 

                    sysTaskTable[taskIndex].recvCntr = tmp;
                    sysTaskTable[taskIndex].recvCntr++;

#ifdef AHOYA
                    if (hwWatchDogEnable)
                    {
                        hwWatchDogTiggle = FALSE;
                    }
#endif /* AHOYA */
                }     
            }
            else if ((sysTaskTable[taskIndex].tid != 0) && 
                          (sysTaskTable[taskIndex].siwTID != SIW_INVALID_TID))
            {
                /* check if task is still existence */
                if ((SiwTaskIdVerify(sysTaskTable[taskIndex].siwTID) != OK) ||
                               (taskIsSuspended(sysTaskTable[taskIndex].tid) == TRUE))
                {   /* Task is disappear or suspended */
#ifdef AHOYA
                    if (hwWatchDogEnable)
                    {
                        hwWatchDogTiggle = TRUE;
                    }
#endif /* AHOYA */
                    if (RecoverTask(sysTaskTable[taskIndex].taskPara.suspendAction,
                                     &sysTaskTable[taskIndex]) == ERROR)
                    {
                        if (RecoverTask(sysTaskTable[taskIndex].taskPara.failAction, 
                                               &sysTaskTable[taskIndex]) == ERROR)
                        {
                            /* Log event error */
                            SiwEventLog(NULL, SIW_EVENT_SYM(SYS, TASKISDEAD),
                                              sysTaskTable[taskIndex].taskPara.name);
                            taskDelay(100000);
                        }
                    }
#ifdef AHOYA
                    if (hwWatchDogEnable)
                    {
                        hwWatchDogTiggle = FALSE;
                    }
#endif /* AHOYA */
                }
            }

            /* increment task index */
            taskIndex++;
        } /* End of if (tmEnable) */

        if (sysLockTaskId != 0)
        {
            taskSuspend(sysLockTaskId);
            SiwEventLog(NULL, SIW_EVENT_SYM(SYS, CPUISHOLD), taskName(sysLockTaskId));
            sysLockTaskId = 0;
            TaskMonitorOff();
        }

        taskDelay (10);

    } /* End of Main For loop */

    return (ERROR);
}

/************************************************************************
 *  Function:     BgndTaskCheck()                                 
 *  Description:  This task performs background system checking.  it is
 *                setup with the lowest priority in the system.   
 * 
 *  In Parms:     None                                                           
 *  Out Parms:    None                                            
 *  Returns:      ERROR
 ****************************************************************/
STATUS_t
BgndTaskCheck()
{
    for (;;)
    {
        systemAlive = TRUE;
        taskDelay(SECONDS(10));
    }	
 
    return (ERROR);
}

/**************************************************************************
 *  Function:     RecoverTask()
 *  Description:  This function recovers a task from disappeared or 
 *                suspended.
 * 
 *  In Parms:     SIW_RECV_ACTION_t actionType - action
 *                SYS_TASK_ENTRY_t p - pointer to the Task Table  
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 **************************************************************************/
static STATUS_t
RecoverTask(SIW_RECV_ACTION_t actionType, SYS_TASK_ENTRY_t *p)
{
     STATUS_t status = OK;
     SIW_TID_t siwTID;
     SIW_RECV_ACTION_t action = actionType;

     return OK;

     if (action == SIW_RECV_REG_FUNC)
     {
         action = (*p->taskPara.recvFunc)(p->siwTID);
     }

     switch (action) {
       case SIW_RECV_NO_ACTION:
         SiwEventLog(NULL, SIW_EVENT_SYM(SYS, RECVTASK), 
                     p->taskPara.name, "NO ACTION");
         break;

       case SIW_RECV_LOG_ERROR:
         SiwEventLog(NULL, SIW_EVENT_SYM(SYS, TASKISDEAD), 
                     p->taskPara.name);
         break;

       case SIW_RECV_RESTART_TASK:
         SiwEventLog(NULL, SIW_EVENT_SYM(SYS, RECVTASK), 
                     p->taskPara.name, "RESPAWN TASK");

          /* Delete task entry from Task Table */
          siwTID = p->siwTID;
          if (SiwTaskDelete(p->siwTID) == ERROR)
          {
              break;
          }

          /* respawn task */
          sysTaskTable[(U32_t)siwTID].siwTID = siwTID;
          if ((status = RespawnTask(siwTID, p)) == OK)
          {
              if ( SysTaskWaitAck(p->siwTID) != OK )
              {
                  SiwEventLog(NULL, SIW_EVENT_SYM(SYS, INITACKFAIL), 
                              p->taskPara.name);
                  status = ERROR;
              }
          }
 
          /* increment the task recovery counter */
          /* p->recvCntr++; */
          break;

       case SIW_RECV_SOFT_RESET:
          SiwEventLog(NULL, SIW_EVENT_SYM(SYS, CTASKISDEAD), 
                     p->taskPara.name);
          taskDelay(100000);
          break;

       default:
         SiwEventLog(NULL, SIW_EVENT_SYM(SYS, RECVTASK), 
                     p->taskPara.name, "WRONG ACTION");
     }

     return (status);
}

/****************************************************************************
 *  FUNCTION:     RespawnTask()
 *  Description:  This function respawns a task that is missing or suspended.
 * 
 *  In Parms:     SIW_TID_t siwTID - SIW task ID
 *                SYS_TASK_ENTRY_t *pTbl - pointer to the Task Table
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 ******************************************************************************/
static STATUS_t
RespawnTask(SIW_TID_t siwTID, SYS_TASK_ENTRY_t *pTbl)
{
    struct tm curtm;
    int       tmp = pTbl->recvCntr;
    STATUS_t  status = ERROR;

    memset(pTbl->dateStamp, 0, DATESTAMP_SIZE + 1);
    memset(pTbl->timeStamp, 0, TIMESTAMP_SIZE + 1);
    /* get the current system time */
    SiwDateTimeGet(&curtm);
    /* put the info of event error into the data buffer */
    strftime(pTbl->dateStamp, DATESTAMP_SIZE, "%m/%d/%Y", &curtm);
    strftime(pTbl->timeStamp, TIMESTAMP_SIZE, "%H:%M:%S", &curtm);

    if (SysTaskSpawn(siwTID) != SIW_INVALID_TID)
    {
        status = OK;
    }
    pTbl->recvCntr = tmp;
    pTbl->recvCntr++;
    return (status);
}

/**************************************************************************
 *  Function:     TaskMonitorOn()
 *  Description:  This function turns on the featur of monitoring tasks
 * 
 *  In Parms:     None 
 *  Out Parms:    None 
 *  Returns:      None
 *************************************************************************/
void
TaskMonitorOn()
{
    /* turn task monitoring on */
    tmEnable = 1;
}

/**************************************************************************
 *  Function:    TaskMonitorOff()
 *  Description: This function turns off the feature of monitoring task.
 * 
 *  In Parms:    None
 *  Out parms:   None
 **************************************************************************/
void
TaskMonitorOff() 
{
    /* turn task monitoring off */
    tmEnable = 0;
}

/**********************************************************************
 *                                                                    *
 * this is a dummy task, shall be removed after all task are register *
 *                                                                    *
 **********************************************************************/
STATUS_t SysDummy()
{
#define LONG_DELAY       1000000000
    STATUS_t status = OK;

    /* send ACK back to parent */
    status = SiwTaskInitAck(status);
    for (;;) 
    {
        taskDelay(LONG_DELAY);
    }

    return (status);
}

/*********************************************************************
 *  Funcation:   enablemodem()
 *  Description: This function initializes Robotics modem for talking
 *               to MMS-1600.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 **********************************************************************/
void enablemodem()
{
    /* BootString("atq2&e0&c0&d0&b1&h0&i0&n6&r1\n\r\0"); */
}

void SysTaskShow()
{
    int i;

    if (tmEnable)
    {
        printf("Task Monitor is enabled\n");
    }
    else
    {
        printf("Task Monitor is disabled\n");
    }

    printf("  TASKNAME    RESPAWNED COUNTER    LAST RESPAWNED TIME\n");
    /* scan all static tasks */
    for (i=2; i<NUM_STATIC_TASKS; i++)
    {
        if (sysTaskTable[i].recvCntr > 0)
        {
            printf("%10.10s          %4.4d           %10.10s-%8.8s\n",
                    sysTaskTable[i].taskPara.name,
                    sysTaskTable[i].recvCntr,
                    sysTaskTable[i].dateStamp,
                    sysTaskTable[i].timeStamp);
        }
    }
}
#endif /* !BOOT */

#ifdef BOOT
int checkAbort();
extern int goFw();
extern STATUS   tffsAppInit(void);
extern void     dosFsDateTimeInstall (FUNCPTR pDateTimeFunc);

extern 	int consoleFd;
int fwAbortFlag = 0;
int stopCheckAbort = 0;

#define ABORT_TIMEOUT 8
STATUS_t
SysBootMonitor()
{
    sysClkInit();
    /* install date/time function for dosFs */
    dosFsDateTimeInstall((FUNCPTR)SysDateTimeCallback);

    tffsAppInit();

    if (checkAbort(ABORT_TIMEOUT) == 'c') {

        goFw();
	}

	return OK;
}


void getAbortCh(MSG_Q_ID msgQId)
{
    char   ch;
    STATUS status;

    ch = getchar();

    status = msgQSend(msgQId,
                      (char *) &ch,
                      sizeof(char),
                      WAIT_FOREVER,
                      MSG_PRI_NORMAL);
}


STATUS_t checkAbort(int timeout)
{
    char     ch;
    int      tid;
    STATUS   status;
    MSG_Q_ID msgQId;
    
    /* Wilson Added: To check whether the firmware exist*/

		FILE *fp;
    fp= fopen("C:image/tssfw.bin","r");
		if (fp==NULL)
		{ 
		 printf ("The image file, tssfw.bin, does not exist in the c:image\\\n");
		 printf ("Please install the image file\n");
		 status = 'a';    /* abort */     
		 return status;      
		} 
		else
		{
		 printf ("The image file, tssfw.bin, has been installed\n"); 
		 fclose(fp);
    /* Start of Wilson Added: To check whether the firmware exist*/
    
    printf("Type 'a' or 'A' to stay in the Boot mode.   %d", timeout);

    msgQId  = msgQCreate(5, sizeof(char), MSG_Q_FIFO);

    tid = taskSpawn("getAbortCh",
                    0,
                    0,
                    0x4000,
                    (FUNCPTR) getAbortCh,
                    (int) msgQId, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    if (tid == ERROR) {
        logMsg("taskSpawn() failed.\n", 0, 0, 0, 0, 0, 0);
        return ERROR;
    }

    while (timeout > 0) {
        status = msgQReceive(msgQId, (char *) &ch, (UINT) sizeof(char), sysClkRateGet());
        if (status != ERROR) {
            status = OK;
            break;
        } else {
            printf("\b%d",timeout);
        }
        timeout--;
    }

    printf("\r\n\r\n");

    if (taskIdVerify(tid) == OK) {
        taskDelete(tid);
    }
    msgQDelete(msgQId);

    if (ch == 'a' || ch == 'A') {
        status = 'a';    /* abort */
    } else {
        status = 'c';    /* continue */
    }

    return status;
    }
}
#endif