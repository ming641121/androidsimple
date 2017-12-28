/* $Id: csm_events.h,v 1.3 2006/07/14 04:33:58 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_events.h,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of CSM Data Base
 *    Management for redundancy
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Log: csm_events.h,v $
 * Revision 1.3  2006/07/14 04:33:58  ffang
 * sync recheck-in
 *
 *
 *====================================================================
 * $Endlog $
 */

#ifndef _CSM_EVENTS_H_
#define _CSM_EVENTS_H_

#include "siw_events.h"
#include "siw_trace.h"

/*
 *====================================================================
 *=                          Module Defn			     =
 *====================================================================
 */

SIW_MODULE_DEF(CSM, TRACE_LEVEL_5);

/*
 *====================================================================
 *=                          Function Trace                   	     =
 *====================================================================
 */

#ifdef CSM_DEBUG_TRACE
#define CSM_FUNC_TRACE(func,a1,a2,a3,a4,a5)  SIW_FUNC_TRACE(CSM,func,a1,a2,a3,a4,a5)
#define CSM_FUNC_TRACE0(func)		         CSM_FUNC_TRACE(func,0,0,0,0,0)
#define CSM_FUNC_TRACE1(func,a1)             CSM_FUNC_TRACE(func,a1,0,0,0,0)
#define CSM_FUNC_TRACE2(func,a1,a2)	         CSM_FUNC_TRACE(func,a1,a2,0,0,0)
#define CSM_FUNC_TRACE3(func,a1,a2,a3)	     CSM_FUNC_TRACE(func,a1,a2,a3,0,0)
#define CSM_FUNC_TRACE4(func,a1,a2,a3,a4)    CSM_FUNC_TRACE(func,a1,a2,a3,a4,0)
#define CSM_FUNC_TRACE5(func,a1,a2,a3,a4,a5) CSM_FUNC_TRACE(func,a1,a2,a3,a4,a5)
#else /* !CSM_DEBUG_TRACE */
#define CSM_FUNC_TRACE(func,a1,a2,a3,a4,a5)  
#define CSM_FUNC_TRACE0(func)		         
#define CSM_FUNC_TRACE1(func,a1)             
#define CSM_FUNC_TRACE2(func,a1,a2)	         
#define CSM_FUNC_TRACE3(func,a1,a2,a3)	     
#define CSM_FUNC_TRACE4(func,a1,a2,a3,a4)    
#define CSM_FUNC_TRACE5(func,a1,a2,a3,a4,a5) 
#endif /* CSM_DEBUG_TRACE */

/*
 *====================================================================
 *=                           Debug Trace                     	     =
 *====================================================================
 */

#ifdef CSM_DEBUG_TRACE
#define CSM_TRACE1(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(CSM,f,t,TRACE_LEVEL_5,(a1),(a2),(a3),(a4),(a5));
#define CSM_TRACE2(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(CSM,f,t,TRACE_LEVEL_4,(a1),(a2),(a3),(a4),(a5));
#define CSM_TRACE3(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(CSM,f,t,TRACE_LEVEL_3,(a1),(a2),(a3),(a4),(a5));
#define CSM_TRACE4(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(CSM,f,t,TRACE_LEVEL_2,(a1),(a2),(a3),(a4),(a5));
#define CSM_TRACE5(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(CSM,f,t,TRACE_LEVEL_1,(a1),(a2),(a3),(a4),(a5));
#else /* !CSM_DEBUG_TRACE */
#define CSM_TRACE1(f,t,a1,a2,a3,a4,a5) 
#define CSM_TRACE2(f,t,a1,a2,a3,a4,a5) 
#define CSM_TRACE3(f,t,a1,a2,a3,a4,a5) 
#define CSM_TRACE4(f,t,a1,a2,a3,a4,a5) 
#define CSM_TRACE5(f,t,a1,a2,a3,a4,a5)
#endif /* CSM_DEBUG_TRACE */

/*
 *====================================================================
 *=                             Events                        	     =
 *====================================================================
 */

SIW_EVENT_DEF(CSM, CLK_SWITCH, EVENT_MINOR, 
    EVENT_NO_OPTION, EVENT_NO_ACTION, "CSM %s");

SIW_EVENT_DEF(CSM, CLK_MAJOR_ALRM, EVENT_MINOR, 
    EVENT_NO_OPTION, EVENT_NO_ACTION, "CSM %s");

SIW_EVENT_DEF(CSM, CLK_MINOR_ALRM, EVENT_MINOR, 
    EVENT_NO_OPTION, EVENT_NO_ACTION, "CSM %s");

SIW_EVENT_DEF(CSM, CLK_ALRM_CLEAR, EVENT_MINOR, 
    EVENT_NO_OPTION, EVENT_NO_ACTION, "CSM %s");

/* EVENT_INFO, EVENT_MINOR, EVENT_MAJOR, EVENT_FATAL */
SIW_EVENT_DEF(CSM, INIT_FAILED, EVENT_MINOR, EVENT_NO_OPTION,
    EVENT_NO_ACTION, "CSM init failed ");

SIW_EVENT_DEF(CSM, DBM_CB_FAILED, EVENT_MINOR, EVENT_NO_OPTION,
    EVENT_NO_ACTION, "CSM dbm callback failed ");

SIW_EVENT_DEF(CSM, HW_CHECK_FAILED, EVENT_MAJOR, EVENT_NO_OPTION,
    EVENT_NO_ACTION, "CSM hw check failed ");

SIW_EVENT_DEF(CSM, HW_SELECT_FAILED, EVENT_MAJOR, EVENT_NO_OPTION,
    EVENT_NO_ACTION, "CSM hw select failed ");

SIW_EVENT_DEF(CSM, U_CONFIG_FAILED, EVENT_MINOR, EVENT_NO_OPTION,
    EVENT_NO_ACTION, "CSM user config failed ");

SIW_EVENT_DEF(CSM, U_SELECT_FAILED, EVENT_MINOR, EVENT_NO_OPTION,
    EVENT_NO_ACTION, "CSM user select failed ");

/* CSM event log macros */
#define CSM_LOG0(e)		           SiwEventLog(NULL, SIW_EVENT_SYM(CSM,e))
#define CSM_LOG1(e,v)              SiwEventLog(NULL, SIW_EVENT_SYM(CSM,e),(v))
#define CSM_LOG2(e,v1,v2)          SiwEventLog(NULL, SIW_EVENT_SYM(CSM,e),(v1),(v2))
#define CSM_LOG3(e,v1,v2,v3)       SiwEventLog(NULL, SIW_EVENT_SYM(CSM,e),(v1),(v2),(v3))
#define CSM_LOG4(e,v1,v2,v3,v4)    SiwEventLog(NULL, SIW_EVENT_SYM(CSM,e),(v1),(v2),(v3),(v4))
#define CSM_LOG5(e,v1,v2,v3,v4,v5) SiwEventLog(NULL, SIW_EVENT_SYM(CSM,e),(v1),(v2),(v3),(v4),(v5))

#endif /* _CSM_EVENTS_H_ */
