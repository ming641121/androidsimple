/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: framer.h
 *
 *input framer using maxim ds26502
 *
 *Note:
 *
 *Author(s):
 * shuming chang
 */
 /********************************************************************
 * $Id: framer.c,v 1.42 2006/10/03 05:41:39 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/driver/framer/input/framer.c,v $
 *********************************************************************/

/* readme
todo
2. alarm handler and test
4. test T1 ssm [CAS]
5. add disable/enable framer in inpLinConfig

*/

#include <stdlib.h>
#include <stdio.h>
#include <vxWorks.h>
#include <sys_memmap.h>
#include <tss_types.h>
#include "ds26502.h"
#include <taskLib.h>
#include <intLib.h>
#include <iv.h>
#include "framer_event.h"
#include "trapdef.h"
#include "siw_events.h"
#include "trap_if.h"
#include "led_if.h"
#include "pm.h"
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



/* global variable declaration */
INP_FRAMER_t inpFramer[MAX_FRAMERS];
U32_t inpFramerBaseAddr[MAX_FRAMERS] = {
    BITS_0_ADDR_BASE, BITS_1_ADDR_BASE, BITS_2_ADDR_BASE,
    BITS_3_ADDR_BASE, BITS_4_ADDR_BASE
};
SSM_t receivedSSM[MAX_FRAMERS][SSM_MESSAGES];
TE1_CONFIG_t te1Cfg[MAX_FRAMERS];



/* global function declaration */
int inpFramerTask(void);
STATUS_t getInpFramerSSM(int n);
STATUS_t checkInpFramer(int n);
STATUS_t enableInpFramerSSM(int n, int ch);
STATUS_t disableInpFramerSSM(int n, int ch);
void alarmIntHandler(void);
STATUS_t setInpFramerAlarm(TE1_ALARM_t alarmtype, TE1_ALARMACTION_t action,
                           int ch);
STATUS_t inpLineConfig(int n);
STATUS_t framerSeverityCheck(int n);
STATUS_t framerDataAccess(char *fname, int mode, int dataType);


#ifdef FRAMER_DEBUG
STATUS_t framerDebugLedTest(void);
STATUS_t framerDebugShow(void);
STATUS_t framerDebugModeSet(int testcase);
void framerDebugEnablessm(void);
void framerDebugRestart(void);
#endif



/* static function declaration */
static void initInpFramer(void);

static STATUS_t alarmIntInit(int n);
static void processAlarmInt(int n);
static U8_t readStatusReg(int n, WR_REG_INDEX_t reg, U8_t bits);
static STATUS_t pollAlarm(int n, int alarm_type, U8_t * valueptr);
static STATUS_t framerLED(int n, int color);

extern SYS_CONFIG_T sysConfig;
extern PM_SEVERITY_t pmSeverity[LINK_TYPES];


/**************************************************************************************
* function: inpFramerTask
*  input parameter
*			int n: which framer
*  return
*			
***************************************************************************************/
int inpFramerTask()
{
    int n;

    /* initialize Input Framers */
    initInpFramer();

	SiwTaskInitAck(OK);

    for (;;) {
        for (n = 0; n < MAX_FRAMERS; n++) {
        	if (te1Cfg[n].linkEnable == FRAMER_LINK_ENABLE_YES){
	            if (checkInpFramer(n) == OK) {
	                getInpFramerSSM(n);
	            }
	
	            else {
	            }
        	}
        }
        taskDelay(POLL_FRAMER_INTERVAL);
    }
}
/**************************************************************************************
* function: initInpFramer
*  input parameter
*			int n: which framer
*  return
*			
***************************************************************************************/
static void initInpFramer()
{
    int i, n;
    INP_FRAMER_t *ptr;
    static U8_t malloc4linkIdYet=0; /* if not '0', means linkId has been mallocated.*/
    
    printf("framer init start\n");
    memset(&inpFramer[0], (sizeof(INP_FRAMER_t) * MAX_FRAMERS), 0);
    memset(&receivedSSM[0], (sizeof(SSM_t) * MAX_FRAMERS * SSM_MESSAGES),
           1);
    memset(&te1Cfg[0], (sizeof(TE1_CONFIG_t) * MAX_FRAMERS), 0);
    /* get the settings */
    {
    	/* line configuraiton */
      	if ( (framerDataAccess("telCfg.dat", FLASH_READ, TEL_CFG_DATA)==ERROR) ) {
        		for (n = 0; n < MAX_FRAMERS; n++) {
        		    if (malloc4linkIdYet ==0){
        		    	te1Cfg[n].linkId.octet_ptr = malloc(FRAMER_LINKID_LEN);
                  malloc4linkIdYet++;
                }
        			te1Cfg[n].linkId.length = snprintf(te1Cfg[n].linkId.octet_ptr,
        											FRAMER_LINKID_LEN, 
        											"inputFramer-%d", n);
            		te1Cfg[n].linkEnable = FRAMER_LINK_ENABLE_NO;
            		te1Cfg[n].lineType = LT_T1;
            		te1Cfg[n].framing = FF_T1_ESF;
            		te1Cfg[n].coding = LC_B8ZS;
            		te1Cfg[n].termination = LTM_100OHM;
            }
        		framerDataAccess("telCfg.dat", FLASH_WRITE, TEL_CFG_DATA);
         }
      /* SSM */
      	if ( (framerDataAccess("revSSM.dat", FLASH_READ, RECEIVED_SSM)==ERROR) ) {
            for (n = 0; n < MAX_FRAMERS; n++) {
            		receivedSSM[n][0].ssm = 0;
            		receivedSSM[n][1].ssm = 0;
            		receivedSSM[n][2].ssm = 0;
            		receivedSSM[n][3].ssm = 0;
            		receivedSSM[n][4].ssm = 0;
            		receivedSSM[n][5].ssm = 0;
            }
        		framerDataAccess("revSSM.dat", FLASH_WRITE, RECEIVED_SSM);
        }
    }

    /* init LED */
    for (n = 0; n < MAX_FRAMERS; n++) {
        framerLED(n, FRAMER_LED_OFF);
    }



    /* configure framer */
    for (n = 0; n < MAX_FRAMERS; n++) {
        ptr = &inpFramer[n];
        ptr->reg = (DS26502_t *) inpFramerBaseAddr[n];

        /* Soft reset Framer */
        ptr->reg->tstrreg = 0;
        ptr->reg->tstrreg = TSTRREG_SFTRST;
        printf("-- on framer %d[addr: %x]\n", n, (int) ptr->reg);

        /* wait for reset bit to clear */
        for (i = 0; i < MAX_RESET_WAIT; i++)
            if (!(ptr->reg->tstrreg & TSTRREG_SFTRST))
                break;
        if (i >= MAX_RESET_WAIT) {
            printf("Failed to initialize Framer %d\n", n);
            ptr->status = FRAMER_OUT_OF_SERVICE;
            continue;
        }

        /* Reset the Line Interface */
        printf("  on framer init reset\n");
        ptr->reg->lic2 = 0;
        ptr->reg->lic2 = LIC2_LIRST;
        ptr->reg->lic2 = 0;
        ptr->reg->iocr1 = 1;    /* TS_8K_4 is an output */

        /* configure control Register */
        printf("  on framer init config control reg\n");
        if ((te1Cfg[n].lineType == LT_E1)
            || (te1Cfg[n].lineType == LT_SQW))
            ptr->reg->e1rcr = E1RCR_FRC;

        else if (te1Cfg[n].lineType == LT_T1) {
            ptr->reg->t1rcr1 = 0;
            ptr->reg->t1rcr1 = T1RCR1_SYNCT | T1RCR1_SYNCC |
                (T1RCR1_OOF(OOF_CRITERIA)) | T1RCR1_RESYNC;
            ptr->reg->t1rcr2 = 0;
            ptr->reg->bocc = BOCC_RBF(RX_BOC_THRESHOLD);
        }

        else if (te1Cfg[n].lineType == LT_J1) {
            ptr->reg->t1rcr1 = 0;
            ptr->reg->t1rcr1 = T1RCR1_SYNCT | T1RCR1_SYNCC |
                (T1RCR1_OOF(OOF_CRITERIA)) | T1RCR1_RESYNC;
            ptr->reg->t1rcr2 = T1RCR2_RD4YM | T1RCR2_RJC;
            ptr->reg->bocc = BOCC_RBF(RX_BOC_THRESHOLD);
        }

        else {

            /* There is no CC Control Register */
        }

        
        printf("  on framer init clear crc\n");
        ptr->reg->e1rcr &= ~(1 << 3);



        /* Make sure all interrupts are off */
        printf("  on framer init mark out all interrupt\n");
        ptr->reg->imr1 = 0;
        ptr->reg->imr2 = 0;
        ptr->reg->imr3 = 0;
        ptr->reg->imr4 = 0;

        /* disable loopback */
        ptr->reg->lbcr = 0;

        /* configure LIU */
        printf("  on framer init config LIU\n");
        ptr->reg->lic1 = LIC1_TPD;
        ptr->reg->lic4 = LIC4_MPS(MCLK_MODE);
        if ((te1Cfg[n].lineType == LT_T1) || (te1Cfg[n].lineType == LT_J1))
            ptr->reg->lic2 = LIC2_JACKS;

        /* setup line configuration */
        printf("  on framer init --> inpLineConfig\n");
        inpLineConfig(n);
        ptr->status = FRAMER_SERVICE_INIT;


        /* remove some config to inpLineconfig */

        /* Enable alarm interrupt */
        printf("  on framer init alarmIntInit\n");

        /* shuming temp mark out *//* alarmIntInit(n); */
    }
}
/**************************************************************************************
* function: inpLineConfig
*  input parameter
*			int n: which framer
*  return
*			
***************************************************************************************/
STATUS_t inpLineConfig(int n)
{
    INP_FRAMER_t *ptr = &inpFramer[n];

/* framing type config */
    switch (te1Cfg[n].framing) {
    case FF_T1_D4:
        ptr->reg->mcreg = MCREG_RMODE(0);
        ptr->reg->iocr1 &= ~(IOCR1_RSMS1);
        ptr->reg->iocr2 |= LIC2_JACKS;
        break;
    case FF_T1_ESF:
        ptr->reg->mcreg = MCREG_RMODE(1);
        ptr->reg->iocr1 |= IOCR1_RSMS1;
        ptr->reg->iocr2 |= LIC2_JACKS;
        break;
    case FF_J1_D4:
        ptr->reg->mcreg = MCREG_RMODE(2);
        ptr->reg->iocr1 &= ~(IOCR1_RSMS1);
        ptr->reg->iocr2 |= LIC2_JACKS;
        ptr->reg->t1rcr2 |= T1RCR2_RD4YM;
        break;
    case FF_J1_ESF:
        ptr->reg->mcreg = MCREG_RMODE(3);
        ptr->reg->iocr1 |= IOCR1_RSMS1;
        ptr->reg->iocr2 |= LIC2_JACKS;
        ptr->reg->t1rcr2 |= T1RCR2_RD4YM;
        ptr->reg->t1rcr2 |= T1RCR2_RJC;
        break;
    case FF_E1:
        ptr->reg->mcreg = MCREG_RMODE(4);
        ptr->reg->iocr2 &= ~(LIC2_JACKS);
        break;
    case FF_E1_MF:
        ptr->reg->mcreg = MCREG_RMODE(5);
        ptr->reg->iocr2 &= ~(LIC2_JACKS);
        break;
    case FF_E1_CRC:
        ptr->reg->mcreg = MCREG_RMODE(6);
        ptr->reg->iocr2 &= ~(LIC2_JACKS);
        break;
    case FF_E1_CRC_MF:
        ptr->reg->mcreg = MCREG_RMODE(7);
        ptr->reg->iocr2 &= ~(LIC2_JACKS);
        break;
    case FF_E1_SQW:
        ptr->reg->mcreg = MCREG_RMODE(8);
        ptr->reg->iocr2 &= ~(LIC2_JACKS);
        break;
    case FF_CC:
        ptr->reg->mcreg = MCREG_RMODE(9);
        ptr->reg->iocr2 &= ~(LIC2_JACKS);
        break;
    case FF_CC_400:
        ptr->reg->mcreg = MCREG_RMODE(10);
        ptr->reg->iocr2 &= ~(LIC2_JACKS);
        break;
    default:
        printf("Undefinded Frame: [%d]: %d\n", n, te1Cfg[n].framing);
        return ERROR;
    }

/* line code config */
    switch (te1Cfg[n].coding) {
    case LC_HDB3:
        ptr->reg->e1rcr |= (E1RCR_RHDB3);
        break;
    case LC_AMI:
        if ((te1Cfg[n].lineType == LT_E1) ||
            (te1Cfg[n].lineType == LT_SQW))
            ptr->reg->e1rcr &= ~(E1RCR_RHDB3);

        else if ((te1Cfg[n].lineType == LT_T1) ||
                 (te1Cfg[n].lineType == LT_J1)) {
            ptr->reg->t1rcr2 &= ~(T1RCR2_RB8ZS);
            ptr->reg->t1tcr2 &= ~(T1TCR2_TB8ZS);
            ptr->reg->t1ccr |= (T1CCR_PDE);     /* PDE required in AMI */
        }
        break;
    case LC_B8ZS:
        ptr->reg->t1rcr2 |= (T1RCR2_RB8ZS);
        ptr->reg->t1tcr2 |= (T1TCR2_TB8ZS);
        ptr->reg->t1ccr &= ~(T1CCR_PDE);        /* PDE not required in B8ZS */
        break;
    default:
        printf("Undefinded Line Coding: [%d]: %d\n", n, te1Cfg[n].coding);
        return ERROR;
    }

/* termination config */
    switch (te1Cfg[n].termination) {
    case LTM_DISABLE:
        ptr->reg->lic4 = LIC4_RT(RX_TERM_DISABLE);
        break;
    case LTM_75OHM:
        ptr->reg->lic4 = LIC4_RT(RX_TERM_75OHM);
        break;
    case LTM_100OHM:
        ptr->reg->lic4 = LIC4_RT(RX_TERM_100OHM);
        break;
    case LTM_110OHM:
        ptr->reg->lic4 = LIC4_RT(RX_TERM_110OHM);
        break;
    case LTM_120OHM:
        ptr->reg->lic4 = LIC4_RT(RX_TERM_120OHM);
        break;
    default:
        printf("Undefinded Termination: [%d]: %d\n", n,
               te1Cfg[n].termination);
        return ERROR;
    }
    
  /* PLL output config */
			/* have PLL out to be 1.544 no matter what input */
    switch (te1Cfg[n].framing) {
    case FF_T1_D4:
    case FF_T1_ESF:
    case FF_J1_D4:
    case FF_J1_ESF:
    		ptr->reg->tpcr = (TPCR_TPLLIFS(0))|(TPCR_TPLLOFS(0));
    		break;
    case FF_E1:
    case FF_E1_MF:
    case FF_E1_CRC:
    case FF_E1_CRC_MF:
    case FF_E1_SQW:
    		ptr->reg->tpcr = (TPCR_TPLLIFS(1))|(TPCR_TPLLOFS(0));
    		break;
    case FF_CC:
    case FF_CC_400:
    		ptr->reg->tpcr = (TPCR_TPLLIFS(2))|(TPCR_TPLLOFS(0));
    		break;
    default:
        printf("fail to set PLL out for mode %d in framer %d\n", te1Cfg[n].framing, n);
        return ERROR;
    }
	/* misc config*/
    if ((te1Cfg[n].lineType == LT_E1) || (te1Cfg[n].lineType == LT_SQW)) {
        ptr->reg->e1rcr |= E1RCR_RESYNC;
        ptr->reg->e1rcr &= ~E1RCR_RESYNC;
    } else if ((te1Cfg[n].lineType == LT_T1) ||
               (te1Cfg[n].lineType == LT_J1)) {
        ptr->reg->bocc |= (BOCC_RBOCE | BOCC_RBR);
        ptr->reg->bocc &= ~(BOCC_SBOC);
        ptr->reg->bocc &= ~(BOCC_RBR);
    }
    return OK;
}

