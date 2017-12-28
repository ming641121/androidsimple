/*
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: ds26502.h
 *
 *input framer maxim ds26502 code
 *
 *Note:
 *
 *Author(s):
 * shuming chang
 */


 /* $Id: ds26502.h,v 1.20 2006/10/03 05:43:22 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/ds26502.h,v $
 *====================================================================
 * $Log: ds26502.h,v $
 * Revision 1.20  2006/10/03 05:43:22  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.19  2006/09/25 11:00:42  cvsshuming
 * add ssm action func
 *
 * Revision 1.18  2006/09/22 10:48:07  cvsshuming
 * fix link enable issue
 *
 * Revision 1.17  2006/09/15 06:37:40  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.16  2006/09/13 06:09:30  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.15  2006/09/12 07:34:15  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.14  2006/09/05 10:05:43  cvsshuming
 * add PRS trap support
 *
 *
 *====================================================================
 * $Endlog $
 */

 
 
#ifndef DS26502_H
#define DS26502_H


#include "sr_snmp.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpdefs.h"
#include "snmppart.h"
#include "snmpsupp.h"




#define MAX_FRAMERS                  5				/* number of input framer*/
  
#define MAX_RESET_WAIT		         0xfffff  /* This is the max count value */
#define OOF_CRITERIA		         3    	  /* 0 = 2 out of 4 bits in error */
					                          /* 1 = 2 out of 5 bits in error */
					                          /* 2,3 = 2 out of 6 bits in error */
#define RX_BOC_THRESHOLD	         7	      /* Set the consecutive sequence to */
					                          /* Be valid */
					                          /* 0 = NONE */
					                          /* 1 = 3 BOCs */
					                          /* 2 = 5 BOCs */
					                          /* 3 = 7 BOCs */
#define MCLK_MODE		             0		  /* 2.048 MHz */
#define RX_TERM_DISABLE              0
#define RX_TERM_75OHM                1
#define RX_TERM_100OHM               2
#define RX_TERM_120OHM               3
#define RX_TERM_110OHM               4
#define SSM_MESSAGES                 5
#define ALARM_ON                     1
#define ALARM_OFF                    0
#define FRAMER_OUT_OF_SERVICE		0
#define FRAMER_IN_SERVICE            1
#define FRAMER_SERVICE_INIT	2
#define POLL_FRAMER_INTERVAL         5		  /* 50msc */
#define RED_ALARM_TIMER              (250 / POLL_FRAMER_INTERVAL)
#define SSM_DISABLE                  0
#define SSM_ENABLE                   1
#define TMS_RXLOS		             0x20000001	/* the various Alarm Types */
#define TMS_RXOOF		             0x20000002
#define TMS_RXAIS		             0x20000004
#define TMS_RXRAI		             0x20000008
#define TMS_RXRFI		             0x20000010
#define TMS_RXBSF		             0x20000020
#define TMS_RXBSD		             0x20000040
#define TMS_RXLOP		             0x20000080
#define TMS_RXTIM		             0x20000100
#define TMS_RXUNEQ		             0x20000200
#define TMS_RXPLM		             0x20000400
#define TMS_RXRFIS		             0x20000800
#define TMS_RXRFIC		             0x20001000
#define TMS_RXRFIP		             0x20002000
#define TMS_RXIDLE		             0x20004000
#define TMS_RXOOFDL		             0x20008000	
#define TMS_RXRED		             0x20010000


