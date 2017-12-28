/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: oscApp.c
 *
 *Define all Event objects for osc
 *
 *Note:
 *
 *Author(s):
 * Betta 
 *
 *********************************************************************/
#include <string.h>
#include <stdio.h>
#include <trapdef.h>
#include "osc_event.h"
#include "serial_if.h"
#include "osc_if.h"
#include "trap_if.h"
#include "uartdrv.h"
#define OSC_ALM_READ 0
#define OSC_ALM_WRITE 1
#define OSC_PRESENT_LAST_TIME 1
#define OSC_ABSENT_LAST_TIME 0
#define OSC_PRESENT_NOW 0 
#define SWITCH_TO_OSC1 0
#define SWITCH_TO_OSC2 1
#define WARMING_UP 0

/* default severity for each  status  associated with osc1 and osc2*/
/* oscAlmEntry[0] for osc1; oscAlmEntry[1] for osc2 */


#ifdef stsOscillatorRedStatusChange_t_DEFINED
static void sendOscRedStsChangeTrap(int oscIdx);
#endif

static STATUS_t oscAlarmSettingSeverityDataAccess(char *fname, int mode, int oscIdx);
static void renewOscPriority(int port, int stsValue, int queryResult);

static STATUS_t sendOscStsChangeTrap(int oscIndex);


int osc1Priority;
int osc2Priority;

 /* currentOscStatus[0] for osc1,
    currentOscStatus[1] for osc2 */
int currentOscStatus[2]={WARMING_UP, WARMING_UP};

/* oscAlmEntry[0].oscAlarmSettingSeverity[0]=0;
oscAlmEntry[0].oscAlarmSettingSeverity[1]=0;
*/
stsAlmSet_t oscAlmEntry[2]; 
   
    
/* 
   The tag indicating to which switch as output timing source. 
   0: switch to osc1; 1: switch to osc2
*/
static short switchoverTag = SWITCH_TO_OSC1; 

/* field definition :
1. queryResult - storing the current querying result.
2. flagWhetherDevReady -  flag indicates whether worked normally last time.
3. DelayTime - querying frequency
4. devType - serial device type.
5. portNum - uart port. 
6. formerStatus - for comparing the current stsGpsStatus/stsOscillatorStatus with the former one.
7. stsDevIndex - device index. ex: stsOscillatorIndex 
*/
static QUERY_APP_t oscApp[2] = { 
	  /* oscApp[0] for osc1,
       oscApp[1] for osc2 */
    {OSC_PRESENT_NOW, OSC_ABSENT_LAST_TIME,  0,  sroDev, SMC1PORT, D_stsOscillatorStatus_absent, D_stsOscillatorIndex_oscillator1}, /* SMC1  OSC1 */
    {OSC_PRESENT_NOW  , OSC_ABSENT_LAST_TIME,  0,  sroDev, SCC4PORT, D_stsOscillatorStatus_absent, D_stsOscillatorIndex_oscillator2}  /* OSC2 */
    };

static char *oscRefParm[] = {"TC1500\r\n", "TW255\r\n", "RA16\r\n", "TR3\r\n", "SY0\r\n"};
static char *oscGpsParm[] = {"TC1500\r\n", "TW255\r\n", "RA16\r\n", "TR3\r\n", "SY3\r\n"};
 
STATUS_t oscSetup(int clockInput);
   
