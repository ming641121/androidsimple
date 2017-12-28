/*
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: tiu_ds26502.h
 *
 *tiu framer maxim ds26502 code
 *
 *Note:
 *
 *Author(s):
 * shuming
 */
 /********************************************************************
 * $Id: tiu_ds26502.c,v 1.17 2006/10/03 05:41:39 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/driver/framer/tiu/tiu_ds26502.c,v $
 *********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <vxWorks.h>
#include <sys_memmap.h>
#include <semLib.h>
#include <tss_types.h>
#include <taskLib.h>
#include <intLib.h>
#include <iv.h>
#include <semLib.h>
#include <sm.h>
#include <ds26502.h>
#include <tiu_if.h>
#include "tiu_event.h"

/*global variable declaration */
TIU_FRAMER_t tiuFramer[TIU_MAX_SHELF][TIU_MAX_SLOT][TIU_FRAMERS];
TIU_STATUS_t tiuStatus[TIU_MAX_SHELF][TIU_MAX_SLOT][TIU_FRAMERS];


/* Semaphore to control tiu from sm */
SEM_ID TiuTaskSemaphore;

/* func declaration */
void tiuTask();
void tiuFramerInit();

STATUS_t tiuAdd(int shelf, int slot);
STATUS_t tiuRemove(int shelf, int slot);

STATUS_t tiuCheck(int shelf, int slot, int framer);
STATUS_t tiuFramerConfig(int shelf, int slot, int framer);
static STATUS_t tiuFramerLineModeConfig(int shelf,
                                        DS26502_t * tiuFramerReg_p,
                                        TE1_CONFIG_t * lineConfig_p);


static U8_t tiuReadStatusReg(int shelf, DS26502_t * tiuFramerReg_p,
                             WR_REG_INDEX_t reg, U8_t bits);


static void tiuFPGAAcess(int shelf, void *addr, U8_t * data, U8_t command);
static STATUS_t tiuCfgAccess(char *fname, int mode);
extern void expFPGAAccess(SHELF_t shelf_index, U8_t addrHigh, U8_t addrLow,
                          U8_t * data, U8_t command);

STATUS_t snmpTiuGet(stsOutputTiuSettingEntry_t * mibstructIn);
STATUS_t snmpTiuSet(stsOutputTiuSettingEntry_t * mibstructIn);

#ifdef TIU_DEBUG
STATUS_t tiuDebugRemoteLoopSet(int shelf, int slot, int framer, int enable)
STATUS_t tiuDebugModeSetAll(int testcase);
STATUS_t tiuDebugModeSet(int shelf, int slot, int framer, int testcase);
static void tiuDebugModeHelp();

#endif                          

/**************************************************************************************
* function: void tiuTask()
*  input 
*			
*			
* return
* 	void
***************************************************************************************/
void tiuTask()
{
    /* initialize TIU */
    int shelf, slot, framer;
    TIU_FRAMER_t *fptr = NULL;
    for(;;){
    	semTake(TiuTaskSemaphore, WAIT_FOREVER);
    	for (shelf = 0; shelf < TIU_MAX_SHELF; shelf++) {
    		for (slot = 0; slot < TIU_MAX_SLOT; slot++) {
    			for (framer = 0; framer < TIU_FRAMERS; framer++) {
    				if (fptr->status == TRUE) {
    					fptr = &tiuFramer[shelf][slot][framer];
    					tiuCheck(shelf, slot, framer);
    				}
           }
         }
       }
    	
    
    
			semGive(TiuTaskSemaphore);
			taskDelay(POLL_TIU_INTERVAL);
		
	}  
}



/**************************************************************************************
* function: void tiuFramerInit()
*  input 
*			
*			
* return
* 	void
***************************************************************************************/
void tiuFramerInit()
{
    int shelf, slot, framer;
    FILE *fhd = NULL;
    TIU_FRAMER_t *fptr = NULL;

    /* Retrieve Framer configuration from Flash */
    if (tiuCfgAccess(TIU_CFG_FILENAME, TIU_FLASH_READ) != OK) {
	    for (shelf = 0; shelf < TIU_MAX_SHELF; shelf++) {
            for (slot = 0; slot < TIU_MAX_SLOT; slot++) {
                for (framer = 0; framer < TIU_FRAMERS; framer++) {
                    fptr = &tiuFramer[shelf][slot][framer];
                    fptr->lnCfg.lineType = LT_T1;
                    fptr->lnCfg.framing = FF_T1_ESF;
                    fptr->lnCfg.coding = LC_B8ZS;
                    fptr->lnCfg.termination = LTM_100OHM;
                    fptr->status = FALSE;
                }
            }
        }

        tiuCfgAccess(TIU_CFG_FILENAME, TIU_FLASH_WRITE);
    }


    memset(&tiuStatus[0][0][0], 0,
           sizeof(TIU_STATUS_t) * TIU_MAX_SHELF * TIU_MAX_SLOT *
           TIU_FRAMERS);

    for (shelf = 0; shelf < TIU_MAX_SHELF; shelf++) {
        for (slot = 0; slot < TIU_MAX_SLOT; slot++) {
            for (framer = 0; framer < TIU_FRAMERS; framer++) {
                fptr = &tiuFramer[shelf][slot][framer];

                if (fptr->status == TRUE) {
                    tiuFramerConfig(shelf, slot, framer);
                }
            }
        }
    }
}

