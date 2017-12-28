/* smc8260Sio.c - Motorola MPC8260 SMC UART serial driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,22oct01,g_h  Clean warning.
01d,12oct01,g_h  Clean for T2.2
01c,07may01,g_h  rename to smc8260Sio.c
01b,26apr99,elk  added SMC1.
01a,10mar99,elk	 adapted from ppc860Sio.c (ver 01b).
*/

/*
DESCRIPTION
This is the driver for the SMCs in the internal Communications Processor (CP)
of the Motorola MPC8260.  This driver only supports the SMCs in 
asynchronous UART mode.

USAGE
A PPC8260SMC_CHAN structure is used to describe the chip.
The BSP's sysHwInit() routine typically calls sysSerialHwInit(),
which initializes all the values in the PPC8260SMC_CHAN structure (except
the SIO_DRV_FUNCS) before calling smc8260DevInit().

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chip's interrupts via intConnect().

INCLUDE FILES: smc8260Sio.h
*/

/* includes */

#include "vxWorks.h"
#include "intLib.h"
#include "errno.h"
#include "sioLib.h"
#include "drv/mem/m8260Siu.h"
#include "drv/sio/m8260Brg.h"
#include "drv/sio/m8260Cp.h"
#include "drv/sio/m8260CpmMux.h"
#include "smc8260Sio.h"
#include "logLib.h"
#include "cacheLib.h"
#include "uartdrv.h"
#include "drv/intrCtl/m8260IntrCtl.h"
IMPORT  UINT32  vxImmrGet ();

void bcopyLongs(char *source, char *destination, int nlongs);

/* defines */



/* forward declarations */

LOCAL void   smc8260ResetChannel2 (PPC8260SMC_CHAN *pChan);


/* local driver function table */



/*******************************************************************************
*
* smc8260DevInit2 - initialize the SMC
*
* This routine is called to initialize the chip to a quiescent state.
* Note that the `smcNum' field of PPC8260SMC_CHAN must be either 1 or 2.
*
* RETURNS: N/A.
*/

void smc8260DevInit2
    (
    PPC8260SMC_CHAN *pChan
    )
    {
    /* masks off this SMC's interrupt. */

#if 1 /* BSP_MOD */	
    *M8260_SIMR_L(pChan->regBase) &= (~(0x00001000 >> (pChan->uart.smcNum - 1)));
#else
    *M8260_SIMR_L(pChan->regBase) &= (~(0x00000800));
#endif
    pChan->baudRate  = 9600;
   
    }

/*******************************************************************************
*
* smc8260ResetChannel2 - reset the SMC channel
*
* This routine reset the SMC channel
*
* RETURNS: N/A.
*/

