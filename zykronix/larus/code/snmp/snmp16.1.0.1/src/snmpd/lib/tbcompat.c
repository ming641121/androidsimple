/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include <sys/types.h>

#include <stdlib.h>

#include "sr_type.h"
#include "sr_time.h"	/* all time related .h's pulled in here */




#include <sys/socket.h>




#include "sr_snmp.h"
#include "tmq.h"


int
CanSetTimeout()
{
    return TmCanSetTimeout(GetTimerQueue());
}

void
DontNeedTimeout()
/* call this if you called CanSetTimeout but decided you don't need to
   set the timeout afterall */
{
    TmDontNeedTimeout(GetTimerQueue());
}

int
SetPeriodicTimeout(when, period, info)
    SR_UINT32   when;
    SR_UINT32   period;
    void           *info;
{
    return TmSetPeriodicTimeout(GetTimerQueue(), when, period, info);
}

int
SetOneshotTimeout(when, info)
    SR_UINT32       when;
    void           *info;
{
    return TmSetOneshotTimeout(GetTimerQueue(), when, info);
}

int
CancelTimeout(id)
     int             id;
{
    return TmCancelTimeout(GetTimerQueue(), id);
}

int
DeferTimeout(id, when, absolute)
    int             id;
    SR_UINT32       when;
    int             absolute;
{
    return TmDeferTimeout(GetTimerQueue(), id, when, absolute);
}

SR_UINT32
GetTimeoutInfo(id, when, period, info, fired)
    SR_UINT32 id;
    SR_UINT32 *when;
    SR_UINT32 *period;
    void      **info;
    char      *fired;
{
    return TmGetTimeoutInfo(GetTimerQueue(), id, when, period, info, fired);
}

void
AdjustTimeouts(delta)
    SR_INT32 delta;
{
    TmAdjustTimeouts(GetTimerQueue(), delta);
}

SR_UINT32
TimerCurTime()
{
    SR_UINT32 n;
    n = GetTimeNow();
    return n;
}
