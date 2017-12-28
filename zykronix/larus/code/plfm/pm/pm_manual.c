#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <axss_types.h>
#include <siw_if.h>
#include <ds26502.h>
#include <pm.h>

char manTestDate[DATESTAMP_SIZE];
char manTestTime[TIMESTAMP_SIZE];

U32_t       stopTIE_Man[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0101,
                                        SMU_FPGA_BASE_ADDR + 0x0108,
                                        SMU_FPGA_BASE_ADDR + 0x011F,
									    SMU_FPGA_BASE_ADDR + 0x0126,
									    SMU_FPGA_BASE_ADDR + 0x012D};

U32_t       getTIE_Man[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0008,
                                       SMU_FPGA_BASE_ADDR + 0x0034,
                                       SMU_FPGA_BASE_ADDR + 0x0060,
			 			    		   SMU_FPGA_BASE_ADDR + 0x0080,
		     					       SMU_FPGA_BASE_ADDR + 0x00B8};

U32_t       stopMTIE_Man[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0102,
                                         SMU_FPGA_BASE_ADDR + 0x0109,
                                         SMU_FPGA_BASE_ADDR + 0x0120,
				    		             SMU_FPGA_BASE_ADDR + 0x0127,
									     SMU_FPGA_BASE_ADDR + 0x012E};

U32_t       getMTIEMax_Man[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x000c,
				    					   SMU_FPGA_BASE_ADDR + 0x0038,
									       SMU_FPGA_BASE_ADDR + 0x0064,
									       SMU_FPGA_BASE_ADDR + 0x0090,
									       SMU_FPGA_BASE_ADDR + 0x00BC};

U32_t       getMTIEMin_Man[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0010,
				    					   SMU_FPGA_BASE_ADDR + 0x003C,
					 				       SMU_FPGA_BASE_ADDR + 0x0068,
									       SMU_FPGA_BASE_ADDR + 0x0094,
									       SMU_FPGA_BASE_ADDR + 0x00C0};

U32_t       stopJITTER_Man[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0103,
                                           SMU_FPGA_BASE_ADDR + 0x010A,
                                           SMU_FPGA_BASE_ADDR + 0x0121,
				    	    	           SMU_FPGA_BASE_ADDR + 0x0128,
									       SMU_FPGA_BASE_ADDR + 0x012F};

U32_t       getJITTERMax_Man[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0014,
									         SMU_FPGA_BASE_ADDR + 0x0040,
									         SMU_FPGA_BASE_ADDR + 0x006C,
									         SMU_FPGA_BASE_ADDR + 0x0098,
									         SMU_FPGA_BASE_ADDR + 0x00C4};

U32_t       getJITTERMin_Man[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0018,
									         SMU_FPGA_BASE_ADDR + 0x0044,
									         SMU_FPGA_BASE_ADDR + 0x0070,
									         SMU_FPGA_BASE_ADDR + 0x009C,
									         SMU_FPGA_BASE_ADDR + 0x00C8};

U32_t       resetPhase_Man[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0100,
                                           SMU_FPGA_BASE_ADDR + 0x0107,
                                           SMU_FPGA_BASE_ADDR + 0x011E,
									       SMU_FPGA_BASE_ADDR + 0x0125,
									       SMU_FPGA_BASE_ADDR + 0x012C};

U32_t       getRawPhase[MAX_FRAMERS] = {SMU_FPGA_BASE_ADDR + 0x0004,
									    SMU_FPGA_BASE_ADDR + 0x0030,
									    SMU_FPGA_BASE_ADDR + 0x005C,
									    SMU_FPGA_BASE_ADDR + 0x0088,
									    SMU_FPGA_BASE_ADDR + 0x00B4};

PM_MANUAL_t pmManual[MAX_FRAMERS];  
int         manPMTaskId = 0;
STATUS_t    resetMTIEFlag_MANUAL[MAX_FRAMERS];
U32_t       resetMTIEMaxData_MANUAL[MAX_FRAMERS];
U32_t       resetMTIEMinData_MANUAL[MAX_FRAMERS];
STATUS_t    startScanLoop = FALSE;
int         reptCount = 0;
STATUS_t    manReptStatus = FALSE;
U32_t       manRptCount = 1;

