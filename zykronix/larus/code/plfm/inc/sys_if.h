/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: sys_if.h
 *
 *Define for System Interface
 *
 *Note:
 *
 *Author(s):
 *
 */

#ifndef _SYS_IF_H_
#define _SYS_IF_H_

#ifndef BOOT
#include <blkIo.h>
#include <stdio.h>
#include <sysLib.h>
#include <sys/stat.h>
#include <taskLib.h>
#include <siw_if.h>
#include <sys_os.h>

#endif
#include <dosFsLib.h>

/***********************************************************************
 *  Marco Defines 
 **********************************************************************/
#define SYS_START_TYPE       sysStartType

extern BOOL_t errHandlerEnable ;
/************************************************************************
 *  General defines for screen display
 *************************************************************************/
#define MAX_NUM_LINES           20
#define NEXT_PAGE               1
#define QUIT                    2
#define IP_ADDR_SIZE            16
#define MAC_ADDR_SIZE           6
#define TFTP_KEY_SIZE           6

/***********************************************************************
 *  Defines for sys log
 ***********************************************************************/
#define SYS_FILTER_EXCLUDE_NONE 0x00
#define SYS_FILTER_EXCLUDE_EQ   0x01
#define SYS_FILTER_EXCLUDE_GT   0x02
#define SYS_FILTER_EXCLUDE_LT   0x04
#define SYS_FILTER_INCLUDE_ALL  (SYS_FILTER_EXCLUDE_NONE)
#define SYS_FILTER_INCLUDE_GEQ  (SYS_FILTER_EXCLUDE_LT)
#define SYS_FILTER_INCLUDE_GT   (SYS_FILTER_EXCLUDE_LT | SYS_FILTER_EXCLUDE_EQ)
#define SYS_FILTER_INCLUDE_LEQ  (SYS_FILTER_EXCLUDE_GT)
#define SYS_FILTER_INCLUDE_LT   (SYS_FILTER_EXCLUDE_GT | SYS_FILTER_EXCLUDE_EQ)
#define SYS_FILTER_INCLUDE_EQ   (SYS_FILTER_EXCLUDE_LT | SYS_FILTER_EXCLUDE_GT)

/*
 * Number of generic filter option data
 */
#define SYS_LOG_OPTION_NUM  5

/***********************************************************************
 *  Typedefs for sys log
 ***********************************************************************/

/*
 * Sys filter info
 */
typedef struct {
    union {
        struct
        {
            U8_t    pad1;               /* unused, must be zero */
            U8_t    year;               /* year:   0 to 129 (since 1970) */
            U8_t    month;              /* month:  1 to 12 */
            U8_t    day;                /* day:    1 to 31 */
        } v1;
        S32_t  ymd;                     /* 0x0 year month day */
    } date_key;                         /* for comparing with record date */
    S32_t   s32_key[SYS_LOG_OPTION_NUM]; /* for comparing w/ record S32_t's */
    S8_t    date_option;                /* to control date comparison */
    S8_t    s32_option[SYS_LOG_OPTION_NUM]; /* to control S32_t comparisons */
} SYS_FILTER_INFO_t;

/*
 * Sys log filter
 *   First element must be a SIW_LOG_FILE *
 */
typedef struct {
    SIW_LOG_FILE *      log_file;       /* stream assoc with log file */
    U32_t               record_count;   /* # of recs remaining to read */
    U32_t               record_size;    /* bytes per record */
    U64_t               latest_date;    /* most recent date to be read */
    SYS_FILTER_INFO_t   filter_info;    /* filter parameters */
} * SYS_LOG_FILTER_t;

extern int     sysStartType;

#ifndef BOOT
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
 *  Returns:     OK or ERROR
 **********************************************************************/
extern SIW_TID_t SysTaskSpawn(IN SIW_TID_t tblTid);

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
extern STATUS_t SysTaskDelete(WIND_TCB *tcbPtr);

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
 *               S32_t option - message queue option
 *  Out Parms:   None
 *  Returns:     A valid SIW_MQID_t or SIW_INVALID_MQID if failure
 **********************************************************************/
extern SIW_MQID_t SysMsgQCreate(IN SIW_MQID_t tblMqId, 
                                IN S32_t option);

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
extern SIW_SEMID_t SysSemCreate(IN SIW_SEMID_t tblSemId);

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
extern STATUS_t SysTaskWaitAck(IN SIW_TID_t tblTid);

/***********************************************************************
 *  Function:    SysSemReturn()
 *  Description: This function returns all semaphores that are taken
 *               by a deleted task.
 *
 *  In Parms:    SIW_TID_t tid - SIW Task Id
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 **********************************************************************/
extern STATUS_t SysSemReturn(IN SIW_TID_t tid);

/************************************************************************
 *  Function:    SysTimerDelete()
 *  Description: This function deletes the timer that was created
 *               by a specific task.
 *
 *  In Parms:    SIW_TID_t taskId - task ID
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *************************************************************************/
extern void SysTimerDelete(SIW_TID_t taskId);

/***************************************************************************
 *  Function:     SysTaskMonitor()
 *  Description:  This function creates all the message queues, spawn all
 *                the tasks, and monitors all the spawned task status
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 ****************************************************************************/
extern STATUS_t SysTaskMonitor(void);

/***************************************************************************
 *  Function:     CopyString()
 *  Description:  This function copies character string from source to
 *                destination data buffer.
 *
 *  In Parms:     char *desStr - pointer to the destination data buffer
 *                const char *srcStr - pointer to the source data buffer
 *                int desLen - size of the destination data buffer
 *                int srcLen - length of bytes in source data buffer
 *  Out Parms:    None
 *  Returns:      None
 ****************************************************************************/
