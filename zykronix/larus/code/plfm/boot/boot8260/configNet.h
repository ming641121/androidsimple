/* configNet.h - network configuration header */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,28jan04,dtr  Modify FCC defines.
01a,18dec03,dtr   Created from ads826x/configNet.h.  
*/
 
#ifndef INCconfigNeth
#define INCconfigNeth

#include "vxWorks.h"
#include "config.h"
#include "end.h"
#include "miiLib.h"

/* defines */
 
#ifdef INCLUDE_MOTFCCEND

/* Motorola Fast Communication Controller */

#define MOT_FCC_LOAD_FUNC	sysMotFccEndLoad

/* 
 * the default initialization string for the FCC is built in sysMotFccEnd.c
 */

#define MOT_FCC_LOAD_STRING ""

IMPORT END_OBJ* MOT_FCC_LOAD_FUNC (char *, void*);
#endif /* INCLUDE_MOTFCCEND */

#ifdef INCLUDE_FEI_END
#define FEI_LOAD_FUNC   sysFei82557EndLoad
IMPORT END_OBJ* FEI_LOAD_FUNC (char *, void*);
#define FEI_LOAD_STRING ""
#endif /* INCLUDE_FEI_END */

/* max number of SENS ipAttachments we can have */

#ifndef IP_MAX_UNITS
#   define IP_MAX_UNITS (NELEMENTS (endDevTbl) - 1)
#endif

END_TBL_ENTRY endDevTbl [] =
{
#ifdef INCLUDE_MOTFCCEND
#ifdef INCLUDE_FCC1
    { 0, MOT_FCC_LOAD_FUNC, MOT_FCC_LOAD_STRING, 1, NULL, FALSE},
#endif /*INCLUDE_FCC1 */
#ifdef INCLUDE_FCC2
    { 1, MOT_FCC_LOAD_FUNC, MOT_FCC_LOAD_STRING, 1, NULL, FALSE},
#endif /* INCLUDE_FCC2 */
#endif /* INCLUDE_MOTFCCEND */
#ifdef INCLUDE_FEI_END
    { 0, FEI_LOAD_FUNC, FEI_LOAD_STRING, 1, NULL, FALSE},
#endif /*INCLUDE_FEI_END */
    { 0, END_TBL_END, NULL, 0, NULL, FALSE},
};
#endif /* INCconfigNeth */
