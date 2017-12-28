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


#include <stdlib.h>


#include <errno.h>

#include <sys/types.h>

#include <types.h>




#include <string.h>



#include <netinet/in.h>





#include "sr_snmp.h"
#include "sr_trans.h"
#include "snmpid.h"
#include "diag.h"
SR_FILENAME

#ifdef SR_DEBUG
/* for ConvToken */
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#endif /* SR_DEBUG */

#ifdef SR_IP
#include "sr_ip.h"    /* for GetLocalIPAddress() */
#endif        /* SR_IP */



static OctetString *simple_snmpEngineID = NULL;

/*
 * SrGetSnmpID:  Determines an engine's snmpID or snmpEngineID value.
 */
OctetString *
SrGetSnmpID(
    SR_UINT32 algorithm,
    void *data)
{
    FNAME("SrGetSnmpID")
    OctetString *snmpID;
#ifdef SR_DEBUG
    char *token = NULL;
#endif /* SR_DEBUG */


    /*
     * instead of generating snmpEngineID, just return 
     * simple_snmpEngineID if it is not NULL.
     */
    switch (algorithm) {
        case SR_SNMPID_ALGORITHM_SIMPLE:
        default:
	    if (simple_snmpEngineID != NULL){
	       snmpID = CloneOctetString(simple_snmpEngineID);
	       if (snmpID == NULL) {
                   DPRINTF((APERROR, 
		       "%s: can not clone snmpID/snmpEngineID\n", Fname));
	       } 
	       else {
		   return snmpID;
	       }
	    }
            break;
    }

    snmpID = MakeOctetString(NULL, 12);
    if (snmpID == NULL) {
        DPRINTF((APERROR, "%s:  CANNOT ALLOCATE snmpID/snmpEngineID\n", Fname));
        return NULL;
    }

    /* IANA_ENTERPRISE_NUMBER defined in include/features.h */
    *(SR_UINT32 *)(snmpID->octet_ptr) = htonl(IANA_ENTERPRISE_NUMBER);

    switch (algorithm) {
        case SR_SNMPID_ALGORITHM_SIMPLE:
            snmpID->octet_ptr[4] = (unsigned char)algorithm;

            if (data == SR_SNMPID_ALGORITHM_SIMPLE_AGT) {
                snmpID->octet_ptr[5] = (unsigned char)0;
                *(unsigned short *)(snmpID->octet_ptr + 6) =
                                htons((unsigned short)GetSNMPPort());
            } else if (data == SR_SNMPID_ALGORITHM_SIMPLE_MGR) {
                snmpID->octet_ptr[5] = (unsigned char)1;
                *(unsigned short *)(snmpID->octet_ptr + 6) =
                                htons((unsigned short)GetSNMPTrapPort());
            } 
            else {
                DPRINTF((APERROR, "%s:  BAD ALGORITHM DATA\n", Fname));
                *(unsigned short *)(snmpID->octet_ptr + 6) =
                                htons((unsigned short)GetSNMPPort());
            }


            *(SR_UINT32 *)(snmpID->octet_ptr + 8) =
                htonl(GetLocalIPAddress());
            if (simple_snmpEngineID != NULL) {
		FreeOctetString(simple_snmpEngineID);
            }
	    simple_snmpEngineID = CloneOctetString(snmpID);
#ifdef SR_DEBUG
            ConvToken_textOctetString(PARSER_CONVERT_TO_TOKEN, &token, &snmpID);
            if (token != NULL) {
                DPRINTF((APTRACE|APCONFIG|APVERBOSE,
                    "%s: localSnmpID is %s\n", Fname, token));
                free(token);
                token = NULL;
            } else {
                DPRINTF((APWARN,
                    "%s: could not convert localSnmpID for debug\n",
                    Fname, token));
            }
#endif /* SR_DEBUG */
            break;
        default:
            DPRINTF((APERROR, "%s:  BAD ALGORITHM\n", Fname));
            break;
    }

    return snmpID;
}

/*
 * SrAssignSnmpID:  
 *      Assign an engine's static snmpEngineID value explicitly.
 *      assigned snmpEngineID won't be changed during run time
 *   return value: success  0
 *                 fail    -1
 */
int SrAssignSnmpID(OctetString *snmpID)
{
    FNAME("SrAssignSnmpID")
    if (simple_snmpEngineID != NULL) {
	FreeOctetString(simple_snmpEngineID);
	simple_snmpEngineID = NULL;
    }

    simple_snmpEngineID = CloneOctetString(snmpID);

    if (simple_snmpEngineID == NULL) {
        DPRINTF((APERROR, "%s: could not assign snmpEngineID\n", Fname));
	return -1;
    }
    return 0;
}
