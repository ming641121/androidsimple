 /* Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of Real Time Clock(RTC) Driver
 *
 * Note:
 *
 * Author(s): Klim
 *
 **********************************************************************
 * $Id: rtc.c,v 1.13 2006/10/03 05:43:04 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/driver/rtc/rtc.c,v $
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <tss_types.h>
#include <sys_os.h>
#include "register_map.h"
#include <rtc_if.h>


/* func declaration */
STATUS_t SysRtcRead(int rtcEntry, int offset, char *dataBuf, int nBytes);
STATUS_t SysRtcWrite(int rtcEntry, int offset, char *dataBuf, int nBytes);
int nvram_read(int nvOffset, char *srcAddr, int len);
int nvram_write(int nvOffset, char *srcAddr, int len);

int rtc_read(serialRtc_t *rtc);
int rtc_write(int hh, int mm, int ss, int dd, int dt, int mn, int yr, int ct);

#ifdef RTC_DEBUG
	static void rtctestsettime(void);
	static void rtctestshowtime(void);
	static void nvtestread(void);
	static void nvtestwrite(int toWrite);
#endif


/*========================================================================
* Function:	rtc_read
* 
* Description: 	Routine to read the rtc 	
* 
* In Parms:	the register_map.h structure of serialRtc_t
*			hr: hour
*			min: minute
*			sec: second
*			dy: day
*			dt: date
*			mn: month
*			yr: year
*			ct: century
*
*			sec_a: second alarm
*			min_a: minute alarm
*			hr_a: hour alarm
*			dydt_a: date/day alarm
* Out Parms:    none
*
* Returns: 	OK if success, ERROR otherwise
*/
#ifndef BOOT
/********************************************************************************
* STATUS_t SysRtcRead(RTC_ENTRY_t rtcEntry, int offset, char *dataBuf, int nBytes)
* function: 
*   to read data from RTC
* paramenter:
*  int rtcEntry - the entry to RTC static table
*  int Offset - the offset address
*  char *dataBuf - a data buffer to store the read-out
*  int len -  number of bytes to read
* return
*  OK or ERROR
********************************************************************************/
STATUS_t SysRtcRead(int rtcEntry, int offset, char *dataBuf, int nBytes)
{
    if (nvram_read(sysStaticRtcTable[rtcEntry].startLoc+offset, 
                   dataBuf, nBytes) == nBytes)
	    return OK;
	else
	    return ERROR;   
}

/********************************************************************************
* STATUS_t SysRtcWrite(RTC_ENTRY_t rtcEntry, int offset, char *dataBuf, int nBytes)
* function: 
*   to read data from RTC
* paramenter:
*  int rtcEntry - the entry to RTC static table
*  int Offset - the offset address
*  char *dataBuf - a data buffer to store the read-out
*  int len -  number of bytes to read
* return
*  OK or ERROR
********************************************************************************/
STATUS_t SysRtcWrite(int rtcEntry, int offset, char *dataBuf, int nBytes)
{
    if (nvram_write(sysStaticRtcTable[rtcEntry].startLoc+offset, 
                    dataBuf, nBytes) == nBytes)
	    return OK;
	else
	    return ERROR;   
}
#endif /* ! BOOT */
/*************************************************************************
* int nvram_read(int nvOffset, char *srcAddr, int len)
* function: 
*   to read nvram in real time clock
* paramenter:
*  int nvOffset - the offset address, range from 0 to 255
*  char *srcAddr - srcAddr is to stored the read-out
*      	Note: the storage should be allocated before calling this function.
*  int len -  the size(bytes) of the data being read.
* return
*  the number of bytes to be read out actually
***************************************************************************/ 
int nvram_read(int nvOffset, char *srcAddr, int len)
{
	int i;
	if (nvOffset > 255 || nvOffset < 0){return 0;}
	if (len < 0){return 0;}
	

	for (i=0; i < len; i++){
		if ( ( nvOffset + i ) > 255   )return i;    /* if the offset over the range */
		*RTC_ADDR_ERAM_ADDR = nvOffset + i;
		
		*(srcAddr +i) = *RTC_ADDR_ERAM_DATA ;	
		
	}

	return i+1;
}
/*************************************************************************
* int nvram_write(int nvOffset, char *srcAddr, int len)
* function: 
*   to write nvram in real time clock
* paramenter:
*  int nvOffset - the offset address, range from 0 to 255
*  char *srcAddr - srcAddr is to hold the data to be written.
*      	Note: the storage should be allocated before calling this function.
*  int len -  the size(bytes) of the data being read.
* return
*  the number of bytes being written actually
***************************************************************************/ 