/**************************************************************************************
* function: enableInpFramerSSM
*  input parameter
*			int n: which framer
*  return
*			
***************************************************************************************/
STATUS_t enableInpFramerSSM(int n, int ch)
{
    INP_FRAMER_t *ptr = &inpFramer[n];
    int i, count;
    U8_t tmp1, tmp2;
    receivedSSM[n][ch].status == SSM_ENABLE;
    if (te1Cfg[n].lineType == LT_E1) {
        for (i = 0; i < 5; i++) {
            tmp1 = (U8_t) ptr->reg->rsa4;
            taskDelay(10);
            tmp2 = (U8_t) ptr->reg->rsa4;
            if (tmp1 == tmp2)
                break;
        }
        if (tmp1 == tmp2) {
            printf("in enableInpFramerSSM  E1, rsa4: %x\n", tmp2);
            receivedSSM[n][ch].ssm = tmp1;
            return OK;
        }

        else {
            receivedSSM[n][ch].ssm = 0;
            return ERROR;
        }
    }

    else if ((te1Cfg[n].lineType == LT_T1) ||
             (te1Cfg[n].lineType == LT_J1)) {
        for (i = 0; i < 5; i++) {
            tmp1 = (U8_t) ((ptr->reg->rfdl & 0x3f) << 1);
            taskDelay(10);
            tmp2 = (U8_t) ((ptr->reg->rfdl & 0x3f) << 1);
            if (tmp1 == tmp2)
                break;
        }
        if (tmp1 == tmp2) {
            printf("in enableInpFramerSSM  T1, rsa4: %x\n", tmp2);
            receivedSSM[n][0].ssm = tmp1;
            return OK;
        }

        else {
            receivedSSM[n][ch].ssm = 0;
            return ERROR;
        }
    }
    return OK;
}

/**************************************************************************************
* function: disableInpFramerSSM
*  input parameter
*			int n: which framer
*  return
*			
***************************************************************************************/

STATUS_t disableInpFramerSSM(int n, int ch)
{
    receivedSSM[n][ch].status = SSM_DISABLE;
    return OK;
}

/**************************************************************************************
* function: getInpFramerSSM
*  input parameter
*			int n: which framer
*  return
*			
***************************************************************************************/

