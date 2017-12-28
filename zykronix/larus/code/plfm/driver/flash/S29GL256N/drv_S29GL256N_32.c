#include <stdio.h>
#include <taskLib.h>
#include "drv_S29GL256N.h"

#define U8  unsigned char
#define U16 unsigned short
#define U32 unsigned long
#define U64 unsigned long long
 
volatile U32 *FlashBaseAddress;

void FlashCommand(int data);

/* test subroutine  */
int TargetAddress= 0xB7FFF000; 
int targetSize= 0x1000;
int SourceAddress; 
int FileSize= 32;

const U32 pData[] =
{
      0x00010002, 0x00030004, 0x00050006, 0x00070008,
      0x0009000A, 0x000B000C, 0x000D000E, 0x000F0010
};

void test_blank(void)
{
	 BlankCheck(TargetAddress, targetSize);
}

void test_programming(void)
{ 
	S29GL256N_Programming((int)pData, TargetAddress, FileSize);
}

void test_compare(void)
{
 	S29GL256N_Programming((int)pData, TargetAddress, FileSize);
    SourceAddress= 0xB0000000;
	S29GL256N_Programming((int)pData, SourceAddress, FileSize);
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

/*  S29GL256N APIs  */
int S29GL256N_CheckId(void)
{
    volatile U32 manId, devId;

	FlashBaseAddress = (U32 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f000f0;
    
	FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00aa00aa;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x000002aa<<3));
    *FlashBaseAddress = 0x00550055;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00900090;

	FlashBaseAddress = (U32 *)( FLASH_ADDR);
    manId= *FlashBaseAddress;
    FlashBaseAddress +=2;
	devId= *FlashBaseAddress;

    printf("\n\nManufacture ID0(0x01)=0x%x,Device ID0(0x227E)=0x%x\n",(U16)manId,(U16)devId);
    printf("\n\nManufacture ID1(0x01)=0x%x,Device ID1(0x227E)=0x%x\n",(U16)((manId&0xffff0000)>>16),(U16)((devId&0xffff0000)>>16));
/*
	FlashBaseAddress = (U32 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f000f0;
*/
	FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3)+4);
    *FlashBaseAddress = 0x00aa00aa;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x000002aa<<3)+4);
    *FlashBaseAddress = 0x00550055;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3)+4);
    *FlashBaseAddress = 0x00900090;

	FlashBaseAddress = (U32 *)( FLASH_ADDR+4);
    manId= *FlashBaseAddress;
    FlashBaseAddress +=2;
	devId= *FlashBaseAddress;

    printf("\n\nManufacture ID2(0x01)=0x%x,Device ID2(0x227E)=0x%x\n",(U16)manId,(U16)devId);
    printf("\n\nManufacture ID3(0x01)=0x%x,Device ID3(0x227E)=0x%x\n",(U16)((manId&0xffff0000)>>16),(U16)((devId&0xffff0000)>>16));
	
    return 1;
}

int S29GL256N_Chip_Erase(void)
{
    U32 Data=0;
    int i;


	FlashBaseAddress = (U32 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f000f0;

    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00aa00aa;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x000002aa<<3));
    *FlashBaseAddress = 0x00550055;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00800080;

    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00aa00aa;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x000002aa<<3));
    *FlashBaseAddress = 0x00550055;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00100010;

	taskDelay(100); /* 1ms * 100 = 100ms */

    for (i=0; i<6000 ; i++)   /* 6000*1ms  = 6sec  */
        {
           Data = *FlashBaseAddress;
           if(Data == 0xffffffff)
           {
              return 1;
           }
           taskDelay(10);  /* 10ms */
        }

	FlashBaseAddress = (U32 *)( FLASH_ADDR+4);
    *FlashBaseAddress = 0x00f000f0;

    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3)+4);
    *FlashBaseAddress = 0x00aa00aa;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x000002aa<<3)+4);
    *FlashBaseAddress = 0x00550055;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3)+4);
    *FlashBaseAddress = 0x00800080;

    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3)+4);
    *FlashBaseAddress = 0x00aa00aa;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x000002aa<<3)+4);
    *FlashBaseAddress = 0x00550055;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3)+4);
    *FlashBaseAddress = 0x00100010;

	taskDelay(100); /* 1ms * 100 = 100ms */

    for (i=0; i<6000 ; i++)   /* 6000*1ms  = 6sec  */
        {
           Data = *FlashBaseAddress;
           if(Data == 0xffffffff)
           {
              return 1;
           }
           taskDelay(10);  /* 10ms */
        }

    return(0);
}

