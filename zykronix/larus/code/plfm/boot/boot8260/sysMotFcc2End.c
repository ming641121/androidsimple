/* sysMotFcc2End.c - system configuration module for motFccEnd driver */

/* Copyright 1984-2003 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,04oct04,dtr  SPR 99953.
01a,18dec03,dtr  created from ads85xx.
*/

/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks
m8260FccEnd END driver.

It performs the dynamic parameterization of the motFccEnd driver.
This technique of 'just-in-time' parameterization allows driver
parameter values to be declared as any other defined constants rather
than as static strings.
*/

#include "vxWorks.h"
#include "config.h"

#include "vmLib.h"
#include "stdio.h"
#include "sysLib.h"
#include "logLib.h"
#include "stdlib.h"
#include "string.h"
#include "end.h"
#include "intLib.h"
#include "lstLib.h"
#include "drv/mem/m82xxDpramLib.h"
#include "miiLib.h"
#include "drv/end/motFcc2End.h"
/* defines */

#ifdef INCLUDE_MOTFCCEND
#define MOT_FCC_BASE_NUM        0x01    /* First FCC to be used */
#define MOT_FCC_TBD_NUM         0x40    /* transmit buffer descriptors (TBD)*/
#define MOT_FCC_RBD_NUM         0x20    /* receive buffer descriptors (RBD)*/
#define MOT_FCC_DEF_PHY_MODE    0x02    /* PHY's default operating mode */
#define MOT_FCC_BDS_SIZE        (2 * 1024)
#define MOT_FCC_MBLK_RBD_RATIO  0x03    /* Driver allocates (3* MOT_FCC_RBD_NUM) mBlk's */
#define MOT_FCC_CLUS_RBD_RATIO  0x04    /* Driver allocates (4* MOT_FCC_RBD_NUM) Clusters*/

/* imports */

IMPORT END_OBJ * motFcc2EndLoad (char *);
IMPORT UINT32    sysPCGet (void);
FUNCPTR   _func_m82xxDpramFree;
FUNCPTR   _func_m82xxDpramFccMalloc;
FUNCPTR   _func_m82xxDpramFccFree;
IMPORT STATUS    miiPhyInit (PHY_INFO * phyInfo);

/* globals */

UINT32 miiNumLinkChgInts = 0;
UINT32 miiBitRdCount     = 0;
UINT32 miiBitWrCount     = 0;

/* locals */
UINT32 fccPhyAdrs[] = { 0x0 /* FCC1 */, 0x3 /* FCC2 */};

/*
 * this table may be customized by the user to force a
 * particular order how different technology abilities may be
 * negotiated by the PHY. Entries in this table may be freely combined
 * and even OR'd together.
 */

LOCAL INT16 motFccAnOrderTbl [] = {
                                MII_TECH_100BASE_TX,    /* 100Base-T */
                                MII_TECH_100BASE_T4,    /* 10Base-T */
                                MII_TECH_10BASE_T,      /* 100Base-T4 */
                                MII_TECH_10BASE_FD,     /* 100Base-T FD*/
                                MII_TECH_100BASE_TX_FD, /* 10Base-T FD */
                                -1                      /* end of table */
                               };

/* forward declarations */


STATUS sysFccEnetEnable (UINT32 immrVal, UINT8 fccNum);
STATUS sysFccEnetCommand(UINT32 immrVal, UINT8  fccNum, UINT16  command);
STATUS sysFccMiiBitWr (UINT32 immrVal, UINT8 fccNum, INT32 bitVal);
STATUS sysFccMiiBitRd (UINT32 immrVal, UINT8 fccNum, INT8 * bitVal);
STATUS sysFccEnetDisable (UINT32 immrVal, UINT8 fccNum);
LOCAL STATUS sysMiiPhyInit    (PHY_INFO * pPhyInfo);
LOCAL void sysMiiInt(PHY_INFO * pPhyInfo);
LOCAL STATUS sysMiiPhyDuplex(PHY_INFO * pPhyInfo,int * pDuplex);
FCC_END_FUNCS fccFuncs;


