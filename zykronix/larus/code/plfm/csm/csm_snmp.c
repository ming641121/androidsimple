/* $Id: csm_snmp.c,v 1.8 2006/09/06 04:35:13 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_snmp.c,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of CSM SNMP
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Log: csm_snmp.c,v $
 * Revision 1.8  2006/09/06 04:35:13  ffang
 * add physical link status for manual mode
 *
 * Revision 1.7  2006/08/30 04:43:18  ffang
 * add API for osc, imu links
 *
 * Revision 1.6  2006/07/27 03:13:01  ffang
 * change return type of snmp action routine from CSM_RET_e to STATUS_t
 *
 * Revision 1.5  2006/07/25 02:39:21  ffang
 * 1st integration with framer
 *
 * Revision 1.4  2006/07/18 03:15:52  joehsu
 * *** empty log message ***
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
#include <string.h>
#include "sys_os.h"
#include "csm_if.h"
#include "csm_events.h"
#include "snmppart.h"

/*========================================================================
 * Function: CsmClockInputGetReq    
 *
 * Description:  CSM SNMP Get routine for stsClockInput_t struct
 *
 * In Parms:     inp - pointer to stsClockInput_t struct
 * Out Parms:    none
 *
 * Returns:      OK
 */
STATUS_t
CsmClockInputGetReq (stsClockInput_t * inp)
{
	CSM_FUNC_TRACE1(CsmClockInputGetReq, inp);

    memcpy((char *)inp, (char *)&csmCb.sav.inp, sizeof(csmCb.sav.inp));
    return (OK);
}

/*========================================================================
 * Function: CsmClockInputSetReq    
 *
 * Description:  CSM SNMP Set routine for stsClockInput_t struct
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK or ERROR if save to DB failed              
 */
STATUS_t
CsmClockInputSetReq (stsClockInput_t * inp)
{
    STATUS_t   ret = OK;

	CSM_FUNC_TRACE1(CsmClockInputSetReq, inp);
	/*CsmEnterCriticalSection(); */

 	if (VALID(I_stsClockInputOperMode, inp->valid)) {
	   csmCb.sav.inp.stsClockInputOperMode = 
	      inp->stsClockInputOperMode;
#if 1 /* do we need this?  maybe, it's possible this mode called only without input? */
		if (D_stsClockInputOperMode_manual == csmCb.sav.inp.stsClockInputOperMode) {
			CsmClockInputManualSet(csmCb.sav.inp.stsClockInputManualSelect-1);
		}
#endif 
	}	
    if (VALID(I_stsClockInputManualSelect, inp->valid)) {
		csmCb.sav.inp.stsClockInputManualSelect = 
	      inp->stsClockInputManualSelect;
		if (D_stsClockInputOperMode_manual == csmCb.sav.inp.stsClockInputOperMode) {
			CsmClockInputManualSet(inp->stsClockInputManualSelect-1);
		}
	} 
	if (VALID(I_stsClockInputSetSlip, inp->valid)) {
	   csmCb.sav.inp.stsClockInputSetSlip = 
	      inp->stsClockInputSetSlip;
	}
#if 0	
	if (VALID(I_stsClockInputSSMSelect, inp->valid)) {
	   csmCb.sav.inp.stsClockInputSSMSelect = 
	      inp->stsClockInputSSMSelect;
	}
#endif /* 0 */	
	if (VALID(I_stsClockInputValue, inp->valid)) {
	   csmCb.sav.inp.stsClockInputValue = 
	      inp->stsClockInputValue;
	} 

    /* Program the FPGA registers accordingly */

    /* Need to save the changes to data base for persistance */
	if (CsmDbSav(&csmCb.sav, sizeof(csmCb.sav)) != CSM_RET_OK) {
	   CsmLogErrEvent(CSM_SAV_DATA_FAILURE);
	   ret = ERROR;
	}

/*CSM_CLK_IN_SET_EXIT:*/
	/*CsmExitCriticalSection();*/
    return (ret);
}

/*========================================================================
 * Function: CsmClockInputStatusGetReq    
 *
 * Description:  CSM SNMP Get routine for stsClockInput_t struct
 *
 * In Parms:     inp - pointer to stsClockInputStatusEntry_t struct
 * Out Parms:    none
 *
 * Returns:      OK
 */
