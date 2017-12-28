#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vxWorks.h>
#include <axss_types.h>
#include <snmppart.h>
#include <pm.h>

U8_t  manSelect;
S32_t manScanInterval;
S32_t manNumReport;
S32_t manNumObservation;
OctetString  pmTmp;
U8_t  pmTmpBuf[4];
S32_t manSmuTestSetFlag = 0;

/* Called by SNMP */
STATUS_t pmThresholdSet(stsAlarmThresholdEntry_t *threshold)
{
    U8_t ch;
    PM_FRAMER_t *ptr;
	FILE *fhd = NULL;
	PM_THRESHOLD_t tmp;

    ch = threshold->stsAlarmThresholdCurrentMon - 1;

	if (ch > (MAX_FRAMERS-1))
	    return ERROR;
	     
	ptr = &pmFramer[ch];

    switch (threshold->stsAlarmThresholdType) {
	  case CVL_THRESHOLD:
	    ptr->bpvThreshold = threshold->stsAlarmThresholdValue;
		/* ptr->gv_BPVPeriod = threshold->stsAlarmThresholdTimeInterval; */
		break;

      case CVP_THRESHOLD:
	    ptr->crcThreshold = threshold->stsAlarmThresholdValue;
		/* ptr->gv_CRCPeriod = threshold->stsAlarmThresholdTimeInterval; */
		break;

      case CS_THRESHOLD:
	    ptr->csThreshold = threshold->stsAlarmThresholdValue;
		/* ptr->gv_CSPeriod = threshold->stsAlarmThresholdTimeInterval; */
		break;

	  case JITTER_THRESHOLD:
	    ptr->jitterThreshold = threshold->stsAlarmThresholdValue;
		/* ptr->gv_JITTERPeriod = threshold->stsAlarmThresholdTimeInterval; */
		break;

      case TIE_THRESHOLD:
	    ptr->tieThreshold = threshold->stsAlarmThresholdValue;
		/* ptr->gv_TIEPeriod = threshold->stsAlarmThresholdTimeInterval; */
		break;
     
	  case MTIE_THRESHOLD:
	    ptr->mtieThreshold = threshold->stsAlarmThresholdValue;
		/* ptr->gv_MTIEPeriod = threshold->stsAlarmThresholdTimeInterval; */
		break;

      case WANDER_THRESHOLD:
	    ptr->wanderThreshold = threshold->stsAlarmThresholdValue;
		break;

      default:
	    break;
    }

    if ((fhd = fopen("pmthres.dat", "w")) == NULL)
	{
	    printf("Failed to open pmthres.dat for writing\n");
		return ERROR;
	}

    for (ch=0; ch<MAX_FRAMERS; ch++)
	{
	    ptr = &pmFramer[ch];
	    tmp.bpvThreshold = ptr->bpvThreshold;
		tmp.crcThreshold = ptr->crcThreshold;
		tmp.csThreshold = ptr->csThreshold;
		tmp.jitterThreshold = ptr->jitterThreshold;
		tmp.tieThreshold = ptr->tieThreshold;
		tmp.mtieThreshold = ptr->mtieThreshold;
		tmp.wanderThreshold = ptr->wanderThreshold;
		tmp.bpvPeriod = ptr->gv_BPVPeriod;
		tmp.crcPeriod = ptr->gv_CRCPeriod;
		tmp.csPeriod = ptr->gv_CSPeriod;
		tmp.jitterPeriod = ptr->gv_JITTERPeriod;
		tmp.tiePeriod = ptr->gv_TIEPeriod;
		tmp.mtiePeriod = ptr->gv_MTIEPeriod;
		fwrite((char *)&tmp, sizeof(PM_THRESHOLD_t), 1, fhd);
	}
	fclose(fhd);

    return OK;
}

