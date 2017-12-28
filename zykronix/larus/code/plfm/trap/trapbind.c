/*
 * Copyright (C) 1998, All Rights Reserved, by
 * Ahoya Communications, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file contains the routine for TRAP pdu
 *
 * Note:
 *    - xxxx
 *
 * Author(s):
 *
 *     Shinn Lin
 */
/*
 *====================================================================
 *=                            include                               =
 *====================================================================
 */

#include <stdarg.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <inetLib.h>
#include <sockLib.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "ioLib.h"
#include "axss_types.h"
#include "sys_os.h"
#include "sr_conf.h"
#include "sr_snmp.h"
#include "sr_trans.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#undef SR_OFFSET /* must following method.h */
#include "sr_msg.h"
#include "objectdb.h"
#include "makevb.h"
#include "mibout.h"
#include "snmp_util.h"

#include "axss_if.h"
#include "sys_if.h"
#include "siw_if.h"
#include "trap_events.h"
#include "trapobjs.h"
#include "traprcd.h"
#include "trap_if.h"
#include "inst_lib.h" /* CSCds15384 */
#include "trapdef.h"


/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */
#define I_TRAP_GLOBAL_SEQ             0
#define I_TRAP_MGR_SEQ                1
#define I_TRAP_SEVERITY               2
#define I_TRAP_systemNodeName         3
#define I_TRAP_systemNodeNum          4
#define I_TRAP_shelfIntegratedAlarm   5

/*
 * Print Control
 */
extern U32_t   serialNum;
extern int   snmpUserType;

/*
 *====================================================================
 *=                         external variables                       =
 *====================================================================
 */
extern stsTrapRegEntry_t stsTrapRegData[];
extern TRAP_TPLEN_RCD_t trapTpInfo[];
extern TRAP_OIDTBL_t trapOidTbl[];
extern U32_t ttlTrapCnt;
extern SnmpLcd agt_snmp_lcd;

extern S32_t trapcmp(
	const void *a, 
	const void *b);

extern ObjectInfo      OidList[];

void SendSnmpTrap(
	Pdu         *pdu,
	OctetString *community,
	U32_t       ipAddr,
	U32_t       udpPort);

extern SnmpMessage *SrCreateV1SnmpMessage(
	OctetString *community,
	int cache);

extern char *GetSystemNodeName(
	char *nodeName);

extern S32_t GetSystemNodeNum();


extern STATUS_t Value2Inst(
	ObjectInfo *objPtr,
	U32_t      value,
	OID        *inst);




/*
 *====================================================================
 *=                         local variables                       =
 *====================================================================
 */


U32_t transOid[] = {1, 3, 6, 1, 4, 1, 24628};
OID cxrlarus = {7, transOid};

extern S32_t  trapFd;


/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */


  
/****************************************************************************
 * Function:    TrapObjTblInit()
 * Descripion:  This routine initialize the trap obj table
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
STATUS_t
TrapObjTblInit()
{
    U32_t  i, j; 
    int    val;
    pTRAP_OIDTBL_t     trapObjPtr;
    pTRAP_OBJ_t        trapPtr;

    SIW_FUNC_TRACE(TRM, TrapObjTblInit, 0, 0, 0, 0, 0);


    qsort(trapOidTbl, ttlTrapCnt, sizeof(TRAP_OIDTBL_t), trapcmp);
    trapObjPtr = trapOidTbl;
 
    for (i = 0; i < ttlTrapCnt; i++, trapObjPtr++)
    {
        trapPtr = trapObjPtr->objList;
        for (j = 0; j < trapObjPtr->numOfEntry; j++, trapPtr++)
	{
            val = find_object(&(trapPtr->oid), EXACT);
            if (val == -1)
	    {
                SiwEventLog(NULL, SIW_EVENT_SYM(TRM, OBJ_INIT_FAIL), i, j);
                SIW_DEBUG_TRACE(TRM, TrapObjTblInit, 0xF0000001, TRACE_LEVEL_1, 
                    i, j,0,0,0);
                return(ERROR);
	    }
            trapPtr->objPtr = &OidList[val];
	}
    }

    return(OK);
}


/****************************************************************************
 * Function:    trapMibShow()
 * Descripion:  This routine initialize the trap obj table
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
U32_t
trapMibShow(VarBind *vb_ptr, 
            U8_t *msgBuf)
{
    U32_t  value;

    switch (vb_ptr->value.type) {
    case COUNTER_TYPE:		/* handle unsigned integers including
				 * COUNTER_32 */
    case GAUGE_TYPE:		/* includes GAUGE_32 */
    case TIME_TICKS_TYPE:
    case INTEGER_TYPE:		/* handle signed integers including */
        value = vb_ptr->value.ul_value;
        sprintf(msgBuf, "%d", value);
	break;

    case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
    {
        OID   *oid;
        int   i;
        U32_t len = 0;

	oid = vb_ptr->value.oid_value;
        for (i = 0; i < oid->length; i++)
	{
            sprintf(&msgBuf[len], "%u.", oid->oid_ptr[i]);
            len += strlen(&msgBuf[len]);
	}
        msgBuf[len-1] = '\0';
    }
        break;
