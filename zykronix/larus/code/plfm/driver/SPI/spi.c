/*
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: spi.c - driver for MPC8248 SPI connect to MX25L2005(EEPROM)
 *  
 *
 *
 * Note: EEPROM is indeed flash memory. which only allow bite wise from 1 to 0
 *
 *Author(s):
 * marvin, betta and shuming
 *
 */
 /*====================================================================
 * $Log: spi.c,v $
 * Revision 1.13  2006/09/20 08:23:53  cvsshuming
 * have eeprom access much faster and also fix some error and warning
 *
 * Revision 1.12  2006/07/31 10:21:25  cvsshuming
 * fix the reading error
 *
 * Revision 1.11  2006/07/19 16:48:25  cvsshuming
 * use U32_t instead of int for portability
 *
 * Revision 1.10  2006/07/19 06:55:49  cvsshuming
 * add more debug routine
 *
 * Revision 1.9  2006/07/18 02:40:09  cvsshuming
 * add more debug routine
 *
 * Revision 1.8  2006/07/14 08:51:19  cvsshuming
 * add debug routine
 *
 *
 *
 *====================================================================
 * $Endlog $
 */

#include <stdio.h>
#include <taskLib.h>
#include <intLib.h>
#include <sysLib.h>
#include <string.h>
#include "tss_types.h"
#include "config.h"
#include "spi.h"
#include "cpm.h"
#include "m8260Pio.h"
#include "drv/mem/m82xxDpramLib.h"
#include "drv/parallel/m8260IOPort.h"

bd *rx;
bd *tx;
SPI_PARAM *pSPIPram;
U32 allocatedAdrs_txbd, allocatedAdrs_rxbd;
U32 allocatedAdrs_txbuf, allocatedAdrs_rxbuf;
U32 allocatedAdrs_pram;

U32 immrVal = INTERNAL_MEM_MAP_ADDR;    /* Base address of Internal Memory */


/* function exported */
STATUS_t EEPROM_write(U32 targetAddrInEEPROM, U8 *SourceAddress, U32_t len);
STATUS_t EEPROM_read(U8 *targetAddress, U32_t srcAddrInEEPROM, U32_t len);
STATUS_t EEPROM_ChipErase();
STATUS_t EEPROM_GetID();


/* SPI buffer functions */
static void SPI_Init();
static void SPI_Free();
static U32_t SPIWrite(U8 *SourceAddress, U32_t TargetAddress, U32_t FileSize);
static U32_t SPIRead(U8 *TargetAddress, U32_t SourceAddress, U32_t FileSize);
static U32_t SPIErase(U8_t mode, U32_t sectorOrAddr);







#ifdef SPI_DEBUG
/**************************************************************************/

void spidebugselftest(void);
U32_t spidebuginit(U32_t init_sector, U32_t last_sector);
U32_t spidebugwrite(U32_t targetaddr, U32_t n2write, U32_t filesize);
U32_t spidebugread(U32_t sector);