STATUS_t getInpFramerSSM(int n)
{
    INP_FRAMER_t *ptr = &inpFramer[n];
    U8_t tmp;
    if (te1Cfg[n].lineType == LT_E1) {

        /* Get Sa4 Byte */
        if (receivedSSM[n][0].status == SSM_ENABLE) {
            tmp = (U8_t) ptr->reg->rsa4;
            printf("received e1 ssm[4]: %x\n", tmp);
            if (tmp != receivedSSM[n][0].ssm) {
                if (receivedSSM[n][0].alarm != ALARM_ON) {
                    if (receivedSSM[n][0].count > 2) {
                        receivedSSM[n][0].alarm = ALARM_ON;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][0].count = 0;
                        receivedSSM[n][0].ssm = tmp;
                    }

                    else
                        receivedSSM[n][0].count++;
                }

                else
                    receivedSSM[n][0].count = 0;
            }

            else {
                if (receivedSSM[n][0].alarm == ALARM_ON) {
                    if (receivedSSM[n][0].count > 2) {
                        receivedSSM[n][0].alarm = ALARM_OFF;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][0].count = 0;
                    }

                    else
                        receivedSSM[n][0].count++;
                }

                else
                    receivedSSM[n][0].count = 0;
            }
        }

        /* Get Sa5 Byte */
        if (receivedSSM[n][1].status == SSM_ENABLE) {
            tmp = (U8_t) ptr->reg->rsa5;
            printf("received e1 ssm[5]: %x\n", tmp);
            if (tmp != receivedSSM[n][1].ssm) {
                if (receivedSSM[n][1].alarm != ALARM_ON) {
                    if (receivedSSM[n][1].count > 2) {
                        receivedSSM[n][1].alarm = ALARM_ON;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][1].count = 0;
                        receivedSSM[n][1].ssm = tmp;
                    }

                    else
                        receivedSSM[n][1].count++;
                }

                else
                    receivedSSM[n][1].count = 0;
            }

            else {
                if (receivedSSM[n][1].alarm == ALARM_ON) {
                    if (receivedSSM[n][1].count > 2) {
                        receivedSSM[n][1].alarm = ALARM_OFF;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][1].count = 0;
                    }

                    else
                        receivedSSM[n][1].count++;
                }

                else
                    receivedSSM[n][1].count = 0;
            }
        }

        /* Get Sa6 Byte */
        if (receivedSSM[n][2].status == SSM_ENABLE) {
            tmp = (U8_t) ptr->reg->rsa6;
            printf("received e1 ssm[6]: %x\n", tmp);
            if (tmp != receivedSSM[n][2].ssm) {
                if (receivedSSM[n][2].alarm != ALARM_ON) {
                    if (receivedSSM[n][2].count > 2) {
                        receivedSSM[n][2].alarm = ALARM_ON;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][2].count = 0;
                        receivedSSM[n][2].ssm = tmp;
                    }

                    else
                        receivedSSM[n][2].count++;
                }

                else
                    receivedSSM[n][2].count = 0;
            }

            else {
                if (receivedSSM[n][2].alarm == ALARM_ON) {
                    if (receivedSSM[n][2].count > 2) {
                        receivedSSM[n][2].alarm = ALARM_OFF;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][2].count = 0;
                    }

                    else
                        receivedSSM[n][2].count++;
                }

                else
                    receivedSSM[n][2].count = 0;
            }
        }

        /* Get Sa7 Byte */
        if (receivedSSM[n][3].status == SSM_ENABLE) {
            tmp = (U8_t) ptr->reg->rsa7;
            printf("received e1 ssm[7]: %x\n", tmp);
            if (tmp != receivedSSM[n][3].ssm) {
                if (receivedSSM[n][3].alarm != ALARM_ON) {
                    if (receivedSSM[n][3].count > 2) {
                        receivedSSM[n][3].alarm = ALARM_ON;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][3].count = 0;
                        receivedSSM[n][3].ssm = tmp;
                    }

                    else
                        receivedSSM[n][3].count++;
                }

                else
                    receivedSSM[n][3].count = 0;
            }

            else {
                if (receivedSSM[n][3].alarm == ALARM_ON) {
                    if (receivedSSM[n][3].count > 2) {
                        receivedSSM[n][3].alarm = ALARM_OFF;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][3].count = 0;
                    }

                    else
                        receivedSSM[n][3].count++;
                }

                else
                    receivedSSM[n][3].count = 0;
            }
        }

        /* Get Sa8 Byte */
        if (receivedSSM[n][4].status == SSM_ENABLE) {
            tmp = (U8_t) ptr->reg->rsa8;
            printf("received e1 ssm[8]: %x\n", tmp);
            if (tmp != receivedSSM[n][4].ssm) {
                if (receivedSSM[n][4].alarm != ALARM_ON) {
                    if (receivedSSM[n][4].count > 2) {
                        receivedSSM[n][4].alarm = ALARM_ON;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][4].count = 0;
                        receivedSSM[n][4].ssm = tmp;
                    }

                    else
                        receivedSSM[n][4].count++;
                }

                else
                    receivedSSM[n][4].count = 0;
            }

            else {
                if (receivedSSM[n][4].alarm == ALARM_ON) {
                    if (receivedSSM[n][4].count > 2) {
                        receivedSSM[n][4].alarm = ALARM_OFF;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][4].count = 0;
                    }

                    else
                        receivedSSM[n][4].count++;
                }

                else
                    receivedSSM[n][4].count = 0;
            }
        }
    }

    else if ((te1Cfg[n].lineType == LT_T1) ||
             (te1Cfg[n].lineType == LT_J1)) {
        if (receivedSSM[n][0].status == SSM_ENABLE) {
            receivedSSM[n][0].ssm = (U8_t) ((ptr->reg->rfdl & 0x3f) << 1);
            printf("received T1 ssm: %x\n",
                   (U8_t) ((ptr->reg->rfdl & 0x3f) << 1));
            if (readStatusReg(n, SR3_REG, SR3_RBOC)) {
                if (receivedSSM[n][0].alarm != ALARM_ON) {
                    if (receivedSSM[n][0].count > 2) {  /* Debouncing */
                        receivedSSM[n][0].alarm = ALARM_ON;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][0].count = 0;
                        receivedSSM[n][0].ssm = tmp;
                    }

                    else
                        receivedSSM[n][0].count++;
                }

                else
                    receivedSSM[n][0].count = 0;
            }

            else {
                if (receivedSSM[n][0].alarm == ALARM_ON) {
                    if (receivedSSM[n][0].count > 2) {
                        receivedSSM[n][0].alarm = ALARM_OFF;

                        /* checkitout sendAlarm(); */
                        receivedSSM[n][0].count = 0;
                    }

                    else
                        receivedSSM[n][0].count++;
                }

                else
                    receivedSSM[n][0].count = 0;
            }
        }
    }else {
        receivedSSM[n][0].ssm = 0;
        receivedSSM[n][1].ssm = 0;
        receivedSSM[n][2].ssm = 0;
        receivedSSM[n][3].ssm = 0;
        receivedSSM[n][4].ssm = 0;
        receivedSSM[n][5].ssm = 0;
        receivedSSM[n][0].count = 0;
        receivedSSM[n][0].alarm = ALARM_OFF;
    }
    return OK;
}

/**************************************************************************************
* function: checkPrsPresent
*  input parameter
*			int n: which framer
*  return
*			0: not exist
*			1: present
***************************************************************************************/

int checkPrsPresent(void){
	U8_t tmpData=0, ret;
	stsLinkUp_t     snmpLinkUp;      
    stsLinkDown_t   snmpLinkDown;
	static U8_t prs_state_lastCheck= PRS_STATUS_INIT;
	tmpData	=	(*(U8_t *) (MAIN_FPGA_R_REG_7)); 
	if (MAIN_FPGA_R_REG_7_PRS_PRESENT & tmpData){
		/* present */
		ret	=	PRS_STATUS_UP;

	}else{
		/* not present*/
		ret	=	PRS_STATUS_DOWN;
		
	}

    

	
	if (ret != prs_state_lastCheck){

#if LINKUPDOWN_DEFINED		
        
        if (ret == PRS_STATUS_UP){
            /* send linkup_trap */
            snmpLinkUp.stsSystemId.octet_ptr 					= sysConfig.stsSystemId.octet_ptr;
            snmpLinkUp.stsSystemId.length 						= sysConfig.stsSystemId.length;
            snmpLinkUp.stsClockInputManualSelect			=	D_stsClockInputManualSelect_prs;
            TrapEventLog(stsLinkUp_TRAP, D_stsClockInputManualSelect_prs, D_stsCardShelf_main,
    		    D_stsCardIndex_imu, D_stsClockInputManualSelect_prs, TRAP_CLEAR, &snmpLinkDown,
    		    SIW_EVENT_SYM(PRS, LINKUP));
    	
        }else if (ret == PRS_STATUS_DOWN){
    	    /* send linkdown_trap */
    	
            snmpLinkDown.stsSystemId.octet_ptr 				= sysConfig.stsSystemId.octet_ptr;
    		snmpLinkDown.stsSystemId.length 					= sysConfig.stsSystemId.length;
    		snmpLinkDown.stsClockInputManualSelect		=	D_stsClockInputManualSelect_prs;
    						
    		TrapEventLog(stsLinkDown_TRAP, D_stsClockInputManualSelect_prs, D_stsCardShelf_main,
    		    D_stsCardIndex_imu, D_stsClockInputManualSelect_prs, TRAP_MAJOR, &snmpLinkDown,
    			SIW_EVENT_SYM(PRS, LINKDOWN));
    	}
#endif
		prs_state_lastCheck	=	ret;
    }

	
	
	}

/**************************************************************************************
* function: checkInpFramer
*  input parameter
*			int n: which framer
*  return
*			
***************************************************************************************/

STATUS_t checkInpFramer(int n)
{
    INP_FRAMER_t *ptr = &inpFramer[n];
    U8_t alarms, tmp;

    U32_t *dumPtr = NULL;

    U32_t ledflag = -1;

    stsLinkAlarmStatusChange_t 	snmpLinkAlarm;
    stsLinkUp_t 	snmpLinkUp;      
    stsLinkDown_t	snmpLinkDown;
    
    snmpLinkAlarm.stsLinkAlarmSettingLinkType = n + 1;  /* system 0-based, snmp 1-based */
    snmpLinkAlarm.stsSystemId.octet_ptr 			= sysConfig.stsSystemId.octet_ptr;
    snmpLinkAlarm.stsSystemId.length 					= sysConfig.stsSystemId.length;

    snmpLinkUp.stsSystemId.octet_ptr 					= sysConfig.stsSystemId.octet_ptr;
    snmpLinkUp.stsSystemId.length 						= sysConfig.stsSystemId.length;
    
    snmpLinkDown.stsSystemId.octet_ptr 				= sysConfig.stsSystemId.octet_ptr;
    snmpLinkDown.stsSystemId.length 					= sysConfig.stsSystemId.length;
    
#ifdef POLL_MODE
    /* Read Alarms from SR2 */
    alarms = readStatusReg(n, SR2_REG, 0x0F);
    if (alarms & 0x0F) {

        /* check LOS */
        if (alarms & SR2_RLOS) {
            if (ptr->losAlarm != ALARM_ON) {
                ptr->losAlarm = ALARM_ON;


                /* sendAlarm(T1_COND_LOS,SERVICE); */
                SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOS), n);
                snmpLinkAlarm.stsLinkAlarmSettingType =
                    D_stsLinkAlarmSettingType_los;
                snmpLinkAlarm.stsLinkAlarmSevirity =
                    pmSeverity[n].los;
                TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                             snmpLinkAlarm.stsLinkAlarmSettingType,
                             D_stsCardShelf_main, D_stsCardIndex_imu,
                             n + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                             (U32_t *) & snmpLinkAlarm,
                             SIW_EVENT_SYM(FRAMER, LOS), n + 1);


#ifdef FRAMER_DEBUG
                printf("framer %d LOS occurs\n", n);
#endif
            }
        }
        if (te1Cfg[n].lineType != FF_E1_SQW) {

            /* check LOF */
            if (alarms & SR2_RLOF) {
                if (ptr->lofAlarm != ALARM_ON) {
                    ptr->lofAlarm = ALARM_ON;



                    /* sendAlarm( T1_COND_LOF,SERVICE ); */
                    SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOF), n);
                    snmpLinkAlarm.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
                    snmpLinkAlarm.stsSystemId.length = sysConfig.stsSystemId.length;
                    snmpLinkAlarm.stsLinkAlarmSettingType =
                        D_stsLinkAlarmSettingType_lof;
                    snmpLinkAlarm.stsLinkAlarmSevirity =
                        pmSeverity[n].lof;
                    TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                                 snmpLinkAlarm.stsLinkAlarmSettingType,
                                 D_stsCardShelf_main, D_stsCardIndex_imu,
                                 n + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                                 (U32_t *) & snmpLinkAlarm,
                                 SIW_EVENT_SYM(FRAMER, LOS), n + 1);

#ifdef FRAMER_DEBUG
                    printf("framer %d LOF occurs\n", n);
#endif
                }
            }

            /* check RAIS */
            if (alarms & SR2_RAIS) {
                if (ptr->raisAlarm != ALARM_ON) {
                    ptr->raisAlarm = ALARM_ON;

                    ledflag = FRAMER_LED_YELLOW;
                    /* sendAlarm( T1_COND_RAIS,SERVICE ); */
                    SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RAIS), n);


