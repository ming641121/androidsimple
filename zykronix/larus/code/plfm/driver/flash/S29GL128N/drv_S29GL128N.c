/* 
 *
 * 128 Sector, Each Sector 128K Bytes
 */

#include <stdio.h>
#include <taskLib.h>
#include "drv_S29GL128N.h"

#define U8  unsigned char
#define U16 unsigned short
#define U32 unsigned long
#define U64 unsigned long long
 
volatile U16 *FlashBaseAddress;

void FlashCommand(int data);
extern void flDelayLoop(int delay);
#define FLASH_DELAY_COUNT         10000
#define FLASH_PROGRAM_DELAY_COUNT 100
  
/* test subroutine  */
int TargetAddress= 0xB01F8000; 
int targetSize= 0x1000;
int SourceAddress= 0xB0010000; 
int FileSize= 32;

const U16 pData[] =
{
      0x0102, 0x0304, 0x0506, 0x0708,
      0x090A, 0x0B0C, 0x0D0E, 0x0F10
};

void test_blank(void)
{
	 BlankCheck(TargetAddress, targetSize);
}

void test_programming(void)
{ 
	S29GL128N_Programming((int)pData, TargetAddress, FileSize);
}

void test_compare(void)
{
 	S29GL128N_Programming((int)pData, TargetAddress, FileSize);
    SourceAddress= 0xB0000000;
	S29GL128N_Programming((int)pData, SourceAddress, FileSize);
	Verify(SourceAddress, TargetAddress, targetSize);
}

U16 test_read(void)
{
     U16 data;
	  
	 data= *((U16 *)TargetAddress);
     printf("\n0x%x",data);

	 return data;
}

U16 test_read_block(void)
{
    U16 data;
	
    U16 *TargetPt=(U16 *)TargetAddress;
    U16 *EndPt= (U16 *)(TargetAddress+FileSize);

    do
     {
        data= *TargetPt;    
        printf("\n0x%x",data);
		TargetPt++;
     }while (TargetPt < (EndPt));
	
	 return data;
}

/*  S29GL128N APIs  */
int S29GL128N_CheckId(void)
{
    volatile U16 manId, devId;

	FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f0;
    
    FlashCommand(0x90);

	FlashBaseAddress = (U16 *)( FLASH_ADDR);
    manId= *FlashBaseAddress;
    FlashBaseAddress ++;
	devId= *FlashBaseAddress;

	FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f0;

    printf("\n\nManufacture ID0(0x01)=0x%x,Device ID0(0x227E)=0x%x\n",(U16)manId,(U16)devId);

    return 1;
}

int S29GL128N_Chip_Erase(void)
{
    U16 Data=0;
    int i;


	FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f0;

    FlashCommand(0x80);

    FlashCommand(0x10);

	flDelayLoop(FLASH_DELAY_COUNT);

    for (i=0; i<6000000 ; i++)   /* 600000 * 100us  = 60 sec  */
        {
           Data = *FlashBaseAddress;
           if(Data == 0xffff)
           {
		      /* printf("Chip Erase OK, Time: %d (100us)\n", i);  */
              return 1;
           }
	       flDelayLoop(FLASH_DELAY_COUNT);
        }
	printf("Chip Erase Timeout 60sec\n"); 
    return(0);
}

int S29GL128N_Erase_Sector(unsigned long SectorOffset)
{
    U16 Data=0;
    int i;


	FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f0;

    FlashCommand(0x80);

    FlashBaseAddress = (U16 *)(( FLASH_ADDR | 0x00000555<<1));
    *FlashBaseAddress = 0xaa;
    FlashBaseAddress = (U16 *)(( FLASH_ADDR | 0x000002aa<<1));
    *FlashBaseAddress = 0x55;
    FlashBaseAddress = (U16 *)(( FLASH_ADDR | SectorOffset));
    *FlashBaseAddress = 0x30;

	flDelayLoop(FLASH_DELAY_COUNT);

    for (i=0; i<6000000 ; i++)   /* 600000 *100us  = 60 sec  */
        {
           Data = *FlashBaseAddress;
           if(Data == 0xffff)
           {
		      /* printf("Sector Erase OK, Time: %d (100us)\n", i);  */
              return 1;
           }
	       flDelayLoop(FLASH_DELAY_COUNT);
        }
	printf("Sector Erase Timeout: 60sec\n");
    return(0);
}

int BlankCheck(int TargetAddress,int targetSize)
{
    int i;
    U16 j;

    FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;

    for(i=0;i<targetSize;i+=2)
    {
        j=*((U16 *)(i+TargetAddress));
        if( j!=0xffff)
          {
			 printf("\nError:0x%x=0x%x\n",(i+TargetAddress),(U16)j);
             return 0;
          }
    }
    printf("\nBlankCheck done. %d",i);
    return 1;
}

int S29GL128N_Programming(int SourceAddress,int TargetAddress,int FileSize)
{
    U16 *TargetPt;
    U16 *EndPt;
    U16 *SourcePt;
    U16 Data;
    int j;

    SourcePt=(U16 *) SourceAddress;
    TargetPt=(U16 *) TargetAddress;
    EndPt= (U16 *)(TargetAddress+FileSize);

    FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;
    do
     {
       FlashCommand(0xa0);
       Data=*SourcePt;
       *TargetPt=Data;
       for(j=0 ; j<360000 ; j++)
          {
            Data=*TargetPt;
            if( Data == *SourcePt )
			{
			    /* printf("Program Time: %d(100us)\n", j); */
                break;
			}
	        flDelayLoop(FLASH_PROGRAM_DELAY_COUNT);
          }
          if(j == 360000)
		  {
		     printf("Timeout 360ms\n");
             return (0);
          }
		  TargetPt++;
          SourcePt++;

     }while (TargetPt < (EndPt));
     /* printf("\n\nProgram Flash success."); */
     return(FileSize);

}

int Verify(int SourceAddress,int TargetAddress,int targetSize)
{
    U16 *SourcePt;
    U16 j;
    U16 i;
       
    SourcePt = (U16 *) SourceAddress;
    FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;

    for(i=0;i<targetSize;i+=2)
    {
        j=*((U16 *)(i+TargetAddress));
        if( j!=*SourcePt)
         {
            printf("\nError:0x%x=0x%x\n",((U16)(i+TargetAddress)),(U16)j);
            return 0;
         }
         SourcePt++;             
    }
    printf("\nVerify Pass.");
        
    return 1;
}

void FlashCommand(int data)
{
    FlashBaseAddress = (U16 *)(( FLASH_ADDR | 0x00000555<<1));
    *FlashBaseAddress = 0xaa;
    FlashBaseAddress = (U16 *)(( FLASH_ADDR | 0x000002aa<<1));
    *FlashBaseAddress = 0x55;
    FlashBaseAddress = (U16 *)(( FLASH_ADDR | 0x00000555<<1));
    *FlashBaseAddress = data;
}