STATUS_t startSmuManualPM(void);
STATUS_t abortSmuManualPM(void);
void initSmuManualPM(int ch, S8_t interval);
int smuManPMTask(void);
STATUS_t procOneSecondManualPM(int ch);
void captureRawData(int ch);
void readRawData(int ch);
void reportPM(int ch);

void pmDelayTimeTest(int delay, int n);

STATUS_t startSmuManualPM()
{
    if (manPMTaskId == 0)
	{
	    if ((manPMTaskId = taskSpawn("smuManualPM", 70, VX_FP_TASK, 0x5000/* 20KB */, 
	                             smuManPMTask, 0,0,0,0,0,0,0,0,0,0)) == ERROR)
	    {
	        printf("Failed to create manPMTask\n");
		    manPMTaskId = 0;

	        return ERROR;
		}
	}
	
	return OK;
}

STATUS_t abortSmuManualPM()
{
    if (manPMTaskId > 0)
	{
	    if (taskDelete(manPMTaskId) == ERROR)
		{
#ifdef MAN_PM_DEBUG
		    printf("Failed to delete tid: %x\n", manPMTaskId); 
#endif
			return ERROR;
		}
		else
		{
	        manPMTaskId = 0;
#ifdef MAN_PM_DEBUG
			printf("Deleted tid: %x\n", manPMTaskId);		
#endif
    		return OK;
		}
	}
	else
	{
#ifdef MAN_PM_DEBUG
	    printf("Invalid tid: %x\n", manPMTaskId);
#endif
	    return ERROR;
	}
}

void initSmuManualPM(int ch, S8_t interval)
{
    PM_MANUAL_t *ptr = &pmManual[ch];
	struct tm  curtm;
    
    /* reset Manual phase detector */
	*(U8_t *)resetPhase_Man[ch] = 1;
	SHORT_DELAY;

    /* initialization */
	memset(ptr, 0, sizeof(PM_MANUAL_t));
	 						  
	ptr->interval = interval;
	ptr->status = MANUAL_PM_ON;
	ptr->maxMTIE = 0;
	ptr->minMTIE = 0;
	readTIEData(ch, MANUAL_PM);
	ptr->tieStartLongValue.l = ptr->tieEndLongValue.l;
	ptr->tiePM = 0;
    ptr->eBPV = 0;
	ptr->eCRC = 0;
	ptr->eESP = 0;
	ptr->eSESP = 0;
	ptr->eCS = 0;
	ptr->eJITTER = 0;
	ptr->eTIE = 0;
	ptr->eMTIE = 0;
	ptr->eWANDER5 = 0;
	ptr->eWANDER24 = 0;
	resetMTIEFlag_MANUAL[ch] = FALSE;
	resetMTIEMaxData_MANUAL[ch] = 0;
	resetMTIEMinData_MANUAL[ch] = 0;
	startScanLoop = TRUE;

    /* get the current system time */
    SiwDateTimeGet(&curtm);
	strftime(manTestDate, DATESTAMP_SIZE-1, "%m-%d-%Y", &curtm);
    strftime(manTestTime, TIMESTAMP_SIZE-1, "%H:%M:%S", &curtm);
}

