/* $Id: csm_dbm.c,v 1.10 2006/10/03 05:19:19 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_dbm.c,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of CSM Data Base
 *    Management for persistancy
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Id: csm_dbm.c,v 1.10 2006/10/03 05:19:19 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_dbm.c,v $
 */
#include <stdio.h>
#include <errno.h>

#include "sys_os.h"
#include "csm_if.h"

static FILE * csm_fp = NULL; 
const char * csm_fname = "C:/config/csmdb.cfg";

static void
CsmDefaultInit (void)
{
	U8_t				i;

   	/* There is no Valid Data Base, use default value */
   	csmCb.sav.inp.stsClockInputManualSelect = D_stsClockInputManualSelect_esr1;
   	csmCb.sav.inp.stsClockInputOperMode = D_stsClockInputOperMode_protected;
   	csmCb.sav.inp.stsClockInputSetSlip = D_stsClockInputSetSlip_refAb;
   	csmCb.sav.inp.stsClockInputValue = D_stsClockInputValue_st3;	  /* ??? all input */
		
   	for (i = 0; i < CSM_MAX_CLK_INPUT; i++) {
	 	csmCb.sav.inpClk[i].stratum = D_stsClockInputValue_st3;	 /* ??? */	
	} /* for (i = 0; i < CSM_MAX_CLK_INPUT; i++) */
		
	/* The following is the default priority for clock input */
	csmCb.sav.inpClk[0].priority = D_stsClockPriSetting_priority_1;	/* ESR 1 */
	csmCb.sav.inpClk[1].priority = D_stsClockPriSetting_priority_1;	/* ESR 2 */
	csmCb.sav.inpClk[2].priority = D_stsClockPriSetting_priority_2;	/* PRS */
	csmCb.sav.inpClk[3].priority = D_stsClockPriSetting_priority_3;	/* LINK 1 */
	csmCb.sav.inpClk[4].priority = D_stsClockPriSetting_priority_3;	/* LINK 2 */
}

/*========================================================================
 * Function:     CsmDbInit
 *
 * Description:  Routine to commit the data to DBM for persistence
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
CSM_RET_e
CsmDbInit (void)
{
    CSM_RET_e			ret = CSM_RET_OK;
	int 				fsize;

	if ((csm_fp = fopen(csm_fname,"a+b")) != NULL) {
		fseek(csm_fp, 0, SEEK_END);
		if ((fsize = ftell(csm_fp)) > 0) { 
			printf("!!! %s exist, file size = %d\n", csm_fname, fsize);  
			ret = CsmDbRead(&csmCb.sav, sizeof(csmCb.sav));
		} else {
			printf("!!! %s is first time created\n", csm_fname);  

			CsmDefaultInit();

			/* Save the Data Base */

			ret = CsmDbSav(&csmCb.sav, sizeof(csmCb.sav));
		} 
	} else {
		perror("CsmDbInit");
		CsmDefaultInit();
		ret = CSM_DBINIT_ERROR;
	}

	if (D_stsClockInputOperMode_manual == csmCb.sav.inp.stsClockInputOperMode) {
		CsmClockInputManualSet(csmCb.sav.inp.stsClockInputManualSelect-1);
	}
    return (ret); 
}

/*========================================================================
 * Function:     CsmDbSav
 *
 * Description:  Routine for CSM to commit data to DBM for persistance
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
CSM_RET_e
CsmDbSav (void * sav, U32_t len)
{
    CSM_RET_e			ret = CSM_RET_OK;
	int 				wlen = 0;

	printf("\n!!! %s called, len=%d\n", __FUNCTION__, len);
	if (csm_fp) {
		/* always stored from beginning of file, overwrite from beninning */
		if (fseek(csm_fp, 0, SEEK_SET) != 0) {
			perror("CsmDbSav:fseek");
			return CSM_DBSAV_FWRITE_ERROR;
		}
		if ((wlen = fwrite(sav, 1, len, csm_fp)) != len) {
			printf("wlen=%d,len=%d\n", wlen, len);
			perror("CsmDbSav");
			return CSM_DBSAV_FWRITE_ERROR;
		} else {
			printf("save db with len=%d\n", wlen);
			fflush(csm_fp);
		}
	} else {
		printf("CsmDbSav, NULL fp\n");
		return CSM_DB_NULL_FP_ERROR;
	}
	/* Save the data to NV */
    return (ret);
}

/*========================================================================
 * Function:     CsmDbRead
 *
 * Description:  Routine for CSM to commit data to DBM for persistance
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
CSM_RET_e
CsmDbRead (void * sav, U32_t len)
{
    CSM_RET_e            ret = CSM_RET_OK;

	printf("\n!!! %s called\n", __FUNCTION__);

	if (csm_fp) {
		/* always read from beginning of file */
		fseek(csm_fp, 0, SEEK_SET);
		if (fread(sav, 1, len, csm_fp) != len) {
			perror("CsmDbRead");
			return CSM_DBSAV_FREAD_ERROR;
		}
	} else {
		printf("CsmDbRead, NULL fp\n");
		return CSM_DB_NULL_FP_ERROR;
	}	

	/* Save the data to NV */
    return (ret);
}
