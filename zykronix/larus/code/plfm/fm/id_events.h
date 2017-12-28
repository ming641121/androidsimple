/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file is a local header file for Image downloader. It includes all
 *    events for the Image Downloader.
 *
 * Author(s):
 *
 *    
 */


#ifndef _ID_EVENTS_H_
#define _ID_EVENTS_H_

#include <siw_events.h>
#include <siw_trace.h>

SIW_MODULE_DEF(ID, TRACE_LEVEL_1);

/* 
 * !!!!!WARNING!!!!! TRACE1 is really the lowest level and TRACE5 is the
 * highest level. Therefore the levels are reversed to accomodate it. In the
 * code, use TRACE5 to log severe cases and TRACE1 to log info only type of
 * cases.
 */
#define ID_TRACE1(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(ID, f, t, TRACE_LEVEL_5, (a1), (a2), (a3), (a4), (a5));
#define ID_TRACE2(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(ID, f, t, TRACE_LEVEL_4, (a1), (a2), (a3), (a4), (a5));
#define ID_TRACE3(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(ID, f, t, TRACE_LEVEL_3, (a1), (a2), (a3), (a4), (a5));
#define ID_TRACE4(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(ID, f, t, TRACE_LEVEL_2, (a1), (a2), (a3), (a4), (a5));
#define ID_TRACE5(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(ID, f, t, TRACE_LEVEL_1, (a1), (a2), (a3), (a4), (a5));


SIW_EVENT_DEF(ID, INIT_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "IdMain(): Could not initialize ID ret = %d");
SIW_EVENT_DEF(ID, RCV_MSG_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "IdMain: error message queue receive %d");
SIW_EVENT_DEF(ID, RCV_MSG_NULL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "ID recevied a Message with NULL Buffer ptr %x");
SIW_EVENT_DEF(ID, INFO, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "IdMain(): Got Message(%x); srcSlot(%x)"); 
SIW_EVENT_DEF(ID, ILLEGAL_MSG, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "IdMain(): ID received an unknown Msg(%d); srcSlot (%x)");

#define ID_LOG0(e)       SiwEventLog(NULL, SIW_EVENT_SYM(ID, e))
#define ID_LOG(e,v)      SiwEventLog(NULL, SIW_EVENT_SYM(ID, e), (v))
#define ID_LOG2(e,v1,v2) SiwEventLog(NULL, SIW_EVENT_SYM(ID, e), (v1), (v2))
#define ID_LOG3(e,v1,v2,v3) SiwEventLog(NULL, SIW_EVENT_SYM(ID, e), (v1), (v2), (v3))

#endif  /* _ID_EVENTS_H_ */