/***********************************************************************
*
* sysMotFccEndLoad - load an istance of the motFccEnd driver
*
* This routine loads the motFccEnd driver with proper parameters. It also
* reads the BCSR3 to find out which type of processor is being used, and
* sets up the load string accordingly.
*
* The END device load string formed by this routine is in the following
* format.
* <immrVal>:<fccNum>:<bdBase>:<bdSize>:<bufBase>:<bufSize>:<fifoTxBase>
* :<fifoRxBase>:<tbdNum>:<rbdNum>:<phyAddr>:<phyDefMode>:<pAnOrderTbl>:
* <userFlags>:<mBlkRatio>:<mBlkRatio>
*
* .IP <immrVal>
* Internal memory address
* .IP <fccNum>
* FCC number being used
* .IP <bdBase>
* buffer descriptors base address
* .IP <bdSize>
* buffer descriptors space size
* .IP <bufBase>
* data buffers base address
* .IP <bufSize>
* data buffers space size
* .IP <fifoTxBase>
* tx buffer in internal memory
* .IP <fifoRxBase>
* rx buffer in internal memory
* .IP <tbdNum>
* number of TBDs or NONE
* .IP <rbdNum>
* number of RBDs or NONE
* .IP <phyAddr>
* address of a MII-compliant PHY device
* .IP <phyDefMode>
* default operating mode of a MII-compliant PHY device
* .IP <pAnOrderTbl>
* auto-negotiation order table for a MII-compliant PHY device or NONE
* .IP <userFlags>
* User flages
* .IP <mBlkRatio>
* Ratio between mBlk's and Rx BD's
* .IP <clusterRatio>
* Ratio between Clusters and Rx BD's
*
* This routine only loads and initializes one instance of the device.
* If the user wishes to use more than one motFccEnd devices, this routine
* should be changed.
*
* RETURNS: pointer to END object or NULL.
*
* SEE ALSO: motFccEndLoad ()
*/

