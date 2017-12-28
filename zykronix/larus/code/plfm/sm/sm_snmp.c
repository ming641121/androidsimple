/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: sm_snmp.c
 *
 *Define all Event objects for SM Module
 *
 *Note:
 *
 *Author(s):shuming
 *
 *********************************************************************/

 /********************************************************************
 * $Id: sm_snmp.c,v 1.9 2006/10/05 04:14:00 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/sm/sm_snmp.c,v $
 *====================================================================
 * $Log: sm_snmp.c,v $
 * Revision 1.9  2006/10/05 04:14:00  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.8  2006/10/03 12:10:33  cvsshuming
 * add action function support in shelf manager for special card type, such as top, smu and imu
 *
 * Revision 1.7  2006/10/03 02:43:50  cvsshuming
 * rename redundance and state to be redundancy_enable and redundancy_state
 *
 * Revision 1.6  2006/10/02 12:08:38  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.5  2006/09/20 10:37:38  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.4  2006/09/15 09:59:26  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.3  2006/09/13 07:20:27  cvsshuming
 * for portalarmstatus
 *
 * Revision 1.2  2006/09/12 10:19:51  cvsshuming
 * update snmp action func
 *
 * Revision 1.1  2006/09/12 05:48:52  cvsshuming
 * mv snmp and some utils out of sm.c
 *
 *====================================================================
 * $Endlog $
 *********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <vxWorks.h>
#include <tss_types.h>
#include <semLib.h>
#include "sm_event.h"
#include "trapdef.h"
#include <trap_if.h>
#include "tiu_if.h"
#include "siw_events.h"
#include "led_if.h"
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


/* snmp function declaration */


    STATUS_t smShelfGet(stsShelfEntry_t * mibstructIn);
    STATUS_t smShelfSet(stsShelfEntry_t * mibstructIn);
    			
    STATUS_t smOutputCardGet(stsOutputCardEntry_t * mibstructIn);
    STATUS_t smOutputCardSet(stsOutputCardEntry_t * mibstructIn);
    		
    STATUS_t smOutputPortGet(stsOutputPortSettingEntry_t *mibstructIn);
    STATUS_t smOutputPortSet(stsOutputPortSettingEntry_t *mibstructIn);
    			
    /* new one and not yet sent to joe */
			
			
		


/********************************************************************
* function: STATUS_t smShelfGet 
* description: the snmp action function 
* input parameter: [index]
		SR_INT32        stsShelfIndex;
			
* output parameter:
        SR_INT32        stsShelfIndex;
        OctetString     *stsShelfSerial;
        OctetString     *stsShelfCLEICode;
        SR_INT32        stsShelfReset;
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/


STATUS_t smShelfGet(stsShelfEntry_t *mibstructIn){
    
    mibstructIn->stsShelfSerial		=	&shelves[mibstructIn->stsShelfIndex].serial;
    mibstructIn->stsShelfCLEICode	=	&shelves[mibstructIn->stsShelfIndex].cleicode;
    
    mibstructIn->stsShelfReset = D_stsShelfReset_noAction;
    
    return OK;
}
/********************************************************************
* function: STATUS_t smShelfSet 
* input parameter: 
        SR_INT32        stsShelfIndex;
        OctetString     *stsShelfSerial;
        OctetString     *stsShelfCLEICode;
        SR_INT32        stsShelfReset;
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/
STATUS_t smShelfSet(stsShelfEntry_t * mibstructIn)
{
    U8_t tmp = 0;
    
    if (VALID(I_stsShelfSerial, mibstructIn->valid)){
        if (mibstructIn->stsShelfSerial->length  < SM_SHELF_SERIAL_LEN){
            
            memcpy(shelves[mibstructIn->stsShelfIndex].serial.octet_ptr,
                mibstructIn->stsShelfSerial->octet_ptr, mibstructIn->stsShelfSerial->length);
        
            shelves[mibstructIn->stsShelfIndex].serial.length   =  mibstructIn->stsShelfSerial->length;
            
            
        }else{
            memcpy(shelves[mibstructIn->stsShelfIndex].serial.octet_ptr,
                mibstructIn->stsShelfSerial->octet_ptr, SM_SHELF_SERIAL_LEN);
        
            shelves[mibstructIn->stsShelfIndex].serial.length   =  SM_SHELF_SERIAL_LEN;
        
        }

   	}
   	    
   if (VALID(I_stsShelfCLEICode, mibstructIn->valid)){
   	
        if (mibstructIn->stsShelfCLEICode->length  < SM_SHELF_CLEICODE_LEN){
            
            memcpy(shelves[mibstructIn->stsShelfIndex].cleicode.octet_ptr,
                mibstructIn->stsShelfCLEICode->octet_ptr, mibstructIn->stsShelfCLEICode->length);
        
            shelves[mibstructIn->stsShelfIndex].cleicode.length   =  mibstructIn->stsShelfCLEICode->length;
            
            
        }else{
            memcpy(shelves[mibstructIn->stsShelfIndex].cleicode.octet_ptr,
                mibstructIn->stsShelfCLEICode->octet_ptr, SM_SHELF_CLEICODE_LEN);
        
            shelves[mibstructIn->stsShelfIndex].cleicode.length   =  SM_SHELF_CLEICODE_LEN;
        
        }

   	}
 
 

      	
    if (VALID(I_stsShelfReset, mibstructIn->valid)){
   	
					    if (mibstructIn->stsShelfReset == D_stsShelfReset_reset) {
					
					        switch (mibstructIn->stsShelfIndex) {
					        case D_stsShelfIndex_main:
					
					            *(U8_t *) MAIN_FPGA_W_REG_2 = 0xFF;
					            break;
					        case D_stsShelfIndex_expansion_1:
					            expFPGAAccess(EXP1_SHELF, 0x00, 0x01, &tmp, FPGA_WRITE);
					            expFPGAAccess(EXP1_SHELF, 0x00, 0x02, &tmp, FPGA_WRITE);
					            break;
					        case D_stsShelfIndex_expansion_2:
					            expFPGAAccess(EXP2_SHELF, 0x00, 0x01, &tmp, FPGA_WRITE);
					            expFPGAAccess(EXP2_SHELF, 0x00, 0x02, &tmp, FPGA_WRITE);
					            break;
					        case D_stsShelfIndex_expansion_3:
					            expFPGAAccess(EXP3_SHELF, 0x00, 0x01, &tmp, FPGA_WRITE);
					            expFPGAAccess(EXP3_SHELF, 0x00, 0x02, &tmp, FPGA_WRITE);
					            break;
					        case D_stsShelfIndex_expansion_4:
					            expFPGAAccess(EXP4_SHELF, 0x00, 0x01, &tmp, FPGA_WRITE);
					            expFPGAAccess(EXP4_SHELF, 0x00, 0x02, &tmp, FPGA_WRITE);
					            break;
					        }
					    }

   	}      
 
    return OK;
}

/********************************************************************
* function: STATUS_t smCardGet(stsCardEntry_t * mibstructIn)
* input parameter: [index]
        SR_INT32        stsCardShelf;
        SR_INT32        stsCardIndex;
  output parameter:
        SR_INT32        stsCardType;
        OctetString     *stsCardDescr;
        OctetString     *stsCardSerial;
        OctetString     *stsCardHwVersion;
        OctetString     *stsCardCLEICode;
        SR_INT32        stsCardOperStatus;
        SR_INT32        stsCardReset;
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/

STATUS_t smCardGet(stsCardEntry_t *mibstructIn){
    OUTPUT_CARD_PARA_t 	*thisCard;
    SPECIAL_CARD_PARA_t *specialCard;
    U8_t isOutputCard = SM_YES; 
    		/* 	SM_YES: normal output card; 
    			SM_NO:	special card */
    
    
	/* sanity check*/
