/*
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Network. San Jose, CA
 *
 *Abstract:
 *
 *File Name: sys_.c
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
#include <sntpsLib.h>

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


#define M8260_SIEXR_EDI5      0x00000400
#define M8260_SIPNR_H_IRQ5    0x00000400



void sys1ppsHandler();
STATUS_t sys1secHandler();

ULONG ppsTimeStamp = 0;

struct timespec    gpsTime;
struct timespec    sntpTime;

int gpsUpdate = 0;
extern int sntpsDbg;

STATUS_t sysTimeStampInit()
{
    sysTimestampConnect((FUNCPTR)sys1secHandler, 0);
	sysTimestampEnable();

	return OK;
}

STATUS_t sys1secHandler()
{
    /* logMsg("In sys1secHandler\r\n", 0,0,0,0,0,0); */
    return OK;
}

/* IRQ5, Rising Edge interrupt */
STATUS_t sys1PPsEnable()
{
    UINT32 immrVal = vxImmrGet();

    intConnect (INUM_TO_IVEC(INUM_IRQ5),sys1ppsHandler,(int)0);
				    
    /* Set IRQ5 to Edge interrupt */
    * M8260_SIEXR(immrVal) |= M8260_SIEXR_EDI5;
	 
    /* Clear the pending interrupt */
    * M8260_SIPNR_H(immrVal) |= M8260_SIPNR_H_IRQ5;

    /* Enable the Mask */
    intEnable (INUM_IRQ5);
	return OK;
}

STATUS_t sys1PPsDisable()
{
	intDisable (INUM_IRQ5);
	return OK;
}

void sys1ppsHandler()
{
    UINT32 immrVal = vxImmrGet();

    /* Clear the pending interrupt */
    * M8260_SIPNR_H(immrVal) |= M8260_SIPNR_H_IRQ5;
    ppsTimeStamp = sysTimestamp();
	sntpTime.tv_sec ++;

	if(gpsUpdate)
	{
	     sntpTime.tv_sec  = gpsTime.tv_sec;
		 gpsUpdate = 0;
	}
	if(sntpsDbg)
        logMsg("sys1ppsHandler: sntp 0x%x, gps 0x%x, ppsTimeStamp 0x%x\r\n", sntpTime.tv_sec,gpsTime.tv_sec,ppsTimeStamp,0,0,0);
}

