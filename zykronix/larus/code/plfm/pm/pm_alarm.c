#include <stdlib.h>
#include <stdio.h>
#include <axss_types.h>
#include <ds26502.h>
#include <pm.h>

extern PM_FRAMER_t pmFramer[MAX_FRAMERS];

void procTIEAlarm(int ch);
void procMTIEAlarm(int ch);
void procCSAlarm(int ch);
void updateCodeThresholdAlarm(int ch);


void procTIEAlarm(int ch)
{
    PM_FRAMER_t *ptr = &pmFramer[ch];

    if (abs(ptr->gv_TIEData) > ptr->tieThreshold)
    {
#if 0
        TIELedOn();
#endif
        if ( ptr->tieAlarm != ON )
		{
            setInpFramerAlarm(TE1_ALARM_TIE, TE1_ALARM_SET, ch);
			ptr->tieAlarm = ON;
		}
    }
    else
    {
#if 0
        TIELedOff();
#endif
        if ( ptr->tieAlarm == ON )
		{
            setInpFramerAlarm(TE1_ALARM_TIE, TE1_ALARM_CLEAR, ch);
			ptr->tieAlarm = OFF;
		}
    }
}

void procMTIEAlarm(int ch)
{
    PM_FRAMER_t *ptr = &pmFramer[ch];

    if ( abs(ptr->maxMTIE + ptr->minMTIE) > ptr->mtieThreshold ) 
    {
        if ( ptr->mtieAlarm != ON )
        {
            setInpFramerAlarm(TE1_ALARM_MTIE, TE1_ALARM_SET, ch);
			ptr->mtieAlarm = ON;
        }
    }
    else
    {
        if ( ptr->mtieAlarm == ON )
        {
            setInpFramerAlarm(TE1_ALARM_MTIE, TE1_ALARM_CLEAR, ch);
			ptr->mtieAlarm = OFF;
        }
    }
}

void procCSAlarm(int ch)
{
    PM_FRAMER_t *ptr = &pmFramer[ch];

    /* SQ - 2048 format don't need Frameslip, CS alarm */
    if (te1Cfg[ch].lineType == LT_SQW )
        ptr->pmRec.sec_100[0].eCS = 0;

    if ( ptr->pmRec.sec_100[0].eCS >= ptr->csThreshold )
    {
        if ( ptr->csAlarm != ON )
        {
            setInpFramerAlarm(TE1_ALARM_CS, TE1_ALARM_SET, ch);
			ptr->csAlarm = ON;
        }
    }
    else
    {
        if ( ptr->csAlarm == ON )
        {
            setInpFramerAlarm(TE1_ALARM_CS, TE1_ALARM_CLEAR, ch);
			ptr->csAlarm = OFF;
        }
    }
}

void procFRAME_SLIPAlarm(int ch)
{
#if 0
    PM_FRAMER_t *ptr = &pmFramer[ch];

    if ( ptr->channelType == SQW )
         accumCoarseDiff = 0;

    if ( abs( accumCoarseDiff ) > 256)
    {
        if ( ptr->fsAlarm != ON )
        {
            setInpFramerAlarm();
			ptr->fsAlarm = ON;
        }
        if ( accumCoarseDif > MAX_COARSE )
            accumCoarseDiff = MAX_COARSE;
        else if ( accumCoarseDiff < negMAX_COARSE )
            accumCoarseDiff = negMAX_COARSE;
    }
    else
    {
        if ( ptr->fsAlarm == ON )
        {
            setInpFramerAlarm();
			ptr->fsAlarm = OFF;
        }
    }
#endif
}

void updateCodeThresholdAlarm(int ch)
{
	PM_FRAMER_t *ptr = &pmFramer[ch];

	/* ptr->alarmBPVCount is updated in checkFramer() */
    /* if ( ptr->alarmBPVCount * alarmM( ptr->gv_BPVPeriod ) > ptr->bpvThreshold ) */
	if ( ptr->alarmBPVcount > ptr->bpvThreshold )
    {
        if ( ptr->bpvAlarm != ON )
        {
            setInpFramerAlarm(TE1_ALARM_CVL, TE1_ALARM_SET, ch);
			ptr->bpvAlarm = ON;
        }
    }
    else
    {
        if ( ptr->bpvAlarm == ON )
        {
            setInpFramerAlarm(TE1_ALARM_CVL, TE1_ALARM_CLEAR, ch);
			ptr->bpvAlarm = OFF;
        }
    }

    if (((te1Cfg[ch].lineType == LT_E1) && 
         ((te1Cfg[ch].framing == FF_E1_CRC) || 
          (te1Cfg[ch].framing == FF_E1_CRC_MF))) ||
		((te1Cfg[ch].lineType == LT_T1) && (te1Cfg[ch].framing == FF_T1_ESF)))
    {
		/* ptr->alarmCRCcount is updated in checkFramer() */
        /* if ( ptr->alarmCRCcount * alarmM(ptr->gv_CRCPeriod) > ptr->crcThreshold ) */
		if ( ptr->alarmCRCcount > ptr->crcThreshold )
        {
            if ( ptr->crcAlarm != ON )
            {
                setInpFramerAlarm(TE1_ALARM_CVP, TE1_ALARM_SET, ch);
                ptr->crcAlarm = ON;
            }
        }
        else if ( ptr->crcAlarm == ON )
        {
            setInpFramerAlarm(TE1_ALARM_CVP, TE1_ALARM_CLEAR, ch);
		    ptr->crcAlarm = OFF;
        }
    }
}    
