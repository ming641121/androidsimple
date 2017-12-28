/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_if.h
 *
 *Public API of System Interface Wrapper
 *
 *Note:
 *
 *Author(s):
 * 
 */

#ifndef _SIW_IF_H_
#define _SIW_IF_H_

#include <time.h>
#include <msgQLib.h>

/******************************************************************
 *  System Interface Wrapper(SIW) provides following APIs that are      
 *  used by application modules to access vxWorks OS and system
 *  resources.
 ******************************************************************/
/***********************************************************************
 *  Defines for timeout values
 **********************************************************************/
#define NO_WAIT         0
#define WAIT_FOREVER    (-1)
#define TIMEOUT         (-2)

/***********************************************************************
 *  Defines for CPU Clock Rate
 **********************************************************************/
#ifdef SCC
#define CPU_CLOCK_RATE         200000000
#else
#define CPU_CLOCK_RATE         266666666
#endif

/***********************************************************************
 * System definitions for services offered by different cards based on
 * daughter card types.
 ***********************************************************************/
#define ATM_SERVICE     1
#define FR_SERVICE      2
#define VOICE_SERVICE   4
#define ENET_SERVICE    8
#define NO_SERVICE      0

/***********************************************************************
 *  The following type declarations are used to uniquely identify
 *  tasks, message queues, semaphores, and timer.
 **********************************************************************/
typedef struct siw_tid       *  SIW_TID_t;
typedef struct siw_mqid      *  SIW_MQID_t;
typedef struct siw_semid     *  SIW_SEMID_t;
typedef struct siw_tmrid     *  SIW_TMRID_t;
typedef struct siw_tmrfuncid *  SIW_TMRFUNCID_t;
typedef U32_t                   SIW_ITMRID_t;   /* interval timer ID */

/***********************************************************************
 *  The following defines are used to define invalid values for the
 *  identifiers of tasks, message queues, and semaphores. Also defined
 *  is a value of SIW_TID for self identfication.
 **********************************************************************/
#define SIW_INVALID_TID         (SIW_TID_t)  (-1)
#define SIW_INVALID_MQID        (SIW_MQID_t) (-1)
#define SIW_INVALID_SEMID       (SIW_SEMID_t)(-1)
#define SIW_INVALID_TMRID       (SIW_TMRID_t)(-1)
#define SIW_INVALID_TMRFUNCID   (SIW_TMRFUNCID_t)(-1)
#define SIW_TID_SELF            (SIW_TID_t)  (0)

/***********************************************************************
 *  The following defines are used to define the stack size for a task.
 **********************************************************************/
#define SIW_DEFAULT_STACK       0x2000
#define SIW_LARGE_STACK         0x8000
#define SIW_EXTRA_LARGE_STACK   0x10000

/********************************************************************
 *  Priority defined for platform tasks
 *  Task Priority range is from 1(highest) to 10(lower)
 ********************************************************************/
#define ROOT_TASK_PRI           1
#define MRPC_TASK_PRI           2
#define HI_SERVER_FN_PRI        3
#define MED_SERVER_FN_PRI       4
#define LO_SERVER_FN_PRI        5
#define EVENTLOG_TASK_PRI       9
#define FM_TASK_PRI             5
#define CM_TASK_PRI             5
#define ID_TASK_PRI             5
#define SM_TASK_PRI             5
#define DS1_TASK_PRI            5
#define TCG_TASK_PRI            2
#define PCC_TASK_PRI            5
#define PRC_TASK_PRI            6
#define SNMPD_TASK_PRI          6
#define WEB_TASK_PRI            6
#define MSCP_TASK_PRI           2
#define LSP_TASK_PRI            3
#define DEFAULT_TASK_PRI        5

/***********************************************************************
 *  Defines for main card type
 ***********************************************************************/
#define ALL_CARD              0
#define SCC_CARD              1
#define NMC_CARD              2

/***********************************************************************
 *  Defines for task running on which image
 ***********************************************************************/
#define ALL_IMAGE             0
#define FLASH_IMAGE           1
#define FW_IMAGE              2

/***********************************************************************
 *  Defines for maximum message size in a message queue.
 ***********************************************************************/
#define SIW_MAX_MESSAGE_SIZE  sizeof(MSG_BUF_HDR_t)


/***********************************************************************
 *  Defines for message type using in the message queue.
 ***********************************************************************/
#define EVENT_LOG_MSG          1
#define TIMER_FUNC_MSG         2
#define MRPC_MSG               3
#define EVENT_TRAP             4
#define EVENT_ALARM            5
#define CALL_LOG_MSG           6
#define CALL_LOG_CLR           7
#define SIW_MSGTYPE_BASE       100

/***********************************************************************
 *  Typedef for type of memory pool. This value is used in the
 *  memory allocation interface to indicate type of memory pool.
 **********************************************************************/
typedef enum {
 SIW_TMR = 1,                       /* get memory from System Dynamic buffer pool,
                                       this is used by trillium code */
 SIW_STATIC,                        /* get memory from System memory partition */
 SIW_DYNAMIC,                       /* get memory from System Dynamic buffer pool */
 SIW_EVENT,                         /* get memory from Dynamic pool for Event */
} SIW_MEM_TYPE_t;

/* We carefully control the size of this structure since sar uses a mix of sizeof and the constant 12 to */
/* determine the size of this structure */
typedef struct {
 U8_t      memType;                 /* memory type */
 U8_t      poolId;                  /* memory pool ID */
 U8_t      memBlockId;              /* memory block ID in the pool */
 U8_t      status;                  /* used or free */
 U32_t     *memPtr;
 int       taskId;
} SIW_MEM_HEADER_t;

#define MEM_TRACE_SIZE_MASK     0x00ffffff
#define MEM_TRACE_INDEX_MASK    0xff000000

/***********************************************************************
 *  The following type declaration is used to uniquely identify
 *  applications within the platform.
 **********************************************************************/