/********************************************************************************************
function: 
  sendOscRedStsChangeTrap - send trap for oscillator redundancy status changing. 
  paramenter:
  int oscIdx -  1/2 (OSC1/OSC2)											
return:
  N/A 
*/
/*
typedef
struct _stsOscillatorRedStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32             stsOscillatorAlarmSettingClock; ex: D_stsOscillatorAlarmSettingClock_oscillator1
            SR_INT32              stsOscillatorRedStatus; ex: D_stsOscillatorRedStatus_standby
            SR_INT32                 stsOscillatorAlarmSeverity; ex: D_stsOscillatorAlarmSeverity_cl
} stsOscillatorRedStatusChange_t    
*/
#ifdef stsOscillatorRedStatusChange_t_DEFINED
void sendOscRedStsChangeTrap(int oscIdx)
{
    static int formerOsc1RedStatus = D_stsOscillatorRedStatus_active, 
	  static int formerOsc2RedStatus = D_stsOscillatorRedStatus_standby;
	 
    int formerStatusOscRedStatus;
	
	  stsOscillatorRedStatusChange_t oscRedSts;
	  
	  /* for assigning the parameters associated with TrapEventLog */ 
    int trapType, slot; 
    char oscRedStsString[2][8] = 
	  {
	  	{"active"},
	  	{"standby"}
	  };
	  
	  stsOscillatorRedStatusChange_t oscRedSts;
    
	  oscRedSts.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
    oscRedSts.stsSystemId.length = sysConfig.stsSystemId.length;
	  if (oscIdx==D_stsOscillatorIndex_oscillator1)
	  {
	    formerStatusOscRedStatus = formerOsc1RedStatus;
		  oscRedSts.stsOscillatorAlarmSettingClock = D_stsOscillatorAlarmSettingClock_oscillator1;
		  trapType = D_stsOscillatorAlarmSettingClock_oscillator1;
		  slot = D_stsCardIndex_oscillator1;
		  
    }
    else if(oscIdx==D_stsOscillatorIndex_oscillator2)
    {
	      formerStatusOscRedStatus = formerOsc2RedStatus;
	      oscRedSts.stsOscillatorAlarmSettingClock = D_stsOscillatorAlarmSettingClock_oscillator2;
	      trapType = D_stsOscillatorAlarmSettingClock_oscillator2;
	      slot = D_stsCardIndex_oscillator2;
	  }
	
    oscRedSts.stsOscillatorAlarmSeverity = oscAlmEntry[oscIdx].oscAlarmSettingSeverity[D_stsOscillatorAlarmSettingType_switchover-1];
	  oscRedSts.stsOscillatorRedStatus = oscGetRedStatus(oscIdx); 
	 
	  if (oscRedSts.stsOscillatorRedStatus != formerStatusOscRedStatus)
	  {
	  
		    SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL, OSC_RED_STATUS_CHANGE), oscIdx,oscRedStsString[oscIdx-1]);
        TrapEventLog(stsOscillatorRedStatusChange_TRAP, trapType,
				     D_stsCardShelf_main, slot, 0, 
				     oscRedSts.stsOscillatorAlarmSeverity, (U32_t *) &oscRedSts, 
				     SIW_EVENT_SYM(SERIAL, OSC_RED_STATUS_CHANGE), oscIdx, oscRedStsString[oscIdx-1]);
	  }
	 
	 
}   	    
#endif
/********************************************************************************************
function: 
  sendOscStsChangeTrap - Query osc status 
                         and send trap if oscillator status changes. 
  paramenter:
  int port -  port number(SMC1PORT/SCC4PORT)											
return:
  N/A 
*/

