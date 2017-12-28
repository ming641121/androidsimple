#include <stdlib.h>
#include <stdio.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <axss_types.h>
#include <ds26502.h>
#include <pm.h>

U32_t       stopTIE_Def[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0104,
                                        SMU_FPGA_BASE_ADDR + 0x010B,
                                        SMU_FPGA_BASE_ADDR + 0x0122,
									    SMU_FPGA_BASE_ADDR + 0x0129,
									    SMU_FPGA_BASE_ADDR + 0x0130};

U32_t       getTIE_Def[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x001C,
                                       SMU_FPGA_BASE_ADDR + 0x0048,
                                       SMU_FPGA_BASE_ADDR + 0x0074,
			 			    		   SMU_FPGA_BASE_ADDR + 0x00A0,
		     					       SMU_FPGA_BASE_ADDR + 0x00CC};

U32_t       stopMTIE_Def[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0105,
                                         SMU_FPGA_BASE_ADDR + 0x010C,
                                         SMU_FPGA_BASE_ADDR + 0x0123,
				    		             SMU_FPGA_BASE_ADDR + 0x012A,
									     SMU_FPGA_BASE_ADDR + 0x0131};

U32_t       getMTIEMax_Def[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0020,
				    					   SMU_FPGA_BASE_ADDR + 0x004C,
									       SMU_FPGA_BASE_ADDR + 0x0078,
									       SMU_FPGA_BASE_ADDR + 0x00A4,
									       SMU_FPGA_BASE_ADDR + 0x00D0};

U32_t       getMTIEMin_Def[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0024,
				    					   SMU_FPGA_BASE_ADDR + 0x0050,
					 				       SMU_FPGA_BASE_ADDR + 0x007C,
									       SMU_FPGA_BASE_ADDR + 0x00A8,
									       SMU_FPGA_BASE_ADDR + 0x00D4};

U32_t       stopJITTER_Def[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0106,
                                           SMU_FPGA_BASE_ADDR + 0x010D,
                                           SMU_FPGA_BASE_ADDR + 0x0124,
				    	    	           SMU_FPGA_BASE_ADDR + 0x012B,
									       SMU_FPGA_BASE_ADDR + 0x0132};

U32_t       getJITTERMax_Def[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0028,
									         SMU_FPGA_BASE_ADDR + 0x0054,
									         SMU_FPGA_BASE_ADDR + 0x0080,
									         SMU_FPGA_BASE_ADDR + 0x00AC,
									         SMU_FPGA_BASE_ADDR + 0x00D8};

U32_t       getJITTERMin_Def[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x002C,
									         SMU_FPGA_BASE_ADDR + 0x0058,
									         SMU_FPGA_BASE_ADDR + 0x0084,
									         SMU_FPGA_BASE_ADDR + 0x00B0,
									         SMU_FPGA_BASE_ADDR + 0x00DC};

U32_t       resetPhase_Def[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x005C,
                                           SMU_FPGA_BASE_ADDR + 0x009C,
                                           SMU_FPGA_BASE_ADDR + 0x00DC,
									       SMU_FPGA_BASE_ADDR + 0x011C,
									       SMU_FPGA_BASE_ADDR + 0x015C};

S32_t         oneSecondJitter;
U8_t          gv_startVLDTY_ADJ;
U8_t          gv_CLKSEL_FROM_OUTPUT;
U8_t          gv_ClkcelShadow;
U8_t          gv_CLKSEL_Setting;
PM_FRAMER_t   pmFramer[MAX_FRAMERS];
PM_SEVERITY_t pmSeverity[LINK_TYPES];

void     initDefPM(int ch);
void     initThreshold(void);
void     initSeverity(void);
int      smuPMTask(void);
void     procOneHundredSecondPM(int ch);
STATUS_t updateJitter(int ch);
void     update1SecondPhaseData(int ch);
void     readTIEData(int ch, char mode);
void     readMTIEData(int ch, char mode);
S32_t    readJitterData(int ch, char mode);
void     capture1SecondError(int ch);
void     shiftPMBucket(int ch, int mode);

