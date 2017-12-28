/*
****************************************************************
*                      Magic Pixel Inc.
*
*    Copyright 2007, Magic Pixel Inc., HsinChu, Taiwan
*                    All rights reserved.
* Filename:    CF.C
*
* Programmer:        MPX E220 division
*
* Created: 03/22/2007
*
* Description:  SPI memory Test 
                      1.Read SPI memory ID 
                      2.WRITE 256 byte to      SPI MEMORY
                      3.Read   256 byte from   SPI MEMORY
*          
****************************************************************
/*
*******************************************************************************
*        #include
*******************************************************************************
*/
#define __MAIN_C

#include "typedef.h"
#include "regfile.h"
#include "global.h"
#include "Macro.h"
/*
*******************************************************************************
*        #define CONSTANTS
*******************************************************************************
*/
#define SPI_PASS                             0
#define SPI_TX_Error                         -1
#define SPI_RX_Error                      -2
#define SPI_TIMEOUT                     -3
/*
*******************************************************************************
*        GLOBAL FUNCTION PROTOTYPES
*******************************************************************************
*/

SBYTE  SPI_Wait_DataTransfer(void);
void SPI_FIFO_Reset(void);
void SPI_Command(DWORD Buf , DWORD CMD );

/*
*****************************************************************************
*****************************************************************************
*/
void RegBaseAddrInit(void)
{
    pstSiu = (STSIUREG *) SIU_BASE;
    pstIpu = (STIPUREG *) IPU_BASE;
    pstIdu = (STIDUREG *) IDU_BASE;
    pstJpg = (STJPGREG *) JPG_BASE;		//Init vaule for jpeg, Timothy
    pstMp4 = (STMP4REG *) MP4_BASE;
    pstDrai = (STDRAIREG *) DRAI_BASE;
    pstUsb = (STUSBREG *) USB_BASE;
    pstHiu = (STHIUREG *) HIU_BASE;
    pstCkg = (STCKGREG *) CKG_BASE;
    pstIcu = (STICUREG *) ICU_BASE;
    pstGpio = (STGPIOREG *) GPIO_BASE;
    pstUart = (STUARTREG *) UART_BASE;
    pstTimer0 = (STTIMERREG *) TIMER0_BASE;
    pstTimer1 = (STTIMERREG *) TIMER1_BASE;
    pstTimer2 = (STTIMERREG *) TIMER2_BASE;
    pstTimer3 = (STTIMERREG *) TIMER3_BASE;
    pstTimer4 = (STTIMERREG *) TIMER4_BASE;
    pstI2s = (STI2SREG *) I2S0_BASE;
    pstMcard = (STMCARDREG *) MCARD_BASE;
    pstRTC     =(STRTCREG *) RTC_BASE;
    pstSPI     =(STSPIREG *) SPI_BASE;
    pstDMA2     =(STDMA2REG *) DMA2_BASE;
}

