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
 * automatically generated by the mib compiler -- do not edit
 */

#ifndef SR_COREPROT_H
#define SR_COREPROT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "sr_proto.h"

extern int k_initialize
    SR_PROTOTYPE((void));

extern int k_terminate
    SR_PROTOTYPE((void));


/*
 * The system family. 
 */

extern VarBind * system_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int system_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int system_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int system_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void system_free
    SR_PROTOTYPE((system_t *data));

extern system_t *new_k_system_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  system_t *systemData));

extern system_t *k_system_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_system_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_system_set
    SR_PROTOTYPE((system_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_system_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int system_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern system_t *Clone_system
    SR_PROTOTYPE((system_t *data));

extern void k_systemCloneUserpartData
    SR_PROTOTYPE((system_t *dst, system_t *src));

extern void k_systemFreeUserpartData
    SR_PROTOTYPE((system_t *data));



/*
 * The sysOREntry family. 
 */

extern VarBind * sysOREntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));


extern sysOREntry_t *new_k_sysOREntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  sysOREntry_t *sysOREntryData));

extern sysOREntry_t *k_sysOREntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 sysORIndex));



/*
 * The interfaces family. 
 */

extern VarBind * interfaces_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern interfaces_t *new_k_interfaces_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  interfaces_t *interfacesData));

extern interfaces_t *k_interfaces_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The ifEntry family. 
 */

extern VarBind * ifEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));


extern int ifEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));


extern int ifEntry_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int ifEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void ifEntry_free
    SR_PROTOTYPE((ifEntry_t *data));

extern ifEntry_t *new_k_ifEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  ifEntry_t *ifEntryData));

extern ifEntry_t *k_ifEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 ifIndex));

extern int k_ifEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_ifEntry_set
    SR_PROTOTYPE((ifEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_ifEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int ifEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern ifEntry_t *Clone_ifEntry
    SR_PROTOTYPE((ifEntry_t *data));

extern int k_ifEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));

extern void k_ifEntryCloneUserpartData
    SR_PROTOTYPE((ifEntry_t *dst, ifEntry_t *src));

extern void k_ifEntryFreeUserpartData
    SR_PROTOTYPE((ifEntry_t *data));



/*
 * The atEntry family. 
 */

extern VarBind * atEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));


extern int atEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));


extern int atEntry_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int atEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void atEntry_free
    SR_PROTOTYPE((atEntry_t *data));

extern atEntry_t *new_k_atEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  atEntry_t *atEntryData));

extern atEntry_t *k_atEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 atIfIndex,
                  OctetString * atNetAddress));

extern int k_atEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_atEntry_set
    SR_PROTOTYPE((atEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_atEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int atEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern atEntry_t *Clone_atEntry
    SR_PROTOTYPE((atEntry_t *data));

extern int k_atEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));

extern void k_atEntryCloneUserpartData
    SR_PROTOTYPE((atEntry_t *dst, atEntry_t *src));

extern void k_atEntryFreeUserpartData
    SR_PROTOTYPE((atEntry_t *data));



/*
 * The ip family. 
 */

extern VarBind * ip_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int ip_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int ip_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int ip_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void ip_free
    SR_PROTOTYPE((ip_t *data));

extern ip_t *new_k_ip_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  ip_t *ipData));

extern ip_t *k_ip_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_ip_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_ip_set
    SR_PROTOTYPE((ip_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_ip_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int ip_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern ip_t *Clone_ip
    SR_PROTOTYPE((ip_t *data));

extern void k_ipCloneUserpartData
    SR_PROTOTYPE((ip_t *dst, ip_t *src));

extern void k_ipFreeUserpartData
    SR_PROTOTYPE((ip_t *data));



/*
 * The ipAddrEntry family. 
 */

extern VarBind * ipAddrEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));


extern ipAddrEntry_t *new_k_ipAddrEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  ipAddrEntry_t *ipAddrEntryData));

