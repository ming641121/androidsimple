#ifdef ADSL24V
#include "drv_api.h"

#include "drv_GEMINAXD_config.h"
#include "drv_GEMINAXD_interface.h"
#include "drv_GEMINAXD_api.h"
#include "drv_GEMINAXD_vxworks.h"
#include "drv_GEMINAXD_firmware.h"

#include "drv_MX29LV320AB.h"

#define U16 unsigned short
#define U32 unsigned long
#define U8  unsigned char
 
volatile U16 *FlashBaseAddress;

void FlashCommand(int data);

/* test subroutine  */
int TargetAddress= 0xB01F8000;
int targetSize= 0x1000;
int SourceAddress= 0xB0010000;
int FileSize= 32;

const U16 pData[] =
{
      0x0001, 0x0000, 0x0018, 0x0000, 0x0001, 0x7C00, 0x0059, 0x0000,
      0x6274, 0x0001, 0x0018, 0x0000, 0x00AA, 0x7800, 0x0059, 0x0000
};

void test_blank(void)
{
	 BlankCheck(TargetAddress, targetSize);
}

void test_programming(void)
{ 
	MX29LV320_Programming((int)pData, TargetAddress, FileSize);
}

void test_compare(void)
{
 	MX29LV320_Programming((int)pData, TargetAddress, FileSize);
    SourceAddress= 0xB0000000;
	MX29LV320_Programming((int)pData, SourceAddress, FileSize);
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


/*  MX29LV320 APIs  */
int MX29LV320_CheckId(void)
{
    volatile U16 manId,devId;
/*    
	FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;

    *((U16 *)  FLASH_ADDR + (0x555)) = 0xAAAA;
    *((U16 *)  FLASH_ADDR + (0x2AA)) = 0x5555;
    *((U16 *)  FLASH_ADDR + (0x555)) = 0x9090;
    manId = *((U16 *)  FLASH_ADDR); 
	devId=  *((U16 *)  FLASH_ADDR +1); 

	FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;
*/
	FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;
	
    FlashCommand(0x90);

    FlashBaseAddress = (U16 *)( FLASH_ADDR);
    manId=*FlashBaseAddress;

    FlashBaseAddress++; 
    devId=*FlashBaseAddress;

    FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;

    printf("\n\nManufacture ID(0xC2)=0x%2x, Device ID(0x22A8)=0x%2x\n",manId,devId);

    return 1;
}

int MX29LV320_Chip_Erase(void)
{
    U8 Data=0;
    int i;


    FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;

    FlashCommand(0x80);

    FlashCommand(0x10);
    taskDelay(100); /* 1 ms * 100 = 100ms */

    for (i=0; i<6000 ; i++)   /* 6000*1ms  = 6sec  */
        {
           Data = *FlashBaseAddress;
           if(Data == 0xff)
           {
              return(1);
           }
           taskDelay(10);  /* 10ms */
        }
        return(0);
}

int BlankCheck(int TargetAddress,int targetSize)
{
    int i,j;
 
    FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;
    for(i=0;i<targetSize;i+=2)
    {
        j=*((U16 *)(i+TargetAddress));
        if( j!=0xffff)
          {
			 printf("\nError:%x=%x\n",(i+TargetAddress),j);
             return 0;
          }
    }
    printf("\nBlankCheck done. %d",i);
    return 1;
}

int MX29LV320_Programming(int SourceAddress,int TargetAddress,int FileSize)
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
       for(j=0 ; j<360 ; j++)
          {
            Data=*TargetPt;
            if( Data == *SourcePt )
                break;
                taskDelay(1);
          }
          if(j == 360)
             return (0);
          
		  TargetPt++;
          SourcePt++;

     }while (TargetPt < (EndPt));
     printf("\n\nProgram Flash success.");
     return(1);
}

int Verify(int SourceAddress,int TargetAddress,int targetSize)
{
    U16 *SourcePt;
    U16 j;
    U32 i;
       
    SourcePt = (U16 *) SourceAddress;
    FlashBaseAddress = (U16 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;
    for(i=0;i<targetSize;i+=2)
    {
        j=*((U16 *)(i+TargetAddress));
        if( j!=*SourcePt)
         {
            printf("\nError:%x=%x\n",((int)i+TargetAddress),j);
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
#endif  /* ADSL24V */