STATUS_t sendOscStsChangeTrap(int oscIndex)
{
 	
   static stsOscillatorEntry_t oscEntry;
   stsOscillatorAlarmStatusChange_t oscCurrentAlarmStatus, oscFormerAlarmStatus;
  
   /* we don't want to send trap as query 1st time */ 
   static short fgFirsrTimeQuery = 1;
   int i,oscAlarmSettingType;
 
   char oscStsString[5][13] =
   {   
   	  {"holdoversync"},
	    {"aqrsync"},
	    {"frngsync"},
	    {"lkdsync"},
	    {"absent"} 
   };
 
   /* for assigning the parameters associated with TrapEventLog */ 
   int oscAlarmSettingClock;
   int stsCardIndex;	        
  
  
     
   memset( &oscEntry,0,sizeof(stsOscillatorEntry_t) );
   oscCurrentAlarmStatus.stsSystemId.octet_ptr 
   = sysConfig.stsSystemId.octet_ptr;
   oscFormerAlarmStatus.stsSystemId.octet_ptr 
   = sysConfig.stsSystemId.octet_ptr;
   oscFormerAlarmStatus.stsSystemId.length = oscCurrentAlarmStatus.stsSystemId.length
    = sysConfig.stsSystemId.length; 	
   oscEntry.stsOscillatorIndex = oscIndex; 
   oscEntry.valid[0]=oscEntry.valid[1]=0xff;
     

   oscGet(&oscEntry); 
   

   i=oscIndex-1;
   currentOscStatus[i] = oscEntry.stsOscillatorStatus;
#ifdef MODIFIED_stsOscillatorAlarmStatusChange_t
   oscCurrentAlarmStatus.stsOscillatorStatus = currentOscStatus[i];
   oscFormerAlarmStatus.stsOscillatorStatus = oscApp[i].formerStatus;
#else 
   oscCurrentAlarmStatus.stsOscillatorAlarmSettingType = currentOscStatus[i];
   oscFormerAlarmStatus.stsOscillatorAlarmSettingType = oscApp[i].formerStatus;
#endif
   

   if (oscEntry.stsOscillatorStatus==D_stsOscillatorStatus_absent) 
   {
      /* When the osc device is absent now, stsOscillatorAlarmSeverity value
         must be always either D_stsOscillatorAlarmSeverity_mn or D_stsOscillatorAlarmSeverity_mj. 
      */
      
      if (oscApp[ (i+1)%2 ].flagWhetherDevReady==OSC_ABSENT_LAST_TIME)
        oscCurrentAlarmStatus.stsOscillatorAlarmSeverity = D_stsOscillatorAlarmSeverity_mj;
      else if (oscApp[ (i+1)%2 ].flagWhetherDevReady==OSC_PRESENT_LAST_TIME)
        oscCurrentAlarmStatus.stsOscillatorAlarmSeverity = D_stsOscillatorAlarmSeverity_mn;
   } 
   else 
   {
     if ( (oscAlarmSettingType = oscStatus_To_oscAlarmSettingType(oscEntry.stsOscillatorStatus))>0)
     {
   	   oscCurrentAlarmStatus.stsOscillatorAlarmSeverity = oscAlmEntry[i].oscAlarmSettingSeverity[oscAlarmSettingType-1];
     }
     else
     {
       return ERROR;	 
     }
   }
     
  
   if (oscIndex == D_stsOscillatorIndex_oscillator1) {
   	  oscAlarmSettingClock = D_stsOscillatorAlarmSettingClock_oscillator1;
      stsCardIndex = D_stsCardIndex_oscillator1;	        
                 
	 }
	 else if (oscIndex == D_stsOscillatorIndex_oscillator2) {
		  oscAlarmSettingClock = D_stsOscillatorAlarmSettingClock_oscillator2;
      stsCardIndex = D_stsCardIndex_oscillator2;	        
       				       
   }  
   oscCurrentAlarmStatus.stsOscillatorAlarmSettingClock = oscAlarmSettingClock;
   oscFormerAlarmStatus.stsOscillatorAlarmSettingClock = oscAlarmSettingClock;


/* if alarm status changes, send stsOscillatorStatusChange_TRAP */
#ifdef MODIFIED_stsOscillatorAlarmStatusChange_t  
   if (oscApp[i].formerStatus!=oscCurrentAlarmStatus.stsOscillatorStatus)
#else
   if (oscApp[i].formerStatus!=oscCurrentAlarmStatus.stsOscillatorAlarmSettingType)
#endif   
   {

      if ( !fgFirsrTimeQuery )  
      {   
      	  if ( (oscApp[i].formerStatus>(D_stsOscillatorStatus_holdoversync-1))&&(oscApp[i].formerStatus < (D_stsOscillatorStatus_absent+1)) )
      	  {                
              SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL, OSC_FORMER_STATUS), oscAlarmSettingClock,&oscStsString[oscApp[i].formerStatus-1]);
              TrapEventLog(stsOscillatorStatusChange_TRAP, oscAlarmSettingClock,
						         D_stsCardShelf_main, stsCardIndex, 0, 
						         D_stsOscillatorAlarmSeverity_cl, (U32_t *) &oscFormerAlarmStatus, 
       					     SIW_EVENT_SYM(SERIAL, OSC_FORMER_STATUS), oscIndex, &oscStsString[oscApp[i].formerStatus-1]);
	        } 
	        if ( (currentOscStatus[i]>(D_stsOscillatorStatus_holdoversync-1)) && (currentOscStatus[i] < (D_stsOscillatorStatus_absent+1)) )
	        {	
	            SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL, OSC_CURRENT_STATUS), oscAlarmSettingClock,&oscStsString[currentOscStatus[i]-1]);
              TrapEventLog(stsOscillatorStatusChange_TRAP, oscAlarmSettingClock,
					    	     D_stsCardShelf_main, stsCardIndex, 0, 
					    	     D_stsOscillatorAlarmSeverity_cl, (U32_t *) &oscCurrentAlarmStatus, 
					    	     SIW_EVENT_SYM(SERIAL, OSC_CURRENT_STATUS), oscIndex, &oscStsString[currentOscStatus[i]-1]);
	        }
	    }
    
	    oscApp[i].formerStatus=oscCurrentAlarmStatus.stsOscillatorAlarmSettingType;			         
   } 
   fgFirsrTimeQuery = 0;	

   return OK;
}  
/************************************************************************
function: 
  renewOscPriority - set priority for oscillator based on its status. 
parameter
  int port - serial port number.
  int stsValue - oscillator stauts coming from ST command. 
  int queryResult - the value returned by queryRb_Cry routine.										
return:
  N/A 
*/

