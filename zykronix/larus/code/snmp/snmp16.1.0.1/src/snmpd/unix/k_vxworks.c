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
#include <netinet/in.h>
#include <m2Lib.h>

#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "sitedefs.h"
#include "trap.h"
#include "diag.h"
SR_FILENAME


#include "k_ipforw.c"
#include "linktrap.c"

#define M2_SEARCH(x) (((x)==(EXACT)) ? (M2_EXACT_VALUE) : (M2_NEXT_VALUE))
#define M2_SEARCH_TEXT(x) (((x)==(EXACT)) ? "M2_EXACT_VALUE" : "M2_NEXT_VALUE")


extern system_t systemData;

int
print_errno()
{
   switch(errno) {
       case S_m2Lib_INVALID_PARAMETER:
	   DPRINTF((APERROR,
		"errno 0x%x: S_m2Lib_INVALID_PARAMETER\n", errno));
	   break;
       case S_m2Lib_ENTRY_NOT_FOUND:
	   DPRINTF((APTRACE,
		"errno 0x%x: S_m2Lib_ENTRY_NOT_FOUND\n", errno));
	   break;
       case S_m2Lib_TCPCONN_FD_NOT_FOUND:
	   DPRINTF((APERROR,
		"errno 0x%x: S_m2Lib_TCPCONN_FD_NOT_FOUND\n", errno));
	   break;
       case S_m2Lib_INVALID_VAR_TO_SET:
	   DPRINTF((APERROR,
		"errno 0x%x: S_m2Lib_INVALID_VAR_TO_SET\n", errno));
	   break;
       case S_m2Lib_CANT_CREATE_SYS_SEM:
	   DPRINTF((APERROR,
		"errno 0x%x: S_m2Lib_CANT_CREATE_SYS_SEM\n", errno));
	   break;
       case S_m2Lib_CANT_CREATE_IF_SEM:
	   DPRINTF((APERROR,
		"errno 0x%x: S_m2Lib_CANT_CREATE_IF_SEM\n", errno));
	   break;
       case S_m2Lib_CANT_CREATE_ROUTE_SEM:
	   DPRINTF((APERROR,
		"errno 0x%x: S_m2Lib_CANT_CREATE_ROUTE_SEM\n", errno));
	   break;
       case S_m2Lib_ARP_PHYSADDR_NOT_SPECIFIED:
	   DPRINTF((APERROR,
		"errno 0x%x: S_m2Lib_ARP_PHYSADDR_NOT_SPECIFIED\n", errno));
	   break;
       case S_m2Lib_IF_TBL_IS_EMPTY:
           DPRINTF((APWARN,
		"errno 0x%x: S_m2Lib_IF_TBL_IS_EMPTY\n", errno));
	   break;
       case S_m2Lib_IF_CNFG_CHANGED:
           DPRINTF((APWARN,
		"errno 0x%x: S_m2Lib_IF_CNFG_CHANGED\n", errno));
	   break;
       default:
           DPRINTF((APERROR,
		"errno 0x%x (unknown)\n", errno));
	   break;
   }

   return 1;
}

void
TrapGenerator(trapType, interfaceIndex, timerInfo)
   int trapType;
   int interfaceIndex;
   void *timerInfo;
{
    FNAME("TrapGenerator")
	switch(trapType) {
	    case M2_LINK_DOWN_TRAP:
		DPRINTF((APTRAP, "%s: calling send_linkDown_trap()\n", Fname));
		send_linkDown_trap((SR_INT32) interfaceIndex, NULL, NULL);
		break;
	    case M2_LINK_UP_TRAP:
		DPRINTF((APTRAP, "%s: callling send_linkUp_trap()\n", Fname));
		send_linkUp_trap((SR_INT32) interfaceIndex, NULL, NULL);
		break;
	    default:
		DPRINTF((APWARN, "%s: trapType %d not implemented.\n",
		    Fname, trapType));
		break;
	}
}

