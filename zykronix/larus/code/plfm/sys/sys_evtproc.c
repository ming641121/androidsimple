/*
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: sys_evtproc.c
 *
 *Event Log handling routines.
 *
 *Note:
 *
 *Author(s):
 *
 */

#include <stdio.h>
#include <sysLib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <intLib.h>
#include <msgQLib.h>
#include <time.h>
#include <lstLib.h>
#include <usrLib.h>
#include <tickLib.h>
#include <ioLib.h>
#include <axss_types.h>
#include <axss_if.h>
#include <siw_if.h>
#include <siw_events.h>
#include <siw_trace.h>
#include <sys_memmap.h>
#include <sys_if.h>
#include <trap_if.h>
#include <clicmd_if.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "comunity.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "lookup.h"
#include "v2table.h"
#include "min_v.h"
#include "mibout.h"
#include <snmptype.h>

#include <trapdef.h>
#include "sys_events.h"
#include <rtc_if.h>


#define TRAP

/***********************************************************************
 *  General defines for event and error log
 **********************************************************************/
#define MAX_EVENT_FILES            5
#define MAX_RECS_PER_FILE          100
#define MAX_EVENTMSG_SIZE          sizeof(EVENT_LOG_ENTRY_t)
#define MAX_LOGFILE_SIZE           (MAX_RECS_PER_FILE * MAX_EVENTMSG_SIZE)
#define MAX_TRAPFILE_ENTRIES       100
#define EVENT_MSGLEN               250
#define EVENT_LOG_DIRECTORY        "C:/LOG"
#define EVENT_FILENAME             "C:/LOG/event"
#define LOG_EXTENSION              ".log"
#define FILE_NAME_SIZE             20
#define MAX_ERROR_NUM              99999
#define EVENT_HUNDRED_MILLISECONDS 10

/***********************************************************************
 *  Defines of recovery threshold count
 **********************************************************************/
#define RECV_ERROR_SUSPEND_TH   10
#define DISK_WRITE_SUSPEND_TH   3
#define FILE_OPEN_RETRY         3
#define OPEN_TRY_COUNT          3

/************************************************************************
 * Offset defines for retrieving Event & Error Log info from RTC
 ************************************************************************/
#define EVENTLOG_ADDR           130	     /* ?????????????? */
#define EVENTSLOT_ADDR          0x0F	 /* ?????????????? */

/***********************************************************************
 *  General defines for trap log
 **********************************************************************/
#define TRAP_FILENAME           "C:/LOG/TRAP.LOG"
#define SNMP_FILENAME           "C:/LOG/TRAP.SNMP"

/*************************************************************************
 *  General typedef
 *************************************************************************/
struct EVENT_REG_NODE
{
  SIW_TID_t               taskId;
  SIW_MQID_t              msgQId;
  BOOL_t                  standby;
  struct EVENT_REG_NODE  *next;
};

typedef struct EVENT_REG_NODE   EVENT_REG_NODE_t;

/**************************************************************************
 *  Static global variable definition
 **************************************************************************/
static char              eventFilename[FILE_NAME_SIZE];
static int               currentEventFile = ERROR; /* The value is ERROR (-1)                                                    if the file is not open */
static U16_t             sysEventSlot = 0;
static int               msgTimeout = WAIT_FOREVER;
static BOOL_t            logTaskInitialized = FALSE;
static FILE              *snmpFd = NULL;
static BOOL_t            snmpFileOpen = FALSE;
static U16_t             trapIndex = 0;
static int               snmpIndex, snmpIndexSave;

#ifndef TRAP
static EVENT_REG_NODE_t  *eventRegTable[10000];
#endif

/* start lsevt for web */

#define EVENT_COPY_FILENAME  "C:/LOG/event.copy"
#define LAST_FILE_INDEX      1 /* total 2 files */

static char              evtDate[DATESTAMP_SIZE + 1];
static int               evtSlotNo;
static int               evtSeverity;
static BOOL_t            webListOn = FALSE;
static int               preEventSlot;
static int               evtCopyFd;
static BOOL_t            evtCpFileOpen = FALSE;
static long              evtStartOffset;
static int               fileIndex;

/* stop lsevt for web */

/* memory block trace parameters */
int memAllocCount = 0;
int sendFailCount = 0;
int sendOKCount = 0;
int wrongSizeCount = 0;
int msgRecvdCount = 0;
int recvTimeoutCount = 0;
int recv4ByteCount = 0;
int recvMHdCount = 0;
int recvWrongCount = 0;
int recvUnknownCount = 0;
int nullEventCount = 0;
int nullEventxxCount = 0;
int trapDebugId = -1;

BOOL_t trapDebug = FALSE;
BOOL_t errHandlerEnable = TRUE;
BOOL_t eventDebugOn = FALSE;

/***************************************************************************
 *   Static Functional Prototype 
 ***************************************************************************/
static STATUS_t ErrLogInit(void);
static STATUS_t NextEventFileGet(void);
static STATUS_t CloseFile(int *);
static STATUS_t OpenFile(int *, char *);
static STATUS_t SaveToDisk(EVENT_LOG_ENTRY_t *);
static STATUS_t SaveTrapToDisk(EVENT_LOG_HDR_t *evtLogHdr, EVENT_LOG_ENTRY_t *strBuf);
static void     ErrMsgProcess(char *eventBufPtr,
                              SIW_EVENT_OBJ_t *event, char *);
static void     EventSlotSave(void);
static void     SysErrMsgLog(MSG_t *, EVENT_LOG_HDR_t *, EVENT_LOG_ENTRY_t *);
static void     ConvertEventMsg(SIW_EVENT_OBJ_t *event,
                                char *msg, EVENT_LOG_ENTRY_t *evtLog);
static void     EventTrap(MSG_t *msgBuf, BOOL_t standby);
static void     EventDispatch(MSG_t *msgBuf, int alarmId);
static void     SaveTrapIndex(void);
void            GetTrapIndex(void);
void            ErrEventResetEnable(void);
void            ErrEventResetDisable(void);
STATUS_t        SysTrapFileSet(void);
STATUS_t        SysTrapGet(char *date, int slotNo, int severity, SNMP_EVENT_LOG_t *bufPtr);
STATUS_t        SysTrapEventPrint(int days, int severity);

void            trapRegister(int alarmId);
void            trapSend(int alarmId, int severity, int num);
int             trapReceive(void);

/*****************************************************************************
 *  External Function prototype
 *****************************************************************************/
extern STATUS_t EventClr(void);
extern STATUS_t TrapEventClr(void);
extern STATUS_t ErrorLogClr(void);
extern void     EventBlockTrace(void);
extern void     TrapDebugOn(void);
extern void     TrapDebugOff(void);
extern void     EventLogDebugOn(void);
extern void     EventLogDebugOff(void);
extern STATUS_t TrapRegTblGet(U32_t trapNum, void  **regTbl);
extern void     EventTrapRegisterDisp(int alarmId);
extern RET_t    ErrHandler (S32_t slot, S32_t eType, S32_t eId, MSG_t *msg);
extern void	 	CopyString(char *desStr, const char *srcStr, int desLen, 
                           int srcLen, char filler);
extern void	 	ClrScrn(void);
extern int		CheckScroll(int *pLineNum, int limit);

/*****************************************************************************
 *  Function:    SiwEventLog()
 *
 *  Description: This function reports the event passed to it.  The event
 *               will be added to the current event log and will initiate
 *               the actions associated with the event object.
 *
 *  In Parms:    char *eventBufPtr - pointer to the event buffer
 *               SIW_EVENT_OBJ_t *event - Pointer to SIW_EVENT_OBJ_t to be
 *                                        reported
 *               ... - optional parameters to fill format string of event
 *                     message
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ****************************************************************************/
STATUS_t
SiwEventLog(char *eventBufPtr, SIW_EVENT_OBJ_t *event, ...)
{
    va_list args;

    /* to block Event reported by ISR */
    if ( intContext() == TRUE )
    {
        return (ERROR);
    }

    /* Call function trace */
    SIW_FUNC_TRACE(SYS, SiwEventLog, 0, 0, 0, 0, 0);

    if (SiwTaskIdSelf() == TASK_LOGD)
    {
if (eventDebugOn)
{
    printf("Event is reported by Task LOGD\n");
}
        return (OK);
    }

    va_start(args, event);
    va_end(args);

    event->slotNo = 0;
	event->portNo = 0;

    return (SysEventLog(eventBufPtr, event, args));
}