void renewOscPriority(int port, int stsValue, int queryResult)
{
   	/* The smaller osc1Priority/osc2Priority vlaue, 
   	   the higher priority. */
   	
   if (port==SMC1PORT)
   {
	       if (queryResult!=OSC_PRESENT_NOW  )
	       {
	          osc1Priority+=400;
	          return;
           }   	
           if (stsValue==2||stsValue==3) 
              osc1Priority+=100;
           else if (stsValue==5||stsValue==6)
              osc1Priority+=200;
           else if (stsValue==0||stsValue==1||stsValue==4)
              osc1Priority+=300;
           else if(stsValue==9)
              osc1Priority+=400;
   }
   else if(SCC4PORT)
   {       if (queryResult!=OSC_PRESENT_NOW  )
	       {
	          osc2Priority+=400;
	          return;
           }   	
           if (stsValue==2||stsValue==3) 
              osc2Priority+=100;
           else if (stsValue==5||stsValue==6)
              osc2Priority+=200;
           else if (stsValue==0||stsValue==1||stsValue==4)
              osc2Priority+=300;
           else if(stsValue==9)
              osc2Priority+=400;
	   
   }              
                  
}
/************************************************************************
function: 
  oscTask - monitoring task for oscillators devices. 											
return:
  N/A 
*/
STATUS_t oscTask(void)
{
    /* indicates the current status such as "warming up". */
	  int osc1Status = WARMING_UP;
    int osc2Status = WARMING_UP;
   
  /* for storing the former status such as "warming up". */
    int osc1FormerStatus, osc2FormerSta; 
    
#if 0  
	oscNormal();
#endif
   
  /* Initialize severity for each  stsOscillatorAlarmSettingType */
    oscInitAlmSetting();
    
    /* Bring up uart driver */
    uartSerialReset();
   
	  osc1PowerOnOff(POWER_ON);
    CLOCK_SELECT_VIA_FPGA = SWITCH_TO_OSC1;
	
    SiwTaskInitAck(OK);
  /* 
     Data associated with oscApp[D_stsOscillatorIndex_oscillator1-1] is applicable to OSC1, 
     but oscApp[D_stsOscillatorIndex_oscillator2-1] is applicable to OSC2. 
  */
        		
    while( 1 )
    {                
      
       
        if (oscApp[D_stsOscillatorIndex_oscillator1-1].flagWhetherDevReady==OSC_PRESENT_LAST_TIME)
          /* osc1 was absent last time */
        {  
        	
        	
          /* Query osc1,renew priority vlaue
             and send trap if alarm status changes */
     
            oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult=queryRb_Cry(oscApp[D_stsOscillatorIndex_oscillator1-1].portNum , &oscApp[D_stsOscillatorIndex_oscillator1-1].DelayTime, &osc1Status,sroDev);
            renewOscPriority(oscApp[D_stsOscillatorIndex_oscillator1-1].portNum,  osc1Status, oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult);       
        
		        sendOscStsChangeTrap(oscApp[D_stsOscillatorIndex_oscillator1-1].stsDevIndex);
		 
		     /* if osc1 is absent now, renew flagWhetherDevReady and power off */
            if (oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult != OSC_PRESENT_NOW   )
            { 
	            /* OSC1 was OK to is not OK  */
   		          /* renew flagWhetherDevReady */
                oscApp[D_stsOscillatorIndex_oscillator1-1].flagWhetherDevReady = OSC_ABSENT_LAST_TIME;
                osc1PowerOnOff(POWER_OFF); /* osc1 powered off */
            }
          /* Query osc2, renew priority vlaue 
            and send trap if alarm status changes */ 
            if ( oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady==OSC_PRESENT_LAST_TIME )
              /* Both osc1 and osc2 were present */ 
            {
            	  /* Query osc2, renew priority vlaue 
                   and send trap if alarm status changes */ 
	             oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult=queryRb_Cry(oscApp[D_stsOscillatorIndex_oscillator2-1].portNum , &oscApp[D_stsOscillatorIndex_oscillator2-1].DelayTime, &osc2Status,sroDev);
	             renewOscPriority(oscApp[D_stsOscillatorIndex_oscillator2-1].portNum,  osc2Status, oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult);
	           
	             sendOscStsChangeTrap(oscApp[D_stsOscillatorIndex_oscillator2-1].stsDevIndex);
   
	             if (oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult != OSC_PRESENT_NOW   ) 
	                 /* osc2 is absent now*/
	             {
		              /* renew flagWhetherDevReady */
	                 oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady=OSC_ABSENT_LAST_TIME;
	                 osc2PowerOnOff(POWER_OFF); /* osc2 powered off */
               }    
            } 
            else   /* (oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady
                     ==OSC_ABSENT_LAST_TIME) */   
                   
	          {
		            
               if ( (osc1Status>WARMING_UP) || (oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult != OSC_PRESENT_NOW  ) ) 
		           {         
			             osc2PowerOnOff(POWER_ON); /* osc2 powered on */
			           /* Query osc2, renew priority vlaue 
                    and send trap if alarm status changes */ 
			             oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult=queryRb_Cry(oscApp[D_stsOscillatorIndex_oscillator2-1].portNum , &oscApp[D_stsOscillatorIndex_oscillator2-1].DelayTime, &osc2Status,sroDev);
			             renewOscPriority(oscApp[D_stsOscillatorIndex_oscillator2-1].portNum,  osc2Status, oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult);
		            
			             sendOscStsChangeTrap(oscApp[D_stsOscillatorIndex_oscillator2-1].stsDevIndex);
		            
			             if (oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult == OSC_PRESENT_NOW  )
			             { 		
			             	   /* renew flagWhetherDevReady */		        
			                 oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady = OSC_PRESENT_LAST_TIME;
			                
			                 /* OSC2 was not OK to is OK (betta) */
			                
		               }   
			             else
			             {
				               osc2PowerOnOff(POWER_OFF); /* osc2 powered off */
				           }
	               
	             }/*(osc1Status>WARMING_UP) */
            
               
            }  /*oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady==OSC_ABSENT_LAST_TIME*/
        
        }   
        else  
           /* (oscApp[D_stsOscillatorIndex_oscillator1-1].flagWhetherDevReady
              ==OSC_ABSENT_LAST_TIME) case */
        { 
            if (oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady==OSC_PRESENT_LAST_TIME)
              /* osc1 was absent, but osc2 was present last time */ 
            {
            	  /* Query osc2, renew priority vlaue 
                    and send trap if alarm status changes */ 
	              oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult=queryRb_Cry(oscApp[D_stsOscillatorIndex_oscillator2-1].portNum , &oscApp[D_stsOscillatorIndex_oscillator2-1].DelayTime, &osc2Status,sroDev);
	              renewOscPriority(oscApp[D_stsOscillatorIndex_oscillator2-1].portNum,  osc2Status, oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult);
              
	              sendOscStsChangeTrap(oscApp[D_stsOscillatorIndex_oscillator2-1].stsDevIndex);
     
                if (oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult != OSC_PRESENT_NOW   )
                {  /* OSC2 was OK to is not OK  */
                
                    /* renew flagWhetherDevReady */                     
                    oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady=OSC_ABSENT_LAST_TIME;
                    osc2PowerOnOff(POWER_OFF); 
                }
           /*********************************************************/      
                if ( (osc2Status>WARMING_UP) || (oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult != OSC_PRESENT_NOW  ) ) 
		            {   
		                   
			              osc1PowerOnOff(POWER_ON); /* osc1 powered on */
			              
			              oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult=queryRb_Cry(oscApp[D_stsOscillatorIndex_oscillator1-1].portNum , &oscApp[D_stsOscillatorIndex_oscillator1-1].DelayTime, &osc1Status,sroDev);
			              renewOscPriority(oscApp[D_stsOscillatorIndex_oscillator1-1].portNum,  osc1Status, oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult);
			              
			              sendOscStsChangeTrap(oscApp[D_stsOscillatorIndex_oscillator1-1].stsDevIndex);
		 
			              if (oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult== OSC_PRESENT_NOW  )
			              { 	
			              		/* renew flagWhetherDevReady */   		        
			                  oscApp[D_stsOscillatorIndex_oscillator1-1].flagWhetherDevReady = OSC_PRESENT_LAST_TIME;
			                 
			                  /* OSC1 was not OK to is OK (betta) */
			                 
		                }   
			              else
			              {
				                osc1PowerOnOff(POWER_OFF);
				            }
           	             
                }/*(osc1Status>WARMING_UP) */
          /*********************************************************/    
              
              
            } 
            else /*(oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady==OSC_ABSENT_LAST_TIME)*/        
              /* Both osc1 and osc2 were absent */
            {
	              if (oscApp[D_stsOscillatorIndex_oscillator1-1].DelayTime==0 )
	              {
		               osc1PowerOnOff(POWER_ON);
		                /* Query osc1, renew priority vlaue 
                    and send trap if alarm status changes */ 
		               oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult=queryRb_Cry(oscApp[D_stsOscillatorIndex_oscillator1-1].portNum , &oscApp[D_stsOscillatorIndex_oscillator1-1].DelayTime, &osc1Status,sroDev);
		               renewOscPriority(oscApp[D_stsOscillatorIndex_oscillator1-1].portNum,  osc1Status, oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult);		              
		             
		               sendOscStsChangeTrap(oscApp[D_stsOscillatorIndex_oscillator1-1].stsDevIndex);
		         
		               if (oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult == OSC_PRESENT_NOW  )
		                   /* OSC1 was not OK to is OK */ 
		                   oscApp[D_stsOscillatorIndex_oscillator1-1].flagWhetherDevReady=OSC_PRESENT_LAST_TIME; 
		           }
		           else
		           {
			             if ( oscApp[D_stsOscillatorIndex_oscillator1-1].DelayTime>0 ) 
			                 oscApp[D_stsOscillatorIndex_oscillator1-1].DelayTime--;  
			         }
			         /* if osc1 is still absent now, query osc2 immediately */
			         if (oscApp[D_stsOscillatorIndex_oscillator1-1].queryResult != OSC_PRESENT_NOW  ) 
			         {
			         	/*
		               if (oscApp[D_stsOscillatorIndex_oscillator2-1].DelayTime==0)
	                 {
	              */
		                   osc2PowerOnOff(POWER_ON);
		             /* Query osc2, renew priority vlaue 
                    and send trap if alarm status changes */ 
		                   oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult=queryRb_Cry(oscApp[D_stsOscillatorIndex_oscillator2-1].portNum , &oscApp[D_stsOscillatorIndex_oscillator2-1].DelayTime, &osc2Status,sroDev);
		                   renewOscPriority(oscApp[D_stsOscillatorIndex_oscillator2-1].portNum ,  osc2Status, oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult);
		             
		                   sendOscStsChangeTrap(oscApp[D_stsOscillatorIndex_oscillator2-1].stsDevIndex);
		         
		                   if (oscApp[D_stsOscillatorIndex_oscillator2-1].queryResult == OSC_PRESENT_NOW  ) 
		                       oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady=OSC_PRESENT_LAST_TIME; /* OSC2 was not OK to is OK (betta) */
/*		               
		               }
		               else
		               {
			                 oscApp[D_stsOscillatorIndex_oscillator2-1].DelayTime--;  
			             }
*/
		           }  
	          } /*end of (oscApp[D_stsOscillatorIndex_oscillator2-1].flagWhetherDevReady==OSC_ABSENT_LAST_TIME)*/  			      
		    } 
  	/* if status chages, switch to oscillator device 
	     with higher priority */ 
   		
		    if(switchoverTag==SWITCH_TO_OSC2)
		    {
		    		/* The smaller osc1Priority/osc2Priority vlaue, 
		    	   the higher priority. */ 
		        if (osc2FormerSta != osc2Status) 
		    	  {
		    	  	
		    		    if (osc2Priority>osc1Priority)  	
		    		    {
		    		        CLOCK_SELECT_VIA_FPGA = SWITCH_TO_OSC1; 
		    			      switchoverTag = SWITCH_TO_OSC1; 
		    	      }
		        } 
		    }
		    else if (switchoverTag == SWITCH_TO_OSC1)
        {
		    	  if (osc1FormerStatus != osc1Status) 
		    	  {
		    		    if (osc1Priority>osc2Priority)
		    		    {
		    			     CLOCK_SELECT_VIA_FPGA =SWITCH_TO_OSC2 ; 
		    			     switchoverTag =SWITCH_TO_OSC2 ; 
		    	      }
		        } 
        }
#ifdef stsOscillatorRedStatusChange_t_DEFINED    
	      sendOscRedStsChangeTrap(D_stsOscillatorIndex_oscillator1);
	      sendOscRedStsChangeTrap(D_stsOscillatorIndex_oscillator2);
#endif	    
      /* Stores status value resulting from quering oscillator device*/
		    osc1FormerStatus=osc1Status; 
        osc2FormerSta=osc2Status; 
        taskDelay(200);
            		
    }
    return ERROR;	
}
/********************************************************************************************
function: 
    oscSetSwitchState - to set switched state.
paramenter:
    int oscIndex -  D_stsOscillatorIndex_oscillator1/D_stsOscillatorIndex_oscillator2		
    int oscSwitchState - switched state value will cause oscillator to switch. 									
return:
    switch State - D_stsOscillatorSwitchState_switchOver/D_stsOscillatorSwitchState_noAction
*/
void oscSetSwitchState(int oscIndex ,int oscSwitchState)
{
	if (oscSwitchState==D_stsOscillatorSwitchState_switchOver)
	{
		if (oscIndex==D_stsOscillatorIndex_oscillator1)
		{
		  CLOCK_SELECT_VIA_FPGA=SWITCH_TO_OSC1; 
			switchoverTag = SWITCH_TO_OSC1;
		}
		else if(oscIndex==D_stsOscillatorIndex_oscillator2)
		{
			CLOCK_SELECT_VIA_FPGA = SWITCH_TO_OSC2; 
			switchoverTag = SWITCH_TO_OSC2;
		}
	}
}
/********************************************************************************************
function: 
    oscGetSwitchState - to get switched state. 
paramenter:
    int oscIndex -  D_stsOscillatorIndex_oscillator1/D_stsOscillatorIndex_oscillator2											
return:
    switch State - D_stsOscillatorSwitchState_switchOver/D_stsOscillatorSwitchState_noAction
*/
int oscGetSwitchState(int oscIndex)
{
	int switchState;
	if (oscIndex==D_stsOscillatorIndex_oscillator1)
	{ 
			if (switchoverTag==SWITCH_TO_OSC1)
			{
				switchState = D_stsOscillatorSwitchState_switchOver;
			}
			else if (switchoverTag==SWITCH_TO_OSC2)
			{
				switchState = D_stsOscillatorSwitchState_noAction;
			}
	}
	else if(oscIndex==D_stsOscillatorIndex_oscillator2)
	{
		  if (switchoverTag==SWITCH_TO_OSC2)
			{
				switchState = D_stsOscillatorSwitchState_switchOver;
			}
			else if (switchoverTag==SWITCH_TO_OSC1)
			{
				switchState = D_stsOscillatorSwitchState_noAction;
			}
	}
	return switchState;
   	
}

