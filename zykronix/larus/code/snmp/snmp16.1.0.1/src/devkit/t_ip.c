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

#include <unistd.h>


#include <errno.h>

#include <sys/types.h>


#include <types.h>


#include <netinet/in.h>

#include <string.h>

#include <sys/socket.h>

#include <arpa/inet.h>
#include <sockLib.h>
#include <hostLib.h>




#include "sr_type.h"
#include "sr_time.h"










#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME

#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "sr_trans.h"




#include "sr_ip.h"




static SR_UINT32       LocalIPAddress = (SR_UINT32)0;

#define SA_CAST(x) (struct sockaddr *)x





 

/*
 * GetLocalIPAddress:
 *
 *   This routine will attempt to determine the local IP address of a device,
 *   and return this value as an unsigned 32-bit integer.  Returned in host
 *   byte order.
 */
SR_UINT32
GetLocalIPAddress()
{
    FNAME("GetLocalIPAddress")
    char hostname[64];

    if (LocalIPAddress != (SR_UINT32)0) {
        return LocalIPAddress;
    }






    if (!gethostname(hostname, 64)) {
        DPRINTF((APTRACE, "%s: hostname is %s\n", Fname, hostname));
        LocalIPAddress = inet_addr(hostname);
        if (LocalIPAddress == (SR_UINT32) -1) {
            LocalIPAddress = hostGetByName(hostname);
            if (LocalIPAddress == ERROR) {
                LocalIPAddress = -1;
            }
        }
        LocalIPAddress = ntohl(LocalIPAddress);
    }








    if (LocalIPAddress == (SR_UINT32) -1) {
        DPRINTF((APERROR, "%s: %s\n%s\n",
		Fname,
	        "Could not determine entity IP address.  This can cause",
                "SNMP communication failure.  Is host system misconfigured?"));
    }


    return LocalIPAddress;
}

/*
 * TranslateAddress
 *
 * Translate a string representing an IP address to an unsigned long
 * representation in HOST byte order.  The string can be the dotted
 * form, or some alias for the host.
 */
SR_UINT32
TranslateAddress(dest)
    char           *dest;
{
    struct hostent *host;
    SR_UINT32   sin_addr;

    sin_addr = inet_addr(dest);


    return(ntohl(sin_addr));
}

/*
 * Convert an IP Address (in the form of an unsigned long) to
 * an OctetString.
 */
OctetString *
IPToOctetString(ipAddr)
   SR_UINT32     ipAddr;
{
   unsigned char buff[4];
 
   buff[0] = (ipAddr >> 24) & 0xff;
   buff[1] = (ipAddr >> 16) & 0xff;
   buff[2] = (ipAddr >>  8) & 0xff;
   buff[3] = (ipAddr >>  0) & 0xff;

   return (MakeOctetString(buff, ((SR_INT32)4)));
}

/*
 * Convert an OctetString to an IP Address (in the form of an unsigned long).
 */
SR_UINT32
OctetStringToIP(os)
   const OctetString *os;
{
   int i;
   SR_UINT32     ipAddr = ((SR_INT32)0);

   for (i = 3; i >= 0; i--) {
       ipAddr |= (SR_UINT32) os->octet_ptr[i] << ((3 - i) * 8);
   }
   return ipAddr;
}

/*
 * Convert a Network Address to an IP Address (in the form of an unsigned long).
 */
SR_UINT32
NetworkAddressToIP(os)
   const OctetString *os;
{
   int i;
   SR_UINT32     ipAddr = ((SR_INT32)0);

   for (i = 4; i >= 1; i--) {
       ipAddr |= (SR_UINT32) os->octet_ptr[i] << ((4 - i) * 8);
   }
   return ipAddr;
}

/*
 * Convert an OctetString to an unsigned long.
 * This function will take an OctetString of length <= 4 and
 * convert it to an unsigned long. This function is similiar
 * to OctetStringToIP(), but it also checks the length.
 */
SR_UINT32
OctetStringToUlong (os)
    const OctetString *os;
{
    int i, len;
    SR_UINT32 ul = ((SR_INT32)0);

    /*
     * limit the conversion to 4 octets [array elements 0-3]
     */
    len = os->length;
    if (len > 4) {
        len = 4;
    }
 
    for (i = 0; i < len; i++) {
        ul = (ul << 8) | os->octet_ptr[i];
    }
    return ul;
}

