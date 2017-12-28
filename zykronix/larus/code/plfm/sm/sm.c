/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: sm.c
 *
 *Define all Event objects for SM Module
 *
 *Note:
 *
 *Author(s):
 *  shuming
 *
 ********************************************************************
 * $Id: sm.c,v 1.29 2006/10/05 00:41:23 jim Exp $
 * $Source: /cvsroot/larus5820/code/plfm/sm/sm.c,v $
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

/* Global variables declaration */
		SHELFS_t shelves[NUMBER_SHELVES];
		OUTPUT_CARD_t outpCard;
		STATUS_t smReady = FALSE;
		U8_t shelfIntCnt = 0;
		STATUS_t outpMainValid = ERROR;
		
/* Global variables declaration - snmp trap use */
		stsShelfAdded_t 		smTrapMib_shelfAdded;
		stsShelfRemoved_t		smTrapMib_shelfRemoved;
		stsOutputCardConfigChange_t		smTrapMib_outputCardConfig;


/* Global variables declaration - extern */
		extern SYS_CONFIG_T sysConfig;

/* Global routine declaration */
        STATUS_t shelfTask(void);
		void outpCardInit(SHELF_t shelf_index);
		STATUS_t outpCardUpdate(SHELF_t shelf_index);
		void shelfUpdateInt(void);
		S8_t cardTypeGet(SHELF_t shelf_index, U8_t slot);
		void outpPortConfig(SHELF_t shelf_index, int slot, int port, int type,
		                    int framing);
		void outpAlarmCheck(SHELF_t shelf_index);
		STATUS_t smDataAccess(char *fname, int mode, int dataType);
		STATUS_t SMUPresent(void);
		void outpPortActivate(SHELF_t shelf_index, int slot, U8_t mode);
		void ABSlipCheck();
		

		


/**************************************************************************************
* function: void shelfTask() 
*  input 
*			void
*			
*  return 
*     STATUS_t
***************************************************************************************/
STATUS_t shelfTask(void)
{
    U8_t presentStatus, slot, card, alarm, i, ints, tmp;
    U32_t *dumPtr = NULL;
	stsCardEntry_t snmpTrapCardReply;
	STATUS_t ret;

	#ifdef SM_DEBUG
		printf("in sm, task start\n");
	#endif

    /* reset all output cards on Main Shelf */
    *(U8_t *) MAIN_FPGA_W_REG_2 = 0xFF;

    /* reset all output cards on EXPANSION Shelf */
    tmp = 0;
    expFPGAAccess(EXP1_SHELF, 0x00, 0x01, &tmp, FPGA_WRITE);
    expFPGAAccess(EXP1_SHELF, 0x00, 0x02, &tmp, FPGA_WRITE);
    expFPGAAccess(EXP2_SHELF, 0x00, 0x01, &tmp, FPGA_WRITE);
    expFPGAAccess(EXP2_SHELF, 0x00, 0x02, &tmp, FPGA_WRITE);
    expFPGAAccess(EXP3_SHELF, 0x00, 0x01, &tmp, FPGA_WRITE);
    expFPGAAccess(EXP3_SHELF, 0x00, 0x02, &tmp, FPGA_WRITE);
    expFPGAAccess(EXP4_SHELF, 0x00, 0x01, &tmp, FPGA_WRITE);
    expFPGAAccess(EXP4_SHELF, 0x00, 0x02, &tmp, FPGA_WRITE);

    /* select OSC & CLKA */
    *(U8_t *) MAIN_FPGA_W_REG_1 |= 0x00;


#ifdef SM_DEBUG
	printf("in sm, task init\n");
#endif
    ret=shelfInit();
    if (ret == ERROR){return ERROR;}
    smReady = TRUE;
#ifdef SM_DEBUG
	printf("in sm, task monitoring loop\n");
#endif

    ledOp(LED_STATUS_PLUS, LED_ON, 0);
	ledOp(LED_STATUS_MINUS, LED_OFF, 0);

    for (;;) {                  /* forever loop */

        /* monitor shelf, card, and alarm events */
        if (shelfIntCnt > 0) {

            /* check shelf interrup */
            ints = *(U8_t *) MAIN_FPGA_R_REG_4; 
            if (ints & 0x0F) {
                /* interrupt from expansion shelf*/

                /* check shelf present */
                presentStatus = *(U8_t *) MAIN_FPGA_R_REG_2;
                for (i = 1; i < NUMBER_SHELVES; i++) {
                    if (presentStatus & (1 << (i - 1))) {
                        if (shelves[i].present == FALSE) {

                            /* add a new shelf */
                            shelves[i].present = TRUE;
                            
                            /* sendAlarm(a new shelf is added); */
                           SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDSHELF), i);
					        /* checkitout 
						        TrapEventLog(smShelfAddEvent_TRAP, i,  0, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDSHELF), i);
					        */
						#ifdef SM_DEBUG
								printf("in sm, new shelf [%d] added\n", i);
						#endif
 
                           
                            outpCardInit(i);

                            /* Save configs to Flash */
                            smDataAccess("smcard.dat", FLASH_WRITE,
                                         SM_OUTPUT_CARD);
                            if (i == EXP1_SHELF)
                                smDataAccess("smexp1.out", FLASH_WRITE,
                                             SM_EXP1_OUTPUT_PORT);

                            else if (i == EXP2_SHELF)
                                smDataAccess("smexp2.out", FLASH_WRITE,
                                             SM_EXP1_OUTPUT_PORT);

                            else if (i == EXP3_SHELF)
                                smDataAccess("smexp3.out", FLASH_WRITE,
                                             SM_EXP1_OUTPUT_PORT);

                            else if (i == EXP4_SHELF)
                                smDataAccess("smexp4.out", FLASH_WRITE,
                                             SM_EXP1_OUTPUT_PORT);
                        }
                    } else {
                        if (shelves[i].present == TRUE) {

                            /* shelf is removed */
                            shelves[i].present = FALSE;
                            
                            /* sendAlarm(shelf is moved); */
				              SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVESHELF), i);
				              /* checkitout 
				              	TrapEventLog(smShelfRemoveEvent_TRAP, i,  0, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVESHELF), i);
						*/
						#ifdef SM_DEBUG
							printf("in sm, shelf [%d] removed\n", i);
						#endif


                        }
                    }
                }

                /* check alarm status on output card */
                alarm = *(U8_t *) MAIN_FPGA_R_REG_3;
                for (i = 1; i < NUMBER_SHELVES; i++) {
                    if (alarm & (1 << (i - 1))) {
                        if (shelves[i].present == TRUE)
                            outpAlarmCheck(i);
                    }
                }
            }

            /* check card interrupt on Main Shelf */
            if ((ints & 0x80) == 0x80) {
                /* interrupt from main shelf*/
                
                outpAlarmCheck(MAIN_SHELF);
                outpCardUpdate(MAIN_SHELF);
            }

            /* check card interrupt on Expansion Shelf */
            for (i = 1; NUMBER_SHELVES; i++) {
                if (shelves[i].present == TRUE) {
                    outpCardUpdate(i);
                }
            }

            /* zero out interrupt count */
            shelfIntCnt = 0;
        }

        /* check A/B clock Slip */
        ABSlipCheck();

        /* update output SSM */
        outpSSMUpdate();

				/* power monitor has been removed to emm module */
        taskDelay(1);
    }
}

