/*
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc.
 *
 * Abstract:
 *
 *    This file contains the Card Manager Task
 *
 * Author(s):
 *
 */


/*
 *====================================================================
 *=                            include                               =
 *====================================================================
 */
#include "vxWorks.h"
#include "stdio.h"
#include "arch/ppc/vxPpcLib.h"
#include "drv/parallel/m8260IOPort.h"
#include "drv/mem/m8260Siu.h"
#include "drv/mem/m8260Memc.h"
#include "drv/timer/m8260Clock.h"
#include "drv/sio/m8260Cp.h"
#include "drv/sio/m8260CpmMux.h"
#include "drv/mem/m82xxDpramLib.h"
#include "ads827x.h"
#include "led_if.h"
#include <taskLib.h>

int ledDbg = 0;

#define LED_DEBUG



/*
 *====================================================================
 *=                            definition                            =
 *====================================================================
 */

/*
 *====================================================================
 *=                            extern                                =
 *====================================================================
 */
IMPORT UINT32 vxImmrGet(void); 

/*									 
 *====================================================================
 *=                            global                                =
 *====================================================================
 */
LED_ENTRY_T ledTable[MAX_LED_NUM] =
{
/*  0 */ 	{  LED_IP_1_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC28   },
/*  1 */ 	{  LED_IP_2_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC27   },
/*  2 */ 	{  LED_IP_3_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC26   },
/*  3 */ 	{  LED_IP_4_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC25   },
/*  4 */ 	{  LED_IP_5_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC24   },
/*  5 */ 	{  LED_ENABLE,    	   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC22   },
/*  6 */ 	{  LED_SERIAL_PORT,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC19   },
#if 0
/*  7 */ 	{  LED_HOLD,           LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC18   },  /* Temp disable */
#else
/*  7 */ 	{  LED_HOLD,           LED_OFF,    LED_BLINK_OFF,   LED_PORT_B,     PB18   },  /* Temp disable */
#endif
/*  8 */ 	{  LED_IP_1_PRESENT,   LED_OFF,			LED_BLINK_OFF,   LED_PORT_C,     PC17   },
/*  9 */ 	{  LED_IP_2_PRESENT,   LED_OFF,			LED_BLINK_OFF,   LED_PORT_C,     PC16   },
/* 10 */ 	{  LED_IP_3_PRESENT,   LED_OFF,			LED_BLINK_OFF,   LED_PORT_C,     PC15   },
/* 11 */ 	{  LED_IP_4_PRESENT,   LED_OFF,			LED_BLINK_OFF,   LED_PORT_C,     PC14   },
/* 12 */ 	{  LED_IP_5_PRESENT,   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC13   },
/* 13 */ 	{  LED_IP_A_TRACK,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC12   },
/* 14 */ 	{  LED_IP_B_TRACK,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC11   },
/* 15 */ 	{  LED_GPS_A_TRACK,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC10   },
/* 16 */ 	{  LED_GPS_B_TRACK,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC9    },
/* 17 */ 	{  LED_OUTPUT_ALARM,   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC8    },

/* 18 */ 	{  LED_FUSE_ALARM,     LED_OFF,    	LED_BLINK_OFF,   LED_PORT_D,     PD23   },
/* 19 */ 	{  LED_STATUS_PLUS,    LED_OFF,  		LED_BLINK_OFF, 	LED_PORT_A,   	PA30   },
/* 20 */ 	{  LED_STATUS_MINUS,   LED_OFF,  		LED_BLINK_OFF, 	LED_PORT_A,   	PA24   },
/* 21 */ 	{  LED_CRITICAL_ALARM, LED_OFF,    LED_BLINK_OFF,   LED_PORT_A,     PA13   },
/* 22 */ 	{  LED_MAJOR_ALARM,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_A,     PA12   },
/* 23 */ 	{  LED_MINOR_ALARM,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_A,     PA11   },
/* 24 */ 	{  LED_ACO,            LED_OFF,    LED_BLINK_OFF,   LED_PORT_A,     PA10   }
};