END_OBJ * sysMotFccEndLoad
    (
    char * pParamStr,   /* ptr to initialization parameter string */
    void * unused       /* unused optional argument */
    )
    {
    /*
     * The motFccEnd driver END_LOAD_STRING should be:
     * <immrVal>:<fccNum>:<bdBase>:<bdSize>:<bufBase>:<bufSize>:<fifoTxBase>
     * :<fifoRxBase>:<tbdNum>:<rbdNum>:<phyAddr>:<phyDefMode>
     * :<pAnOrderTbl>:<userFlags>:<mBlkRatio>:<clusterRatio>
     * Note that unit string is prepended by the mux, so we
     * don't put it here.
     */

    char * pStr = NULL;
    char   paramStr [300];
    UINT32 regPC;
    UINT32 motFccBufsDescAdr;
    UINT32 motFccBufsDescSize;
    UINT32 motFccBufsAdr;
    UINT32 motFccBufsSize;
    UINT32 motFccFlags;

    static char motFccEndParamTemplate [] =
    "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x";

    END_OBJ * pEnd;

    int fccUnit=0;
    char *  tok;                /* an initString token */
    char *  holder = NULL;      /* points to pParamStr fragment beyond tok */

#ifdef TAIFAN_DBG
    fccFuncs.miiPhyInit = sysMiiPhyInit;       /* bsp MiiPhy init function */
    fccFuncs.miiPhyInt = NULL;        /* Driver function for BSP to call */
    fccFuncs.miiPhyBitRead = (FUNCPTR)sysFccMiiBitRd;    /* Bit Read funtion */
    fccFuncs.miiPhyBitWrite = (FUNCPTR)sysFccMiiBitWr;   /* Bit Write function */
    fccFuncs.miiPhyDuplex = sysMiiPhyDuplex;     /* duplex status call back */
    fccFuncs.miiPhySpeed = NULL;      /* speed status call back */
    fccFuncs.hbFail = NULL;           /* heart beat fail */
    fccFuncs.intDisc = NULL;          /* disconnect Function */
    fccFuncs.dpramFree = NULL;
    fccFuncs.dpramFccMalloc = NULL;
    fccFuncs.dpramFccFree = NULL;
#endif
    fccFuncs.miiPhyInit     = NULL;       /* bsp MiiPhy init function */
    fccFuncs.miiPhyInt      = NULL;        /* Driver function for BSP to call */
    fccFuncs.miiPhyBitRead  = NULL;    /* Bit Read funtion */
    fccFuncs.miiPhyBitWrite = NULL;   /* Bit Write function */
    fccFuncs.miiPhyDuplex   = NULL;     /* duplex status call back */
    fccFuncs.miiPhySpeed    = NULL;      /* speed status call back */
    fccFuncs.hbFail         = NULL;           /* heart beat fail */
    fccFuncs.intDisc        = NULL;          /* disconnect Function */
    fccFuncs.dpramFree      = NULL;
    fccFuncs.dpramFccMalloc = NULL;
    fccFuncs.dpramFccFree   = NULL;

    _func_m82xxDpramFree = (FUNCPTR)m82xxDpramFree;
    _func_m82xxDpramFccMalloc = (FUNCPTR)m82xxDpramFccMalloc;
    _func_m82xxDpramFccFree = (FUNCPTR)m82xxDpramFccFree;

    if (strlen (pParamStr) == 0)
        {
        /*
         * muxDevLoad() calls us twice.  If the string is
         * zero length, then this is the first time through
         * this routine.
         */

        pEnd = (END_OBJ *) motFcc2EndLoad  (pParamStr);
        }
    else
        {
	regPC = sysPCGet ();

	if ((regPC & ROM_BASE_ADRS) == ROM_BASE_ADRS)
	    {
            /* We are running from ROM */

	    /*
             * ROM resident applications MUST use the option, wail RAM resident
             * MAY use the option, which forces all
             * driver buffers (rx and tx) to reside in local bus space. The
             * drawback to having buffers on the local bus is that the driver
             * MUST copy all transmit packets to the local bus -- so zcopy
             * must be inhibited. The driver still passes rx data zcopy.
             */

	    /* for ROM apps, specify local bus space for the buffers */
            motFccBufsAdr  = LOCAL_MEM_LOCAL_ADRS + (LOCAL_MEM_SIZE >> 1);
            motFccBufsSize = LOCAL_MEM_SIZE >> 1;

            /* for ROM apps, MOT_FCC_FLAGS *must* include the following. */
            motFccFlags = 0x0000D000; /*(MOT_FCC_USR_BUF_LBUS|MOT_FCC_USR_NO_ZCOPY|MOT_FCC_USR_DPRAM_ALOC)*/

        /*
             * NOTE! The FCC driver internally forces MOT_FCC_USR_NO_ZCOPY if
             * MOT_FCC_USR_BUF_LBUS is set in MOT_FCC_FLAGS, making the
         * inclusion of 'NO_ZCOPY not strictly required. The 'NO_ZCOPY
         * flag can be used independently of the bus on which buffers
         * reside -- for those rare apps that might need this option.
             */
        }
    else
        {
            /* We are running from RAM */

            /* for RAM apps, let the driver find space for the buffers */
            motFccBufsAdr  = -1UL;
            motFccBufsSize = -1UL;
            motFccFlags    = 0x00008000; /* MOT_FCC_USR_DPRAM_ALOC */
            }
    /*
         * NOTE! Regarding bus choices for BDs and buffers.
         * If you give a local bus address for BDs and don't include
         * MOT_FCC_USR_BD_LBUS in MOT_FCC_FLAGS, the CPM will hang.
         * If you give a local bus address for buffers and don't include
         * MOT_FCC_USR_BUF_LBUS in MOT_FCC_FLAGS, the CPM will hang.
         * If you give a 60x bus address for buffers and/or BDs and
         * mistakenly include either a 'LBUS flag, the CPM will hang.
         */

    /*
         * If you want the driver to decide where to put the BDs, set BOTH of the
     * following motFccBufsDescAdr & motFccBufsDescSize to -1UL.
     * If you decide to force the driver to use BDs on the local bus, set the
         * following motFccBufsDescAdr to a local bus address and size AND be sure to add
         * MOT_FCC_USR_BD_LBUS to the definition of MOT_FCC_FLAGS below.
         * The defaults use internal DPRAM for BDS. You may optionally specify
         * a 60x bus address for BDS.  In these cases do not set the LBUS flag.
     * Because the Wind River ATM driver need as much as posible free DPRAM, when using
     * the ATM driver the BDs will be on the 60x bus.
         */

    motFccBufsDescSize = MOT_FCC_BDS_SIZE;

    #ifdef INCLUDE_ATM
    motFccBufsDescAdr = -1UL;
    #else /* INCLUDE_ATM is not defined */
    if ((motFccBufsDescAdr = (UINT32)m82xxDpramAlignedMalloc (MOT_FCC_BDS_SIZE,8)) == 0)
        {
        motFccBufsDescAdr = -1UL;
            }
    #endif /* INCLUDE_ATM */

        /*
         * On the second pass through here, we actually create
         * the initialization parameter string on the fly.
         * Note that we will be handed our unit number on the
         * second pass and we need to preserve that information.
         * So we use the unit number handed from the input string.
         */

        pStr = strcpy (paramStr, pParamStr);

        /* Now, we get to the end of the string */

        pStr += strlen (paramStr);

        tok = strtok_r (pParamStr, ":", &holder);

        if (tok == NULL)
           {
           return (END_OBJ*)NULL;
           }

        fccUnit = ((int) strtoul (tok, NULL, 16));

    /* finish off the initialization parameter string */
        sprintf (pStr, motFccEndParamTemplate,
            (UINT) vxImmrGet (),
            fccUnit + MOT_FCC_BASE_NUM,
            motFccBufsDescAdr,
            motFccBufsDescSize,
            motFccBufsAdr,
            motFccBufsSize,
            -1UL,-1UL,
            MOT_FCC_TBD_NUM,
            MOT_FCC_RBD_NUM,
            fccPhyAdrs[fccUnit],
            MOT_FCC_DEF_PHY_MODE,
            &motFccAnOrderTbl,
            motFccFlags,
	    &fccFuncs,
            MOT_FCC_MBLK_RBD_RATIO,
            MOT_FCC_CLUS_RBD_RATIO
         );

        if ((pEnd = (END_OBJ *) motFcc2EndLoad  (paramStr)) == (END_OBJ *)NULL)
            {
            logMsg ("Error: motFccEndLoad  failed to load driver\n", 0, 0, 0, 0, 0, 0);
            }
        }
    return pEnd;
    }


