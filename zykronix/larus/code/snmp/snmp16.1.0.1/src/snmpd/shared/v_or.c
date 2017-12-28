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
 * -apo -f mibcompflags -userpart -v_stubs_only 
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>


#include <string.h>


#include <stddef.h>
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

static const SR_UINT32 sysOREntry_last_sid_array[] = {
    1, /* sysORIndex */
    2, /* sysORID */
    3, /* sysORDescr */
    4  /* sysORUpTime */
};

const SnmpType sysOREntryTypeTable[] = {
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE, offsetof(sysOREntry_t, sysORIndex), 0 },
    { OBJECT_ID_TYPE, SR_READ_ONLY, offsetof(sysOREntry_t, sysORID), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(sysOREntry_t, sysORDescr), -1 },
    { TIME_TICKS_TYPE, SR_READ_ONLY, offsetof(sysOREntry_t, sysORUpTime), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrIndexInfo sysOREntryIndexInfo[] = {
#ifdef I_sysORIndex
    { I_sysORIndex, T_uint, -1 },
#endif /* I_sysORIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry sysOREntryGetInfo = {
    (SR_KGET_FPTR) new_k_sysOREntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(sysOREntry_t),
    I_sysOREntry_max,
    (SnmpType *) sysOREntryTypeTable,
    sysOREntryIndexInfo,
    (short) offsetof(sysOREntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the sysOREntry family.
 *---------------------------------------------------------------------*/
VarBind *
sysOREntry_get(OID *incoming, ObjectInfo *object, int searchType,
               ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_sysORIndex) )
    return NULL;
#else	/* ! ( defined(I_sysORIndex) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &sysOREntryGetInfo));
#endif	/* ! ( defined(I_sysORIndex) ) */
}

sysOREntry_t *
new_k_sysOREntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator, int searchType,
                     sysOREntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_sysOREntry_get(serialNum, contextInfo, nominator,
                            searchType, data->sysORIndex);
}