/*
 *====================================================================
 *=                            routines                              =
 *====================================================================
 */

void initLED()
{
    int	immrVal = vxImmrGet();
    



    /* PORT A    */
    * M8260_IOP_PADAT(immrVal) &= ~ ( PA24  | PA30 |PA13  | PA12 | PA11 | PA10 );                                  
    * M8260_IOP_PADIR(immrVal) |=   ( PA24  | PA30 |PA13  | PA12 | PA11 | PA10 );
    * M8260_IOP_PAPAR(immrVal) &= ~ ( PA24  | PA30 |PA13  | PA12 | PA11 | PA10 );

 		/* PORT B  */
    * M8260_IOP_PBDAT(immrVal) &= ~ ( PB18  );                                        
    * M8260_IOP_PBDIR(immrVal) |=   ( PB18  );
    * M8260_IOP_PBPAR(immrVal) &= ~ ( PB18 );
        
  	/* PORT C  	 */   
    * M8260_IOP_PCDAT(immrVal) &= ~ ( PC8	 | PC9  | PC10 | PC11 | PC12 
                                    | PC13 | PC14 | PC15 | PC16 | PC17 | PC19 
                                    | PC22 | PC24 | PC25 | PC26 | PC27 | PC28 ); 
                                       
    * M8260_IOP_PCDIR(immrVal) |=   ( PC8	 | PC9  | PC10 | PC11 | PC12 
                                    | PC13 | PC14 | PC15 | PC16 | PC17 | PC19 
                                    | PC22 | PC24 | PC25 | PC26 | PC27 | PC28 ); 

    * M8260_IOP_PCPAR(immrVal) &= ~ ( PC8	 | PC9  | PC10 | PC11 | PC12 
                                    | PC13 | PC14 | PC15 | PC16 | PC17 | PC19 
                                    | PC22 | PC24 | PC25 | PC26 | PC27 | PC28 ); 


    /* PORT D                                          */
		* M8260_IOP_PDDAT(immrVal) &= ~ (   PD23 );                                        
    * M8260_IOP_PDDIR(immrVal) |=   (   PD23 );
    * M8260_IOP_PDPAR(immrVal) &= ~ (   PD23 );




}

int ledOp(char ledNum, char ledOnOff, char ledBlink)
{
    int	immrVal = vxImmrGet();

    (ledTable[(int)ledNum]).ledOnOff = ledOnOff;
    (ledTable[(int)ledNum]).ledBlink = ledBlink;

    switch(ledTable[(int)ledNum].ledPort)
	{
	    case LED_PORT_A:
			if(ledDbg)
		        printf("LED_PORT_A\r\n");
		    if(ledOnOff == 	LED_ON)
			{
    		    * M8260_IOP_PADAT(immrVal) |=   ( (ledTable[(int)ledNum]).ledBit ); 
			}
			else
			{
                * M8260_IOP_PADAT(immrVal) &= ~ ( (ledTable[(int)ledNum]).ledBit); 
			}
		break;

	    case LED_PORT_B:
			if(ledDbg)
		        printf("LED_PORT_B\r\n");
		    if(ledOnOff == 	LED_ON)
			{
    		    * M8260_IOP_PBDAT(immrVal) |=   ( (ledTable[(int)ledNum]).ledBit ); 
			}
			else
			{
                * M8260_IOP_PBDAT(immrVal) &= ~ ( (ledTable[(int)ledNum]).ledBit); 
			}
		break;

	    case LED_PORT_C:
			if(ledDbg)
		        printf("LED_PORT_C\r\n");
		    if(ledOnOff == 	LED_ON)
			{
    		    * M8260_IOP_PCDAT(immrVal) |=   ( (ledTable[(int)ledNum]).ledBit ); 
			}
			else
			{
                * M8260_IOP_PCDAT(immrVal) &= ~ ( (ledTable[(int)ledNum]).ledBit); 
			}
		break;

	    case LED_PORT_D:
			if(ledDbg)
		        printf("LED_PORT_D\r\n");
		    if(ledOnOff == 	LED_ON)
			{
    		    * M8260_IOP_PDDAT(immrVal) |=   ( (ledTable[(int)ledNum]).ledBit ); 
			}
			else
			{
                * M8260_IOP_PDDAT(immrVal) &= ~ ( (ledTable[(int)ledNum]).ledBit); 
			}
		break;

        default:
		break;
	}
		   
	return OK;
}

