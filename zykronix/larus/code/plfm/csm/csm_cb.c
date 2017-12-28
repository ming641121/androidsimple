/* $Id: csm_cb.c,v 1.11 2006/09/07 04:41:06 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_cb.c,v $
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
 * $Log: csm_cb.c,v $
 * Revision 1.11  2006/09/07 04:41:06  ffang
 * add phy up down status display
 *
 * Revision 1.10  2006/09/06 04:35:13  ffang
 * add physical link status for manual mode
 *
 * Revision 1.9  2006/08/15 02:55:56  ffang
 * add code for crystal/rb handling
 *
 * Revision 1.8  2006/08/03 04:27:28  ffang
 * unit test with FPGA
 *
 * Revision 1.7  2006/08/03 02:56:56  ffang
 * add unit test routines for dbm and events
 *
 * Revision 1.6  2006/07/28 04:43:56  ffang
 * add FPGA setting
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

#include <stdio.h>

/* other modules includes */
#include "sys_os.h"
#include "ds26502.h"

/* CSM module includes */
#include "csm_if.h"
#include "csm_hw.h"
#include "csm_events.h"

/*========================================================================
 * Function:     CsmRead8
 *
 * Description:  Routine to read 8 bits data
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      8 bits data read
 */
U8_t
CsmRead8 (VU32_t * addr)
{
	U8_t data = *(U8_t *)addr;
	return data;
}

/*========================================================================
 * Function:     CsmSetFpgaMuxReg
 *
 * Description:  Routine to write 8 bits data
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      void
 */
void
CsmWrite8 (VU32_t * addr, U8_t data)
{
	*(U8_t *)addr = data;
/*#ifdef CSM_WRITE_VERIFY*/
	if (data != *(U8_t *)addr) {
		CsmLogErrEvent(CSM_WRITE_8_FAILURE);		
		CSM_TRACE1(CsmWrite8,0,addr,data,0,0,0);
	}
/*#endif*/ /* CSM_WRITE_VERIFY	*/
}

/*========================================================================
 * Function:     CsmGetFpgaReg
 *
 * Description:  Routine to config the FPGA MUX register setting
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      return of CSM_RET_e is defined in csm_if.h
 */
U16_t
CsmGetFpgaReg (void)
{
    
    /*CSM_FUNC_TRACE1(CsmGetFpgaReg, val); */

    return (CSM_FPGA_BASE_0);
}

/*========================================================================
 * Function:     CsmSetFpgaReg
 *
 * Description:  Routine to set the FPGA MUX register
 *
 * In Parms:     none
 * Outarms:    none
 *
 * Returns:      return of CSM_RET_e is defined in csm_if.h
 *
 */
static CSM_RET_e
CsmSetFpgaReg (U16_t val)
{
    CSM_RET_e   ret = CSM_RET_OK;
	CSM_FUNC_TRACE1(CsmSetFpgaReg, val);
	printf("%s:setting 0x%x to FPGA\n", __FUNCTION__, val);
	
	CSM_FPGA_BASE_0 = val;

    return (ret);
}

/*========================================================================
 * Function:     CsmSwitchFpgaClkSrc
 *
 * Description:  Routine to switch the clock source by programming FPGA
 *
 * In Parms:     none
 * Outarms:    none
 *
 * Returns:      return of CSM_RET_e is defined in csm_if.h
 *
 * Note: the definition of FPGA:
 * FPGA BASE ADDRESS + 0000h Write = 1 PPS/CLK Select 
 * Selects which 1 PPS goes to the oscillator modules, 5/10 MHz Input and Clock Source 
 * D7 = NC
 * D6 = NC
 * D5 = Clock Out Select ?0 = OSC; 1=DDFS 
 * D4 = Clock In Select - 0 = CLKA; 1 = CLKB, this is rb
 * D3 = 5/10 MHz Input Select 0=10MHz, 1=5MHz 
 * D2 = 1 PPS to Oscillators        100 = 5/10 MHz
 * D1 =                             010 = Input CLK A, 011 = Input CLK B, 
 * D0 =                             000 = GPS A, 001 = GPS B, 
 */
static CSM_RET_e
CsmSwitchFpgaClkSrc (U16_t oldIndx, U16_t newIndx)
{
	U8_t	fpgaReg = 0x00;

	printf("%s oldIndx=%d, newIndx=%d\n", __FUNCTION__,oldIndx,newIndx); 
	/* Read out the current FPGA register */
	fpgaReg = CsmGetFpgaReg();

	/* First clear the last 3 bits */
	fpgaReg &= 0xF8;

	/* program the FPGA register, note:
	 * index 0 - GPS A 
	 * index 1 - GPS B 
	 * index 2 - 5/10 MHz 
	 * index 3 - Input CLK A 
	 * index 4 - Input CLK B 
	 */
	switch (newIndx) {
		case 0:
			/* GPS A, last 3 bits is 000, do nothing */
			break;
		case 1:
			/* GPS B, last 3 bits is 001 */
			CSM_BIT_SET(fpgaReg, CSM_BIT_0);
			break;			
		case 2:
			/* 5/10 MHz, last 3 bits is 100 */
			CSM_BIT_SET(fpgaReg, CSM_BIT_2);
			break;
		case 3:
			/* Input CLK A, last 3 bits is 010 */
			CSM_BIT_SET(fpgaReg, CSM_BIT_1);
			break;
		case 4:
			/* Input CLK B, last 3 bits is 011 */
			CSM_BIT_SET(fpgaReg, CSM_BIT_0);
			CSM_BIT_SET(fpgaReg, CSM_BIT_1);
			break;
		default:
			/* unknown index */
			printf("ERR:unknown index=%d\n", newIndx);
			return CSM_INVALID_INDEX;
	}
	
	/* Got the best clock input, switch to it by programming FPGA */
	return CsmSetFpgaReg(fpgaReg);
}

