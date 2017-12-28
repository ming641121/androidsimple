/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya System, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file is a header file for SNMP Utilities that are used
 *    by the access functions as well as some action functions. 
 *
 * Author(s):
 *
 * 
 */

#ifndef _SNMP_UTIL_H
#define _SNMP_UTIL_H

#include "axss_types.h"
#include "axss_if.h"
#include "sys_if.h"
#include "siw_if.h"
#include "snmptype.h"


#define NUM_OF_BYTES(n)     ((n-1)/8 + 1)

typedef struct _TrapName
{
    U32_t trapNum;
    char  trapName[100];
} TrapName_t;

/*
 * Definitions of Key SNMP Limitation Constants
 */
#define MAX_TRAP_MGRS        10
#define MAX_TRAP_COMSTR_LEN  20
#define MIN_COMMSTR_ENTRY    1
#define MAX_COMMSTR_ENTRY    15
#define MAX_COMMSTR_LEN      MAX_TRAP_COMSTR_LEN
#define MAX_MSM_NUM          2
#define MIN_MSM_NUM          1
#define MAX_DSP_NUM          8
#define MIN_DSP_NUM          1

/*
 * Define the wildcard index for commStrTable. With this index
 * users can use commStrCommString as the key for various actions. This
 * table can be only operated upon wildcard index. Regular index (if given)
 * will be rejected.
 *
 * The combination of commStrCommString and commStrMgrIpAddr uniquely 
 * identifies a table entry.
 *
 * This table, designed only for WebEm and CLI, cannot be accessed 
 * through SNMP.
 */
#define COMSTR_WILDCARD_INDEX 100



/*
 *====================================================================
 *=                             Externs                              =
 *====================================================================
 */
           
#endif /* _SNMP_UTIL_H */

