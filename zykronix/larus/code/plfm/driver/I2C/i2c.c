#include <stdio.h>
#include "i2c.h"

#include <intLib.h> 
static SEM_ID i2cSem;

static void startCommunication(void);
static void stopCommunication(void);
static unsigned char receiveByte(void);
static unsigned char sendByte(unsigned char b);
static void i2cTaskDelay();

int i2cDebug = 0;

/*************************************************************************
function: 
  i2cInit - configure the GPIO pin associated with SDA and SCL signals.  
paramenter:
 
  N/A
  	
return:
  N/A
*/
void i2cInit(void)
{
	 int oldlevel = intLock ();	/* lock interrupts */ 
	 * M8260_IOP_PDPAR(immrVal) &= ~SDA;
	 * M8260_IOP_PDDIR(immrVal) |= SDA;/* output */
	 * M8260_IOP_PDODR(immrVal) |= SDA;
	 
	 * M8260_IOP_PDPAR(immrVal) &= ~SCL;
	 * M8260_IOP_PDDIR(immrVal) |= SCL;/* output */
	 * M8260_IOP_PDODR(immrVal) |= SCL;
	 
	 SDAHigh();  
	 SCLHigh(); 
	 i2cSem=semBCreate (SEM_Q_PRIORITY, SEM_FULL);
	 intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
}

/*************************************************************************
function: 
  startCommunication - sets SDA and SCL in the idle state then 
                      initiates the 'start' condition.  
paramenter:
 
  N/A
  	
return:
  N/A
*/
void startCommunication(void)
{
   SDAHigh();
   SCLHigh();
   i2cTaskDelay();
   SDALow();  
   i2cTaskDelay(); 
   SCLLow();  
}

/*************************************************************************
function: 
  stopCommunication - sets SDA and SCL in an known state then 
                      initiates the 'stop' condition.  
paramenter:
 
  N/A
  	
return:
  N/A
*/
void stopCommunication(void)
{
   
   SCLLow();
   SDALow();
   
   i2cTaskDelay();  
     
   SCLHigh();
   SDAHigh();
   
}
/*************************************************************************
function: 
  receiveByte - receives one byte data.  
paramenter:
 
   N/A
  	
return:
  the received data.
*/
unsigned char receiveByte(void)
{
    unsigned i,b;
    SDAHigh();
    b=0;
    * M8260_IOP_PDDIR(immrVal) &= ~SDA; 
 
    for ( i=0; i<8; i++)
    { 
	   i2cTaskDelay();
	   SCLHigh();
	   i2cTaskDelay();
	   b=(b<<1)|readSDA();  
	   SCLLow();
	}
	* M8260_IOP_PDDIR(immrVal) |=  SDA;
	 
	i2cTaskDelay();

	SCLHigh();

	i2cTaskDelay();
    i2cTaskDelay();
	SCLLow();
	return b;
} 
/*************************************************************************
function: 
  sendByte - sends one byte data to device.  
paramenter:
 
  unsigned char b - the data  will be sent.
  	
return:
  1 - didn't get an ack for that bye sent
  0 - sent the data to device sucessfully.
*/
unsigned char sendByte(unsigned char b)
{
	unsigned i,n=1000;
	for (i=0; i<8; i++)
	{
		if (0x80==(b & 0x80))
		  SDAHigh();
		else
		  SDALow();  

        i2cTaskDelay();     
   
		SCLHigh();

        i2cTaskDelay();
    
        SCLLow();
        b= (b << 1);  
    }
    
    i2cTaskDelay();
    
    SDAHigh();
    SCLHigh();
    * M8260_IOP_PDDIR(immrVal) &= ~SDA;   
    
    i2cTaskDelay();
    i2cTaskDelay();
   /* read the ack */
    
   
    
    i= readSDA();
    while ( (n--) && (i!=0) )
    {          	    
       i= readSDA();
    }
    
    * M8260_IOP_PDDIR(immrVal) |= SDA;      
   
    SCLLow();
    
    return i;
}
/*************************************************************************
function: 
  writei2c - starts communication, sends the address, then sends the
            data to slave device.  
paramenter:
  unsigned address - device address.
  unsigned char *buffer - to store the data will be sent.
  unsigned length - the length of sent data
  												
return:
  1 - didn't get an ack at that address/get an ack for that bye sent
  0 - sent the data to device sucessfully.
*/
unsigned char writei2c(unsigned address, unsigned char *buffer, unsigned length)
{
	unsigned char i, error;
	
	startCommunication();
	error=sendByte(address & 0xFE);
	if (error) /* didn't get an ack at that address */
	{
	   stopCommunication();
if (i2cDebug)
	   printf("no ack at device address 0x%2X\n", address);
	   return error;	
	}
	
	for (i=0; i<length; i++)
	{	
	   error=sendByte(buffer[i]);
	   if (error) /* didn't get an ack for that bye sent */
	   {
	      stopCommunication();
if (i2cDebug)
	      printf("no ack at data byte %d\n", i);
	      return error;	
	   }
	}
	
	stopCommunication();
	
	return 0;	
}
/*************************************************************************
function: 
  readi2c - starts communication, sends the address, then reads the
            reply into buffer.  
paramenter:
  unsigned address - device address.
  unsigned char *buffer - to store the reply data.
  unsigned length - the length of reply data
  												
return:
  1 - didn't get an ack at that address
  0 - reads the reply sucessfully.
*/
unsigned char readi2c(unsigned address, unsigned char *buffer, unsigned length)
{
	unsigned char i, error;
	
	startCommunication();
	error=sendByte(address | 0x01);
	if (error) /* didn't get an ack at that address */
	{
	   stopCommunication();
if (i2cDebug)
	   printf("no ack at device address 0x%2X\n", address);
	   return error;	
	}
	
	for (i=0; i<length; i++)
	{
	   buffer[i]=receiveByte();
	}
	stopCommunication();
	return 0;
}
/*************************************************************************
function: 
  i2cTaskDelay - Just for delay a period.
    
paramenter:
   N/A
  												
return:
   N/A
*/
void i2cTaskDelay()
{ 
	int oldlevel = intLock ();	/* lock interrupts */ 
	unsigned i=50;
	while(i--);
	intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
}

