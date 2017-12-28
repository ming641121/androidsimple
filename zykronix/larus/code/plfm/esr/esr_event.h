#ifndef _ESR_EVENT_H_
#define _ESR_EVENT_H_

#include <siw_events.h>




SIW_EVENT_DEF(SERIAL, ESR_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "ESR[GPS/CDMA] in serial port [%d] is not functioning well now due to %s");
SIW_EVENT_DEF(SERIAL, ESR_OK, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "ESR[GPS/CDMA] in serial port [%d] is now functioning well.");

SIW_EVENT_DEF(SERIAL, ESR_STATUS_CHANGE, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "ESR[GPS/CDMA] status in serial port [%d] is %s now.");


/* SERIAL event log macros */
#define SERIAL_LOG0(e)		            SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e))
#define SERIAL_LOG1(e,v)              SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v))
#define SERIAL_LOG2(e,v1,v2)          SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v1),(v2))
#define SERIAL_LOG3(e,v1,v2,v3)       SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v1),(v2),(v3))
#define SERIAL_LOG4(e,v1,v2,v3,v4)    SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v1),(v2),(v3),(v4))
#define SERIAL_LOG5(e,v1,v2,v3,v4,v5) SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL,e),(v1),(v2),(v3),(v4),(v5))

 
#endif  /* _OSC_EVENT_H_ */
