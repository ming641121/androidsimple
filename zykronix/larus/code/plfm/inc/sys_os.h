/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: sys_os.h
 *
 *This file contains all defines for System Module 
 *
 *Note:
 *
 *Author(s):
 * 
 */

#ifndef _SYS_OS_H_
#define _SYS_OS_H_

#include <msgQLib.h>
#include <semLib.h>
#include <lstLib.h>
#include <wdLib.h>
#include <tss_types.h>
#include <tss_if.h>
#include <siw_events.h>
#include <siw_if.h>

/***********************************************************************
 * System clock define for show routine
 ***********************************************************************/
#define SYS_CLOCKS_PER_USEC  100

/***********************************************************************
 * Signal define for Event Log Module
 **********************************************************************/
#define ERROR_SIGNAL          SIGUSR2

/***********************************************************************
 *  Defines for OS object table sizes
 **********************************************************************/
#define MAX_TASK_ENTRIES      50
#define MAX_MSGQ_ENTRIES      50
#define MAX_SEM_ENTRIES       60
#define MAX_TMR_ENTRIES       30
#define MAX_RTC_ENTRIES       30

/***********************************************************************
 *  Defines for Id values in tables
 **********************************************************************/
#define SYS_TID_START_VALUE   NUM_STATIC_TASKS + 1
#define SYS_MQID_START_VALUE  NUM_STATIC_MSGQS + 1
#define SYS_SEMID_START_VALUE NUM_STATIC_SEMS + 1

/***********************************************************************
 *  Defines used for range checking
 **********************************************************************/
#define SYS_MAX_QUEUE_DEPTH   10000
#define SYS_MIN_STACK_SIZE    0x800
#define SYS_MAX_STACK_SIZE    0x10000

/***********************************************************************
 *  Defines used in task priority ranges
 **********************************************************************/
#define SYS_MAX_PRIO_VAL      20
#define SYS_PLFM_HIGH_BASE    10
#define SYS_PLFM_NORMAL_BASE  60
#define SYS_APPL_HIGH_BASE    20
#define SYS_APPL_NORMAL_BASE  80
#define SYS_BACKGROUND_BASE   100
#define SYS_APPL_PRIO_ADJUST  40

/***********************************************************************
 *  Defines used in task priority ranges
 **********************************************************************/
#define CARD_BIT(x)             (1 << x)

/***********************************************************************
 *  The following type is used to define the static task table entries
 *  to be spawned during the platform initialization process.
 **********************************************************************/
typedef struct {
    char              name[SIW_TASK_NAME_SIZE]; /* Name of task */
    SIW_PRIORITY_t    taskPriority;    /* Task priority */
    SIW_TASKINFO_t    taskInfo;        /* Task information to create
                                        * task */
    SIW_RECVFUNC_t    recvFunc;        /* recovery function */
    SIW_RECV_ACTION_t suspendAction;   /* Action to take when task
                                        *  suspends */  
    SIW_RECV_ACTION_t failAction;      /* Action to take when suspend
                                        *  action fails */
    U32_t             moduleNum;       /* Task module number */
} SYS_TASK_STATIC_ENTRY_t;

/***********************************************************************
 *  The following type is used to define the static message queue
 *  entries to be created during platform initialization process.
 **********************************************************************/
typedef struct {
    S32_t          maxMsgs;           /* Max message that can be
                                       * queued */
    S32_t          maxMsgLength;      /* Max bytes of msg in queue */
    SIW_TID_t      ownerTaskId;       /* Task id owning queue */
    S32_t          priority;          /* Priority of queue relative to
                                       *  other msg queues for same
                                       *  task */  
    char          queueName[SIW_MQ_NAME_SIZE]; /* Name of msg queue */
} SYS_MQ_STATIC_ENTRY_t;

/***********************************************************************
 *  The following type is used to define the static semaphore entries
 *  to be created during the platform initialization process.
 **********************************************************************/
typedef struct {
    char              semName[SIW_SEM_NAME_SIZE]; /* Name of semaphore */
    SIW_SEM_TYPE_t    semType;         /* Type of semaphore */
    SIW_SEM_OPTION_t  option;          /* Options set on semaphore */
    S32_t             initState;       /* Initial state of semaphore */
    SIW_RECV_ACTION_t deadlockAction;  /* Action to be taken when
                                        *  semaphore deadlocks */ 
} SYS_SEM_STATIC_ENTRY_t;

