/* sysSerial.c - BSP serial device initialization */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,10dec01,g_h  Move to use the generic m82xxDpramLib
01c,17jul01,g_h  changed to use the dynamic DPRAM memory allocation
01b,26apr99,elk  added SMC2.
01a,05mar99,elk  adopted from sysSerial/estMDP8xx (ver 01a).
*/

#include "vxWorks.h"
#include "iv.h"
#include "intLib.h"
#include "config.h"
#include "sysLib.h"
#include "smc8260Sio.h"
#include "drv/mem/m82xxDpramLib.h"
#include "drv/sio/m8260Cp.h"
#include "drv/sio/m8260CpmMux.h"
#include "drv/mem/m8260Siu.h"
#include "drv/intrCtl/m8260IntrCtl.h"
#include "drv/parallel/m8260IOPort.h"

#include "smc8260Sio.c"

/* Global data    */

static PPC8260SMC_CHAN ppcChn[N_SIO_CHANNELS] ;

/* External References */
extern uint32_t sysBaudClkFreq(void);

/******************************************************************************
*
* sysSerialHwInit - initialize the BSP serial devices to a quiesent state
*
* This routine initializes the BSP serial device descriptors and puts the
* devices in a quiesent state.  It is called from sysHwInit() with
* interrupts locked.
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit()
*/ 

void sysSerialHwInit
    ( 
    void 
    )
    {
    int    i;
    UINT32 dpram1Base;
    UINT32 allocatedAdrs;
    int immrVal = vxImmrGet();

    /* SMC2: PA9 TX, PA8 RX */
    * M8260_IOP_PADIR(immrVal) &= ~(PA8);  
    * M8260_IOP_PADIR(immrVal) |=  (PA9);   	
    * M8260_IOP_PAPAR(immrVal) |=  (PA8 | PA9); 
    * M8260_IOP_PASO(immrVal)  &= ~(PA8 | PA9); 	  

    for (i = 0; i < N_SIO_CHANNELS; i++)
         {
         /*
          * intialize the chips device descriptors.
          */

         ppcChn[i].regBase        = vxImmrGet ()       ; /* IMMR reg has base adr */
         ppcChn[i].clockRate      = sysBaudClkFreq ()  ; /* Baud clock freq (Hz)  */
#if 0 /* BSP_MOD */
         ppcChn[i].bgrNum         = 1 + i              ; /* use BRGx              */
         ppcChn[i].uart.smcNum    = 1 + i              ; /* SMCx wired for rs232  */
#else
         ppcChn[i].bgrNum         = 2 + i              ; /* use BRG2              */
         ppcChn[i].uart.smcNum    = 2 + i              ; /* SMC2 wired for rs232  */
#endif
         ppcChn[i].uart.txBdNum   = 1                  ; /* use 1 transmit BD     */
         ppcChn[i].uart.rxBdNum   = 0x10               ; /* use 16 receive BD     */

         /* transmit BD base adrs */
	 if ((allocatedAdrs = (0x0000ffff & (UINT32)m82xxDpramAlignedMalloc (8,8))) != 0)
	      ppcChn[i].uart.txBdBase  = (SMC_BUF *) allocatedAdrs;
	 else
	     {
	     sysToMonitor (BOOT_NO_AUTOBOOT);
	     }

         /* receive BD base adrs  */
         if ((allocatedAdrs = (0x0000ffff & (UINT32)m82xxDpramAlignedMalloc (128,8))) != 0)
	     ppcChn[i].uart.rxBdBase  = (SMC_BUF *) allocatedAdrs;
         else
	     {
	     sysToMonitor (BOOT_NO_AUTOBOOT);
             }

         ppcChn[i].uart.txBufSize = 0x1                ; /* transmit buffer size  */

         dpram1Base = MPC8260_DPRAM1_BASE( ppcChn[i].regBase );

         if ((allocatedAdrs = (UINT32)m82xxDpramAlignedMalloc (0x80,2)) != 0)
	     ppcChn[i].uart.txBufBase = (u_char *) allocatedAdrs;
	else
            {
	    sysToMonitor (BOOT_NO_AUTOBOOT);
	    }

	if ((allocatedAdrs = (UINT32)m82xxDpramAlignedMalloc (0x80,2)) != 0)
	    ppcChn[i].uart.rxBufBase = (u_char *)allocatedAdrs;
	else
	    {
	    sysToMonitor (BOOT_NO_AUTOBOOT);
	    }

	if ((allocatedAdrs = (0x0000ffff & (UINT32)m82xxDpramAlignedMalloc (0x34,64))) != 0)
	    {
#if 0 /* BSP_SMC2 */  
            *PPC8260_DPR_SMC1(dpram1Base + (0x100 * i)) = allocatedAdrs;
#else
            *PPC8260_DPR_SMC2(dpram1Base + (0x100 * i)) = allocatedAdrs;
#endif
		}
	else
	    {
	    sysToMonitor (BOOT_NO_AUTOBOOT);
	    }


#if 0 /* BSP_SMC2 */  
        /* DPRAM address of SMCx parameter RAM. */
        ppcChn[i].uart.pSmc = (SMC *) ( (UINT32)
        (*PPC8260_DPR_SMC1(dpram1Base + (0x100 * i)) + ppcChn[i].regBase) );
#else
        ppcChn[i].uart.pSmc = (SMC *) ( (UINT32)
        (*PPC8260_DPR_SMC2(dpram1Base + (0x100 * i)) + ppcChn[i].regBase) );
#endif                                                  
        /*
         * SMCMR1 for SMC1.
         */
#if 0 /* BSP_SMC2 */  
        ppcChn[i].uart.pSmcReg = 
        (SMC_REG *) ( (UINT32) SMCMR1( ppcChn[i].regBase + (0x10 * i)) ) ;

        ppcChn[i].pBaud = (VINT32 *) BRGC1(ppcChn[i].regBase + (0x4 * i)) ;
        ppcChn[i].channelMode = 0;
#else
        ppcChn[i].uart.pSmcReg = 
        (SMC_REG *) ( (UINT32) SMCMR2( ppcChn[i].regBase + (0x10 * i)) ) ;

        ppcChn[i].pBaud = (VINT32 *) BRGC2(ppcChn[i].regBase + (0x4 * i)) ;
        ppcChn[i].channelMode = 0;    
#endif

        /*
         * reset the device.
         */

        smc8260DevInit ( &ppcChn[i] ) ;
        }
    }

