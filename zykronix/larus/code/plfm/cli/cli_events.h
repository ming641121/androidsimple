/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file is a local header file for SCC BIM Manager (BIM). 
 *    It includes all events for the SCC BIM  Manager (BIM).
 *
 * Author(s):
 *
 *    Shinn Lin
 */


#ifndef _CLI_EVENTS_H_
#define _CLI_EVENTS_H_

#include <siw_events.h>
#include <siw_trace.h>

SIW_MODULE_DEF(CLI, TRACE_LEVEL_1);

SIW_EVENT_DEF(CLI, USER_COMMAND, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "%15s:  %s");


#endif  /* _CLI_EVENTS_H_ */