/***********************************************************************
*
* sysMiiPhyDuplex - check if link is Full Duplex
*
* This routine check if the link is Full Duplex or not
*
* RETURNS: 1 if Ful Duplex or 0 if not.
*/

LOCAL STATUS sysMiiPhyDuplex
    (
    PHY_INFO * pPhyInfo,
    int      * pDuplex
    )
    {
    UINT16 miiStat;
    int    retVal;

#if 0
    MII_READ(pPhyInfo->phyAddr,17,&miiStat,retVal);
#endif

    if (retVal == OK)
      {
      if ( (miiStat & 0x5000) !=0 )
         * pDuplex = 1;
      else
         * pDuplex = 0;
      }

    return retVal;
    }

/***********************************************************************
*
* sysMiiInt - MII interrupt service routine
*
* This routine check if the link up or down and update a flag
*
* RETURNS: None.
*/

LOCAL void sysMiiInt
    (
    PHY_INFO * pPhyInfo
    )
    {
    UINT16 miiIntStatusReg;
    int    tmp;

#if 0
    /* Clear MII interrupt by reading Int status reg */
    MII_READ(pPhyInfo->phyAddr,21,&miiIntStatusReg,tmp);
#endif
    ++miiNumLinkChgInts;
    
    fccFuncs.miiPhyInt(pPhyInfo->pDrvCtrl);
    
    }

/***********************************************************************
*
* sysMiiPhyInit - initialize and configure the PHY devices
*
* This routine scans, initializes and configures the PHY device.
*
* RETURNS: OK, or ERROR.
*/


