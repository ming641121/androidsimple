/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: esrApp.c
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
#include "esr_event.h"
#include "serial_if.h"
#include "esr_if.h"
#include "trap_if.h"
#include "uartdrv.h"
#include <trapdef.h>


/* Two type of GPS receiver -> GT8031 and Jupiter30 */
#define GPS_TYPE_NUM 2
#define ESR_NUM 2 /* Two ESR devices will be query. (They are connected to scc1 and scc3 uart port) */
STATUS_t antGet(stsAntEntry_t *antEntry);
STATUS_t antSet(stsAntEntry_t *antEntry);
extern int getRedState(int stsCardIndex, int whichlink);
static STATUS_t queryEsrAndSendTrap(QUERY_APP_t *esrAppData);
static GET_UTC_FUNC getUTCfunc[GPS_TYPE_NUM]= {getUTC_GT, getUTC_30};

/* field definition :
1. queryResult - storing the current querying result.
2. flagWhetherDevReady - flag indicates whether device worked normally last time.
3. DelayTime - querying frequency
4. devType - serial device type.
5. portNum - uart port. 
6. formerStatus - for comparing the current stsGpsStatus with the former one. 
7. stsDevIndex - device index. ex: stsGpsIndex/stsOscillatorIndex 
*/

QUERY_APP_t gps_cdmaAPP[4] = {
{ESR_READY_NOW, ESR_READY_LAST_TIME, 0, gpsDev, SCC1PORT,D_stsGpsStatus_ok, D_stsGpsIndex_esr1},
{ESR_READY_NOW, ESR_READY_LAST_TIME, 0, gpsDev, SCC3PORT,D_stsGpsStatus_ok, D_stsGpsIndex_esr2},
{NO_CDMA_DEVICE, ESR_NOT_READY_LAST_TIME, 0, cdmaDev, SCC1PORT,D_stsGpsStatus_ok, D_stsGpsIndex_esr1},
{NO_CDMA_DEVICE, ESR_NOT_READY_LAST_TIME, 0,  cdmaDev, SCC3PORT,D_stsGpsStatus_ok, D_stsGpsIndex_esr2}
 };	


/************************************************************************
function: 
  esrTask - monitoring task for ESR(GPS/CDMA) devices. 											
return:
  N/A 
*/
STATUS_t esrTask(void)
{
  
  
   int gps_cdmaAPP_Index; /* gps_cdmaAPP[] table index */
   
   int gpsDeviceTypeIndex;
   int utc_yy, utc_mn, utc_dy, utc_hh, utc_mm, utc_ss;
  
   uartSerialReset();
    
 
   
 
   SiwTaskInitAck(OK);

  /* 
   gps_cdmaAPP[0] :  information for GPS connected to SCC1.
   gps_cdmaAPP[1] :  information for GPS connected to SCC3.
   gps_cdmaAPP[2] :  information for CDMA connected to SCC1.
   gps_cdmaAPP[3] :  information for CDMA connected to SCC3.
*/
   while( 1 )
   {  	  
   	   /* Two ESR devices will be query. ( ESR_NUM==2)
   	    (They are connected to scc1 and scc3 uart port)
   	    
   	    if (gps_cdmaAPP_Index==0) 
   	       device connected to scc1 will be query.
   	    else if
   	       device connected to scc3 will be query.
   	    */
  
       for (gps_cdmaAPP_Index=0; gps_cdmaAPP_Index < ESR_NUM; gps_cdmaAPP_Index++) /* query scc1 and scc3 */
	     { /* gps_cdmaAPP_Index is the index of gps_cdmaAPP[] */
            
            /* Query GPS receiver and send trap if needed*/
           queryEsrAndSendTrap( &gps_cdmaAPP[gps_cdmaAPP_Index] );   		  
		       if (gps_cdmaAPP[gps_cdmaAPP_Index].devType == gpsDev)
		       {
		           if (gps_cdmaAPP[gps_cdmaAPP_Index].queryResult == NO_GPS_DEVICE)
               {
               	   /* If GPS receiver is absent now, 
               	      query CDMA device and send trap .*/
                   queryEsrAndSendTrap( &gps_cdmaAPP[gps_cdmaAPP_Index+2] ); 
               }
           } 
                       
       } /* end for (gps_cdmaAPP_Index=0; gps_cdmaAPP_Index<2; gps_cdmaAPP_Index++) */		  
     
     
      /*****  query UTC information   ****/ 
       if ( (D_stsClockInputState_active == getRedState( D_stsCardIndex_esr1, 0)) )
		   {               
		       gpsDeviceTypeIndex=devIdxScc1; /* index which gps device type over SCC1 port */
		       if ( gpsDeviceTypeIndex < GPS_TYPE_NUM)
		           getUTCfunc[gpsDeviceTypeIndex]( &utc_yy, &utc_mn, &utc_dy, &utc_hh, &utc_mm, &utc_ss, SCC1PORT);	     
		   }               
		   else if ( (D_stsClockInputState_active == getRedState( D_stsCardIndex_esr2, 0)) )
		   {               
		       gpsDeviceTypeIndex=devIdxScc3;  /* index to gps device type over SCC3 port */ 
		       if ( gpsDeviceTypeIndex < GPS_TYPE_NUM)
		           getUTCfunc[gpsDeviceTypeIndex]( &utc_yy, &utc_mn, &utc_dy, &utc_hh, &utc_mm, &utc_ss, SCC3PORT);
		   }               
		                             
       taskDelay(100); 
                       
   } /* end while(1) */	  
                       
   return ERROR;	     
}                      
     
                       

