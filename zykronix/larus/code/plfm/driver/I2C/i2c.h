#ifndef _I2C_H_
#define _I2C_H_

#include "config.h" /* for PD14/PD15 */
#include "m8260Pio.h"
#include "drv/parallel/m8260IOPort.h"
#include "drv/sio/m8260Sio.h" /* For vxImmrGet (); */
#include "semLib.h"
#include <stdio.h>
#include <taskLib.h>

#define immrVal vxImmrGet()
#define PDDAT * M8260_IOP_PDDAT(immrVal) 
#define readPDDAT  (* M8260_IOP_PDDIR(immrVal) & (~SDA) )

#define SDA PD15
#define SCL PD14
#define MAXLEN 8 


/* Here are the I2C signaling macros */
#define SCLLow()    PDDAT &= ~SCL
#define SCLHigh()   PDDAT |=  SCL
#define SDALow()    PDDAT &= ~SDA
#define SDAHigh()   PDDAT |=  SDA
#define readSDA()   (SDA == ( PDDAT & SDA) )
#define readSCL()   (SCL == ( PDDAT & SCL) )

/* I2C subroutines */
unsigned char writei2c(unsigned address, unsigned char *buffer, unsigned length);
unsigned char readi2c(unsigned address, unsigned char *buffer, unsigned length);
int i2cWriteReg(unsigned char devID,unsigned char *regAddr,unsigned regLen,unsigned char *txBuf, unsigned txLen);
int i2cReadReg(unsigned char devID, unsigned char *regAddr, unsigned regLen, unsigned char *rxBuf, unsigned rxLen);
void i2cInit();
#endif