/******************************************************************************
*
* sysSerialHwInit2 - connect BSP serial device interrupts
*
* This routine connects the BSP serial device interrupts.  It is called from
* sysHwInit2().  Serial device interrupts could not be connected in
* sysSerialHwInit() because the kernel memory allocator was not initialized
* at that point, and intConnect() calls malloc().
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit2()
*/ 

void sysSerialHwInit2
    ( 
    void 
    )
    {
    UINT32 immrVal = vxImmrGet ();
    int i;

    for (i = 0; i < N_SIO_CHANNELS; i++)
        {
        /* Connect serial interrupts */
#if 0 /* BSP_SMC2 */
        *SMCE1( immrVal + (0x10 * i) ) = *SMCE1( immrVal + (0x10 * i) ) ;
#else
        *SMCE2( immrVal + (0x10 * i) ) = *SMCE2( immrVal + (0x10 * i) ) ;
#endif

#if 0
        (void) intConnect ( INUM_TO_IVEC(4 + (1 * i)), (VOIDFUNCPTR) smc8260Int, (int) &ppcChn[i] ) ;
#else
        (void) intConnect ( INUM_TO_IVEC(5), (VOIDFUNCPTR) smc8260Int, (int) &ppcChn[i] ) ;
#endif
        /*
         * Enable SMCi interrupts.
         */
        *M8260_SIMR_L( immrVal ) |= (0x00001000 >> i);
        } 
    }

/******************************************************************************
*
* sysSerialChanGet - get the SIO_CHAN device associated with a serial channel
*
* This routine gets the SIO_CHAN device associated with a specified serial
* channel.
*
* RETURNS: A pointer to the SIO_CHAN structure for the channel, or ERROR
* if the channel is invalid.
*/

SIO_CHAN * sysSerialChanGet
    (
       int channel		/* serial channel - [0, 1]                  */
    )
    {
    /*                                                                      */
    /* Check the parameters for validity.                                   */
    /*                                                                      */

    if ( channel >= N_SIO_CHANNELS )
        {
	return( (SIO_CHAN *) ERROR ) ;
        }

    /*                                                                      */
    /* Return a pointer to the private space.                               */
    /*                                                                      */

    return( (SIO_CHAN *) &ppcChn[channel] ) ;
    }

/*******************************************************************************
*
* sysSerialReset - reset the serail device 
*
* This function calls sysSerialHwInit() to reset the serail device
*
* RETURNS: N/A
*
*/

void sysSerialReset
    ( 
    void 
    )
    {
    sysSerialHwInit () ;
    }