/*************************************
* function: spidebugselftest
*
**************************************/
void spidebugselftest(void)
{

  U32_t i; 
  U32_t firstsector2beInspected	=	0;
  U32_t lastsector2beInspected	=	5;
  U32_t baseSector, baseAddr;
  U32_t testAreaSize0, testAreaSize1, testAreaSize2, testAreaSize3;
  printf("\n  start init\n");

	/* test 1 - init some sector */
		printf("test 1\n\n");
		spidebuginit(firstsector2beInspected,lastsector2beInspected);
		for (i=firstsector2beInspected; i<lastsector2beInspected+1; i++){
		  	spidebugread(i);
		}
	
	/* test 2 - write middle sector*/
		printf("test 2\n\n");
		
		baseSector = 6;
		testAreaSize0 = 0x480;
		testAreaSize1 = 0x480;
		testAreaSize2 = 0x2000;
		testAreaSize3 = 0x300;
		
		baseAddr = baseSector * 0x1000;
		spidebugwrite(baseAddr	, 0x11, testAreaSize0);
		baseAddr += testAreaSize0; 
		spidebugwrite(baseAddr	, 0x12, testAreaSize1);
		baseAddr += testAreaSize1;		
		spidebugwrite(baseAddr	, 0x13, testAreaSize2);
		baseAddr += testAreaSize2; 
		spidebugwrite(baseAddr	, 0x14, testAreaSize3);
		baseAddr += testAreaSize3;
	  	for (i=baseSector; i<( (baseAddr/0x1000) + 1); i++){
	  		spidebugread(i);
	  	}		
	/* test 2 - write end sector */
		printf("test 3\n\n");
		
		baseSector = 61;
		testAreaSize0 = 0x480;
		testAreaSize1 = 0x480;
		testAreaSize2 = 0x2000;
		testAreaSize3 = 0x300;
		baseAddr = baseSector * 0x1000;
		spidebugwrite(baseAddr	, 0x11, testAreaSize0);
		baseAddr += testAreaSize0; 
		spidebugwrite(baseAddr	, 0x12, testAreaSize1);
		baseAddr += testAreaSize1;		
		spidebugwrite(baseAddr	, 0x13, testAreaSize2);
		baseAddr += testAreaSize2; 
		spidebugwrite(baseAddr	, 0x14, testAreaSize3);
		baseAddr += testAreaSize3;
	  	for (i=baseSector; i<( (baseAddr/0x1000) + 1); i++){
	  		spidebugread(i);
	  	}	
  
  
  printf("  Done \n\n");

	
}





/*************************************
* function: spidebuginit
*
*
**************************************/
U32_t spidebuginit(U32_t init_sector, U32_t last_sector){
	U32_t i;
	U32_t data2write;
	if (init_sector==0 && last_sector == 0 ){
		init_sector = 0;
		last_sector = 63;
		}
	for (i=init_sector; i <= last_sector; i++){
		
		data2write= ((int)(i/10))*16 + i%10;
		spidebugwrite((0x1000 * i), data2write, 0x1000);
 }
	return 0;
	}

/*************************************
* function: spidebugwrite
* description: write block of size 'filesize'
*				to 'targetaddr' 
*				with value of 'n2write'
*
**************************************/
U32_t spidebugwrite(U32_t targetaddr, U32_t n2write, U32_t filesize)
{
    STATUS_t ret = 0;
    U8 *pDat;

    U32 i;
    i=0;
    pDat = malloc(filesize);

    if (filesize == 0) {
        printf
            ("\nusage:\nspidebugwrite(U32_t targetaddr, U32_t n2write, U32_t filesize)\n");
        return ERROR;
    }

    memset(pDat, n2write, filesize);
    

    ret = EEPROM_write(targetaddr, pDat, filesize);

    if (ret == OK) {
        printf("  writing OK \n");

    } else {
        printf("  writing ERROR \n");
    }


    free(pDat);
    


    return ret;
}



/*************************************
* function: spidebugread
*
*
**************************************/

U32_t spidebugread(U32_t sector)
{

    U8 *pDat;
    U16 i;
    pDat = malloc(4096);
    printf("the sector %d to be read out", sector%64);
    EEPROM_read(pDat, 4096 * sector, 4096);
    for (i = 0; i < 4096; i++) {
        if (i % 4 == 0)
            printf(" ");
        if (i % 16 == 0)
            printf(" ");
        if (i % 32 == 0)
            printf("\n");
        if (i % 1024 == 0)
            printf("------sector %02d-%d, KB %d\n", sector, i / 1024,
                   sector * 4 + i / 1024);
        printf("%02x", pDat[i]);
    }

    printf("\n---------- done --------\n");

    free(pDat);
    return 1;
}



/**************************************************************************/
#endif



/*************************************************************************
* STATUS_T EEPROM_read(U8 *targetAddress,U32_t srcAddrInEEPROM,U32_t len)
* function: 
*  
* paramenter:
*  U8 *targetAddress - the place to hold data to be read out.
*  U32_t srcAddrInEEPROM - the internal address of EEPROM to be written.
*      the range of TargetAddress: 0 to 262143 (i.e. 64 sectors, 4KB per sector)										
*  U32_t len -  the size(bytes) of the data being stored.
* 
* return
*   OK: if everything ok
*   ERROR: if something wrong
***************************************************************************/
STATUS_t EEPROM_read(U8 *targetAddress, U32_t srcAddrInEEPROM, U32_t len)
{

    return SPIRead(targetAddress, srcAddrInEEPROM, len);
}

