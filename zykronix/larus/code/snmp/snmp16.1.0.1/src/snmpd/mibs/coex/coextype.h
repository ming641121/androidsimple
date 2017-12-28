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
 * -c coex snmpCommunityTable snmpTrapAddress snmpTrapCommunity -apo -row_status -userpart -separate_type_file 
 */

/*
 * automatically generated by the mib compiler -- do not edit
 */

#ifndef SR_COEXTYPE_H
#define SR_COEXTYPE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "userpart.h"


/*
 * The snmpCommunityMIBObjects family. 
 */


typedef
struct _snmpCommunityMIBObjects_t {
        SR_UINT32       snmpTrapAddress;
        OctetString     *snmpTrapCommunity;
#ifdef U_snmpCommunityMIBObjects
U_snmpCommunityMIBObjects
#endif /* U_snmpCommunityMIBObjects */
        char            valid[1];
} snmpCommunityMIBObjects_t;


/*
 * The snmpCommunityEntry family. 
 */


typedef
struct _snmpCommunityEntry_t {
        OctetString     *snmpCommunityIndex;
        OctetString     *snmpCommunityName;
        OctetString     *snmpCommunitySecurityName;
        OctetString     *snmpCommunityContextEngineID;
        OctetString     *snmpCommunityContextName;
        OctetString     *snmpCommunityTransportTag;
        SR_INT32        snmpCommunityStorageType;
        SR_INT32        snmpCommunityStatus;
        long            RowStatusTimerId;
#ifdef U_snmpCommunityEntry
U_snmpCommunityEntry
#endif /* U_snmpCommunityEntry */
        char            valid[2];
} snmpCommunityEntry_t;


#ifdef __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SR_COEXTYPE_H */
