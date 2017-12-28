/*
 *Copyright(C) 2006, All Rights Reserved, by
 
 
 /*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract: Defines of interrupt vectors
 *
 *
 *Note:
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: sys_int.h,v 1.2 2006/10/03 04:24:30 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/sys_int.h,v $
 *====================================================================
 * $Log: sys_int.h,v $
 * Revision 1.2  2006/10/03 04:24:30  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/
#ifndef _SYS_INT_H_
#define _SYS_INT_H_

/* Enable all interrupt masks: intrrupts that has not been intConnected
 * will be handled by sysAutoAck().
 */
#define INT_LVL_SR_IMASK        (INT_LVL_SW0 | INT_LVL_SW1 | INT_LVL_ASX |\
                                 INT_LVL_DMC | INT_LVL_GT64120 | INT_LVL_IDE |\
                                 INT_LVL_SCC_SO | INT_LVL_TIMER )

/* R4000 interrupt levels:  Used in intEnable() and intDisable() */
#define INT_LVL_TIMER           SR_IBIT8        /* R4000 timer (fixed)      */
#define INT_LVL_SCC_SO          SR_IBIT7        /* SCC Switch Over          */
#define INT_LVL_CLKSYC_SO       SR_IBIT7        /* Clk Syn Fail Switch Over */
#define INT_LVL_ENV_MON         SR_IBIT7        /* Enviromental Monitor     */
#define INT_LVL_CSM             SR_IBIT7        /* CSM                      */
#define INT_LVL_BTIS            SR_IBIT7        /* BTIS                     */
#define INT_LVL_IDE             SR_IBIT6        /* IDE                      */
#define INT_LVL_ENET1           SR_IBIT6        /* Fast Ethernet Port 1     */
#define INT_LVL_ENET2           SR_IBIT6        /* Fast Ethernet Port 2     */
#define INT_LVL_PCIBIM          SR_IBIT6        /* PCI BIM Port             */
#define INT_LVL_I2C             SR_IBIT6        /* I2C Controller           */
#define INT_LVL_SAR             SR_IBIT6        /* ATMizer-II SAR           */
#define INT_LVL_DUART           SR_IBIT6        /* Serial Port              */
#define INT_LVL_GT64120         SR_IBIT5        /* GT64120                  */
#define INT_LVL_BIMPHYLYR       SR_IBIT4        /* BIM Physical Layer       */
#define INT_LVL_DMC             SR_IBIT4        /* DMC Card                 */
#define INT_LVL_CUBITA          SR_IBIT4        /* CUBIT A                  */
#define INT_LVL_CUBITB          SR_IBIT4        /* CUBIT B                  */
#define INT_LVL_RTC             SR_IBIT4        /* DS17887 Realtime Clock   */
#define INT_LVL_DTACK           SR_IBIT4        /* Bus Timeout              */
#define INT_LVL_DMCOK           SR_IBIT4        /* DMC_OK state changed     */
#define INT_LVL_ASX             SR_IBIT3        /* ASX                      */
#define INT_LVL_CBALM           SR_IBIT3        /* Cellbus ALM              */
#define INT_LVL_BIMALM          SR_IBIT3        /* BIM ALM                  */
#define INT_LVL_CBABM           SR_IBIT3        /* Cellbus ABM              */
#define INT_LVL_BIMABM          SR_IBIT3        /* BIM ABM                  */
#define INT_LVL_SW1             SR_IBIT2        /* sw interrupt 1 (fixed)   */
#define INT_LVL_SW0             SR_IBIT1        /* sw interrupt 0 (fixed)   */

/* interrupt indexes: Used in splx() etc. */
 
