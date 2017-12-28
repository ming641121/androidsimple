/*
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: tiu_event.h
 *
 *Define all Event objects for TIU
 *
 *Note:
 *
 *Author(s):
 *
 */

#ifndef _TIU_EVENT_H_
#define _TIU_EVENT_H_
 
#include <siw_events.h>
#include "siw_trace.h"




/*
 *====================================================================
 *=                          Module Defn			     =
 *====================================================================
 */

SIW_MODULE_DEF(TIU, TRACE_LEVEL_5);

/*
 *====================================================================
 *=                          Function Trace                   	     =
 *====================================================================
 */

#ifdef 	TIU_DEBUG_TRACE
#define TIU_FUNC_TRACE(func,a1,a2,a3,a4,a5)  SIW_FUNC_TRACE(TIU,func,a1,a2,a3,a4,a5)
#define TIU_FUNC_TRACE0(func)		         TIU_FUNC_TRACE(func,0,0,0,0,0)
#define TIU_FUNC_TRACE1(func,a1)             TIU_FUNC_TRACE(func,a1,0,0,0,0)
#define TIU_FUNC_TRACE2(func,a1,a2)	         TIU_FUNC_TRACE(func,a1,a2,0,0,0)
#define TIU_FUNC_TRACE3(func,a1,a2,a3)	     TIU_FUNC_TRACE(func,a1,a2,a3,0,0)
#define TIU_FUNC_TRACE4(func,a1,a2,a3,a4)    TIU_FUNC_TRACE(func,a1,a2,a3,a4,0)
#define TIU_FUNC_TRACE5(func,a1,a2,a3,a4,a5) TIU_FUNC_TRACE(func,a1,a2,a3,a4,a5)
#else /* !TIU_DEBUG_TRACE */
#define TIU_FUNC_TRACE(func,a1,a2,a3,a4,a5)  
#define TIU_FUNC_TRACE0(func)		         
#define TIU_FUNC_TRACE1(func,a1)             
#define TIU_FUNC_TRACE2(func,a1,a2)	         
#define TIU_FUNC_TRACE3(func,a1,a2,a3)	     
#define TIU_FUNC_TRACE4(func,a1,a2,a3,a4)    
#define TIU_FUNC_TRACE5(func,a1,a2,a3,a4,a5) 
#endif /* TIU_DEBUG_TRACE */

/*
 *====================================================================
 *=                           Debug Trace                     	     =
 *====================================================================
 */

#ifdef TIU_DEBUG_TRACE
#define TIU_TRACE1(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(TIU,f,t,TRACE_LEVEL_5,(a1),(a2),(a3),(a4),(a5));
#define TIU_TRACE2(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(TIU,f,t,TRACE_LEVEL_4,(a1),(a2),(a3),(a4),(a5));
#define TIU_TRACE3(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(TIU,f,t,TRACE_LEVEL_3,(a1),(a2),(a3),(a4),(a5));
#define TIU_TRACE4(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(TIU,f,t,TRACE_LEVEL_2,(a1),(a2),(a3),(a4),(a5));
#define TIU_TRACE5(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(TIU,f,t,TRACE_LEVEL_1,(a1),(a2),(a3),(a4),(a5));
#else /* !TIU_DEBUG_TRACE */
#define TIU_TRACE1(f,t,a1,a2,a3,a4,a5) 
#define TIU_TRACE2(f,t,a1,a2,a3,a4,a5) 
#define TIU_TRACE3(f,t,a1,a2,a3,a4,a5) 
#define TIU_TRACE4(f,t,a1,a2,a3,a4,a5) 
#define TIU_TRACE5(f,t,a1,a2,a3,a4,a5)
#endif /* TIU_DEBUG_TRACE */

/*
 *====================================================================
 *=                             Events                        	     =
 *====================================================================
 */

     
SIW_EVENT_DEF(TIU, LOS, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  LOS alarm");
SIW_EVENT_DEF(TIU, LOS_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  LOS alarm clear");
     

SIW_EVENT_DEF(TIU, LOF, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  LOF alarm");
SIW_EVENT_DEF(TIU, LOF_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  LOF alarm clear");
     

SIW_EVENT_DEF(TIU, RAIS, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  RAIS alarm");
SIW_EVENT_DEF(TIU, RAIS_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  RAIS alarm clear");


SIW_EVENT_DEF(TIU, RED, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  RED alarm");
SIW_EVENT_DEF(TIU, RED_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  RED alarm clear");


SIW_EVENT_DEF(TIU, RYEL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  RED alarm");
SIW_EVENT_DEF(TIU, RYEL_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "TIU framer in shel/slot/port [%d/%d/%d]  RED alarm clear");



/* TIU event log macros */
#define TIU_LOG0(e)		           SiwEventLog(NULL, SIW_EVENT_SYM(TIU,e))
#define TIU_LOG1(e,v)              SiwEventLog(NULL, SIW_EVENT_SYM(TIU,e),(v))
#define TIU_LOG2(e,v1,v2)          SiwEventLog(NULL, SIW_EVENT_SYM(TIU,e),(v1),(v2))
#define TIU_LOG3(e,v1,v2,v3)       SiwEventLog(NULL, SIW_EVENT_SYM(TIU,e),(v1),(v2),(v3))
#define TIU_LOG4(e,v1,v2,v3,v4)    SiwEventLog(NULL, SIW_EVENT_SYM(TIU,e),(v1),(v2),(v3),(v4))
#define TIU_LOG5(e,v1,v2,v3,v4,v5) SiwEventLog(NULL, SIW_EVENT_SYM(TIU,e),(v1),(v2),(v3),(v4),(v5))




#endif  /* _TIU_EVENT_H_ */
