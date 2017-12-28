/*
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Network. San Jose, CA
 *
 *Abstract:
 *
 *File Name: sys_utils.c
 *
 *Miscellaneous System Utility routines.
 *
 *Note:
 *
 *Author(s):
 *
 */

#include "vxWorks.h"
#include <tickLib.h>
#include <stdio.h>
#include <sysLib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <intLib.h>
#include <msgQLib.h>
#include <time.h>
#include <lstLib.h>
#include <usrLib.h>

#include <axss_types.h>
#include <axss_if.h>
#include <siw_if.h>
#include <siw_events.h>
#include <siw_trace.h>
#include <sys_memmap.h>
#include <sys_if.h>
#include <dosFsLib.h>
#include <ifLib.h>
#include <logLib.h>

#include "ioLib.h"
#include "sys_events.h"
#ifndef BOOT
#include "sys_os.h"
#endif
#include "rtc_if.h"
#include "register_map.h"
#include "eeprom_if.h"
#include "config.h"
#include "configAll.h"
#include "smc8260Sio.h"
#include "drv\sio\m8260Sio.h"

#define ONE_MS          (CPU_CLOCK_RATE/1000)

void SysTimeShow(void);
void SysClkCheck(void);

extern unsigned char sysFccEnetAddr [2][6];

extern void     SysPrintIntCount(void);
extern STATUS_t MemTest(U8_t *memStart, U32_t memSize);

#ifndef BOOT
static U32_t SysTaskWaitCycle[2] = {0, 0}; /* timestamps */

SIW_SEMID_t	SysWaitSemIdGet(void);

/***********************************************************************
 *  Function:      SysWaitSemIdGet()
 *  Description:   This routine determines the Semaphore ID that should
 *                 be taken to achieve 2ms delay.
 *
 *  In Parms:      None
 *  Out Parms:     None
 *  Returns:       Semaphore ID that should be taken to achieve 2ms delay.
 ***********************************************************************/
SIW_SEMID_t
SysWaitSemIdGet()
{
#define MAX_CYCLE       ((U32_t)~0)             /* 0xffffffff */

    SIW_SEMID_t earlySemId, lateSemId;
    U32_t lateCycle, nowCycle, deltaCycle;

    /* figure out which of two semaphores ticked later (rollover possible) */
    if ((SysTaskWaitCycle[0] < SysTaskWaitCycle[1]) &&
        ((SysTaskWaitCycle[1] - SysTaskWaitCycle[0]) < MAX_CYCLE / 2))
    {
        earlySemId = SEM_TASKDELAY0;
        lateSemId = SEM_TASKDELAY1;
        lateCycle = SysTaskWaitCycle[1];
    }
    else
    {
        earlySemId = SEM_TASKDELAY1;
        lateSemId = SEM_TASKDELAY0;
        lateCycle = SysTaskWaitCycle[0];
    }

    /* calculate the time difference between the latest tick and now */
    nowCycle = CYCLES();        /* timestamp */
    if (lateCycle > nowCycle)
    {
        deltaCycle = MAX_CYCLE - (lateCycle - nowCycle);    /* rollover */
    }
    else
    {
        deltaCycle = nowCycle - lateCycle;
    }

    /* based on the time elapsed since the latest tick, choose semaphore */
    if (deltaCycle > ONE_MS)
    {
        return (lateSemId);
    }
    else
    {
        return (earlySemId);
    }
}
#endif

/************************************************************************
 *  Function:     SysGetDramSize()
 *  Description:  This function gets SDRAM size through I2C.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      SDRAM size in bytes.
 ************************************************************************/
U32_t SysGetDramSize ()
{
	return (0x4000000);
}

/***************************************************************************
 *  Function:     CopyString()
 *  Description:  This function copies character string from source to
 *                destination data buffer.
 *
 *  In Parms:     char *desStr - pointer to the destination data buffer
 *                const char *srcStr - pointer to the source data buffer
 *                int desLen - size of the destination data buffer
 *                int srcLen - length of bytes in source data buffer
 *  Out Parms:    None
 *  Returns:      None
 ********************************************************************/