/*****************************************************************************
 *  Function:    SysEventLog()
 *
 *  Description: This function reports the event log message to Event Log Task. 
 *
 *  In Parms:    char *eventBufPtr - pointer to the event buffer
 *               SIW_EVENT_OBJ_t *event - Pointer to SIW_EVENT_OBJ_t to be
 *                                        reported
 *               va_list args - optional parameters to fill format string of 
 *                              event message
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ****************************************************************************/
STATUS_t
SysEventLog(char *eventBufPtr, SIW_EVENT_OBJ_t *event, va_list args)
{
#define EVENTLOG_CLOSE_TICKS     110

    U32_t   taskIndex = 0;
    U32_t   ticks;
    char    str[EVENT_MSGLEN];
 
    /* to block Event reported by ISR */
    if ( intContext() == TRUE )
    {
        return (ERROR);
    }

    if (event == NULL)
    {
if (eventDebugOn)
{
    printf("There is no SIW Event Object\n");
}
        return (ERROR);
    }

    /* Get task table index */
    if ( intContext() == FALSE )
    {
        taskIndex = (U32_t)SiwTaskIdSelf();
    }
    else
    {
        taskIndex = 0;
    }

    /* Check if already in processing an event */
    if (sysTaskTable[taskIndex].procEvt == TRUE)
    {
if (eventDebugOn)
{
    printf("Event processing is in recursive loop\n");
}
        return (ERROR);
    }

    if (event->option <= EVENT_STKTRC)
    {
        ticks = (U32_t)tickGet();

if (eventDebugOn)
{
char tmp[40];

memset(tmp, 0, sizeof(tmp));
strncpy(tmp, event->eventname, EVENT_NAME_SIZE);
printf("Event -- %s, ticks -- %d %d\n", tmp, ticks, event->ticks);
}

        /* check if the event is in flooding condition */
        if (ticks < event->ticks)
        {
            event->ticks = ticks;
        }
        if ((ticks - event->ticks) < EVENTLOG_CLOSE_TICKS)
        {
if (eventDebugOn)
{
    printf("filtering out the flooding events\n");
}
            event->ticks = ticks;
            return (ERROR);
        }
        event->ticks = ticks;
    }

#if 0
    /* validate check of event buffer pointer */
    if ((event->option > EVENT_STKTRC) && (eventBufPtr == NULL))
    {
if (eventDebugOn)
{
    printf("Invalid Event Option: %d, or Event Buf Ptr: %p\n", 
           event->option, eventBufPtr);
}
        return (ERROR);
    }
#endif

    /* Set processing event */
    sysTaskTable[taskIndex].procEvt = TRUE;

    /* convert a formatted string into a data buffer */
    memset(str, 0, sizeof(str));
    vsprintf(str, event->format, args);

    /* report error event */
    ErrMsgProcess(eventBufPtr, event, str);

    /* Clear processing event */
    sysTaskTable[taskIndex].procEvt = FALSE;

    if (intContext() == FALSE)  /* It is in Task context */
    {
        /* check the recovery flag specified with this error event */
        if (event->action == EVENT_TASK_SUSPEND)
            taskSuspend(taskIdSelf());
    }

    return (OK);
}

/*******************************************************************************
 *  Function:     ErrMsgProcess()
 *  Description:  This function compiles event log message format, and
 *                reports the event log to the Event Log Task.
 *
 *  In Parms:     char *eventBufPtr - pointer to the event buffer
 *                SIW_EVENT_OBJ_t *event - pointer to the Event 
 *                                               object identity
 *                char *msg - pointer to the the event log message string
 *  Out Parms:    None
 *  Returns:      None
 *******************************************************************************/
static void
ErrMsgProcess(char                  *eventBufPtr,
              SIW_EVENT_OBJ_t       *event,
              char                  *msg)
{
    EVENT_LOG_ENTRY_t *evtLog = NULL;
    EVENT_LOG_HDR_t   *evtLogHdr = NULL;
    MSG_t             *msgBuf = NULL;
    MSG_BUF_HDR_t      eventBuf;
    S32_t              msgStatus = 0;
    U8_t               trapEvent;
    U16_t              alarmId;
    U32_t              bufOffset = 0;

    /* Call function trace */
    SIW_FUNC_TRACE(SYS, ErrMsgProcess, event, msg, 0, 0, 0);

    if (logTaskInitialized == FALSE)
    {
        return;
    }

    /* extract the info of Trap and Alarm */
    trapEvent = (U8_t)((event->option & EVENT_TRAP_FLAG) >> 15);
    alarmId = (U16_t)(event->option & EVENT_TRAP_MASK);

    /* get an Dynamic memory buffer */
	msgBuf = (MSG_t *)SiwAllocMem(SIW_EVENT, sizeof(MSG_t), WAIT_FOREVER);
    memAllocCount++;

    /* filling message header */
    eventBuf.dstTaskId = TASK_LOGD;
    eventBuf.msgType = EVENT_LOG_MSG;
    eventBuf.srcSlot = event->slotNo;;
  
    /* get location of event entry in buffer */
    eventBuf.msgBufPtr = msgBuf;

    /* fill in Event Log Header */
    evtLogHdr = (EVENT_LOG_HDR_t *)&msgBuf->data[0+bufOffset];
    evtLogHdr->msgType = EVENT_LOG_MSG;
    evtLogHdr->trapEvent = trapEvent;
    evtLogHdr->alarmId = alarmId;
	evtLogHdr->shelf = event->shelf;
	evtLogHdr->slotNo = event->slotNo;
	evtLogHdr->portNo = event->portNo;
	evtLogHdr->severity = event->severity;
	evtLogHdr->trapType = event->trapType;


    /* get the starting data block */ 
    evtLog = (EVENT_LOG_ENTRY_t *)&msgBuf->data[sizeof(EVENT_LOG_HDR_t)+bufOffset];

    /* extract event log message */ 
    ConvertEventMsg(event, msg, evtLog);

    /* send the error event to the Error Log Task */ 
    if ((msgStatus = SiwMsgQSend(MSGQ2LOG, &eventBuf,
              sizeof(MSG_BUF_HDR_t), 10, SIW_MSG_PRI_NORMAL)) != OK)
    {
if (eventDebugOn)
{
    printf("Failed sending Event to Task LOGD\n");
}
        sendFailCount++;

        return;
    }
    else
    {
        sendOKCount++;
    }
    
    if ((event->option == EVENT_STKTRC) && (intContext() == FALSE))
    {
    }
    return;
}

/*******************************************************************************
 *  Function:     SysEventTask()
 *  Description:  This function acts as an Event Log Task, and is responsible
 *                for screening, saving and forwarding event log.  If the event 
 *                log is leveled to FATAL, it will reset the card.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 *******************************************************************************/
STATUS_t
SysEventTask()
{
    EVENT_LOG_ENTRY_t  *evtLog = NULL;
    EVENT_LOG_HDR_t    *evtLogHdr = NULL;
    MSG_BUF_HDR_t      evtBuf;
    STATUS_t           status = OK;
    DIR                *dd = NULL;

    /* make sure the log directory exists */ 
    if ((dd = opendir(EVENT_LOG_DIRECTORY)) == NULL)
    {
        status = mkdir(EVENT_LOG_DIRECTORY);
    }
    else
    {
        char fname[50], testData[10], verData[10];
        FILE *fhd = NULL;

        closedir(dd);
        memset(fname, 0, sizeof(fname));
        strcpy(fname, EVENT_FILENAME);
        strcat(fname,".tst");
        if ((fhd = fopen(fname, "w")) == NULL)
        {
            rmdir(EVENT_LOG_DIRECTORY);
            mkdir(EVENT_LOG_DIRECTORY);
        }
        else
        {
            strcpy(testData, "5ADWKQ");
            fwrite(testData, 6, 1, fhd);
            fclose(fhd);
            if ((fhd = fopen(fname, "w")) == NULL)
            {
                rmdir(EVENT_LOG_DIRECTORY);
                mkdir(EVENT_LOG_DIRECTORY);
            }
            else
            {
                memset(verData, 0, sizeof(verData));
                fread(verData, 6, 1, fhd);
                fclose(fhd);
                if (strcmp(testData, verData) != 0)
                {
                    rmdir(EVENT_LOG_DIRECTORY);
                    mkdir(EVENT_LOG_DIRECTORY);
                }
            }
        }
    }

    GetTrapIndex();

    /* get the information of Event Log file from RTC */
    ErrLogInit();

#ifndef TRAP
{
    int i;

    /* zap the Alarm Registration Table */
    for (i=0; i<10000; i++)
    {
        eventRegTable[i] = NULL;
    }
}
#endif

    /* Set Log Task Initialize flag */
    logTaskInitialized = TRUE;

    /* send ACK back to parent */
    if (SiwTaskInitAck(status) == ERROR || status == ERROR)
    {
        /* Log Trace */
        SIW_DEBUG_TRACE(SYS, SysEventTask, 12, TRACE_LEVEL_1,
                        0, 0, 0, 0, 0);
       
        return (status);
    }

    /* waiting here for card become ready */
#if 0
    SiwWaitCardReady(SiwMySlotNo());        /* ????????????????????????? */
#endif

    for (;;)  /* main task loop */
    {
        /* wait for message or sleep */
        status = SiwMsgQReceive(MSGQ2LOG, &evtBuf,
                               sizeof(MSG_BUF_HDR_t), msgTimeout);
        msgRecvdCount++;
        if (status == TIMEOUT)
        {
            /* no more message in the message queue */
            recvTimeoutCount++;
        }
        else if (status == sizeof(MSG_BUF_HDR_t))
        {
            /* get the header of the event */
			evtLogHdr = (EVENT_LOG_HDR_t *)&((MSG_t *)evtBuf.msgBufPtr)->data[0];
            /* check the message type */
            if (evtBuf.msgType == EVENT_LOG_MSG)
            {
                /* Get message pointer and log message to disk */
                evtLog = (EVENT_LOG_ENTRY_t *)&((MSG_t *)evtBuf.msgBufPtr)->data[sizeof(EVENT_LOG_HDR_t)];
                recvMHdCount++;
                SysErrMsgLog(evtBuf.msgBufPtr, evtLogHdr, evtLog);
            }
            else
            {
                /* Log Trace */
                SIW_DEBUG_TRACE(SYS, SysEventTask, 14, TRACE_LEVEL_1,
                                evtBuf.msgType, 0, 0, 0, 0);
 
                /* free MSG_t message buffer */
                wrongSizeCount++;
				memAllocCount--;
                SiwFreeMem((void *)evtBuf.msgBufPtr);
            }
        }
        else if (status != ERROR)
        {
            /* Increment error count */
            recvWrongCount++;

            /* free MSG_t message buffer */
            memAllocCount--;
            SiwFreeMem((void *)evtBuf.msgBufPtr);
        }
        else
        {
            /* Increment error count */
			recvUnknownCount++;

            /* Suspend task if consecutive error count exceed threshold */
            if (recvUnknownCount >= RECV_ERROR_SUSPEND_TH) 
            {
                taskSuspend(taskIdSelf());
            } /* end if errCount >= ERROR_SUSPEND_COUNT */
            

            /* potential a memory leak!!! */

        } 
    }

    return (ERROR);
}
 
