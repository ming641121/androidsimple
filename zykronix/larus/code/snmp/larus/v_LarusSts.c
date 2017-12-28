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
#endif	/* HAVE_STDLIB_H */

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif	/* HAVE_MALLOC_H */

#ifdef HAVE_STRING_H
#include <string.h>
#endif	/* HAVE_STRING_H */

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif /* HAVE_STDDEF_H */
#include "sr_snmp.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "inst_lib.h"
#include "oid_lib.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME
#include "min_v.h"
#include "mibout.h"
#define SETS /* joehsu */
const SnmpType stsSystemTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemId), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemMgmtIpOption), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemMgmtIpAddress), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemMgmtIpSubnetMask), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemMgmtIpGatewayAddress), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemNtpIpOption), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemNtpIpAddress), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemNtpIpSubnetMask), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemNtpIpGatewayAddress), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemDate), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemTime), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemTimeZone), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsSystem_t, stsSystemIntegratedAlarm), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemAlarmAcoMode), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsSystem_t, stsSystemAlarmAcoLed), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemAlarmAcoCutOff), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemAlarmEscalatingTime), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsSystem_t, stsSystemMinorLED), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsSystem_t, stsSystemMajorLED), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsSystem_t, stsSystemCriticalLED), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemUpgradeSWAction), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(stsSystem_t, stsSystemCurrentSWVersion), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsSystem_t, stsSystemServiceState), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsSystem_t, stsSystemReset), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry stsSystemGetInfo = {
    (SR_KGET_FPTR) new_k_stsSystem_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsSystem_t),
    I_stsSystem_max,
    (SnmpType *) stsSystemTypeTable,
    NULL,
    (short) offsetof(stsSystem_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsSystem family.
 *---------------------------------------------------------------------*/
VarBind *
stsSystem_get(OID *incoming, ObjectInfo *object, int searchType,
              ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsSystemGetInfo));
}

stsSystem_t *
new_k_stsSystem_get(int serialNum, ContextInfo *contextInfo,
                    int nominator, int searchType,
                    stsSystem_t *data)
{
    return k_stsSystem_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsSystem_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsSystem family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  stsSystemId_len[] = { { 1, 24 } };
static RangeTest_t   stsSystemMgmtIpOption_range[] = { { 1, 2 } };
static RangeTest_t   stsSystemNtpIpOption_range[] = { { 1, 2 } };
static LengthTest_t  stsSystemDate_len[] = { { 1, 6 } };
static LengthTest_t  stsSystemTime_len[] = { { 1, 6 } };
static RangeTest_t   stsSystemTimeZone_range[] = { { -48, 52 } };
static RangeTest_t   stsSystemAlarmAcoMode_range[] = { { 1, 2 } };
static RangeTest_t   stsSystemAlarmAcoCutOff_range[] = { { 1, 2 } };
static RangeTest_t   stsSystemAlarmEscalatingTime_range[] = { { 1, 360 } };
static RangeTest_t   stsSystemUpgradeSWAction_range[] = { { 1, 2 } };
static RangeTest_t   stsSystemReset_range[] = { { 1, 4 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsSystem family.
 */
static struct sr_member_test stsSystem_member_test[] =
{
    /* stsSystemId */
    { MINV_LENGTH_TEST, 
      sizeof(stsSystemId_len)/sizeof(LengthTest_t), /* 1 */
      stsSystemId_len, DisplayString_test },

    /* stsSystemMgmtIpOption */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsSystemMgmtIpOption_range)/sizeof(RangeTest_t), /* 2 */
      stsSystemMgmtIpOption_range, NULL },

    /* stsSystemMgmtIpAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsSystemMgmtIpSubnetMask */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsSystemMgmtIpGatewayAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsSystemNtpIpOption */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsSystemNtpIpOption_range)/sizeof(RangeTest_t), /* 2 */
      stsSystemNtpIpOption_range, NULL },

    /* stsSystemNtpIpAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsSystemNtpIpSubnetMask */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsSystemNtpIpGatewayAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsSystemDate */
    { MINV_LENGTH_TEST, 
      sizeof(stsSystemDate_len)/sizeof(LengthTest_t), /* 1 */
      stsSystemDate_len, DisplayString_test },

    /* stsSystemTime */
    { MINV_LENGTH_TEST, 
      sizeof(stsSystemTime_len)/sizeof(LengthTest_t), /* 1 */
      stsSystemTime_len, DisplayString_test },

    /* stsSystemTimeZone */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsSystemTimeZone_range)/sizeof(RangeTest_t), /* 1 */
      stsSystemTimeZone_range, NULL },

    /* stsSystemIntegratedAlarm */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsSystemAlarmAcoMode */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsSystemAlarmAcoMode_range)/sizeof(RangeTest_t), /* 2 */
      stsSystemAlarmAcoMode_range, NULL },

    /* stsSystemAlarmAcoLed */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsSystemAlarmAcoCutOff */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsSystemAlarmAcoCutOff_range)/sizeof(RangeTest_t), /* 2 */
      stsSystemAlarmAcoCutOff_range, NULL },

    /* stsSystemAlarmEscalatingTime */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsSystemAlarmEscalatingTime_range)/sizeof(RangeTest_t), /* 1 */
      stsSystemAlarmEscalatingTime_range, NULL },

    /* stsSystemMinorLED */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsSystemMajorLED */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsSystemCriticalLED */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsSystemUpgradeSWAction */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsSystemUpgradeSWAction_range)/sizeof(RangeTest_t), /* 2 */
      stsSystemUpgradeSWAction_range, NULL },

    /* stsSystemCurrentSWVersion */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsSystemServiceState */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsSystemReset */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsSystemReset_range)/sizeof(RangeTest_t), /* 4 */
      stsSystemReset_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsSystem family.
 */
const SrTestInfoEntry stsSystemTestInfo = {
    &stsSystemGetInfo,
    stsSystem_member_test,
    NULL,
    k_stsSystem_test,
    k_stsSystem_ready,
#ifdef SR_stsSystem_UNDO
    stsSystem_undo,
#else /* SR_stsSystem_UNDO */
    NULL,
#endif /* SR_stsSystem_UNDO */
    stsSystem_ready,
    stsSystem_set,
    stsSystem_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * cleanup after stsSystem set/undo
 *---------------------------------------------------------------------*/
static int
stsSystem_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsSystemTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsSystem family
 *---------------------------------------------------------------------*/
stsSystem_t *
Clone_stsSystem(stsSystem_t *stsSystem)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsSystem,
                         stsSystemGetInfo.family_size,
                         stsSystemGetInfo.type_table,
                         stsSystemGetInfo.highest_nominator,
                         stsSystemGetInfo.valid_offset,
                         stsSystemTestInfo.userpart_clone_func,
                         stsSystemGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsSystem_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
               doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsSystemTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsSystem_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsSystemTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsSystem_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsSystem_set((stsSystem_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


const SnmpType stsClockInputTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsClockInput_t, stsClockInputManualSelect), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsClockInput_t, stsClockInputOperMode), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsClockInput_t, stsClockInputSetSlip), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsClockInput_t, stsClockInputSlipState), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsClockInput_t, stsClockInputSlipAlarmSetting), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsClockInput_t, stsClockInputSlipAlarmSeverity), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsClockInput_t, stsClockInputValue), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry stsClockInputGetInfo = {
    (SR_KGET_FPTR) new_k_stsClockInput_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsClockInput_t),
    I_stsClockInput_max,
    (SnmpType *) stsClockInputTypeTable,
    NULL,
    (short) offsetof(stsClockInput_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsClockInput family.
 *---------------------------------------------------------------------*/
VarBind *
stsClockInput_get(OID *incoming, ObjectInfo *object, int searchType,
                  ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsClockInputGetInfo));
}

stsClockInput_t *
new_k_stsClockInput_get(int serialNum, ContextInfo *contextInfo,
                        int nominator, int searchType,
                        stsClockInput_t *data)
{
    return k_stsClockInput_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsClockInput_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsClockInput family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsClockInputManualSelect_range[] = { { 1, 5 } };
static RangeTest_t   stsClockInputOperMode_range[] = { { 1, 2 } };
static RangeTest_t   stsClockInputSetSlip_range[] = { { 1, 2 } };
static RangeTest_t   stsClockInputSlipAlarmSetting_range[] = { { 1, 5 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsClockInput family.
 */
static struct sr_member_test stsClockInput_member_test[] =
{
    /* stsClockInputManualSelect */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsClockInputManualSelect_range)/sizeof(RangeTest_t), /* 5 */
      stsClockInputManualSelect_range, NULL },

    /* stsClockInputOperMode */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsClockInputOperMode_range)/sizeof(RangeTest_t), /* 2 */
      stsClockInputOperMode_range, NULL },

    /* stsClockInputSetSlip */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsClockInputSetSlip_range)/sizeof(RangeTest_t), /* 2 */
      stsClockInputSetSlip_range, NULL },

    /* stsClockInputSlipState */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsClockInputSlipAlarmSetting */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsClockInputSlipAlarmSetting_range)/sizeof(RangeTest_t), /* 5 */
      stsClockInputSlipAlarmSetting_range, NULL },

    /* stsClockInputSlipAlarmSeverity */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsClockInputValue */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsClockInput family.
 */
const SrTestInfoEntry stsClockInputTestInfo = {
    &stsClockInputGetInfo,
    stsClockInput_member_test,
    NULL,
    k_stsClockInput_test,
    k_stsClockInput_ready,
#ifdef SR_stsClockInput_UNDO
    stsClockInput_undo,
#else /* SR_stsClockInput_UNDO */
    NULL,
#endif /* SR_stsClockInput_UNDO */
    stsClockInput_ready,
    stsClockInput_set,
    stsClockInput_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * cleanup after stsClockInput set/undo
 *---------------------------------------------------------------------*/
static int
stsClockInput_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsClockInputTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsClockInput family
 *---------------------------------------------------------------------*/
stsClockInput_t *
Clone_stsClockInput(stsClockInput_t *stsClockInput)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsClockInput,
                         stsClockInputGetInfo.family_size,
                         stsClockInputGetInfo.type_table,
                         stsClockInputGetInfo.highest_nominator,
                         stsClockInputGetInfo.valid_offset,
                         stsClockInputTestInfo.userpart_clone_func,
                         stsClockInputGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsClockInput_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                   doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsClockInputTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsClockInput_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsClockInputTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsClockInput_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsClockInput_set((stsClockInput_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


const SnmpType stsClockInputSSMTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsClockInputSSM_t, stsClockInputSSMSelect), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry stsClockInputSSMGetInfo = {
    (SR_KGET_FPTR) new_k_stsClockInputSSM_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsClockInputSSM_t),
    I_stsClockInputSSM_max,
    (SnmpType *) stsClockInputSSMTypeTable,
    NULL,
    (short) offsetof(stsClockInputSSM_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsClockInputSSM family.
 *---------------------------------------------------------------------*/
VarBind *
stsClockInputSSM_get(OID *incoming, ObjectInfo *object, int searchType,
                     ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsClockInputSSMGetInfo));
}

stsClockInputSSM_t *
new_k_stsClockInputSSM_get(int serialNum, ContextInfo *contextInfo,
                           int nominator, int searchType,
                           stsClockInputSSM_t *data)
{
    return k_stsClockInputSSM_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsClockInputSSM_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsClockInputSSM family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsClockInputSSMSelect_range[] = { { 1, 6 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsClockInputSSM family.
 */
static struct sr_member_test stsClockInputSSM_member_test[] =
{
    /* stsClockInputSSMSelect */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsClockInputSSMSelect_range)/sizeof(RangeTest_t), /* 6 */
      stsClockInputSSMSelect_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsClockInputSSM family.
 */
const SrTestInfoEntry stsClockInputSSMTestInfo = {
    &stsClockInputSSMGetInfo,
    stsClockInputSSM_member_test,
    NULL,
    k_stsClockInputSSM_test,
    k_stsClockInputSSM_ready,
#ifdef SR_stsClockInputSSM_UNDO
    stsClockInputSSM_undo,
#else /* SR_stsClockInputSSM_UNDO */
    NULL,
#endif /* SR_stsClockInputSSM_UNDO */
    stsClockInputSSM_ready,
    stsClockInputSSM_set,
    stsClockInputSSM_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * cleanup after stsClockInputSSM set/undo
 *---------------------------------------------------------------------*/
static int
stsClockInputSSM_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsClockInputSSMTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsClockInputSSM family
 *---------------------------------------------------------------------*/
stsClockInputSSM_t *
Clone_stsClockInputSSM(stsClockInputSSM_t *stsClockInputSSM)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsClockInputSSM,
                         stsClockInputSSMGetInfo.family_size,
                         stsClockInputSSMGetInfo.type_table,
                         stsClockInputSSMGetInfo.highest_nominator,
                         stsClockInputSSMGetInfo.valid_offset,
                         stsClockInputSSMTestInfo.userpart_clone_func,
                         stsClockInputSSMGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsClockInputSSM_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                      doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsClockInputSSMTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsClockInputSSM_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsClockInputSSMTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsClockInputSSM_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsClockInputSSM_set((stsClockInputSSM_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


const SnmpType stsTopTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTop_t, stsTopSourceIpOption), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsTop_t, stsTopSourceIpAddress), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsTop_t, stsTopSourceIpSubnetMask), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(stsTop_t, stsTopSourceIpGatewayAddress), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTop_t, stsTopPacketRate), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsTop_t, stsTopInputSelect), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsTop_t, stsTopTxPackets), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(stsTop_t, stsTopTxBytes), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry stsTopGetInfo = {
    (SR_KGET_FPTR) new_k_stsTop_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsTop_t),
    I_stsTop_max,
    (SnmpType *) stsTopTypeTable,
    NULL,
    (short) offsetof(stsTop_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsTop family.
 *---------------------------------------------------------------------*/
VarBind *
stsTop_get(OID *incoming, ObjectInfo *object, int searchType,
           ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsTopGetInfo));
}