typedef enum {
    SIW_PLATFORM_SW = 1,             /* platform software */
    SIW_APPL_SW,                     /* application software */
} SIW_APPLID_t;

/***********************************************************************
 *  The following types are used to declare the priority of a task. The
 *  first type defines the priority groups of task for an application.
 **********************************************************************/
typedef enum {
    SIW_PRI_GRP_HIGH = 1,            /* High Priority Group */
    SIW_PRI_GRP_NORMAL,              /* Normal Priority Group */
    SIW_PRI_GRP_BACKGROUND           /* Lower Priority Group */
} SIW_PRIO_GRP_t;

typedef struct {
    SIW_APPLID_t    applId;          /* Application ID of task */
    SIW_PRIO_GRP_t  prioGroup;       /* Priority Group of task */
    U8_t            priority;        /* Priority of task within group
                                      *  0-19 */
} SIW_PRIORITY_t;

/***********************************************************************
 *  The following type is used to provide information about the
 *  stacksize of a task and where to start the task and what arguments
 *  to use. Also if the task sends an acknowledgement after init.
 **********************************************************************/
typedef struct {
    S32_t       stackSize;       /* Size of task stack */
    FUNC_PTR_t  entryPt;         /* Entry point into task */
    S32_t       arg1;            /* Argument 1 to task entry */
    S32_t       arg2;            /* Argument 2 to task entry */
    S32_t       arg3;            /* Argument 3 to task entry */
    S32_t       arg4;            /* Argument 4 to task entry */
    S32_t       arg5;            /* Argument 5 to task entry */
    S32_t       arg6;            /* Argument 6 to task entry */
    S32_t       arg7;            /* Argument 7 to task entry */
    S32_t       arg8;            /* Argument 8 to task entry */
    S32_t       waitAck;         /* Amount of time to wait for init
                                  * of task in ticks - use 0 for
                                 * tasks that do not ack init */
} SIW_TASKINFO_t;

/***********************************************************************
 *  The following type is used to provide the recovery action to
 *  perform when a task is suspended, when the suspend action fails, or
 *  when a deadlock condition is detected for a semaphore. Not all
 *  actions apply to each condition.
 **********************************************************************/
typedef enum {
    SIW_RECV_NO_ACTION      = 1, /* No Action taken */
    SIW_RECV_LOG_ERROR,          /* Only Log error */
    SIW_RECV_RESTART_TASK,       /* Restart task from start */
    SIW_RECV_SOFT_RESET,         /* Graceful reset of card  */
    SIW_RECV_REG_FUNC,           /* Run registered recovery func */
                                 /* valid only in task management */
    SIW_SEM_RESET                /* reset semaphore */
} SIW_RECV_ACTION_t;

/***********************************************************************
 *  Typedef for signature of task recovery function. Function is passed
 *  task id and returns action to be taken by task monitor.
 **********************************************************************/
typedef SIW_RECV_ACTION_t (*SIW_RECVFUNC_t)(SIW_TID_t);

/***********************************************************************
 * Defines used for option to create message queue
 ***********************************************************************/
#define SIW_MSG_Q_FIFO          0
#define SIW_MSG_Q_PRIORITY      1

/***********************************************************************
 * Defines used for priority to send message through message queue
 ***********************************************************************/
#define SIW_MSG_PRI_NORMAL      0
#define SIW_MSG_PRI_URGENT      1

/***********************************************************************
 * Defines for reliability of sending message 
 ***********************************************************************/
#define SIW_MSGQ_UNRELIABLE     (1 << 31)
#define SIW_MSGQ_UNRELIABLE_MASK ~(SIW_MSGQ_UNRELIABLE)

/***********************************************************************
 *  Typedef for information that can be retrieved about a message
 *  queue.
 **********************************************************************/
typedef struct {
    S32_t    numMsgs;            /* Number of message in queue */
    S32_t    numTasks;           /* Number of task waiting */
    S32_t    maxMsgs;            /* Max number of messages in queue */
    S32_t    maxMsgLength;       /* Max message size in queue */
} SIW_MQ_INFO_t;

/***********************************************************************
 *  Typedef for the header of a message that is sending through 
 *  message queue.
 **********************************************************************/
typedef struct {
   SIW_TID_t  srcTaskId;   /* this parameter will be over-written by system */
   void       *msgBufPtr;  /* this parameter is needed by system */
   U16_t      msgType;     /* space starting from this point can be  */
   U16_t      ecnNum;      /* re-used by user.                       */
   SIW_TID_t  dstTaskId;
   U32_t      dataLen;
   U8_t       srcSlot;
   U8_t       spare1;
   U16_t      spare2;
   U32_t      data;
} MSG_BUF_HDR_t;

/***********************************************************************
 *  Typedef for type of semaphores supported. This value is used in the
 *  semaphore creation interface to indicate type of semaphore.
 **********************************************************************/
typedef enum {
    SIW_SEM_BINARY            = 1,
    SIW_SEM_COUNT             = 2,
    SIW_SEM_MUTUAL_EXCLUSION  = 3
} SIW_SEM_TYPE_t;

typedef enum {
    SIW_SEM_Q_FIFO         =0,    /* Queue waiting tasks FIFO */
    SIW_SEM_Q_PRIORITY     =1,    /* Queue waiting task by priority */
    SIW_SEM_DELETE_SAFE    =4,    /* Owner delete safe(mutex opt) */
    SIW_SEM_INVERSION_SAFE =8,    /* No priority inversion(mutex opt) */
} SIW_SEM_OPTION_t;

/***********************************************************************
 *  Defines for the initial state of a binary semaphore.
 **********************************************************************/
#define SIW_SEM_EMPTY       0
#define SIW_SEM_FULL        1

/***********************************************************************
 *  Typedef for type of timer registration supported. 
 **********************************************************************/