LOCAL void smc8260ResetChannel2 
    (
    PPC8260SMC_CHAN *pChan
    )
    {
    int smc;			/* the SMC number being initialized */
    int baud;			/* the baud rate generator being used */
    int frame;
    int ix = 0;
    VINT32  cpcrVal = 0;         		/* a convenience */
    int immrVal = vxImmrGet();

    int oldlevel = intLock ();	/* lock interrupts */ 

#if 0 /* BSP_MOD */
    smc  = pChan->uart.smcNum - 1;		/* get SMC number */
    baud = pChan->bgrNum - 1;			/* get BRG number */

    pChan->uart.intMask = 0x00001000 >> smc;
#else
    smc  = 1;		                    /* get SMC number */
    baud = 1;			                /* get BRG number */

    pChan->uart.intMask = 0x00000800;
#endif

    /* set up SMC as NMSI, select Baud Rate Generator */

    switch( baud ) 
	{
	default:        /* default to BRG1 */
	case 0:         /* Select BRG1 */ 
	    * CMXSMR(pChan->regBase) |= (0x00 >> (4 * smc));
	    break;
	case 1:         /* Select BRG2 */ 
	    * CMXSMR(pChan->regBase) |= (0x00 >> (4 * smc));
	    break;
	case 6:         /* select BRG7 */
	    * CMXSMR(pChan->regBase) |= (0x10 >> (4 * smc));
	    break;
	case 7:         /* select BRG8 */
	    * CMXSMR(pChan->regBase) |= (0x10 >> (4 * smc));
	    break;
        }


/******************************* BSP ADD **********************************/
    CACHE_PIPE_FLUSH ();

    /* wait until the CP is clear */

    do
        {
		cpcrVal = *M8260_CPCR(immrVal);
        if (ix++ == M8260_CPCR_LATENCY)
            break;
        } while (cpcrVal & M8260_CPCR_FLG) 
	    ;

    if (ix >= M8260_CPCR_LATENCY)
        {

        /* what to do, other than log an error? */

        }

    /* Stop transmitting on SCC, if doing so */

    cpcrVal = (M8260_CPCR_OP (M8260_CPCR_TX_STOP)
               | M8260_CPCR_SBC (M8260_CPCR_SBC_SMC1)
               | M8260_CPCR_PAGE (M8260_CPCR_PAGE_SMC1)
               | M8260_CPCR_FLG);

    *M8260_CPCR(immrVal) = cpcrVal;

    /* flush cache pipe when switching from write to read */

    CACHE_PIPE_FLUSH ();

   /* wait until the CP is clear */

    ix = 0;
    do
        {
		cpcrVal = *M8260_CPCR(immrVal);

        if (ix++ == M8260_CPCR_LATENCY)
            break;
        } while (cpcrVal & M8260_CPCR_FLG) ;

    if (ix >= M8260_CPCR_LATENCY)
        {

        /* what to do, other than log an error? */

        }
/******************************* BSP ADD **********************************/

 
    /* reset baud rate generator, wait for reset to clear... */
 
    *pChan->pBaud |= M8260_BRGC_RST;

    /* flush cache pipe when switching from write to read */

    CACHE_PIPE_FLUSH ();

    while (*pChan->pBaud & M8260_BRGC_RST);

    smc8260Ioctl2 (pChan, SIO_BAUD_SET, pChan->baudRate);

    /* set up transmit buffer descriptors */


    pChan->uart.txBdBase = (SMC_BUF *) (pChan->regBase +
			 ((UINT32) pChan->uart.txBdBase ));	
	pChan->uart.pSmc->param.tbase = (UINT16) ((UINT32) pChan->uart.txBdBase);
    pChan->uart.pSmc->param.tbptr = (UINT16) ((UINT32) pChan->uart.txBdBase);		 	 

    /* initialize each transmit buffer descriptor */

    for (frame = 0; frame < pChan->uart.txBdNum; frame++)
        {
        pChan->uart.txBdBase[frame].statusMode = BD_TX_INTERRUPT_BIT;

        pChan->uart.txBdBase[frame].dataPointer = pChan->uart.txBufBase +(frame * pChan->uart.txBufSize);
        }

        
    /* set the last BD to wrap to the first */

    pChan->uart.txBdBase[(frame - 1)].statusMode |= BD_TX_WRAP_BIT;

    
    /* set up receive buffer descriptors */

    pChan->uart.rxBdBase = (SMC_BUF *) (pChan->regBase +
		         ((UINT32) pChan->uart.rxBdBase ));

    pChan->uart.pSmc->param.rbase = (UINT16) ((UINT32) pChan->uart.rxBdBase);
    pChan->uart.pSmc->param.rbptr = (UINT16) ((UINT32) pChan->uart.rxBdBase);

    pChan->uart.rxBdNext = 0;

    /* initialize each receive buffer descriptor */

    for (frame = 0; frame < pChan->uart.rxBdNum; frame++)
        {
        pChan->uart.rxBdBase[frame].statusMode = BD_RX_EMPTY_BIT |
						 BD_RX_INTERRUPT_BIT;
        pChan->uart.rxBdBase[frame].dataLength = 2; /* char oriented */
        pChan->uart.rxBdBase[frame].dataPointer = pChan->uart.rxBufBase + frame;
        }
    /* set the last BD to wrap to the first */

    pChan->uart.rxBdBase[(frame - 1)].statusMode |= BD_RX_WRAP_BIT;


    /* set SMC attributes to standard UART mode */

    pChan->uart.pSmcReg->smcmr = (SMCMR_CLEN_9_BITS | \
				 SMCMR_SL_1_BIT    | \
				 SMCMR_SM_UART	   | \
				 SMCMR_DM_NORMAL);

    /* initialize parameter RAM area for this SMC */

    pChan->uart.pSmc->param.rfcr   = 0x10;	/* supervisor data access */
    pChan->uart.pSmc->param.tfcr   = 0x10;	/* supervisor data access */
   
    pChan->uart.pSmc->param.mrblr  = 0x1;	/* one character rx buffers */
    pChan->uart.pSmc->param.maxidl = 0x0;	/* no idle features */
    pChan->uart.pSmc->param.brkln  = 0x0;	/* no breaks received yet */
    pChan->uart.pSmc->param.brkec  = 0x0;	/* zero break condition ctr */
    pChan->uart.pSmc->param.brkcr  = 0x1;	/* xmit 1 BRK on stop */

    /* clear all events */

    pChan->uart.pSmcReg->smce |= SMCE_ALL_EVENTS;


/******************************* BSP ADD **********************************/

    CACHE_PIPE_FLUSH ();

   /* wait until the CP is clear */
    ix = 0;
    do
        {
		cpcrVal = *M8260_CPCR(immrVal);

        if (ix++ == M8260_CPCR_LATENCY)
            break;
        } while (cpcrVal & M8260_CPCR_FLG) ;

    if (ix >= M8260_CPCR_LATENCY)
        {

        /* what to do, other than log an error? */

        }

    /* Tell CP to initialize tx and rx parameters for SCC */

    cpcrVal = (M8260_CPCR_OP (M8260_CPCR_RT_INIT)
               | M8260_CPCR_SBC (M8260_CPCR_SBC_SMC1)
               | M8260_CPCR_PAGE (M8260_CPCR_PAGE_SMC1)
               | M8260_CPCR_MCN (M8260_CPCR_MCN_ETH)
               | M8260_CPCR_FLG);

	*M8260_CPCR(immrVal) = cpcrVal;

    CACHE_PIPE_FLUSH ();

    /* wait until the CP is clear */

    ix = 0;
    do
        {
		cpcrVal = *M8260_CPCR(immrVal);

        if (ix++ == M8260_CPCR_LATENCY)
            break;
        } while (cpcrVal & M8260_CPCR_FLG) ;

    if (ix >= M8260_CPCR_LATENCY)
        {

        /* what to do, other than log an error? */

        }

    CACHE_PIPE_FLUSH ();
/******************************* BSP ADD **********************************/


    /* enables the transmitter and receiver  */

    pChan->uart.pSmcReg->smcmr |= SMCMR_TEN | SMCMR_REN ;
   /* | SMCMR_DM_LCLB;  loopback */

    /* unmask interrupt (Tx, Rx only) */

    pChan->uart.pSmcReg->smcm  = SMCM_TX_MSK | SMCM_RX_MSK;
    *M8260_SIMR_L(pChan->regBase) |= pChan->uart.intMask;

                     
    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* smc8260Ioctl2 - special device control
*
* This routine is the special device control.
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*/

STATUS smc8260Ioctl2
    (
    PPC8260SMC_CHAN *	pChan,		/* device to control */
    int			request,	/* request code */
    int			arg		/* some argument */
    )
    {
    int		baudRate;
    int 	oldlevel;
    STATUS 	status = OK;

    switch (request)
	{
	case SIO_BAUD_SET:
            if (arg >=  SMC_MIN_BAUD && arg <= SMC_MAX_BAUD)
		{
		/* calculate proper counter value, then enable BRG */

		baudRate = (pChan->clockRate + (8 * arg)) / (16 * arg);
		if (--baudRate > 0xfff)
		    *pChan->pBaud = (BRGC_CD_MSK &
		    (((baudRate + 8) / 16) << M8260_BRGC_CD_SHIFT)) | M8260_BRGC_EN |
		    M8260_BRGC_DIV16;
                else
                    *pChan->pBaud = (BRGC_CD_MSK & 
			(baudRate << 1)) | M8260_BRGC_EN;

                pChan->baudRate = arg;
		}
            else
	        status = EIO;
	    break;
    
	case SIO_BAUD_GET:
	    * (int *) arg = pChan->baudRate;
	    break;

	case SIO_MODE_SET:
            if (!((int) arg == SIO_MODE_POLL || (int) arg == SIO_MODE_INT))
                {
                status = EIO;
                break;
                }

            /* lock interrupt  */

            oldlevel = intLock();

            /* initialize channel on first MODE_SET */

            if (!pChan->channelMode)
                smc8260ResetChannel2(pChan);

            /*
             * if switching from POLL to INT mode, wait for all characters to
             * clear the output pins
             */

            if ((pChan->channelMode == SIO_MODE_POLL) && (arg == SIO_MODE_INT))
                {
		int i;

                for (i=0; i < pChan->uart.txBdNum; i++)
                
                    while (pChan->uart.txBdBase
                           [(pChan->uart.txBdNext + i) % pChan->uart.txBdNum].
                           statusMode & BD_TX_READY_BIT);

                           
                }

            if (arg == SIO_MODE_INT)
		{
                * M8260_SIPNR_L(pChan->regBase) = pChan->uart.intMask;
				/* reset the SMC's interrupt status bit */

		* M8260_SIMR_L(pChan->regBase) |= pChan->uart.intMask;
				/* enable this SMC's interrupt  */

		pChan->uart.pSmcReg->smce = SMCE_RX;	
				/* reset the receiver status bit */ 

                pChan->uart.pSmcReg->smcm = SMCM_RX_MSK | SMCM_TX_MSK;
				/* enables receive and transmit interrupts */
		}
            else
		{
                pChan->uart.pSmcReg->smcm = 0;
				/* mask off the receive and transmit intrs */

		* M8260_SIMR_L(pChan->regBase) &= (~(pChan->uart.intMask));
				/* mask off this SMC's interrupt */ 

                }

            pChan->channelMode = arg;

            intUnlock(oldlevel);

            break;

        case SIO_MODE_GET:
            * (int *) arg = pChan->channelMode;
	    break;

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    break;

	default:
	    status = ENOSYS;
	}

    return (status);
    }

/*******************************************************************************
*
* smc8260Int1 - handle an SMC interrupt
*
* This routine is called to handle SMC interrupts.
*
* RETURNS: N/A
*/

void smc8260Int1
    (
    PPC8260SMC_CHAN *pChan
    )
    {
    char		outChar;
    FAST UINT16		dataLen = 0;


    /* check for a receive event */

    if (pChan->uart.pSmcReg->smce & SMCE_RX)
	{
        pChan->uart.pSmcReg->smce = SMCE_RX;

	while (!(pChan->uart.rxBdBase [pChan->uart.rxBdNext].statusMode &
		 BD_RX_EMPTY_BIT))
	 
	    {
	    /* process all filled receive buffers */

	    outChar = pChan->uart.rxBdBase[pChan->uart.rxBdNext].dataPointer[0];
            pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode |=
                BD_RX_EMPTY_BIT;

            /* incr BD count */

            pChan->uart.rxBdNext = (pChan->uart.rxBdNext + 1) %
                                  pChan->uart.rxBdNum;

            /* acknowledge interrupt ??? multiple events ??? */

            pChan->uart.pSmcReg->smce = SMCE_RX;
#if 0   

        logMsg("%c\r\n", outChar, 0, 0, 0,0,0);      
#endif  
   
       if (fgUartReadyToRead[1])	
       { 
            msgQSend(uartMsgQ[1], &outChar, 1, NO_WAIT, MSG_PRI_NORMAL);	     	
       	    if ( outChar == '\n') 
       	    { 
       
       	      fgUartReadyToRead[1] = 0;
       	   
       	    } 
       	
       }
       
 
   
/*
	    (*pChan->putRcvChar) (pChan->putRcvArg,outChar);
*/
	    if (pChan->channelMode == SIO_MODE_POLL)
		break;
	    }
	}

    /* check for a transmit event and if a character needs to be output */

    if ((pChan->uart.pSmcReg->smce & SMCE_TX) &&
        (pChan->channelMode != SIO_MODE_POLL))
	{
        pChan->uart.pSmcReg->smce = SMCE_TX;
    
        if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK)
	    {
	    do
	        {
        
	        pChan->uart.txBdBase[pChan->uart.txBdNext].dataPointer[dataLen++]
		    = outChar;

                if (pChan->channelMode == SIO_MODE_POLL)
                    break;
	        }
	    while ((dataLen < pChan->uart.txBufSize) &&
                   ((*pChan->getTxChar) (pChan->getTxArg, &outChar)
		       == OK));
         
	    pChan->uart.txBdBase[pChan->uart.txBdNext].dataLength = dataLen;

	      /* acknowledge interrupt */

            pChan->uart.pSmcReg->smce = SMCE_TX;

	    /* send transmit buffer */

	    pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode |=
	        BD_TX_READY_BIT;

	    /* incr BD count */

 	    pChan->uart.txBdNext = (pChan->uart.txBdNext+ 1) %
							pChan->uart.txBdNum;
	    }
	}

    /* acknowledge all other interrupts - ignore events */

    pChan->uart.pSmcReg->smce = (pChan->uart.pSmcReg->smce & 
							~(SMCE_RX | SMCE_TX));

    * M8260_SIPNR_L(pChan->regBase) = pChan->uart.intMask;
    }