stsTop_t *
new_k_stsTop_get(int serialNum, ContextInfo *contextInfo,
                 int nominator, int searchType,
                 stsTop_t *data)
{
    return k_stsTop_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsTop_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsTop family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsTopSourceIpOption_range[] = { { 1, 2 } };
static RangeTest_t   stsTopPacketRate_range[] = { { 10, 1000 } };
static RangeTest_t   stsTopInputSelect_range[] = { { 1, 3 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsTop family.
 */
static struct sr_member_test stsTop_member_test[] =
{
    /* stsTopSourceIpOption */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTopSourceIpOption_range)/sizeof(RangeTest_t), /* 2 */
      stsTopSourceIpOption_range, NULL },

    /* stsTopSourceIpAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsTopSourceIpSubnetMask */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsTopSourceIpGatewayAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsTopPacketRate */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTopPacketRate_range)/sizeof(RangeTest_t), /* 1 */
      stsTopPacketRate_range, NULL },

    /* stsTopInputSelect */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTopInputSelect_range)/sizeof(RangeTest_t), /* 3 */
      stsTopInputSelect_range, NULL },

    /* stsTopTxPackets */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsTopTxBytes */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsTop family.
 */
const SrTestInfoEntry stsTopTestInfo = {
    &stsTopGetInfo,
    stsTop_member_test,
    NULL,
    k_stsTop_test,
    k_stsTop_ready,
#ifdef SR_stsTop_UNDO
    stsTop_undo,
#else /* SR_stsTop_UNDO */
    NULL,
#endif /* SR_stsTop_UNDO */
    stsTop_ready,
    stsTop_set,
    stsTop_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * cleanup after stsTop set/undo
 *---------------------------------------------------------------------*/
static int
stsTop_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsTopTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsTop family
 *---------------------------------------------------------------------*/
stsTop_t *
Clone_stsTop(stsTop_t *stsTop)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsTop,
                         stsTopGetInfo.family_size,
                         stsTopGetInfo.type_table,
                         stsTopGetInfo.highest_nominator,
                         stsTopGetInfo.valid_offset,
                         stsTopTestInfo.userpart_clone_func,
                         stsTopGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsTop_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
            doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsTopTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsTop_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsTopTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsTop_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsTop_set((stsTop_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


const SnmpType stsPerfMonManualTypeTable[] = {
    { BITS_TYPE, SR_READ_WRITE, offsetof(stsPerfMonManual_t, stsPerfMonManualInputSelect), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsPerfMonManual_t, stsPerfMonManualInputScanInterval), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsPerfMonManual_t, stsPerfMonManualInputNumReport), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsPerfMonManual_t, stsPerfMonManualInputNumObservation), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(stsPerfMonManual_t, stsPerfMonManualInputAbortTest), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry stsPerfMonManualGetInfo = {
    (SR_KGET_FPTR) new_k_stsPerfMonManual_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsPerfMonManual_t),
    I_stsPerfMonManual_max,
    (SnmpType *) stsPerfMonManualTypeTable,
    NULL,
    (short) offsetof(stsPerfMonManual_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsPerfMonManual family.
 *---------------------------------------------------------------------*/
VarBind *
stsPerfMonManual_get(OID *incoming, ObjectInfo *object, int searchType,
                     ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsPerfMonManualGetInfo));
}

stsPerfMonManual_t *
new_k_stsPerfMonManual_get(int serialNum, ContextInfo *contextInfo,
                           int nominator, int searchType,
                           stsPerfMonManual_t *data)
{
    return k_stsPerfMonManual_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsPerfMonManual_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsPerfMonManual family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static BitsTest_t    stsPerfMonManualInputSelect_bits = 4;
static RangeTest_t   stsPerfMonManualInputScanInterval_range[] = { { 1, 6 } };
static RangeTest_t   stsPerfMonManualInputNumReport_range[] = { { 1, 100 } };
static RangeTest_t   stsPerfMonManualInputNumObservation_range[] = { { 1, 100 } };
static RangeTest_t   stsPerfMonManualInputAbortTest_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsPerfMonManual family.
 */
static struct sr_member_test stsPerfMonManual_member_test[] =
{
    /* stsPerfMonManualInputSelect */
    { MINV_BITS_TEST, 1, &stsPerfMonManualInputSelect_bits, NULL },

    /* stsPerfMonManualInputScanInterval */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsPerfMonManualInputScanInterval_range)/sizeof(RangeTest_t), /* 6 */
      stsPerfMonManualInputScanInterval_range, NULL },

    /* stsPerfMonManualInputNumReport */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsPerfMonManualInputNumReport_range)/sizeof(RangeTest_t), /* 1 */
      stsPerfMonManualInputNumReport_range, NULL },

    /* stsPerfMonManualInputNumObservation */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsPerfMonManualInputNumObservation_range)/sizeof(RangeTest_t), /* 1 */
      stsPerfMonManualInputNumObservation_range, NULL },

    /* stsPerfMonManualInputAbortTest */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsPerfMonManualInputAbortTest_range)/sizeof(RangeTest_t), /* 2 */
      stsPerfMonManualInputAbortTest_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsPerfMonManual family.
 */
const SrTestInfoEntry stsPerfMonManualTestInfo = {
    &stsPerfMonManualGetInfo,
    stsPerfMonManual_member_test,
    NULL,
    k_stsPerfMonManual_test,
    k_stsPerfMonManual_ready,
#ifdef SR_stsPerfMonManual_UNDO
    stsPerfMonManual_undo,
#else /* SR_stsPerfMonManual_UNDO */
    NULL,
#endif /* SR_stsPerfMonManual_UNDO */
    stsPerfMonManual_ready,
    stsPerfMonManual_set,
    stsPerfMonManual_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * cleanup after stsPerfMonManual set/undo
 *---------------------------------------------------------------------*/
static int
stsPerfMonManual_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsPerfMonManualTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsPerfMonManual family
 *---------------------------------------------------------------------*/
stsPerfMonManual_t *
Clone_stsPerfMonManual(stsPerfMonManual_t *stsPerfMonManual)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsPerfMonManual,
                         stsPerfMonManualGetInfo.family_size,
                         stsPerfMonManualGetInfo.type_table,
                         stsPerfMonManualGetInfo.highest_nominator,
                         stsPerfMonManualGetInfo.valid_offset,
                         stsPerfMonManualTestInfo.userpart_clone_func,
                         stsPerfMonManualGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsPerfMonManual_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                      doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsPerfMonManualTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsPerfMonManual_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsPerfMonManualTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsPerfMonManual_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsPerfMonManual_set((stsPerfMonManual_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


const SnmpType stsCommStrTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsCommStr_t, stsReadOnlyCommStr), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(stsCommStr_t, stsReadWriteCommStr), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry stsCommStrGetInfo = {
    (SR_KGET_FPTR) new_k_stsCommStr_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsCommStr_t),
    I_stsCommStr_max,
    (SnmpType *) stsCommStrTypeTable,
    NULL,
    (short) offsetof(stsCommStr_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsCommStr family.
 *---------------------------------------------------------------------*/
VarBind *
stsCommStr_get(OID *incoming, ObjectInfo *object, int searchType,
               ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsCommStrGetInfo));
}

stsCommStr_t *
new_k_stsCommStr_get(int serialNum, ContextInfo *contextInfo,
                     int nominator, int searchType,
                     stsCommStr_t *data)
{
    return k_stsCommStr_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsCommStr_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsCommStr family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  stsReadOnlyCommStr_len[] = { { 1, 40 } };
static LengthTest_t  stsReadWriteCommStr_len[] = { { 1, 40 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsCommStr family.
 */
static struct sr_member_test stsCommStr_member_test[] =
{
    /* stsReadOnlyCommStr */
    { MINV_LENGTH_TEST, 
      sizeof(stsReadOnlyCommStr_len)/sizeof(LengthTest_t), /* 1 */
      stsReadOnlyCommStr_len, DisplayString_test },

    /* stsReadWriteCommStr */
    { MINV_LENGTH_TEST, 
      sizeof(stsReadWriteCommStr_len)/sizeof(LengthTest_t), /* 1 */
      stsReadWriteCommStr_len, DisplayString_test }
};

/*
 * Initialize SrTestInfoEntry for the stsCommStr family.
 */
const SrTestInfoEntry stsCommStrTestInfo = {
    &stsCommStrGetInfo,
    stsCommStr_member_test,
    NULL,
    k_stsCommStr_test,
    k_stsCommStr_ready,
#ifdef SR_stsCommStr_UNDO
    stsCommStr_undo,
#else /* SR_stsCommStr_UNDO */
    NULL,
#endif /* SR_stsCommStr_UNDO */
    stsCommStr_ready,
    stsCommStr_set,
    stsCommStr_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * cleanup after stsCommStr set/undo
 *---------------------------------------------------------------------*/
static int
stsCommStr_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsCommStrTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsCommStr family
 *---------------------------------------------------------------------*/
stsCommStr_t *
Clone_stsCommStr(stsCommStr_t *stsCommStr)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsCommStr,
                         stsCommStrGetInfo.family_size,
                         stsCommStrGetInfo.type_table,
                         stsCommStrGetInfo.highest_nominator,
                         stsCommStrGetInfo.valid_offset,
                         stsCommStrTestInfo.userpart_clone_func,
                         stsCommStrGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsCommStr_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsCommStrTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsCommStr_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsCommStrTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsCommStr_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsCommStr_set((stsCommStr_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsClockInputStatusEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsClockInputStatusEntryTypeTable[];

const SrIndexInfo stsClockInputStatusEntryIndexInfo[] = {
#ifdef I_stsClockInputStatusIndex
    { I_stsClockInputStatusIndex, T_uint, -1 },
#endif /* I_stsClockInputStatusIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsClockInputStatusEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsClockInputStatusEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsClockInputStatusEntry_t),
    I_stsClockInputStatusEntry_max,
    (SnmpType *) stsClockInputStatusEntryTypeTable,
    stsClockInputStatusEntryIndexInfo,
    (short) offsetof(stsClockInputStatusEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsClockInputStatusEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsClockInputStatusEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                             ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsClockInputStatusIndex)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsClockInputStatusEntryGetInfo));
#endif /* all indices are supported */
}

stsClockInputStatusEntry_t *
new_k_stsClockInputStatusEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator, int searchType,
                                   stsClockInputStatusEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsClockInputStatusEntry_get(serialNum, contextInfo, nominator,
                                          searchType,
                                          data->stsClockInputStatusIndex);
}

/*----------------------------------------------------------------------
 * Free the stsClockInputStatusEntry data object.
 *---------------------------------------------------------------------*/
void
stsClockInputStatusEntry_free(stsClockInputStatusEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsClockInputStatusEntryGetInfo.type_table,
                 stsClockInputStatusEntryGetInfo.highest_nominator,
                 stsClockInputStatusEntryGetInfo.valid_offset,
                 stsClockInputStatusEntryGetInfo.userpart_free_func);
}

/*
 * The stsClockInputSSMEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsClockInputSSMEntryTypeTable[];

const SrIndexInfo stsClockInputSSMEntryIndexInfo[] = {
#ifdef I_stsClockInputSSMIndex
    { I_stsClockInputSSMIndex, T_uint, -1 },
#endif /* I_stsClockInputSSMIndex */
#ifdef I_stsClockInputSSMInputSelect
    { I_stsClockInputSSMInputSelect, T_uint, -1 },
#endif /* I_stsClockInputSSMInputSelect */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsClockInputSSMEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsClockInputSSMEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsClockInputSSMEntry_t),
    I_stsClockInputSSMEntry_max,
    (SnmpType *) stsClockInputSSMEntryTypeTable,
    stsClockInputSSMEntryIndexInfo,
    (short) offsetof(stsClockInputSSMEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsClockInputSSMEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsClockInputSSMEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                          ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsClockInputSSMIndex) || !defined(I_stsClockInputSSMInputSelect)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsClockInputSSMEntryGetInfo));
#endif /* all indices are supported */
}

stsClockInputSSMEntry_t *
new_k_stsClockInputSSMEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator, int searchType,
                                stsClockInputSSMEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsClockInputSSMEntry_get(serialNum, contextInfo, nominator,
                                       searchType,
                                       data->stsClockInputSSMIndex,
                                       data->stsClockInputSSMInputSelect);
}

/*----------------------------------------------------------------------
 * Free the stsClockInputSSMEntry data object.
 *---------------------------------------------------------------------*/
void
stsClockInputSSMEntry_free(stsClockInputSSMEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsClockInputSSMEntryGetInfo.type_table,
                 stsClockInputSSMEntryGetInfo.highest_nominator,
                 stsClockInputSSMEntryGetInfo.valid_offset,
                 stsClockInputSSMEntryGetInfo.userpart_free_func);
}

/*
 * The stsCardEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsCardEntryTypeTable[];

const SrIndexInfo stsCardEntryIndexInfo[] = {
#ifdef I_stsCardShelf
    { I_stsCardShelf, T_uint, -1 },
#endif /* I_stsCardShelf */
#ifdef I_stsCardIndex
    { I_stsCardIndex, T_uint, -1 },
#endif /* I_stsCardIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsCardEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsCardEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsCardEntry_t),
    I_stsCardEntry_max,
    (SnmpType *) stsCardEntryTypeTable,
    stsCardEntryIndexInfo,
    (short) offsetof(stsCardEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsCardEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsCardEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                 ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsCardShelf) || !defined(I_stsCardIndex)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsCardEntryGetInfo));
#endif /* all indices are supported */
}

stsCardEntry_t *
new_k_stsCardEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator, int searchType,
                       stsCardEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsCardEntry_get(serialNum, contextInfo, nominator,
                              searchType, data->stsCardShelf,
                              data->stsCardIndex);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsCardEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsCardEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsCardShelf_range[] = { { 1, 5 } };
static RangeTest_t   stsCardIndex_range[] = { { 1, 23 } };
static RangeTest_t   stsCardReset_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsCardEntry family.
 */
static struct sr_member_test stsCardEntry_member_test[] =
{
    /* stsCardShelf */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsCardShelf_range)/sizeof(RangeTest_t), /* 5 */
      stsCardShelf_range, NULL },

    /* stsCardIndex */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsCardIndex_range)/sizeof(RangeTest_t), /* 23 */
      stsCardIndex_range, NULL },

    /* stsCardType */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsCardDescr */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsCardSerial */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsCardHwVersion */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsCardCLEICode */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsCardOperStatus */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsCardReset */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsCardReset_range)/sizeof(RangeTest_t), /* 2 */
      stsCardReset_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsCardEntry family.
 */