typedef struct {
 SIW_MQID_t   msgQId;
 BOOL_t       urgent;
 void         *dataBuf;
} SIW_TMR_MQ_t;

typedef struct {
 FUNC_PTR_t      tmrFunc;
 U32_t           arg1;
 U32_t           arg2;
 U32_t           arg3;
 U32_t           arg4;
 U32_t           arg5;
} SIW_TMR_FUNC_t;

/***********************************************************************
 *  Defines for interval timers
 **********************************************************************/
#define SIW_ITMR_MAX_TIMER_MAX  8192
#define SIW_ITMR_DELAY_MIN      1000
#define SIW_ITMR_DELAY_MAX      180000

/***********************************************************************
 *  Defines used to specify name sizes in tables
 **********************************************************************/
#define SIW_TASK_NAME_SIZE    12
#define SIW_MQ_NAME_SIZE      16
#define SIW_SEM_NAME_SIZE     16
#define SIW_RTC_NAME_SIZE     12

/***********************************************************************
 *  Defines for card state
 **********************************************************************/
typedef enum {
 SIW_CARD_BOOT     = 0,
 SIW_CARD_ACTIVE, 
 SIW_CARD_STANDBY
} SIW_CARD_STATE_t;

/********************************************************************
 *  The following function interfaces are used to allocate and free 
 *  a block of memory from and to a specific memory pool.
 ********************************************************************/
/********************************************************************
 * Function:     SiwAllocMem()                                      
 * Description:  This function allocate memory from the appropriate 
 *               memory partition based on size and memory type.    
 *               Memory type specifies the memory block will be     
 *               allocated from which memory pool.  A timeout       
 *               specifies a number of clock in ticks to wait till 
 *               the memory becomes available to be allocated.          
 *                                                                  
 * In Parms:     SIW_MEM_TYPE_t memType  -  type of memory allocation 
 *               U32_t          size     -  size of memory block to   
 *                                          allocate                  
 *               S32_t          timeout  -  clock ticks to wait for  
 *                                           memory                    
 * Out Parms:    None                                               
 * Returns:      A valid memory block pointer or NULL if failure    
 ********************************************************************/
extern void *SiwAllocMem(IN SIW_MEM_TYPE_t memType,
                         IN U32_t size,
                         IN S32_t timeout);

/********************************************************************
 * Function:     SiwFreeMem()                                       
 * Description:  This function frees a memory block.  The memory    
 *               is returned to back to the memory pool it was      
 *               previously allocate from by SiwAllocMem().         
 *                                                                  
 * In Parms:     void * memPtr - pointer to memory block to free    
 * Out Parms:    None                                               
 * Returns:      OK or ERROR                                        
 ********************************************************************/
extern STATUS_t SiwFreeMem(IN void *memPtr);

/**********************************************************************
 *  The following function interfaces are used to create, delete and
 *  manipulate tasks and their execution.
 **********************************************************************/
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
 *               is an SIW_TID or SIW_INVALID_TID if a failure occurs.
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
 *               SIW_RECV_ACTION_t failAction - action taken by task monitor if
 *                                     the suspend action fails
 *               SIW_RECVFUNC_t recvFunc - recovery function
 *  Out Parms:   STATUS_t *childStatus - status value returned by
 *                                     initialization of child if wait for
 *                                     task ack init
 *  Returns:     A SIW_TID_t or SIW_INVALID_TID on error
 **********************************************************************/
extern SIW_TID_t SiwTaskSpawn(IN char *            name,
                              IN SIW_PRIORITY_t    *taskPriority,
                              IN SIW_TASKINFO_t    *taskInfo,
                              IN SIW_RECV_ACTION_t suspendAction,
                              IN SIW_RECV_ACTION_t failAction,
                              IN SIW_RECVFUNC_t    recvFunc,
                              IN STATUS_t *        childStatus);

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
 *  In Parms:    SIW_TID_t taskId - task id to delete
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
extern STATUS SiwTaskDelete(IN SIW_TID_t taskId);

/***********************************************************************
 *  Function:    SiwTaskWait()
 *  Description: This function is used to delay a task from executing
 *               for delay time 2ms (+-) 1ms. 
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 **********************************************************************/
extern void SiwTaskWait(void);

/***********************************************************************
 *  Function:    SiwTaskInitAck()
 *  Description: This function is used to acknowlegde to the task
 *               spawner the status of the initialization of the task.
 *               If status of ERROR is reported then the task will be
 *               suspended after sending the init status.
 *
 *  In Parms:    STATUS_t status - report initialization status of OK or ERROR
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
extern STATUS SiwTaskInitAck(IN STATUS_t status);

/***********************************************************************
 *  Function:    SiwWaitCardReady()
 *  Description: This function is used to wait for the card state to
 *               become ready. This function will block the calling task 
 *               until the card is ready for processing.
 *
 *  In Parms:    U8_t slot - slot number
 *  Out Parms:   None
 *  Returns:     OK or ERROR if unable to wait for card ready
 **********************************************************************/
extern STATUS_t SiwWaitCardReady(U8_t slot);

/***********************************************************************
 *  Function:    SiwWaitCardActive()
 *  Description: This function is used to wait for the card state to
 *               enter the ACTIVE state. 
 *
 *  In Parms:    U8_t slot - slot number
 *  Out Parms:   None
 *  Returns:     OK or ERROR if unable to wait for card becoming Active
 **********************************************************************/
extern STATUS_t SiwWaitCardActive(U8_t slot);

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
extern STATUS_t SiwTaskRegRecv(IN SIW_TID_t taskId,
                               IN SIW_RECVFUNC_t recvFunc);

