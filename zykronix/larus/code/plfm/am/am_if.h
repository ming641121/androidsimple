/*====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc
 *
 * Abstract:
 *    This file contains the routines of Alarm Management
 *
 * Note:
 *
 * Author(s):
 *
 */

#ifndef _AM_IF_H_
#define _AM_IF_H_

/*
 *====================================================================
 *=                        define's                                 =
 *====================================================================
 */
#define CRITICAL_LIST    1
#define MAJOR_LIST       2
#define MINOR_LIST       3
#define NA_LIST          4

/*
 *====================================================================
 *=                        typedef's                                 =
 *====================================================================
 */
typedef struct alarm_node
{
    NODE              	  node;
	MSG_t               * msgPtr;
} ALARM_NODE_T;


/*
 *====================================================================
 *=                        Macro's                                   =
 *====================================================================
 */

/*
 *====================================================================
 *=                        Extern's                                  =
 *====================================================================
 */
extern void amListAdd(int list, MSG_t * msgPtr);
extern void amListDel(int list, ALARM_NODE_T * pAlarmNode);
extern void amListClear(EVENT_LOG_HDR_t * pAlarmHdr);
extern void sysIntegratedAlarm();


#endif /* _AM_IF_H_ */