extern ipAddrEntry_t *k_ipAddrEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_UINT32 ipAdEntAddr));



/*
 * The ipRouteEntry family. 
 */

extern VarBind * ipRouteEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));


extern int ipRouteEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));


extern int ipRouteEntry_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int ipRouteEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void ipRouteEntry_free
    SR_PROTOTYPE((ipRouteEntry_t *data));

extern ipRouteEntry_t *new_k_ipRouteEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  ipRouteEntry_t *ipRouteEntryData));

extern ipRouteEntry_t *k_ipRouteEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_UINT32 ipRouteDest));

extern int k_ipRouteEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_ipRouteEntry_set
    SR_PROTOTYPE((ipRouteEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_ipRouteEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int ipRouteEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern ipRouteEntry_t *Clone_ipRouteEntry
    SR_PROTOTYPE((ipRouteEntry_t *data));

extern int k_ipRouteEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));

extern void k_ipRouteEntryCloneUserpartData
    SR_PROTOTYPE((ipRouteEntry_t *dst, ipRouteEntry_t *src));

extern void k_ipRouteEntryFreeUserpartData
    SR_PROTOTYPE((ipRouteEntry_t *data));



/*
 * The ipNetToMediaEntry family. 
 */

extern VarBind * ipNetToMediaEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));


extern int ipNetToMediaEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));


extern int ipNetToMediaEntry_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int ipNetToMediaEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void ipNetToMediaEntry_free
    SR_PROTOTYPE((ipNetToMediaEntry_t *data));

extern ipNetToMediaEntry_t *new_k_ipNetToMediaEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  ipNetToMediaEntry_t *ipNetToMediaEntryData));

extern ipNetToMediaEntry_t *k_ipNetToMediaEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 ipNetToMediaIfIndex,
                  SR_UINT32 ipNetToMediaNetAddress));

extern int k_ipNetToMediaEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_ipNetToMediaEntry_set
    SR_PROTOTYPE((ipNetToMediaEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_ipNetToMediaEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int ipNetToMediaEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern ipNetToMediaEntry_t *Clone_ipNetToMediaEntry
    SR_PROTOTYPE((ipNetToMediaEntry_t *data));

extern int k_ipNetToMediaEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));

extern void k_ipNetToMediaEntryCloneUserpartData
    SR_PROTOTYPE((ipNetToMediaEntry_t *dst, ipNetToMediaEntry_t *src));

extern void k_ipNetToMediaEntryFreeUserpartData
    SR_PROTOTYPE((ipNetToMediaEntry_t *data));



/*
 * The icmp family. 
 */

extern VarBind * icmp_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern icmp_t *new_k_icmp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  icmp_t *icmpData));

extern icmp_t *k_icmp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The tcp family. 
 */

extern VarBind * tcp_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern tcp_t *new_k_tcp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  tcp_t *tcpData));

extern tcp_t *k_tcp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The tcpConnEntry family. 
 */

extern VarBind * tcpConnEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));


extern int tcpConnEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));


extern int tcpConnEntry_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int tcpConnEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void tcpConnEntry_free
    SR_PROTOTYPE((tcpConnEntry_t *data));

extern tcpConnEntry_t *new_k_tcpConnEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  tcpConnEntry_t *tcpConnEntryData));

extern tcpConnEntry_t *k_tcpConnEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_UINT32 tcpConnLocalAddress,
                  SR_INT32 tcpConnLocalPort,
                  SR_UINT32 tcpConnRemAddress,
                  SR_INT32 tcpConnRemPort));

extern int k_tcpConnEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_tcpConnEntry_set
    SR_PROTOTYPE((tcpConnEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_tcpConnEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int tcpConnEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern tcpConnEntry_t *Clone_tcpConnEntry
    SR_PROTOTYPE((tcpConnEntry_t *data));

extern int k_tcpConnEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));

extern void k_tcpConnEntryCloneUserpartData
    SR_PROTOTYPE((tcpConnEntry_t *dst, tcpConnEntry_t *src));