void initDefPM(int ch)
{
    PM_FRAMER_t *ptr = &pmFramer[ch];

    if (inpFramer[ch].status == FRAMER_IN_SERVICE)
	{
	    /* reset Default phase detector */
	    *(U8_t *)resetPhase_Def[ch] = 1;
	    SHORT_DELAY;

        ptr->maxMTIE = 0;
	    ptr->minMTIE = 0;
	    readTIEData(ch, DEFAULT_PM);
	    ptr->tieStartLongValue.l = ptr->tieEndLongValue.l;
	    ptr->tiePM = 0;
	    ptr->pmJitter = 0;
	    ptr->alarmCRCcount = 0;
	    ptr->alarmBPVcount = 0;
	    ptr->alarmCScount = 0;
		ptr->pmRec.secElapsed = 0;
		ptr->status = TRUE;
	}
	else
	    memset((void *)&pmFramer[ch], sizeof(PM_FRAMER_t), 0);
}

void initThreshold()
{
    int i;
	PM_FRAMER_t *ptr;
   	FILE *fhd = NULL;
	PM_THRESHOLD_t tmp;

    if ((fhd = fopen("pmthres.dat", "r")) == NULL)
	{
        for (i=0; i<MAX_FRAMERS; i++ )
        {
	        ptr = (PM_FRAMER_t *)&pmFramer[i];
			ptr->bpvThreshold = BPV_DEFAULT;
            ptr->crcThreshold = CRC_DEFAULT;
            ptr->mtieThreshold = MTIE_DEFAULT; 
            ptr->tieThreshold = TIE_DEFAULT; 
            ptr->jitterThreshold = JITTER_DEFAULT;
            ptr->csThreshold = CS_DEFAULT; 
			ptr->gv_TIEPeriod = ONE_HOUR;
            ptr->gv_MTIEPeriod = ONE_HOUR;
            ptr->gv_CSPeriod = ONE_HOUR;
            ptr->gv_JITTERPeriod = ONE_SECOND;
            ptr->gv_CRCPeriod = TEN_SECONDS;
            ptr->gv_BPVPeriod = TEN_SECONDS;
		}
    }
	else
	{
	    for (i=0; i<MAX_FRAMERS; i++ )
        {
		    fread((char *)&tmp, sizeof(PM_THRESHOLD_t), 1, fhd);
		    ptr = (PM_FRAMER_t *)&pmFramer[i];
			ptr->bpvThreshold = tmp.bpvThreshold;
		    ptr->crcThreshold = tmp.crcThreshold;
		    ptr->csThreshold = tmp.csThreshold;
		    ptr->jitterThreshold = tmp.jitterThreshold;
		    ptr->tieThreshold = tmp.tieThreshold;
		    ptr->mtieThreshold = tmp.mtieThreshold;
		    ptr->wanderThreshold = tmp.wanderThreshold;

            ptr->gv_BPVPeriod = tmp.bpvPeriod;
		    ptr->gv_CRCPeriod = tmp.crcPeriod;
		    ptr->gv_CSPeriod = tmp.csPeriod;
		    ptr->gv_JITTERPeriod = tmp.jitterPeriod;
		    ptr->gv_TIEPeriod = tmp.tiePeriod;
		    ptr->gv_MTIEPeriod = tmp.mtiePeriod;
		}
	    fclose(fhd);
	}
}

void initSeverity()
{
    int i;
	FILE *fhd = NULL;
	PM_SEVERITY_t tmp;

    if ((fhd = fopen("pmseverity.dat", "r")) == NULL)
	{
        for (i=0; i<PM_LINK_TYPE; i++ )
        {
		    pmSeverity[i].act = PM_NON_REPORT;
			pmSeverity[i].los = PM_MAJOR;
			pmSeverity[i].aisp = PM_MINOR;
			pmSeverity[i].lof = PM_MAJOR;
			pmSeverity[i].cvl = PM_MINOR;
			pmSeverity[i].cvp = PM_MINOR;
			pmSeverity[i].cs = PM_MINOR;
			pmSeverity[i].jitter = PM_MINOR;
			pmSeverity[i].tie = PM_MINOR;
			pmSeverity[i].mtie = PM_MINOR;
			pmSeverity[i].wander = PM_MINOR;
		}
    }
	else
	{
	    for (i=0; i<PM_LINK_TYPE; i++ )
        {
		    fread((char *)&tmp, sizeof(PM_SEVERITY_t), 1, fhd);
		    pmSeverity[i].act = tmp.act;
			pmSeverity[i].los = tmp.los;
			pmSeverity[i].aisp = tmp.aisp;
			pmSeverity[i].lof = tmp.lof;
			pmSeverity[i].cvl = tmp.cvl;
			pmSeverity[i].cvp = tmp.cvp;
			pmSeverity[i].cs = tmp.cs;
			pmSeverity[i].jitter = tmp.jitter;
			pmSeverity[i].tie = tmp.tie;
			pmSeverity[i].mtie = tmp.mtie;
			pmSeverity[i].wander = tmp.wander;
		}
	    fclose(fhd);
	}
}

