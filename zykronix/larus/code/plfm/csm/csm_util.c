/* $Id: csm_util.c,v 1.9 2006/09/07 04:41:06 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_util.c,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of CSM Data Base
 *    Management for redundancy
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Log: csm_util.c,v $
 * Revision 1.9  2006/09/07 04:41:06  ffang
 * add phy up down status display
 *
 * Revision 1.8  2006/09/06 04:35:13  ffang
 * add physical link status for manual mode
 *
 * Revision 1.7  2006/08/30 04:43:18  ffang
 * add API for osc, imu links
 *
 * Revision 1.6  2006/08/03 04:27:28  ffang
 * unit test with FPGA
 *
 * Revision 1.5  2006/07/26 03:02:54  ffang
 * *** empty log message ***
 *
 * Revision 1.4  2006/07/25 02:39:21  ffang
 * 1st integration with framer
 *
 * Revision 1.3  2006/07/14 04:28:16  ffang
 * sync recheck-in
 *
 * Revision 1.2  2006/06/15 03:54:40  ffang
 * Check-in after integrated with siw/sys modules
 *
 * Revision 1.1  2006/05/16 04:09:49  ffang
 * *** empty log message ***
 *
 *====================================================================
 * $Endlog $
 */

#include "stdio.h"
#include <string.h>
#include "sys_os.h"
#include "csm_if.h"

static U32_t csmErrCnt[CSM_RET_MAX] = { 0L };
extern U16_t CsmGetFpgaReg(void);

static const char * csmInpName[] = 
{
	"NULL",
	"ESR1",
	"ESR2",
	"PRS ",
	"LNK1",
	"LNK2"
};

static const char * csmInpSts[] = 
{
	"NULL",
	"ACTV",
	"STNB",
	"DOWN",
	"RCOV"
};

static const char * csmInpOp[] = 
{
	"NULL",
	"PRTECT",
	"MANUAL"
};

static const char * csmInpVal[] = 
{
	"NUL",
	"PRS",
	"STU",
	"ST2",
	"ST3",
	"SIC",
	"DUC"
};

/*
 *====================================================================
 *=                     Utility routines                             =
 *====================================================================
 */

/*========================================================================
 * Function:     CsmPrintReg
 *
 * Description:  Routine to Print CSM registers
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
void
CsmPrintAllReg (void)
{
}

void
CsmPrintCb (void)
{
	U8_t i;

	printf("csmCb=0x%x,actvIndx=%d", (U32_t)&csmCb, csmCb.actvIndx);
	if (D_stsClockInputState_active != csmCb.inpSts[csmCb.actvIndx].stsClockInputState) {
		printf(" (not a active line)\n");
	} else {
		printf(" (current active line)\n");
	}

	printf("FPGA reg=0x%X\n", CsmGetFpgaReg());
	printf("ManualSelect=%s[%d],OperMode=%s[%d],SetSlip=%d,Value=%s[%d]\n",
		csmInpName[csmCb.sav.inp.stsClockInputManualSelect],
		csmCb.sav.inp.stsClockInputManualSelect, 
		csmInpOp[csmCb.sav.inp.stsClockInputOperMode],
		csmCb.sav.inp.stsClockInputOperMode,
		csmCb.sav.inp.stsClockInputSetSlip, 
		csmInpVal[csmCb.sav.inp.stsClockInputValue],
		csmCb.sav.inp.stsClockInputValue);

	for (i = 0; i < CSM_MAX_CLK_INPUT; i++) {
		printf("[Line %d %s],stratum=%s[%d],priority=%d,phy=%s,state=%s[%d]\n", 
			i, csmInpName[i+1], csmInpVal[csmCb.sav.inpClk[i].stratum],
			csmCb.sav.inpClk[i].stratum,
			csmCb.sav.inpClk[i].priority, 
			(csmCb.phySts[i] == CSM_PHY_LINK_UP) ? "UP" : "DN",
			csmInpSts[csmCb.inpSts[i].stsClockInputState],
			csmCb.inpSts[i].stsClockInputState);
	}

#if 0
	printf("[OSC 0],state=%s[%d]\n",csmInpSts[csmCb.oscSts[0]],csmCb.oscSts[0]);
	printf("[OSC 1],state=%s[%d]\n",csmInpSts[csmCb.oscSts[1]],csmCb.oscSts[1]);

	for (i = 0; i < D_stsLinkIndex_link5; i++) {
		printf("[LNK %d],state=%s[%d]\n",i,csmInpSts[csmCb.imuSts[i]],csmCb.imuSts[i]);
	}
#endif
}

/*========================================================================
 * Function:     CsmPrintStatus
 *
 * Description:  Routine to Print CSM status
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
void
CsmPrintStatus (void)
{
    CsmPrintAllReg();
    CsmPrintCb();
}

/*========================================================================
 * Function:     CsmLogErrEvent
 *
 * Description:  Routine to log the CSM error event
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
void        
CsmLogErrEvent (CSM_RET_e ev)
{
	csmErrCnt[ev]++;
}

void        
pendSysReady (void)
{
}

void        
pendDbmReady (void)
{
}

/*========================================================================
 * Function:     CsmEnterCriticalSection
 *
 * Description:  Routine to Print CSM status
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
void        
CsmEnterCriticalSection (void)
{
    /* Take the semaphore */
    SiwSemTake(csmSemId, WAIT_FOREVER);
}

/*========================================================================
 * Function:     CsmExitCriticalSection
 *
 * Description:  Routine to Print CSM status
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
void        
CsmExitCriticalSection (void)
{
    /* Give back the semaphore */
    SiwSemGive(csmSemId);
}

void
csmdbg()
{
	CsmPrintStatus();
}