int
k_initialize()
{
    FNAME("k_initialize")
   int err = 1; /* NO ERROR */

   DPRINTF((APTRACE, "%s: calling m2IfInit()\n", Fname));
   if (m2IfInit((FUNCPTR) NULL, NULL)==ERROR) {
      DPRINTF((APERROR, "%s: can't initialize the if group\n", Fname));
      err = -1;
   }
   DPRINTF((APTRACE, "%s: calling m2IpInit()\n", Fname));

#ifndef M2_MAX_ROUTE_TABLE_SIZE
/*
 * %%% DSS - UNDOCUMENTED IN THE VXWORKS 5.2 PROGRAMMER'S GUIDE:
 * The units are rows.
 * If you call m2IpInit with parameter 0 it will pick the default of 40 rows.
 * The default value should be enought for most applications.
 */
#define M2_MAX_ROUTE_TABLE_SIZE 0
#endif /* M2_MAX_ROUTE_TABLE_SIZE */

   if (m2IpInit((int) M2_MAX_ROUTE_TABLE_SIZE)==ERROR) {
      DPRINTF((APERROR, "%s: can't initialize the ip group\n", Fname));
      err = -1;
   }
   DPRINTF((APTRACE, "%s: calling m2IcmpInit()\n", Fname));
   if (m2IcmpInit()==ERROR) {
      DPRINTF((APERROR, "%s: can't initialize the icmp group\n", Fname));
      err = -1;
   }
   DPRINTF((APTRACE, "%s: calling m2TcpInit()\n", Fname));
   if (m2TcpInit()==ERROR) {
      DPRINTF((APERROR, "%s: can't initialize the tcp group\n", Fname));
      err = -1;
   }
   DPRINTF((APTRACE, "%s: calling m2UdpInit()\n", Fname));
   if (m2UdpInit()==ERROR) {
      DPRINTF((APERROR, "%s: can't initialize the udp group\n", Fname));
      err = -1;
   }
   return err;
}

int
k_terminate()
{
   return 1;
}