int smuManPMTask()
{
#if 0
   	PM_MANUAL_t *ptr;
	int waitTime, ch;
#endif
	int n, iterations;

    taskDelay(100);
	manSmuTestSetFlag = 1;

	for (n=0; n<FRAMER_ALL; n++)
	{
	   	if ((manSelect & (1 << n)) >> n)
		{
			/* initialization */
	        initSmuManualPM(n, manScanInterval);
	    }
	}	

	iterations = manNumReport * manNumObservation;

	if (iterations == 0)
	    return OK;

	reptCount = 0;
	manRptCount = 1;

	for (;;)
	{
#if 0
	    if (reptCount >= iterations)
		    break;

	    for (ch=0; ch<MAX_FRAMERS; ch++)
	    {
	   	    if ((scanChannel & (1 << ch)) >> ch)
		    {
			    ptr = &pmManual[ch];

                if (startScanLoop)
				    ptr->status = MANUAL_PM_ON;
#if 0
   	            if ((inpFramer[ch].status == FRAMER_IN_SERVICE) &&
				    (ptr->status == MANUAL_PM_ON))
#endif
                if (ptr->status == MANUAL_PM_ON)
			    {
				    captureRawData(ch);
	                ptr->loopCount++;

	                if (ptr->loopCount >= RAW_PHASE_BUF)
	                {
		                if (procOneSecondManualPM(ch))
						    manReptStatus = TRUE;
						ptr->loopCount = 0;
						captureRawData(ch);
	                    ptr->loopCount = 1;
						waitTime = TICK_3;
	                }
	                else if (ptr->loopCount < RAW_PHASE_BUF - 4)
		                waitTime = TICK_3;
	                else
                        waitTime = TICK_4;
   				}
				else
				{
				    ptr->loopCount = 0;
					ptr->status = MANUAL_PM_OFF;
				}
			}
		}

        if (manReptStatus)
		{
		    manReptStatus = FALSE;
			reptCount++;
		}

		startScanLoop = FALSE;
        taskDelay(waitTime);
#else
        taskDelay(200);
#endif
	}

    manPMTaskId = 0;
    return OK;
}