/***********************************************************************
 *  The following type is used to define the static RTC	entries.
 **********************************************************************/
typedef struct {
    char        name[SIW_RTC_NAME_SIZE]; /* Name of RTC User */
    U16_t       startLoc;
    U16_t       nBytes;
} SYS_RTC_STATIC_ENTRY_t;

/***********************************************************************
 *  Typedef of Message Queue table entry used to maintain all
 *  information about message queues in the system.
 **********************************************************************/
typedef struct sys_mqentry {
    SYS_MQ_STATIC_ENTRY_t mqPara;   /* info from static MsgQ */
    SIW_MQID_t            siwMqId;  /* SIW message queue id */
    MSG_Q_ID              msgQId;   /* VxWorks message queue id */
    struct sys_mqentry *  mqNext;   /* the next message queue id */
    U16_t                 quota;    /* max messages to be processed */
                                    /* before delay */
    U16_t                 qProc;    /* num of msgs in quota processed */
    SIW_TID_t             cTask;    /* Task that created */
    time_t                cTime;    /* Time queue was created */
    U32_t                 msgRcvd;  /* num of msgs received */
    U32_t                 msgXmtd;  /* num of msgs transmitted */
    U32_t                 sfCount;  /* num of failed sends detected */ 
    SIW_TID_t             fTask;    /* Last task to fail send msg */
    S32_t                 fErrno;   /* Last system error number */
    U32_t                 maxQDepth;/* high water mark */
    U64_t                 qDelay;   /* total ticks msgs delayed in queue */
} SYS_MSGQ_ENTRY_t;

/***********************************************************************
 *  Typedef of Task table entry used to maintain all information needed
 *  about a monitored task in the system.
 **********************************************************************/
typedef struct {
    SYS_TASK_STATIC_ENTRY_t taskPara; /* info related with task */
    S32_t                   tid;      /* VxWorks task id */
    SIW_TID_t               siwTID;   /* SIW task id */
    SIW_TID_t               parent;   /* parent task id */
    S32_t                   priority; /* base priority for task */
    SEM_ID                  sembId;   /* binary Semaphore */
    struct sys_mqentry *    mqChain;  /* the entry to the task's
                                       * message queues */ 
    SEM_ID                  spawnSem; /* the Semaphore is used for
                                       * child to ACK parent about
                                       * the completion of task
                                       * creation */
    STATUS_t                iStatus;  /* Init task status */
    BOOL_t                  procEvt;  /* Processing an event */
    char *                  errorLog; /* filename to dump stack trace */
    U32_t                   tmrCntr;  /* timer creation counter */
    U32_t                   recvCntr; /* task recovery counter */
    char                    dateStamp[DATESTAMP_SIZE+1];
                                       /* date stamp of the last time to */
                                       /* respawn task */
    char                    timeStamp[TIMESTAMP_SIZE+1];     
                                       /* time stamp of the last time to */
                                       /* respawn task */
} SYS_TASK_ENTRY_t;

/***********************************************************************
 *  Typedef of Semaphore table entry used to maintain information about
 *  semaphores in the system.
 **********************************************************************/
typedef struct {
    SYS_SEM_STATIC_ENTRY_t semPara;  /* info from static Semaphore */
    SIW_SEMID_t            siwSemId; /* SIW Semaphore id */
    SEM_ID                 semId;    /* VxWorks Semaphore id */
    SIW_RECVFUNC_t         recvFunc; /* recovery function pointer */
    SIW_TID_t              cTask;    /* Task that created */
    time_t                 cTime;    /* Time semaphore was created */
    SIW_TID_t              tTask;    /* Last task to take Semaphore */ 
    U32_t                  tRecurse; /* Number of recurvive takes */
    SIW_TID_t              ltTask;   /* Last task to take and give */
    U32_t                  tCount;   /* Number of times taken */
    U32_t                  tTicks;   /* Time in clk ticks when last
                                      * taken */
    U32_t                  dTicks;   /* Clk Ticks delayed */
    U32_t                  hTicks;   /* Clk Ticks held */
} SYS_SEM_ENTRY_t;