/**************************************************************************************
* function: STATUS_t tiuCheck
*  input 
*			int shelf 
*			int slot
*			int framer
*			
* return
*  	OK:  
*  	ERROR:  
***************************************************************************************/
STATUS_t tiuCheck(int shelf, int slot, int framer)
{
    TIU_STATUS_t *sptr = &tiuStatus[shelf][slot][framer];
    
    
    U8_t alarms, tmp;
    U32_t *dumPtr = NULL;
    U8_t tmpdata;
    U8_t statusChange_yellowAlarm=0, statusChange_TAIS=0;
    
		DS26502_t *fReg = (DS26502_t *) (TIU_FRAMER_BASE_ADDR + (framer * 0x100) +
                       (slot * 0x1000) + 0x200);
                       
    /* Read Alarms from SR1 : Rx status*/
    	alarms = tiuReadStatusReg(shelf, fReg, SR1_REG, 0x0F);  
	  	if (alarms & 0xFF){
	  		 /* check open circuit condition*/
	  		 if (alarms & SR1_TOCD){	  		 	
	  		 	
	  		 	}
	  		 /* check current limit exceeded condition */
	  		 if (alarms & SR1_TCLE){}
	  		 /* check jitter attenuator limit trip*/
	  		 if (alarms & SR1_JALT){}
	  	}else{}                     
    
    /* Read Alarms from SR2 */
	    alarms = tiuReadStatusReg(shelf, fReg, SR2_REG, 0x0F);
	    if (alarms & 0x0F) {
	    	/* check LOS */
	    	if (alarms & SR2_RLOS) {
	          if (sptr->losAlarm != ALARM_ON) {
	              sptr->losAlarm = ALARM_ON;
	              statusChange_TAIS++;
	              
	              /* sendAlarm(T1_COND_LOS,SERVICE); */
	            }
	        }
	        
	
	      /* check LOF */
	        if (alarms & SR2_RLOF) {
	        	if (sptr->lofAlarm != ALARM_ON) {
	        		sptr->lofAlarm = ALARM_ON;
	        		statusChange_TAIS++;
	        		/* sendAlarm( T1_COND_LOF,SERVICE ); */
	        		}
	        	}
	
	      /* check RAIS */
	        if (alarms & SR2_RAIS) {
	        	if (sptr->raisAlarm != ALARM_ON) {
	          	sptr->raisAlarm = ALARM_ON;
	          	statusChange_TAIS++;
	          	/* sendAlarm( T1_COND_RAIS,SERVICE ); */
	              SiwEventLog(NULL, SIW_EVENT_SYM(TIU, RAIS), shelf, slot, framer);
	            	}
	         	}
	         	
	      /* check YELLOW */
	       	if (alarms & SR2_RYEL) {
	       		if (sptr->yelAlarm != ALARM_ON) {
	       			sptr->yelAlarm = ALARM_ON;
	       			statusChange_yellowAlarm++;
	       			/* sendAlarm( T1_COND_YELLOW,SERVICE ); */
	         			}
	            }
	        
	
	      /* Integrate LOS and LOF into Red Alarm */
	      	if (sptr->losAlarm || sptr->lofAlarm) {
	          if (sptr->redTimer > 8) {    /*  250ms * 10 = 2.5sec  */
	          	if (sptr->redAlarm != ALARM_ON) {
	            	sptr->redAlarm = ALARM_ON;
	                    
	              /* sendAlarm(RED_ALARM); */
	
	             }
	           	}else{
	           		sptr->redTimer++;
	           	}
	
	        	}

	      /* Transmit Alarm Indication Signal (TAIS) when LOS/LOF received*/
	      	if (statusChange_TAIS != 0) {
	      		if (shelf ==0 ){
	      			fReg->lic2|=LIC2_TAIS;
	      		}else{
	      			tiuFPGAAcess(shelf, (void *)&(fReg->iocr2), &tmpdata, FPGA_READ);
	      			tmpdata = tmpdata | (LIC2_TAIS);
	      			tiuFPGAAcess(shelf, (void *)&(fReg->iocr2), &tmpdata, FPGA_WRITE);	
	      		}
							
	
	        }
	        	
	      /* Transmit Yellow Alarm (TYEL) */
	      	if (statusChange_yellowAlarm != 0) {
	      		if (shelf ==0 ){
	      			fReg->t1tcr1|=T1TCR1_TYEL;
	      		}else{
	      			tiuFPGAAcess(shelf, (void *)&(fReg->t1tcr1), &tmpdata, FPGA_READ);
	      			tmpdata = tmpdata | (T1TCR1_TYEL);
	      			tiuFPGAAcess(shelf, (void *)&(fReg->t1tcr1), &tmpdata, FPGA_WRITE);	
	      		}
	
	        }

        /* TIU Framer is in alarm */
        sptr->alarmFlag = TRUE;
        if (sptr->redAlarm || sptr->raisAlarm || sptr->yelAlarm) {

            /* Framer is out of service */
            if (sptr->status != TIU_OUT_OF_SERVICE) {

            }
            sptr->status = TIU_OUT_OF_SERVICE;


            return ERROR;
        }

    } else {
        /* no any alarm case */

        if (sptr->redAlarm == ALARM_ON) {


            if (sptr->redTimer > 0)
                sptr->redTimer--;

            else {
                sptr->redAlarm = ALARM_OFF;
                sptr->redTimer = 0;

                /*  sendAlarm(); */
                SiwEventLog(NULL, SIW_EVENT_SYM(TIU, RED_CLEAR), shelf, slot, framer);



            }
        }
        if (sptr->losAlarm == ALARM_ON) {
            sptr->losAlarm = ALARM_OFF;
            statusChange_TAIS--;
            

            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(TIU, LOS_CLEAR),shelf, slot, framer);




        }
        if (sptr->lofAlarm == ALARM_ON) {
            sptr->lofAlarm = ALARM_OFF;
            statusChange_TAIS--;



            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(TIU, LOF_CLEAR), shelf, slot, framer);

        }
        if (sptr->raisAlarm == ALARM_ON) {
            sptr->raisAlarm = ALARM_OFF;
            statusChange_TAIS--;




            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(TIU, RAIS_CLEAR), shelf, slot, framer);


        }
        if (sptr->yelAlarm == ALARM_ON) {

            sptr->yelAlarm = ALARM_OFF;
            statusChange_yellowAlarm--;


            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(TIU, RYEL_CLEAR),shelf, slot, framer);



        }





	      /* Transmit Alarm Indication Signal (TAIS) when LOS/LOF received*/
	      	if (statusChange_TAIS != 0) {
	      		if (shelf ==0 ){
	      			fReg->lic2&=~(LIC2_TAIS);
	      		}else{
	      			tiuFPGAAcess(shelf, (void *)&(fReg->iocr2), &tmpdata, FPGA_READ);
	      			tmpdata = tmpdata & ~(LIC2_TAIS);
	      			tiuFPGAAcess(shelf, (void *)&(fReg->iocr2), &tmpdata, FPGA_WRITE);	
	      		}
							
	
	        }
	        	
	      /* Transmit Yellow Alarm (TYEL) */
	      	if (statusChange_yellowAlarm != 0) {
	      		if (shelf ==0 ){
	      			fReg->t1tcr1&=~(T1TCR1_TYEL);
	      		}else{
	      			tiuFPGAAcess(shelf, (void *)&(fReg->t1tcr1), &tmpdata, FPGA_READ);
	      			tmpdata = tmpdata & ~(T1TCR1_TYEL);
	      			tiuFPGAAcess(shelf, (void *)&(fReg->t1tcr1), &tmpdata, FPGA_WRITE);	
	      		}
	
	        }



        /* Framer is not in alarm */
        sptr->alarmFlag = FALSE;
        if (!sptr->redAlarm && !sptr->raisAlarm && !sptr->yelAlarm) {

            /* Framer is back to service */
            if (sptr->status != TIU_IN_SERVICE) {

 
            }
            sptr->status = TIU_IN_SERVICE;







        }
    }

    
    
}





