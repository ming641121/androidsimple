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
 * -apo -f mibcompflags -userpart -v_stubs_only -row_status 
 * 
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

static const SR_UINT32 snmpEngine_last_sid_array[] = {
    1, /* snmpEngineID */
    2, /* snmpEngineBoots */
    3, /* snmpEngineTime */
    4  /* snmpEngineMaxMessageSize */
};

const SnmpType snmpEngineTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(snmpEngine_t, snmpEngineID), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(snmpEngine_t, snmpEngineBoots), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(snmpEngine_t, snmpEngineTime), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(snmpEngine_t, snmpEngineMaxMessageSize), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry snmpEngineGetInfo = {
    (SR_KGET_FPTR) new_k_snmpEngine_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(snmpEngine_t),
    I_snmpEngine_max,
    (SnmpType *) snmpEngineTypeTable,
    NULL,
    (short) offsetof(snmpEngine_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the snmpEngine family.
 *---------------------------------------------------------------------*/
VarBind *
snmpEngine_get(OID *incoming, ObjectInfo *object, int searchType,
               ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpEngineGetInfo));
}

snmpEngine_t *
new_k_snmpEngine_get(int serialNum, ContextInfo *contextInfo,
                     int nominator, int searchType,
                     snmpEngine_t *data)
{
    return k_snmpEngine_get(serialNum, contextInfo, nominator);
}

