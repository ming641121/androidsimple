/*====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc.
 * Abstract:
 *    This file contains the routines of Alarm Management
 *
 * Note:
 *
 * Author(s):
 *
 */


#if 0
typedef struct buffer 
{
   U32_t               fe:1;
   U32_t               hold:1;
   U32_t               hi:1;
   U32_t               no:13;         /* length of the data in this buffer */
   U32_t               v_110:1;
   U32_t               rsv1:2;
   U32_t               no13:1;
   U32_t               csm:1;
   U32_t               fnum:11;
   struct buffer       *next;          /* next buffer in buffer chain */
   U8_t                *dataPtr;       /* 1st data byte address */
   U32_t               usedCnt:8;      /* buffer used count
                                        * the sarFrameBufRet put the 
                                        * buffer to the free pool if
                                        * usedCnt == 0
                                        * This can be used to simulate
                                        * the buffer copy or retransmission
                                        * After the buffer is transmitted by
                                        * the sarFrameBufTx() function,
                                        * the sarFrameBufRet() is called to 
                                        * return the buffer to the free pool
                                        * if the application needs to hold the
                                        * buffer for retransmission, it should 
                                        * call sarFrameBufCntUp() which
                                        * increment the usedCnt by 1
                                        */
   U32_t               reserved_1:3;
   U32_t               edmaFreeSel:3; 
   U32_t               edmaBuffFree:1;
   U32_t               edmaBuffLarge:1;
   U32_t               edmaEgrBfsNum:16;
   U32_t               frLength:16;     /* length of the frame 
                                         * equal to length of data 
                                         * if no buffer chainning
                                         */
   U32_t               reserved_2:1;  
   U32_t               frEfci:1;
   U32_t               frClp:1;
   U32_t               ConNum:13;
   struct buffer       *frBfdTailPtr;    /* point to end of frame buffer */
   MRPC_HDR_t          mrpcHdr;
   U8_t                data[MAX_BUFF_LEN];
   U32_t               sscopTrailer;
   U8_t                reserved1[MAX_PAD_TO_2032];
} BUFFER_t;

typedef BUFFER_t MSG_t;

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
		.


(BUFFER_t) MSG_t * = MSG_BUF_HDR_t.msgBufPtr

ANY DATA  = (&(MSG_t *)->data[0]);


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
    char   slotno[SLOT_NUMBER_SIZE];       /* Card slot number */
    char   blankpad2[SNMP_SPACES];         /* Blank */
    char   pslotno[SLOT_NUMBER_SIZE];      /* Card slot number */
    char   blankpad3[SNMP_SPACES];         /* Blank */
    char   severityName[SEVERITY_SIZE];    /* Name of Severity of event */
    char   blankpad4[SNMP_SPACES];         /* Blank */
    char   eventType[EVENT_TYPE_SIZE];     /* Type of event */
    char   blankpad5[SNMP_SPACES];         /* Blank */
    char   message[EVENT_MESSAGE_SIZE];    /* Event Message */
    char   blankpad6[SNMP_SPACES-1];       /* Blank */
    char   nullterm;                       /* Null terminator */
} SNMP_EVENT_LOG_t;

typedef struct {
    U8_t       msgType;       /* MsgType of ERR_MSG_LOG */
    U8_t       trapEvent;     /* Trap Event */
    U16_t      alarmId;       /* Alarm ID */ 
	S8_t       shelf;         /* Shelf */
	S8_t       slotNo;        /* Physical Slot Number */
	S8_t       portNo;        /* Physical Port Number */
	S8_t       severity;      /* Channel */
} EVENT_LOG_HDR_t;

SiwEventLog
   SysEventLog
      ErrMsgProcess
		 SiwAllocMem(SIW_EVENT, sizeof(MSG_t), WAIT_FOREVER);
		 SiwMsgQSend(MSGQ2LOG, &eventBuf, sizeof(MSG_BUF_HDR_t), 10, SIW_MSG_PRI_NORMAL)

