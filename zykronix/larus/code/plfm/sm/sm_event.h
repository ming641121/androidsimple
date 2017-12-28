/*
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: sm_event.h
 *
 *Define all Event objects for SM Module
 *
 *Note:
 *
 *Author(s):
 *
 */

#ifndef _SM_EVENT_H_
#define _SM_EVENT_H_
 
#include "siw_events.h"
#include "siw_trace.h"

/*
 *====================================================================
 *=                          Module Defn			     =
 *====================================================================
 */

SIW_MODULE_DEF(SM, TRACE_LEVEL_5);

/*
 *====================================================================
 *=                          Function Trace                   	     =
 *====================================================================
 */

#ifdef  SM_DEBUG_TRACE
		#define SM_FUNC_TRACE(func,a1,a2,a3,a4,a5)  SIW_FUNC_TRACE(SM,func,a1,a2,a3,a4,a5)
		#define SM_FUNC_TRACE0(func)		         SM_FUNC_TRACE(func,0,0,0,0,0)
		#define SM_FUNC_TRACE1(func,a1)             SM_FUNC_TRACE(func,a1,0,0,0,0)
		#define SM_FUNC_TRACE2(func,a1,a2)	         SM_FUNC_TRACE(func,a1,a2,0,0,0)
		#define SM_FUNC_TRACE3(func,a1,a2,a3)	     SM_FUNC_TRACE(func,a1,a2,a3,0,0)
		#define SM_FUNC_TRACE4(func,a1,a2,a3,a4)    SM_FUNC_TRACE(func,a1,a2,a3,a4,0)
		#define SM_FUNC_TRACE5(func,a1,a2,a3,a4,a5) SM_FUNC_TRACE(func,a1,a2,a3,a4,a5)
#else /* !SM_DEBUG_TRACE */
		#define SM_FUNC_TRACE(func,a1,a2,a3,a4,a5)  
		#define SM_FUNC_TRACE0(func)		         
		#define SM_FUNC_TRACE1(func,a1)             
		#define SM_FUNC_TRACE2(func,a1,a2)	         
		#define SM_FUNC_TRACE3(func,a1,a2,a3)	     
		#define SM_FUNC_TRACE4(func,a1,a2,a3,a4)    
		#define SM_FUNC_TRACE5(func,a1,a2,a3,a4,a5) 
#endif /* SM_DEBUG_TRACE */

/*
 *====================================================================
 *=                           Debug Trace                     	     =
 *====================================================================
 */

#ifdef  SM_DEBUG_TRACE
#define SM_TRACE1(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(SM,f,t,TRACE_LEVEL_5,(a1),(a2),(a3),(a4),(a5));
#define SM_TRACE2(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(SM,f,t,TRACE_LEVEL_4,(a1),(a2),(a3),(a4),(a5));
#define SM_TRACE3(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(SM,f,t,TRACE_LEVEL_3,(a1),(a2),(a3),(a4),(a5));
#define SM_TRACE4(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(SM,f,t,TRACE_LEVEL_2,(a1),(a2),(a3),(a4),(a5));
#define SM_TRACE5(f,t,a1,a2,a3,a4,a5) SIW_DEBUG_TRACE(SM,f,t,TRACE_LEVEL_1,(a1),(a2),(a3),(a4),(a5));
#else /* !SM_DEBUG_TRACE */
#define SM_TRACE1(f,t,a1,a2,a3,a4,a5) 
#define SM_TRACE2(f,t,a1,a2,a3,a4,a5) 
#define SM_TRACE3(f,t,a1,a2,a3,a4,a5) 
#define SM_TRACE4(f,t,a1,a2,a3,a4,a5) 
#define SM_TRACE5(f,t,a1,a2,a3,a4,a5)
#endif /* SM_DEBUG_TRACE */


/* SM event log macros */
#define SM_LOG0(e)		           SiwEventLog(NULL, SIW_EVENT_SYM(SM,e))
#define SM_LOG1(e,v)              SiwEventLog(NULL, SIW_EVENT_SYM(SM,e),(v))
#define SM_LOG2(e,v1,v2)          SiwEventLog(NULL, SIW_EVENT_SYM(SM,e),(v1),(v2))
#define SM_LOG3(e,v1,v2,v3)       SiwEventLog(NULL, SIW_EVENT_SYM(SM,e),(v1),(v2),(v3))
#define SM_LOG4(e,v1,v2,v3,v4)    SiwEventLog(NULL, SIW_EVENT_SYM(SM,e),(v1),(v2),(v3),(v4))
#define SM_LOG5(e,v1,v2,v3,v4,v5) SiwEventLog(NULL, SIW_EVENT_SYM(SM,e),(v1),(v2),(v3),(v4),(v5))


/*
 *====================================================================
 *=                             Events                        	     =
 *====================================================================
 */
SIW_EVENT_DEF(SM, ADDSHELF, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "A new shelf [%d] is added to the system");

SIW_EVENT_DEF(SM, REMOVESHELF, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Shelf [%d] is removed from the system");


SIW_EVENT_DEF(SM, MISMATCH, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Output card [%d] is mismatch on shelf [%d]");

SIW_EVENT_DEF(SM, ADDCARD, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "A new output card on slot [%d] in shelf [%d] is added to the system");

SIW_EVENT_DEF(SM, REMOVECARD, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "An output card on slot [%d] in shelf [%d] is removed from the system");

SIW_EVENT_DEF(SM, OUTPALARM, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "An output alarm occured on port [%d], slot [%d] in %s");

SIW_EVENT_DEF(SM, ABSLIPON, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "A/B Slip ON");

SIW_EVENT_DEF(SM, ABSLIPOFF, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "A/B Slip OFF");









#endif  /* _SM_EVENT_H_ */