int smuPMTask()
{
    int ch, i;
	
#if SMU
    /* If SMU is not present, suspend the task */
    if (!SMUPresent())
	    taskSuspend(0);
#endif

    /* reset PM data structure */
    memset((void *)&pmFramer[0], (sizeof(PM_FRAMER_t) * MAX_FRAMERS), 0);

    /* initialize the Threshold */
    initThreshold();

    /* initialize severity */
	initSeverity();

	for (i=0; i<MAX_FRAMERS; i++)
	{
	    initDefPM(i);
	}
  
	for (;;)
	{
        for (ch=0; ch<MAX_FRAMERS; ch++)
        {
		    if ((inpFramer[ch].status == FRAMER_IN_SERVICE) &&
			    /* (inpFramer[ch].linkEnable == SCAN_ENABLE) && */
			    (pmFramer[ch].status == TRUE))
            {
                updateJitter(ch);
                update1SecondPhaseData(ch);
				capture1SecondError(ch);
                updateCodeThresholdAlarm(ch);

			    if (pmFramer[ch].pmRec.secElapsed++ >= 99)
			    {
                    procOneHundredSecondPM(ch);
					initDefPM(ch);

                    /* init next ready channel for monitoring */
					if (ch < MAX_FRAMERS-1)
					{
                        if (pmFramer[ch+1].status == PM_READY)
						{
					        initDefPM(ch+1);
							ch++;
						}
					}
					else
					{
					    if (pmFramer[0].status == PM_READY)
					        initDefPM(0);
					}
			    }
            }
			else if ((inpFramer[ch].status == FRAMER_IN_SERVICE) &&
			         (pmFramer[ch].status == FALSE))
			{
			    pmFramer[ch].status = PM_READY;
			}
		    else if (inpFramer[ch].status != FRAMER_IN_SERVICE)  
		    {
		        /* Framer is out of service */
				pmFramer[ch].status = FALSE;
		    }
        }

        taskDelay(ONE_SECOND);
	} 
}