/******************************************************************************
 *  Function:    SysErrMsgLog()                                    
 *  Description: This function saves event log into IDE drive, and at
 *               the same time forward the event log to standby unit 
 *               if necessary.
 *                                                                 
 *  In Parms:    MSG_t *msgBuf - pointer to the MRPC message buffer
 *               EVENT_LOG_HDR_t *evtLogHdr - pointer to the header of
 *                                            the event
 *               EVENT_LOG_ENTRY_t *evtLog - pointer to the event log message
 *                                           buffer   
 *  Out Parms:   None                                              
 *  Returns:     None
 *******************************************************************************/
static void
SysErrMsgLog(MSG_t *msgBuf, EVENT_LOG_HDR_t *evtLogHdr, EVENT_LOG_ENTRY_t *evtLog)
{
    static char   errCount = 0;
    
    /* Call function trace */
    SIW_FUNC_TRACE(SYS, SysErrMsgLog, msgBuf, evtLog, 0, 0, 0);

    /* validate event error message */ 
    if (evtLog == NULL) 
    {
        /* Log Trace */
        SIW_DEBUG_TRACE(SYS, SysErrMsgLog, 0, TRACE_LEVEL_1,
                        0, 0, 0, 0, 0);

        if (msgBuf != NULL)
        {
            /* free MSG_t message buffer */
            nullEventCount++;
			memAllocCount--;
            SiwFreeMem(msgBuf);
        }
        else
        {
            nullEventxxCount++;
        }
        
        return; 
    }

    /* Set null terminators in event entry */
    evtLog->nullterm1 = '\0';
    evtLog->nullterm2 = '\0';        

    if ((evtLogHdr->trapEvent == 0) &&
        (evtLogHdr->alarmId == 0) )
    {
	    /* grab the semaphore for accessing the event error log */
        SiwSemTake(SEM_EVENTLOG, WAIT_FOREVER);

        if (SaveToDisk(evtLog) == ERROR)
        {
		    errCount++;
        }
        else
        {
            /* reset error count */
            errCount = 0;
        }

	    /* release semaphore */
        SiwSemGive(SEM_EVENTLOG);
    }        
    else if ((evtLogHdr->trapEvent == 1) ||
             (evtLogHdr->alarmId > 0))
    {
        /* request the Semaphore to access the Trap Log File */
        SiwSemTake(SEM_TRAPLOG, WAIT_FOREVER);
	    /* save Trap Event */
        SaveTrapToDisk(evtLogHdr, evtLog);
        SiwSemGive(SEM_TRAPLOG);
    }

    /* dispatch event */
    /* send event to the trap manager */
    if (evtLogHdr->trapEvent)
    {
        /* forward event to Trap Manager */
        EventTrap(msgBuf, FALSE);
    }

    /* send event to the registered tasks */
    if (evtLogHdr->alarmId > 0)
    {
        EventDispatch(msgBuf, evtLogHdr->alarmId);
    }
  			 
    /* check event severity */
    if ((evtLog->severity < EVENT_INFO) &&
        (evtLog->eventType == NORMAL_EVENT))
    {
        /* take action on FATAL and MAJOR event */
        if (errHandlerEnable)
        {
            ErrHandler(evtLog->slotNo, evtLog->severity, evtLog->eventId, NULL); 
        }
    }

    /* free MSG_t message buffer */
    memAllocCount--;
    SiwFreeMem(msgBuf);
}

/*******************************************************************************
 *  Function:     ConvertEventMsg()                                   
 *  Description:  This function converts event log message from raw data format
 *                to formatted data that will be saved in Novram.
 *                                                             
 *  In Parms:     SIW_EVENT_OBJ_t *event - pointer to the Event Object
 *                                               identity
 *                char *msg - Event Log message with formatted character string 
 *  Out Parms:    EVENT_LOG_ENTRY_t *evtLog - where to store the formatted data 
 *  Returns:      None
 *******************************************************************************/
static void
ConvertEventMsg(SIW_EVENT_OBJ_t        *event,
                char                   *msg,
                EVENT_LOG_ENTRY_t      *evtLog)
{
    struct tm  curtm;
    char *     tname;
    S32_t      index;
    S32_t      len;
    U8_t       slot = event->slotNo;

    /* Call function trace */
    SIW_FUNC_TRACE(SYS, ConvertEventMsg, event, 0, msg, evtLog, 0);

    /* zero out the data buffer */
    memset((char *)evtLog, 0, sizeof(EVENT_LOG_ENTRY_t));

    /* get the current system time */
    SiwDateTimeGet(&curtm);
 
    /* put the info of event error into the data buffer */
    strftime(evtLog->datestamp, DATESTAMP_SIZE, "%m/%d/%Y", &curtm);
    evtLog->blankpad0 = ' ';
    strftime(evtLog->timestamp, TIMESTAMP_SIZE, "%H:%M:%S", &curtm);
    evtLog->blankpad1[0] = ' ';
    evtLog->blankpad1[1] = 'L';
    evtLog->slotno[0] = ((slot / 10) % 10) + '0';
    evtLog->slotno[1] = (slot % 10) + '0';
    slot = event->slotNo;
    evtLog->blankpad2[0] = ' ';
    evtLog->blankpad2[1] = 'P';
    evtLog->pslotno[0] = ((slot / 10) % 10) + '0';
    evtLog->pslotno[1] = (slot % 10) + '0';
    evtLog->blankpad3 = ' ';
    if (intContext() == FALSE)
    {
        tname = SiwTaskName(SiwTaskIdSelf());
        if ( tname != NULL )
        {
            CopyString(evtLog->taskname, tname, TASK_NAME_SIZE,
                       strlen(tname), ' ');
        }
        else
        {
            CopyString(evtLog->taskname, "UNKNOWN", TASK_NAME_SIZE, 7, ' ');
        }
    }
    else
    {
        CopyString(evtLog->taskname, "ISR", TASK_NAME_SIZE, 3, ' ');
    }
    evtLog->blankpad4 = ' ';
    len = strlen(event->module);
    if ( len < MODULE_NAME_SIZE ) 
    {
        for ( index=0; index<MODULE_NAME_SIZE - len; index++ ) 
        {
            evtLog->module[index] = ' '; 
        }
        CopyString(&evtLog->module[index], event->module,
                   MODULE_NAME_SIZE - index, len, ' ');
    }
    else
    {
        CopyString(evtLog->module, event->module, MODULE_NAME_SIZE, len, ' ');
    }
    evtLog->dashpad1 = '-';
    CopyString(evtLog->eventname, event->eventname, EVENT_NAME_SIZE,
               strlen(event->eventname), '*');
    evtLog->dashpad2 = '-';
    if (event->severity == EVENT_FATAL)
    {
        CopyString(evtLog->severityName, "FATAL", 5, 5, '*');
        evtLog->severity = EVENT_FATAL;
    }
    else if (event->severity == EVENT_MAJOR)
    {
        CopyString(evtLog->severityName, "MAJOR", 5, 5, '*');
        evtLog->severity = EVENT_MAJOR;
    }
    else if (event->severity == EVENT_MINOR)
    {
        CopyString(evtLog->severityName, "MINOR", 5, 5, '*');
        evtLog->severity = EVENT_MINOR;
    }
    else
    {
        CopyString(evtLog->severityName, "INFO", 5, 4, '*');
        evtLog->severity = EVENT_INFO;
    }

    /* evtLog->eventId = event->eventId; */
	evtLog->slotNo = event->slotNo;
    if ((evtLog->slotNo < 0) || (evtLog->slotNo > 15))
    {
        evtLog->slotNo = 0;
    }
    
    evtLog->nullterm1 = '\0';
    CopyString(evtLog->message, msg, EVENT_MESSAGE_SIZE,
               EVENT_MESSAGE_SIZE, ' ');
    evtLog->nullterm2 = '\0';

    if (event->option > EVENT_STKTRC)
    {
        if (event->option & EVENT_TRAP_FLAG)
        {
            if ((event->option & EVENT_TRAP_MASK) > EVENT_STKTRC)
            {
                evtLog->eventType = ALARM_TRAP_EVENT;
            }
            else
            {
                evtLog->eventType = TRAP_EVENT;
            }
        }
        else
        {
            evtLog->eventType = ALARM_EVENT;
        }
    }
    else
    {
        evtLog->eventType = NORMAL_EVENT;
    }
}