SysEventTask
   SiwMsgQReceive(MSGQ2LOG, &evtBuf, sizeof(MSG_BUF_HDR_t), msgTimeout);
   SysErrMsgLog
	  EventTrap
	  EventDispatch

    char                  *eventBufPtr
    SIW_EVENT_OBJ_t       *event
	char                  *msg


    EVENT_LOG_ENTRY_t     *evtLog = NULL;
    EVENT_LOG_HDR_t       *evtLogHdr = NULL;
    MSG_t                 *msgBuf = NULL;
    MSG_BUF_HDR_t          eventBuf;

	msgBuf = (MSG_t *)SiwAllocMem(SIW_EVENT, sizeof(MSG_t), WAIT_FOREVER);

    eventBuf.msgBufPtr = msgBuf;

    evtLogHdr = (EVENT_LOG_HDR_t *)&msgBuf->data[0+bufOffset];

    evtLog = (EVENT_LOG_ENTRY_t *)&msgBuf->data[sizeof(EVENT_LOG_HDR_t)+bufOffset];

    ConvertEventMsg(event, msg, evtLog);

    SiwMsgQSend(MSGQ2LOG, &eventBuf, sizeof(MSG_BUF_HDR_t), 10, SIW_MSG_PRI_NORMAL)

typedef struct {
    char *  module;
    char *  eventname;
    char *  format;
    U8_t    severity;
    U8_t    action;
    U16_t   option;
    /* U32_t   eventId; */
    U32_t   ticks;
	S8_t    shelf;
    S8_t    slotNo;
	S8_t    portNo;
	S8_t    channel;
} SIW_EVENT_OBJ_t;
#endif	/* 0 */

#include <vxWorks.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys_os.h"
#include "am_if.h"
#include "trap_if.h"
#include "snmpdefs.h"
#include "led_if.h"
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
#include "axss_types.h"
#include "snmpsupp.h"
#include "sys_snmp.h"

#define   AM_TASK_PRI    10


/*
 *====================================================================
 *=                               global                             =
 *====================================================================
 */

BOOL_t       dbgAm = FALSE;
LIST         amCriticalList;
LIST         amMajorList;
LIST         amMinorList;
LIST         amNaList;

/*
 *====================================================================
 *=                               local                              =
 *====================================================================
 */

SIW_TID_t     amTaskId;
SIW_MQID_t    amMsgQId;

/*
 *====================================================================
 *=                               extern                             =
 *====================================================================
 */
extern SYS_CONFIG_T sysConfig;

/*
 *====================================================================
 *=                               routine                            =
 *====================================================================
 */
void amInit();
void amInitDB();
void amListInit();


/*========================================================================
 * Function:     amProcessQueue
 *
 * Description:  Process the alarm msgQ
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
void 
amProcessQueue (void)
{
    U16_t               alarmId;
    MSG_t             * msgPtr;
    MSG_BUF_HDR_t       siwMsg;
    EVENT_LOG_HDR_t   * evtLogHdr = NULL;
    char              * evtPtr;
	S8_t                severity;

    /* See if there is any Trap message in the queue */
	if (SiwMsgQReceive(MSGQ2AM, &siwMsg, sizeof(MSG_BUF_HDR_t), WAIT_FOREVER) > 0)
    {
        msgPtr = (MSG_t *) siwMsg.msgBufPtr;

        if (msgPtr)
        {
            /* Yes, there is message */
            evtPtr    = SIW_GET_EVENTTRAP_PTR(msgPtr);
            evtLogHdr = (EVENT_LOG_HDR_t *) &msgPtr->data[0];

            severity  = evtLogHdr->severity;
			alarmId   = evtLogHdr->alarmId;

            if (dbgAm)
			{
                printf("amProcessQueue: Alarm ID: %d, Severity: %d\n", evtLogHdr->alarmId, evtLogHdr->severity);
			}

            switch (alarmId)
			{
			    default:
				break;
			}

            switch (severity)
            {
                case TRAP_CRITICAL:
                    if (dbgAm)
                        printf("Receieved CRITICAL ALARM\r\n");

				    amListAdd(CRITICAL_LIST, msgPtr);

                    break;

                case TRAP_MAJOR:
                    if (dbgAm)
                        printf("Receieved MAJOR ALARM\r\n");

				    amListAdd(MAJOR_LIST, msgPtr);

                    break;

                case TRAP_MINOR:
                    if (dbgAm)
                        printf("Receieved MINOR ALARM\r\n");

				    amListAdd(MINOR_LIST, msgPtr);

                    break;

                case TRAP_NA:
                    if (dbgAm)
                        printf("Receieved NOT ALARM EVENT\r\n");

				    amListAdd(NA_LIST, msgPtr);

                    break;

                case TRAP_CLEAR:
                    if (dbgAm)
                        printf("Receieved CLEAR ALARM\r\n");
				    amListClear(evtLogHdr);
                    SiwFreeMem (msgPtr);
					break;

                default:
                    if (dbgAm)
                        printf("Receieved Severity Unknown\r\n");

                    SiwFreeMem (msgPtr);
                    break;
            }
        }
        sysIntegratedAlarm();

    }
}