/*************************************************************************
* STATUS_t EEPROM_write(U32_t targetAddrInEEPROM,U8 *SourceAddress, U32_t len)
* function: 
*  
* paramenter:
*  U32_t targetAddrInEEPROM - the internal address of EEPROM to be written.
*      the range of TargetAddress: 0 to 262143 (i.e. 64 sectors, 4KB per sector)	
*  U8 *SourceAddress - the place to hold data to be written in.								
*  U32_t len -  the size(bytes) of the data being stored.
* 
* return
*   OK: if everything ok
*   ERROR: if something wrong
***************************************************************************/

STATUS_t EEPROM_write(U32 targetAddrInEEPROM, U8 * SourceAddress, U32_t len)
{

    U8 cur_sector;
    U32 firstByteToWrite, nByteToCopy;

    U32 targaddr = targetAddrInEEPROM;
    U8 *srceaddr = SourceAddress;
    U32 srcLen = len;
    U8 *pDat;
    U32 ret=0;

   
    /* sanity check */
    if (targaddr > 262144)
        return ERROR;
    if (srcLen <= 0)
        return ERROR;



    /* calibrate and init */
    if ((targaddr + srcLen) > 262144) {
        srcLen = 262144 - targaddr;
    }

    pDat = malloc(4096);




    while (srcLen > 0) {
        /* have something to write */
        cur_sector = (U8) (targaddr / 4096);

        if (targaddr % 4096 == 0 && srcLen >= 4096) {
            /* whole sector to write */

#ifdef SPI_DEBUG
            printf("writing to whole sector\n");
#endif 
          
            SPIErase(SPIERASE_MODE_SECTOR,cur_sector);
            ret = SPIWrite(srceaddr, targaddr,4096 );
            if ( ret == 4096){
            	srceaddr += 4096;
            	targaddr += 4096;
            	srcLen	 -= 4096;
            }else{
                free(pDat);
                return ERROR;
            }
            



        } else if (targaddr % 4096 == 0 && srcLen < 4096) {
            /* last sector to write */


            /* read in other data currently in EEPROM */
#ifdef SPI_DEBUG
            printf("writing to last sector\n");
#endif
            SPIRead(pDat+srcLen, (4096 * cur_sector)+srcLen, 4096-srcLen);
            memcpy(pDat, srceaddr, srcLen);
            SPIErase(SPIERASE_MODE_SECTOR,cur_sector);



            ret = SPIWrite(pDat, targaddr,4096 );
            if ( ret == 4096){
            	targaddr += srcLen;
            	srcLen	 = 0;
            }else{
                free(pDat);
                return ERROR;
            }
            


        } else if (targaddr % 4096 != 0) {
            /* First sector to write */
#ifdef SPI_DEBUG
            printf("writing to first sector\n");
#endif            
            firstByteToWrite = cur_sector * 4096;


            if (firstByteToWrite + 4096 > (targaddr + srcLen - 1)) {
                /* the last byte not yet beyound sector boundary */
                nByteToCopy = srcLen;
                SPIRead(pDat, (4096 * cur_sector), (targaddr % 4096));
                SPIRead(pDat+(targaddr % 4096)+srcLen,
                		(4096 * cur_sector)+(targaddr % 4096)+srcLen,
                		4096-(targaddr % 4096)-srcLen);                
            } else {
                /* the last byte is beyound sector boundary */
                SPIRead(pDat, (4096 * cur_sector), (targaddr % 4096));
                nByteToCopy = 4096 - (targaddr % 4096);

            } 
            memcpy(pDat + (targaddr % 4096), srceaddr, nByteToCopy);
            SPIErase(SPIERASE_MODE_SECTOR,cur_sector); 


            ret = SPIWrite(pDat, firstByteToWrite,4096 );
            if ( ret == 4096){
	            targaddr = targaddr + nByteToCopy;
	            srcLen = srcLen - nByteToCopy;
	            srceaddr = srceaddr + nByteToCopy;
            }else{
            	free(pDat);
                return ERROR;
            }

        } else {
            /* otherwise case */
            printf("Abnormal case\n");
            free(pDat);
            return ERROR;
        }
   } /*end of while */



    free(pDat);
    return OK;
}