static FILE *fdtrap = NULL;

/****************************************************************************
 *  Function:     SaveTrapToDisk()                                   
 *  Description:  This function saves trap event to IDE.     
 *                                                             
 *  In Parms:     EVENT_LOG_ENTRY_t *event - pointer to the formatted event
 *                                            log message data buffer 
 *  Out Parms:    None
 *  Returns:      OK or ERROR                                    
 *****************************************************************************/
static STATUS_t
SaveTrapToDisk(EVENT_LOG_HDR_t *evtLogHdr, EVENT_LOG_ENTRY_t *event)
{
    int      offset, slot;
    int      size = sizeof(EVENT_LOG_ENTRY_t);
	char     temps[SEVERITY_SIZE];

    slot = evtLogHdr->slotNo;
	slot++;	  /* convert to 1-base */
    event->slotno[0] = ((slot / 10) % 10) + '0';
    event->slotno[1] = (slot % 10) + '0';

    slot = evtLogHdr->shelf;
    event->pslotno[0] = ((slot / 10) % 10) + '0';
    event->pslotno[1] = (slot % 10) + '0';

    event->eventId = evtLogHdr->alarmId;
	event->slotNo = evtLogHdr->portNo;

    memset(temps, 0, SEVERITY_SIZE);
    strncpy(temps, event->severityName, 5);

    if (evtLogHdr->severity == TRAP_CRITICAL)
        strcpy(event->severityName, "CRIT*");
	else if (evtLogHdr->severity == TRAP_MAJOR)
        strcpy(event->severityName, "MAJOR");
	else if (evtLogHdr->severity == TRAP_MINOR)
        strcpy(event->severityName, "MINOR");
	else if (evtLogHdr->severity == TRAP_NA)
        strcpy(event->severityName, "EVENT");

    if (fdtrap == NULL)
    {
        fdtrap = fopen(TRAP_FILENAME, "a+");
    }
    if (fdtrap == NULL)
    {
        return (ERROR);
    }

    offset = trapIndex * size;
    if (fseek(fdtrap, offset, SEEK_SET) != 0)
    {
        /* fseek() returned error.  Try reopening trap file */

        (void) fclose (fdtrap);
        offset = trapIndex = 0;     /* go back to beginning of file */

        if (trapDebug)
        {
            printf("Reopening %s\n", TRAP_FILENAME);
        }
        fdtrap = fopen(TRAP_FILENAME, "a+");
        if (fdtrap == NULL)
        {
            return (ERROR);
        }
        if (fseek(fdtrap, offset, SEEK_SET) != 0)
        {
            /* fseek() returned error again.  Return error. */
            fdtrap = NULL;
            return (ERROR);
        }
    }
    fwrite((char *)event, size, 1, fdtrap);
    (void) fflush(fdtrap);
    /* ioctl is run on file descriptor field fdtrap->_file in FILE struct */
    (void) ioctl(fdtrap->_file,FIOSYNC,0);

    if (trapIndex >= (int)(MAX_TRAPFILE_ENTRIES - 1))
    {
        trapIndex = 0;
    }
    else
    {
        trapIndex++;
    }
    SaveTrapIndex();

    /* The following events are set back to original severity as an event yshen  */ 
    strncpy(event->severityName, temps, 5);

    return (OK);
}

/****************************************************************************
 *  Function:     SaveToDisk()                                   
 *  Description:  This function saves event log message to the IDE drive      
 *                                                             
 *  In Parms:     EVENT_LOG_ENTRY_t *strBuf - pointer to the formatted event
 *                                            log message data buffer 
 *  Out Parms:    None
 *  Returns:      OK or ERROR                                    
 *****************************************************************************/
static STATUS_t
SaveToDisk(EVENT_LOG_ENTRY_t *strBuf)
{
    int  offset;
    int  slot;
    char tmp[3], ptmp[3];

    /* Call function trace */
    SIW_FUNC_TRACE(SYS, SaveToDisk, strBuf, 0, 0, 0, 0);

    tmp[0] = strBuf->slotno[0];
    tmp[1] = strBuf->slotno[1];
    ptmp[0] = strBuf->pslotno[0];
    ptmp[1] = strBuf->pslotno[1];
    slot = ((strBuf->slotno[0] - '0') * 10) + (strBuf->slotno[1] - '0');
    slot++;
    strBuf->slotno[0] = ((slot / 10) % 10) + '0';
    strBuf->slotno[1] = (slot % 10) + '0';
    slot = ((strBuf->pslotno[0] - '0') * 10) + (strBuf->pslotno[1] - '0');
    slot++;
    strBuf->pslotno[0] = ((slot / 10) % 10) + '0';
    strBuf->pslotno[1] = (slot % 10) + '0';

    /* open Event Log file */
    if (OpenFile(&currentEventFile, eventFilename) == ERROR) 
    {   /* failed to open the file */
        /* get next error log file */
        if (NextEventFileGet() == ERROR)  /* failed to get next file */
        {
            /* Log Trace */
            SIW_DEBUG_TRACE(SYS, SaveToDisk, 0, TRACE_LEVEL_1,
                            0, 0, 0, 0, 0);
            return (ERROR);
        }
        if (OpenFile(&currentEventFile, eventFilename) == ERROR)
        {
            /* Log Trace */
            SIW_DEBUG_TRACE(SYS, SaveToDisk, 1, TRACE_LEVEL_1,
                            0, 0, 0, 0, 0);
            return (ERROR);
        }
    }

    /* seek to end of the file */
    if ((offset = lseek(currentEventFile, 0, SEEK_END)) != ERROR)
    {
        if (offset >= (int)(MAX_LOGFILE_SIZE - 1))
        {
            CloseFile (&currentEventFile);
            if (NextEventFileGet() == ERROR) /* failed to get next file */
            {
                /* Log Trace */
                SIW_DEBUG_TRACE(SYS, SaveToDisk, 2, TRACE_LEVEL_1,
                                0, 0, 0, 0, 0);
                return (ERROR);
            }
            if (OpenFile(&currentEventFile, eventFilename) == ERROR)
            {
                /* Log Trace */
                SIW_DEBUG_TRACE(SYS, SaveToDisk, 3, TRACE_LEVEL_1,
                                0, 0, 0, 0, 0);
                return (ERROR);
            }
            if ((offset = lseek(currentEventFile, 0, SEEK_END)) == ERROR)
            {
                CloseFile (&currentEventFile);
                /* Log Trace */
                SIW_DEBUG_TRACE(SYS, SaveToDisk, 4, TRACE_LEVEL_1,
                                0, 0, 0, 0, 0);
                return (ERROR);
            }
        }
        if (write(currentEventFile, (char *)strBuf, sizeof(EVENT_LOG_ENTRY_t)) == ERROR)
        {
            CloseFile (&currentEventFile);
            /* Log Trace */
            SIW_DEBUG_TRACE(SYS, SaveToDisk, 5, TRACE_LEVEL_1,
                            0, 0, 0, 0, 0);
            return (ERROR);
        }
        ioctl (currentEventFile, FIOSYNC, 0);
    }
    else
    {
        CloseFile (&currentEventFile);

        /* get next error log file */
        if (NextEventFileGet() == ERROR)  /* failed to get next file */
        {
            /* Log Trace */
            SIW_DEBUG_TRACE(SYS, SaveToDisk, 6, TRACE_LEVEL_1,
                            0, 0, 0, 0, 0);
            return (ERROR);
        }
        if (OpenFile(&currentEventFile, eventFilename) == ERROR)
        {
            /* Log Trace */
            SIW_DEBUG_TRACE(SYS, SaveToDisk, 7, TRACE_LEVEL_1,
                            0, 0, 0, 0, 0);
            return (ERROR);
        }
        if ((offset = lseek(currentEventFile, 0, SEEK_END)) == ERROR)
        {
            CloseFile (&currentEventFile);
            /* Log Trace */
            SIW_DEBUG_TRACE(SYS, SaveToDisk, 8, TRACE_LEVEL_1,
                            0, 0, 0, 0, 0);
            return (ERROR);
        }
        if (write(currentEventFile, (char *)strBuf, sizeof(EVENT_LOG_ENTRY_t)) == ERROR)
        {
            CloseFile (&currentEventFile);
            /* Log Trace */
            SIW_DEBUG_TRACE(SYS, SaveToDisk, 9, TRACE_LEVEL_1,
                            0, 0, 0, 0, 0);
            return (ERROR);
        }
        ioctl (currentEventFile, FIOSYNC, 0);
    }

    strBuf->slotno[0] = tmp[0];
    strBuf->slotno[1] = tmp[1];
    strBuf->pslotno[0] = ptmp[0];
    strBuf->pslotno[1] = ptmp[1];

    return (OK);
}

