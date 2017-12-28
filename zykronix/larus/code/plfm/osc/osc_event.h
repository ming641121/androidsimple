#ifndef _OSC_EVENT_H_
#define _OSC_EVENT_H_

#include <siw_events.h>



SIW_EVENT_DEF(SERIAL, OSC_CURRENT_STATUS, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Oscillator-%d's current status : %s");    

SIW_EVENT_DEF(SERIAL, OSC_FORMER_STATUS, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Oscillator-%d's former status : %s"); 

SIW_EVENT_DEF(SERIAL, OSC_RED_STATUS_CHANGE, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Oscillator-%d's redundancy status : %s");      


/* SERIAL event log macros */
#define SERIAL_LOG0(e)		            SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e))
#define SERIAL_LOG1(e,v)              SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v))
#define SERIAL_LOG2(e,v1,v2)          SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v1),(v2))
#define SERIAL_LOG3(e,v1,v2,v3)       SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v1),(v2),(v3))
#define SERIAL_LOG4(e,v1,v2,v3,v4)    SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v1),(v2),(v3),(v4))
#define SERIAL_LOG5(e,v1,v2,v3,v4,v5) SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v1),(v2),(v3),(v4),(v5))

 
#endif  /* _OSC_EVENT_H_ */