#ifdef SR_SNMPv2_PDU
    case COUNTER_64_TYPE:	/* handle large unsigned integers */
        value = vb_ptr->value.uint64_value;
        sprintf(msgBuf, "%d", value);
	break;
#endif /* SR_SNMPv2_PDU */
    case OCTET_PRIM_TYPE:
    case OPAQUE_PRIM_TYPE:
    {
        OctetString  *os;

        os = vb_ptr->value.os_value;
        strncpy(msgBuf, os->octet_ptr, os->length);
        msgBuf[os->length] = '\0';
    }
	break;
    case IP_ADDR_PRIM_TYPE:
    {
        OctetString  *os;
        int   i;
        U32_t len = 0;

        os = vb_ptr->value.os_value;
        for (i = 0; i < os->length; i++)
	{
            sprintf(&msgBuf[len], "%u.", os->octet_ptr[i]);
            len += strlen(&msgBuf[len]);
	}
        msgBuf[len-1] = '\0';
    }
        break;
    case NULL_TYPE:
	break;
    default:
	return (ERROR);
    }
    return(OK);
}				/* end of switch */



/****************************************************************************
 * Function:    MakeTrapVarBind()
 * Descripion:  This routine initialize the trap obj table
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
Pdu *
MakeTrapVarBind(U32_t trapNum,
                U32_t severity,
                U32_t *packBuf,
                U32_t len,
                U32_t newFlag)
{
    OID             keyInst;
    U32_t           instBuf[100];
    pTRAP_TPLEN_RCD_t  trapTpRcdPtr;
    pTRAP_DATA_TP_t    dataTpPtr;
    pTRAP_OIDTBL_t     trapObjPtr;
    pTRAP_OBJ_t        trapPtr;
    VarBind  *vb_ptr;
    U8_t     *datBuf;
    U32_t  datOffs;
    U32_t  i;
    S32_t  val;
    Pdu            *pdu_ptr;

    SIW_FUNC_TRACE(TRM, MakeTrapVarBind, trapNum, severity, packBuf, len, 0);
    keyInst.length = 0; 
    keyInst.oid_ptr = instBuf;
    trapTpRcdPtr = (pTRAP_TPLEN_RCD_t)bsearch((void *)&trapNum, trapTpInfo, 
        ttlTrapCnt, sizeof(TRAP_TPLEN_RCD_t), trapcmp);

    if (trapTpRcdPtr == NULL)
    {
        SiwEventLog(NULL, SIW_EVENT_SYM(TRM, TRAP_NUM_FAIL), trapNum);
        SIW_DEBUG_TRACE(TRM, MakeTrapVarBind, 0xF0000001, TRACE_LEVEL_1, 
            trapNum, 0,0,0,0);
        return(NULL);
    }

    trapObjPtr = (pTRAP_OIDTBL_t)bsearch((void *)&trapNum, trapOidTbl, 
        ttlTrapCnt, sizeof(TRAP_OIDTBL_t), trapcmp);


    if (trapObjPtr == NULL)
    {
        SiwEventLog(NULL, SIW_EVENT_SYM(TRM, TRAP_NUM_FAIL), trapNum);
        SIW_DEBUG_TRACE(TRM, MakeTrapVarBind, 0xF0000002, TRACE_LEVEL_1, 
            trapNum, 0,0,0,0);
        return(NULL);
    }

    datOffs = trapTpRcdPtr->numOfEntry * sizeof(U32_t);
    datBuf = (U8_t *)packBuf;

    dataTpPtr = trapTpRcdPtr->dataTpPtr;
    trapPtr = trapObjPtr->objList;

    for (i = 0; i < trapTpRcdPtr->numOfEntry;
         i++, dataTpPtr++, packBuf++, trapPtr++)
    {
        if (trapPtr->keyInx)
	{
            switch (dataTpPtr->type)
	    {
	    case TYPE_DATA_32:
                if (trapPtr->objPtr->oidtype == IP_ADDR_PRIM_TYPE)
		{
                    OctetString *os_ptr;

                    os_ptr = IPToOctetString(*packBuf);
                    if (Value2Inst(trapPtr->objPtr, (U32_t)os_ptr, &keyInst) == ERROR)
		    {
                        FreeOctetString(os_ptr);
                        return(NULL); 
		    }
                    FreeOctetString(os_ptr);
		}
                else if (Value2Inst(trapPtr->objPtr, *packBuf, &keyInst) == ERROR)
                    return(NULL); 
                break;
	    case TYPE_STR:
                val = (U32_t)MakeOctetString(datBuf + (*packBuf & 0xffff), (*packBuf) >> 16);
                if (Value2Inst(trapPtr->objPtr, val, &keyInst) == ERROR)
                    return(NULL); 
                FreeOctetString((OctetString *)val);
                break;
	    case TYPE_OID:
                val = (U32_t)MakeOID((U32_t *)(datBuf + (*packBuf & 0xffff)), (*packBuf) >> 16);
                if (Value2Inst(trapPtr->objPtr, val, &keyInst) == ERROR)
                    return(NULL); 
                FreeOID((OID *)val);
                break;

	    } /* switch */

	} /* if trapPtr->keyInx */

    } /* for */

    if ((pdu_ptr = (Pdu *) malloc(sizeof(Pdu))) == NULL) {
        SiwEventLog(NULL, SIW_EVENT_SYM(TRM, TRAP_PDU_FAIL), trapNum);
        SIW_DEBUG_TRACE(TRM, MakeTrapVarBind, 0xF0000003, TRACE_LEVEL_1, 
            trapNum, 0,0,0,0);
	return (NULL);
    }

    memset(pdu_ptr, 0, sizeof(Pdu));



    pdu_ptr->type = TRAP_TYPE;

    pdu_ptr->u.trappdu.enterprise = CloneOID(&cxrlarus);

    pdu_ptr->u.trappdu.generic_trap = ENTERPRISE_TRAP;
    pdu_ptr->u.trappdu.specific_trap = trapNum;

    if (keyInst.length == 0)
    {
        keyInst.length = 1;
        instBuf[0] = 0;
    }
    packBuf = (U32_t *)datBuf;;
    dataTpPtr = trapTpRcdPtr->dataTpPtr;
    trapPtr = trapObjPtr->objList;


    {
        for (i = 0; i < trapTpRcdPtr->numOfEntry;
            i++, dataTpPtr++, packBuf++, trapPtr++)
        {
            switch (dataTpPtr->type)
	    {
	        case TYPE_DATA_32:
                    if (trapPtr->objPtr->oidtype == IP_ADDR_PRIM_TYPE)
	            {

                        val = (U32_t)IPToOctetString(*packBuf);
                        vb_ptr = MakeVarBind(trapPtr->objPtr, &keyInst, (void *)val);
	            }
                    else
	            {

                        vb_ptr = MakeVarBind(trapPtr->objPtr, &keyInst, (void *)packBuf);
                        val = *(U32_t *)packBuf;
	            }
                    break;

	        case TYPE_STR:

                    val = (U32_t)MakeOctetString(datBuf + (*packBuf & 0xffff), (*packBuf) >> 16);
                    vb_ptr = MakeVarBind(trapPtr->objPtr, &keyInst, (void *)val);
                    break;

	        case TYPE_OID:
                    val = (U32_t)MakeOID((U32_t *)(datBuf + (*packBuf & 0xffff)), (*packBuf) >> 16);
                    vb_ptr = MakeVarBind(trapPtr->objPtr, &keyInst, (void *)val);
                    break;

	    } /* switch */
  

            if (LinkVarBind(pdu_ptr, vb_ptr) == -1)
	    {
                FreePdu(pdu_ptr);
                printf("system error: LinkVarBind Fail!!!\n");
                return(NULL);
	    }
            pdu_ptr->num_varbinds++;

        } /* for numOfEntry */

    } /* else ==> not event-trap */



    return(pdu_ptr);
}