/**************************************************************************************
* function: STATUS_t tiuFramerConfig
*  input 
*			int shelf 
*			int slot
*			int framer
*			
* return
*  	OK:  
*  	ERROR:  
***************************************************************************************/
STATUS_t tiuFramerConfig(int shelf, int slot, int framer)
{
    TIU_FRAMER_t *fptr = &tiuFramer[shelf][slot][framer];
    TIU_STATUS_t *sptr = &tiuStatus[shelf][slot][framer];
    DS26502_t *fReg = NULL;
    int i, n;

    fReg =
        (DS26502_t *) (TIU_FRAMER_BASE_ADDR + (framer * 0x100) +
                       (slot * 0x1000) + 0x200);
    if (shelf == 0) {
        /* Soft reset Framer */
        fReg->tstrreg = 0;
        fReg->tstrreg = TSTRREG_SFTRST;

        /* wait for reset bit to clear */
        for (i = 0; i < MAX_RESET_WAIT; i++)
            if (!(fReg->tstrreg & TSTRREG_SFTRST))
                break;

        if (i >= MAX_RESET_WAIT) {
            printf("Failed to initialize Framer %d\n", framer);
            sptr->status = FRAMER_OUT_OF_SERVICE;

            return ERROR;
        }

        /* Reset the Line Interface */
        printf("  on framer init reset\n");
        fReg->lic2 = 0;
        fReg->lic2 = LIC2_LIRST;
        fReg->lic2 = 0;
        fReg->iocr1 = IOCR1_TSIO;       /* TS_8K_4 is an output */

        /* configure control Register */
        printf("  on framer init config control reg\n");
        fReg->t1rcr1 = 0;
        fReg->t1rcr1 = T1RCR1_SYNCT |
            T1RCR1_SYNCC | (T1RCR1_OOF(OOF_CRITERIA)) | T1RCR1_SYNCE;
        fReg->t1rcr2 = 0;
        fReg->bocc = BOCC_RBF(RX_BOC_THRESHOLD);

        /* Clear CRC4 and BV count */
        printf("  on framer init clear crc\n");
        fReg->e1rcr &= ~(1 << 3);

        /* Make sure all interrupts are off */
        printf("  on framer init mark out all interrupt\n");
        fReg->imr1 = 0;
        fReg->imr2 = 0;
        fReg->imr3 = 0;
        fReg->imr4 = 0;

        /* disable loopback */
        fReg->lbcr = 0;

        /* configure LIU */
        printf("  on framer init config LIU\n");
        fReg->lic1 = LIC1_TPD;
        fReg->lic4 = LIC4_MPS(MCLK_MODE);
        fReg->lic2 = LIC2_JACKS | LIC2_TAIS;

        /* setup line configuration */
        printf("  on framer init --> tiuFramerLineModeConfig\n");
        tiuFramerLineModeConfig(shelf, fReg, &(fptr->lnCfg));
        sptr->status = FRAMER_IN_SERVICE;

        /* Enable alarm interrupt */
        printf("  on framer init tiuAlarmIntInit\n");
    } else {
        U8_t tmpdata;
        /* Soft reset Framer */
        /* 
           fReg->tstrreg = 0;
           fReg->tstrreg = TSTRREG_SFTRST;

           hAddr                = (U8_t) (((int) (&fReg->tstrreg) & 0xFF00) >> 8);
           lAddr                = (U8_t) ((int) (&fReg->tstrreg) & 0xFF);
           tmpdata      =       0;
           expFPGAAccess(shelf, hAddr, lAddr, 0, FPGA_WRITE);
           hAddr        = (U8_t) (((int) (&fReg->tstrreg) & 0xFF00) >> 8);
           lAddr        = (U8_t) ((int) (&fReg->tstrreg) & 0xFF);
           tmpdata      =       TSTRREG_SFTRST;
           expFPGAAccess(shelf, hAddr, lAddr, TSTRREG_SFTRST, FPGA_WRITE);
         */
        tmpdata = 0;
        tiuFPGAAcess(shelf, (void *) (&(fReg->tstrreg)), &tmpdata,
                     FPGA_WRITE);

        tmpdata = TSTRREG_SFTRST;
        tiuFPGAAcess(shelf, (void *) &(fReg->tstrreg), &tmpdata,
                     FPGA_WRITE);


        /* wait for reset bit to clear */
        /*          for(i = 0; i < MAX_RESET_WAIT; i++)
           if(!(fReg->tstrreg & TSTRREG_SFTRST)) break;
         */
        for (i = 0; i < MAX_RESET_WAIT; i++) {

            tiuFPGAAcess(shelf, (void *) &(fReg->tstrreg), &tmpdata,
                         FPGA_READ);
            if (!(tmpdata & TSTRREG_SFTRST))
                break;
        }

        if (i >= MAX_RESET_WAIT) {
            printf
                ("Failed to initialize Framer [%d] in slot [%d] of shelf [%d]\n",
                 framer, slot, shelf);
            sptr->status = FRAMER_OUT_OF_SERVICE;

            return ERROR;
        }

        /* Reset the Line Interface */
        printf("  on framer init reset\n");
        /*
           fReg->lic2 = 0;
           fReg->lic2 = LIC2_LIRST;
           fReg->lic2 = 0;
           fReg->iocr1 = 1;  >>      TS_8K_4 is an output 
         */
        tmpdata = 0;
        tiuFPGAAcess(shelf, (void *) &(fReg->lic2), &tmpdata, FPGA_WRITE);
        tmpdata = LIC2_LIRST;
        tiuFPGAAcess(shelf, (void *) &(fReg->lic2), &tmpdata, FPGA_WRITE);
        tmpdata = 0;
        tiuFPGAAcess(shelf, (void *) &(fReg->lic2), &tmpdata, FPGA_WRITE);
        tmpdata = IOCR1_TSIO;
        tiuFPGAAcess(shelf, (void *) &(fReg->iocr1), &tmpdata, FPGA_WRITE);


        /* configure control Register */
        printf("  on framer init config control reg\n");
        /*
           fReg->t1rcr1 = 0;
           fReg->t1rcr1 = T1RCR1_SYNCT | T1RCR1_SYNCC | (T1RCR1_OOF(OOF_CRITERIA)) | T1RCR1_RESYNC;
           fReg->t1rcr2 = 0;
           fReg->bocc = BOCC_RBF(RX_BOC_THRESHOLD);
         */
        tmpdata = 0;
        tiuFPGAAcess(shelf, (void *) &(fReg->t1rcr1), &tmpdata,
                     FPGA_WRITE);

        tmpdata =
            T1RCR1_SYNCT | T1RCR1_SYNCC | (T1RCR1_OOF(OOF_CRITERIA)) |
            T1RCR1_RESYNC;
        tiuFPGAAcess(shelf, (void *) &(fReg->t1rcr1), &tmpdata,
                     FPGA_WRITE);

        tmpdata = 0;
        tiuFPGAAcess(shelf, (void *) &(fReg->t1rcr2), &tmpdata,
                     FPGA_WRITE);

        tmpdata = BOCC_RBF(RX_BOC_THRESHOLD);
        tiuFPGAAcess(shelf, (void *) &(fReg->bocc), &tmpdata, FPGA_WRITE);


        /* Clear CRC4 and BV count */
        printf("  on framer init clear crc\n");
        /*
           fReg->e1rcr &= ~(1 << 3);
         */

        tiuFPGAAcess(shelf, (void *) &(fReg->e1rcr), &tmpdata, FPGA_READ);
        tmpdata = tmpdata & (~(1 << 3));
        tiuFPGAAcess(shelf, (void *) &(fReg->e1rcr), &tmpdata, FPGA_WRITE);

        /* Make sure all interrupts are off */
        printf("  on framer init mark out all interrupt\n");
        /*
           fReg->imr1 = 0;
           fReg->imr2 = 0;
           fReg->imr3 = 0;
           fReg->imr4 = 0;
         */
        tmpdata = 0;
        tiuFPGAAcess(shelf, (void *) &(fReg->imr1), &tmpdata, FPGA_WRITE);
        tiuFPGAAcess(shelf, (void *) &(fReg->imr2), &tmpdata, FPGA_WRITE);
        tiuFPGAAcess(shelf, (void *) &(fReg->imr3), &tmpdata, FPGA_WRITE);
        tiuFPGAAcess(shelf, (void *) &(fReg->imr4), &tmpdata, FPGA_WRITE);

        /* disable loopback */
        /*
           fReg->lbcr = 0;
         */
        tmpdata = 0;
        tiuFPGAAcess(shelf, (void *) &(fReg->lbcr), &tmpdata, FPGA_WRITE);

        /* configure LIU */
        printf("  on framer init config LIU\n");
        /*
           fReg->lic1 = LIC1_TPD;
           fReg->lic4 = LIC4_MPS(MCLK_MODE);
           fReg->lic2 = LIC2_JACKS;
         */
        tmpdata = LIC1_TPD;
        tiuFPGAAcess(shelf, (void *) &(fReg->lic1), &tmpdata, FPGA_WRITE);
        tmpdata = LIC4_MPS(MCLK_MODE);
        tiuFPGAAcess(shelf, (void *) &(fReg->lic4), &tmpdata, FPGA_WRITE);
        tmpdata = LIC2_JACKS | LIC2_TAIS;
        tiuFPGAAcess(shelf, (void *) &(fReg->lic2), &tmpdata, FPGA_WRITE);



        /* setup line configuration */
        printf("  on framer init --> tiuFramerLineModeConfig\n");

        tiuFramerLineModeConfig(shelf, fReg, &(fptr->lnCfg));

        sptr->status = FRAMER_IN_SERVICE;

        /* Enable alarm interrupt */
        printf("  on framer init tiuAlarmIntInit\n");
    }
}