/***********************************************************************
 *  Function:    SiwTaskDelay()
 *  Description: This function is used to delay a task from executing
 *               for the specified delay time in ticks.
 *
 *  In Parms:    S32_t delay - time to delay in ticks
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
extern STATUS SiwTaskDelay(IN S32_t delay);

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
extern SIW_TID_t  SiwTaskIdSelf(void);

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
extern STATUS_t SiwTaskIdVerify(IN SIW_TID_t taskId);

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
extern STATUS_t SiwTaskLock(void);

/***********************************************************************
 *  Function:    SiwReleaseTaskLock()
 *  Description: This function releases a task from locking the CPU.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 **********************************************************************/
extern void SiwReleaseTaskLock(void);

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
extern STATUS_t SiwTaskUnlock(void);

/***********************************************************************
 *  Function:    SiwTaskName()
 *  Description: This function returns a pointer to the task name string
 *               which can up to 10 characters in length.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     A string pointer to task name or NULL if task id invalid
 **********************************************************************/
extern char *SiwTaskName(IN SIW_TID_t taskId);

/***********************************************************************
 *  Function:    SiwTaskNameToId()
 *  Description: This function returns the SIW_TID associated with the
 *               specified task name.
 *
 *  In Parms:    char * taskName - task name to lookup to get task id
 *  Out Parms:   None
 *  Returns:     SIW_TID_t or SIW_INVALID_TID
 *********************************************************************/
extern SIW_TID_t SiwTaskNameToId(IN char *taskName);

/***********************************************************************
 *  Function:    SiwIntLock()
 *  Description: This function disables interrupts. The function
 *               locks the interrupt, but not lock the context switch.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     S32_t key used in SiwIntUnlock()
 **********************************************************************/
extern S32_t SiwIntLock(void);

/***********************************************************************
 *  Function:    SiwIntUnlock()
 *  Description: This function re-enables interrupts previously
 *               disabled by SiwIntLock.
 *
 *  In Parms:    S32_t lockkey - key returned by SiwIntLock()
 *  Out Parms:   None
 *  Returns:     None
 **********************************************************************/
extern void SiwIntUnlock(IN S32_t lockkey);

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
extern STATUS_t SiwTaskShow(IN SIW_TID_t taskId,
                            IN S32_t level);

/***********************************************************************
 *  The following function interfaces are used to create and delete
 *  message queues as well as send and receive messages to and from a
 *  message queue.
 **********************************************************************/
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
 *               char *qName - unique name for message queue
 *  Out Parms:   None
 *  Returns:     A valid SIW_MQID_t or SIW_INVALID_MQID
 **********************************************************************/
extern SIW_MQID_t SiwMsgQCreate(IN S32_t     maxMsgs,
                                IN S32_t     maxMsgLength,
                                IN S32_t     option,
                                IN char *    queueName);

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
extern STATUS_t SiwMsgQDelete(IN SIW_MQID_t msgQId);

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
extern STATUS_t SiwMsgQFlush(IN SIW_MQID_t msgQId);

/***********************************************************************
 *  Function:    SiwMsgQSend()
 *  Description: This function sends a message to a message queue. If
 *               the message queue is full a TIMEOUT is returned.
 *
 *  In Parms:    SIW_MQID_t msgQId  - message queue to receive sent
 *                                    message
 *               MSG_BUF_HDR_t *bufPtr - pointer to message buffer
 *               U32_t    bufSize   - size of message being sent
 *               S32_t    timeout   - amount of time in clock ticks
 *                                    to wait
 *               S32_t    urgent    - priority to send message
 *  Out Parms:   None
 *  Returns:     OK, ERROR, or TIMEOUT
 **********************************************************************/
extern STATUS_t SiwMsgQSend(IN SIW_MQID_t    msgQId,
                           IN MSG_BUF_HDR_t *bufPtr,
                           IN U32_t          bufSize,
                           IN S32_t          timeout,
                           IN BOOL_t         urgent);

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
 *               U32_t msgSize    - size of passed message buffer
 *               S32_t timeout     - amount of time in clock ticks to wait
 *  Out Parms:   None
 *  Returns:     Size of received message, ERROR, or TIMEOUT
 **********************************************************************/
extern S32_t SiwMsgQReceive(IN SIW_MQID_t     msgQId,
                            IN MSG_BUF_HDR_t *bufPtr,
                            IN U32_t          msgSize,
                            IN S32_t          timeout);

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
extern SIW_MQID_t SiwMsgQNameToId(IN char *queueName);

/***********************************************************************
 *  Function:    SiwMsgQNumMsgs()
 *  Description: This function is used to get the number of messages
 *               in the specified message queue.
 *
 *  In Parms:    SIW_MQID_t msgQId - message queue to examine
 *  Out Parms:   None
 *  Returns:     The number of messages queued or ERROR
 **********************************************************************/
extern S32_t SiwMsgQNumMsgs(IN SIW_MQID_t msgQId);

/***********************************************************************
 *  Function:    SiwMsgQInfoGet()
 *  Description: This function is used to get information about the
 *               state and contents of a message queue.
 *
 *  In Parms:    SIW_MQID_t    msgQId    - message queue to query
 *  Out Parmes:  SIW_MQ_INFO_t *msgQInfo - where to return message
 *                                         information
 *  Returns:     OK or ERROR
 **********************************************************************/
extern STATUS_t SiwMsgQInfoGet(IN SIW_MQID_t     msgQId,
                               IN SIW_MQ_INFO_t  *msgQInfoPtr);

/***********************************************************************
 *  Function:    SiwMsgQId()
 *  Description: This function is used to get vxWorks message queue
 *               ID.
 *
 *  In Parms:    SIW_MQID_t msgQId - SIW message queue ID
 *  Out Parms:   None
 *  Returns:     MSG_Q_ID or NULL
 ***********************************************************************/
extern MSG_Q_ID SiwMsgQId(IN SIW_MQID_t msgQId);