/*************************************************************************
* function: STATUS_t EEPROM_ChipErase()
*  to erase the whole EEPROM to be all '1'	
*    
* paramenter:
*  N/A  
* 												
*
***************************************************************************/
STATUS_t EEPROM_ChipErase()
{
    U8 StsReg = 0;
    U32_t i, level, n;

    SPI_Init();
    for (n = 0; n < SPI_TRX_SIZE; n++) {
        rx->pointer[n] = 0;
    }

    for (n = 0; n < SPI_TRX_SIZE; n++) {
        tx->pointer[n] = 0;
    }

    /* Setting Write Enable Latch bit */
    level = intLock();
    tx->length = 1;
    *(U8 *) (tx->pointer) = 0x06;
    *SPIE = 0xFF;
    *SPIM = 0x37;
    IO_SYNC;
    tx->status |= 0x8000;
    rx->status |= 0x8000;
    *M8260_IOP_PDDAT(immrVal) &= ~PD19;
    IO_SYNC;
    *SPICOM = 0x80;
    IO_SYNC;
    while (tx->status & 0x8000);
    while (rx->status & 0x8000);
    *M8260_IOP_PDDAT(immrVal) |= PD19;
    *SPIE = 0xFF;
    IO_SYNC;

    /* Write Chip Erase command */
    *(U8 *) (tx->pointer) = 0xC7;
    tx->status |= 0x8000;
    rx->status |= 0x8000;
    *M8260_IOP_PDDAT(immrVal) &= ~PD19;
    IO_SYNC;
    *SPICOM = 0x80;
    IO_SYNC;
    while (tx->status & 0x8000);
    while (rx->status & 0x8000);
    *M8260_IOP_PDDAT(immrVal) |= PD19;
    *SPIE = 0xFF;
    IO_SYNC;

    /* Write Read Status command */
    for (i = 0; i < 2000; i++) {        /* 2000*1ms  = 2s */
        tx->length = 2;         /* 2 system clocks to get FLASH status within one SPISEL cycle */
        *(U8 *) (tx->pointer) = 0x05;
        tx->status |= 0x8000;
        rx->status |= 0x8000;
        *M8260_IOP_PDDAT(immrVal) &= ~PD19;
        IO_SYNC;
        *SPICOM = 0x80;
        IO_SYNC;
        while (tx->status & 0x8000);
        while (rx->status & 0x8000);
        StsReg = rx->pointer[1];
        *M8260_IOP_PDDAT(immrVal) |= PD19;
        if (!(StsReg & 0x1)) {
            SPI_Free();
            intUnlock(level);
            return (1);
        }
        taskDelay(1);           /* 1ms */
    }
    SPI_Free();
    intUnlock(level);
    return (0);
}






/*************************************************************************
* function: 
*  EEPROM_GetID - Get ID
* paramenter:
*  N/A  
* 												
*
***************************************************************************/
STATUS_t EEPROM_GetID()
{
    U32_t level, n;

    SPI_Init();
    for (n = 0; n < SPI_TRX_SIZE; n++) {
        rx->pointer[n] = 0;
    }

    for (n = 0; n < SPI_TRX_SIZE; n++) {
        tx->pointer[n] = 0;
    }

    level = intLock();
    tx->length = 4;             /* 4 system clocks to s/w reset and get IDs within one SPISEL cycle */
    tx->pointer[0] = 0x9F;
    *SPIE = 0xFF;               /* clear all previus evets   */
    *SPIM = 0x37;               /* enable interrupt */
    tx->status |= 0x8000;       /* buffer ready */
    rx->status |= 0x8000;       /* buffer ready */
    *M8260_IOP_PDDAT(immrVal) &= ~PD19; /* set low level on the PD19 (SPISEL for the SPI slave) */
    *SPICOM = 0x80;             /* start */
    while (tx->status & 0x8000);
    while (rx->status & 0x8000);

    *M8260_IOP_PDDAT(immrVal) |= PD19;  /* set high level on the PD19 (SPISEL for the SPI slave) */
    SPI_Free();
    intUnlock(level);
    
    printf("\nManufacture ID(0xC2)=0x%X\n", rx->pointer[1]);
    printf("Device ID(0x20)=0x%X\n", rx->pointer[2]);
    printf("Device ID(0x12)=0x%X\n", rx->pointer[3]);
    
    return (1);
}