extern void CopyString(IN char       *desStr,
                       IN const char *srcStr,
                       IN int        desLen,
                       IN int        srcLen,
                       IN char       filler);

/****************************************************************************
 *  Function:     ClrScrn()
 *  Description:  This function cleans up screen for displaying.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 *****************************************************************************/
extern void ClrScrn(void);

/*******************************************************************************
 *  Function:     CheckScroll()
 *  Description:  This function checks whether the screen needs to be
 *                scrolled ior not.
 *
 *  In Parms:     int limit - the limit line displaied before scroll screen
 *  Out Parms:    int *pLineNum - where to store the number of line has
 *                                been displaied
 *  returns:      QUIT or NEXT
 ******************************************************************************/
extern int CheckScroll(IN int *pLineNum, IN int limit);

/******************************************************************************
 *  Function:     SysNovramSet()
 *  Description:  This function gets data from Novram
 *
 *  In Parms::    int   cardType - card type
 *                U32_t offset - offset to the data location
 *                U32_t size - number of bytes to read
 *                U8_t  *dataBuf - data buffer
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 ******************************************************************************/
extern STATUS_t SysNovramSet(IN int   cardType, 
                             IN U32_t offset, 
                             IN U32_t size, 
                             IN U8_t  *dataBuf);

/********************************************************************
 *  FUNCTION:     NovramDia()
 *  Description:  This function provides a suite of test pattern for Novram One Byte by One Byte
 *
 *  In Parms:     int cardType - NOVRAM on which card
 *  Out Parms:    None
 *  Returns:      0 for OK or negative value for ERROR
 ********************************************************************/
extern RET_t NovramDia(IN int cardType);
extern U8_t  diaTraceN;

/************************************************************************
 *  Function:     SysGetDramSize()
 *  Description:  This function gets SDRAM size through I2C.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      SDRAM size in bytes.
 ************************************************************************/
extern U32_t SysGetDramSize(void);

/************************************************************************
 *  Function:     SysWait()
 *  Description:  This routine is using Galileo Timer2 to perform
 *                delay in usec.
 *  Note:         Maximum delay of 240msec.
 *
 *  In Parms:     U32_t usec15 - delay in 1.5 microsecond unit.
 *  Out Parms:    None
 *  Returns:      None
 ************************************************************************/
extern void SysWait(U32_t usec15);

/************************************************************
 *  Function:    SysSwitchOver()
 *  Description: This is SCC switch over handling routine.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 *************************************************************/
extern void SysSwitchOver(void);

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
extern STATUS_t 
SysEventLog(char *eventBufPtr, SIW_EVENT_OBJ_t *event, va_list args);

/****************************************************************************
 *  Function:     SysFilterEvent()
 *  Description:  This function filters out events based on date, slot,
 *                severity and trap type.
 *
 *  In Parms:     char *date - date of the event log occured
 *                int slotNo - slot to generate the event log
 *                int trapType - trap type
 *                int severity - event severity
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 ****************************************************************************/
extern STATUS_t SysFilterEvent(IN char *date, IN int slotNo, 
                               IN int trapType, IN int severity);

/****************************************************************************
 *  Function:     SysRetrieveEvent()
 *  Description:  This function retrieves an event log from sorted event
 *                record.
 *
 *  In Parms:     int index - index to the event log records
 *  Out Parms:    SNMP_EVENT_LOG_t *bufPtr - where to return event log
 *  Returns:      OK or ERROR
 ****************************************************************************/
extern STATUS_t SysRetrieveEvent(IN int index, OUT SNMP_EVENT_LOG_t *bufPtr);

/*******************************************************************************
 *  Function:    SysQueueEvent()
 *  Description: This function allows application tasks to pass event to
 *               Event Log task.
 *
 *  In Parms:    MSG_t *bufPtr
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *******************************************************************************/
extern STATUS_t SysQueueEvent(MSG_t *bufPtr);

/****************************************************************
 *  Function:    SysTimeShow()
 *  Description: This function display date/time stored in the
 *               vxWorks System.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 ****************************************************************/
extern void SysTimeShow();

/********************************************************************************
* STATUS_t SysRtcRead(int rtcEntry, int offset, char *dataBuf, int nBytes)
* function: 
*   to read data from RTC
* paramenter:
*  int rtcEntry - the entry to RTC static table
*  int Offset - the offset address
*  char *dataBuf - a data buffer to store the read-out
*  int len -  number of bytes to read
* return
*  OK or ERROR
********************************************************************************/
extern STATUS_t SysRtcRead(int rtcEntry, int offset, char *dataBuf, int nBytes);

/********************************************************************************
* STATUS_t SysRtcWrite(int rtcEntry, int offset, char *dataBuf, int nBytes)
* function: 
*   to read data from RTC
* paramenter:
*  int rtcEntry - the entry to RTC static table
*  int Offset - the offset address
*  char *dataBuf - a data buffer to store the read-out
*  int len -  number of bytes to read
* return
*  OK or ERROR
********************************************************************************/
extern STATUS_t SysRtcWrite(int rtcEntry, int offset, char *dataBuf, int nBytes);

/***************  TEMPORARY, need to be removed ******************/
extern STATUS_t SysDummy(void);
extern U64_t    SysHrTimeGet(void);

/******* This is a assembly language routine ***************
extern void     SimClkInit(void);
*/
#endif /* NOT BOOT */

extern STATUS_t sysClkInit();
extern void     SysDateTimeCallback(DOS_DATE_TIME *dosDateTime);

#endif  /* _SYS_IF_H_ */