/**************************************************************************************
* function: void tiuAdd
*  input 
*			int shelf
*			int slot			
* return
*  	OK:  
*  	ERROR:  
***************************************************************************************/
STATUS_t tiuAdd(int shelf, int slot)
{
    int framer;
    TIU_FRAMER_t *fptr;

    for (framer = 0; framer < TIU_FRAMERS; framer++) {

        fptr = &tiuFramer[shelf][slot][framer];
        fptr->status == TRUE;
        tiuFramerConfig(shelf, slot, framer);
    }
    tiuCfgAccess(TIU_CFG_FILENAME, TIU_FLASH_WRITE);

}

/**************************************************************************************
* function: void tiuRemove()
*  input 
*			int shelf
*			int slot
* return
*  	OK:  
*  	ERROR:  
***************************************************************************************/
STATUS_t tiuRemove(int shelf, int slot)
{
    int framer;
    TIU_STATUS_t *sptr = NULL;
    TIU_FRAMER_t *fptr;

    for (framer = 0; framer < TIU_FRAMERS; framer++) {
        sptr = &tiuStatus[shelf][slot][framer];
        sptr->status = FRAMER_OUT_OF_SERVICE;
        fptr = &tiuFramer[shelf][slot][framer];
        fptr->status == FALSE;


    }
    tiuCfgAccess(TIU_CFG_FILENAME, TIU_FLASH_WRITE);

}

