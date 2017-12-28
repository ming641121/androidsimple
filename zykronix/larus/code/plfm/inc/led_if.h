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

#ifndef _LED_IF_H_
#define _LED_IF_H_

/*
 *====================================================================
 *=                            include                               =
 *====================================================================
 */

/*
 *====================================================================
 *=                            definition                            =
 *====================================================================
 */
#if 0
#define  LED_IP_1_ALARM	            0	    T
#define  LED_IP_2_ALARM	            1		T
#define  LED_IP_3_ALARM	            2		T
#define  LED_IP_4_ALARM	            3		T
#define  LED_IP_5_ALARM	            4		T
#define  LED_ENABLE	                5 x  
#define  LED_SERIAL_PORT	        6		B
#define  LED_HOLD 	                7		B
#define  LED_IP_1_PRESENT	        8
#define  LED_IP_2_PRESENT	        9
#define  LED_IP_3_PRESENT	        10
#define  LED_IP_4_PRESENT	        11
#define  LED_IP_5_PRESENT	        12
#define  LED_IP_A_TRACK	            13 x	B FREERUN
#define  LED_IP_B_TRACK	            14 x	B TRACK
#define  LED_GPS_A_TRACK	        15		B
#define  LED_GPS_B_TRACK			16		B
#define  LED_OUTPUT_ALARM	        17		B

#define  LED_FUSE_ALARM	            18		B
#define  LED_STATUS	                19		R/G

#define  LED_CRITICAL_ALARM	    	20		R/G
#define  LED_MAJOR_ALARM	        21		R/G
#define  LED_MINOR_ALARM	        22		R/G
#define  LED_ACO          	        23		B

#define  MAX_LED_NUM     	        24
#endif


#define  LED_IP_1_ALARM	            0	    
#define  LED_IP_2_ALARM	            1		
#define  LED_IP_3_ALARM	            2		
#define  LED_IP_4_ALARM	            3		
#define  LED_IP_5_ALARM	            4		
#define  LED_ENABLE	                5   
#define  LED_SERIAL_PORT	        6		
#define  LED_HOLD 	                7		
#define  LED_IP_1_PRESENT	        8
#define  LED_IP_2_PRESENT	        9
#define  LED_IP_3_PRESENT	        10
#define  LED_IP_4_PRESENT	        11
#define  LED_IP_5_PRESENT	        12
#define  LED_IP_A_TRACK	            13 
#define  LED_IP_B_TRACK	            14
#define  LED_GPS_A_TRACK	        15		
#define  LED_GPS_B_TRACK			16		
#define  LED_OUTPUT_ALARM	        17		

#define  LED_FUSE_ALARM	            18		
#define  LED_STATUS_PLUS		19		
#define  LED_STATUS_MINUS		20
#define  LED_CRITICAL_ALARM	    	21		
#define  LED_MAJOR_ALARM	        22		
#define  LED_MINOR_ALARM	        23		
#define  LED_ACO          	        24	

#define  MAX_LED_NUM     	        25

#define  LED_OFF			        0
#define  LED_ON			            1

#define  LED_CRITICAL_ALARM_OFF		1
#define  LED_CRITICAL_ALARM_ON      0

#define  LED_MAJOR_ALARM_OFF		1
#define  LED_MAJOR_ALARM_ON         0

#define  LED_MINOR_ALARM_OFF		1
#define  LED_MINOR_ALARM_ON         0

#define  LED_BLINK_OFF			    0
#define  LED_BLINK_LOW			    1
#define  LED_BLINK_MED			    2
#define  LED_BLINK_HIGH			    3

#define  LED_PORT_A			        0
#define  LED_PORT_B			        1
#define  LED_PORT_C			        2
#define  LED_PORT_D			        3


/*
 *====================================================================
 *=                            typedef                               =
 *====================================================================
 */

typedef struct 
{
    char ledNum;
	char ledOnOff;
	char ledBlink;
	char ledPort;
	int  ledBit;
} LED_ENTRY_T;

/*
 *====================================================================
 *=                            function                              =
 *====================================================================
 */

extern int ledOp(char ledNum, char ledOnOff, char ledBlink);
extern void initLED();

#endif  /* _LED_IF_H_ */