/*========================================================================
 * Function:     amTask
 *
 * Description:  AM main task routine
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
STATUS_t
amTask (void)
{
    /* first thing that you do */
    amInit();

    /* Init SNMP saved data for persistance */
	amInitDB();

    /* Start processing */
    if (dbgAm)
        printf("AM init is complete\n");

    /* send ACK back to parent */
    SiwTaskInitAck(OK);

    sysIntegratedAlarm();
    /* main processing loop */
    while (TRUE) {
	   amProcessQueue();

    }
	/* should never happended */
    return 0; 
}


void amInit()
{
    amListInit();
}

void amInitDB()
{
}


void amListInit()
{
    lstInit (&amCriticalList);
    lstInit (&amMajorList);
    lstInit (&amMinorList);
    lstInit (&amNaList);

}

void amListAdd(int list, MSG_t * msgPtr)
{
    ALARM_NODE_T *	pAlarmNode = NULL;

    pAlarmNode              = SiwAllocMem(SIW_DYNAMIC, sizeof(ALARM_NODE_T), WAIT_FOREVER);
	pAlarmNode->msgPtr      = msgPtr;

    SiwSemTake(SEM_AM, WAIT_FOREVER);     
    switch(list)
	{
	    case CRITICAL_LIST:
		    lstAdd(&amCriticalList, (NODE *)pAlarmNode);
		break;

	    case MAJOR_LIST:
		    lstAdd(&amMajorList, (NODE *)pAlarmNode);
		break;

	    case MINOR_LIST:
		    lstAdd(&amMinorList, (NODE *)pAlarmNode);
		break;

	    case NA_LIST:
		    lstAdd(&amNaList, (NODE *)pAlarmNode);
		break;

		default:
		break;
    }
	SiwSemGive(SEM_AM);
}

void amListDel(int list, ALARM_NODE_T * pAlarmNode)
{
    SiwSemTake(SEM_AM, WAIT_FOREVER);     
    switch(list)
	{
	    case CRITICAL_LIST:
		    lstDelete(&amCriticalList, (NODE *)pAlarmNode);
		break;

	    case MAJOR_LIST:
		    lstDelete(&amMajorList, (NODE *)pAlarmNode);
		break;

	    case MINOR_LIST:
		    lstDelete(&amMinorList, (NODE *)pAlarmNode);
		break;

	    case NA_LIST:
		    lstDelete(&amNaList, (NODE *)pAlarmNode);
		break;

		default:
		break;
    }
	SiwSemGive(SEM_AM);

    SiwFreeMem (pAlarmNode->msgPtr);
	SiwFreeMem (pAlarmNode);
}