int nvram_write(int nvOffset, char *srcAddr, int len)
{	
	int i;
	if (nvOffset > 255 || nvOffset < 0){return 0;}
	if (len < 0){return 0;}
	
	for (i=0; i < len; i++){
		if ( ( nvOffset + i ) > 255   )return i;    /* if the offset over the range */
		*RTC_ADDR_ERAM_ADDR = nvOffset + i;
		
		*RTC_ADDR_ERAM_DATA = *(srcAddr +i);	
		
	}
	
		return i+1;
}
/*========================================================================
* Function:	rtc_read
* 
* Description: 	Routine to read the rtc 	
* 
* In Parms:	the register_map.h structure of serialRtc_t
*			hh: hour
*			mm: minute
*			ss: second
*			dd: day
*			dt: date
*			mn: month
*			yr: year
*			ct: century
*			
*			te: transfer enable  
*			(disable first then set the value , when finished setting then enable it.)
* Out Parms:    none
*
* Returns: 	OK if success, ERROR otherwise
*/
int rtc_read(serialRtc_t *rtc)
{
	int sec_r, sec_3l;
	int min_r, min_3l;
	int hr_r, hr_2l;
	int dy, dt_2l, dt_r;
	int mn_1l, mn_r;
	int yr_l, yr_r;
	int ct_l, ct_r;
	int a1, a2, a3, a4;
	int sec_al, sec_ar;
	int min_al, min_ar;
	int hr_al, hr_ar;
	int dydt_al, dt_al, dydt_ar;	
	
	char *sec = RTC_ADDR_SEC;
	char *min = RTC_ADDR_MIN;
	char *hr = RTC_ADDR_HR;
	char *day = RTC_ADDR_DAY;
	char *date = RTC_ADDR_DATE;
	char *month = RTC_ADDR_MONTH;
	char *year = RTC_ADDR_YEAR;
	char *century = RTC_ADDR_CENTURY;
	char *sec_a = RTC_ADDR_SEC_A;
	char *min_a = RTC_ADDR_MIN_A;
	char *hr_a = RTC_ADDR_HR_A;
	char *dydt_a = RTC_ADDR_DAY_DATE_A;
	
	sec_r = (int)(*sec)&0x0F;
	sec_3l = (int)((*sec)>>4)&0x07;
	
	min_r = (int)(*min)&0x0F;
	min_3l = (int)((*min)>>4)&0x07;
	
	hr_r = (int)(*hr)&0x0F;
	hr_2l = (int)((*hr)>>4)&0x03;
	
	dy = (int)(*day)&0x07; 
	
	dt_r = (int)(*date)&0x0F;
	dt_2l = (int)((*date)>>4)&0x03;	
	
	mn_r = (int)(*month)&0x0F;
	mn_1l = (int)((*month)>>4)&0x01;	
	
	yr_r = (int)(*year)&0x0F;
	yr_l = (int)((*year)>>4)&0x0F;
	
	ct_r = (int)(*century)&0x0F;
	ct_l = (int)((*century)>>4)&0x0F;
	
	sec_ar = (int)(*sec_a)&0x0F;
	sec_al = (int)((*sec_a)>>4)&0x07;
	a1 = (int)((*sec_a)>>7)&0x01;
	
	min_ar = (int)(*min_a)&0x0F;
	min_al = (int)((*min_a)>>4)&0x07;
	a2 = (int)((*min_a)>>7)&0x01;	

	hr_ar = (int)(*hr_a)&0x0F;
	hr_al = (int)((*hr_a)>>4)&0x03;
	a3 = (int)((*hr_a)>>7)&0x01;
	
	dydt_ar = (int)(*dydt_a)&0x0F;
	dt_al = (int)((*dydt_a)>>4)&0x03;
	dydt_al = (int)((*dydt_a)>>6)&0x01;
	a4 = (int)((*dydt_a)>>7)&0x01;
		 
	if (sec_r < 0 && sec_r > 9 && sec_3l <0 && sec_3l >6){
		printf("over range second value\n");
	}
	else if (min_r < 0 && min_r > 9 && min_3l <0 && min_3l >6){
		printf("over range minute value\n");
	}
	else if (hr_r < 0 && hr_r > 9 && hr_2l <0 && hr_2l >2){
		printf("over range hour value\n");
	}
	else if (dy < 1 && dy > 7){
		printf("over range day value\n");
	}
	else if (dt_r < 0 && dt_r > 9 && dt_2l <0 && dt_2l >3){
		printf("over range date value\n");
	}
	else if (mn_r < 0 && mn_r > 9 && mn_1l <0 && mn_1l >1){
		printf("over range month value\n");
	}
	else if (yr_r < 0 && yr_r > 9 && yr_l <0 && yr_l >9){
		printf("over range year value\n");
	}
	else if (ct_r < 0 && ct_r > 9 && ct_l <0 && ct_l >3){
		printf("over range century value\n");
	}
	else if (sec_ar < 0 && sec_ar > 9 && sec_al <0 && sec_al >6){
		printf("over range alarm second value\n");
	}
	else if (min_ar < 0 && min_ar > 9 && min_al <0 && min_al >6){
		printf("over range alarm minute value\n");
	}
	else if (hr_ar < 0 && hr_ar > 9 && hr_al <0 && hr_al >3){
		printf("over range alarm hour value\n");
	}
	else if (dydt_al < 0 && dydt_al >1 && dt_al <0 && dt_al > 3 && dydt_ar< 0 && dydt_ar >9 ){
		printf("over range alarm day/date value\n");
	}	
	else{
		rtc->ss = sec_3l*10 + sec_r;
		rtc->mm = min_3l*10 + min_r;
		rtc->hh = hr_2l*10 + hr_r;
		rtc->day = dy;
		rtc->date = dt_2l*10 + dt_r;
		rtc->month = mn_1l*10 + mn_r;
		rtc->year = yr_l*10 + yr_r;
		rtc->century = ct_l*10 + ct_r;
		rtc->a1 = a1;
		rtc->ss_a = sec_al*10 + sec_ar;
		rtc->a2 = a2;
		rtc->mm_a = min_al*10 + min_ar;
		rtc->a3 = a3;
		rtc->hh_a = hr_al*10 + hr_ar;
		rtc->a4 = a4;
		rtc->daydate = dt_al*10 + dydt_ar;
		rtc->dydt = dydt_al;
	
	#if 0	
		printf("the time of seconds is:%d\n", rtc->ss);
		printf("the time of minutes is:%d\n", rtc->mm);
		printf("the time of hours is:%d\n", rtc->hh);
		printf("the time of days is:%d\n", rtc->day);
		printf("the time of date is:%d\n", rtc->date);
		printf("the time of month is:%d\n", rtc->month);
		printf("the time of year is:%d\n", rtc->year);
		printf("the time of century is:%d\n", rtc->century);
		printf("the time of alarm 1 is:%d\n", rtc->a1);
		printf("the time of alarm second is:%d\n", rtc->ss_a);
		printf("the time of alarm 2 is:%d\n", rtc->a2);
		printf("the time of alarm minute is:%d\n", rtc->mm_a);
		printf("the time of alarm 3 is:%d\n", rtc->a3);
		printf("the time of alarm hours is:%d\n", rtc->hh_a);
		printf("the time of alarm 4 is:%d\n", rtc->a4);
		printf("the time of alarm daydate is:%d\n", rtc->daydate);
		printf("the time of alarm dydt 1 bit is:%d\n", rtc->dydt);
	#endif
	}
		
	return 0;
}


