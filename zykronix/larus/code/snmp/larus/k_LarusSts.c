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
 */


/*
 * Arguments used to create this file:
 * -search_table (implies -row_status) -c LarusSts larusMib -agent -per_file_init 
 * 
 */

#include "sr_conf.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif /* HAVE_MEMORY_H */
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif /* HAVE_STDDEF_H */
#include "sr_snmp.h"
#include "sr_trans.h"
#include "comunity.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "lookup.h"
#include "v2table.h"
#include "min_v.h"
#include "mibout.h"
#include "axss_types.h" /* joehsu */


#define SETS /* joehsu */

int kDebug = FALSE;

extern SnmpV2Table communities; /* joehsu */
extern const SnmpType communitiesType[]; /* joehsu */
extern STATUS_t pmThresholdSet(stsAlarmThresholdEntry_t *threshold);
extern STATUS_t pmThresholdGet(stsAlarmThresholdEntry_t *threshold);
extern STATUS_t pmSeveritySet(stsLinkAlarmEntry_t *severity); 
extern STATUS_t pmSeverityGet(stsLinkAlarmEntry_t *severity); 
extern STATUS_t pmPerfMonGet(stsPerfMonEntry_t *pmRec); 
extern STATUS_t ManSmuTestSet(stsPerfMonManual_t *manTest); 
extern STATUS_t ManSmuTestGet(stsPerfMonManual_t *manTest); 

extern STATUS_t CsmClockInputGetReq (stsClockInput_t *inp);
extern STATUS_t CsmClockInputSetReq (stsClockInput_t *inp);
extern STATUS_t CsmClockInputStatusGetReq (stsClockInputStatusEntry_t *inp);
extern STATUS_t CsmClockPriGetReq (stsClockPriEntry_t *pri);
extern STATUS_t CsmClockPriSetReq (stsClockPriEntry_t *pri);

extern STATUS_t smShelfGet(stsShelfEntry_t * mibstructIn);
extern STATUS_t smShelfSet(stsShelfEntry_t * mibstructIn);

extern STATUS_t smOutputCardGet(stsOutputCardEntry_t * mibstructIn);
extern STATUS_t smOutputCardSet(stsOutputCardEntry_t * mibstructIn);

extern STATUS_t smOutputPortGet(stsOutputPortSettingEntry_t *mibstructIn);
extern STATUS_t smOutputPortSet(stsOutputPortSettingEntry_t *mibstructIn); 
extern STATUS_t inpFramerLinkSet(stsLinkEntry_t * mibstructIn);
extern STATUS_t inpFramerLinkGet(stsLinkEntry_t * mibstructIn);
extern STATUS_t gpsGet(stsGpsEntry_t *input);
extern STATUS_t gpsSet(stsGpsEntry_t *input); 
extern STATUS_t oscGet(stsOscillatorEntry_t *input);
extern STATUS_t oscSet(stsOscillatorEntry_t *input);
extern STATUS_t oscAlarmEntry_set(stsOscillatorAlarmEntry_t *input);
extern STATUS_t oscAlarmEntry_get(stsOscillatorAlarmEntry_t * input);
extern STATUS_t tmpEmmGet(stsEmmStatusEntry_t * emmEntry);
extern STATUS_t tmpEmmSet(stsEmmStatusEntry_t * emmEntry);
extern STATUS_t antGet(stsAntEntry_t *antEntry);
extern STATUS_t antSet(stsAntEntry_t *antEntry);
extern STATUS_t systemActionFuncGet (stsSystem_t *sys);
extern STATUS_t systemActionFuncSet (stsSystem_t *sys);


extern STATUS_t topActionFuncGet (stsTop_t *sys);
extern STATUS_t topActionFuncSet (stsTop_t *sys);

stsTrapRegEntry_t  stsTrapRegData[10];
OctetString communityPtr[10];
char        communityStr[10][30];

/* global data describing the stsSystem family */
stsSystem_t stsSystemData;

/* global data describing the stsClockInput family */
stsClockInput_t stsClockInputData;

/* global data describing the stsClockInputSSM family */
stsClockInputSSM_t stsClockInputSSMData;

/* global data describing the stsTop family */
stsTop_t stsTopData;

/* global data describing the stsPerfMonManual family */
stsPerfMonManual_t stsPerfMonManualData;

/* global data describing the stsCommStr family */
stsCommStr_t stsCommStrData;

/* global data describing the stsClockInputStatusTable entries */
Index_t stsClockInputStatusEntryIndex[] = {
    { offsetof(stsClockInputStatusEntry_t, stsClockInputStatusIndex),  T_uint}
};

SnmpV2Table stsClockInputStatusTable = {
    NULL,
    stsClockInputStatusEntryIndex,
    0,
    1,
    sizeof(stsClockInputStatusEntry_t)
};

const SnmpType stsClockInputStatusEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsClockInputStatusEntry_t, stsClockInputStatusIndex), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsClockInputStatusEntry_t, stsClockInputState), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsClockInputStatusEntry
    SR_PROTOTYPE((int index));
extern void stsClockInputStatusEntry_free
    SR_PROTOTYPE((stsClockInputStatusEntry_t *data));

/* global data describing the stsClockInputSSMTable entries */
Index_t stsClockInputSSMEntryIndex[] = {
    { offsetof(stsClockInputSSMEntry_t, stsClockInputSSMIndex),  T_uint},
    { offsetof(stsClockInputSSMEntry_t, stsClockInputSSMInputSelect),  T_uint}
};

SnmpV2Table stsClockInputSSMTable = {
    NULL,
    stsClockInputSSMEntryIndex,
    0,
    2,
    sizeof(stsClockInputSSMEntry_t)
};

const SnmpType stsClockInputSSMEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsClockInputSSMEntry_t, stsClockInputSSMIndex), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsClockInputSSMEntry_t, stsClockInputSSMInputSelect), 1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsClockInputSSMEntry_t, stsClockInputSSMValue), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsClockInputSSMEntry
    SR_PROTOTYPE((int index));
extern void stsClockInputSSMEntry_free
    SR_PROTOTYPE((stsClockInputSSMEntry_t *data));

/* global data describing the stsCardTable entries */
Index_t stsCardEntryIndex[] = {
    { offsetof(stsCardEntry_t, stsCardShelf),  T_uint},
    { offsetof(stsCardEntry_t, stsCardIndex),  T_uint}
};

SnmpV2Table stsCardTable = {
    NULL,
    stsCardEntryIndex,
    0,
    2,
    sizeof(stsCardEntry_t)
};

const SnmpType stsCardEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsCardEntry_t, stsCardShelf), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsCardEntry_t, stsCardIndex), 1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsCardEntry_t, stsCardType), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsCardEntry_t, stsCardDescr), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsCardEntry_t, stsCardSerial), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsCardEntry_t, stsCardHwVersion), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsCardEntry_t, stsCardCLEICode), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsCardEntry_t, stsCardOperStatus), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsCardEntry_t, stsCardReset), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsCardEntry
    SR_PROTOTYPE((int index));
extern void stsCardEntry_free
    SR_PROTOTYPE((stsCardEntry_t *data));

/* global data describing the stsOscillatorTable entries */
Index_t stsOscillatorEntryIndex[] = {
    { offsetof(stsOscillatorEntry_t, stsOscillatorIndex),  T_uint}
};

SnmpV2Table stsOscillatorTable = {
    NULL,
    stsOscillatorEntryIndex,
    0,
    1,
    sizeof(stsOscillatorEntry_t)
};

const SnmpType stsOscillatorEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOscillatorEntry_t, stsOscillatorIndex), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOscillatorEntry_t, stsOscillatorModuleType), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsOscillatorEntry_t, stsOscillatorEFER), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsOscillatorEntry_t, stsOscillatorESSD), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsOscillatorEntry_t, stsOscillatorESSN), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsOscillatorEntry_t, stsOscillatorFAGE), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsOscillatorEntry_t, stsOscillatorTIMD), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOscillatorEntry_t, stsOscillatorSwitchState), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOscillatorEntry_t, stsOscillatorStatus), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOscillatorEntry_t, stsOscillatorRedStatus), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsOscillatorEntry
    SR_PROTOTYPE((int index));
extern void stsOscillatorEntry_free
    SR_PROTOTYPE((stsOscillatorEntry_t *data));

/* global data describing the stsGpsTable entries */
Index_t stsGpsEntryIndex[] = {
    { offsetof(stsGpsEntry_t, stsGpsIndex),  T_uint}
};

SnmpV2Table stsGpsTable = {
    NULL,
    stsGpsEntryIndex,
    0,
    1,
    sizeof(stsGpsEntry_t)
};

const SnmpType stsGpsEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsIndex), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsStatus), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsState), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsANTD), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsAZEL), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsRSTG), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsBITS), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsPAVG), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsSIGQ), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsGpsEntry_t, stsGpsSPOS), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsGpsEntry_t, stsGpsTRMO), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsUTCT), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsVERS), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsDETF), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsEFER), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsESSD), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsESSN), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsFAGE), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsGpsEntry_t, stsGpsTIMD), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsGpsEntry_t, stsGpsReset), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};



void DeletestsGpsEntry
    SR_PROTOTYPE((int index));
extern void stsGpsEntry_free
    SR_PROTOTYPE((stsGpsEntry_t *data));

/* global data describing the stsLinkTable entries */
Index_t stsLinkEntryIndex[] = {
    { offsetof(stsLinkEntry_t, stsLinkIndex),  T_uint}
};

SnmpV2Table stsLinkTable = {
    NULL,
    stsLinkEntryIndex,
    0,
    1,
    sizeof(stsLinkEntry_t)
};

const SnmpType stsLinkEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsLinkEntry_t, stsLinkIndex), 0 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsLinkEntry_t, stsLinkType), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsLinkEntry_t, stsLinkEnable), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsLinkEntry_t, stsLinkLineCode), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsLinkEntry_t, stsLinkFMT), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsLinkEntry_t, stsLinkTermination), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsLinkEntry_t, stsLinkMFCHK), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsLinkEntry_t, stsLinkId), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsLinkEntry
    SR_PROTOTYPE((int index));
extern void stsLinkEntry_free
    SR_PROTOTYPE((stsLinkEntry_t *data));

/* global data describing the stsShelfTable entries */
Index_t stsShelfEntryIndex[] = {
    { offsetof(stsShelfEntry_t, stsShelfIndex),  T_uint}
};

SnmpV2Table stsShelfTable = {
    NULL,
    stsShelfEntryIndex,
    0,
    1,
    sizeof(stsShelfEntry_t)
};

const SnmpType stsShelfEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsShelfEntry_t, stsShelfIndex), 0 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsShelfEntry_t, stsShelfSerial), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsShelfEntry_t, stsShelfCLEICode), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsShelfEntry_t, stsShelfReset), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsShelfEntry
    SR_PROTOTYPE((int index));
extern void stsShelfEntry_free
    SR_PROTOTYPE((stsShelfEntry_t *data));

/* global data describing the stsAlarmThresholdTable entries */
Index_t stsAlarmThresholdEntryIndex[] = {
    { offsetof(stsAlarmThresholdEntry_t, stsAlarmThresholdCurrentMon),  T_uint},
    { offsetof(stsAlarmThresholdEntry_t, stsAlarmThresholdType),  T_uint}
};

SnmpV2Table stsAlarmThresholdTable = {
    NULL,
    stsAlarmThresholdEntryIndex,
    0,
    2,
    sizeof(stsAlarmThresholdEntry_t)
};