LOCAL STATUS sysMiiPhyInit
    (
    PHY_INFO * pPhyInfo
    )
    {
    int    retVal,tmp;
    VUINT16 miiSpecial,miiIntEnableReg;

    retVal = miiPhyInit (pPhyInfo);

    if (retVal == OK)
        {
#if 0
        MII_READ(pPhyInfo->phyAddr,18,&miiIntEnableReg,tmp);
#endif

        if (tmp == OK)
            {
#ifdef TSS
            intConnect (INUM_TO_IVEC(INUM_IRQ2),sysMiiInt,(int)pPhyInfo);
#else
            intConnect (INUM_TO_IVEC(INUM_IRQ5),sysMiiInt,(int)pPhyInfo);
#endif
            miiIntEnableReg = 0x0800; /* Full Duplex interrupt */
#if 0
            MII_WRITE(pPhyInfo->phyAddr, 21, miiIntEnableReg, tmp);
#endif
            if (tmp == OK)
                {
#ifdef TSS
                intEnable (INUM_IRQ2);
#else
                intEnable (INUM_IRQ5);
#endif
                }
            }
        }

#if 0
    miiSpecial = 0x0400;
    MII_WRITE(pPhyInfo->phyAddr,16,miiSpecial,retVal);
#endif
    return retVal;
    }

/***********************************************************************
*
* sysFccEnetEnable - enable the MII interface to the FCC controller
*
* This routine is expected to perform any target specific functions required
* to enable the Ethernet device and to connect the MII interface to the FCC.
*
* RETURNS: OK, or ERROR if the FCC controller cannot be enabled.
*/

STATUS sysFccEnetEnable
    (
    UINT32  immrVal,    /* base address of the on-chip RAM */
    UINT8   fccNum  /* FCC being used */
    )
    {
    int             intLevel;

    intLevel = intLock ();


    /* assert the enable signal */

    if (fccNum==2)
      {
#if 0 /* BSP_RMV */
	*BCSR3 |= 0x08000000;
	*BCSR3 &= ~0x10000000;
#endif

#if 0
    /* 	PC20 : CLK12 */
	*M8260_IOP_PCPAR(immrVal) |=  (PC18);
	*M8260_IOP_PCSO(immrVal)  &= ~(PC18);
	*M8260_IOP_PCDIR(immrVal) &= ~(PC18);

    /* PB29 TX_EN, PB30 RX_DV, PB28 RX_ER */
    /* PB23, PB22 : TX Data */
    /* PB21, PB20 : RX Data*/
	*M8260_IOP_PBSO(immrVal)  &= ~(PB30 | PB28);	

	*M8260_IOP_PBSO(immrVal)  |= (PB29);	
	*M8260_IOP_PBDIR(immrVal) |= (PB29 | PB23 | PB22);
	*M8260_IOP_PBPAR(immrVal) |= (PB30 | PB29 | PB28 | PB23 | PB22 | PB21 | PB20);	
    
    
	/* connect FCC2 clocks */

	*M8260_CMXFCR (immrVal)  &= ~0x00ff0000;
	*M8260_CMXFCR (immrVal)  |= (M8260_CMXFCR_T2CS_CLK14);

	/* NMSI mode */

	*M8260_CMXFCR (immrVal)  &= ~(M8260_CMXFCR_FC2_MUX);
#else
    /* 	PC20 : CLK12 */
	*M8260_IOP_PCPAR(immrVal) |=  (PC18);
	*M8260_IOP_PCSO(immrVal)  &= ~(PC18);
	*M8260_IOP_PCDIR(immrVal) &= ~(PC18);

    /* PB29 TX_EN, PB30 RX_DV, PB28 RX_ER */
    /* PB23, PB22 : TX Data */
    /* PB21, PB20 : RX Data*/
	*M8260_IOP_PBSO(immrVal)  &= ~(PB30);	

	*M8260_IOP_PBSO(immrVal)  |= (PB29);	
	*M8260_IOP_PBDIR(immrVal) |= (PB29 | PB23 | PB22);
	*M8260_IOP_PBPAR(immrVal) |= (PB30 | PB29 | PB23 | PB22 | PB21 | PB20);	
    
    
	/* connect FCC2 clocks */

	*M8260_CMXFCR (immrVal)  &= ~0x00ff0000;
	*M8260_CMXFCR (immrVal)  |= (M8260_CMXFCR_T2CS_CLK14);

	/* NMSI mode */

	*M8260_CMXFCR (immrVal)  &= ~(M8260_CMXFCR_FC2_MUX);
#endif

    }

    if (fccNum==1)
      {
#if 0 /*BSP_RMV */
	*BCSR1 &= ~0x08000000;
	*BCSR1 |= 0x04000000;
#endif

#if 0
    /* 	PC20 : CLK12 */
	*M8260_IOP_PCPAR(immrVal) |=  (PC20);
	*M8260_IOP_PCSO(immrVal)  &= ~(PC20);
	*M8260_IOP_PCDIR(immrVal) &= ~(PC20);

    /* PA28 TX_EN, PA27 RX_DV, PA26 RX_ER */
    /* PA19, PA18 : TX Data */
    /* PA17, PA16 : RX Data*/

	*M8260_IOP_PASO(immrVal)  |= (PA28 | PA27 | PA26);	
	*M8260_IOP_PADIR(immrVal) |= (PA28 | PA19 | PA18);
	*M8260_IOP_PAPAR(immrVal) |= (PA28 | PA27 | PA26 | PA19 | PA18 | PA17 | PA16);	
    
    
	/* connect FCC1 clocks */

	*M8260_CMXFCR (immrVal)  &= ~0xff000000;
	*M8260_CMXFCR (immrVal)  |= (M8260_CMXFCR_T1CS_CLK12);

	/* NMSI mode */

	*M8260_CMXFCR (immrVal)  &= ~(M8260_CMXFCR_FC1_MUX);
#else
    /* 	PC20 : CLK12 */
	*M8260_IOP_PCPAR(immrVal) |=  (PC20);
	*M8260_IOP_PCSO(immrVal)  &= ~(PC20);
	*M8260_IOP_PCDIR(immrVal) &= ~(PC20);

    /* PA28 TX_EN, PA27 RX_DV, PA26 RX_ER */
    /* PA19, PA18 : TX Data */
    /* PA17, PA16 : RX Data*/

	*M8260_IOP_PASO(immrVal)  |= (PA28 | PA27);	
	*M8260_IOP_PADIR(immrVal) |= (PA28 | PA19 | PA18);
	*M8260_IOP_PAPAR(immrVal) |= (PA28 | PA27 | PA19 | PA18 | PA17 | PA16);	
    
    
	/* connect FCC1 clocks */

	*M8260_CMXFCR (immrVal)  &= ~0xff000000;
	*M8260_CMXFCR (immrVal)  |= (M8260_CMXFCR_T1CS_CLK12);

	/* NMSI mode */

	*M8260_CMXFCR (immrVal)  &= ~(M8260_CMXFCR_FC1_MUX);
#endif	         
      }

    intUnlock (intLevel);

    taskDelay (sysClkRateGet() >> 2);


    return(OK);
    }

