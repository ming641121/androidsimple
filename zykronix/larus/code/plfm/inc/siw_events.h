/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_events.h
 *
 *Public API of System Interface Wrapper to report
 *error events.
 *
 *Note:
 *
 *Author(s):
 * 
 */

#ifndef _SIW_EVENTS_H_
#define _SIW_EVENTS_H_

#include <msgQLib.h>
#include <tss_types.h>
#include <tss_if.h>
#include <tss_events.h>
#include <siw_if.h>

/***********************************************************************
 *  Event Severity Defines
 **********************************************************************/
#define EVENT_FATAL             ERROR_FATAL        /* platform needs reset */
#define EVENT_MAJOR             ERROR_MAJOR        /* Major alert condition */
#define EVENT_MINOR             ERROR_MINOR        /* Minor alert condition */
#define EVENT_INFO              ERROR_INFO         /* informational */

/***********************************************************************
 *  Defines for Event Type
 **********************************************************************/
#define NORMAL_EVENT            0        /* normal event */
#define ALARM_EVENT             1        /* alarm event */
#define TRAP_EVENT              2        /* trap event */
#define ALARM_TRAP_EVENT        3        /* alarm & trap event */

/***********************************************************************
 *  Event Option Defines - Used to control additional logging
 **********************************************************************/
#define EVENT_NO_OPTION         0x00     /* No special log action */
#define EVENT_STKTRC            0x01     /* Log stack trace with event */
#define COMET_MAJOR_ALARM       0x02
#define COMET_MINOR_ALARM       0x03

#define EVENT_TRAP_FLAG         (1 << 15) /* Trap Flag */
#define EVENT_TRAP_MASK         ~(EVENT_TRAP_FLAG) /* Trap Mask */

/***********************************************************************
 *  Event Action Defines
 **********************************************************************/
#define EVENT_NO_ACTION         0x00     /* No action */
#define EVENT_TASK_SUSPEND      0x01     /* Suspend Task */
#define EVENTLOG_CLOSE          0x00
#define EVENTLOG_OPEN           0x01

/***********************************************************************
 *  Structure Objects for Events
 **********************************************************************/
typedef struct {
    char *  module;
    char *  eventname;
    char *  format;
    U8_t    severity;
    U8_t    action;
    U16_t   option;
    /* U32_t   eventId; */
    U32_t   ticks;
	S8_t    trapType;
	S8_t    shelf;
    S8_t    slotNo;
	S8_t    portNo;
} SIW_EVENT_OBJ_t;

/***********************************************************************
 *  Macro Definitions for Events
 **********************************************************************/

/***********************************************************************
 *  Macro:       SIW_EVENT_SYM()
 *  Description: This macro generates the symbolic name for an event
 *               object. It is called to get a reference to an event object.
 *
 *  In Parms:    module - module identifier, uppercase
 *               eventname - event name identifier, uppercase
 **********************************************************************/
#define SIW_EVENT_SYM(module, eventname)   &(Event_ ## module ## _ ## eventname)

#define SIW_EVENT_SLOTNO_SET(module, eventname, s)   \
{                                            \
  SIW_EVENT_OBJ_t *__tmp;                    \
                                             \
   __tmp = SIW_EVENT_SYM(module, eventname); \
   __tmp->slotNo = s;                        \
}


#ifdef DEFINE_EVENTS

/***********************************************************************
 *  Macro:       SIW_EVENT_DEF()
 *  Description: This macro defines an event object.
 *
 *  In Parms:    module - module identifier, uppercase
 *               eventname - event name identifier, uppercase
 *               severity - severity of event
 *               option - option of logging control
 *               action - action for event
 *               format - event message in printf string format
 **********************************************************************/
#define SIW_EVENT_DEF(module, eventname, severity, option, action, format) \
        SIW_EVENT_OBJ_t Event_ ## module ## _ ## eventname = \
           { #module, #eventname, format, severity, action, option, \
             /* _ ## module ## _ ## eventname, */ 0, 0, 0, 0, 0}

#else

#define SIW_EVENT_DEF(module, eventname, severity, option, action, format)  \
extern SIW_EVENT_OBJ_t Event_ ## module ## _ ## eventname;
 
#endif  /* DEFINE_EVENTS */

/***********************************************************************
 *  General Define of using in Event Log Entry 
 **********************************************************************/
#define EVENT_MESSAGE_SIZE      79 
#define EVENT_NAME_SIZE         15
#define SEVERITY_SIZE           5
#define FW_REV_SIZE             20
#define MODULE_NAME_SIZE        4
#define NODE_NAME_SIZE          8
#define SLOT_NUMBER_SIZE        2
#define TASK_NAME_SIZE          10
#define DATESTAMP_SIZE          10
#define TIMESTAMP_SIZE          8
#define SNMP_SPACES             1
#define EVENT_TYPE_SIZE         4