const SnmpType stsAlarmThresholdEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsAlarmThresholdEntry_t, stsAlarmThresholdCurrentMon), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsAlarmThresholdEntry_t, stsAlarmThresholdType), 1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsAlarmThresholdEntry_t, stsAlarmThresholdValue), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsAlarmThresholdEntry_t, stsAlarmThresholdTimeInterval), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsAlarmThresholdEntry
    SR_PROTOTYPE((int index));
extern void stsAlarmThresholdEntry_free
    SR_PROTOTYPE((stsAlarmThresholdEntry_t *data));

/* global data describing the stsLinkAlarmTable entries */
Index_t stsLinkAlarmEntryIndex[] = {
    { offsetof(stsLinkAlarmEntry_t, stsLinkAlarmSettingLinkType),  T_uint},
    { offsetof(stsLinkAlarmEntry_t, stsLinkAlarmSettingType),  T_uint}
};

SnmpV2Table stsLinkAlarmTable = {
    NULL,
    stsLinkAlarmEntryIndex,
    0,
    2,
    sizeof(stsLinkAlarmEntry_t)
};

const SnmpType stsLinkAlarmEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsLinkAlarmEntry_t, stsLinkAlarmSettingLinkType), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsLinkAlarmEntry_t, stsLinkAlarmSettingType), 1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsLinkAlarmEntry_t, stsLinkAlarmSettingSevirity), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsLinkAlarmEntry_t, stsLinkAlarmSevirity), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsLinkAlarmEntry
    SR_PROTOTYPE((int index));
extern void stsLinkAlarmEntry_free
    SR_PROTOTYPE((stsLinkAlarmEntry_t *data));

/* global data describing the stsOscillatorAlarmTable entries */
Index_t stsOscillatorAlarmEntryIndex[] = {
    { offsetof(stsOscillatorAlarmEntry_t, stsOscillatorAlarmSettingClock),  T_uint},
    { offsetof(stsOscillatorAlarmEntry_t, stsOscillatorAlarmSettingType),  T_uint}
};

SnmpV2Table stsOscillatorAlarmTable = {
    NULL,
    stsOscillatorAlarmEntryIndex,
    0,
    2,
    sizeof(stsOscillatorAlarmEntry_t)
};

const SnmpType stsOscillatorAlarmEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOscillatorAlarmEntry_t, stsOscillatorAlarmSettingClock), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOscillatorAlarmEntry_t, stsOscillatorAlarmSettingType), 1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOscillatorAlarmEntry_t, stsOscillatorAlarmSettingSeverity), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOscillatorAlarmEntry_t, stsOscillatorAlarmSeverity), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsOscillatorAlarmEntry
    SR_PROTOTYPE((int index));
extern void stsOscillatorAlarmEntry_free
    SR_PROTOTYPE((stsOscillatorAlarmEntry_t *data));

/* global data describing the stsOsPwrAlarmTable entries */
Index_t stsOsPwrAlarmEntryIndex[] = {
    { offsetof(stsOsPwrAlarmEntry_t, stssPwrAlarmSettingShelf),  T_uint},
    { offsetof(stsOsPwrAlarmEntry_t, stsOsPwrAlarmSettingOsPwr),  T_uint}
};

SnmpV2Table stsOsPwrAlarmTable = {
    NULL,
    stsOsPwrAlarmEntryIndex,
    0,
    2,
    sizeof(stsOsPwrAlarmEntry_t)
};

const SnmpType stsOsPwrAlarmEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOsPwrAlarmEntry_t, stssPwrAlarmSettingShelf), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOsPwrAlarmEntry_t, stsOsPwrAlarmSettingOsPwr), 1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOsPwrAlarmEntry_t, stsOsPwrAlarmSettingSeverity), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOsPwrAlarmEntry_t, stsOsPwrAlarmSeverity), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsOsPwrAlarmEntry
    SR_PROTOTYPE((int index));
extern void stsOsPwrAlarmEntry_free
    SR_PROTOTYPE((stsOsPwrAlarmEntry_t *data));

/* global data describing the stsOutputAlarmTable entries */
Index_t stsOutputAlarmEntryIndex[] = {
    { offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSettingShelf),  T_uint},
    { offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSettingSlot),  T_uint},
    { offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSettingPort),  T_uint},
    { offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSettingType),  T_uint}
};

SnmpV2Table stsOutputAlarmTable = {
    NULL,
    stsOutputAlarmEntryIndex,
    0,
    4,
    sizeof(stsOutputAlarmEntry_t)
};

const SnmpType stsOutputAlarmEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSettingShelf), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSettingSlot), 1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSettingPort), 2 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSettingType), 3 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSettingSeverity), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputAlarmEntry_t, stsOutputAlarmSeverity), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsOutputAlarmEntry
    SR_PROTOTYPE((int index));
extern void stsOutputAlarmEntry_free
    SR_PROTOTYPE((stsOutputAlarmEntry_t *data));

/* global data describing the stsOutputPortSettingTable entries */
Index_t stsOutputPortSettingEntryIndex[] = {
    { offsetof(stsOutputPortSettingEntry_t, stsOutputPortSettingShelf),  T_uint},
    { offsetof(stsOutputPortSettingEntry_t, stsOutputPortSettingSlot),  T_uint},
    { offsetof(stsOutputPortSettingEntry_t, stsOutputPortSettingPort),  T_uint}
};

SnmpV2Table stsOutputPortSettingTable = {
    NULL,
    stsOutputPortSettingEntryIndex,
    0,
    3,
    sizeof(stsOutputPortSettingEntry_t)
};

const SnmpType stsOutputPortSettingEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputPortSettingEntry_t, stsOutputPortSettingShelf), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputPortSettingEntry_t, stsOutputPortSettingSlot), 1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputPortSettingEntry_t, stsOutputPortSettingPort), 2 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputPortSettingEntry_t, stsOutputPortSettingType), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsOutputPortSettingEntry_t, stsOutputPortSettingName), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsOutputPortSettingEntry
    SR_PROTOTYPE((int index));
extern void stsOutputPortSettingEntry_free
    SR_PROTOTYPE((stsOutputPortSettingEntry_t *data));

/* global data describing the stsOutputTiuSettingTable entries */
Index_t stsOutputTiuSettingEntryIndex[] = {
    { offsetof(stsOutputTiuSettingEntry_t, stsOutputTiuSettingShelf),  T_uint},
    { offsetof(stsOutputTiuSettingEntry_t, stsOutputTiuSettingSlot),  T_uint},
    { offsetof(stsOutputTiuSettingEntry_t, stsOutputTiuSettingPort),  T_uint}
};

SnmpV2Table stsOutputTiuSettingTable = {
    NULL,
    stsOutputTiuSettingEntryIndex,
    0,
    3,
    sizeof(stsOutputTiuSettingEntry_t)
};

const SnmpType stsOutputTiuSettingEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputTiuSettingEntry_t, stsOutputTiuSettingShelf), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputTiuSettingEntry_t, stsOutputTiuSettingSlot), 1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputTiuSettingEntry_t, stsOutputTiuSettingPort), 2 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputTiuSettingEntry_t, stsOutputTiuSettingLineCoding), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputTiuSettingEntry_t, stsOutputTiuSettingFMT), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputTiuSettingEntry_t, stsOutputTiuSettingTermination), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsOutputTiuSettingEntry
    SR_PROTOTYPE((int index));
extern void stsOutputTiuSettingEntry_free
    SR_PROTOTYPE((stsOutputTiuSettingEntry_t *data));

/* global data describing the stsPerfMonTable entries */
Index_t stsPerfMonEntryIndex[] = {
    { offsetof(stsPerfMonEntry_t, stsPerfMonInputCurrentMon),  T_uint},
    { offsetof(stsPerfMonEntry_t, stsPerfMonInputMonPeriod),  T_uint},
    { offsetof(stsPerfMonEntry_t, stsPerfMonInputMonBucket),  T_uint},
    { offsetof(stsPerfMonEntry_t, stsPerfMonInputMonType),  T_uint}
};

SnmpV2Table stsPerfMonTable = {
    NULL,
    stsPerfMonEntryIndex,
    0,
    4,
    sizeof(stsPerfMonEntry_t)
};

const SnmpType stsPerfMonEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsPerfMonEntry_t, stsPerfMonInputCurrentMon), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsPerfMonEntry_t, stsPerfMonInputMonPeriod), 1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsPerfMonEntry_t, stsPerfMonInputMonBucket), 2 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsPerfMonEntry_t, stsPerfMonInputMonType), 3 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsPerfMonEntry_t, stsPerfMonInputMonValue), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsPerfMonEntry_t, stsPerfMonInputClockType), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsPerfMonEntry
    SR_PROTOTYPE((int index));
extern void stsPerfMonEntry_free
    SR_PROTOTYPE((stsPerfMonEntry_t *data));

/* global data describing the stsTerminalTable entries */
Index_t stsTerminalEntryIndex[] = {
    { offsetof(stsTerminalEntry_t, stsTerminalIndex),  T_uint}
};

SnmpV2Table stsTerminalTable = {
    NULL,
    stsTerminalEntryIndex,
    0,
    1,
    sizeof(stsTerminalEntry_t)
};

const SnmpType stsTerminalEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsTerminalEntry_t, stsTerminalIndex), 0 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTerminalEntry_t, stsTerminalBaudrate), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTerminalEntry_t, stsTerminalDataBits), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTerminalEntry_t, stsTerminalParity), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTerminalEntry_t, stsTerminalStopBits), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsTerminalEntry
    SR_PROTOTYPE((int index));
extern void stsTerminalEntry_free
    SR_PROTOTYPE((stsTerminalEntry_t *data));

/* global data describing the stsAlarmStatusTable entries */
Index_t stsAlarmStatusEntryIndex[] = {
    { offsetof(stsAlarmStatusEntry_t, stsAlarmSource),  T_uint}
};

SnmpV2Table stsAlarmStatusTable = {
    NULL,
    stsAlarmStatusEntryIndex,
    0,
    1,
    sizeof(stsAlarmStatusEntry_t)
};

const SnmpType stsAlarmStatusEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsAlarmStatusEntry_t, stsAlarmSource), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsAlarmStatusEntry_t, stsAlarmSourceType), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsAlarmStatusEntry_t, stsAlarmSeverity), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsAlarmStatusEntry_t, stsAlarmSrvEff), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsAlarmStatusEntry
    SR_PROTOTYPE((int index));
extern void stsAlarmStatusEntry_free
    SR_PROTOTYPE((stsAlarmStatusEntry_t *data));

/* global data describing the stsOutputCardTable entries */
Index_t stsOutputCardEntryIndex[] = {
    { offsetof(stsOutputCardEntry_t, stsOutputShelf),  T_uint},
    { offsetof(stsOutputCardEntry_t, stsOutputSlot),  T_uint}
};

SnmpV2Table stsOutputCardTable = {
    NULL,
    stsOutputCardEntryIndex,
    0,
    2,
    sizeof(stsOutputCardEntry_t)
};

const SnmpType stsOutputCardEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputCardEntry_t, stsOutputShelf), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputCardEntry_t, stsOutputSlot), 1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsOutputCardEntry_t, stsOutputRedState), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputCardEntry_t, stsOutputRedEnableState), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputCardEntry_t, stsOutputRedSwitchState), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputCardEntry_t, stsOutputCRCCHK), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputCardEntry_t, stsOutputCAS), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputCardEntry_t, stsOutputSSM), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsOutputCardEntry_t, stsOutputSSMValue), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsOutputCardEntry_t, stsOutputTimingFormat), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};



void DeletestsOutputCardEntry
    SR_PROTOTYPE((int index));
extern void stsOutputCardEntry_free
    SR_PROTOTYPE((stsOutputCardEntry_t *data));