/******************************************************************************
 * Function:     OpenFile() 
 * Description:  This function opens a static file, and will report ERROR before 
 *                3 times retry.
 * In Parms:      
 * Out Parms:     
 * Returns:       OK or ERROR                                      
 *******************************************************************************/
static STATUS_t
OpenFile(int *fd, char * fname)
{
    STATUS_t status = ERROR;
    int      tryCnt = 0;

    /* Call function trace */
    SIW_FUNC_TRACE(SYS, OpenFile, *fd, fname, 0, 0, 0);

    /* If file is already open then just return */ 
    if (*fd != ERROR)
	{
        if (lseek(*fd, 0, SEEK_END) == ERROR)
            CloseFile (fd);
        else
            return (OK);
	}

    /* open the file */
    while (tryCnt++ < FILE_OPEN_RETRY)
    {    /* file system is not reliable, try 3 times */
        *fd = open(fname, O_WRONLY, 0644);
        if ( *fd != ERROR)
        {
            status = OK;
            break;
        }
    }

    return (status);
}

/******************************************************************************
 * Function:     CloseFile() 
 * Description:  This function closes a static file. 
 * In Parms:      
 * Out Parms:     
 * Returns:       OK or ERROR                                      
 *******************************************************************************/
static STATUS_t
CloseFile(int *fd)
{
    close (*fd);
    *fd = ERROR;

    return (OK);
}

/******************************************************************************
 *  Function:     ErrLogInit()                                    
 *  Description:  This function restores file information for error and 
 *                event log        
 *                                                             
 *  In Parms:     None                                            
 *  Out Parms:    None
 *  Returns:      OK or ERROR                                     
 *****************************************************************************/
static STATUS_t
ErrLogInit()
{
    char dataBuf[5];

	SysRtcRead(RTC_LOG, 0, dataBuf, 1);
    sysEventSlot = (U16_t)dataBuf[0];

    /* if index is wrapped arround, reset the index  */
    if (sysEventSlot >= MAX_EVENT_FILES)
    {
        sysEventSlot = 0;
    }
    sprintf(eventFilename, "%s%02d%s", EVENT_FILENAME, sysEventSlot,
                                       LOG_EXTENSION);
    EventSlotSave();

    return (OK);
}

/***************************************************************************
 *  Function:     NextEventFileGet()
 *  Description:  This function searchs the next available file slot for 
 *                storing event log message.
 * 
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 ****************************************************************************/
static STATUS_t
NextEventFileGet()
{
    int     fd;

    /* Call function trace */
    SIW_FUNC_TRACE(SYS, NextEventFileGet, 0, 0, 0, 0, 0);

    /* increment of the Event Slot counter */
    sysEventSlot++;

    /* if index is wrapped arround, reset the index  */
    if (sysEventSlot >= MAX_EVENT_FILES)
    {
        sysEventSlot = 0;
    }

    /* generate the next file name */
    memset(eventFilename, 0, sizeof(eventFilename));
    sprintf(eventFilename, "%s%02d%s", EVENT_FILENAME, sysEventSlot,
            LOG_EXTENSION);

    /* save info of the eventLog */
    EventSlotSave();

    /* reset the file content */
    if ((fd = creat(eventFilename, 0644)) == ERROR)
        return ERROR;
    close (fd);
    return OK;
}

/****************************************************************************
 *  Function:     EventSlotSave()
 *  Description:  This function saves current slot # for event log into 
 *                NOVRAM.
 * 
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 ****************************************************************************/
static void
EventSlotSave()
{
	char dataBuf[5];

    dataBuf[0] = sysEventSlot;
	SysRtcWrite(RTC_LOG, 0, dataBuf, 1);  
}

/****************************************************************************
 *  Function:     EventPrint()
 *  Description:  This function shows all saved event logs in IDE drive on 
 *                screen. 
 * 
 *  In Parms:     char *date     - "dd/mm/yyyy" (or NULL for all)
 *                int   pslot    - physical slot number 0-15 (or -1 for all)
 *                int   severity - bit mask: fatal=8|major=4|minor=2|info=1
 *  Out Parms:    None
 *  Returns:      None
 ****************************************************************************/
void
EventPrint(U8_t *date, U32_t pslot, U32_t severity)
{  
    int  i, offset, slot, tryCnt;
    int  fd;
    int  size = sizeof(EVENT_LOG_ENTRY_t);
    int  lineNum = 0;
    char fname[25];
    EVENT_LOG_ENTRY_t event;

    /* clear screen */
    ClrScrn();

    for (i=0, slot=sysEventSlot; i<MAX_EVENT_FILES; i++, slot--)
    {
        sprintf(fname, "%s%02d%s", EVENT_FILENAME, slot,
                LOG_EXTENSION);
        if (slot == 0)
        {
            slot = MAX_EVENT_FILES;
        }
    
        /* request the Semaphore to access the Event Log File */
        SiwSemTake(SEM_EVENTLOG, WAIT_FOREVER);

        tryCnt = 0;
        while (tryCnt++ < OPEN_TRY_COUNT)
        /* file system is not reliable, try 3 times */
        {
            if ( (fd = open(fname, O_RDONLY, 0644)) != ERROR)
                break;
        }
        if (fd == ERROR)
        {
             /* release the Semaphore for accessing to the Event Log File */ 
             SiwSemGive(SEM_EVENTLOG); 
             continue;
        }

        /* read from the end of the file */
        offset = lseek(fd, 0, SEEK_END);

        /* release the Semaphore for accessing to the Event Log File */ 
        SiwSemGive(SEM_EVENTLOG); 

        /* print out Event Log */
        while (offset >  0)
        {
            offset -= size;
            offset = lseek(fd, offset, SEEK_SET);
            read(fd, (char *)&event, size);  

            /* filter event based on date, pslot, and severity */

            if (date && strncmp(event.datestamp, date, DATESTAMP_SIZE) != 0)
            {
                continue;
            }
            if (((int)pslot != -1) && (((int)pslot + 1) != atoi(event.pslotno)))
            {
                continue;
            }
            if (!(severity & (1 << (EVENT_INFO - event.severity))))
            {
                continue;
            }
            printf("%s\n %s\n", &event.datestamp[0], event.message);
            if (CheckScroll(&lineNum, MAX_NUM_LINES) == QUIT)
            {
                close (fd);
                return;
            }
        }
        close (fd);
    }
}

/****************************************************************************
 *  Function:    EventClr()
 *  Description: This function removes all saved Event logs from IDE drive
 * 
 *  In Parms:    None
 *  Out Parms:   one
 *  Returns:     OK or ERROR
 ****************************************************************************/
STATUS_t
EventClr()
{
    char  fname[50];
    UINT8 i;

    /* request the Semaphore to access the Event Log File */
    SiwSemTake(SEM_EVENTLOG, WAIT_FOREVER);

    for (i=0; i<MAX_EVENT_FILES; i++)
    {
        memset(fname, 0, sizeof(fname));
        sprintf(fname, "%s%02d%s", EVENT_FILENAME, i,
                                          LOG_EXTENSION);
        remove(fname);
    } 
 
    sysEventSlot = 0;
    sprintf(eventFilename, "%s%02d%s", EVENT_FILENAME, sysEventSlot,
                                      LOG_EXTENSION);
    EventSlotSave();

    /* release the Semaphore for accessing to the Event Log File */
    SiwSemGive(SEM_EVENTLOG);

    return OK;
}