STATUS_t procOneSecondManualPM(int ch)
{
    PM_MANUAL_t *ptr = &pmManual[ch];
	U32_t jitter;
	S32_t gv_TIEData;
	STATUS_t status = FALSE;

    if (TRUE /* inpFramer[ch].status == FRAMER_IN_SERVICE */)
    {
	    jitter = readJitterData(ch, MANUAL_PM);
		if (jitter > ptr->eJITTER)
		    ptr->eJITTER = jitter;
	
	    readMTIEData(ch, MANUAL_PM);
		readTIEData(ch, MANUAL_PM);

        gv_TIEData = abs(ptr->tieEndLongValue.l - ptr->tieStartLongValue.l);
        ptr->tiePM += ptr->tieEndLongValue.l - ptr->tieStartLongValue.l;

#if 0
        if (resetTIEFlag[ch])
	        ptr->tieStartLongValue.l = 0;
	    else
	        ptr->tieStartLongValue.l = ptr->tieEndLongValue.l;
#endif

	    ptr->eCS += (((S32_t)abs( gv_TIEData ) ) / UI_Nsec ) / SLIP_DEFAULT;
        
		/* Get BV and CRC4 errors from Framer */
#if 0
		ptr->alarmBPVcount = pmFramer[ch].bvCountMan;
		ptr->alarmCRCcount = pmFramer[ch].crcCountMan;
		pmFramer[ch].bvCountMan = 0;
		pmFramer[ch].crcCountMan = 0;
#endif

      	ptr->eBPV += ptr->alarmBPVcount;
	    ptr->eCRC += ptr->alarmCRCcount;
   
        if ((te1Cfg[ch].lineType == LT_E1) &&
	        (ptr->alarmCRCcount > 805))
	        ptr->eSESP++;
	    else if ((te1Cfg[ch].lineType == LT_T1) &&
	             (ptr->alarmCRCcount > 320))
            ptr->eSESP++;

        if ( ptr->alarmCRCcount >= 1 )
        {
		    ptr->eESP++;
        }
		
		/* increase time count */
		ptr->timeElapsed++;
		
		if ((ptr->interval == ONE_SECOND_INTERVAL) ||    
	        ((ptr->interval == TEN_SECOND_INTERVAL) && (ptr->timeElapsed >= 10)) ||
		    ((ptr->interval == ONE_HUNDRED_SECOND_INTERVAL) && (ptr->timeElapsed >= 100)))
		{
		    ptr->eMTIE = abs(ptr->maxMTIE + ptr->minMTIE );
			ptr->eTIE = ptr->tieEndLongValue.l - ptr->tieStartLongValue.l;
						
			/* make sure that TIE is not greater than MTIE */
			if (abs(ptr->eTIE) > ptr->eMTIE)
			    ptr->eTIE = (ptr->eTIE/abs(ptr->eTIE))*ptr->eMTIE;            

		    reportPM(ch);
			initSmuManualPM(ch, ptr->interval);
			status = TRUE;
			ptr->timeElapsed = 0;
		}
		else if ((ptr->interval == ONE_THOUSAND_SECOND_INTERVAL) && 
		         (ptr->timeElapsed >= 1000))
		{
		    ptr->eMTIE = abs(ptr->maxMTIE + ptr->minMTIE );
			ptr->eTIE = ptr->tieEndLongValue.l - ptr->tieStartLongValue.l;
						
			/* make sure that TIE is not greater than MTIE */
			if (abs(ptr->eTIE) > ptr->eMTIE)
			    ptr->eTIE = (ptr->eTIE/abs(ptr->eTIE))*ptr->eMTIE; 
		    
		    ptr->eWANDER5 = ptr->eMTIE;
            if ( ptr->eWANDER5 > ( 3 * ptr->eMTIE / 10 ) )
                ptr->eWANDER5 = ( 3 *  ptr->eMTIE / 10 );

		    reportPM(ch);
			initSmuManualPM(ch, ptr->interval);
			status = TRUE;
			ptr->timeElapsed = 0;
		}
		else if ((ptr->interval == TEN_THOUSAND_SECOND_INTERVAL) && 
		         (ptr->timeElapsed >= 10000))
		{
			ptr->eMTIE = abs(ptr->maxMTIE + ptr->minMTIE );
			ptr->eTIE = ptr->tieEndLongValue.l - ptr->tieStartLongValue.l;
						
			/* make sure that TIE is not greater than MTIE */
			if (abs(ptr->eTIE) > ptr->eMTIE)
			    ptr->eTIE = (ptr->eTIE/abs(ptr->eTIE))*ptr->eMTIE; 

		    ptr->eWANDER5 = ptr->eMTIE;
            if ( ptr->eWANDER5 > ( 3 * ptr->eMTIE / 100 ) )
                ptr->eWANDER5 = ( 3 *  ptr->eMTIE / 100 );

		    reportPM(ch);
			initSmuManualPM(ch, ptr->interval);
			status = TRUE;
			ptr->timeElapsed = 0;
		}
		else if ((ptr->interval == ONE_HUNDRED_THOUDSAND_SECOND_INTERVAL) && 
		         (ptr->timeElapsed >= 100000))
		{
		    ptr->eMTIE = abs(ptr->maxMTIE + ptr->minMTIE );
			ptr->eTIE = ptr->tieEndLongValue.l - ptr->tieStartLongValue.l;
						
			/* make sure that TIE is not greater than MTIE */
			if (abs(ptr->eTIE) > ptr->eMTIE)
			    ptr->eTIE = (ptr->eTIE/abs(ptr->eTIE))*ptr->eMTIE; 		  

		    ptr->eWANDER24 = ptr->eMTIE;
            if ( ptr->eWANDER24 > ( 9 * ptr->eMTIE / 10 ) )
                ptr->eWANDER24 = ( 9 *  ptr->eMTIE / 10 );

		    reportPM(ch);
			initSmuManualPM(ch, ptr->interval);
			status = TRUE;
			ptr->timeElapsed = 0;
		}
#if 0  
		else
		{
		    printf("Unknown Manual Interval: %d, timeElapsed: %d\n", 
		           ptr->interval, ptr->timeElapsed);
		}
#endif
	}
	
	return (status);	
}