void procOneHundredSecondPM(int ch)
{
    PM_FRAMER_t *ptr = &pmFramer[ch];
	S32_t mtie, tie;

#if 0
    tie = ptr->tieEndLongValue.l - ptr->tieStartLongValue.l;
#else
    tie = ptr->tiePM;
#endif
	mtie = abs(ptr->maxMTIE + ptr->minMTIE ); 
	/* make sure that TIE is not greater than MTIE */
	if (abs(tie) > mtie)
	    tie = (tie/abs(tie))*mtie;
    ptr->pmRec.sec_100[0].eJITTER /=  UI_Nsec;
	ptr->pmRec.sec_100[0].eCS = ( mtie / UI_Nsec ) / SLIP_DEFAULT;

#if JIM
    ptrint = (U32_t * )&ptr->pm.jitter;
    *ptrint = (U32_t)((ptr->pmJitter % UI_Nsec) * 100 / UI_Nsec);
    swap(ptrint);
#endif

    ptr->pmRec.sec_100[0].eMTIE = mtie; 
	ptr->pmRec.sec_100[0].eTIE = tie;

	/* make sure that TIE is not greater than MTIE */
	if (abs(tie) > mtie)
	    ptr->pmRec.sec_100[0].eTIE = (tie/abs(tie))*mtie;

#if 0
    ptr->pmRec.TIETime++;
#endif

    ptr->pmRec.sec_100[0].clkType = te1Cfg[ch].lineType;     
   	ptr->pmRec.sec_100[0].validity = TRUE;

	/* update to Hourly PM Bucket */
    if (ptr->pmRec.sec_100[0].eJITTER > ptr->pmRec.hour[0].eJITTER)
        ptr->pmRec.hour[0].eJITTER = ptr->pmRec.sec_100[0].eJITTER; 
	if (ptr->pmRec.sec_100[0].eTIE > ptr->pmRec.hour[0].eTIE)
	    ptr->pmRec.hour[0].eTIE = ptr->pmRec.sec_100[0].eTIE;
	if (ptr->pmRec.sec_100[0].eMTIE > ptr->pmRec.hour[0].eMTIE)
	    ptr->pmRec.hour[0].eMTIE = ptr->pmRec.sec_100[0].eMTIE;
	ptr->pmRec.hour[0].eCS += ptr->pmRec.sec_100[0].eCS;
	ptr->pmRec.hour[0].eBPV += ptr->pmRec.sec_100[0].eBPV;
	ptr->pmRec.hour[0].eCRC += ptr->pmRec.sec_100[0].eCRC;
	ptr->pmRec.hour[0].eESP += ptr->pmRec.sec_100[0].eESP;
	ptr->pmRec.hour[0].eSESP += ptr->pmRec.sec_100[0].eSESP;
	ptr->pmRec.hour[0].clkType = ptr->pmRec.sec_100[0].clkType;

     /* Shift 100 Seconds PM Bucket */
	 shiftPMBucket(ch, SEC100_PM);

	if (ptr->pmRec.sec100Elapsed++ > (Bucket_100 - 2))
	{
	    /* Update Daily PM Bucket */
      	if (ptr->pmRec.hour[0].eJITTER > ptr->pmRec.day[0].eJITTER)
            ptr->pmRec.day[0].eJITTER = ptr->pmRec.hour[0].eJITTER; 
	    if (ptr->pmRec.hour[0].eTIE > ptr->pmRec.day[0].eTIE)
            ptr->pmRec.day[0].eTIE = ptr->pmRec.hour[0].eTIE;
	    if (ptr->pmRec.hour[0].eMTIE > ptr->pmRec.day[0].eMTIE)
            ptr->pmRec.day[0].eMTIE = ptr->pmRec.hour[0].eMTIE;
	    ptr->pmRec.day[0].eCS += ptr->pmRec.hour[0].eCS;
	    ptr->pmRec.day[0].eBPV += ptr->pmRec.hour[0].eBPV;
	    ptr->pmRec.day[0].eCRC += ptr->pmRec.hour[0].eCRC;
	    ptr->pmRec.day[0].eESP += ptr->pmRec.hour[0].eESP;
	    ptr->pmRec.day[0].eSESP += ptr->pmRec.hour[0].eSESP;
		ptr->pmRec.day[0].clkType = ptr->pmRec.hour[0].clkType;

	    /* Shift Hourly Bucket */
	    ptr->pmRec.hour[0].validity = TRUE;
	    shiftPMBucket(ch, HOURLY_PM);

		if (ptr->pmRec.hourElapsed++ > (Bucket_Hour - 2))
		{
		    /* Shift Daily Bucket */
		    ptr->pmRec.day[0].validity = TRUE;
		    shiftPMBucket(ch, DAILY_PM);
        }
	}
}

STATUS_t updateJitter(int ch)
{
    PM_FRAMER_t *ptr = &pmFramer[ch];

    ptr->oneSecondJitter = readJitterData(ch, DEFAULT_PM);

    /* update Jitter */
    if (ptr->oneSecondJitter > ptr->pmJitter)
        ptr->pmJitter = ptr->oneSecondJitter;
	if (ptr->pmJitter > ptr->pmRec.sec_100[0].eJITTER)
	    ptr->pmRec.sec_100[0].eJITTER = ptr->pmJitter;

    if (ptr->pmJitter > ptr->jitterThreshold)
    {
        if (ptr->jitterStatus == OK)
           ptr->jitterStatus = ERROR;
        else if (ptr->jitterStatus != OK)
        {
            if (ptr->jitterAlarm != ON)
            {
                /* turn on Jitter Alarm */
                setInpFramerAlarm(TE1_ALARM_JITTER, TE1_ALARM_SET, ch);
                ptr->jitterAlarm = ON;
            }
        }
    }
    else
    {
        ptr->jitterStatus = OK;
        if (ptr->jitterAlarm == ON)
        {
            /* clear Jitter Alarm */
            setInpFramerAlarm(TE1_ALARM_JITTER, TE1_ALARM_CLEAR, ch);
            ptr->jitterAlarm = OFF;
        }
    }

    return OK;
}

void update1SecondPhaseData(int ch)
{
    PM_FRAMER_t *ptr = &pmFramer[ch];

    readMTIEData(ch, DEFAULT_PM);
	readTIEData(ch, DEFAULT_PM);

    ptr->gv_TIEData = abs(ptr->tieEndLongValue.l - ptr->tieStartLongValue.l);
	ptr->tiePM += ptr->tieEndLongValue.l - ptr->tieStartLongValue.l;

#if 0
    if (resetTIEFlag[ch])
	    ptr->tieStartLongValue.l = 0
	else
	    ptr->tieStartLongValue.l = ptr->tieEndLongValue.l;
#endif

	ptr->pmRec.sec_100[0].eCS += (((S64_t)abs( ptr->gv_TIEData ) ) / UI_Nsec ) / SLIP_DEFAULT;

    procTIEAlarm(ch); 
    procMTIEAlarm(ch);
	procCSAlarm(ch);

#if 0
	procFRAME_SLIPAlarm(ch); /* Default mode does not check coarse data */
#endif
}