/*************************************************************************
* function: 
*  SPI_Init - Initialize SPI controller.
* paramenter:
*  N/A  
* 												
*
***************************************************************************/

/* SPI bus routines */
static void SPI_Init()
{
    U16 *a;

    U32_t oldlevel = intLock();   /* lock interrupts */

    /* enable SPI pins, PD16~PD19 */
    *M8260_IOP_PDPAR(immrVal) |= (PD16 | PD17 | PD18);
    *M8260_IOP_PDPAR(immrVal) &= ~PD19;
    *M8260_IOP_PDDIR(immrVal) |= PD19;
    *M8260_IOP_PDDIR(immrVal) &= ~(PD16 | PD17 | PD18);
    *M8260_IOP_PDSO(immrVal) |= (PD16 | PD17 | PD18);
    *M8260_IOP_PDDAT(immrVal) |= PD19;

    /* transmit BD base address */
    if ((allocatedAdrs_txbd =
         ((UINT32) m82xxDpramAlignedMalloc(8, 8))) != 0)
        tx = (bd *) (allocatedAdrs_txbd);
    else {
        sysToMonitor(BOOT_NO_AUTOBOOT);
    }

    /* receive BD base address  */
    if ((allocatedAdrs_rxbd =
         ((UINT32) m82xxDpramAlignedMalloc(8, 8))) != 0)
        rx = (bd *) (allocatedAdrs_rxbd);
    else {
        sysToMonitor(BOOT_NO_AUTOBOOT);
    }

    /* transmit buffer base address */
    if ((allocatedAdrs_txbuf =
         ((UINT32) m82xxDpramAlignedMalloc(SPI_TRX_SIZE, 2))) != 0)
        tx->pointer = (U8 *) (allocatedAdrs_txbuf);
    else {
        sysToMonitor(BOOT_NO_AUTOBOOT);
    }

    /* receive buffer base address */
    if ((allocatedAdrs_rxbuf =
         ((UINT32) m82xxDpramAlignedMalloc(SPI_TRX_SIZE, 2))) != 0)
        rx->pointer = (U8 *) (allocatedAdrs_rxbuf);
    else {
        sysToMonitor(BOOT_NO_AUTOBOOT);
    }

    /* initialize parameter RAM specifics */
    if ((allocatedAdrs_pram =
         ((UINT32) m82xxDpramAlignedMalloc(0x34, 64))) != 0) {
        a = (U16 *) (BASE_ADDR + BASE_SPI);
        *a = (allocatedAdrs_pram - BASE_ADDR);
        pSPIPram = (SPI_PARAM *) (allocatedAdrs_pram);
        pSPIPram->rbase = (U16) ((U32) rx);
        pSPIPram->tbase = (U16) ((U32) tx);
        pSPIPram->rfcr = 0x10;
        pSPIPram->tfcr = 0x10;
        pSPIPram->mrblr = 8;    /*SPI_RXBL_MAX; */
    } else {
        sysToMonitor(BOOT_NO_AUTOBOOT);
    }

    tx->status = 0x3800;        /* not ready, wrap last, last character, non continuous mode */
    tx->length = 1;

    rx->status = 0xB000;        /* empty */
    rx->length = 0;

    /* issue initialize Rx and Tx parameters command */
    *CPCR(immrVal) = W15 + W2 + W5 + W7 + W9;
    while ((*CPCR(immrVal)) & W15);

    *SPIE = 0xFF;
    *SPIM = 0x00;

    *(SPIMOD) =
        HW5 + HW6 + HW7 + HW9 + HW10 + HW11 + HW12 + HW13 + HW14 + HW15;
    intUnlock(oldlevel);        /* UNLOCK INTERRUPTS */
}

/*************************************************************************
* function: 
*  SPI_Free - Release memory resource.
* paramenter:
*  N/A  
* 												
*
***************************************************************************/
static void SPI_Free()
{
    m82xxDpramFree((void *) allocatedAdrs_txbd);
    m82xxDpramFree((void *) allocatedAdrs_rxbd);
    m82xxDpramFree((void *) allocatedAdrs_txbuf);
    m82xxDpramFree((void *) allocatedAdrs_rxbuf);
    m82xxDpramFree((void *) allocatedAdrs_pram);
}