typedef struct ds26502 {
	VU8_t tstrreg;		/* 0 */
#define TSTRREG_TEST(x) 	((x & 0x03) << 4)
#define TSTRREG_SFTRST		(1 << 0)
	 VU8_t iocr1;		    /* 1 */
#define IOCR1_RSMS2		    (1 << 6)
#define IOCR1_RSMS1		    (1 << 5)
#define IOCR1_RLOFF		    (1 << 4)
#define IOCR1_CSM_TSDW		(1 << 3)
#define IOCR1_TSM		    (1 << 2)
#define IOCR1_TSIO		    (1 << 1)
#define IOCR1_ODF		    (1 << 0)
	VU8_t iocr2;		    /* 2 */
#define IOCR2_RCLKINV		(1 << 7)
#define IOCR2_TCLKINV		(1 << 6)
#define IOCR2_RS_8KINV		(1 << 5)
#define IOCR2_TS_8K_4INV	(1 << 4)
	VU8_t t1rcr1;		    /* 3 */
#define T1RCR1_ARC		    (1 << 6)
#define T1RCR1_OOF(x)		((x & 0x03) << 4)
#define T1RCR1_SYNCC		(1 << 3)
#define T1RCR1_SYNCT		(1 << 2)
#define T1RCR1_SYNCE		(1 << 1)
#define T1RCR1_RESYNC		(1 << 0)
	VU8_t t1rcr2;		    /* 4 */
#define T1RCR2_RB8ZS		(1 << 5)
#define T1RCR2_RJC		    (1 << 1)
#define T1RCR2_RD4YM		(1 << 0)
	VU8_t t1tcr1;		    /* 5 */
#define T1TCR1_TJC		    (1 << 7)
#define T1TCR1_TFPT		    (1 << 6)
#define T1TCR1_TCPT		    (1 << 5)
#define T1TCR1_TYEL		    (1 << 0)
	VU8_t t1tcr2;		    /* 6 */
#define T1TCR2_TB8ZS		(1 << 7)
#define T1TCR2_TFSE		    (1 << 6)
#define T1TCR2_FBCT2(x)		((x & 0x03) << 3)
#define T1TCR2_TD4YM		(1 << 2)
#define T1TCR2_TB7ZS		(1 << 0)
	VU8_t t1ccr;		    /* 7 */
#define T1CCR_TRAICI		(1 << 4)
#define T1CCR_TAISCI		(1 << 3)
#define T1CCR_PDE		    (1 << 1)
	VU8_t mcreg;		    /* 8 */
#define MCREG_TMODE(x)		((x & 0x0f) << 4)
#define MCREG_RMODE(x)		((x & 0x0f) << 0)
	VU8_t tpcr;		    /* 9 */
#define TPCR_TPLLOFS(x)		((x & 0x03) << 6)
#define TPCR_PLLOS		    (1 << 5)
#define TPCR_TPLLIFS(x)		((x & 0x03) << 3)
#define TPCR_TPLLSS		    (1 << 2)
#define TPCR_TCSS(x)		((x & 0x03) << 0)
	VU8_t spare1[6];	    /* a - f */
	VU8_t idr;		    /* 10 */
	VU8_t info1;		    /* 11 */
#define INFO1_RL(x)		    ((x & 0x0f) << 0)
	VU8_t info2;		    /* 12 */
#define INFO2_CRCRC		    (1 << 2)
#define INFO2_FASRC		    (1 << 1)
#define INFO2_CASRC		    (1 << 0)
	VU8_t iir;		    /* 13 */
	VU8_t sr1;		    /* 14 */
#define SR1_JALT		    (1 << 4)
#define SR1_TCLE		    (1 << 2)
#define SR1_TOCD		    (1 << 1)
	VU8_t imr1;		    /* 15 */
#define IMR1_JALT		    (1 << 4)
#define IMR1_TCLE		    (1 << 2)
#define IMR1_TOCD		    (1 << 1)
	VU8_t sr2;		    /* 16 */
#define SR2_RYELC		    (1 << 7)
#define SR2_RAISC		    (1 << 6)
#define SR2_RLOSC		    (1 << 5)
#define SR2_RLOFC		    (1 << 4)
#define SR2_RYEL		    (1 << 3)
#define SR2_RAIS		    (1 << 2)
#define SR2_RLOS		    (1 << 1)
#define SR2_RLOF		    (1 << 0)
	VU8_t imr2;		    /* 17 */
#define IMR2_RYELC		    (1 << 7)
#define IMR2_RAISC		    (1 << 6)
#define IMR2_RLOSC		    (1 << 5)
#define IMR2_RLOFC		    (1 << 4)
#define IMR2_RYEL		    (1 << 3)
#define IMR2_RAIS		    (1 << 2)
#define IMR2_RLOS		    (1 << 1)
#define IMR2_RLOF		    (1 << 0)
	VU8_t sr3;		    /* 18 */
#define SR3_LOTC		    (1 << 6)
#define SR3_BOCC		    (1 << 5)
#define SR3_RFDLAD		    (1 << 4)
#define SR3_RFDLF		    (1 << 3)
#define SR3_TFDLE		    (1 << 2)
#define SR3_RMTCH		    (1 << 1)
#define SR3_RBOC		    (1 << 0)
	VU8_t imr3;		    /* 19 */
#define IMR3_LOTC		    (1 << 6)
#define IMR3_BOCC		    (1 << 5)
#define IMR3_RFDLAD		    (1 << 4)
#define IMR3_RFDLF		    (1 << 3)
#define IMR3_TFDLE		    (1 << 2)
#define IMR3_RMTCH		    (1 << 1)
#define IMR3_RBOC		    (1 << 0)
	VU8_t sr4;		    /* 1a */
#define SR4_RSA(x)		    ((x & 0x03) << 5)
#define SR4_TMF			    (1 << 4)
#define SR4_TAF			    (1 << 3)
#define SR4_RMF			    (1 << 2)
#define SR4_RCMF		    (1 << 1)
#define SR4_RAF			    (1 << 0)
	VU8_t imr4;		    /* 1b */
#define IMR4_RSA(x)		    ((x & 0x03) << 5)
#define IMR4_TMF		    (1 << 4)
#define IMR4_TAF		    (1 << 3)
#define IMR4_RMF		    (1 << 2)
#define IMR4_RCMF		    (1 << 1)
#define IMR4_RAF		    (1 << 0)
	VU8_t info3;		    /* 1c */
#define INFO3_CSC(x)	    ((x & 0x1f) << 3)
#define INFO3_FASSA		    (1 << 2)
#define INFO3_CASSA		    (1 << 1)
#define INFO3_CRC4SA	    (1 << 0)
	VU8_t e1rcr;		    /* 1d */
#define E1RCR_RLOSA		    (1 << 6)
#define E1RCR_RHDB3		    (1 << 5)
#define E1RCR_FRC		    (1 << 2)
#define E1RCR_SYNCE		    (1 << 1)
#define E1RCR_RESYNC		(1 << 0)
	VU8_t e1tcr;		    /* 1e */
#define E1TCR_TFPT		    (1 << 7)
#define E1TCR_TSIS		    (1 << 4)
#define E1TCR_THDB3		    (1 << 1)
	VU8_t bocc;		    /* 1f */
#define BOCC_RBOCE		    (1 << 4)
#define BOCC_RBR		    (1 << 3)
#define BOCC_RBF(x)		    ((x & 0x03) << 1)
#define BOCC_SBOC		    (1 << 0)
	VU8_t lbcr;		    /* 20 */
#define LBCR_LLB		    (1 << 3)
#define LBCR_RLB		    (1 << 2)
	VU8_t spare2[15];	    /* 21 - 2f */
	VU8_t lic1;		    /* 30 */
#define LIC1_L(x)		    ((x & 0x07) << 5)
#define LIC1_EGL		    (1 << 4)
#define LIC1_JAS		    (1 << 3)
#define LIC1_JABDS		    (1 << 2)
#define LIC1_DJA		    (1 << 1)
#define LIC1_TPD		    (1 << 0)
	VU8_t lic2;		    /* 31 */
#define LIC2_LIRST		    (1 << 6)
#define LIC2_IBPV		    (1 << 5)
#define LIC2_TAIS		    (1 << 4)
#define LIC2_JACKS		    (1 << 3)
#define LIC2_RCCFE		    (1 << 2)
#define LIC2_SCLD		    (1 << 1)
#define LIC2_CLDS		    (1 << 0)
	VU8_t lic3;		    /* 32 */
#define LIC3_CMIE		    (1 << 7)
#define LIC3_CMII		    (1 << 6)
#define LIC3_MM(x)	     	((x & 0x30) << 3)
#define LIC3_TAOZ	    	(1 << 0)
	VU8_t lic4;	    	/* 33 */
#define LIC4_MPS(x)	    	((x & 0x03) << 6)
#define LIC4_TT(x)	    	((x & 0x07) << 3)
#define LIC4_RT(x)	    	((x & 0x07) << 0)
	VU8_t tlbc;	    	/* 34 */
#define TLBC_AGCE	    	(1 << 6)
#define TLBC_GC(x)	    	((x & 0x3f) << 0)
	VU8_t spare3[11]; 	/* 35 - 3f */
	VU8_t taf;	    	/* 40 */
#define TAF_SI		    	(1 << 7)
	VU8_t tnaf;	    	/* 41 */
#define TNAF_SI		    	(1 << 7)
#define TNAF_1		    	(1 << 6)
#define TNAF_A		    	(1 << 5)
#define TNAF_SA(x)	    	((x & 0x1f) << 0)
	VU8_t tsiaf;	    	/* 42 */
	VU8_t tsinaf;	    	/* 43 */
	VU8_t tra;	    	/* 44 */
	VU8_t tsa4;	    	/* 45 */
	VU8_t tsa5;	    	/* 46 */
	VU8_t tsa6;	    	/* 47 */
	VU8_t tsa7;	    	/* 48 */
	VU8_t tsa8;	    	/* 49 */
	VU8_t tsacr;	    	/* 4a */
#define TSACR_SIAF	    	(1 << 7)
#define TSACR_SINAF	    	(1 << 6)
#define TSACR_RA	    	(1 << 5)
#define TSACR_TSA8F(x)		((x & 0x1f) << 0)
	VU8_t spare4[5];  	/* 4b - 4f */
	VU8_t rfdl;	    	/* 50 */
	VU8_t tfdl;	    	/* 51 */
	VU8_t rfdlm1;	    	/* 52 */
	VU8_t rfdlm2;	    	/* 53 */
	VU8_t spare5[2];  	/* 54 - 55 */
	VU8_t raf;	    	/* 56 */
#define RAF_SI		    	(1 << 7)
#define RAF_FAS(x)	    	((x & 0x7f) << 0)
	VU8_t rnaf;	    	/* 57 */
#define RNAF_SI		    	(1 << 7)
#define RNAF_1		    	(1 << 6)
#define RNAF_A		    	(1 << 5)
#define RNAF_SA(x)	    	((x & 0x1f) << 0)
	VU8_t rsiaf;	    	/* 58 */
	VU8_t rsinaf;	    	/* 59 */
	VU8_t rra;	    	/* 5a */
	VU8_t rsa4;	    	/* 5b */
	VU8_t rsa5;	    	/* 5c */
	VU8_t rsa6;	    	/* 5d */
	VU8_t rsa7;	    	/* 5e */
	VU8_t rsa8;	    	/* 5f */
	VU8_t spare6[144];	/* 60 - ef */
	VU8_t test[16];   	/* f0 - ff */
} DS26502_t;