const SrTestInfoEntry stsCardEntryTestInfo = {
    &stsCardEntryGetInfo,
    stsCardEntry_member_test,
#ifdef stsCardEntry_READ_CREATE
    k_stsCardEntry_set_defaults,
#else /* stsCardEntry_READ_CREATE */
    NULL,
#endif /* stsCardEntry_READ_CREATE */
    k_stsCardEntry_test,
    k_stsCardEntry_ready,
#ifdef SR_stsCardEntry_UNDO
    stsCardEntry_undo,
#else /* SR_stsCardEntry_UNDO */
    NULL,
#endif /* SR_stsCardEntry_UNDO */
    stsCardEntry_ready,
    stsCardEntry_set,
    stsCardEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsCardEntry data object.
 *---------------------------------------------------------------------*/
void
stsCardEntry_free(stsCardEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsCardEntryGetInfo.type_table,
                 stsCardEntryGetInfo.highest_nominator,
                 stsCardEntryGetInfo.valid_offset,
                 stsCardEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsCardEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsCardEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsCardEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsCardEntry family
 *---------------------------------------------------------------------*/
stsCardEntry_t *
Clone_stsCardEntry(stsCardEntry_t *stsCardEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsCardEntry,
                         stsCardEntryGetInfo.family_size,
                         stsCardEntryGetInfo.type_table,
                         stsCardEntryGetInfo.highest_nominator,
                         stsCardEntryGetInfo.valid_offset,
                         stsCardEntryTestInfo.userpart_clone_func,
                         stsCardEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsCardEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                  doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsCardEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsCardEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsCardEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsCardEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsCardEntry_set((stsCardEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsOscillatorEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsOscillatorEntryTypeTable[];

const SrIndexInfo stsOscillatorEntryIndexInfo[] = {
#ifdef I_stsOscillatorIndex
    { I_stsOscillatorIndex, T_uint, -1 },
#endif /* I_stsOscillatorIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsOscillatorEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsOscillatorEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsOscillatorEntry_t),
    I_stsOscillatorEntry_max,
    (SnmpType *) stsOscillatorEntryTypeTable,
    stsOscillatorEntryIndexInfo,
    (short) offsetof(stsOscillatorEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsOscillatorEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsOscillatorEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                       ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsOscillatorIndex)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsOscillatorEntryGetInfo));
#endif /* all indices are supported */
}

stsOscillatorEntry_t *
new_k_stsOscillatorEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator, int searchType,
                             stsOscillatorEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsOscillatorEntry_get(serialNum, contextInfo, nominator,
                                    searchType, data->stsOscillatorIndex);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsOscillatorEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsOscillatorEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsOscillatorIndex_range[] = { { 1, 2 } };
static RangeTest_t   stsOscillatorSwitchState_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsOscillatorEntry family.
 */
static struct sr_member_test stsOscillatorEntry_member_test[] =
{
    /* stsOscillatorIndex */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOscillatorIndex_range)/sizeof(RangeTest_t), /* 2 */
      stsOscillatorIndex_range, NULL },

    /* stsOscillatorModuleType */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsOscillatorEFER */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsOscillatorESSD */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsOscillatorESSN */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsOscillatorFAGE */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsOscillatorTIMD */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsOscillatorSwitchState */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOscillatorSwitchState_range)/sizeof(RangeTest_t), /* 2 */
      stsOscillatorSwitchState_range, NULL },

    /* stsOscillatorStatus */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsOscillatorRedStatus */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsOscillatorEntry family.
 */