#define INT_INDX_TIMER          7       /* R4000 timer (fixed)      */
#define INT_INDX_SCC_SO         6       /* SCC Switch Over          */
#define INT_INDX_CLKSYC_SO      6       /* Clk Syn Fail Switch Over */
#define INT_INDX_ENV_MON        6       /* Enviromental Monitor     */
#define INT_INDX_CSM            6       /* CSM                      */
#define INT_INDX_BTIS           6       /* BTIS                     */
#define INT_INDX_IDE            5       /* IDE                      */
#define INT_INDX_ENET1          5       /* Fast Ethernet Port 1     */
#define INT_INDX_ENET2          5       /* Fast Ethernet Port 2     */
#define INT_INDX_PCIBIM         5       /* PCI BIM Port             */
#define INT_INDX_I2C            5       /* I2C Controller           */
#define INT_INDX_SAR            5       /* ATMizer-II SAR           */
#define INT_INDX_DUART          5       /* Serial Port              */
#define INT_INDX_GT64120        4       /* GT64120                  */
#define INT_INDX_BIMPHYLYR      3       /* BIM Physical Layer       */
#define INT_INDX_DMC            3       /* DMC Card                 */
#define INT_INDX_CUBITA         3       /* CUBIT A                  */
#define INT_INDX_CUBITB         3       /* CUBIT B                  */
#define INT_INDX_RTC            3       /* DS17887 Realtime Clock   */
#define INT_INDX_DTACK          3       /* Bus Timeout           */
#define INT_INDX_DMCOK          3       /* DMC_OK state changed     */
#define INT_INDX_ASX            2       /* ASX                      */
#define INT_INDX_CBALM          2       /* Cellbus ALM              */
#define INT_INDX_BIMALM         2       /* BIM ALM                  */
#define INT_INDX_CBABM          2       /* Cellbus ABM              */
#define INT_INDX_BIMABM         2       /* BIM ABM                  */
#define INT_INDX_SW1            1       /* sw interrupt 1 (fixed)   */
#define INT_INDX_SW0            0       /* sw interrupt 0 (fixed)   */

/* interrupt vectors: Used in intPrioTable[] in sysLib.c and intConnect() */
#define IV_IBOUND_VEC           2       /* Instruction bound exception */
#define IV_DBOUND_VEC           3       /* Data bound exception */
#define IV_FPE_VEC              15      /* R4000 floating point vector */

#define IV_SWTRAP0_VEC          32      /* R4000 software trap 0 */
#define IV_SWTRAP1_VEC          33      /* R4000 software trap 1 */

#define IV_TIMER_VEC		64	/* R4000 Timer (fixed)   */

#define IV_SCCSO_VEC            65      /* SCC Switch Over       */
#define IV_CLKSYCSO_VEC         66      /* Clk Syn Switch Over   */
#define IV_ENV_MON_VEC          67      /* Enviromental Monitor     */
#define IV_CSM_VEC              68      /* CSM                      */
#define IV_BITS_VEC             69      /* BTIS                     */

#define IV_IDE_VEC  		70      /* IDE controller        */
#define IV_ETHERNET_VEC         71	/* Ethernet Port 1       */
#define IV_ETHERNET2_VEC	72	/* Ethernet Port 2       */
#define IV_VEC_MPSC		73	/* SCC channel B (Serial)*/
#define IV_SCC_A_VEC		74	/* SCC channel A (HDLC)  */
#define IV_PCIBIM_VEC           75      /* PCI BIM Port          */
#define IV_I2C_VEC              76      /* I2C Controller        */
#define IV_SAR_VEC              77      /* ATMizer-II SAR        */

#define IV_BIMPHYLYR_VEC        80      /* BIM Physical Layer    */
#define IV_DMC_VEC              81      /* DMC Card              */
#define IV_CUBITA_VEC           82      /* CUBIT A               */
#define IV_CUBITB_VEC           83      /* CUBIT B               */
#define IV_RTC_VEC              84      /* Realtime Clock        */
#define IV_DTACK_VEC            85      /* Bus Timeout           */
#define IV_DMCOK_VEC            86      /* DMC_OK state changed  */

#define IV_ASX_VEC              90      /* ASX                   */
#define IV_CBALM_VEC            91      /* Cellbus ALM           */
#define IV_BIMALM_VEC           92      /* BIM ALM               */
#define IV_CBABM_VEC            93      /* Cellbus ABM           */
#define IV_BIMABM_VEC           94      /* BIM ABM               */
#define IV_POWEROFF_VEC         95      /* Power is off          */

