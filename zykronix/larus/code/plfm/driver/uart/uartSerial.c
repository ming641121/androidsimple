/* uartSerial.c -  MPC827x SCC UART BSP serial device initialization */

/* Copyright 1984-2003 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01k,03oct04,dtr  SPR 98762.
01j,23dec03,dtr  modified to use dpram mem alloc library calls and 
                 SCC4 support.
01i,01oct03,dee  removed hardcoding of internal map base, use immrVal
01h,17oct01,jrs  Upgrade to veloce
		 corrected SCC2 initialization in sysSerialHwInit() - SPR #64407
		 removed useless init of baud rate generators - SPR #88989
01g,09sep99,ms_  change bsp name from vads8260 to ads8260
01f,15jul99,ms_  make compliant with our coding standards
01e,19apr99,ms_  adhere to coding conventions
01d,14apr99,ms_  switch from using interrupt vectors to interrupt numbers
01c,08apr99,ms_  upgrade to multiple channels
01b,05apr99,ms_  upgrade to support two channels
01a,21jan99,ms_  adapted from sysSerial.c for ads860 
*/

/*
DESCRIPTION

This library contains routines for M8260 SMC UART BSP serial device
initialization
*/
#include "drv/mem/m82xxDpramLib.h" 
#include <stdio.h>  
#include "drv/sio/m8260CpmMux.h"
#include "uartdrv.h"  
#include "sioLib.h"
#include <string.h>
#include <stdlib.h>
#include "logLib.h"
#include "serial_if.h"
#include "uartdrv.h"
#include "vxWorks.h"
#include "iv.h"
#include "intLib.h"
#include "config.h"
#include "sysLib.h"
#include "drv/sio/m8260Scc.h"

#include "drv/parallel/m8260IOPort.h"
#include "drv/sio/m8260Brg.h"
#include "smc8260Sio.h"

#define withSCC4 1
static STATUS scc3GetTxChar(char *, char *);
static STATUS scc1GetTxChar(char *, char *);
static STATUS scc4GetTxChar(char *, char *);
static int gLen3,gLen1,gLen4 ; /* length of the data will be transmit */
static int uartReadRespond(char *rxBuf, int port);

static SEM_ID uartSemMutex[4];
extern PPC8260SMC_CHAN ppcChn1 ;     /* driver information for smc1 */

/*
IMPORT    UINT32  vxImmrGet (void);
*/



/******************************************************************************
*
* SccUartSerialHwInit - initialize the BSP serial devices to a quiesent state
*
* This routine initializes the BSP serial device descriptors and puts the
* devices in a quiesent state.  It is called from sysHwInit() with
* interrupts locked.
*
* Buffers and Buffer Descriptors for the two channels:
* 
* .CS
*                                                   Address per SCC
*                                                ---------------------------
* field                                 size        SCC1            SCC2
* ------                                -------  -----------     -----------
* Receive Buffer Descriptor             8 bytes IMMAP+0x0000    IMMAP+0x0100
* Receive Buffer Status                 2 bytes IMMAP+0x0000    IMMAP+0x0100
* Receive Buffer Length                 2 bytes IMMAP+0x0002    IMMAP+0x0102
* Pointer to Receive Buffer             4 bytes IMMAP+0x0004    IMMAP+0x0104
* Receive Buffer                        1 bytes IMMAP+0x0040    IMMAP+0x0140
*
* Transmit Buffer Descriptor            8 bytes IMMAP+0x0008    IMMAP+0x0108
* Transmit Buffer Status                2 bytes IMMAP+0x0008    IMMAP+0x0108
* Transmit Buffer Length                2 bytes IMMAP+0x000A    IMMAP+0x010A
* Transmit to Receive Buffer            4 bytes IMMAP+0x000C    IMMAP+0x010C
* Transmit Buffer                       1 bytes IMMAP+0x0060    IMMAP+0x0160
* .CE
*
*
*
* RETURNS: N/A
*/ 
 