/**************************************************************************************
* function: void tiuFramerInit()
*  input 
*			
*			
* return
***************************************************************************************/
static STATUS_t tiuFramerLineModeConfig(int shelf,
                                        DS26502_t * tiuFramerReg_p,
                                        TE1_CONFIG_t * lineConfig_p)
{

    U8_t tmpdata;
    DS26502_t *fReg = tiuFramerReg_p;

    /*
       the lineType is adjusted to the rate input.
     */
    if (shelf == 0) {

        switch (lineConfig_p->lineType) {
        case LT_T1:
            /* 1.544 M */
            tiuFramerReg_p->tpcr =
                (TPCR_TPLLOFS(0) | TPCR_TPLLIFS(0) | TPCR_TCSS(0)) &
                (~(TPCR_PLLOS));
			tiuFramerReg_p->tpcr |= TPCR_TPLLSS;
            break;

        case LT_J1:
            /* 1.544 M */

            tiuFramerReg_p->tpcr =
                (TPCR_TPLLOFS(0) | TPCR_TPLLIFS(0) | TPCR_TCSS(1)) &
                (~(TPCR_PLLOS));
            tiuFramerReg_p->tpcr |= TPCR_TPLLSS;
            break;
#ifdef TIU_DEBUG
        case LT_E1:
            /* 2.048 M */
            tiuFramerReg_p->tpcr =
                (TPCR_TPLLOFS(1) | TPCR_TPLLIFS(0) | TPCR_TCSS(1)) &
                (~(TPCR_PLLOS));
            tiuFramerReg_p->tpcr |= TPCR_TPLLSS;

            break;




#endif
        default:
            printf("Undefinded LineType: %d\n", lineConfig_p->framing);
            return ERROR;

        }



        switch (lineConfig_p->framing) {
        case FF_T1_D4:
            tiuFramerReg_p->mcreg = MCREG_RMODE(0) | MCREG_TMODE(0);
			tiuFramerReg_p->iocr1 &= ~(IOCR1_RSMS1);
			tiuFramerReg_p->iocr2 |= LIC2_JACKS;
            tiuFramerReg_p->t1tcr2 |= T1TCR2_TFSE;
            break;

        case FF_T1_ESF:
		    tiuFramerReg_p->mcreg = MCREG_RMODE(1) | MCREG_TMODE(1);
			tiuFramerReg_p->iocr1 |= IOCR1_RSMS1;
			tiuFramerReg_p->iocr2 |= LIC2_JACKS;
			tiuFramerReg_p->t1tcr2 &= ~(T1TCR2_TFSE);
            break;

        case FF_J1_D4:
            tiuFramerReg_p->mcreg = MCREG_RMODE(2)| MCREG_TMODE(2);
            tiuFramerReg_p->iocr1 &= ~(IOCR1_RSMS1);
            tiuFramerReg_p->iocr2 |= LIC2_JACKS;
            tiuFramerReg_p->t1rcr2 |= T1RCR2_RD4YM;
            tiuFramerReg_p->t1tcr2 |= T1TCR2_TFSE;
            break;


        case FF_J1_ESF:
            tiuFramerReg_p->mcreg = MCREG_RMODE(3)| MCREG_TMODE(3);
            tiuFramerReg_p->iocr1 |= IOCR1_RSMS1;
            tiuFramerReg_p->iocr2 |= LIC2_JACKS;
            tiuFramerReg_p->t1rcr2 |= T1RCR2_RD4YM;
            tiuFramerReg_p->t1rcr2 |= T1RCR2_RJC;
            tiuFramerReg_p->t1tcr2 &= ~(T1TCR2_TFSE);
            break;

#ifdef TIU_DEBUG

        case FF_E1:
            tiuFramerReg_p->mcreg = MCREG_RMODE(4)| MCREG_TMODE(4);
            tiuFramerReg_p->iocr2 &= ~(LIC2_JACKS);
            break;

        case FF_E1_MF:
            tiuFramerReg_p->mcreg = MCREG_RMODE(5)| MCREG_TMODE(5);
            tiuFramerReg_p->iocr2 &= ~(LIC2_JACKS);
            break;

        case FF_E1_CRC:
            tiuFramerReg_p->mcreg = MCREG_RMODE(6)| MCREG_TMODE(6);
            tiuFramerReg_p->iocr2 &= ~(LIC2_JACKS);

            break;


#endif
        default:
            printf("Undefinded Frame: %d\n", lineConfig_p->framing);
            return ERROR;
        }





        switch (lineConfig_p->coding) {
        case LC_HDB3:
            tiuFramerReg_p->e1rcr |= (E1RCR_RHDB3);
            break;


        case LC_AMI:
            switch (lineConfig_p->lineType) {
#ifdef TIU_DEBUG
            case LT_E1:
                tiuFramerReg_p->e1rcr &= ~(E1RCR_RHDB3);
                break
#endif
            case LT_T1:
            case LT_J1:
			    tiuFramerReg_p->t1rcr2 &= ~(T1RCR2_RB8ZS);
				tiuFramerReg_p->t1tcr2 &= ~(T1TCR2_TB8ZS);
				tiuFramerReg_p->t1ccr |= (T1CCR_PDE);   /* PDE required in AMI */
				break;
            }
            break;

        case LC_B8ZS:
		    tiuFramerReg_p->t1rcr2 |= T1RCR2_RB8ZS;
            tiuFramerReg_p->t1tcr2 |= T1TCR2_TB8ZS;
            tiuFramerReg_p->t1ccr &= ~(T1CCR_PDE);  /* PDE not required in B8ZS */
			break;

        default:
            printf("Undefinded Line Coding:  %d\n", lineConfig_p->coding);
            return ERROR;
        }




        switch (lineConfig_p->termination) {
        case LTM_DISABLE:
		    tiuFramerReg_p->lic4 |= LIC4_TT(RX_TERM_DISABLE);
            break;

        case LTM_75OHM:
            tiuFramerReg_p->lic4 |= LIC4_TT(RX_TERM_75OHM);
            break;

        case LTM_100OHM:
            tiuFramerReg_p->lic4 |= LIC4_TT(RX_TERM_100OHM);
            break;

        case LTM_110OHM:
		    tiuFramerReg_p->lic4 |= LIC4_TT(RX_TERM_110OHM);
            break;

        case LTM_120OHM:
		    tiuFramerReg_p->lic4 |= LIC4_TT(RX_TERM_120OHM);
            break;

        default:
            printf("Undefinded Termination:  %d\n",
                   lineConfig_p->termination);
            return ERROR;
        }


        switch (lineConfig_p->lineType) {
#ifdef TIU_BUG
        case LT_E1:
            tiuFramerReg_p->e1rcr |= E1RCR_RESYNC;
            tiuFramerReg_p->e1rcr &= ~E1RCR_RESYNC;
            break;
#endif
        case LT_T1:
        case LT_J1:
			tiuFramerReg_p->bocc = 0;
			tiuFramerReg_p->bocc = BOCC_RBR;
			tiuFramerReg_p->bocc = BOCC_RBOCE;
            break;
        }



    } else {                    /* for expansion shelf case */

        switch (lineConfig_p->lineType) {
        case LT_T1:
            /* 1.544 M */
            /*
               tiuFramerReg_p->tpcr =
               (TPCR_TPLLOFS(0) | TPCR_TPLLIFS(0) | TPCR_TCSS(1)) &
               (~(TPCR_PLLOS));
               tiuFramerReg_p->tpcr |= TPCR_TPLLSS;
             */
            tmpdata =
                (TPCR_TPLLOFS(0) | TPCR_TPLLIFS(0) | TPCR_TCSS(0)) &
                (~(TPCR_PLLOS));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->tpcr), &tmpdata,
                         FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->tpcr), &tmpdata,
                         FPGA_READ);
            tmpdata = tmpdata | TPCR_TPLLSS;
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->tpcr), &tmpdata,
                         FPGA_WRITE);
            break;

        case LT_E1:
            /* 2.048 M */
            /*
               tiuFramerReg_p->tpcr =
               (TPCR_TPLLOFS(1) | TPCR_TPLLIFS(0) | TPCR_TCSS(1)) &
               (~(TPCR_PLLOS));
               tiuFramerReg_p->tpcr |= TPCR_TPLLSS;
             */

            tmpdata = (TPCR_TPLLOFS(1) | TPCR_TPLLIFS(0) | TPCR_TCSS(1)) &
                (~(TPCR_PLLOS));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->tpcr), &tmpdata,
                         FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->tpcr), &tmpdata,
                         FPGA_READ);
            tmpdata = tmpdata | TPCR_TPLLSS;
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->tpcr), &tmpdata,
                         FPGA_WRITE);

            break;

        case LT_J1:
            /* 1.544 M */

            /*
               tiuFramerReg_p->tpcr =
               (TPCR_TPLLOFS(0) | TPCR_TPLLIFS(0) | TPCR_TCSS(1)) &
               (~(TPCR_PLLOS));
               tiuFramerReg_p->tpcr |= TPCR_TPLLSS;
             */

            tmpdata = (TPCR_TPLLOFS(0) | TPCR_TPLLIFS(0) | TPCR_TCSS(1)) &
                (~(TPCR_PLLOS));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->tpcr), &tmpdata,
                         FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->tpcr), &tmpdata,
                         FPGA_READ);
            tmpdata = tmpdata | TPCR_TPLLSS;
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->tpcr), &tmpdata,
                         FPGA_WRITE);


            break;
        case LT_SQW:
            /* 1.544 M */


            printf("wrong LineType for output card TIU :   %d\n",
                   lineConfig_p->framing);
            break;


        case LT_CC:
            /* */

            printf("wrong LineType for output card TIU :   %d\n",
                   lineConfig_p->framing);
            break;

        default:
            printf("Undefinded LineType: %d\n", lineConfig_p->framing);
            return ERROR;

        }



        switch (lineConfig_p->framing) {
        case FF_T1_D4:
            /*
               tiuFramerReg_p->mcreg = MCREG_RMODE(0);
               tiuFramerReg_p->iocr1 &= ~(IOCR1_RSMS1);
               tiuFramerReg_p->iocr2 |= LIC2_JACKS;
             */
            tmpdata = MCREG_RMODE(0) | MCREG_TMODE(0);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->mcreg),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr1),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata & (~(IOCR1_RSMS1));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr1),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr2),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | (LIC2_JACKS);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr2),
                         &tmpdata, FPGA_WRITE);
            tmpdata = tmpdata | (T1TCR2_TFSE);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1tcr2),
                         &tmpdata, FPGA_WRITE);
            break;

        case FF_T1_ESF:
            /*
               tiuFramerReg_p->mcreg = MCREG_RMODE(1);
               tiuFramerReg_p->iocr1 |= IOCR1_RSMS1;
               tiuFramerReg_p->iocr2 |= LIC2_JACKS;
             */
            tmpdata = MCREG_RMODE(1) | MCREG_TMODE(1);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->mcreg),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr1),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | ((IOCR1_RSMS1));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr1),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr2),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | (LIC2_JACKS);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr2),
                         &tmpdata, FPGA_WRITE);
            tmpdata = tmpdata & ~(T1TCR2_TFSE);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1tcr2),
                         &tmpdata, FPGA_WRITE);
            break;

        case FF_J1_D4:
            /*
               tiuFramerReg_p->mcreg = MCREG_RMODE(2);
               tiuFramerReg_p->iocr1 &= ~(IOCR1_RSMS1);
               tiuFramerReg_p->iocr2 |= LIC2_JACKS;
               tiuFramerReg_p->t1rcr2 |= T1RCR2_RD4YM;
             */
            tmpdata = MCREG_RMODE(2) | MCREG_TMODE(2);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->mcreg),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr1),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata & (~(IOCR1_RSMS1));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr1),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr2),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | (LIC2_JACKS);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr2),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | (T1RCR2_RD4YM);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_WRITE);

            break;

        case FF_J1_ESF:
            /*
               tiuFramerReg_p->mcreg = MCREG_RMODE(3);
               tiuFramerReg_p->iocr1 |= IOCR1_RSMS1;
               tiuFramerReg_p->iocr2 |= LIC2_JACKS;
               tiuFramerReg_p->t1rcr2 |= T1RCR2_RD4YM;
               tiuFramerReg_p->t1rcr2 |= T1RCR2_RJC;
             */
            tmpdata = MCREG_RMODE(3) | MCREG_TMODE(3);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->mcreg),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr1),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | ((IOCR1_RSMS1));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr1),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr2),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | (LIC2_JACKS);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->iocr2),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | (T1RCR2_RD4YM);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | (T1RCR2_RJC);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_WRITE);


            break;



        default:
            printf("Undefinded Frame: %d\n", lineConfig_p->framing);
            return ERROR;
        }





        switch (lineConfig_p->coding) {
        case LC_HDB3:
            /*
               tiuFramerReg_p->e1rcr |= (E1RCR_RHDB3);
             */
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->e1rcr),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | ((E1RCR_RHDB3));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->e1rcr),
                         &tmpdata, FPGA_WRITE);

            break;


        case LC_AMI:
            if ((lineConfig_p->lineType == LT_E1) ||
                (lineConfig_p->lineType == LT_SQW)) {

                /* 
                   tiuFramerReg_p->e1rcr &= ~(E1RCR_RHDB3);
                 */
                tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->e1rcr),
                             &tmpdata, FPGA_READ);
                tmpdata = tmpdata & (~(E1RCR_RHDB3));
                tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->e1rcr),
                             &tmpdata, FPGA_WRITE);

            } else if ((lineConfig_p->lineType == LT_T1) ||
                       (lineConfig_p->lineType == LT_J1)) {
                /*
                   tiuFramerReg_p->t1rcr2 &= ~(T1RCR2_RB8ZS);
                   tiuFramerReg_p->t1tcr2 &= ~(T1TCR2_TB8ZS);
                   tiuFramerReg_p->t1ccr |= (T1CCR_PDE);   
                 */
                /* PDE required in AMI */
                tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                             &tmpdata, FPGA_READ);
                tmpdata = tmpdata & (~(T1RCR2_RB8ZS));
                tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                             &tmpdata, FPGA_WRITE);

                tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                             &tmpdata, FPGA_READ);
                tmpdata = tmpdata & (~(T1TCR2_TB8ZS));
                tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                             &tmpdata, FPGA_WRITE);

                tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1ccr),
                             &tmpdata, FPGA_READ);
                tmpdata = tmpdata | ((T1CCR_PDE));
                tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1ccr),
                             &tmpdata, FPGA_WRITE);

            }
            break;

        case LC_B8ZS:
            /*
               tiuFramerReg_p->t1rcr2 |= (T1RCR2_RB8ZS);
               tiuFramerReg_p->t1tcr2 |= (T1TCR2_TB8ZS);
               tiuFramerReg_p->t1ccr &= ~(T1CCR_PDE);      
             */
            /* PDE not required in B8ZS */

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | ((T1RCR2_RB8ZS));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | ((T1TCR2_TB8ZS));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1rcr2),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1ccr),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata & (~(T1CCR_PDE));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->t1ccr),
                         &tmpdata, FPGA_WRITE);

            break;

        default:
            printf("Undefinded Line Coding:  %d\n", lineConfig_p->coding);
            return ERROR;
        }




        switch (lineConfig_p->termination) {
        case LTM_DISABLE:
            /*
               tiuFramerReg_p->lic4 = LIC4_RT(RX_TERM_DISABLE);
             */
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_READ);
            tmpdata |= LIC4_TT(RX_TERM_DISABLE);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_WRITE);

            break;

        case LTM_75OHM:
            /*
               tiuFramerReg_p->lic4 = LIC4_RT(RX_TERM_75OHM);
             */
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_READ);
            tmpdata |= LIC4_TT(RX_TERM_75OHM);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_WRITE);

            break;

        case LTM_100OHM:
            /*
               tiuFramerReg_p->lic4 = LIC4_RT(RX_TERM_100OHM);
             */
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_READ);
            tmpdata |= LIC4_TT(RX_TERM_100OHM);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_WRITE);

            break;

        case LTM_110OHM:
            /*
               tiuFramerReg_p->lic4 = LIC4_RT(RX_TERM_110OHM);
             */
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_READ);
            tmpdata |= LIC4_TT(RX_TERM_110OHM);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_WRITE);

            break;

        case LTM_120OHM:
            /*
               tiuFramerReg_p->lic4 = LIC4_RT(RX_TERM_120OHM);
             */
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_READ);
            tmpdata |= LIC4_TT(RX_TERM_120OHM);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->lic4), &tmpdata,
                         FPGA_WRITE);

            break;

        default:
            printf("Undefinded Termination:  %d\n",
                   lineConfig_p->termination);
            return ERROR;
        }


        if ((lineConfig_p->lineType == LT_E1) ||
            (lineConfig_p->lineType == LT_SQW)) {

            /*
               tiuFramerReg_p->e1rcr |= E1RCR_RESYNC;
               tiuFramerReg_p->e1rcr &= ~E1RCR_RESYNC;
             */
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->e1rcr),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata | ((E1RCR_RESYNC));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->e1rcr),
                         &tmpdata, FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->e1rcr),
                         &tmpdata, FPGA_READ);
            tmpdata = tmpdata & (~(E1RCR_RESYNC));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->e1rcr),
                         &tmpdata, FPGA_WRITE);


        } else if ((lineConfig_p->lineType == LT_T1) ||
                   (lineConfig_p->lineType == LT_J1)) {

            /*
               tiuFramerReg_p->bocc |= (BOCC_RBOCE | BOCC_RBR);
               tiuFramerReg_p->bocc &= ~(BOCC_SBOC);
               tiuFramerReg_p->bocc &= ~(BOCC_RBR);
             */
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->bocc), &tmpdata,
                         FPGA_READ);
            tmpdata = tmpdata | (BOCC_RBOCE | BOCC_RBR);
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->bocc), &tmpdata,
                         FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->bocc), &tmpdata,
                         FPGA_READ);
            tmpdata = tmpdata & (~(BOCC_SBOC));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->bocc), &tmpdata,
                         FPGA_WRITE);

            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->bocc), &tmpdata,
                         FPGA_READ);
            tmpdata = tmpdata & (~(BOCC_RBR));
            tiuFPGAAcess(shelf, (void *) &(tiuFramerReg_p->bocc), &tmpdata,
                         FPGA_WRITE);

        }




    }


    return OK;
}





