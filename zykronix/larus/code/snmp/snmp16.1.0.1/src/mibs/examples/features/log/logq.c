/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#ifndef SR_DEBUG
#define SR_DEBUG
#endif /* SR_DEBUG */

#include "sr_conf.h"

#include <stdio.h>
#include "sr_snmp.h"
#include "sr_proto.h"
#include "sr_type.h"
#include "diag.h"
#include "logq.h"

/* search type (i.e., get or get-next) */
#define NEXT       		GET_NEXT_REQUEST_TYPE
#define EXACT      		GET_REQUEST_TYPE

/*
 * QueueLogMessage()
 *
 * This is an example of a custom log handler function.  This function
 * stores the log message for processing later.  When the time for
 * processing the messages has arrived, SetLogFunction() should be called
 * to select a new log handler, and the function DumpLogMessageQueue()
 * should be called.
 *
 * This implementation of QueueLogMessage() uses a simple unidirectional
 * linked list.  A more efficient implementation should probably be used
 * if APTRACE messages are to be stored or for large values of logTableSize.
 *
 * This log handler interprets LogUserData as an integer value which
 * controls the maximum length of the queue (may be NULL/zero).
 */

static LogEntry *loghead = NULL;
static SR_INT32 logHeadSerialNum = 1;


/*
 * DequeueLogMessage()
 *
 * Remove the first log message from the queue.
 *
 */

LogEntry *
DequeueLogMessage()
{
    LogEntry *cur = NULL;

    cur = loghead;
    if (cur) {
	loghead = cur->UserData;
	logHeadSerialNum++;
	if (logHeadSerialNum <= 0) {   /* wrap? */
	    logHeadSerialNum = 1;
	}
    }
    return cur;
}


/*
 * QueueLogMessage()
 *
 * Add a new log message to the end of the queue.  If the queue
 * is too long, call DequeueLogMessage() to shorten the queue.
 *
 */

void
QueueLogMessage(log)
    LogEntry *log;
{
    LogEntry *cur = NULL;
    int       num = 0;
    int       logTableSize = 0;

    if (log) {
	logTableSize = (int) (long) log->UserData;
	if (logTableSize > 0) {
	    log->UserData = NULL;
	    if (loghead) {
		num = 1;  /* loghead is not NULL */
		for (cur = loghead; cur->UserData != NULL;
		     cur = (LogEntry *) cur->UserData) {
		    num++;
		}
		cur->UserData = (void *) CopyLogMessage(log);
		/*
		 * If CopyLogMessage() failed, the linked list was not
		 * changed; i.e., the last element already pointed to NULL.
		 * The num counter should only be incremented on success.
		 */
		if (cur->UserData) {
		    num++;
		}
		while (num > logTableSize) {
		    FreeLogMessage(DequeueLogMessage());
		    num--;
		}
	    }
	    else {
		loghead = CopyLogMessage(log);
	    }
	}
	else {
	    while (loghead != NULL) {
		FreeLogMessage(DequeueLogMessage());
	    }
	}
    }
}


/*
 * SearchLogMessageQueue()
 *
 * Find the specified message in the queue.
 * Return NULL if no message matches the search criteria.
 *
 */

LogEntry *
SearchLogMessageQueue(searchType, logIndex)
    int searchType;
    SR_INT32 *logIndex;
{
    LogEntry *cur = NULL;
    int       num = 0;

    cur = loghead;
    for (num = logHeadSerialNum; num < *logIndex; num++) {
	if (cur) {
	    cur = (LogEntry *) cur->UserData;
	}
	else {
	    break;
	}
    }
    if (cur == NULL) {
	return NULL;
    }
    if (searchType == EXACT) {
        if (num != *logIndex) {
            return NULL;
        }
    }
    *logIndex = num;
    return cur;
}


/*
 * SizeofLogMessageQueue()
 *
 * Count the number of log messages currently in the queue.
 *
 */

int
SizeofLogMessageQueue()
{
    LogEntry *cur = NULL;
    int       num = 0;

    for (cur=loghead; cur!=NULL; cur=(LogEntry *)cur->UserData) {
        num++;
    }
    return num;
}


/*
 * DumpLogMessageQueue()
 *
 * Send all of the log messages currently in the queue to the log
 * handler function which is currently registered in the global log
 * book.
 *
 * This function begins by counting the number of log messages in the
 * queue.  This ensures that the correct number of log messages are dumped,
 * even if the currently registered log handler is QueueLogMessage().
 *
 */

void
DumpLogMessageQueue()
{
    LogEntry *cur = NULL;
    int       num = 0;

    num = SizeofLogMessageQueue();
    while (num > 0) {
	cur = DequeueLogMessage();
	dsender(GetLogBook(), cur);
	FreeLogMessage(cur);
	num--;
    }
}