/*========================================================================
* Function:	rtc_write
* 
* Description: 	Routine to set the rtc 	
* 
* In Parms:	the register_map.h structure of serialRtc_t
*			hh: hour
*			mm: minute
*			ss: second
*			dd: day
*			dt: date
*			mn: month
*			yr: year
*			ct: century
*			
*			te: transfer enable  
*			(disable first then set the value , when finished setting then enable it.)
* Out Parms:    none
*
* Returns: 	OK if success, ERROR otherwise
*/

int rtc_write(int hh, int mm, int ss, int dd, int dt, int mn, int yr, int ct)
{
	int te;
	
	char *control_b;
	
	*RTC_ADDR_CONTROL_B &= ~0x80;
	*control_b = *RTC_ADDR_CONTROL_B;
	te = (int)((*control_b)>>7)&0x01;
	printf("before u wanna set the time, the te value is:%d\n", te);
	
	if (hh < 0 && hh > 23 && mm < 0 && mm > 60 && ss < 0 && ss > 60){
		printf("over the setup range\n");
	}	
	else{
		
		*RTC_ADDR_SEC = 0x00;
		*RTC_ADDR_SEC |=(ss / 10) << 4;
		*RTC_ADDR_SEC |= ss % 10;
		
		*RTC_ADDR_MIN = 0x00;
		*RTC_ADDR_MIN |= (mm / 10) << 4;
		*RTC_ADDR_MIN |= mm % 10;

		*RTC_ADDR_HR = 0x00;
		*RTC_ADDR_HR |= (hh / 10) << 4;
		*RTC_ADDR_HR |= hh % 10;	
		
		*RTC_ADDR_DAY = 0x00;
		*RTC_ADDR_DAY |= dd % 7;
		
		*RTC_ADDR_DATE = 0x00;
		*RTC_ADDR_DATE |= (dt / 10) << 4;	
		*RTC_ADDR_DATE |= dt % 10;
		
		*RTC_ADDR_MONTH = 0x00;
		*RTC_ADDR_MONTH |= (mn / 10) << 4;
		*RTC_ADDR_MONTH |= mn % 10;

		*RTC_ADDR_YEAR = 0x00;
		*RTC_ADDR_YEAR |= (yr / 10) << 4;
		*RTC_ADDR_YEAR |= yr % 10;

		*RTC_ADDR_CENTURY = 0x00;
		*RTC_ADDR_CENTURY |= (ct / 10) << 4;
		*RTC_ADDR_CENTURY |= ct % 10;			
	}
	*RTC_ADDR_CONTROL_B |= 0x80;
	*control_b = *RTC_ADDR_CONTROL_B;
	te = (int)((*control_b)>>7)&0x01;
	printf("after u set the time, the te value is:%d\n", te);
		
	return 0;
}