#ifdef SM_SNMP_DEBUG
	printf("in smCardGet, start sanity check\n");
#endif
    switch(mibstructIn->stsCardShelf){
        case D_stsCardShelf_main:
        	if ( ((mibstructIn->stsCardIndex) < D_stsCardIndex_output1) ) return ERROR;        		
        	if (  ((mibstructIn->stsCardIndex) >=  D_stsCardIndex_output9) &&
        		  ((mibstructIn->stsCardIndex) <=  D_stsCardIndex_output16 ) )return ERROR;
            break;
            
        case D_stsCardShelf_expansion_1:
        case D_stsCardShelf_expansion_2:
        case D_stsCardShelf_expansion_3:
        case D_stsCardShelf_expansion_4:
        	if (  ((mibstructIn->stsCardIndex) < D_stsCardIndex_output1)||
        		(mibstructIn->stsCardIndex) > D_stsCardIndex_output16  ) return ERROR;
			break;

        default:
            return ERROR;
    
    } 
    	
    /* get the global variable*/
#ifdef SM_SNMP_DEBUG
    printf("in smCardGet, get global variable\n");
#endif
    switch(mibstructIn->stsCardShelf){
        case D_stsCardShelf_main:

        	if (  ((mibstructIn->stsCardIndex) >=  D_stsCardIndex_output1) &&
        		  ((mibstructIn->stsCardIndex) <=  D_stsCardIndex_output8 ) ) {
            	thisCard=&outpCard.main[(mibstructIn->stsCardIndex)-1];
        	} else {
        		isOutputCard = SM_NO;
        		switch(  (mibstructIn->stsCardIndex) ){
        			case D_stsCardIndex_smu:
        				specialCard = &outpCard.smu;
        				break;
        			case D_stsCardIndex_top:
        				specialCard = &outpCard.top;
        				break;
        			case D_stsCardIndex_imu:
        				specialCard = &outpCard.imu;
        				break;
        			default:
        				return ERROR;
        				break;
        			
        		}
        	}
            break;
            
        case D_stsCardShelf_expansion_1:
			thisCard=&outpCard.exp1[(mibstructIn->stsCardIndex)-1];	
			break;
					       
        case D_stsCardShelf_expansion_2:
        	thisCard=&outpCard.exp2[(mibstructIn->stsCardIndex)-1];	
			break;
			
        case D_stsCardShelf_expansion_3:
        	thisCard=&outpCard.exp3[(mibstructIn->stsCardIndex)-1];	
			break;
			       
        case D_stsCardShelf_expansion_4:
        	thisCard=&outpCard.exp4[(mibstructIn->stsCardIndex)-1];	
			break;
			
        default:
            return ERROR;
            break;        

    } 

    /* get values from global variable */
    	if (isOutputCard == SM_YES){
    		/* this is normal output card case */
    		/*checkitout. if re-number*/
    		if (thisCard->status == OUTPUT_PRESENT){
	    		mibstructIn->stsCardType 		= thisCard->cardType + 1;
	        	mibstructIn->stsCardDescr 		= &(thisCard->cardDescr);
		        mibstructIn->stsCardSerial		= &(thisCard->cardSerial);
		        mibstructIn->stsCardHwVersion	= &(thisCard->cardHwVersion);
		        mibstructIn->stsCardCLEICode	= &(thisCard->cardCLEICode);
		        
		        mibstructIn->stsCardOperStatus = thisCard->status;  
		        mibstructIn->stsCardReset = D_stsCardReset_noAction;
	    	}else{
	    		return ERROR;
	    	}
		}else{
			
	    		mibstructIn->stsCardType 		= specialCard->cardType + 1;
	        	mibstructIn->stsCardDescr 		= &(specialCard->cardDescr);
		        mibstructIn->stsCardSerial		= &(specialCard->cardSerial);
		        mibstructIn->stsCardHwVersion	= &(specialCard->cardHwVersion);
		        mibstructIn->stsCardCLEICode	= &(specialCard->cardCLEICode);
		        
		        mibstructIn->stsCardOperStatus = specialCard->status;  
		        mibstructIn->stsCardReset = D_stsCardReset_noAction;
		}