STATUS_t
CsmClockInputStatusGetReq (stsClockInputStatusEntry_t * inp)
{
	STATUS_t   ret = OK;

	CSM_FUNC_TRACE1(CsmClockInputStatusGetReq, inp);
    if (inp->stsClockInputStatusIndex) {
       inp->stsClockInputState = 
          csmCb.inpSts[inp->stsClockInputStatusIndex-1].stsClockInputState;
	}

    return (ret);
}

/*========================================================================
 * Function: CsmClockInputStatusSetReq    
 *
 * Description:  There is no set request for stsClockInputStatusEntry_t 
 *               structure
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */

/*========================================================================
 * Function: CsmClockPriGetReq    
 *
 * Description:  CSM SNMP Get routine for stsClockPriEntry_t struct
 *
 * In Parms:     inp - pointer to stsClockPriEntry_t struct
 * Out Parms:    none
 *
 * Returns:      OK 
 */
STATUS_t
CsmClockPriGetReq (stsClockPriEntry_t * pri)
{
	STATUS_t   ret = OK;

	CSM_FUNC_TRACE1(CsmClockPriGetReq, pri);
    if (D_stsClockPriIndex_esr == pri->stsClockPriIndex) {
		/* give them the ESR-1 (index 0, same as ESR-2) priority */
		pri->stsClockPriSetting = csmCb.sav.inpClk[0].priority;
	} else if (D_stsClockPriIndex_input == pri->stsClockPriIndex) {
		/* give them the Link-1 (index 3, same as Link-2) priority */
		pri->stsClockPriSetting = csmCb.sav.inpClk[3].priority;
	} else if (D_stsClockPriIndex_prs == pri->stsClockPriIndex) {
		/* give them the PRS (index 2) priority */
		pri->stsClockPriSetting = csmCb.sav.inpClk[2].priority;
	}

    return (ret);
}

/*========================================================================
 * Function: CsmClockPriSetReq    
 *
 * Description:  CSM SNMP Set routine for stsClockPriEntry_t struct 
 *
 * In Parms:     inp - pointer to stsClockPriEntry_t struct
 * Out Parms:    none
 *
 * Returns:      OK or ERROR if save to DB failed              
 */
STATUS_t
CsmClockPriSetReq (stsClockPriEntry_t * pri)
{
	STATUS_t   ret = OK;

	CSM_FUNC_TRACE1(CsmClockPriGetReq, pri);

	/*CsmEnterCriticalSection(); */

    if (D_stsClockPriIndex_esr == pri->stsClockPriIndex) {
		/* Set priority of ESR-1 (index 0, same as ESR-2) priority */
		csmCb.sav.inpClk[0].priority = pri->stsClockPriSetting;
		csmCb.sav.inpClk[1].priority = pri->stsClockPriSetting;
	} else if (D_stsClockPriIndex_input == pri->stsClockPriIndex) {
		/* give them the Link-1 (index 3, same as Link-2) priority */
		csmCb.sav.inpClk[3].priority = pri->stsClockPriSetting;
		csmCb.sav.inpClk[4].priority = pri->stsClockPriSetting;
	} else if (D_stsClockPriIndex_prs == pri->stsClockPriIndex) {
		/* give them the PRS (index 2) priority */
		csmCb.sav.inpClk[2].priority = pri->stsClockPriSetting;
	}

	/*CsmExitCriticalSection();*/

    return (ret);
}