/*************************************************************************
* SPIErase
* function: 
* input
*	U8_t mode:
*		SPIERASE_MODE_SECTOR: erase by sector
*		SPIERASE_MODE_ADDRESS: erase by address
*	U32_t sectorOrAddr 
* return:
* 	OK
*	ERROR
***************************************************************************/
static U32_t SPIErase(U8_t mode, U32_t sectorOrAddr)
{
    volatile U8 StsReg = 0;
    U32_t j;

    volatile U32 AD1;
    volatile U32 AD2;
    volatile U32 AD3;
    U32_t level;
    U32_t TargetAddress;
    
    switch (mode){
    	case SPIERASE_MODE_SECTOR:
    		TargetAddress = 4096 * sectorOrAddr;
    		break;
    	case SPIERASE_MODE_ADDRESS:
    		TargetAddress = sectorOrAddr;
    		break;
    	default:
    		return(ERROR);
    	
    }

    SPI_Init();
    /* Setting Write Enable Latch bit */
    level = intLock();
    tx->length = 1;
    *(U8 *) (tx->pointer) = 0x06;
    *SPIE = 0xFF;
    *SPIM = 0x37;
    IO_SYNC;
    tx->status |= 0x8000;
    rx->status |= 0x8000;
    *M8260_IOP_PDDAT(immrVal) &= ~PD19;
    IO_SYNC;
    *SPICOM = 0x80;
    IO_SYNC;
    while (tx->status & 0x8000);
    while (rx->status & 0x8000);
    *M8260_IOP_PDDAT(immrVal) |= PD19;
    *SPIE = 0xFF;
    IO_SYNC;


    /* Write Sector Erase command */
    tx->length = 4;
    tx->pointer[0] = 0x20;
    tx->status |= 0x8000;
    rx->status |= 0x8000;
    *M8260_IOP_PDDAT(immrVal) &= ~PD19;
    AD1 = (volatile unsigned int) (TargetAddress & 0xFF0000);
    AD1 >>= 16;
    AD2 = (volatile unsigned int) (TargetAddress & 0x00F000);
    AD2 >>= 8;
    AD3 = (volatile unsigned int) (TargetAddress & 0x000000);
    tx->pointer[1] = (volatile unsigned char) AD1;
    tx->pointer[2] = (volatile unsigned char) AD2;
    tx->pointer[3] = (volatile unsigned char) AD3;

    *SPICOM = 0x80;
    while (tx->status & 0x8000);
    while (rx->status & 0x8000);
    *M8260_IOP_PDDAT(immrVal) |= PD19;




    /* Write Read Status command */
    rx->pointer[1] = 1;
    for (j = 0; j < 100; j++) {
        tx->length = 2;
        tx->pointer[0] = 0x05;
        tx->status |= 0x8000;
        rx->status |= 0x8000;
        *M8260_IOP_PDDAT(immrVal) &= ~PD19;
        *SPICOM = 0x80;
        while (tx->status & 0x8000);
        while (rx->status & 0x8000);
        StsReg = rx->pointer[1];
        *M8260_IOP_PDDAT(immrVal) |= PD19;
        if (!(StsReg & 0x1)) {
            SPI_Free();
            intUnlock(level);
            return (OK);
        }
        taskDelay(1);
    }/*
    if (j == 100) {
        SPI_Free();
        intUnlock(level);
        return (0);
    }*/
    SPI_Free();
    intUnlock(level);
    return (OK);
}