/**************************************************************************************
* function: tiuReadStatusReg
*  input 
*			int shelf:										which shelf
*			DS26502_t * tiuFramerReg_p: 	the address of the framer
*			WR_REG_INDEX_t reg: 					which register to be read
*			U8_t bits:										which bits to be read out
*			
* return
*		 U8_t value  
***************************************************************************************/
static U8_t tiuReadStatusReg(int shelf, DS26502_t * tiuFramerReg_p,
                             WR_REG_INDEX_t reg, U8_t bits)
{
    VU8_t *cptr = NULL;
    VU8_t value;
    U8_t tmpdata;
    switch (reg) {
    case SR1_REG:
        cptr = &tiuFramerReg_p->sr1;
        break;

    case SR2_REG:
        cptr = &tiuFramerReg_p->sr2;
        break;

    case SR3_REG:
        cptr = &tiuFramerReg_p->sr3;
        break;

    case SR4_REG:
        cptr = &tiuFramerReg_p->sr4;
        break;

    case INFO1_REG:
        cptr = &tiuFramerReg_p->info1;
        break;

    case INFO2_REG:
        cptr = &tiuFramerReg_p->info2;
        break;

    default:
        return (0);
    }

    if (shelf == 0) {

        *cptr = bits;
        value = *cptr & bits;

    } else {
        /*          
         *cptr = bits;
         value = *cptr & bits;
         */
        tmpdata = bits;
        tiuFPGAAcess(shelf, (void *) cptr, &tmpdata, FPGA_WRITE);
        value = tmpdata;


    }



    return (value);
}