/* global data describing the stsEmmStatusTable entries */
Index_t stsEmmStatusEntryIndex[] = {
    { offsetof(stsEmmStatusEntry_t, stsEmmShelf),  T_uint},
    { offsetof(stsEmmStatusEntry_t, stsEmmSensorType),  T_uint}
};



SnmpV2Table stsEmmStatusTable = {
    NULL,
    stsEmmStatusEntryIndex,
    0,
    2,
    sizeof(stsEmmStatusEntry_t)
};

const SnmpType stsEmmStatusEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsEmmStatusEntry_t, stsEmmShelf), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsEmmStatusEntry_t, stsEmmSensorType), 1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsEmmStatusEntry_t, stsEmmAlarmStatus), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsEmmStatusEntry_t, stsEmmSensorReading), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsEmmStatusEntry_t, stsEmmMajorAlmHiThresh), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsEmmStatusEntry_t, stsEmmMajorAlmLoThresh), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsEmmStatusEntry_t, stsEmmMinorAlmHiThresh), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsEmmStatusEntry_t, stsEmmMinorAlmLoThresh), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};




void DeletestsEmmStatusEntry
    SR_PROTOTYPE((int index));
extern void stsEmmStatusEntry_free
    SR_PROTOTYPE((stsEmmStatusEntry_t *data));

/* global data describing the stsAntTable entries */
Index_t stsAntEntryIndex[] = {
    { offsetof(stsAntEntry_t, stsAntIndex),  T_uint}
};

SnmpV2Table stsAntTable = {
    NULL,
    stsAntEntryIndex,
    0,
    1,
    sizeof(stsAntEntry_t)
};

const SnmpType stsAntEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsAntEntry_t, stsAntIndex), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsAntEntry_t, stsAntStatus), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsAntEntry_t, stsAntSwitchState), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsAntEntry
    SR_PROTOTYPE((int index));
extern void stsAntEntry_free
    SR_PROTOTYPE((stsAntEntry_t *data));

/* global data describing the stsTopDestTable entries */
Index_t stsTopDestEntryIndex[] = {
    { offsetof(stsTopDestEntry_t, stsTopDestNumber),  T_uint}
};

SnmpV2Table stsTopDestTable = {
    NULL,
    stsTopDestEntryIndex,
    0,
    1,
    sizeof(stsTopDestEntry_t)
};

const SnmpType stsTopDestEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsTopDestEntry_t, stsTopDestNumber), 0 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsTopDestEntry_t, stsTopDestIp), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTopDestEntry_t, stsTopDestUdpPort), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTopDestEntry_t, stsTopDestVlanId), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsTopDestEntry
    SR_PROTOTYPE((int index));
extern void stsTopDestEntry_free
    SR_PROTOTYPE((stsTopDestEntry_t *data));

/* global data describing the stsTrapRegTable entries */
Index_t stsTrapRegEntryIndex[] = {
    { offsetof(stsTrapRegEntry_t, stsTrapRegMgrNumber),  T_uint}
};

SnmpV2Table stsTrapRegTable = {
    NULL,
    stsTrapRegEntryIndex,
    0,
    1,
    sizeof(stsTrapRegEntry_t)
};

const SnmpType stsTrapRegEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsTrapRegEntry_t, stsTrapRegMgrNumber), 0 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsTrapRegEntry_t, stsTrapRegMgrIp), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTrapRegEntry_t, stsTrapRegMgrUdpPort), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsTrapRegEntry_t, stsTrapRegCommunityStr), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTrapRegEntry_t, stsTrapRegState), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsTrapRegEntry
    SR_PROTOTYPE((int index));
extern void stsTrapRegEntry_free
    SR_PROTOTYPE((stsTrapRegEntry_t *data));

/* global data describing the stsClockPriTable entries */
Index_t stsClockPriEntryIndex[] = {
    { offsetof(stsClockPriEntry_t, stsClockPriIndex),  T_uint}
};

SnmpV2Table stsClockPriTable = {
    NULL,
    stsClockPriEntryIndex,
    0,
    1,
    sizeof(stsClockPriEntry_t)
};

const SnmpType stsClockPriEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsClockPriEntry_t, stsClockPriIndex), 0 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsClockPriEntry_t, stsClockPriSetting), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeletestsClockPriEntry
    SR_PROTOTYPE((int index));
extern void stsClockPriEntry_free
    SR_PROTOTYPE((stsClockPriEntry_t *data));

int
k_LarusSts_initialize(void)
{
if (kDebug)
printf(" enter k_LarusSts_initialize\n");
    return 1;
}

int
k_LarusSts_terminate(void)
{
if (kDebug)
printf(" enter k_LarusSts_terminate\n");
    return 1;
}

stsSystem_t *
k_stsSystem_get(int serialNum, ContextInfo *contextInfo,
                int nominator)
{
   int ret;

if (kDebug)
printf("In k_stsSystem_get\r\n");
   ret = systemActionFuncGet(&stsSystemData);

   if(ret != OK)
	   return NULL;

   if (nominator == -1)
      ZERO_VALID(stsSystemData.valid);
   else
      SET_ALL_VALID(stsSystemData.valid);

   return (&stsSystemData);
}

#ifdef SETS