#ifdef SM_SNMP_DEBUG
	printf("in smCardGet, good bye\n");
#endif
    return OK;
    
}



/********************************************************************
* function: STATUS_t smCardSet(stsCardEntry_t * mibstructIn)
* input parameter: [index]
        SR_INT32        stsCardShelf;
        SR_INT32        stsCardIndex;
  output parameter:
        SR_INT32        stsCardType;
        OctetString     *stsCardDescr;
        OctetString     *stsCardSerial;
        OctetString     *stsCardHwVersion;
        OctetString     *stsCardCLEICode;
        SR_INT32        stsCardOperStatus;
        SR_INT32        stsCardReset;
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/
STATUS_t smCardSet(stsCardEntry_t *mibstructIn){
    U8_t tmp;
    OUTPUT_CARD_PARA_t *thisCard;
    SPECIAL_CARD_PARA_t *specialCard;
    U8_t isOutputCard = 1; /* 1: normal output card; otherwise special card */
    
    
    /* sanity check*/
	    switch(mibstructIn->stsCardShelf){
	        case D_stsCardShelf_main:
	        	if ( ((mibstructIn->stsCardIndex) < D_stsCardIndex_output1) ) return ERROR;        		
	        	if (  ((mibstructIn->stsCardIndex) >=  D_stsCardIndex_output9) &&
	        		  ((mibstructIn->stsCardIndex) <=  D_stsCardIndex_output16 ) )return ERROR;
	            break;
	            
	        case D_stsCardShelf_expansion_1:
	        case D_stsCardShelf_expansion_2:
	        case D_stsCardShelf_expansion_3:
	        case D_stsCardShelf_expansion_4:
	        	if (  ((mibstructIn->stsCardIndex) < D_stsCardIndex_output1)||
	        		(mibstructIn->stsCardIndex) > D_stsCardIndex_output16  ) return ERROR;
				break;
	
	        default:
	            return ERROR;
	    
	    }



    /* get the global variable*/
    switch(mibstructIn->stsCardShelf){
        case D_stsCardShelf_main:

        	if (  ((mibstructIn->stsCardIndex) >=  D_stsCardIndex_output1) &&
        		  ((mibstructIn->stsCardIndex) <=  D_stsCardIndex_output8 ) ) {
            	thisCard=&outpCard.main[(mibstructIn->stsCardIndex)-1];
        	} else {
        		isOutputCard = 0;
        		switch(  (mibstructIn->stsCardIndex) ){
        			case D_stsCardIndex_smu:
        				specialCard = &outpCard.smu;
        				break;
        			case D_stsCardIndex_top:
        				specialCard = &outpCard.top;
        				break;
        			case D_stsCardIndex_imu:
        				specialCard = &outpCard.imu;
        				break;
        			default:
        				return ERROR;
        				break;
        			
        		}
        	}
            break;
            
        case D_stsCardShelf_expansion_1:
			thisCard=&outpCard.exp1[(mibstructIn->stsCardIndex)-1];	
			break;
					       
        case D_stsCardShelf_expansion_2:
        	thisCard=&outpCard.exp2[(mibstructIn->stsCardIndex)-1];	
			break;
			
        case D_stsCardShelf_expansion_3:
        	thisCard=&outpCard.exp3[(mibstructIn->stsCardIndex)-1];	
			break;
			       
        case D_stsCardShelf_expansion_4:
        	thisCard=&outpCard.exp4[(mibstructIn->stsCardIndex)-1];	
			break;
			
        default:
            return ERROR;
            break;        

    }   


		
   if (VALID(I_stsCardType, mibstructIn->valid)){
        if (isOutputCard == 1){
        	thisCard->cardType = mibstructIn->stsCardType - 1; 
		}else{
			printf("This card Type can not be configured.\n");
			/* specialCard->cardType = mibstructIn->stsCardType*/; 
		}
    }
    
    
   if (VALID(I_stsCardDescr, mibstructIn->valid)){
        if (isOutputCard == 1){
	        if (mibstructIn->stsCardDescr->length  < SM_CARD_DESCRIPTON_LEN ){
	            memcpy(thisCard->cardDescr.octet_ptr, mibstructIn->stsCardDescr->octet_ptr, mibstructIn->stsCardDescr->length);
	            thisCard->cardDescr.length = mibstructIn->stsCardDescr->length;
	        }else{
	            memcpy(thisCard->cardDescr.octet_ptr, mibstructIn->stsCardDescr->octet_ptr, SM_CARD_DESCRIPTON_LEN);
	            thisCard->cardDescr.length = SM_CARD_DESCRIPTON_LEN;        
	        }
		}else{
	        if (mibstructIn->stsCardDescr->length  < SM_CARD_DESCRIPTON_LEN ){
	            memcpy(specialCard->cardDescr.octet_ptr, mibstructIn->stsCardDescr->octet_ptr, mibstructIn->stsCardDescr->length);
	            specialCard->cardDescr.length = mibstructIn->stsCardDescr->length;
	        }else{
	            memcpy(specialCard->cardDescr.octet_ptr, mibstructIn->stsCardDescr->octet_ptr, SM_CARD_DESCRIPTON_LEN);
	            specialCard->cardDescr.length = SM_CARD_DESCRIPTON_LEN;        
	        }
		}

    }


   if (VALID(I_stsCardSerial, mibstructIn->valid)){
        if (isOutputCard == 1){
	        if (mibstructIn->stsCardSerial->length  < SM_CARD_SERIAL_LEN ){
	            memcpy(thisCard->cardSerial.octet_ptr, mibstructIn->stsCardSerial->octet_ptr, mibstructIn->stsCardSerial->length);
	            thisCard->cardSerial.length = mibstructIn->stsCardSerial->length;
	        }else{
	            memcpy(thisCard->cardSerial.octet_ptr, mibstructIn->stsCardSerial->octet_ptr, SM_CARD_SERIAL_LEN);
	            thisCard->cardSerial.length = SM_CARD_SERIAL_LEN;        
	        }
		}else{
	        if (mibstructIn->stsCardSerial->length  < SM_CARD_SERIAL_LEN ){
	            memcpy(specialCard->cardSerial.octet_ptr, mibstructIn->stsCardSerial->octet_ptr, mibstructIn->stsCardSerial->length);
	            specialCard->cardSerial.length = mibstructIn->stsCardSerial->length;
	        }else{
	            memcpy(specialCard->cardSerial.octet_ptr, mibstructIn->stsCardSerial->octet_ptr, SM_CARD_SERIAL_LEN);
	            specialCard->cardSerial.length = SM_CARD_SERIAL_LEN;        
	        }
		}

    }
    
  
  
   if (VALID(I_stsCardHwVersion, mibstructIn->valid)){
        if (isOutputCard == 1){
	        if (mibstructIn->stsCardHwVersion->length  < SM_CARD_HW_VERSION_LEN ){
	            memcpy(thisCard->cardHwVersion.octet_ptr, mibstructIn->stsCardHwVersion->octet_ptr, mibstructIn->stsCardHwVersion->length);
	            thisCard->cardHwVersion.length = mibstructIn->stsCardHwVersion->length;
	        }else{
	            memcpy(thisCard->cardHwVersion.octet_ptr, mibstructIn->stsCardHwVersion->octet_ptr, SM_CARD_HW_VERSION_LEN);
	            thisCard->cardHwVersion.length = SM_CARD_HW_VERSION_LEN;        
	        }	
		}else{
	        if (mibstructIn->stsCardHwVersion->length  < SM_CARD_HW_VERSION_LEN ){
	            memcpy(specialCard->cardHwVersion.octet_ptr, mibstructIn->stsCardHwVersion->octet_ptr, mibstructIn->stsCardHwVersion->length);
	            specialCard->cardHwVersion.length = mibstructIn->stsCardHwVersion->length;
	        }else{
	            memcpy(specialCard->cardHwVersion.octet_ptr, mibstructIn->stsCardHwVersion->octet_ptr, SM_CARD_HW_VERSION_LEN);
	            specialCard->cardHwVersion.length = SM_CARD_HW_VERSION_LEN;        
	        }
		}

    }
    
  
   if (VALID(I_stsCardCLEICode, mibstructIn->valid)){
        if (isOutputCard == 1){
	        if (mibstructIn->stsCardCLEICode->length  < SM_CARD_CLEICODE_LEN ){
	            memcpy(thisCard->cardCLEICode.octet_ptr, mibstructIn->stsCardCLEICode->octet_ptr, mibstructIn->stsCardCLEICode->length);
	            thisCard->cardCLEICode.length = mibstructIn->stsCardCLEICode->length;
	        }else{
	            memcpy(thisCard->cardCLEICode.octet_ptr, mibstructIn->stsCardCLEICode->octet_ptr, SM_CARD_CLEICODE_LEN);
	            thisCard->cardCLEICode.length = SM_CARD_CLEICODE_LEN;        
	        }
		}else{
	        if (mibstructIn->stsCardCLEICode->length  < SM_CARD_CLEICODE_LEN ){
	            memcpy(specialCard->cardCLEICode.octet_ptr, mibstructIn->stsCardCLEICode->octet_ptr, mibstructIn->stsCardCLEICode->length);
	            specialCard->cardCLEICode.length = mibstructIn->stsCardCLEICode->length;
	        }else{
	            memcpy(specialCard->cardCLEICode.octet_ptr, mibstructIn->stsCardCLEICode->octet_ptr, SM_CARD_CLEICODE_LEN);
	            specialCard->cardCLEICode.length = SM_CARD_CLEICODE_LEN;        
	        }
		}

    }
    
 
   if (VALID(I_stsCardOperStatus, mibstructIn->valid)){
        if (isOutputCard == 1){
	        /*checkitout. what does it mean for change of status*/
	        thisCard->status = mibstructIn->stsCardOperStatus;
		}else{
			specialCard->status = mibstructIn->stsCardOperStatus;
			
		}

   }
        


   if (VALID(I_stsCardReset, mibstructIn->valid)){
        switch(mibstructIn->stsCardShelf){
            case D_stsCardShelf_main:
            	switch(mibstructIn->stsCardIndex){
            		case D_stsCardIndex_output1:
            		case D_stsCardIndex_output2:
            		case D_stsCardIndex_output3:
            		case D_stsCardIndex_output4:
            		case D_stsCardIndex_output5:
            		case D_stsCardIndex_output6:
            		case D_stsCardIndex_output7:
            		case D_stsCardIndex_output8:
		                if (mibstructIn->stsCardReset == D_stsCardReset_reset){
		                    *(char *)MAIN_FPGA_W_REG_2 = (1 << ((mibstructIn->stsCardIndex)-1));               
		                }
		                break;
            		case D_stsCardIndex_smu:
            			/*checkitout*/
            			break;
            		case D_stsCardIndex_top:
            			/*checkitout*/
            			break;
            		case D_stsCardIndex_imu:
            			printf("Please type shelf reset\n");
            			break;
        			default:
        				return ERROR;
            			break;
            	}

            case D_stsCardShelf_expansion_1:
            case D_stsCardShelf_expansion_2:
            case D_stsCardShelf_expansion_3:
            case D_stsCardShelf_expansion_4:
                if ( (0 < (mibstructIn->stsCardIndex)  ) &&   ((mibstructIn->stsCardIndex) < 9 )   ){
                   tmp = ( 1 << ((mibstructIn->stsCardIndex)-1));
                   expFPGAAccess(((mibstructIn->stsCardShelf)-1), 0x00, 0x01, &tmp, FPGA_WRITE);
					           
					            
                 }else if ( (8 < (mibstructIn->stsCardIndex)  ) &&   ((mibstructIn->stsCardIndex) < 17 ) ){
                   tmp = ( 1 << ((mibstructIn->stsCardIndex)-9));
                   expFPGAAccess(((mibstructIn->stsCardShelf)-1), 0x00, 0x02, &tmp, FPGA_WRITE);
                    
                 }
                
                break;
            default:
                return ERROR;
                break;        

        }
    }

    return OK;
    
}


