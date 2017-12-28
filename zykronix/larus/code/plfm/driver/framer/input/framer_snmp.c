/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: framer_snmp.c
 *
 * 
 *
 *Note:
 *
 *Author(s):  shuming chang
 */
 /********************************************************************
 * $Id: framer_snmp.c,v 1.5 2006/09/25 11:00:51 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/driver/framer/input/framer_snmp.c,v $
 *====================================================================
 * $Log: framer_snmp.c,v $
 * Revision 1.5  2006/09/25 11:00:51  cvsshuming
 * add ssm action func
 *
 * Revision 1.4  2006/09/22 10:47:58  cvsshuming
 * fix link enable issue
 *
 * Revision 1.3  2006/09/15 09:58:13  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.2  2006/09/15 06:37:50  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.1  2006/09/12 07:34:04  cvsshuming
 * move framer snmp part out of framer.c
 * 
 *
 * $Endlog $
 *********************************************************************/
 

#include <stdlib.h>
#include <stdio.h>
#include <vxWorks.h>
#include <sys_memmap.h>
#include <tss_types.h>
#include "ds26502.h"
#include <taskLib.h>
#include <intLib.h>
#include <iv.h>
#include "framer_event.h"
#include "trapdef.h"
#include "siw_events.h"
#include "trap_if.h"
#include "led_if.h"
#include "pm.h"
#include "sm.h"




#include "sr_snmp.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpdefs.h"
#include "snmppart.h"
#include "snmpsupp.h"
#include "snmptrap.h"
#include "sys_snmp.h"


/* SNMP function declaration */

#if 0   /* this has been implemented in PM module*/
STATUS_t inpFramerAlarmGet(stsLinkAlarmEntry_t * mibstructIn);
STATUS_t inpFramerAlarmSet(stsLinkAlarmEntry_t * mibstructIn);
#endif

STATUS_t inpFramerLinkSet(stsLinkEntry_t * mibstructIn);
STATUS_t inpFramerLinkGet(stsLinkEntry_t * mibstructIn);
STATUS_t SSMSet(stsClockInputSSMEntry_t *mibstructIn);
STATUS_t SSMGet(stsClockInputSSMEntry_t *mibstructIn);




#if 0   /* this has been implemented in PM module*/ 
/**************************************************************************************
* function: STATUS_T inpFramerAlarmGet(stsLinkAlarmEntry_t * mibstructIn)
*  input parameter
*			SR_INT32 stsLinkAlarmSettingLinkType
*			SR_INT32 stsLinkAlarmSettingType;
*  output parameter
*			SR_INT32 stsLinkAlarmSettingSevirity;
*			SR_INT32 stsLinkAlarmSevirity;
*  return
*    OK: if alarmtype is legal and alarm is sent
*    ERROR: otherwise
***************************************************************************************/



STATUS_t inpFramerAlarmGet(stsLinkAlarmEntry_t * mibstructIn)
{
    /* sanity check */
    if (mibstructIn->stsLinkAlarmSettingLinkType <
        D_stsLinkAlarmSettingLinkType_link1
        || mibstructIn->stsLinkAlarmSettingLinkType >
        D_stsLinkAlarmSettingLinkType_slnk) {
        return ERROR;
    }


    /* process alarm severity */
    framerSeverityCheck((mibstructIn->stsLinkAlarmSettingLinkType - 1));
    /* reply */
    switch (mibstructIn->stsLinkAlarmSettingLinkType) {
    case D_stsLinkAlarmSettingType_act:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].act;
        break;
    case D_stsLinkAlarmSettingType_los:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].los;
        break;
    case D_stsLinkAlarmSettingType_aisp:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].aisp;
        break;
    case D_stsLinkAlarmSettingType_lof:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].lof;
        break;
    case D_stsLinkAlarmSettingType_cvl:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].cvl;
        break;
    case D_stsLinkAlarmSettingType_cvp:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].cvp;
        break;
    case D_stsLinkAlarmSettingType_cs:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].cs;
        break;
    case D_stsLinkAlarmSettingType_jitter:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].jitter;
        break;
    case D_stsLinkAlarmSettingType_tie:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].tie;
        break;
    case D_stsLinkAlarmSettingType_mtie:
        mibstructIn->stsLinkAlarmSettingSevirity
            =
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].mtie;
        break;
    default:
        return ERROR;
    }


    mibstructIn->stsLinkAlarmSevirity
        =
        inpFramer[(mibstructIn->stsLinkAlarmSettingLinkType -
                   1)].alarmSeverity;



    return OK;
}