STATUS_t SysTrapEventPrint(int days, int severity)
{
	struct tm  curtm;
	char datestamp[12];
	int day, month, year;
	SNMP_EVENT_LOG_t *bufPtr;
	int i;
	
    /* get the current system time */
    SiwDateTimeGet(&curtm);
	memset(datestamp, 0, sizeof(datestamp)); 
	strftime(datestamp, DATESTAMP_SIZE, "%m/%d/%Y", &curtm);
	SysTrapFileSet();
	bufPtr = SiwAllocMem(SIW_DYNAMIC, sizeof(SNMP_EVENT_LOG_t)+1, 0);

	for (i=0; i<days; i++)
	{
	    memset(bufPtr, 0, sizeof(SNMP_EVENT_LOG_t)+1);
		if (SysTrapGet(&datestamp[0], 0, severity, bufPtr) == ERROR)
		    break;

		day = (((datestamp[3] - '0') * 10) + (datestamp[4] - '0')) - 1;
		if (day < 0)
		{
		    day = 0;
			month = (((datestamp[0] - '0') * 10) + (datestamp[1] - '0')) -1;
			if (month < 0)
			{
			    month = 0;
				year = (((datestamp[6] - '0') * 1000) + 
				       ((datestamp[7] - '0') * 100) + 
					   ((datestamp[8] - '0') * 10) +
				       (datestamp[9] - '0')) - 1;

				datestamp[0] = '0';
			    datestamp[1] = '0';
				datestamp[3] = '0';
			    datestamp[4] = '0';
				datestamp[6] = ((year / 1000) % 10) + '0';
				datestamp[7] = ((year / 100) % 10) + '0';
				datestamp[8] = ((year / 10) % 10) + '0';
				datestamp[9] = (year % 10) + '0';
			}
			else
			{
			    datestamp[0] = ((month / 10) % 10) + '0';
			    datestamp[1] = (month % 10) + '0';
				datestamp[3] = '0';
			    datestamp[4] = '0';
			}
		}
		else
		{
		    datestamp[3] = ((day / 10) % 10) + '0';
			datestamp[4] = (day % 10) + '0';
		}        
	}

    SiwFreeMem(bufPtr);

    return OK;
}

/****************************************************************************
 *  Function:     SysTrapFileSet()
 *  Description:  This function copies TRAP.LOG for searching 
 * 
 *  In Parms:     
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 ****************************************************************************/
STATUS_t SysTrapFileSet()
{
    /* check if SNMP file is open */
    if (snmpFileOpen)
    {
        snmpFileOpen = FALSE;
        fclose (snmpFd);
    }

    SiwSemTake(SEM_TRAPLOG, WAIT_FOREVER);
    if (trapIndex == 0)
    {
        snmpIndex = MAX_TRAPFILE_ENTRIES - 1;
    }
    else
    {
        snmpIndex = (int)(trapIndex - 1);
    }
    snmpIndexSave = snmpIndex;
    copy (TRAP_FILENAME, SNMP_FILENAME);
    SiwSemGive(SEM_TRAPLOG);
    if ((snmpFd = fopen(SNMP_FILENAME, "r")) == NULL)
    {
        snmpFileOpen = FALSE;
    }
    else
    {
        snmpFileOpen = TRUE;
    }
 
    taskDelay (1);
    return (OK);
}

/****************************************************************************
 *  Function:     SysTrapGet()
 *  Description:  This function retrieves an event log from sorted event
 *                record.
 *
 *  In Parms:     char *date - date of the event log occured 
 *                int slotNo - slot to generate the event log
 *                int severity - event severity
 *  Out Parms:    SNMP_EVENT_LOG_t *bufPtr - where to return event log
 *  Returns:      OK or ERROR
 ****************************************************************************/
STATUS_t SysTrapGet(char *date, int slotNo, int severity, SNMP_EVENT_LOG_t *bufPtr)
{  
    int  offset, s;
    int  size = sizeof(EVENT_LOG_ENTRY_t);
    STATUS_t status = ERROR;
    EVENT_LOG_ENTRY_t event;

    /* check if SNMP file is open */
    if (!snmpFileOpen)
    {
        return (status);
    } 
            
    do
    {
        offset = snmpIndex * size;
        if (fseek(snmpFd, offset, SEEK_SET) != 0)
        {
            return (status);
        }

        if (feof(snmpFd))
        {
            return (status);
        }

        if (fread((char *)&event, size, 1, snmpFd) != 1)
        {
            break;
        }

        snmpIndex--;
        if (snmpIndex < 0)
        {
            snmpIndex = MAX_TRAPFILE_ENTRIES - 1;
        }

        if (date != NULL)
        {
            if (strncmp(event.datestamp, date, DATESTAMP_SIZE) != 0)
			{
                return OK;
			}
       
			CopyString(&bufPtr->date[0], &event.datestamp[0],
                       DATESTAMP_SIZE, DATESTAMP_SIZE, ' ');
            CopyString(&bufPtr->time[0], &event.timestamp[0],
                       TIMESTAMP_SIZE, TIMESTAMP_SIZE, ' ');
        }
        else
            return (status); 

        if (slotNo > 0)
        {
            s = ((event.slotno[0] - '0') * 10) + (event.slotno[1] - '0');
            if (s != slotNo)
            {
                continue;
            }
        }
		CopyString(&bufPtr->shelf[0], &event.pslotno[0],
                   SLOT_NUMBER_SIZE, SLOT_NUMBER_SIZE, ' ');
        CopyString(&bufPtr->slotno[0], &event.slotno[0],
                   SLOT_NUMBER_SIZE, SLOT_NUMBER_SIZE, ' ');
		bufPtr->portno[0] = ((event.slotNo / 10) % 10) + '0';
        bufPtr->portno[1] = (event.slotNo % 10) + '0';
        
        if (severity > 0)
        {
            if (event.severity != severity)
            {
                continue;
            }
            if (severity == TRAP_CRITICAL)
            {
                CopyString(&bufPtr->severityName[0], "CRITI", 5, 5, ' ');
            }
            else if (severity == TRAP_MAJOR)
            {
                CopyString(&bufPtr->severityName[0], "MAJOR", 5, 5, ' ');
            }
            else if (severity == TRAP_MINOR)
            {
                CopyString(&bufPtr->severityName[0], "MINOR", 5, 5, ' ');
            }
            else if (severity == TRAP_NA)
            {
                CopyString(&bufPtr->severityName[0], "EVENT", 5, 5, ' ');
            }
        }
        else
        {
            CopyString(&bufPtr->severityName[0], &event.severityName[0],
                       SEVERITY_SIZE, SEVERITY_SIZE, ' ');
        }
		sprintf(&bufPtr->eventType[0], "%d", event.eventId);
        /* CopyString(&bufPtr->eventType[0], "REPT", 4, 4, ' '); */
        CopyString(&bufPtr->message[0], &event.message[0],
                   EVENT_MESSAGE_SIZE, EVENT_MESSAGE_SIZE, ' ');
        bufPtr->blankpad0[0] = ' ';
        bufPtr->blankpad1[0] = ' ';
        bufPtr->blankpad2[0] = ' ';
        bufPtr->blankpad3[0] = ' ';
        bufPtr->blankpad4[0] = ' ';
        bufPtr->blankpad5[0] = ' ';
        bufPtr->blankpad6[0] = '\0';
        bufPtr->nullterm = '\0';
		printf("%s\n", &bufPtr->date[0]);
		printf("%s\n", &bufPtr->message[0]);      
    } while (snmpIndex != snmpIndexSave);

    if (snmpIndex == snmpIndexSave)
    {
        status = ERROR;
    }

    return (status);
}

/*******************************************************************************
 *  Function:    EventTrap()
 *  Description: This function forwards event buffer to Trap manager. 
 *
 *  In Parms:    MSG_t *msgBuf - pointer to the MRPC message buffer
 *               BOOL_t standby - this event is coming from active SCC
 *  Out Parms:   None
 *  Returns:     None
 *******************************************************************************/
static void 
EventTrap(MSG_t *msgBuf, BOOL_t standby)
{
    MSG_BUF_HDR_t  eventBuf;
	MSG_t          *msgBufPtr;

    /* filling message header */
    eventBuf.srcTaskId = TASK_LOGD;
    eventBuf.dstTaskId = TASK_TRAP;
    eventBuf.msgType = EVENT_TRAP;
    eventBuf.srcSlot = SiwMySlotNo();

    /* get location of event entry in buffer */
	msgBufPtr = (MSG_t *)SiwAllocMem(SIW_EVENT, sizeof(MSG_t), WAIT_FOREVER);
    memcpy(&msgBufPtr->data[0], &msgBuf->data[0], MAX_BUFF_LEN);
    eventBuf.msgBufPtr = msgBufPtr;

    /* send data buffer to Trap Manager */
    if (SiwMsgQSend(MSGQ2AM, &eventBuf,
                     sizeof(MSG_BUF_HDR_t), 10, SIW_MSG_PRI_NORMAL) != OK)
    {
	    sendFailCount++;
		SiwFreeMem(msgBufPtr);
    }

if (trapDebug)
printf("forward Trap to TrapManager: %p\n", msgBuf);

#ifdef TRAP_FOWARD
	/* send data buffer to Trap Manager */
    if (SiwMsgQSend(MSGQ2TRAP, &eventBuf,
                     sizeof(MSG_BUF_HDR_t), 10, SIW_MSG_PRI_NORMAL) != OK)
    {
	    sendFailCount++;
		SiwFreeMem(msgBufPtr);
    }
#endif
}

/*******************************************************************************
 *  Function:    EventDispatch()
 *  Description: This function forwards event buffer to all registered tasks.
 *
 *  In Parms:    MSG_t *msgBuf - pointer to the MRPC message buffer
 *               int alarmId - alarm ID
 *               
 *  Out Parms:   None
 *  Returns:     None
 *******************************************************************************/
