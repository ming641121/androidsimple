#ifndef _UARTDRV_H_
#define   _UARTDRV_H_
/* global forward declarations */

#include "drv/sio/m8260Scc.h"
#include "semLib.h"
#include "msgQLib.h"
#include "taskLib.h"
#include "drv/sio/m8260Sio.h"
#include "smc8260Sio.h"

#define maxMsgNum 	(50)
#define maxMsgLen	(1)

#define LOOPBACK_MODE 0  /* 0: clode loopback mode;  1: open loopback mode */




/* declarations of global variable */

MSG_Q_ID uartMsgQ[4];
/* specifify if ISR will send response data with msgQSend */
int fgUartReadyToRead[4];
/* driver information for scc3 */
M8260_SCC_CHAN m8260SccChan1; 
/* driver information for scc1 */
M8260_SCC_CHAN m8260SccChan2; 
/* driver information for scc4 */
M8260_SCC_CHAN m8260SccChan3; 


/* declarations of subroutine */

void   m8260SioResetChannel (M8260_SCC_CHAN *) ;
STATUS m8260SioIoctl (M8260_SCC_CHAN *pSccChan,int request,int arg) ;
int    m8260SioStartup (M8260_SCC_CHAN *pSccChan) ;
int    m8260SioCallbackInstall (SIO_CHAN *pSioChan, int, STATUS (*)(), void *) ;
void   sccSerialSend( char *Cmd ,int cmdLen, int port);
void uartCmdAct(char *cmdString, int cmdLen,char *rxBuf, char fgReadAct, int port);
void SccUartSerialHwInit2 (void);
void SccUartSerialHwInit (void);


void uartSerialReset (void);
void SccUartSerialHwInit (void);
void SccUartSerialHwInit2 (void);

#endif