void amListClear(EVENT_LOG_HDR_t * pAlarmHdr)
{
    ALARM_NODE_T    * pAlarmNode = NULL;
	EVENT_LOG_HDR_t * evtLogHdr;

    SiwSemTake(SEM_AM, WAIT_FOREVER);     

    for(pAlarmNode = (ALARM_NODE_T *) lstFirst(&amCriticalList);
        pAlarmNode != NULL;
        pAlarmNode = (ALARM_NODE_T *) lstNext((NODE *) pAlarmNode))
	{   
       evtLogHdr = (EVENT_LOG_HDR_t *) &(pAlarmNode->msgPtr->data[0]);

       if (dbgAm)
	   {
	       printf("Stored Alarm   >>> ");
	       printf("alarmId:  0x%x, ",   evtLogHdr->alarmId);
	       printf("shelf:    0x%x, ",   evtLogHdr->shelf);
	       printf("slotNo:   0x%x, ",   evtLogHdr->slotNo);
	       printf("portNo:   0x%x, ",   evtLogHdr->portNo);
	       printf("severity: 0x%x\r\n", evtLogHdr->severity);

	       printf("Searched Alarm >>> ");
	       printf("alarmId:  0x%x, ",   pAlarmHdr->alarmId);
	       printf("shelf:    0x%x, ",   pAlarmHdr->shelf);
	       printf("slotNo:   0x%x, ",   pAlarmHdr->slotNo);
	       printf("portNo:   0x%x, ",   pAlarmHdr->portNo);
	       printf("severity: 0x%x\r\n", pAlarmHdr->severity);
	   }

	   if((evtLogHdr->alarmId == pAlarmHdr->alarmId) &&
	      (evtLogHdr->shelf   == pAlarmHdr->shelf)	 &&
		  (evtLogHdr->slotNo  == pAlarmHdr->slotNo)	 &&
		  (evtLogHdr->portNo  == pAlarmHdr->portNo))
	   {
	       /* Find the Node */
		   amListDel(CRITICAL_LIST, pAlarmNode);
	   }
	}


    for(pAlarmNode = (ALARM_NODE_T *) lstFirst(&amMajorList);
        pAlarmNode != NULL;
        pAlarmNode = (ALARM_NODE_T *) lstNext((NODE *) pAlarmNode))
	{
       evtLogHdr = (EVENT_LOG_HDR_t *) &(pAlarmNode->msgPtr->data[0]);

       if (dbgAm)
	   {
	       printf("Stored Alarm   >>> ");
	       printf("alarmId:  0x%x, ",   evtLogHdr->alarmId);
	       printf("shelf:    0x%x, ",   evtLogHdr->shelf);
	       printf("slotNo:   0x%x, ",   evtLogHdr->slotNo);
	       printf("portNo:   0x%x, ",   evtLogHdr->portNo);
	       printf("severity: 0x%x\r\n", evtLogHdr->severity);

	       printf("Searched Alarm >>> ");
	       printf("alarmId:  0x%x, ",   pAlarmHdr->alarmId);
	       printf("shelf:    0x%x, ",   pAlarmHdr->shelf);
	       printf("slotNo:   0x%x, ",   pAlarmHdr->slotNo);
	       printf("portNo:   0x%x, ",   pAlarmHdr->portNo);
	       printf("severity: 0x%x\r\n", pAlarmHdr->severity);
	   }

	   if((evtLogHdr->alarmId == pAlarmHdr->alarmId) &&
	      (evtLogHdr->shelf   == pAlarmHdr->shelf)	 &&
		  (evtLogHdr->slotNo  == pAlarmHdr->slotNo)	 &&
		  (evtLogHdr->portNo  == pAlarmHdr->portNo))
	   {
	       /* Find the Node */
		   amListDel(MAJOR_LIST, pAlarmNode);
	   }
	}

    for(pAlarmNode = (ALARM_NODE_T *) lstFirst(&amMinorList);
        pAlarmNode != NULL;
        pAlarmNode = (ALARM_NODE_T *) lstNext((NODE *) pAlarmNode))
	{
       evtLogHdr = (EVENT_LOG_HDR_t *) &(pAlarmNode->msgPtr->data[0]);

       if (dbgAm)
	   {
	       printf("Stored Alarm   >>> ");
	       printf("alarmId:  0x%x, ",   evtLogHdr->alarmId);
	       printf("shelf:    0x%x, ",   evtLogHdr->shelf);
	       printf("slotNo:   0x%x, ",   evtLogHdr->slotNo);
	       printf("portNo:   0x%x, ",   evtLogHdr->portNo);
	       printf("severity: 0x%x\r\n", evtLogHdr->severity);

	       printf("Searched Alarm >>> ");
	       printf("alarmId:  0x%x, ",   pAlarmHdr->alarmId);
	       printf("shelf:    0x%x, ",   pAlarmHdr->shelf);
	       printf("slotNo:   0x%x, ",   pAlarmHdr->slotNo);
	       printf("portNo:   0x%x, ",   pAlarmHdr->portNo);
	       printf("severity: 0x%x\r\n", pAlarmHdr->severity);
	   }

	   if((evtLogHdr->alarmId == pAlarmHdr->alarmId) &&
	      (evtLogHdr->shelf   == pAlarmHdr->shelf)	 &&
		  (evtLogHdr->slotNo  == pAlarmHdr->slotNo)	 &&
		  (evtLogHdr->portNo  == pAlarmHdr->portNo))
	   {
	       /* Find the Node */
		   amListDel(MINOR_LIST, pAlarmNode);
	   }
	}

    for(pAlarmNode = (ALARM_NODE_T *) lstFirst(&amNaList);
        pAlarmNode != NULL;
        pAlarmNode = (ALARM_NODE_T *) lstNext((NODE *) pAlarmNode))
	{
       evtLogHdr = (EVENT_LOG_HDR_t *) &(pAlarmNode->msgPtr->data[0]);

       if (dbgAm)
	   {
	       printf("Stored Alarm   >>> ");
	       printf("alarmId:  0x%x, ",   evtLogHdr->alarmId);
	       printf("shelf:    0x%x, ",   evtLogHdr->shelf);
	       printf("slotNo:   0x%x, ",   evtLogHdr->slotNo);
	       printf("portNo:   0x%x, ",   evtLogHdr->portNo);
	       printf("severity: 0x%x\r\n", evtLogHdr->severity);

	       printf("Searched Alarm >>> ");
	       printf("alarmId:  0x%x, ",   pAlarmHdr->alarmId);
	       printf("shelf:    0x%x, ",   pAlarmHdr->shelf);
	       printf("slotNo:   0x%x, ",   pAlarmHdr->slotNo);
	       printf("portNo:   0x%x, ",   pAlarmHdr->portNo);
	       printf("severity: 0x%x\r\n", pAlarmHdr->severity);
	   }

	   if((evtLogHdr->alarmId == pAlarmHdr->alarmId) &&
	      (evtLogHdr->shelf   == pAlarmHdr->shelf)	 &&
		  (evtLogHdr->slotNo  == pAlarmHdr->slotNo)	 &&
		  (evtLogHdr->portNo  == pAlarmHdr->portNo))
	   {
	       /* Find the Node */
		   amListDel(NA_LIST, pAlarmNode);
	   }
	}

	SiwSemGive(SEM_AM);

}