/************************************************************************
function: 
  queryEsrAndSendTrap - for query ESR(GPS/CDMA) devices and send 
                                trap based on the querying result. 											
parameter:
  QUERY_APP_t esrAppData - for recording ESR inforamtion.
return:
  OK - if serial device type is either GPS or CDMA.
  ERROR - if serial device type is others.  
*/
STATUS_t queryEsrAndSendTrap(QUERY_APP_t *esrAppData)
{
	
	
#ifdef LinkUpDown	    
	  stsLinkUp_t 								snmpLinkUp;      
    stsLinkDown_t								snmpLinkDown;
#endif
     
    static   stsGpsEntry_t gpsEntry;
    static   stsGpsStatusChange_t gpsStatus;
    static   int flagFirstTimeQuery=1;
    char str[27];
    char gpsStsString[][13]=
    {
	     {"ok"},
	     {"noSignal"},
	     {"poorSignal"},
	     {"nonAvailable"},
	     {"atennaShort"}
    };

    gpsStatus.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
    gpsStatus.stsSystemId.length = sysConfig.stsSystemId.length;
    
    if (esrAppData->devType==gpsDev)
    {  
         
          /* query GPS */
        if (esrAppData->flagWhetherDevReady==ESR_NOT_READY_LAST_TIME || flagFirstTimeQuery)
        {
             close_gps_allOutputSentence(esrAppData->portNum);
		         close_gps30_allOutputSentence(esrAppData->portNum) ;
		        
        }
        esrAppData->queryResult = queryGPS(esrAppData->portNum, &esrAppData->DelayTime);
        
        if (esrAppData->queryResult == ESR_READY_NOW)
        {
            if (esrAppData->flagWhetherDevReady == ESR_NOT_READY_LAST_TIME)
            {
                /* not OK to OK */
               
                    
                if ( !flagFirstTimeQuery )
                {
                    SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL, ESR_OK), esrAppData->portNum);
#ifdef LinkUpDown	                
                    if (esrAppData->portNum == SCC1PORT)
		    			      {
	                      snmpLinkUp.stsClockInputManualSelect	=	D_stsClockInputManualSelect_esr1;
		    	              TrapEventLog(stsLinkUp_TRAP, D_stsClockInputManualSelect_esr1, D_stsCardShelf_main, D_stsCardIndex_esr1, D_stsClockInputManualSelect_esr1, TRAP_CLEAR, &snmpLinkUp, SIW_EVENT_SYM(SERIAL, ESR_OK), esrAppData->portNum);
		    	          }
		    	          else
			              {
				                snmpLinkUp.stsClockInputManualSelect	=	D_stsClockInputManualSelect_esr2;
				                TrapEventLog(stsLinkUp_TRAP, D_stsClockInputManualSelect_esr2, D_stsCardShelf_main, D_stsCardIndex_esr2, D_stsClockInputManualSelect_esr2, TRAP_CLEAR, &snmpLinkUp, SIW_EVENT_SYM(SERIAL, ESR_OK), esrAppData->portNum);
			              } 
#endif			    
                } /* end of if ( !flagFirstTimeQuery ) */
                esrAppData->flagWhetherDevReady = ESR_READY_LAST_TIME;     
            }
        }
        else /* For esrAppData->queryResult != ESR_READY_NOW */
        {
            if (esrAppData->flagWhetherDevReady == ESR_READY_LAST_TIME)
            {
               /* GPS-device was OK to is not OK over scc1/scc3(betta)*/
                
                
                if ( !flagFirstTimeQuery )
                {
                    if (esrAppData->queryResult == NO_GPS_DEVICE) 
                       strcpy(str,"GPS device may be absent.");
	                  else if (esrAppData->queryResult == GPS_FIX_INVALID) 
	                     strcpy(str,"GPS fix not invalid");             
			              SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL, ESR_FAIL), esrAppData->portNum,str);
#ifdef LinkUpDown		
			               					                
		                if (esrAppData->portNum == SCC1PORT)
		                {
	                      snmpLinkDown.stsClockInputManualSelect	=	D_stsClockInputManualSelect_esr1;
			                  TrapEventLog(stsLinkDown_TRAP, D_stsClockInputManualSelect_esr1, D_stsCardShelf_main, D_stsCardIndex_esr1, D_stsClockInputManualSelect_esr1, TRAP_MAJOR, &snmpLinkDown, SIW_EVENT_SYM(SERIAL, ESR_FAIL), esrAppData->portNum,str);
			              }
			              else 
			              {
				                snmpLinkDown.stsClockInputManualSelect	=	D_stsClockInputManualSelect_esr2;
				                TrapEventLog(stsLinkDown_TRAP, D_stsClockInputManualSelect_esr2, D_stsCardShelf_main, D_stsCardIndex_esr2, D_stsClockInputManualSelect_esr2, TRAP_MAJOR, &snmpLinkDown, SIW_EVENT_SYM(SERIAL, ESR_FAIL), esrAppData->portNum,str);
			              }
#endif       
                } /* end of if ( !flagFirstTimeQuery ) */ 
                esrAppData->flagWhetherDevReady = ESR_NOT_READY_LAST_TIME;
            }    
        }                
       /* If GPS status changes, send stsGpsStatusChange_TRAP trap. */
		    gpsEntry.valid[0]=0xff; gpsEntry.valid[1]=0xff;
		    gpsStatus.stsGpsIndex=gpsEntry.stsGpsIndex = esrAppData->stsDevIndex;
	      gpsGet(&gpsEntry);
	      gpsStatus.stsGpsStatus = gpsEntry.stsGpsStatus;
	      gpsStatus.stsGpsState = gpsEntry.stsGpsState;
	      gpsStatus.stsGpsSPOS.length = gpsEntry.stsGpsSPOS->length;
        gpsStatus.stsGpsSPOS.octet_ptr = gpsEntry.stsGpsSPOS->octet_ptr;
        
        gpsStatus.stsGpsTRMO.length = gpsEntry.stsGpsTRMO->length;
		    gpsStatus.stsGpsTRMO.octet_ptr = gpsEntry.stsGpsTRMO->octet_ptr;
		  
		    if (esrAppData->formerStatus != gpsStatus.stsGpsStatus) 
        {
            esrAppData->formerStatus = gpsStatus.stsGpsStatus;
            if ( !flagFirstTimeQuery )
            { 
                if ( gpsStatus.stsGpsStatus > (D_stsGpsStatus_ok-1) && gpsStatus.stsGpsStatus < (D_stsGpsStatus_atennaShort+1) )
                {
           	    
                    SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL, ESR_STATUS_CHANGE), esrAppData->portNum, gpsStsString[gpsStatus.stsGpsStatus-1]   );
                
                   
                    if (esrAppData->portNum == SCC1PORT) {
                        TrapEventLog(stsGpsStatusChange_TRAP, gpsStatus.stsGpsStatus,
				                D_stsCardShelf_main, D_stsCardIndex_esr1, 0, 
				                gpsStatus.stsGpsStatus, (U32_t *) & gpsStatus, 
				                SIW_EVENT_SYM(SERIAL, ESR_STATUS_CHANGE), esrAppData->portNum,  gpsStsString[gpsStatus.stsGpsStatus-1]   );
				            }
				            else if (esrAppData->portNum == SCC3PORT) {
				                TrapEventLog(stsGpsStatusChange_TRAP, gpsStatus.stsGpsStatus,
				                D_stsCardShelf_main, D_stsCardIndex_esr2, 0, 
				                gpsStatus.stsGpsStatus, (U32_t *) & gpsStatus, 
				        
				                SIW_EVENT_SYM(SERIAL, ESR_STATUS_CHANGE), esrAppData->portNum, gpsStsString[gpsStatus.stsGpsStatus-1]   );   
				            
				            } 
				        } 
				    } /* end of if ( !flagFirstTimeQuery )  */
				
        }  
        
             
                  
               
		/*******************************************************/    
    }/*end of if (esrAppData->devType == gpsDev)*/ 

    else if (esrAppData->devType == cdmaDev)
    {   /* query CDMA */ 
        if (esrAppData->flagWhetherDevReady == ESR_NOT_READY_LAST_TIME || flagFirstTimeQuery)
        {   
            cdmaCTIME(esrAppData->portNum);
            taskDelay(200);
        }
		    esrAppData->queryResult = queryCDMA(esrAppData->portNum ,&esrAppData->DelayTime); 
		    if (esrAppData->queryResult == ESR_READY_NOW)
		    { 
		        if(esrAppData->flagWhetherDevReady == ESR_NOT_READY_LAST_TIME)
		        {
		            
		                     
		            if ( !flagFirstTimeQuery ) 
		            {
		                SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL, ESR_OK), esrAppData->portNum);
                     /*  "CDMA-device was not OK" to "CDMA-device is OK" over scc1/scc3 */	
#ifdef LinkUpDown
		                if (esrAppData->portNum == SCC1PORT)
		                {
	                      snmpLinkUp.stsClockInputManualSelect	=	D_stsClockInputManualSelect_esr1;
		                    TrapEventLog(stsLinkUp_TRAP, D_stsClockInputManualSelect_esr1, D_stsCardShelf_main, D_stsCardIndex_esr1, D_stsClockInputManualSelect_esr1, TRAP_CLEAR, &snmpLinkUp, SIW_EVENT_SYM(SERIAL, ESR_OK), esrAppData->portNum);
		                }
		                else
		    	          {
		    		            snmpLinkUP.stsClockInputManualSelect	=	D_stsClockInputManualSelect_esr2;
		    		            TrapEventLog(stsLinkUp_TRAP, D_stsClockInputManualSelect_esr2, D_stsCardShelf_main, D_stsCardIndex_esr2, D_stsClockInputManualSelect_esr2, TRAP_CLEAR, &snmpLinkUp, SIW_EVENT_SYM(SERIAL, ESR_OK), esrAppData->portNum);
		    	          } 
#endif	        
                }
                esrAppData->flagWhetherDevReady = ESR_READY_LAST_TIME;
            }
	      }
	      else 
	      {/* For ( esrAppData->queryResult
	          != ESR_READY_NOW ) */
	          
            if (esrAppData->flagWhetherDevReady == ESR_READY_LAST_TIME )
            { 
                if ( !flagFirstTimeQuery ) 
                { 
                    if (esrAppData->queryResult == NO_CDMA_DEVICE) 
                       strcpy(str,"CDMA device may be absent.");
	                  else if (esrAppData->queryResult == NO_SIG_TIMEOUT) 
	                     strcpy(str,"CDMA no signal time-out");     
                 /* CDMA-device was OK to is not OK over scc1/scc3*/
			              SiwEventLog(NULL, SIW_EVENT_SYM(SERIAL, ESR_FAIL), esrAppData->portNum,str);
#ifdef LinkUpDown		        
                    if (esrAppData->portNum == SCC1PORT)
                    {
	                      snmpLinkDown.stsClockInputManualSelect	=	D_stsClockInputManualSelect_esr1;
			                  TrapEventLog(stsLinkDown_TRAP, D_stsClockInputManualSelect_esr1, D_stsCardShelf_main, D_stsCardIndex_esr1, D_stsClockInputManualSelect_esr1, TRAP_MAJOR, &snmpLinkDown, SIW_EVENT_SYM(SERIAL, ESR_FAIL), esrAppData->portNum,str);
			              }
			              else 
			              {
				                snmpLinkDown.stsClockInputManualSelect	=	D_stsClockInputManualSelect_esr2;
				                TrapEventLog(stsLinkDown_TRAP, D_stsClockInputManualSelect_esr2, D_stsCardShelf_main, D_stsCardIndex_esr2, D_stsClockInputManualSelect_esr1, TRAP_MAJOR, &snmpLinkDown, SIW_EVENT_SYM(SERIAL, ESR_FAIL), esrAppData->portNum,str);
			              }             			   
#endif          
                }
                esrAppData->flagWhetherDevReady = ESR_NOT_READY_LAST_TIME;
            } /* end of if (esrAppData->flagWhetherDevReady == ESR_READY_LAST_TIME ) */   
	      } /* end of For esrAppData->queryResult != ESR_READY_NOW)*/   
    }/* end of if (esrAppData->devType==cdmaDev) */
    else 
    {  /* for not gps/cdma type */
        return ERROR; 
    } 
    flagFirstTimeQuery = 0;          
    return OK;
}  