/*
	prs - primary reference (stratum 1)
	stu - sync traceability unknown
	st2 - stratum 2 reference
	st3 - stratum 3 reference
	sic - sonet minimum clock traceable
	dus - don't use for sync

*/
#define SSM_E1_PRS			0x02	/*0010*/		/* stratum 1 		*/   
#define SSM_E1_STU			0x00	/*0000*/		/* unknown 			*/
#define SSM_E1_ST2			0x04	/*0100*/		/* stratum 2 		*/
#define SSM_E1_ST3			0x08	/*1000*/		/* stratum 3 		*/
#define SSM_E1_SIC			0x0b	/*1011*/		/* sonet traceable 	*/
#define SSM_E1_DUS			0x0f	/*1111*/		/* do not use 		*/

#define SSM_T1_PRS			0x20	/*0010 0000*/	/* stratum 1 		*/   
#define SSM_T1_STU			0x10	/*0001 0000*/	/* unknown 			*/
#define SSM_T1_ST2			0x30	/*0011 0000*/	/* stratum 2 		*/
#define SSM_T1_ST3			0x04	/*0000 0100*/	/* stratum 3 		*/
#define SSM_T1_SIC			0x44	/*0100 0100*/	/* sonet traceable 	*/
#define SSM_T1_DUS			0x0c	/*0000 1100*/	/* do not use 		*/