/********************************************************************************************
function: 
    oscGetRedStatus - to get osc1/osc2 redundancy. 
paramenter:
    int oscIndex -  D_stsOscillatorIndex_oscillator1/D_stsOscillatorIndex_oscillator2											
return:
    redundancy value - D_stsOscillatorRedStatus_active/D_stsOscillatorRedStatus_standby
*/
int oscGetRedStatus(int oscIndex)
{
	int redStatus;
	if (oscIndex==D_stsOscillatorIndex_oscillator1)
	{ 
			if (switchoverTag==SWITCH_TO_OSC1)
			{
				redStatus = D_stsOscillatorRedStatus_active;
			}
			else if (switchoverTag==SWITCH_TO_OSC2)
			{
				redStatus = D_stsOscillatorRedStatus_standby;
			}
	}
	else if(oscIndex==D_stsOscillatorIndex_oscillator2)
	{
		  if (switchoverTag==SWITCH_TO_OSC2)
			{
				redStatus = D_stsOscillatorRedStatus_active;
			}
			else if (switchoverTag==SWITCH_TO_OSC1)
			{
				redStatus = D_stsOscillatorRedStatus_standby;
			}
	}
	return redStatus;
   	
}

   
/************************************************************************
function: 
  oscInitAlmSetting -  to set default severity for each  stsOscillatorAlarmSettingType.   											
return:
  OK
*/
void oscInitAlmSetting(void)
{	
/* oscAlmEntry[0] for osc1,
   oscAlmEntry[1] for osc2 */
	if (oscAlarmSettingSeverityDataAccess("osc1AlarmSettingSeverity.dat", OSC_ALM_READ, D_stsOscillatorIndex_oscillator1) == ERROR) 
	{ 
     oscAlmEntry[D_stsOscillatorIndex_oscillator1-1].oscAlarmSettingSeverity[0] = D_stsOscillatorAlarmSettingSeverity_cr; /* for D_stsOscillatorAlarmSettingType_aqrsync */
	   oscAlmEntry[D_stsOscillatorIndex_oscillator1-1].oscAlarmSettingSeverity[1] = D_stsOscillatorAlarmSettingSeverity_mn; /* for D_stsOscillatorAlarmSettingType_frngsync */
	   oscAlmEntry[D_stsOscillatorIndex_oscillator1-1].oscAlarmSettingSeverity[2] = D_stsOscillatorAlarmSettingSeverity_nr; /* for D_stsOscillatorAlarmSettingType_hldovrsync   */
	   oscAlmEntry[D_stsOscillatorIndex_oscillator1-1].oscAlarmSettingSeverity[3] = D_stsOscillatorAlarmSettingSeverity_na; /* for D_stsOscillatorAlarmSettingType_lkdsync */
	   oscAlmEntry[D_stsOscillatorIndex_oscillator1-1].oscAlarmSettingSeverity[4] = D_stsOscillatorAlarmSettingSeverity_mj; /* for D_stsOscillatorAlarmSettingType_switchover */
     oscAlarmSettingSeverityDataAccess("osc1AlarmSettingSeverity.dat", OSC_ALM_WRITE, D_stsOscillatorIndex_oscillator1);
  }
  if (oscAlarmSettingSeverityDataAccess("osc1AlarmSettingSeverity.dat", OSC_ALM_READ, D_stsOscillatorIndex_oscillator1) == ERROR) 
	{ 
     oscAlmEntry[D_stsOscillatorIndex_oscillator2-1].oscAlarmSettingSeverity[0] = D_stsOscillatorAlarmSettingSeverity_cr; /* for D_stsOscillatorAlarmSettingType_aqrsync */
	   oscAlmEntry[D_stsOscillatorIndex_oscillator2-1].oscAlarmSettingSeverity[1] = D_stsOscillatorAlarmSettingSeverity_mn; /* for D_stsOscillatorAlarmSettingType_frngsync */
	   oscAlmEntry[D_stsOscillatorIndex_oscillator2-1].oscAlarmSettingSeverity[2] = D_stsOscillatorAlarmSettingSeverity_nr; /* for D_stsOscillatorAlarmSettingType_hldovrsync   */
	   oscAlmEntry[D_stsOscillatorIndex_oscillator2-1].oscAlarmSettingSeverity[3] = D_stsOscillatorAlarmSettingSeverity_na; /* for D_stsOscillatorAlarmSettingType_lkdsync */
	   oscAlmEntry[D_stsOscillatorIndex_oscillator2-1].oscAlarmSettingSeverity[4] = D_stsOscillatorAlarmSettingSeverity_mj; /* for D_stsOscillatorAlarmSettingType_switchover */
     oscAlarmSettingSeverityDataAccess("osc1AlarmSettingSeverity.dat", OSC_ALM_WRITE, D_stsOscillatorIndex_oscillator1);
  }	
   
}