/**************************************************************************************
* function: static void tiuFPGAAcess
*  input 
*			int shelf				which shelf
*			void *addr			the address to be written/read
*			U8_t *data			the data
*			U8_t command		FPGA_WRITE / FPGA_READ
*			
* return
*  	OK:  
*  	ERROR:  
***************************************************************************************/


static void tiuFPGAAcess(int shelf, void *addr, U8_t * data, U8_t command)
{
    U8_t hAddr, lAddr, tmpdata;
    if (shelf == 0) {
        if (command == FPGA_WRITE) {
            memcpy(addr, data, 1);
        }
        if (command == FPGA_READ) {
            memcpy(data, addr, 1);
        }



    } else {

        hAddr = (U8_t) (((int) (addr) & 0xFF00) >> 8);
        lAddr = (U8_t) ((int) (addr) & 0xFF);


        expFPGAAccess(shelf, hAddr, lAddr, data, command);




    }

}


/**************************************************************************************
* function: tiuCfgAccess: this routine read/write configuration from flash
*  input 
*			char *fname
*			int mode
*			
* return
*  	OK:  
*  	ERROR:  
***************************************************************************************/

static STATUS_t tiuCfgAccess(char *fname, int mode)
{
    int shelf, slot, framer;
    FILE *fhd = NULL;
    STATUS_t tiuCfgAccessStatus;
    TIU_FRAMER_t *fptr = NULL;


    switch (mode) {
    case TIU_FLASH_READ:
        /* Retrieve Framer configuration from Flash */
        if ((fhd = fopen(fname, "r")) != NULL) {
            fptr = &tiuFramer[0][0][0];
            fread((char *) fptr, sizeof(TIU_FRAMER_t)* TIU_MAX_SHELF * 
                                 TIU_MAX_SLOT * TIU_FRAMERS, 1, fhd);
            fclose(fhd);
        }
        tiuCfgAccessStatus = OK;
        break;

    case TIU_FLASH_WRITE:
        if ((fhd = fopen(fname, "w")) != NULL) {
            fptr = &tiuFramer[0][0][0];
            fwrite((char *) fptr, sizeof(TIU_FRAMER_t) * TIU_MAX_SHELF * 
                                  TIU_MAX_SLOT * TIU_FRAMERS, 1, fhd);
            fclose(fhd);
        } else {
            tiuCfgAccessStatus = ERROR;
        }
        break;
    default:
        tiuCfgAccessStatus = ERROR;
        break;




    }
    return tiuCfgAccessStatus;
}







/******************************************************************************/
/*SNMP Action func */

/*
typedef
struct _stsOutputTiuSettingEntry_t {
        SR_INT32        stsOutputTiuSettingShelf;
        SR_INT32        stsOutputTiuSettingSlot;
        SR_INT32        stsOutputTiuSettingPort;
        SR_INT32        stsOutputTiuSettingLineCoding;
        SR_INT32        stsOutputTiuSettingFMT;
        SR_INT32        stsOutputTiuSettingTermination;
        char            valid[1];
} stsOutputTiuSettingEntry_t;
*/

/**************************************************************************************
* function: STATUS_t snmpTiuGet
*  input 
*			stsOutputTiuSettingEntry_t *mibstructIn
*			
* return
*  	OK:  
*  	ERROR:  
***************************************************************************************/
STATUS_t snmpTiuGet(stsOutputTiuSettingEntry_t * mibstructIn)
{

    TIU_FRAMER_t *fptr =
        &tiuFramer[(mibstructIn->stsOutputTiuSettingShelf) -
                   1][(mibstructIn->stsOutputTiuSettingSlot) -
                      1][(mibstructIn->stsOutputTiuSettingPort) - 1];

    if (VALID(I_stsOutputTiuSettingLineCoding, mibstructIn->valid)) {
        switch (fptr->lnCfg.coding) {
        case LC_AMI:
            mibstructIn->stsOutputTiuSettingLineCoding =
                D_stsOutputTiuSettingLineCoding_ami;
            break;
        case LC_B8ZS:
            mibstructIn->stsOutputTiuSettingLineCoding =
                D_stsOutputTiuSettingLineCoding_b8zs;
            break;
        case LC_HDB3:
            mibstructIn->stsOutputTiuSettingLineCoding =
                D_stsOutputTiuSettingLineCoding_hdb3;
            break;
        default:
            mibstructIn->stsOutputTiuSettingLineCoding =
                D_stsOutputTiuSettingLineCoding_other;
            break;
        }
    }
    if (VALID(I_stsOutputTiuSettingFMT, mibstructIn->valid)) {
        mibstructIn->stsOutputTiuSettingFMT = fptr->lnCfg.framing + 1;
    }
    if (VALID(I_stsOutputTiuSettingTermination, mibstructIn->valid)) {
        mibstructIn->stsOutputTiuSettingTermination =
            fptr->lnCfg.termination;
    }


    return OK;
}


STATUS_t snmpTiuSet(stsOutputTiuSettingEntry_t * mibstructIn)
{

    TIU_FRAMER_t *fptr =
        &tiuFramer[(mibstructIn->stsOutputTiuSettingShelf) -
                   1][(mibstructIn->stsOutputTiuSettingSlot) -
                      1][(mibstructIn->stsOutputTiuSettingPort) - 1];
    switch (mibstructIn->stsOutputTiuSettingLineCoding) {
    case D_stsOutputTiuSettingLineCoding_ami:
        fptr->lnCfg.coding = LC_AMI;
        break;
    case D_stsOutputTiuSettingLineCoding_b8zs:
        fptr->lnCfg.coding = LC_B8ZS;
        break;
    case D_stsOutputTiuSettingLineCoding_hdb3:
        fptr->lnCfg.coding = LC_HDB3;
        break;
    case D_stsOutputTiuSettingLineCoding_other:
        fptr->lnCfg.coding = LC_OTHER;
        break;
    default:
        return (ERROR);
    }


    switch (mibstructIn->stsOutputTiuSettingFMT) {
    case D_stsOutputTiuSettingFMT_t1_d4:
        fptr->lnCfg.lineType = LT_T1;
        fptr->lnCfg.framing = FF_T1_D4;
        break;
    case D_stsOutputTiuSettingFMT_t1_esf:
        fptr->lnCfg.lineType = LT_T1;
        fptr->lnCfg.framing = FF_T1_ESF;
        break;
    case D_stsOutputTiuSettingFMT_j1_d4:
        fptr->lnCfg.lineType = LT_J1;
        fptr->lnCfg.framing = FF_J1_D4;
        break;
    case D_stsOutputTiuSettingFMT_j1_esf:
        fptr->lnCfg.lineType = LT_J1;
        fptr->lnCfg.framing = FF_J1_ESF;
        break;
    default:
        return (ERROR);

    }

    fptr->lnCfg.termination = mibstructIn->stsOutputTiuSettingTermination;



    return (tiuFramerConfig(((mibstructIn->stsOutputTiuSettingShelf) - 1),
                            ((mibstructIn->stsOutputTiuSettingSlot) - 1),
                            ((mibstructIn->stsOutputTiuSettingPort) - 1)));


}


