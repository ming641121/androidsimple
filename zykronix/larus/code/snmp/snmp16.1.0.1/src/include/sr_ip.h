/*
 *
 * Copyright (C) 1996-2003 by SNMP Research, Incorporated.
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

#ifndef SR_SR_IP_H
#define SR_SR_IP_H

#ifdef  __cplusplus
extern "C" {
#endif

SR_UINT32 GetLocalIPAddress(void);

void CopyIpTransportInfo(TransportInfo *dst, const TransportInfo *src);

void CopyIpTransportAddr(TransportInfo *dst, const TransportInfo *src);

int CmpIpTransportInfo(const TransportInfo *ti1, const TransportInfo *ti2);

int CmpIpTransportInfoWithMask(
    const TransportInfo *ti1,
    const TransportInfo *ti2,
    const TransportInfo *mask);
 
OctetString *IpTransportInfoToOctetString(const TransportInfo *ti);

int OctetStringToIpTransportInfo(TransportInfo *ti, const OctetString *os);

int TMaskOctetStringToIpTransportInfo(TransportInfo *ti, const OctetString *os);

int SendToIpTransport(const char *data, int len, const TransportInfo *ti);

int ReceiveFromIpTransport(char *data, int len, TransportInfo *ti);

char *FormatIpTransportString(char *buf, int len, const TransportInfo *ti);

int OpenIpTransport(TransportInfo *ti);

int BindIpTransport(TransportInfo *ti, int tries);

void CloseIpTransport(TransportInfo *ti);

SR_INT32 ConvToken_IP_TAddr(SR_INT32 direction, char **token, void *value);

SR_INT32 ConvToken_IP_TAddrMask(SR_INT32 direction, char **token, void *value);

SR_UINT32     TranslateAddress(
    char           *dest);

OctetString *IPToOctetString(SR_UINT32 IPAddr);

SR_UINT32 OctetStringToIP(const OctetString *os);

SR_UINT32 NetworkAddressToIP(const OctetString *os);

SR_UINT32 OctetStringToUlong(const OctetString *os);

#ifdef  __cplusplus
}
#endif

#endif	/* SR_SR_IP_H */