typedef struct {
    char   datestamp[DATESTAMP_SIZE];      /* Date-Time of event */
    char   blankpad0;                      /* Blank */
    char   timestamp[TIMESTAMP_SIZE];      /* Date-Time of event */
    char   blankpad1[SLOT_NUMBER_SIZE];    /* Blank */
    char   slotno[SLOT_NUMBER_SIZE];       /* Card slot number */
    char   blankpad2[SLOT_NUMBER_SIZE];    /* Blank */
    char   pslotno[SLOT_NUMBER_SIZE];      /* Card slot number */
    char   blankpad3;                      /* Blank */
    char   taskname[TASK_NAME_SIZE];       /* Task name - blank padded */
    char   blankpad4;                      /* Blank */
    char   module[MODULE_NAME_SIZE];       /* Module Identifier - right just */
    char   dashpad1;                       /* Dash */
    char   eventname[EVENT_NAME_SIZE];     /* Event name - blank padded */
    char   dashpad2;                       /* Dash */
    char   severityName[SEVERITY_SIZE];    /* Name of Severity of event */
    char   nullterm1;                      /* Null terminator */
    char   message[EVENT_MESSAGE_SIZE];    /* Event Message */
    char   nullterm2;                      /* Null terminator */
    char   severity;                       /* Severity of event */
    char   eventType;                      /* Event Type */
    char   reserved;
    U32_t  eventId;                        /* event Id */
    int    slotNo;                         /* slot No */
} EVENT_LOG_ENTRY_t;

typedef struct {
    char   date[DATESTAMP_SIZE];           /* Date-Time of event */
    char   blankpad0[SNMP_SPACES];         /* Blank */
    char   time[TIMESTAMP_SIZE];           /* Date-Time of event */
    char   blankpad1[SNMP_SPACES];         /* Blank */
    char   shelf[SLOT_NUMBER_SIZE];        /* shelf */
    char   blankpad2[SNMP_SPACES];         /* Blank */
    char   slotno[SLOT_NUMBER_SIZE];       /* slot number */
    char   blankpad3[SNMP_SPACES];         /* Blank */
    char   portno[SLOT_NUMBER_SIZE];       /* port number */   
    char   blankpad4[SNMP_SPACES];         /* Blank */
	char   severityName[SEVERITY_SIZE];    /* Name of Severity of event */
    char   blankpad5[SNMP_SPACES];         /* Blank */
	char   eventType[EVENT_TYPE_SIZE];     /* Name of Severity of event */
    char   blankpad6[SNMP_SPACES];         /* Blank */
    char   message[EVENT_MESSAGE_SIZE];    /* Event Message */
    char   nullterm;                       /* Null terminator */
} SNMP_EVENT_LOG_t;

typedef struct {
    U8_t       msgType;       /* MsgType of ERR_MSG_LOG */
    U8_t       trapEvent;     /* Trap Event */
    U16_t      alarmId;       /* Alarm ID */ 
	S8_t       trapType;      /* Trap Type */
	S8_t       shelf;         /* Shelf */
	S8_t       slotNo;        /* Physical Slot Number */
	S8_t       portNo;        /* Physical Port Number */
	S8_t       severity;      /* Severity */
	S8_t       lineNo;
	S8_t       spare1;
	S8_t       spare2;
} EVENT_LOG_HDR_t;
    
/***********************************************************************
 *  Define of Event Log saved in Novram.
 **********************************************************************/
typedef struct {
    int               okFlag;     /* flag indicating the validation */
    EVENT_LOG_ENTRY_t nvEvtLog;   /* event log */
} NOVRAM_EVENTLOG_t;

/***********************************************************************
 *  External API Functions
 **********************************************************************/
/***********************************************************************
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
 **********************************************************************/
extern STATUS_t SiwEventLog(IN char *eventBufPtr, 
                            IN SIW_EVENT_OBJ_t *  event, ...);

/***********************************************************************
 *  Defines for ALARM registration
 ***********************************************************************/
#define NUM_ALARM_TABLE_ENTRIES     20
#define EVENT_TEST                  10

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
 ******************************************************************************/
extern STATUS_t EventTrapRegister(IN int alarmId, IN SIW_MQID_t msgQId, IN BOOL_t standby);