#ifdef FRAMER_DEBUG
                    printf("framer %d RAIS occurs\n", n);
#endif

                }
            }

            /* check YELLOW */
            if (alarms & SR2_RYEL) {
                if (ptr->yelAlarm != ALARM_ON) {
                    ptr->yelAlarm = ALARM_ON;
                    ledflag = FRAMER_LED_YELLOW;

                    /* sendAlarm( T1_COND_YELLOW,SERVICE ); */
                    SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RYEL), n);

#ifdef FRAMER_DEBUG
                    printf("framer %d in yellow ALARM\n", n);
#endif

                }
            }
        }

        /* Integrate LOS and LOF into Red Alarm */
        if (ptr->losAlarm || ptr->lofAlarm) {
            if (ptr->redTimer > 8) {    /*  250ms * 10 = 2.5sec  */
                if (ptr->redAlarm != ALARM_ON) {
                    ptr->redAlarm = ALARM_ON;

                    ledflag = FRAMER_LED_RED;
                    /* sendAlarm(RED_ALARM); */
                    SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RED), n);


#ifdef FRAMER_DEBUG
                    printf("framer %d in red ALARM\n", n);
#endif
                }
            }

            else
                ptr->redTimer++;
        }

        /* framer LED setting */

        if (ledflag != -1) {
#ifdef FRAMER_DEBUG
            printf("Framer[%d] LED to be changed [%d]\n", n, ledflag);
#endif

            if ((ptr->redAlarm == ALARM_ON) && (ledflag == FRAMER_LED_RED)) {
                /* red alarm case */
                framerLED(n, FRAMER_LED_RED);
            } else if (ptr->raisAlarm || ptr->yelAlarm) {
                /* yellow alarm case */
                framerLED(n, FRAMER_LED_YELLOW);
            } else {
                /* no error case */
                framerLED(n, FRAMER_LED_GREEN);
            }
        }



        /* Framer is in alarm */
        ptr->alarmFlag = TRUE;
        if (ptr->redAlarm || ptr->raisAlarm || ptr->yelAlarm) {

            /* Framer is out of service */
            if (ptr->status != FRAMER_OUT_OF_SERVICE) {

                SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LINKDOWN), n);

#if LINKUPDOWN_DEFINED
			/* send linkdown trap out*/
			if (n == 0){
			    snmpLinkDown.stsClockInputManualSelect	=	D_stsClockInputManualSelect_link1;
                TrapEventLog(stsLinkDown_TRAP, D_stsClockInputManualSelect_link1, D_stsCardShelf_main,
                    D_stsCardIndex_imu, D_stsClockInputManualSelect_link1, TRAP_MAJOR, &snmpLinkDown,
                    SIW_EVENT_SYM(FRAMER, LINKDOWN), 1);
           }
    		if (n == 1){
    		    snmpLinkDown.stsClockInputManualSelect	=	D_stsClockInputManualSelect_link2;
                   TrapEventLog(stsLinkDown_TRAP, D_stsClockInputManualSelect_link2, D_stsCardShelf_main,
                       D_stsCardIndex_imu, D_stsClockInputManualSelect_link2, TRAP_MAJOR, &snmpLinkDown,
                       SIW_EVENT_SYM(FRAMER, LINKDOWN), 2);
            }

#endif

#ifdef FRAMER_DEBUG
                printf
                    ("framer %d Alarm Occurs. LinkDown_Trap sent[pollmode]\n",
                     n);
#endif
            }
            ptr->status = FRAMER_OUT_OF_SERVICE;


            return ERROR;
        }

    } else {
        /* no any alarm case */

        if (ptr->redAlarm == ALARM_ON) {


            if (ptr->redTimer > 0)
                ptr->redTimer--;

            else {
                ptr->redAlarm = ALARM_OFF;
                ptr->redTimer = 0;
                ledflag = FRAMER_LED_GREEN;


                /*  sendAlarm(); */
                SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RED_CLEAR), n);
                /* checkitout
                   TrapEventLog(framerLosEvent_TRAP, D_stsCardShelf_main,  D_stsCardIndex_imu, n+1, 0, dumPtr, SIW_EVENT_SYM(FRAMER, RED_CLEAR), n+1);
                 */

#ifdef FRAMER_DEBUG
                printf("framer %d redAlarm clear\n", n);
#endif
            }
        }
        if (ptr->losAlarm == ALARM_ON) {
            ptr->losAlarm = ALARM_OFF;
            ledflag = FRAMER_LED_GREEN;


            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOS_CLEAR), n);
            snmpLinkAlarm.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
            snmpLinkAlarm.stsSystemId.length = sysConfig.stsSystemId.length;
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_los;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardIndex_imu, n + 1,
                         snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  LOS_CLEAR),
                         n + 1);

#ifdef FRAMER_DEBUG
            printf("framer %d LoS clear\n", n);
#endif

        }
        if (ptr->lofAlarm == ALARM_ON) {
            ptr->lofAlarm = ALARM_OFF;
            ledflag = FRAMER_LED_GREEN;


            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOF_CLEAR), n);

            snmpLinkAlarm.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
            snmpLinkAlarm.stsSystemId.length = sysConfig.stsSystemId.length;
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_lof;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardIndex_imu, n + 1,
                         snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  LOF_CLEAR),
                         n + 1);
#ifdef FRAMER_DEBUG
            printf("framer %d LoF clear\n", n);
#endif
        }
        if (ptr->raisAlarm == ALARM_ON) {
            ptr->raisAlarm = ALARM_OFF;
            ledflag = FRAMER_LED_GREEN;



            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RAIS_CLEAR), n);
            /*checkitout
               TrapEventLog(framerLosEvent_TRAP, D_stsCardShelf_main,  D_stsCardIndex_imu, n+1, 0, dumPtr, SIW_EVENT_SYM(FRAMER, RAIS_CLEAR), n+1);
             */
#ifdef FRAMER_DEBUG
            printf("framer %d RAIS clear\n", n);
#endif
        }
        if (ptr->yelAlarm == ALARM_ON) {

            ptr->yelAlarm = ALARM_OFF;
            ledflag = FRAMER_LED_GREEN;


            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RYEL_CLEAR), n);
            /*    checkitout
               TrapEventLog(framerLosEvent_TRAP, D_stsCardShelf_main,  D_stsCardIndex_imu, n+1, 0, dumPtr, SIW_EVENT_SYM(FRAMER, RYEL_CLEAR), n+1);
             */

#ifdef FRAMER_DEBUG
            printf("framer %d Yellow Alarm clear\n", n);
#endif
        }


        /* framer LED setting */
        if (ledflag == FRAMER_LED_GREEN) {
            if (ptr->redAlarm == ALARM_ON) {
                /* red alarm case */
                framerLED(n, FRAMER_LED_RED);
            } else if (ptr->raisAlarm || ptr->yelAlarm) {
                /* yellow alarm case */
                framerLED(n, FRAMER_LED_YELLOW);
            } else {
                /* no error case */
                framerLED(n, FRAMER_LED_GREEN);
            }
        }


        /* Framer is not in alarm */
        ptr->alarmFlag = FALSE;
        if (!ptr->redAlarm && !ptr->raisAlarm && !ptr->yelAlarm) {

            /* Framer is back to service */
            if (ptr->status != FRAMER_IN_SERVICE) {
                SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LINKUP), n);

#if LINKUPDOWN_DEFINED
			/* send linkup trap out*/
			if (n == 0){
				snmpLinkUp.stsClockInputManualSelect	=	D_stsClockInputManualSelect_link1;
                TrapEventLog(stsLinkUp_TRAP, D_stsClockInputManualSelect_link1, D_stsCardShelf_main,
                             D_stsCardIndex_imu, D_stsClockInputManualSelect_link1, TRAP_CLEAR, &snmpLinUp,
                             SIW_EVENT_SYM(FRAMER, LINKUP),1);
                }
			if (n == 1){
				snmpLinkUp.stsClockInputManualSelect	=	D_stsClockInputManualSelect_link2;
                TrapEventLog(stsLinkUp_TRAP, D_stsClockInputManualSelect_link2, D_stsCardShelf_main,
                    D_stsCardIndex_imu, D_stsClockInputManualSelect_link2, TRAP_CLEAR, &snmpLinkUp,
                    SIW_EVENT_SYM(FRAMER, LINKUP),2);
                }

#endif

                framerLED(n, FRAMER_LED_GREEN);

#ifdef FRAMER_DEBUG
                printf
                    ("framer %d ALARM all clear.LinkUp_Trap sent[pollmode]\n",
                     n);
#endif
            }
            ptr->status = FRAMER_IN_SERVICE;







        }
    }

#else                           /*  not polling mode */
    if ((ptr->losEvent || ptr->lofEvent || ptr->raiEvent
         || ptr->raisEvent)) {

        /*  LOS Alarm */
        if (ptr->losEvent) {
            if (ptr->losAlarm != ALARM_ON) {
                ptr->losAlarm = ALARM_ON;


                /* sendAlarm(T1_COND_LOS,SERVICE); */
                SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOS), n);
                snmpLinkAlarm.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
                snmpLinkAlarm.stsSystemId.length = sysConfig.stsSystemId.length;
                snmpLinkAlarm.stsLinkAlarmSettingType =
                    D_stsLinkAlarmSettingType_los;
                snmpLinkAlarm.stsLinkAlarmSevirity =
                    pmSeverity[n].los;
                TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                             snmpLinkAlarm.stsLinkAlarmSettingType,
                             D_stsCardShelf_main, D_stsCardIndex_imu,
                             n + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                             (U32_t *) & snmpLinkAlarm,
                             SIW_EVENT_SYM(FRAMER, LOS), n + 1);


#ifdef FRAMER_DEBUG
                printf("framer %d LOS occurs\n", n);
#endif

            }
        }
        if (te1Cfg[n].lineType != LT_SQW) {

            /*  LOF Alarm */
            if (ptr->lofEvent) {
                if (ptr->lofAlarm != ALARM_ON) {
                    ptr->lofAlarm = ALARM_ON;

                    /* sendAlarm( T1_COND_LOF,SERVICE ); */
                    SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOF), n);
                    snmpLinkAlarm.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
                    snmpLinkAlarm.stsSystemId.length = sysConfig.stsSystemId.length;
                    snmpLinkAlarm.stsLinkAlarmSettingType =
                        D_stsLinkAlarmSettingType_lof;
                    snmpLinkAlarm.stsLinkAlarmSevirity =
                        pmSeverity[n].lof;
                    TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                                 snmpLinkAlarm.stsLinkAlarmSettingType,
                                 D_stsCardShelf_main, D_stsCardIndex_imu,
                                 n + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                                 (U32_t *) & snmpLinkAlarm,
                                 SIW_EVENT_SYM(FRAMER, LOS), n + 1);