/*========================================================================
* Function:	rtc_write_date
* 
* Description: 	Routine to set the rtc 	
* 
* In Parms:	the register_map.h structure of serialRtc_t
*			hh: hour
*			mm: minute
*			ss: second
*			dd: day
*			dt: date
*			mn: month
*			yr: year
*			ct: century
*			
*			te: transfer enable  
*			(disable first then set the value , when finished setting then enable it.)
* Out Parms:    none
*
* Returns: 	OK if success, ERROR otherwise
*/

int rtc_write_date(int dt, int mn, int yr, int ct)
{
	int te;
	
	char *control_b;
	
	*RTC_ADDR_CONTROL_B &= ~0x80;
	*control_b = *RTC_ADDR_CONTROL_B;
	te = (int)((*control_b)>>7)&0x01;
	printf("before u wanna set the time, the te value is:%d\n", te);
			
		*RTC_ADDR_DATE = 0x00;
		*RTC_ADDR_DATE |= (dt / 10) << 4;	
		*RTC_ADDR_DATE |= dt % 10;
		
		*RTC_ADDR_MONTH = 0x00;
		*RTC_ADDR_MONTH |= (mn / 10) << 4;
		*RTC_ADDR_MONTH |= mn % 10;

		*RTC_ADDR_YEAR = 0x00;
		*RTC_ADDR_YEAR |= (yr / 10) << 4;
		*RTC_ADDR_YEAR |= yr % 10;

		*RTC_ADDR_CENTURY = 0x00;
		*RTC_ADDR_CENTURY |= (ct / 10) << 4;
		*RTC_ADDR_CENTURY |= ct % 10;			

	*RTC_ADDR_CONTROL_B |= 0x80;
	*control_b = *RTC_ADDR_CONTROL_B;
	te = (int)((*control_b)>>7)&0x01;
	printf("after u set the time, the te value is:%d\n", te);
		
	return 0;
}