void
CopyString(char *        desStr,
           const char *  srcStr,
           int           desLen,
           int           srcLen,
           char          filler)
{
    int i;

    /* copy string char from source to destination buffer */
    for (i=0; i<srcLen && i<desLen; i++)
    {
        desStr[i] = srcStr[i];
    }

    /* patch space char to the remain space in destination buffer */
    for (i=srcLen; i<desLen; i++)
    {
        desStr[i] = filler;
    }
}

/****************************************************************************
 *  Function:     ClrScrn()
 *  Description:  This function cleans up screen for displaying.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 *****************************************************************************/
void
ClrScrn()
{
    static char *ClrScreen = "\015\015\033\[?25h\033[0m\033[2J\214";

    /* print the esc sequence of screen cleanning */
    printf("%s", ClrScreen);
}


/*******************************************************************************
 *  Function:     CheckScroll()
 *  Description:  This function checks whether the screen needs to be
 *                scrolled ior not.
 *
 *  In Parms:     int limit - the limit line displaied before scroll screen
 *  Out Parms:    int *pLineNum - where to store the number of line has
 *                                been displaied
 *  returns:      QUIT or NEXT_PAGE
 ********************************************************************************/
int
CheckScroll( int *pLineNum, int limit )
{
    char buf[200], *ptr;
    int  len;

    if( (*pLineNum)++ >= limit ) /* make sure next line will be printed */
    {
        /* ioctl( STD_IN, FIOFLUSH, 0 ); */  /* flush the buffer */

        /* if one screen is not enough */
        do {
            printf( "\nType <CR> to continue, Q<CR> to stop: " );
        } while ((len = read(STD_IN, buf, sizeof(buf))) == 0);

        buf[len] = 0;
        ptr = &buf[0];
        while( *ptr == ' ')
        {
            ptr++;
        }

        /* check if quit */
        if( (*ptr ==  'q') || (*ptr == 'Q') )
        {
            return (QUIT);
        }
 
        /* clean up the screen before displaying */
        /* ClrScrn();*/

        *pLineNum = 0;
    }

    return (NEXT_PAGE);
}

/****************************************************************
 *  Function:    SysTimeShow()
 *  Description: This function display date/time stored in the
 *               vxWorks System.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     None
 ****************************************************************/
void SysTimeShow()
{
    struct tm brknTime;
    time_t currtime;
    char datestamp[DATESTAMP_SIZE+1];
    char timestamp[TIMESTAMP_SIZE+1];

    /* read time from vxWorks */
    currtime = time ((time_t *)NULL);
    localtime_r (&currtime, &brknTime);

    memset(datestamp, 0, sizeof(datestamp));
    memset(timestamp, 0, sizeof(timestamp));
    strftime(datestamp, DATESTAMP_SIZE, "%m/%d/%Y", &brknTime);
    strftime(timestamp, TIMESTAMP_SIZE, "%H:%M:%S", &brknTime);
    printf("%s - %s\n", datestamp, timestamp);

#if 0
    printf("current VxWorks time: %d/%d/%d/%d/%d/%d\n\n",
                    brknTime.tm_year, brknTime.tm_mon+1,
                    brknTime.tm_mday, brknTime.tm_hour,
                    brknTime.tm_min, brknTime.tm_sec);
#endif
}

void SysClkCheck()
{
    printf("Starting the 10 seconds clock check\n");
    taskDelay (SECONDS(10));    
    printf("Completed\n");
}

/* AHOYA - Added stub func */

/**************************************************************
* FUNCTION:  SysClkTickGet()                                  *
*            get the system clock tick                        *
*                                                             *
* INPUT:     None                                             *
* RETURNS:   system clock tick                                *
***************************************************************/
U32_t SysClkTickGet()
{
    return ((U32_t)tickGet());
}

/****************************************************************************
 * Function:    ErrHandler ()
 * Descripion:  This routine returns the card state for a perpective card 
 *              
 * In Parms:    slot number 
 *              S32_t eType - Error Type (from mms_if.h) 
 *              S32_t eId   - Event Id (from mms_events.h) 
 *              MSG_t *msg  - POinter to SAR buffer allocated by application
 * Out Parms:   none
 * Return Values:
 *               OK (0) ot ERROR (-1)
 */