void Main_SPI()
{
  DWORD  i,j;
  SBYTE  sts;
   DWORD Buf = 0xA0800000;
  DWORD *Buf1 = (DWORD *)0xA0820000;
  BYTE     *Buf2 = (BYTE *)0xA0800000;
  DWORD *Buffer = (DWORD *)0xA0800000;
  j=0;
  // Memory initial 
        for ( i=0; i<4*1024 ;i++){
	   *(Buf2+i) = i;
        /*  if(i%2)
             *(Buf2+i) = 0x88;
           else
             *(Buf2+i) =0x11;*/
   	}
  printf("\r\nSPI Test Start ");		
  RegBaseAddrInit();
  pstGpio->SPIGPCFG          = 0x0000ffff; //CFG  GPIO  for SPI
  //Set SPI CLK
  pstCkg   ->CFG_SPI1_CLK  = 0x03;
  pstCkg  ->SPI1CK_SELECT = 0x0e;
//[0]:ModeSel  [1]:EN_Transfer  [2]:CPHA  [3]:CPOL [4]TAGD [5]EN_SCLK 
//[6]RxAsDummy  [7]Fifo_reset  [8-16] txByteNum [17-25]rxByteNum
 pstSPI->SPCON =0x0008013E;
 SPI_Command(Buf , 0x9f );
 sts =SPI_Wait_DataTransfer();
 SPI_FIFO_Reset();
 printf("\r\nSPI  ID :  %x ",*Buf1);
////[0]:ModeSel  [1]:EN_Transfer  [2]:CPHA  [3]:CPOL [4]TAGD [5]EN_SCLK 
//[6]RxAsDummy  [7]Fifo_reset  [8-16] txByteNum [17-25]rxByteNum
 pstSPI->SPCON =0x0004013E;
 SPI_Command(Buf , 0x05 );
 sts =SPI_Wait_DataTransfer();
 SPI_FIFO_Reset();
 printf("\r\nREAD STATUS(initial) :  %x ",*Buf1);
 //[0]:ModeSel  [1]:EN_Transfer  [2]:CPHA  [3]:CPOL [4]TAGD [5]EN_SCLK 
//[6]RxAsDummy  [7]Fifo_reset  [8-16] txByteNum [17-25]rxByteNum
 pstSPI->SPCON =0x0000016E;
 SPI_Command(Buf , 0x06 );
 sts =SPI_Wait_DataTransfer();
 SPI_FIFO_Reset();
 printf("\r\nWrite Enable ");
 //[0]:ModeSel  [1]:EN_Transfer  [2]:CPHA  [3]:CPOL [4]TAGD [5]EN_SCLK 
//[6]RxAsDummy  [7]Fifo_reset  [8-16] txByteNum [17-25]rxByteNum
 pstSPI->SPCON =0x0004013E;
 SPI_Command(Buf , 0x05 );
 sts =SPI_Wait_DataTransfer();
 SPI_FIFO_Reset();
 //printf("\r\nREAD STATUS (write Enable):  %x ",*Buf1);
 //[0]:ModeSel  [1]:EN_Transfer  [2]:CPHA  [3]:CPOL [4]TAGD [5]EN_SCLK 
//[6]RxAsDummy  [7]Fifo_reset  [8-16] txByteNum [17-25]rxByteNum
 pstSPI->SPCON =0x0000016E;
 SPI_Command(Buf , 0xC7 );
 sts =SPI_Wait_DataTransfer();
 SPI_FIFO_Reset();
 printf("\r\nCHIP Erase ");
 //[0]:ModeSel  [1]:EN_Transfer  [2]:CPHA  [3]:CPOL [4]TAGD [5]EN_SCLK 
//[6]RxAsDummy  [7]Fifo_reset  [8-16] txByteNum [17-25]rxByteNum
 pstSPI->SPCON =0x0004013E;
 SPI_Command(Buf , 0x05 );
 sts =SPI_Wait_DataTransfer();
 SPI_FIFO_Reset();
 printf("\r\nSPI STATUS (Chip Erase):  %x ",*Buf1);
 do{
 	pstSPI->SPCON =0x0004013E;
 	SPI_Command(Buf , 0x05 );
 	sts =SPI_Wait_DataTransfer();
 	SPI_FIFO_Reset();
	i=*Buf1;
 	//printf("\r\nSPI STATUS (Chip Erase):  %x ",i);
	wait(500);
      }while((i & 0xffffff00)!=0x00);
 printf("\r\nSPI STATUS (Chip Erase):  %x ",*Buf1);
 printf("\r\nCHIP Erase complete\r\n");
 //write enable again for status 
 //[0]:ModeSel  [1]:EN_Transfer  [2]:CPHA  [3]:CPOL [4]TAGD [5]EN_SCLK 
//[6]RxAsDummy  [7]Fifo_reset  [8-16] txByteNum [17-25]rxByteNum
 pstSPI->SPCON =0x0000016E;
 SPI_Command(Buf , 0x06 );
 sts =SPI_Wait_DataTransfer();
 SPI_FIFO_Reset();
 printf("\r\nWrite Enable ");
 //[0]:ModeSel  [1]:EN_Transfer  [2]:CPHA  [3]:CPOL [4]TAGD [5]EN_SCLK 
//[6]RxAsDummy  [7]Fifo_reset  [8-16] txByteNum [17-25]rxByteNum
 pstSPI->SPCON =0x0004013E;
 SPI_Command(Buf , 0x05 );
 sts =SPI_Wait_DataTransfer();
 SPI_FIFO_Reset();
 printf("\r\nREAD STATUS (write Enable):  %x  ",*Buf1);
 //Write 256byte
 printf("\r\nWrite Start  :256 byte data");
*Buffer = 0x00005002; //write command + write address+ write data
 pstSPI->SPCON =0x0001046E;
 pstDMA2->DMA_CFG0 =0x00;
 //DMA 0
 pstDMA2->DMA0_ISA = 0xa0800000;
 //pstDMA2->DMA0_IDA = 0x08048408;
 pstDMA2->DMA0_IDA =   0xA8610008;
 pstDMA2->DMA0_CFG0= 0x00000104;
 //pstDMA2->DMA0_CFG1= 0x00000003;
 pstDMA2->DMA0_CFG1= 0x01002023;
 pstDMA2->DMA0_SWT = 0x0000000b;
 sts =SPI_Wait_DataTransfer();
 //wait(100);
 SPI_FIFO_Reset();
 printf("\r\nWrite Complete ");
//
pstSPI->SPCON =0x0004013E;
 SPI_Command(Buf , 0x05 );
 sts =SPI_Wait_DataTransfer();
 SPI_FIFO_Reset();
 //printf("\r\nSPI STATUS :  %x ",*Buf1);

//Read 256byte
 printf("\r\nRead Start : 256 byte data");
*Buffer = 0x00005003; //write command + write address+ write data
 pstSPI->SPCON =0x0208043E;
 pstDMA2->DMA_CFG0 =0x00;
 //DMA 0
 pstDMA2->DMA0_ISA = 0xa0800000;
 //pstDMA2->DMA0_IDA = 0x08048408;
 pstDMA2->DMA0_IDA = 0xA8610008;
 pstDMA2->DMA0_CFG0= 0x00000004;
// pstDMA2->DMA0_CFG1= 0x00400003;
 pstDMA2->DMA0_CFG1= 0x00402023;
 pstDMA2->DMA0_SWT = 0x0000000b;
//DMA1
 //pstDMA2->DMA1_ISA = 0x0804840C;
pstDMA2->DMA1_ISA = 0xA861000C;
 pstDMA2->DMA1_IDA = 0xa0820000;
 pstDMA2->DMA1_CFG0= 0x00000104;
 //pstDMA2->DMA1_CFG1= 0x00400300;
 pstDMA2->DMA1_CFG1= 0x01002320;
 pstDMA2->DMA1_SWT = 0x0000000b;
 sts =SPI_Wait_DataTransfer();
 //wait(100);
 SPI_FIFO_Reset();
 printf("\r\nRead Complete ");

 for ( i=0; i<(256/4 );i++){
     	if(*(Buffer+1+i) !=  *(Buf1+1+i))
       	{
          j++;      
        }
  }
 printf("\r\nSPI Memory data Error number %i  ",j);

if( j==0)	
 printf("\r\nSPI  TEST PASS !!! ");
else
 printf("\r\nSPI  TEST FAIL  !!!  ");	
 printf("\r\n");

}
void SPI_Command(DWORD Buf , DWORD CMD )
{ 
 DWORD *Buffer = (DWORD *)(Buf);
 *Buffer =CMD; 
 pstDMA2->DMA_CFG0 =0x00;
 //DMA 0
 pstDMA2->DMA0_ISA = Buf;
 //pstDMA2->DMA0_IDA = 0x08048408;
 pstDMA2->DMA0_IDA = 0xA8610008;
 pstDMA2->DMA0_CFG0= 0x00000004;
 //pstDMA2->DMA0_CFG1= 0x00400003;
 pstDMA2->DMA0_CFG1= 0x00402023;
 pstDMA2->DMA0_SWT = 0x0000000b;
 //DMA1
 //pstDMA2->DMA1_ISA = 0x0804840C;
 pstDMA2->DMA1_ISA = 0xA861000C;
 pstDMA2->DMA1_IDA = Buf +0x20000;
 pstDMA2->DMA1_CFG0= 0x00000004;
 //pstDMA2->DMA1_CFG1= 0x00400300;
 pstDMA2->DMA1_CFG1= 0x00402320;
 pstDMA2->DMA1_SWT = 0x0000000b;

}
SBYTE  SPI_Wait_DataTransfer(void)
{
    register DWORD cause;
    register DWORD timeout;

    timeout = 0x10000;
    // printf("\r\nSPI  STATUS REGISTER %x ",pstSPI->SPSTA);
    while(!(pstSPI->SPSTA & 0x00000100) && timeout)
    {
        timeout--;
    }                      
  //  printf("\r\nSPI  STATUS REGISTER %x , Timeout %i ",pstSPI->SPSTA,timeout);
    if(timeout == 0x00)
    return SPI_TIMEOUT;


    cause = pstSPI->SPSTA;

    if(!(cause & 0x000000C0))
    {
       //printf("\r\nSPI Transfer ok");
	return SPI_PASS;
    }
    else if(cause & 0x00000040)
    {
        printf("\r\nTX  Data  Error");
        return SPI_TX_Error;
    }
    else if(cause & 0x00000080)
    {
        printf("\r\nRX Data  Error");
        return SPI_RX_Error;
    }
    else
    {
      return 0;
    }
}

void SPI_FIFO_Reset()
{

	 pstSPI->SPSTA = 0x00000000;
	 pstSPI->SPCON   = 0x00000080;
	 pstSPI->SPCON  = 0x00000000;
	// printf("\r\nSPI  STATUS REGISTER %x ",pstSPI->SPSTA);

}