/***********************************************************************
 *  Typedef of Timer table entry used to maintain all information needed
 *  about a timer in the system.
 **********************************************************************/
typedef struct {
    SIW_TMRID_t siwTimerId;
    timer_t     timerId;
    SIW_TID_t   taskId;
    LIST        tmrList;
    U8_t        state;
    U8_t        status;
    U8_t        option;
    U8_t        reserved;
    U32_t       timeSetting;
} SYS_TMR_ENTRY_t;

/***********************************************************************
 *  Typedef of InitFunc table entry used to maintain all information needed
 *  needed to properly initialize a card.
 **********************************************************************/
typedef struct {
    U8_t        cardType;
    U8_t        imageType;
    S8_t        subCardType;
    S8_t        initFlag;
    FUNC_PTR_t  initFunc;
} SYS_INITFUNC_ENTRY_t;

/***********************************************************************
 *  Defines for the static task ids in static task table.
 **********************************************************************/
#define TASK_ROOT       (SIW_TID_t) 1
#define TASK_LOGD       (SIW_TID_t) 2


#define TASK_SM 		(SIW_TID_t) 3
#define TASK_FRAMER     (SIW_TID_t) 4
#define TASK_ESR        (SIW_TID_t) 5
#define TASK_OSC        (SIW_TID_t) 6
#define TASK_EMM        (SIW_TID_t) 7

#define TASK_CSM        (SIW_TID_t) 8
#define TASK_AM 		(SIW_TID_t) 9

#define TASK_TRAP       (SIW_TID_t) 9




#define NUM_STATIC_TASKS            9

/***********************************************************************
 *  Defines for the static message queue ids in static message queue
 *  table.
 **********************************************************************/
#define MSGQ2ROOT       (SIW_MQID_t) 1
#define MSGQ2LOG        (SIW_MQID_t) 2
#define MSGQ2CSM        (SIW_MQID_t) 3
#define MSGQ2TEST       (SIW_MQID_t) 4
#define MSGQ2AM         (SIW_MQID_t) 5
#define MSGQ2TRAP       (SIW_MQID_t) 5

#define NUM_STATIC_MSGQS             5

/***********************************************************************
 *  Defines for the static semaphores ids in static semaphore table.
 **********************************************************************/
#define SEM_CARDREADY   (SIW_SEMID_t) 1
#define SEM_EVENTLOG    (SIW_SEMID_t) 2
#define SEM_ERRORSLOT   (SIW_SEMID_t) 3
#define SEM_TRAPLOG     (SIW_SEMID_t) 4
#define SEM_TASKDELAY0  (SIW_SEMID_t) 5
#define SEM_TASKDELAY1  (SIW_SEMID_t) 6
#define SEM_AM		    (SIW_SEMID_t) 7

#define NUM_STATIC_SEMS               7

/***********************************************************************
 *  Defines for the static RTC ids in static RTC table.
 **********************************************************************/
#define RTC_LOG         1

/***********************************************************************
 *  Static Initialization of static task table
 **********************************************************************/
#ifndef DEFINE_TABLES
extern SYS_TASK_STATIC_ENTRY_t  sysStaticTaskTable[];
#else
#ifndef BOOT
extern STATUS_t SysDummy();
extern STATUS_t SysTaskMonitor();
extern STATUS_t SysEventTask();
extern STATUS_t CsmTask();
extern STATUS_t amTask();
extern STATUS_t inpFramerTask();
extern STATUS_t shelfTask();
extern STATUS_t esrTask(void);
extern STATUS_t oscTask(void);
extern STATUS_t emmTask(void);