/*************************************************************************
* function: 
*  SPIWrite - Write data into device via SPI.
* paramenter:
*  	U8 *SourceAddress - the data being stored.
* 	U32_t TargetAddress - the internal address where will store data.											
*  	U32_t FileSize -  the size(bytes) of the data being stored.
* return:
*	U32_t the actual byte written
***************************************************************************/
static U32_t SPIWrite(U8 *SourceAddress, U32_t TargetAddress, U32_t FileSize)
{

    volatile U8 StsReg = 0;
    volatile U32 AD1;
    volatile U32 AD2;
    volatile U32 AD3;
 
    U32_t level;
    U32_t n,i,j, ret=0, nPage=0,exactPage=0, nByte2Write=0;
    U32_t curTargetAddress;
    volatile U8 *SourcePt=(volatile U8 *) SourceAddress;

    SPI_Init();

    nPage=FileSize/WRITE_PROGRAM_PAGE_SIZE + 1;
    
    if (FileSize%WRITE_PROGRAM_PAGE_SIZE == 0 ){
    	exactPage=1;
    	nPage=FileSize/WRITE_PROGRAM_PAGE_SIZE;
    }else{
    	exactPage=0;
    	if (FileSize == 0){
    		nPage = 0;
    	}else{
    		nPage=( FileSize/WRITE_PROGRAM_PAGE_SIZE)+1;
    	}
    }
	
    level = intLock();

	/* start to write */  
    for (i=0; i < nPage ; i++){


    	
		/* Preparation  */
		
			if ( i != (nPage-1) ){
				nByte2Write = WRITE_PROGRAM_PAGE_SIZE;
			}else if (exactPage == 1){
				nByte2Write = WRITE_PROGRAM_PAGE_SIZE;	
			}else {
				nByte2Write = FileSize%WRITE_PROGRAM_PAGE_SIZE;
			}
		
			curTargetAddress = TargetAddress + i*WRITE_PROGRAM_PAGE_SIZE;


	
		/* clear rx/tx buff */
			for (n = 0; n < SPI_TRX_SIZE; n++) {
				rx->pointer[n] = 0;
			}
			
			for (n = 0; n < SPI_TRX_SIZE; n++) {
				tx->pointer[n] = 0;
			}

    	/* Setting Write Enable Latch bit */

		    tx->length = 1;
		    rx->length = 1; 
		    tx->pointer[0] = 0x06;
		    *SPIE = 0xFF;
		    *SPIM = 0x37;
		    IO_SYNC;
		    tx->status |= 0x8000;
		    rx->status |= 0x8000;
		    *M8260_IOP_PDDAT(immrVal) &= ~PD19;
		    *SPICOM = 0x80;
		    while (tx->status & 0x8000);
		    rx->status |= 0x8000;
		    while (rx->status & 0x8000);
		    *M8260_IOP_PDDAT(immrVal) |= PD19;
   
   
		/* Write Page Program command */

			tx->length = 4+ nByte2Write;
			rx->length = 4+ nByte2Write;
			
 
			tx->pointer[0] = 0x02;
			tx->status |= 0x8000;
			*M8260_IOP_PDDAT(immrVal) &= ~PD19;
			AD1 = (volatile unsigned int) (curTargetAddress & 0xFF0000);
			AD1 >>= 16;
			AD2 = (volatile unsigned int) (curTargetAddress & 0x00FF00);
			AD2 >>= 8;
			AD3 = (volatile unsigned int) (curTargetAddress & 0x0000FF);
			tx->pointer[1] = (volatile unsigned char) AD1;
			tx->pointer[2] = (volatile unsigned char) AD2;
			tx->pointer[3] = (volatile unsigned char) AD3;
			
			for (n = 0; n < nByte2Write; n++) {
				tx->pointer[n+4] = *(SourcePt + n);
			}
			*SPICOM = 0x80;
			while (tx->status & 0x8000);
			while (rx->status & 0x8000);
			
			taskDelay(1);
			*M8260_IOP_PDDAT(immrVal) |= PD19;

     /* test 'Write Status' command */

		    StsReg=1;
		    for (j = 0; j < WRITE_STATUS_TEST_COUNT; j++) {   /* shuming */
		
		        tx->length = 2;
		        tx->pointer[0] = 0x05;
		        tx->status |= 0x8000;
		        rx->status |= 0x8000; 
		        *M8260_IOP_PDDAT(immrVal) &= ~PD19;
		        *SPICOM = 0x80;
		        while (tx->status & 0x8000);
		        while (rx->status & 0x8000);
		
		        StsReg = rx->pointer[1];
		        *M8260_IOP_PDDAT(immrVal) |= PD19;
		        if (!(StsReg & 0x1)) {
		        	/* when write complete, (StsReg & 0x1) == 0 */
		        	ret += nByte2Write;
		        	SourcePt += nByte2Write;
		        	break;
		        }
			      taskDelay(1);
		   
		    }
		    if ( j == WRITE_STATUS_TEST_COUNT) {
		    	ret = -1;
		    	break;
		    }

    } /* end for write page program loop */

    
    SPI_Free();
    intUnlock(level);
    return (ret);  
 
}