/********************************************************************
* function: STATUS_t smOutputCardGet(stsOutputCardEntry_t * mibstructIn)
* input parameter: 
        SR_INT32        stsOutputShelf;
        SR_INT32        stsOutputSlot;
* output parameter: 
        SR_INT32        stsOutputRedState;
        SR_INT32        stsOutputRedSwitchState;
        SR_INT32        stsOutputCRCCHK;
        SR_INT32        stsOutputCAS;
        SR_INT32        stsOutputSSM;
        SR_INT32        stsOutputSSMValue;
        OctetString     *stsOutputTimingFormat;
        char            valid[2];
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/
/*checkitout this function*/

STATUS_t smOutputCardGet(stsOutputCardEntry_t * mibstructIn)
{

OUTPUT_CARD_PARA_t *thisCard;
    switch ( mibstructIn->stsOutputShelf){
			case D_stsOutputShelf_main:
					thisCard=&outpCard.main[(mibstructIn->stsOutputSlot)-1];
					break;
			case D_stsOutputShelf_expansion_1:
					thisCard=&outpCard.exp1[(mibstructIn->stsOutputSlot)-1];	
					break;
			case D_stsOutputShelf_expansion_2:
					thisCard=&outpCard.exp2[(mibstructIn->stsOutputSlot)-1];	
					break;
			case D_stsOutputShelf_expansion_3:
					thisCard=&outpCard.exp3[(mibstructIn->stsOutputSlot)-1];	
					break;
			case D_stsOutputShelf_expansion_4:
					thisCard=&outpCard.exp4[(mibstructIn->stsOutputSlot)-1];	
					break;
				
			default:
				return ERROR;
		    
			
    }
     /* redundancy status */
   		mibstructIn->stsOutputRedState = thisCard->redundancy_state;
   		mibstructIn->stsOutputRedSwitchState = thisCard->redundancy_enable;
	    mibstructIn->stsOutputCRCCHK = thisCard->outputCRCCHK;
	    mibstructIn->stsOutputCAS = thisCard->outputCAS;
	    mibstructIn->stsOutputSSM = thisCard->outputSSM;
	    mibstructIn->stsOutputSSMValue = thisCard->outputSSMValue;
	    mibstructIn->stsOutputTimingFormat = &(thisCard->outputTimingFormat);

	return OK;
}

