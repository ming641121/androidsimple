/*
 *Copyright(C) 1998, All Rights Reserved, by
/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 * Ahoya Netowrks, Inc. San Jose, CA
 *
 * File Name: nwg_events.h
 * Abstract:Define all Event objects for Network Module
 *
 *
 * Note:
 *
 * Author(s):
 */
 /********************************************************************
 * $Id: nwg_events.h,v 1.2 2006/10/03 04:27:17 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/nwg/nwg_events.h,v $
 *====================================================================
 * $Log: nwg_events.h,v $
 * Revision 1.2  2006/10/03 04:27:17  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/
#ifndef _NWG_EVENTS_H_
#define _NWG_EVENTS_H_

#include <siw_events.h>
#include <siw_trace.h>

SIW_MODULE_DEF(NWG, TRACE_LEVEL_1);

SIW_EVENT_DEF(NWG, SCKBNDFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to open TFTP socket");

SIW_EVENT_DEF(NWG, DSCRINITFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to initialize TFTP descriptors");

SIW_EVENT_DEF(NWG, IOCTLRDFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Failed to use ioctl to read/flush socket");

SIW_EVENT_DEF(NWG, FOPENFAIL, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Failed to open file %s");

SIW_EVENT_DEF(NWG, FDWNLOAD, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Downloaded file: %s");

SIW_EVENT_DEF(NWG, CFGBKUP, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Back up Configuration to file: %s");

SIW_EVENT_DEF(NWG, CFGRST, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Restore Configuration from file: %s");

SIW_EVENT_DEF(NWG, UPFLS, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Update flash for slot %d with flash file %s");

SIW_EVENT_DEF(NWG, UPFLSFAIL, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION, 
     "Update flash failed slot %d with flash file %s");

#endif  /* _SYS_EVENTS_H_ */