/*************************************************************************
* U32_t SPIRead(U32_t SourceAddress,U32_t TargetAddress,U32_t FileSize)
 
* function: SPIWriteByte - Write one-byte data into device via SPI.
*  
* paramenter:
 
*  U32_t SourceAddress - the data being retrieve.
*  U8 *TargetAddress - the buffer that will store data from being retrieved.
*  U32_t FileSize -  the size(bytes) of the data being retrieved.
* 												
*
***************************************************************************/
static U32_t SPIRead(U8 *TargetAddress, U32_t SourceAddress, U32_t FileSize)
{

    volatile U32 AD1;
    volatile U32 AD2;
    volatile U32 AD3; 
    
    U32_t level;
    U32_t n,i, ret=0, nPage=0,exactPage=0, nByte2Read=0;
    U32_t curSourceAddress;
    volatile U8 *TargetPt=(volatile U8 *)TargetAddress;


    SPI_Init();

    nPage=FileSize/READ_PAGE_SIZE + 1;
    
    if (FileSize%READ_PAGE_SIZE == 0 ){
    	exactPage=1;
    	nPage=FileSize/READ_PAGE_SIZE;
    }else{
    	exactPage=0;
    	if (FileSize == 0){
    		nPage = 0;
    	}else{
    		nPage=( FileSize/READ_PAGE_SIZE)+1;
    	}
    }
    
	/* read loop procedure */
	level = intLock();  
    for (i=0; i < nPage ; i++){
    
    	/* Preparation and interrupt lock */
		
			
			if ( i != (nPage-1) ){
				nByte2Read = READ_PAGE_SIZE;
			}else if (exactPage == 1){
				nByte2Read = READ_PAGE_SIZE;	
			}else {
				nByte2Read = FileSize%READ_PAGE_SIZE;
			}
			
			curSourceAddress = SourceAddress + i*READ_PAGE_SIZE;
			TargetPt = TargetAddress + i*READ_PAGE_SIZE;
			
		    for (n = 0; n < SPI_TRX_SIZE; n++) {
		        rx->pointer[n] = n%16;
		    }
		
		    for (n = 0; n < SPI_TRX_SIZE; n++) {
		        tx->pointer[n] = n%16;
		    }

		/* start to read */
	
		    tx->length = nByte2Read+4;             /* total system clocks */
		    rx->length = nByte2Read+4;  
		    tx->pointer[0] = 0x03;


				
		    *SPIE = 0xFF;
		    *SPIM = 0x37;
		    IO_SYNC;
		
		    tx->status |= 0x8000;
		    rx->status |= 0x8000;
		    *M8260_IOP_PDDAT(immrVal) &= ~PD19;
		    
		    /* read addresses */
		    AD1 = (volatile unsigned int) (curSourceAddress & 0xFF0000);
		    AD1 >>= 16;
		    AD2 = (volatile unsigned int) (curSourceAddress & 0x00FF00);
		    AD2 >>= 8;
		    AD3 = (volatile unsigned int) (curSourceAddress & 0x0000FF);
		    tx->pointer[1] = (volatile unsigned char) AD1;
		    tx->pointer[2] = (volatile unsigned char) AD2;
		    tx->pointer[3] = (volatile unsigned char) AD3;
		    
		    *SPICOM = 0x80;
		    IO_SYNC;
		 
		 	while (tx->status & 0x8000);
		 	while (rx->status & 0x8000);
 	
 			


		    *M8260_IOP_PDDAT(immrVal) |= PD19;
		    *SPIE = 0xFF;
		    

			for (n = 0; n < nByte2Read; n++) {
				*(TargetPt + n) = rx->pointer[n+4];
			}
			ret = ret + nByte2Read;
			 /* taskDelay(2);   shuming good result*/  

	} /* end of read loop*/
    intUnlock(level);
    SPI_Free();  
    return (ret); 
  
}