STATUS_t pmThresholdGet(stsAlarmThresholdEntry_t *threshold)
{
    U8_t ch;
    PM_FRAMER_t *ptr;

    ch = threshold->stsAlarmThresholdCurrentMon - 1;
	if (ch > (MAX_FRAMERS-1))
	{ 
	    return ERROR;
	}

	ptr = &pmFramer[ch];

    switch (threshold->stsAlarmThresholdType) {
	  case CVL_THRESHOLD:
	    threshold->stsAlarmThresholdValue = ptr->bpvThreshold;
		threshold->stsAlarmThresholdTimeInterval = ptr->gv_BPVPeriod;
		break;

      case CVP_THRESHOLD:
	    threshold->stsAlarmThresholdValue = ptr->crcThreshold;
		threshold->stsAlarmThresholdTimeInterval = ptr->gv_CRCPeriod;
		break;

      case CS_THRESHOLD:
	    threshold->stsAlarmThresholdValue = ptr->csThreshold;
		threshold->stsAlarmThresholdTimeInterval = ptr->gv_CSPeriod;
		break;

	  case JITTER_THRESHOLD:
	    threshold->stsAlarmThresholdValue = ptr->jitterThreshold;
		threshold->stsAlarmThresholdTimeInterval = ptr->gv_JITTERPeriod;
		break;

      case TIE_THRESHOLD:
	    threshold->stsAlarmThresholdValue = ptr->tieThreshold;
		threshold->stsAlarmThresholdTimeInterval = ptr->gv_TIEPeriod;
		break;
     
	  case MTIE_THRESHOLD:
	    threshold->stsAlarmThresholdValue = ptr->mtieThreshold;
		threshold->stsAlarmThresholdTimeInterval = ptr->gv_MTIEPeriod;
		break;

      case WANDER_THRESHOLD:
	    threshold->stsAlarmThresholdValue = ptr->wanderThreshold;
		break;

      default:
	    break;
    }

    return OK;
}

STATUS_t pmSeveritySet(stsLinkAlarmEntry_t *severity)
{
    U8_t ch;
    FILE *fhd = NULL;
	PM_SEVERITY_t tmp;

#if 0
printf("pmSeveritySet - Link: %d, SettingType: %d, Severity: %d\n", 
severity->stsLinkAlarmSettingLinkType, severity->stsLinkAlarmSettingType, severity->stsLinkAlarmSettingSevirity);
#endif

    ch = severity->stsLinkAlarmSettingLinkType - 1;

    switch (severity->stsLinkAlarmSettingType) {
	  case ACT_SEVERITY:
	    pmSeverity[ch].act = severity->stsLinkAlarmSettingSevirity;
		break;

      case LOS_SEVERITY:
	    pmSeverity[ch].los = severity->stsLinkAlarmSettingSevirity;
		break;

	  case AISP_SEVERITY:
	    pmSeverity[ch].aisp = severity->stsLinkAlarmSettingSevirity;
		break;

      case LOF_SEVERITY:
	    pmSeverity[ch].lof = severity->stsLinkAlarmSettingSevirity;
		break;

      case CVL_SEVERITY:
	    pmSeverity[ch].cvl = severity->stsLinkAlarmSettingSevirity;
		break;

      case CVP_SEVERITY:
	    pmSeverity[ch].cvp = severity->stsLinkAlarmSettingSevirity;
		break;

      case CS_SEVERITY:
	    pmSeverity[ch].act = severity->stsLinkAlarmSettingSevirity;
		break;

      case JITTER_SEVERITY:
	    pmSeverity[ch].jitter = severity->stsLinkAlarmSettingSevirity;
		break;

      case TIE_SEVERITY:
	    pmSeverity[ch].tie = severity->stsLinkAlarmSettingSevirity;
		break;

      case MTIE_SEVERITY:
	    pmSeverity[ch].mtie = severity->stsLinkAlarmSettingSevirity;
		break;

      case WANDER_SEVERITY:
	    pmSeverity[ch].wander = severity->stsLinkAlarmSettingSevirity;
		break;

      default:
	    break;
    }

    if ((fhd = fopen("pmseverity.dat", "w")) == NULL)
	{
	    printf("Failed to open pmseverity.dat for writing\n");
		return ERROR;
	}

    for (ch=0; ch<PM_LINK_TYPE; ch++)
	{
	    tmp.act = pmSeverity[ch].act;
		tmp.los = pmSeverity[ch].los;
		tmp.aisp = pmSeverity[ch].aisp;
		tmp.lof = pmSeverity[ch].lof;
		tmp.cvl = pmSeverity[ch].cvl;
		tmp.cvp = pmSeverity[ch].cvp;
		tmp.cs = pmSeverity[ch].cs;
		tmp.jitter = pmSeverity[ch].jitter;
		tmp.tie = pmSeverity[ch].tie;
		tmp.mtie = pmSeverity[ch].mtie;
		tmp.wander = pmSeverity[ch].wander;
		fwrite((char *)&tmp, sizeof(PM_SEVERITY_t), 1, fhd);
	}
	fclose(fhd);

    return OK;
}