const SrTestInfoEntry stsOscillatorEntryTestInfo = {
    &stsOscillatorEntryGetInfo,
    stsOscillatorEntry_member_test,
#ifdef stsOscillatorEntry_READ_CREATE
    k_stsOscillatorEntry_set_defaults,
#else /* stsOscillatorEntry_READ_CREATE */
    NULL,
#endif /* stsOscillatorEntry_READ_CREATE */
    k_stsOscillatorEntry_test,
    k_stsOscillatorEntry_ready,
#ifdef SR_stsOscillatorEntry_UNDO
    stsOscillatorEntry_undo,
#else /* SR_stsOscillatorEntry_UNDO */
    NULL,
#endif /* SR_stsOscillatorEntry_UNDO */
    stsOscillatorEntry_ready,
    stsOscillatorEntry_set,
    stsOscillatorEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsOscillatorEntry data object.
 *---------------------------------------------------------------------*/
void
stsOscillatorEntry_free(stsOscillatorEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsOscillatorEntryGetInfo.type_table,
                 stsOscillatorEntryGetInfo.highest_nominator,
                 stsOscillatorEntryGetInfo.valid_offset,
                 stsOscillatorEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsOscillatorEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsOscillatorEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsOscillatorEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsOscillatorEntry family
 *---------------------------------------------------------------------*/
stsOscillatorEntry_t *
Clone_stsOscillatorEntry(stsOscillatorEntry_t *stsOscillatorEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsOscillatorEntry,
                         stsOscillatorEntryGetInfo.family_size,
                         stsOscillatorEntryGetInfo.type_table,
                         stsOscillatorEntryGetInfo.highest_nominator,
                         stsOscillatorEntryGetInfo.valid_offset,
                         stsOscillatorEntryTestInfo.userpart_clone_func,
                         stsOscillatorEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsOscillatorEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                        doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsOscillatorEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsOscillatorEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsOscillatorEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsOscillatorEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsOscillatorEntry_set((stsOscillatorEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsGpsEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsGpsEntryTypeTable[];

const SrIndexInfo stsGpsEntryIndexInfo[] = {
#ifdef I_stsGpsIndex
    { I_stsGpsIndex, T_uint, -1 },
#endif /* I_stsGpsIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsGpsEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsGpsEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsGpsEntry_t),
    I_stsGpsEntry_max,
    (SnmpType *) stsGpsEntryTypeTable,
    stsGpsEntryIndexInfo,
    (short) offsetof(stsGpsEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsGpsEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsGpsEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsGpsIndex)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsGpsEntryGetInfo));
#endif /* all indices are supported */
}

stsGpsEntry_t *
new_k_stsGpsEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator, int searchType,
                      stsGpsEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsGpsEntry_get(serialNum, contextInfo, nominator,
                             searchType, data->stsGpsIndex);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsGpsEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsGpsEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsGpsIndex_range[] = { { 1, 2 } };
static LengthTest_t  stsGpsSPOS_len[] = { { 1, 80 } };
static LengthTest_t  stsGpsTRMO_len[] = { { 1, 80 } };
static RangeTest_t   stsGpsReset_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsGpsEntry family.
 */
static struct sr_member_test stsGpsEntry_member_test[] =
{
    /* stsGpsIndex */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsGpsIndex_range)/sizeof(RangeTest_t), /* 2 */
      stsGpsIndex_range, NULL },

    /* stsGpsStatus */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsState */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsANTD */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsAZEL */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsRSTG */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsBITS */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsPAVG */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsSIGQ */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsSPOS */
    { MINV_LENGTH_TEST, 
      sizeof(stsGpsSPOS_len)/sizeof(LengthTest_t), /* 1 */
      stsGpsSPOS_len, NULL },

    /* stsGpsTRMO */
    { MINV_LENGTH_TEST, 
      sizeof(stsGpsTRMO_len)/sizeof(LengthTest_t), /* 1 */
      stsGpsTRMO_len, NULL },

    /* stsGpsUTCT */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsVERS */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsDETF */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsEFER */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsESSD */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsESSN */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsFAGE */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsTIMD */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsGpsReset */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsGpsReset_range)/sizeof(RangeTest_t), /* 2 */
      stsGpsReset_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsGpsEntry family.
 */
const SrTestInfoEntry stsGpsEntryTestInfo = {
    &stsGpsEntryGetInfo,
    stsGpsEntry_member_test,
#ifdef stsGpsEntry_READ_CREATE
    k_stsGpsEntry_set_defaults,
#else /* stsGpsEntry_READ_CREATE */
    NULL,
#endif /* stsGpsEntry_READ_CREATE */
    k_stsGpsEntry_test,
    k_stsGpsEntry_ready,
#ifdef SR_stsGpsEntry_UNDO
    stsGpsEntry_undo,
#else /* SR_stsGpsEntry_UNDO */
    NULL,
#endif /* SR_stsGpsEntry_UNDO */
    stsGpsEntry_ready,
    stsGpsEntry_set,
    stsGpsEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsGpsEntry data object.
 *---------------------------------------------------------------------*/
void
stsGpsEntry_free(stsGpsEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsGpsEntryGetInfo.type_table,
                 stsGpsEntryGetInfo.highest_nominator,
                 stsGpsEntryGetInfo.valid_offset,
                 stsGpsEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsGpsEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsGpsEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsGpsEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsGpsEntry family
 *---------------------------------------------------------------------*/
stsGpsEntry_t *
Clone_stsGpsEntry(stsGpsEntry_t *stsGpsEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsGpsEntry,
                         stsGpsEntryGetInfo.family_size,
                         stsGpsEntryGetInfo.type_table,
                         stsGpsEntryGetInfo.highest_nominator,
                         stsGpsEntryGetInfo.valid_offset,
                         stsGpsEntryTestInfo.userpart_clone_func,
                         stsGpsEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsGpsEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                 doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsGpsEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsGpsEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsGpsEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsGpsEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsGpsEntry_set((stsGpsEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsLinkEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsLinkEntryTypeTable[];

const SrIndexInfo stsLinkEntryIndexInfo[] = {
#ifdef I_stsLinkIndex
    { I_stsLinkIndex, T_uint, -1 },
#endif /* I_stsLinkIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsLinkEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsLinkEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsLinkEntry_t),
    I_stsLinkEntry_max,
    (SnmpType *) stsLinkEntryTypeTable,
    stsLinkEntryIndexInfo,
    (short) offsetof(stsLinkEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsLinkEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsLinkEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                 ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsLinkIndex)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsLinkEntryGetInfo));
#endif /* all indices are supported */
}

stsLinkEntry_t *
new_k_stsLinkEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator, int searchType,
                       stsLinkEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsLinkEntry_get(serialNum, contextInfo, nominator,
                              searchType, data->stsLinkIndex);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsLinkEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsLinkEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsLinkIndex_range[] = { { 1, 5 } };
static RangeTest_t   stsLinkType_range[] = { { 1, 5 } };
static RangeTest_t   stsLinkEnable_range[] = { { 1, 2 } };
static RangeTest_t   stsLinkLineCode_range[] = { { 1, 6 } };
static RangeTest_t   stsLinkFMT_range[] = { { 1, 11 } };
static RangeTest_t   stsLinkTermination_range[] = { { 1, 4 } };
static RangeTest_t   stsLinkMFCHK_range[] = { { 1, 2 } };
static LengthTest_t  stsLinkId_len[] = { { 1, 24 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsLinkEntry family.
 */
static struct sr_member_test stsLinkEntry_member_test[] =
{
    /* stsLinkIndex */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkIndex_range)/sizeof(RangeTest_t), /* 5 */
      stsLinkIndex_range, NULL },

    /* stsLinkType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkType_range)/sizeof(RangeTest_t), /* 5 */
      stsLinkType_range, NULL },

    /* stsLinkEnable */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkEnable_range)/sizeof(RangeTest_t), /* 2 */
      stsLinkEnable_range, NULL },

    /* stsLinkLineCode */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkLineCode_range)/sizeof(RangeTest_t), /* 6 */
      stsLinkLineCode_range, NULL },

    /* stsLinkFMT */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkFMT_range)/sizeof(RangeTest_t), /* 11 */
      stsLinkFMT_range, NULL },

    /* stsLinkTermination */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkTermination_range)/sizeof(RangeTest_t), /* 4 */
      stsLinkTermination_range, NULL },

    /* stsLinkMFCHK */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkMFCHK_range)/sizeof(RangeTest_t), /* 2 */
      stsLinkMFCHK_range, NULL },

    /* stsLinkId */
    { MINV_LENGTH_TEST, 
      sizeof(stsLinkId_len)/sizeof(LengthTest_t), /* 1 */
      stsLinkId_len, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsLinkEntry family.
 */
const SrTestInfoEntry stsLinkEntryTestInfo = {
    &stsLinkEntryGetInfo,
    stsLinkEntry_member_test,
#ifdef stsLinkEntry_READ_CREATE
    k_stsLinkEntry_set_defaults,
#else /* stsLinkEntry_READ_CREATE */
    NULL,
#endif /* stsLinkEntry_READ_CREATE */
    k_stsLinkEntry_test,
    k_stsLinkEntry_ready,
#ifdef SR_stsLinkEntry_UNDO
    stsLinkEntry_undo,
#else /* SR_stsLinkEntry_UNDO */
    NULL,
#endif /* SR_stsLinkEntry_UNDO */
    stsLinkEntry_ready,
    stsLinkEntry_set,
    stsLinkEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsLinkEntry data object.
 *---------------------------------------------------------------------*/
void
stsLinkEntry_free(stsLinkEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsLinkEntryGetInfo.type_table,
                 stsLinkEntryGetInfo.highest_nominator,
                 stsLinkEntryGetInfo.valid_offset,
                 stsLinkEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsLinkEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsLinkEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsLinkEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsLinkEntry family
 *---------------------------------------------------------------------*/
stsLinkEntry_t *
Clone_stsLinkEntry(stsLinkEntry_t *stsLinkEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsLinkEntry,
                         stsLinkEntryGetInfo.family_size,
                         stsLinkEntryGetInfo.type_table,
                         stsLinkEntryGetInfo.highest_nominator,
                         stsLinkEntryGetInfo.valid_offset,
                         stsLinkEntryTestInfo.userpart_clone_func,
                         stsLinkEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsLinkEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                  doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsLinkEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsLinkEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsLinkEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsLinkEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsLinkEntry_set((stsLinkEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsShelfEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsShelfEntryTypeTable[];

const SrIndexInfo stsShelfEntryIndexInfo[] = {
#ifdef I_stsShelfIndex
    { I_stsShelfIndex, T_uint, -1 },
#endif /* I_stsShelfIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsShelfEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsShelfEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsShelfEntry_t),
    I_stsShelfEntry_max,
    (SnmpType *) stsShelfEntryTypeTable,
    stsShelfEntryIndexInfo,
    (short) offsetof(stsShelfEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsShelfEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsShelfEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                  ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsShelfIndex)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsShelfEntryGetInfo));
#endif /* all indices are supported */
}

stsShelfEntry_t *
new_k_stsShelfEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator, int searchType,
                        stsShelfEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsShelfEntry_get(serialNum, contextInfo, nominator,
                               searchType, data->stsShelfIndex);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsShelfEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsShelfEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsShelfIndex_range[] = { { 1, 5 } };
static RangeTest_t   stsShelfReset_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsShelfEntry family.
 */
static struct sr_member_test stsShelfEntry_member_test[] =
{
    /* stsShelfIndex */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsShelfIndex_range)/sizeof(RangeTest_t), /* 5 */
      stsShelfIndex_range, NULL },

    /* stsShelfSerial */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsShelfCLEICode */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsShelfReset */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsShelfReset_range)/sizeof(RangeTest_t), /* 2 */
      stsShelfReset_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsShelfEntry family.
 */
const SrTestInfoEntry stsShelfEntryTestInfo = {
    &stsShelfEntryGetInfo,
    stsShelfEntry_member_test,
#ifdef stsShelfEntry_READ_CREATE
    k_stsShelfEntry_set_defaults,
#else /* stsShelfEntry_READ_CREATE */
    NULL,
#endif /* stsShelfEntry_READ_CREATE */
    k_stsShelfEntry_test,
    k_stsShelfEntry_ready,
#ifdef SR_stsShelfEntry_UNDO
    stsShelfEntry_undo,
#else /* SR_stsShelfEntry_UNDO */
    NULL,
#endif /* SR_stsShelfEntry_UNDO */
    stsShelfEntry_ready,
    stsShelfEntry_set,
    stsShelfEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsShelfEntry data object.
 *---------------------------------------------------------------------*/
void
stsShelfEntry_free(stsShelfEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsShelfEntryGetInfo.type_table,
                 stsShelfEntryGetInfo.highest_nominator,
                 stsShelfEntryGetInfo.valid_offset,
                 stsShelfEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsShelfEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsShelfEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsShelfEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsShelfEntry family
 *---------------------------------------------------------------------*/
stsShelfEntry_t *
Clone_stsShelfEntry(stsShelfEntry_t *stsShelfEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsShelfEntry,
                         stsShelfEntryGetInfo.family_size,
                         stsShelfEntryGetInfo.type_table,
                         stsShelfEntryGetInfo.highest_nominator,
                         stsShelfEntryGetInfo.valid_offset,
                         stsShelfEntryTestInfo.userpart_clone_func,
                         stsShelfEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsShelfEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                   doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsShelfEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsShelfEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsShelfEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsShelfEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsShelfEntry_set((stsShelfEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsAlarmThresholdEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsAlarmThresholdEntryTypeTable[];

const SrIndexInfo stsAlarmThresholdEntryIndexInfo[] = {
#ifdef I_stsAlarmThresholdCurrentMon
    { I_stsAlarmThresholdCurrentMon, T_uint, -1 },
#endif /* I_stsAlarmThresholdCurrentMon */
#ifdef I_stsAlarmThresholdType
    { I_stsAlarmThresholdType, T_uint, -1 },
#endif /* I_stsAlarmThresholdType */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsAlarmThresholdEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsAlarmThresholdEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsAlarmThresholdEntry_t),
    I_stsAlarmThresholdEntry_max,
    (SnmpType *) stsAlarmThresholdEntryTypeTable,
    stsAlarmThresholdEntryIndexInfo,
    (short) offsetof(stsAlarmThresholdEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsAlarmThresholdEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsAlarmThresholdEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                           ContextInfo *contextInfo, int serialNum)
{

#if !defined(I_stsAlarmThresholdCurrentMon) || !defined(I_stsAlarmThresholdType)
    return NULL;
#else /* all indices are supported */
   return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsAlarmThresholdEntryGetInfo));
#endif /* all indices are supported */
}

stsAlarmThresholdEntry_t *
new_k_stsAlarmThresholdEntry_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator, int searchType,
                                 stsAlarmThresholdEntry_t *data)
{
	
    if (data == NULL) {
        return NULL;
    }
    return k_stsAlarmThresholdEntry_get(serialNum, contextInfo, nominator,
                                        searchType,
                                        data->stsAlarmThresholdCurrentMon,
                                        data->stsAlarmThresholdType);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsAlarmThresholdEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsAlarmThresholdEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsAlarmThresholdCurrentMon_range[] = { { 1, 5 } };
static RangeTest_t   stsAlarmThresholdType_range[] = { { 1, 6 } };
static RangeTest_t   stsAlarmThresholdValue_range[] = { { 0, 10000 } };
static RangeTest_t   stsAlarmThresholdTimeInterval_range[] = { { 1, 7 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsAlarmThresholdEntry family.
 */
static struct sr_member_test stsAlarmThresholdEntry_member_test[] =
{
    /* stsAlarmThresholdCurrentMon */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsAlarmThresholdCurrentMon_range)/sizeof(RangeTest_t), /* 5 */
      stsAlarmThresholdCurrentMon_range, NULL },

    /* stsAlarmThresholdType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsAlarmThresholdType_range)/sizeof(RangeTest_t), /* 6 */
      stsAlarmThresholdType_range, NULL },

    /* stsAlarmThresholdValue */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsAlarmThresholdValue_range)/sizeof(RangeTest_t), /* 1 */
      stsAlarmThresholdValue_range, NULL },

    /* stsAlarmThresholdTimeInterval */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsAlarmThresholdTimeInterval_range)/sizeof(RangeTest_t), /* 7 */
      stsAlarmThresholdTimeInterval_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsAlarmThresholdEntry family.
 */
const SrTestInfoEntry stsAlarmThresholdEntryTestInfo = {
    &stsAlarmThresholdEntryGetInfo,
    stsAlarmThresholdEntry_member_test,
#ifdef stsAlarmThresholdEntry_READ_CREATE
    k_stsAlarmThresholdEntry_set_defaults,
#else /* stsAlarmThresholdEntry_READ_CREATE */
    NULL,
#endif /* stsAlarmThresholdEntry_READ_CREATE */
    k_stsAlarmThresholdEntry_test,
    k_stsAlarmThresholdEntry_ready,
#ifdef SR_stsAlarmThresholdEntry_UNDO
    stsAlarmThresholdEntry_undo,
#else /* SR_stsAlarmThresholdEntry_UNDO */
    NULL,
#endif /* SR_stsAlarmThresholdEntry_UNDO */
    stsAlarmThresholdEntry_ready,
    stsAlarmThresholdEntry_set,
    stsAlarmThresholdEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsAlarmThresholdEntry data object.
 *---------------------------------------------------------------------*/
void
stsAlarmThresholdEntry_free(stsAlarmThresholdEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsAlarmThresholdEntryGetInfo.type_table,
                 stsAlarmThresholdEntryGetInfo.highest_nominator,
                 stsAlarmThresholdEntryGetInfo.valid_offset,
                 stsAlarmThresholdEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsAlarmThresholdEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsAlarmThresholdEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsAlarmThresholdEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsAlarmThresholdEntry family
 *---------------------------------------------------------------------*/
stsAlarmThresholdEntry_t *
Clone_stsAlarmThresholdEntry(stsAlarmThresholdEntry_t *stsAlarmThresholdEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsAlarmThresholdEntry,
                         stsAlarmThresholdEntryGetInfo.family_size,
                         stsAlarmThresholdEntryGetInfo.type_table,
                         stsAlarmThresholdEntryGetInfo.highest_nominator,
                         stsAlarmThresholdEntryGetInfo.valid_offset,
                         stsAlarmThresholdEntryTestInfo.userpart_clone_func,
                         stsAlarmThresholdEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsAlarmThresholdEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsAlarmThresholdEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsAlarmThresholdEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsAlarmThresholdEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsAlarmThresholdEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsAlarmThresholdEntry_set((stsAlarmThresholdEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsLinkAlarmEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsLinkAlarmEntryTypeTable[];

const SrIndexInfo stsLinkAlarmEntryIndexInfo[] = {
#ifdef I_stsLinkAlarmSettingLinkType
    { I_stsLinkAlarmSettingLinkType, T_uint, -1 },
#endif /* I_stsLinkAlarmSettingLinkType */
#ifdef I_stsLinkAlarmSettingType
    { I_stsLinkAlarmSettingType, T_uint, -1 },
#endif /* I_stsLinkAlarmSettingType */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsLinkAlarmEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsLinkAlarmEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsLinkAlarmEntry_t),
    I_stsLinkAlarmEntry_max,
    (SnmpType *) stsLinkAlarmEntryTypeTable,
    stsLinkAlarmEntryIndexInfo,
    (short) offsetof(stsLinkAlarmEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsLinkAlarmEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsLinkAlarmEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                      ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsLinkAlarmSettingLinkType) || !defined(I_stsLinkAlarmSettingType)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsLinkAlarmEntryGetInfo));
#endif /* all indices are supported */
}

stsLinkAlarmEntry_t *
new_k_stsLinkAlarmEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator, int searchType,
                            stsLinkAlarmEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsLinkAlarmEntry_get(serialNum, contextInfo, nominator,
                                   searchType,
                                   data->stsLinkAlarmSettingLinkType,
                                   data->stsLinkAlarmSettingType);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsLinkAlarmEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsLinkAlarmEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsLinkAlarmSettingLinkType_range[] = { { 1, 8 } };
static RangeTest_t   stsLinkAlarmSettingType_range[] = { { 1, 10 } };
static RangeTest_t   stsLinkAlarmSettingSevirity_range[] = { { 1, 5 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsLinkAlarmEntry family.
 */
static struct sr_member_test stsLinkAlarmEntry_member_test[] =
{
    /* stsLinkAlarmSettingLinkType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkAlarmSettingLinkType_range)/sizeof(RangeTest_t), /* 8 */
      stsLinkAlarmSettingLinkType_range, NULL },

    /* stsLinkAlarmSettingType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkAlarmSettingType_range)/sizeof(RangeTest_t), /* 10 */
      stsLinkAlarmSettingType_range, NULL },

    /* stsLinkAlarmSettingSevirity */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsLinkAlarmSettingSevirity_range)/sizeof(RangeTest_t), /* 5 */
      stsLinkAlarmSettingSevirity_range, NULL },

    /* stsLinkAlarmSevirity */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsLinkAlarmEntry family.
 */
const SrTestInfoEntry stsLinkAlarmEntryTestInfo = {
    &stsLinkAlarmEntryGetInfo,
    stsLinkAlarmEntry_member_test,
#ifdef stsLinkAlarmEntry_READ_CREATE
    k_stsLinkAlarmEntry_set_defaults,
#else /* stsLinkAlarmEntry_READ_CREATE */
    NULL,
#endif /* stsLinkAlarmEntry_READ_CREATE */
    k_stsLinkAlarmEntry_test,
    k_stsLinkAlarmEntry_ready,
#ifdef SR_stsLinkAlarmEntry_UNDO
    stsLinkAlarmEntry_undo,
#else /* SR_stsLinkAlarmEntry_UNDO */
    NULL,
#endif /* SR_stsLinkAlarmEntry_UNDO */
    stsLinkAlarmEntry_ready,
    stsLinkAlarmEntry_set,
    stsLinkAlarmEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsLinkAlarmEntry data object.
 *---------------------------------------------------------------------*/
void
stsLinkAlarmEntry_free(stsLinkAlarmEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsLinkAlarmEntryGetInfo.type_table,
                 stsLinkAlarmEntryGetInfo.highest_nominator,
                 stsLinkAlarmEntryGetInfo.valid_offset,
                 stsLinkAlarmEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsLinkAlarmEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsLinkAlarmEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsLinkAlarmEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsLinkAlarmEntry family
 *---------------------------------------------------------------------*/
stsLinkAlarmEntry_t *
Clone_stsLinkAlarmEntry(stsLinkAlarmEntry_t *stsLinkAlarmEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsLinkAlarmEntry,
                         stsLinkAlarmEntryGetInfo.family_size,
                         stsLinkAlarmEntryGetInfo.type_table,
                         stsLinkAlarmEntryGetInfo.highest_nominator,
                         stsLinkAlarmEntryGetInfo.valid_offset,
                         stsLinkAlarmEntryTestInfo.userpart_clone_func,
                         stsLinkAlarmEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsLinkAlarmEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                       doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsLinkAlarmEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsLinkAlarmEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsLinkAlarmEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsLinkAlarmEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsLinkAlarmEntry_set((stsLinkAlarmEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsOscillatorAlarmEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsOscillatorAlarmEntryTypeTable[];

const SrIndexInfo stsOscillatorAlarmEntryIndexInfo[] = {
#ifdef I_stsOscillatorAlarmSettingClock
    { I_stsOscillatorAlarmSettingClock, T_uint, -1 },
#endif /* I_stsOscillatorAlarmSettingClock */
#ifdef I_stsOscillatorAlarmSettingType
    { I_stsOscillatorAlarmSettingType, T_uint, -1 },
#endif /* I_stsOscillatorAlarmSettingType */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsOscillatorAlarmEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsOscillatorAlarmEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsOscillatorAlarmEntry_t),
    I_stsOscillatorAlarmEntry_max,
    (SnmpType *) stsOscillatorAlarmEntryTypeTable,
    stsOscillatorAlarmEntryIndexInfo,
    (short) offsetof(stsOscillatorAlarmEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsOscillatorAlarmEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsOscillatorAlarmEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                            ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsOscillatorAlarmSettingClock) || !defined(I_stsOscillatorAlarmSettingType)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsOscillatorAlarmEntryGetInfo));
#endif /* all indices are supported */
}

stsOscillatorAlarmEntry_t *
new_k_stsOscillatorAlarmEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator, int searchType,
                                  stsOscillatorAlarmEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsOscillatorAlarmEntry_get(serialNum, contextInfo, nominator,
                                         searchType,
                                         data->stsOscillatorAlarmSettingClock,
                                         data->stsOscillatorAlarmSettingType);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsOscillatorAlarmEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsOscillatorAlarmEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsOscillatorAlarmSettingClock_range[] = { { 1, 3 } };
static RangeTest_t   stsOscillatorAlarmSettingType_range[] = { { 1, 5 } };
static RangeTest_t   stsOscillatorAlarmSettingSeverity_range[] = { { 1, 5 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsOscillatorAlarmEntry family.
 */
static struct sr_member_test stsOscillatorAlarmEntry_member_test[] =
{
    /* stsOscillatorAlarmSettingClock */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOscillatorAlarmSettingClock_range)/sizeof(RangeTest_t), /* 3 */
      stsOscillatorAlarmSettingClock_range, NULL },

    /* stsOscillatorAlarmSettingType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOscillatorAlarmSettingType_range)/sizeof(RangeTest_t), /* 5 */
      stsOscillatorAlarmSettingType_range, NULL },

    /* stsOscillatorAlarmSettingSeverity */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOscillatorAlarmSettingSeverity_range)/sizeof(RangeTest_t), /* 5 */
      stsOscillatorAlarmSettingSeverity_range, NULL },

    /* stsOscillatorAlarmSeverity */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsOscillatorAlarmEntry family.
 */
const SrTestInfoEntry stsOscillatorAlarmEntryTestInfo = {
    &stsOscillatorAlarmEntryGetInfo,
    stsOscillatorAlarmEntry_member_test,
#ifdef stsOscillatorAlarmEntry_READ_CREATE
    k_stsOscillatorAlarmEntry_set_defaults,
#else /* stsOscillatorAlarmEntry_READ_CREATE */
    NULL,
#endif /* stsOscillatorAlarmEntry_READ_CREATE */
    k_stsOscillatorAlarmEntry_test,
    k_stsOscillatorAlarmEntry_ready,
#ifdef SR_stsOscillatorAlarmEntry_UNDO
    stsOscillatorAlarmEntry_undo,
#else /* SR_stsOscillatorAlarmEntry_UNDO */
    NULL,
#endif /* SR_stsOscillatorAlarmEntry_UNDO */
    stsOscillatorAlarmEntry_ready,
    stsOscillatorAlarmEntry_set,
    stsOscillatorAlarmEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsOscillatorAlarmEntry data object.
 *---------------------------------------------------------------------*/
void
stsOscillatorAlarmEntry_free(stsOscillatorAlarmEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsOscillatorAlarmEntryGetInfo.type_table,
                 stsOscillatorAlarmEntryGetInfo.highest_nominator,
                 stsOscillatorAlarmEntryGetInfo.valid_offset,
                 stsOscillatorAlarmEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsOscillatorAlarmEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsOscillatorAlarmEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsOscillatorAlarmEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsOscillatorAlarmEntry family
 *---------------------------------------------------------------------*/
stsOscillatorAlarmEntry_t *
Clone_stsOscillatorAlarmEntry(stsOscillatorAlarmEntry_t *stsOscillatorAlarmEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsOscillatorAlarmEntry,
                         stsOscillatorAlarmEntryGetInfo.family_size,
                         stsOscillatorAlarmEntryGetInfo.type_table,
                         stsOscillatorAlarmEntryGetInfo.highest_nominator,
                         stsOscillatorAlarmEntryGetInfo.valid_offset,
                         stsOscillatorAlarmEntryTestInfo.userpart_clone_func,
                         stsOscillatorAlarmEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsOscillatorAlarmEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsOscillatorAlarmEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsOscillatorAlarmEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsOscillatorAlarmEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsOscillatorAlarmEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsOscillatorAlarmEntry_set((stsOscillatorAlarmEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsOsPwrAlarmEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsOsPwrAlarmEntryTypeTable[];

const SrIndexInfo stsOsPwrAlarmEntryIndexInfo[] = {
#ifdef I_stssPwrAlarmSettingShelf
    { I_stssPwrAlarmSettingShelf, T_uint, -1 },
#endif /* I_stssPwrAlarmSettingShelf */
#ifdef I_stsOsPwrAlarmSettingOsPwr
    { I_stsOsPwrAlarmSettingOsPwr, T_uint, -1 },
#endif /* I_stsOsPwrAlarmSettingOsPwr */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsOsPwrAlarmEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsOsPwrAlarmEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsOsPwrAlarmEntry_t),
    I_stsOsPwrAlarmEntry_max,
    (SnmpType *) stsOsPwrAlarmEntryTypeTable,
    stsOsPwrAlarmEntryIndexInfo,
    (short) offsetof(stsOsPwrAlarmEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsOsPwrAlarmEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsOsPwrAlarmEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                       ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stssPwrAlarmSettingShelf) || !defined(I_stsOsPwrAlarmSettingOsPwr)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsOsPwrAlarmEntryGetInfo));
#endif /* all indices are supported */
}

stsOsPwrAlarmEntry_t *
new_k_stsOsPwrAlarmEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator, int searchType,
                             stsOsPwrAlarmEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsOsPwrAlarmEntry_get(serialNum, contextInfo, nominator,
                                    searchType,
                                    data->stssPwrAlarmSettingShelf,
                                    data->stsOsPwrAlarmSettingOsPwr);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsOsPwrAlarmEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsOsPwrAlarmEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stssPwrAlarmSettingShelf_range[] = { { 1, 5 } };
static RangeTest_t   stsOsPwrAlarmSettingOsPwr_range[] = { { 1, 3 } };
static RangeTest_t   stsOsPwrAlarmSettingSeverity_range[] = { { 1, 5 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsOsPwrAlarmEntry family.
 */
static struct sr_member_test stsOsPwrAlarmEntry_member_test[] =
{
    /* stssPwrAlarmSettingShelf */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stssPwrAlarmSettingShelf_range)/sizeof(RangeTest_t), /* 5 */
      stssPwrAlarmSettingShelf_range, NULL },

    /* stsOsPwrAlarmSettingOsPwr */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOsPwrAlarmSettingOsPwr_range)/sizeof(RangeTest_t), /* 3 */
      stsOsPwrAlarmSettingOsPwr_range, NULL },

    /* stsOsPwrAlarmSettingSeverity */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOsPwrAlarmSettingSeverity_range)/sizeof(RangeTest_t), /* 5 */
      stsOsPwrAlarmSettingSeverity_range, NULL },

    /* stsOsPwrAlarmSeverity */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsOsPwrAlarmEntry family.
 */
const SrTestInfoEntry stsOsPwrAlarmEntryTestInfo = {
    &stsOsPwrAlarmEntryGetInfo,
    stsOsPwrAlarmEntry_member_test,
#ifdef stsOsPwrAlarmEntry_READ_CREATE
    k_stsOsPwrAlarmEntry_set_defaults,
#else /* stsOsPwrAlarmEntry_READ_CREATE */
    NULL,
#endif /* stsOsPwrAlarmEntry_READ_CREATE */
    k_stsOsPwrAlarmEntry_test,
    k_stsOsPwrAlarmEntry_ready,
#ifdef SR_stsOsPwrAlarmEntry_UNDO
    stsOsPwrAlarmEntry_undo,
#else /* SR_stsOsPwrAlarmEntry_UNDO */
    NULL,
#endif /* SR_stsOsPwrAlarmEntry_UNDO */
    stsOsPwrAlarmEntry_ready,
    stsOsPwrAlarmEntry_set,
    stsOsPwrAlarmEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsOsPwrAlarmEntry data object.
 *---------------------------------------------------------------------*/
void
stsOsPwrAlarmEntry_free(stsOsPwrAlarmEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsOsPwrAlarmEntryGetInfo.type_table,
                 stsOsPwrAlarmEntryGetInfo.highest_nominator,
                 stsOsPwrAlarmEntryGetInfo.valid_offset,
                 stsOsPwrAlarmEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsOsPwrAlarmEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsOsPwrAlarmEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsOsPwrAlarmEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsOsPwrAlarmEntry family
 *---------------------------------------------------------------------*/
stsOsPwrAlarmEntry_t *
Clone_stsOsPwrAlarmEntry(stsOsPwrAlarmEntry_t *stsOsPwrAlarmEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsOsPwrAlarmEntry,
                         stsOsPwrAlarmEntryGetInfo.family_size,
                         stsOsPwrAlarmEntryGetInfo.type_table,
                         stsOsPwrAlarmEntryGetInfo.highest_nominator,
                         stsOsPwrAlarmEntryGetInfo.valid_offset,
                         stsOsPwrAlarmEntryTestInfo.userpart_clone_func,
                         stsOsPwrAlarmEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsOsPwrAlarmEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                        doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsOsPwrAlarmEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsOsPwrAlarmEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsOsPwrAlarmEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsOsPwrAlarmEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsOsPwrAlarmEntry_set((stsOsPwrAlarmEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsOutputAlarmEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsOutputAlarmEntryTypeTable[];

const SrIndexInfo stsOutputAlarmEntryIndexInfo[] = {
#ifdef I_stsOutputAlarmSettingShelf
    { I_stsOutputAlarmSettingShelf, T_uint, -1 },
#endif /* I_stsOutputAlarmSettingShelf */
#ifdef I_stsOutputAlarmSettingSlot
    { I_stsOutputAlarmSettingSlot, T_uint, -1 },
#endif /* I_stsOutputAlarmSettingSlot */
#ifdef I_stsOutputAlarmSettingPort
    { I_stsOutputAlarmSettingPort, T_uint, -1 },
#endif /* I_stsOutputAlarmSettingPort */
#ifdef I_stsOutputAlarmSettingType
    { I_stsOutputAlarmSettingType, T_uint, -1 },
#endif /* I_stsOutputAlarmSettingType */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsOutputAlarmEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsOutputAlarmEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsOutputAlarmEntry_t),
    I_stsOutputAlarmEntry_max,
    (SnmpType *) stsOutputAlarmEntryTypeTable,
    stsOutputAlarmEntryIndexInfo,
    (short) offsetof(stsOutputAlarmEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsOutputAlarmEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsOutputAlarmEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                        ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsOutputAlarmSettingShelf) || !defined(I_stsOutputAlarmSettingSlot) || !defined(I_stsOutputAlarmSettingPort) || !defined(I_stsOutputAlarmSettingType)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsOutputAlarmEntryGetInfo));
#endif /* all indices are supported */
}

stsOutputAlarmEntry_t *
new_k_stsOutputAlarmEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator, int searchType,
                              stsOutputAlarmEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsOutputAlarmEntry_get(serialNum, contextInfo, nominator,
                                     searchType,
                                     data->stsOutputAlarmSettingShelf,
                                     data->stsOutputAlarmSettingSlot,
                                     data->stsOutputAlarmSettingPort,
                                     data->stsOutputAlarmSettingType);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsOutputAlarmEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsOutputAlarmEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsOutputAlarmSettingShelf_range[] = { { 1, 5 } };
static RangeTest_t   stsOutputAlarmSettingSlot_range[] = { { 1, 16 } };
static RangeTest_t   stsOutputAlarmSettingPort_range[] = { { 1, 8 } };
static RangeTest_t   stsOutputAlarmSettingType_range[] = { { 1, 8 } };
static RangeTest_t   stsOutputAlarmSettingSeverity_range[] = { { 1, 5 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsOutputAlarmEntry family.
 */
static struct sr_member_test stsOutputAlarmEntry_member_test[] =
{
    /* stsOutputAlarmSettingShelf */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputAlarmSettingShelf_range)/sizeof(RangeTest_t), /* 5 */
      stsOutputAlarmSettingShelf_range, NULL },

    /* stsOutputAlarmSettingSlot */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputAlarmSettingSlot_range)/sizeof(RangeTest_t), /* 1 */
      stsOutputAlarmSettingSlot_range, NULL },

    /* stsOutputAlarmSettingPort */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputAlarmSettingPort_range)/sizeof(RangeTest_t), /* 1 */
      stsOutputAlarmSettingPort_range, NULL },

    /* stsOutputAlarmSettingType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputAlarmSettingType_range)/sizeof(RangeTest_t), /* 8 */
      stsOutputAlarmSettingType_range, Identity_test },

    /* stsOutputAlarmSettingSeverity */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputAlarmSettingSeverity_range)/sizeof(RangeTest_t), /* 5 */
      stsOutputAlarmSettingSeverity_range, NULL },

    /* stsOutputAlarmSeverity */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsOutputAlarmEntry family.
 */
const SrTestInfoEntry stsOutputAlarmEntryTestInfo = {
    &stsOutputAlarmEntryGetInfo,
    stsOutputAlarmEntry_member_test,
#ifdef stsOutputAlarmEntry_READ_CREATE
    k_stsOutputAlarmEntry_set_defaults,
#else /* stsOutputAlarmEntry_READ_CREATE */
    NULL,
#endif /* stsOutputAlarmEntry_READ_CREATE */
    k_stsOutputAlarmEntry_test,
    k_stsOutputAlarmEntry_ready,
#ifdef SR_stsOutputAlarmEntry_UNDO
    stsOutputAlarmEntry_undo,
#else /* SR_stsOutputAlarmEntry_UNDO */
    NULL,
#endif /* SR_stsOutputAlarmEntry_UNDO */
    stsOutputAlarmEntry_ready,
    stsOutputAlarmEntry_set,
    stsOutputAlarmEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsOutputAlarmEntry data object.
 *---------------------------------------------------------------------*/
void
stsOutputAlarmEntry_free(stsOutputAlarmEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsOutputAlarmEntryGetInfo.type_table,
                 stsOutputAlarmEntryGetInfo.highest_nominator,
                 stsOutputAlarmEntryGetInfo.valid_offset,
                 stsOutputAlarmEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsOutputAlarmEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsOutputAlarmEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsOutputAlarmEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsOutputAlarmEntry family
 *---------------------------------------------------------------------*/
stsOutputAlarmEntry_t *
Clone_stsOutputAlarmEntry(stsOutputAlarmEntry_t *stsOutputAlarmEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsOutputAlarmEntry,
                         stsOutputAlarmEntryGetInfo.family_size,
                         stsOutputAlarmEntryGetInfo.type_table,
                         stsOutputAlarmEntryGetInfo.highest_nominator,
                         stsOutputAlarmEntryGetInfo.valid_offset,
                         stsOutputAlarmEntryTestInfo.userpart_clone_func,
                         stsOutputAlarmEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsOutputAlarmEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsOutputAlarmEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsOutputAlarmEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsOutputAlarmEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsOutputAlarmEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsOutputAlarmEntry_set((stsOutputAlarmEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsOutputPortSettingEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsOutputPortSettingEntryTypeTable[];

const SrIndexInfo stsOutputPortSettingEntryIndexInfo[] = {
#ifdef I_stsOutputPortSettingShelf
    { I_stsOutputPortSettingShelf, T_uint, -1 },
#endif /* I_stsOutputPortSettingShelf */
#ifdef I_stsOutputPortSettingSlot
    { I_stsOutputPortSettingSlot, T_uint, -1 },
#endif /* I_stsOutputPortSettingSlot */
#ifdef I_stsOutputPortSettingPort
    { I_stsOutputPortSettingPort, T_uint, -1 },
#endif /* I_stsOutputPortSettingPort */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsOutputPortSettingEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsOutputPortSettingEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsOutputPortSettingEntry_t),
    I_stsOutputPortSettingEntry_max,
    (SnmpType *) stsOutputPortSettingEntryTypeTable,
    stsOutputPortSettingEntryIndexInfo,
    (short) offsetof(stsOutputPortSettingEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsOutputPortSettingEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsOutputPortSettingEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                              ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsOutputPortSettingShelf) || !defined(I_stsOutputPortSettingSlot) || !defined(I_stsOutputPortSettingPort)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsOutputPortSettingEntryGetInfo));
#endif /* all indices are supported */
}

stsOutputPortSettingEntry_t *
new_k_stsOutputPortSettingEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator, int searchType,
                                    stsOutputPortSettingEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsOutputPortSettingEntry_get(serialNum, contextInfo, nominator,
                                           searchType,
                                           data->stsOutputPortSettingShelf,
                                           data->stsOutputPortSettingSlot,
                                           data->stsOutputPortSettingPort);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsOutputPortSettingEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsOutputPortSettingEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsOutputPortSettingShelf_range[] = { { 1, 5 } };
static RangeTest_t   stsOutputPortSettingSlot_range[] = { { 1, 16 } };
static RangeTest_t   stsOutputPortSettingPort_range[] = { { 1, 8 } };
static RangeTest_t   stsOutputPortSettingType_range[] = { { 1, 11 } };
static LengthTest_t  stsOutputPortSettingName_len[] = { { 0, 40 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsOutputPortSettingEntry family.
 */
static struct sr_member_test stsOutputPortSettingEntry_member_test[] =
{
    /* stsOutputPortSettingShelf */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputPortSettingShelf_range)/sizeof(RangeTest_t), /* 5 */
      stsOutputPortSettingShelf_range, NULL },

    /* stsOutputPortSettingSlot */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputPortSettingSlot_range)/sizeof(RangeTest_t), /* 1 */
      stsOutputPortSettingSlot_range, NULL },

    /* stsOutputPortSettingPort */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputPortSettingPort_range)/sizeof(RangeTest_t), /* 1 */
      stsOutputPortSettingPort_range, NULL },

    /* stsOutputPortSettingType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputPortSettingType_range)/sizeof(RangeTest_t), /* 11 */
      stsOutputPortSettingType_range, NULL },

    /* stsOutputPortSettingName */
    { MINV_LENGTH_TEST, 
      sizeof(stsOutputPortSettingName_len)/sizeof(LengthTest_t), /* 1 */
      stsOutputPortSettingName_len, DisplayString_test }
};

/*
 * Initialize SrTestInfoEntry for the stsOutputPortSettingEntry family.
 */
const SrTestInfoEntry stsOutputPortSettingEntryTestInfo = {
    &stsOutputPortSettingEntryGetInfo,
    stsOutputPortSettingEntry_member_test,
#ifdef stsOutputPortSettingEntry_READ_CREATE
    k_stsOutputPortSettingEntry_set_defaults,
#else /* stsOutputPortSettingEntry_READ_CREATE */
    NULL,
#endif /* stsOutputPortSettingEntry_READ_CREATE */
    k_stsOutputPortSettingEntry_test,
    k_stsOutputPortSettingEntry_ready,
#ifdef SR_stsOutputPortSettingEntry_UNDO
    stsOutputPortSettingEntry_undo,
#else /* SR_stsOutputPortSettingEntry_UNDO */
    NULL,
#endif /* SR_stsOutputPortSettingEntry_UNDO */
    stsOutputPortSettingEntry_ready,
    stsOutputPortSettingEntry_set,
    stsOutputPortSettingEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsOutputPortSettingEntry data object.
 *---------------------------------------------------------------------*/
void
stsOutputPortSettingEntry_free(stsOutputPortSettingEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsOutputPortSettingEntryGetInfo.type_table,
                 stsOutputPortSettingEntryGetInfo.highest_nominator,
                 stsOutputPortSettingEntryGetInfo.valid_offset,
                 stsOutputPortSettingEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsOutputPortSettingEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsOutputPortSettingEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsOutputPortSettingEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsOutputPortSettingEntry family
 *---------------------------------------------------------------------*/
stsOutputPortSettingEntry_t *
Clone_stsOutputPortSettingEntry(stsOutputPortSettingEntry_t *stsOutputPortSettingEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsOutputPortSettingEntry,
                         stsOutputPortSettingEntryGetInfo.family_size,
                         stsOutputPortSettingEntryGetInfo.type_table,
                         stsOutputPortSettingEntryGetInfo.highest_nominator,
                         stsOutputPortSettingEntryGetInfo.valid_offset,
                         stsOutputPortSettingEntryTestInfo.userpart_clone_func,
                         stsOutputPortSettingEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsOutputPortSettingEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsOutputPortSettingEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsOutputPortSettingEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsOutputPortSettingEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsOutputPortSettingEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsOutputPortSettingEntry_set((stsOutputPortSettingEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsOutputTiuSettingEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsOutputTiuSettingEntryTypeTable[];

const SrIndexInfo stsOutputTiuSettingEntryIndexInfo[] = {
#ifdef I_stsOutputTiuSettingShelf
    { I_stsOutputTiuSettingShelf, T_uint, -1 },
#endif /* I_stsOutputTiuSettingShelf */
#ifdef I_stsOutputTiuSettingSlot
    { I_stsOutputTiuSettingSlot, T_uint, -1 },
#endif /* I_stsOutputTiuSettingSlot */
#ifdef I_stsOutputTiuSettingPort
    { I_stsOutputTiuSettingPort, T_uint, -1 },
#endif /* I_stsOutputTiuSettingPort */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsOutputTiuSettingEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsOutputTiuSettingEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsOutputTiuSettingEntry_t),
    I_stsOutputTiuSettingEntry_max,
    (SnmpType *) stsOutputTiuSettingEntryTypeTable,
    stsOutputTiuSettingEntryIndexInfo,
    (short) offsetof(stsOutputTiuSettingEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsOutputTiuSettingEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsOutputTiuSettingEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                             ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsOutputTiuSettingShelf) || !defined(I_stsOutputTiuSettingSlot) || !defined(I_stsOutputTiuSettingPort)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsOutputTiuSettingEntryGetInfo));
#endif /* all indices are supported */
}

stsOutputTiuSettingEntry_t *
new_k_stsOutputTiuSettingEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator, int searchType,
                                   stsOutputTiuSettingEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsOutputTiuSettingEntry_get(serialNum, contextInfo, nominator,
                                          searchType,
                                          data->stsOutputTiuSettingShelf,
                                          data->stsOutputTiuSettingSlot,
                                          data->stsOutputTiuSettingPort);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsOutputTiuSettingEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsOutputTiuSettingEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsOutputTiuSettingShelf_range[] = { { 1, 5 } };
static RangeTest_t   stsOutputTiuSettingSlot_range[] = { { 1, 16 } };
static RangeTest_t   stsOutputTiuSettingPort_range[] = { { 1, 8 } };
static RangeTest_t   stsOutputTiuSettingLineCoding_range[] = { { 1, 4 } };
static RangeTest_t   stsOutputTiuSettingFMT_range[] = { { 1, 4 } };
static RangeTest_t   stsOutputTiuSettingTermination_range[] = { { 1, 4 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsOutputTiuSettingEntry family.
 */
static struct sr_member_test stsOutputTiuSettingEntry_member_test[] =
{
    /* stsOutputTiuSettingShelf */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputTiuSettingShelf_range)/sizeof(RangeTest_t), /* 5 */
      stsOutputTiuSettingShelf_range, NULL },

    /* stsOutputTiuSettingSlot */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputTiuSettingSlot_range)/sizeof(RangeTest_t), /* 1 */
      stsOutputTiuSettingSlot_range, NULL },

    /* stsOutputTiuSettingPort */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputTiuSettingPort_range)/sizeof(RangeTest_t), /* 1 */
      stsOutputTiuSettingPort_range, NULL },

    /* stsOutputTiuSettingLineCoding */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputTiuSettingLineCoding_range)/sizeof(RangeTest_t), /* 4 */
      stsOutputTiuSettingLineCoding_range, NULL },

    /* stsOutputTiuSettingFMT */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputTiuSettingFMT_range)/sizeof(RangeTest_t), /* 4 */
      stsOutputTiuSettingFMT_range, NULL },

    /* stsOutputTiuSettingTermination */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputTiuSettingTermination_range)/sizeof(RangeTest_t), /* 4 */
      stsOutputTiuSettingTermination_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsOutputTiuSettingEntry family.
 */
