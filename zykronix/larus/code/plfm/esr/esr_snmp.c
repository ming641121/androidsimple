#include "serial_if.h"
#include "esr_if.h"

extern int getRedState(int stsCardIndex, int whichlink);
/************************************************************************
function:              
  antGet - action function to get antenna information. 			
parameter:             
  stsAntEntry_t *antEntry  								
return:                
  OK/ERROR 
*/

STATUS_t antGet(stsAntEntry_t *antEntry)
{
	if (antEntry->stsAntIndex==1) {
	   if (gps_cdmaAPP[0].flagWhetherDevReady) 
	      antEntry->stsAntStatus=D_stsAntStatus_active; 
	   else
	      antEntry->stsAntStatus=D_stsAntStatus_absent;  
    }
    else if(antEntry->stsAntIndex==2)
    {
	   if (gps_cdmaAPP[1].flagWhetherDevReady)   
	      antEntry->stsAntStatus=D_stsAntStatus_active; 
	   else
	      antEntry->stsAntStatus=D_stsAntStatus_absent; 
	}

        
	return OK;
}

/************************************************************************
function: 
  antGet - action function to switch or enable antenna. 			
parameter:
  stsAntEntry_t *antEntry   								
return:
  OK
*/
STATUS_t antSet(stsAntEntry_t *antEntry)
{
	if (antEntry->stsAntIndex==1) {
	   if (gps_cdmaAPP[0].flagWhetherDevReady) 
	    
	      antEntry->stsAntSwitchState=getRedState(D_stsCardIndex_esr1, 0); 
	   else
	      antEntry->stsAntSwitchState=D_stsAntSwitchState_noAction;  
    }
    else if(antEntry->stsAntIndex==2)
    {
	   if (gps_cdmaAPP[1].flagWhetherDevReady)   
	      antEntry->stsAntSwitchState=getRedState(D_stsCardIndex_esr2, 0);
	   else
	      antEntry->stsAntSwitchState=D_stsAntSwitchState_noAction; 
	}
   

	return OK; 
}