STATUS_t pmSeverityGet(stsLinkAlarmEntry_t *severity)
{
    U8_t ch;
    
    ch = severity->stsLinkAlarmSettingLinkType - 1;

    switch (severity->stsLinkAlarmSettingType) {
	  case ACT_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].act;
		break;

      case LOS_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].los;
		break;

	  case AISP_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].aisp;
		break;

      case LOF_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].lof;
		break;

      case CVL_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].cvl;
		break;

      case CVP_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].cvp;
		break;

      case CS_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].act;
		break;

      case JITTER_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].jitter;
		break;

      case TIE_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].tie;
		break;

      case MTIE_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].mtie;
		break;

      case WANDER_SEVERITY:
	    severity->stsLinkAlarmSettingSevirity = pmSeverity[ch].wander;
		break;

      default:
	    break;
    }

#if 0
printf("pmSeverityGet - Link: %d, SettingType: %d, Severity: %d\n", 
severity->stsLinkAlarmSettingLinkType, severity->stsLinkAlarmSettingType, severity->stsLinkAlarmSettingSevirity);
#endif

    return OK;
}

STATUS_t pmPerfMonGet(stsPerfMonEntry_t *pmRec)
{
	U8_t ch, period, bucket, monType;
	PM_FRAMER_t *ptr;

    ch = pmRec->stsPerfMonInputCurrentMon - 1;
	period = pmRec->stsPerfMonInputMonPeriod;
	bucket = pmRec->stsPerfMonInputMonBucket;
	monType = pmRec->stsPerfMonInputMonType;
	ptr = &pmFramer[ch];

    switch (period) {
	  case PM_ONE_HUNDRED_SECOND:
	    if (bucket > (Bucket_100 - 2))
		    return ERROR;

		pmRec->stsPerfMonInputClockType = ptr->pmRec.sec_100[bucket].clkType;

		switch (monType) {
		  case T_BPV:
	        pmRec->stsPerfMonInputMonValue = ptr->pmRec.sec_100[bucket].eBPV;
			break;
		  case T_CRC: 
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.sec_100[bucket].eCRC;
			break;
		  case T_CS:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.sec_100[bucket].eCS;
			break;
		  case T_ESP:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.sec_100[bucket].eESP;
			break;
		  case T_SESP:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.sec_100[bucket].eSESP;
			break;
		  case T_JITTER:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.sec_100[bucket].eJITTER;
			break;
		  case T_TIE:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.sec_100[bucket].eTIE;
			break;
		  case T_MTIE:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.sec_100[bucket].eMTIE;
			break;
		  default:
		    return ERROR;
		}  
	    break;

      case PM_ONE_HOUR:
	    if (bucket > (Bucket_Hour - 2))
		    return ERROR;
		pmRec->stsPerfMonInputClockType = ptr->pmRec.hour[bucket].clkType;
		switch (monType) {
		  case T_BPV:
	        pmRec->stsPerfMonInputMonValue = ptr->pmRec.hour[bucket].eBPV;
			break;
		  case T_CRC: 
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.hour[bucket].eCRC;
			break;
		  case T_CS:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.hour[bucket].eCS;
			break;
		  case T_ESP:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.hour[bucket].eESP;
			break;
		  case T_SESP:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.hour[bucket].eSESP;
			break;
		  case T_JITTER:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.hour[bucket].eJITTER;
			break;
		  case T_TIE:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.hour[bucket].eTIE;
			break;
		  case T_MTIE:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.hour[bucket].eMTIE;
			break;
		  default:
		    return ERROR;
		}
	    break;

      case PM_ONE_DAY:
	    if (bucket > (Bucket_Day - 2))
		    return ERROR;
	    pmRec->stsPerfMonInputClockType = ptr->pmRec.day[bucket].clkType;
		switch (monType) {
		  case T_BPV:
	        pmRec->stsPerfMonInputMonValue = ptr->pmRec.day[bucket].eBPV;
			break;
		  case T_CRC: 
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.day[bucket].eCRC;
			break;
		  case T_CS:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.day[bucket].eCS;
			break;
		  case T_ESP:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.day[bucket].eESP;
			break;
		  case T_SESP:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.day[bucket].eSESP;
			break;
		  case T_JITTER:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.day[bucket].eJITTER;
			break;
		  case T_TIE:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.day[bucket].eTIE;
			break;
		  case T_MTIE:
		    pmRec->stsPerfMonInputMonValue = ptr->pmRec.day[bucket].eMTIE;
			break;
		  default:
		    return ERROR;
		}
	    break;

      default:
	    return ERROR;
	}

    return OK;
}