static U8_t
CsmGetBestStandby(void)
{
	U8_t i;
	U8_t bestIndx = CSM_INVALID_LINE;
	U8_t bestPriority = 0xFF;		  /* start from the lowest */

	for (i = 0; i < CSM_MAX_CLK_INPUT; i++) {
		/* only the line in standby state will be checked */
		if ((D_stsClockInputState_standby == csmCb.inpSts[i].stsClockInputState) &&
			(csmCb.sav.inpClk[i].priority < bestPriority)) {
			bestIndx = i;
			bestPriority = csmCb.sav.inpClk[i].priority;
		}
	}
	return bestIndx;		
}

void 
CsmClockInputDown(U8_t indx)
{
	U8_t	bestIndx;

	printf("%s indx=%d\n", __FUNCTION__,indx); 

	CsmEnterCriticalSection();

	csmCb.phySts[indx] = CSM_PHY_LINK_DOWN;
	
	/* note that in manual mode, even the link down is received on active line
	 * we will do nothing because user want the active line to be "active"
	 */	
	if ((D_stsClockInputOperMode_manual == csmCb.sav.inp.stsClockInputOperMode)	&&
		(csmCb.actvIndx == indx)) {
		printf("%s on active line (%d manual mode), do nothing\n",__FUNCTION__,indx);
	 	return;
	}

	/* Mark the line as down */
	csmCb.inpSts[indx].stsClockInputState = D_stsClockInputState_down;
		
	if (csmCb.actvIndx == indx) {
		/* Active clock input is now down, need to switch to other input */		
		if (((bestIndx = CsmGetBestStandby()) !=  CSM_INVALID_LINE) &&
			(bestIndx != indx)) {
			/* Got the best clock input, switch to it by programming FPGA */
			if (CsmSwitchFpgaClkSrc(indx, bestIndx) ==  CSM_RET_OK) {
				csmCb.actvIndx = bestIndx;
				csmCb.inpSts[bestIndx].stsClockInputState = D_stsClockInputState_active;
			} else {
				printf("%s CsmSwitchFpgaClkSrc error\n", __FUNCTION__);
			}
		} else {
			/* There is no clock input can be used, alarm!!! */
			printf("WARNING: no input clock source available\n");
		} 
	}

	CsmExitCriticalSection();
}

void 
CsmClockInputUp(U8_t indx)
{	
	printf("%s indx=%d\n", __FUNCTION__,indx); 

	CsmEnterCriticalSection();

	csmCb.phySts[indx] = CSM_PHY_LINK_UP;

	if (D_stsClockInputOperMode_manual == csmCb.sav.inp.stsClockInputOperMode) { 
		if (csmCb.actvIndx != indx) {
			/* manual, active indx should have been at active state, it's possible
			 * that the line is "active" and then link up is received
			 */
			csmCb.inpSts[indx].stsClockInputState = D_stsClockInputState_standby;
		}
	} else if (D_stsClockInputOperMode_protected == csmCb.sav.inp.stsClockInputOperMode) { 
		csmCb.inpSts[indx].stsClockInputState = D_stsClockInputState_standby;
		/* 
		 * Check if it's protected mode, if yes, need to find if the line has
	 	 * the priority to become active
		 */
		if ((D_stsClockInputState_active != 
			csmCb.inpSts[csmCb.actvIndx].stsClockInputState) || 
			(csmCb.sav.inpClk[indx].priority < 
			csmCb.sav.inpClk[csmCb.actvIndx].priority)) {
			/* We have the higher priority, switch clock by programming FPGA */
			if (CsmSwitchFpgaClkSrc(csmCb.actvIndx, indx) == CSM_RET_OK) {
				/* Put the original active to standby if it's active */
				if (D_stsClockInputState_active == 
					csmCb.inpSts[csmCb.actvIndx].stsClockInputState) {
					csmCb.inpSts[csmCb.actvIndx].stsClockInputState = 
						D_stsClockInputState_standby;
				}

				/* Put our line into active state */
				csmCb.inpSts[indx].stsClockInputState = D_stsClockInputState_active;
				csmCb.actvIndx = indx;
			}  else {
					printf("%s CsmSwitchFpgaClkSrc error\n", __FUNCTION__);
			}
		} 
	}

	CsmExitCriticalSection();
}