/***********************************************************************
*
* sysFccEnetDisable - disable MII interface to the FCC controller
*
* This routine is expected to perform any target specific functions required
* to disable the Ethernet device and the MII interface to the FCC
* controller.  This involves restoring the default values for all the Port
* B and C signals.
*
* RETURNS: OK, always.
*/

STATUS sysFccEnetDisable
    (
    UINT32  immrVal,    /* base address of the on-chip RAM */
    UINT8   fccNum  /* FCC being used */
    )
    {
    volatile UINT32 tmp;
    int             intLevel;

    intLevel = intLock ();

    /*
     * configure all Port B and C pins previously used as general
     * purpose input pins
     */

#ifdef TAIFAN_ORG
    *M8260_IOP_PBPAR(immrVal) &= ~(PB15 | PB14);    /* clear PAR bits for xcvr RST and PWRDWN pins */
    *M8260_IOP_PBDIR(immrVal) |=  (PB15 | PB14);    /* make them outputs */

    tmp = *M8260_IOP_PBDAT(immrVal);        /* get current port B data */
    tmp |=  (PB14);         /* drive the active high PWRWN pin high */
    tmp &= ~(PB15);         /* drive the active low RST low too */
    *M8260_IOP_PBDAT(immrVal) = tmp;
#endif


    if (fccNum==2)
        {
#ifdef TAIFAN_ORG
	*M8260_IOP_PCPAR(immrVal) &= ~(PC17 | PC16);

	*M8260_IOP_PCDIR(immrVal) &= ~(PC10);

	*M8260_IOP_PBPAR(immrVal) &= ~(PB31 | PB30 | PB29 | PB28 | PB27 | 
				       PB26 | PB25 | PB24 | PB23 | PB22 | 
				       PB21 | PB20 | PB19 | PB18);
	*M8260_IOP_PBDIR(immrVal) &= ~(PB31 | PB29 | PB25 | PB24 | PB23 | 
				       PB22);
#endif
        /* Disable the interrupt */
	intDisable (INUM_FCC2);
#if 0 /*BSP_RMV */
	 /* de-assert the enable signal */
	*BCSR3 |= 0x10000000;
	 /* assert the reset signal */
	*BCSR3 &= ~0x08000000;
#endif
	}

    if (fccNum==1)
        {
#ifdef TAIFAN_ORG
	*M8260_IOP_PCPAR(immrVal) &=  ~(PC21 | PC20);

	*M8260_IOP_PASO(immrVal) &= ~(PD29 | PA28 | PA27 | PA26);	
	*M8260_IOP_PADIR(immrVal) &= ~(PB29 | PB28 | PB21 | PB20 | PB19 | PB18);
	*M8260_IOP_PCSO(immrVal) &= ~(PC15 | PC14 | PC13 | PC12 | PC7 | PC6);
	*M8260_IOP_PCDIR(immrVal) &= ~(PC15 | PC14 | PC13 | PC12 | PC7 | PC6); 
	*M8260_IOP_PDSO(immrVal) &= ~(PD29 | PD7);	
	*M8260_IOP_PDDIR(immrVal) &= ~(PD29 | PD19 | PD18 | PD7);	
	*M8260_IOP_PAPAR(immrVal) &= ~(PA29 | PA28 | PA27 | PA26 | PA21 | PA20 |PA19 | PA18 | PA17 | PA16 | PA15 | PA14);	
	*M8260_IOP_PCPAR(immrVal) &= ~(PC15 | PC14 | PC13 | PC12 | PC7 | PC6);	
	*M8260_IOP_PDPAR(immrVal) &= ~(PD29 | PD19 | PD18);
#endif
	 /* Disable the interrupt */
	intDisable (INUM_FCC1);
#if 0 /*BSP_RMV */
	 /* de-assert the enable signal */
	*BCSR1 |= 0x08000000;
	 /* assert the reset signal */
	*BCSR1 &= ~0x04000000;
#endif
	}

    intUnlock (intLevel);

    return(OK);
    }


