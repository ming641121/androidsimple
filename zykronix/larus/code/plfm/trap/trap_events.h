/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 * Abstract:
 *
 *    This file is a local header file for Trap Manager. 
 *      including all events for Trap  Manager.
 *
 *
 *Note:
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: trap_events.h,v 1.2 2006/10/03 04:33:30 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/trap/trap_events.h,v $
 *====================================================================
 * $Log: trap_events.h,v $
 * Revision 1.2  2006/10/03 04:33:30  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/

#ifndef _TRAP_EVENTS_H_
#define _TRAP_EVENTS_H_

#include <siw_events.h>
#include <siw_trace.h>

SIW_MODULE_DEF(TRM, TRACE_LEVEL_1);

SIW_EVENT_DEF(TRM, DB_FAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "TrapRamDbInit(): Could not create ram db ret = %d");

SIW_EVENT_DEF(TRM, DBTBL_FAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "TrapRamDbInit(): Could not create ram db table ret = %d");

SIW_EVENT_DEF(TRM, TBLENTRY_FAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "TrapRamDbInit(): Could not create ram table entry ret = %d");

SIW_EVENT_DEF(TRM, TBLCMT_FAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "TrapLogAdd(): Could not commit ram table entry = %u ret = %d");

SIW_EVENT_DEF(TRM, RCV_MSG_FAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "TrapMgr(): SiwMsgQReceive ret = %d");

SIW_EVENT_DEF(TRM, MSG_TYPE_FAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "TrapMgr(): SiwMsgQReceive msg type = %d");

SIW_EVENT_DEF(TRM, OBJ_INIT_FAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "TrapObjTblInit(): Object not found on table %d entry %d");

SIW_EVENT_DEF(TRM, SOCK_INIT_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "TrapObjTblInit(): Could not open the socket for trap");

SIW_EVENT_DEF(TRM, TRAP_NUM_FAIL, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "MakeTrapVarBind(): Trap Number %d not exist");

SIW_EVENT_DEF(TRM, TRAP_PDU_FAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "MakeTrapVarBind(): No memory for trap pdu");

SIW_EVENT_DEF(TRM, TRAP_SEND_FAIL, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "SendSnmpTrap(): Send trap to socket %u ip %x port %u fail");

SIW_EVENT_DEF(TRM, TRAP_BUILD_FAIL, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "SendSnmpTrap(): Build Snmp trap message failed");

#endif  /* _TRM_EVENTS_H_ */