const SrTestInfoEntry stsOutputTiuSettingEntryTestInfo = {
    &stsOutputTiuSettingEntryGetInfo,
    stsOutputTiuSettingEntry_member_test,
#ifdef stsOutputTiuSettingEntry_READ_CREATE
    k_stsOutputTiuSettingEntry_set_defaults,
#else /* stsOutputTiuSettingEntry_READ_CREATE */
    NULL,
#endif /* stsOutputTiuSettingEntry_READ_CREATE */
    k_stsOutputTiuSettingEntry_test,
    k_stsOutputTiuSettingEntry_ready,
#ifdef SR_stsOutputTiuSettingEntry_UNDO
    stsOutputTiuSettingEntry_undo,
#else /* SR_stsOutputTiuSettingEntry_UNDO */
    NULL,
#endif /* SR_stsOutputTiuSettingEntry_UNDO */
    stsOutputTiuSettingEntry_ready,
    stsOutputTiuSettingEntry_set,
    stsOutputTiuSettingEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsOutputTiuSettingEntry data object.
 *---------------------------------------------------------------------*/
void
stsOutputTiuSettingEntry_free(stsOutputTiuSettingEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsOutputTiuSettingEntryGetInfo.type_table,
                 stsOutputTiuSettingEntryGetInfo.highest_nominator,
                 stsOutputTiuSettingEntryGetInfo.valid_offset,
                 stsOutputTiuSettingEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsOutputTiuSettingEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsOutputTiuSettingEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsOutputTiuSettingEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsOutputTiuSettingEntry family
 *---------------------------------------------------------------------*/
stsOutputTiuSettingEntry_t *
Clone_stsOutputTiuSettingEntry(stsOutputTiuSettingEntry_t *stsOutputTiuSettingEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsOutputTiuSettingEntry,
                         stsOutputTiuSettingEntryGetInfo.family_size,
                         stsOutputTiuSettingEntryGetInfo.type_table,
                         stsOutputTiuSettingEntryGetInfo.highest_nominator,
                         stsOutputTiuSettingEntryGetInfo.valid_offset,
                         stsOutputTiuSettingEntryTestInfo.userpart_clone_func,
                         stsOutputTiuSettingEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsOutputTiuSettingEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                              doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsOutputTiuSettingEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsOutputTiuSettingEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsOutputTiuSettingEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsOutputTiuSettingEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsOutputTiuSettingEntry_set((stsOutputTiuSettingEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsPerfMonEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsPerfMonEntryTypeTable[];

const SrIndexInfo stsPerfMonEntryIndexInfo[] = {
#ifdef I_stsPerfMonInputCurrentMon
    { I_stsPerfMonInputCurrentMon, T_uint, -1 },
#endif /* I_stsPerfMonInputCurrentMon */
#ifdef I_stsPerfMonInputMonPeriod
    { I_stsPerfMonInputMonPeriod, T_uint, -1 },
#endif /* I_stsPerfMonInputMonPeriod */
#ifdef I_stsPerfMonInputMonBucket
    { I_stsPerfMonInputMonBucket, T_uint, -1 },
#endif /* I_stsPerfMonInputMonBucket */
#ifdef I_stsPerfMonInputMonType
    { I_stsPerfMonInputMonType, T_uint, -1 },
#endif /* I_stsPerfMonInputMonType */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsPerfMonEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsPerfMonEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsPerfMonEntry_t),
    I_stsPerfMonEntry_max,
    (SnmpType *) stsPerfMonEntryTypeTable,
    stsPerfMonEntryIndexInfo,
    (short) offsetof(stsPerfMonEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsPerfMonEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsPerfMonEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                    ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsPerfMonInputCurrentMon) || !defined(I_stsPerfMonInputMonPeriod) || !defined(I_stsPerfMonInputMonBucket) || !defined(I_stsPerfMonInputMonType)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsPerfMonEntryGetInfo));
#endif /* all indices are supported */
}

stsPerfMonEntry_t *
new_k_stsPerfMonEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator, int searchType,
                          stsPerfMonEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsPerfMonEntry_get(serialNum, contextInfo, nominator,
                                 searchType, data->stsPerfMonInputCurrentMon,
                                 data->stsPerfMonInputMonPeriod,
                                 data->stsPerfMonInputMonBucket,
                                 data->stsPerfMonInputMonType);
}

/*----------------------------------------------------------------------
 * Free the stsPerfMonEntry data object.
 *---------------------------------------------------------------------*/
void
stsPerfMonEntry_free(stsPerfMonEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsPerfMonEntryGetInfo.type_table,
                 stsPerfMonEntryGetInfo.highest_nominator,
                 stsPerfMonEntryGetInfo.valid_offset,
                 stsPerfMonEntryGetInfo.userpart_free_func);
}

/*
 * The stsTerminalEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsTerminalEntryTypeTable[];

const SrIndexInfo stsTerminalEntryIndexInfo[] = {
#ifdef I_stsTerminalIndex
    { I_stsTerminalIndex, T_uint, -1 },
#endif /* I_stsTerminalIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsTerminalEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsTerminalEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsTerminalEntry_t),
    I_stsTerminalEntry_max,
    (SnmpType *) stsTerminalEntryTypeTable,
    stsTerminalEntryIndexInfo,
    (short) offsetof(stsTerminalEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsTerminalEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsTerminalEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                     ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsTerminalIndex)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsTerminalEntryGetInfo));
#endif /* all indices are supported */
}