/*========================================================================
* Function:	rtc_write_time
* 
* Description: 	Routine to set the rtc 	
* 
* In Parms:	the register_map.h structure of serialRtc_t
*			hh: hour
*			mm: minute
*			ss: second
*			dd: day
*			dt: date
*			mn: month
*			yr: year
*			ct: century
*			
*			te: transfer enable  
*			(disable first then set the value , when finished setting then enable it.)
* Out Parms:    none
*
* Returns: 	OK if success, ERROR otherwise
*/

int rtc_write_time(int hh, int mm, int ss)
{
	int te;
	
	char *control_b;
	
	*RTC_ADDR_CONTROL_B &= ~0x80;
	*control_b = *RTC_ADDR_CONTROL_B;
	te = (int)((*control_b)>>7)&0x01;
	printf("before u wanna set the time, the te value is:%d\n", te);
	
	if (hh < 0 && hh > 23 && mm < 0 && mm > 60 && ss < 0 && ss > 60){
		printf("over the setup range\n");
	}	
	else{
		
		*RTC_ADDR_SEC = 0x00;
		*RTC_ADDR_SEC |=(ss / 10) << 4;
		*RTC_ADDR_SEC |= ss % 10;
		
		*RTC_ADDR_MIN = 0x00;
		*RTC_ADDR_MIN |= (mm / 10) << 4;
		*RTC_ADDR_MIN |= mm % 10;

		*RTC_ADDR_HR = 0x00;
		*RTC_ADDR_HR |= (hh / 10) << 4;
		*RTC_ADDR_HR |= hh % 10;			
	}
	*RTC_ADDR_CONTROL_B |= 0x80;
	*control_b = *RTC_ADDR_CONTROL_B;
	te = (int)((*control_b)>>7)&0x01;
	printf("after u set the time, the te value is:%d\n", te);
		
	return 0;
}

/*========================================================================
* Function:	rtctestsettime
* 
* Description: 	Routine to test the setting time 	
* 
* In Parms:	the register_map.h structure of serialRtc_t
*			hh: hour
*			mm: minute
*			ss: second
*			dd: day
*			dt: date
*			mn: month
*			yr: year
*			ct: century
* Out Parms:    none
*
* Returns: 	OK if success, ERROR otherwise
*/