extern void k_tcpConnEntryFreeUserpartData
    SR_PROTOTYPE((tcpConnEntry_t *data));



/*
 * The udp family. 
 */

extern VarBind * udp_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern udp_t *new_k_udp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  udp_t *udpData));

extern udp_t *k_udp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The udpEntry family. 
 */

extern VarBind * udpEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));


extern udpEntry_t *new_k_udpEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  udpEntry_t *udpEntryData));

extern udpEntry_t *k_udpEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_UINT32 udpLocalAddress,
                  SR_INT32 udpLocalPort));



/*
 * The egp family. 
 */

extern VarBind * egp_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern egp_t *new_k_egp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  egp_t *egpData));

extern egp_t *k_egp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The egpNeighEntry family. 
 */

extern VarBind * egpNeighEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));


extern int egpNeighEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));


extern int egpNeighEntry_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int egpNeighEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void egpNeighEntry_free
    SR_PROTOTYPE((egpNeighEntry_t *data));

extern egpNeighEntry_t *new_k_egpNeighEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  egpNeighEntry_t *egpNeighEntryData));

extern egpNeighEntry_t *k_egpNeighEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_UINT32 egpNeighAddr));

extern int k_egpNeighEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_egpNeighEntry_set
    SR_PROTOTYPE((egpNeighEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_egpNeighEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int egpNeighEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern egpNeighEntry_t *Clone_egpNeighEntry
    SR_PROTOTYPE((egpNeighEntry_t *data));

extern int k_egpNeighEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));

extern void k_egpNeighEntryCloneUserpartData
    SR_PROTOTYPE((egpNeighEntry_t *dst, egpNeighEntry_t *src));

extern void k_egpNeighEntryFreeUserpartData
    SR_PROTOTYPE((egpNeighEntry_t *data));



/*
 * The snmp family. 
 */

extern VarBind * snmp_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int snmp_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int snmp_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int snmp_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void snmp_free
    SR_PROTOTYPE((snmp_t *data));

extern snmp_t *new_k_snmp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  snmp_t *snmpData));

extern snmp_t *k_snmp_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_snmp_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_snmp_set
    SR_PROTOTYPE((snmp_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_snmp_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int snmp_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern snmp_t *Clone_snmp
    SR_PROTOTYPE((snmp_t *data));

extern void k_snmpCloneUserpartData
    SR_PROTOTYPE((snmp_t *dst, snmp_t *src));

extern void k_snmpFreeUserpartData
    SR_PROTOTYPE((snmp_t *data));



/*
 * The srAgentInfo family. 
 */

extern VarBind * srAgentInfo_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern srAgentInfo_t *new_k_srAgentInfo_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  srAgentInfo_t *srAgentInfoData));

extern srAgentInfo_t *k_srAgentInfo_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The snmpTrap family. 
 */

extern VarBind * snmpTrap_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern snmpTrap_t *new_k_snmpTrap_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  snmpTrap_t *snmpTrapData));

extern snmpTrap_t *k_snmpTrap_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The snmpSet family. 
 */

extern VarBind * snmpSet_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int snmpSet_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int snmpSet_ready
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int snmpSet_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void snmpSet_free
    SR_PROTOTYPE((snmpSet_t *data));

extern snmpSet_t *new_k_snmpSet_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  snmpSet_t *snmpSetData));

extern snmpSet_t *k_snmpSet_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_snmpSet_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_snmpSet_set
    SR_PROTOTYPE((snmpSet_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_snmpSet_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int snmpSet_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern snmpSet_t *Clone_snmpSet
    SR_PROTOTYPE((snmpSet_t *data));

extern void k_snmpSetCloneUserpartData
    SR_PROTOTYPE((snmpSet_t *dst, snmpSet_t *src));

extern void k_snmpSetFreeUserpartData
    SR_PROTOTYPE((snmpSet_t *data));



#ifdef __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SR_COREPROT_H */