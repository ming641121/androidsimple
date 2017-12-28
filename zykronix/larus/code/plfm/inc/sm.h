/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *
 *Note:
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: sm.h,v 1.21 2006/10/05 04:11:47 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/sm.h,v $
 *====================================================================
 * $Log: sm.h,v $
 * Revision 1.21  2006/10/05 04:11:47  cvsshuming
 * remove inter-include
 *
 * Revision 1.20  2006/10/03 12:10:54  cvsshuming
 * add action function support in shelf manager for special card type, such as top, smu and imu
 *
 * Revision 1.19  2006/10/03 02:43:56  cvsshuming
 * rename redundance and state to be redundancy_enable and redundancy_state
 *
 * Revision 1.18  2006/10/02 12:08:45  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.17  2006/10/02 07:26:16  cvsshuming
 * add smVariableInit
 *
 * Revision 1.16  2006/09/20 10:37:48  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.15  2006/09/15 09:59:37  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.14  2006/09/15 09:24:29  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.13  2006/09/13 07:20:10  cvsshuming
 * for portalarmstatus
 *
 * Revision 1.12  2006/09/13 06:06:20  cvsshuming
 * change shelfTask return from void to  STATUS_t
 *
 * Revision 1.11  2006/09/12 10:24:46  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.10  2006/09/12 05:49:02  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.9  2006/09/11 15:20:00  cvsshuming
 * check in snmp action func. still in process
 *
 * Revision 1.8  2006/09/01 05:31:33  cvsshuming
 * for PRS
 *
 * Revision 1.7  2006/08/29 05:48:24  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.6  2006/08/25 02:35:36  cvsshuming
 * change header message
 *
 * $Endlog $
 *********************************************************************/
#ifndef _SM_H_
#define _SM_H_


#include "sr_snmp.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpdefs.h"
#include "snmppart.h"
#include "snmpsupp.h"

/* Address defined for output card on Main board */
#define MAIN_OUTPUTCARD_FPGA_BASE_ADDR            0x41000000
#define MAIN_OUTPUTCARD_TIU_FRAMER_B_BASE_ADDR		0x41001000
#define MAIN_OUTPUTCARD_TIU_FRAMER_A_BASE_ADDR		0x41002000
#define MAIN_OUTPUTCARD_TIU_FRAMER_ADDR_SPACING		0x100





/* Address defined for FPGA on Main board */
#define MAIN_FPGA_BASE_ADDR            0x40000000
#define MAIN_FPGA_W_REG_1				 (MAIN_FPGA_BASE_ADDR + 0x00)
#define MAIN_FPGA_W_REG_2			   (MAIN_FPGA_BASE_ADDR + 0x01)	
#define MAIN_FPGA_R_REG_1			   (MAIN_FPGA_BASE_ADDR + 0x00)	/* clock select */
#define MAIN_FPGA_R_REG_2			   (MAIN_FPGA_BASE_ADDR + 0x01)	/* expansion shelf present */
#define MAIN_FPGA_R_REG_3			   (MAIN_FPGA_BASE_ADDR + 0x02)	/* expansion shelf alarm */
#define MAIN_FPGA_R_REG_4			   (MAIN_FPGA_BASE_ADDR + 0x03)	/* expansion interrupt register*/
#define MAIN_FPGA_R_REG_5			   (MAIN_FPGA_BASE_ADDR + 0x04)	/* master output card interrupt */
#define MAIN_FPGA_R_REG_6			   (MAIN_FPGA_BASE_ADDR + 0x05)	/* master output card present */
#define MAIN_FPGA_R_REG_7			   (MAIN_FPGA_BASE_ADDR + 0x06)	/* master status rgister */
#define MAIN_FPGA_R_REG_7_PRS_PRESENT								(1 << 2)
#define MAIN_FPGA_R_REG_8			   (MAIN_FPGA_BASE_ADDR + 0x07)	/* EXP4 - card present */
#define MAIN_FPGA_R_REG_9			   (MAIN_FPGA_BASE_ADDR + 0x08)	/* EXP4 - card present */
#define MAIN_FPGA_R_REG_10		   (MAIN_FPGA_BASE_ADDR + 0x09)
#define MAIN_FPGA_R_REG_11		   (MAIN_FPGA_BASE_ADDR + 0x0A)
#define MAIN_FPGA_R_REG_12		   (MAIN_FPGA_BASE_ADDR + 0x0B)
#define MAIN_FPGA_R_REG_13		   (MAIN_FPGA_BASE_ADDR + 0x0C)
#define MAIN_FPGA_R_REG_14		   (MAIN_FPGA_BASE_ADDR + 0x0D)
#define MAIN_FPGA_R_REG_15		   (MAIN_FPGA_BASE_ADDR + 0x0E)
#define MAIN_FPGA_R_REG_16		   (MAIN_FPGA_BASE_ADDR + 0x0F)
#define MAIN_FPGA_R_REG_17		   (MAIN_FPGA_BASE_ADDR + 0x10)