/**************************************************************************************
* function: STATUS_T inpFramerAlarmSet(stsLinkAlarmEntry_t * mibstructIn)
*  input parameter
*			SR_INT32 stsLinkAlarmSettingLinkType
*			SR_INT32 stsLinkAlarmSettingType;
*			SR_INT32 stsLinkAlarmSettingSevirity;
*	 not-setable item
*			SR_INT32 stsLinkAlarmSevirity;
*  return
*    OK: if alarmtype is legal and alarm is sent
*    ERROR: otherwise
***************************************************************************************/

STATUS_t inpFramerAlarmSet(stsLinkAlarmEntry_t * mibstructIn)
{
    /* sanity check */
    if (mibstructIn->stsLinkAlarmSettingLinkType <
        D_stsLinkAlarmSettingLinkType_link1
        || mibstructIn->stsLinkAlarmSettingLinkType >
        D_stsLinkAlarmSettingLinkType_slnk) {
        return ERROR;
    }



    /* reply */
    if (VALID(I_stsLinkAlarmSettingSevirity, mibstructIn->valid)) {
        switch (mibstructIn->stsLinkAlarmSettingLinkType) {
        case D_stsLinkAlarmSettingType_act:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].act =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        case D_stsLinkAlarmSettingType_los:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].los =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        case D_stsLinkAlarmSettingType_aisp:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].aisp =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        case D_stsLinkAlarmSettingType_lof:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].lof =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        case D_stsLinkAlarmSettingType_cvl:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].cvl =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        case D_stsLinkAlarmSettingType_cvp:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].cvp =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        case D_stsLinkAlarmSettingType_cs:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].cs =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        case D_stsLinkAlarmSettingType_jitter:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].jitter =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        case D_stsLinkAlarmSettingType_tie:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].tie =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        case D_stsLinkAlarmSettingType_mtie:
            pmSeverity[(mibstructIn->stsLinkAlarmSettingLinkType -
                       1)].mtie =
                mibstructIn->stsLinkAlarmSettingSevirity;
            break;

        default:
            return ERROR;
        }

    }



    return OK;
}



#endif


/**************************************************************************************
* function: STATUS_t inpFramerLinkGet(stsLinkEntry_t * mibstructIn)
*  input parameter
*			stsLinkAlarmSettingLinkType  mibstructIn
*  output parameter

        SR_INT32        stsLinkType;
        SR_INT32        stsLinkEnable;
        SR_INT32        stsLinkLineCode;
        SR_INT32        stsLinkFMT;
        SR_INT32        stsLinkTermination;
        SR_INT32        stsLinkMFCHK;
        OctetString     *stsLinkId;
        char            valid[2];
*  return
*    OK: if alarmtype is legal and alarm is sent
*    ERROR: otherwise
***************************************************************************************/
STATUS_t inpFramerLinkGet(stsLinkEntry_t * mibstructIn)
{

    /* reply */
    mibstructIn->stsLinkType =
        te1Cfg[(mibstructIn->stsLinkIndex - 1)].lineType + 1;
    /*checkitout below */
    
    switch(te1Cfg[(mibstructIn->stsLinkIndex - 1)].linkEnable){
    	case FRAMER_LINK_ENABLE_YES:
    		mibstructIn->stsLinkEnable = D_stsLinkEnable_yes;
    		break;
    	case FRAMER_LINK_ENABLE_NO:
    		mibstructIn->stsLinkEnable = D_stsLinkEnable_no;
    		break;
    }

    mibstructIn->stsLinkLineCode =
        te1Cfg[(mibstructIn->stsLinkIndex - 1)].coding + 1;

    mibstructIn->stsLinkFMT =
        te1Cfg[(mibstructIn->stsLinkIndex - 1)].framing + 1;

    mibstructIn->stsLinkTermination =
        te1Cfg[(mibstructIn->stsLinkIndex - 1)].termination + 1;

    mibstructIn->stsLinkMFCHK =
        te1Cfg[(mibstructIn->stsLinkIndex - 1)].lineType + 1;

		mibstructIn->stsLinkId = 
				&(te1Cfg[(mibstructIn->stsLinkIndex - 1)].linkId);
    
    
    return OK;
}

/**************************************************************************************
* function: STATUS_t inpFramerLinkSet(stsLinkEntry_t * mibstructIn)
*  input parameter
*			stsLinkAlarmSettingLinkType  mibstructIn
        SR_INT32        stsLinkType;
        SR_INT32        stsLinkEnable;
        SR_INT32        stsLinkLineCode;
        SR_INT32        stsLinkFMT;
        SR_INT32        stsLinkTermination;
        SR_INT32        stsLinkMFCHK;
        OctetString     *stsLinkId;
        char            valid[2];
*  return
*    OK: if alarmtype is legal and alarm is sent
*    ERROR: otherwise
***************************************************************************************/