stsTerminalEntry_t *
new_k_stsTerminalEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator, int searchType,
                           stsTerminalEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsTerminalEntry_get(serialNum, contextInfo, nominator,
                                  searchType, data->stsTerminalIndex);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsTerminalEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsTerminalEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsTerminalIndex_range[] = { { 1, 1 } };
static RangeTest_t   stsTerminalBaudrate_range[] = { { 1, 6 } };
static RangeTest_t   stsTerminalDataBits_range[] = { { 1, 2 } };
static RangeTest_t   stsTerminalParity_range[] = { { 1, 3 } };
static RangeTest_t   stsTerminalStopBits_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsTerminalEntry family.
 */
static struct sr_member_test stsTerminalEntry_member_test[] =
{
    /* stsTerminalIndex */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTerminalIndex_range)/sizeof(RangeTest_t), /* 1 */
      stsTerminalIndex_range, NULL },

    /* stsTerminalBaudrate */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTerminalBaudrate_range)/sizeof(RangeTest_t), /* 6 */
      stsTerminalBaudrate_range, NULL },

    /* stsTerminalDataBits */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTerminalDataBits_range)/sizeof(RangeTest_t), /* 2 */
      stsTerminalDataBits_range, NULL },

    /* stsTerminalParity */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTerminalParity_range)/sizeof(RangeTest_t), /* 3 */
      stsTerminalParity_range, NULL },

    /* stsTerminalStopBits */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTerminalStopBits_range)/sizeof(RangeTest_t), /* 2 */
      stsTerminalStopBits_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsTerminalEntry family.
 */