#if 0
GPIO_ENTRY_T gpioTable[MAX_LED_NUM] =
{
	{  LED_IP_1_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC28   },
	{  LED_IP_2_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC27   },
	{  LED_IP_3_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC26   },
	{  LED_IP_4_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC25   },
	{  LED_IP_5_ALARM,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC24   },
	{  LED_ENABLE,    	   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC22   },
	{  LED_SERIAL_PORT,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC19   },
	{  LED_HOLD,           LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC18   },
	{  LED_IP_1_PRESENT,   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC17   },
	{  LED_IP_2_PRESENT,   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC16   },
	{  LED_IP_3_PRESENT,   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC15   },
	{  LED_IP_4_PRESENT,   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC14   },
	{  LED_IP_5_PRESENT,   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC13   },
	{  LED_IP_A_TRACK,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC12   },
	{  LED_IP_B_TRACK,     LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC11   },
	{  LED_GPS_A_TRACK,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC10   },
	{  LED_GPS_B_TRACK,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC9    },
	{  LED_OUTPUT_ALARM,   LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC8    },
	{  LED_CRITICAL_ALARM, LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC7    },
	{  LED_MAJOR_ALARM,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC6    },
	{  LED_MINOR_ALARM,    LED_OFF,    LED_BLINK_OFF,   LED_PORT_C,     PC5    }


};
#endif
void init_osc_power()
{
    int	immrVal = vxImmrGet();

	/* OSC1, output off PA31 */
    * M8260_IOP_PADAT(immrVal) &= ~ ( PA31 ); 

    * M8260_IOP_PADIR(immrVal) |=   ( PA31 );

    /* OSC2, output off PB31 */
    * M8260_IOP_PBDAT(immrVal) &= ~ ( PB31 ); 

    * M8260_IOP_PBDIR(immrVal) |=   ( PB31 );

    /* OSC RESET, output high, PA20 */
    * M8260_IOP_PADAT(immrVal) |=   ( PA20 ); 

    * M8260_IOP_PADIR(immrVal) |=   ( PA20 );
}

void set_osc_power(int oscnum, int osconoff)
{
    int	immrVal = vxImmrGet();

    if(oscnum == 0)
    {
	    if(osconoff == 0)
		{
            * M8260_IOP_PADAT(immrVal) &= ~ ( PA31 ); 
		}
		else
		{
            * M8260_IOP_PADAT(immrVal) |=   ( PA31 ); 
		}
    }
    else
	{
	    if(osconoff == 0)
		{
            * M8260_IOP_PBDAT(immrVal) &= ~ ( PB31 ); 
		}
		else
		{
            * M8260_IOP_PBDAT(immrVal) |=   ( PB31 ); 
		}
    }   

}

#ifdef LED_DEBUG