void captureRawData(int ch)
{
    PM_MANUAL_t *ptr = &pmManual[ch];  
    
    if (ptr->loopCount >= RAW_PHASE_BUF) return;

    if ( ptr->interval == ONE_SECOND_INTERVAL )
    {
        readRawData(ch);
    }
    else if ( ptr->interval == TEN_SECOND_INTERVAL )
    {
        ptr->t10++;
        if ( ptr->t10 == 10 )
        {
            ptr->t10 = 0;
            readRawData(ch);
        } 
    } 
    else
    {
        ptr->t100++;
        if ( ptr->t100 == 100 )
        {
            /* 3200 msec */
            ptr->t100 = 0;
            if ( ptr->interval == ONE_HUNDRED_SECOND_INTERVAL )
                readRawData(ch);

            ptr->t1000++;
            if ( ptr->t1000 == 10 )
            {
                /* 32000 msec */
                ptr->t1000 = 0;
                if ( ptr->interval == ONE_THOUSAND_SECOND_INTERVAL )
                    readRawData(ch);
            }

            ptr->t10000++;
            if ( ptr->t10000 == 100 )
            {
                /* 320000 msec */
                ptr->t10000 = 0;
                if ( ptr->interval == TEN_THOUSAND_SECOND_INTERVAL )
                    readRawData(ch);

                ptr->t100000++;
                if ( ptr->t100000 == 10 )
                {
                    /* 3200000 msec */
                    ptr->t100000 = 0;
                    if ( ptr->interval == ONE_HUNDRED_THOUDSAND_SECOND_INTERVAL)
                        readRawData(ch);
                }
            }
        }
    }
}

void readRawData(int ch)
{
    PM_MANUAL_t *ptr = &pmManual[ch];
	U8_t *rawPhasePtr;
	union u_ulong rawPhase;

#if 0
	U32_t gv_RawPhaseByteH_PD3;
    U32_t gv_RawPhaseByteM_PD3;
    U32_t gv_RawPhaseByteL_PD3;
	U32_t currPhase;

	/* FPGA */
    gv_RawPhaseByteH_PD3 = (ulong) XDATA[RAW_PHASE_OUT_BYTE_H_PD3];
    gv_RawPhaseByteM_PD3 = (ulong) XDATA[RAW_PHASE_OUT_BYTE_M_PD3];
    gv_RawPhaseByteL_PD3 = (ulong) XDATA[RAW_PHASE_OUT_BYTE_L_PD3];

    gv_RawPhaseByteH_PD3 = (ulong) ( gv_RawPhaseByteH_PD3 << 16 );
    gv_RawPhaseByteM_PD3 = (ulong) ( gv_RawPhaseByteM_PD3 << 8 );
    gv_RawPhaseByteL_PD3 = (ulong) ( gv_RawPhaseByteL_PD3 );

    currPhase = gv_RawPhaseByteH_PD3 + 
                gv_RawPhaseByteM_PD3 +
                gv_RawPhaseByteL_PD3 ;

    if ( 0x800000 == ( 0x00800000 & gv_RawPhaseByteH_PD3 ) )
    {
        currPhase = 0x800000 | (currPhase >> 1);
        currPhase = ((0x1000000 - currPhase) & 0x00FFFFFF) + 10000000;
    }
    else
    {   
        currPhase = currPhase >> 1;
        currPhase = currPhase & 0x00FFFFFF;
    }   
#endif

    rawPhasePtr = (U8_t *)getRawPhase[ch];
   
	rawPhase.ch.hh = *(U8_t *)(rawPhasePtr);
	rawPhase.ch.hm = *(U8_t *)(rawPhasePtr + 1);
	rawPhase.ch.lm = *(U8_t *)(rawPhasePtr + 2);
	rawPhase.ch.ll = *(U8_t *)(rawPhasePtr + 3);

    if (ptr->rawPhaseIndex < RAW_PHASE_BUF)
	{
        ptr->rawPhaseBuf[ptr->rawPhaseIndex] = rawPhase.ul;
		ptr->rawPhaseIndex++;
	}
	else
	    ptr->rawPhaseIndex = 0;
}