STATUS_t inpFramerLinkSet(stsLinkEntry_t * mibstructIn)
{

    if (VALID(I_stsLinkType, mibstructIn->valid)) {

        te1Cfg[(mibstructIn->stsLinkIndex - 1)].lineType =
            mibstructIn->stsLinkType - 1;

    }
    if (VALID(I_stsLinkEnable, mibstructIn->valid)) {
	    switch(mibstructIn->stsLinkEnable){
	    	case D_stsLinkEnable_yes:
	    		te1Cfg[(mibstructIn->stsLinkIndex)-1].linkEnable = FRAMER_LINK_ENABLE_YES;
	    		break;
	    	case D_stsLinkEnable_no:
	    		te1Cfg[(mibstructIn->stsLinkIndex)-1].linkEnable = FRAMER_LINK_ENABLE_NO;
	    		break;
	    }
    }
    if (VALID(I_stsLinkLineCode, mibstructIn->valid)) {

        te1Cfg[(mibstructIn->stsLinkIndex - 1)].coding =
            mibstructIn->stsLinkLineCode - 1;
    }
    if (VALID(I_stsLinkFMT, mibstructIn->valid)) {

        te1Cfg[(mibstructIn->stsLinkIndex - 1)].framing =
            mibstructIn->stsLinkFMT - 1;
    }
    if (VALID(I_stsLinkTermination, mibstructIn->valid)) {

        te1Cfg[(mibstructIn->stsLinkIndex - 1)].termination =
            mibstructIn->stsLinkTermination - 1;

    }
    if (VALID(I_stsLinkMFCHK, mibstructIn->valid)) {

        te1Cfg[(mibstructIn->stsLinkIndex - 1)].lineType =
            mibstructIn->stsLinkMFCHK - 1;
    }

    if (VALID(I_stsLinkId, mibstructIn->valid)) {
		if (mibstructIn->stsLinkId->length  < FRAMER_LINKID_LEN){
			te1Cfg[(mibstructIn->stsLinkIndex - 1)].linkId.length   =  mibstructIn->stsLinkId->length;            
			memcpy(te1Cfg[(mibstructIn->stsLinkIndex - 1)].linkId.octet_ptr,
                mibstructIn->stsLinkId->octet_ptr, mibstructIn->stsLinkId->length);
        }else{
			memcpy(te1Cfg[(mibstructIn->stsLinkIndex - 1)].linkId.octet_ptr,
                mibstructIn->stsLinkId->octet_ptr, FRAMER_LINKID_LEN);
			te1Cfg[(mibstructIn->stsLinkIndex - 1)].linkId.length   =  FRAMER_LINKID_LEN;
        
        }
	}
		
	/* save the configuration to flash*/	
	if (inpLineConfig((mibstructIn->stsLinkIndex) - 1) == OK ){
			return (framerDataAccess("telCfg.dat", FLASH_WRITE, TEL_CFG_DATA));
		}else{
			return ERROR;
	}
		
		
}


/**************************************************************************************
* function: STATUS_t SSMGet(stsClockInputSSMEntry_t *mibstructIn)
*  input parameter
        SR_INT32        stsClockInputSSMIndex;
        SR_INT32        stsClockInputSSMInputSelect;
        SR_INT32        stsClockInputSSMValue;
        char            valid[1];
*  return
*    OK: 	OK
*    ERROR: otherwise
***************************************************************************************/