/******************************************************************************
function:
    oscAlarmSettingSeverityDataAccess - Access  severity for each stsOscillatorAlarmSettingType 
                         via accessing files. 
              
paramenter: 
    char *fname - file name storing serivity value.
    int mode - IO mode (R/W)
    int oscIdx - oscillator index. (D_stsOscillatorIndex_oscillator1/D_stsOscillatorIndex_oscillator2)
RETURN: 
   	OK    : Access sucessfully.
    ERROR : Fail to access.
*/
STATUS_t oscAlarmSettingSeverityDataAccess(char *fname, int mode, int oscIdx)
{
    FILE *fhd = NULL;

    STATUS_t status = OK;
    int tmp[5];

    if (fname == NULL) {
        return ERROR;
    }

    if (mode == OSC_ALM_READ) {
        if ((fhd = fopen(fname, "r")) == NULL)
            status = ERROR;

        else {
            if (fread((char *) &tmp, sizeof(int), 5, fhd) != 5)
                status = ERROR;
            else {
                
                oscAlmEntry[oscIdx-1].oscAlarmSettingSeverity[0] = tmp[0];; /* holdoversync */
	              oscAlmEntry[oscIdx-1].oscAlarmSettingSeverity[1] = tmp[1];; /* aqrsync      */
	              oscAlmEntry[oscIdx-1].oscAlarmSettingSeverity[2] = tmp[2];; /* frngsync     */
	              oscAlmEntry[oscIdx-1].oscAlarmSettingSeverity[3] = tmp[3];; /* lkdsync      */
	              oscAlmEntry[oscIdx].oscAlarmSettingSeverity[4] = tmp[4];; /* absent       */
            }
        }
        fclose(fhd);
    } else if (mode == OSC_ALM_WRITE) {

        if ((fhd = fopen(fname, "w")) == NULL)
            status = ERROR;
        else {
            tmp[0] = oscAlmEntry[oscIdx-1].oscAlarmSettingSeverity[0];
            tmp[1] = oscAlmEntry[oscIdx-1].oscAlarmSettingSeverity[1];
            tmp[2] = oscAlmEntry[oscIdx-1].oscAlarmSettingSeverity[2];
            tmp[3] = oscAlmEntry[oscIdx-1].oscAlarmSettingSeverity[3];
            tmp[4] = oscAlmEntry[oscIdx-1].oscAlarmSettingSeverity[4];
        }
        fwrite((char *) &tmp, sizeof(char), 5, fhd);
        fclose(fhd);
    }
    return status;
}