#ifdef FRAMER_DEBUG
                    printf("framer %d LOF occurs\n", n);
#endif


                }
            }

            /*  RAIS Alarm */
            if (ptr->raisEvent) {
                if (ptr->raisAlarm != ALARM_ON) {
                    ptr->raisAlarm = ALARM_ON;

                    ledflag = FRAMER_LED_YELLOW;

                    /* sendAlarm( T1_COND_RAIS,SERVICE ); */
                    SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RAIS), n);
                    /* checkitout
                       TrapEventLog(framerLosEvent_TRAP, D_stsCardShelf_main,  D_stsCardIndex_imu, n+1, 0, dumPtr, SIW_EVENT_SYM(FRAMER, RAIS), n+1);
                     */
#ifdef FRAMER_DEBUG
                    printf("framer %d RAIS occurs\n", n);
#endif


                }
            }

            /*  YELLOW Alarm */
            if (ptr->raiEvent) {
                if (ptr->yelAlarm != ALARM_ON) {
                    ptr->yelAlarm = ALARM_ON;
                    ledflag = FRAMER_LED_YELLOW;

                    /* sendAlarm( T1_COND_YELLOW,SERVICE ); */
                    SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RYEL), n);
                    /* checkitout
                       TrapEventLog(framerLosEvent_TRAP, D_stsCardShelf_main,  D_stsCardIndex_imu, n+1, 0, dumPtr, SIW_EVENT_SYM(FRAMER, RYEL), n+1);
                     */ D_stsCardIndex_imu
#ifdef FRAMER_DEBUG
                        printf("framer %d in yellow ALARM\n", n);
#endif


                }
            }
        }

        /* Integrate LOS and LOF into Red Alarm */
        if (ptr->losAlarm || ptr->lofAlarm) {
            if (ptr->redTimer > RED_ALARM_TIMER) {      /*  250ms * 10 = 2.5sec  */
                if (ptr->redAlarm != ALARM_ON) {
                    ptr->redAlarm = ALARM_ON;
                    ledflag = FRAMER_LED_RED;

                    /* sendAlarm(RED_ALARM); */
                    SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RED), n);
                    /* checktitout
                       TrapEventLog(framerLosEvent_TRAP, D_stsCardShelf_main,  D_stsCardIndex_imu, n+1, 0, dumPtr, SIW_EVENT_SYM(FRAMER, RED), n+1);
                     */
#ifdef FRAMER_DEBUG
                    printf("framer %d in red ALARM\n", n);
#endif
                }
            }

            else
                ptr->redTimer++;
        }

        /* framer LED setting */
        if (ledflag != -1) {
            if (ptr->redAlarm == ALARM_ON) {
                /* red alarm case */
                framerLED(n, FRAMER_LED_RED);
            } else if (ptr->raisAlarm || ptr->yelAlarm) {
                /* yellow alarm case */
                framerLED(n, FRAMER_LED_YELLOW);
            } else {
                /* no error case */
                framerLED(n, FRAMER_LED_GREEN);
            }
        }


        /* Framer is in alarm */
        ptr->alarmFlag = TRUE;
        if (ptr->redAlarm || ptr->raisAlarm || ptr->yelAlarm) {

            /* Framer is out of service */
            if (ptr->status != FRAMER_OUT_OF_SERVICE) {

                SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LINKDOWN), n);

#if LINKUPDOWN_DEFINED
			/* send linkdown trap out*/
			if (n == 0){
				snmpLinkDown.stsClockInputManualSelect	=	D_stsClockInputManualSelect_link1;
                TrapEventLog(stsLinkDown_TRAP,D_stsClockInputManualSelect_link1, D_stsCardShelf_main,
                     D_stsCardIndex_imu, D_stsClockInputManualSelect_link1, TRAP_MAJOR, &snmpLinkDown,
                     SIW_EVENT_SYM(FRAMER, LINKDOWN), 1);
                }
			if (n == 1){
				snmpLinkDown.stsClockInputManualSelect	=	D_stsClockInputManualSelect_link2;
                TrapEventLog(stsLinkDown_TRAP, D_stsClockInputManualSelect_link2, D_stsCardShelf_main,
                      D_stsCardIndex_imu, D_stsClockInputManualSelect_link2, TRAP_MAJOR, &snmpLinkDown,
                      SIW_EVENT_SYM(FRAMER, LINKDOWN), 2);
                }
#endif

#ifdef FRAMER_DEBUG
                printf
                    ("framer %d ALARM occurs. and LinkDown_Trap sent[interrup mode]\n",
                     n);
#endif

            }
            ptr->status = FRAMER_OUT_OF_SERVICE;


            return ERROR;






        }
    }

    else {
        if (ptr->redAlarm == ALARM_ON) {
            ledflag = FRAMER_LED_GREEN;
            if (ptr->redTimer > 0) {
                ptr->redTimer--;
                ledflag = FRAMER_LED_RED;


            } else {
                ptr->redAlarm = ALARM_OFF;
                ptr->redTimer = 0;


                /*  sendAlarm(); */
                SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RED_CLEAR), n);


                /* checkitout
                   snmpLinkAlarm.stsLinkAlarmSettingType        =       D_stsLinkAlarmSettingType_??;
                   snmpLinkAlarm.stsLinkAlarmSevirity           =       D_stsLinkAlarmSevirity_cl;
                   TrapEventLog(stsLinkAlarmStatusChange_TRAP, D_stsCardShelf_main,  D_stsCardIndex_imu, n+1, 0, (U32_t *)&snmpLinkAlarm, SIW_EVENT_SYM(FRAMER, RED_CLEAR), n+1);
                 */

#ifdef FRAMER_DEBUG
                printf("framer %d redAlarm clear\n", n);
#endif



            }
        }
        if (!ptr->losEvent && (ptr->losAlarm == ALARM_ON)) {
            ptr->losAlarm = ALARM_OFF;


            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOS_CLEAR), n);
            snmpLinkAlarm.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
            snmpLinkAlarm.stsSystemId.length = sysConfig.stsSystemId.length;
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_los;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardIndex_imu, n + 1,
                         snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  LOS_CLEAR),
                         n + 1);

#ifdef FRAMER_DEBUG
            printf("framer %d LoS clear\n", n);
#endif

        }
        if ((!ptr->lofEvent) && (ptr->lofAlarm == ALARM_ON)) {
            ptr->lofAlarm = ALARM_OFF;
            statusChangeFlag = 1;

            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOF_CLEAR), n);
            snmpLinkAlarm.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
            snmpLinkAlarm.stsSystemId.length = sysConfig.stsSystemId.length;
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_lof;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardIndex_imu, n + 1,
                         snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  LOF_CLEAR),
                         n + 1);
#ifdef FRAMER_DEBUG
            printf("framer %d LoF clear\n", n);
#endif


        }
        if (!ptr->raisEvent && (ptr->raisAlarm == ALARM_ON)) {
            ptr->raisAlarm = ALARM_OFF;
            ledflag = FRAMER_LED_GREEN;
            statusChangeFlag = 1;

            /*  sendAlarm(); */

#ifdef FRAMER_DEBUG
            printf("framer %d RAIS clear\n", n);
#endif
        }
        if (!ptr->yelEvent && (ptr->yelAlarm == ALARM_ON)) {
            ptr->yelAlarm = ALARM_OFF;
            ledflag = FRAMER_LED_GREEN;

            /*  sendAlarm(); */
            /*checkitout
               SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RAIS_CLEAR), n);
               TrapEventLog(framerLosEvent_TRAP, D_stsCardShelf_main,  D_stsCardIndex_imu, n+1, 0, dumPtr, SIW_EVENT_SYM(FRAMER, RAIS_CLEAR), n+1);
             */
#ifdef FRAMER_DEBUG
            printf("framer %d YellowAlarm clear\n", n);
#endif
        }

        if (ptr->yelAlarm == ALARM_ON) {
            ptr->yelAlarm = ALARM_OFF;
            ledflag = FRAMER_LED_GREEN;


            /*  sendAlarm(); */
            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, RYEL_CLEAR), n);
            /* checkitout
               TrapEventLog(framerLosEvent_TRAP, D_stsCardShelf_main,  D_stsCardIndex_imu, n+1, 0, dumPtr, SIW_EVENT_SYM(FRAMER, RYEL_CLEAR), n+1);
             */

#ifdef FRAMER_DEBUG
            printf("framer %d Yellow Alarm clear\n", n);
#endif
        }

        /* framer LED setting */
        if (ledflag != -1) {
            if (ptr->redAlarm == ALARM_ON) {
                /* red alarm case */
                framerLED(n, FRAMER_LED_RED);
            } else if (ptr->raisAlarm || ptr->yelAlarm) {
                /* yellow alarm case */
                framerLED(n, FRAMER_LED_YELLOW);
            } else {
                /* no error case */
                framerLED(n, FRAMER_LED_GREEN);
            }
        }

        /* Framer is not in alarm */
        ptr->alarmFlag = FALSE;
        if (!ptr->redAlarm && !ptr->raisAlarm && !ptr->yelAlarm) {

            if (ptr->status != FRAMER_IN_SERVICE) {
                SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LINKUP), n);

#if LINKUPDOWN_DEFINED
		/* send linkup trap out*/
		if (n == 0){
			snmpLinkUp.stsClockInputManualSelect	=	D_stsClockInputManualSelect_link1;
            TrapEventLog(stsLinkUp_TRAP, D_stsClockInputManualSelect_link1, D_stsCardShelf_main,
                D_stsCardIndex_imu, D_stsClockInputManualSelect_link1, TRAP_CLEAR, &snmpLinkUp,
                SIW_EVENT_SYM(FRAMER, LINKUP),1);
        }
		if (n == 1){
			snmpLinkUp.stsClockInputManualSelect	=	D_stsClockInputManualSelect_link2;
            TrapEventLog(stsLinkUp_TRAP, D_stsClockInputManualSelect_link1, D_stsCardShelf_main,
                 D_stsCardIndex_imu, D_stsClockInputManualSelect_link2, TRAP_CLEAR, &snmpLinkUp,
                 SIW_EVENT_SYM(FRAMER, LINKUP), 2);
       }

#endif

#ifdef FRAMER_DEBUG
                printf
                    ("framer %d ALARM all clear and LinkUp_Trap sent [interrup mode]\n",
                     n);