#define EXP_FPGA_ADDR_SHELF            (MAIN_FPGA_BASE_ADDR + 0x100)
#define EXP_FPGA_ADDR_COMMAND          (MAIN_FPGA_BASE_ADDR + 0x101)
#define EXP_FPGA_ADDR_LSB              (MAIN_FPGA_BASE_ADDR + 0x102)
#define EXP_FPGA_ADDR_MSB              (MAIN_FPGA_BASE_ADDR + 0x103)
#define EXP_FPGA_ADDR_DATA             (MAIN_FPGA_BASE_ADDR + 0x104)

/* Address defined for FPGA on Output card */
#define OUTPUT_FPGA_BASE_ADDR          0x41000000		/* checkitout. should use 0x41001000 instead? */
#define OUTPUT_FPGA_R_REG_1(slot)      ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x00)
#define OUTPUT_FPGA_R_REG_2(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x01)
#define OUTPUT_FPGA_R_REG_3(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x02)
#define OUTPUT_FPGA_R_REG_4(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x03)
#define OUTPUT_FPGA_W_REG_1(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x00)
#define OUTPUT_FPGA_W_REG_2(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x01)
#define OUTPUT_FPGA_W_REG_3(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x02)
#define OUTPUT_FPGA_W_REG_4(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x03)
#define OUTPUT_FPGA_W_REG_5(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x04)
#define OUTPUT_FPGA_W_REG_6(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x05)
#define OUTPUT_FPGA_W_REG_7(slot) 	   ((OUTPUT_FPGA_BASE_ADDR | (slot << 8)) + 0x06)


#define	SMU_FRAMER_BASE_ADDR						0x43040000
#define	SMU_FRAMER_DEV_ID_ADDR					(SMU_FRAMER_BASE_ADDR + 0x00F8)
#define	SMU_FRAMER_DEV_ID_MASK					0x58

#define OUTPUT_OFF              0
#define OUTPUT_ON 				1
#define NUMBER_SHELVES          5
#define NUM_PORTS               8
#define STRATUM_CLOCK_MAX       7
#define FPGA_READ               1
#define FPGA_WRITE              2
#define FLASH_READ              1
#define FLASH_WRITE             2

/*
note: DisplayString: max length 13
	shelf
		stsShelfSerial				DisplayString
		stsShelfCLEICode			DisplayString
	card
		stsCardDescr				DisplayString
		stsCardSerial				DisplayString
		stsCardHwVersion			DisplayString
		stsCardCLEICode				DisplayString
		
		stsOutputTimingFormat		OctetString		(1..60)
		stsOutputPortSettingName	OctetString		(0..40)	


*/
#define SM_SHELF_SERIAL_LEN		13
#define	SM_SHELF_CLEICODE_LEN	13

#define	SM_CARD_DESCRIPTON_LEN	13
#define	SM_CARD_SERIAL_LEN		13
#define	SM_CARD_HW_VERSION_LEN	13
#define	SM_CARD_CLEICODE_LEN	13
#define SM_CARD_TIMINGFORMAT_LEN		60

