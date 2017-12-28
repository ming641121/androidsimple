/* sysSerial.c - BSP serial device initialization */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,10dec01,g_h  Move to use the generic m82xxDpramLib
01c,17jul01,g_h  changed to use the dynamic DPRAM memory allocation
01b,26apr99,elk  added SMC1.
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
#include <string.h>
#include "logLib.h"
#include "smc8260Sio.h"

/*****************/

#include "drv/sio/m8260Sio.h" /* For vxImmrGet (); */
#include "uartdrv.h"

/*******************************/
/* Global data    */

PPC8260SMC_CHAN ppcChn1 ;     /* for smc1 */
static int smcLen ;
int smcSerialDebug = 0;


void smcSerialHwInit(void);
void smcSerialHwInit2(void);
void smc8260DevInit2(PPC8260SMC_CHAN *);


static STATUS smc1GetTxChar(char *txData, char *outChar);
/* External References */
extern uint32_t sysBaudClkFreq(void);

/******************************************************************************
*
* smcSerialHwInit - initialize the BSP serial devices to a quiesent state
*
* This routine initializes the BSP serial device descriptors and puts the
* devices in a quiesent state.  It is called from sysHwInit() with
* interrupts locked.
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit()
*/ 

UINT32 allocatedAdrs; 
void smcSerialHwInit
    ( 
    void 
    )
    {
    int    i;
    
    UINT32 dpram1Base;   
    UINT32 allocatedAdrs;
   
    
   
    int immrVal = vxImmrGet();

    /* SMC1: PC5 TX, PC4 RX */
    logMsg("Before M8260_IOP_PADIR \r\n", 0, 0, 0, 0, 0, 0); 
    * M8260_IOP_PCDIR(immrVal) &= ~(PC4);  
    * M8260_IOP_PCDIR(immrVal) |=  (PC5);   	
    * M8260_IOP_PCPAR(immrVal) |=  (PC4 | PC5); 
    * M8260_IOP_PCSO(immrVal)  &= ~(PC4 | PC5); 	  
    logMsg("AfterM8260_IOP_PADIR \r\n", 0, 0, 0, 0, 0, 0); 
#if 0 /* BSP_MOD */
    for (i = 0; i < N_SIO_CHANNELS; i++)
#else
	i=0;
#endif
	  
         {
         /*
          * intialize the chips device descriptors.
          */

         ppcChn1.regBase        = vxImmrGet ()       ; /* IMMR reg has base adr */
         ppcChn1.clockRate      = sysBaudClkFreq ()  ; /* Baud clock freq (Hz)  */
#if 1 /* BSP_MOD */
         ppcChn1.bgrNum         = 1 + i              ; /* use BRGx              */
         ppcChn1.uart.smcNum    = 1 + i              ; /* SMCx wired for rs232  */
#else
         ppcChn1.bgrNum         = 2 + i              ; /* use BRGx              */
         ppcChn1.uart.smcNum    = 2 + i              ; /* SMCx wired for rs232  */
#endif
         ppcChn1.uart.txBdNum   = 1                  ; /* use 1 transmit BD     */
         ppcChn1.uart.rxBdNum   = 0x10               ; /* use 16 receive BD     */

         /* transmit BD base adrs */

     if ((allocatedAdrs =   (UINT32)(0x0000ffff & (UINT32)m82xxDpramAlignedMalloc (8,8))) != 0)
     {
	   

	     ppcChn1.uart.txBdBase  = (SMC_BUF *) allocatedAdrs;
	   
     }
	 else
	     {
	     sysToMonitor (BOOT_NO_AUTOBOOT);
	     }


         /* receive BD base adrs  */
         if ((allocatedAdrs = (UINT32)(0x0000ffff & (UINT32)m82xxDpramAlignedMalloc (128,8))) != 0)
             
	        ppcChn1.uart.rxBdBase  = (SMC_BUF *) allocatedAdrs; 
         else
	     {
	     sysToMonitor (BOOT_NO_AUTOBOOT);
             }



         ppcChn1.uart.txBufSize = 0x1                ; /* transmit buffer size  */

         dpram1Base = MPC8260_DPRAM1_BASE( ppcChn1.regBase );    /* Here is bug */    

         if ((allocatedAdrs = (UINT32)m82xxDpramAlignedMalloc (0x80,2)) != 0)
  #if 1         
	        ppcChn1.uart.txBufBase = (u_char *) allocatedAdrs;/* Here is bug */  

                
  #endif         
	     else
         {
	        sysToMonitor (BOOT_NO_AUTOBOOT);
	     }

	    if ((allocatedAdrs =(UINT32)m82xxDpramAlignedMalloc (0x80,2)) != 0)

     	   ppcChn1.uart.rxBufBase = (u_char *)allocatedAdrs; 
     	   
	    else

	    {
	    sysToMonitor (BOOT_NO_AUTOBOOT);
	    }


	if ((allocatedAdrs = (0x0000ffff & (UINT32)m82xxDpramAlignedMalloc (0x34,64))) != 0)
	    {
#if 1 /* BSP_MOD */
            *PPC8260_DPR_SMC1(dpram1Base  + (0x100 * i)) = allocatedAdrs ;
#else
            *PPC8260_DPR_SMC2(dpram1Base + (0x100 * i)) = allocatedAdrs;
#endif
		}
	else
	    {
	    sysToMonitor (BOOT_NO_AUTOBOOT);
	    }

        /* DPRAM address of SMCx parameter RAM. */
#if 1
        ppcChn1.uart.pSmc = (SMC *) ( (UINT32)
        (*PPC8260_DPR_SMC1(dpram1Base + (0x100 * i)) + ppcChn1.regBase) );
#else
        ppcChn.uart.pSmc = (SMC *) ( (UINT32)
        (*PPC8260_DPR_SMC2(dpram1Base + (0x100 * i)) + ppcChn.regBase) );
#endif

        /*
         * SMCMR1 for SMC1.
         */
#if 1 /* BSP_MOD */
        ppcChn1.uart.pSmcReg = 
        (SMC_REG *) ( (UINT32) SMCMR1( ppcChn1.regBase + (0x10 * i)) ) ;

        ppcChn1.pBaud = (VINT32 *) BRGC1(ppcChn1.regBase + (0x4 * i)) ;
        ppcChn1.channelMode = 0;
#else
        ppcChn.uart.pSmcReg = 
        (SMC_REG *) ( (UINT32) SMCMR2( ppcChn.regBase + (0x10 * i)) ) ;

        ppcChn.pBaud = (VINT32 *) BRGC2(ppcChn.regBase + (0x4 * i)) ;
        ppcChn.channelMode = 0;
#endif

        smc8260DevInit2(&ppcChn1);
        ppcChn1.baudRate = 9600;
        }
     
    }