#endif
            }

            /* Framer is in service */
            ptr->status = FRAMER_IN_SERVICE;


        }
    }

#endif                          /*  */
    /* read Bipolar violation from FPGA */
    /* tmp = READ_FGPA_BV; *//*shuming. ask david for this, mark out this line temporarily */
    ptr->bvCountMan += tmp;
    ptr->bvCountDef += tmp;

    /* clear Bipolar violation in FPGA */
    /* CLEAR_FPGA_BV;  *//* this line is marked out by shuming temp */
    if ((te1Cfg[n].framing == FF_E1_CRC) || (te1Cfg[n].framing == FF_E1_CRC_MF) || (te1Cfg[n].framing == FF_T1_ESF)) {  /*shuming modified */

        /* get CRC4 error count */
        tmp =
            (((ptr->reg->info3 >> 4) & 0x0f) << 2) +
            ((ptr->reg->info3 >> 3) & 0x01);
        ptr->crcCountMan += tmp;
        ptr->crcCountDef += tmp;

        /* Clear CRC4 count */
        ptr->reg->e1rcr &= ~(1 << 3);
    }
    return OK;
}
/**************************************************************************************
* function: alarmIntInit
*  input parameter
*  return
*			
***************************************************************************************/
static STATUS_t alarmIntInit(int n)
{
    INP_FRAMER_t *ptr = &inpFramer[n];
    VOIDFUNCPTR framer_interrupt = (VOIDFUNCPTR) FRAMER_INT_VECTOR;
    intConnect(&framer_interrupt, alarmIntHandler, 0);
    printf("in alarmIntInit set int mask\n");
    ptr->reg->imr2 |=
        IMR2_RLOS | IMR2_RLOSC | IMR2_RLOFC | IMR2_RYELC | IMR2_RLOF |
        IMR2_RYEL | IMR2_RAIS | IMR2_RAISC;
    return OK;
}
/**************************************************************************************
* function: alarmIntHandler
*  input parameter
*  return
*			void
***************************************************************************************/
void alarmIntHandler(void)
{
    U8_t intStatus;
    int n;

    /* read interrupt status byte from FPGA */
    intStatus = (*(U8_t *) (0x40000013));       /*shuming pls chk */
    for (n = 0; n < MAX_FRAMERS; n++) {
        if (intStatus & (1 << n)) {
            processAlarmInt(n);
        }
    }
}

/**************************************************************************************
* function: processAlarmInt
*  input parameter
*    	int n: which framer
*  return
*			void
***************************************************************************************/

static void processAlarmInt(int n)
{
    INP_FRAMER_t *ptr = &inpFramer[n];
    U8_t iir;
    U8_t status;
    U8_t SR2 = 0x02;
    U8_t alarm;

    /* get interrupt info */
    iir = (ptr->reg->iir & 0xf);

    /* check if it is Alarm interrupt */
    if (iir & SR2) {
        status = readStatusReg(n, SR2_REG, ptr->reg->imr2);

        /* Loss of Frame Alarm */
        if (status & SR2_RLOF) {
            if (pollAlarm(n, TMS_RXOOF, &alarm) != OK)
                return;
            if (alarm) {
                ptr->lofEvent = TRUE;
                ptr->reg->imr2 &= ~(IMR2_RLOF);
            }
        }

        /* Loss of Signal Alarm */
        if (status & SR2_RLOS) {
            if (pollAlarm(n, TMS_RXLOS, &alarm) != OK)
                return;
            if (alarm) {
                ptr->losEvent = TRUE;
                ptr->reg->imr2 &= ~(IMR2_RLOS);

                /* Add necessary actions below */
            }
        }

        /* Yellow Alarm */
        if (status & SR2_RYEL) {
            if (pollAlarm(n, TMS_RXRAI, &alarm) != OK)
                return;
            if (alarm) {
                ptr->raiEvent = TRUE;
                ptr->reg->imr2 &= ~(IMR2_RYEL);
            }
        }

        /* RAIS Alarm */
        if (status & SR2_RAIS) {
            if (pollAlarm(n, TMS_RXAIS, &alarm) != OK)
                return;
            if (alarm) {
                ptr->raisEvent = TRUE;
                ptr->reg->imr2 &= ~(IMR2_RAIS);
            }
        }

        /* Loss of Frame Alarm is Clear */
        if (status & SR2_RLOFC) {
            ptr->lofEvent = FALSE;
            ptr->reg->imr2 |= (IMR2_RLOF);
        }

        /* Loss of Signal Alarm is Clear */
        if (status & SR2_RLOSC) {
            ptr->lofEvent = FALSE;
            ptr->reg->imr2 |= (IMR2_RLOS);
        }

        /* Yellow Alarm is Clear */
        if (status & SR2_RYELC) {
            ptr->raiEvent = FALSE;
            ptr->reg->imr2 |= (IMR2_RYEL);
        }

        /* RAIS Alarm is Clear */
        if (status & SR2_RAISC) {
            ptr->raisEvent = FALSE;
            ptr->reg->imr2 |= (IMR2_RAIS);
        }
    }
}
/**************************************************************************************
* function: readStatusReg
*  input parameter
*    	int n: which framer
*			WR_REG_INDEX_t reg	which status register
*			U8_t bits:	which bits
*  return
*			the readout value
***************************************************************************************/
static U8_t readStatusReg(int n, WR_REG_INDEX_t reg, U8_t bits)
{
    VU8_t *cptr = NULL;
    VU8_t value;
    INP_FRAMER_t *ptr = &inpFramer[n];
    switch (reg) {
    case SR1_REG:
        cptr = &ptr->reg->sr1;
        break;
    case SR2_REG:
        cptr = &ptr->reg->sr2;
        break;
    case SR3_REG:
        cptr = &ptr->reg->sr3;
        break;
    case SR4_REG:
        cptr = &ptr->reg->sr4;
        break;
    case INFO1_REG:
        cptr = &ptr->reg->info1;
        break;
    case INFO2_REG:
        cptr = &ptr->reg->info2;
        break;
    default:
        return (0);
    }
    *cptr = bits;
    value = *cptr & bits;
    return (value);
}


/**************************************************************************************
* function: STATUS_t pollAlarm
*  input parameter
*    	int n: which framer
*			int alarm_type: alarm type
*			U8_t * valueptr: the value
*  return
*    OK: if alarmtype is legal and alarm is sent
*    ERROR: otherwise
***************************************************************************************/
static STATUS_t pollAlarm(int n, int alarm_type, U8_t * valueptr)
{
    INP_FRAMER_t *ptr = &inpFramer[n];
    switch (alarm_type) {
    case TMS_RXLOS:
        *valueptr = readStatusReg(n, SR2_REG, SR2_RLOS) ? 1 : 0;
        break;
    case TMS_RXOOF:
        if (te1Cfg[n].lineType == LT_E1) {
            U8_t oof, status;
            oof = 0;
            status = ptr->reg->info3;
            if (status & INFO3_FASSA)
                oof = 1;
            if ((te1Cfg[n].framing == FF_E1_CRC
                 || te1Cfg[n].framing == FF_E1_CRC_MF)
                && (status & INFO3_CRC4SA))
                oof = 1;
            if ((te1Cfg[n].framing == FF_E1_MF
                 || te1Cfg[n].framing == FF_E1_CRC_MF)
                && (status & INFO3_CASSA))
                oof = 1;
            *valueptr = oof;
        }

        else if ((te1Cfg[n].lineType == LT_T1) ||
                 (te1Cfg[n].lineType == LT_J1))
            *valueptr = readStatusReg(n, SR2_REG, SR2_RLOF) ? 1 : 0;
        break;
    case TMS_RXAIS:
        *valueptr = readStatusReg(n, SR2_REG, SR2_RAIS) ? 1 : 0;
        break;
    case TMS_RXRAI:
        if (te1Cfg[n].lineType == LT_E1)
            *valueptr = (ptr->reg->rnaf & RNAF_A) ? 1 : 0;

        else if ((te1Cfg[n].lineType == LT_T1) ||
                 (te1Cfg[n].lineType == LT_J1))
            *valueptr = readStatusReg(n, SR2_REG, SR2_RYEL) ? 1 : 0;
        break;
    default:
        return ERROR;
    }
    return OK;
}


/**************************************************************************************
* function: static STATUS_t framerLED(int n, char color)
*  input parameter
*    int n: which framer associated LED to be configured
*    char color: the color, yellow, red, green or off to be set
*  return
*    OK: if alarmtype is legal and alarm is sent
*    ERROR: otherwise
***************************************************************************************/