SR_INT32
ConvToken_IP_TAddr(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    SR_INT32 s1, s2, s3, s4, port;
    short unsigned int s_port;
    int cc;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            cc = sscanf(*token, INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT":"INT32_FORMAT,
                        &s1, &s2, &s3, &s4, &port);
            if (cc < 4) {
                return -1;
            }
            if (cc == 4) {
                port = (SR_INT32)0;
            }
            if ((s1 > 255) ||
                (s1 < 0)   ||
                (s2 > 255) ||
                (s2 < 0)   ||
                (s3 > 255) ||
                (s3 < 0)   ||
                (s4 > 255) ||
                (s4 < 0)   ||
                (port < 0) ||
                (port > 65535)) {
                return -1;
            }
            *local = MakeOctetString(NULL, 6);
            if (*local == NULL) {
                return -1;
            }
            s_port = (short unsigned int)port;
            (*local)->octet_ptr[0] = (unsigned char)s1;
            (*local)->octet_ptr[1] = (unsigned char)s2;
            (*local)->octet_ptr[2] = (unsigned char)s3;
            (*local)->octet_ptr[3] = (unsigned char)s4;
            (*local)->octet_ptr[4] = (unsigned char)((int) (s_port & 0xff00) >> 8);
            (*local)->octet_ptr[5] = (unsigned char)(s_port & 0xff);
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            if ((*local)->length != 6) {
                strcpy(*token, "0.0.0.0:0");
                return 0;
            } else {
                s1 = (SR_INT32)(*local)->octet_ptr[0];
                s2 = (SR_INT32)(*local)->octet_ptr[1];
                s3 = (SR_INT32)(*local)->octet_ptr[2];
                s4 = (SR_INT32)(*local)->octet_ptr[3];
                s_port = (((short unsigned int)(*local)->octet_ptr[4]) << 8) +
                         ((short unsigned int)(*local)->octet_ptr[5]);
                port = (SR_UINT32)s_port;
                sprintf(*token, "%d.%d.%d.%d:%d", (int)s1, (int)s2, (int)s3, (int)s4, (int)port);
                return 0;
            }
        default:
            return -1;
    }
}

SR_INT32
ConvToken_IP_TAddrMask(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    SR_INT32 s1, s2, s3, s4, port;
    short unsigned int s_port;
    int cc;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if (strcmp(*token, "-") != 0) {
                cc = sscanf(*token, INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT":"INT32_FORMAT,
                        &s1, &s2, &s3, &s4, &port);
                if (cc < 4) {
                    return -1;
                }
                if (cc == 4) {
                    port = (SR_INT32)0;
                }
                if ((s1 > 255) ||
                    (s1 < 0)   ||
                    (s2 > 255) ||
                    (s2 < 0)   ||
                    (s3 > 255) ||
                    (s3 < 0)   ||
                    (s4 > 255) ||
                    (s4 < 0)   ||
                    (port < 0) ||
                    (port > 65535)) {
                    return -1;
            }
                *local = MakeOctetString(NULL, 6);
                if (*local == NULL) {
                    return -1;
                }
                s_port = (short unsigned int)port;
                (*local)->octet_ptr[0] = (unsigned char)s1;
                (*local)->octet_ptr[1] = (unsigned char)s2;
                (*local)->octet_ptr[2] = (unsigned char)s3;
                (*local)->octet_ptr[3] = (unsigned char)s4;
                (*local)->octet_ptr[4] = (unsigned char)((int) (s_port & 0xff00) >> 8);
                (*local)->octet_ptr[5] = (unsigned char)(s_port & 0xff);
                return 0;
            }
            else {
                *local = MakeOctetString(NULL, 0);
                if (*local == NULL) {
                    return -1;
                }
                return 0;
            }
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            if ((*local)->length == 0) {
                strcpy(*token, "-");
                return 0;
            } else if ((*local)->length != 6) {
                strcpy(*token, "0.0.0.0:0");
                return 0;
            } else {
                s1 = (SR_INT32)(*local)->octet_ptr[0];
                s2 = (SR_INT32)(*local)->octet_ptr[1];
                s3 = (SR_INT32)(*local)->octet_ptr[2];
                s4 = (SR_INT32)(*local)->octet_ptr[3];
                s_port = (((short unsigned int)(*local)->octet_ptr[4]) << 8) +
                         ((short unsigned int)(*local)->octet_ptr[5]);
                port = (SR_UINT32)s_port;
                sprintf(*token, "%d.%d.%d.%d:%d", (int)s1, (int)s2, (int)s3, (int)s4, (int)port);
                return 0;
            }
        default:
            return -1;
    }
}

void
CopyIpTransportInfo(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    dst->type = src->type;
    dst->flags = src->flags;
    dst->fd = src->fd;
    dst->t_ipAddr = src->t_ipAddr;
    dst->t_ipPort = src->t_ipPort;
}

void
CopyIpTransportAddr(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    dst->t_ipAddr = src->t_ipAddr;
    dst->t_ipPort = src->t_ipPort;
}

int
CmpIpTransportInfo(ti1, ti2)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
{
    if (ti1->t_ipAddr != ti2->t_ipAddr) {
        return -1;
    }
    if (ti1->t_ipPort != ti2->t_ipPort) {
        return -1;
    }
    return 0;
}

int
CmpIpTransportInfoWithMask(ti1, ti2, mask)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
    const TransportInfo *mask;
 {
     if ((ti1->t_ipAddr & mask->t_ipAddr) != 
	 (ti2->t_ipAddr & mask->t_ipAddr)) {
         return -1;
     }
     if ((ti1->t_ipPort & mask->t_ipPort) !=
	 (ti2->t_ipPort & mask->t_ipPort)) {
         return -1;
     }
     return 0;
}