#define FRAMER_INT_VECTOR		13
#define FRAMER_LINKID_LEN	 24

#define FLASH_READ              1
#define FLASH_WRITE             2

#define FRAMER_LINK_ENABLE_YES	1
#define FRAMER_LINK_ENABLE_NO	2


typedef enum {
  	LT_T1,
	LT_E1,
	LT_J1,
	LT_SQW,
	LT_CC
} TE1_LINETYPE_t;


typedef enum {
  FF_T1_D4=0,
	FF_T1_ESF,
	FF_J1_D4,
	FF_J1_ESF,
	FF_E1,
	FF_E1_CRC,
	FF_E1_MF,
	FF_E1_CRC_MF,
	FF_E1_SQW,
	FF_CC,
	FF_CC_400
} TE1_FORMAT_t;			/* define the frame format */

typedef enum {
	LC_JBZS,
	LC_B8ZS,
	LC_HDB3,
	LC_ZBTSI,
	LC_AMI,
	LC_OTHER
} TE1_CODING_t;			/* define the line coding */



typedef enum {
  LTM_DISABLE=0,
	LTM_75OHM,
	LTM_100OHM,
	LTM_120OHM,
	LTM_110OHM	
} TE1_TERMINATION_t;



typedef struct {
	OctetString 		linkId;
	U8_t				linkEnable; /*enable status. when disabled. no monitoring*/
	TE1_FORMAT_t		framing;	
	TE1_CODING_t		coding;
	TE1_LINETYPE_t		lineType;
	TE1_TERMINATION_t	termination;
} TE1_CONFIG_t;