#define SM_PORT_PORTNAME_LEN			40




#define	SM_INT_VECTOR			11  /*checkitout*/

#define ON      				1
#define OFF     				0
#define OUTPUT_ALARM_ON     	1
#define OUTPUT_ALARM_OFF    	0
#define SM_YES					1
#define SM_NO					0



typedef enum {
    MAIN_SHELF = 0,
    EXP1_SHELF,
    EXP2_SHELF,
    EXP3_SHELF,
    EXP4_SHELF
} SHELF_t;

typedef enum {		/* OUTPUT_CARD_TYPE_t */   
    OUTPUT_FREQUENCY = 1,	/* T1 E1 .. */
    OUTPUT_IRIGB,			/* IRIG-B*/
    OUTPUT_TIMING,			/* 1 PPS.. */
    OUTPUT_5_10MHZ,			/* 5/10 MHz */
    OUTPUT_TIU,				/* TIU */
} OUTPUT_CARD_TYPE_t;


enum {		/* OUTPUT_CARD_REDUNDANCY_t */    
	REDUNDANCY_DISABLE=0,
	REDUNDANCY_ENABLE
};

enum {		/* OUTPUT_CARD_STATE_t */    
	REDUNDANCY_STANDBY=0,
	REDUNDANCY_ACTIVE
};



enum {		/* OUTPUT_CARD_STATUS_t */    
    OUTPUT_ABSENT,
    OUTPUT_PRESENT,
    OUTPUT_ACTIVE,
    OUTPUT_MISMATCH   
};


typedef enum {			/* OUTPUT_PORT_TYPE_t */  
    OUTPUT_NULL,
    OUTPUT_T1,
    OUTPUT_E1,
    OUTPUT_CC,
    OUTPUT_SQW,
    OUTPUT_1PPS,
    OUTPUT_1PPM,
    OUTPUT_1PPH
} OUTPUT_PORT_TYPE_t;

enum {
    OUTPUT_FM_NULL,
    OUTPUT_FM_SF,
    OUTPUT_FM_ESF,
    OUTPUT_FM_E1,		/* CAS OFF & CRC OFF */
    OUTPUT_FM_E1_MF,		/* CAS ON & CRC OFF */
    OUTPUT_FM_E1_CRC,		/* CAS OFF & CRC ON */
    OUTPUT_FM_E1_MF_CRC,	/* CAS ON & CRC ON */
};

enum {
    STRATUM_1,
    STRATUM_2,
    STRATUM_3,
    STRATUM_DO_NOT_USE
};

enum {
    SM_SHELF,
    SM_OUTPUT_CARD,
    SM_MAIN_OUTPUT_PORT,
    SM_EXP1_OUTPUT_PORT,
    SM_EXP2_OUTPUT_PORT,
    SM_EXP3_OUTPUT_PORT,
    SM_EXP4_OUTPUT_PORT
};

typedef struct {

U8_t outputAlarm_lossClock  ;
U8_t outputAlarm_tiu_act    ;
U8_t outputAlarm_tiu_los    ;
U8_t outputAlarm_tiu_t_tie  ;
U8_t outputAlarm_tiu_t_mtie ;
U8_t outputAlarm_tiu_aisp   ;
U8_t outputAlarm_tiu_lof    ;
U8_t outputAlarm_tiu_t_cs   ;

} OUTPUT_ALARM_TYPE_t;