void SccUartSerialHwInit (void)
    {

    int immrVal = vxImmrGet();
    char * bufferBase;

     /* SCC1 */
    * M8260_IOP_PDDIR(immrVal) &= ~(PD31 | PC30);  
    
    * M8260_IOP_PDDIR(immrVal) |= (PD30);   
    
    * M8260_IOP_PDPAR(immrVal) |= (PD31 | PD30); 
    
    * M8260_IOP_PDSO(immrVal) &= ~(PD31 | PD30); 
    
    * M8260_IOP_PDSO(immrVal) |= PD30; 
    
    

  
    /* SCC3 */
    * M8260_IOP_PDDIR(immrVal) &= ~(PD25);     
    * M8260_IOP_PDDIR(immrVal) |= PD24 ;     
    * M8260_IOP_PDPAR(immrVal) |= (PD24 | PD25);        
    * M8260_IOP_PDSO(immrVal) &= ~(PD24 | PD25); 
   
   
    
    /* SCC 4 */
    * M8260_IOP_PDSO(immrVal) &= ~(PD22 | PD21);
    * M8260_IOP_PDDIR(immrVal) &= ~(PD22);
    * M8260_IOP_PDDIR(immrVal) |= (PD21);
    * M8260_IOP_PDPAR(immrVal) |= (PD22 | PD21);


    /* CPM muxs */

    /* for the value of 0x0009121B:
     * field    description     SCC1    SCC2    SCC3    SCC4
     * GR       Grant Support   <-----always asserted------>
     * SC       SCC connection  <---not connected to TSA--->
     * RSxCS    Rcv Clk Source  BRG1    BRG2    BRG3    BRG4
     * TSxCS    Tx Clk Source   BRG1    BRG2    BRG3    BRG4
     */

    * M8260_CMXSCR(immrVal) = 0x0009121B;

    /* enable both RS232 ports on the board */
#if 0 /*BSP_RMV */
    *BCSR1 &= ~(BCSR1_RS232EN_1_L);
    *BCSR1 &= ~(BCSR1_RS232EN_2_L);
#endif
    /* intialize the two serial channels */

    /* this is in order of the structure contents */

    /* indicate that this is a fresh device */
    m8260SccChan1.channelMode = 0;
    m8260SccChan2.channelMode = 0;
    m8260SccChan3.channelMode = 0;

    m8260SccChan1.baudRate = 9600; /*DEFAULT_BAUD;*/
    m8260SccChan2.baudRate = 9600;
    m8260SccChan3.baudRate = 9600;

    m8260SccChan1.sccNum = 3;
    m8260SccChan2.sccNum = 1;
    m8260SccChan3.sccNum = 4;

    m8260SccChan1.immrVal = immrVal;
    m8260SccChan2.immrVal = immrVal;    
    m8260SccChan3.immrVal = immrVal;
    
    bufferBase = (char *)m82xxDpramAlignedMalloc(0x300,0x100);

    memset ((char*)bufferBase,0x0,0x200);
   
    m8260SccChan1.pBdBase = (char *) ((UINT32)bufferBase + 0x000);
    m8260SccChan1.rcvBufferAddr = (char *) ((UINT32)bufferBase + 0x040);
    m8260SccChan1.txBufferAddr = (char *) ((UINT32)bufferBase + 0x060);
   

    m8260SccChan2.pBdBase = (char *) ((UINT32)bufferBase + 0x100);
    m8260SccChan2.rcvBufferAddr = (char *) ((UINT32)bufferBase + 0x140);
    m8260SccChan2.txBufferAddr = (char *) ((UINT32)bufferBase + 0x160);
  
#if withSCC4   
    m8260SccChan3.pBdBase = (char *) ((UINT32)bufferBase + 0x200);
    m8260SccChan3.rcvBufferAddr = (char *) ((UINT32)bufferBase + 0x240);
    m8260SccChan3.txBufferAddr = (char *) ((UINT32)bufferBase + 0x260);
    
#endif
    /* disable interrupts from SCC1 and SCC3 */

    m8260IntDisable(INUM_SCC3);
    m8260IntDisable(INUM_SCC1);
#if withSCC4  
    m8260IntDisable(INUM_SCC4);
#endif  
    

    printf("Exit SccUartSerialHwInit()\n");
    }