/********************************************************************
* function: STATUS_t smOutputCardSet(stsOutputCardEntry_t * mibstructIn)
* input parameter: 
        SR_INT32        stsOutputShelf;
        SR_INT32        stsOutputSlot;
        SR_INT32        stsOutputRedState;
        SR_INT32        stsOutputRedSwitchState;
        SR_INT32        stsOutputCRCCHK;
        SR_INT32        stsOutputCAS;
        SR_INT32        stsOutputSSM;
        SR_INT32        stsOutputSSMValue;
        OctetString     *stsOutputTimingFormat;
        char            valid[2];
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/
/*checkitout if any real action missing after this snmp action func*/
STATUS_t smOutputCardSet(stsOutputCardEntry_t * mibstructIn)
{
    
OUTPUT_CARD_PARA_t *thisCard;   
 
 		switch ( mibstructIn->stsOutputShelf){
			case D_stsOutputShelf_main:
					thisCard=&outpCard.main[(mibstructIn->stsOutputSlot)-1];
					break;
			case D_stsOutputShelf_expansion_1:
					thisCard=&outpCard.exp1[(mibstructIn->stsOutputSlot)-1];	
					break;
			case D_stsOutputShelf_expansion_2:
					thisCard=&outpCard.exp2[(mibstructIn->stsOutputSlot)-1];	
					break;
			case D_stsOutputShelf_expansion_3:
					thisCard=&outpCard.exp3[(mibstructIn->stsOutputSlot)-1];	
					break;
			case D_stsOutputShelf_expansion_4:
					thisCard=&outpCard.exp4[(mibstructIn->stsOutputSlot)-1];	
					break;
				
			default:
				return ERROR;
		    
			
			}

    
   /* redundancy status */
   if (VALID(I_stsOutputRedState, mibstructIn->valid)){
   		thisCard->redundancy_state = mibstructIn->stsOutputRedState;
   }
   
   /* toggle redundancy enable or disable */
   if (VALID(I_stsOutputRedSwitchState, mibstructIn->valid)){
	    thisCard->redundancy_enable = mibstructIn->stsOutputRedSwitchState;
   	}
   
   if (VALID(I_stsOutputCRCCHK, mibstructIn->valid)){
   		thisCard->outputCRCCHK = mibstructIn->stsOutputCRCCHK;
   		
   	}
   
   if (VALID(I_stsOutputCAS, mibstructIn->valid)){
	    thisCard->outputCAS = mibstructIn->stsOutputCAS;
		}
   
   if (VALID(I_stsOutputSSM, mibstructIn->valid)){
	     thisCard->outputSSM = mibstructIn->stsOutputSSM;
   	}
   
   if (VALID(I_stsOutputSSMValue, mibstructIn->valid)){
	    thisCard->outputSSMValue = mibstructIn->stsOutputSSMValue;
	   }

   if (VALID(I_stsOutputTimingFormat, mibstructIn->valid)){

		if (mibstructIn->stsOutputTimingFormat->length  < SM_CARD_TIMINGFORMAT_LEN){            
            memcpy(thisCard->outputTimingFormat.octet_ptr,
            mibstructIn->stsOutputTimingFormat->octet_ptr, mibstructIn->stsOutputTimingFormat->length);
            thisCard->outputTimingFormat.length   =  mibstructIn->stsOutputTimingFormat->length;
        }else{
        	memcpy(thisCard->outputTimingFormat.octet_ptr,
                mibstructIn->stsOutputTimingFormat->octet_ptr, SM_CARD_TIMINGFORMAT_LEN);
				thisCard->outputTimingFormat.length   =  SM_CARD_TIMINGFORMAT_LEN;
        }
   	}

	return OK;
}

