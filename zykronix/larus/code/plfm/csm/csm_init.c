/* $Id: csm_init.c,v 1.6 2006/09/06 04:35:13 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_init.c,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of CSM Initialization
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Log: csm_init.c,v $
 * Revision 1.6  2006/09/06 04:35:13  ffang
 * add physical link status for manual mode
 *
 * Revision 1.5  2006/08/30 04:43:18  ffang
 * add API for osc, imu links
 *
 * Revision 1.4  2006/08/15 03:52:55  ffang
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

#include "stdio.h"
#include <string.h>
#include "sys_os.h"
#include "csm_if.h"

/*
 *====================================================================
 *=                     CSM init routineS                             =
 *====================================================================
 */

/*static	VU8_t		*CSM_BASE; */
SIW_SEMID_t	csmSemId;

/*========================================================================
 * Function:     CsmHwInit
 *
 * Description:  Routine to init the CSM related Hardware (if any)
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
CSM_RET_e
CsmHwInit(void)
{
    CSM_RET_e   ret = CSM_RET_OK;
    return (ret);
}

/*========================================================================
 * Function:     CsmCbInit
 *
 * Description:  Routine to init the CSM control block
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
CSM_RET_e
CsmCbInit (void)
{
    CSM_RET_e   ret = CSM_RET_OK;
	U8_t        i;

	memset((char *)&csmCb, 0, sizeof(csmCb));

	for (i = 0; i < CSM_MAX_CLK_INPUT; i++) {
	    csmCb.inpSts[i].stsClockInputStatusIndex = i+1;	 /* SNMP is 1 based */
	    csmCb.inpSts[i].stsClockInputState = D_stsClockInputState_down;
	    csmCb.phySts[i] = CSM_PHY_LINK_DOWN;     
		csmCb.imuSts[i] = D_stsClockInputState_down;
	}

	csmCb.oscSts[0] = D_stsClockInputState_down;
	csmCb.oscSts[1] = D_stsClockInputState_down;

#if 0
	/* grab the semaphore */
	SiwSemTake(csmSemId, WAIT_FOREVER);

	/* free the semaphore */
	SiwSemGive(csmSemId);

	return (CSM_OK);
#endif /* 0 */
    return (ret);
}

/*========================================================================
 * Function:     CsmInit
 *
 * Description:  Routine to init CSM
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
CSM_RET_e
CsmInit (void)
{
    CSM_RET_e   ret = CSM_RET_OK;
    CSM_DBG_ENTER;

    /* create CSM semaphore ID */
    if (SIW_INVALID_SEMID == (csmSemId = SiwSemCreate(
        "CsmCbSem",
        SIW_SEM_BINARY,
        SIW_SEM_Q_PRIORITY,
        1,
        SIW_RECV_LOG_ERROR))) {
        printf("CSM: CsmInit failed in SemCreate\n");
        return CSM_RET_SEM_ERROR;
    }

    /* init the hw */
    if ((ret = CsmHwInit()) != CSM_RET_OK) {
        return ret;
    }    

    /* init the cb */
    if ((ret = CsmCbInit()) != CSM_RET_OK) {
        return ret;
    }    

	/* 	CsmDbInit is separate and to be initialized in cms_main */

    CSM_DBG_PROBE;
    CSM_DBG_EXIT;
    return (ret);
}