void ledDebugSelfTest(){
	int i;
	int delaytime=50;
	initLED();

    /*sequencial test*/       
    ledOp(LED_SERIAL_PORT, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_SERIAL_PORT, LED_OFF, 0);
    
    ledOp(LED_STATUS_PLUS, LED_ON, 0);
    ledOp(LED_STATUS_MINUS, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_STATUS_PLUS, LED_ON, 0);
    ledOp(LED_STATUS_MINUS, LED_OFF, 0);
    taskDelay(delaytime);
    ledOp(LED_STATUS_PLUS, LED_OFF, 0);
    ledOp(LED_STATUS_MINUS, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_STATUS_PLUS, LED_OFF, 0);  
    ledOp(LED_STATUS_MINUS, LED_OFF, 0);   

    ledOp(LED_GPS_A_TRACK, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_GPS_A_TRACK, LED_OFF, 0); 

    ledOp(LED_GPS_B_TRACK, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_GPS_B_TRACK, LED_OFF, 0);     

		for (i=0; i < 5; i++){
       
        ledOp(LED_IP_1_ALARM + i, LED_ON, 0);
        ledOp(LED_IP_1_PRESENT + i, LED_ON, 0);
				taskDelay(delaytime);
        
        ledOp(LED_IP_1_ALARM +i, LED_ON, 0);
        ledOp(LED_IP_1_PRESENT +i, LED_OFF, 0);
				taskDelay(delaytime);
        
        ledOp(LED_IP_1_ALARM +i, LED_OFF, 0);
        ledOp(LED_IP_1_PRESENT +i, LED_ON, 0);
				taskDelay(delaytime);
        
        ledOp(LED_IP_1_ALARM +i, LED_OFF, 0);
        ledOp(LED_IP_1_PRESENT +i, LED_OFF, 0);
				
				taskDelay(delaytime);
     }





    ledOp(LED_IP_A_TRACK, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_IP_A_TRACK, LED_OFF, 0);  		
		
    ledOp(LED_IP_B_TRACK, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_IP_B_TRACK, LED_OFF, 0);  

    ledOp(LED_HOLD, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_HOLD, LED_OFF, 0);  		
		
    ledOp(LED_OUTPUT_ALARM, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_OUTPUT_ALARM, LED_OFF, 0);  

    ledOp(LED_CRITICAL_ALARM, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_CRITICAL_ALARM, LED_OFF, 0);  		
		
    ledOp(LED_MAJOR_ALARM, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_MAJOR_ALARM, LED_OFF, 0);  

    ledOp(LED_MINOR_ALARM, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_MINOR_ALARM, LED_OFF, 0);  		
		
    ledOp(LED_FUSE_ALARM, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_FUSE_ALARM, LED_OFF, 0);      
    
		ledOp(LED_ACO, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_ACO, LED_OFF, 0);  
	
	
	
		/*all on test*/
	    ledOp(LED_SERIAL_PORT, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_STATUS_PLUS, LED_ON, 0);
    taskDelay(delaytime);
    ledOp(LED_STATUS_MINUS, LED_ON, 0);
    taskDelay(delaytime);
		ledOp(LED_GPS_A_TRACK, LED_ON, 0);
		taskDelay(delaytime);
		ledOp(LED_GPS_B_TRACK, LED_ON, 0);
		taskDelay(delaytime);
		for (i=0; i < 5; i++){
        ledOp(LED_IP_1_ALARM + i, LED_ON, 0);
        taskDelay(delaytime);
        ledOp(LED_IP_1_PRESENT + i, LED_ON, 0);
        taskDelay(delaytime);

     }

    ledOp(LED_IP_A_TRACK, LED_ON, 0);
    taskDelay(delaytime);
 		
		
    ledOp(LED_IP_B_TRACK, LED_ON, 0);
    taskDelay(delaytime);
 

    ledOp(LED_HOLD, LED_ON, 0);
    taskDelay(delaytime);
 
		
    ledOp(LED_OUTPUT_ALARM, LED_ON, 0);
    taskDelay(delaytime);
  

    ledOp(LED_CRITICAL_ALARM, LED_ON, 0);
    taskDelay(delaytime); 	
		
    ledOp(LED_MAJOR_ALARM, LED_ON, 0);
    taskDelay(delaytime); 

    ledOp(LED_MINOR_ALARM, LED_ON, 0);
    taskDelay(delaytime); 		
		
    ledOp(LED_FUSE_ALARM, LED_ON, 0);
    taskDelay(delaytime);      
    
		ledOp(LED_ACO, LED_ON, 0);
    taskDelay(delaytime); 
	}
void ledtest(){
	ledDebugSelfTest();
	}	
#endif