/********************************************************************
* function: STATUS_t smOutputPortGet(stsOutputPortSettingEntry_t *mibstructIn)
* input parameter: 
        SR_INT32        stsOutputPortSettingShelf;
        SR_INT32        stsOutputPortSettingSlot;
        SR_INT32        stsOutputPortSettingPort;
        SR_INT32        stsOutputPortSettingType;
        OctetString     *stsOutputPortSettingName;
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/

STATUS_t smOutputPortGet(stsOutputPortSettingEntry_t *mibstructIn)
{
	OUTPUT_CARD_PARA_t *thisCard;   

		switch ( mibstructIn->stsOutputPortSettingShelf){
			case D_stsOutputPortSettingShelf_main:
					thisCard=&outpCard.main[(mibstructIn->stsOutputPortSettingSlot)-1];
					break;
					
			case D_stsOutputPortSettingShelf_expansion_1:
					thisCard=&outpCard.exp1[(mibstructIn->stsOutputPortSettingSlot)-1];	
					break;
					
			case D_stsOutputPortSettingShelf_expansion_2:
					thisCard=&outpCard.exp2[(mibstructIn->stsOutputPortSettingSlot)-1];	
					break;
			case D_stsOutputPortSettingShelf_expansion_3:
					thisCard=&outpCard.exp3[(mibstructIn->stsOutputPortSettingSlot)-1];	
					break;
			case D_stsOutputPortSettingShelf_expansion_4:
					thisCard=&outpCard.exp4[(mibstructIn->stsOutputPortSettingSlot)-1];	
					break;
				
			default:
				return ERROR;
		    
			
			}


	    mibstructIn->stsOutputPortSettingType = 
	    		thisCard->portType[(mibstructIn->stsOutputPortSettingPort)-1];
	    
	    mibstructIn->stsOutputPortSettingName = 
	    	&( thisCard->portName[(mibstructIn->stsOutputPortSettingPort)-1] );



    return OK;
}
/********************************************************************
* function: STATUS_t smOutputPortSet(stsOutputPortSettingEntry_t *mibstructIn)
* input parameter: 
        SR_INT32        stsOutputPortSettingShelf;
        SR_INT32        stsOutputPortSettingSlot;
        SR_INT32        stsOutputPortSettingPort;
        SR_INT32        stsOutputPortSettingType;
        OctetString     *stsOutputPortSettingName;
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/
STATUS_t smOutputPortSet(stsOutputPortSettingEntry_t *mibstructIn)
{
	OUTPUT_CARD_PARA_t *thisCard;   
 
 		switch ( mibstructIn->stsOutputPortSettingShelf){
			case D_stsOutputPortSettingShelf_main:
					thisCard=&outpCard.main[(mibstructIn->stsOutputPortSettingSlot)-1];
					break;	
			case D_stsOutputPortSettingShelf_expansion_1:
					thisCard=&outpCard.exp1[(mibstructIn->stsOutputPortSettingSlot)-1];	
					break;		
			case D_stsOutputPortSettingShelf_expansion_2:
					thisCard=&outpCard.exp2[(mibstructIn->stsOutputPortSettingSlot)-1];	
					break;
			case D_stsOutputPortSettingShelf_expansion_3:
					thisCard=&outpCard.exp3[(mibstructIn->stsOutputPortSettingSlot)-1];	
					break;
			case D_stsOutputPortSettingShelf_expansion_4:
					thisCard=&outpCard.exp4[(mibstructIn->stsOutputPortSettingSlot)-1];	
					break;	
			default:
				return ERROR;
		}
    

   if (VALID(I_stsOutputPortSettingType, mibstructIn->valid)){

	    thisCard->portType[(mibstructIn->stsOutputPortSettingPort)-1]
	    		= mibstructIn->stsOutputPortSettingType;
	    		
	    
   	}
   if (VALID(I_stsOutputPortSettingName, mibstructIn->valid)){
   	
		if (mibstructIn->stsOutputPortSettingName->length  < SM_PORT_PORTNAME_LEN){            
            memcpy(thisCard->portName[(mibstructIn->stsOutputPortSettingPort)-1].octet_ptr,
            	mibstructIn->stsOutputPortSettingName->octet_ptr, mibstructIn->stsOutputPortSettingName->length);
            thisCard->portName[(mibstructIn->stsOutputPortSettingPort)-1].length   =  mibstructIn->stsOutputPortSettingName->length;
        }else{
        	memcpy(thisCard->portName[(mibstructIn->stsOutputPortSettingPort)-1].octet_ptr,
                mibstructIn->stsOutputPortSettingName->octet_ptr, SM_PORT_PORTNAME_LEN);
			thisCard->portName[(mibstructIn->stsOutputPortSettingPort)-1].length   =  SM_PORT_PORTNAME_LEN;
        } 
	    
   	}
		
    
    outpPortConfig(	(mibstructIn->stsOutputPortSettingShelf - 1), 
    				(mibstructIn->stsOutputPortSettingSlot)-1, 
    				(mibstructIn->stsOutputPortSettingPort)-1, 
    				mibstructIn->stsOutputPortSettingType,
                   	0);
                   	/* checkitout. last argument seems not right*/

     return OK;
}