void readTIEData(int ch, char mode)
{
    U8_t *tiePtr;

	if (mode == DEFAULT_PM)
 	{
        PM_FRAMER_t *ptr = &pmFramer[ch];

		/* update TIE register */
	    *(U8_t *)stopTIE_Def[ch] = 1;

        /* point to TIE register for reading */
        tiePtr = (U8_t *)getTIE_Def[ch];

        /* wait for 16us */
	    SHORT_DELAY;
	
	    ptr->tieEndLongValue.ch.hh = *(U8_t *)(tiePtr); 
		ptr->tieEndLongValue.ch.hm = *(U8_t *)(tiePtr + 1);
        ptr->tieEndLongValue.ch.lm = *(U8_t *)(tiePtr + 2);
        ptr->tieEndLongValue.ch.ll = *(U8_t *)(tiePtr + 3);
	    ptr->tieEndLongValue.l >>= 1;
	}
	else
	{   
	    PM_MANUAL_t *ptr = &pmManual[ch];

        /* update TIE register */
	    *(U8_t *)stopTIE_Man[ch] = 1;
	    
	    /* point to TIE register for reading */
        tiePtr = (U8_t *)getTIE_Man[ch];

        /* wait for 16us */
	    SHORT_DELAY;
	
	    ptr->tieEndLongValue.ch.hh = *(U8_t *)(tiePtr); 
		ptr->tieEndLongValue.ch.hm = *(U8_t *)(tiePtr + 1);
        ptr->tieEndLongValue.ch.lm = *(U8_t *)(tiePtr + 2);
        ptr->tieEndLongValue.ch.ll = *(U8_t *)(tiePtr + 3);
	    ptr->tieEndLongValue.l >>= 1;
    }
}

void readMTIEData(int ch, char mode)
{
	U8_t *mtieMaxPtr, *mtieMinPtr;
	union u_ulong mtieMaxLongValue, mtieMinLongValue;
    
	if (mode == DEFAULT_PM)
 	{ 
		/* update MTIE register */
	    *(U8_t *)stopMTIE_Def[ch] = 1;

        /* point to MTIE register for reading */
        mtieMaxPtr = (U8_t *)getMTIEMax_Def[ch];
		mtieMinPtr = (U8_t *)getMTIEMin_Def[ch];
	}
	else
	{   
        /* update MTIE register */
	    *(U8_t *)stopMTIE_Man[ch] = 1;
	    
	    /* point to MTIE register for reading */
        mtieMaxPtr = (U8_t *)getMTIEMax_Man[ch];
		mtieMinPtr = (U8_t *)getMTIEMin_Man[ch];
    }

	/* wait for 16us */
	SHORT_DELAY;

	/* read MTIE Max Data */
	mtieMaxLongValue.ch.hh = *(U8_t *)(mtieMaxPtr);
	mtieMaxLongValue.ch.hm = *(U8_t *)(mtieMaxPtr + 1);
    mtieMaxLongValue.ch.lm = *(U8_t *)(mtieMaxPtr + 2);
    mtieMaxLongValue.ch.ll = *(U8_t *)(mtieMaxPtr + 3);

	/* read MTIE Min Data */
	mtieMinLongValue.ch.hh = *(U8_t *)(mtieMinPtr);
	mtieMinLongValue.ch.hm = *(U8_t *)(mtieMinPtr + 1);
    mtieMinLongValue.ch.lm = *(U8_t *)(mtieMinPtr + 2);
    mtieMinLongValue.ch.ll = *(U8_t *)(mtieMinPtr + 3);

    mtieMaxLongValue.ul >>= 1;
	mtieMinLongValue.ul >>= 1;

    if (mode == DEFAULT_PM)
 	{ 
        PM_FRAMER_t *ptr = &pmFramer[ch];

		if (mtieMaxLongValue.ul > ptr->maxMTIE)
            ptr->maxMTIE = mtieMaxLongValue.ul;
        if (mtieMinLongValue.ul > ptr->minMTIE)
            ptr->minMTIE = mtieMinLongValue.ul;
	}
	else
	{
		PM_MANUAL_t *ptr = &pmManual[ch];

        if (mtieMaxLongValue.ul > ptr->maxMTIE)
            ptr->maxMTIE = mtieMaxLongValue.ul;
        if (mtieMinLongValue.ul > ptr->minMTIE)
            ptr->minMTIE = mtieMinLongValue.ul;
	}
}

