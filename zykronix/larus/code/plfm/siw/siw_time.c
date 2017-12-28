/*
 *Copyright(C) 2005, All Rights Reserved, by
 *ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_time.c
 *
 *System Interface Wrapper code for Tasks
 *
 *Note:
 *
 *Author(s):
 * 
 */

#include <vxWorks.h>
#include <stdio.h>
#include <time.h>
#include <tickLib.h>

#include <axss_types.h>
#include <sys_if.h>

U64_t SysHrTimeGet()
{
    return (1);
}

/************************************************************************
 *  Function:    SiwDateTimeGet()
 *  Description: This function gets the current calendar time from
 *               system timer and convert it to the broken-down time.
 *
 *  In Parms:    None
 *  Out Parms:   struct tm *ctime - where to store the broken-down
 *                                  time
 *  Returns:     OK or ERROR
 ************************************************************************/
STATUS_t 
SiwDateTimeGet(struct tm *ctime)
{
    time_t currtime;

    /* set the time and date */
    currtime = time ((time_t *)NULL);
    localtime_r (&currtime, ctime);
    
    return (OK);
}

/************************************************************************
 *  Function:    SiwTimeInSecondGet()
 *  Description: This function gets the current calendar time from
 *               system timer in calendar seconds.
 *
 *  In Parms:    None
 *  Out Parms:   struct time_t *ctime - where to store the calendar time 
 *                                      in seconds
 *  Returns:     OK or ERROR
 ************************************************************************/
STATUS_t
SiwTimeInSecondGet(time_t *ctime)
{

    /* set the time and date */
    time ((time_t *) ctime);
    return (OK);
}

/************************************************************************
 *  Function:    SiwDateTimeSet()
 *  Description: This function sets a new date and time to the system
 *               timer, and also update it to the realtime clock(RTC).
 *
 *  In Parms:    struct tm *newTime - buffer for the broken-down time
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ************************************************************************/
STATUS_t 
SiwDateTimeSet(IN struct tm *newTime)
{
    struct timespec secTime;

    /* set the system clock to the current time */
    secTime.tv_sec = mktime (newTime);
    secTime.tv_nsec = 0;

    return (clock_settime (CLOCK_REALTIME, &secTime));
}

/************************************************************************
 *  Function:    SiwTickGet()
 *  Description: This function gets the value of the system tick
 *               counter.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     the value of the ticks
 ************************************************************************/
U32_t 
SiwTickGet(void)
{
#if 0
    return (sysClkTick);
#else
    return ((U32_t)tickGet());
#endif
}

/************************************************************************
 *  Function:    SiwTickSet()
 *  Description: This function sets the internal tick counter to the
 *               specified value in newTicks.
 *
 *  In Parms:    U32_t newTicks - new time in ticks
 *  Out Parms:   None
 *  Returns:     OK or ERROR
 ************************************************************************/
STATUS_t 
SiwTickSet(IN U32_t newTicks)
{
#if 0
    sysClkTick = newTicks;
#else
    tickSet((ULONG)newTicks);
#endif

    return (OK);
}



