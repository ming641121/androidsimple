/* $Id: csm_if.h,v 1.11 2006/09/06 04:35:13 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/csm_if.h,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file defines the CSM basic interface and struct
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Log: csm_if.h,v $
 * Revision 1.11  2006/09/06 04:35:13  ffang
 * add physical link status for manual mode
 *
 * Revision 1.10  2006/08/30 04:43:18  ffang
 * add API for osc, imu links
 *
 * Revision 1.9  2006/08/15 03:52:55  ffang
 * *** empty log message ***
 *
 * Revision 1.8  2006/08/15 02:57:23  ffang
 * add prototype for reference clock
 *
 * Revision 1.7  2006/08/03 04:26:48  ffang
 * add CSM_INVALID_INDEX
 *
 * Revision 1.6  2006/07/27 03:12:02  ffang
 * change the return type of snmp action routine from CSM_RET_e to STATUS_t
 *
 * Revision 1.5  2006/07/27 02:22:22  ffang
 * add snmp API
 *
 * Revision 1.4  2006/07/25 02:59:10  ffang
 * 1st integration with framer
 *
 * Revision 1.3  2006/07/18 03:55:13  ffang
 * Add #include "snmpsupp.h"
 *
 * Revision 1.2  2006/07/14 04:19:47  joehsu
 * *** empty log message ***
 *
 * Revision 1.1  2006/07/13 04:23:33  ffang
 * move the file from csm module to inc
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

#ifndef _CSM_IF_H_
#define _CSM_IF_H_

 
#include "sr_snmp.h"
#include "context.h"
#include "method.h"
 

#include "snmptype.h"
#include "snmpdefs.h"
#include "snmppart.h"
#include "snmpsupp.h"

#if 0
#include "snmpprot.h"
#endif

#include "tss_types.h"

/*
 *====================================================================
 *=                        define's                                 =
 *====================================================================
 */

#define CSM_RECOVER_PERIOD		10000 /* 10 sec (in millisec unit) */
#define CSM_MAX_CLK_INPUT		D_stsClockInputManualSelect_link2
#define CSM_MAX_STRATUM			D_stsClockInputType_stratum3
#define CSM_INVALID_LINE		0xFF
#define CSM_INVALID_SR_INT32	0xFFFF
#define CSM_PHY_LINK_DOWN		0
#define CSM_PHY_LINK_UP			1

/*
 *====================================================================
 *=                        typedef's                                 =
 *====================================================================
 */

/* The enum of CSM function return code */
typedef enum 
{
    CSM_RET_OK = 0,

    /* system init errors */
    CSM_TASK_INIT_FAILURE,
    CSM_CB_INIT_FAILURE,
    CSM_SNMP_INIT_FAILURE,

    /* system routine errors */
	CSM_WRITE_8_FAILURE,
	CSM_FEATURE_NOT_IMPLEMENTED,
	CSM_INVALID_INDEX,
	CSM_RET_SEM_ERROR,

    /* DBM related errors */
	CSM_SAV_DATA_FAILURE,
    CSM_DBINIT_ERROR,
    CSM_DBSAV_FWRITE_ERROR,
	CSM_DBSAV_FREAD_ERROR,
	CSM_DB_NULL_FP_ERROR,
    
    /* SNMP related errors */
    	
    CSM_UNKNOWN_ERROR,          /* default, use more specific return code */
    /* Add any return code before this line */
    CSM_RET_MAX,
} CSM_RET_e;

/* The struct of CSM input clock */
typedef struct 
{
	SR_INT32					stratum;	/* stratum level */
	SR_INT32					priority;	/* priority level */
	/*SR_INT32					state;*/		/* state */
} CSM_INPUT_CLK_s;

/* 
 * The structure of CSM saved data for persistance, whatever needs to be
 * saved for persistance, keep them here
 */
typedef struct
{
    stsClockInput_t				inp;
	CSM_INPUT_CLK_s				inpClk[CSM_MAX_CLK_INPUT];
} CSM_SAV_s;

/* The structure of CSM control block */
typedef struct 
{
    CSM_SAV_s					sav;
	stsClockInputStatusEntry_t	inpSts[CSM_MAX_CLK_INPUT];
	SR_INT32					phySts[CSM_MAX_CLK_INPUT]; 
	int							oscSts[2];
	int							imuSts[D_stsLinkIndex_link5];
	U8_t						actvIndx;	/* Current active line index */									
} CSM_CB_s;

/*
 *====================================================================
 *=                        Macro's                                   =
 *====================================================================
 */
#ifdef CSM_DBG_ON