/***********************************************************************
*
* sysFccEnetCommand - issue a command to the Ethernet interface controller
*
* RETURNS: OK, or ERROR if the Ethernet controller could not be restarted.
*/

STATUS sysFccEnetCommand
    (
    UINT32      immrVal,        /* base address of the on-chip RAM */
    UINT8   fccNum,     /* FCC being used */
    UINT16  command
    )
    {
    return (OK);
    }

#ifndef NSDELAY
#define MOTOROLA_FCC_LOOP_NS 2

#define NSDELAY(nsec)                                                   \
    {                                                                   \
    volatile int nx = 0;                                                \
    volatile int loop = (int)(nsec*MOTOROLA_FCC_LOOP_NS);               \
                                                                        \
    for (nx = 0; nx < loop; nx++);                                      \
    }
#endif /* NSDELAY */

/***********************************************************************
*
* sysFccMiiBitWr - write one bit to the MII interface
*
* This routine writes the value in <bitVal> to the MDIO line of a MII
* interface. The MDC line is asserted for a while, and then negated.
* If <bitVal> is NONE, then the MDIO pin should be left in high-impedance
* state.
*
* SEE ALSO: sysFccMiiBitRd()
*
* RETURNS: OK, or ERROR.
*/
STATUS sysFccMiiBitWr
    (
    UINT32      immrVal,        /* base address of the on-chip RAM */
    UINT8   fccNum,     /* FCC being used */
    INT32        bitVal          /* the bit being written */
    )
    {
    miiBitWrCount++;
#if 0
    /*
     * we create the timing reference for transfer of info on the MDIO line
     * MDIO is mapped on PC18, MDC on PC19. We need to keep the same data
     * on MDIO for at least 400 nsec.
     */
    *M8260_IOP_PCPAR(immrVal) &= ~(PC19 | PC18);
    *M8260_IOP_PCDIR(immrVal) |= (PC19 | PC18);
    *M8260_IOP_PCDAT(immrVal) |= (PC19);

    switch (bitVal)
    {
    case 0:
        *M8260_IOP_PCDAT(immrVal) &= ~(PC18);
        break;
    case 1:
        *M8260_IOP_PCDAT(immrVal) |= (PC18);
        break;
    case ((INT32) NONE):
        /* put it in high-impedance state */
        *M8260_IOP_PCDIR(immrVal) &= ~(PC18);
        break;
    default:
        return (ERROR);
    }

    /* delay about 200 nsec. */

    NSDELAY (200);

    /* now we toggle the clock and delay again */

    *M8260_IOP_PCDAT(immrVal) &= ~(PC19);
#else
    /*
     * we create the timing reference for transfer of info on the MDIO line
     * MDIO is mapped on PA22, MDC on PA23. We need to keep the same data
     * on MDIO for at least 400 nsec.
     */
    *M8260_IOP_PAPAR(immrVal) &= ~(PA23 | PA22);
    *M8260_IOP_PADIR(immrVal) |= (PA23 | PA22);
    *M8260_IOP_PADAT(immrVal) |= (PA23);

    switch (bitVal)
    {
    case 0:
        *M8260_IOP_PADAT(immrVal) &= ~(PA22);
        break;
    case 1:
        *M8260_IOP_PADAT(immrVal) |= (PA22);
        break;
    case ((INT32) NONE):
        /* put it in high-impedance state */
        *M8260_IOP_PADIR(immrVal) &= ~(PA22);
        break;
    default:
        return (ERROR);
    }

    /* delay about 200 nsec. */

    NSDELAY (200);

    /* now we toggle the clock and delay again */

    *M8260_IOP_PADAT(immrVal) &= ~(PA23);
#endif
    NSDELAY (200);

    return (OK);
    }

