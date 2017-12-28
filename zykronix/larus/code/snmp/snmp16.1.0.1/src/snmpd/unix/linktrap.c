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


/*
 * This file contains special versions of the functions
 * send_linkUp_trap() and send_linkDown_trap() which do NOT
 * rely upon i_GetVar().  These are provided for releases
 * prior to and including 12.3.x.x.
 */

#include "sr_conf.h"

#include <stdio.h>
#include "context.h"
#include "method.h"
#include "diag.h"

VarBind *
build_ifIndex_VarBindList(ifIndex_0)
    SR_INT32 ifIndex_0;
{
    OID             *object = NULL;
    OID             *instance = NULL;
    OID             inst;
    int             index;
    SR_UINT32       buffer[1];
    VarBind         *vb = NULL;
    void            *val = NULL;

    /* build the object portion of the OID for MakeVarBindWithValue() */
    /* "1.3.6.1.2.1.2.2.1.1" = ifIndex */
    object = MakeOIDFromDot("1.3.6.1.2.1.2.2.1.1");
    if (object == NULL) {
        return NULL;
    }

    /* build the instance portion of the OID for MakeVarBindWithValue() */
    index = 0;
    inst.oid_ptr = buffer;
    inst.oid_ptr[index++] = (unsigned long) ifIndex_0;
    inst.length = index;
    instance = &inst;

    /* select a value for MakeVarBindWithValue() */
    val = (void *) &ifIndex_0;

    /* build the VarBind for do_trap() */
    vb = MakeVarBindWithValue(object, instance, INTEGER_TYPE, val);
    FreeOID(object);
    if (vb == NULL) {
	return NULL;
    } else {
	vb->next_var = NULL; /* terminate the VarBindList */
        return vb;
    }
}

int
send_linkUp_trap(ifIndex_0, add_vblist, contextInfo)
    SR_INT32 ifIndex_0;
    VarBind *add_vblist;
    ContextInfo *contextInfo;
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;

    if ((vb = build_ifIndex_VarBindList(ifIndex_0)) != NULL) {
        /* "1.3.6.1.6.3.1.1.5.4" = snmpTraps.4 */
        DPRINTF((APTRAP, "Sending linkUp trap\n"));
        do_trap(3, 0, vb, enterprise, "1.3.6.1.6.3.1.1.5.4");
    } else {
        DPRINTF((APTRAP, "Failure sending linkUp trap\n"));
    }
 
    return 1;
}

int
send_linkDown_trap(ifIndex_0, add_vblist, contextInfo)
    SR_INT32 ifIndex_0;
    VarBind *add_vblist;
    ContextInfo *contextInfo;
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;

    if ((vb = build_ifIndex_VarBindList(ifIndex_0)) != NULL) {
        /* "1.3.6.1.6.3.1.1.5.3" = snmpTraps.3 */
        DPRINTF((APTRAP, "Sending linkDown trap\n"));
        do_trap(2, 0, vb, enterprise, "1.3.6.1.6.3.1.1.5.3");
    } else {
        DPRINTF((APTRAP, "Failure sending linkDown trap\n"));
    }

    return 1;
}