S32_t readJitterData(int ch, char mode)
{
    U8_t *jitterMaxPtr, *jitterMinPtr;
	union u_ulong jitterMaxLongValue, jitterMinLongValue;

    if (mode == DEFAULT_PM)
 	{ 
		/* update JITTER register */
	    *(U8_t *)stopJITTER_Def[ch] = 1;

        /* point to JITTER register for reading */
        jitterMaxPtr = (U8_t *)getJITTERMax_Def[ch];
		jitterMinPtr = (U8_t *)getJITTERMin_Def[ch];
	}
	else
	{   
        /* update JITTER register */
	    *(U8_t *)stopJITTER_Man[ch] = 1;
	    
	    /* point to JITTER register for reading */
		jitterMaxPtr = (U8_t *)getJITTERMax_Man[ch];
		jitterMinPtr = (U8_t *)getJITTERMin_Man[ch];
    }

	/* wait for 16us */
	SHORT_DELAY;

	/* read JITTER Max Data */
	jitterMaxLongValue.ch.hh = *(U8_t *)(jitterMaxPtr);
	jitterMaxLongValue.ch.hm = *(U8_t *)(jitterMaxPtr + 1);
    jitterMaxLongValue.ch.lm = *(U8_t *)(jitterMaxPtr + 2);
    jitterMaxLongValue.ch.ll = *(U8_t *)(jitterMaxPtr + 3);
 
	/* read JITTER Min Data */
	jitterMinLongValue.ch.hh = *(U8_t *)(jitterMinPtr);
	jitterMinLongValue.ch.hm = *(U8_t *)(jitterMinPtr + 1);
    jitterMinLongValue.ch.lm = *(U8_t *)(jitterMinPtr + 2);
    jitterMinLongValue.ch.ll = *(U8_t *)(jitterMinPtr + 3);

    return ((jitterMaxLongValue.ul + jitterMinLongValue.ul) >> 1);
}

void capture1SecondError(int ch)
{ 
    PM_FRAMER_t *ptr = &pmFramer[ch];

#if TMP
    ptr->alarmBPVcount = inpFramer[ch].bvCountDef;
	ptr->alarmCRCcount = inpFramer[ch].crcCountDef;
	inpFramer[ch].bvCountDef = 0;
	inpFramer[ch].crcCountDef = 0;
#endif

	ptr->pmRec.sec_100[0].eBPV += ptr->alarmBPVcount;
	ptr->pmRec.sec_100[0].eCRC += ptr->alarmCRCcount;
   
    if ((te1Cfg[ch].lineType == LT_E1) &&
	    (ptr->alarmCRCcount > 805))
	   ptr->pmRec.sec_100[0].eSESP++;
	else if ((te1Cfg[ch].lineType == LT_T1) &&
	         (ptr->alarmCRCcount > 320))
       ptr->pmRec.sec_100[0].eSESP++;

    if ( ptr->alarmCRCcount >= 1 )
    {
		ptr->pmRec.sec_100[0].eESP++;
    }
}

void shiftPMBucket(int ch, int mode)
{
    PM_FRAMER_t *ptr = &pmFramer[ch];
	int i;

    switch (mode) {
	  case SEC100_PM:
	     for (i=Bucket_100-1; i>0; i--)
		     memcpy(&ptr->pmRec.sec_100[i], &ptr->pmRec.sec_100[i-1], sizeof(PMPara_t));  
		 memset(&ptr->pmRec.sec_100[0], sizeof(PMPara_t), 0);
		 break;
		
	  case HOURLY_PM:
	     for (i=Bucket_Hour-1; i>0; i--)
		     memcpy(&ptr->pmRec.hour[i], &ptr->pmRec.hour[i-1], sizeof(PMPara_t));  
		 memset(&ptr->pmRec.hour[0], sizeof(PMPara_t), 0);
		 break;
    
	  case DAILY_PM:
	     for (i=Bucket_Day-1; i>0; i--)
		     memcpy(&ptr->pmRec.day[i], &ptr->pmRec.day[i-1], sizeof(PMPara_t));  
		 memset(&ptr->pmRec.day[0], sizeof(PMPara_t), 0);
		 break;

      default:
	     printf("shiftDataBocket: wrong mode: %d\n", mode);
		 break;
	}
}