int BlankCheck(int TargetAddress,int targetSize)
{
    int i;
    U64 j;

    FlashBaseAddress = (U32 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;

    for(i=0;i<targetSize;i+=4)
    {
        j=*((U32 *)(i+TargetAddress));
        if( j!=0xffffffff)
          {
			 printf("\nError:0x%x=0x%x\n",(i+TargetAddress),(U16)j);
             return 0;
          }
    }
    printf("\nBlankCheck done. %d",i);
    return 1;
}

int S29GL256N_Programming(int SourceAddress,int TargetAddress,int FileSize)
{
    U32 *TargetPt;
    U32 *EndPt;
    U32 *SourcePt;
    U32 Data;
    int j;

	SourcePt=(U32 *)SourceAddress;
    TargetPt=(U32 *)TargetAddress;
    EndPt= (U32 *)(TargetAddress+FileSize);
    printf("reset\n");
    FlashBaseAddress = (U32 *)(FLASH_ADDR);
    *FlashBaseAddress = 0x00f000f0;

    /*  write 1st and 2nd flash */   
    Data=*SourcePt;
	printf("0x00aa00aa\n");
	FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00aa00aa;
	printf("0x00550055\n");
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x000002aa<<3));
    *FlashBaseAddress = 0x00550055;
	printf("0x00a000a0\n");
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00a000a0;
 	TargetPt=(U32 *)TargetAddress;
    *TargetPt=Data;
	for(j=0 ; j<360 ; j++)
    {
        Data=*TargetPt;
	    printf("Data=0x%2x,*SourcePt=0x%2x\n",(U16)Data,(U16)*SourcePt);
        if( Data == *SourcePt )
            break;
	    taskDelay(1);
    }
    if(j == 360)
       return (0);

    /*  write 3rd and 4th flash */ 
	SourcePt++;
    Data=*SourcePt;
	printf("0x00aa00aa\n");
	FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3)+4);
    *FlashBaseAddress = 0x00aa00aa;
	printf("0x00550055\n");
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x000002aa<<3)+4);
    *FlashBaseAddress = 0x00550055;
	printf("0x00a000a0\n");
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555<<3)+4);
    *FlashBaseAddress = 0x00a000a0;
 	TargetPt=(U32 *)(TargetAddress+4);
    *TargetPt=Data;
	for(j=0 ; j<360 ; j++)
    {
        Data=*TargetPt;
	    printf("Data=0x%2x,*SourcePt=0x%2x\n",(U16)Data,(U16)*SourcePt);
        if( Data == *SourcePt )
            break;
	    taskDelay(1);
    }
    if(j == 360)
       return (0);

/*
    Data=*SourcePt;
    FlashCommand2(0xa0);
	TargetPt++;
    *TargetPt=Data;
    SourcePt++;

	Data=*SourcePt;
    FlashCommand4(0xa0);
	TargetPt++;
    *TargetPt=Data;
    SourcePt++;

	Data=*SourcePt;
    FlashCommand6(0xa0);
	TargetPt++;
    *TargetPt=Data;

*/

/*
    do
     {
       Data=*SourcePt;
       FlashCommand0(0xa0);
       *TargetPt=Data;
	   FlashCommand2(0xa0);
	   *(TargetPt+2)=Data;
	   FlashCommand4(0xa0);
       *(TargetPt+4)=Data;
	   FlashCommand6(0xa0);
	   *(TargetPt+6)=Data;
	   j=0;

       for(j=0 ; j<360 ; j++)
          {
            Data=*TargetPt;
			printf("Data=0x%2x,*SourcePt=0x%2x\n",Data,*SourcePt);
            if( Data == *SourcePt )
                break;
		    taskDelay(1);
          }
          if(j == 360)
             return (0);

		  TargetPt++;
          SourcePt++;

     }while (TargetPt < (EndPt));
*/
     printf("\n\nProgram Flash success.");
     return(1);
}

int Verify(int SourceAddress,int TargetAddress,int targetSize)
{
    U32 *SourcePt;
    U32 j;
    U32 i;
       
    SourcePt = (U32 *) SourceAddress;
    FlashBaseAddress = (U32 *)( FLASH_ADDR);
    *FlashBaseAddress = 0xf0;

    for(i=0;i<targetSize;i+=2)
    {
        j=*((U32 *)(i+TargetAddress));
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
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555));
    *FlashBaseAddress = 0x00aa00aa;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x000002aa));
    *FlashBaseAddress = 0x00550055;
    FlashBaseAddress = (U32 *)(( FLASH_ADDR | 0x00000555));
    *FlashBaseAddress = data<<16|data;
}