/*******************************************************************************
*
* smc8260Startup2 - transmitter startup routine
*
* This routine is the tranmitter startup routine.
*
* RETURNS: N/A
*/

void smc8260Startup2
    (
    PPC8260SMC_CHAN *pChan		/* ty device to start up */
    )
    {
    char outChar;
    FAST UINT16 dataLen = 0;

    if (pChan->channelMode == SIO_MODE_POLL)
	return;

    /* check if buffer is ready and if a character needs to be output */

    if ((!(pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode &
	   BD_TX_READY_BIT)) &&
        ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK))
      
	{
	do
	    {	    
	    pChan->uart.txBdBase[pChan->uart.txBdNext].dataPointer[dataLen++] =
		outChar;
	
	    }
	while ((dataLen < pChan->uart.txBufSize) &&
               ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK));
	       						/* fill buffer */

	/* send transmit buffer */
	pChan->uart.txBdBase[pChan->uart.txBdNext].dataLength  = dataLen;
	pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode |=
	    BD_TX_READY_BIT;

	/* incr BD count */

        pChan->uart.txBdNext = (pChan->uart.txBdNext + 1) % pChan->uart.txBdNum;
	}
    }


/******************************************************************************
*
* smc8260CallbackInstall2 - install ISR callbacks to get put chars.
*
* This routine is the call back install routine.
*
* RETURNS: OK or ENOSYS
*/

int smc8260CallbackInstall2
    (
    SIO_CHAN *	pSioChan,
    int		callbackType,
    STATUS	(* callback)(),
    void *	callbackArg
    )
    {
    PPC8260SMC_CHAN * pChan = (PPC8260SMC_CHAN *) pSioChan;

    switch (callbackType)
        {
        case SIO_CALLBACK_GET_TX_CHAR:
            pChan->getTxChar    = callback;
            pChan->getTxArg     = callbackArg;
            return (OK);
	    break;

        case SIO_CALLBACK_PUT_RCV_CHAR:
            pChan->putRcvChar   = callback;
            pChan->putRcvArg    = callbackArg;
            return (OK);
	    break;

        default:
            return (ENOSYS);
        }
    }