OctetString *
IpTransportInfoToOctetString(ti)
    const TransportInfo *ti;
{
    OctetString *os;
 
    os = MakeOctetString(NULL, 6);
    if (os == NULL) {
        return NULL;
    }
    memcpy(os->octet_ptr, &ti->t_ipAddr, 4);
    memcpy(os->octet_ptr + 4, &ti->t_ipPort, 2);
    return os;
}
 
int
OctetStringToIpTransportInfo(ti, os)
    TransportInfo *ti;
    const OctetString *os;
{
    if (os == NULL) {
        return -1;
    }
    if (os->length != 6) {
        return -1;
    }
    ti->type = SR_IP_TRANSPORT;
    memcpy(&ti->t_ipAddr, os->octet_ptr, 4);
    memcpy(&ti->t_ipPort, os->octet_ptr + 4, 2);
    return 0;
}

int
TMaskOctetStringToIpTransportInfo(ti, os)
    TransportInfo *ti;
    const OctetString *os;
{
    if (os == NULL) {
        return -1;
    }
    if (os->length == 0) {
        ti->type = SR_IP_TRANSPORT;
        ti->t_ipAddr = inet_addr("255.255.255.255");
        ti->t_ipPort = 0;
        return 0;
    }
    if (os->length != 6) {
        return -1;
    }
    ti->type = SR_IP_TRANSPORT;
    memcpy(&ti->t_ipAddr, os->octet_ptr, 4);
    memcpy(&ti->t_ipPort, os->octet_ptr + 4, 2);
    return 0;
}



int
SendToIpTransport(data, len, ti)
    const char *data;
    int len;
    const TransportInfo *ti;
{
    struct sockaddr_in addr;

#ifdef SR_CLEAR_AUTOS
    memset(&addr, 0, sizeof(addr));
#endif	/* SR_CLEAR_AUTOS */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ti->t_ipAddr;
    addr.sin_port = ti->t_ipPort;


    return sendto(ti->fd, (char *)data, len, 0, SA_CAST(&addr), sizeof(addr));
}

int
ReceiveFromIpTransport(data, len, ti)
    char *data;
    int len;
    TransportInfo *ti;
{
    int status;
    struct sockaddr_in from;
     int             fromlen = sizeof(from);


    status = recvfrom(ti->fd, data, len, 0, SA_CAST(&from), &fromlen);

    ti->t_ipAddr = from.sin_addr.s_addr;
    ti->t_ipPort = from.sin_port;


    return status;
}

char *
FormatIpTransportString(buf, len, ti)
    char *buf;
    int len;
    const TransportInfo *ti;
{
    SR_UINT32 addr = ntohl(ti->t_ipAddr);
    unsigned long port = (unsigned long)ntohs(ti->t_ipPort);

    if (len < 22) {
        strcpy(buf, "");
    } else {
        snprintf(buf, len, "%lu.%lu.%lu.%lu:%lu",
                (unsigned long)((addr & 0xff000000) >> 24),
                (unsigned long)((addr & 0xff0000) >> 16),
                (unsigned long)((addr & 0xff00) >> 8),
                (unsigned long)(addr & 0xff),
                port);
    }
    return buf;
}


int
OpenIpTransport(ti)
    TransportInfo *ti;
{
#ifdef SR_DEBUG
#define OPEN_IP_TRANSPORT_BUFLEN 22
    static char fmt[OPEN_IP_TRANSPORT_BUFLEN];

    FormatIpTransportString(fmt, OPEN_IP_TRANSPORT_BUFLEN,
                            (const TransportInfo *) ti);
    DPRINTF((APTRACE, "Opening socket for IP transport %s\n", fmt));
#endif /* SR_DEBUG */
    ti->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ti->fd < 0) {
        DPRINTF((APERROR, "Could not open socket for IP\n"));
        return -1;
    }
    return 0;
}

int
BindIpTransport(ti, tries)
    TransportInfo *ti;
    int tries;
{
    struct sockaddr_in addr;
    int status;

#ifdef SR_CLEAR_AUTOS
    memset(&addr, 0, sizeof(addr));
#endif	/* SR_CLEAR_AUTOS */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ti->t_ipAddr;
    addr.sin_port = ti->t_ipPort;

    DPRINTF((APTRACE, "Binding to port %u\n", ntohs(ti->t_ipPort)));
    while (tries != 0) {
        DPRINTF((APVERBOSE, "Attempt #%u to bind to socket\n", tries));
        status = bind(ti->fd, SA_CAST(&addr), sizeof(addr));
        if (status >= 0) {
            return 0;
        }
        if (tries > 0) {
            tries--;
        }
    }
    DPRINTF((APWARN, "Cannot bind to socket: %s\n", sys_errname(errno)));
    return -1;
}

void
CloseIpTransport(ti)
    TransportInfo *ti;
{
    close(ti->fd);
}