/*========================================================================
 * Function: CsmSendTrap    
 *
 * Description:  CSM SNMP Send Trap routine
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
STATUS_t
CsmSendTrap (U32_t trapType, char * msg)
{
    STATUS_t   ret = OK;
	CSM_FUNC_TRACE2(CsmSendTrap, trapType, msg);
#if 0
    mmsclock_t clk;

    if (dbgCsm)
        printf("%s:trap=%d,msg=%s\n", __FUNCTION__,
            trapType, msg);

    memset((char *) &clk, 0, sizeof(mmsclock_t));
    CsmGetInfo(&clk);
    switch (trapType)
    {
        case clockMajorAlarm_TRAP:
            SiwEventLog(NULL, SIW_EVENT_SYM(CSM, CLK_MAJOR_ALRM), 
                msg);
            TrapEventLog(trapType, (U32_t *) &clk, 
                SIW_EVENT_SYM(CSM, CLK_MAJOR_ALRM), msg);
            break;
        case clockMinorAlarm_TRAP:
            SiwEventLog(NULL, SIW_EVENT_SYM(CSM, CLK_MINOR_ALRM), 
                msg);
            TrapEventLog(trapType, (U32_t *) &clk, 
                SIW_EVENT_SYM(CSM, CLK_MINOR_ALRM), msg);
            break;
        case clockAlarmCleared_TRAP:
            SiwEventLog(NULL, SIW_EVENT_SYM(CSM, CLK_ALRM_CLEAR), 
                msg);
            TrapEventLog(trapType, (U32_t *) &clk, 
                SIW_EVENT_SYM(CSM, CLK_ALRM_CLEAR), msg);
            break;
        case clockSwitched_TRAP:
            SiwEventLog(NULL, SIW_EVENT_SYM(CSM, CLK_SWITCH), msg);
            TrapEventLog(trapType, (U32_t *) &clk, 
                SIW_EVENT_SYM(CSM, CLK_SWITCH), msg);
            break;
    }

#endif  
    return (ret);
}

#if 0
/*========================================================================
 * Function: ClockSnmpAction    
 *
 * Description:  CSM SNMP Action routine
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
STATUS_t
ClockSnmpAction (void)
{
	STATUS_t   ret = CSM_RET_OK;

    switch(type)
    {
        case SNMP_GET_REQ:
            return CsmGetReq(clockData);
        case SNMP_SET_REQ:
            return CsmSetReq(clockData);
        default:
            printf("ERR:ClockSnmpAction:ACCESS_t=%d not supported\n",
                type);
            return CSM_INVALID_SNMP_REQ;
    }

    return (ret);
}
#endif /* 0 */

/*========================================================================
 * Function: CsmStratumLevelGet    
 *
 * Description:  CSM API to get the current active input clock's stratum level
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      The stratum level of the current active input clock
 */
SR_INT32 
CsmStratumLevelGet (void)
{
	SR_INT32	level = D_stsClockInputValue_stu;

	CSM_FUNC_TRACE0(CsmStratumLevelGet);

	if (CsmIsHoldover()) {
		/* need to ask SM about stratum level */
		level = SmHoldoverStratumLevelGet();
	} else {
		/* need to check which input is active, check ESR and Input */
		if ((level = CsmActiveEsrStratumLevelGet()) == D_stsClockInputValue_stu) {
			/* no ESR is active, check input */
			level = CsmActiveInputStratumLevelGet();
		}
	}
	return level;
}

SR_INT32 
CsmSsmValueGet (void)
{
	if (CSM_INVALID_LINE != csmCb.actvIndx) {
		return csmCb.sav.inpClk[csmCb.actvIndx].stratum;
	} else {
		return (D_stsClockInputValue_stu);
	}
}

/*========================================================================
 * Function: CsmClockSourceGet    
 *
 * Description:  CSM API to get the current active input clock's stratum level
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      The stratum level of the current active input clock
 */
SR_INT32 
CsmClockSourceGet (void)
{
	CSM_FUNC_TRACE0(CsmClockSourceGet);

	if (CSM_INVALID_LINE != csmCb.actvIndx) {
		return csmCb.sav.inpClk[csmCb.actvIndx].stratum;
	} else {
		return (CSM_INVALID_SR_INT32);
	}
}

int getRedState(int stsCardIndex, int whichlink)
{
	switch (stsCardIndex) {
		case D_stsCardIndex_esr1:
			return csmCb.inpSts[0].stsClockInputState;
		case D_stsCardIndex_esr2:
			return csmCb.inpSts[1].stsClockInputState;
		case D_stsCardIndex_oscillator1:
			return csmCb.oscSts[0];
		case D_stsCardIndex_oscillator2:
			return csmCb.oscSts[1];
		case D_stsCardIndex_imu:
			if ((whichlink > 0) && (whichlink <= D_stsLinkIndex_link5))	{
				return csmCb.imuSts[whichlink-1];
			} else {
				return -1;
			}
		default:
			return -1;
	}
}