static void
EventDispatch(MSG_t *msgBuf, int alarmId)
{
    MSG_BUF_HDR_t       eventBuf;
    EVENT_REG_NODE_t  **tblPtr = NULL;
    EVENT_REG_NODE_t   *tempPtr = NULL;
    int                 loopCount;
	MSG_t              *msgBufPtr;

    /* validate check */
#ifdef TRAP
    if (TrapRegTblGet(alarmId, (void **)&tempPtr) != OK)
    {
        return;
    }
    tblPtr = (EVENT_REG_NODE_t  **)tempPtr;
#else
    if ((alarmId <= EVENT_STKTRC) ||
        (alarmId >= 10000))
    {
        return;
    }

    /* get the table entry */
    tblPtr = &eventRegTable[alarmId];
#endif

    tempPtr = *tblPtr;
    loopCount = 0;

    /* dispatch event to the registered task */
    while ((tempPtr != NULL) && 
           ((tempPtr)->taskId != NULL) && 
           ((tempPtr)->msgQId != NULL) &&
           (loopCount < 10))
    {
        /* fill in message header */
        eventBuf.srcTaskId = TASK_LOGD;
        eventBuf.dstTaskId = tempPtr->taskId;
        eventBuf.msgType = EVENT_ALARM;
        eventBuf.srcSlot = SIW_GET_SLOT_NO(msgBuf);

        /* get location of event entry in buffer */
		msgBufPtr = (MSG_t *)SiwAllocMem(SIW_EVENT, sizeof(MSG_t), WAIT_FOREVER);
        memcpy(&msgBufPtr->data[0], &msgBuf->data[0], MAX_BUFF_LEN);
        eventBuf.msgBufPtr = msgBufPtr;
 
if (alarmId == trapDebugId)
{
printf("Received alarm: %d\n", alarmId);
printf("forward Sar Buffer to Message Queue: %p,  %p\n", msgBuf, tempPtr->msgQId);
}

        /* send the error event to the Error Log Task */
        if (SiwMsgQSend(tempPtr->msgQId, &eventBuf,
               sizeof(MSG_BUF_HDR_t), 10, SIW_MSG_PRI_NORMAL) != OK)
        {
		    sendFailCount++;
			SiwFreeMem(msgBufPtr);
        }

        /* index to next registered task */
        tempPtr = tempPtr->next;
        loopCount++;
    }
}

/*******************************************************************************
 *  Function:    EventTrapRegister()
 *  Description: This function allows a task to register for receiving 
 *               a trap event.
 *
 *  In Parms:    int alarmId - alarm ID
 *               SIW_MQID_t msgQId - message queue ID for reveiving the event
 *               BOOL_t standby - this event is coming from active SCC
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *******************************************************************************/
STATUS_t EventTrapRegister(int alarmId, SIW_MQID_t msgQId, BOOL_t standby)
{
    EVENT_REG_NODE_t **tblPtr = NULL;
    EVENT_REG_NODE_t *eventNode = NULL;
#ifdef TRAP
    EVENT_REG_NODE_t *tempPtr = NULL;
#endif

    /* validation check */
#ifdef TRAP
    if (TrapRegTblGet(alarmId, (void **)&tempPtr) != OK)
    {
        return (ERROR);
    }
    tblPtr = (EVENT_REG_NODE_t  **)tempPtr;
#else
    if ((alarmId <= EVENT_STKTRC) ||
        (alarmId >= 10000))
    {
        return (ERROR);
    }

    /* get the table entry */
    tblPtr = &eventRegTable[alarmId];
#endif

    /* allocate a memory block */
    eventNode = (EVENT_REG_NODE_t *)SiwAllocMem(SIW_STATIC, 
                                       sizeof(EVENT_REG_NODE_t), WAIT_FOREVER);
    /* fill in the event node */
    eventNode->taskId = SiwTaskIdSelf();
    eventNode->msgQId = msgQId;
    eventNode->standby = standby;
    /* walk through the link list */
    while (*tblPtr != NULL)
    {
        tblPtr = &(*tblPtr)->next;
    }
    /* link the new node to the list */
    *tblPtr = eventNode;
    (*tblPtr)->next = NULL;
    return (OK);
}

/*******************************************************************************
 *  Function:    EventBlockTrace()
 *  Description: This function prints information for event block trace. 
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 *******************************************************************************/
void EventBlockTrace()
{
    printf("\n");
    printf("SiwAllocMemCount:      %d\n", memAllocCount);
    printf("msgQSendOK:            %d\n", sendOKCount);
    printf("msgQSendFail:          %d\n\n", sendFailCount);
    printf("msgRecvdTotal:         %d\n", msgRecvdCount);
    printf("recvTimeout:           %d\n", recvTimeoutCount);
    printf("recvWrongSizeMessage:  %d\n", wrongSizeCount);
    printf("recvMsgHdrMessage:     %d\n", recvMHdCount);
    printf("recvUnknownMessage:    %d\n\n", recvUnknownCount);
    printf("emptyEventError:       %d\n", nullEventCount);
    printf("emptyEventError0Ptr:   %d\n", nullEventxxCount);
}

/*******************************************************************************
 *  Function:    EventTrapRegisterDisp()
 *  Description: This function prints out all trap registered tasks.
 *
 *  In Parms:    int alarmId - alarm ID
 *  Out Parms:   None
 *  Returns:     None
 *******************************************************************************/
void EventTrapRegisterDisp(int alarmId)
{
    EVENT_REG_NODE_t  **tblPtr = NULL;
    EVENT_REG_NODE_t   *tempPtr = NULL;

    /* validate check */
#ifdef TRAP
    if (TrapRegTblGet(alarmId, (void **)&tempPtr) != OK)
    {
        return;
    }
    tblPtr = (EVENT_REG_NODE_t  **)tempPtr;
#else
    if ((alarmId <= EVENT_STKTRC) ||
        (alarmId >= NUM_ALARM_TABLE_ENTRIES))
    {
        return;
    }

    /* get the table entry */
    tblPtr = &eventRegTable[alarmId];
#endif

    tempPtr = *tblPtr;

    /* search registered task */
    while ((tempPtr != NULL) && 
           ((tempPtr)->taskId != NULL) && 
           ((tempPtr)->msgQId != NULL))
    {
        printf("Registered MSGQ[%p] for Trap ID %d\n", tempPtr->msgQId, alarmId);

        /* index to next registered task */
        tempPtr = tempPtr->next;
    }
}

/*******************************************************************************
 *  Function:    SysQueueEvent()
 *  Description: This function allows application tasks to pass event to
 *               Event Log task.
 *
 *  In Parms:    MSG_t *bufPtr
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *******************************************************************************/
STATUS_t SysQueueEvent(MSG_t *bufPtr)
{
    MSG_t         *msgBuf = NULL;
    MSG_BUF_HDR_t  eventBuf;
    S32_t          msgStatus = 0;

    /* validation check */
    if (bufPtr == NULL)
    {
        return (ERROR);
    }

    /* get an Dynamic memory buffer */
    msgBuf = (MSG_t *)SiwAllocMem(SIW_EVENT, sizeof(MSG_t), WAIT_FOREVER);

    /* copy data to event buffer */
    memcpy(&msgBuf->data[0], &bufPtr->data[0], 
           (sizeof(EVENT_LOG_ENTRY_t) + sizeof(EVENT_LOG_HDR_t)));

    /* filling message header */
    eventBuf.dstTaskId = TASK_LOGD;
    eventBuf.msgType = EVENT_LOG_MSG;
    eventBuf.srcSlot = SiwMyPhySlotNo();
    eventBuf.msgBufPtr = msgBuf;

    /* send the error event to the Event Log Task */
    if ((msgStatus = SiwMsgQSend(MSGQ2LOG, &eventBuf,
                 sizeof(MSG_BUF_HDR_t), 10, SIW_MSG_PRI_NORMAL)) != OK)
    {
        return (ERROR);
    }

    return (OK);
}

void TrapDebugOn()
{
    trapDebug = TRUE;
}

void TrapDebugOff()
{
    trapDebug = FALSE;
}

static void SaveTrapIndex()
{
    char dataBuf[5];

    dataBuf[0] = (S8_t)(trapIndex >> 8);
	dataBuf[1] = (S8_t)trapIndex;
	dataBuf[2] = (S8_t)(trapIndex >> 8);
	dataBuf[3] = (S8_t)trapIndex;
	SysRtcWrite(RTC_LOG, 1, dataBuf, 4);
}

