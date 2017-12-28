#ifdef SCM
#include <stdio.h>
#include <taskLib.h>
#include "drv_S29GL256N.h"

#define U8  unsigned char
#define U16 unsigned short
#define U32 unsigned long
#define U64 unsigned long long
 
volatile U64 *FlashBaseAddress;

void FlashCommand(int data);
IMPORT U64	reset_flash(double a); 
IMPORT U64	flashid_command0(double x);
IMPORT U64	flashid_command1(double y); 
IMPORT U64	flashid_command2(double z); 
IMPORT U64  get_flashid();

/* test subroutine  */
int TargetAddress= 0xB7FFF000; 
int targetSize= 0x1000;
int SourceAddress; 
int FileSize= 32;

const U64 pData[] =
{
      0x0001000200030004, 0x0005000600070008,
      0x0009000A000B000C, 0x000D000E000F0010
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
		TargetPt++;
     }while (TargetPt < (EndPt));
	
	 return data;
}

/*  S29GL256N APIs  */
int S29GL256N_CheckId(void)
{
    /* volatile U64 manId; */ 
	U64 command,arg0,arg1,arg2,arg3;
	
	arg0=0x00f000f000f000f0; 
	arg1=0x00aa00aa00aa00aa;
	arg2=0x0055005500550055;
	arg3=0x0090009000900090;
	FlashBaseAddress = (U64 *)( FLASH_ADDR);

	command=reset_flash(arg0);
	printf("reset command0=0x%X\n",(U16)command);
	printf("reset command1=0x%X\n",(U16)((command&0xffff0000)>>16));
	printf("reset command2=0x%X\n",(U16)((command&0xffff00000000)>>32));
	printf("reset command3=0x%X\n",(U16)((command&0xffff000000000000)>>48));

	command=flashid_command0(arg1);
	printf("reset command0=0x%X\n",(U16)command);
	printf("reset command1=0x%X\n",(U16)((command&0xffff0000)>>16));
	printf("reset command2=0x%X\n",(U16)((command&0xffff00000000)>>32));
	printf("reset command3=0x%X\n",(U16)((command&0xffff000000000000)>>48));
/*
	command=flashid_command1(arg2);
	printf("reset command0=0x%X\n",(U16)command);
	printf("reset command1=0x%X\n",(U16)((command&0xffff0000)>>16));
	printf("reset command2=0x%X\n",(U16)((command&0xffff00000000)>>32));
	printf("reset command3=0x%X\n",(U16)((command&0xffff000000000000)>>48));

	command=flashid_command2(arg3);
	printf("reset command0=0x%X\n",(U16)command);
	printf("reset command1=0x%X\n",(U16)((command&0xffff0000)>>16));
	printf("reset command2=0x%X\n",(U16)((command&0xffff00000000)>>32));
	printf("reset command3=0x%X\n",(U16)((command&0xffff000000000000)>>48));

    command=get_flashid(FlashBaseAddress);
	printf("reset command0=0x%X\n",(U16)command);
	printf("reset command1=0x%X\n",(U16)((command&0xffff0000)>>16));
	printf("reset command2=0x%X\n",(U16)((command&0xffff00000000)>>32));
	printf("reset command3=0x%X\n",(U16)((command&0xffff000000000000)>>48));
*/
/*
	printf("devId0=0x%X\n",(U16)devId);
	printf("devId1=0x%X\n",(U16)((devId&0xffff0000)>>16));
	printf("devId2=0x%X\n",(U16)((devId&0xffff00000000)>>32));
	printf("devId3=0x%X\n",(U16)((devId&0xffff000000000000)>>48));

	manId=flashid_command();
	
    FlashBaseAddress = (U64 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f000f000f000f0; 
	
	FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00aa00aa00aa00aa;
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x000002aa<<3));
    *FlashBaseAddress = 0x0055005500550055;
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x0090009000900090;

	FlashBaseAddress = (U64 *)( FLASH_ADDR);
    manId= *FlashBaseAddress;
    FlashBaseAddress++;
	devId= *FlashBaseAddress;

    printf("\n\nManufacture ID0(0x01)=0x%x,Device ID0(0x227E)=0x%x\n",(U16)manId,(U16)devId);
    printf("\n\nManufacture ID1(0x01)=0x%x,Device ID1(0x227E)=0x%x\n",(U16)((manId&0xffff0000)>>16),(U16)((devId&0xffff0000)>>16));
	printf("\n\nManufacture ID2(0x01)=0x%x,Device ID2(0x227E)=0x%x\n",(U16)((manId&0xffff00000000)>>32),(U16)((devId&0xffff00000000)>>32));
    printf("\n\nManufacture ID3(0x01)=0x%x,Device ID3(0x227E)=0x%x\n",(U16)((manId&0xffff000000000000)>>48),(U16)((devId&0xffff000000000000)>>48));
*/
	return 1;
}