SYS_TASK_STATIC_ENTRY_t sysStaticTaskTable[] = {
    { "tRootTask", { SIW_PLATFORM_SW, SIW_PRI_GRP_HIGH, ROOT_TASK_PRI},
      { SIW_DEFAULT_STACK, SysTaskMonitor, 0, 0, 0, 0, 0, 0, 0, 0, 500},
      NULL, SIW_RECV_SOFT_RESET, SIW_RECV_SOFT_RESET, SYS_MOD}, 
      
	{ "tLOGD", { SIW_PLATFORM_SW, SIW_PRI_GRP_NORMAL, EVENTLOG_TASK_PRI},
      { SIW_DEFAULT_STACK, SysEventTask, 0, 0, 0, 0, 0, 0, 0, 0, 500 },
      NULL, SIW_RECV_RESTART_TASK, SIW_RECV_SOFT_RESET, SYS_MOD}, 

     { "tShelfTask", 
        { SIW_PLATFORM_SW, SIW_PRI_GRP_NORMAL, DEFAULT_TASK_PRI},
      { SIW_DEFAULT_STACK, shelfTask, 0, 0, 0, 0, 0, 0, 0, 0, 500 },
      NULL, SIW_RECV_RESTART_TASK, SIW_RECV_SOFT_RESET, SM_MOD},


            
     { "tFramerTask", { SIW_PLATFORM_SW, SIW_PRI_GRP_NORMAL, DEFAULT_TASK_PRI},
      { SIW_DEFAULT_STACK, inpFramerTask, 0, 0, 0, 0, 0, 0, 0, 0, 500 },
      NULL, SIW_RECV_RESTART_TASK, SIW_RECV_SOFT_RESET, DS1_MOD},

     { "tEsrTask", { SIW_PLATFORM_SW, SIW_PRI_GRP_NORMAL, DEFAULT_TASK_PRI},
      { SIW_DEFAULT_STACK, esrTask, 0, 0, 0, 0, 0, 0, 0, 0, 500 },
      NULL, SIW_RECV_RESTART_TASK, SIW_RECV_SOFT_RESET, DS1_MOD},
 
     { "tOscTask", { SIW_PLATFORM_SW, SIW_PRI_GRP_NORMAL, DEFAULT_TASK_PRI},
      { SIW_DEFAULT_STACK, oscTask, 0, 0, 0, 0, 0, 0, 0, 0, 500 },
      NULL, SIW_RECV_RESTART_TASK, SIW_RECV_SOFT_RESET, DS1_MOD},


 
     { "tEmmTask", { SIW_PLATFORM_SW, SIW_PRI_GRP_NORMAL, DEFAULT_TASK_PRI},
      { SIW_DEFAULT_STACK, emmTask, 0, 0, 0, 0, 0, 0, 0, 0, 500 },
      NULL, SIW_RECV_RESTART_TASK, SIW_RECV_SOFT_RESET, EMM_MOD},
 
 
 

    { "tCSM", { SIW_PLATFORM_SW, SIW_PRI_GRP_NORMAL, DEFAULT_TASK_PRI},
      { SIW_DEFAULT_STACK, CsmTask, 0, 0, 0, 0, 0, 0, 0, 0, 500 },
      NULL, SIW_RECV_RESTART_TASK, SIW_RECV_SOFT_RESET, CSM_MOD},
      
    { "tAM", { SIW_PLATFORM_SW, SIW_PRI_GRP_NORMAL, DEFAULT_TASK_PRI},
      { SIW_DEFAULT_STACK, amTask, 0, 0, 0, 0, 0, 0, 0, 0, 500 },
      NULL, SIW_RECV_RESTART_TASK, SIW_RECV_SOFT_RESET, ALARM_MOD}
      

};
#endif
#endif /* DEFINE_TABLES */

/***********************************************************************
 *  Static initialization of static message queue table.
 **********************************************************************/
#ifndef DEFINE_TABLES
extern SYS_MQ_STATIC_ENTRY_t  sysStaticMsgQTable[];
#else

SYS_MQ_STATIC_ENTRY_t sysStaticMsgQTable[] = {
    {50, SIW_MAX_MESSAGE_SIZE, TASK_ROOT,      1, "plfm:root"},
	{3500, SIW_MAX_MESSAGE_SIZE, TASK_LOGD,    1, "plfm:logd"},
    {200, SIW_MAX_MESSAGE_SIZE, TASK_CSM,      1, "plfm:csm"},
	{100, SIW_MAX_MESSAGE_SIZE, TASK_LOGD,     1, "TrapTest"},
    {1000, SIW_MAX_MESSAGE_SIZE, TASK_AM,      1, "plfm:am"},
};
#endif

/***********************************************************************
 *  Static initialization of static semaphore table.
 **********************************************************************/