void reportPM(int ch)
{
    struct tm  curtm;
    char tmpInterval[12];
	char reptDate[DATESTAMP_SIZE];
	char reptTime[TIMESTAMP_SIZE];
	int i;

	PM_MANUAL_t *ptr = &pmManual[ch];

  	ptr->eJITTER /= UI_Nsec;
	ptr->eCS = ( ptr->eMTIE / UI_Nsec ) / SLIP_DEFAULT;

	memset(tmpInterval, 0, sizeof(tmpInterval));

    switch (ptr->interval) {
	  case ONE_SECOND_INTERVAL:
	    strcpy(tmpInterval, "1-SEC");
	    break;
	  case TEN_SECOND_INTERVAL: 
	    strcpy(tmpInterval, "10-SEC");
	    break;
	  case ONE_HUNDRED_SECOND_INTERVAL: 
	    strcpy(tmpInterval, "100-SEC");
	    break;
      case ONE_THOUSAND_SECOND_INTERVAL: 
	    strcpy(tmpInterval, "1000-SEC");
	    break;
      case TEN_THOUSAND_SECOND_INTERVAL:  
	    strcpy(tmpInterval, "10000-SEC");
	    break;
      case ONE_HUNDRED_THOUDSAND_SECOND_INTERVAL: 
	    strcpy(tmpInterval, "100000-SEC");
	    break;
	}

    /* get the current system time */
	memset(reptDate, 0, sizeof(reptDate));
	memset(reptTime, 0, sizeof(reptTime));
    SiwDateTimeGet(&curtm);
	strftime(reptDate, DATESTAMP_SIZE-1, "%m-%d-%Y", &curtm);
    strftime(reptTime, TIMESTAMP_SIZE-1, "%H:%M:%S", &curtm);

	/* print PM report to console */
	printf("%s %s %s %s %s\n", "TiemPo", "PRIMARY-ID", "SECONDARY-ID", reptDate, reptTime);  
	printf("A    %010d REPT PM LINK\n", manRptCount++);
	printf("     MON%d, CVL, %d, %s, %d, %s, %s\n", ch, ptr->eBPV, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);   
	printf("     MON%d, CVP, %d, %s, %d, %s, %s\n", ch, ptr->eCRC, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);
    printf("     MON%d, ESP, %d, %s, %d, %s, %s\n", ch, ptr->eESP, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);
	printf("     MON%d, SESP, %d, %s, %d, %s, %s\n", ch, ptr->eSESP, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);
	printf("     MON%d, CS, %d, %s, %d, %s, %s\n", ch, ptr->eCS, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);
    printf("     MON%d, JITTER, %d, %s, %d, %s, %s\n", ch, ptr->eJITTER, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);
    printf("     MON%d, TIE, %d, %s, %d, %s, %s\n", ch, ptr->eTIE, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);
    printf("     MON%d, MTIE, %d, %s, %d, %s, %s\n", ch, ptr->eMTIE, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);
    printf("     MON%d, WDR5M, %f, %s, %d, %s, %s\n", ch, ptr->eWANDER5, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);
    printf("     MON%d, WDR24H, %f, %s, %d, %s, %s\n", ch, ptr->eWANDER24, tmpInterval, reptCount+1, 
	                                                manTestDate, manTestTime);
	printf("     MON%d, PHDATA, ", ch);
	for (i=0; i<RAW_PHASE_BUF; i++)
	    printf("%12.12u ", ptr->rawPhaseBuf[i]);
	printf("\n\n\n");
}

void pmDelayTimeTest(int delay, int n)
{
	struct tm  curtm;
	char reptTime[TIMESTAMP_SIZE+1];
	int i;

	for (i=0; i<n; i++)
	{
    memset(reptTime, 0, sizeof(reptTime));
	SiwDateTimeGet(&curtm);
    strftime(reptTime, TIMESTAMP_SIZE-1, "%H:%M:%S", &curtm);
	printf("Start Time: %s\n", reptTime);

    taskDelay(delay);

    memset(reptTime, 0, sizeof(reptTime));
	SiwDateTimeGet(&curtm);
    strftime(reptTime, TIMESTAMP_SIZE-1, "%H:%M:%S", &curtm);
	printf("End Time: %s\n", reptTime);
    }
}