/**************************************************************************************
* function: STATUS_t shelfInit() 
*  description
*			1. get the existing setting or use default
*			2. 
*  input 
*			void
*			
*  return 
*     STATUS_t
***************************************************************************************/
STATUS_t shelfInit()
{
    int i, j, thisshelf;
    U8_t shelf_index, slot, port;
    STATUS_t outpExp1Valid, outpExp2Valid, outpExp3Valid, outpExp4Valid;
    static U8_t malloc4octetYet=0;
    
    
    VOIDFUNCPTR sm_interrupt = (VOIDFUNCPTR) SM_INT_VECTOR;  /* checkitout */
     

    /* init those global variable */
    smVariableInit();

	/* int those special card. checkitout. this should be saved to flash */
		thisshelf = 0;
		outpCard.smu.cardType				=	D_stsCardIndex_smu;
        outpCard.smu.cardDescr.length 		= sprintf(outpCard.smu.cardDescr.octet_ptr,
                                              	"smu");
        outpCard.smu.cardSerial.length 		= sprintf(outpCard.smu.cardSerial.octet_ptr,
            									"%d00", thisshelf);
        outpCard.smu.cardHwVersion.length 	= sprintf(outpCard.smu.cardHwVersion.octet_ptr,
            									"");
        outpCard.smu.cardCLEICode.length 	= sprintf(outpCard.smu.cardCLEICode.octet_ptr,
                                                 "CLEI_%01d", thisshelf);

		outpCard.top.cardType				=	D_stsCardIndex_top;
        outpCard.top.cardDescr.length 		= sprintf(outpCard.top.cardDescr.octet_ptr,
                                              	"top");
        outpCard.top.cardSerial.length 		= sprintf(outpCard.top.cardSerial.octet_ptr,
            									"%d00", thisshelf);
        outpCard.top.cardHwVersion.length 	= sprintf(outpCard.top.cardHwVersion.octet_ptr,
            									"");
        outpCard.top.cardCLEICode.length 	= sprintf(outpCard.top.cardCLEICode.octet_ptr,
                                                 "CLEI_%01d", thisshelf);
                                                 
        outpCard.imu.cardType				=	D_stsCardIndex_imu;                                         
        outpCard.imu.cardDescr.length 		= sprintf(outpCard.imu.cardDescr.octet_ptr,
                                              	"imu");
        outpCard.imu.cardSerial.length 		= sprintf(outpCard.imu.cardSerial.octet_ptr,
            									"%d00", thisshelf);
        outpCard.imu.cardHwVersion.length 	= sprintf(outpCard.imu.cardHwVersion.octet_ptr,
            									"");
        outpCard.imu.cardCLEICode.length 	= sprintf(outpCard.imu.cardCLEICode.octet_ptr,
                                                 "CLEI_%01d", thisshelf);
                                                 
                                                 

    /* read output card info from Flash or set default */
    if (smDataAccess("smcard.dat", FLASH_READ, SM_OUTPUT_CARD) == ERROR) {

        /* record does not exist */
        
        for (slot = 0; slot < 8; slot++) {
            outpCard.main[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpCard.main[slot].cardType = OUTPUT_NULL;
            outpCard.main[slot].redundancy_state = REDUNDANCY_STANDBY;
             
			/*
				Plase be aware the init lenght. Do not go beyound.
				#define	SM_CARD_DESCRIPTON_LEN	13
				#define	SM_CARD_SERIAL_LEN		13
				#define	SM_CARD_HW_VERSION_LEN	13
				#define	SM_CARD_CLEICODE_LEN	13
			*/
			thisshelf = 0;
            outpCard.main[slot].cardDescr.length 		= sprintf(outpCard.main[slot].cardDescr.octet_ptr,
                                                    		"outpcard_%01d-%02d", thisshelf, slot);
            outpCard.main[slot].cardSerial.length 		= sprintf(outpCard.main[slot].cardSerial.octet_ptr,
            												"%d-%d-00", thisshelf, slot);
            outpCard.main[slot].cardHwVersion.length 	= sprintf(outpCard.main[slot].cardHwVersion.octet_ptr,
            												"");
            outpCard.main[slot].cardCLEICode.length 	= sprintf(outpCard.main[slot].cardCLEICode.octet_ptr,
                                                    		"CLEI_%01d-%02d", thisshelf, slot);

                
        }
        for (slot = 0; slot < 16; slot++) {
            outpCard.exp1[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpCard.exp1[slot].cardType = OUTPUT_NULL;
            outpCard.exp1[slot].redundancy_state = REDUNDANCY_STANDBY;
			/*
				Plase be aware the init lenght. Do not go beyound.
				#define	SM_CARD_DESCRIPTON_LEN	13
				#define	SM_CARD_SERIAL_LEN		13
				#define	SM_CARD_HW_VERSION_LEN	13
				#define	SM_CARD_CLEICODE_LEN	13
			*/
			thisshelf = 1;
            outpCard.exp1[slot].cardDescr.length 		= sprintf(outpCard.exp1[slot].cardDescr.octet_ptr,
															"outpcard_%01d-%02d", thisshelf, slot);
            outpCard.exp1[slot].cardSerial.length 		= sprintf(outpCard.exp1[slot].cardSerial.octet_ptr,
            												"%01d-%02d-00", thisshelf, slot);
            outpCard.exp1[slot].cardHwVersion.length 	= sprintf(outpCard.exp1[slot].cardHwVersion.octet_ptr,
            												"");
            outpCard.exp1[slot].cardCLEICode.length 	= sprintf(outpCard.exp1[slot].cardCLEICode.octet_ptr,
                                                    		"CLEI_%01d-%02d", thisshelf, slot);
            
        }
        for (slot = 0; slot < 16; slot++) {
            outpCard.exp2[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpCard.exp2[slot].cardType = OUTPUT_NULL;
            outpCard.exp2[slot].redundancy_state = REDUNDANCY_STANDBY;

			/*
				Plase be aware the init lenght. Do not go beyound.
				#define	SM_CARD_DESCRIPTON_LEN	13
				#define	SM_CARD_SERIAL_LEN		13
				#define	SM_CARD_HW_VERSION_LEN	13
				#define	SM_CARD_CLEICODE_LEN	13
			*/
			thisshelf = 2;
            outpCard.exp1[slot].cardDescr.length 		= sprintf(outpCard.exp1[slot].cardDescr.octet_ptr,
															"outpcard_%01d-%02d", thisshelf, slot);
            outpCard.exp1[slot].cardSerial.length 		= sprintf(outpCard.exp1[slot].cardSerial.octet_ptr,
            												"%01d-%02d-00", thisshelf, slot);
            outpCard.exp1[slot].cardHwVersion.length 	= sprintf(outpCard.exp1[slot].cardHwVersion.octet_ptr,
            												"");
            outpCard.exp1[slot].cardCLEICode.length 	= sprintf(outpCard.exp1[slot].cardCLEICode.octet_ptr,
                                                    		"CLEI_%01d-%02d", thisshelf, slot);
            
        }
        for (slot = 0; slot < 16; slot++) {
            outpCard.exp3[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpCard.exp3[slot].cardType = OUTPUT_NULL;
            outpCard.exp3[slot].redundancy_state = REDUNDANCY_STANDBY;
			/*
				Plase be aware the init lenght. Do not go beyound.
				#define	SM_CARD_DESCRIPTON_LEN	13
				#define	SM_CARD_SERIAL_LEN		13
				#define	SM_CARD_HW_VERSION_LEN	13
				#define	SM_CARD_CLEICODE_LEN	13
			*/
			thisshelf = 3;
            outpCard.exp1[slot].cardDescr.length 		= sprintf(outpCard.exp1[slot].cardDescr.octet_ptr,
															"outpcard_%01d-%02d", thisshelf, slot);
            outpCard.exp1[slot].cardSerial.length 		= sprintf(outpCard.exp1[slot].cardSerial.octet_ptr,
            												"%01d-%02d-00", thisshelf, slot);
            outpCard.exp1[slot].cardHwVersion.length 	= sprintf(outpCard.exp1[slot].cardHwVersion.octet_ptr,
            												"");
            outpCard.exp1[slot].cardCLEICode.length 	= sprintf(outpCard.exp1[slot].cardCLEICode.octet_ptr,
                                                    		"CLEI_%01d-%02d", thisshelf, slot);
        }
        for (slot = 0; slot < 16; slot++) {
            outpCard.exp4[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpCard.exp4[slot].cardType = OUTPUT_NULL;
            outpCard.exp4[slot].redundancy_state = REDUNDANCY_STANDBY;
			/*
				Plase be aware the init lenght. Do not go beyound.
				#define	SM_CARD_DESCRIPTON_LEN	13
				#define	SM_CARD_SERIAL_LEN		13
				#define	SM_CARD_HW_VERSION_LEN	13
				#define	SM_CARD_CLEICODE_LEN	13
			*/
			thisshelf = 4;
            outpCard.exp1[slot].cardDescr.length 		= sprintf(outpCard.exp1[slot].cardDescr.octet_ptr,
															"outpcard_%01d-%02d", thisshelf, slot);
            outpCard.exp1[slot].cardSerial.length 		= sprintf(outpCard.exp1[slot].cardSerial.octet_ptr,
            												"%01d-%02d-00", thisshelf, slot);
            outpCard.exp1[slot].cardHwVersion.length 	= sprintf(outpCard.exp1[slot].cardHwVersion.octet_ptr,
            												"");
            outpCard.exp1[slot].cardCLEICode.length 	= sprintf(outpCard.exp1[slot].cardCLEICode.octet_ptr,
                                                    		"CLEI_%01d-%02d", thisshelf, slot);
        }

        /* create record */
        smDataAccess("smcard.dat", FLASH_WRITE, SM_OUTPUT_CARD);
    }

    /* read Output port config in main shelf from Flash */
	    if ((outpMainValid = smDataAccess("smmain.out", FLASH_READ,  SM_MAIN_OUTPUT_PORT)) == ERROR) {
	
	        /* record does not exist */
	        for (slot = 0; slot < 8; slot++) {
	            for (port = 0; port < NUM_PORTS; port++) {
	                outpCard.main[slot].portType[port] = OUTPUT_NULL;
	            }
	            outpCard.main[slot].reg0 = 0;
	            outpCard.main[slot].reg1 = 0;
	            outpCard.main[slot].reg2 = 0;
	            outpCard.main[slot].reg3 = 0;
	            outpCard.main[slot].reg4 = 0;
	            outpCard.main[slot].reg5 = 0;
	        }
	
	        /* create record */
	        smDataAccess("smmain.out", FLASH_WRITE, SM_MAIN_OUTPUT_PORT);
	    }
    /* read Output port config in expansion shelf 1 from Flash */
	    if ((outpExp1Valid =
	         smDataAccess("smexp1.out", FLASH_READ,
	                      SM_EXP1_OUTPUT_PORT)) == ERROR) {
	
	        /* record does not exist */
	        for (slot = 0; slot < 16; slot++) {
	            for (port = 0; port < NUM_PORTS; port++) {
	                outpCard.exp1[slot].portType[port] = OUTPUT_NULL;
	            }
	            outpCard.exp1[slot].reg0 = 0;
	            outpCard.exp1[slot].reg1 = 0;
	            outpCard.exp1[slot].reg2 = 0;
	            outpCard.exp1[slot].reg3 = 0;
	            outpCard.exp1[slot].reg4 = 0;
	            outpCard.exp1[slot].reg5 = 0;
	        }
	
	        /* create record */
	        smDataAccess("smexp1.out", FLASH_WRITE, SM_EXP1_OUTPUT_PORT);
	    }
    /* read Output port config in expansion shelf 2 from Flash */
	    if ((outpExp2Valid =
	         smDataAccess("smexp2.out", FLASH_READ,
	                      SM_EXP2_OUTPUT_PORT)) == ERROR) {
	
	        /* record does not exist */
	        for (slot = 0; slot < 16; slot++) {
	            for (port = 0; port < NUM_PORTS; port++) {
	                outpCard.exp2[slot].portType[port] = OUTPUT_NULL;
	            }
	            outpCard.exp2[slot].reg0 = 0;
	            outpCard.exp2[slot].reg1 = 0;
	            outpCard.exp2[slot].reg2 = 0;
	            outpCard.exp2[slot].reg3 = 0;
	            outpCard.exp2[slot].reg4 = 0;
	            outpCard.exp2[slot].reg5 = 0;
	        }
	
	        /* create record */
	        smDataAccess("smexp2.out", FLASH_WRITE, SM_EXP2_OUTPUT_PORT);
	    }
    /* read Output port config in expansion shelf 3 from Flash */
	    if ((outpExp3Valid =
	         smDataAccess("smexp3.out", FLASH_READ,
	                      SM_EXP3_OUTPUT_PORT)) == ERROR) {
	
	        /* record does not exist */
	        for (slot = 0; slot < 16; slot++) {
	            for (port = 0; port < NUM_PORTS; port++) {
	                outpCard.exp3[slot].portType[port] = OUTPUT_NULL;
	            }
	            outpCard.exp3[slot].reg0 = 0;
	            outpCard.exp3[slot].reg1 = 0;
	            outpCard.exp3[slot].reg2 = 0;
	            outpCard.exp3[slot].reg3 = 0;
	            outpCard.exp3[slot].reg4 = 0;
	            outpCard.exp3[slot].reg5 = 0;
	        }
	
	        /* create record */
	        smDataAccess("smexp3.out", FLASH_WRITE, SM_EXP3_OUTPUT_PORT);
	    }
    /* read Output port config in expansion shelf 4 from Flash */
	    if ((outpExp4Valid =
	         smDataAccess("smexp4.out", FLASH_READ,
	                      SM_EXP4_OUTPUT_PORT)) == ERROR) {
	
	        /* record does not exist */
	        for (slot = 0; slot < 16; slot++) {
	            for (port = 0; port < NUM_PORTS; port++) {
	                outpCard.exp4[slot].portType[port] = OUTPUT_NULL;
	            }
	            outpCard.exp4[slot].reg0 = 0;
	            outpCard.exp4[slot].reg1 = 0;
	            outpCard.exp4[slot].reg2 = 0;
	            outpCard.exp4[slot].reg3 = 0;
	            outpCard.exp4[slot].reg4 = 0;
	            outpCard.exp4[slot].reg5 = 0;
	        }
	
	        /* create record */
	        smDataAccess("smexp4.out", FLASH_WRITE, SM_EXP4_OUTPUT_PORT);
	    }

    /* read for shelf info from FPGA  */
    shelf_index = *(U8_t *) MAIN_FPGA_R_REG_2;
    

	/* get shelf previous state or set default*/
	if ((smDataAccess("smshelf.dat", FLASH_READ,  SM_SHELF)) == ERROR) {

        /* record does not exist */
        	/*
				Plase be aware the init lenght. Do not go beyound.
				
				#define SM_SHELF_SERIAL_LEN		13
				#define	SM_SHELF_CLEICODE_LEN	13
        	*/


     		for (i = 0; i < NUMBER_SHELVES; i++) {
	     		shelves[i].serial.length 	= sprintf(shelves[i].serial.octet_ptr,
	    										"serial_%01d", i);
	    		shelves[i].cleicode.length 	= sprintf(shelves[i].cleicode.octet_ptr,
	    										"CLEI_%01d", i);    				
    		}
        /* create record */
        smDataAccess("smshelf.dat", FLASH_WRITE, SM_SHELF);
    }
    
    
    
    /* init main shelf  */
	    shelves[0].present = TRUE;
	    outpCardInit(MAIN_SHELF);
    
		
		/* init expansion shelf */

	    for (i = 1; i < NUMBER_SHELVES; i++) {
					  
	        if (shelf_index & (1 << (i - 1))) {     /*checkitout. use i-1 instead. */
	            shelves[i].present = TRUE;
	            
	            outpCardInit(i);
	        } else {
	            shelves[i].present = FALSE;
	           
	        }
	    }


		/* tiu init */
		TiuTaskSemaphore=semBCreate (SEM_Q_PRIORITY, SEM_FULL);
		tiuFramerInit();
		
		
		
		
    /* save records into Flash */
    smDataAccess("smcard.dat", FLASH_WRITE, SM_OUTPUT_CARD);

    /* Save configs to Flash */
    if (outpMainValid == ERROR)
        outpMainValid = OK;
    smDataAccess("smmain.out", FLASH_WRITE, SM_MAIN_OUTPUT_PORT);
    if (outpExp1Valid == ERROR)
        outpExp1Valid = OK;
    smDataAccess("smexp1.out", FLASH_WRITE, SM_EXP1_OUTPUT_PORT);
    if (outpExp2Valid == ERROR)
        outpExp2Valid = OK;
    smDataAccess("smexp2.out", FLASH_WRITE, SM_EXP2_OUTPUT_PORT);
    if (outpExp3Valid == ERROR)
        outpExp3Valid = OK;
    smDataAccess("smexp3.out", FLASH_WRITE, SM_EXP3_OUTPUT_PORT);
    if (outpExp4Valid == ERROR)
        outpExp4Valid = OK;
    smDataAccess("smexp4.out", FLASH_WRITE, SM_EXP4_OUTPUT_PORT);

		

    /* connect int vector */
   
    intConnect(&sm_interrupt, shelfUpdateInt, (int)0);
    return OK;                  /*checkitout, should we deal with file save fail case? */
}
/**************************************************************************************
* function: void shelfUpdateInt()
*	 description: the ISR of interrupt
*  input 
*			
*			
*  return 
*     
***************************************************************************************/
void shelfUpdateInt()
{
    if (!smReady)
        return;

    else
        shelfIntCnt++;
}
/**************************************************************************************
* function: void outpCardInit(SHELF_t shelf_index)
*  input 
*			
*			
*  return 
*     
***************************************************************************************/
void outpCardInit(SHELF_t shelf_index)
{
    U8_t slot, port, card, i;
    U8_t cardType;
    U32_t *dumPtr = NULL;
    stsCardEntry_t snmpTrapCardReply;
    STATUS_t outpExp1Valid, outpExp2Valid, outpExp3Valid, outpExp4Valid;
    
    
    switch (shelf_index) {
    case MAIN_SHELF:

        /* detect how many output cards presented in Main shelf */
        card = *(U8_t *) MAIN_FPGA_R_REG_6;
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {

                /* output card presents */
                outpCard.main[slot].status = OUTPUT_PRESENT;

                /* read card type from FPGA */
                cardType = cardTypeGet(MAIN_SHELF, slot);
                if ((cardType != outpCard.main[slot].cardType) &&
                    (outpCard.main[slot].cardType != OUTPUT_NULL)) {
                    outpCard.main[slot].status = OUTPUT_MISMATCH;
                    

						/* sendAlarm(misMatch); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);

						snmpTrapCardReply.stsCardShelf	=	D_stsCardShelf_main; /* main shelf ->100 not (shelf_index+1)*/
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;
						TrapEventLog(stsCardStatusChange_TRAP, D_stsCardOperStatus_misMatch, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);
						
						#ifdef SM_DEBUG
							printf("in sm, output Card mismatch !!\n");
						#endif	

                    
                    
                } else if (outpCard.main[slot].cardType == OUTPUT_NULL)
                    outpCard.main[slot].cardType = cardType;
            } else {
                outpCard.main[slot].status = OUTPUT_ABSENT;
            }

            /* Config output port on Main SHELF */
            if (outpCard.main[slot].status == OUTPUT_PRESENT) {
                if (outpCard.main[slot].cardType == OUTPUT_FREQUENCY) {
                    if (outpMainValid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++) {
                            outpCard.main[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(MAIN_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        }

                        /* update SSM to shadow RAM */
                        outpCard.main[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        *(U8_t *) OUTPUT_FPGA_W_REG_2(slot) =
                            outpCard.main[slot].reg1;
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(MAIN_SHELF, slot, port,
                                           outpCard.
                                           main[slot].portType[port],
                                           outpCard.main[slot].reg0);

                        /* update SSM to FPGA */
                        *(U8_t *) OUTPUT_FPGA_W_REG_2(slot) =
                            outpCard.main[slot].reg1;
                    }
                } else if (outpCard.main[slot].cardType == OUTPUT_TIMING) {
                    for (port = 0; port < NUM_PORTS; port++) {
                        if (outpMainValid == ERROR) {
                            outpCard.main[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(MAIN_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        } else
                            outpPortConfig(MAIN_SHELF, slot, port,
                                           outpCard.
                                           main[slot].portType[port],
                                           outpCard.main[slot].reg0);
                    }
                    outpCard.main[slot].reg1 =
                        ssmMessages[STRATUM_CLOCK_MAX - 1];
                } else if (outpCard.main[slot].cardType == OUTPUT_IRIGB) {

                    /* config time of date */
                } else if (outpCard.main[slot].cardType == OUTPUT_TIU) {
                    if (outpMainValid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(MAIN_SHELF, slot, port, 0,
                                           OUTPUT_FM_SF);

                        /* update SSM to shadow RAM */
                        outpCard.main[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        *(U8_t *) OUTPUT_FPGA_W_REG_2(slot) =
                            outpCard.main[slot].reg1;
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(MAIN_SHELF, slot, port,
                                           outpCard.
                                           main[slot].portType[port],
                                           outpCard.main[slot].reg0);

                        /* update SSM to FPGA */
                        *(U8_t *) OUTPUT_FPGA_W_REG_2(slot) =
                            outpCard.main[slot].reg1;
                    }
                }
            }

            /* activate output port */
            if ((outpCard.main[slot].status == OUTPUT_PRESENT) &&
                (outpCard.main[slot].redundancy_enable == REDUNDANCY_DISABLE))
                outpPortActivate(MAIN_SHELF, slot, OUTPUT_ON);

            else if ((outpCard.main[slot].status == OUTPUT_PRESENT) &&
                     (outpCard.main[slot].redundancy_enable ==
                      REDUNDANCY_ENABLE) && (outpCard.main[slot].redundancy_state ==
                                             OUTPUT_ACTIVE))
                outpPortActivate(MAIN_SHELF, slot, OUTPUT_ON);

            else
                outpPortActivate(MAIN_SHELF, slot, OUTPUT_OFF);
        }
        break;
    case EXP1_SHELF:

        /* detect how many output cards presented in EXP1 shelf */
        expFPGAAccess(EXP1_SHELF, 0x00, 0x03, &card, FPGA_READ);
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {
                outpCard.exp1[slot].status = OUTPUT_PRESENT;
                cardType = cardTypeGet(EXP1_SHELF, slot);
                if ((cardType != outpCard.exp1[slot].cardType) &&
                    (outpCard.exp1[slot].cardType != OUTPUT_NULL)) {
                    outpCard.exp1[slot].status = OUTPUT_MISMATCH;
 
						/* sendAlarm(misMatch); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);

						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);
						
						#ifdef SM_DEBUG
							printf("in sm, output Card mismatch !!\n");
						#endif	

                    
                } else if (outpCard.exp1[slot].cardType == OUTPUT_NULL)
                    outpCard.exp1[slot].cardType = cardType;
            } else
                outpCard.exp1[slot].status = OUTPUT_ABSENT;
        }
        expFPGAAccess(EXP1_SHELF, 0x00, 0x04, &card, FPGA_READ);
        for (slot = 8; slot < 16; slot++) {
            if (card & (1 << (slot - 8))) {
                outpCard.exp1[slot].status = OUTPUT_PRESENT;
                cardType = cardTypeGet(EXP1_SHELF, slot);
                if ((cardType != outpCard.exp1[slot].cardType) &&
                    (outpCard.exp1[slot].cardType != OUTPUT_NULL)) {
                    outpCard.exp1[slot].status = OUTPUT_MISMATCH;

						/* sendAlarm(misMatch); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);

						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);
						
						#ifdef SM_DEBUG
							printf("in sm, output Card mismatch !!\n");
						#endif	
	
                } else if (outpCard.exp1[slot].cardType == OUTPUT_NULL)
                    outpCard.exp1[slot].cardType = cardType;
            } else
                outpCard.exp1[slot].status = OUTPUT_ABSENT;
        }

        /* Config output port on EXP1 SHELF */
        for (slot = 0; slot < 16; slot++) {
            if (outpCard.exp1[slot].status == OUTPUT_PRESENT) {
                if (outpCard.exp1[slot].cardType == OUTPUT_FREQUENCY) {
                    if (outpExp1Valid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++) {
                            outpCard.exp1[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(EXP1_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        }

                        /* update SSM to shadow RAM */
                        outpCard.exp1[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP1_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp1[slot].reg1,
                                      FPGA_WRITE);
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP1_SHELF, slot, port,
                                           outpCard.
                                           exp1[slot].portType[port],
                                           outpCard.exp1[slot].reg0);

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP1_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp1[slot].reg1,
                                      FPGA_WRITE);
                    }
                } else if (outpCard.exp1[slot].cardType == OUTPUT_TIMING) {
                    for (port = 0; port < NUM_PORTS; port++) {
                        if (outpExp1Valid == ERROR) {
                            outpCard.exp1[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(EXP1_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        } else
                            outpPortConfig(EXP1_SHELF, slot, port,
                                           outpCard.
                                           exp1[slot].portType[port],
                                           outpCard.exp1[slot].reg0);
                    }
                    outpCard.exp1[slot].reg1 =
                        ssmMessages[STRATUM_CLOCK_MAX - 1];
                } else if (outpCard.exp1[slot].cardType == OUTPUT_IRIGB) {

                    /* config time of date */
                } else if (outpCard.exp1[slot].cardType == OUTPUT_TIU) {
                    if (outpExp1Valid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP1_SHELF, slot, port, 0,
                                           OUTPUT_FM_SF);

                        /* update SSM to shadow RAM */
                        outpCard.exp1[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP1_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp1[slot].reg1,
                                      FPGA_WRITE);
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP1_SHELF, slot, port,
                                           outpCard.
                                           exp1[slot].portType[port],
                                           outpCard.exp1[slot].reg0);

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP1_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp1[slot].reg1,
                                      FPGA_WRITE);
                    }
                }
            }
        }

        /* activate output port */
        for (slot = 0; slot < 16; slot++) {
            if ((outpCard.exp1[slot].status == OUTPUT_PRESENT) &&
                (outpCard.exp1[slot].redundancy_enable == REDUNDANCY_DISABLE))
                outpPortActivate(EXP1_SHELF, slot, OUTPUT_ON);

            else if ((outpCard.exp1[slot].status == OUTPUT_PRESENT) &&
                     (outpCard.exp1[slot].redundancy_enable ==
                      REDUNDANCY_ENABLE) && (outpCard.exp1[slot].redundancy_state ==
                                             OUTPUT_ACTIVE))
                outpPortActivate(EXP1_SHELF, slot, OUTPUT_ON);

            else
                outpPortActivate(EXP1_SHELF, slot, OUTPUT_OFF);
        }
        break;
    case EXP2_SHELF:

        /* decide how many output cards presented in EXP2 shelf */
        expFPGAAccess(EXP2_SHELF, 0x00, 0x03, &card, FPGA_READ);
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {
                outpCard.exp2[slot].status = OUTPUT_PRESENT;
                cardType = cardTypeGet(EXP2_SHELF, slot);
                if ((cardType != outpCard.exp2[slot].cardType) &&
                    (outpCard.exp2[slot].cardType != OUTPUT_NULL)) {
                    outpCard.exp2[slot].status = OUTPUT_MISMATCH;


						/* sendAlarm(misMatch); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
						
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

						#ifdef SM_DEBUG
							printf("in sm, output Card mismatch !!\n");
						#endif	

                } else if (outpCard.exp2[slot].cardType == OUTPUT_NULL)
                    outpCard.exp2[slot].cardType = cardType;
            } else
                outpCard.exp2[slot].status = OUTPUT_ABSENT;
        }
        expFPGAAccess(EXP2_SHELF, 0x00, 0x04, &card, FPGA_READ);
        for (slot = 8; slot < 16; slot++) {
            if (card & (1 << (slot - 8))) {
                outpCard.exp2[slot].status = OUTPUT_PRESENT;
                cardType = cardTypeGet(EXP2_SHELF, slot);
                if ((cardType != outpCard.exp2[slot].cardType)
                    && (outpCard.exp2[slot].cardType != OUTPUT_NULL)) {
                    outpCard.exp2[slot].status = OUTPUT_MISMATCH;

					/* sendAlarm(misMatch); */
					SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
					
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

					#ifdef SM_DEBUG
						printf("in sm, output Card mismatch !!\n");
					#endif	

                } else if (outpCard.exp2[slot].cardType == OUTPUT_NULL)
                    outpCard.exp2[slot].cardType = cardType;
            } else
                outpCard.exp2[slot].status = OUTPUT_ABSENT;
        }

        /* Config output port on EXP2 SHELF */
        for (slot = 0; slot < 16; slot++) {
            if (outpCard.exp2[slot].status == OUTPUT_PRESENT) {
                if (outpCard.exp2[slot].cardType == OUTPUT_FREQUENCY) {
                    if (outpExp2Valid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++) {
                            outpCard.exp2[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(EXP2_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        }

                        /* update SSM to shadow RAM */
                        outpCard.exp2[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP2_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp2[slot].reg1,
                                      FPGA_WRITE);
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP2_SHELF, slot, port,
                                           outpCard.
                                           exp2[slot].portType[port],
                                           outpCard.exp2[slot].reg0);

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP2_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp2[slot].reg1,
                                      FPGA_WRITE);
                    }
                } else if (outpCard.exp2[slot].cardType == OUTPUT_TIMING) {
                    for (port = 0; port < NUM_PORTS; port++) {
                        if (outpExp2Valid == ERROR) {
                            outpCard.exp2[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(EXP2_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        } else
                            outpPortConfig(EXP2_SHELF, slot, port,
                                           outpCard.
                                           exp2[slot].portType[port],
                                           outpCard.exp2[slot].reg0);
                    }
                    outpCard.exp2[slot].reg1 =
                        ssmMessages[STRATUM_CLOCK_MAX - 1];
                } else if (outpCard.exp2[slot].cardType == OUTPUT_IRIGB) {

                    /* config time of date */
                } else if (outpCard.exp2[slot].cardType == OUTPUT_TIU) {
                    if (outpExp2Valid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP2_SHELF, slot, port, 0,
                                           OUTPUT_FM_SF);

                        /* update SSM to shadow RAM */
                        outpCard.exp2[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP2_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp2[slot].reg1,
                                      FPGA_WRITE);
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP2_SHELF, slot, port,
                                           outpCard.
                                           exp2[slot].portType[port],
                                           outpCard.exp2[slot].reg0);

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP2_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp2[slot].reg1,
                                      FPGA_WRITE);
                    }
                }
            }
        }

        /* activate output port */
        for (slot = 0; slot < 16; slot++) {
            if ((outpCard.exp2[slot].status == OUTPUT_PRESENT) &&
                (outpCard.exp2[slot].redundancy_enable == REDUNDANCY_DISABLE))
                outpPortActivate(EXP2_SHELF, slot, OUTPUT_ON);

            else if ((outpCard.exp2[slot].status == OUTPUT_PRESENT) &&
                     (outpCard.exp2[slot].redundancy_enable ==
                      REDUNDANCY_ENABLE) && (outpCard.exp2[slot].redundancy_state ==
                                             OUTPUT_ACTIVE))
                outpPortActivate(EXP2_SHELF, slot, OUTPUT_ON);

            else
                outpPortActivate(EXP2_SHELF, slot, OUTPUT_OFF);
        }
        break;
    case EXP3_SHELF:

        /* decide how many output cards presented in EXP3 unit */
        expFPGAAccess(EXP3_SHELF, 0x00, 0x03, &card, FPGA_READ);
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {
                outpCard.exp3[slot].status = OUTPUT_PRESENT;
                cardType = cardTypeGet(EXP3_SHELF, slot);
                if ((cardType != outpCard.exp3[slot].cardType) &&
                    (outpCard.exp3[slot].cardType != OUTPUT_NULL)) {
                    outpCard.exp3[slot].status = OUTPUT_MISMATCH;

						/* sendAlarm(misMatch); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
						
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

						#ifdef SM_DEBUG
							printf("in sm, output Card mismatch !!\n");
						#endif	

                } else if (outpCard.exp3[slot].cardType == OUTPUT_NULL)
                    outpCard.exp3[slot].cardType = cardType;
            } else
                outpCard.exp3[slot].status = OUTPUT_ABSENT;
        }
        expFPGAAccess(EXP3_SHELF, 0x00, 0x03, &card, FPGA_READ);        /*shuming pls check addrhigh and addrlow */
        for (slot = 8; slot < 16; slot++) {
            if (card & (1 << (slot - 8))) {
                outpCard.exp3[slot].status = OUTPUT_PRESENT;
                cardType = cardTypeGet(EXP3_SHELF, slot);
                if ((cardType != outpCard.exp3[slot].cardType) &&
                    (outpCard.exp3[slot].cardType != OUTPUT_NULL)) {
                    outpCard.exp3[slot].status = OUTPUT_MISMATCH;

						/* sendAlarm(misMatch); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
						
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

						#ifdef SM_DEBUG
							printf("in sm, output Card mismatch !!\n");
						#endif	
	
                } else if (outpCard.exp3[slot].cardType == OUTPUT_NULL)
                    outpCard.exp3[slot].cardType = cardType;
            } else
                outpCard.exp3[slot].status = OUTPUT_ABSENT;
        }

        /* Config output port on EXP2 SHELF */
        for (slot = 0; slot < 16; slot++) {
            if (outpCard.exp3[slot].status == OUTPUT_PRESENT) {
                if (outpCard.exp3[slot].cardType == OUTPUT_FREQUENCY) {
                    if (outpExp3Valid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++) {
                            outpCard.exp3[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(EXP3_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        }

                        /* update SSM to shadow RAM */
                        outpCard.exp3[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP3_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp3[slot].reg1,
                                      FPGA_WRITE);
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP3_SHELF, slot, port,
                                           outpCard.
                                           exp3[slot].portType[port],
                                           outpCard.exp3[slot].reg0);

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP3_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp3[slot].reg1,
                                      FPGA_WRITE);
                    }
                } else if (outpCard.exp3[slot].cardType == OUTPUT_TIMING) {
                    for (port = 0; port < NUM_PORTS; port++) {
                        if (outpExp3Valid == ERROR) {
                            outpCard.exp3[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(EXP3_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        } else
                            outpPortConfig(EXP3_SHELF, slot, port,
                                           outpCard.
                                           exp3[slot].portType[port],
                                           outpCard.exp3[slot].reg0);
                    }
                    outpCard.exp3[slot].reg1 =
                        ssmMessages[STRATUM_CLOCK_MAX - 1];
                } else if (outpCard.exp3[slot].cardType == OUTPUT_IRIGB) {

                    /* config time of date */
                } else if (outpCard.exp3[slot].cardType == OUTPUT_TIU) {
                    if (outpExp3Valid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP3_SHELF, slot, port, 0,
                                           OUTPUT_FM_SF);

                        /* update SSM to shadow RAM */
                        outpCard.exp3[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP3_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp3[slot].reg1,
                                      FPGA_WRITE);
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP3_SHELF, slot, port,
                                           outpCard.
                                           exp3[slot].portType[port],
                                           outpCard.exp3[slot].reg0);

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP3_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp3[slot].reg1,
                                      FPGA_WRITE);
                    }
                }
            }
        }

        /* activate output port */
        for (slot = 0; slot < 16; slot++) {
            if ((outpCard.exp3[slot].status == OUTPUT_PRESENT) &&
                (outpCard.exp3[slot].redundancy_enable == REDUNDANCY_DISABLE))
                outpPortActivate(EXP3_SHELF, slot, OUTPUT_ON);

            else if ((outpCard.exp3[slot].status == OUTPUT_PRESENT) &&
                     (outpCard.exp3[slot].redundancy_enable ==
                      REDUNDANCY_ENABLE) && (outpCard.exp3[slot].redundancy_state ==
                                             OUTPUT_ACTIVE))
                outpPortActivate(EXP3_SHELF, slot, OUTPUT_ON);

            else
                outpPortActivate(EXP3_SHELF, slot, OUTPUT_OFF);
        }
        break;
    case EXP4_SHELF:

        /* decide how many output cards presented in EXP4 unit */
        expFPGAAccess(EXP4_SHELF, 0x00, 0x03, &card, FPGA_READ);
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {
                outpCard.exp4[slot].status = OUTPUT_PRESENT;
                cardType = cardTypeGet(EXP4_SHELF, slot);
                if ((cardType != outpCard.exp4[slot].cardType) &&
                    (outpCard.exp4[slot].cardType != OUTPUT_NULL)) {
                    outpCard.exp4[slot].status = OUTPUT_MISMATCH;


						/* sendAlarm(misMatch); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
						
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

						#ifdef SM_DEBUG
							printf("in sm, output Card mismatch !!\n");
						#endif	

                } else if (outpCard.exp4[slot].cardType == OUTPUT_NULL)
                    outpCard.exp4[slot].cardType = cardType;
            } else
                outpCard.exp4[slot].status = OUTPUT_ABSENT;
        }
        expFPGAAccess(EXP4_SHELF, 0x00, 0x04, &card, FPGA_READ);
        for (slot = 8; slot < 16; slot++) {
            if (card & (1 << (slot - 8))) {
                outpCard.exp4[slot].status = OUTPUT_PRESENT;
                cardType = cardTypeGet(EXP4_SHELF, slot);
                if ((cardType != outpCard.exp4[slot].cardType) &&
                    (outpCard.exp4[slot].cardType != OUTPUT_NULL)) {
                    outpCard.exp4[slot].status = OUTPUT_MISMATCH;

						/* sendAlarm(misMatch); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
						
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

						#ifdef SM_DEBUG
							printf("in sm, output Card mismatch !!\n");
						#endif	

                } else if (outpCard.exp4[slot].cardType == OUTPUT_NULL)
                    outpCard.exp4[slot].cardType = cardType;
            } else
                outpCard.exp4[slot].status = OUTPUT_ABSENT;
        }

        /* Config output port on EXP4 SHELF */
        for (slot = 0; slot < 16; slot++) {
            if (outpCard.exp4[slot].status == OUTPUT_PRESENT) {
                if (outpCard.exp4[slot].cardType == OUTPUT_FREQUENCY) {
                    if (outpExp4Valid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++) {
                            outpCard.exp4[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(EXP4_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        }

                        /* update SSM to shadow RAM */
                        outpCard.exp4[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP4_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp4[slot].reg1,
                                      FPGA_WRITE);
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP4_SHELF, slot, port,
                                           outpCard.
                                           exp4[slot].portType[port],
                                           outpCard.exp4[slot].reg0);

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP4_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp4[slot].reg1,
                                      FPGA_WRITE);
                    }
                } else if (outpCard.exp4[slot].cardType == OUTPUT_TIMING) {
                    for (port = 0; port < NUM_PORTS; port++) {
                        if (outpExp4Valid == ERROR) {
                            outpCard.exp4[slot].portType[port] =
                                OUTPUT_NULL;
                            outpPortConfig(EXP4_SHELF, slot, port,
                                           OUTPUT_NULL, OUTPUT_FM_NULL);
                        } else
                            outpPortConfig(EXP4_SHELF, slot, port,
                                           outpCard.
                                           exp4[slot].portType[port],
                                           outpCard.exp4[slot].reg0);
                    }
                    outpCard.exp4[slot].reg1 =
                        ssmMessages[STRATUM_CLOCK_MAX - 1];
                } else if (outpCard.exp4[slot].cardType == OUTPUT_IRIGB) {

                    /* config time of date */
                } else if (outpCard.exp4[slot].cardType == OUTPUT_TIU) {
                    if (outpExp4Valid == ERROR) {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP4_SHELF, slot, port, 0,
                                           OUTPUT_FM_SF);

                        /* update SSM to shadow RAM */
                        outpCard.exp4[slot].reg1 =
                            ssmMessages[STRATUM_CLOCK_MAX - 1];

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP4_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp4[slot].reg1,
                                      FPGA_WRITE);
                    } else {
                        for (port = 0; port < NUM_PORTS; port++)
                            outpPortConfig(EXP4_SHELF, slot, port,
                                           outpCard.
                                           exp4[slot].portType[port],
                                           outpCard.exp4[slot].reg0);

                        /* update SSM to FPGA */
                        expFPGAAccess(EXP4_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp4[slot].reg1,
                                      FPGA_WRITE);
                    }
                }
            }
        }

        /* activate output port */
        for (slot = 0; slot < 16; slot++) {
            if ((outpCard.exp4[slot].status == OUTPUT_PRESENT) &&
                (outpCard.exp4[slot].redundancy_enable == REDUNDANCY_DISABLE))
                outpPortActivate(EXP4_SHELF, slot, OUTPUT_ON);

            else if ((outpCard.exp4[slot].status == OUTPUT_PRESENT) &&
                     (outpCard.exp4[slot].redundancy_enable ==
                      REDUNDANCY_ENABLE) && (outpCard.exp4[slot].redundancy_state ==
                                             OUTPUT_ACTIVE))
                outpPortActivate(EXP4_SHELF, slot, OUTPUT_ON);

            else
                outpPortActivate(EXP4_SHELF, slot, OUTPUT_OFF);
        }
        break;
    default:
        printf("outpCardInit: invalid shelf: %d\n", shelf_index);
        break;
    }
}

/**************************************************************************************
* function:  STATUS_t outpCardUpdate(SHELF_t shelf_index)
*  input 
*			
*			
*  return 
*     
***************************************************************************************/
STATUS_t outpCardUpdate(SHELF_t shelf_index)
{
    U8_t slot, card, i, port;
    U8_t cardType;
    STATUS_t cardChange = FALSE;
		
		U32_t *dumPtr = NULL;
		stsCardEntry_t snmpTrapCardReply;
		
    switch (shelf_index) {
    case MAIN_SHELF:

        /* get output cards info from Main shelf */
        card = *(U8_t *) MAIN_FPGA_R_REG_6;
        cardChange = FALSE;
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {
                if (outpCard.main[slot].status != OUTPUT_PRESENT) {

                    /* a new output card is added to Main shelf */
                    cardChange = TRUE;
                    outpCard.main[slot].status = OUTPUT_PRESENT;
                    
					cardType = cardTypeGet(MAIN_SHELF, slot);
					
					/* sendAlarm(a new card is added); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDCARD), slot, shelf_index);

						/*
							TrapEventLog(smCardAddEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDCARD), slot,shelf_index);
						*/
						
						#ifdef SM_DEBUG
							printf("in sm, a new card [%d] is added in shelf [%d]\n", slot, shelf_index);
						#endif
                    
                    
                    
                    if ((cardType != outpCard.main[slot].cardType) &&
                        (outpCard.main[slot].cardType != OUTPUT_NULL)) {
                        outpCard.main[slot].status = OUTPUT_MISMATCH;
                        

								/* sendAlarm(misMatch); */
								SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
								
								snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
								snmpTrapCardReply.stsCardIndex	=	slot+1;
								snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
								snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
								TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

								#ifdef SM_DEBUG
									printf("in sm, output Card mismatch !!\n");
								#endif	

                        
                    } else if (outpCard.main[slot].cardType == OUTPUT_NULL)
                        outpCard.main[slot].cardType = cardType;
                    if (outpCard.main[slot].status == OUTPUT_PRESENT) {
                        if (outpCard.main[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(MAIN_SHELF, slot, port,
                                               outpCard.
                                               main[slot].
                                               portType[port],
                                               outpCard.main[slot].reg0);
                            }

                            /* update SSM to FPGA */
                            *(U8_t *) OUTPUT_FPGA_W_REG_2(slot) =
                                outpCard.main[slot].reg1;
                        } else if (outpCard.main[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(MAIN_SHELF, slot, port,
                                               outpCard.
                                               main[slot].
                                               portType[port],
                                               outpCard.main[slot].reg0);
                            }
                        } else if (outpCard.main[slot].cardType ==
                                   OUTPUT_IRIGB) {

                            /* config time of date */
                        } else if (outpCard.main[slot].cardType ==
                                   OUTPUT_TIU) {
                            for (port = 0; port < NUM_PORTS; port++)
                                outpPortConfig(MAIN_SHELF, slot, port,
                                               outpCard.
                                               main[slot].
                                               portType[port],
                                               outpCard.main[slot].reg0);

                            /* update SSM to FPGA */
                            *(U8_t *) OUTPUT_FPGA_W_REG_2(slot) =
                                outpCard.main[slot].reg1;
                        }

                        /* check Redundancy */
                        if (slot % 2) {
                            if (outpCard.main[slot - 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.main[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(MAIN_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.main[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < NUM_PORTS; i++) {
                                    outpCard.main[slot].portType[i] =
                                        outpCard.main[slot -
                                                      1].portType[i];
                                    outpPortConfig(MAIN_SHELF, slot, i,
                                                   outpCard.
                                                   main[slot].
                                                   portType[i],
                                                   outpCard.
                                                   main[slot].reg0);
                                }
                                if (outpCard.main[slot - 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(MAIN_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.main[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(MAIN_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.main[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        } else {
                            if (outpCard.main[slot + 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.main[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(MAIN_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.main[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < NUM_PORTS; i++) {
                                    outpCard.main[slot].portType[i] =
                                        outpCard.main[slot +
                                                      1].portType[i];
                                    outpPortConfig(MAIN_SHELF, slot, i,
                                                   outpCard.
                                                   main[slot].
                                                   portType[i],
                                                   outpCard.
                                                   main[slot].reg0);
                                }
                                if (outpCard.main[slot + 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(MAIN_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.main[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(MAIN_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.main[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        }
                    }
                } else {
                    outpPortActivate(MAIN_SHELF, slot, OUTPUT_OFF);
                }
            } else {
                if (outpCard.main[slot].status != OUTPUT_ABSENT) {

                    /* a output card is removed from Main shelf */
                    cardChange = TRUE;
                    outpCard.main[slot].status = OUTPUT_ABSENT;
                    
					/* sendAlarm(a output card is removed); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVECARD), slot, shelf_index);
						
						/* checkitout
							TrapEventLog(smCardRemoveEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVECARD), slot,shelf_index);
						*/
						
						#ifdef SM_DEBUG
							printf("in sm, card [%d] is removed in shelf [%d]\n", slot, shelf_index);
						#endif	

                    
                    if (outpCard.main[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE) {
                        outpCard.main[slot].redundancy_state = REDUNDANCY_STANDBY;

                        /* redundancy switch */
                        if (slot % 2) {
                            outpCard.main[slot - 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(MAIN_SHELF, slot - 1,
                                             OUTPUT_ON);
                        } else {
                            outpCard.main[slot + 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(MAIN_SHELF, slot + 1,
                                             OUTPUT_ON);
                        }
                    }
                }
            }
        }
        if (cardChange) {

            /* Save configs to Flash */
            smDataAccess("smcard.dat", FLASH_WRITE, SM_OUTPUT_CARD);
            smDataAccess("smmain.out", FLASH_WRITE, SM_MAIN_OUTPUT_PORT);
        }
        break;
    case EXP1_SHELF:

        /* get output cards info from EXP1 shelf */
        expFPGAAccess(EXP1_SHELF, 0x00, 0x03, &card, FPGA_READ);
        cardChange = FALSE;
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {
                if (outpCard.exp1[slot].status != OUTPUT_PRESENT) {

                    /* a new output card is added to EXP1 shelf */
                    cardChange = TRUE;
                    outpCard.exp1[slot].status = OUTPUT_PRESENT;
                    
                    
					/* sendAlarm(a new card is added); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDCARD), slot, shelf_index);
						
						/* checkitout
							TrapEventLog(smCardAddEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDCARD), slot,shelf_index);
						*/
						
						#ifdef SM_DEBUG
							printf("in sm, a new card [%d] is added in shelf [%d]\n", slot, shelf_index);
						#endif	

                    
                    cardType = cardTypeGet(EXP1_SHELF, slot);
                    if ((cardType != outpCard.exp1[slot].cardType) &&
                        (outpCard.exp1[slot].cardType != OUTPUT_NULL)) {
                        outpCard.exp1[slot].status = OUTPUT_MISMATCH;
                        

								/* sendAlarm(misMatch); */
								SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
								
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

								#ifdef SM_DEBUG
									printf("in sm, output Card mismatch !!\n");
								#endif	                       
                        
                        
                    } else if (outpCard.exp1[slot].cardType == OUTPUT_NULL)
                        outpCard.exp1[slot].cardType = cardType;
                    if (outpCard.exp1[slot].status == OUTPUT_PRESENT) {
                        if (outpCard.exp1[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP1_SHELF, slot, port,
                                               outpCard.
                                               exp1[slot].
                                               portType[port],
                                               outpCard.exp1[slot].reg0);
                            }

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP1_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp1[slot].reg1,
                                          FPGA_WRITE);
                        } else if (outpCard.exp1[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP1_SHELF, slot, port,
                                               outpCard.
                                               exp1[slot].
                                               portType[port],
                                               outpCard.exp1[slot].reg0);
                            }
                        } else if (outpCard.exp1[slot].cardType ==
                                   OUTPUT_IRIGB) {

                            /* config time of date */
                        } else if (outpCard.exp1[slot].cardType ==
                                   OUTPUT_TIU) {
                            for (port = 0; port < NUM_PORTS; port++)
                                outpPortConfig(EXP1_SHELF, slot, port,
                                               outpCard.
                                               exp1[slot].
                                               portType[port],
                                               outpCard.exp1[slot].reg0);

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP1_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp1[slot].reg1,
                                          FPGA_WRITE);
                        }

                        /* check Redundancy */
                        if (slot % 2) {
                            if (outpCard.exp1[slot - 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp1[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP1_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp1[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp1[slot].portType[i] =
                                        outpCard.exp1[slot -
                                                      1].portType[i];
                                    outpPortConfig(EXP1_SHELF, slot, i,
                                                   outpCard.
                                                   exp1[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp1[slot].reg0);
                                }
                                if (outpCard.exp1[slot - 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP1_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp1[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP1_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp1[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        } else {
                            if (outpCard.exp1[slot + 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp1[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP1_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp1[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp1[slot].portType[i] =
                                        outpCard.exp1[slot +
                                                      1].portType[i];
                                    outpPortConfig(EXP1_SHELF, slot, i,
                                                   outpCard.
                                                   exp1[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp1[slot].reg0);
                                }
                                if (outpCard.exp1[slot + 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP1_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp1[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP1_SHELF, slot, OUTPUT_ON);      /*shuming pls check this if it is off or on */
                                    outpCard.exp1[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        }
                    }
                } else {
                    outpPortActivate(EXP1_SHELF, slot, OUTPUT_OFF);
                }
            } else {
                if (outpCard.exp1[slot].status != OUTPUT_ABSENT) {

                    /* a output card is removed from EXP1 shelf */
                    cardChange = TRUE;
                    outpCard.exp1[slot].status = OUTPUT_ABSENT;

					/* sendAlarm(a output card is removed); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVECARD), slot, shelf_index);
						
						/* checkitout
							TrapEventLog(smCardRemoveEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVECARD), slot,shelf_index);
						*/
						
						#ifdef SM_DEBUG
							printf("in sm, card [%d] is removed in shelf [%d]\n", slot, shelf_index);
						#endif                    
                    
                    if (outpCard.exp1[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE) {
                        outpCard.exp1[slot].redundancy_state == REDUNDANCY_STANDBY;

                        /* redundancy switch */
                        if (slot % 2) {
                            outpCard.exp1[slot - 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP1_SHELF, slot - 1,
                                             OUTPUT_ON);
                        } else {
                            outpCard.exp1[slot + 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP1_SHELF, slot + 1,
                                             OUTPUT_ON);
                        }
                    }
                }
            }
        }
        expFPGAAccess(EXP1_SHELF, 0x00, 0x04, &card, FPGA_READ);
        for (slot = 8; slot < 16; slot++) {
            if (card & (1 << (slot - 8))) {
                if (outpCard.exp1[slot].status != OUTPUT_PRESENT) {

                    /* a new output card is added to EXP1 shelf */
                    cardChange = TRUE;
                    outpCard.exp1[slot].status = OUTPUT_PRESENT;
                   
					/* sendAlarm(a new card is added); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDCARD), slot, shelf_index);
							
						/* checkitout
							TrapEventLog(smCardAddEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDCARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, a new card [%d] is added in shelf [%d]\n", slot, shelf_index);
						#endif
                    
                    
                    
                    cardType = cardTypeGet(EXP1_SHELF, slot);
                    if ((cardType != outpCard.exp1[slot].cardType) &&
                        (outpCard.exp1[slot].cardType != OUTPUT_NULL)) {
                        outpCard.exp1[slot].status = OUTPUT_MISMATCH;
                        

			            /* sendAlarm(misMatch); */
								SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
								
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

								#ifdef SM_DEBUG
									printf("in sm, output Card mismatch !!\n");
								#endif	
                        
                        
                        
                    } else if (outpCard.exp1[slot].cardType == OUTPUT_NULL)
                        outpCard.exp1[slot].cardType = cardType;
                    if (outpCard.exp1[slot].status == OUTPUT_PRESENT) {
                        if (outpCard.exp1[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP1_SHELF, slot, port,
                                               outpCard.
                                               exp1[slot].
                                               portType[port],
                                               outpCard.exp1[slot].reg0);
                            }

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP1_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp1[slot].reg1,
                                          FPGA_WRITE);
                        } else if (outpCard.exp1[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP1_SHELF, slot, port,
                                               outpCard.
                                               exp1[slot].
                                               portType[port],
                                               outpCard.exp1[slot].reg0);
                            }
                        } else if (outpCard.exp1[slot].cardType ==
                                   OUTPUT_IRIGB) {

                            /* config time of date */
                        } else if (outpCard.exp1[slot].cardType ==
                                   OUTPUT_TIU) {
                            for (port = 0; port < NUM_PORTS; port++)
                                outpPortConfig(EXP1_SHELF, slot, port,
                                               outpCard.
                                               exp1[slot].
                                               portType[port],
                                               outpCard.exp1[slot].reg0);

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP1_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp1[slot].reg1,
                                          FPGA_WRITE);
                        }

                        /* check Redundancy */
                        if (slot % 2) {
                            if (outpCard.exp1[slot - 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp1[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP1_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp1[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp1[slot].portType[i] =
                                        outpCard.exp1[slot -
                                                      1].portType[i];
                                    outpPortConfig(EXP1_SHELF, slot, i,
                                                   outpCard.
                                                   exp1[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp1[slot].reg0);
                                }
                                if (outpCard.exp1[slot - 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP1_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp1[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP1_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp1[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        } else {
                            if (outpCard.exp1[slot + 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp1[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP1_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp1[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp1[slot].portType[i] =
                                        outpCard.exp1[slot +
                                                      1].portType[i];
                                    outpPortConfig(EXP1_SHELF, slot, i,
                                                   outpCard.
                                                   exp1[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp1[slot].reg0);
                                }
                                if (outpCard.exp1[slot + 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP1_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp1[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP1_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp1[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        }
                    }
                } else {
                    outpPortActivate(EXP1_SHELF, slot, OUTPUT_OFF);
                }
            } else {
                if (outpCard.exp1[slot].status != OUTPUT_ABSENT) {

                    /* a output card is removed from EXP1 shelf */
                    cardChange = TRUE;
                    outpCard.exp1[slot].status = OUTPUT_ABSENT;


						/* sendAlarm(a output card is removed); */
							SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVECARD), slot, shelf_index);
							
							/* checkitout
								TrapEventLog(smCardRemoveEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVECARD), slot,shelf_index);
							*/
							
							#ifdef SM_DEBUG
								printf("in sm, card [%d] is removed in shelf [%d]\n", slot, shelf_index);
							#endif	
                    
                    
                    if (outpCard.exp1[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE) {
                        outpCard.exp1[slot].redundancy_state == REDUNDANCY_STANDBY;

                        /* redundancy switch */
                        if (slot % 2) {
                            outpCard.exp1[slot - 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP1_SHELF, slot - 1,
                                             OUTPUT_ON);
                        } else {
                            outpCard.exp1[slot + 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP1_SHELF, slot + 1,
                                             OUTPUT_ON);
                        }
                    }
                }
            }
        }
        if (cardChange) {

            /* Save configs to Flash */
            smDataAccess("smcard.dat", FLASH_WRITE, SM_OUTPUT_CARD);
            smDataAccess("smexp1.out", FLASH_WRITE, SM_EXP1_OUTPUT_PORT);
        }
        break;
    case EXP2_SHELF:

        /* get output cards info from EXP2 shelf */
        expFPGAAccess(EXP2_SHELF, 0x00, 0x03, &card, FPGA_READ);
        cardChange = FALSE;
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {
                if (outpCard.exp2[slot].status != OUTPUT_PRESENT) {

                    /* a new output card is added to EXP2 shelf */
                    cardChange = TRUE;
                    outpCard.exp2[slot].status = OUTPUT_PRESENT;
                    
					/* sendAlarm(a new card is added); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDCARD), slot, shelf_index);
							
						/* checkitout
							TrapEventLog(smCardAddEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDCARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, a new card [%d] is added in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    
                    cardType = cardTypeGet(EXP2_SHELF, slot);
                    if ((cardType != outpCard.exp2[slot].cardType) &&
                        (outpCard.exp2[slot].cardType != OUTPUT_NULL)) {
                        outpCard.exp2[slot].status = OUTPUT_MISMATCH;

								/* sendAlarm(misMatch); */
								SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
			
								snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
								snmpTrapCardReply.stsCardIndex	=	slot+1;
								snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
								snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
								TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

								#ifdef SM_DEBUG
									printf("in sm, output Card mismatch !!\n");
								#endif	
                        
                    } else if (outpCard.exp2[slot].cardType == OUTPUT_NULL)
                        outpCard.exp2[slot].cardType = cardType;
                    if (outpCard.exp2[slot].status == OUTPUT_PRESENT) {
                        if (outpCard.exp2[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP2_SHELF, slot, port,
                                               outpCard.
                                               exp2[slot].
                                               portType[port],
                                               outpCard.exp2[slot].reg0);
                            }

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP2_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp2[slot].reg1,
                                          FPGA_WRITE);
                        } else if (outpCard.exp2[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP2_SHELF, slot, port,
                                               outpCard.
                                               exp2[slot].
                                               portType[port],
                                               outpCard.exp2[slot].reg0);
                            }
                        } else if (outpCard.exp2[slot].cardType ==
                                   OUTPUT_IRIGB) {

                            /* config time of date */
                        } else if (outpCard.exp2[slot].cardType ==
                                   OUTPUT_TIU) {
                            for (port = 0; port < NUM_PORTS; port++)
                                outpPortConfig(EXP2_SHELF, slot, port,
                                               outpCard.
                                               exp2[slot].
                                               portType[port],
                                               outpCard.exp2[slot].reg0);

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP2_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp2[slot].reg1,
                                          FPGA_WRITE);
                        }

                        /* check Redundancy */
                        if (slot % 2) {
                            if (outpCard.exp2[slot - 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp2[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP2_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp2[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp2[slot].portType[i] =
                                        outpCard.exp2[slot -
                                                      1].portType[i];
                                    outpPortConfig(EXP2_SHELF, slot, i,
                                                   outpCard.
                                                   exp2[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp2[slot].reg0);
                                }
                                if (outpCard.exp2[slot - 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP2_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp2[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP2_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp2[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        } else {
                            if (outpCard.exp2[slot + 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp2[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP2_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp2[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp2[slot].portType[i] =
                                        outpCard.exp2[slot +
                                                      1].portType[i];
                                    outpPortConfig(EXP2_SHELF, slot, i,
                                                   outpCard.
                                                   exp2[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp2[slot].reg0);
                                }
                                if (outpCard.exp2[slot + 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP2_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp2[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP2_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp2[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        }
                    }
                } else {
                    outpPortActivate(EXP2_SHELF, slot, OUTPUT_OFF);
                }
            } else {
                if (outpCard.exp2[slot].status != OUTPUT_ABSENT) {

                    /* a output card is removed from EXP2 shelf */
                    cardChange = TRUE;
                    outpCard.exp2[slot].status = OUTPUT_ABSENT;

					/* sendAlarm(a output card is removed); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVECARD), slot, shelf_index);
	
						/* checkitout
						TrapEventLog(smCardRemoveEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVECARD), slot,shelf_index);
						*/
						
						#ifdef SM_DEBUG
							printf("in sm, card [%d] is removed in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    if (outpCard.exp2[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE) {
                        outpCard.exp2[slot].redundancy_state == REDUNDANCY_STANDBY;

                        /* redundancy switch */
                        if (slot % 2) {
                            outpCard.exp2[slot - 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP2_SHELF, slot - 1,
                                             OUTPUT_ON);
                        } else {
                            outpCard.exp2[slot + 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP2_SHELF, slot + 1,
                                             OUTPUT_ON);
                        }
                    }
                }
            }
        }
        expFPGAAccess(EXP2_SHELF, 0x00, 0x04, &card, FPGA_READ);
        for (slot = 8; slot < 16; slot++) {
            if (card & (1 << (slot - 8))) {
                if (outpCard.exp2[slot].status != OUTPUT_PRESENT) {

                    /* a new output card is added to EXP2 shelf */
                    cardChange = TRUE;
                    outpCard.exp2[slot].status = OUTPUT_PRESENT;
                    
					/* sendAlarm(a new card is added); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDCARD), slot, shelf_index);
							
						/* checkitout
							TrapEventLog(smCardAddEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDCARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, a new card [%d] is added in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    cardType = cardTypeGet(EXP2_SHELF, slot);
                    if ((cardType != outpCard.exp2[slot].cardType) &&
                        (outpCard.exp2[slot].cardType != OUTPUT_NULL)) {
                        outpCard.exp2[slot].status = OUTPUT_MISMATCH;
                        

						/* sendAlarm(misMatch); */
								SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
								
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

			            #ifdef SM_DEBUG
									printf("in sm, output Card mismatch !!\n");
						#endif
                        
                    } else if (outpCard.exp2[slot].cardType == OUTPUT_NULL) {
                        outpCard.exp2[slot].cardType = cardType;
                        if (outpCard.exp2[slot].status == OUTPUT_PRESENT) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP2_SHELF, slot, port,
                                               outpCard.
                                               exp2[slot].
                                               portType[port],
                                               outpCard.exp2[slot].reg0);
                            }

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP2_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp2[slot].reg1,
                                          FPGA_WRITE);
                        } else if (outpCard.exp2[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP2_SHELF, slot, port,
                                               outpCard.
                                               exp2[slot].
                                               portType[port],
                                               outpCard.exp2[slot].reg0);
                            }
                        } else if (outpCard.exp2[slot].cardType ==
                                   OUTPUT_IRIGB) {

                            /* config time of date */
                        } else if (outpCard.exp2[slot].cardType ==
                                   OUTPUT_TIU) {
                            for (port = 0; port < NUM_PORTS; port++)
                                outpPortConfig(EXP2_SHELF, slot, port,
                                               outpCard.
                                               exp2[slot].
                                               portType[port],
                                               outpCard.exp2[slot].reg0);

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP2_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp2[slot].reg1,
                                          FPGA_WRITE);
                        }

                        /* check Redundancy */
                        if (slot % 2) {
                            if (outpCard.exp2[slot - 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp2[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP2_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp2[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp2[slot].portType[i] =
                                        outpCard.exp2[slot -
                                                      1].portType[i];
                                    outpPortConfig(EXP2_SHELF, slot, i,
                                                   outpCard.
                                                   exp2[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp2[slot].reg0);
                                }
                                if (outpCard.exp2[slot - 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP2_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp2[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP2_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp2[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        } else {
                            if (outpCard.exp2[slot + 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp2[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP2_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp2[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp2[slot].portType[i] =
                                        outpCard.exp2[slot +
                                                      1].portType[i];
                                    outpPortConfig(EXP2_SHELF, slot, i,
                                                   outpCard.
                                                   exp2[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp2[slot].reg0);
                                }
                                if (outpCard.exp2[slot + 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP2_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp2[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP2_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp2[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        }
                    }
                } else {
                    outpPortActivate(EXP2_SHELF, slot, OUTPUT_OFF);
                }
            } else {
                if (outpCard.exp2[slot].status != OUTPUT_ABSENT) {

                    /* a output card is removed from EXP2 shelf */
                    cardChange = TRUE;
                    outpCard.exp2[slot].status = OUTPUT_ABSENT;
                    
					/* sendAlarm(a output card is removed); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVECARD), slot, shelf_index);
						
						/* checkitout
							TrapEventLog(smCardRemoveEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVECARD), slot,shelf_index);
						*/
						
						#ifdef SM_DEBUG
							printf("in sm, card [%d] is removed in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    
                    if (outpCard.exp2[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE) {
                        outpCard.exp2[slot].redundancy_state == REDUNDANCY_STANDBY;

                        /* redundancy switch */
                        if (slot % 2) {
                            outpCard.exp2[slot - 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP2_SHELF, slot - 1,
                                             OUTPUT_ON);
                        } else {
                            outpCard.exp2[slot + 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP2_SHELF, slot + 1,
                                             OUTPUT_ON);
                        }
                    }
                }
            }
        }
        if (cardChange) {

            /* Save configs to Flash */
            smDataAccess("smcard.dat", FLASH_WRITE, SM_OUTPUT_CARD);
            smDataAccess("smexp2.out", FLASH_WRITE, SM_EXP2_OUTPUT_PORT);
        }
        break;
    case EXP3_SHELF:

        /* get output cards info from EXP3 shelf */
        expFPGAAccess(EXP3_SHELF, 0x00, 0x03, &card, FPGA_READ);
        cardChange = FALSE;
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {
                if (outpCard.exp3[slot].status != OUTPUT_PRESENT) {

                    /* a new output card is added to EXP3 shelf */
                    cardChange = TRUE;
                    outpCard.exp3[slot].status = OUTPUT_PRESENT;
                    
					/* sendAlarm(a new card is added); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDCARD), slot, shelf_index);
												
						/* checkitout
							TrapEventLog(smCardAddEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDCARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, a new card [%d] is added in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    cardType = cardTypeGet(EXP3_SHELF, slot);
                    if ((cardType != outpCard.exp3[slot].cardType) &&
                        (outpCard.exp3[slot].cardType != OUTPUT_NULL)) {
                        outpCard.exp3[slot].status = OUTPUT_MISMATCH;
                        
							/* sendAlarm(misMatch); */
								SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
													
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

								#ifdef SM_DEBUG
									printf("in sm, output Card mismatch !!\n");
								#endif	
                        
                    } else if (outpCard.exp3[slot].cardType == OUTPUT_NULL)
                        outpCard.exp3[slot].cardType = cardType;
                    if (outpCard.exp3[slot].status == OUTPUT_PRESENT) {
                        if (outpCard.exp3[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP3_SHELF, slot, port,
                                               outpCard.
                                               exp3[slot].
                                               portType[port],
                                               outpCard.exp3[slot].reg0);
                            }

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP3_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp3[slot].reg1,
                                          FPGA_WRITE);
                        } else if (outpCard.exp3[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP3_SHELF, slot, port,
                                               outpCard.
                                               exp3[slot].
                                               portType[port],
                                               outpCard.exp3[slot].reg0);
                            }
                        } else if (outpCard.exp3[slot].cardType ==
                                   OUTPUT_IRIGB) {

                            /* config time of date */
                        } else if (outpCard.exp3[slot].cardType ==
                                   OUTPUT_TIU) {
                            for (port = 0; port < NUM_PORTS; port++)
                                outpPortConfig(EXP3_SHELF, slot, port,
                                               outpCard.
                                               exp3[slot].
                                               portType[port],
                                               outpCard.exp3[slot].reg0);

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP3_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp3[slot].reg1,
                                          FPGA_WRITE);
                        }

                        /* check Redundancy */
                        if (slot % 2) {
                            if (outpCard.exp3[slot - 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp3[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP3_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp3[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp3[slot].portType[i] =
                                        outpCard.exp3[slot -
                                                      1].portType[i];
                                    outpPortConfig(EXP3_SHELF, slot, i,
                                                   outpCard.
                                                   exp3[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp3[slot].reg0);
                                }
                                if (outpCard.exp3[slot - 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP3_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp3[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP3_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp3[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        } else {
                            if (outpCard.exp3[slot + 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp3[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP3_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp3[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp3[slot].portType[i] =
                                        outpCard.exp3[slot +
                                                      1].portType[i];
                                    outpPortConfig(EXP3_SHELF, slot, i,
                                                   outpCard.
                                                   exp3[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp3[slot].reg0);
                                }
                                if (outpCard.exp3[slot + 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP3_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp3[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP3_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp3[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        }
                    }
                } else {
                    outpPortActivate(EXP3_SHELF, slot, OUTPUT_OFF);
                }
            } else {
                if (outpCard.exp3[slot].status != OUTPUT_ABSENT) {

                    /* a output card is removed from EXP3 shelf */
                    cardChange = TRUE;
                    outpCard.exp3[slot].status = OUTPUT_ABSENT;
                   
					/* sendAlarm(a output card is removed); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVECARD), slot, shelf_index);
												
						/* checkitout
						TrapEventLog(smCardRemoveEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVECARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, card [%d] is removed in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    
                    if (outpCard.exp3[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE) {
                        outpCard.exp3[slot].redundancy_state == REDUNDANCY_STANDBY;

                        /* redundancy switch */
                        if (slot % 2) {
                            outpCard.exp3[slot - 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP3_SHELF, slot - 1,
                                             OUTPUT_ON);
                        } else {
                            outpCard.exp3[slot + 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP3_SHELF, slot + 1,
                                             OUTPUT_ON);
                        }
                    }
                }
            }
        }
        expFPGAAccess(EXP3_SHELF, 0x00, 0x04, &card, FPGA_READ);
        for (slot = 8; slot < 16; slot++) {
            if (card & (1 << (slot - 8))) {
                if (outpCard.exp3[slot].status != OUTPUT_PRESENT) {

                    /* a new output card is added to EXP3 shelf */
                    cardChange = TRUE;
                    outpCard.exp3[slot].status = OUTPUT_PRESENT;
                    
					/* sendAlarm(a new card is added); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDCARD), slot, shelf_index);
												
						/* checkitout
							TrapEventLog(smCardAddEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDCARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, a new card [%d] is added in shelf [%d]\n", slot, shelf_index);
						#endif	
											
                    cardType = cardTypeGet(EXP3_SHELF, slot);
                    if ((cardType != outpCard.exp3[slot].cardType) &&
                        (outpCard.exp3[slot].cardType != OUTPUT_NULL)) {
                        outpCard.exp3[slot].status = OUTPUT_MISMATCH;
                        
							/* sendAlarm(misMatch); */
								SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
								
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

								#ifdef SM_DEBUG
									printf("in sm, output Card mismatch !!\n");
								#endif	
													
                    } else if (outpCard.exp3[slot].cardType == OUTPUT_NULL)
                        outpCard.exp3[slot].cardType = cardType;
                    if (outpCard.exp3[slot].status == OUTPUT_PRESENT) {
                        if (outpCard.exp3[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP3_SHELF, slot, port,
                                               outpCard.
                                               exp3[slot].
                                               portType[port],
                                               outpCard.exp3[slot].reg0);
                            }

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP3_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp3[slot].reg1,
                                          FPGA_WRITE);
                        } else if (outpCard.exp3[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP3_SHELF, slot, port,
                                               outpCard.
                                               exp3[slot].
                                               portType[port],
                                               outpCard.exp3[slot].reg0);
                            }
                        } else if (outpCard.exp3[slot].cardType ==
                                   OUTPUT_IRIGB) {

                            /* config time of date */
                        } else if (outpCard.exp3[slot].cardType ==
                                   OUTPUT_TIU) {
                            for (port = 0; port < NUM_PORTS; port++)
                                outpPortConfig(EXP3_SHELF, slot, port,
                                               outpCard.
                                               exp3[slot].
                                               portType[port],
                                               outpCard.exp3[slot].reg0);

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP3_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp3[slot].reg1,
                                          FPGA_WRITE);
                        }

                        /* check Redundancy */
                        if (slot % 2) {
                            if (outpCard.exp3[slot - 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp3[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP3_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp3[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp3[slot].portType[i] =
                                        outpCard.exp3[slot -
                                                      1].portType[i];
                                    outpPortConfig(EXP3_SHELF, slot, i,
                                                   outpCard.
                                                   exp3[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp3[slot].reg0);
                                }
                                if (outpCard.exp3[slot - 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP3_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp3[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP3_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp3[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        } else {
                            if (outpCard.exp3[slot + 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp3[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP3_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp3[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp3[slot].portType[i] =
                                        outpCard.exp3[slot +
                                                      1].portType[i];
                                    outpPortConfig(EXP3_SHELF, slot, i,
                                                   outpCard.
                                                   exp3[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp3[slot].reg0);
                                }
                                if (outpCard.exp3[slot + 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP3_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp3[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP3_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp3[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        }
                    }
                } else {
                    outpPortActivate(EXP3_SHELF, slot, OUTPUT_OFF);
                }
            } else {
                if (outpCard.exp3[slot].status != OUTPUT_ABSENT) {

                    /* a output card is removed from EXP3 shelf */
                    cardChange = TRUE;
                    outpCard.exp3[slot].status = OUTPUT_ABSENT;


					/* sendAlarm(a output card is removed); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVECARD), slot, shelf_index);
												
						/* checkitout
						TrapEventLog(smCardRemoveEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVECARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, card [%d] is removed in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    if (outpCard.exp3[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE) {
                        outpCard.exp3[slot].redundancy_state == REDUNDANCY_STANDBY;

                        /* redundancy switch */
                        if (slot % 2) {
                            outpCard.exp3[slot - 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP3_SHELF, slot - 1,
                                             OUTPUT_ON);
                        } else {
                            outpCard.exp3[slot + 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP3_SHELF, slot + 1,
                                             OUTPUT_ON);
                        }
                    }
                }
            }
        }
        if (cardChange) {

            /* Save configs to Flash */
            smDataAccess("smcard.dat", FLASH_WRITE, SM_OUTPUT_CARD);
            smDataAccess("smexp3.out", FLASH_WRITE, SM_EXP3_OUTPUT_PORT);
        }
        break;
    case EXP4_SHELF:

        /* get output cards info from EXP4 shelf */
        expFPGAAccess(EXP4_SHELF, 0x00, 0x03, &card, FPGA_READ);
        cardChange = FALSE;
        for (slot = 0; slot < 8; slot++) {
            if (card & (1 << slot)) {
                if (outpCard.exp4[slot].status != OUTPUT_PRESENT) {

                    /* a new output card is added to EXP4 shelf */
                    cardChange = TRUE;
                    outpCard.exp4[slot].status = OUTPUT_PRESENT;
                    
					/* sendAlarm(a new card is added); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDCARD), slot, shelf_index);
												
						/* checkitout
							TrapEventLog(smCardAddEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDCARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, a new card [%d] is added in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    cardType = cardTypeGet(EXP4_SHELF, slot);
                    if ((cardType != outpCard.exp4[slot].cardType) &&
                        (outpCard.exp4[slot].cardType != OUTPUT_NULL)) {
                        outpCard.exp4[slot].status = OUTPUT_MISMATCH;
                        
                       
							/* sendAlarm(misMatch); */
								SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
								
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

								#ifdef SM_DEBUG
									printf("in sm, output Card mismatch !!\n");
								#endif	
	
	
                    } else if (outpCard.exp4[slot].cardType == OUTPUT_NULL)
                        outpCard.exp4[slot].cardType = cardType;
                    if (outpCard.exp4[slot].status == OUTPUT_PRESENT) {
                        if (outpCard.exp4[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP4_SHELF, slot, port,
                                               outpCard.
                                               exp4[slot].
                                               portType[port],
                                               outpCard.exp4[slot].reg0);
                            }

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP4_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp4[slot].reg1,
                                          FPGA_WRITE);
                        } else if (outpCard.exp4[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP4_SHELF, slot, port,
                                               outpCard.
                                               exp4[slot].
                                               portType[port],
                                               outpCard.exp4[slot].reg0);
                            }
                        } else if (outpCard.exp4[slot].cardType ==
                                   OUTPUT_IRIGB) {

                            /* config time of date */
                        } else if (outpCard.exp4[slot].cardType ==
                                   OUTPUT_TIU) {
                            for (port = 0; port < NUM_PORTS; port++)
                                outpPortConfig(EXP4_SHELF, slot, port,
                                               outpCard.
                                               exp4[slot].
                                               portType[port],
                                               outpCard.exp4[slot].reg0);

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP4_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp4[slot].reg1,
                                          FPGA_WRITE);
                        }

                        /* check Redundancy */
                        if (slot % 2) {
                            if (outpCard.exp4[slot - 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp4[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP4_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp4[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp4[slot].portType[i] =
                                        outpCard.exp4[slot -
                                                      1].portType[i];
                                    outpPortConfig(EXP4_SHELF, slot, i,
                                                   outpCard.
                                                   exp4[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp4[slot].reg0);
                                }
                                if (outpCard.exp4[slot - 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP4_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp4[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP4_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp4[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        } else {
                            if (outpCard.exp4[slot + 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp4[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP4_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp4[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp4[slot].portType[i] =
                                        outpCard.exp4[slot +
                                                      1].portType[i];
                                    outpPortConfig(EXP4_SHELF, slot, i,
                                                   outpCard.
                                                   exp4[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp4[slot].reg0);
                                }
                                if (outpCard.exp4[slot + 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP4_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp4[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP4_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp4[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        }
                    }
                } else {
                    outpPortActivate(EXP4_SHELF, slot, OUTPUT_OFF);
                }
            } else {
                if (outpCard.exp4[slot].status != OUTPUT_ABSENT) {

                    /* a output card is removed from EXP4 shelf */
                    cardChange = TRUE;
                    outpCard.exp4[slot].status = OUTPUT_ABSENT;
                    
					/* sendAlarm(a output card is removed); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVECARD), slot, shelf_index);
												
						/* checkitout
						TrapEventLog(smCardRemoveEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVECARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, card [%d] is removed in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    if (outpCard.exp4[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE) {
                        outpCard.exp4[slot].redundancy_state == REDUNDANCY_STANDBY;

                        /* redundancy switch */
                        if (slot % 2) {
                            outpCard.exp4[slot - 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP4_SHELF, slot - 1,
                                             OUTPUT_ON);
                        } else {
                            outpCard.exp4[slot + 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP4_SHELF, slot + 1,
                                             OUTPUT_ON);
                        }
                    }
                }
            }
        }
        expFPGAAccess(EXP4_SHELF, 0x00, 0x04, &card, FPGA_READ);
        for (slot = 8; slot < 16; slot++) {
            if (card & (1 << (slot - 8))) {
                if (outpCard.exp4[slot].status != OUTPUT_PRESENT) {

                    /* a new output card is added to EXP4 shelf */
                    cardChange = TRUE;
                    outpCard.exp4[slot].status = OUTPUT_PRESENT;
                    
					/* sendAlarm(a new card is added); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ADDCARD), slot, shelf_index);
												
						/* checkitout
							TrapEventLog(smCardAddEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ADDCARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, a new card [%d] is added in shelf [%d]\n", slot, shelf_index);
						#endif	
                    
                    cardType = cardTypeGet(EXP4_SHELF, slot);
                    if ((cardType != outpCard.exp4[slot].cardType) &&
                        (outpCard.exp4[slot].cardType != OUTPUT_NULL)) {
                        outpCard.exp4[slot].status = OUTPUT_MISMATCH;
                        
							/* sendAlarm(misMatch); */
								SiwEventLog(NULL,  SIW_EVENT_SYM(SM, MISMATCH), slot, shelf_index);
								
						snmpTrapCardReply.stsCardShelf	=	shelf_index+1;
						snmpTrapCardReply.stsCardIndex	=	slot+1;
						snmpTrapCardReply.stsCardType		=	cardType+1; /*checkitout*/
						snmpTrapCardReply.stsCardOperStatus		=	D_stsCardOperStatus_misMatch;		
						TrapEventLog(stsCardStatusChange_TRAP, snmpTrapCardReply.stsCardOperStatus, shelf_index,  slot, 0, TRAP_MINOR, (U32_t *)&snmpTrapCardReply, SIW_EVENT_SYM(SM, MISMATCH), slot,shelf_index);

								#ifdef SM_DEBUG
									printf("in sm, output Card mismatch !!\n");
								#endif	
	

                    } else if (outpCard.exp4[slot].cardType == OUTPUT_NULL)
                        outpCard.exp4[slot].cardType = cardType;
                    if (outpCard.exp4[slot].status == OUTPUT_PRESENT) {
                        if (outpCard.exp4[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP4_SHELF, slot, port,
                                               outpCard.
                                               exp4[slot].
                                               portType[port],
                                               outpCard.exp4[slot].reg0);
                            }

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP4_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp4[slot].reg1,
                                          FPGA_WRITE);
                        } else if (outpCard.exp4[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < NUM_PORTS; port++) {
                                outpPortConfig(EXP4_SHELF, slot, port,
                                               outpCard.
                                               exp4[slot].
                                               portType[port],
                                               outpCard.exp4[slot].reg0);
                            }
                        } else if (outpCard.exp4[slot].cardType ==
                                   OUTPUT_IRIGB) {

                            /* config time of date */
                        } else if (outpCard.exp4[slot].cardType ==
                                   OUTPUT_TIU) {
                            for (port = 0; port < NUM_PORTS; port++)
                                outpPortConfig(EXP4_SHELF, slot, port,
                                               outpCard.
                                               exp4[slot].
                                               portType[port],
                                               outpCard.exp4[slot].reg0);

                            /* update SSM to FPGA */
                            expFPGAAccess(EXP4_SHELF, (0x10 + slot),
                                          0x01,
                                          &outpCard.exp4[slot].reg1,
                                          FPGA_WRITE);
                        }

                        /* check Redundancy */
                        if (slot % 2) {
                            if (outpCard.exp4[slot - 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp4[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP4_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp4[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp4[slot].portType[i] =
                                        outpCard.exp4[slot -
                                                      1].portType[i];
                                    outpPortConfig(EXP4_SHELF, slot, i,
                                                   outpCard.
                                                   exp4[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp4[slot].reg0);
                                }
                                if (outpCard.exp4[slot - 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP4_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp4[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP4_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp4[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        } else {
                            if (outpCard.exp4[slot + 1].redundancy_enable ==
                                REDUNDANCY_DISABLE) {

                                /* Redundancy is not enabled */
                                outpCard.exp4[slot].redundancy_enable =
                                    REDUNDANCY_DISABLE;
                                outpPortActivate(EXP4_SHELF, slot,
                                                 OUTPUT_ON);
                            } else {

                                /* Redundancy is enabled */
                                outpCard.exp4[slot].redundancy_enable =
                                    REDUNDANCY_ENABLE;
                                for (i = 0; i < 8; i++) {
                                    outpCard.exp4[slot].portType[i] =
                                        outpCard.exp4[slot +
                                                      1].portType[i];
                                    outpPortConfig(EXP4_SHELF, slot, i,
                                                   outpCard.
                                                   exp4[slot].
                                                   portType[i],
                                                   outpCard.
                                                   exp4[slot].reg0);
                                }
                                if (outpCard.exp4[slot + 1].status ==
                                    OUTPUT_PRESENT) {
                                    outpPortActivate(EXP4_SHELF, slot,
                                                     OUTPUT_OFF);
                                    outpCard.exp4[slot].redundancy_state =
                                        REDUNDANCY_STANDBY;
                                } else {
                                    outpPortActivate(EXP4_SHELF, slot,
                                                     OUTPUT_ON);
                                    outpCard.exp4[slot].redundancy_state =
                                        REDUNDANCY_ACTIVE;
                                }
                            }
                        }
                    }
                } else {
                    outpPortActivate(EXP4_SHELF, slot, OUTPUT_OFF);
                }
            } else {
                if (outpCard.exp4[slot].status != OUTPUT_ABSENT) {

                    /* a output card is removed from EXP4 shelf */
                    cardChange = TRUE;
                    outpCard.exp4[slot].status = OUTPUT_ABSENT;
                    
                    /* sendAlarm(a output card is removed); */
						SiwEventLog(NULL,  SIW_EVENT_SYM(SM, REMOVECARD), slot, shelf_index);
												
						/* checkitout
						TrapEventLog(smCardRemoveEvent_TRAP, shelf_index,  slot, 0, 0, dumPtr, SIW_EVENT_SYM(SM, REMOVECARD), slot,shelf_index);
						*/

						#ifdef SM_DEBUG
							printf("in sm, card [%d] is removed in shelf [%d]\n", slot, shelf_index);
						#endif
						
                    if (outpCard.exp4[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE) {
                        outpCard.exp4[slot].redundancy_state == REDUNDANCY_STANDBY;

                        /* redundancy switch */
                        if (slot % 2) {
                            outpCard.exp4[slot - 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP4_SHELF, slot - 1,
                                             OUTPUT_ON);
                        } else {
                            outpCard.exp4[slot + 1].redundancy_state =
                                REDUNDANCY_ACTIVE;
                            outpPortActivate(EXP4_SHELF, slot + 1,
                                             OUTPUT_ON);
                        }
                    }
                }
            }
        }
        if (cardChange) {

            /* Save configs to Flash */
            smDataAccess("smcard.dat", FLASH_WRITE, SM_OUTPUT_CARD);
            smDataAccess("smexp4.out", FLASH_WRITE, SM_EXP4_OUTPUT_PORT);
        }
        break;
    default:
        printf("outpCardInit: invalid shelf: %d\n", shelf_index);
        break;
    }
    return (cardChange);
}

STATUS_t redundancySwitch(SHELF_t shelf_index, int slot)
{
    switch (shelf_index) {
    case MAIN_SHELF:
        if ((slot < 0) || (slot > 8)) {
            printf("redundancySwitch: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        }
        if (outpCard.main[slot].redundancy_enable == REDUNDANCY_ENABLE) {
            outpCard.main[slot].redundancy_state = REDUNDANCY_STANDBY;
            outpPortActivate(MAIN_SHELF, slot, OUTPUT_OFF);
            if (slot % 2) {
                if (outpCard.main[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.main[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(MAIN_SHELF, slot - 1, OUTPUT_ON);
                }
            } else {
                if (outpCard.main[slot + 1].status == OUTPUT_PRESENT) {
                    outpCard.main[slot + 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(MAIN_SHELF, slot + 1, OUTPUT_ON);
                }
            }
        }
        break;
    case EXP1_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf
                ("redundancySwitch-EXP1_SHELF: Invalid Shelf/Slot: %d/%d\n",
                 shelf_index, slot);
            return ERROR;
        }
        if (outpCard.exp1[slot].redundancy_enable == REDUNDANCY_ENABLE) {
            outpCard.exp1[slot].redundancy_state = REDUNDANCY_STANDBY;
            outpPortActivate(EXP1_SHELF, slot, OUTPUT_OFF);
            if (slot % 2) {
                if (outpCard.exp1[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.exp1[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP1_SHELF, slot - 1, OUTPUT_ON);
                }
            } else {
                if (outpCard.exp1[slot + 1].status == OUTPUT_PRESENT) {
                    outpCard.exp1[slot + 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP1_SHELF, slot + 1, OUTPUT_ON);
                }
            }
        }
        break;
    case EXP2_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf
                ("redundancySwitch-EXP2_SHELF: Invalid Shelf/Slot: %d/%d\n",
                 shelf_index, slot);
            return ERROR;
        }
        if (outpCard.exp2[slot].redundancy_enable == REDUNDANCY_ENABLE) {
            outpCard.exp2[slot].redundancy_state = REDUNDANCY_STANDBY;
            outpPortActivate(EXP2_SHELF, slot, OUTPUT_OFF);
            if (slot % 2) {
                if (outpCard.exp2[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.exp2[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP2_SHELF, slot - 1, OUTPUT_ON);
                }
            } else {
                if (outpCard.exp2[slot + 1].status == OUTPUT_PRESENT) {
                    outpCard.exp2[slot + 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP2_SHELF, slot + 1, OUTPUT_ON);
                }
            }
        }
        break;
    case EXP3_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf
                ("redundancySwitch-EXP3_SHELF: Invalid Shelf/Slot: %d/%d\n",
                 shelf_index, slot);
            return ERROR;
        }
        if (outpCard.exp3[slot].redundancy_enable == REDUNDANCY_ENABLE) {
            outpCard.exp3[slot].redundancy_state = REDUNDANCY_STANDBY;
            outpPortActivate(EXP3_SHELF, slot, OUTPUT_OFF);
            if (slot % 2) {
                if (outpCard.exp3[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.exp3[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP3_SHELF, slot - 1, OUTPUT_ON);
                }
            } else {
                if (outpCard.exp3[slot + 1].status == OUTPUT_PRESENT) {
                    outpCard.exp3[slot + 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP3_SHELF, slot + 1, OUTPUT_ON);
                }
            }
        }
        break;
    case EXP4_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf
                ("redundancySwitch-EXP4_SHELF: Invalid Shelf/Slot: %d/%d\n",
                 shelf_index, slot);
            return ERROR;
        }
        if (outpCard.exp4[slot].redundancy_enable == REDUNDANCY_ENABLE) {
            outpCard.exp4[slot].redundancy_state = REDUNDANCY_STANDBY;
            outpPortActivate(EXP4_SHELF, slot, OUTPUT_OFF);
            if (slot % 2) {
                if (outpCard.exp4[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.exp4[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP4_SHELF, slot - 1, OUTPUT_ON);
                }
            } else {
                if (outpCard.exp4[slot + 1].status == OUTPUT_PRESENT) {
                    outpCard.exp4[slot + 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP4_SHELF, slot + 1, OUTPUT_ON);
                }
            }
        }
        break;
    default:
        printf("redundancySwitch: Invalid Shelf: %d\n", shelf_index);
        return ERROR;
    }

    /* Save configs to Flash */
    smDataAccess("smcard.dat", FLASH_WRITE, SM_OUTPUT_CARD);
    return OK;
}


/**************************************************************************************
* function:  STATUS_t redundancyConfig(SHELF_t shelf_index, int slot, int action)
*  input 
*			
*			
*  return 
*     
***************************************************************************************/
STATUS_t redundancyConfig(SHELF_t shelf_index, int slot, int action)
{
    switch (shelf_index) {
    case MAIN_SHELF:
        if ((slot < 0) || (slot > 8)) {
            printf("redundancyConfig: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        }
        if (action == REDUNDANCY_DISABLE) {
            outpCard.main[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpPortActivate(MAIN_SHELF, slot, OUTPUT_ON);
            if (slot % 2) {
                outpCard.main[slot - 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.main[slot - 1].status == OUTPUT_PRESENT)
                    outpPortActivate(MAIN_SHELF, slot - 1, OUTPUT_ON);
            } else {
                outpCard.main[slot + 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.main[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(MAIN_SHELF, slot + 1, OUTPUT_ON);
            }
        } else {
            if (slot % 2) {
                outpCard.main[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.main[slot - 1].redundancy_enable = REDUNDANCY_ENABLE;
                if (outpCard.main[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.main[slot].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.main[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(MAIN_SHELF, slot, OUTPUT_OFF);
                    outpPortActivate(MAIN_SHELF, slot - 1, OUTPUT_ON);
                } else {
                    outpCard.main[slot - 1].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.main[slot].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(MAIN_SHELF, slot, OUTPUT_ON);
                }
            } else {
                outpCard.main[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.main[slot + 1].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.main[slot].redundancy_state = REDUNDANCY_ACTIVE;
                outpCard.main[slot + 1].redundancy_state = REDUNDANCY_STANDBY;
                outpPortActivate(MAIN_SHELF, slot, OUTPUT_ON);
                if (outpCard.main[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(MAIN_SHELF, slot + 1, OUTPUT_OFF);
            }
        }
        break;
    case EXP1_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf
                ("redundancyConfig-EXP1_SHELF: Invalid Shelf/Slot: %d/%d\n",
                 shelf_index, slot);
            return ERROR;
        }
        if (action == REDUNDANCY_DISABLE) {
            outpCard.exp1[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpPortActivate(EXP1_SHELF, slot, OUTPUT_ON);
            if (slot % 2) {
                outpCard.exp1[slot - 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.exp1[slot - 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP1_SHELF, slot - 1, OUTPUT_ON);
            } else {
                outpCard.exp1[slot + 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.exp1[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP1_SHELF, slot + 1, OUTPUT_ON);
            }
        } else {
            if (slot % 2) {
                outpCard.exp1[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp1[slot - 1].redundancy_enable = REDUNDANCY_ENABLE;
                if (outpCard.exp1[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.exp1[slot].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.exp1[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP1_SHELF, slot, OUTPUT_OFF);
                    outpPortActivate(EXP1_SHELF, slot - 1, OUTPUT_ON);
                } else {
                    outpCard.exp1[slot - 1].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.exp1[slot].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP1_SHELF, slot, OUTPUT_ON);
                }
            } else {
                outpCard.exp1[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp1[slot + 1].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp1[slot].redundancy_state = REDUNDANCY_ACTIVE;
                outpCard.exp1[slot + 1].redundancy_state = REDUNDANCY_STANDBY;
                outpPortActivate(EXP1_SHELF, slot, OUTPUT_ON);
                if (outpCard.exp1[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP1_SHELF, slot + 1, OUTPUT_OFF);
            }
        }
        break;
    case EXP2_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf
                ("outpCardSwitch-EXP2_SHELF: Invalid Shelf/Slot: %d/%d\n",
                 shelf_index, slot);
            return ERROR;
        }
        if (action == REDUNDANCY_DISABLE) {
            outpCard.exp2[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpPortActivate(EXP2_SHELF, slot, OUTPUT_ON);
            if (slot % 2) {
                outpCard.exp2[slot - 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.exp2[slot - 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP2_SHELF, slot - 1, OUTPUT_ON);
            } else {
                outpCard.exp2[slot + 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.exp2[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP2_SHELF, slot + 1, OUTPUT_ON);
            }
        } else {
            if (slot % 2) {
                outpCard.exp2[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp2[slot - 1].redundancy_enable = REDUNDANCY_ENABLE;
                if (outpCard.exp1[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.exp2[slot].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.exp2[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP2_SHELF, slot, OUTPUT_OFF);
                    outpPortActivate(EXP2_SHELF, slot - 1, OUTPUT_ON);
                } else {
                    outpCard.exp2[slot - 1].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.exp2[slot].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP2_SHELF, slot, OUTPUT_ON);
                }
            } else {
                outpCard.exp2[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp2[slot + 1].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp2[slot].redundancy_state = REDUNDANCY_ACTIVE;
                outpCard.exp2[slot + 1].redundancy_state = REDUNDANCY_STANDBY;
                outpPortActivate(EXP2_SHELF, slot, OUTPUT_ON);
                if (outpCard.exp2[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP2_SHELF, slot + 1, OUTPUT_OFF);
            }
        }
        break;
    case EXP3_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf
                ("outpCardSwitch-EXP3_SHELF: Invalid Shelf/Slot: %d/%d\n",
                 shelf_index, slot);
            return ERROR;
        }
        if (action == REDUNDANCY_DISABLE) {
            outpCard.exp3[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpPortActivate(EXP3_SHELF, slot, OUTPUT_ON);
            if (slot % 2) {
                outpCard.exp3[slot - 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.exp3[slot - 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP3_SHELF, slot - 1, OUTPUT_ON);
            } else {
                outpCard.exp3[slot + 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.exp3[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP3_SHELF, slot + 1, OUTPUT_ON);
            }
        } else {
            if (slot % 2) {
                outpCard.exp3[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp3[slot - 1].redundancy_enable = REDUNDANCY_ENABLE;
                if (outpCard.exp3[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.exp3[slot].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.exp3[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP3_SHELF, slot, OUTPUT_OFF);
                    outpPortActivate(EXP3_SHELF, slot - 1, OUTPUT_ON);
                } else {
                    outpCard.exp3[slot - 1].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.exp3[slot].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP3_SHELF, slot, OUTPUT_ON);
                }
            } else {
                outpCard.exp3[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp3[slot + 1].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp3[slot].redundancy_state = REDUNDANCY_ACTIVE;
                outpCard.exp3[slot + 1].redundancy_state = REDUNDANCY_STANDBY;
                outpPortActivate(EXP3_SHELF, slot, OUTPUT_ON);
                if (outpCard.exp3[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP3_SHELF, slot + 1, OUTPUT_OFF);
            }
        }
        break;
    case EXP4_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf
                ("outpCardSwitch-EXP4_SHELF: Invalid Shelf/Slot: %d/%d\n",
                 shelf_index, slot);
            return ERROR;
        }
        if (action == REDUNDANCY_DISABLE) {
            outpCard.exp4[slot].redundancy_enable = REDUNDANCY_DISABLE;
            outpPortActivate(EXP4_SHELF, slot, OUTPUT_ON);
            if (slot % 2) {
                outpCard.exp4[slot - 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.exp4[slot - 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP4_SHELF, slot - 1, OUTPUT_ON);
            } else {
                outpCard.exp4[slot + 1].redundancy_enable = REDUNDANCY_DISABLE;
                if (outpCard.exp4[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP4_SHELF, slot + 1, OUTPUT_ON);
            }
        } else {
            if (slot % 2) {
                outpCard.exp4[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp4[slot - 1].redundancy_enable = REDUNDANCY_ENABLE;
                if (outpCard.exp4[slot - 1].status == OUTPUT_PRESENT) {
                    outpCard.exp4[slot].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.exp4[slot - 1].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP4_SHELF, slot, OUTPUT_OFF);
                    outpPortActivate(EXP4_SHELF, slot - 1, OUTPUT_ON);
                } else {
                    outpCard.exp4[slot - 1].redundancy_state = REDUNDANCY_STANDBY;
                    outpCard.exp4[slot].redundancy_state = REDUNDANCY_ACTIVE;
                    outpPortActivate(EXP4_SHELF, slot, OUTPUT_ON);
                }
            } else {
                outpCard.exp4[slot].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp4[slot + 1].redundancy_enable = REDUNDANCY_ENABLE;
                outpCard.exp4[slot].redundancy_state = REDUNDANCY_ACTIVE;
                outpCard.exp4[slot + 1].redundancy_state = REDUNDANCY_STANDBY;
                outpPortActivate(EXP4_SHELF, slot, OUTPUT_ON);
                if (outpCard.exp4[slot + 1].status == OUTPUT_PRESENT)
                    outpPortActivate(EXP4_SHELF, slot + 1, OUTPUT_OFF);
            }
        }
        break;
    default:
        printf("outpCardSwitch: Invalid Shelf: %d\n", shelf_index);
        return ERROR;
    }

    /* Save configs to Flash */
    smDataAccess("smcard.dat", FLASH_WRITE, SM_OUTPUT_CARD);
    return OK;
}

/**************************************************************************************
* function:  void outpPortActivate(SHELF_t shelf_index, int slot, U8_t mode)
*  input 
*			
*			
*  return 
*     
***************************************************************************************/

void outpPortActivate(SHELF_t shelf_index, int slot, U8_t mode){
	
	
    switch (shelf_index) {
    case MAIN_SHELF:
        *(U8_t *) OUTPUT_FPGA_W_REG_7(slot) = mode;
        break;
    case EXP1_SHELF:
        expFPGAAccess(EXP1_SHELF, (0x10 + slot), 0x06, &mode, FPGA_WRITE);
        break;
    case EXP2_SHELF:
        expFPGAAccess(EXP2_SHELF, (0x10 + slot), 0x06, &mode, FPGA_WRITE);
        break;
    case EXP3_SHELF:
        expFPGAAccess(EXP3_SHELF, (0x10 + slot), 0x06, &mode, FPGA_WRITE);
        break;
    case EXP4_SHELF:
        expFPGAAccess(EXP4_SHELF, (0x10 + slot), 0x06, &mode, FPGA_WRITE);
        break;
    default:
        break;
    }
}


/**************************************************************************************
* function:  STATUS_t outpPortSet(SHELF_t shelf_index, int slot, int port, int type,
                     int framing)
*  input 
*			
*			
*  return 
*     
***************************************************************************************/
STATUS_t outpPortSet(SHELF_t shelf_index, int slot, int port, int type,
                     int framing)
{
    STATUS_t status = OK;
    switch (shelf_index) {
    case MAIN_SHELF:
        if ((slot < 0) || (slot > 8)) {
            printf("outpPortSet-MAIN_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        } else if ((port < 0) || (port > 8)) {
            printf
                ("outpPortSet-MAIN_SHELF: Invalid Shelf/Slot/port: %d/%d/%d\n",
                 shelf_index, slot, port);
            return ERROR;
        }
        outpPortConfig(MAIN_SHELF, slot, port, type, framing);
        break;
    case EXP1_SHELF:
    case EXP2_SHELF:
    case EXP3_SHELF:
    case EXP4_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf("outpPortSet-EXP_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        } else if ((port < 0) || (port > 8)) {
            printf
                ("outpPortSet-EXP1_SHELF: Invalid Shelf/Slot/port: %d/%d/%d\n",
                 shelf_index, slot, port);
            return ERROR;
        }
        outpPortConfig(shelf_index, slot, port, type, framing);
        break;
    default:
        printf("outpPortSet: Invalid Shelf: %d\n", shelf_index);
        status = ERROR;
        break;
    }

    /* Save config to Flash */
    return (status);
}

/**************************************************************************************
* function:  SS8_t outpPortGet(SHELF_t shelf_index, int slot, int port)
*  input 
*			
*			
*  return 
*     
***************************************************************************************/
S8_t outpPortGet(SHELF_t shelf_index, int slot, int port)
{
    S8_t type;
    switch (shelf_index) {
    case MAIN_SHELF:
        if ((slot < 0) || (slot > 8)) {
            printf("outpPortGet-MAIN_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        } else if ((port < 0) || (port > 8)) {
            printf
                ("outpPortGet-MAIN_SHELF: Invalid Shelf/Slot/port: %d/%d/%d\n",
                 shelf_index, slot, port);
            return ERROR;
        }
        type = outpCard.main[slot].portType[port];
        break;
    case EXP1_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf("outpPortGet-EXP1_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        } else if ((port < 0) || (port > 8)) {
            printf
                ("outpPortGet-EXP1_SHELF: Invalid Shelf/Slot/port: %d/%d/%d\n",
                 shelf_index, slot, port);
            return ERROR;
        }
        type = outpCard.exp1[slot].portType[port];
        break;
    case EXP2_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf("outpPortGet-EXP2_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        } else if ((port < 0) || (port > 8)) {
            printf
                ("outpPortGet-EXP2_SHELF: Invalid Shelf/Slot/port: %d/%d/%d\n",
                 shelf_index, slot, port);
            return ERROR;
        }
        type = outpCard.exp2[slot].portType[port];
        break;
    case EXP3_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf("outpPortGet-EXP3_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        } else if ((port < 0) || (port > 8)) {
            printf
                ("outpPortGet-EXP3_SHELF: Invalid Shelf/Slot/port: %d/%d/%d\n",
                 shelf_index, slot, port);
            return ERROR;
        }
        type = outpCard.exp3[slot].portType[port];
        break;
    case EXP4_SHELF:
        if ((slot < 0) || (slot > 16)) {
            printf("outpPortGet-EXP4_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        } else if ((port < 0) || (port > 8)) {
            printf
                ("outpPortGet-EXP4_SHELF: Invalid Shelf/Slot/port: %d/%d/%d\n",
                 shelf_index, slot, port);
            return ERROR;
        }
        type = outpCard.exp4[slot].portType[port];
        break;
    default:
        printf("outpPortGet: Invalid Shelf: %d\n", shelf_index);
        type = OUTPUT_NULL;
        break;
    }
    return (type);
}

/**************************************************************************************
* function:  void outpPortConfig(SHELF_t shelf_index, int slot, int port, int type,
                    int framing)
*  input 
*			
*			
*  return 
*     
***************************************************************************************/

void outpPortConfig(SHELF_t shelf_index, int slot, int port, int type,
                    int framing)
{
    U8_t tmp;
    switch (shelf_index) {
    case MAIN_SHELF:
        if (outpCard.main[slot].cardType == OUTPUT_FREQUENCY) {
            if (type == OUTPUT_T1) {

                /* update to shadow ram */
                outpCard.main[slot].reg2 |= (1 << port);
                outpCard.main[slot].reg3 &= ~(1 << port);
                outpCard.main[slot].reg4 &= ~(1 << port);
                outpCard.main[slot].reg5 &= ~(1 << port);
                outpCard.main[slot].reg0 |= framing;

                /* update to FPGA */
                *(U8_t *) OUTPUT_FPGA_W_REG_1(slot) =
                    outpCard.main[slot].reg0;
                *(U8_t *) OUTPUT_FPGA_W_REG_3(slot) =
                    outpCard.main[slot].reg2;
                *(U8_t *) OUTPUT_FPGA_W_REG_4(slot) =
                    outpCard.main[slot].reg3;
                *(U8_t *) OUTPUT_FPGA_W_REG_5(slot) =
                    outpCard.main[slot].reg4;
                *(U8_t *) OUTPUT_FPGA_W_REG_6(slot) =
                    outpCard.main[slot].reg5;
            } else if (type == OUTPUT_E1) {

                /* update to shadow ram */
                outpCard.main[slot].reg2 &= ~(1 << port);
                outpCard.main[slot].reg3 |= (1 << port);
                outpCard.main[slot].reg4 &= ~(1 << port);
                outpCard.main[slot].reg5 &= ~(1 << port);
                outpCard.main[slot].reg0 |= framing;

                /* update to FPGA */
                *(U8_t *) OUTPUT_FPGA_W_REG_1(slot) =
                    outpCard.main[slot].reg0;
                *(U8_t *) OUTPUT_FPGA_W_REG_3(slot) =
                    outpCard.main[slot].reg2;
                *(U8_t *) OUTPUT_FPGA_W_REG_4(slot) =
                    outpCard.main[slot].reg3;
                *(U8_t *) OUTPUT_FPGA_W_REG_5(slot) =
                    outpCard.main[slot].reg4;
                *(U8_t *) OUTPUT_FPGA_W_REG_6(slot) =
                    outpCard.main[slot].reg5;
            } else if (type == OUTPUT_SQW) {

                /* update to shadow ram */
                outpCard.main[slot].reg2 &= ~(1 << port);
                outpCard.main[slot].reg3 &= ~(1 << port);
                outpCard.main[slot].reg4 |= (1 << port);
                outpCard.main[slot].reg5 &= ~(1 << port);

                /* update to FPGA */
                *(U8_t *) OUTPUT_FPGA_W_REG_3(slot) =
                    outpCard.main[slot].reg2;
                *(U8_t *) OUTPUT_FPGA_W_REG_4(slot) =
                    outpCard.main[slot].reg3;
                *(U8_t *) OUTPUT_FPGA_W_REG_5(slot) =
                    outpCard.main[slot].reg4;
                *(U8_t *) OUTPUT_FPGA_W_REG_6(slot) =
                    outpCard.main[slot].reg5;
            } else if (type == OUTPUT_CC) {

                /* update to shadow ram */
                outpCard.main[slot].reg2 &= ~(1 << port);
                outpCard.main[slot].reg3 &= ~(1 << port);
                outpCard.main[slot].reg4 &= ~(1 << port);
                outpCard.main[slot].reg5 |= (1 << port);

                /* update to FPGA */
                *(U8_t *) OUTPUT_FPGA_W_REG_3(slot) =
                    outpCard.main[slot].reg2;
                *(U8_t *) OUTPUT_FPGA_W_REG_4(slot) =
                    outpCard.main[slot].reg3;
                *(U8_t *) OUTPUT_FPGA_W_REG_5(slot) =
                    outpCard.main[slot].reg4;
                *(U8_t *) OUTPUT_FPGA_W_REG_6(slot) =
                    outpCard.main[slot].reg5;
            }
        } else if (outpCard.main[slot].cardType == OUTPUT_TIMING) {
            if (type == OUTPUT_1PPS) {

                /* update to shadow ram */
                outpCard.main[slot].reg2 |= (1 << port);
                outpCard.main[slot].reg3 &= ~(1 << port);
                outpCard.main[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                *(U8_t *) OUTPUT_FPGA_W_REG_3(slot) =
                    outpCard.main[slot].reg2;
                *(U8_t *) OUTPUT_FPGA_W_REG_4(slot) =
                    outpCard.main[slot].reg3;
                *(U8_t *) OUTPUT_FPGA_W_REG_5(slot) =
                    outpCard.main[slot].reg4;
            } else if (type == OUTPUT_1PPM) {

                /* update to shadow ram */
                outpCard.main[slot].reg2 &= ~(1 << port);
                outpCard.main[slot].reg3 |= (1 << port);
                outpCard.main[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                *(U8_t *) OUTPUT_FPGA_W_REG_3(slot) =
                    outpCard.main[slot].reg2;
                *(U8_t *) OUTPUT_FPGA_W_REG_4(slot) =
                    outpCard.main[slot].reg3;
                *(U8_t *) OUTPUT_FPGA_W_REG_5(slot) =
                    outpCard.main[slot].reg4;
            } else if (type == OUTPUT_1PPH) {

                /* update to shadow ram */
                outpCard.main[slot].reg2 &= ~(1 << port);
                outpCard.main[slot].reg3 &= ~(1 << port);
                outpCard.main[slot].reg4 |= (1 << port);

                /* update to FPGA */
                *(U8_t *) OUTPUT_FPGA_W_REG_3(slot) =
                    outpCard.main[slot].reg2;
                *(U8_t *) OUTPUT_FPGA_W_REG_4(slot) =
                    outpCard.main[slot].reg3;
                *(U8_t *) OUTPUT_FPGA_W_REG_5(slot) =
                    outpCard.main[slot].reg4;
            }
        } else if (outpCard.main[slot].cardType == OUTPUT_IRIGB) {
        } else if (outpCard.main[slot].cardType == OUTPUT_TIU) {

            /* update shadow ram */
            outpCard.main[slot].reg0 |= framing;

            /* update to FPGA */
            *(U8_t *) OUTPUT_FPGA_W_REG_1(slot) = outpCard.main[slot].reg0;
        } else if (outpCard.main[slot].cardType == OUTPUT_5_10MHZ) {
        }
        break;
    case EXP1_SHELF:
        if (outpCard.exp1[slot].cardType == OUTPUT_FREQUENCY) {
            if (type == OUTPUT_T1) {

                /* update to shadow ram */
                outpCard.exp1[slot].reg2 |= (1 << port);
                outpCard.exp1[slot].reg3 &= ~(1 << port);
                outpCard.exp1[slot].reg4 &= ~(1 << port);
                outpCard.exp1[slot].reg5 &= ~(1 << port);
                outpCard.exp1[slot].reg0 |= framing;

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                              &outpCard.exp1[slot].reg0, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp1[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp1[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp1[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp1[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_E1) {

                /* update to shadow ram */
                outpCard.exp1[slot].reg2 &= ~(1 << port);
                outpCard.exp1[slot].reg3 |= (1 << port);
                outpCard.exp1[slot].reg4 &= ~(1 << port);
                outpCard.exp1[slot].reg5 &= ~(1 << port);
                outpCard.exp1[slot].reg0 |= framing;

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                              &outpCard.exp1[slot].reg0, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp1[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp1[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp1[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp1[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_SQW) {

                /* update to shadow ram */
                outpCard.exp1[slot].reg2 &= ~(1 << port);
                outpCard.exp1[slot].reg3 &= ~(1 << port);
                outpCard.exp1[slot].reg4 |= (1 << port);
                outpCard.exp1[slot].reg5 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp1[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp1[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp1[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp1[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_CC) {

                /* update to shadow ram */
                outpCard.exp1[slot].reg2 &= ~(1 << port);
                outpCard.exp1[slot].reg3 &= ~(1 << port);
                outpCard.exp1[slot].reg4 &= ~(1 << port);
                outpCard.exp1[slot].reg5 |= (1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp1[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp1[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp1[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp1[slot].reg5, FPGA_WRITE);
            }
        } else if (outpCard.exp1[slot].cardType == OUTPUT_TIMING) {
            if (type == OUTPUT_1PPS) {

                /* update to shadow ram */
                outpCard.exp1[slot].reg2 |= (1 << port);
                outpCard.exp1[slot].reg3 &= ~(1 << port);
                outpCard.exp1[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp1[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp1[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp1[slot].reg4, FPGA_WRITE);
            } else if (type == OUTPUT_1PPM) {

                /* update to shadow ram */
                outpCard.exp1[slot].reg2 &= ~(1 << port);
                outpCard.exp1[slot].reg3 |= (1 << port);
                outpCard.exp1[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp1[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp1[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp1[slot].reg4, FPGA_WRITE);
            } else if (type == OUTPUT_1PPH) {

                /* update to shadow ram */
                outpCard.exp1[slot].reg2 &= ~(1 << port);
                outpCard.exp1[slot].reg3 &= ~(1 << port);
                outpCard.exp1[slot].reg4 |= (1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp1[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp1[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp1[slot].reg4, FPGA_WRITE);
            }
        } else if (outpCard.exp1[slot].cardType == OUTPUT_IRIGB) {
        } else if (outpCard.exp1[slot].cardType == OUTPUT_TIU) {

            /* update to shadow ram */
            outpCard.exp1[slot].reg0 |= framing;

            /* update to FPGA */
            expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                          &outpCard.exp1[slot].reg0, FPGA_WRITE);
        } else if (outpCard.exp1[slot].cardType == OUTPUT_5_10MHZ) {
        }
        break;
    case EXP2_SHELF:
        if (outpCard.exp2[slot].cardType == OUTPUT_FREQUENCY) {
            if (type == OUTPUT_T1) {

                /* update to shadow ram */
                outpCard.exp2[slot].reg2 |= (1 << port);
                outpCard.exp2[slot].reg3 &= ~(1 << port);
                outpCard.exp2[slot].reg4 &= ~(1 << port);
                outpCard.exp2[slot].reg5 &= ~(1 << port);
                outpCard.exp2[slot].reg0 |= framing;

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                              &outpCard.exp2[slot].reg0, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp2[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp2[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp2[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp2[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_E1) {

                /* update to shadow ram */
                outpCard.exp2[slot].reg2 &= ~(1 << port);
                outpCard.exp2[slot].reg3 |= (1 << port);
                outpCard.exp2[slot].reg4 &= ~(1 << port);
                outpCard.exp2[slot].reg5 &= ~(1 << port);
                outpCard.exp2[slot].reg0 |= framing;

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                              &outpCard.exp2[slot].reg0, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp2[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp2[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp2[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp2[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_SQW) {

                /* update to shadow ram */
                outpCard.exp2[slot].reg2 &= ~(1 << port);
                outpCard.exp2[slot].reg3 &= ~(1 << port);
                outpCard.exp2[slot].reg4 |= (1 << port);
                outpCard.exp2[slot].reg5 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp2[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp2[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp2[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp2[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_CC) {

                /* update to shadow ram */
                outpCard.exp2[slot].reg2 &= ~(1 << port);
                outpCard.exp2[slot].reg3 &= ~(1 << port);
                outpCard.exp2[slot].reg4 &= ~(1 << port);
                outpCard.exp2[slot].reg5 |= (1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp2[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp2[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp2[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp2[slot].reg5, FPGA_WRITE);
            }
        } else if (outpCard.exp2[slot].cardType == OUTPUT_TIMING) {
            if (type == OUTPUT_1PPS) {

                /* update to shadow ram */
                outpCard.exp2[slot].reg2 |= (1 << port);
                outpCard.exp2[slot].reg3 &= ~(1 << port);
                outpCard.exp2[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp2[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp2[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp2[slot].reg4, FPGA_WRITE);
            } else if (type == OUTPUT_1PPM) {

                /* update to shadow ram */
                outpCard.exp2[slot].reg2 &= ~(1 << port);
                outpCard.exp2[slot].reg3 |= (1 << port);
                outpCard.exp2[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp2[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp2[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp2[slot].reg4, FPGA_WRITE);
            } else if (type == OUTPUT_1PPH) {

                /* update to shadow ram */
                outpCard.exp2[slot].reg2 &= ~(1 << port);
                outpCard.exp2[slot].reg3 &= ~(1 << port);
                outpCard.exp2[slot].reg4 |= (1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp2[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp2[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp2[slot].reg4, FPGA_WRITE);
            }
        } else if (outpCard.exp2[slot].cardType == OUTPUT_IRIGB) {
        } else if (outpCard.exp2[slot].cardType == OUTPUT_TIU) {

            /* update to shadow ram */
            outpCard.exp2[slot].reg0 |= framing;

            /* update to FPGA */
            expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                          &outpCard.exp2[slot].reg0, FPGA_WRITE);
        } else if (outpCard.exp2[slot].cardType == OUTPUT_5_10MHZ) {
        }
        break;
    case EXP3_SHELF:
        if (outpCard.exp3[slot].cardType == OUTPUT_FREQUENCY) {
            if (type == OUTPUT_T1) {

                /* update to shadow ram */
                outpCard.exp3[slot].reg2 |= (1 << port);
                outpCard.exp3[slot].reg3 &= ~(1 << port);
                outpCard.exp3[slot].reg4 &= ~(1 << port);
                outpCard.exp3[slot].reg5 &= ~(1 << port);
                outpCard.exp3[slot].reg0 |= framing;

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                              &outpCard.exp3[slot].reg0, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp3[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp3[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp3[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp3[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_E1) {

                /* update to shadow ram */
                outpCard.exp3[slot].reg2 &= ~(1 << port);
                outpCard.exp3[slot].reg3 |= (1 << port);
                outpCard.exp3[slot].reg4 &= ~(1 << port);
                outpCard.exp3[slot].reg5 &= ~(1 << port);
                outpCard.exp3[slot].reg0 |= framing;

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                              &outpCard.exp3[slot].reg0, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp3[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp3[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp3[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp3[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_SQW) {

                /* update to shadow ram */
                outpCard.exp3[slot].reg2 &= ~(1 << port);
                outpCard.exp3[slot].reg3 &= ~(1 << port);
                outpCard.exp3[slot].reg4 |= (1 << port);
                outpCard.exp3[slot].reg5 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp3[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp3[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp3[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp3[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_CC) {

                /* update to shadow ram */
                outpCard.exp3[slot].reg2 &= ~(1 << port);
                outpCard.exp3[slot].reg3 &= ~(1 << port);
                outpCard.exp3[slot].reg4 &= ~(1 << port);
                outpCard.exp3[slot].reg5 |= (1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp3[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp3[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp3[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp3[slot].reg5, FPGA_WRITE);
            }
        } else if (outpCard.exp3[slot].cardType == OUTPUT_TIMING) {
            if (type == OUTPUT_1PPS) {

                /* update to shadow ram */
                outpCard.exp3[slot].reg2 |= (1 << port);
                outpCard.exp3[slot].reg3 &= ~(1 << port);
                outpCard.exp3[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp3[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp3[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp3[slot].reg4, FPGA_WRITE);
            } else if (type == OUTPUT_1PPM) {

                /* update to shadow ram */
                outpCard.exp3[slot].reg2 &= ~(1 << port);
                outpCard.exp3[slot].reg3 |= (1 << port);
                outpCard.exp3[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp3[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp3[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp3[slot].reg4, FPGA_WRITE);
            } else if (type == OUTPUT_1PPH) {

                /* update to shadow ram */
                outpCard.exp3[slot].reg2 &= ~(1 << port);
                outpCard.exp3[slot].reg3 &= ~(1 << port);
                outpCard.exp3[slot].reg4 |= (1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp3[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp3[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp3[slot].reg4, FPGA_WRITE);
            }
        } else if (outpCard.exp3[slot].cardType == OUTPUT_IRIGB) {
        } else if (outpCard.exp3[slot].cardType == OUTPUT_TIU) {

            /* update to shadow ram */
            outpCard.exp3[slot].reg0 |= framing;

            /* update to FPGA */
            expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                          &outpCard.exp3[slot].reg0, FPGA_WRITE);
        } else if (outpCard.exp3[slot].cardType == OUTPUT_5_10MHZ) {
        }
        break;
    case EXP4_SHELF:
        if (outpCard.exp4[slot].cardType == OUTPUT_FREQUENCY) {
            if (type == OUTPUT_T1) {

                /* update to shadow ram */
                outpCard.exp4[slot].reg2 |= (1 << port);
                outpCard.exp4[slot].reg3 &= ~(1 << port);
                outpCard.exp4[slot].reg4 &= ~(1 << port);
                outpCard.exp4[slot].reg5 &= ~(1 << port);
                outpCard.exp4[slot].reg0 |= framing;

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                              &outpCard.exp4[slot].reg0, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp4[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp4[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp4[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp4[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_E1) {

                /* update to shadow ram */
                outpCard.exp4[slot].reg2 &= ~(1 << port);
                outpCard.exp4[slot].reg3 |= (1 << port);
                outpCard.exp4[slot].reg4 &= ~(1 << port);
                outpCard.exp4[slot].reg5 &= ~(1 << port);
                outpCard.exp4[slot].reg0 |= framing;

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                              &outpCard.exp4[slot].reg0, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp4[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp4[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp4[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp4[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_SQW) {

                /* update to shadow ram */
                outpCard.exp4[slot].reg2 &= ~(1 << port);
                outpCard.exp4[slot].reg3 &= ~(1 << port);
                outpCard.exp4[slot].reg4 |= (1 << port);
                outpCard.exp4[slot].reg5 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp4[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp4[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp4[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp4[slot].reg5, FPGA_WRITE);
            } else if (type == OUTPUT_CC) {

                /* update to shadow ram */
                outpCard.exp4[slot].reg2 &= ~(1 << port);
                outpCard.exp4[slot].reg3 &= ~(1 << port);
                outpCard.exp4[slot].reg4 &= ~(1 << port);
                outpCard.exp4[slot].reg5 |= (1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp4[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp4[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp4[slot].reg4, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x05,
                              &outpCard.exp4[slot].reg5, FPGA_WRITE);
            }
        } else if (outpCard.exp4[slot].cardType == OUTPUT_TIMING) {
            if (type == OUTPUT_1PPS) {

                /* update to shadow ram */
                outpCard.exp4[slot].reg2 |= (1 << port);
                outpCard.exp4[slot].reg3 &= ~(1 << port);
                outpCard.exp4[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp4[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp4[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp4[slot].reg4, FPGA_WRITE);
            } else if (type == OUTPUT_1PPM) {

                /* update to shadow ram */
                outpCard.exp4[slot].reg2 &= ~(1 << port);
                outpCard.exp4[slot].reg3 |= (1 << port);
                outpCard.exp4[slot].reg4 &= ~(1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp4[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp4[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp4[slot].reg4, FPGA_WRITE);
            } else if (type == OUTPUT_1PPH) {

                /* update to shadow ram */
                outpCard.exp4[slot].reg2 &= ~(1 << port);
                outpCard.exp4[slot].reg3 &= ~(1 << port);
                outpCard.exp4[slot].reg4 |= (1 << port);

                /* update to FPGA */
                expFPGAAccess(shelf_index, (0x10 + slot), 0x02,
                              &outpCard.exp4[slot].reg2, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03,
                              &outpCard.exp4[slot].reg3, FPGA_WRITE);
                expFPGAAccess(shelf_index, (0x10 + slot), 0x04,
                              &outpCard.exp4[slot].reg4, FPGA_WRITE);
            }
        } else if (outpCard.exp4[slot].cardType == OUTPUT_IRIGB) {
        } else if (outpCard.exp4[slot].cardType == OUTPUT_TIU) {

            /* update to shadow ram */
            outpCard.exp4[slot].reg0 |= framing;

            /* update to FPGA */
            expFPGAAccess(shelf_index, (0x10 + slot), 0x00,
                          &outpCard.exp4[slot].reg0, FPGA_WRITE);
        } else if (outpCard.exp4[slot].cardType == OUTPUT_5_10MHZ) {
        }
        break;
    default:
        break;
    }
}


/**************************************************************************************
* function: cardTypeGet
*  description: this routine get the card info from the first few bytes in card
*   and update the global variable 'outpCard'
*  input
*	SHELF_t shelf_index
*   U8_t slot	
*			
*  return 
*     S8_t the card type number
***************************************************************************************/
S8_t cardTypeGet(SHELF_t shelf_index, U8_t slot)
{
    S8_t ctype;
    switch (shelf_index) {
    case MAIN_SHELF:
        if (  (slot > 7) ) {
            printf("cardTypeGet-MAIN_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        }



        
        ctype = *(U8_t *) OUTPUT_FPGA_R_REG_1(slot);
        outpCard.main[slot].list = *(U8_t *) OUTPUT_FPGA_R_REG_2(slot);
        outpCard.main[slot].issue = *(U8_t *) OUTPUT_FPGA_R_REG_3(slot);
        break;
    case EXP1_SHELF:
        if ( (slot > 15)) {
            printf("cardTypeGet-EXP1_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        }
        expFPGAAccess(shelf_index, (0x10 + slot), 0x00, &ctype, FPGA_READ);
        expFPGAAccess(shelf_index, (0x10 + slot), 0x01,
                      &outpCard.exp1[slot].list, FPGA_READ);
        expFPGAAccess(shelf_index, (0x10 + slot), 0x01,
                      &outpCard.exp1[slot].issue, FPGA_READ);
        break;
    case EXP2_SHELF:
        if ( (slot > 15)) {
            printf("cardTypeGet-EXP2_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        }
        expFPGAAccess(shelf_index, (0x10 + slot), 0x00, &ctype, FPGA_READ);
        expFPGAAccess(shelf_index, (0x10 + slot), 0x01,
                      &outpCard.exp1[slot].list, FPGA_READ);
        expFPGAAccess(shelf_index, (0x10 + slot), 0x01,
                      &outpCard.exp1[slot].issue, FPGA_READ);
        break;
    case EXP3_SHELF:
        if (  (slot > 15)) {
            printf("cardTypeGet-EXP3_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        }
        expFPGAAccess(shelf_index, (0x10 + slot), 0x00, &ctype, FPGA_READ);
        expFPGAAccess(shelf_index, (0x10 + slot), 0x01,
                      &outpCard.exp1[slot].list, FPGA_READ);
        expFPGAAccess(shelf_index, (0x10 + slot), 0x01,
                      &outpCard.exp1[slot].issue, FPGA_READ);
        break;
    case EXP4_SHELF:
        if ( (slot > 15)) {
            printf("cardTypeGet-EXP4_SHELF: Invalid Shelf/Slot: %d/%d\n",
                   shelf_index, slot);
            return ERROR;
        }
        expFPGAAccess(shelf_index, (0x10 + slot), 0x00, &ctype, FPGA_READ);
        expFPGAAccess(shelf_index, (0x10 + slot), 0x01,
                      &outpCard.exp1[slot].list, FPGA_READ);
        expFPGAAccess(shelf_index, (0x10 + slot), 0x01,
                      &outpCard.exp1[slot].issue, FPGA_READ);
        break;
    default:
        printf("cardTypeGet: Invalid Shelf: %d\n", shelf_index);
        ctype = OUTPUT_NULL;
        break;
    }
    return (ctype);
}


/**************************************************************************************
* function:  outpAlarmCheck(SHELF_t shelf_index)
*  input 
*			
*			
*  return 
*     
***************************************************************************************/
void outpAlarmCheck(SHELF_t shelf_index)
{
    U8_t slot, port;
    U8_t alarms;
    U32_t *dumPtr = NULL;
    stsCardEntry_t snmpTrapCardReply;
    if (shelf_index == MAIN_SHELF) {
        for (slot = 0; slot < 8; slot++) {
            if (outpCard.main[slot].status == OUTPUT_PRESENT) {
                alarms = *(U8_t *) OUTPUT_FPGA_R_REG_4(slot);
                if (alarms) {
                    for (port = 0; port < 8; port++) {
                        if (alarms & (1 << port)){
                            /* sendAlarm(output alarm); */
                            SiwEventLog(NULL,  SIW_EVENT_SYM(SM, OUTPALARM), port, slot, shelf_index);
                            
                            /* checkitout
                                TrapEventLog(smOupAlarmEvent_TRAP, shelf_index,  slot, port, 0, dumPtr, SIW_EVENT_SYM(SM, OUTPALARM), port, slot,shelf_index);
							*/

							#ifdef SM_DEBUG
							    printf("in sm, alarm in port [%d], card [%d], shelf [%d]\n", port, slot, shelf_index);
							#endif
						}
	
      
                    }
                    if (outpCard.main[slot].redundancy_enable ==  REDUNDANCY_ENABLE)
                        redundancySwitch(MAIN_SHELF, slot);
                }
            }
        }
    } else if (shelf_index == EXP1_SHELF) {
        for (slot = 0; slot < 16; slot++) {
            if (outpCard.exp1[slot].status == OUTPUT_PRESENT) {
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03, &alarms,
                              FPGA_READ);
                if (alarms) {
                    for (port = 0; port < 8; port++) {
                        if (alarms & (1 << port)){
                        	
                            /* sendAlarm(output alarm); */
							SiwEventLog(NULL,  SIW_EVENT_SYM(SM, OUTPALARM), port, slot, shelf_index);
														
							/* checkitout
								TrapEventLog(smOupAlarmEvent_TRAP, shelf_index,  slot, port, 0, dumPtr, SIW_EVENT_SYM(SM, OUTPALARM), port, slot,shelf_index);
							*/


							#ifdef SM_DEBUG
								printf("in sm, alarm in port [%d], card [%d], shelf [%d]\n", port, slot, shelf_index);
							#endif
													}
                    }
                    if (outpCard.exp1[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE)
                        redundancySwitch(EXP1_SHELF, slot);
                }
            }
        }
    } else if (shelf_index == EXP2_SHELF) {
        for (slot = 0; slot < 16; slot++) {
            if (outpCard.exp2[slot].status == OUTPUT_PRESENT) {
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03, &alarms,
                              FPGA_READ);
                if (alarms) {
                    for (port = 0; port < 8; port++) {
                        if (alarms & (1 << port)){
                            /* sendAlarm(output alarm); */
							SiwEventLog(NULL,  SIW_EVENT_SYM(SM, OUTPALARM), port, slot, shelf_index);
														
							/* checkitout
								TrapEventLog(smOupAlarmEvent_TRAP, shelf_index,  slot, port, 0, dumPtr, SIW_EVENT_SYM(SM, OUTPALARM), port, slot,shelf_index);
							*/


							#ifdef SM_DEBUG
								printf("in sm, alarm in port [%d], card [%d], shelf [%d]\n", port, slot, shelf_index);
							#endif
													}
                    }
                    if (outpCard.exp2[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE)
                        redundancySwitch(EXP2_SHELF, slot);
                }
            }
        }
    } else if (shelf_index == EXP3_SHELF) {
        for (slot = 0; slot < 16; slot++) {
            if (outpCard.exp3[slot].status == OUTPUT_PRESENT) {
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03, &alarms,
                              FPGA_READ);
                if (alarms) {
                    for (port = 0; port < 8; port++) {
                        if (alarms & (1 << port)){
                            /* sendAlarm(output alarm); */
							SiwEventLog(NULL,  SIW_EVENT_SYM(SM, OUTPALARM), port, slot, shelf_index);
									
							/* checkitout
								TrapEventLog(smOupAlarmEvent_TRAP, shelf_index,  slot, port, 0, dumPtr, SIW_EVENT_SYM(SM, OUTPALARM), port, slot,shelf_index);
							*/


									#ifdef SM_DEBUG
					printf("in sm, alarm in port [%d], card [%d], shelf [%d]\n", port, slot, shelf_index);
									#endif
                        	
                        	}

                    }
                    if (outpCard.exp3[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE)
                        redundancySwitch(EXP3_SHELF, slot);
                }
            }
        }
    } else if (shelf_index == EXP4_SHELF) {
        for (slot = 0; slot < 16; slot++) {
            if (outpCard.exp4[slot].status == OUTPUT_PRESENT) {
                expFPGAAccess(shelf_index, (0x10 + slot), 0x03, &alarms,
                              FPGA_READ);
                if (alarms) {
                    for (port = 0; port < 8; port++) {
                        if (alarms & (1 << port)){
                        	/* sendAlarm(output alarm); */
				            SiwEventLog(NULL,  SIW_EVENT_SYM(SM, OUTPALARM), port, slot, shelf_index);
				            /* checkitout
				                TrapEventLog(smOupAlarmEvent_TRAP, shelf_index,  slot, port, 0, dumPtr, SIW_EVENT_SYM(SM, OUTPALARM), port, slot,shelf_index);
							*/


									#ifdef SM_DEBUG
					printf("in sm, alarm in port [%d], card [%d], shelf [%d]\n", port, slot, shelf_index);
									#endif
                        	
                        	}
                            								
                    }
                    if (outpCard.exp4[slot].redundancy_enable ==
                        REDUNDANCY_ENABLE)
                        redundancySwitch(EXP4_SHELF, slot);
                }
            }
        }
    }
}
/**************************************************************************************
* function:  ABSlipCheck
*  input 
*			
*			
*  return 
*     
***************************************************************************************/
void ABSlipCheck()
{
    static U8_t abSlipOld;
    static STATUS_t abSlipAlarm = OFF;
    static STATUS_t first = TRUE;  
    
    U32_t *dumPtr = NULL;
    stsCardEntry_t snmpTrapCardReply;
    U8_t abSlipCurrent;
    abSlipCurrent = (*(U8_t *) MAIN_FPGA_R_REG_17 & 0x03); /*checkitout*/
    if (first){
        /* for the first time*/
        abSlipOld = abSlipCurrent;
        first = FALSE;
        

    }else {
        if (abSlipCurrent != abSlipOld) {
            abSlipOld = abSlipCurrent;
            if (!abSlipAlarm) {
                abSlipAlarm = ON;
                /* sendAlarm(AB Slip Alarm On); */
				SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ABSLIPON));
				
				/* checkitout
                    TrapEventLog(smABSlipOnEvent_TRAP, 0,  0, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ABSLIPON));
				*/
				
				#ifdef SM_DEBUG
					printf("in sm, A/B Slip ON");
				#endif	
                
                
                
            }
        } else {
            if (abSlipAlarm) {
                abSlipAlarm = OFF;
			    /* sendAlarm(AB Slip Alarm Off); */
			    SiwEventLog(NULL,  SIW_EVENT_SYM(SM, ABSLIPOFF));
			    
			    /* checkitout
						TrapEventLog(smABSlipOffEvent_TRAP, 0,  0, 0, 0, dumPtr, SIW_EVENT_SYM(SM, ABSLIPOFF));
				*/
					
				#ifdef SM_DEBUG
					printf("in sm, A/B Slip Off");
				#endif	
            }
        }
    }
}