extern BOOL_t dbgCsm;
#define CSM_DBG_ENTER \
   if (dbgCsm) { \
      printf("%s entering...\n", __FUNCTION__); \
   } \

#define CSM_DBG_EXIT \
   if (dbgCsm) { \
      printf("%s exiting...\n", __FUNCTION__); \
   } \

#define CSM_DBG_PROBE \
   if (dbgCsm) { \
      printf("DBG>> %s,%s,line:%d\n", __FILE__, __FUNCTION__, __LINE__); \
   } \

#else /* CSM_DBG_ON */

#define CSM_DBG_ENTER 
#define CSM_DBG_EXIT 
#define CSM_DBG_PROBE 

#endif /* CSM_DBG_ON */

/*
 *====================================================================
 *=                        Extern's                                  =
 *====================================================================
 */

extern SIW_SEMID_t	csmSemId;
extern CSM_CB_s		csmCb;
extern BOOL_t		dbgCsm;
/*extern CSM_INPUT_CLK_s csmInpClk[CSM_MAX_CLK_INPUT];*/

extern CSM_RET_e	CsmInit(void);
extern BOOL_t		CsmDbIsValid(void);
extern CSM_RET_e	CsmDbSav(void *, U32_t);
extern CSM_RET_e	CsmDbRead(void *, U32_t);
extern CSM_RET_e	CsmDbInit(void);
extern BOOL_t		CsmIsHoldover(void);

extern void			CsmLogErrEvent(CSM_RET_e);
extern void			pendSysReady(void);
extern void			pendDbmReady(void);
extern void			CsmEnterCriticalSection(void);
extern void			CsmExitCriticalSection(void);
extern U8_t			CsmRead8(VU32_t * addr);
extern void			CsmWrite8(VU32_t * addr, U8_t data);
extern void			CsmClockInputUp(U8_t indx);
extern void			CsmClockInputDown(U8_t indx);
extern void 		CsmClockInputManualSet(U8_t indx);
extern void			CsmClockRefUp(U8_t indx);
extern void			CsmClockRefDown(U8_t indx);
extern CSM_RET_e	CsmCheckClkSrc(void);

extern SR_INT32		SmInputStratumLevelGet(U8_t line);
extern SR_INT32		SmHoldoverStratumLevelGet(void);

/* CSM API's */
/*=== begin SNMP action routines API ===*/
/*========================================================================
 * Function: CsmClockInputGetReq    
 *
 * Description:  CSM SNMP Get routine for stsClockInput_t struct
 *
 * In Parms:     inp - pointer to stsClockInput_t struct
 * Out Parms:    none
 *
 * Returns:      OK	or ERROR
 */
extern STATUS_t CsmClockInputGetReq (stsClockInput_t * inp);

/*========================================================================
 * Function: CsmClockInputSetReq    
 *
 * Description:  CSM SNMP Set routine for stsClockInput_t struct
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK	or ERROR
 */
extern STATUS_t CsmClockInputSetReq (stsClockInput_t * inp);

/*========================================================================
 * Function: CsmClockInputStatusGetReq    
 *
 * Description:  CSM SNMP Get routine for stsClockInputStatusEntry_t struct
 *
 * In Parms:     inp - pointer to stsClockInputStatusEntry_t struct
 * Out Parms:    none
 *
 * Returns:      OK	or ERROR
 */
extern STATUS_t CsmClockInputStatusGetReq (stsClockInputStatusEntry_t * inp);

/*========================================================================
 * Function: CsmClockPriGetReq    
 *
 * Description:  CSM SNMP Get routine for stsClockPriEntry_t struct
 *
 * In Parms:     inp - pointer to stsClockPriEntry_t struct
 * Out Parms:    none
 *
 * Returns:      OK	or ERROR
 */
extern STATUS_t CsmClockPriGetReq (stsClockPriEntry_t * pri);

/*========================================================================
 * Function: CsmClockPriSetReq    
 *
 * Description:  CSM SNMP Set routine for stsClockPriEntry_t struct 
 *
 * In Parms:     inp - pointer to stsClockPriEntry_t struct
 * Out Parms:    none
 *
 * Returns:      OK	or ERROR
 */
extern STATUS_t CsmClockPriSetReq (stsClockPriEntry_t * pri);
/*=== end SNMP action routines API ===*/

extern SR_INT32		CsmStratumLevelGet(void);	
extern SR_INT32		CsmClockSourceGet(void);
extern SR_INT32		CsmStratumLevelGet(void);
extern SR_INT32		CsmActiveEsrStratumLevelGet(void);
extern SR_INT32		CsmActiveInputStratumLevelGet(void);
extern int			getRedState(int stsCardIndex, int whichlink);

#endif /* _CSM_MAIN_H_ */