static STATUS_t framerLED(int n, int color)
{
    switch (n) {
    case 0:
        switch (color) {
        case FRAMER_LED_YELLOW:
            ledOp(LED_IP_1_ALARM, LED_ON, 0);
            ledOp(LED_IP_1_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_RED:
            ledOp(LED_IP_1_ALARM, LED_ON, 0);
            ledOp(LED_IP_1_PRESENT, LED_OFF, 0);
            break;
        case FRAMER_LED_GREEN:
            ledOp(LED_IP_1_ALARM, LED_OFF, 0);
            ledOp(LED_IP_1_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_OFF:
            ledOp(LED_IP_1_ALARM, LED_OFF, 0);
            ledOp(LED_IP_1_PRESENT, LED_OFF, 0);
            break;
        default:
            return ERROR;
        }
        break;

    case 1:
        switch (color) {
        case FRAMER_LED_YELLOW:
            ledOp(LED_IP_2_ALARM, LED_ON, 0);
            ledOp(LED_IP_2_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_RED:
            ledOp(LED_IP_2_ALARM, LED_ON, 0);
            ledOp(LED_IP_2_PRESENT, LED_OFF, 0);
            break;
        case FRAMER_LED_GREEN:
            ledOp(LED_IP_2_ALARM, LED_OFF, 0);
            ledOp(LED_IP_2_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_OFF:
            ledOp(LED_IP_2_ALARM, LED_OFF, 0);
            ledOp(LED_IP_2_PRESENT, LED_OFF, 0);
            break;
        default:
            return ERROR;
        }
        break;
    case 2:
        switch (color) {
        case FRAMER_LED_YELLOW:
            ledOp(LED_IP_3_ALARM, LED_ON, 0);
            ledOp(LED_IP_3_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_RED:
            ledOp(LED_IP_3_ALARM, LED_ON, 0);
            ledOp(LED_IP_3_PRESENT, LED_OFF, 0);
            break;
        case FRAMER_LED_GREEN:
            ledOp(LED_IP_3_ALARM, LED_OFF, 0);
            ledOp(LED_IP_3_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_OFF:
            ledOp(LED_IP_3_ALARM, LED_OFF, 0);
            ledOp(LED_IP_3_PRESENT, LED_OFF, 0);
            break;
        default:
            return ERROR;
        }
        break;
    case 3:
        switch (color) {
        case FRAMER_LED_YELLOW:
            ledOp(LED_IP_4_ALARM, LED_ON, 0);
            ledOp(LED_IP_4_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_RED:
            ledOp(LED_IP_4_ALARM, LED_ON, 0);
            ledOp(LED_IP_4_PRESENT, LED_OFF, 0);
            break;
        case FRAMER_LED_GREEN:
            ledOp(LED_IP_4_ALARM, LED_OFF, 0);
            ledOp(LED_IP_4_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_OFF:
            ledOp(LED_IP_4_ALARM, LED_OFF, 0);
            ledOp(LED_IP_4_PRESENT, LED_OFF, 0);
            break;
        default:
            return ERROR;
        }
        break;

    case 4:
        switch (color) {
        case FRAMER_LED_YELLOW:
            ledOp(LED_IP_5_ALARM, LED_ON, 0);
            ledOp(LED_IP_5_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_RED:
            ledOp(LED_IP_5_ALARM, LED_ON, 0);
            ledOp(LED_IP_5_PRESENT, LED_OFF, 0);
            break;
        case FRAMER_LED_GREEN:
            ledOp(LED_IP_5_ALARM, LED_OFF, 0);
            ledOp(LED_IP_5_PRESENT, LED_ON, 0);
            break;
        case FRAMER_LED_OFF:
            ledOp(LED_IP_5_ALARM, LED_OFF, 0);
            ledOp(LED_IP_5_PRESENT, LED_OFF, 0);
            break;
        default:
            return ERROR;
        }
        break;


    default:
        return ERROR;
    }

    return OK;



}

/**************************************************************************************
* function: static STATUS_t framerDataAccess
*  input parameter
*			char *fname - file name storing user value.
*			int mode - IO mode (R/W)
*			int dataType - 
*    
*  return
*    OK: 
*    ERROR: otherwise
***************************************************************************************/

STATUS_t framerDataAccess(char *fname, int mode, int dataType)
{
    FILE *fhd = NULL;
    int slot, port, i;
    STATUS_t status = OK;
    static U8_t malloc4linkIdYet=0;
    
    U8_t tmp[29]; /* 4+24(FRAMER_LINKID_LEN)+1(id length) */
    if (fname == NULL) {
        printf("framerDataAccess: Invalid file name\n");
        return ERROR;
    }
    memset(tmp, 0, sizeof(tmp));
    if (mode == FLASH_READ) {
        if ((fhd = fopen(fname, "r")) == NULL)
            status = ERROR;

        else {
            switch (dataType) {

            case TEL_CFG_DATA:
            	
 
              /* read out link link config */
               for (i=0; i<MAX_FRAMERS; i++){
                  if (fread((char *) &tmp, sizeof(char), 30, fhd) != 30) {
					status = ERROR;
                  }else {
					/* readout linkId */
                  		te1Cfg[i].linkId.length = tmp[0];
                  		if (malloc4linkIdYet ==0){
                  			te1Cfg[i].linkId.octet_ptr = malloc(FRAMER_LINKID_LEN);
                  			malloc4linkIdYet++;
                  		}
                  		memcpy(te1Cfg[i].linkId.octet_ptr, tmp+1, tmp[0]);
                  		
                  	/* read out other configuration */                  		
                  		te1Cfg[i].lineType 		= tmp[25];
                  		te1Cfg[i].framing 		= tmp[26];
                  		te1Cfg[i].coding 		= tmp[27];
                  		te1Cfg[i].termination 	= tmp[28];
                  		te1Cfg[i].linkEnable 	= tmp[29];
                  		
                  }    
                }
               fclose(fhd);
               break;
            case RECEIVED_SSM:
               for (i=0; i<MAX_FRAMERS; i++){
                  if (fread((char *) &tmp, sizeof(char), 6, fhd) != 6) {
                    status = ERROR;
                  }else {
                      receivedSSM[i][0].ssm = tmp[0];
                      receivedSSM[i][1].ssm = tmp[1];
                      receivedSSM[i][2].ssm = tmp[2];
                      receivedSSM[i][3].ssm = tmp[3];
                      receivedSSM[i][4].ssm = tmp[4];
                      receivedSSM[i][5].ssm = tmp[5];
                  }    
                }
                fclose(fhd);
                break;
             default:
                break; 
            }/* switch (dataType) */
        }     
    }
    else if (mode == FLASH_WRITE)
    {
       if ((fhd = fopen(fname, "w")) == NULL)
	       status = ERROR;
	   else {             
           	   
	       switch (dataType) {

            case TEL_CFG_DATA:
               for (i=0; i<MAX_FRAMERS; i++){
               		tmp[0] = 	te1Cfg[i].linkId.length;
                  memcpy(tmp+1,te1Cfg[i].linkId.octet_ptr, tmp[0]);               
                  tmp[25] = te1Cfg[i].lineType;
                  tmp[26] = te1Cfg[i].framing;
                  tmp[27] = te1Cfg[i].coding;
                  tmp[28] = te1Cfg[i].termination;
                  tmp[29] = te1Cfg[i].linkEnable;      
                  fwrite((char *) &tmp, sizeof(char), 30, fhd);      
               }
               fclose(fhd);
               break;
            case RECEIVED_SSM:
               for (i=0; i<MAX_FRAMERS; i++){
                  tmp[0] = receivedSSM[i][0].ssm;
                  tmp[1] = receivedSSM[i][1].ssm;
                  tmp[2] = receivedSSM[i][2].ssm;
                  tmp[3] = receivedSSM[i][3].ssm;
                  tmp[4] = receivedSSM[i][4].ssm;
                  tmp[5] = receivedSSM[i][5].ssm;
                  fwrite((char *) &tmp, sizeof(char), 6, fhd);           
               }
               fclose(fhd);
               break;
            default:
               fclose(fhd);
               break;   
           }/* end of switch (dataType) */         
       }/* end of "else" */
          	  
    }/* end of "else if (mode == FLASH_WRITE)" */ 
    return status;   
}  
/**************************************************************************************
* function: static STATUS_t framerSeverityCheck(int n)
*  Description: this is to summarize the framer alarm status as whole
*  input parameter
*    int n: which framer to be processed. 
*    
*  return
*    OK: if alarmtype is legal and alarm is sent
*    ERROR: otherwise
***************************************************************************************/
STATUS_t framerSeverityCheck(int n)
{
    INP_FRAMER_t *ptr = &inpFramer[n];
    U32_t alarmSeverityTmp = D_stsLinkAlarmSevirity_cl;



    if (ptr->actAlarm == ALARM_ON)
        alarmSeverityTmp = 
        	(alarmSeverityTmp < pmSeverity[n].act) ? alarmSeverityTmp : pmSeverity[n].act;
    if (ptr->losAlarm == ALARM_ON)
        alarmSeverityTmp =
        	(alarmSeverityTmp < pmSeverity[n].los) ? alarmSeverityTmp : pmSeverity[n].los;
    if (ptr->aispAlarm == ALARM_ON)
        alarmSeverityTmp =
          (alarmSeverityTmp < pmSeverity[n].aisp) ? alarmSeverityTmp : pmSeverity[n].aisp;
    if (ptr->lofAlarm == ALARM_ON)
        alarmSeverityTmp =
        	(alarmSeverityTmp < pmSeverity[n].lof) ? alarmSeverityTmp : pmSeverity[n].lof;
    if (ptr->cvlAlarm == ALARM_ON)
        alarmSeverityTmp =
        	(alarmSeverityTmp < pmSeverity[n].cvl) ? alarmSeverityTmp : pmSeverity[n].cvl;
    if (ptr->cvpAlarm == ALARM_ON)
        alarmSeverityTmp =
        	(alarmSeverityTmp < pmSeverity[n].cvp) ? alarmSeverityTmp : pmSeverity[n].cvp;
    if (ptr->csAlarm == ALARM_ON)
        alarmSeverityTmp =
        	(alarmSeverityTmp < pmSeverity[n].cs) ? alarmSeverityTmp : pmSeverity[n].cs;
    if (ptr->jitterAlarm == ALARM_ON)
        alarmSeverityTmp =
        	(alarmSeverityTmp <  pmSeverity[n].jitter) ? alarmSeverityTmp : pmSeverity[n].jitter;
    if (ptr->tieAlarm == ALARM_ON)
        alarmSeverityTmp =
        	(alarmSeverityTmp < pmSeverity[n].tie) ? alarmSeverityTmp : pmSeverity[n].tie;
    if (ptr->mtieAlarm == ALARM_ON)
        alarmSeverityTmp =
        	(alarmSeverityTmp < pmSeverity[n].mtie) ? alarmSeverityTmp : pmSeverity[n].mtie;


    ptr->alarmSeverity = alarmSeverityTmp;

    return OK;


}



/**************************************************************************************
* function: STATUS_T setInpFramerAlarm(TE1_ALARM_t alarmtype, TE1_ALARMACTION_t action, int ch)
*  input parameter
*    TE1_ALARM_t alarmtype: alarm type
*    TE1_ALARMACTION_t action: set or clean
*		 ch: which framer
*  return
*    OK: if alarmtype is legal and alarm is sent
*    ERROR: otherwise
***************************************************************************************/

STATUS_t setInpFramerAlarm(TE1_ALARM_t alarmtype, TE1_ALARMACTION_t action,
                           int ch)
{
    INP_FRAMER_t *ptr = &inpFramer[ch];
    U32_t *dumPtr = NULL;

    stsLinkAlarmStatusChange_t snmpLinkAlarm;
    snmpLinkAlarm.stsLinkAlarmSettingLinkType = ch + 1; /* system 0-based, snmp 1-based */
    snmpLinkAlarm.stsSystemId.octet_ptr = sysConfig.stsSystemId.octet_ptr;
    snmpLinkAlarm.stsSystemId.length = sysConfig.stsSystemId.length;
    if (action == TE1_ALARM_SET) {

        switch (action) {
        case TE1_ALARM_ACT:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, ACT), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_act;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].act;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  ACT),
                         ch + 1);
            ptr->actAlarm = ALARM_ON;

            break;

        case TE1_ALARM_LOS:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOS), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_los;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].los;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  LOS),
                         ch + 1);
            ptr->losAlarm = ALARM_ON;


            break;

        case TE1_ALARM_AISP:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, AISP), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_aisp;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].aisp;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  AISP),
                         ch + 1);
            ptr->aispAlarm = ALARM_ON;

            break;

        case TE1_ALARM_LOF:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOF), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_lof;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].lof;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  LOF),
                         ch + 1);
            ptr->lofAlarm = ALARM_ON;

            break;

        case TE1_ALARM_CVL:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, CVL), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_cvl;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].cvl;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  CVL),
                         ch + 1);
            ptr->cvlAlarm = ALARM_ON;

            break;

        case TE1_ALARM_CVP:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, CVP), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_cvp;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].cvp;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  CVP),
                         ch + 1);
            ptr->cvpAlarm = ALARM_ON;

            break;

        case TE1_ALARM_CS:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, CS), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_cs;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].cs;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  CS),
                         ch + 1);
            ptr->csAlarm = ALARM_ON;

            break;

        case TE1_ALARM_JITTER:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, JITTER), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_jitter;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].jitter;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  JITTER),
                         ch + 1);
            ptr->jitterAlarm = ALARM_ON;

            break;

        case TE1_ALARM_TIE:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, TIE), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_tie;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].tie;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  TIE),
                         ch + 1);
            ptr->tieAlarm = ALARM_ON;

            break;

        case TE1_ALARM_MTIE:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, MTIE), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_mtie;
            snmpLinkAlarm.stsLinkAlarmSevirity =
                pmSeverity[ch].mtie;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  MTIE),
                         ch + 1);
            ptr->mtieAlarm = ALARM_ON;

            break;

        default:
            return ERROR;

        }



    }


    if (action == TE1_ALARM_CLEAR) {

        switch (action) {
        case TE1_ALARM_ACT:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, ACT_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_act;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  ACT_CLEAR),
                         ch + 1);
            ptr->actAlarm = ALARM_OFF;

            break;

        case TE1_ALARM_LOS:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOS_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_los;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  LOS_CLEAR),
                         ch + 1);
            ptr->losAlarm = ALARM_OFF;

            break;

        case TE1_ALARM_AISP:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, AISP_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_aisp;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  AISP_CLEAR),
                         ch + 1);
            ptr->aispAlarm = ALARM_OFF;

            break;

        case TE1_ALARM_LOF:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, LOF_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_lof;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;
            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  LOF_CLEAR),
                         ch + 1);
            ptr->lofAlarm = ALARM_OFF;

            break;


        case TE1_ALARM_CVL:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, CVL_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_cvl;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;

            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  CVL_CLEAR),
                         ch + 1);
            ptr->cvlAlarm = ALARM_OFF;

            break;

        case TE1_ALARM_CVP:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, CVP_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_cvp;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;

            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  CVP_CLEAR),
                         ch + 1);
            ptr->cvpAlarm = ALARM_OFF;

            break;

        case TE1_ALARM_CS:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, CS_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_cs;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;

            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  CS_CLEAR),
                         ch + 1);
            ptr->csAlarm = ALARM_OFF;

            break;

        case TE1_ALARM_JITTER:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, JITTER_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_jitter;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;

            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  JITTER_CLEAR),
                         ch + 1);
            ptr->jitterAlarm = ALARM_OFF;

            break;

        case TE1_ALARM_TIE:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, TIE_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_tie;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;

            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  TIE_CLEAR),
                         ch + 1);
            ptr->tieAlarm = ALARM_OFF;

            break;

        case TE1_ALARM_MTIE:

            SiwEventLog(NULL, SIW_EVENT_SYM(FRAMER, MTIE_CLEAR), ch);
            snmpLinkAlarm.stsLinkAlarmSettingType =
                D_stsLinkAlarmSettingType_mtie;
            snmpLinkAlarm.stsLinkAlarmSevirity = D_stsLinkAlarmSevirity_cl;

            TrapEventLog(stsLinkAlarmStatusChange_TRAP,
                         snmpLinkAlarm.stsLinkAlarmSettingType,
                         D_stsCardShelf_main, D_stsCardType_cardTypeSmu,
                         ch + 1, snmpLinkAlarm.stsLinkAlarmSevirity,
                         (U32_t *) & snmpLinkAlarm, SIW_EVENT_SYM(FRAMER,
                                                                  MTIE_CLEAR),
                         ch + 1);
            ptr->mtieAlarm = ALARM_OFF;
            break;

        default:
            return ERROR;

        }



    }
    framerSeverityCheck(ch);

    return OK;


}