/********************************************************************
* function: STATUS_t smOutputAlarmGet(stsOutputAlarmEntry_t *mibstructIn)
* input parameter:
    key 
        SR_INT32        stsOutputAlarmSettingShelf;
        SR_INT32        stsOutputAlarmSettingSlot;
        SR_INT32        stsOutputAlarmSettingPort;
        SR_INT32        stsOutputAlarmSettingType;
    setting
        SR_INT32        stsOutputAlarmSettingSeverity;
        SR_INT32        stsOutputAlarmSeverity;
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/


STATUS_t smOutputAlarmGet(stsOutputAlarmEntry_t *mibstructIn){

    OUTPUT_CARD_PARA_t *thiscard;
    
    switch ( mibstructIn->stsOutputAlarmSettingShelf){
		case D_stsOutputAlarmSettingShelf_main:
		    thiscard=&outpCard.main[(mibstructIn->stsOutputAlarmSettingSlot)-1];
			break;
		case D_stsOutputAlarmSettingShelf_expansion_1:
		    thiscard=&outpCard.exp1[(mibstructIn->stsOutputAlarmSettingSlot)-1];
		    break;
		case D_stsOutputAlarmSettingShelf_expansion_2:
		    thiscard=&outpCard.exp2[(mibstructIn->stsOutputAlarmSettingSlot)-1];
		    break;
		case D_stsOutputAlarmSettingShelf_expansion_3:	
		    thiscard=&outpCard.exp3[(mibstructIn->stsOutputAlarmSettingSlot)-1];
		    break;
		case D_stsOutputAlarmSettingShelf_expansion_4:
		    thiscard=&outpCard.exp4[(mibstructIn->stsOutputAlarmSettingSlot)-1];
		    break;
			
		default:
			return ERROR;
	}

    switch ( mibstructIn->stsOutputAlarmSettingType){
        case D_stsOutputAlarmSettingType_lossClock:
            mibstructIn->stsOutputAlarmSettingSeverity = 
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_lossClock;
            break;
            
        case D_stsOutputAlarmSettingType_tiu_act:
            mibstructIn->stsOutputAlarmSettingSeverity = 
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_act;
            break;
            
        case D_stsOutputAlarmSettingType_tiu_los:
            mibstructIn->stsOutputAlarmSettingSeverity = 
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_los;
            break;
            
        case D_stsOutputAlarmSettingType_tiu_t_tie:	
            mibstructIn->stsOutputAlarmSettingSeverity = 
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_t_tie;
            break;
            
        case D_stsOutputAlarmSettingType_tiu_t_mtie:
            mibstructIn->stsOutputAlarmSettingSeverity = 
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_t_mtie;
            break;
            
        case D_stsOutputAlarmSettingType_tiu_aisp:				
            mibstructIn->stsOutputAlarmSettingSeverity = 
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_aisp;
            break;
            
        case D_stsOutputAlarmSettingType_tiu_lof:
            mibstructIn->stsOutputAlarmSettingSeverity = 
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_lof;
            break;
            
        case D_stsOutputAlarmSettingType_tiu_t_cs:
            mibstructIn->stsOutputAlarmSettingSeverity = 
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_t_cs;
            break;
            
        default:
            return ERROR;
    
    }	    

    mibstructIn->stsOutputAlarmSeverity = portAlarmSummary( thiscard, ((mibstructIn->stsOutputAlarmSettingPort)-1) );
    
        
    return OK;
    
    
    
    
}


/********************************************************************
* function: STATUS_t smOutputAlarmSet(stsOutputAlarmEntry_t *mibstructIn)
* input parameter:
    key 
        SR_INT32        stsOutputAlarmSettingShelf;
        SR_INT32        stsOutputAlarmSettingSlot;
        SR_INT32        stsOutputAlarmSettingPort;
        SR_INT32        stsOutputAlarmSettingType;
    setting
        SR_INT32        stsOutputAlarmSettingSeverity;
        SR_INT32        stsOutputAlarmSeverity;
* return
*  	OK:  
*  	ERROR: 
*********************************************************************/
STATUS_t smOutputAlarmSet(stsOutputAlarmEntry_t *mibstructIn){
    OUTPUT_CARD_PARA_t *thiscard;
    
    switch ( mibstructIn->stsOutputAlarmSettingShelf){
		case D_stsOutputAlarmSettingShelf_main:
		    thiscard=&outpCard.main[(mibstructIn->stsOutputAlarmSettingSlot)-1];
			break;
		case D_stsOutputAlarmSettingShelf_expansion_1:
		    thiscard=&outpCard.exp1[(mibstructIn->stsOutputAlarmSettingSlot)-1];
		    break;
		case D_stsOutputAlarmSettingShelf_expansion_2:
		    thiscard=&outpCard.exp2[(mibstructIn->stsOutputAlarmSettingSlot)-1];
		    break;
		case D_stsOutputAlarmSettingShelf_expansion_3:	
		    thiscard=&outpCard.exp3[(mibstructIn->stsOutputAlarmSettingSlot)-1];
		    break;
		case D_stsOutputAlarmSettingShelf_expansion_4:
		    thiscard=&outpCard.exp4[(mibstructIn->stsOutputAlarmSettingSlot)-1];
		    break;
			
		default:
			return ERROR;
	}
	
    if (VALID(I_stsOutputAlarmSettingType, mibstructIn->valid)){
        switch ( mibstructIn->stsOutputAlarmSettingType){
            case D_stsOutputAlarmSettingType_lossClock:
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_lossClock = 
                    mibstructIn->stsOutputAlarmSettingSeverity;
                break;
                
            case D_stsOutputAlarmSettingType_tiu_act:
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_act = 
                    mibstructIn->stsOutputAlarmSettingSeverity;
                break;
                
            case D_stsOutputAlarmSettingType_tiu_los:
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_los = 
                    mibstructIn->stsOutputAlarmSettingSeverity;
                break;
                
            case D_stsOutputAlarmSettingType_tiu_t_tie:
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_t_tie = 
                    mibstructIn->stsOutputAlarmSettingSeverity;
                break;
                
            case D_stsOutputAlarmSettingType_tiu_t_mtie:
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_t_mtie = 
                    mibstructIn->stsOutputAlarmSettingSeverity;
                break;
                
            case D_stsOutputAlarmSettingType_tiu_aisp:
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_aisp = 
                    mibstructIn->stsOutputAlarmSettingSeverity;
                break;
                
            case D_stsOutputAlarmSettingType_tiu_lof:
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_lof = 
                    mibstructIn->stsOutputAlarmSettingSeverity;
                break;
                
            case D_stsOutputAlarmSettingType_tiu_t_cs:
                thiscard->portAlarmSetting[(mibstructIn->stsOutputAlarmSettingPort)-1].outputAlarm_tiu_t_cs = 
                    mibstructIn->stsOutputAlarmSettingSeverity;
                break;
                
            default:
                return ERROR;
        
        }
    }
    return OK;
	
}








