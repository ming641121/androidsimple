/* $Id: csm_test.c,v 1.4 2006/08/03 02:56:56 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_test.c,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of CSM Data Base
 *    Management for redundancy
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Log: csm_test.c,v $
 * Revision 1.4  2006/08/03 02:56:56  ffang
 * add unit test routines for dbm and events
 *
 * Revision 1.3  2006/07/14 04:28:16  ffang
 * sync recheck-in
 *
 * Revision 1.2  2006/06/15 03:54:40  ffang
 * Check-in after integrated with siw/sys modules
 *
 * Revision 1.1  2006/05/16 04:09:49  ffang
 * *** empty log message ***
 *
 *====================================================================
 * $Endlog $
 */

#include <stdio.h>
#include <string.h>
#include <taskLib.h>
#include <intLib.h>
#include <objLib.h>
#include <stdio.h>
#include <string.h>
#include "sys_os.h"
#include "csm_if.h"

extern void CsmTask (void);
extern SIW_MQID_t    msgQId;

void
CsmSnmpTest()
{
#if 0
    mmsclock_t clockData;

    memset((char *) &clockData, 0, sizeof(mmsclock_t));

    if (clkType == D_primClkSrcType_externalClk)
        clockData.clkSrcCardType = D_clkSrcCardType_bits;

    if (masSel == CSM_SEL_CLK_PRI)
    {
        SET_VALID(I_primClkSrcType, clockData.valid);
        SET_VALID(I_primClkSrcSlot, clockData.valid);
        SET_VALID(I_primClkSrcLine, clockData.valid);

        clockData.primClkSrcType = clkType;
        clockData.primClkSrcSlot = slot;
        clockData.primClkSrcLine = port;
    }
    else if (masSel == CSM_SEL_CLK_SEC)
    {
        SET_VALID(I_secClkSrcType, clockData.valid);
        SET_VALID(I_secClkSrcSlot, clockData.valid);
        SET_VALID(I_secClkSrcLine, clockData.valid);

        clockData.secClkSrcType = clkType;
        clockData.secClkSrcSlot = slot;
        clockData.secClkSrcLine = port;
    }

    printf("ClockSnmpAction ret=%d\n", ClockSnmpAction(&clockData, type));
#endif /* 0 */
}

int yourTaskEntry(int arg1, int arg2, int arg3, int arg4, int arg5,
                  int arg6, int arg7, int arg8, int arg9, int arg10)
{
	while(1) {
		printf(".");
		taskDelay(1000);
	}
	return 0;

} 


void
CsmTestTask()
{
#if 0
/* defines */
#define MAX_MESSAGES 100
#define MAX_MESSAGE_LENGTH 50

/* create message queue */
if ((msgQId = msgQCreate(MAX_MESSAGES,MAX_MESSAGE_LENGTH,MSG_Q_FIFO))
	== NULL)
	printf("msgQCreate in failed\n");
#endif

taskSpawn("csmTestTask", 10, 0, 100000, (FUNCPTR)CsmTask,
          0,0,0,0,0,0,0,0,0,0);
} 

#if 0
STATUS_t
TrapEventLog(U32_t trapNum,
             U32_t *mibEntryPtr,
             SIW_EVENT_OBJ_t *event,
             ...)
{
	return 0;
}
#endif

void
csmtestsnmp(SR_INT32 manualSelect, SR_INT32 operMode, SR_INT32 setSlip, 
SR_INT32 inputValue)
{
	stsClockInput_t inp;
	char flag = 0;

	memset((char *) &inp, 0, sizeof(stsClockInput_t));
	if (manualSelect) {
		inp.stsClockInputManualSelect = manualSelect;
	   	SET_VALID(I_stsClockInputManualSelect, inp.valid);
	   	flag = 1;	
	}

	if (operMode) {
		inp.stsClockInputOperMode = operMode;
	   	SET_VALID(I_stsClockInputOperMode, inp.valid);	
	   	flag = 1;	
	}

	if (setSlip) {
		inp.stsClockInputSetSlip = setSlip;
	   	SET_VALID(I_stsClockInputSetSlip, inp.valid);	
	   	flag = 1;	
	}

	if (inputValue) {
		inp.stsClockInputValue = inputValue;
	   	SET_VALID(I_stsClockInputValue, inp.valid);	
	   	flag = 1;	
	}

	if (flag) {
		CsmClockInputSetReq(&inp);
	}
}
