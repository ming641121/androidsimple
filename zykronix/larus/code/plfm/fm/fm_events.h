/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file is a local header file for File Manager. It includes all
 *    events for the File Manager.
 *
 * Author(s):
 *
 *   
 */


#ifndef _FM_EVENTS_H_
#define _FM_EVENTS_H_

#include <siw_events.h>
#include <siw_trace.h>

SIW_MODULE_DEF(FM, TRACE_LEVEL_1);

/* 
 * !!!!!WARNING!!!!! TRACE1 is really the lowest level and TRACE5 is the
 * highest level. Therefore the levels are reversed to accomodate it. In the
 * code, use TRACE5 to log severe cases and TRACE1 to log info only type of
 * cases.
 */
#define FM_TRACE1(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(FM, f, t, TRACE_LEVEL_5, (a1), (a2), (a3), (a4), (a5));
#define FM_TRACE2(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(FM, f, t, TRACE_LEVEL_4, (a1), (a2), (a3), (a4), (a5));
#define FM_TRACE3(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(FM, f, t, TRACE_LEVEL_3, (a1), (a2), (a3), (a4), (a5));
#define FM_TRACE4(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(FM, f, t, TRACE_LEVEL_2, (a1), (a2), (a3), (a4), (a5));
#define FM_TRACE5(f,t,a1,a2,a3,a4,a5) \
     SIW_DEBUG_TRACE(FM, f, t, TRACE_LEVEL_1, (a1), (a2), (a3), (a4), (a5));


SIW_EVENT_DEF(FM, ADD_PORT_FAILED, EVENT_MAJOR, EVENT_NO_OPTION, 
	      EVENT_NO_ACTION,
	      "initFm(): MRPC Add port failed ret(%d); for Mod %d; service %d");
SIW_EVENT_DEF(FM, INIT_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "FmMain(): Could not initialize FM ret = %d");
SIW_EVENT_DEF(FM, RCV_MSG_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "FmMain: error message queue receive %d");
SIW_EVENT_DEF(FM, RCV_MSG_NULL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "FM recevied a Message with NULL Buffer ptr %x");
SIW_EVENT_DEF(FM, INFO, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "FmMain(): Got Message(%x); srcSlot(%x)"); 
SIW_EVENT_DEF(FM, ILLEGAL_MSG, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmMain(): FM received an unknown Msg(%d); srcSlot (%x)");
SIW_EVENT_DEF(FM, FMWRITE_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmMain(): FmWrite failed ret(%d); fdp(%x), fileName offset(%d)");
SIW_EVENT_DEF(FM, MRPC_ASYNC_FAIL,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmMain(): MrpcAsync failed ret(%d); For Target card (%d)");
SIW_EVENT_DEF(FM, OUT_OF_FDPS,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmCreate(): Ran out of FDPs (%x)");
SIW_EVENT_DEF(FM, ILLEGAL_FILE,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmDelete(): Illegal File Type found in FDP (%x)");
SIW_EVENT_DEF(FM, SEM_ERROR,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmOpen(): Could not Take Semaphore (%x) for FDP (%x)");
SIW_EVENT_DEF(FM, WR_OVERRUN,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmWrite():Going to OverRun file nextWr(%x), start(%x), len(%x)");
SIW_EVENT_DEF(FM, WR_ILLEGAL_FT,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmWrite(): Illegal file Type FDP(%x), FileType(%x)");
SIW_EVENT_DEF(FM, NULL_ADDR,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FM(): Illegal Buffer Address (%x)");
SIW_EVENT_DEF(FM, FMCREATE_FAIL,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRemCreate(): FmCreate Failed ret(%d); arg1(%x), arg2(%x)");
SIW_EVENT_DEF(FM, FMDELETE_FAIL,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRemDelete(): FmDelete Failed ret(%d); arg1(%x)");
SIW_EVENT_DEF(FM, FMOPEN_FAIL,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRemOpen(): FmOpen Failed fdp(%x); arg1(%x), arg2(%x)");
SIW_EVENT_DEF(FM, FMCLOSE_FAIL,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRemClose(): FmClose Failed ret(%d); arg1(%x)");
SIW_EVENT_DEF(FM, FMREAD_FAIL,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRemRead(): FmRead Failed ret(%d); arg1(%x), arg4(%x)");
SIW_EVENT_DEF(FM, ILLEGAL_FDP,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRemWrite(): Illegal FDP (%x)");
SIW_EVENT_DEF(FM, FAIL_2Q_MSG,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRemWrite(): Failed to Q Msg SiwMsgQsend() ret error (%x)");

SIW_EVENT_DEF(FM, ADD_MICS_FILES_RAM_FAIL,EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "fmAddMiscFiles2ImageFileTb(): add file (%s)to RAM image file table Failed ");

SIW_EVENT_DEF(FM, ADD_MICS_FILES_HD_FAIL,EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "fmAddMiscFiles2ImageFileTb(): add file (%s)to HD image failed ");
SIW_EVENT_DEF(FM, START_REBUILD_MSG,EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRebuildImageFile(): start build (%x)");
SIW_EVENT_DEF(FM, FINISH_REBUILD_MSG,EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRebuildImageFile(): finish build (%x)");

SIW_EVENT_DEF(FM,FAIL_OPEN_HD_FILE,EVENT_MAJOR,EVENT_NO_OPTION, EVENT_NO_ACTION,
	      "FmOpen(): Can't open HD file %s; flags(%x); ret code(%d)");
SIW_EVENT_DEF(FM, FMRENAME_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
              "FmRemRename(): FmRename failed ret(%d); sFile (%x) dFile(%x)");

#define FM_LOG0(e)       SiwEventLog(NULL, SIW_EVENT_SYM(FM, e))
#define FM_LOG(e,v)      SiwEventLog(NULL, SIW_EVENT_SYM(FM, e), (v))
#define FM_LOG2(e,v1,v2) SiwEventLog(NULL, SIW_EVENT_SYM(FM, e), (v1), (v2))
#define FM_LOG3(e,v1,v2,v3) SiwEventLog(NULL, SIW_EVENT_SYM(FM, e), (v1), (v2), (v3))

#endif  /* _FM_EVENTS_H_ */
