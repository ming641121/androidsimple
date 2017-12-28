
 /*====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file defines the RTC basic interface and struct
 *
 * Note:
 *
 * Author: Klim and shuming
 *
 *====================================================================
 * $Id: rtc_if.h,v 1.1 2006/07/26 07:17:54 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/rtc_if.h,v $
 * $Log: rtc_if.h,v $
 * Revision 1.1  2006/07/26 07:17:54  cvsshuming
 * init.
 * rtc_if.h is the interface to rtc
 *
 * *** empty log message ***
 *
 *====================================================================
 * $Endlog $
 */

#ifndef _RTC_IF_H_
#define _RTC_IF_H_



#include "tss_types.h"

/* The struct of RTC register map */
typedef struct {
    short hh;
    short mm;
    short ss;
    short day;
    short date;
    short month;
    short year;
    short century;
    short hh_a;
    short mm_a;
    short ss_a;
    short day_a;
    short date_a;
    short a1;
    short a2;
    short a3;
    short a4;
    short daydate;
    short dydt;
    int xram_add;
    int xram_data;
} serialRtc_t;


/*
 *====================================================================
 *=                        Extern's                                  =
 *====================================================================
 */

extern STATUS_t 		SysRtcRead(int rtcEntry, int offset, char *dataBuf, int nBytes);
extern STATUS_t 		SysRtcWrite(int rtcEntry, int offset, char *dataBuf, int nBytes);
extern int 					nvram_read(int nvOffset, char *srcAddr, int len);
extern int 					nvram_write(int nvOffset, char *srcAddr, int len);

extern int rtc_read(serialRtc_t *rtc);
extern int rtc_write(int hh, int mm, int ss, int dd, int dt, int mn, int yr, int ct);


#endif /* _RTC_IF_H_ */