/*******************************************************************************
 *  Function:    EventTrapDeregister()
 *  Description: This function disconnects to receive the specific event trap.
 *
 *  In Parms:    int alarmId - alarm ID
 *               SIW_MQID_t msgQId - message queue ID for reveiving the event
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 *******************************************************************************/
extern STATUS_t EventTrapDeregister(IN int alarmId, IN SIW_MQID_t msgQId);

/*************************************************************
 *  Marco:    SIW_GET_EVENTLOG_PTR()
 *  Description: This marco returns the pointer pointing to
 *               the Event Log data in the Event buffer.
 *
 *  In Parms:    MSG_t *msgPtr - pointer to the Event buffer
 *  Out Parms:   None
 *  Returns:     pointer to the EVENT LOG
 **************************************************************/
#define SIW_GET_EVENTLOG_PTR(msgPtr)   \
            (EVENT_LOG_ENTRY_t *)&msgPtr->data[sizeof(EVENT_LOG_HDR_t)]

/*************************************************************
 *  Marco:    SIW_GET_EVENTTRAP_PTR()
 *  Description: This marco returns the pointer pointing to
 *               the Event Log data in the Event buffer.
 *
 *  In Parms:    MSG_t *msgPtr - pointer to the Event buffer
 *  Out Parms:   None
 *  Returns:     pointer to the EVENT TRAP
 **************************************************************/
#define SIW_GET_EVENTTRAP_PTR(msgPtr)   \
    (char *)&msgPtr->data[sizeof(EVENT_LOG_ENTRY_t) + sizeof(EVENT_LOG_HDR_t)]

/*************************************************************
 *  Marco:    SIW_GET_EVENT_SEVERITY()
 *  Description: This marco returns the severity of the 
 *               the Event.
 *
 *  In Parms:    MSG_t *msgPtr - pointer to the Event buffer
 *  Out Parms:   None
 *  Returns:     pointer to the EVENT LOG
 **************************************************************/
#define SIW_GET_EVENT_SEVERITY(msgPtr)   \
         ((EVENT_LOG_ENTRY_t *)(&msgPtr->data[sizeof(EVENT_LOG_HDR_t)]))->severity

/*************************************************************
 *  Marco:    SIW_GET_ALARM_ID()
 *  Description: This marco returns the severity of the 
 *               the Event.
 *
 *  In Parms:    MSG_t *msgPtr - pointer to the Event buffer
 *  Out Parms:   None
 *  Returns:     U8_t Alarm ID
 **************************************************************/
#define SIW_GET_ALARM_ID(msgPtr)        \
              (U16_t)((EVENT_LOG_HDR_t *)(&msgPtr->data[0]))->alarmId
               
/*************************************************************
 *  Marco:    SIW_GET_SEVERITY()
 *  Description: This marco returns the severity of the 
 *               the Event.
 *
 *  In Parms:    MSG_t *msgPtr - pointer to the Event buffer
 *  Out Parms:   None
 *  Returns:     U8_t Alarm ID
 **************************************************************/
#define SIW_GET_SEVERITY(msgPtr)        \
              (S8_t)((EVENT_LOG_HDR_t *)(&msgPtr->data[0]))->severity

/*************************************************************
 *  Marco:    SIW_GET_SHELF()
 *  Description: This marco returns the shelf index of the 
 *               the Event.
 *
 *  In Parms:    MSG_t *msgPtr - pointer to the Event buffer
 *  Out Parms:   None
 *  Returns:     U8_t Alarm ID
 **************************************************************/
#define SIW_GET_SHELF(msgPtr)        \
              (S8_t)((EVENT_LOG_HDR_t *)(&msgPtr->data[0]))->shelf

/*************************************************************
 *  Marco:    SIW_GET_SLOT_NO()
 *  Description: This marco returns the slot number of the 
 *               the Event.
 *
 *  In Parms:    MSG_t *msgPtr - pointer to the Event buffer
 *  Out Parms:   None
 *  Returns:     U8_t Alarm ID
 **************************************************************/
#define SIW_GET_SLOT_NO(msgPtr)        \
              ((EVENT_LOG_HDR_t *)(&msgPtr->data[0]))->slotNo

/*************************************************************
 *  Marco:    SIW_GET_PORT_NO()
 *  Description: This marco returns the port number of the 
 *               the Event.
 *
 *  In Parms:    MSG_t *msgPtr - pointer to the Event buffer
 *  Out Parms:   None
 *  Returns:     U8_t Alarm ID
 **************************************************************/
#define SIW_GET_PORT_NO(msgPtr)        \
              ((EVENT_LOG_HDR_t *)(&msgPtr->data[0]))->portNo
	   

#endif  /* _SIW_EVENTS_H_ */