RET_t 
ErrHandler (S32_t slot, S32_t eType, S32_t eId, MSG_t *msg)
{
	return 0;
}

/****************************************************************************
 * Function:    TrapRegTblGet(U32_t trapNum, void  **regTbl)
 * Descripion:  get the address of the tran reg table
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */

STATUS_t
sysClkInit()
{
    struct tm       time;
    serialRtc_t     rtc;
    struct timespec secTime;

    rtc_read(&rtc);

    time.tm_sec   = rtc.ss;
    time.tm_min   = rtc.mm;
    time.tm_hour  = rtc.hh;
    time.tm_mday  = rtc.date;
    time.tm_mon   = rtc.month;
    time.tm_year  = rtc.century * 100 + rtc.year - 1900;
    time.tm_wday  = rtc.day;
    time.tm_yday  = -1;
    time.tm_isdst = -1;

    time.tm_mon --;
    secTime.tv_sec = mktime(&time);
    secTime.tv_nsec = 0;
    clock_settime(CLOCK_REALTIME, &secTime);

	return OK;
}

char systemTime[20];

STATUS_t
sysClkSetTime(int hh, int mm, int ss, int dd, int dt, int mn, int yr, int ct)
{
    struct tm       time;
    struct timespec secTime;

    time.tm_sec   = ss;
    time.tm_min   = mm;
    time.tm_hour  = hh;
    time.tm_mday  = dt;
    time.tm_mon   = mn;
    time.tm_year  = ct * 100 + yr - 1900;
    time.tm_wday  = dd;
    time.tm_yday  = -1;
    time.tm_isdst = -1;
    
    rtc_write(hh, mm, ss, dd, dt, mn, yr, ct);

    time.tm_mon --;
    secTime.tv_sec = mktime(&time);
    secTime.tv_nsec = 0;
    clock_settime(CLOCK_REALTIME, &secTime);

	return OK;
}

STATUS_t
sysClkTimeShow()
{
    struct tm     time;
    serialRtc_t   rtc;

    rtc_read(&rtc);

    time.tm_sec   = rtc.ss;
    time.tm_min   = rtc.mm;
    time.tm_hour  = rtc.hh;
    time.tm_mday  = rtc.date;
    time.tm_mon   = rtc.month;
    time.tm_year  = rtc.century * 100 + rtc.year;
    time.tm_wday  = rtc.day;
    time.tm_yday  = -1;
    time.tm_isdst = -1;

    printf("current RTC time: %d/%d/%d/%d/%d/%d\n\n",
                    time.tm_year, time.tm_mon,
                    time.tm_mday, time.tm_hour,
                    time.tm_min,  time.tm_sec);
	return OK;
}

STATUS_t
sysTimeShowTest()
{
    struct timespec  tp;
	struct tm        brokenTime;

    clock_gettime(CLOCK_REALTIME, &tp);

    localtime_r(&(tp.tv_sec), 	&brokenTime);

	printf("%d/%d/%d - %d:%d:%d\r\n",  brokenTime.tm_year + 1900,
	                                   brokenTime.tm_mon + 1,
	                                   brokenTime.tm_mday,
	                                   brokenTime.tm_hour,
	                                   brokenTime.tm_min,
	                                   brokenTime.tm_sec);
    return OK;
}
void
SysDateTimeCallback(DOS_DATE_TIME *dosDateTime)
{
    time_t currtime;
    struct tm brknTime;

    /* read time from vxWorks */
    currtime = time ((time_t *)NULL);
    localtime_r (&currtime, &brknTime);

    /* fill in DOS_DATE_TIME structure */
    dosDateTime->dosdt_year     = brknTime.tm_year + 1900;
    dosDateTime->dosdt_month    = brknTime.tm_mon  + 1;
    dosDateTime->dosdt_day      = brknTime.tm_mday;
    dosDateTime->dosdt_hour     = brknTime.tm_hour;
    dosDateTime->dosdt_minute   = brknTime.tm_min;
    dosDateTime->dosdt_second   = brknTime.tm_sec;
}