#ifdef RTC_DEBUG
void rtctestsettime(void)
{

	int hh = 15;
	int mm = 29;
	int ss = 11;
	int dd = 3;
	int dt = 30;
	int mn = 6;
	int yr = 88;
	int ct = 33;
	int choose;
	int getout = 0;
	
	while(getout == 0 ){
		printf("Are u sure to change it y or n?:");
		choose=getchar();
		switch (choose){
			case 'Y':
			case 'y':
				printf("please enter hour [%d]\n", hh);
				scanf("%d",&hh);
				while(hh < 0 || hh > 24){
					printf("the range is 00~23\n");
					printf("please reenter hour\n");
					scanf("%d",&hh);
				}

				printf("please enter minute [%d]\n", mm);
				scanf("%d",&mm);
				while(mm < 0 || mm > 59){
					printf("the range is 00~59\n");
					printf("please reenter minute\n");
					scanf("%d",&mm);
				}	
	
				printf("please enter second [%d]\n", ss);
				scanf("%d",&ss);
				while(ss < 0 || ss > 59){
					printf("the range is 00~59\n");
					printf("please reenter second\n");
					scanf("%d",&ss);
				}
								
				printf("please enter day [%d]\n", dd);
				scanf("%d",&dd);
				while(dd < 1 || dd > 7){
					printf("the range is 1~7\n");
					printf("please reenter day\n");
					scanf("%d",&dd);
				}

				printf("please enter date [%d]\n", dt);
				scanf("%d",&dt);
				while(dt < 1 || dt > 31){
					printf("the range is 1~31\n");
					printf("please reenter date\n");
					scanf("%d",&dt);
				}

				printf("please enter month [%d]\n", mn);
				scanf("%d",&mn);
				while(mn < 1 || mn > 12){
					printf("the range is 1~12\n");
					printf("please reenter month\n");
					scanf("%d",&mn);
				}			
				
				printf("please enter year [%d]\n", yr);
				scanf("%d",&yr);
				while(yr < 0 || yr > 99){
					printf("the range is 0~99\n");
					printf("please reenter year\n");
					scanf("%d",&yr);
				}
				
				printf("please enter century [%d]\n", ct);
				scanf("%d",&ct);
				while(ct < 0 || ct > 39){
					printf("the range is 0~39\n");
					printf("please reenter month\n");
					scanf("%d",&ct);
				}				

		 		printf("the clock will be set to\n hour: %d min: %d sec: %d\nday: %d date: %d month: %d\nyear: %d century: %d\n"
				, hh, mm, ss, dd, dt, mn, yr, ct);
				
				rtc_write(hh, mm, ss, dd, dt, mn, yr, ct);
				getout = 1;				
				
				break;
			case 'N':
			case 'n':
				printf("i don't want to change\n");
				getout = 1;				
				break;
			default:
							
			break;
		}

	}
		
}

void rtctestshowtime(void)
{		
	serialRtc_t myrtc;
	if ( rtc_read(&myrtc) !=0 ){
		printf("RTC not working\n");
		
		}else{
		
				printf("century:%d  %d/%d/%d  day:%d \n time:%d:%d:%d", myrtc.century, myrtc.year, myrtc.month, myrtc.date, myrtc.day, myrtc.hh, myrtc.mm, myrtc.ss);
				printf("the time of alarm 1 is:%d\n", myrtc.a1);
				printf("the time of alarm second is:%d\n", myrtc.ss_a);
				printf("the time of alarm 2 is:%d\n", myrtc.a2);
				printf("the time of alarm minute is:%d\n", myrtc.mm_a);
				printf("the time of alarm 3 is:%d\n", myrtc.a3);
				printf("the time of alarm hours is:%d\n", myrtc.hh_a);
				printf("the time of alarm 4 is:%d\n", myrtc.a4);
				printf("the time of alarm daydate is:%d\n", myrtc.daydate);
				printf("the time of alarm dydt 1 bit is:%d\n", myrtc.dydt);
				
		}
	
	
}

void nvtestread(void)
{		
	int i; 
	char *srcAddr;
	srcAddr=malloc(1025);
	*(srcAddr+1024)=0;
	i=nvram_read(0, srcAddr, 1024);	
	*(srcAddr+i)=0;
	printf("\nread %d bytes\n%s\n", i, srcAddr);
	free(srcAddr);
	
	
}

void nvtestwrite(int toWrite)
{		
 	int i; 
 	char *srcAddr;
 	srcAddr=malloc(1025);
	*(srcAddr+1024)=0;
	memset(srcAddr, toWrite, 1024);
 	i=nvram_write(0, srcAddr, 1024);
 	*(srcAddr+i)=0;
	printf("\nwrite %d bytes\n%s\n", i, srcAddr);	
	free(srcAddr);
}

#endif