typedef struct {
    U8_t                    cardType;
    U8_t                    redundancy_enable;     	/* toggle redundancy enable or disable */
    U8_t                    redundancy_state;		/* redundancy status */
    U8_t                    status;					/* card running status */
    U8_t                    portType[8];
    OctetString				portName[8];
    OUTPUT_ALARM_TYPE_t     portAlarmSetting[8];  	/* alarm severity setting for each output port*/
    OUTPUT_ALARM_TYPE_t     portAlarmStatus[8];
    U8_t                    list;
    U8_t                    issue;

    U8_t reg0;
    U8_t reg1;
    U8_t reg2;
    U8_t reg3;
    U8_t reg4;
    U8_t reg5;
    
    SR_INT32        outputCRCCHK;
    SR_INT32        outputCAS;
    SR_INT32        outputSSM;
    SR_INT32        outputSSMValue;
    OctetString     outputTimingFormat;

    
    OctetString     cardDescr;
    OctetString     cardHwVersion;
    OctetString     cardSerial;
    OctetString     cardCLEICode;


/* 
    TIU_FRAMER_t	*tiuFramer_A;
    TIU_FRAMER_t	*tiuFramer_B;
    TE1_CONFIG_t	*tiuConfig_A;
    TE1_CONFIG_t	*tiuConfig_B;

*/
} OUTPUT_CARD_PARA_t;


typedef struct {
    U8_t                    cardType;
    U8_t                    redundancy_enable;     /* toggle redundancy enable or disable */
    U8_t                    redundancy_state;			/* redundancy status */
    U8_t                    status;			/* card running status */
    OctetString     cardDescr;
    OctetString     cardHwVersion;
    OctetString     cardSerial;
    OctetString     cardCLEICode;

} SPECIAL_CARD_PARA_t;

typedef struct {
    OUTPUT_CARD_PARA_t main[8];
    OUTPUT_CARD_PARA_t exp1[16];
    OUTPUT_CARD_PARA_t exp2[16];
    OUTPUT_CARD_PARA_t exp3[16];
    OUTPUT_CARD_PARA_t exp4[16];
    SPECIAL_CARD_PARA_t smu;
    SPECIAL_CARD_PARA_t top;
    SPECIAL_CARD_PARA_t imu;
} OUTPUT_CARD_t;




typedef struct {
    U8_t present;
    U8_t state;
    U8_t powerA;
    U8_t powerB;
   OctetString serial;
   OctetString cleicode;
} SHELFS_t;




/* function declaration*/
extern STATUS_t shelfTask(void);
extern STATUS_t shelfInit(void);
extern STATUS_t redundancySwitch(SHELF_t shelf_index, int slot);
extern STATUS_t redundancyConfig(SHELF_t shelf_index, int slot, int action);
extern STATUS_t outpPortSet(SHELF_t shelf_index, int slot, int port, int type, int framing);
extern S8_t outpPortGet(SHELF_t shelf_index, int slot, int port);
extern void outpSSMUpdate(void);
extern STATUS_t outpSSMSet(int stratumClock);
extern STATUS_t outpSSMClear();
extern U8_t outpSSMGet();
extern void expFPGAAccess(SHELF_t shelf_index, U8_t addrHigh, U8_t addrLow, U8_t *data, U8_t command);
extern STATUS_t SMUPresent(void);


 
/* function declaration - SNMP [reported to Joe]*/		
extern STATUS_t smShelfGet(stsShelfEntry_t *mibstructIn);
extern STATUS_t smShelfSet(stsShelfEntry_t *mibstructIn);
    				
extern STATUS_t smOutputCardGet(stsOutputCardEntry_t *mibstructIn);
extern STATUS_t smOutputCardSet(stsOutputCardEntry_t *mibstructIn);
    				
extern STATUS_t smOutputPortGet(stsOutputPortSettingEntry_t *mibstructIn);
extern STATUS_t smOutputPortSet(stsOutputPortSettingEntry_t *mibstructIn);

/* function declaration - SNMP [not yet reported to Joe]*/			
 


/* extern the global variable used in shelf manager*/
extern SHELFS_t shelves[NUMBER_SHELVES];
extern OUTPUT_CARD_t outpCard;
extern U8_t ssmMessages[STRATUM_CLOCK_MAX];

#if 0		/* checkitout. if we need a setting for each shelf or just systemwide*/
	extern STATUS_t userSSMEnable[NUMBER_SHELVES][16];
	extern U8_t userSSM[NUMBER_SHELVES][16];
	extern U8_t outpSSM[NUMBER_SHELVES][16];
#endif






#endif				/* _SM_H_ */
