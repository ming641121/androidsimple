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

#include <errno.h>

#include <string.h>


#include "sr_snmp.h"
#include "sr_trans.h"
#include "diag.h"
SR_FILENAME

#include "oid_lib.h"

#ifdef SR_IP
#include "sr_ip.h"
#endif /* SR_IP */



static const SR_UINT32 rfc1157Domain_sids[] = {1, 3, 6, 1, 6, 2, 1, 1};
static const OID rfc1157Domain_OID =
    { OIDSIZE(rfc1157Domain_sids), (SR_UINT32 *) rfc1157Domain_sids };
const OID      *rfc1157DomainOID = &rfc1157Domain_OID;

static const SR_UINT32 snmpUDPDomain_sids[] = {1, 3, 6, 1, 6, 1, 1};
static const OID snmpUDPDomain_OID =
    { OIDSIZE(snmpUDPDomain_sids), (SR_UINT32 *) snmpUDPDomain_sids };
const OID      *snmpUDPDomainOID = &snmpUDPDomain_OID;


void
CopyTransportInfo(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    switch (src->type) {
#ifdef SR_IP
        case SR_IP_TRANSPORT:
            CopyIpTransportInfo(dst, src);
            break;
#endif /* SR_IP */
        default:
            DPRINTF((APERROR, "attempt to copy unknown transport type info\n"));
            break;
    }
}

void
CopyTransportAddr(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    switch (src->type) {
#ifdef SR_IP
        case SR_IP_TRANSPORT:
            CopyIpTransportAddr(dst, src);
            break;
#endif /* SR_IP */
        default:
            DPRINTF((APERROR, "attempt to copy unknown transport type addr\n"));
            break;
    }
}

int
CmpTransportInfo(ti1, ti2)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
{
    if (ti1->type == ti2->type) {
        switch (ti1->type) {
#ifdef SR_IP
            case SR_IP_TRANSPORT:
                return CmpIpTransportInfo(ti1, ti2);
#endif /* SR_IP */
            default:
                DPRINTF((APERROR, "attempt to compare unknown transport type\n"));
                break;
        }
    }

    return -1;
}

int
CmpTransportInfoWithMask(ti1, ti2, mask)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
    const TransportInfo *mask;
{
    if (ti1->type == ti2->type) {
	if (ti1->type != mask->type) {
	    return CmpTransportInfo(ti1, ti2);
	}
	switch (ti1->type) {
#ifdef SR_IP
	case SR_IP_TRANSPORT:
	    return CmpIpTransportInfoWithMask(ti1, ti2, mask);
#endif /* SR_IP */
	default:
	    DPRINTF((APERROR, "attempt to compare unknown transport type\n"));
	    break;
	}
    }
    
    return -1;
}

int
TransportInfoToOIDAndOctetString(ti, domain, address)
    const TransportInfo *ti;
    OID **domain;
    OctetString **address;
{
    switch (ti->type) {
#ifdef SR_IP
        case SR_IP_TRANSPORT:
            *domain = CloneOID(snmpUDPDomainOID);
            *address = IpTransportInfoToOctetString(ti);
            break;
#endif /* SR_IP */
        default:
            DPRINTF((APERROR, "attempt to convert unknown transport type from oid to octetstring\n"));
            return -1;
    }
    return 0;
}

int
OIDAndOctetStringToTransportInfo(ti, domain, address)
    TransportInfo *ti;
    OID *domain;
    const OctetString *address;
{
#ifdef SR_IP
    if (!CmpOID(domain, snmpUDPDomainOID)) {
        return OctetStringToIpTransportInfo(ti, address);
    }
#endif /* SR_IP */


    return -1;
}

int
SendToTransport(data, len, ti)
    const char *data;
    int len;
    const TransportInfo *ti;
{
    switch (ti->type) {
#ifdef SR_IP
        case SR_IP_TRANSPORT:
            return SendToIpTransport(data, len, ti);
#endif /* SR_IP */
        default:
            DPRINTF((APERROR, "attempt to send to unknown transport type\n"));
    }
    return -1;
}

int
TMaskOIDAndOctetStringToTransportInfo(ti, domain, address)
    TransportInfo *ti;
    OID *domain;
    const OctetString *address;
{
#ifdef SR_IP
    if (!CmpOID(domain, snmpUDPDomainOID)) {
        return TMaskOctetStringToIpTransportInfo(ti, address);
    }
#endif /* SR_IP */


    return -1;
}

int
ReceiveFromTransport(data, len, ti)
    char *data;
    int len;
    TransportInfo *ti;
{
    switch (ti->type) {
#ifdef SR_IP
        case SR_IP_TRANSPORT:
            return ReceiveFromIpTransport(data, len, ti);
#endif /* SR_IP */
        default:
            DPRINTF((APERROR, "attempt to receive from unknown transport type\n"));
    }
    return -1;
}

char *
FormatTransportString(buf, len, ti)
    char *buf;
    int len;
    const TransportInfo *ti;
{
    switch (ti->type) {
#ifdef SR_IP
        case SR_IP_TRANSPORT:
            return FormatIpTransportString(buf, len, ti);
#endif /* SR_IP */
        default:
            DPRINTF((APERROR, "attempt to format unknown transport type\n"));
    }
    strcpy(buf, "");
    return buf;
}

int
OpenTransport(ti)
    TransportInfo *ti;
{
    switch (ti->type) {
#ifdef SR_IP
        case SR_IP_TRANSPORT:
            return OpenIpTransport(ti);
#endif /* SR_IP */
        default:
            DPRINTF((APERROR, "attempt to open unknown transport type\n"));
    }
    return -1;
}

int
BindTransport(ti, tries)
    TransportInfo *ti;
    int tries;
{
    switch (ti->type) {
#ifdef SR_IP
        case SR_IP_TRANSPORT:
            return BindIpTransport(ti, tries);
#endif /* SR_IP */
        default:
            DPRINTF((APERROR, "attempt to bind unknown transport type\n"));
    }
    return -1;
}

void
CloseTransport(ti)
    TransportInfo *ti;
{
    switch (ti->type) {
#ifdef SR_IP
        case SR_IP_TRANSPORT:
            CloseIpTransport(ti);
            break;
#endif /* SR_IP */
        default:
            DPRINTF((APERROR, "attempt to close unknown transport type\n"));
            break;
    }
}