/***********************************************************************
 *  Function:    SiwTaskNameToMsgQId()
 *  Description: This function is used to get the SIW message queue
 *               ID for the specific task.
 *
 *  In Parms:    char *taskName - task name
 *  Out Parms:   None
 *  Returns:     SIW_MQID_t or SIW_INVALID_MQID
 ***********************************************************************/
extern SIW_MQID_t SiwTaskNameToMsgQId(char *taskName);

/***********************************************************************
 *  Function:    SiwMsgQShow()
 *  Description: This function is used to show information about the
 *               specified message queue.  The level is used to specify
 *               the level of detail to be displayed. A level of 0
 *               specifies that summary information is to be displayed,
 *               while level 1 specifies that detailed information is to
 *               be displayed.
 *
 *  In Parms:    SIW_MQID_t msgQId - message queue to display
 *               S32_t level - level of information to display
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
extern STATUS_t SiwMsgQShow(IN S32_t       level,
                            IN SIW_MQID_t  msgQId);

/***********************************************************************
 *  Marco:       SIW_MSGTYPE_GET()
 *  Description: This marco gets the message type from message queue
 *               header that is used in inter-task message
 *               communication.
 *
 *  In Parms:    MSG_BUF_HDR_t *msgHdrPtr - pointer to message header
 *  Out Parms:   None
 *  Returns:     message type
 ***********************************************************************/
#define SIW_MSGTYPE_GET(msgHdrPtr)       msgHdrPtr->msgType

/***********************************************************************
 *  The following functions are used to create, delete, take and give
 *  semaphores used to protect resource accesses and synchronization. 
 **********************************************************************/
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
 *  In Parms     char *semName - unique name for the semaphore
 *               SIW_SEM_TYPE_t semType - semaphore type to create
 *               SIW_SEM_OPTION_t option - options to set on semaphore
 *               S32_t initState - initial state of binary or counting
 *                                 semaphore
 *               SIW_RECV_ACTION deadlockAction - action to be taken when
 *                                        deadlock condition is detected
 *  Out Parms:   None
 *  Returns:     A valid SIW_SEMID_t or SIW_INVALID_SEMID
 **********************************************************************/
extern SIW_SEMID_t SiwSemCreate(IN char             *semName,
                                IN SIW_SEM_TYPE_t    semType,
                                IN SIW_SEM_OPTION_t  option,
                                IN S32_t             initState,
                                IN SIW_RECV_ACTION_t deadlockAction);

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
extern STATUS_t SiwSemDelete(IN SIW_SEMID_t semId);

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
extern STATUS_t SiwSemTake(IN SIW_SEMID_t  semId,
                           IN S32_t        timeout);

/***********************************************************************
 *  Function:    SiwSemGive()
 *  Description: This function gives the semaphore to allow it to be
 *               taken by whatever task is waiting.
 *
 *  In Parms:    SIW_SEMID_t semId - semaphore id to give
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
extern STATUS_t SiwSemGive(IN SIW_SEMID_t semId);

/***********************************************************************
 *  Function:    SiwSemCondGive()
 *  Description: This function gives all semaphores that are taken
 *               by the specific task.
 *
 *  In Parms:    SIW_TID_t tid - SIW Task Id
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
extern STATUS_t SiwSemCondGive(IN SIW_TID_t tid);

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
extern STATUS_t SiwSemFlush(IN SIW_SEMID_t semId);

/***********************************************************************
 *  Function:    SiwSemShow()
 *  Description: This function show information about the specified
 *               semaphore. The level is used to specify the level of
 *               detail to be displayed. A level of 0 specifies that
 *               summary information is to be displayed, while level 1
 *               specifies that detailed information is to be displayed.
 *
 *  Inp Parms:   SIW_SEMID_t semId - semaphore to display information
 *               INT32 level - level of information to display
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
extern STATUS_t SiwSemShow(IN SIW_SEMID_t  semId,
                           IN S32_t        level);

/***********************************************************************
 *  The following functions are used to create & cancel a timer,
 *  add & delete a timer function, and start & stop a timer.
 **********************************************************************/
/***********************************************************************
 *  Function:     SiwTimerCreate()
 *  Description:  This function create a timer that can be used to
 *                activate a timer function.  The new created timer is
 *                defaulted to active.  Each task is allowed to create
 *                16 timer only.
 *
 *  In Parms:     None
 *  Out Parms:    SIW_TMRID_t *pTimerId - where to return the timer ID
 *  Returns:      OK or ERROR
 ***********************************************************************/
 extern STATUS_t SiwTimerCreate(OUT SIW_TMRID_t *pTimerId);

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
extern SIW_TMRFUNCID_t SiwTimerFuncAdd(IN SIW_TMRID_t    timerId,
                                       IN U32_t          lapseTime,
                                       IN SIW_TMR_MQ_t   *tmrMsgQPtr,
                                       IN SIW_TMR_FUNC_t *tmrFuncPtr);

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
 extern STATUS_t SiwTimerFuncDelete(IN SIW_TMRID_t timerId,
                                    IN SIW_TMRFUNCID_t tmrFuncId);

/************************************************************************
 *  Function:    SiwTimerStart()
 *  Description: This function starts to activate a timer for a list of
 *               timer function.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
 extern STATUS_t SiwTimerStart(IN SIW_TMRID_t timerId);

/************************************************************************
 *  Function:    SiwTimerStop()
 *  Description: This function stops a timer for a list of timer
 *               function.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
 extern STATUS_t SiwTimerStop(IN SIW_TMRID_t timerId);

/************************************************************************
 *  Function:    SiwTimerReset()
 *  Description: This function resets the timer for the submitted timer
 *               function.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
extern STATUS_t SiwTimerReset(SIW_TMRID_t timerId);

/************************************************************************
 *  Function:    SiwTimerCancel()
 *  Description: This function removes a previously created timer.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
 extern STATUS_t SiwTimerCancel(IN SIW_TMRID_t timerId);

/***********************************************************************
 *  Function:    SiwTimerPublic()
 *  Description: This function make a timer to public
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ***********************************************************************/
extern STATUS_t SiwTimerPublic(SIW_TMRID_t timerId);