/******************************************************************************
function:
    oscStatus_To_oscAlarmSettingType - 
     Transition stsOscillatorStatus value to stsOscillatorAlarmSettingType value
     for getting stsOscillatorAlarmSettingSeverity value                                 
              
paramenter: 
    char *fname - file name storing serivity value.
    int mode - IO mode (R/W)
    int oscIdx - oscillator index. (D_stsOscillatorIndex_oscillator1/D_stsOscillatorIndex_oscillator2)
RETURN: 
   	stsOscillatorAlarmSettingType value
*/
int oscStatus_To_oscAlarmSettingType(int stsOscStatus)
{
	  int oscAlarmSettingType;
	  switch (stsOscStatus)
	  {
	      case D_stsOscillatorStatus_holdoversync :
	      	oscAlarmSettingType = D_stsOscillatorAlarmSettingType_hldovrsync;
	      	break;
	      case D_stsOscillatorStatus_aqrsync :	  	
	  	    oscAlarmSettingType = D_stsOscillatorAlarmSettingType_aqrsync;
	      	break;
	      case D_stsOscillatorStatus_frngsync :
	      	oscAlarmSettingType = D_stsOscillatorAlarmSettingType_frngsync;
	      	break; 	
	      case D_stsOscillatorStatus_lkdsync :
	      	oscAlarmSettingType = D_stsOscillatorAlarmSettingType_lkdsync;
	      	break;	
	      default :
	      	oscAlarmSettingType = -1; 
	      	break;	
	  }
	
    
    return oscAlarmSettingType;
           	
	
}

STATUS_t oscSetup(int clockInput)
{
    char rxBuf[10];
	int i;

    switch (clockInput) {
      case REF_CLOCK_INPUT:
	    for (i=0; i<5; i++)
		{
		    /* printf("%s", oscRefParm[i]); */
		    uartCmdAct(oscRefParm[i], strlen(oscRefParm[i]), &rxBuf[0], 0, 2);
			uartCmdAct(oscRefParm[i], strlen(oscRefParm[i]), &rxBuf[0], 0, 4);
		}
        break;
      
      case GPS_CLOCK_INPUT:
	    for (i=0; i<5; i++)
		{
		    /* printf("%s", oscRefParm[i]); */
		    uartCmdAct(oscGpsParm[i], strlen(oscGpsParm[i]), &rxBuf[0], 0, 2);
			uartCmdAct(oscGpsParm[i], strlen(oscGpsParm[i]), &rxBuf[0], 0, 4);
        }
        break;
    }
    
    return OK;
}  