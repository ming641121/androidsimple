/*
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: framer_event.h
 *
 *Define all Event objects for input framer
 *
 *Note:
 *
 *Author(s):
 *
 */

#ifndef _FRAMER_EVENT_H_
#define _FRAMER_EVENT_H_
 
#include <siw_events.h>
#include "siw_trace.h"

/*
 *====================================================================
 *=                          Module Defn			     =
 *====================================================================
 */

SIW_MODULE_DEF(FRAMER, TRACE_LEVEL_5);

/*
 *====================================================================
 *=                          Function Trace                   	     =
 *====================================================================
 */

#ifdef FRAMER_DEBUG_TRACE
#define FRAMER_FUNC_TRACE(func,a1,a2,a3,a4,a5)  SIW_FUNC_TRACE(FRAMER,func,a1,a2,a3,a4,a5)
#define FRAMER_FUNC_TRACE0(func)		         FRAMER_FUNC_TRACE(func,0,0,0,0,0)
#define FRAMER_FUNC_TRACE1(func,a1)             FRAMER_FUNC_TRACE(func,a1,0,0,0,0)
#define FRAMER_FUNC_TRACE2(func,a1,a2)	         FRAMER_FUNC_TRACE(func,a1,a2,0,0,0)
#define FRAMER_FUNC_TRACE3(func,a1,a2,a3)	     FRAMER_FUNC_TRACE(func,a1,a2,a3,0,0)
#define FRAMER_FUNC_TRACE4(func,a1,a2,a3,a4)    FRAMER_FUNC_TRACE(func,a1,a2,a3,a4,0)
#define FRAMER_FUNC_TRACE5(func,a1,a2,a3,a4,a5) FRAMER_FUNC_TRACE(func,a1,a2,a3,a4,a5)
#else /* !FRAMER_DEBUG_TRACE */
#define FRAMER_FUNC_TRACE(func,a1,a2,a3,a4,a5)  
#define FRAMER_FUNC_TRACE0(func)		         
#define FRAMER_FUNC_TRACE1(func,a1)             
#define FRAMER_FUNC_TRACE2(func,a1,a2)	         
#define FRAMER_FUNC_TRACE3(func,a1,a2,a3)	     
#define FRAMER_FUNC_TRACE4(func,a1,a2,a3,a4)    
#define FRAMER_FUNC_TRACE5(func,a1,a2,a3,a4,a5) 
#endif /* FRAMER_DEBUG_TRACE */

/*
 *====================================================================
 *=                           Debug Trace                     	     =
 *====================================================================
 */

#ifdef FRAMER_DEBUG_TRACE
#define FRAMER_TRACE1(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(FRAMER,f,t,TRACE_LEVEL_5,(a1),(a2),(a3),(a4),(a5));
#define FRAMER_TRACE2(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(FRAMER,f,t,TRACE_LEVEL_4,(a1),(a2),(a3),(a4),(a5));
#define FRAMER_TRACE3(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(FRAMER,f,t,TRACE_LEVEL_3,(a1),(a2),(a3),(a4),(a5));
#define FRAMER_TRACE4(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(FRAMER,f,t,TRACE_LEVEL_2,(a1),(a2),(a3),(a4),(a5));
#define FRAMER_TRACE5(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(FRAMER,f,t,TRACE_LEVEL_1,(a1),(a2),(a3),(a4),(a5));
#else /* !FRAMER_DEBUG_TRACE */
#define FRAMER_TRACE1(f,t,a1,a2,a3,a4,a5) 
#define FRAMER_TRACE2(f,t,a1,a2,a3,a4,a5) 
#define FRAMER_TRACE3(f,t,a1,a2,a3,a4,a5) 
#define FRAMER_TRACE4(f,t,a1,a2,a3,a4,a5) 
#define FRAMER_TRACE5(f,t,a1,a2,a3,a4,a5)
#endif /* FRAMER_DEBUG_TRACE */