/************************************************************************
 *  Function:    SiwTimerIdle()
 *  Description: This function resets the timer for the submitted timer
 *               function.
 *
 *  In Parms:    SIW_TMRID_t timerId - timer ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
extern STATUS_t SiwTimerIdle(IN SIW_TMRID_t timerId);

/***********************************************************************
 *  The following functions are used to initialize the interval timer
 *  library and start, stop, and show interval timers.
 **********************************************************************/
/********************************************************************
 *  Function:    SiwITmrLibInit()
 *
 *  Description: This routine initializes the SIW interval timer library.
 *               The amount of memory allocated is (maxTimers * 16) bytes.
 *
 *  In Parms:    U32_t maxTimers - maximum number of concurrent timers
 *
 *  Out Parms:   None.
 *
 *  Returns:     OK, or ERROR if an error occurs while allocating
 *               resources, the library has already been initialized,
 *               maxTimers is greater than SIW_ITMR_MAX_TIMER_MAX,
 *               or maxTimers is less than one.
 ********************************************************************/
extern STATUS_t SiwITmrLibInit (IN U32_t maxTimers);

/********************************************************************
 *  Function:    SiwITmrStart()
 *
 *  Description: This routine adds an interval timer to the SIW timer
 *               queue.  The specified callback routine will be called
 *               in the context of the timer task after the specified
 *               number  of  milliseconds +/- 100 milliseconds have elapsed.
 *               SiwITmrStart() must not be called from interrupt level.
 *               Although any delay between 1000 and 60000 ms may be
 *               specified, the last two digits are not significant.
 *               So, for example, two timers with delays of 4002 and
 *               4036 are not distinguishable and may run in either order.
 *
 *  In Parms:    U32_t delay         - delay in msec (1000 - 60000)
 *               FUNC_PTR_t pRoutine - callback routine
 *               void *parm          - parameter to pass to callback routine
 *
 *  Out Parms:   U32_t *error - error number
 *
 *  Errors:      Possible values for error include:
 *               0 (zero) - no error
 *               EAGAIN   - service temporarily unavailable
 *               ENOMEM   - no interval timers are available
 *               EINVAL   - invalid parameter
 *
 *  Returns:     The ID of the started interval timer, or NULL on error
 ********************************************************************/
extern SIW_ITMRID_t SiwITmrStart (IN U32_t delay, IN FUNC_PTR_t pRoutine,
              IN void *parm, OUT U32_t *error);

/********************************************************************
 *  Function:    SiwITmrStop()
 *
 *  Description: This routine stops a currently running interval timer.
 *               An ID that has been passed to SiwITmrStop() is no
 *               longer valid and cannot be 'restarted'.  The memory
 *               used for the timer will be recycled automatically.
 *               SiwITmrStop() may be called from interrupt level.
 *
 *  In Parms:    SIW_ITMRID_t iTmrId - ID of SIW interval timer to stop
 *
 *  Out Parms:   None
 *
 *  Returns:     OK, or ERROR if iTmrId was invalid on entry.
 ********************************************************************/
extern STATUS_t SiwITmrStop (IN SIW_ITMRID_t iTmrId);

/********************************************************************
 *  Function:    SiwITmrShow()
 *
 *  Description: This function prints SIW interval timer information.
 *
 *  In Parms:    SIW_ITMRID_t iTmrId - ID of interval timer to show,
 *                                     or 0 for summary of all timers
 *               S32_t level         - level of information (0 is default)
 *
 *  Out Parms:   None
 *
 *  Returns:     OK, or ERROR if iTmrId is neither 0 nor a valid timer ID.
 ********************************************************************/
extern STATUS_t SiwITmrShow (IN SIW_ITMRID_t iTmrId, IN S32_t level);

/***********************************************************************
 *  The following functions are used to get & set the system time,
 *  and get & set clock tick.
 **********************************************************************/
/************************************************************************
 *  Function:    SiwDateTimeGet()
 *  Description: This function gets the current calendar time from
 *               system timer and convert it to the broken-down time.
 *
 *  In Parms:    None
 *  Out Parms:   struct tm *ctime - where to store the broken-down
 *                                  time
 *  Returns:     OK or ERROR
 ************************************************************************/
 extern STATUS_t SiwDateTimeGet(IN struct tm *ctime);

/************************************************************************
 *  Function:    SiwTimeInSecondGet()
 *  Description: This function gets the current calendar time from
 *               system timer in seconds.
 *
 *  In Parms:    None
 *  Out Parms:   struct time_t *ctime - where to store the calendar 
 *                                      time in seconds
 *  Returns:     OK or ERROR
 ************************************************************************/
extern STATUS_t SiwTimeInSecondGet (IN time_t *ctime);

/************************************************************************
 *  Function:    SiwDateTimeSet()
 *  Description: This function sets a new date and time to the system
 *               timer, and also update it to the realtime clock(RTC).
 *
 *  In Parms:    struct tm *newTime - buffer for the broken-down time
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ************************************************************************/
 extern STATUS_t SiwDateTimeSet(IN struct tm *newTime);

/************************************************************************
 *  Function:    SiwTickGet()
 *  Description: This function gets the value of the system tick
 *               counter.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     the value of the ticks
 ************************************************************************/
 extern U32_t SiwTickGet(void);

/************************************************************************
 *  Function:    SiwTickSet()
 *  Description: This function sets the internal tick counter to the
 *               specified value in newTicks.
 *
 *  In Parms:    U32_t newTicks - new time in ticks
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ************************************************************************/
 extern STATUS_t SiwTickSet(IN U32_t newTicks);

/***********************************************************************
 *  The following functions are used to manipulate allocated SAR buffer 
 *  in the chain.
 **********************************************************************/
