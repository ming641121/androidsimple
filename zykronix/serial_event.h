/*
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: serial_event.h
 *
 *Define all Event objects for serial Module
 *
 *Note:
 *
 *Author(s):
 *  shuming.chang
 *  betta.lin
 *
 */

#ifndef _SERIAL_EVENT_H_
#define _SERIAL_EVENT_H_

#include <siw_events.h>


SIW_EVENT_DEF(SERIAL, GPSFAILURE, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "GPS in serial port [%d] is not functioning well due to %s");

SIW_EVENT_DEF(SERIAL, CLOCKFAIULE, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Clock in serial port [%d] is not fuctioning well due to %s");

SIW_EVENT_DEF(SERIAL, GPSOK, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "GPS in serial port [%d] is now functioning well");

SIW_EVENT_DEF(SERIAL, CLOCKOK, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Clock in serial port [%d] is now fuctioning well");

 
#endif  /* _SERIAL_EVENT_H_ */