void GetTrapIndex()
{
    char dataBuf[5];
	U16_t tmp1, tmp2;

    dataBuf[0] = (S8_t)(trapIndex >> 8);
	dataBuf[1] = (S8_t)trapIndex;
	dataBuf[2] = (S8_t)(trapIndex >> 8);
	dataBuf[3] = (S8_t)trapIndex;
	SysRtcRead(RTC_LOG, 1, dataBuf, 4);

    tmp1 = (U16_t)((dataBuf[0] << 8)+ dataBuf[1]);
    tmp2 = (U16_t)(U16_t)((dataBuf[2] << 8)+ dataBuf[3]);

    if (tmp1 >= (int)(MAX_TRAPFILE_ENTRIES - 1))
	{
	    trapIndex = 0;
        remove (TRAP_FILENAME);
		return;
	}

    if (tmp1 == tmp2)
    {
        trapIndex = tmp1;
    }
    else
    {
        trapIndex = 0;
        remove (TRAP_FILENAME);
    }
}

STATUS_t TrapEventClr()
{
    /* request the Semaphore to access the Trap Log File */
    SiwSemTake(SEM_TRAPLOG, WAIT_FOREVER);

    remove (TRAP_FILENAME);
    trapIndex = 0;

    SiwSemGive(SEM_TRAPLOG);
    
    return (OK);
}

void TrapEventPrint()
{
}

/* start lsevt for web */

/****************************************************************************
 *  Function:     SysFilterAllEvt()
 *  Description:  This function is used to save web query event information:
 *                date, physical slot number and severity. 
 * 
 *  In Parms:     char *date - date of the event 
 *                int slotNo - physical slot number of the event
 *                int severity - event severity
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 *
 ****************************************************************************/
STATUS_t 
SysFilterAllEvt(
	char *date, 
	int slotNo, 
	int severity)
{

    int lockKey;

    lockKey = intLock();
    if (webListOn == FALSE)
    {
        webListOn = TRUE;
        intUnlock(lockKey);
    }
    else
    {
        intUnlock(lockKey);
        return ERROR;
    }
        
    memset(evtDate, 0, sizeof(evtDate));

    if (date != NULL)
    {
        memcpy(evtDate, date, DATESTAMP_SIZE);
    }
    evtSlotNo = slotNo;      /* 0 - 16 */
    evtSeverity = severity;  /* 0 - 4 */

    return OK;
}

/****************************************************************************
 *  Function:     SysCopyOpenEvtFile()
 *
 *  Description:  This function is used to copy event log file to another 
 *                file and open the file.
 *
 *  In Parms:     int   fileIndex 
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 *
 ****************************************************************************/
STATUS_t
SysCopyOpenEvtFile(
	int fileIdx)
{
    int  slot;
    char fname[25];

    if (webListOn == FALSE)
        return ERROR;

    fileIndex = fileIdx;

    if (fileIdx == 0)
    {
        slot = sysEventSlot;
    }
    else
    {
        slot = preEventSlot - 1;

        if (slot == 0)
        {
            slot = MAX_EVENT_FILES;
        }
    }

    preEventSlot = slot;   

    sprintf(fname, "%s%02d%s", EVENT_FILENAME, slot, LOG_EXTENSION);

    if (fileIdx == 0)
    {
        /* request the Semaphore for accessing the current Event Log File */

        SiwSemTake(SEM_EVENTLOG, WAIT_FOREVER);

        remove(EVENT_COPY_FILENAME);
        copy(fname, EVENT_COPY_FILENAME);

        /* release the Semaphore for accessing the current Event Log File */ 
        SiwSemGive(SEM_EVENTLOG); 
    }
    else
    {
        remove(EVENT_COPY_FILENAME);
        copy(fname, EVENT_COPY_FILENAME);
    }

    if ((evtCopyFd = open(EVENT_COPY_FILENAME, O_RDONLY, 0644)) != ERROR)
    {
        evtCpFileOpen = TRUE;
        evtStartOffset = lseek(evtCopyFd, 0, SEEK_END); /* read from end */
        if (evtStartOffset > 0)
        {
            return OK;
        }
        else /* log file is empty */
        {
            if (fileIndex == LAST_FILE_INDEX)
            {
                webListOn = FALSE;
            }
            close(evtCopyFd);
            evtCpFileOpen = FALSE;
            return ERROR;
        }
    }
    else
    {
        evtCpFileOpen = FALSE;
        
        if (fileIndex == LAST_FILE_INDEX)
        {
            webListOn = FALSE;
        }

        return ERROR;
    }

    return OK;
}

/****************************************************************************
 *  Function:     SysRetrieveEvt()
 *
 *  Description:  This function retrieves an event log from event log file
 *                copy
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      VENT_LOG_ENTRY_t *evtLog - return the event, if it is 
 *                                           passed the filtering
 *                NULL                     - otherwise
 *
 ****************************************************************************/
EVENT_LOG_ENTRY_t *SysRetrieveEvt()
{  
    int                       size = sizeof(EVENT_LOG_ENTRY_t);
    static EVENT_LOG_ENTRY_t  evtLogRecord;
    BOOL_t                    getEvent = FALSE;

    if ((evtStartOffset <= 0) || (evtCpFileOpen == FALSE) || (webListOn == FALSE))
    {
        if (fileIndex == LAST_FILE_INDEX)
        {
            
            webListOn = FALSE;
        }
        return NULL;
    }
   
    while (evtStartOffset > 0)
    {
        evtStartOffset -= size;  
        evtStartOffset = lseek(evtCopyFd, evtStartOffset, SEEK_SET);
        read(evtCopyFd, (char *)&evtLogRecord, size);  
        getEvent = TRUE;
  
        /* filter event based on evtDate, evtSlotNo, and evtSeverity */

        if (evtDate[0] != '\0') 
        {
            if (strncmp(evtLogRecord.datestamp, evtDate, DATESTAMP_SIZE) != 0)
            {
                getEvent = FALSE;
                continue;
            }
        }

        if ((evtSlotNo != 0) && (evtSlotNo != atoi(evtLogRecord.pslotno)))
        {
            getEvent = FALSE;
            continue;
        }

        if ((evtSeverity != 0) && (evtSeverity != evtLogRecord.severity))
        {
            getEvent = FALSE;
            continue;
        }
        break;

     } /* while */

     if (evtStartOffset <= 0)
     {
         close(evtCopyFd);
         evtCpFileOpen = FALSE;
     }

     if (getEvent == TRUE)
     {
         return (&evtLogRecord);
     }
     else
     {
         if (fileIndex == LAST_FILE_INDEX)
             webListOn = FALSE;

         return (NULL);
     }

    return NULL;
}

void EventLogTest()
{
    SiwEventLog(NULL, SIW_EVENT_SYM(SYS, CPUISHOLD), "DebugTask");
}

void ErrEventResetEnable()
{
    errHandlerEnable = TRUE;
}

void ErrEventResetDisable()
{
    errHandlerEnable = FALSE;
}

U16_t GetSysEventSlot ()
{
    return (sysEventSlot);
}

void trapRegister(int alarmId)
{
    EventTrapRegister(alarmId, MSGQ2TEST, FALSE);
}

stsPerfMonManual_t trapTmpTest;

void trapSend(int alarmId, int severity, int num)
{
    TrapEventLog(stsPerfMonManualConfigChange_TRAP, 0, 0, 5, 1, severity, (U32_t *)&trapTmpTest, SIW_EVENT_SYM(SYS, UNKNOWNCRDTYP), num);
}



int trapReceive()
{
    MSG_t             * msgPtr;
    MSG_BUF_HDR_t      siwMsg;
	EVENT_LOG_HDR_t   *evtLogHdr = NULL;

    for (;;)
	{
        if (SiwMsgQReceive(MSGQ2TEST, &siwMsg,
                sizeof(MSG_BUF_HDR_t), WAIT_FOREVER) > 0)
        {
            msgPtr = (MSG_t *) siwMsg.msgBufPtr;
			evtLogHdr = (EVENT_LOG_HDR_t *)&msgPtr->data[0];
            printf("Alarm ID: %d\n", evtLogHdr->alarmId);
		printf("Alarm Trap type: %d\n", evtLogHdr->trapType);
            printf("Alarm Severity: %d\n", evtLogHdr->severity);
	printf("Alarm occurs in shelf/slot/port : %d/%d/%d\n", evtLogHdr->shelf, evtLogHdr->slotNo, evtLogHdr->portNo);
		printf("Alarm Severity: %d\n", evtLogHdr->severity);
		printf("Alarm Severity: %d\n", evtLogHdr->severity);
            SiwFreeMem(msgPtr); 
		}
	}

    return OK;
}



STATUS_t
clear_log(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
    TrapEventClr();

    return OK;
}

STATUS_t
read_log(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
    SysTrapEventPrint(dataVal[0].u.val[0], dataVal[1].u.val[0]);

    return OK;
}

void trapSend2(int alarmId, int shelf, int slot, int port, int severity, int num)
{
	TrapEventLog(stsPerfMonManualConfigChange_TRAP, 0, shelf, slot, port, severity, (U32_t *)&trapTmpTest, SIW_EVENT_SYM(SYS, UNKNOWNCRDTYP), num);
#if 0
	TrapEventLog(alarmId, shelf, slot, port, severity, dumPtr, SIW_EVENT_SYM(SYS, UNKNOWNCRDTYP), num);
#endif
}