void 
CsmClockInputManualSet(U8_t indx)
{	
	printf("!!! %s indx=%d\n", __FUNCTION__,indx); 

	CsmEnterCriticalSection();

	if (CsmSwitchFpgaClkSrc(csmCb.actvIndx, indx) ==  CSM_RET_OK) {
		if ((csmCb.actvIndx != indx) && 
			(D_stsClockInputState_active ==
			csmCb.inpSts[csmCb.actvIndx].stsClockInputState)) {
			if (CSM_PHY_LINK_UP == csmCb.phySts[csmCb.actvIndx]) {
				csmCb.inpSts[csmCb.actvIndx].stsClockInputState = 
					D_stsClockInputState_standby;
			} else {
				csmCb.inpSts[csmCb.actvIndx].stsClockInputState = 
					D_stsClockInputState_down;
			}
		}
		csmCb.actvIndx = indx;
		csmCb.inpSts[indx].stsClockInputState = D_stsClockInputState_active;
	} else {
		printf("%s CsmSwitchFpgaClkSrc error\n", __FUNCTION__);
	}

	CsmExitCriticalSection();
}

void 
CsmClockRefDown(U8_t indx)
{	
	printf("%s indx=%d\n", __FUNCTION__,indx); 

	CsmEnterCriticalSection();
	CsmExitCriticalSection();
}

void 
CsmClockRefUp(U8_t indx)
{	
	printf("%s indx=%d\n", __FUNCTION__,indx); 

	CsmEnterCriticalSection();
	CsmExitCriticalSection();
}

/*========================================================================
 * Function:     CsmCheckClkSrc
 *
 * Description:  Routine to select the clock source
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
CSM_RET_e
CsmCheckClkSrc (void)
{
    CSM_RET_e   ret = CSM_RET_OK;
	U8_t		bestIndx = 0;

	CsmEnterCriticalSection();

	if (D_stsClockInputOperMode_protected == csmCb.sav.inp.stsClockInputOperMode) {
		/* 
		 * only if the mode is protected, then need to check if there is any 
		 * better clock source
		 */
		if (((bestIndx = CsmGetBestStandby()) !=  CSM_INVALID_LINE) &&
			(bestIndx != csmCb.actvIndx) &&
			((csmCb.sav.inpClk[bestIndx].priority < 
			csmCb.sav.inpClk[csmCb.actvIndx].priority) ||
			(csmCb.phySts[csmCb.actvIndx] != CSM_PHY_LINK_UP))) {
			/* find a better clock source and under protected mode, switch */
			if (CsmSwitchFpgaClkSrc(csmCb.actvIndx, bestIndx) ==  CSM_RET_OK) {
				if (CSM_PHY_LINK_UP == 	csmCb.phySts[csmCb.actvIndx]) {
					/* Put the original active to standby */
					csmCb.inpSts[csmCb.actvIndx].stsClockInputState = 
						D_stsClockInputState_standby;
				} else {
					csmCb.inpSts[csmCb.actvIndx].stsClockInputState = 
						D_stsClockInputState_down;
				}

				csmCb.actvIndx = bestIndx;
				csmCb.inpSts[bestIndx].stsClockInputState = D_stsClockInputState_active;
			} else {
				printf("%s CsmSwitchFpgaClkSrc error\n", __FUNCTION__);
			}
		}
	}

	CsmExitCriticalSection();

    return (ret);
}

BOOL_t
CsmIsHoldover (void)
{
	U8_t i;

	for (i = 0; i < CSM_MAX_CLK_INPUT; i++) {
		/* If there is any line in active, then it's not holdover */
		if (D_stsClockInputState_active == csmCb.inpSts[i].stsClockInputState) {
			return FALSE;
		}
	}
	return TRUE;		
}

SR_INT32		
CsmActiveEsrStratumLevelGet(void)
{
	if ((D_stsClockInputState_active == csmCb.inpSts[0].stsClockInputState) ||
		(D_stsClockInputState_active == csmCb.inpSts[1].stsClockInputState)) {
			return D_stsClockInputValue_prs;	/* stratum 1 */
	}
	return D_stsClockInputValue_stu;			/* stratum unknown */
}


SR_INT32		
CsmActiveInputStratumLevelGet(void)
{
	if (D_stsClockInputState_active == csmCb.inpSts[3].stsClockInputState) {
		return SmInputStratumLevelGet(0);
	} else if (D_stsClockInputState_active == csmCb.inpSts[4].stsClockInputState) {
		return SmInputStratumLevelGet(1);
	} else {
		return D_stsClockInputValue_stu;			/* stratum unknown */
	}
}

SR_INT32		
SmInputStratumLevelGet(U8_t line)
{
	return D_stsClockInputValue_stu;			/* stratum unknown */
}

SR_INT32		
SmHoldoverStratumLevelGet(void)
{
	return D_stsClockInputValue_stu;			/* stratum unknown */
}