int
k_stsSystem_test(ObjectInfo *object, ObjectSyntax *value,
                 doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_stsSystem_ready(ObjectInfo *object, ObjectSyntax *value, 
                  doList_t *doHead, doList_t *dp)
{
	
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsSystem_set(stsSystem_t *data,
                ContextInfo *contextInfo, int function)
{
  int ret;

  ret = systemActionFuncSet (data);

  if(ret != OK)
	  return  COMMIT_FAILED_ERROR;
  
 
   return NO_ERROR;
}

#ifdef SR_stsSystem_UNDO
/* add #define SR_stsSystem_UNDO in sitedefs.h to
 * include the undo routine for the stsSystem family.
 */
int
stsSystem_undo(doList_t *doHead, doList_t *doCur,
               ContextInfo *contextInfo)
{
if (kDebug)
printf(" enter k_stsSystem_undo\n");
    return UNDO_FAILED_ERROR;
}
#endif /* SR_stsSystem_UNDO */

#endif /* SETS */

stsClockInput_t *
k_stsClockInput_get(int serialNum, ContextInfo *contextInfo,
                    int nominator)
{
   int ret;

   ret = CsmClockInputGetReq(&stsClockInputData);

   if(ret != OK)
	   return NULL;

   if (nominator == -1)
      ZERO_VALID(stsClockInputData.valid);
   else
      SET_ALL_VALID(stsClockInputData.valid);

   return (&stsClockInputData);
}

#ifdef SETS
int
k_stsClockInput_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsClockInput_ready(ObjectInfo *object, ObjectSyntax *value, 
                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsClockInput_set(stsClockInput_t *data,
                    ContextInfo *contextInfo, int function)
{
  int ret;

  ret = CsmClockInputSetReq (data);

  if(ret != OK)
	  return  COMMIT_FAILED_ERROR;
  
 
   return NO_ERROR;
}

#ifdef SR_stsClockInput_UNDO
/* add #define SR_stsClockInput_UNDO in sitedefs.h to
 * include the undo routine for the stsClockInput family.
 */
int
stsClockInput_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsClockInput_UNDO */

#endif /* SETS */

stsClockInputSSM_t *
k_stsClockInputSSM_get(int serialNum, ContextInfo *contextInfo,
                       int nominator)
{


   return (&stsClockInputSSMData);
}

#ifdef SETS
int
k_stsClockInputSSM_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsClockInputSSM_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsClockInputSSM_set(stsClockInputSSM_t *data,
                       ContextInfo *contextInfo, int function)
{

  
   /* stsClockInputSSMData = Clone_stsClockInputSSM(data);  */
   return NO_ERROR;
}

#ifdef SR_stsClockInputSSM_UNDO
/* add #define SR_stsClockInputSSM_UNDO in sitedefs.h to
 * include the undo routine for the stsClockInputSSM family.
 */
int
stsClockInputSSM_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsClockInputSSM_UNDO */

#endif /* SETS */

stsTop_t *
k_stsTop_get(int serialNum, ContextInfo *contextInfo,
             int nominator)
{
   int ret;
if (kDebug)
printf("In k_top_get\r\n");
   ret = topActionFuncGet(&stsTopData);

   if(ret != OK)
	   return NULL;

   if (nominator == -1)
      ZERO_VALID(stsTopData.valid);
   else
      SET_ALL_VALID(stsTopData.valid);

   return (&stsTopData);
}

#ifdef SETS
int
k_stsTop_test(ObjectInfo *object, ObjectSyntax *value,
              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsTop_ready(ObjectInfo *object, ObjectSyntax *value, 
               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsTop_set(stsTop_t *data,
             ContextInfo *contextInfo, int function)
{
    int ret;

    ret = topActionFuncSet (data);

    if(ret != OK)
	    return  COMMIT_FAILED_ERROR;
   
    return NO_ERROR;
}

#ifdef SR_stsTop_UNDO
/* add #define SR_stsTop_UNDO in sitedefs.h to
 * include the undo routine for the stsTop family.
 */
int
stsTop_undo(doList_t *doHead, doList_t *doCur,
            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsTop_UNDO */

#endif /* SETS */

stsPerfMonManual_t *
k_stsPerfMonManual_get(int serialNum, ContextInfo *contextInfo,
                       int nominator)
{
   int ret;

   ret = ManSmuTestGet(&stsPerfMonManualData);

   if(ret != OK)
	   return NULL;

   if (nominator == -1)
      ZERO_VALID(stsPerfMonManualData.valid);
   else
      SET_ALL_VALID(stsPerfMonManualData.valid);

    return (&stsPerfMonManualData);
}

#ifdef SETS
int
k_stsPerfMonManual_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsPerfMonManual_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsPerfMonManual_set(stsPerfMonManual_t *data,
                       ContextInfo *contextInfo, int function)
{

  int ret;

  ret = ManSmuTestSet(data);

  if(ret != OK)
	  return  COMMIT_FAILED_ERROR;
  
 
   return NO_ERROR;
}

#ifdef SR_stsPerfMonManual_UNDO
/* add #define SR_stsPerfMonManual_UNDO in sitedefs.h to
 * include the undo routine for the stsPerfMonManual family.
 */
int
stsPerfMonManual_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsPerfMonManual_UNDO */

#endif /* SETS */

int
initCommStr_set()
{
    int new_index;
	stsCommStr_t data;

    data.stsReadOnlyCommStr = MakeOctetStringFromText("public");
 
    data.stsReadWriteCommStr = MakeOctetStringFromText("private");

    /*
     * Safe checking whether the communities.tp has been initialized and,
     * if not, initialize here.
     */
    if (communities.tp == NULL)
        init_communities();
        
    /*
     * Initialize the values for the two indices for the new entry:
     * - community string
     * - ip address
     */
    communities.tip[0].value.octet_val = data.stsReadOnlyCommStr;
    communities.tip[1].value.uint_val  = 0;
    
    /*
     * NewTableEntry (utility function provided by SR for v2 tables)
     * uses the new indices to allocate a new entry, clear it up, and
     * return the new indes to community.tp[] pointer.
     */
    if ((new_index = NewTableEntry(&communities)) == -1)
        return(-1);

    /*
     * Fill in the values for community string, ip address and privilege
     * for the new entry.
     */
	
    ((COMMUNITY *)(communities.tp[new_index]))->community_name = CloneOctetString(data.stsReadOnlyCommStr);
    ((COMMUNITY *)(communities.tp[new_index]))->ip_addr = 0;
    ((COMMUNITY *)(communities.tp[new_index]))->privs = SR_READ_ONLY; 

		
    /*
     * Initialize the values for the two indices for the new entry:
     * - community string
     * - ip address
     */
    communities.tip[0].value.octet_val = data.stsReadWriteCommStr;
    communities.tip[1].value.uint_val  = 0;
   

    /*
     * NewTableEntry (utility function provided by SR for v2 tables)
     * uses the new indices to allocate a new entry, clear it up, and
     * return the new indes to community.tp[] pointer.
     */
    if ((new_index = NewTableEntry(&communities)) == -1)
        return(-1);

    /*
     * Fill in the values for community string, ip address and privilege
     * for the new entry.
     */
	
    ((COMMUNITY *)communities.tp[new_index])->community_name = CloneOctetString(data.stsReadWriteCommStr);
    ((COMMUNITY *)communities.tp[new_index])->ip_addr = 0;
    ((COMMUNITY *)communities.tp[new_index])->privs = SR_READ_WRITE; 

		
/* end */
   
   /* stsCommStr_free(stsCommStrData);  */
   /* stsCommStrData = Clone_stsCommStr(data);	*/
   
    stsCommStrData.stsReadOnlyCommStr = MakeOctetStringFromText("public");
 
    stsCommStrData.stsReadWriteCommStr = MakeOctetStringFromText("private");

   
   return NO_ERROR;
}

/* ------------------------------------------------------------------------
 * AddCommStrGetIndexOnCommStr()
 *
 * Add the communities.tp[] entry ptr with the matching commStrCommString.
 * and commStrMgrIpAddr.
 *
 */
int
AddCommStrGetIndexOnCommStr(OctetString *commStrCommString, int access)
{
    int new_index;
   
   /*
    * Reject NULL octet string as the key.
    */
   if (commStrCommString == NULL)
       return ERROR;

    if (communities.tp == NULL)
        init_communities();
        
    /*
     * Initialize the values for the two indices for the new entry:
     * - community string
     * - ip address
     */
    communities.tip[0].value.octet_val = commStrCommString;
    communities.tip[1].value.uint_val  = 0;
    
    /*
     * NewTableEntry (utility function provided by SR for v2 tables)
     * uses the new indices to allocate a new entry, clear it up, and
     * return the new indes to community.tp[] pointer.
     */
    if ((new_index = NewTableEntry(&communities)) == -1)
        return(-1);

    /*
     * Fill in the values for community string, ip address and privilege
     * for the new entry.
     */
	
    ((COMMUNITY *)(communities.tp[new_index]))->community_name = CloneOctetString(commStrCommString);
    ((COMMUNITY *)(communities.tp[new_index]))->ip_addr = 0;
    ((COMMUNITY *)(communities.tp[new_index]))->privs = access; 
   
     return NO_ERROR;

}


/* ------------------------------------------------------------------------
 * DelCommStrGetIndexOnCommStr()
 *
 * Get the communities.tp[] entry ptr with the matching commStrCommString.
 * and commStrMgrIpAddr.
 *
 */
int
DelCommStrGetIndexOnCommStr(OctetString *commStrCommString)
{
   int index;
   
   /*
    * Reject NULL octet string as the key.
    */
   if (commStrCommString == NULL)
       return ERROR;
   
   /*
    * Search for the entry based on userID 
    */
   communities.tip[0].value.octet_val = commStrCommString;
   communities.tip[1].value.uint_val  = 0;
   index = SearchTable(&communities, EXACT);
   if (index == -1)
       return ERROR;
   /*
     * Free the memory allocated for the entry before removing it.
     */    
    FreeEntries(communitiesType, communities.tp[index]);
    free(communities.tp[index]);
    
    /*
     * Remove the entry in SNMP Research Table
     */ 
    RemoveTableEntry(&communities, index);

	return (NO_ERROR);

}



stsCommStr_t *
k_stsCommStr_get(int serialNum, ContextInfo *contextInfo,
                 int nominator)
{
	 SET_ALL_VALID(stsCommStrData.valid);

 
   return (&stsCommStrData);
}

#ifdef SETS
int
k_stsCommStr_test(ObjectInfo *object, ObjectSyntax *value,
                  doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsCommStr_ready(ObjectInfo *object, ObjectSyntax *value, 
                   doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsCommStr_set(stsCommStr_t *data,
                 ContextInfo *contextInfo, int function)
{

    char prtString[101];

	int status;

    if (VALID(I_stsReadOnlyCommStr, data->valid)) {
       if (data->stsReadOnlyCommStr == NULL) {
            return(GEN_ERROR);
         }

       if (data->stsReadOnlyCommStr == stsCommStrData.stsReadWriteCommStr) {
            return(GEN_ERROR);
        }

       /*
        * Cannot find the existing entry to delete.
        */
       if ((status = DelCommStrGetIndexOnCommStr(stsCommStrData.stsReadOnlyCommStr)) == ERROR)
            return(GEN_ERROR);
        
  
        if (stsCommStrData.stsReadOnlyCommStr!= NULL) {
            FreeOctetString(stsCommStrData.stsReadOnlyCommStr);
        }
        if ((stsCommStrData.stsReadOnlyCommStr= 
             CloneOctetString(data->stsReadOnlyCommStr)) == NULL) {
               strncpy(prtString, 
                        data->stsReadOnlyCommStr->octet_ptr,
                        data->stsReadOnlyCommStr->length);

                prtString[data->stsReadOnlyCommStr->length] = '\0';
if (kDebug)
printf("Could not set stsReadOnlyCommStr: %s\n", prtString);
            return (GEN_ERROR);
        }

       if ((status = AddCommStrGetIndexOnCommStr(stsCommStrData.stsReadOnlyCommStr, SR_READ_ONLY )) == ERROR)
            return(GEN_ERROR);
    }


     if (VALID(I_stsReadWriteCommStr, data->valid)) {
       if (data->stsReadWriteCommStr == NULL) {
            return(GEN_ERROR);
         }

       if (data->stsReadWriteCommStr == stsCommStrData.stsReadOnlyCommStr) {
            return(GEN_ERROR);
        }

       /*
        * Cannot find the existing entry to delete.
        */
       if ((status = DelCommStrGetIndexOnCommStr(stsCommStrData.stsReadWriteCommStr)) == ERROR)
            return(GEN_ERROR);
        
  
        if (stsCommStrData.stsReadWriteCommStr!= NULL) {
            FreeOctetString(stsCommStrData.stsReadWriteCommStr);
        }
        if ((stsCommStrData.stsReadWriteCommStr= 
             CloneOctetString(data->stsReadWriteCommStr)) == NULL) {
               strncpy(prtString, 
                        data->stsReadWriteCommStr->octet_ptr,
                        data->stsReadWriteCommStr->length);

                prtString[data->stsReadWriteCommStr->length] = '\0';
if (kDebug)
printf("Could not set stsReadWriteCommStr: %s\n", prtString);
            return (GEN_ERROR);
        }

       if ((status = AddCommStrGetIndexOnCommStr(stsCommStrData.stsReadWriteCommStr, SR_READ_WRITE )) == ERROR)
            return(GEN_ERROR);
    }

  
    /* stsCommStrData = Clone_stsCommStr(data);  */
   return NO_ERROR;
}

#ifdef SR_stsCommStr_UNDO
/* add #define SR_stsCommStr_UNDO in sitedefs.h to
 * include the undo routine for the stsCommStr family.
 */
int
stsCommStr_undo(doList_t *doHead, doList_t *doCur,
                ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsCommStr_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsClockInputStatusTable */
void
DeletestsClockInputStatusEntry (int index)
{
    stsClockInputStatusEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsClockInputStatusEntry_t *) stsClockInputStatusTable.tp[index];

    /* free the old entry and remove it from the table */
 
    RemoveTableEntry(&stsClockInputStatusTable, index);
}

stsClockInputStatusEntry_t *
k_stsClockInputStatusEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 stsClockInputStatusIndex)
{
   static stsClockInputStatusEntry_t stsClockInputStatusEntryData;
   int ret;


   stsClockInputStatusEntryData.stsClockInputStatusIndex = stsClockInputStatusIndex;

   /*
    * put your code to retrieve the information here
    */


   ret = CsmClockInputStatusGetReq (&stsClockInputStatusEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsClockInputStatusEntryData.valid);
   else
      SET_ALL_VALID(stsClockInputStatusEntryData.valid);

   return(&stsClockInputStatusEntryData);
 
}

/* This routine deletes an entry from the stsClockInputSSMTable */
void
DeletestsClockInputSSMEntry (int index)
{
    stsClockInputSSMEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsClockInputSSMEntry_t *) stsClockInputSSMTable.tp[index];

    /* free the old entry and remove it from the table */
 
    RemoveTableEntry(&stsClockInputSSMTable, index);
}

stsClockInputSSMEntry_t *
k_stsClockInputSSMEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 stsClockInputSSMIndex,
                            SR_INT32 stsClockInputSSMInputSelect)
{
#if 0
    int index;

    stsClockInputSSMTable.tip[0].value.uint_val = stsClockInputSSMIndex;
    stsClockInputSSMTable.tip[1].value.uint_val = stsClockInputSSMInputSelect;
    if ((index = SearchTable(&stsClockInputSSMTable, searchType)) == -1) {
        return NULL;
    }

    return (stsClockInputSSMEntry_t *) stsClockInputSSMTable.tp[index];
#endif

static stsClockInputSSMEntry_t stsClockInputSSMEntryData;
int ret;

stsClockInputSSMEntryData.stsClockInputSSMIndex = stsClockInputSSMIndex;
stsClockInputSSMEntryData.stsClockInputSSMInputSelect = stsClockInputSSMInputSelect;

ret = SSMGet(&stsClockInputSSMEntryData);

if (ret != OK){
	return NULL;
}

return(&stsClockInputSSMEntryData);


}

/* This routine deletes an entry from the stsCardTable */
void
DeletestsCardEntry (int index)
{
    stsCardEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsCardEntry_t *) stsCardTable.tp[index];

    /* free the old entry and remove it from the table */
  
    RemoveTableEntry(&stsCardTable, index);
}

stsCardEntry_t *
k_stsCardEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_INT32 stsCardShelf,
                   SR_INT32 stsCardIndex)
{
  static stsCardEntry_t stsCardEntryData;
   int ret;

   SR_INT32  cardShelf;
   SR_INT32  cardIndex;

   
   /*
    * put your code to retrieve the information here
    */
   stsCardEntryData.stsCardShelf = stsCardShelf;
   stsCardEntryData.stsCardIndex =stsCardIndex;
   ret = smCardGet(&stsCardEntryData);



   if (searchType == EXACT)
   {
     cardShelf--;
     cardIndex--;

 
     if (ret != OK)
      return NULL;
   }
   else  /* GETNEXT */
   {
     if ( (cardShelf < 0) || (cardIndex < 0) )
        return NULL;

 
   }

 
   if (nominator == -1)
      ZERO_VALID(stsCardEntryData.valid);
   else
      SET_ALL_VALID(stsCardEntryData.valid);
   return(&stsCardEntryData);
}

#ifdef SETS
int
k_stsCardEntry_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsCardEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsCardEntry_set_defaults(doList_t *dp)
{


    stsCardEntry_t *data = (stsCardEntry_t *) (dp->data);
if (kDebug)
printf(" ====  enter k_stsCardEntry_set_defaults \n");

    if ((data->stsCardDescr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsCardSerial = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsCardHwVersion = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsCardCLEICode = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsCardEntry_set(stsCardEntry_t *data,
                   ContextInfo *contextInfo, int function)
{

    int cardShelf, cardIndex;
    int ret;

   cardShelf = data->stsCardShelf;
   cardIndex  = data->stsCardIndex;

   if ( (cardShelf < 1) || (cardShelf > 4) )
   {
if (kDebug)
printf("Shelf number is out of sequence.\n");
      return(COMMIT_FAILED_ERROR);
   }
   if ( (cardIndex < 1) || (cardIndex > 18) )
   {
if (kDebug)
printf("Card index number is out of sequence.\n");
     return(COMMIT_FAILED_ERROR);
   }
   
   if ((ret = smCardSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
   }
   

    return NO_ERROR;

}

#ifdef SR_stsCardEntry_UNDO
/* add #define SR_stsCardEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsCardEntry family.
 */
int
stsCardEntry_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsCardEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsOscillatorTable */
void
DeletestsOscillatorEntry (int index)
{
    stsOscillatorEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsOscillatorEntry_t *) stsOscillatorTable.tp[index];

    /* free the old entry and remove it from the table */
 
    RemoveTableEntry(&stsOscillatorTable, index);
}

stsOscillatorEntry_t *
k_stsOscillatorEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 stsOscillatorIndex)
{
   static stsOscillatorEntry_t stsOscillatorEntryData;
   int ret;


   if(stsOscillatorIndex > 2 || stsOscillatorIndex < 1)
	       return NULL;

   stsOscillatorEntryData.stsOscillatorIndex = stsOscillatorIndex;

   /*
    * put your code to retrieve the information here
    */


   ret = oscGet(&stsOscillatorEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsOscillatorEntryData.valid);
   else
      SET_ALL_VALID(stsOscillatorEntryData.valid);

   return(&stsOscillatorEntryData);

}

#ifdef SETS
int
k_stsOscillatorEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsOscillatorEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsOscillatorEntry_set_defaults(doList_t *dp)
{
    stsOscillatorEntry_t *data = (stsOscillatorEntry_t *) (dp->data);

    if ((data->stsOscillatorEFER = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsOscillatorESSD = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsOscillatorESSN = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsOscillatorFAGE = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsOscillatorTIMD = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsOscillatorEntry_set(stsOscillatorEntry_t *data,
                         ContextInfo *contextInfo, int function)
{

    int ret;

 
    if ((ret = oscSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

    return NO_ERROR;

}

#ifdef SR_stsOscillatorEntry_UNDO
/* add #define SR_stsOscillatorEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsOscillatorEntry family.
 */
int
stsOscillatorEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsOscillatorEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsGpsTable */
void
DeletestsGpsEntry (int index)
{
    stsGpsEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsGpsEntry_t *) stsGpsTable.tp[index];

    /* free the old entry and remove it from the table */
 
    RemoveTableEntry(&stsGpsTable, index);
}

stsGpsEntry_t *
k_stsGpsEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 stsGpsIndex)
{
   static stsGpsEntry_t stsGpsEntryData;
   int ret;


   if(stsGpsIndex > 2 || stsGpsIndex < 1)
	       return NULL;

   stsGpsEntryData.stsGpsIndex = stsGpsIndex;

   /*
    * put your code to retrieve the information here
    */


   ret = gpsGet(&stsGpsEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsGpsEntryData.valid);
   else
      SET_ALL_VALID(stsGpsEntryData.valid);

   return(&stsGpsEntryData);
 
}

#ifdef SETS
int
k_stsGpsEntry_test(ObjectInfo *object, ObjectSyntax *value,
                   doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsGpsEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                    doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsGpsEntry_set_defaults(doList_t *dp)
{
    stsGpsEntry_t *data = (stsGpsEntry_t *) (dp->data);

    if ((data->stsGpsANTD = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsAZEL = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsRSTG = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsBITS = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsPAVG = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsSIGQ = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsSPOS = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsTRMO = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsUTCT = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsVERS = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsDETF = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsEFER = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsESSD = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsESSN = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsFAGE = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsGpsTIMD = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsGpsEntry_set(stsGpsEntry_t *data,
                  ContextInfo *contextInfo, int function)
{

   int ret;

 
    if ((ret = gpsSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

#ifdef SR_stsGpsEntry_UNDO
/* add #define SR_stsGpsEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsGpsEntry family.
 */
int
stsGpsEntry_undo(doList_t *doHead, doList_t *doCur,
                 ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsGpsEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsLinkTable */
void
DeletestsLinkEntry (int index)
{
    stsLinkEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsLinkEntry_t *) stsLinkTable.tp[index];

    /* free the old entry and remove it from the table */
  
    RemoveTableEntry(&stsLinkTable, index);
}

stsLinkEntry_t *
k_stsLinkEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_INT32 stsLinkIndex)
{
   static stsLinkEntry_t stsLinkEntryData;
   int ret;


   if(stsLinkIndex > 5 || stsLinkIndex < 1)
	       return NULL;

   stsLinkEntryData.stsLinkIndex = stsLinkIndex;

   /*
    * put your code to retrieve the information here
    */


   ret = inpFramerLinkGet(&stsLinkEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsLinkEntryData.valid);
   else
      SET_ALL_VALID(stsLinkEntryData.valid);

   return(&stsLinkEntryData);
 
}

#ifdef SETS
int
k_stsLinkEntry_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsLinkEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsLinkEntry_set_defaults(doList_t *dp)
{
    stsLinkEntry_t *data = (stsLinkEntry_t *) (dp->data);

    if ((data->stsLinkId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsLinkEntry_set(stsLinkEntry_t *data,
                   ContextInfo *contextInfo, int function)
{

     int ret;

 
    if ((ret = inpFramerLinkSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

    return NO_ERROR;

}

#ifdef SR_stsLinkEntry_UNDO
/* add #define SR_stsLinkEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsLinkEntry family.
 */
int
stsLinkEntry_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsLinkEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsShelfTable */
void
DeletestsShelfEntry (int index)
{
    stsShelfEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsShelfEntry_t *) stsShelfTable.tp[index];

    /* free the old entry and remove it from the table */
  
    RemoveTableEntry(&stsShelfTable, index);
}

stsShelfEntry_t *
k_stsShelfEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 stsShelfIndex)
{
   static stsShelfEntry_t stsShelfEntryData;
   int ret;


   stsShelfEntryData.stsShelfIndex = stsShelfIndex;

   /*
    * put your code to retrieve the information here
    */


   ret = smShelfGet(&stsShelfEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsShelfEntryData.valid);
   else
      SET_ALL_VALID(stsShelfEntryData.valid);

   return(&stsShelfEntryData);
 
}

#ifdef SETS
int
k_stsShelfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsShelfEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsShelfEntry_set_defaults(doList_t *dp)
{
    stsShelfEntry_t *data = (stsShelfEntry_t *) (dp->data);

    if ((data->stsShelfSerial = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->stsShelfCLEICode = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsShelfEntry_set(stsShelfEntry_t *data,
                    ContextInfo *contextInfo, int function)
{

   int ret;

 
    if ((ret = smShelfSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

#ifdef SR_stsShelfEntry_UNDO
/* add #define SR_stsShelfEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsShelfEntry family.
 */
int
stsShelfEntry_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsShelfEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsAlarmThresholdTable */
void
DeletestsAlarmThresholdEntry (int index)
{
    stsAlarmThresholdEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsAlarmThresholdEntry_t *) stsAlarmThresholdTable.tp[index];

    /* free the old entry and remove it from the table */
 
    RemoveTableEntry(&stsAlarmThresholdTable, index);
}

stsAlarmThresholdEntry_t *
k_stsAlarmThresholdEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 stsAlarmThresholdCurrentMon,
                             SR_INT32 stsAlarmThresholdType)
{
   static stsAlarmThresholdEntry_t stsAlarmThresholdData;
   int ret;

   if(stsAlarmThresholdCurrentMon > 5 || stsAlarmThresholdCurrentMon < 1)
	       return NULL;
   if(stsAlarmThresholdType > 6 || stsAlarmThresholdType < 1)
   	       return NULL;

   stsAlarmThresholdData.stsAlarmThresholdCurrentMon = stsAlarmThresholdCurrentMon;

   stsAlarmThresholdData.stsAlarmThresholdType = stsAlarmThresholdType;
   /*
    * put your code to retrieve the information here
    */
if (kDebug)
printf("before k_stsAlarmThresholdEntry_get %d, %d\n", stsAlarmThresholdCurrentMon, stsAlarmThresholdType);

   ret = pmThresholdGet(&stsAlarmThresholdData);

if (kDebug)
printf("after k_stsAlarmThresholdEntry_get ret = %d\n", ret);


   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsAlarmThresholdData.valid);
   else
      SET_ALL_VALID(stsAlarmThresholdData.valid);

   return(&stsAlarmThresholdData);
 

}

#ifdef SETS
int
k_stsAlarmThresholdEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_stsAlarmThresholdEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsAlarmThresholdEntry_set_defaults(doList_t *dp)
{
    stsAlarmThresholdEntry_t *data = (stsAlarmThresholdEntry_t *) (dp->data);
if (kDebug)
printf("enter k_stsAlarmThresholdEntry_set_defaults\n");

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsAlarmThresholdEntry_set(stsAlarmThresholdEntry_t *data,
                             ContextInfo *contextInfo, int function)
{

    int ret;

if (kDebug)
printf("enter k_stsAlarmThresholdEntry_set\n");
    if ((ret = pmThresholdSet(data)) != OK) {
if (kDebug)
printf("not OK\n");
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

    return NO_ERROR;

}

#ifdef SR_stsAlarmThresholdEntry_UNDO
/* add #define SR_stsAlarmThresholdEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsAlarmThresholdEntry family.
 */
int
stsAlarmThresholdEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsAlarmThresholdEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsLinkAlarmTable */
void
DeletestsLinkAlarmEntry (int index)
{
    stsLinkAlarmEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsLinkAlarmEntry_t *) stsLinkAlarmTable.tp[index];

    /* free the old entry and remove it from the table */
 
    RemoveTableEntry(&stsLinkAlarmTable, index);
}

stsLinkAlarmEntry_t *
k_stsLinkAlarmEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_INT32 stsLinkAlarmSettingLinkType,
                        SR_INT32 stsLinkAlarmSettingType)
{
   static stsLinkAlarmEntry_t stsLinkAlarmEntryData;
   int ret;

   if(stsLinkAlarmSettingLinkType > 8 || stsLinkAlarmSettingLinkType < 1)
	       return NULL;
   if(stsLinkAlarmSettingType > 10 || stsLinkAlarmSettingType < 1)
   	       return NULL;

   stsLinkAlarmEntryData.stsLinkAlarmSettingLinkType = stsLinkAlarmSettingLinkType;

   stsLinkAlarmEntryData.stsLinkAlarmSettingType = stsLinkAlarmSettingType;
   /*
    * put your code to retrieve the information here
    */

   ret = pmSeverityGet(&stsLinkAlarmEntryData);

if (kDebug) 
printf("after k_stsLinkAlarmEntry_get ret = %d\n", ret);
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsLinkAlarmEntryData.valid);
   else
      SET_ALL_VALID(stsLinkAlarmEntryData.valid);

   return(&stsLinkAlarmEntryData);

}

#ifdef SETS
int
k_stsLinkAlarmEntry_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsLinkAlarmEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsLinkAlarmEntry_set_defaults(doList_t *dp)
{
    stsLinkAlarmEntry_t *data = (stsLinkAlarmEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsLinkAlarmEntry_set(stsLinkAlarmEntry_t *data,
                        ContextInfo *contextInfo, int function)
{

   int ret;

 
    if ((ret = pmSeveritySet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

  
    return NO_ERROR;

}

#ifdef SR_stsLinkAlarmEntry_UNDO
/* add #define SR_stsLinkAlarmEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsLinkAlarmEntry family.
 */
int
stsLinkAlarmEntry_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsLinkAlarmEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsOscillatorAlarmTable */
void
DeletestsOscillatorAlarmEntry (int index)
{
    stsOscillatorAlarmEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsOscillatorAlarmEntry_t *) stsOscillatorAlarmTable.tp[index];

    /* free the old entry and remove it from the table */
 
    RemoveTableEntry(&stsOscillatorAlarmTable, index);
}

stsOscillatorAlarmEntry_t *
k_stsOscillatorAlarmEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_INT32 stsOscillatorAlarmSettingClock,
                              SR_INT32 stsOscillatorAlarmSettingType)
{
   
   static stsOscillatorAlarmEntry_t stsOscillatorAlarmEntryData;
   int ret;

  
   if(stsOscillatorAlarmSettingClock > 2 || stsOscillatorAlarmSettingClock < 1)
	       return NULL;
   if(stsOscillatorAlarmSettingType > 5 || stsOscillatorAlarmSettingType < 1)
	       return NULL;
   stsOscillatorAlarmEntryData.stsOscillatorAlarmSettingClock = stsOscillatorAlarmSettingClock;
   stsOscillatorAlarmEntryData.stsOscillatorAlarmSettingType = stsOscillatorAlarmSettingType;
   /*
    * put your code to retrieve the information here
    */


   ret = oscAlarmEntry_get(&stsOscillatorAlarmEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsOscillatorAlarmEntryData.valid);
   else
      SET_ALL_VALID(stsOscillatorAlarmEntryData.valid);
   
   return(&stsOscillatorAlarmEntryData);  
}

#ifdef SETS
int
k_stsOscillatorAlarmEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsOscillatorAlarmEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsOscillatorAlarmEntry_set_defaults(doList_t *dp)
{
    stsOscillatorAlarmEntry_t *data = (stsOscillatorAlarmEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsOscillatorAlarmEntry_set(stsOscillatorAlarmEntry_t *data,
                              ContextInfo *contextInfo, int function)
{

    int ret;

    if ((ret = oscAlarmEntry_set(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);
   

    return NO_ERROR;

}

#ifdef SR_stsOscillatorAlarmEntry_UNDO
/* add #define SR_stsOscillatorAlarmEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsOscillatorAlarmEntry family.
 */
int
stsOscillatorAlarmEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsOscillatorAlarmEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsOsPwrAlarmTable */
void
DeletestsOsPwrAlarmEntry (int index)
{
    stsOsPwrAlarmEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsOsPwrAlarmEntry_t *) stsOsPwrAlarmTable.tp[index];

    /* free the old entry and remove it from the table */
  
    RemoveTableEntry(&stsOsPwrAlarmTable, index);
}

stsOsPwrAlarmEntry_t *
k_stsOsPwrAlarmEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 stssPwrAlarmSettingShelf,
                         SR_INT32 stsOsPwrAlarmSettingOsPwr)
{
    int index;

    stsOsPwrAlarmTable.tip[0].value.uint_val = stssPwrAlarmSettingShelf;
    stsOsPwrAlarmTable.tip[1].value.uint_val = stsOsPwrAlarmSettingOsPwr;
    if ((index = SearchTable(&stsOsPwrAlarmTable, searchType)) == -1) {
        return NULL;
    }

    return (stsOsPwrAlarmEntry_t *) stsOsPwrAlarmTable.tp[index];

}

#ifdef SETS
int
k_stsOsPwrAlarmEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsOsPwrAlarmEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsOsPwrAlarmEntry_set_defaults(doList_t *dp)
{
    stsOsPwrAlarmEntry_t *data = (stsOsPwrAlarmEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsOsPwrAlarmEntry_set(stsOsPwrAlarmEntry_t *data,
                         ContextInfo *contextInfo, int function)
{

    int index;
    stsOsPwrAlarmEntry_t *newrow = NULL;

    /* find this entry in the table */
    stsOsPwrAlarmTable.tip[0].value.uint_val = data->stssPwrAlarmSettingShelf;
    stsOsPwrAlarmTable.tip[1].value.uint_val = data->stsOsPwrAlarmSettingOsPwr;
    if ((index = SearchTable(&stsOsPwrAlarmTable, EXACT)) != -1) {
        newrow = (stsOsPwrAlarmEntry_t *) stsOsPwrAlarmTable.tp[index];
    }
    else {
        return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_stsOsPwrAlarmSeverity, stsOsPwrAlarmEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);


    return NO_ERROR;

}

#ifdef SR_stsOsPwrAlarmEntry_UNDO
/* add #define SR_stsOsPwrAlarmEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsOsPwrAlarmEntry family.
 */
int
stsOsPwrAlarmEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsOsPwrAlarmEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsOutputAlarmTable */
void
DeletestsOutputAlarmEntry (int index)
{
    stsOutputAlarmEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsOutputAlarmEntry_t *) stsOutputAlarmTable.tp[index];

    /* free the old entry and remove it from the table */
  
    RemoveTableEntry(&stsOutputAlarmTable, index);
}

stsOutputAlarmEntry_t *
k_stsOutputAlarmEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 stsOutputAlarmSettingShelf,
                          SR_INT32 stsOutputAlarmSettingSlot,
                          SR_INT32 stsOutputAlarmSettingPort,
                          SR_INT32 stsOutputAlarmSettingType)
{
    int index;

    stsOutputAlarmTable.tip[0].value.uint_val = stsOutputAlarmSettingShelf;
    stsOutputAlarmTable.tip[1].value.uint_val = stsOutputAlarmSettingSlot;
    stsOutputAlarmTable.tip[2].value.uint_val = stsOutputAlarmSettingPort;
    stsOutputAlarmTable.tip[3].value.uint_val = stsOutputAlarmSettingType;
    if ((index = SearchTable(&stsOutputAlarmTable, searchType)) == -1) {
        return NULL;
    }

    return (stsOutputAlarmEntry_t *) stsOutputAlarmTable.tp[index];

}

#ifdef SETS
int
k_stsOutputAlarmEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsOutputAlarmEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsOutputAlarmEntry_set_defaults(doList_t *dp)
{
    stsOutputAlarmEntry_t *data = (stsOutputAlarmEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsOutputAlarmEntry_set(stsOutputAlarmEntry_t *data,
                          ContextInfo *contextInfo, int function)
{

    int index;
    stsOutputAlarmEntry_t *newrow = NULL;

    /* find this entry in the table */
    stsOutputAlarmTable.tip[0].value.uint_val = data->stsOutputAlarmSettingShelf;
    stsOutputAlarmTable.tip[1].value.uint_val = data->stsOutputAlarmSettingSlot;
    stsOutputAlarmTable.tip[2].value.uint_val = data->stsOutputAlarmSettingPort;
    stsOutputAlarmTable.tip[3].value.uint_val = data->stsOutputAlarmSettingType;
    if ((index = SearchTable(&stsOutputAlarmTable, EXACT)) != -1) {
        newrow = (stsOutputAlarmEntry_t *) stsOutputAlarmTable.tp[index];
    }
    else {
        return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_stsOutputAlarmSeverity, stsOutputAlarmEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);


    return NO_ERROR;

}

#ifdef SR_stsOutputAlarmEntry_UNDO
/* add #define SR_stsOutputAlarmEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsOutputAlarmEntry family.
 */
int
stsOutputAlarmEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsOutputAlarmEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsOutputPortSettingTable */
void
DeletestsOutputPortSettingEntry (int index)
{
    stsOutputPortSettingEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsOutputPortSettingEntry_t *) stsOutputPortSettingTable.tp[index];

    /* free the old entry and remove it from the table */
  
    RemoveTableEntry(&stsOutputPortSettingTable, index);
}

stsOutputPortSettingEntry_t *
k_stsOutputPortSettingEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_INT32 stsOutputPortSettingShelf,
                                SR_INT32 stsOutputPortSettingSlot,
                                SR_INT32 stsOutputPortSettingPort)
{
   static stsOutputPortSettingEntry_t stsOutputPortSettingEntryData;
   int ret;


   if(stsOutputPortSettingShelf > 5 || stsOutputPortSettingShelf < 1)
	       return NULL;
   if(stsOutputPortSettingSlot > 16 || stsOutputPortSettingSlot < 1)
   	       return NULL;
   if(stsOutputPortSettingPort > 8 || stsOutputPortSettingPort < 1)
   	       return NULL;

   stsOutputPortSettingEntryData.stsOutputPortSettingShelf = stsOutputPortSettingShelf;
   stsOutputPortSettingEntryData.stsOutputPortSettingSlot = stsOutputPortSettingSlot;
   stsOutputPortSettingEntryData.stsOutputPortSettingPort = stsOutputPortSettingPort;

   /*
    * put your code to retrieve the information here
    */


   ret = smOutputPortGet(&stsOutputPortSettingEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsOutputPortSettingEntryData.valid);
   else
      SET_ALL_VALID(stsOutputPortSettingEntryData.valid);

   return(&stsOutputPortSettingEntryData);
 

}

#ifdef SETS
int
k_stsOutputPortSettingEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsOutputPortSettingEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                  doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsOutputPortSettingEntry_set_defaults(doList_t *dp)
{
    stsOutputPortSettingEntry_t *data = (stsOutputPortSettingEntry_t *) (dp->data);

    if ((data->stsOutputPortSettingName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsOutputPortSettingEntry_set(stsOutputPortSettingEntry_t *data,
                                ContextInfo *contextInfo, int function)
{

    int ret;

 
    if ((ret = smOutputPortSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

    return NO_ERROR;


}

#ifdef SR_stsOutputPortSettingEntry_UNDO
/* add #define SR_stsOutputPortSettingEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsOutputPortSettingEntry family.
 */
int
stsOutputPortSettingEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsOutputPortSettingEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsOutputTiuSettingTable */
void
DeletestsOutputTiuSettingEntry (int index)
{
    stsOutputTiuSettingEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsOutputTiuSettingEntry_t *) stsOutputTiuSettingTable.tp[index];

    /* free the old entry and remove it from the table */
    
    RemoveTableEntry(&stsOutputTiuSettingTable, index);
}

stsOutputTiuSettingEntry_t *
k_stsOutputTiuSettingEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 stsOutputTiuSettingShelf,
                               SR_INT32 stsOutputTiuSettingSlot,
                               SR_INT32 stsOutputTiuSettingPort)
{
    int index;

    stsOutputTiuSettingTable.tip[0].value.uint_val = stsOutputTiuSettingShelf;
    stsOutputTiuSettingTable.tip[1].value.uint_val = stsOutputTiuSettingSlot;
    stsOutputTiuSettingTable.tip[2].value.uint_val = stsOutputTiuSettingPort;
    if ((index = SearchTable(&stsOutputTiuSettingTable, searchType)) == -1) {
        return NULL;
    }

    return (stsOutputTiuSettingEntry_t *) stsOutputTiuSettingTable.tp[index];

}

#ifdef SETS
int
k_stsOutputTiuSettingEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsOutputTiuSettingEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsOutputTiuSettingEntry_set_defaults(doList_t *dp)
{
    stsOutputTiuSettingEntry_t *data = (stsOutputTiuSettingEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsOutputTiuSettingEntry_set(stsOutputTiuSettingEntry_t *data,
                               ContextInfo *contextInfo, int function)
{

    int index;
    stsOutputTiuSettingEntry_t *newrow = NULL;

    /* find this entry in the table */
    stsOutputTiuSettingTable.tip[0].value.uint_val = data->stsOutputTiuSettingShelf;
    stsOutputTiuSettingTable.tip[1].value.uint_val = data->stsOutputTiuSettingSlot;
    stsOutputTiuSettingTable.tip[2].value.uint_val = data->stsOutputTiuSettingPort;
    if ((index = SearchTable(&stsOutputTiuSettingTable, EXACT)) != -1) {
        newrow = (stsOutputTiuSettingEntry_t *) stsOutputTiuSettingTable.tp[index];
    }
    else {
        return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_stsOutputTiuSettingTermination, stsOutputTiuSettingEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);


    return NO_ERROR;

}

#ifdef SR_stsOutputTiuSettingEntry_UNDO
/* add #define SR_stsOutputTiuSettingEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsOutputTiuSettingEntry family.
 */
int
stsOutputTiuSettingEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsOutputTiuSettingEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsPerfMonTable */
void
DeletestsPerfMonEntry (int index)
{
    stsPerfMonEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsPerfMonEntry_t *) stsPerfMonTable.tp[index];

    /* free the old entry and remove it from the table */
   
    RemoveTableEntry(&stsPerfMonTable, index);
}

stsPerfMonEntry_t *
k_stsPerfMonEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_INT32 stsPerfMonInputCurrentMon,
                      SR_INT32 stsPerfMonInputMonPeriod,
                      SR_INT32 stsPerfMonInputMonBucket,
                      SR_INT32 stsPerfMonInputMonType)
{
   static stsPerfMonEntry_t stsPerfMonEntryData;
   int ret;

   if(stsPerfMonInputCurrentMon > 5 || stsPerfMonInputCurrentMon < 1)
	       return NULL;
   if(stsPerfMonInputMonPeriod > 3 || stsPerfMonInputMonPeriod < 1)
   	       return NULL;
   if(stsPerfMonInputMonBucket > 72 || stsPerfMonInputMonBucket < 0)
   	       return NULL;
   if(stsPerfMonInputMonType > 8 || stsPerfMonInputMonType < 1)
   	       return NULL;

   stsPerfMonEntryData.stsPerfMonInputCurrentMon = stsPerfMonInputCurrentMon;
   stsPerfMonEntryData.stsPerfMonInputMonPeriod = stsPerfMonInputMonPeriod;
   stsPerfMonEntryData.stsPerfMonInputMonBucket = stsPerfMonInputMonBucket;
   stsPerfMonEntryData.stsPerfMonInputMonType = stsPerfMonInputMonType;

   /*
    * put your code to retrieve the information here
    */


   ret = pmPerfMonGet(&stsPerfMonEntryData); 
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsPerfMonEntryData.valid);
   else
      SET_ALL_VALID(stsPerfMonEntryData.valid);

   return(&stsPerfMonEntryData);
 


}

/* This routine deletes an entry from the stsTerminalTable */
void
DeletestsTerminalEntry (int index)
{
    stsTerminalEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsTerminalEntry_t *) stsTerminalTable.tp[index];

    /* free the old entry and remove it from the table */
   
    RemoveTableEntry(&stsTerminalTable, index);
}

stsTerminalEntry_t *
k_stsTerminalEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 stsTerminalIndex)
{
   static stsTerminalEntry_t stsTerminalEntryData;
   int ret;

   stsTerminalEntryData.stsTerminalIndex = stsTerminalIndex;

   /*
    * put your code to retrieve the information here
    */

   ret = termActionFuncGet(&stsTerminalEntryData);
if (kDebug)   
printf("After termActionFuncGet, ret = 0x%x\r\n", ret); 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsTerminalEntryData.valid);
   else
      SET_ALL_VALID(stsTerminalEntryData.valid);

   return(&stsTerminalEntryData);
}

#ifdef SETS
int
k_stsTerminalEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsTerminalEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsTerminalEntry_set_defaults(doList_t *dp)
{
    stsTerminalEntry_t *data = (stsTerminalEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsTerminalEntry_set(stsTerminalEntry_t *data,
                       ContextInfo *contextInfo, int function)
{

    int ret;

 
    if ((ret = termActionFuncSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

  
    return NO_ERROR;
}

#ifdef SR_stsTerminalEntry_UNDO
/* add #define SR_stsTerminalEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsTerminalEntry family.
 */
int
stsTerminalEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsTerminalEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsAlarmStatusTable */
void
DeletestsAlarmStatusEntry (int index)
{
    stsAlarmStatusEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsAlarmStatusEntry_t *) stsAlarmStatusTable.tp[index];

    /* free the old entry and remove it from the table */
   
    RemoveTableEntry(&stsAlarmStatusTable, index);
}

stsAlarmStatusEntry_t *
k_stsAlarmStatusEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 stsAlarmSource)
{
    int index;

    stsAlarmStatusTable.tip[0].value.uint_val = stsAlarmSource;
    if ((index = SearchTable(&stsAlarmStatusTable, searchType)) == -1) {
        return NULL;
    }

    return (stsAlarmStatusEntry_t *) stsAlarmStatusTable.tp[index];

}

/* This routine deletes an entry from the stsOutputCardTable */
void
DeletestsOutputCardEntry (int index)
{
    stsOutputCardEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsOutputCardEntry_t *) stsOutputCardTable.tp[index];

    /* free the old entry and remove it from the table */
   
    RemoveTableEntry(&stsOutputCardTable, index);
}

stsOutputCardEntry_t *
k_stsOutputCardEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 stsOutputShelf,
                         SR_INT32 stsOutputSlot)
{
   static stsOutputCardEntry_t stsOutputCardEntryData;
   int ret;


   if(stsOutputShelf > 5 || stsOutputShelf < 1)
	       return NULL;
   if(stsOutputSlot > 16 || stsOutputSlot < 1)
   	       return NULL;

   stsOutputCardEntryData.stsOutputShelf= stsOutputShelf;

   stsOutputCardEntryData.stsOutputSlot = stsOutputSlot;
   /*
    * put your code to retrieve the information here
    */


   ret = smOutputCardGet(&stsOutputCardEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsOutputCardEntryData.valid);
   else
      SET_ALL_VALID(stsOutputCardEntryData.valid);

   return(&stsOutputCardEntryData);
}

#ifdef SETS
int
k_stsOutputCardEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsOutputCardEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsOutputCardEntry_set_defaults(doList_t *dp)
{
    stsOutputCardEntry_t *data = (stsOutputCardEntry_t *) (dp->data);

    if ((data->stsOutputTimingFormat = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsOutputCardEntry_set(stsOutputCardEntry_t *data,
                         ContextInfo *contextInfo, int function)
{

    int ret;

 
    if ((ret = smOutputCardSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

    return NO_ERROR;

}

#ifdef SR_stsOutputCardEntry_UNDO
/* add #define SR_stsOutputCardEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsOutputCardEntry family.
 */
int
stsOutputCardEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsOutputCardEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsEmmStatusTable */
void
DeletestsEmmStatusEntry (int index)
{
    stsEmmStatusEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsEmmStatusEntry_t *) stsEmmStatusTable.tp[index];

    /* free the old entry and remove it from the table */
   
    RemoveTableEntry(&stsEmmStatusTable, index);
}

stsEmmStatusEntry_t *
k_stsEmmStatusEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_INT32 stsEmmShelf,
                        SR_INT32 stsEmmSensorType)
{
#if 0	
    int index;

    stsEmmStatusTable.tip[0].value.uint_val = stsEmmShelf;
    stsEmmStatusTable.tip[1].value.uint_val = stsEmmSensorType;
    if ((index = SearchTable(&stsEmmStatusTable, searchType)) == -1) {
        return NULL;
    }

    return (stsEmmStatusEntry_t *) stsEmmStatusTable.tp[index];
#endif

   static stsEmmStatusEntry_t stsEmmStatusEntryData;
   int ret;

if (kDebug)
printf("Enter k_stsEmmStatusEntry_get()\n");
   if(stsEmmShelf > 5 || stsEmmShelf < 1)
	       return NULL;
   if(stsEmmSensorType > 5 || stsEmmSensorType < 1)
	       return NULL;
   stsEmmStatusEntryData.stsEmmShelf = stsEmmShelf;
   stsEmmStatusEntryData.stsEmmSensorType = stsEmmSensorType;
   /*
    * put your code to retrieve the information here
    */


   ret = tmpEmmGet(&stsEmmStatusEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsEmmStatusEntryData.valid);
   else
      SET_ALL_VALID(stsEmmStatusEntryData.valid);
if (kDebug)
printf("Exit k_stsEmmStatusEntry_get()\n");
   return(&stsEmmStatusEntryData);    

}

#ifdef SETS
int
k_stsEmmStatusEntry_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsEmmStatusEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsEmmStatusEntry_set_defaults(doList_t *dp)
{
    stsEmmStatusEntry_t *data = (stsEmmStatusEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsEmmStatusEntry_set(stsEmmStatusEntry_t *data,
                        ContextInfo *contextInfo, int function)
{
#if 0
    int index;
    stsEmmStatusEntry_t *newrow = NULL;

    /* find this entry in the table */
    stsEmmStatusTable.tip[0].value.uint_val = data->stsEmmShelf;
    stsEmmStatusTable.tip[1].value.uint_val = data->stsEmmSensorType;
    if ((index = SearchTable(&stsEmmStatusTable, EXACT)) != -1) {
        newrow = (stsEmmStatusEntry_t *) stsEmmStatusTable.tp[index];
    }
    else {
        return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_stsEmmMinorAlmLoThresh, stsEmmStatusEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);


    return NO_ERROR;
#endif
    int ret;

if (kDebug)
printf("Enter k_stsEmmStatusEntry_set()\n");
    if ((ret = tmpEmmSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);
if (kDebug)
printf("Exit k_stsEmmStatusEntry_set()\n");
    return NO_ERROR;
}

#ifdef SR_stsEmmStatusEntry_UNDO
/* add #define SR_stsEmmStatusEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsEmmStatusEntry family.
 */
int
stsEmmStatusEntry_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsEmmStatusEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsAntTable */
void
DeletestsAntEntry (int index)
{
    stsAntEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsAntEntry_t *) stsAntTable.tp[index];

    /* free the old entry and remove it from the table */
   
    RemoveTableEntry(&stsAntTable, index);
}

stsAntEntry_t *
k_stsAntEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 stsAntIndex)
{
#if 0	
    int index;

    stsAntTable.tip[0].value.uint_val = stsAntIndex;
    if ((index = SearchTable(&stsAntTable, searchType)) == -1) {
        return NULL;
    }

    return (stsAntEntry_t *) stsAntTable.tp[index];
#endif
   static stsAntEntry_t stsAntEntryData;
   int ret;

if (kDebug)   
printf("Enter k_stsAntEntry_get()\n");
   if(stsAntIndex > 2 || stsAntIndex < 1)
	       return NULL;

   stsAntEntryData.stsAntIndex = stsAntIndex;

   /*
    * put your code to retrieve the information here
    */

   ret = antGet(&stsAntEntryData);
 
   if (ret != OK)
      return NULL;
 
 
   if (nominator == -1)
      ZERO_VALID(stsAntEntryData.valid);
   else
      SET_ALL_VALID(stsAntEntryData.valid);
if (kDebug)      
printf("Exit k_stsAntEntry_get()\n");
   return(&stsAntEntryData);
}

#ifdef SETS
int
k_stsAntEntry_test(ObjectInfo *object, ObjectSyntax *value,
                   doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsAntEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                    doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsAntEntry_set_defaults(doList_t *dp)
{
    stsAntEntry_t *data = (stsAntEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsAntEntry_set(stsAntEntry_t *data,
                  ContextInfo *contextInfo, int function)
{
#if 0
    int index;
    stsAntEntry_t *newrow = NULL;

    /* find this entry in the table */
    stsAntTable.tip[0].value.uint_val = data->stsAntIndex;
    if ((index = SearchTable(&stsAntTable, EXACT)) != -1) {
        newrow = (stsAntEntry_t *) stsAntTable.tp[index];
    }
    else {
        return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_stsAntSwitchState, stsAntEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);


    return NO_ERROR;
#endif
    int ret;
    
if (kDebug)
printf("Enter k_stsAntEntry_set()\n");
    if ((ret = antSet(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);
if (kDebug)
printf("Exit k_stsAntEntry_set()\n");
    return NO_ERROR;    

}

#ifdef SR_stsAntEntry_UNDO
/* add #define SR_stsAntEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsAntEntry family.
 */
int
stsAntEntry_undo(doList_t *doHead, doList_t *doCur,
                 ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsAntEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsTopDestTable */
void
DeletestsTopDestEntry (int index)
{
    stsTopDestEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsTopDestEntry_t *) stsTopDestTable.tp[index];

    /* free the old entry and remove it from the table */
 
    RemoveTableEntry(&stsTopDestTable, index);
}

stsTopDestEntry_t *
k_stsTopDestEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_INT32 stsTopDestNumber)
{
    int index;

    stsTopDestTable.tip[0].value.uint_val = stsTopDestNumber;
    if ((index = SearchTable(&stsTopDestTable, searchType)) == -1) {
        return NULL;
    }

    return (stsTopDestEntry_t *) stsTopDestTable.tp[index];

}

#ifdef SETS
int
k_stsTopDestEntry_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsTopDestEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsTopDestEntry_set_defaults(doList_t *dp)
{
    stsTopDestEntry_t *data = (stsTopDestEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsTopDestEntry_set(stsTopDestEntry_t *data,
                      ContextInfo *contextInfo, int function)
{

    int index;
    stsTopDestEntry_t *newrow = NULL;

    /* find this entry in the table */
    stsTopDestTable.tip[0].value.uint_val = data->stsTopDestNumber;
    if ((index = SearchTable(&stsTopDestTable, EXACT)) != -1) {
        newrow = (stsTopDestEntry_t *) stsTopDestTable.tp[index];
    }
    else {
        return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_stsTopDestVlanId, stsTopDestEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);


    return NO_ERROR;

}

#ifdef SR_stsTopDestEntry_UNDO
/* add #define SR_stsTopDestEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsTopDestEntry family.
 */
int
stsTopDestEntry_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsTopDestEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsTrapRegTable */
void
DeletestsTrapRegEntry (int index)
{
    stsTrapRegEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsTrapRegEntry_t *) stsTrapRegTable.tp[index];

    /* free the old entry and remove it from the table */
    
    RemoveTableEntry(&stsTrapRegTable, index);
}

stsTrapRegEntry_t *
k_stsTrapRegEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_INT32 stsTrapRegMgrNumber)
{
   static stsTrapRegEntry_t stsTrapRegEntryData;
   int nextIndex;
   int index, ip, i;

if (kDebug)
printf("k_stsTrapRegEntry_get Entry - stsTrapRegMgrNumber: %d\n", stsTrapRegMgrNumber);

   if(stsTrapRegMgrNumber > 10 || stsTrapRegMgrNumber < 1)
   {
if (kDebug)
printf("k_stsTrapRegEntry_get - Wrong Number: %d\n", stsTrapRegMgrNumber);
	       return NULL;
   }

   index = stsTrapRegMgrNumber - 1;

   if (searchType == EXACT){
if (kDebug)
printf("k_stsTrapRegEntry_get - EXACT\n");	   

      if(stsTrapRegData[index].stsTrapRegMgrIp == 0)
	  {
if (kDebug)
printf("k_stsTrapRegEntry_get - stsTrapRegMgrIp: %d\n", stsTrapRegData[index].stsTrapRegMgrIp);

          return NULL;
	  }

	   memcpy(&stsTrapRegEntryData, &stsTrapRegData[index], sizeof(stsTrapRegEntry_t));
   }
   else
   {
if (kDebug)
printf("k_stsTrapRegEntry_get - NOT EXACT\n");

      ip = stsTrapRegData[index].stsTrapRegMgrIp;

	  for(i = index; i < 10 && ip == 0; i++)
	  {
         ip = stsTrapRegData[i].stsTrapRegMgrIp;

	  }
           	  
      if( i > 9)
	  {
if (kDebug)
printf("k_stsTrapRegEntry_get - index is more than 9\n");

		  return NULL;
	  }

	  memcpy(&stsTrapRegEntryData, &stsTrapRegData[i], sizeof(stsTrapRegEntry_t));
      nextIndex = i + 1;
	  stsTrapRegEntryData.stsTrapRegMgrNumber = nextIndex;
   }

   if (nominator == -1)
      ZERO_VALID(stsTrapRegEntryData.valid);
   else
      SET_ALL_VALID(stsTrapRegEntryData.valid);

if (kDebug)
printf("k_stsTrapRegEntry_get - stsTrapRegMgrNumber: %d\n", stsTrapRegEntryData.stsTrapRegMgrNumber);

   return(&stsTrapRegEntryData);
}


#ifdef SETS
int
k_stsTrapRegEntry_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsTrapRegEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsTrapRegEntry_set_defaults(doList_t *dp)
{
    stsTrapRegEntry_t *data = (stsTrapRegEntry_t *) (dp->data);


    if ((data->stsTrapRegCommunityStr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
setTrapRegMgrDefaults()
{
    int i;
	FILE *fhd = NULL;

    if ((fhd = fopen("trapip.dat", "r")) == NULL)
    {
	   for (i=0; i < 10; i++)
       {
	       stsTrapRegData[i].stsTrapRegMgrNumber = i + 1;
           stsTrapRegData[i].stsTrapRegState = 2;
           stsTrapRegData[i].stsTrapRegMgrIp = 0;
		   memset(&communityStr[i][0], 0, 30);
		   communityPtr[i].length = 0;
	       communityPtr[i].octet_ptr = &communityStr[i][0];
		   stsTrapRegData[i].stsTrapRegCommunityStr = &communityPtr[i];
       }
	   if ((fhd = fopen("trapip.dat", "w")) == NULL)
		   return ERROR;
	   else
	   {
		   fwrite((char *)&stsTrapRegData[0], sizeof(stsTrapRegEntry_t)*10, 1, fhd);
		   fwrite(&communityStr[0][0], 300, 1, fhd);
		   fwrite(&communityPtr[0], sizeof(OctetString)*10, 1, fhd);
		   fclose(fhd);
	   }
	}
	else
	{
		fread((char *)&stsTrapRegData[0], sizeof(stsTrapRegEntry_t)*10, 1, fhd);
		fread(&communityStr[0][0], 300, 1, fhd);
		fread(&communityPtr[0], sizeof(OctetString)*10, 1, fhd);
		fclose(fhd);
		for (i=0; i < 10; i++)
        {
			communityPtr[i].octet_ptr = &communityStr[i][0];
			stsTrapRegData[i].stsTrapRegCommunityStr = &communityPtr[i];
		}
	}
   
    return OK;
}

int
k_stsTrapRegEntry_set(stsTrapRegEntry_t *data,
                      ContextInfo *contextInfo, int function)
{
     int ret;
     char prtString[80];
     int trapIndex;
	 FILE *fhd = NULL;

if (kDebug)
printf("k_stsTrapRegEntry_set - stsTrapRegMgrNumber: %d, valid: %x, stsTrapRegMgrIp: %x\n", 
data->stsTrapRegMgrNumber, data->valid, data->stsTrapRegMgrIp);


     trapIndex = data->stsTrapRegMgrNumber - 1;
     if(trapIndex < 0 || trapIndex > 9)
        return (GEN_ERROR);
	 else
	    stsTrapRegData[trapIndex].stsTrapRegMgrNumber = data->stsTrapRegMgrNumber;

	if (VALID(I_stsTrapRegMgrUdpPort, data->valid)) {
	   stsTrapRegData[trapIndex].stsTrapRegMgrUdpPort = 
	      data->stsTrapRegMgrUdpPort;
	}	
	if (VALID(I_stsTrapRegState, data->valid)) {
	   stsTrapRegData[trapIndex].stsTrapRegState = 
	      data->stsTrapRegState;
	}

    if (VALID(I_stsTrapRegMgrIp, data->valid)) {
         if(data->stsTrapRegMgrIp == 0)
            stsTrapRegData[trapIndex].stsTrapRegState = 2; /* disable */
		 else
		    stsTrapRegData[trapIndex].stsTrapRegState = 1; /* enable */
	     
	     stsTrapRegData[trapIndex].stsTrapRegMgrIp = data->stsTrapRegMgrIp;
	} 
	if (VALID(I_stsTrapRegCommunityStr, data->valid)) {
        if (data->stsTrapRegCommunityStr->length > 29)
		{
		    strncpy(&communityStr[trapIndex][0], data->stsTrapRegCommunityStr->octet_ptr, 29);
			communityPtr[trapIndex].octet_ptr = &communityStr[trapIndex][0];
			communityPtr[trapIndex].length = 29;
		}
		else
		{
		    strncpy(&communityStr[trapIndex][0], data->stsTrapRegCommunityStr->octet_ptr,
			                                     data->stsTrapRegCommunityStr->length);
			communityPtr[trapIndex].octet_ptr = &communityStr[trapIndex][0];
			communityPtr[trapIndex].length = data->stsTrapRegCommunityStr->length;
		}
   	}

    /* copy values from the scratch copy to live data */
    SET_ALL_VALID(data->valid);

    /* Save into Flash drive */
    if ((fhd = fopen("trapip.dat", "w")) == NULL)
	{
if (kDebug)
printf("Failed to open trapip.dat for writing\n");

		return ERROR;
	}
	else
	{
		fwrite((char *)&stsTrapRegData[0], sizeof(stsTrapRegEntry_t)*10, 1, fhd);
		fwrite(&communityStr[0][0], 300, 1, fhd);
		fwrite(&communityPtr[0], sizeof(OctetString)*10, 1, fhd);
		fclose(fhd);
	}

    return NO_ERROR;
}


#ifdef SR_stsTrapRegEntry_UNDO
/* add #define SR_stsTrapRegEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsTrapRegEntry family.
 */
int
stsTrapRegEntry_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsTrapRegEntry_UNDO */

#endif /* SETS */

/* This routine deletes an entry from the stsClockPriTable */
void
DeletestsClockPriEntry (int index)
{
    stsClockPriEntry_t *data;

    /* get a pointer to the old entry */
    data = (stsClockPriEntry_t *) stsClockPriTable.tp[index];

    /* free the old entry and remove it from the table */
   
    RemoveTableEntry(&stsClockPriTable, index);
}

stsClockPriEntry_t *
k_stsClockPriEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 stsClockPriIndex)
{

   static stsClockPriEntry_t stsClockPriEntryData;
   int ret;


   stsClockPriEntryData.stsClockPriIndex = stsClockPriIndex;

   ret = CsmClockInputGetReq(&stsClockPriEntryData);

   if(ret != OK)
	   return NULL;

   if (nominator == -1)
      ZERO_VALID(stsClockPriEntryData.valid);
   else
      SET_ALL_VALID(stsClockPriEntryData.valid);

   return (&stsClockPriEntryData);

}

#ifdef SETS
int
k_stsClockPriEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_stsClockPriEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_stsClockPriEntry_set_defaults(doList_t *dp)
{
    stsClockPriEntry_t *data = (stsClockPriEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_stsClockPriEntry_set(stsClockPriEntry_t *data,
                       ContextInfo *contextInfo, int function)
{

    int ret;

 
    if ((ret = CsmClockPriSetReq(data)) != OK) {
             return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
  
    SET_ALL_VALID(data->valid);

    return NO_ERROR;

}

#ifdef SR_stsClockPriEntry_UNDO
/* add #define SR_stsClockPriEntry_UNDO in sitedefs.h to
 * include the undo routine for the stsClockPriEntry family.
 */
int
stsClockPriEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_stsClockPriEntry_UNDO */

#endif /* SETS */