int S29GL256N_Chip_Erase(void)
{
    U64 Data=0;
    int i;


	FlashBaseAddress = (U64 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f000f000f000f0;

    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00aa00aa00aa00aa;
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x000002aa<<3));
    *FlashBaseAddress = 0x0055005500550055;
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x0080008000800080;

    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00aa00aa00aa00aa;
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x000002aa<<3));
    *FlashBaseAddress = 0x0055005500550055;
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x0010001000100010;

	taskDelay(100); /* 1ms * 100 = 100ms */

    for (i=0; i<6000 ; i++)   /* 6000*1ms  = 6sec  */
        {
           Data = *FlashBaseAddress;
           if(Data == 0xffffffffffffffff)
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

    FlashBaseAddress = (U64 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f000f000f000f0;

    for(i=0;i<targetSize;i+=8)
    {
        j=*((U64 *)(i+TargetAddress));
        if( j!=0xffffffffffffffff)
          {
			 printf("\nError:0x%x=0x%x\n",(U16)(i+TargetAddress),(U16)j);
             return 0;
          }
    }
    printf("\nBlankCheck done. %d",i);
    return 1;
}

int S29GL256N_Programming(int SourceAddress,int TargetAddress,int FileSize)
{
    U64 *TargetPt;
    U64 *EndPt;
    U64 *SourcePt;
    U64 Data;
    int j;

	SourcePt=(U64 *)SourceAddress;
    TargetPt=(U64 *)TargetAddress;
    EndPt= (U64 *)(TargetAddress+FileSize);
    printf("reset\n");
    FlashBaseAddress = (U64 *)(FLASH_ADDR);
    *FlashBaseAddress = 0x00f000f000f000f0;

    /*  write 1st and 2nd flash */   
    Data=*SourcePt;
	printf("0x00aa00aa00aa00aa\n");
	FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00aa00aa00aa00aa;
	printf("0x0055005500550055\n");
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x000002aa<<3));
    *FlashBaseAddress = 0x0055005500550055;
	printf("0x00a000a000a000a0\n");
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555<<3));
    *FlashBaseAddress = 0x00a000a000a000a0;
 	TargetPt=(U64 *)TargetAddress;
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
/*
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
*/
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
    U64 *SourcePt;
    U64 j;
    int i;
       
    SourcePt = (U64 *) SourceAddress;
    FlashBaseAddress = (U64 *)( FLASH_ADDR);
    *FlashBaseAddress = 0x00f000f000f000f0;

    for(i=0;i<targetSize;i+=8)
    {
        j=*((U64 *)(i+TargetAddress));
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
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555));
    *FlashBaseAddress = 0x00aa00aa;
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x000002aa));
    *FlashBaseAddress = 0x00550055;
    FlashBaseAddress = (U64 *)(( FLASH_ADDR | 0x00000555));
    *FlashBaseAddress = data;
}
#endif  /* SCM */
