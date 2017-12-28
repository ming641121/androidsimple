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
 * -o core -apo -row_status -userpart -separate_type_file 
 */

/*
 * If your implementation does NOT support a MIB variable, add a line in
 * the undefs.h file containing #undef I_that-variable-name.  For example,
 * if you do not support the sysDescr variable in the system family
 * add a line:
 *
 * #undef I_sysDescr
 *
 *
 */


/*
 * automatically generated by the mib compiler -- do not edit
 */

#ifndef SR_CORESUPP_H
#define SR_CORESUPP_H


#ifdef __cplusplus
extern "C" {
#endif

#define I_sysDescr                                          0
#define I_sysObjectID                                       1
#define I_sysUpTime                                         2
#define I_sysContact                                        3
#define I_sysName                                           4
#define I_sysLocation                                       5
#define I_sysServices                                       6
#define I_sysORLastChange                                   7
#define I_system_max                                        7

#define I_sysORIndex                                        0
#define I_sysORID                                           1
#define I_sysORDescr                                        2
#define I_sysORUpTime                                       3
#define I_sysOREntry_max                                    3

#define I_ifNumber                                          0
#define I_interfaces_max                                    0

#define I_ifIndex                                           0
#define I_ifDescr                                           1
#define I_ifType                                            2
#define I_ifMtu                                             3
#define I_ifSpeed                                           4
#define I_ifPhysAddress                                     5
#define I_ifAdminStatus                                     6
#define I_ifOperStatus                                      7
#define I_ifLastChange                                      8
#define I_ifInOctets                                        9
#define I_ifInUcastPkts                                     10
#define I_ifInNUcastPkts                                    11
#define I_ifInDiscards                                      12
#define I_ifInErrors                                        13
#define I_ifInUnknownProtos                                 14
#define I_ifOutOctets                                       15
#define I_ifOutUcastPkts                                    16
#define I_ifOutNUcastPkts                                   17
#define I_ifOutDiscards                                     18
#define I_ifOutErrors                                       19
#define I_ifOutQLen                                         20
#define I_ifSpecific                                        21
#define I_ifEntry_max                                       21

#define I_atIfIndex                                         0
#define I_atPhysAddress                                     1
#define I_atNetAddress                                      2
#define I_atEntry_max                                       2

#define I_ipForwarding                                      0
#define I_ipDefaultTTL                                      1
#define I_ipInReceives                                      2
#define I_ipInHdrErrors                                     3
#define I_ipInAddrErrors                                    4
#define I_ipForwDatagrams                                   5
#define I_ipInUnknownProtos                                 6
#define I_ipInDiscards                                      7
#define I_ipInDelivers                                      8
#define I_ipOutRequests                                     9
#define I_ipOutDiscards                                     10
#define I_ipOutNoRoutes                                     11
#define I_ipReasmTimeout                                    12
#define I_ipReasmReqds                                      13
#define I_ipReasmOKs                                        14
#define I_ipReasmFails                                      15
#define I_ipFragOKs                                         16
#define I_ipFragFails                                       17
#define I_ipFragCreates                                     18
#define I_ipRoutingDiscards                                 19
#define I_ip_max                                            19

#define I_ipAdEntAddr                                       0
#define I_ipAdEntIfIndex                                    1
#define I_ipAdEntNetMask                                    2
#define I_ipAdEntBcastAddr                                  3
#define I_ipAdEntReasmMaxSize                               4
#define I_ipAddrEntry_max                                   4

#define I_ipRouteDest                                       0
#define I_ipRouteIfIndex                                    1
#define I_ipRouteMetric1                                    2
#define I_ipRouteMetric2                                    3
#define I_ipRouteMetric3                                    4
#define I_ipRouteMetric4                                    5
#define I_ipRouteNextHop                                    6
#define I_ipRouteType                                       7
#define I_ipRouteProto                                      8
#define I_ipRouteAge                                        9
#define I_ipRouteMask                                       10
#define I_ipRouteMetric5                                    11
#define I_ipRouteInfo                                       12
#define I_ipRouteEntry_max                                  12

#define I_ipNetToMediaIfIndex                               0
#define I_ipNetToMediaPhysAddress                           1
#define I_ipNetToMediaNetAddress                            2
#define I_ipNetToMediaType                                  3
#define I_ipNetToMediaEntry_max                             3

#define I_icmpInMsgs                                        0
#define I_icmpInErrors                                      1
#define I_icmpInDestUnreachs                                2
#define I_icmpInTimeExcds                                   3
#define I_icmpInParmProbs                                   4
#define I_icmpInSrcQuenchs                                  5
#define I_icmpInRedirects                                   6
#define I_icmpInEchos                                       7
#define I_icmpInEchoReps                                    8
#define I_icmpInTimestamps                                  9
#define I_icmpInTimestampReps                               10
#define I_icmpInAddrMasks                                   11
#define I_icmpInAddrMaskReps                                12
#define I_icmpOutMsgs                                       13
#define I_icmpOutErrors                                     14
#define I_icmpOutDestUnreachs                               15
#define I_icmpOutTimeExcds                                  16
#define I_icmpOutParmProbs                                  17
#define I_icmpOutSrcQuenchs                                 18
#define I_icmpOutRedirects                                  19
#define I_icmpOutEchos                                      20
#define I_icmpOutEchoReps                                   21
#define I_icmpOutTimestamps                                 22
#define I_icmpOutTimestampReps                              23
#define I_icmpOutAddrMasks                                  24
#define I_icmpOutAddrMaskReps                               25
#define I_icmp_max                                          25

#define I_tcpRtoAlgorithm                                   0
#define I_tcpRtoMin                                         1
#define I_tcpRtoMax                                         2
#define I_tcpMaxConn                                        3
#define I_tcpActiveOpens                                    4
#define I_tcpPassiveOpens                                   5
#define I_tcpAttemptFails                                   6
#define I_tcpEstabResets                                    7
#define I_tcpCurrEstab                                      8
#define I_tcpInSegs                                         9
#define I_tcpOutSegs                                        10
#define I_tcpRetransSegs                                    11
#define I_tcpInErrs                                         12
#define I_tcpOutRsts                                        13
#define I_tcp_max                                           13

#define I_tcpConnState                                      0
#define I_tcpConnLocalAddress                               1
#define I_tcpConnLocalPort                                  2
#define I_tcpConnRemAddress                                 3
#define I_tcpConnRemPort                                    4
#define I_tcpConnEntry_max                                  4

#define I_udpInDatagrams                                    0
#define I_udpNoPorts                                        1
#define I_udpInErrors                                       2
#define I_udpOutDatagrams                                   3
#define I_udp_max                                           3

#define I_udpLocalAddress                                   0
#define I_udpLocalPort                                      1
#define I_udpEntry_max                                      1

#define I_egpInMsgs                                         0
#define I_egpInErrors                                       1
#define I_egpOutMsgs                                        2
#define I_egpOutErrors                                      3
#define I_egpAs                                             4
#define I_egp_max                                           4

#define I_egpNeighState                                     0
#define I_egpNeighAddr                                      1
#define I_egpNeighAs                                        2
#define I_egpNeighInMsgs                                    3
#define I_egpNeighInErrs                                    4
#define I_egpNeighOutMsgs                                   5
#define I_egpNeighOutErrs                                   6
#define I_egpNeighInErrMsgs                                 7
#define I_egpNeighOutErrMsgs                                8
#define I_egpNeighStateUps                                  9
#define I_egpNeighStateDowns                                10
#define I_egpNeighIntervalHello                             11
#define I_egpNeighIntervalPoll                              12
#define I_egpNeighMode                                      13
#define I_egpNeighEventTrigger                              14
#define I_egpNeighEntry_max                                 14

#define I_snmpInPkts                                        0
#define I_snmpOutPkts                                       1
#define I_snmpInBadVersions                                 2
#define I_snmpInBadCommunityNames                           3
#define I_snmpInBadCommunityUses                            4
#define I_snmpInASNParseErrs                                5
#define I_snmpInTooBigs                                     6
#define I_snmpInNoSuchNames                                 7
#define I_snmpInBadValues                                   8
#define I_snmpInReadOnlys                                   9
#define I_snmpInGenErrs                                     10
#define I_snmpInTotalReqVars                                11
#define I_snmpInTotalSetVars                                12
#define I_snmpInGetRequests                                 13
#define I_snmpInGetNexts                                    14
#define I_snmpInSetRequests                                 15
#define I_snmpInGetResponses                                16
#define I_snmpInTraps                                       17
#define I_snmpOutTooBigs                                    18
#define I_snmpOutNoSuchNames                                19
#define I_snmpOutBadValues                                  20
#define I_snmpOutGenErrs                                    21
#define I_snmpOutGetRequests                                22
#define I_snmpOutGetNexts                                   23
#define I_snmpOutSetRequests                                24
#define I_snmpOutGetResponses                               25
#define I_snmpOutTraps                                      26
#define I_snmpEnableAuthenTraps                             27
#define I_snmpSilentDrops                                   28
#define I_snmpProxyDrops                                    29
#define I_snmp_max                                          29

#define I_srAgentInfoDescr                                  0
#define I_srAgentInfoOSFlavor                               1
#define I_srAgentInfo_max                                   1

#define I_snmpTrapOID                                       0
#define I_snmpTrapEnterprise                                1
#define I_snmpTrap_max                                      1

#define I_snmpSetSerialNo                                   0
#define I_snmpSet_max                                       0

#include "undefs.h"


#ifdef __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SR_CORESUPP_H */