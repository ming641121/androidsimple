/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: sys_events.h
 *
 *Define all Event objects for System Module
 *
 *Note:
 *
 *Author(s):
 * 
 */

#ifndef _SYS_EVENTS_H_
#define _SYS_EVENTS_H_

#include <siw_events.h>
#include <siw_trace.h>

SIW_MODULE_DEF(SYS, TRACE_LEVEL_1);

SIW_EVENT_DEF(SYS, SAREXCEP, EVENT_FATAL, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "%s");

SIW_EVENT_DEF(SYS, CTASKISDEAD, EVENT_FATAL, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Task[%s] is dead and cause Fatal event");

SIW_EVENT_DEF(SYS, PCIERR, EVENT_FATAL, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "%s");

SIW_EVENT_DEF(SYS, INITACKFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "No ACK response from %s");

SIW_EVENT_DEF(SYS, TASKSPAWNFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to spawn Task of %s");

SIW_EVENT_DEF(SYS, CPUISHOLD, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Task[%s] is hogging the CPU");

SIW_EVENT_DEF(SYS, BGNDTASKSUSPEND, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Background Task is suspened, and failed to restart");

SIW_EVENT_DEF(SYS, TASKISDEAD, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "The task of %s is totally dead");

SIW_EVENT_DEF(SYS, BADDISK, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "IDE disk has bad spot at Cyl: %d, Head: %d, Sec: %d, nSecs: %d");

SIW_EVENT_DEF(SYS, CLKSYNFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Network clock failed sync");

SIW_EVENT_DEF(SYS, FAILRECVTASK, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Failed to recover the task of %s");

SIW_EVENT_DEF(SYS, ISRNOTCALLABLE, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Do not allow ISR to call: %s");

SIW_EVENT_DEF(SYS, NVRSEMTAKEFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to receive Novram sem: %p, errno: %x");

SIW_EVENT_DEF(SYS, NVRAMCKSMFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "%s Novram has failure chksum: C:%x, S:%x");

SIW_EVENT_DEF(SYS, UNKNOWNCRDTYP, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Unknown Card Type: %d");

SIW_EVENT_DEF(SYS, EXCEPTION, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "Exc:%s");

SIW_EVENT_DEF(SYS, STARTUP, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "System is booted up by the reason of: %s");

SIW_EVENT_DEF(SYS, VERSION, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "FW image Rev: [%s]");

SIW_EVENT_DEF(SYS, BOOTFAIL, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Failed of %s");

SIW_EVENT_DEF(SYS, BGNDTASKRESTART, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Background Task is suspened, and restart is successful");

SIW_EVENT_DEF(SYS, RECVTASK, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Recovering the task of %s: %s");

SIW_EVENT_DEF(SYS, EVENTTEST, EVENT_INFO, EVENT_NO_OPTION | EVENT_TRAP_FLAG, EVENT_NO_ACTION,
              "This is a Event Log test sample: %s");

SIW_EVENT_DEF(SYS, RSTCBT, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "CUBIT %d has been reset");

SIW_EVENT_DEF(SYS, MEMSHORT, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "There is too little static memory left in the system");

SIW_EVENT_DEF(SYS, INVALIDCOMETID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid COMET device ID for BITS clock setup, ID: %d");

#endif  /* _SYS_EVENTS_H_ */