/****************************************************************************
 * Function:    TrapToMgrs()
 * Descripion:  
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
STATUS_t
TrapToMgrs(
	U32_t severity,
	U32_t trapTime,
	TRAP_HEADER_t  *trapMsg)
{
    Pdu   *pdu_ptr;
    U32_t i;
    VarBind  *vb_ptr;
    U8_t     ipAddr[20];
    pTRAP_TPLEN_RCD_t trapTpRcdPtr;

    SIW_FUNC_TRACE(TRM, TrapToMgrs, severity, trapTime, trapMsg, 0, 0);

    pdu_ptr = MakeTrapVarBind(trapMsg->trapNumber, 
                              severity, 
                              (U32_t *)((U8_t *)trapMsg + sizeof(TRAP_HEADER_t)), 
                              trapMsg->trapLength - sizeof(TRAP_HEADER_t), 
                              TRUE);

    if (pdu_ptr == NULL)
    {    
        return(ERROR);
    }
    pdu_ptr->u.trappdu.time_ticks = trapTime;

    trapTpRcdPtr = (pTRAP_TPLEN_RCD_t)bsearch((void *)&(trapMsg->trapNumber), trapTpInfo, 
        ttlTrapCnt, sizeof(TRAP_TPLEN_RCD_t), trapcmp);

    vb_ptr = pdu_ptr->var_bind_list;
    for (i = 0; i < I_TRAP_MGR_SEQ; i++)
        vb_ptr = vb_ptr->next_var;

    if (ifAddrGet("motfcc0", ipAddr) == ERROR)
    {    
        return(ERROR);
    }

    if (pdu_ptr->u.trappdu.agent_addr)
        FreeOctetString(pdu_ptr->u.trappdu.agent_addr);
    pdu_ptr->u.trappdu.agent_addr = IPToOctetString(inet_network(ipAddr));
 
    for (i = 0; i < MAX_TRAP_MGRS; i++)
    {
        if (stsTrapRegData[i].stsTrapRegMgrIp &&
            stsTrapRegData[i].stsTrapRegState == 1)
        {
                if (BuildPdu(pdu_ptr))
        	    {
                    FreePdu(pdu_ptr);
                    return(ERROR);
                }

                SendSnmpTrap(pdu_ptr, stsTrapRegData[i].stsTrapRegCommunityStr,
                     stsTrapRegData[i].stsTrapRegMgrIp, 
                     stsTrapRegData[i].stsTrapRegMgrUdpPort);
        }
    }
    FreePdu(pdu_ptr);
    return(OK);
}


/****************************************************************************
 * Function:    SendSnmpTrap()
 * Descripion:  
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
void
SendSnmpTrap(Pdu         *pdu,
             OctetString *community,
             U32_t       ipAddr,
             U32_t       udpPort)
{
    SnmpMessage *snmp_msg = NULL;
    struct sockaddr_in addr;

    SIW_FUNC_TRACE(TRM, SendSnmpTrap, pdu, community, ipAddr, udpPort, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ipAddr;
    addr.sin_port = udpPort;

    snmp_msg = (SnmpMessage *)SrCreateV1SnmpMessage(community, TRUE);
    if (snmp_msg == NULL) 
        return;

    if (SrBuildSnmpMessage(snmp_msg, pdu, &agt_snmp_lcd)) {
        SiwEventLog(NULL, SIW_EVENT_SYM(TRM, TRAP_BUILD_FAIL));
        SIW_DEBUG_TRACE(TRM, SendSnmpTrap, 0xF0000002, TRACE_LEVEL_1, 
            0,0,0,0,0);
        SrFreeSnmpMessage(snmp_msg);
        return;
    }

    if (sendto(trapFd, snmp_msg->packlet->octet_ptr,
           snmp_msg->packlet->length, 0, (struct sockaddr *)(&addr), sizeof(addr)) <= 0)
    {
        SiwEventLog(NULL, SIW_EVENT_SYM(TRM, TRAP_SEND_FAIL), trapFd, ipAddr, udpPort);
        SIW_DEBUG_TRACE(TRM, SendSnmpTrap, 0xF0000003, TRACE_LEVEL_1, 
            trapFd, ipAddr, udpPort,0,0);
    }

    SrFreeSnmpMessage(snmp_msg);
}