void amListShow()
{
    ALARM_NODE_T * pAlarmNode = NULL;
	EVENT_LOG_HDR_t * evtLogHdr;

    SiwSemTake(SEM_AM, WAIT_FOREVER);     

	printf("\r\n <<<<< CRITICAL ALARM LIST >>>>> \r\n");

    for(pAlarmNode = (ALARM_NODE_T *) lstFirst(&amCriticalList);
        pAlarmNode != NULL;
        pAlarmNode = (ALARM_NODE_T *) lstNext((NODE *) pAlarmNode))
	{
       evtLogHdr = (EVENT_LOG_HDR_t *) &(pAlarmNode->msgPtr->data[0]);

	   printf("alarmId: 0x%x,",     evtLogHdr->alarmId);
	   printf("shelf: 0x%x,",       evtLogHdr->shelf);
	   printf("slotNo: 0x%x,",      evtLogHdr->slotNo);
	   printf("portNo: 0x%x,",      evtLogHdr->portNo);
	   printf("severity: 0x%x\r\n", evtLogHdr->severity);

	}

	printf("\r\n <<<<< MAJOR ALARM LIST >>>>> \r\n");

    for(pAlarmNode = (ALARM_NODE_T *) lstFirst(&amMajorList);
        pAlarmNode != NULL;
        pAlarmNode = (ALARM_NODE_T *) lstNext((NODE *) pAlarmNode))
	{
       evtLogHdr = (EVENT_LOG_HDR_t *) &(pAlarmNode->msgPtr->data[0]);

	   printf("alarmId: 0x%x,",     evtLogHdr->alarmId);
	   printf("shelf: 0x%x,",       evtLogHdr->shelf);
	   printf("slotNo: 0x%x,",      evtLogHdr->slotNo);
	   printf("portNo: 0x%x,",      evtLogHdr->portNo);
	   printf("severity: 0x%x\r\n", evtLogHdr->severity);

	}

	printf("\r\n <<<<< MINOR ALARM LIST >>>>> \r\n");

    for(pAlarmNode = (ALARM_NODE_T *) lstFirst(&amMinorList);
        pAlarmNode != NULL;
        pAlarmNode = (ALARM_NODE_T *) lstNext((NODE *) pAlarmNode))
	{
       evtLogHdr = (EVENT_LOG_HDR_t *) &(pAlarmNode->msgPtr->data[0]);

	   printf("alarmId: 0x%x,",     evtLogHdr->alarmId);
	   printf("shelf: 0x%x,",       evtLogHdr->shelf);
	   printf("slotNo: 0x%x,",      evtLogHdr->slotNo);
	   printf("portNo: 0x%x,",      evtLogHdr->portNo);
	   printf("severity: 0x%x\r\n", evtLogHdr->severity);

	}

	printf("\r\n <<<<< NA ALARM LIST >>>>> \r\n");

    for(pAlarmNode = (ALARM_NODE_T *) lstFirst(&amNaList);
        pAlarmNode != NULL;
        pAlarmNode = (ALARM_NODE_T *) lstNext((NODE *) pAlarmNode))
	{
       evtLogHdr = (EVENT_LOG_HDR_t *) &(pAlarmNode->msgPtr->data[0]);

	   printf("alarmId: 0x%x,",     evtLogHdr->alarmId);
	   printf("shelf: 0x%x,",       evtLogHdr->shelf);
	   printf("slotNo: 0x%x,",      evtLogHdr->slotNo);
	   printf("portNo: 0x%x,",      evtLogHdr->portNo);
	   printf("severity: 0x%x\r\n", evtLogHdr->severity);

	}

	SiwSemGive(SEM_AM);

}