#ifndef DEFINE_TABLES
extern SYS_SEM_STATIC_ENTRY_t  sysStaticSemTable[];
#else

SYS_SEM_STATIC_ENTRY_t sysStaticSemTable[] = {
    { "SYS:CARDREADY", SIW_SEM_BINARY, SIW_SEM_Q_FIFO, SIW_SEM_EMPTY,
      SIW_RECV_LOG_ERROR},
    { "SYS:EVENTLOG", SIW_SEM_MUTUAL_EXCLUSION, SIW_SEM_DELETE_SAFE,
      SIW_SEM_FULL,  SIW_RECV_LOG_ERROR},
    { "SYS:ERRORSLOT", SIW_SEM_MUTUAL_EXCLUSION, SIW_SEM_DELETE_SAFE,
      SIW_SEM_FULL,  SIW_RECV_LOG_ERROR},
	{ "SYS:TRAPLOG", SIW_SEM_MUTUAL_EXCLUSION, SIW_SEM_DELETE_SAFE,
      SIW_SEM_FULL,  SIW_RECV_LOG_ERROR},
	{ "SYS:TASKDELAY0", SIW_SEM_BINARY, SIW_SEM_Q_FIFO, SIW_SEM_EMPTY,
      SIW_RECV_LOG_ERROR},
    { "SYS:TASKDELAY1", SIW_SEM_BINARY, SIW_SEM_Q_FIFO, SIW_SEM_EMPTY,
      SIW_RECV_LOG_ERROR},
    { "SYS:AM", SIW_SEM_MUTUAL_EXCLUSION, SIW_SEM_DELETE_SAFE,
      SIW_SEM_FULL, SIW_RECV_LOG_ERROR},
};
#endif

/***********************************************************************
 *  Static initialization of RTC table.
 **********************************************************************/
#ifndef DEFINE_TABLES
extern SYS_RTC_STATIC_ENTRY_t  sysStaticRtcTable[];
#else

SYS_RTC_STATIC_ENTRY_t sysStaticRtcTable[] = {
    {"ROOT_TASK", 0, 0},
	{"LOG_TASK",  0, 10},
};
#endif

/***********************************************************************
 *  Definition of globals for task table, message queue table, and
 *  semaphore table along with needed access index and semaphore. 
 **********************************************************************/
#ifdef DEFINE_SYS_GLOBALS
SYS_TASK_ENTRY_t  sysTaskTable[MAX_TASK_ENTRIES];
SYS_MSGQ_ENTRY_t  sysMsgQTable[MAX_MSGQ_ENTRIES];
SYS_SEM_ENTRY_t   sysSemTable[MAX_SEM_ENTRIES];
SYS_TMR_ENTRY_t   sysTmrTable[MAX_TMR_ENTRIES];

U32_t    sysNextTid   = SYS_TID_START_VALUE;
U32_t    sysNextMqid  = SYS_MQID_START_VALUE;
U32_t    sysNextSemid = SYS_SEMID_START_VALUE;
U32_t    sysNextTmrid = 0;
SEM_ID   sysTaskTblSem;
SEM_ID   sysMsgQTblSem;
SEM_ID   sysSemTblSem;
SEM_ID   sysTmrTblSem;

#else  /* DEFINE_SYS_ROOT_TASK */
extern SYS_TASK_ENTRY_t  sysTaskTable[MAX_TASK_ENTRIES];
extern SYS_MSGQ_ENTRY_t  sysMsgQTable[MAX_MSGQ_ENTRIES];
extern SYS_SEM_ENTRY_t   sysSemTable[MAX_SEM_ENTRIES];
extern SYS_TMR_ENTRY_t   sysTmrTable[MAX_TMR_ENTRIES];
extern U32_t   sysNextTid;
extern U32_t   sysNextMqid;
extern U32_t   sysNextSemid;
extern U32_t   sysNextTmrid;
extern SEM_ID  sysTaskTblSem;
extern SEM_ID  sysMsgQTblSem;
extern SEM_ID  sysSemTblSem;
extern SEM_ID  sysTmrTblSem;
#endif  /* DEFINE_SYS_ROOT_TASK */

#endif  /* _SYS_OS_H_ */