STATUS_t SSMGet(stsClockInputSSMEntry_t *mibstructIn){
	SSM_t *thisSSM;
	TE1_CONFIG_t *thisLineConfig = &te1Cfg[(mibstructIn->stsClockInputSSMIndex )-1];
	
	
	getInpFramerSSM((mibstructIn->stsClockInputSSMIndex )-1);
	switch(thisLineConfig->lineType){
		case LT_E1:
			if ( ((mibstructIn->stsClockInputSSMInputSelect) > D_stsClockInputSSMSelect_e1_sa8_ssm)
					|| ((mibstructIn->stsClockInputSSMInputSelect) < D_stsClockInputSSMSelect_e1_sa4_ssm  )){
				return ERROR;
			}
			thisSSM = &receivedSSM[(mibstructIn->stsClockInputSSMIndex )-1][(mibstructIn->stsClockInputSSMInputSelect )-1];
			switch(thisSSM->ssm){
				case SSM_E1_PRS:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_prs;
					break;
				case SSM_E1_STU:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_stu;
					break;
				case SSM_E1_ST2:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_st2;
					break;
				case SSM_E1_ST3:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_st3;
					break;
				case SSM_E1_SIC:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_sic;
					break;
				case SSM_E1_DUS:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_duc;
					break;
				default:
					return ERROR;
			}
			break;
		case LT_T1:
		case LT_J1:
			if ( (mibstructIn->stsClockInputSSMInputSelect) != D_stsClockInputSSMSelect_t1_ssm){
				return ERROR;
			}
			thisSSM = &receivedSSM[(mibstructIn->stsClockInputSSMIndex )-1][0];
			switch(thisSSM->ssm){
				case SSM_T1_PRS:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_prs;
					break;
				case SSM_T1_STU:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_stu;
					break;
				case SSM_T1_ST2:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_st2;
					break;
				case SSM_T1_ST3:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_st3;
					break;
				case SSM_T1_SIC:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_sic;
					break;
				case SSM_T1_DUS:
					mibstructIn->stsClockInputSSMValue = D_stsClockInputSSMValue_duc;
					break;
				default:
					return ERROR;
			}
			break;
		default:
			return ERROR;
			break;
	}
	return OK;

}


/**************************************************************************************
* function: STATUS_t SSMSet(stsClockInputSSMEntry_t *mibstructIn)
*  input parameter
        SR_INT32        stsClockInputSSMIndex;
        SR_INT32        stsClockInputSSMInputSelect;
        SR_INT32        stsClockInputSSMValue;
        char            valid[1];
*  return
*    OK: 	OK
*    ERROR: otherwise
***************************************************************************************/
				
STATUS_t SSMSet(stsClockInputSSMEntry_t *mibstructIn){
	SSM_t *thisSSM;
	TE1_CONFIG_t *thisLineConfig = &te1Cfg[(mibstructIn->stsClockInputSSMIndex )-1];
	/*sanity check*/
	switch(thisLineConfig->lineType){
		case LT_E1:
			if ( ((mibstructIn->stsClockInputSSMInputSelect) > D_stsClockInputSSMSelect_e1_sa8_ssm)
					|| ((mibstructIn->stsClockInputSSMInputSelect) < D_stsClockInputSSMSelect_e1_sa4_ssm  )){
				return ERROR;
			}
			thisSSM = &receivedSSM[(mibstructIn->stsClockInputSSMIndex )-1][(mibstructIn->stsClockInputSSMInputSelect )-1];

			switch(mibstructIn->stsClockInputSSMValue){
				case D_stsClockInputSSMValue_prs:
					thisSSM->ssm = SSM_E1_PRS;
					break;
				case D_stsClockInputSSMValue_stu:
					thisSSM->ssm = SSM_E1_STU;
					break;
				case D_stsClockInputSSMValue_st2:
					thisSSM->ssm = SSM_E1_ST2;
					break;
				case D_stsClockInputSSMValue_st3:
					thisSSM->ssm = SSM_E1_ST3;
					break;
				case D_stsClockInputSSMValue_sic:
					thisSSM->ssm = SSM_E1_SIC;
					break;
				case D_stsClockInputSSMValue_duc:
					thisSSM->ssm = SSM_E1_DUS;
					break;
				default:
					return ERROR;
			}
			
			break;
		case LT_T1:
		case LT_J1:
			if ( (mibstructIn->stsClockInputSSMInputSelect) != D_stsClockInputSSMSelect_t1_ssm){
				return ERROR;
			}
			thisSSM = &receivedSSM[(mibstructIn->stsClockInputSSMIndex )-1][0];

			switch(mibstructIn->stsClockInputSSMValue){
				case D_stsClockInputSSMValue_prs:
					thisSSM->ssm = SSM_T1_PRS;
					break;
				case D_stsClockInputSSMValue_stu:
					thisSSM->ssm = SSM_T1_STU;
					break;
				case D_stsClockInputSSMValue_st2:
					thisSSM->ssm = SSM_T1_ST2;
					break;
				case D_stsClockInputSSMValue_st3:
					thisSSM->ssm = SSM_T1_ST3;
					break;
				case D_stsClockInputSSMValue_sic:
					thisSSM->ssm = SSM_T1_SIC;
					break;
				case D_stsClockInputSSMValue_duc:
					thisSSM->ssm = SSM_T1_DUS;
					break;
				default:
					return ERROR;
			}
			
			break;
		default:
			return ERROR;
			break;
	}
	return OK;

}