const SrTestInfoEntry stsTerminalEntryTestInfo = {
    &stsTerminalEntryGetInfo,
    stsTerminalEntry_member_test,
#ifdef stsTerminalEntry_READ_CREATE
    k_stsTerminalEntry_set_defaults,
#else /* stsTerminalEntry_READ_CREATE */
    NULL,
#endif /* stsTerminalEntry_READ_CREATE */
    k_stsTerminalEntry_test,
    k_stsTerminalEntry_ready,
#ifdef SR_stsTerminalEntry_UNDO
    stsTerminalEntry_undo,
#else /* SR_stsTerminalEntry_UNDO */
    NULL,
#endif /* SR_stsTerminalEntry_UNDO */
    stsTerminalEntry_ready,
    stsTerminalEntry_set,
    stsTerminalEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsTerminalEntry data object.
 *---------------------------------------------------------------------*/
void
stsTerminalEntry_free(stsTerminalEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsTerminalEntryGetInfo.type_table,
                 stsTerminalEntryGetInfo.highest_nominator,
                 stsTerminalEntryGetInfo.valid_offset,
                 stsTerminalEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsTerminalEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsTerminalEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsTerminalEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsTerminalEntry family
 *---------------------------------------------------------------------*/
stsTerminalEntry_t *
Clone_stsTerminalEntry(stsTerminalEntry_t *stsTerminalEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsTerminalEntry,
                         stsTerminalEntryGetInfo.family_size,
                         stsTerminalEntryGetInfo.type_table,
                         stsTerminalEntryGetInfo.highest_nominator,
                         stsTerminalEntryGetInfo.valid_offset,
                         stsTerminalEntryTestInfo.userpart_clone_func,
                         stsTerminalEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsTerminalEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                      doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsTerminalEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsTerminalEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsTerminalEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsTerminalEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsTerminalEntry_set((stsTerminalEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsAlarmStatusEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsAlarmStatusEntryTypeTable[];

const SrIndexInfo stsAlarmStatusEntryIndexInfo[] = {
#ifdef I_stsAlarmSource
    { I_stsAlarmSource, T_uint, -1 },
#endif /* I_stsAlarmSource */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsAlarmStatusEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsAlarmStatusEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsAlarmStatusEntry_t),
    I_stsAlarmStatusEntry_max,
    (SnmpType *) stsAlarmStatusEntryTypeTable,
    stsAlarmStatusEntryIndexInfo,
    (short) offsetof(stsAlarmStatusEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsAlarmStatusEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsAlarmStatusEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                        ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsAlarmSource)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsAlarmStatusEntryGetInfo));
#endif /* all indices are supported */
}

stsAlarmStatusEntry_t *
new_k_stsAlarmStatusEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator, int searchType,
                              stsAlarmStatusEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsAlarmStatusEntry_get(serialNum, contextInfo, nominator,
                                     searchType, data->stsAlarmSource);
}

/*----------------------------------------------------------------------
 * Free the stsAlarmStatusEntry data object.
 *---------------------------------------------------------------------*/
void
stsAlarmStatusEntry_free(stsAlarmStatusEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsAlarmStatusEntryGetInfo.type_table,
                 stsAlarmStatusEntryGetInfo.highest_nominator,
                 stsAlarmStatusEntryGetInfo.valid_offset,
                 stsAlarmStatusEntryGetInfo.userpart_free_func);
}

/*
 * The stsOutputCardEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsOutputCardEntryTypeTable[];

const SrIndexInfo stsOutputCardEntryIndexInfo[] = {
#ifdef I_stsOutputShelf
    { I_stsOutputShelf, T_uint, -1 },
#endif /* I_stsOutputShelf */
#ifdef I_stsOutputSlot
    { I_stsOutputSlot, T_uint, -1 },
#endif /* I_stsOutputSlot */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsOutputCardEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsOutputCardEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsOutputCardEntry_t),
    I_stsOutputCardEntry_max,
    (SnmpType *) stsOutputCardEntryTypeTable,
    stsOutputCardEntryIndexInfo,
    (short) offsetof(stsOutputCardEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsOutputCardEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsOutputCardEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                       ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsOutputShelf) || !defined(I_stsOutputSlot)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsOutputCardEntryGetInfo));
#endif /* all indices are supported */
}

stsOutputCardEntry_t *
new_k_stsOutputCardEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator, int searchType,
                             stsOutputCardEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsOutputCardEntry_get(serialNum, contextInfo, nominator,
                                    searchType, data->stsOutputShelf,
                                    data->stsOutputSlot);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsOutputCardEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsOutputCardEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsOutputShelf_range[] = { { 1, 5 } };
static RangeTest_t   stsOutputSlot_range[] = { { 1, 16 } };
static RangeTest_t   stsOutputRedEnableState_range[] = { { 1, 2 } };
static RangeTest_t   stsOutputRedSwitchState_range[] = { { 1, 2 } };
static RangeTest_t   stsOutputCRCCHK_range[] = { { 1, 2 } };
static RangeTest_t   stsOutputCAS_range[] = { { 1, 2 } };
static RangeTest_t   stsOutputSSM_range[] = { { 1, 5 } };
static RangeTest_t   stsOutputSSMValue_range[] = { { 1, 6 } };
static LengthTest_t  stsOutputTimingFormat_len[] = { { 1, 60 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsOutputCardEntry family.
 */
static struct sr_member_test stsOutputCardEntry_member_test[] =
{
    /* stsOutputShelf */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputShelf_range)/sizeof(RangeTest_t), /* 5 */
      stsOutputShelf_range, NULL },

    /* stsOutputSlot */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputSlot_range)/sizeof(RangeTest_t), /* 1 */
      stsOutputSlot_range, NULL },

    /* stsOutputRedState */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsOutputRedEnableState */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputRedEnableState_range)/sizeof(RangeTest_t), /* 2 */
      stsOutputRedEnableState_range, NULL },

    /* stsOutputRedSwitchState */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputRedSwitchState_range)/sizeof(RangeTest_t), /* 2 */
      stsOutputRedSwitchState_range, NULL },

    /* stsOutputCRCCHK */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputCRCCHK_range)/sizeof(RangeTest_t), /* 2 */
      stsOutputCRCCHK_range, NULL },

    /* stsOutputCAS */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputCAS_range)/sizeof(RangeTest_t), /* 2 */
      stsOutputCAS_range, NULL },

    /* stsOutputSSM */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputSSM_range)/sizeof(RangeTest_t), /* 5 */
      stsOutputSSM_range, NULL },

    /* stsOutputSSMValue */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsOutputSSMValue_range)/sizeof(RangeTest_t), /* 6 */
      stsOutputSSMValue_range, NULL },

    /* stsOutputTimingFormat */
    { MINV_LENGTH_TEST, 
      sizeof(stsOutputTimingFormat_len)/sizeof(LengthTest_t), /* 1 */
      stsOutputTimingFormat_len, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsOutputCardEntry family.
 */
const SrTestInfoEntry stsOutputCardEntryTestInfo = {
    &stsOutputCardEntryGetInfo,
    stsOutputCardEntry_member_test,
#ifdef stsOutputCardEntry_READ_CREATE
    k_stsOutputCardEntry_set_defaults,
#else /* stsOutputCardEntry_READ_CREATE */
    NULL,
#endif /* stsOutputCardEntry_READ_CREATE */
    k_stsOutputCardEntry_test,
    k_stsOutputCardEntry_ready,
#ifdef SR_stsOutputCardEntry_UNDO
    stsOutputCardEntry_undo,
#else /* SR_stsOutputCardEntry_UNDO */
    NULL,
#endif /* SR_stsOutputCardEntry_UNDO */
    stsOutputCardEntry_ready,
    stsOutputCardEntry_set,
    stsOutputCardEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsOutputCardEntry data object.
 *---------------------------------------------------------------------*/
void
stsOutputCardEntry_free(stsOutputCardEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsOutputCardEntryGetInfo.type_table,
                 stsOutputCardEntryGetInfo.highest_nominator,
                 stsOutputCardEntryGetInfo.valid_offset,
                 stsOutputCardEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsOutputCardEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsOutputCardEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsOutputCardEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsOutputCardEntry family
 *---------------------------------------------------------------------*/
stsOutputCardEntry_t *
Clone_stsOutputCardEntry(stsOutputCardEntry_t *stsOutputCardEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsOutputCardEntry,
                         stsOutputCardEntryGetInfo.family_size,
                         stsOutputCardEntryGetInfo.type_table,
                         stsOutputCardEntryGetInfo.highest_nominator,
                         stsOutputCardEntryGetInfo.valid_offset,
                         stsOutputCardEntryTestInfo.userpart_clone_func,
                         stsOutputCardEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsOutputCardEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                        doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsOutputCardEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsOutputCardEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsOutputCardEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsOutputCardEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsOutputCardEntry_set((stsOutputCardEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsEmmStatusEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsEmmStatusEntryTypeTable[];

const SrIndexInfo stsEmmStatusEntryIndexInfo[] = {
#ifdef I_stsEmmShelf
    { I_stsEmmShelf, T_uint, -1 },
#endif /* I_stsEmmShelf */
#ifdef I_stsEmmSensorType
    { I_stsEmmSensorType, T_uint, -1 },
#endif /* I_stsEmmSensorType */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsEmmStatusEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsEmmStatusEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsEmmStatusEntry_t),
    I_stsEmmStatusEntry_max,
    (SnmpType *) stsEmmStatusEntryTypeTable,
    stsEmmStatusEntryIndexInfo,
    (short) offsetof(stsEmmStatusEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsEmmStatusEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsEmmStatusEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                      ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsEmmShelf) || !defined(I_stsEmmSensorType)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsEmmStatusEntryGetInfo));
#endif /* all indices are supported */
}

stsEmmStatusEntry_t *
new_k_stsEmmStatusEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator, int searchType,
                            stsEmmStatusEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsEmmStatusEntry_get(serialNum, contextInfo, nominator,
                                   searchType, data->stsEmmShelf,
                                   data->stsEmmSensorType);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsEmmStatusEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsEmmStatusEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsEmmShelf_range[] = { { 1, 5 } };
static RangeTest_t   stsEmmSensorType_range[] = { { 1, 5 } };
static RangeTest_t   stsEmmMajorAlmHiThresh_range[] = { { -50, 100 } };
static RangeTest_t   stsEmmMajorAlmLoThresh_range[] = { { -50, 100 } };
static RangeTest_t   stsEmmMinorAlmHiThresh_range[] = { { -50, 100 } };
static RangeTest_t   stsEmmMinorAlmLoThresh_range[] = { { -50, 100 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsEmmStatusEntry family.
 */
static struct sr_member_test stsEmmStatusEntry_member_test[] =
{
    /* stsEmmShelf */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsEmmShelf_range)/sizeof(RangeTest_t), /* 5 */
      stsEmmShelf_range, NULL },

    /* stsEmmSensorType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsEmmSensorType_range)/sizeof(RangeTest_t), /* 5 */
      stsEmmSensorType_range, NULL },

    /* stsEmmAlarmStatus */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsEmmSensorReading */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsEmmMajorAlmHiThresh */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsEmmMajorAlmHiThresh_range)/sizeof(RangeTest_t), /* 1 */
      stsEmmMajorAlmHiThresh_range, NULL },

    /* stsEmmMajorAlmLoThresh */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsEmmMajorAlmLoThresh_range)/sizeof(RangeTest_t), /* 1 */
      stsEmmMajorAlmLoThresh_range, NULL },

    /* stsEmmMinorAlmHiThresh */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsEmmMinorAlmHiThresh_range)/sizeof(RangeTest_t), /* 1 */
      stsEmmMinorAlmHiThresh_range, NULL },

    /* stsEmmMinorAlmLoThresh */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsEmmMinorAlmLoThresh_range)/sizeof(RangeTest_t), /* 1 */
      stsEmmMinorAlmLoThresh_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsEmmStatusEntry family.
 */
