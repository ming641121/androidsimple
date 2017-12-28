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

#include <string.h>


#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "lookup.h"
#include "v2table.h"
#include "sr_cfg.h"
#include "scan.h"
#include "diag.h"
SR_FILENAME

#include "sitedefs.h"


int ipforwarding_state SR_PROTOTYPE((void));



#define SYSSIZE 255

system_t        systemData;

const SnmpType sysDescrType[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysDescrConverters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */

const SnmpType sysObjectIDType[] = {
    { OBJECT_ID_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysObjectIDConverters[] = {
    { ConvToken_OID, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */
const SnmpType sysLocationType[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysLocationConverters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */
const SnmpType sysContactType[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysContactConverters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */

const SnmpType sysNameType[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysNameConverters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};
#endif	/* SR_CONFIG_FP */

#ifdef SR_CONFIG_FP
const PARSER_RECORD_TYPE sysDescrRecord = {
    PARSER_SCALAR, "sysDescr ", sysDescrConverters, sysDescrType,
    NULL, &systemData.sysDescr, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE sysObjectIDRecord = {
    PARSER_SCALAR, "sysObjectID ", sysObjectIDConverters, sysObjectIDType,
    NULL, &systemData.sysObjectID, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE sysLocationRecord = {
    PARSER_SCALAR, "sysLocation ", sysLocationConverters, sysLocationType,
    NULL, &systemData.sysLocation, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE sysContactRecord = {
    PARSER_SCALAR, "sysContact ", sysContactConverters, sysContactType,
    NULL, &systemData.sysContact, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE sysNameRecord = {
    PARSER_SCALAR, "sysName ", sysNameConverters, sysNameType,
    NULL, &systemData.sysName, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#endif	/* SR_CONFIG_FP */


int
k_system_initialize(void)
{
    SR_UINT32 sids[2];

    sids[0] = (SR_UINT32)0;
    sids[1] = (SR_UINT32)0;
    systemData.sysDescr = MakeOctetString(NULL, 0);
#ifndef SYS_OBJECT_ID
    systemData.sysObjectID = MakeOID(sids, 2);
#else /* SYS_OBJECT_ID */
    systemData.sysObjectID = MakeOIDFromDot(SYS_OBJECT_ID);
#endif /* SYS_OBJECT_ID */
    systemData.sysUpTime = (SR_UINT32)0;
    systemData.sysContact = MakeOctetString(NULL, 0);
    systemData.sysName = MakeOctetString(NULL, 0);
    systemData.sysLocation = MakeOctetString(NULL, 0);
    systemData.sysServices = (SR_INT32)72;
#ifdef SR_SYS_OR_TABLE
    systemData.sysORLastChange = (SR_UINT32)0;
#endif /* SR_SYS_OR_TABLE */
    return 1;
}

system_t *
k_system_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    int             ipforwarding;

    ipforwarding = ipforwarding_state();

    if (ipforwarding == 1) {
	systemData.sysServices |= (1 << (3 - 1));  /* %%% MANIFEST CONSTANT */
    }
    else if (ipforwarding == 0) {
	systemData.sysServices &= ~(1 << (3 - 1));
    }
    else {
	return NULL;
    }

    SET_ALL_VALID(systemData.valid);
    systemData.sysUpTime = GetTimeNow();
    return (&systemData);
}

#ifdef SETS
int
k_system_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
#if ((defined(I_sysContact) || defined(I_sysName)) || defined(I_sysLocation))


    switch (object->nominator) {
#ifdef I_sysContact
    case I_sysContact:
#endif
#ifdef I_sysName
    case I_sysName:
#endif
#ifdef I_sysLocation
    case I_sysLocation:
#endif
	if ( (value->os_value->length > SYSSIZE) || 
             (value->os_value->length < 0) ){
	    return (WRONG_LENGTH_ERROR);
	}
        if (!IsDisplayString(value->os_value)) {
	    return (WRONG_VALUE_ERROR);
	}
	break;
    }
#endif	/* ((defined(I_sysContact) || defined(I_sysName)) || defined(I_sysLocation)) */
    return NO_ERROR;
}

int
k_system_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_system_set(data, contextInfo, function)
   system_t *data;
   ContextInfo *contextInfo;
   int function;
{
    FNAME("k_system_set")

    if (VALID(I_sysName, data->valid)) {
        if (systemData.sysName != NULL) {
            FreeOctetString(systemData.sysName);
        }
        if ((systemData.sysName = CloneOctetString(data->sysName)) == NULL) {
            DPRINTF((APERROR, "%s: Could not set sysName.\n", Fname));
            return (GEN_ERROR);
        }
    }
    if (VALID(I_sysContact, data->valid)) {
        if (systemData.sysContact != NULL) {
            FreeOctetString(systemData.sysContact);
        }
        if ((systemData.sysContact = 
             CloneOctetString(data->sysContact)) == NULL) {
            DPRINTF((APERROR, "%s: Could not set sysContact.\n", Fname));
            return (GEN_ERROR);
        }
    }
    if (VALID(I_sysLocation, data->valid)) {
        if (systemData.sysLocation != NULL) {
            FreeOctetString(systemData.sysLocation);
        }
        if ((systemData.sysLocation = 
             CloneOctetString(data->sysLocation)) == NULL) {
            DPRINTF((APERROR, "%s: Could not set sysLocation.\n", Fname));
            return (GEN_ERROR);
        }
    }
    writeConfigFileFlag = TRUE;
    return (NO_ERROR);
}

int
system_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    int             state = SR_ADD_MODIFY;
    system_t       *undodata = (system_t *) doCur->undodata;


    if (undodata == NULL) {
        return UNDO_FAILED_ERROR;
    }

    return k_system_set(undodata, contextInfo, state);
}


#endif /* SETS */