/******************************************************************************
*
* smcSerialHwInit2 - connect BSP serial device interrupts
*
* This routine connects the BSP serial device interrupts.  It is called from
* sysHwInit2().  Serial device interrupts could not be connected in
* smcSerialHwInit2() because the kernel memory allocator was not initialized
* at that point, and intConnect() calls malloc().
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit2()
*/ 

void smcSerialHwInit2
    ( 
    void 
    )
    {
    UINT32 immrVal = vxImmrGet ();
    int i;

#if 0 /* BSP_MOD */
    for (i = 0; i < N_SIO_CHANNELS; i++)
#else
    i = 0;
#endif
        {
        /* Connect serial interrupts */
#if 1 /* BSP_MOD */
        *SMCE1( immrVal + (0x10 * i) ) = *SMCE1( immrVal + (0x10 * i) ) ;
#else
        *SMCE2( immrVal + (0x10 * i) ) = *SMCE2( immrVal + (0x10 * i) ) ;
#endif


#if 1 /* BSP_MOD */
        (void) intConnect ( INUM_TO_IVEC(4 + (1 * i)), (VOIDFUNCPTR) smc8260Int1, (int) &ppcChn1 ) ;
#else
        (void) intConnect ( INUM_TO_IVEC(5), (VOIDFUNCPTR) smc8260Int, (int) &ppcChn[i] ) ;
#endif
        /*
         * Enable SMCi interrupts.
         */
#if 1
        *M8260_SIMR_L( immrVal ) |= (0x00001000 >> i);
#else
        *M8260_SIMR_L( immrVal ) |= (0x00000800);
#endif
        } 
      
    }


/******************************************************************************
function:
  smc1GetTxChar - indicated ISR callback functions that are used to get and characters
parameter:
  char *txData    character that will be transmitted.
  char *outChar   buffer that stores string will be transmitted
  
RETURNS: OK if a character is gotten, EAGAIN if no characters are needed to be assign.
*/
STATUS smc1GetTxChar(char *txData, char *outChar)
{
   
  
    static unsigned int i=0;
   
    *outChar=txData[i];
  
    if( i++ < smcLen )	 return 0;
    i=0;
    return (-1);       
}

/*******************************************************************************************
function:
smcSerialSend - send a command string to smc serial port
parameter:
char *Cmd - the command string ended with \r\n
int port - the serial port number (1 for scc1, and 3 for scc3)
int cmdLen  - the command string length

return void

*/
void smcSerialSend( char *Cmd ,int cmdLen,int port)
{
	  
	  
	  STATUS status;
  
      
	  char *txData = Cmd ;
	  STATUS	(* callback)();
	
	  smcLen = cmdLen ;
	
	  callback=smc1GetTxChar;
   
	  status=smc8260CallbackInstall2( (SIO_CHAN*)&ppcChn1, SIO_CALLBACK_GET_TX_CHAR, \
   	  callback, txData); 
	  
if (smcSerialDebug)
{
      if (status == OK) 
         logMsg ("install ISR tx callbacks function succeed\r\n",0, 0, 0, 0,0,0);
      else
         logMsg ("install ISR tx callbacks function fail\r\n",0, 0, 0, 0,0,0);
}

      smc8260Startup2(&ppcChn1);
  
     
}