const SrTestInfoEntry stsEmmStatusEntryTestInfo = {
    &stsEmmStatusEntryGetInfo,
    stsEmmStatusEntry_member_test,
#ifdef stsEmmStatusEntry_READ_CREATE
    k_stsEmmStatusEntry_set_defaults,
#else /* stsEmmStatusEntry_READ_CREATE */
    NULL,
#endif /* stsEmmStatusEntry_READ_CREATE */
    k_stsEmmStatusEntry_test,
    k_stsEmmStatusEntry_ready,
#ifdef SR_stsEmmStatusEntry_UNDO
    stsEmmStatusEntry_undo,
#else /* SR_stsEmmStatusEntry_UNDO */
    NULL,
#endif /* SR_stsEmmStatusEntry_UNDO */
    stsEmmStatusEntry_ready,
    stsEmmStatusEntry_set,
    stsEmmStatusEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsEmmStatusEntry data object.
 *---------------------------------------------------------------------*/
void
stsEmmStatusEntry_free(stsEmmStatusEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsEmmStatusEntryGetInfo.type_table,
                 stsEmmStatusEntryGetInfo.highest_nominator,
                 stsEmmStatusEntryGetInfo.valid_offset,
                 stsEmmStatusEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsEmmStatusEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsEmmStatusEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsEmmStatusEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsEmmStatusEntry family
 *---------------------------------------------------------------------*/
stsEmmStatusEntry_t *
Clone_stsEmmStatusEntry(stsEmmStatusEntry_t *stsEmmStatusEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsEmmStatusEntry,
                         stsEmmStatusEntryGetInfo.family_size,
                         stsEmmStatusEntryGetInfo.type_table,
                         stsEmmStatusEntryGetInfo.highest_nominator,
                         stsEmmStatusEntryGetInfo.valid_offset,
                         stsEmmStatusEntryTestInfo.userpart_clone_func,
                         stsEmmStatusEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsEmmStatusEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                       doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsEmmStatusEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsEmmStatusEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsEmmStatusEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsEmmStatusEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsEmmStatusEntry_set((stsEmmStatusEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsAntEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsAntEntryTypeTable[];

const SrIndexInfo stsAntEntryIndexInfo[] = {
#ifdef I_stsAntIndex
    { I_stsAntIndex, T_uint, -1 },
#endif /* I_stsAntIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsAntEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsAntEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsAntEntry_t),
    I_stsAntEntry_max,
    (SnmpType *) stsAntEntryTypeTable,
    stsAntEntryIndexInfo,
    (short) offsetof(stsAntEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsAntEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsAntEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsAntIndex)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsAntEntryGetInfo));
#endif /* all indices are supported */
}

stsAntEntry_t *
new_k_stsAntEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator, int searchType,
                      stsAntEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsAntEntry_get(serialNum, contextInfo, nominator,
                             searchType, data->stsAntIndex);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsAntEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsAntEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsAntIndex_range[] = { { 1, 2 } };
static RangeTest_t   stsAntSwitchState_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsAntEntry family.
 */
static struct sr_member_test stsAntEntry_member_test[] =
{
    /* stsAntIndex */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsAntIndex_range)/sizeof(RangeTest_t), /* 1 */
      stsAntIndex_range, NULL },

    /* stsAntStatus */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* stsAntSwitchState */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsAntSwitchState_range)/sizeof(RangeTest_t), /* 2 */
      stsAntSwitchState_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsAntEntry family.
 */
const SrTestInfoEntry stsAntEntryTestInfo = {
    &stsAntEntryGetInfo,
    stsAntEntry_member_test,
#ifdef stsAntEntry_READ_CREATE
    k_stsAntEntry_set_defaults,
#else /* stsAntEntry_READ_CREATE */
    NULL,
#endif /* stsAntEntry_READ_CREATE */
    k_stsAntEntry_test,
    k_stsAntEntry_ready,
#ifdef SR_stsAntEntry_UNDO
    stsAntEntry_undo,
#else /* SR_stsAntEntry_UNDO */
    NULL,
#endif /* SR_stsAntEntry_UNDO */
    stsAntEntry_ready,
    stsAntEntry_set,
    stsAntEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsAntEntry data object.
 *---------------------------------------------------------------------*/
void
stsAntEntry_free(stsAntEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsAntEntryGetInfo.type_table,
                 stsAntEntryGetInfo.highest_nominator,
                 stsAntEntryGetInfo.valid_offset,
                 stsAntEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsAntEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsAntEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsAntEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsAntEntry family
 *---------------------------------------------------------------------*/
stsAntEntry_t *
Clone_stsAntEntry(stsAntEntry_t *stsAntEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsAntEntry,
                         stsAntEntryGetInfo.family_size,
                         stsAntEntryGetInfo.type_table,
                         stsAntEntryGetInfo.highest_nominator,
                         stsAntEntryGetInfo.valid_offset,
                         stsAntEntryTestInfo.userpart_clone_func,
                         stsAntEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsAntEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                 doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsAntEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsAntEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsAntEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsAntEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsAntEntry_set((stsAntEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsTopDestEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsTopDestEntryTypeTable[];

const SrIndexInfo stsTopDestEntryIndexInfo[] = {
#ifdef I_stsTopDestNumber
    { I_stsTopDestNumber, T_uint, -1 },
#endif /* I_stsTopDestNumber */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsTopDestEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsTopDestEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsTopDestEntry_t),
    I_stsTopDestEntry_max,
    (SnmpType *) stsTopDestEntryTypeTable,
    stsTopDestEntryIndexInfo,
    (short) offsetof(stsTopDestEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsTopDestEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsTopDestEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                    ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsTopDestNumber)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsTopDestEntryGetInfo));
#endif /* all indices are supported */
}

stsTopDestEntry_t *
new_k_stsTopDestEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator, int searchType,
                          stsTopDestEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsTopDestEntry_get(serialNum, contextInfo, nominator,
                                 searchType, data->stsTopDestNumber);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsTopDestEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsTopDestEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsTopDestNumber_range[] = { { 1, 28 } };
static RangeTest_t   stsTopDestUdpPort_range[] = { { 0, 65535 } };
static RangeTest_t   stsTopDestVlanId_range[] = { { 1, 32 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsTopDestEntry family.
 */
static struct sr_member_test stsTopDestEntry_member_test[] =
{
    /* stsTopDestNumber */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTopDestNumber_range)/sizeof(RangeTest_t), /* 1 */
      stsTopDestNumber_range, NULL },

    /* stsTopDestIp */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsTopDestUdpPort */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTopDestUdpPort_range)/sizeof(RangeTest_t), /* 1 */
      stsTopDestUdpPort_range, NULL },

    /* stsTopDestVlanId */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTopDestVlanId_range)/sizeof(RangeTest_t), /* 1 */
      stsTopDestVlanId_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsTopDestEntry family.
 */
const SrTestInfoEntry stsTopDestEntryTestInfo = {
    &stsTopDestEntryGetInfo,
    stsTopDestEntry_member_test,
#ifdef stsTopDestEntry_READ_CREATE
    k_stsTopDestEntry_set_defaults,
#else /* stsTopDestEntry_READ_CREATE */
    NULL,
#endif /* stsTopDestEntry_READ_CREATE */
    k_stsTopDestEntry_test,
    k_stsTopDestEntry_ready,
#ifdef SR_stsTopDestEntry_UNDO
    stsTopDestEntry_undo,
#else /* SR_stsTopDestEntry_UNDO */
    NULL,
#endif /* SR_stsTopDestEntry_UNDO */
    stsTopDestEntry_ready,
    stsTopDestEntry_set,
    stsTopDestEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsTopDestEntry data object.
 *---------------------------------------------------------------------*/
void
stsTopDestEntry_free(stsTopDestEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsTopDestEntryGetInfo.type_table,
                 stsTopDestEntryGetInfo.highest_nominator,
                 stsTopDestEntryGetInfo.valid_offset,
                 stsTopDestEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsTopDestEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsTopDestEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsTopDestEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsTopDestEntry family
 *---------------------------------------------------------------------*/
stsTopDestEntry_t *
Clone_stsTopDestEntry(stsTopDestEntry_t *stsTopDestEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsTopDestEntry,
                         stsTopDestEntryGetInfo.family_size,
                         stsTopDestEntryGetInfo.type_table,
                         stsTopDestEntryGetInfo.highest_nominator,
                         stsTopDestEntryGetInfo.valid_offset,
                         stsTopDestEntryTestInfo.userpart_clone_func,
                         stsTopDestEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsTopDestEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                     doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsTopDestEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsTopDestEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsTopDestEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsTopDestEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsTopDestEntry_set((stsTopDestEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsTrapRegEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsTrapRegEntryTypeTable[];

const SrIndexInfo stsTrapRegEntryIndexInfo[] = {
#ifdef I_stsTrapRegMgrNumber
    { I_stsTrapRegMgrNumber, T_uint, -1 },
#endif /* I_stsTrapRegMgrNumber */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsTrapRegEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsTrapRegEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsTrapRegEntry_t),
    I_stsTrapRegEntry_max,
    (SnmpType *) stsTrapRegEntryTypeTable,
    stsTrapRegEntryIndexInfo,
    (short) offsetof(stsTrapRegEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsTrapRegEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsTrapRegEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                    ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsTrapRegMgrNumber)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsTrapRegEntryGetInfo));
#endif /* all indices are supported */
}

stsTrapRegEntry_t *
new_k_stsTrapRegEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator, int searchType,
                          stsTrapRegEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsTrapRegEntry_get(serialNum, contextInfo, nominator,
                                 searchType, data->stsTrapRegMgrNumber);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsTrapRegEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsTrapRegEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsTrapRegMgrNumber_range[] = { { 1, 10 } };
static RangeTest_t   stsTrapRegMgrUdpPort_range[] = { { 0, 65535 } };
static LengthTest_t  stsTrapRegCommunityStr_len[] = { { 0, 24 } };
static RangeTest_t   stsTrapRegState_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsTrapRegEntry family.
 */
static struct sr_member_test stsTrapRegEntry_member_test[] =
{
    /* stsTrapRegMgrNumber */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTrapRegMgrNumber_range)/sizeof(RangeTest_t), /* 1 */
      stsTrapRegMgrNumber_range, NULL },

    /* stsTrapRegMgrIp */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* stsTrapRegMgrUdpPort */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTrapRegMgrUdpPort_range)/sizeof(RangeTest_t), /* 1 */
      stsTrapRegMgrUdpPort_range, NULL },

    /* stsTrapRegCommunityStr */
    { MINV_LENGTH_TEST, 
      sizeof(stsTrapRegCommunityStr_len)/sizeof(LengthTest_t), /* 1 */
      stsTrapRegCommunityStr_len, DisplayString_test },

    /* stsTrapRegState */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsTrapRegState_range)/sizeof(RangeTest_t), /* 2 */
      stsTrapRegState_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsTrapRegEntry family.
 */
const SrTestInfoEntry stsTrapRegEntryTestInfo = {
    &stsTrapRegEntryGetInfo,
    stsTrapRegEntry_member_test,
#ifdef stsTrapRegEntry_READ_CREATE
    k_stsTrapRegEntry_set_defaults,
#else /* stsTrapRegEntry_READ_CREATE */
    NULL,
#endif /* stsTrapRegEntry_READ_CREATE */
    k_stsTrapRegEntry_test,
    k_stsTrapRegEntry_ready,
#ifdef SR_stsTrapRegEntry_UNDO
    stsTrapRegEntry_undo,
#else /* SR_stsTrapRegEntry_UNDO */
    NULL,
#endif /* SR_stsTrapRegEntry_UNDO */
    stsTrapRegEntry_ready,
    stsTrapRegEntry_set,
    stsTrapRegEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsTrapRegEntry data object.
 *---------------------------------------------------------------------*/
void
stsTrapRegEntry_free(stsTrapRegEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsTrapRegEntryGetInfo.type_table,
                 stsTrapRegEntryGetInfo.highest_nominator,
                 stsTrapRegEntryGetInfo.valid_offset,
                 stsTrapRegEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsTrapRegEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsTrapRegEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsTrapRegEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsTrapRegEntry family
 *---------------------------------------------------------------------*/
stsTrapRegEntry_t *
Clone_stsTrapRegEntry(stsTrapRegEntry_t *stsTrapRegEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsTrapRegEntry,
                         stsTrapRegEntryGetInfo.family_size,
                         stsTrapRegEntryGetInfo.type_table,
                         stsTrapRegEntryGetInfo.highest_nominator,
                         stsTrapRegEntryGetInfo.valid_offset,
                         stsTrapRegEntryTestInfo.userpart_clone_func,
                         stsTrapRegEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsTrapRegEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                     doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsTrapRegEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsTrapRegEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsTrapRegEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsTrapRegEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsTrapRegEntry_set((stsTrapRegEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*
 * The stsClockPriEntryTypeTable array should be located in the
 * k_LarusSts.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType stsClockPriEntryTypeTable[];

const SrIndexInfo stsClockPriEntryIndexInfo[] = {
#ifdef I_stsClockPriIndex
    { I_stsClockPriIndex, T_uint, -1 },
#endif /* I_stsClockPriIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry stsClockPriEntryGetInfo = {
    (SR_KGET_FPTR) new_k_stsClockPriEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(stsClockPriEntry_t),
    I_stsClockPriEntry_max,
    (SnmpType *) stsClockPriEntryTypeTable,
    stsClockPriEntryIndexInfo,
    (short) offsetof(stsClockPriEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the stsClockPriEntry family.
 *---------------------------------------------------------------------*/
VarBind *
stsClockPriEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                     ContextInfo *contextInfo, int serialNum)
{
#if !defined(I_stsClockPriIndex)
    return NULL;
#else /* all indices are supported */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &stsClockPriEntryGetInfo));
#endif /* all indices are supported */
}

stsClockPriEntry_t *
new_k_stsClockPriEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator, int searchType,
                           stsClockPriEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_stsClockPriEntry_get(serialNum, contextInfo, nominator,
                                  searchType, data->stsClockPriIndex);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int stsClockPriEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the stsClockPriEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   stsClockPriIndex_range[] = { { 1, 3 } };
static RangeTest_t   stsClockPriSetting_range[] = { { 1, 3 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * stsClockPriEntry family.
 */
static struct sr_member_test stsClockPriEntry_member_test[] =
{
    /* stsClockPriIndex */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsClockPriIndex_range)/sizeof(RangeTest_t), /* 3 */
      stsClockPriIndex_range, NULL },

    /* stsClockPriSetting */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(stsClockPriSetting_range)/sizeof(RangeTest_t), /* 3 */
      stsClockPriSetting_range, NULL }
};

/*
 * Initialize SrTestInfoEntry for the stsClockPriEntry family.
 */
const SrTestInfoEntry stsClockPriEntryTestInfo = {
    &stsClockPriEntryGetInfo,
    stsClockPriEntry_member_test,
#ifdef stsClockPriEntry_READ_CREATE
    k_stsClockPriEntry_set_defaults,
#else /* stsClockPriEntry_READ_CREATE */
    NULL,
#endif /* stsClockPriEntry_READ_CREATE */
    k_stsClockPriEntry_test,
    k_stsClockPriEntry_ready,
#ifdef SR_stsClockPriEntry_UNDO
    stsClockPriEntry_undo,
#else /* SR_stsClockPriEntry_UNDO */
    NULL,
#endif /* SR_stsClockPriEntry_UNDO */
    stsClockPriEntry_ready,
    stsClockPriEntry_set,
    stsClockPriEntry_cleanup,
    (SR_COPY_FPTR) NULL
};

/*----------------------------------------------------------------------
 * Free the stsClockPriEntry data object.
 *---------------------------------------------------------------------*/
void
stsClockPriEntry_free(stsClockPriEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 stsClockPriEntryGetInfo.type_table,
                 stsClockPriEntryGetInfo.highest_nominator,
                 stsClockPriEntryGetInfo.valid_offset,
                 stsClockPriEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after stsClockPriEntry set/undo
 *---------------------------------------------------------------------*/
static int
stsClockPriEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &stsClockPriEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the stsClockPriEntry family
 *---------------------------------------------------------------------*/
stsClockPriEntry_t *
Clone_stsClockPriEntry(stsClockPriEntry_t *stsClockPriEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return SrCloneFamily(stsClockPriEntry,
                         stsClockPriEntryGetInfo.family_size,
                         stsClockPriEntryGetInfo.type_table,
                         stsClockPriEntryGetInfo.highest_nominator,
                         stsClockPriEntryGetInfo.valid_offset,
                         stsClockPriEntryTestInfo.userpart_clone_func,
                         stsClockPriEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
stsClockPriEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                      doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &stsClockPriEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
stsClockPriEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &stsClockPriEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
stsClockPriEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_stsClockPriEntry_set((stsClockPriEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