/******************************************************************************
*
* SccUartSerialHwInit2 - connect BSP serial device interrupts
*
* This routine connects the BSP serial device interrupts.  It is called from
* sysHwInit2().  Serial device interrupts could not be connected in
* SccUartSerialHwInit() because the kernel memory allocator was not initialized
* at that point, and intConnect() calls malloc().
*
* RETURNS: N/A
*/ 

void SccUartSerialHwInit2 (void)
    {

    /* connect serial interrupts */
   
    (void) intConnect (INUM_TO_IVEC(INUM_SCC3), 
		      (VOIDFUNCPTR) m8260SioInt, (int) &m8260SccChan1);
    
    (void) intConnect (INUM_TO_IVEC(INUM_SCC1), 
			  (VOIDFUNCPTR) m8260SioInt, (int) &m8260SccChan2);
#if withSCC4
    
		(void) intConnect (INUM_TO_IVEC(INUM_SCC4), 
			  (VOIDFUNCPTR) m8260SioInt, (int) &m8260SccChan3);
#endif

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
#if 0
SIO_CHAN * uartSerialChanGet
    (
    int channel		/* serial channel */
    )
    {
    if (channel == 3)
		return ((SIO_CHAN *) &m8260SccChan1); 
    else if (channel == 1)
		return ((SIO_CHAN *) &m8260SccChan2); 
    else 
		return ((SIO_CHAN *) ERROR);
    }
#endif
/*******************************************************************************
*
* uartSerialReset - reset the serail device 
*
* This function calls SccUartSerialHwInit() to reset the serail device. 
*
* RETURNS: N/A
*
*/

void uartSerialReset (void)
    {

       static int uartSerialResetDone=0; 
       int i;
       
       if (uartSerialResetDone==0)
       {
          uartSerialResetDone=1;
          
          for (i=0; i<4; i++)
          {
             uartSemMutex[i] = semBCreate (SEM_Q_PRIORITY, SEM_FULL);
             fgUartReadyToRead[i] = 0;
          }
          
          SccUartSerialHwInit(); /* Allocate Buffers and Buffer Descriptors */
          SccUartSerialHwInit2(); /* Register serial interrupts */
          smcSerialHwInit ();  /* Allocate Buffers and Buffer Descriptors */   
          smcSerialHwInit2();  /* Register serial interrupts */
          
          m8260SioIoctl(&m8260SccChan1, SIO_MODE_SET, SIO_MODE_INT);
          m8260SioIoctl(&m8260SccChan2, SIO_MODE_SET, SIO_MODE_INT); 
          m8260SioIoctl(&m8260SccChan3, SIO_MODE_SET, SIO_MODE_INT);   
          smc8260Ioctl2 (&ppcChn1,SIO_MODE_SET,SIO_MODE_INT);    
       }
       
       return ;
          
       
    }  
/******************************************************************************
function:
  scc3GetTxChar/scc1GetTxChar - indicated ISR callback functions that are used to get and characters
parameter:
  char *txData    character that will be transmitted.
  char *outChar   buffer that stores string will be transmitted
  
RETURNS: OK if a character is gotten, EAGAIN if no characters are needed to be assign.
*/
STATUS scc3GetTxChar(char *txData, char *outChar)
{
   
  
    static unsigned int i=0;
   
    *outChar=txData[i];
  
    if( i++ < gLen3 )	 return (OK);
    i=0;
    return (ENOSYS);       
}

STATUS scc1GetTxChar(char *txData, char *outChar)
{
  
  
    static unsigned int i=0;
   
    *outChar=txData[i];
  
    if( i++ < gLen1 )	 return (OK);
    i=0;
    return (ENOSYS); 
}
STATUS scc4GetTxChar(char *txData, char *outChar)
{
  
  
    static unsigned int i=0;
   
    *outChar=txData[i];
  
    if( i++ < gLen4 )	 return (OK);
    i=0;
    return (ENOSYS); 
}       

/*******************************************************************************************
function:
sccSerialSend - send a command string to serial port
parameter:
char *rxBuf				the command string ended with \r\n
int port    the serial port number (1 for scc1, and 3 for scc3)

return void

*/
void sccSerialSend( char *Cmd ,int cmdLen,int port)
{
	  
	  
	  STATUS status;
      M8260_SCC_CHAN *sccChan;
	  char *txData = Cmd ;
	  STATUS	(* callback)();
	  if (port==1)
	  {
	     gLen1 = cmdLen ;
	     sccChan=&m8260SccChan2;
	     callback=scc1GetTxChar;
      }  
      else if (port==3)
	  {
	     gLen3 = cmdLen ;
	     sccChan=&m8260SccChan1;
	     callback=scc3GetTxChar;
      }
      else if (port==4)
      {
	     gLen4 = cmdLen ;
	     sccChan=&m8260SccChan3;
	     callback=scc4GetTxChar; 
	  }   
	  status=m8260SioCallbackInstall( (SIO_CHAN*)sccChan, SIO_CALLBACK_GET_TX_CHAR, \
   	  callback, txData); 
#if 0		
      if (status == OK) 
         logMsg ("install ISR tx callbacks function succeed\r\n",0, 0, 0, 0,0,0);
      else
         logMsg ("install ISR tx callbacks function fail\r\n",0, 0, 0, 0,0,0);
#endif         
      m8260SioStartup(sccChan);  
     
}


/*******************************************************************************************
function: 
uartReadRespond - Create a msgQ to receive response data
parameter:
char *rxBuf				the received string ended with \r\n
int port  -  the serial port number 
            (1 for scc1, 2 for smc1,3 for scc3, and 4 for scc4)

return void

*/

int uartReadRespond(char *rxBuf, int port)
{
	
	 int i=0;
	 /*uartMsgQ[port-1] = msgQCreate (maxMsgNum, maxMsgLen, MSG_Q_FIFO);*/ 
	     
     while (1) 
     {  
	    if (msgQReceive(uartMsgQ[port-1], &rxBuf[i], maxMsgLen, 250) == ERROR)
		{
  			msgQDelete (uartMsgQ[port-1]);
#if 0  			
  			if (port!=SMC1PORT)
  			  logMsg("msgQReceive fail over scc%d\r\n", port, 0, 0, 0,0,0);
  			else
  			  logMsg("msgQReceive fail over smc1\r\n", 0, 0, 0, 0,0,0);
#endif  			     
       	    fgUartReadyToRead[port-1] = 0;
	  		return ERROR;
		}
		else			
		{
			/* logMsg("rxBuf[%d]=%c\n", i, rxBuf[i], 0,0,0,0); */
			if (rxBuf[i]=='\n') break;
		}   
			 i++;   	    
    }   
		rxBuf[i]='\0';
		
		msgQDelete (uartMsgQ[port-1]);
		
        fgUartReadyToRead[port-1] = 0;
 
	
   
    return 0;
}



/*******************************************************************************************
function:
uartCmdAct
parameter:
char *cmdString		the point to the comannd string
int	 cmdLen				the size of cmdString
char *rxBuf				the received string ended with \r\n
int	 maxLenRxBuf  the max size of received rxBuf
char fgReadAct		the flag showing if response data expected.
int port    the serial port number 
           (1 for scc1, 2 for smc1,3 for scc3, and 4 for scc4)

return void

*/

void uartCmdAct(char *cmdString, int cmdLen,char *rxBuf, char fgReadAct, int port)
{
 
 	
	char *rxPtr[4];

	semTake (uartSemMutex[port-1], WAIT_FOREVER); /* lock critical region */
 
	rxPtr[port-1] = rxBuf;

	if(fgReadAct) 
	{
		fgUartReadyToRead[port-1] =1;
		uartMsgQ[port-1] = msgQCreate (maxMsgNum, maxMsgLen, MSG_Q_FIFO); 
    }
    
	if (port==SMC1PORT)
	   smcSerialSend(cmdString ,cmdLen,port);
	else	
     sccSerialSend(cmdString ,cmdLen,port); 
       
    if ( fgReadAct ) uartReadRespond(rxPtr[port-1], port);
    
    semGive (uartSemMutex[port-1]);             /* unlock critical region */
}
 
  