/******************************************************************************/
#ifdef TIU_DEBUG
STATUS_t tiuDebugRemoteLoopSet(int shelf, int slot, int framer, int enable)
{
    int n;
    U8_t tmpdata;
    TIU_FRAMER_t *fptr = &tiuFramer[shelf][slot][framer];
    
    DS26502_t *fReg = (DS26502_t *) (TIU_FRAMER_BASE_ADDR + (framer * 0x100) +
                       (slot * 0x1000) + 0x200);
    if (fptr->status == TRUE){
    	switch(enable){
    		case 0:
    			if (shell ==0){
    				fReg->lbcr &= ~(LBCR_RLB);
    			}else{
    			
    				tiuFPGAAcess(shelf, (void *) &(fReg->lbcr), &tmpdata, FPGA_READ);
            tmpdata &= ~(LBCR_RLB);
            tiuFPGAAcess(shelf, (void *) &(fReg->lbcr), &tmpdata, FPGA_WRITE);

    			}
    			break;
    		
    		case 1:
    			
    			if (shell ==0){
    				fReg->lbcr |= (LBCR_RLB);
    			}else{
    			
    				tiuFPGAAcess(shelf, (void *) &(fReg->lbcr), &tmpdata, FPGA_READ);
            tmpdata |= (LBCR_RLB);
            tiuFPGAAcess(shelf, (void *) &(fReg->lbcr), &tmpdata, FPGA_WRITE);
           }
          break;
    		
    		default:
    			return ERROR;

    
    }else{
    
    	return ERROR; 
    
    }

    
    return OK;
}



STATUS_t tiuDebugModeSetAll(int testcase)
{
    int shelf, slot, framer;


    switch (testcase) {

    case 101:
    case 102:
    case 103:
    case 104:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
    case 208:
    case 209:
    case 210:
    case 301:
    case 302:
    case 303:
    case 304:
        for (shelf = 0; shelf < TIU_MAX_SHELF; shelf++) {
            for (slot = 0; slot < TIU_MAX_SLOT; slot++) {

                for (framer = 0; framer < TIU_FRAMERS; framer++) {
                    framerDebugModeSet(shelf, slot, framer, testcase);

                }


            }

        }
        break;
    default:
        tiuDebugModeHelp();
        break;

    }





}




STATUS_t tiuDebugModeSet(int shelf, int slot, int framer, int testcase)
{
    int n;
    TIU_FRAMER_t *fptr = &tiuFramer[shelf][slot][framer];
    switch (testcase) {

    case 101:
        fptr->lnCfg.lineType = LT_T1;
        fptr->lnCfg.framing = FF_T1_D4;
        fptr->lnCfg.coding = LC_AMI;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 102:

        fptr->lnCfg.lineType = LT_T1;
        fptr->lnCfg.framing = FF_T1_D4;
        fptr->lnCfg.coding = LC_B8ZS;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 103:

        fptr->lnCfg.lineType = LT_T1;
        fptr->lnCfg.framing = FF_T1_ESF;
        fptr->lnCfg.coding = LC_AMI;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 104:

        fptr->lnCfg.lineType = LT_T1;
        fptr->lnCfg.framing = FF_T1_ESF;
        fptr->lnCfg.coding = LC_B8ZS;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 201:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1_CRC;
        fptr->lnCfg.coding = LC_AMI;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 202:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1_CRC;
        fptr->lnCfg.coding = LC_HDB3;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 203:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1;
        fptr->lnCfg.coding = LC_AMI;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 204:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1;
        fptr->lnCfg.coding = LC_HDB3;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 205:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1_MF;
        fptr->lnCfg.coding = LC_AMI;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 206:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1_MF;
        fptr->lnCfg.coding = LC_HDB3;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 207:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1_CRC_MF;
        fptr->lnCfg.coding = LC_AMI;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 208:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1_CRC_MF;
        fptr->lnCfg.coding = LC_HDB3;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 209:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1_SQW;
        fptr->lnCfg.coding = LC_AMI;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 210:

        fptr->lnCfg.lineType = LT_E1;
        fptr->lnCfg.framing = FF_E1_SQW;
        fptr->lnCfg.coding = LC_HDB3;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 301:

        fptr->lnCfg.lineType = LT_J1;
        fptr->lnCfg.framing = FF_J1_D4;
        fptr->lnCfg.coding = LC_AMI;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 302:

        fptr->lnCfg.lineType = LT_J1;
        fptr->lnCfg.framing = FF_J1_D4;
        fptr->lnCfg.coding = LC_JBZS;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 303:

        fptr->lnCfg.lineType = LT_J1;
        fptr->lnCfg.framing = FF_J1_D4;
        fptr->lnCfg.coding = LC_AMI;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    case 304:

        fptr->lnCfg.lineType = LT_J1;
        fptr->lnCfg.framing = FF_J1_ESF;
        fptr->lnCfg.coding = LC_JBZS;
        fptr->lnCfg.termination = LTM_120OHM;
        fptr->status = TRUE;
        break;
    default:
        tiudebugModeHelp();
        break;

    }
    tiuFramerConfig(shelf, slot, framer);
    return OK;
}

static void tiuDebugModeHelp()
{

    printf
        ("the usage\nframerDebugModeSet 205 for example\n for other mode see below\n"
         "case 101:\tLT_T1;\tFF_T1_D4;\tLC_AMI;\tLTM_120OHM;\n"
         "case 102:\tLT_T1;\tFF_T1_D4;\tLC_B8ZS;\tLTM_120OHM;\n"
         "case 103:\tLT_T1;\tFF_T1_ESF;\tLC_AMI;\tLTM_120OHM;\n"
         "case 104:\tLT_T1;\tFF_T1_ESF;\tLC_B8ZS;\tLTM_120OHM;\n"
         "case 201:\tLT_E1;\tFF_E1_CRC;\tLC_AMI;\tLTM_120OHM;\n"
         "case 202:\tLT_E1;\tFF_E1_CRC;\tLC_HDB3;\tLTM_120OHM;\n"
         "case 203:\tLT_E1;\tFF_E1;\tLC_AMI;\tLTM_120OHM;\n"
         "case 204:\tLT_E1;\tFF_E1;\tLC_HDB3;\tLTM_120OHM;\n"
         "case 205:\tLT_E1;\tFF_E1_MF;\tLC_AMI;\tLTM_120OHM;\n"
         "case 206:\tLT_E1;\tFF_E1_MF;\tLC_HDB3;\tLTM_120OHM;\n"
         "case 207:\tLT_E1;\tFF_E1_CRC_MF;\tLC_AMI;\tLTM_120OHM;\n"
         "case 208:\tLT_E1;\tFF_E1_CRC_MF;\tLC_HDB3;\tLTM_120OHM;\n"
         "case 209:\tLT_E1;\tFF_E1_SQW;\tLC_AMI;\tLTM_120OHM;\n"
         "case 210:\tLT_E1;\tFF_E1_SQW;\tLC_HDB3;\tLTM_120OHM;\n"
         "case 301:\tLT_J1;\tFF_J1_D4;\tLC_AMI;\tLTM_120OHM;\n"
         "case 302:\tLT_J1;\tFF_J1_D4;\tLC_JBZS;\tLTM_120OHM;\n"
         "case 303:\tLT_J1;\tFF_J1_D4;\tLC_AMI;\tLTM_120OHM;\n"
         "case 304:\tLT_J1;\tFF_J1_ESF;\tLC_JBZS;\tLTM_120OHM;\n"
         "default:\tLT_T1;\tFF_J1_ESF;\tLC_B8ZS;\tLTM_100OHM;\n");

}

#endif
/******************************************************************************/