/***********************************************************************
 *  Function:    SiwSarBufChain()
 *  Description: This function chains two SAR buffers togather.
 *
 *  In Parms:    void *fSarBufPtr - the front SAR buffer pointer in
 *                                  the chain
 *               void *nSarBufPtr - next to the front SAR buffer
 *                                  pointer in the chain
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ***********************************************************************/
extern STATUS_t SiwSarBufChain(IN void *fSarBufPtr, IN void *nSarBufPtr);

/***********************************************************************
 *  FUNCTION:    SiwSarBufGetNext()
 *  Description: This function gets the next SAR buffer pointer.
 *
 *  In Parms:    void *fSarBufPtr - the front SAR buffer pointer in
 *                                  the chain
 *  Out Parms:   None
 *  Returns:     A pointer to the next SAR buffer or NULL
 ***********************************************************************/
extern void *SiwSarBufGetNext(IN void *fSarBufPtr);

/***********************************************************************
 *  Marco:       SIW_SARBUFLEN_GET()
 *  Description: This marco gets the length of the data in the SAR
 *               buffers.
 *
 *  In Parms:    MSG_t *sarBufPtr - the front SAR buffer in the
 *  Out Parms:   None
 *  Returns:     the length of data
 ***********************************************************************/
#define SIW_SARBUFLEN_GET(sarBufPtr)        ((MSG_t *)(sarBufPtr))->no

/***********************************************************************
 *  Marco:       SIW_SARBUFLEN_SET()
 *  Description: This marco sets the length of the data in the SAR
 *               buffers.
 *
 *  In Parms:    MSG_t *sarBufPtr - the front SAR buffer in the
 *               U32_t length - length of the data
 *  Out Parms:   None
 *  Returns:     None
 ***********************************************************************/
#define SIW_SARBUFLEN_SET(sarBufPtr, len)   ((MSG_t *)(sarBufPtr))->no = (len);

/***********************************************************************
 *  Marco:       SIW_SARBUFOFFSET_GET()
 *  Description: This marco gets the offset to the data in the SAR
 *               buffers.
 *
 *  In Parms:    void *sarBufPtr - the front SAR buffer in the
 *  Out Parms:   None
 *  Returns:     the data address
 ***********************************************************************/
#define SIW_SARBUFOFFSET_GET(sarBufPtr)      ((MSG_t *)(sarBufPtr))->dataPtr

/***********************************************************************
 *  Marco:       SIW_SARBUFOFFSET_SET()
 *  Description: This marco sets the offset to the data in the SAR
 *               buffer.
 *
 *  In Parms:    void *sarBufPtr - the front SAR buffer in the
 *               U8_t *offset - offset to the data in the SAR buffer 
 *  Out Parms:   None
 *  Returns:     None
 ***********************************************************************/
#define SIW_SARBUFOFFSET_SET(sarBufPtr, offset) \
                                        ((MSG_t *)(sarBufPtr))->dataPtr = (offset);

/***********************************************************************
 *  Marco:       SIW_SARBUF_FREE()
 *  Description: This marco sets the offset to the data in the SAR
 *               buffer.
 *
 *  In Parms:    void *sarBufPtr - the front SAR buffer in the
 *               U8_t *offset - offset to the data in the SAR buffer 
 *  Out Parms:   None
 *  Returns:     None
 ***********************************************************************/
#define SIW_SARBUF_FREE(memPtr) \
    ((SIW_MEM_HEADER_t *)((U8_t *)(memPtr) - sizeof(SIW_MEM_HEADER_t)))->taskId = 0; \
    ((SIW_MEM_HEADER_t *)((U8_t *)(memPtr) - sizeof(SIW_MEM_HEADER_t)))->status = 0;

/***********************************************************************
 *  Marco:       SIW_DYNMEM_TASKID_CHANGE()
 *  Description: This marco changes the task Id stored in Dynamic memory
 *               buffer.
 *
 *  In Parms:    void *memPtr - memory pointer
 *               int  taskId - task Id
 *  Out Parms:   None
 *  Returns:     None
 ***********************************************************************/
#define SIW_DYNMEM_TASKID_CHANGE(memPtr, taskName)   \
   ((SIW_MEM_HEADER_t *)((U8_t *)(memPtr) - sizeof(SIW_MEM_HEADER_t)))->taskId = taskNameToId(taskName);

/***********************************************************************
 *  The following functions are used to initialize SIW environment.
 **********************************************************************/

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
 *  Returns:      STATUS - OK or ERROR if unable to init.
 **********************************************************************/
extern STATUS_t SiwInit(void);

/***********************************************************************
 *  Function:     SiwMySlotNo()
 *  Description:  This function is used to get the my logical card slot
 *                 number.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 *************************************************************************/
extern S32_t    SiwMySlotNo(void);

/***********************************************************************
 *  Function:     SiwMyPhySlotNo()
 *  Description:  This function is used to get the my physical card slot
 *                number.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 *************************************************************************/
extern S32_t    SiwMyPhySlotNo(void);

/***********************************************************************
 *  Function:     SiwStandbySlotNo()
 *  Description:  This function is used to get the standby card slot 
 *                number.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 *************************************************************************/
extern S32_t    SiwStandbySlotNo();

/************************************************************
 *  Function:    SiwCardState()
 *  Description: This function returns my card state.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     Card State
 *************************************************************/
extern S8_t SiwCardState(S32_t slot);

/************************************************************
 *  Function:    SiwMyCardType()
 *  Description: This function returns my card type.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     Card Type
 *************************************************************/
extern U32_t SiwMyCardType();

/************************************************************
 *  Function:    SiwCardType()
 *  Description: This function returns the card type for the
 *               specific card.
 *
 *  In Parms:    U8_t nvRamId
 *  Out Parms:   None
 *  Returns:     Card Type or ERROR
 *************************************************************/
extern S32_t SiwCardType(S32_t slot);