STATUS_t ManSmuTestSet(stsPerfMonManual_t *manTest)
{
#if 0
printf("----------ManSmuTestSet Entry valid: %x\n", (int)manTest->valid);
printf("InputSelect octet_ptr: %x\n", manTest->stsPerfMonManualInputSelect->octet_ptr[0]);
printf("ScanInterval: %d\n", manTest->stsPerfMonManualInputScanInterval);
printf("NumReport: %d\n", manTest->stsPerfMonManualInputNumReport);
printf("NumObservation: %d\n", manTest->stsPerfMonManualInputNumObservation);
#endif

	if (manTest->stsPerfMonManualInputAbortTest == MANUAL_PM_ABORT)
	{
	    abortSmuManualPM();
	    manSelect = 0;
	    manScanInterval = 0;
	    manNumReport = 0;
	    manNumObservation = 0;
		manSmuTestSetFlag = 0;

	    return OK;
    }

    if (manSmuTestSetFlag)
	    return ERROR;

    if ((manTest->stsPerfMonManualInputSelect->octet_ptr[0] == 0) ||
		(manTest->stsPerfMonManualInputSelect->octet_ptr[0] > 0xF8))
		return ERROR; 

    manSelect |= (U8_t)manTest->stsPerfMonManualInputSelect->octet_ptr[0];
	manScanInterval = manTest->stsPerfMonManualInputScanInterval;
	manNumReport = manTest->stsPerfMonManualInputNumReport;
	manNumObservation = manTest->stsPerfMonManualInputNumObservation;
	
    startSmuManualPM(); 

    return OK;
}    

STATUS_t ManSmuTestGet(stsPerfMonManual_t *manTest)
{
    memset(pmTmpBuf, 0, sizeof(pmTmpBuf));
    pmTmpBuf[0] = manSelect;
	pmTmp.octet_ptr = &pmTmpBuf[0];
	pmTmp.length = 1;
    manTest->stsPerfMonManualInputSelect = &pmTmp;   

    manTest->stsPerfMonManualInputScanInterval = manScanInterval;
    manTest->stsPerfMonManualInputNumReport = manNumReport;
    manTest->stsPerfMonManualInputNumObservation = manNumObservation;

#if 0
printf("InputSelect octet_ptr: %x\n", manTest->stsPerfMonManualInputSelect->octet_ptr[0]);
printf("ScanInterval: %d\n", manTest->stsPerfMonManualInputScanInterval);
printf("NumReport: %d\n", manTest->stsPerfMonManualInputNumReport);
printf("NumObservation: %d\n", manTest->stsPerfMonManualInputNumObservation);
printf("AbortTest: %d\n", manTest->stsPerfMonManualInputAbortTest);
#endif

    return OK;
}