/*************************************************************************
function: 
  i2cWriteReg - generic routine to write the internal register of the sensor.  
paramenter:
  unsigned char devID - device slave address.
  unsigned char regAddr - internal register address.
  unsigned char regAddr - internal register address.
  unsigned char *txBuf - Buffer to store data that will be sent to the register.    
  unsigned rxLen - size of txBuf.
  												
return:
  N/A 
*/
int i2cWriteReg(unsigned char devID,unsigned char *regAddr,unsigned regLen,unsigned char *txBuf, unsigned txLen)
{
		
	unsigned char dataBuf[MAXLEN]; 
	int i;
	int oldlevel = intLock ();	/* lock interrupts */ 
	semTake(i2cSem, WAIT_FOREVER);
	
	for (i=0; i<regLen; i++)
    {
	   dataBuf[i] = regAddr[i];
	}
  
   

    for (i=0; i<txLen; i++)
    {
	   dataBuf[regLen+i] = txBuf[i];
	}
    for (i=0; i< regLen+txLen; i++)
	{
if (i2cDebug)
       printf("dataBuf[%d]=0x%x\n",i,dataBuf[i]);
	}
	
    if (writei2c(devID, dataBuf, regLen+txLen)) return -1;
    
    semGive(i2cSem); 
    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
    return 0;
	
  
}
/*************************************************************************
function: 
  i2cReadReg - generic routine to read the internal register of the sensor.  
paramenter:
  unsigned char devID - device slave address.
  unsigned char regAddr - internal register address.
  unsigned regLen - length of internal register address.
  unsigned char *rxBuf - Buffer to store data from the register.    
  unsigned rxLen - size of rxBuf.												
return:
  N/A 
*/

int i2cReadReg(unsigned char devID, unsigned char *regAddr, unsigned regLen, unsigned char *rxBuf, unsigned rxLen)
{
	
	unsigned i;
	int oldlevel = intLock ();	/* lock interrupts */ 
	semTake(i2cSem, WAIT_FOREVER);
	
	for (i=0; i< regLen; i++)
	{
if (i2cDebug)
	   printf("regAddr[%d]=0x%x\n", i, regAddr[i]);
	}
	writei2c(devID, regAddr, regLen ) ;
	
    if ( readi2c(devID, rxBuf, rxLen) ) return -1;
    for (i=0; i<rxLen; i++ )
	{
if (i2cDebug)
       printf("rxBuf[%d]=0x%X\n", i, rxBuf[i]);
    }
    semGive(i2cSem);   
    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
    return 0;
    
    
}

