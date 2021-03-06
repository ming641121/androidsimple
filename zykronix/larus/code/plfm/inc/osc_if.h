#ifndef	  _OSC_IF_H_
#define   _OSC_IF_H_


 /*====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    interface to oscillator devices
 *
 * Note:
 *
 * Author: Betta
 *
 *====================================================================
 */


/*
 *====================================================================
 *=                        type declaration                          =
 *====================================================================
 */
 
#define FPGA_BASE_ADDR                              ((char *) 0x40000000) 
#define CLOCK_SELECT_VIA_FPGA                       *FPGA_BASE_ADDR
#define REF_CLOCK_INPUT                             1
#define GPS_CLOCK_INPUT		                        2

typedef struct
{
	
	/* for storing severity for each stsOscillatorAlarmSettingType -
	oscAlarmSettingSeverity[0]   for aqrsync  
  oscAlarmSettingSeverity[1]   for frngsync 
  oscAlarmSettingSeverity[2]   for hldovrsync    
  oscAlarmSettingSeverity[3]   for lkdsync  
  oscAlarmSettingSeverity[4]   for switchover 
  */
	int oscAlarmSettingSeverity[5];        
}stsAlmSet_t;

                    /* Globle valuables */

/* 
   severity for each stsOscillatorAlarmSettingType -  
   oscAlmEntry[0] for osc1,
   oscAlmEntry[1] for osc2 */
extern stsAlmSet_t oscAlmEntry[2];

 /*  information for oscillator -
    oscApp[0] for osc1,
    oscApp[1] for osc2 */
extern QUERY_APP_t oscApp[2]; 
  
 /*  storing osc current alarm status -
    currentOscStatus[0] for osc1,
    currentOscStatus[1] for osc2 */
extern int currentOscStatus[2];  

              /* declaration of routine */
extern STATUS_t oscTask(void);
extern void oscInitAlmSetting(void);
extern STATUS_t oscAlarmEntry_get(stsOscillatorAlarmEntry_t *input);
extern STATUS_t oscAlarmEntry_set(stsOscillatorAlarmEntry_t *input);
extern int oscStatus_To_oscAlarmSettingType(int stsOscStatus);

#endif