void sysIntegratedAlarm()
{
    int criAlarmCount;
    int majAlarmCount;
    int minAlarmCount;
    int naAlarmCount;

    SiwSemTake(SEM_AM, WAIT_FOREVER);     

    criAlarmCount = lstCount(&amCriticalList);
    majAlarmCount = lstCount(&amMajorList);
    minAlarmCount = lstCount(&amMinorList);
    naAlarmCount  = lstCount(&amNaList);

	SiwSemGive(SEM_AM);

    if(criAlarmCount != 0)
	{
	    if(dbgAm)
		    printf("Critical Count: %d\r\n", criAlarmCount);
	    /* Turn On Critical LED */
		ledOp(LED_CRITICAL_ALARM,  LED_CRITICAL_ALARM_ON, 0);
		sysConfig.stsSystemCriticalLED = D_stsSystemCriticalLED_on;
    }
	else
	{
		ledOp(LED_CRITICAL_ALARM,  LED_CRITICAL_ALARM_OFF, 0);	    
		sysConfig.stsSystemCriticalLED = D_stsSystemCriticalLED_off;
	}

    if(majAlarmCount != 0)
	{
	    if(dbgAm)
		    printf("Major    Count: %d\r\n", majAlarmCount);
	    /* Turn On Major LED */
		ledOp(LED_MAJOR_ALARM,  LED_MAJOR_ALARM_ON, 0);
		sysConfig.stsSystemMajorLED = D_stsSystemMajorLED_on;
    }
	else
	{
		ledOp(LED_MAJOR_ALARM,  LED_MAJOR_ALARM_OFF, 0);
		sysConfig.stsSystemMajorLED = D_stsSystemMajorLED_off;
	}

    if(minAlarmCount != 0)
	{
	    if(dbgAm)
		    printf("Minor    Count: %d\r\n", minAlarmCount);
	    /* Turn On Minor LED */
		ledOp(LED_MINOR_ALARM,  LED_MINOR_ALARM_ON, 0);
		sysConfig.stsSystemMinorLED = D_stsSystemMinorLED_on;
    }
	else
	{
		ledOp(LED_MINOR_ALARM,  LED_MINOR_ALARM_OFF, 0);
		sysConfig.stsSystemMinorLED = D_stsSystemMinorLED_off;
	}

    if(naAlarmCount != 0)
	{
	    if(dbgAm)
		    printf("Event    Count: %d\r\n", naAlarmCount);
	    /* Turn On Minor LED */
    }

    if(sysConfig.stsSystemCriticalLED == D_stsSystemCriticalLED_on)
	{
	    sysConfig.stsSystemIntegratedAlarm = D_stsSystemIntegratedAlarm_critical;
	}
	else
	if(sysConfig.stsSystemMajorLED == D_stsSystemMajorLED_on)
	{
	    sysConfig.stsSystemIntegratedAlarm = D_stsSystemIntegratedAlarm_major;
    }
	else
	if(sysConfig.stsSystemMinorLED == D_stsSystemMinorLED_on)
	{
	    sysConfig.stsSystemIntegratedAlarm = D_stsSystemIntegratedAlarm_minor;
	}
	else
	{
	    sysConfig.stsSystemIntegratedAlarm = D_stsSystemIntegratedAlarm_clear;
	}

}