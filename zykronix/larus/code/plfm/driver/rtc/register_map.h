/* $Id: register_map.h,v 1.3 2006/07/26 07:18:42 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/driver/rtc/register_map.h,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of RTC Data Base
 *    Register Map
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Log: register_map.h,v $
 * Revision 1.3  2006/07/26 07:18:42  cvsshuming
 * 1. rtc_if.h
 * 2. add static for not-exported routines
 *
 * Revision 1.2  2006/06/29 11:57:37  klim
 * *** empty log message ***
 *
 * Revision 1.1  2006/06/26 08:26:30  cvsshuming
 * new module - real time clock
 *
 * Revision 1.0  2006/06/26 15:29:46  klim
 * Initial check-in
 *
 *====================================================================
 * $Endlog $
 */


/*
 *====================================================================
 *=                        define's                                 =
 *====================================================================
 */

/*
 *====================================================================
 *=                        typedef's                                 =
 *====================================================================
 */


#define REGISTER_START                                	(0x42000000)
#define RTC_ADDR_BASE                                 	(REGISTER_START + 0xF00)
#define RTC_ADDR_SEC					(char *)(RTC_ADDR_BASE)
#define RTC_ADDR_MIN					(char *)(RTC_ADDR_SEC + 0x01)
#define RTC_ADDR_HR					(char *)(RTC_ADDR_MIN + 0x01)
#define RTC_ADDR_DAY					(char *)(RTC_ADDR_HR + 0x01)
#define RTC_ADDR_DATE					(char *)(RTC_ADDR_DAY + 0x01)
#define RTC_ADDR_MONTH					(char *)(RTC_ADDR_DATE + 0x01)
#define RTC_ADDR_YEAR					(char *)(RTC_ADDR_MONTH + 0x01)
#define RTC_ADDR_CENTURY				(char *)(RTC_ADDR_YEAR + 0x01)
#define RTC_ADDR_SEC_A					(char *)(RTC_ADDR_CENTURY + 0x01)
#define RTC_ADDR_MIN_A					(char *)(RTC_ADDR_SEC_A + 0x01)
#define RTC_ADDR_HR_A					(char *)(RTC_ADDR_MIN_A + 0x01)
#define RTC_ADDR_DAY_DATE_A				(char *)(RTC_ADDR_HR_A + 0x01)
#define RTC_ADDR_WATCHDOG1				(char *)(RTC_ADDR_DAY_DATE_A + 0x01)
#define RTC_ADDR_WATCHDOG2				(char *)(RTC_ADDR_WATCHDOG1 + 0x01)
#define RTC_ADDR_CONTROL_A				(char *)(RTC_ADDR_WATCHDOG2 + 0x01)
#define RTC_ADDR_CONTROL_B				(char *)(RTC_ADDR_CONTROL_A + 0x01) 
#define RTC_ADDR_ERAM_ADDR				(char *)(RTC_ADDR_CONTROL_B + 0x01)
#define RTC_ADDR_RESERVED_1				(char *)(RTC_ADDR_ERAM_ADDR + 0x01)
#define RTC_ADDR_RESERVED_2				(char *)(RTC_ADDR_RESERVED_1 + 0x01)
#define RTC_ADDR_ERAM_DATA				(char *)(RTC_ADDR_RESERVED_2 + 0x01)
#define RTC_ADDR_RESERVED_3				(char *)(RTC_ADDR_ERAM_DATA + 0x01)