/************************************************************
 *  Function:    SiwCardService()
 *  Description: This function returns my card service.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     Card Service
 *************************************************************/
extern S32_t SiwCardService(S32_t slot);

/************************************************************
 *  Function:    SiwIfService()
 *  Description: This function returns the service type for
 *               a card.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     Card Service
 *************************************************************/
extern S32_t SiwIfService(S32_t slot);

extern void SiwFuncTraceDisable(void);

/***********************************************************************
 *  Defines for logging
 **********************************************************************/

#define SIW_LOG_OPEN_READ   1
#define SIW_LOG_OPEN_UPDATE 2

#define SIW_LOG_YMD_SHIFT   24
#define SIW_LOG_YMD_MASK    0xffffff

#define SIW_LOG_DATE_MASK   0xffffffffffffLL

/***********************************************************************
 *  Defines for mem trace
 **********************************************************************/

#define NUM_DYN_POOLS         7

#define SYS_MEM_TRACE_NUM_CELLS 128

/***********************************************************************
 *  Typedefs for logging
 **********************************************************************/

typedef union {
    struct
    {
        U8_t    logType;    /* logging type */
        U8_t    pad1;       /* unused */
        U8_t    year;       /* year:   0 to 129 (since 1970) */
        U8_t    month;      /* month:  1 to 12 */
        U8_t    day;        /* day:    1 to 31 */
        U8_t    hour;       /* hour:   1 to 24 */
        U8_t    minute;     /* minute: 0 to 59 */
        U8_t    second;     /* second: 0 to 59 */
    } v1;
    U64_t  v2;
} SIW_LOG_HEADER_t;

typedef struct siw_log_file SIW_LOG_FILE;

/***********************************************************************
 *  Typedefs for mem trace
 **********************************************************************/

typedef struct {
    BOOL        valid;
    char        name [SIW_TASK_NAME_SIZE];
    int         taskId;
    U32_t       DynMemBytesAlloc[NUM_DYN_POOLS];
    U32_t       StaticMemBytesAlloc;
} MEM_TRACE_CELL_t;

typedef struct {
    MEM_TRACE_CELL_t    tasks [SYS_MEM_TRACE_NUM_CELLS];
    U32_t               StaticMemAlloc; /* Bytes */
    U32_t               DynamicMemAlloc[NUM_DYN_POOLS]; /* Blocks */
    int                 SARMemAlloc; /* Blocks */
    U32_t               DosFsMemAlloc; /* Bytes */
    U32_t               IsoMemAlloc; /* Bytes */
} MEM_TRACE_BUF_t;

/***********************************************************************
 *  Function prototypes for logging
 **********************************************************************/

/********************************************************************
 * Function:     SiwLogOpen()
 *
 * Description:  This routine opens the named log file for read or update.
 *
 * Note: <filesize> is an output parameter if <mode> is SIW_LOG_OPEN_READ,
 *       or an input parameter if <mode> is SIW_LOG_OPEN_UPDATE.
 *
 * In Parms:     const char * filename  - log file name
 *               int          mode      - SIW_LOG_OPEN_READ or
 *                                            SIW_LOG_OPEN_UPDATE
 *               size_t       recsize   - bytes per record
 *
 * In/Out Parm:  long       * filesize  - size of file in bytes
 *
 * Returns:      A file pointer on success, or NULL on error.
 ********************************************************************/
SIW_LOG_FILE *SiwLogOpen(IN const char *filename, IN int mode,
                         IN size_t recsize, IN OUT long *filesize);

/********************************************************************
 * Function:     SiwLogClose()
 *
 * Description:  This routine closes the log file specified by <stream>.
 *
 * In Parms:     SIW_LOG_FILE * stream  - file ptr obtained via SiwLogOpen()
 *
 * Returns:      0 on success, or EOF on error.
 ********************************************************************/
int SiwLogClose(IN SIW_LOG_FILE *stream);

/********************************************************************
 * Function:     SiwLogRead
 *
 * Description:  This routine reads into the buffer <record> one
 *               record of <size> bytes from a specified stream <fp>.
 *               The file position indicator for the  stream  is
 *               decremented to the *previous* record in the file,
 *               or if already at the first record, it is set to
 *               the *last* record in the file.  If an error occurs,
 *               the resulting value of the  file  position indicator
 *               for  the
 *               stream is indeterminate.  If a partial element is
 *               read, its value is indeterminate.
 *
 * In Parms:     size_t         size   - bytes per record
 *               SIW_LOG_FILE * fp     - where to read from
 *
 * Out Parms:    SIW_LOG_HEADER_t * record - where to copy to
 *
 * Returns:      The number of records read.
 ********************************************************************/
size_t SiwLogRead(OUT SIW_LOG_HEADER_t *record, IN size_t size,
                    IN SIW_LOG_FILE *fp);

/********************************************************************
 * Function:     SiwLogWrite()
 *
 * Description:  This routine writes, from the array  <record>, up to
 *               <count> records to a specified stream.  The file
 *               position indicator  for  the  stream is advanced by
 *               the number of characters successfully written.  This
 *               routine wraps to the beginning on end-of-file.
 *               If an error occurs, the resulting value of the file
 *               position indicator for the stream is indeterminate.
 *
 * In Parms:     SIW_LOG_HEADER_t * record   - where to copy from
 *               size_t             nrec     - no. of records
 *               size_t             recsize  - bytes per record
 *               SIW_LOG_FILE *     fp       - stream to write to
 *               long               filesize - size of file in bytes
 *
 * Out Parms:    None
 *
 * Returns:      The number of records written.
 ********************************************************************/
size_t SiwLogWrite(IN SIW_LOG_HEADER_t *record, IN size_t nrec,
                     IN size_t elsize, IN SIW_LOG_FILE *fp, IN long filesize);
#endif  /* _SIW_IF_H_ */