interfaces_t *
k_interfaces_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    FNAME("k_interfaces_get")
   static interfaces_t interfacesData;
   static M2_INTERFACE ifVars;

   DPRINTF((APTRACE, "%s: calling m2IfGroupInfoGet()\n", Fname));
   if (m2IfGroupInfoGet(&ifVars)==OK) {
      interfacesData.ifNumber = (SR_INT32) ifVars.ifNumber;
      SET_ALL_VALID(interfacesData.valid);
      return(&interfacesData);
   } else {
      DPRINTF((APTRACE, "%s: m2IfGroupInfoGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

ifEntry_t *
k_ifEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 ifIndex;
{
    FNAME("k_ifEntry_get")
   static ifEntry_t ifEntryData;
   static M2_INTERFACETBL interfaceEntry;
   static OctetString ifDescr, ifPhysAddress;
   static OID ifSpecific;

   if (searchType == NEXT && ifIndex > 0) {
       ifIndex--;
   }
   interfaceEntry.ifIndex = (int)ifIndex;
   DPRINTF((APTRACE, "%s: calling m2IfTblEntryGet()\n", Fname));
   if (m2IfTblEntryGet(M2_SEARCH(searchType), &interfaceEntry)==OK) {
      ifEntryData.ifIndex = (SR_INT32) interfaceEntry.ifIndex;

      /* build a static OctetString on the fly */
      ifDescr.octet_ptr = (unsigned char *) interfaceEntry.ifDescr;
      ifDescr.length = (SR_INT32) strlen(interfaceEntry.ifDescr);
      ifEntryData.ifDescr = &ifDescr;

      ifEntryData.ifType = (SR_INT32) interfaceEntry.ifType;
      ifEntryData.ifMtu = (SR_INT32) interfaceEntry.ifMtu;
      ifEntryData.ifSpeed = (SR_UINT32) interfaceEntry.ifSpeed;

      /* build a static OctetString on the fly */
      ifPhysAddress.octet_ptr = (unsigned char *) interfaceEntry.ifPhysAddress.phyAddress;
      ifPhysAddress.length = (SR_INT32) interfaceEntry.ifPhysAddress.addrLength;
      ifEntryData.ifPhysAddress = &ifPhysAddress;

      ifEntryData.ifAdminStatus = (SR_INT32) interfaceEntry.ifAdminStatus;
      ifEntryData.ifOperStatus = (SR_INT32) interfaceEntry.ifOperStatus;
      ifEntryData.ifLastChange = (SR_UINT32) interfaceEntry.ifLastChange;
      ifEntryData.ifInOctets = (SR_UINT32) interfaceEntry.ifInOctets;
      ifEntryData.ifInUcastPkts = (SR_UINT32) interfaceEntry.ifInUcastPkts;
      ifEntryData.ifInNUcastPkts = (SR_UINT32) interfaceEntry.ifInNUcastPkts;
      ifEntryData.ifInDiscards = (SR_UINT32) interfaceEntry.ifInDiscards;
      ifEntryData.ifInErrors = (SR_UINT32) interfaceEntry.ifInErrors;
      ifEntryData.ifInUnknownProtos = (SR_UINT32) interfaceEntry.ifInUnknownProtos;
      ifEntryData.ifOutOctets = (SR_UINT32) interfaceEntry.ifOutOctets;
      ifEntryData.ifOutUcastPkts = (SR_UINT32) interfaceEntry.ifOutUcastPkts;
      ifEntryData.ifOutNUcastPkts = (SR_UINT32) interfaceEntry.ifOutNUcastPkts;
      ifEntryData.ifOutDiscards = (SR_UINT32) interfaceEntry.ifOutDiscards;
      ifEntryData.ifOutErrors = (SR_UINT32) interfaceEntry.ifOutErrors;
      ifEntryData.ifOutQLen = (SR_UINT32) interfaceEntry.ifOutQLen;

      /* build a static OID on the fly */
      ifSpecific.oid_ptr = (SR_UINT32 *) interfaceEntry.ifSpecific.idArray;
      ifSpecific.length = (SR_INT32) interfaceEntry.ifSpecific.idLength;
      ifEntryData.ifSpecific = &ifSpecific;

      SET_ALL_VALID(ifEntryData.valid);
      return(&ifEntryData);
   } else {
      DPRINTF((APTRACE, "%s: m2IfTblEntryGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

#ifdef SETS
int
k_ifEntry_set_defaults(dp)
    doList_t       *dp;
{
    return NO_ERROR;
}

int
k_ifEntry_set(data, contextInfo, function)
   ifEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{
    FNAME("k_ifEntry_set")
   M2_INTERFACETBL ifEntryToSet;

   if (VALID(I_ifAdminStatus, data->valid)) {
      switch(data->ifAdminStatus) {
         case D_ifAdminStatus_up:
	    ifEntryToSet.ifAdminStatus = M2_ifAdminStatus_up;
	    break;
         case D_ifAdminStatus_down:
	    ifEntryToSet.ifAdminStatus = M2_ifAdminStatus_down;
	    break;
         case D_ifAdminStatus_testing:
	    ifEntryToSet.ifAdminStatus = M2_ifAdminStatus_testing;
	    break;
      }
   } else {
      return GEN_ERROR;
   }

   ifEntryToSet.ifIndex = (int) data->ifIndex;

   DPRINTF((APTRACE, "%s: calling m2IfTblEntrySet()\n", Fname));
   if (m2IfTblEntrySet(&ifEntryToSet)==OK) {
      return NO_ERROR;
   } else {
      DPRINTF((APTRACE, "%s: m2IfTblEntrySet returned ERROR\n", Fname));
      print_errno();
      return GEN_ERROR;
   }
}
#endif /* SETS */

ip_t *
k_ip_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    FNAME("k_ip_get")
   static ip_t ipData;
   static M2_IP ipVars;

   DPRINTF((APTRACE, "%s: calling m2IpGroupInfoGet()\n", Fname));
   if (m2IpGroupInfoGet(&ipVars)==OK) {
      ipData.ipForwarding = (SR_INT32) ipVars.ipForwarding;
      ipData.ipDefaultTTL = (SR_INT32) ipVars.ipDefaultTTL;
      ipData.ipInReceives = (SR_UINT32) ipVars.ipInReceives;
      ipData.ipInHdrErrors = (SR_UINT32) ipVars.ipInHdrErrors;
      ipData.ipInAddrErrors = (SR_UINT32) ipVars.ipInAddrErrors;
      ipData.ipForwDatagrams = (SR_UINT32) ipVars.ipForwDatagrams;
      ipData.ipInUnknownProtos = (SR_UINT32) ipVars.ipInUnknownProtos;
      ipData.ipInDiscards = (SR_UINT32) ipVars.ipInDiscards;
      ipData.ipInDelivers = (SR_UINT32) ipVars.ipInDelivers;
      ipData.ipOutRequests = (SR_UINT32) ipVars.ipOutRequests;
      ipData.ipOutDiscards = (SR_UINT32) ipVars.ipOutDiscards;
      ipData.ipOutNoRoutes = (SR_UINT32) ipVars.ipOutNoRoutes;
      ipData.ipReasmTimeout = (SR_INT32) ipVars.ipReasmTimeout;
      ipData.ipReasmReqds = (SR_UINT32) ipVars.ipReasmReqds;
      ipData.ipReasmOKs = (SR_UINT32) ipVars.ipReasmOKs;
      ipData.ipReasmFails = (SR_UINT32) ipVars.ipReasmFails;
      ipData.ipFragOKs = (SR_UINT32) ipVars.ipFragOKs;
      ipData.ipFragFails = (SR_UINT32) ipVars.ipFragFails;
      ipData.ipFragCreates = (SR_UINT32) ipVars.ipFragCreates;
      ipData.ipRoutingDiscards = (SR_UINT32) ipVars.ipRoutingDiscards;
      SET_ALL_VALID(ipData.valid);
      return(&ipData);
   } else {
      DPRINTF((APTRACE, "%s: m2IpGroupInfoGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

#ifdef SETS
int
k_ip_set(data, contextInfo, function)
   ip_t *data;
   ContextInfo *contextInfo;
   int function;
{
   FNAME("k_ip_set")
   M2_IP ipVars;
   int varToSet = 0;

   if (VALID(I_ipForwarding, data->valid)) {
      varToSet |= M2_IPFORWARDING;
      switch(data->ipForwarding) {
         case D_ipForwarding_forwarding:
	    ipVars.ipForwarding = M2_ipForwarding_forwarding;
	    break;
         case D_ipForwarding_not_forwarding:
	    ipVars.ipForwarding = M2_ipForwarding_not_forwarding;
	    break;
      }
   }
   if (VALID(I_ipDefaultTTL, data->valid)) {
      varToSet |= M2_IPDEFAULTTTL;
      ipVars.ipDefaultTTL = (long) data->ipDefaultTTL;
   }
   if (varToSet==0) {
      return GEN_ERROR;
   }

   DPRINTF((APTRACE, "%s: calling m2IpGroupInfoSet()\n", Fname));
   if (m2IpGroupInfoSet(varToSet, &ipVars)==OK) {
      return NO_ERROR;
   } else {
      DPRINTF((APTRACE, "%s: m2IpGroupInfoSet returned ERROR\n", Fname));
      print_errno();
      return GEN_ERROR;
   }
}
#endif /* SETS */


ipAddrEntry_t *
k_ipAddrEntry_get(serialNum, contextInfo, nominator, searchType, ipAdEntAddr)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_UINT32 ipAdEntAddr;
{
    FNAME("k_ipAddrEntry_get")
   static ipAddrEntry_t ipAddrEntryData;
   static M2_IPADDRTBL ipAddrEntry;

   ipAddrEntry.ipAdEntAddr = (unsigned long) ((unsigned long) ipAdEntAddr);
   DPRINTF((APTRACE, "%s: calling m2IpAddrTblEntryGet()\n", Fname));
   if (m2IpAddrTblEntryGet(M2_SEARCH(searchType), &ipAddrEntry)==OK) {
      ipAddrEntryData.ipAdEntAddr = (SR_UINT32) ((unsigned long) ipAddrEntry.ipAdEntAddr);
      ipAddrEntryData.ipAdEntIfIndex = (SR_INT32) ipAddrEntry.ipAdEntIfIndex;
      ipAddrEntryData.ipAdEntNetMask = (SR_UINT32) ipAddrEntry.ipAdEntNetMask;
      ipAddrEntryData.ipAdEntBcastAddr = (SR_INT32) ipAddrEntry.ipAdEntBcastAddr;
      ipAddrEntryData.ipAdEntReasmMaxSize = (SR_INT32) ipAddrEntry.ipAdEntReasmMaxSize;

      SET_ALL_VALID(ipAddrEntryData.valid);
      return(&ipAddrEntryData);
   } else {
      DPRINTF((APTRACE, "%s: m2IpAddrTblEntryGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

ipRouteEntry_t *
k_ipRouteEntry_get(serialNum, contextInfo, nominator, searchType, ipRouteDest)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_UINT32 ipRouteDest;
{
    FNAME("k_ipRouteEntry_get")
   static ipRouteEntry_t ipRouteEntryData;
   static M2_IPROUTETBL ipRouteTblEntry;
   static OID ipRouteInfo;

   ipRouteTblEntry.ipRouteDest = (unsigned long) ((unsigned long) ipRouteDest);
   DPRINTF((APTRACE, "%s: calling m2IpRouteTblEntryGet()\n", Fname));
   DPRINTF((APTRACE, "%s: ipRouteDest is %lu\n", Fname, ipRouteTblEntry.ipRouteDest));
   DPRINTF((APTRACE, "%s: search type is %s\n", Fname, M2_SEARCH_TEXT(searchType)));
   if (m2IpRouteTblEntryGet(M2_SEARCH(searchType), &ipRouteTblEntry)==OK) {
      ipRouteEntryData.ipRouteDest = (SR_UINT32) ((unsigned long) ipRouteTblEntry.ipRouteDest);
      ipRouteEntryData.ipRouteIfIndex = (SR_INT32) ipRouteTblEntry.ipRouteIfIndex;
      ipRouteEntryData.ipRouteMetric1 = (SR_INT32) ipRouteTblEntry.ipRouteMetric1;
      ipRouteEntryData.ipRouteMetric2 = (SR_INT32) ipRouteTblEntry.ipRouteMetric2;
      ipRouteEntryData.ipRouteMetric3 = (SR_INT32) ipRouteTblEntry.ipRouteMetric3;
      ipRouteEntryData.ipRouteMetric4 = (SR_INT32) ipRouteTblEntry.ipRouteMetric4;
      ipRouteEntryData.ipRouteNextHop = (SR_UINT32) (ipRouteTblEntry.ipRouteNextHop);
      ipRouteEntryData.ipRouteType = (SR_INT32) ipRouteTblEntry.ipRouteType;
      ipRouteEntryData.ipRouteProto = (SR_INT32) ipRouteTblEntry.ipRouteProto;
      ipRouteEntryData.ipRouteAge = (SR_INT32) ipRouteTblEntry.ipRouteAge;
      ipRouteEntryData.ipRouteMask = (SR_UINT32) ipRouteTblEntry.ipRouteMask;
      ipRouteEntryData.ipRouteMetric5 = (SR_INT32) ipRouteTblEntry.ipRouteMetric5;
      /* build a static OID on the fly */
      ipRouteInfo.oid_ptr = (SR_UINT32 *) ipRouteTblEntry.ipRouteInfo.idArray;
      ipRouteInfo.length = (SR_INT32) ipRouteTblEntry.ipRouteInfo.idLength;
      ipRouteEntryData.ipRouteInfo = &ipRouteInfo;

      SET_ALL_VALID(ipRouteEntryData.valid);
      return(&ipRouteEntryData);
   } else {
      DPRINTF((APTRACE, "%s: m2IpRouteTblEntryGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

#ifdef SETS
int
k_ipRouteEntry_set(data, contextInfo, function)
   ipRouteEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{
    FNAME("k_ipRouteEntry_set")
   static M2_IPROUTETBL ipRouteTblEntryToSet;
   int varToSet = 0;

   if (VALID(I_ipRouteDest, data->valid)) {
      varToSet |= M2_IP_ROUTE_DEST;
      ipRouteTblEntryToSet.ipRouteDest = (unsigned long) ((unsigned long) data->ipRouteDest);
   } else {
      return GEN_ERROR;
   }

   if (VALID(I_ipRouteNextHop, data->valid)) {
      varToSet |= M2_IP_ROUTE_NEXT_HOP;
      ipRouteTblEntryToSet.ipRouteNextHop = (unsigned long) ((unsigned long) data->ipRouteNextHop);
   } else {
      return GEN_ERROR;
   }

   if (VALID(I_ipRouteType, data->valid)) {
      varToSet |= M2_IP_ROUTE_TYPE;
      switch(data->ipRouteType) {
	 case D_ipRouteType_other:
	    ipRouteTblEntryToSet.ipRouteType = M2_ipRouteType_other;
	    break;
	 case D_ipRouteType_invalid:
	    ipRouteTblEntryToSet.ipRouteType = M2_ipRouteType_invalid;
	    break;
	 case D_ipRouteType_direct:
	    ipRouteTblEntryToSet.ipRouteType = M2_ipRouteType_direct;
	    break;
	 case D_ipRouteType_indirect:
	    ipRouteTblEntryToSet.ipRouteType = M2_ipRouteType_indirect;
	    break;
      }
   }

   DPRINTF((APTRACE, "%s: calling m2IpRouteTblEntryGet()\n", Fname));
   if (m2IpRouteTblEntrySet(varToSet, &ipRouteTblEntryToSet)==OK) {
      return NO_ERROR;
   } else {
      DPRINTF((APTRACE, "%s: m2IpRouteTblEntrySet returned ERROR\n", Fname));
      print_errno();
      return GEN_ERROR;
   }
}
#endif /* SETS */

ipNetToMediaEntry_t *
k_ipNetToMediaEntry_get(serialNum, contextInfo, nominator, searchType, ipNetToMediaIfIndex, ipNetToMediaNetAddress)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 ipNetToMediaIfIndex;
   SR_UINT32 ipNetToMediaNetAddress;
{
    FNAME("k_ipNetToMediaEntry_get")
   static ipNetToMediaEntry_t ipNetToMediaEntryData;
   static M2_IPATRANSTBL ipAtEntry;
   static OctetString ipNetToMediaPhysAddress;

   ipAtEntry.ipNetToMediaIfIndex = (long) ipNetToMediaIfIndex;
   ipAtEntry.ipNetToMediaNetAddress = (unsigned long) ((unsigned long) ipNetToMediaNetAddress);
   DPRINTF((APTRACE, "%s: calling m2IpAtransTblEntryGet()\n", Fname));
   if (m2IpAtransTblEntryGet(M2_SEARCH(searchType), &ipAtEntry)==OK) {
      ipNetToMediaEntryData.ipNetToMediaIfIndex = (SR_INT32) ipAtEntry.ipNetToMediaIfIndex;

      /* build a static OctetString on the fly */
      ipNetToMediaPhysAddress.octet_ptr = (unsigned char *) ipAtEntry.ipNetToMediaPhysAddress.phyAddress;
      ipNetToMediaPhysAddress.length = (SR_INT32) ipAtEntry.ipNetToMediaPhysAddress.addrLength;
      ipNetToMediaEntryData.ipNetToMediaPhysAddress = &ipNetToMediaPhysAddress;

      ipNetToMediaEntryData.ipNetToMediaNetAddress = (SR_UINT32) ((unsigned long) ipAtEntry.ipNetToMediaNetAddress);

      switch(ipAtEntry.ipNetToMediaType) {
	 case M2_ipNetToMediaType_other:
	    ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_other;
	    break;
	 case M2_ipNetToMediaType_invalid:
	    ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_invalid;
	    break;
	 case M2_ipNetToMediaType_dynamic:
	    ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_dynamic;
	    break;
	 case M2_ipNetToMediaType_static:
	    ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_static;
	    break;
      }
      if ( (ipNetToMediaEntryData.ipNetToMediaIfIndex < ipNetToMediaIfIndex) ||
	  ((ipNetToMediaEntryData.ipNetToMediaIfIndex == ipNetToMediaIfIndex) &&
	   (ipNetToMediaEntryData.ipNetToMediaNetAddress < ipNetToMediaNetAddress)) ) {
	 DPRINTF((APTRACE, "%s: m2IpAtransTblEntryGet returned a\nlexicographically lesser index for searchType %s\n", Fname, M2_SEARCH_TEXT(searchType)));
	 return(NULL);
      }
     
      SET_ALL_VALID(ipNetToMediaEntryData.valid);
      return(&ipNetToMediaEntryData);
   } else {
      DPRINTF((APTRACE, "%s: m2IpAtransTblEntryGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

#ifdef SETS
int
k_ipNetToMediaEntry_set(data, contextInfo, function)
   ipNetToMediaEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{
    FNAME("k_ipNetToMediaEntry_set")
   static M2_IPATRANSTBL ipAtEntryToSet;

   if (VALID(I_ipNetToMediaType, data->valid)) {
      switch(data->ipNetToMediaType) {
         case D_ipNetToMediaType_other:
	    ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_other;
	    break;
         case D_ipNetToMediaType_invalid:
	    ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_invalid;
	    break;
         case D_ipNetToMediaType_dynamic:
	    ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_dynamic;
	    break;
         case D_ipNetToMediaType_static:
	    ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_static;
	    break;
      }
   } else {
      return GEN_ERROR;
   }

   if (ipAtEntryToSet.ipNetToMediaType != M2_ipNetToMediaType_invalid) {
      if (VALID(I_ipNetToMediaPhysAddress, data->valid)) {
	 ipAtEntryToSet.ipNetToMediaPhysAddress.addrLength = MIN(data->ipNetToMediaPhysAddress->length, MAXIFPHYADDR);
	 memcpy(ipAtEntryToSet.ipNetToMediaPhysAddress.phyAddress, data->ipNetToMediaPhysAddress->octet_ptr, ipAtEntryToSet.ipNetToMediaPhysAddress.addrLength);
      } else {
	 return GEN_ERROR;
      }
   }

   ipAtEntryToSet.ipNetToMediaIfIndex = (long) data->ipNetToMediaIfIndex;
   ipAtEntryToSet.ipNetToMediaNetAddress = (unsigned long) ((unsigned long) data->ipNetToMediaNetAddress);

   DPRINTF((APTRACE, "%s: calling m2IpAtransTblEntrySet()\n", Fname));
   if (m2IpAtransTblEntrySet(&ipAtEntryToSet)==OK) {
      return NO_ERROR;
   } else {
      DPRINTF((APTRACE, "%s: m2IpAtransTblEntrySet returned ERROR\n", Fname));
      print_errno();
      return GEN_ERROR;
   }
}
#endif /* SETS */

icmp_t *
k_icmp_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    FNAME("k_icmp_get")
   static icmp_t icmpData;
   static M2_ICMP icmpVars;

   DPRINTF((APTRACE, "%s: calling m2IcmpGroupInfoGet()\n", Fname));
   if (m2IcmpGroupInfoGet(&icmpVars)==OK) {
      icmpData.icmpInMsgs = (SR_UINT32) icmpVars.icmpInMsgs;
      icmpData.icmpInErrors = (SR_UINT32) icmpVars.icmpInErrors;
      icmpData.icmpInDestUnreachs = (SR_UINT32) icmpVars.icmpInDestUnreachs;
      icmpData.icmpInTimeExcds = (SR_UINT32) icmpVars.icmpInTimeExcds;
      icmpData.icmpInParmProbs = (SR_UINT32) icmpVars.icmpInParmProbs;
      icmpData.icmpInSrcQuenchs = (SR_UINT32) icmpVars.icmpInSrcQuenchs;
      icmpData.icmpInRedirects = (SR_UINT32) icmpVars.icmpInRedirects;
      icmpData.icmpInEchos = (SR_UINT32) icmpVars.icmpInEchos;
      icmpData.icmpInEchoReps = (SR_UINT32) icmpVars.icmpInEchoReps;
      icmpData.icmpInTimestamps = (SR_UINT32) icmpVars.icmpInTimestamps;
      icmpData.icmpInTimestampReps = (SR_UINT32) icmpVars.icmpInTimestampReps;
      icmpData.icmpInAddrMasks = (SR_UINT32) icmpVars.icmpInAddrMasks;
      icmpData.icmpInAddrMaskReps = (SR_UINT32) icmpVars.icmpInAddrMaskReps;
      icmpData.icmpOutMsgs = (SR_UINT32) icmpVars.icmpOutMsgs;
      icmpData.icmpOutErrors = (SR_UINT32) icmpVars.icmpOutErrors;
      icmpData.icmpOutDestUnreachs = (SR_UINT32) icmpVars.icmpOutDestUnreachs;
      icmpData.icmpOutTimeExcds = (SR_UINT32) icmpVars.icmpOutTimeExcds;
      icmpData.icmpOutParmProbs = (SR_UINT32) icmpVars.icmpOutParmProbs;
      icmpData.icmpOutSrcQuenchs = (SR_UINT32) icmpVars.icmpOutSrcQuenchs;
      icmpData.icmpOutRedirects = (SR_UINT32) icmpVars.icmpOutRedirects;
      icmpData.icmpOutEchos = (SR_UINT32) icmpVars.icmpOutEchos;
      icmpData.icmpOutEchoReps = (SR_UINT32) icmpVars.icmpOutEchoReps;
      icmpData.icmpOutTimestamps = (SR_UINT32) icmpVars.icmpOutTimestamps;
      icmpData.icmpOutTimestampReps = (SR_UINT32) icmpVars.icmpOutTimestampReps;
      icmpData.icmpOutAddrMasks = (SR_UINT32) icmpVars.icmpOutAddrMasks;
      icmpData.icmpOutAddrMaskReps = (SR_UINT32) icmpVars.icmpOutAddrMaskReps;
      SET_ALL_VALID(icmpData.valid);
      return(&icmpData);
   } else {
      DPRINTF((APTRACE, "%s: m2IcmpGroupInfoGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

tcp_t *
k_tcp_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    FNAME("k_tcp_get")
   static tcp_t tcpData;
   static M2_TCPINFO tcpVars;

   DPRINTF((APTRACE, "%s: calling m2TcpGroupInfoGet()\n", Fname));
   if (m2TcpGroupInfoGet(&tcpVars)==OK) {
      tcpData.tcpRtoAlgorithm = (SR_INT32) tcpVars.tcpRtoAlgorithm;
      tcpData.tcpRtoMin = (SR_INT32) tcpVars.tcpRtoMin;
      tcpData.tcpRtoMax = (SR_INT32) tcpVars.tcpRtoMax;
      tcpData.tcpMaxConn = (SR_INT32) tcpVars.tcpMaxConn;
      tcpData.tcpActiveOpens = (SR_UINT32) tcpVars.tcpActiveOpens;
      tcpData.tcpPassiveOpens = (SR_UINT32) tcpVars.tcpPassiveOpens;
      tcpData.tcpAttemptFails = (SR_UINT32) tcpVars.tcpAttemptFails;
      tcpData.tcpEstabResets = (SR_UINT32) tcpVars.tcpEstabResets;
      tcpData.tcpCurrEstab = (SR_UINT32) tcpVars.tcpCurrEstab;
      tcpData.tcpInSegs = (SR_UINT32) tcpVars.tcpInSegs;
      tcpData.tcpOutSegs = (SR_UINT32) tcpVars.tcpOutSegs;
      tcpData.tcpRetransSegs = (SR_UINT32) tcpVars.tcpRetransSegs;
      tcpData.tcpInErrs = (SR_UINT32) tcpVars.tcpInErrs;
      tcpData.tcpOutRsts = (SR_UINT32) tcpVars.tcpOutRsts;
      SET_ALL_VALID(tcpData.valid);
      return(&tcpData);
   } else {
      DPRINTF((APTRACE, "%s: m2TcpGroupInfoGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

tcpConnEntry_t *
k_tcpConnEntry_get(serialNum, contextInfo, nominator, searchType, tcpConnLocalAddress, tcpConnLocalPort, tcpConnRemAddress, tcpConnRemPort)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_UINT32 tcpConnLocalAddress;
   SR_INT32 tcpConnLocalPort;
   SR_UINT32 tcpConnRemAddress;
   SR_INT32 tcpConnRemPort;
{
    FNAME("k_tcpConnEntry_get")
   static tcpConnEntry_t tcpConnEntryData;
   static M2_TCPCONNTBL tcpEntry;

   tcpEntry.tcpConnLocalAddress = (unsigned long) ((unsigned long) tcpConnLocalAddress);
   tcpEntry.tcpConnLocalPort = (long) tcpConnLocalPort;
   tcpEntry.tcpConnRemAddress = (unsigned long) ((unsigned long) tcpConnRemAddress);
   tcpEntry.tcpConnRemPort = (long) tcpConnRemPort;

   DPRINTF((APTRACE, "%s: calling m2TcpConnEntryGet()\n", Fname));
   if (m2TcpConnEntryGet(M2_SEARCH(searchType), &tcpEntry)==OK) {
      tcpConnEntryData.tcpConnState = (SR_INT32) tcpEntry.tcpConnState;
      tcpConnEntryData.tcpConnLocalAddress = (SR_UINT32) ((unsigned long) tcpEntry.tcpConnLocalAddress);
      tcpConnEntryData.tcpConnLocalPort = (SR_INT32) tcpEntry.tcpConnLocalPort;
      tcpConnEntryData.tcpConnRemAddress = (SR_UINT32) ((unsigned long) tcpEntry.tcpConnRemAddress);
      tcpConnEntryData.tcpConnRemPort = (SR_INT32) tcpEntry.tcpConnRemPort;
      SET_ALL_VALID(tcpConnEntryData.valid);
      return(&tcpConnEntryData);
   } else {
      DPRINTF((APTRACE, "%s: m2TcpConnEntryGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

#ifdef SETS

int
k_tcpConnEntry_set(data, contextInfo, function)
   tcpConnEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{
    FNAME("k_tcpConnEntry_set")
   static M2_TCPCONNTBL tcpEntryToSet;

   tcpEntryToSet.tcpConnLocalAddress = (unsigned long) ((unsigned long) data->tcpConnLocalAddress);
   tcpEntryToSet.tcpConnLocalPort = (long) data->tcpConnLocalPort;
   tcpEntryToSet.tcpConnRemAddress = (unsigned long) ((unsigned long) data->tcpConnRemAddress);
   tcpEntryToSet.tcpConnRemPort = (long) data->tcpConnRemPort;

   if (VALID(I_tcpConnState, data->valid) && data->tcpConnState == D_tcpConnState_deleteTCB) {
      tcpEntryToSet.tcpConnState = (long) M2_tcpConnState_deleteTCB;
      DPRINTF((APTRACE, "%s: calling m2TcpConnEntrySet()\n", Fname));
      if (m2TcpConnEntrySet(&tcpEntryToSet)==OK) {
	 return NO_ERROR;
      } else {
	 DPRINTF((APTRACE, "%s: m2TcpConnEntrySet returned ERROR\n", Fname));
	 print_errno();
	 return GEN_ERROR;
      }
   }

   return 1;
}
#endif /* SETS */

udp_t *
k_udp_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    FNAME("k_udp_get")
   static udp_t udpData;
   static M2_UDP udpVars;

   DPRINTF((APTRACE, "%s: calling m2UdpGroupInfoGet()\n", Fname));
   if (m2UdpGroupInfoGet(&udpVars)==OK) {
      udpData.udpInDatagrams = (SR_UINT32) udpVars.udpInDatagrams;
      udpData.udpNoPorts = (SR_UINT32) udpVars.udpNoPorts;
      udpData.udpInErrors = (SR_UINT32) udpVars.udpInErrors;
      udpData.udpOutDatagrams = (SR_UINT32) udpVars.udpOutDatagrams;
      SET_ALL_VALID(udpData.valid);
      return(&udpData);
   } else {
      DPRINTF((APTRACE, "%s: m2UdpGroupInfoGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

udpEntry_t *
k_udpEntry_get(serialNum, contextInfo, nominator, searchType, udpLocalAddress, udpLocalPort)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_UINT32 udpLocalAddress;
   SR_INT32 udpLocalPort;
{
    FNAME("k_udpEntry_get")
   static udpEntry_t udpEntryData;
   static M2_UDPTBL udpEntry;

   udpEntry.udpLocalAddress = (unsigned long) ((unsigned long) udpLocalAddress);
   udpEntry.udpLocalPort = (long) udpLocalPort;
   DPRINTF((APTRACE, "%s: calling m2UdpTblEntryGet()\n", Fname));
   if (m2UdpTblEntryGet(M2_SEARCH(searchType), &udpEntry)==OK) {
      udpEntryData.udpLocalAddress = (SR_UINT32) ((unsigned long) udpEntry.udpLocalAddress);
      udpEntryData.udpLocalPort = (SR_INT32) udpEntry.udpLocalPort;
      SET_ALL_VALID(udpEntryData.valid);
      return(&udpEntryData);
   } else {
      DPRINTF((APTRACE, "%s: m2UdpTblEntryGet returned ERROR\n", Fname));
      print_errno();
      return(NULL);
   }
}