void sysBootLineInit()
{
    U32_t ipAddr;
    U32_t gateAddr;

    eepromRead(EE_MGMT_IP_ADDR_ADDR, (char *)&ipAddr, EE_MGMT_IP_ADDR_SIZE);
    eepromRead(EE_MGMT_IP_GATEWAY_ADDR, (char *)&gateAddr, EE_MGMT_IP_GATEWAY_SIZE);


    printf("motfcc(0,0) e=%d.%d.%d.%d g=%d.%d.%d.%d",
         (ipAddr   & 0xFF000000) >> 24, (ipAddr   & 0x00FF0000) >> 16,
         (ipAddr   & 0x0000FF00) >>  8, (ipAddr   & 0x000000FF),
         (gateAddr & 0xFF000000) >> 24, (gateAddr & 0x00FF0000) >> 16,
         (gateAddr & 0x0000FF00) >>  8, (gateAddr & 0x000000FF));

    sprintf(BOOT_LINE_ADRS, "motfcc(0,0) e=%d.%d.%d.%d g=%d.%d.%d.%d",
         (ipAddr   & 0xFF000000) >> 24, (ipAddr   & 0x00FF0000) >> 16,
         (ipAddr   & 0x0000FF00) >>  8, (ipAddr   & 0x000000FF),
         (gateAddr & 0xFF000000) >> 24, (gateAddr & 0x00FF0000) >> 16,
         (gateAddr & 0x0000FF00) >>  8, (gateAddr & 0x000000FF));

}


void sysSetBootMask()
{
	U32_t subnetMask;

    eepromRead(EE_MGMT_IP_SUBNET_ADDR, (char *)&subnetMask, EE_MGMT_IP_SUBNET_SIZE);

    ifMaskSet("motfcc0",subnetMask);
}

void sysSetMgmtIpAddr(U32_t userIpAddr)
{
    U32_t ipAddr;

    ipAddr = userIpAddr;

    eepromWrite(EE_MGMT_IP_ADDR_ADDR, (char *)&ipAddr, EE_MGMT_IP_ADDR_SIZE);
}

void sysSetMgmtIpGateway(U32_t userIpGateway)
{
    U32_t ipGateway;

    ipGateway = userIpGateway;

    eepromWrite(EE_MGMT_IP_GATEWAY_ADDR, (char *)&ipGateway, EE_MGMT_IP_GATEWAY_SIZE);
}

void sysSetMgmtIpSubnet(U32_t userIpSubnet)
{
    U32_t ipSubnet;

    ipSubnet = userIpSubnet;

    eepromWrite(EE_MGMT_IP_SUBNET_ADDR, (char *)&ipSubnet, EE_MGMT_IP_SUBNET_SIZE);
}

STATUS sysMgmtMacAddrSet(char mac0, char mac1, char mac2, char mac3, char mac4, char mac5)
{
	sysFccEnetAddr[0][0] = mac0;
	sysFccEnetAddr[0][1] = mac1;
	sysFccEnetAddr[0][2] = mac2;
	sysFccEnetAddr[0][3] = mac3;
	sysFccEnetAddr[0][4] = mac4;
	sysFccEnetAddr[0][5] = mac5;

    eepromWrite(EE_MGMT_IP_MAC_ADDR, &(sysFccEnetAddr[0][0]),  EE_MGMT_IP_MAC_SIZE);

	return OK;
}

STATUS sysMgmtMacAddrGet()
{
    eepromRead(EE_MGMT_IP_MAC_ADDR, &(sysFccEnetAddr[0][0]),  EE_MGMT_IP_MAC_SIZE);

    printf("MGMT MAC ADDR: %x:%x:%x:%x:%x:%x\r\n", 	    sysFccEnetAddr[0][0],
														sysFccEnetAddr[0][1],
														sysFccEnetAddr[0][2],
														sysFccEnetAddr[0][3],
														sysFccEnetAddr[0][4],
														sysFccEnetAddr[0][5]);

	return OK;
}

STATUS initSysMgmtMacAddr()
{
    eepromRead(EE_MGMT_IP_MAC_ADDR, &(sysFccEnetAddr[0][0]),  EE_MGMT_IP_MAC_SIZE);

	return OK;
}
