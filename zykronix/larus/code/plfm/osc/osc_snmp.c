#include "serial_if.h"
#include "osc_if.h"

/******************************************************************************
function:
oscAlarmEntry_get - to get the mibs data information to the sensor device
              
paramenter: 
 stsEmmStatusEntry_t *emmEntry - the struct to store response data for temperature sensor query response  
  
 RETURNS: 
   		 OK    - if sensor is detected and get the temperature information
       ERROR - otherwise
*/
/*
typedef
struct _stsOscillatorAlarmEntry_t {
        SR_INT32        stsOscillatorAlarmSettingClock; ex: oscillator1(1)
        SR_INT32        stsOscillatorAlarmSettingType;  ex: frngsync
        SR_INT32        stsOscillatorAlarmSettingSeverity; ex: D_stsOscillatorAlarmSettingSeverity_cr
        SR_INT32        stsOscillatorAlarmSeverity;        ex: D_stsOscillatorAlarmSeverity_cl
        char            valid[1];
} stsOscillatorAlarmEntry_t;
*/
STATUS_t oscAlarmEntry_get(stsOscillatorAlarmEntry_t * input)
{ 
	 /* to which OscAlarmSettingType current  osc status mapping */ 
	 int currentOscAlarmSettingType; 
   input->stsOscillatorAlarmSettingSeverity
      = oscAlmEntry[input->stsOscillatorAlarmSettingClock].oscAlarmSettingSeverity[input->stsOscillatorAlarmSettingType-1];
 
   input->stsOscillatorAlarmSettingSeverity 
   = oscAlmEntry[input->stsOscillatorAlarmSettingClock-1].oscAlarmSettingSeverity[input->stsOscillatorAlarmSettingType-1]; 
   
   input->stsOscillatorAlarmSeverity 
   = input->stsOscillatorAlarmSettingSeverity;
   
   currentOscAlarmSettingType 
   = oscStatus_To_oscAlarmSettingType(currentOscStatus[input->stsOscillatorAlarmSettingClock-1]);
  
   if ( input->stsOscillatorAlarmSettingType != currentOscAlarmSettingType )
     input->stsOscillatorAlarmSeverity = D_stsOscillatorAlarmSeverity_cl;
   return OK;
}

/************************************************************************
function: 
  oscAlarmEntry_set - action function to set severity for each alarm status.   											
return:
  OK
*/


STATUS_t oscAlarmEntry_set(stsOscillatorAlarmEntry_t *input)
{

   
	 if ( VALID(I_stsOscillatorAlarmSettingSeverity, input->valid) )
	 {      
		 oscAlmEntry[input->stsOscillatorAlarmSettingClock-1].oscAlarmSettingSeverity[input->stsOscillatorAlarmSettingType-1] 
		 = input->stsOscillatorAlarmSettingSeverity;
   }
   return OK;		
}