#define IV_GALILEO_VEC		100      /* GT-64120              */
#define IV_DMA_CHN0_VEC         101     /* GT-64120 Channel 0 DMA*/
#define IV_DMA_CHN1_VEC         102     /* GT-64120 Channel 1 DMA*/
#define IV_DMA_CHN2_VEC         103     /* GT-64120 Channel 2 DMA*/
#define IV_DMA_CHN3_VEC         104     /* GT-64120 Channel 3 DMA*/
#define IV_GT_MEMOUT_VEC	105	/* GT-64120 Memory out of range */
#define IV_GT_DMAOUT_VEC	106	/* GT-64120 DMA addr out of range */
#define IV_GT_CPUOUT_VEC	107	/* GT-64120 CPU out of range */
#define IV_GT_T0EXP_VEC		108	/* GT-64120 Timer 0 expired */
#define IV_GT_T1EXP_VEC		109	/* GT-64120 Timer 1 expired */
#define IV_GT_T2EXP_VEC		110	/* GT-64120 Timer 2 expired */
#define IV_GT_T3EXP_VEC		111	/* GT-64120 Timer 3 expired */
#define IV_GT_MAS_RD_ERR_VEC	112	/* GT-64120 Master read parity error */
#define IV_GT_SLV_WR_ERR_VEC	113	/* GT-64120 Slave write parity error */
#define IV_GT_MAS_WR_ERR_VEC	114	/* GT-64120 Master write parity error */
#define IV_GT_SLV_RD_ERR_VEC	115	/* GT-64120 Slave read parity error */
#define IV_GT_PAR_ADDR_ERR_VEC	116	/* GT-64120 Address parity error */
#define IV_GT_PAR_MEM_ERR_VEC	117	/* GT-64120 Memory parity error */
#define IV_GT_MAS_ABORT_VEC	118	/* GT-64120 PCI Master abort */
#define IV_GT_TAR_ABORT_VEC	119	/* GT-64120 PCI Target abort */
#define IV_GT_RETRY_CTR_VEC	120	/* GT-64120 PCI retry counter expired */

#define IV_UNKNOWN_VEC	        180	/* Unknown interrupt vector */

typedef struct {
  U32_t           intLevel;
  VOID_FUNC_PTR_t intHandle;
} INT_CONNECT_t;

INT_CONNECT_t intConnectTable[] = 
{
};
int nIntConnectentries = sizeof(intConnectTable)/sizeof(INT_CONNECT_t);

#ifdef INT_CONNECT
INT_CONNECT_t intConnectTable[] = 
{
#ifndef SIM
/* INT 4 */
 {IV_SCCSO_VEC,  SysSwitchOver},
 {IV_CLKSYCSO_VEC, SysClkSyn},
 {IV_CSM_VEC, SysUnknownInt},
/* INT 3 */
 {IV_SAR_VEC, SysUnknownInt},
 {IV_ETHERNET_VEC, SysUnknownInt},
 {IV_ETHERNET2_VEC, SysUnknownInt},
 {IV_IDE_VEC, SysUnknownInt},
 {IV_PCIBIM_VEC, SysUnknownInt},
/* INT 1 */
 {IV_BIMPHYLYR_VEC, SysUnknownInt},
 {IV_DMC_VEC, SysUnknownInt},
 {IV_DTACK_VEC,  SysDTACK},
 {IV_DMCOK_VEC,  SysDMCOK},
/* INT 0 */
 {IV_POWEROFF_VEC, SysPowerOff},
/* INT UNKNOWN */
 {IV_UNKNOWN_VEC, SysUnknownInt},
#endif
};

int nIntConnectentries = sizeof(intConnectTable)/sizeof(INT_CONNECT_t);
#endif  /* INT_CONNECT */

#endif /* _SYS_INT_H_ */