/***********************************************************************
*
* sysFccMiiBitRd - read one bit from the MII interface
*
* This routine reads one bit from the MDIO line of a MII
* interface. The MDC line is asserted for a while, and then negated.
*
* SEE ALSO: sysFccMiiBitWr()
*
* RETURNS: OK, or ERROR.
*/

STATUS sysFccMiiBitRd
    (
    UINT32      immrVal,        /* base address of the on-chip RAM */
    UINT8   fccNum,     /* FCC being used */
    INT8 *      bitVal          /* the bit being read */
    )
    {
    miiBitRdCount++;
#if 0
    /*
     * we create the timing reference for transfer of info on the MDIO line
     * MDIO is mapped on PC18, MDC on PC19. We can read data on MDIO after
     * at least 400 nsec.
     */

    *M8260_IOP_PCPAR(immrVal) &= ~(PC19 | PC18);
    *M8260_IOP_PCDIR(immrVal) &= ~(PC18);

    *M8260_IOP_PCDIR(immrVal) |= (PC19);
    *M8260_IOP_PCDAT(immrVal) |= (PC19);

    /* delay about 200 nsec. */

    NSDELAY (200);

    /* now we toggle the clock and delay again */

    *M8260_IOP_PCDAT(immrVal) &= ~(PC19);

    NSDELAY (200);

    /* we can now read the MDIO data on PC18 */

    *bitVal = (*M8260_IOP_PCDAT(immrVal) & (PC18)) >> 13;
#else
    /*
     * we create the timing reference for transfer of info on the MDIO line
     * MDIO is mapped on PA22, MDC on PA23. We can read data on MDIO after
     * at least 400 nsec.
     */

    *M8260_IOP_PAPAR(immrVal) &= ~(PA23 | PA22);
    *M8260_IOP_PADIR(immrVal) &= ~(PA22);

    *M8260_IOP_PADIR(immrVal) |= (PA23);
    *M8260_IOP_PADAT(immrVal) |= (PA23);

    /* delay about 200 nsec. */

    NSDELAY (200);

    /* now we toggle the clock and delay again */

    *M8260_IOP_PADAT(immrVal) &= ~(PA23);

    NSDELAY (200);

    /* we can now read the MDIO data on PA22 */

    *bitVal = (*M8260_IOP_PADAT(immrVal) & (PA22)) >> 9;
#endif
    return (OK);
    }

#endif /* INCLUDE_MOTFCCEND */