/*
 *====================================================================
 *=                             Events                        	     =
 *====================================================================
 */
/*
typedef enum {
		TE1_ALARM_ACT=1,
		TE1_ALARM_LOS,
		TE1_ALARM_AISP, 
		TE1_ALARM_LOF,
		TE1_ALARM_CVL, 
		TE1_ALARM_CVP,
		TE1_ALARM_CS, 
		TE1_ALARM_JITTER,
		TE1_ALARM_TIE,
		TE1_ALARM_MTIE
}TE1_ALARM_t;

*/

SIW_EVENT_DEF(FRAMER, ACT, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] ACT alarm");
SIW_EVENT_DEF(FRAMER, ACT_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] ACT alarm clear");
     

SIW_EVENT_DEF(FRAMER, LOS, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] LOS alarm");
SIW_EVENT_DEF(FRAMER, LOS_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] LOS alarm clear");


SIW_EVENT_DEF(FRAMER, AISP, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] AISP alarm");
SIW_EVENT_DEF(FRAMER, AISP_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] AISP alarm clear");
     

SIW_EVENT_DEF(FRAMER, LOF, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] LOF alarm");
SIW_EVENT_DEF(FRAMER, LOF_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] LOF alarm clear");
     
     
SIW_EVENT_DEF(FRAMER, CVL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] CVL alarm");
SIW_EVENT_DEF(FRAMER, CVL_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] CVL alarm clear");


SIW_EVENT_DEF(FRAMER, CVP, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] CVP alarm");
SIW_EVENT_DEF(FRAMER, CVP_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] CVP alarm clear");


SIW_EVENT_DEF(FRAMER, CS, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] CS alarm");
SIW_EVENT_DEF(FRAMER, CS_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] CS alarm clear");


SIW_EVENT_DEF(FRAMER, JITTER, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] JITTER alarm");
SIW_EVENT_DEF(FRAMER, JITTER_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] JITTER alarm clear");


SIW_EVENT_DEF(FRAMER, TIE, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] TIE alarm");
SIW_EVENT_DEF(FRAMER, TIE_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] TIE alarm clear");


SIW_EVENT_DEF(FRAMER, MTIE, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] MTIE alarm");
SIW_EVENT_DEF(FRAMER, MTIE_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] MTIE alarm clear");
     

SIW_EVENT_DEF(FRAMER, RAIS, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] RAIS alarm");
SIW_EVENT_DEF(FRAMER, RAIS_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] RAIS alarm clear");


SIW_EVENT_DEF(FRAMER, RYEL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] RYEL alarm");
SIW_EVENT_DEF(FRAMER, RYEL_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] RYEL alarm clear");


SIW_EVENT_DEF(FRAMER, RED, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] RED alarm");
SIW_EVENT_DEF(FRAMER, RED_CLEAR, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] RED alarm clear");

    
SIW_EVENT_DEF(FRAMER, LINKUP, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] link up");
SIW_EVENT_DEF(FRAMER, LINKDOWN, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input framer [%d] link down");

SIW_EVENT_DEF(PRS, LINKUP, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input PRS link up");
SIW_EVENT_DEF(PRS, LINKDOWN, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "input PRS link down");


/* FRAMER event log macros */
#define FRAMER_LOG0(e)		           SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER,e))
#define FRAMER_LOG1(e,v)              SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER,e),(v))
#define FRAMER_LOG2(e,v1,v2)          SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER,e),(v1),(v2))
#define FRAMER_LOG3(e,v1,v2,v3)       SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER,e),(v1),(v2),(v3))
#define FRAMER_LOG4(e,v1,v2,v3,v4)    SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER,e),(v1),(v2),(v3),(v4))
#define FRAMER_LOG5(e,v1,v2,v3,v4,v5) SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER,e),(v1),(v2),(v3),(v4),(v5))




#endif  /* _FRAMER_EVENT_H_ */