/****************************************************************************
				debug routine Section
*****************************************************************************/

#ifdef FRAMER_DEBUG

/* ------------------------------- testing begin - routine for testing */

STATUS_t framerDebugLedTest(void)
{
    int whichframer, color;
    for (color = 0; color < 4; color++) {
        for (whichframer = 0; whichframer < 5; whichframer++) {
            framerLED(whichframer, color);
            taskDelay(100);

        }

    }
}











STATUS_t framerDebugShow(void)
{
    int nFramer2show = 2;
    int n, i, j, k;
    INP_FRAMER_t *ptr;
    printf("----inpframer addr info\n");
    for (n = 0; n < nFramer2show; n++) {
        ptr = &inpFramer[n];
        printf("framer[%d] addr: %x / %x\n", n, inpFramerBaseAddr[n],
               (int) ptr->reg);
    } printf("\n");
    for (i = 0; i < nFramer2show; i++) {
        getInpFramerSSM(i);
    }
    for (i = 0; i < nFramer2show; i++) {
        if (te1Cfg[i].lineType == LT_E1) {
            printf("----SSM info E1\n");
            for (j = 0; j < 5; j++) {
                printf("SSM[framer %d][channel %d] status:%x\n", i, j,
                       receivedSSM[i][j].status);
                printf("SSM[framer %d][channel %d] ssm:%x\n", i, j,
                       receivedSSM[i][j].ssm);
                printf("SSM[framer %d][channel %d] alarm:%x\n", i, j,
                       receivedSSM[i][j].alarm);
                printf("SSM[framer %d][channel %d] count:%x\n", i, j,
                       receivedSSM[i][j].count);
            }
        }
        if (te1Cfg[i].lineType == LT_T1) {
            printf("----SSM info T1\n");
            for (j = 0; j < 1; j++) {
                printf("SSM[framer %d][channel %d] status:%x\n", i, j,
                       receivedSSM[i][j].status);
                printf("SSM[framer %d][channel %d] ssm:%x\n", i, j,
                       receivedSSM[i][j].ssm);
                printf("SSM[framer %d][channel %d] alarm:%x\n", i, j,
                       receivedSSM[i][j].alarm);
                printf("SSM[framer %d][channel %d] count:%x\n", i, j,
                       receivedSSM[i][j].count);
            }
        }
    }
    printf("----line config info\n");
    for (i = 0; i < nFramer2show; i++) {
        printf("line config [framer %d] line type =%d\n", i,
               te1Cfg[i].lineType);
        printf("line config [framer %d] framing   =%d\n", i,
               te1Cfg[i].framing);
        printf("line config [framer %d] coding    =%d\n", i,
               te1Cfg[i].coding);
        printf("line config [framer %d] termin    =%d\n", i,
               te1Cfg[i].termination);
    }
}
STATUS_t framerDebugModeSet(int testcase)
{
    int n;
    switch (testcase) {

    case 101:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_T1;
            te1Cfg[n].framing = FF_T1_D4;
            te1Cfg[n].coding = LC_AMI;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 102:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_T1;
            te1Cfg[n].framing = FF_T1_D4;
            te1Cfg[n].coding = LC_B8ZS;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 103:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_T1;
            te1Cfg[n].framing = FF_T1_ESF;
            te1Cfg[n].coding = LC_AMI;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 104:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_T1;
            te1Cfg[n].framing = FF_T1_ESF;
            te1Cfg[n].coding = LC_B8ZS;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 201:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1_CRC;
            te1Cfg[n].coding = LC_AMI;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 202:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1_CRC;
            te1Cfg[n].coding = LC_HDB3;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 203:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1;
            te1Cfg[n].coding = LC_AMI;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 204:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1;
            te1Cfg[n].coding = LC_HDB3;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 205:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1_MF;
            te1Cfg[n].coding = LC_AMI;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 206:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1_MF;
            te1Cfg[n].coding = LC_HDB3;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 207:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1_CRC_MF;
            te1Cfg[n].coding = LC_AMI;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 208:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1_CRC_MF;
            te1Cfg[n].coding = LC_HDB3;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 209:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1_SQW;
            te1Cfg[n].coding = LC_AMI;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 210:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_E1;
            te1Cfg[n].framing = FF_E1_SQW;
            te1Cfg[n].coding = LC_HDB3;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 301:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_J1;
            te1Cfg[n].framing = FF_J1_D4;
            te1Cfg[n].coding = LC_AMI;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 302:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_J1;
            te1Cfg[n].framing = FF_J1_D4;
            te1Cfg[n].coding = LC_JBZS;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 303:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_J1;
            te1Cfg[n].framing = FF_J1_D4;
            te1Cfg[n].coding = LC_AMI;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    case 304:
        for (n = 0; n < MAX_FRAMERS; n++) {
            te1Cfg[n].lineType = LT_J1;
            te1Cfg[n].framing = FF_J1_ESF;
            te1Cfg[n].coding = LC_JBZS;
            te1Cfg[n].termination = LTM_120OHM;
            inpLineConfig(n);
        }
        break;
    default:

        {
            printf(
            "the usage\nframerDebugModeSet 205 for example\n for other mode see below\n"
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
        break;

    }
    return OK;
}
void framerDebugEnablessm(void)
{
    int i, j;
    for (i = 0; i < MAX_FRAMERS; i++) {
        for (j = 0; j < 5; j++) {
            enableInpFramerSSM(i, j);
        }
    }
}
void framerDebugRestart(void)
{
    int n;
    INP_FRAMER_t *ptr;
    for (n = 0; n < 5; n++) {
        ptr = &inpFramer[n];
        if ((te1Cfg[n].lineType == LT_E1) ||
            (te1Cfg[n].lineType == LT_SQW)) {
            ptr->reg->e1rcr |= E1RCR_RESYNC;
            ptr->reg->e1rcr &= ~E1RCR_RESYNC;
        } else if ((te1Cfg[n].lineType == LT_T1) ||
                   (te1Cfg[n].lineType == LT_J1)) {
            ptr->reg->t1rcr1 |= T1RCR1_RESYNC;
            ptr->reg->t1rcr1 &= ~T1RCR1_RESYNC;
            ptr->reg->t1rcr1 |= T1RCR1_SYNCE;
            ptr->reg->bocc |= (BOCC_RBOCE | BOCC_RBR);
            ptr->reg->bocc &= ~(BOCC_SBOC);
            ptr->reg->bocc &= ~(BOCC_RBR);
        }
    }
}



/* ------------------------------- testing end */

#endif