typedef struct {
	  S8_t      status;	 	  
	  U8_t      alarmFlag;
	  U8_t      redTimer;
	  U8_t      redAlarm;
	  U8_t      yelAlarm;
  
	/* current serverity status */
		U32_t			alarmSeverity;
	
	/* alarm*/
	  U8_t      raisAlarm;
	  U8_t      actAlarm;
	  U8_t      losAlarm;
	  U8_t      aispAlarm;
	  U8_t      lofAlarm;
	  U8_t      cvlAlarm;
	  U8_t      cvpAlarm;
	  U8_t      csAlarm;
	  U8_t      jitterAlarm;
	  U8_t      tieAlarm;
	  U8_t      mtieAlarm;
  
	/* alarm Severity setting*/
	/*
	  U8_t      raisAlarmSeverity;
	  U8_t      actAlarmSeverity;
	  U8_t      losAlarmSeverity;
	  U8_t      aispAlarmSeverity;
	  U8_t      lofAlarmSeverity;
	  U8_t      cvlAlarmSeverity;
	  U8_t      cvpAlarmSeverity;
	  U8_t      csAlarmSeverity;
	  U8_t      jitterAlarmSeverity;
	  U8_t      tieAlarmSeverity;
	  U8_t      mtieAlarmSeverity;
  */
  /* event*/  
	  U8_t       losEvent;
	  U8_t       lofEvent;
	  U8_t       raisEvent;
	  U8_t       raiEvent;
	  U8_t       bvCountMan;
	  U8_t       crcCountMan;
	  U8_t       bvCountDef;
	  U8_t       crcCountDef;
	  DS26502_t *reg;
	  
} INP_FRAMER_t;


typedef enum {
		TE1_ALARM_ACT=1,
		TE1_ALARM_LOS,
		TE1_ALARM_AISP, 
		TE1_ALARM_LOF,
		TE1_ALARM_CVL, 
		TE1_ALARM_CVP,
		TE1_ALARM_CS, 
		TE1_ALARM_JITTER,
		TE1_ALARM_TIE,
		TE1_ALARM_MTIE
}TE1_ALARM_t;

typedef enum{
	TE1_ALARM_SET=0,
	TE1_ALARM_CLEAR
}TE1_ALARMACTION_t;
	
typedef struct {
  U8_t   status;
  U8_t   ssm;
  U8_t   alarm;
  U8_t   count;
} SSM_t;

typedef enum {
	SR1_REG,
	SR2_REG,
	SR3_REG,
	SR4_REG,
	INFO1_REG,
	INFO2_REG
} WR_REG_INDEX_t;

enum {
		  FRAMER_LED_YELLOW,
			FRAMER_LED_RED,
			FRAMER_LED_GREEN,
			FRAMER_LED_OFF
	} ;

enum {
		  PRS_STATUS_UP,
		  PRS_STATUS_DOWN,
		  PRS_STATUS_INIT
} ;
enum 
{
  TEL_CFG_DATA,
  RECEIVED_SSM,
};


/* global variable used framer*/
extern INP_FRAMER_t inpFramer[MAX_FRAMERS];
extern SSM_t receivedSSM[MAX_FRAMERS][SSM_MESSAGES];
extern TE1_CONFIG_t te1Cfg[MAX_FRAMERS];


/* routine used in framer*/
extern STATUS_t inpFramerTask(void);
extern STATUS_t getInpFramerSSM(int n);
extern STATUS_t checkInpFramer(int n);
extern STATUS_t enableInpFramerSSM(int n, int ch);
extern STATUS_t disableInpFramerSSM(int n, int ch);
extern void alarmIntHandler(void);
extern STATUS_t setInpFramerAlarm(TE1_ALARM_t alarmtype, TE1_ALARMACTION_t action, int ch);
extern STATUS_t inpLineConfig(int n);
extern STATUS_t framerSeverityCheck(int n);
extern STATUS_t framerDataAccess(char *fname, int mode, int dataType);


/* Framer SNMP routine*/
	#if 0   /* this has been implemented in pm module*/
		extern STATUS_t inpFramerAlarmGet(stsLinkAlarmEntry_t *mibstructIn);
		extern STATUS_t inpFramerAlarmSet(stsLinkAlarmEntry_t *mibstructIn);
	#endif
	
	extern STATUS_t inpFramerLinkSet(stsLinkEntry_t *mibstructIn);
	extern STATUS_t inpFramerLinkGet(stsLinkEntry_t *mibstructIn);
	extern STATUS_t SSMSet(stsClockInputSSMEntry_t *mibstructIn);
	extern STATUS_t SSMGet(stsClockInputSSMEntry_t *mibstructIn);


/* Framer debug routine*/
	#ifdef FRAMER_DEBUG
	extern STATUS_t framerDebugLedTest(void);
	extern STATUS_t framerDebugShow(void);
	extern STATUS_t framerDebugModeSet(int testcase);
	extern void framerDebugEnablessm(void);
	extern void framerDebugRestart(void);
	#endif


#endif /* DS26502_H */



