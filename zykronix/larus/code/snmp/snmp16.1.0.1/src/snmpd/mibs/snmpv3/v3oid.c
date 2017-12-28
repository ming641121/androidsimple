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
 * -c v3 vacmAccessTable snmpEngine vacmContextTable vacmSecurityToGroupTable snmpMPDStats snmpNotifyTable snmpNotifyFilterProfileTable snmpNotifyFilterTable snmpProxyTable snmpTargetObjects snmpTargetAddrTable snmpTargetParamsTable snmpTargetAddrExtTable usmStats usmUser usmUserTable vacmMIBViews vacmViewTreeFamilyTable usmTargetTagTable -apo -row_status -userpart -separate_type_file 
 */

/*
 * automatically generated by the mib compiler -- do not edit
 */

/* this macro defines the oid value within the structure, including a comma */
#define OIDvalue(name, value) { name , (SR_UINT32 *) value },
/* this macro includes a name string if LIGHT is not defined */
#ifndef LIGHT
#define OIDincludeST(name) (char *) name ,
#else	/* LIGHT */
#define OIDincludeST(name)
#endif	/* LIGHT */
/* this macro includes the access routines in the OID structure */
#ifdef SETS
#define OIDaccess(getr, testr) (GetMethod)getr, (TestMethod)testr
#else /* SETS */
#define OIDaccess(getr, testr) (GetMethod)getr, (TestMethod)readOnly_test
#endif /* SETS */

/* define the oid call table */


#if defined(I_usmTargetTag)
{
    OIDvalue(LNusmTargetTag, IDusmTargetTag)
    OIDincludeST(STusmTargetTag)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_usmTargetTag, 32,
    OIDaccess(usmTargetTagEntry_get, usmTargetTagEntry_test)
},
#endif	/* defined(I_usmTargetTag) */

#if defined(I_snmpEngineID)
{
    OIDvalue(LNsnmpEngineID, IDsnmpEngineID)
    OIDincludeST(STsnmpEngineID)
    OCTET_PRIM_TYPE, SR_READ_ONLY, I_snmpEngineID, 32,
    OIDaccess(snmpEngine_get, readOnly_test)
},
#endif	/* defined(I_snmpEngineID) */

#if defined(I_snmpEngineBoots)
{
    OIDvalue(LNsnmpEngineBoots, IDsnmpEngineBoots)
    OIDincludeST(STsnmpEngineBoots)
    INTEGER_TYPE, SR_READ_ONLY, I_snmpEngineBoots, 32,
    OIDaccess(snmpEngine_get, readOnly_test)
},
#endif	/* defined(I_snmpEngineBoots) */

#if defined(I_snmpEngineTime)
{
    OIDvalue(LNsnmpEngineTime, IDsnmpEngineTime)
    OIDincludeST(STsnmpEngineTime)
    INTEGER_TYPE, SR_READ_ONLY, I_snmpEngineTime, 32,
    OIDaccess(snmpEngine_get, readOnly_test)
},
#endif	/* defined(I_snmpEngineTime) */

#if defined(I_snmpEngineMaxMessageSize)
{
    OIDvalue(LNsnmpEngineMaxMessageSize, IDsnmpEngineMaxMessageSize)
    OIDincludeST(STsnmpEngineMaxMessageSize)
    INTEGER_TYPE, SR_READ_ONLY, I_snmpEngineMaxMessageSize, 32,
    OIDaccess(snmpEngine_get, readOnly_test)
},
#endif	/* defined(I_snmpEngineMaxMessageSize) */

#if defined(I_snmpUnknownSecurityModels)
{
    OIDvalue(LNsnmpUnknownSecurityModels, IDsnmpUnknownSecurityModels)
    OIDincludeST(STsnmpUnknownSecurityModels)
    COUNTER_32_TYPE, SR_READ_ONLY, I_snmpUnknownSecurityModels, 32,
    OIDaccess(snmpMPDStats_get, readOnly_test)
},
#endif	/* defined(I_snmpUnknownSecurityModels) */

#if defined(I_snmpInvalidMsgs)
{
    OIDvalue(LNsnmpInvalidMsgs, IDsnmpInvalidMsgs)
    OIDincludeST(STsnmpInvalidMsgs)
    COUNTER_32_TYPE, SR_READ_ONLY, I_snmpInvalidMsgs, 32,
    OIDaccess(snmpMPDStats_get, readOnly_test)
},
#endif	/* defined(I_snmpInvalidMsgs) */

#if defined(I_snmpUnknownPDUHandlers)
{
    OIDvalue(LNsnmpUnknownPDUHandlers, IDsnmpUnknownPDUHandlers)
    OIDincludeST(STsnmpUnknownPDUHandlers)
    COUNTER_32_TYPE, SR_READ_ONLY, I_snmpUnknownPDUHandlers, 32,
    OIDaccess(snmpMPDStats_get, readOnly_test)
},
#endif	/* defined(I_snmpUnknownPDUHandlers) */

#if defined(I_snmpTargetSpinLock)
{
    OIDvalue(LNsnmpTargetSpinLock, IDsnmpTargetSpinLock)
    OIDincludeST(STsnmpTargetSpinLock)
    INTEGER_TYPE, SR_READ_WRITE, I_snmpTargetSpinLock, 32,
    OIDaccess(snmpTargetObjects_get, snmpTargetObjects_test)
},
#endif	/* defined(I_snmpTargetSpinLock) */



#if defined(I_snmpTargetAddrTDomain)
{
    OIDvalue(LNsnmpTargetAddrTDomain, IDsnmpTargetAddrTDomain)
    OIDincludeST(STsnmpTargetAddrTDomain)
    OBJECT_ID_TYPE, SR_READ_CREATE, I_snmpTargetAddrTDomain, 32,
    OIDaccess(snmpTargetAddrEntry_get, snmpTargetAddrEntry_test)
},
#endif	/* defined(I_snmpTargetAddrTDomain) */

#if defined(I_snmpTargetAddrTAddress)
{
    OIDvalue(LNsnmpTargetAddrTAddress, IDsnmpTargetAddrTAddress)
    OIDincludeST(STsnmpTargetAddrTAddress)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpTargetAddrTAddress, 32,
    OIDaccess(snmpTargetAddrEntry_get, snmpTargetAddrEntry_test)
},
#endif	/* defined(I_snmpTargetAddrTAddress) */

#if defined(I_snmpTargetAddrTimeout)
{
    OIDvalue(LNsnmpTargetAddrTimeout, IDsnmpTargetAddrTimeout)
    OIDincludeST(STsnmpTargetAddrTimeout)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetAddrTimeout, 32,
    OIDaccess(snmpTargetAddrEntry_get, snmpTargetAddrEntry_test)
},
#endif	/* defined(I_snmpTargetAddrTimeout) */

#if defined(I_snmpTargetAddrRetryCount)
{
    OIDvalue(LNsnmpTargetAddrRetryCount, IDsnmpTargetAddrRetryCount)
    OIDincludeST(STsnmpTargetAddrRetryCount)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetAddrRetryCount, 32,
    OIDaccess(snmpTargetAddrEntry_get, snmpTargetAddrEntry_test)
},
#endif	/* defined(I_snmpTargetAddrRetryCount) */

#if defined(I_snmpTargetAddrTagList)
{
    OIDvalue(LNsnmpTargetAddrTagList, IDsnmpTargetAddrTagList)
    OIDincludeST(STsnmpTargetAddrTagList)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpTargetAddrTagList, 32,
    OIDaccess(snmpTargetAddrEntry_get, snmpTargetAddrEntry_test)
},
#endif	/* defined(I_snmpTargetAddrTagList) */

#if defined(I_snmpTargetAddrParams)
{
    OIDvalue(LNsnmpTargetAddrParams, IDsnmpTargetAddrParams)
    OIDincludeST(STsnmpTargetAddrParams)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpTargetAddrParams, 32,
    OIDaccess(snmpTargetAddrEntry_get, snmpTargetAddrEntry_test)
},
#endif	/* defined(I_snmpTargetAddrParams) */

#if defined(I_snmpTargetAddrStorageType)
{
    OIDvalue(LNsnmpTargetAddrStorageType, IDsnmpTargetAddrStorageType)
    OIDincludeST(STsnmpTargetAddrStorageType)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetAddrStorageType, 32,
    OIDaccess(snmpTargetAddrEntry_get, snmpTargetAddrEntry_test)
},
#endif	/* defined(I_snmpTargetAddrStorageType) */

#if defined(I_snmpTargetAddrRowStatus)
{
    OIDvalue(LNsnmpTargetAddrRowStatus, IDsnmpTargetAddrRowStatus)
    OIDincludeST(STsnmpTargetAddrRowStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetAddrRowStatus, 32,
    OIDaccess(snmpTargetAddrEntry_get, snmpTargetAddrEntry_test)
},
#endif	/* defined(I_snmpTargetAddrRowStatus) */



#if defined(I_snmpTargetParamsMPModel)
{
    OIDvalue(LNsnmpTargetParamsMPModel, IDsnmpTargetParamsMPModel)
    OIDincludeST(STsnmpTargetParamsMPModel)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetParamsMPModel, 32,
    OIDaccess(snmpTargetParamsEntry_get, snmpTargetParamsEntry_test)
},
#endif	/* defined(I_snmpTargetParamsMPModel) */

#if defined(I_snmpTargetParamsSecurityModel)
{
    OIDvalue(LNsnmpTargetParamsSecurityModel, IDsnmpTargetParamsSecurityModel)
    OIDincludeST(STsnmpTargetParamsSecurityModel)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetParamsSecurityModel, 32,
    OIDaccess(snmpTargetParamsEntry_get, snmpTargetParamsEntry_test)
},
#endif	/* defined(I_snmpTargetParamsSecurityModel) */

#if defined(I_snmpTargetParamsSecurityName)
{
    OIDvalue(LNsnmpTargetParamsSecurityName, IDsnmpTargetParamsSecurityName)
    OIDincludeST(STsnmpTargetParamsSecurityName)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpTargetParamsSecurityName, 32,
    OIDaccess(snmpTargetParamsEntry_get, snmpTargetParamsEntry_test)
},
#endif	/* defined(I_snmpTargetParamsSecurityName) */

#if defined(I_snmpTargetParamsSecurityLevel)
{
    OIDvalue(LNsnmpTargetParamsSecurityLevel, IDsnmpTargetParamsSecurityLevel)
    OIDincludeST(STsnmpTargetParamsSecurityLevel)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetParamsSecurityLevel, 32,
    OIDaccess(snmpTargetParamsEntry_get, snmpTargetParamsEntry_test)
},
#endif	/* defined(I_snmpTargetParamsSecurityLevel) */

#if defined(I_snmpTargetParamsStorageType)
{
    OIDvalue(LNsnmpTargetParamsStorageType, IDsnmpTargetParamsStorageType)
    OIDincludeST(STsnmpTargetParamsStorageType)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetParamsStorageType, 32,
    OIDaccess(snmpTargetParamsEntry_get, snmpTargetParamsEntry_test)
},
#endif	/* defined(I_snmpTargetParamsStorageType) */

#if defined(I_snmpTargetParamsRowStatus)
{
    OIDvalue(LNsnmpTargetParamsRowStatus, IDsnmpTargetParamsRowStatus)
    OIDincludeST(STsnmpTargetParamsRowStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetParamsRowStatus, 32,
    OIDaccess(snmpTargetParamsEntry_get, snmpTargetParamsEntry_test)
},
#endif	/* defined(I_snmpTargetParamsRowStatus) */

#if defined(I_snmpUnavailableContexts)
{
    OIDvalue(LNsnmpUnavailableContexts, IDsnmpUnavailableContexts)
    OIDincludeST(STsnmpUnavailableContexts)
    COUNTER_32_TYPE, SR_READ_ONLY, I_snmpUnavailableContexts, 32,
    OIDaccess(snmpTargetObjects_get, readOnly_test)
},
#endif	/* defined(I_snmpUnavailableContexts) */

#if defined(I_snmpUnknownContexts)
{
    OIDvalue(LNsnmpUnknownContexts, IDsnmpUnknownContexts)
    OIDincludeST(STsnmpUnknownContexts)
    COUNTER_32_TYPE, SR_READ_ONLY, I_snmpUnknownContexts, 32,
    OIDaccess(snmpTargetObjects_get, readOnly_test)
},
#endif	/* defined(I_snmpUnknownContexts) */



#if defined(I_snmpNotifyTag)
{
    OIDvalue(LNsnmpNotifyTag, IDsnmpNotifyTag)
    OIDincludeST(STsnmpNotifyTag)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpNotifyTag, 32,
    OIDaccess(snmpNotifyEntry_get, snmpNotifyEntry_test)
},
#endif	/* defined(I_snmpNotifyTag) */

#if defined(I_snmpNotifyType)
{
    OIDvalue(LNsnmpNotifyType, IDsnmpNotifyType)
    OIDincludeST(STsnmpNotifyType)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpNotifyType, 32,
    OIDaccess(snmpNotifyEntry_get, snmpNotifyEntry_test)
},
#endif	/* defined(I_snmpNotifyType) */

#if defined(I_snmpNotifyStorageType)
{
    OIDvalue(LNsnmpNotifyStorageType, IDsnmpNotifyStorageType)
    OIDincludeST(STsnmpNotifyStorageType)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpNotifyStorageType, 32,
    OIDaccess(snmpNotifyEntry_get, snmpNotifyEntry_test)
},
#endif	/* defined(I_snmpNotifyStorageType) */

#if defined(I_snmpNotifyRowStatus)
{
    OIDvalue(LNsnmpNotifyRowStatus, IDsnmpNotifyRowStatus)
    OIDincludeST(STsnmpNotifyRowStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpNotifyRowStatus, 32,
    OIDaccess(snmpNotifyEntry_get, snmpNotifyEntry_test)
},
#endif	/* defined(I_snmpNotifyRowStatus) */



#if defined(I_snmpNotifyFilterProfileName)
{
    OIDvalue(LNsnmpNotifyFilterProfileName, IDsnmpNotifyFilterProfileName)
    OIDincludeST(STsnmpNotifyFilterProfileName)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpNotifyFilterProfileName, 32,
    OIDaccess(snmpNotifyFilterProfileEntry_get, snmpNotifyFilterProfileEntry_test)
},
#endif	/* defined(I_snmpNotifyFilterProfileName) */

#if defined(I_snmpNotifyFilterProfileStorType)
{
    OIDvalue(LNsnmpNotifyFilterProfileStorType, IDsnmpNotifyFilterProfileStorType)
    OIDincludeST(STsnmpNotifyFilterProfileStorType)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpNotifyFilterProfileStorType, 32,
    OIDaccess(snmpNotifyFilterProfileEntry_get, snmpNotifyFilterProfileEntry_test)
},
#endif	/* defined(I_snmpNotifyFilterProfileStorType) */

#if defined(I_snmpNotifyFilterProfileRowStatus)
{
    OIDvalue(LNsnmpNotifyFilterProfileRowStatus, IDsnmpNotifyFilterProfileRowStatus)
    OIDincludeST(STsnmpNotifyFilterProfileRowStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpNotifyFilterProfileRowStatus, 32,
    OIDaccess(snmpNotifyFilterProfileEntry_get, snmpNotifyFilterProfileEntry_test)
},
#endif	/* defined(I_snmpNotifyFilterProfileRowStatus) */



#if defined(I_snmpNotifyFilterMask)
{
    OIDvalue(LNsnmpNotifyFilterMask, IDsnmpNotifyFilterMask)
    OIDincludeST(STsnmpNotifyFilterMask)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpNotifyFilterMask, 32,
    OIDaccess(snmpNotifyFilterEntry_get, snmpNotifyFilterEntry_test)
},
#endif	/* defined(I_snmpNotifyFilterMask) */

#if defined(I_snmpNotifyFilterType)
{
    OIDvalue(LNsnmpNotifyFilterType, IDsnmpNotifyFilterType)
    OIDincludeST(STsnmpNotifyFilterType)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpNotifyFilterType, 32,
    OIDaccess(snmpNotifyFilterEntry_get, snmpNotifyFilterEntry_test)
},
#endif	/* defined(I_snmpNotifyFilterType) */

#if defined(I_snmpNotifyFilterStorageType)
{
    OIDvalue(LNsnmpNotifyFilterStorageType, IDsnmpNotifyFilterStorageType)
    OIDincludeST(STsnmpNotifyFilterStorageType)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpNotifyFilterStorageType, 32,
    OIDaccess(snmpNotifyFilterEntry_get, snmpNotifyFilterEntry_test)
},
#endif	/* defined(I_snmpNotifyFilterStorageType) */

#if defined(I_snmpNotifyFilterRowStatus)
{
    OIDvalue(LNsnmpNotifyFilterRowStatus, IDsnmpNotifyFilterRowStatus)
    OIDincludeST(STsnmpNotifyFilterRowStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpNotifyFilterRowStatus, 32,
    OIDaccess(snmpNotifyFilterEntry_get, snmpNotifyFilterEntry_test)
},
#endif	/* defined(I_snmpNotifyFilterRowStatus) */



#if defined(I_snmpProxyType)
{
    OIDvalue(LNsnmpProxyType, IDsnmpProxyType)
    OIDincludeST(STsnmpProxyType)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpProxyType, 32,
    OIDaccess(snmpProxyEntry_get, snmpProxyEntry_test)
},
#endif	/* defined(I_snmpProxyType) */

#if defined(I_snmpProxyContextEngineID)
{
    OIDvalue(LNsnmpProxyContextEngineID, IDsnmpProxyContextEngineID)
    OIDincludeST(STsnmpProxyContextEngineID)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpProxyContextEngineID, 32,
    OIDaccess(snmpProxyEntry_get, snmpProxyEntry_test)
},
#endif	/* defined(I_snmpProxyContextEngineID) */

#if defined(I_snmpProxyContextName)
{
    OIDvalue(LNsnmpProxyContextName, IDsnmpProxyContextName)
    OIDincludeST(STsnmpProxyContextName)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpProxyContextName, 32,
    OIDaccess(snmpProxyEntry_get, snmpProxyEntry_test)
},
#endif	/* defined(I_snmpProxyContextName) */

#if defined(I_snmpProxyTargetParamsIn)
{
    OIDvalue(LNsnmpProxyTargetParamsIn, IDsnmpProxyTargetParamsIn)
    OIDincludeST(STsnmpProxyTargetParamsIn)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpProxyTargetParamsIn, 32,
    OIDaccess(snmpProxyEntry_get, snmpProxyEntry_test)
},
#endif	/* defined(I_snmpProxyTargetParamsIn) */

#if defined(I_snmpProxySingleTargetOut)
{
    OIDvalue(LNsnmpProxySingleTargetOut, IDsnmpProxySingleTargetOut)
    OIDincludeST(STsnmpProxySingleTargetOut)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpProxySingleTargetOut, 32,
    OIDaccess(snmpProxyEntry_get, snmpProxyEntry_test)
},
#endif	/* defined(I_snmpProxySingleTargetOut) */

#if defined(I_snmpProxyMultipleTargetOut)
{
    OIDvalue(LNsnmpProxyMultipleTargetOut, IDsnmpProxyMultipleTargetOut)
    OIDincludeST(STsnmpProxyMultipleTargetOut)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpProxyMultipleTargetOut, 32,
    OIDaccess(snmpProxyEntry_get, snmpProxyEntry_test)
},
#endif	/* defined(I_snmpProxyMultipleTargetOut) */

#if defined(I_snmpProxyStorageType)
{
    OIDvalue(LNsnmpProxyStorageType, IDsnmpProxyStorageType)
    OIDincludeST(STsnmpProxyStorageType)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpProxyStorageType, 32,
    OIDaccess(snmpProxyEntry_get, snmpProxyEntry_test)
},
#endif	/* defined(I_snmpProxyStorageType) */

#if defined(I_snmpProxyRowStatus)
{
    OIDvalue(LNsnmpProxyRowStatus, IDsnmpProxyRowStatus)
    OIDincludeST(STsnmpProxyRowStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpProxyRowStatus, 32,
    OIDaccess(snmpProxyEntry_get, snmpProxyEntry_test)
},
#endif	/* defined(I_snmpProxyRowStatus) */

#if defined(I_usmStatsUnsupportedSecLevels)
{
    OIDvalue(LNusmStatsUnsupportedSecLevels, IDusmStatsUnsupportedSecLevels)
    OIDincludeST(STusmStatsUnsupportedSecLevels)
    COUNTER_32_TYPE, SR_READ_ONLY, I_usmStatsUnsupportedSecLevels, 32,
    OIDaccess(usmStats_get, readOnly_test)
},
#endif	/* defined(I_usmStatsUnsupportedSecLevels) */

#if defined(I_usmStatsNotInTimeWindows)
{
    OIDvalue(LNusmStatsNotInTimeWindows, IDusmStatsNotInTimeWindows)
    OIDincludeST(STusmStatsNotInTimeWindows)
    COUNTER_32_TYPE, SR_READ_ONLY, I_usmStatsNotInTimeWindows, 32,
    OIDaccess(usmStats_get, readOnly_test)
},
#endif	/* defined(I_usmStatsNotInTimeWindows) */

#if defined(I_usmStatsUnknownUserNames)
{
    OIDvalue(LNusmStatsUnknownUserNames, IDusmStatsUnknownUserNames)
    OIDincludeST(STusmStatsUnknownUserNames)
    COUNTER_32_TYPE, SR_READ_ONLY, I_usmStatsUnknownUserNames, 32,
    OIDaccess(usmStats_get, readOnly_test)
},
#endif	/* defined(I_usmStatsUnknownUserNames) */

#if defined(I_usmStatsUnknownEngineIDs)
{
    OIDvalue(LNusmStatsUnknownEngineIDs, IDusmStatsUnknownEngineIDs)
    OIDincludeST(STusmStatsUnknownEngineIDs)
    COUNTER_32_TYPE, SR_READ_ONLY, I_usmStatsUnknownEngineIDs, 32,
    OIDaccess(usmStats_get, readOnly_test)
},
#endif	/* defined(I_usmStatsUnknownEngineIDs) */

#if defined(I_usmStatsWrongDigests)
{
    OIDvalue(LNusmStatsWrongDigests, IDusmStatsWrongDigests)
    OIDincludeST(STusmStatsWrongDigests)
    COUNTER_32_TYPE, SR_READ_ONLY, I_usmStatsWrongDigests, 32,
    OIDaccess(usmStats_get, readOnly_test)
},
#endif	/* defined(I_usmStatsWrongDigests) */

#if defined(I_usmStatsDecryptionErrors)
{
    OIDvalue(LNusmStatsDecryptionErrors, IDusmStatsDecryptionErrors)
    OIDincludeST(STusmStatsDecryptionErrors)
    COUNTER_32_TYPE, SR_READ_ONLY, I_usmStatsDecryptionErrors, 32,
    OIDaccess(usmStats_get, readOnly_test)
},
#endif	/* defined(I_usmStatsDecryptionErrors) */

#if defined(I_usmUserSpinLock)
{
    OIDvalue(LNusmUserSpinLock, IDusmUserSpinLock)
    OIDincludeST(STusmUserSpinLock)
    INTEGER_TYPE, SR_READ_WRITE, I_usmUserSpinLock, 32,
    OIDaccess(usmUser_get, usmUser_test)
},
#endif	/* defined(I_usmUserSpinLock) */



#if defined(I_usmUserSecurityName)
{
    OIDvalue(LNusmUserSecurityName, IDusmUserSecurityName)
    OIDincludeST(STusmUserSecurityName)
    OCTET_PRIM_TYPE, SR_READ_ONLY, I_usmUserSecurityName, 32,
    OIDaccess(usmUserEntry_get, readOnly_test)
},
#endif	/* defined(I_usmUserSecurityName) */

#if defined(I_usmUserCloneFrom)
{
    OIDvalue(LNusmUserCloneFrom, IDusmUserCloneFrom)
    OIDincludeST(STusmUserCloneFrom)
    OBJECT_ID_TYPE, SR_READ_CREATE, I_usmUserCloneFrom, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserCloneFrom) */

#if defined(I_usmUserAuthProtocol)
{
    OIDvalue(LNusmUserAuthProtocol, IDusmUserAuthProtocol)
    OIDincludeST(STusmUserAuthProtocol)
    OBJECT_ID_TYPE, SR_READ_CREATE, I_usmUserAuthProtocol, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserAuthProtocol) */

#if defined(I_usmUserAuthKeyChange)
{
    OIDvalue(LNusmUserAuthKeyChange, IDusmUserAuthKeyChange)
    OIDincludeST(STusmUserAuthKeyChange)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_usmUserAuthKeyChange, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserAuthKeyChange) */

#if defined(I_usmUserOwnAuthKeyChange)
{
    OIDvalue(LNusmUserOwnAuthKeyChange, IDusmUserOwnAuthKeyChange)
    OIDincludeST(STusmUserOwnAuthKeyChange)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_usmUserOwnAuthKeyChange, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserOwnAuthKeyChange) */

#if defined(I_usmUserPrivProtocol)
{
    OIDvalue(LNusmUserPrivProtocol, IDusmUserPrivProtocol)
    OIDincludeST(STusmUserPrivProtocol)
    OBJECT_ID_TYPE, SR_READ_CREATE, I_usmUserPrivProtocol, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserPrivProtocol) */

#if defined(I_usmUserPrivKeyChange)
{
    OIDvalue(LNusmUserPrivKeyChange, IDusmUserPrivKeyChange)
    OIDincludeST(STusmUserPrivKeyChange)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_usmUserPrivKeyChange, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserPrivKeyChange) */

#if defined(I_usmUserOwnPrivKeyChange)
{
    OIDvalue(LNusmUserOwnPrivKeyChange, IDusmUserOwnPrivKeyChange)
    OIDincludeST(STusmUserOwnPrivKeyChange)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_usmUserOwnPrivKeyChange, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserOwnPrivKeyChange) */

#if defined(I_usmUserPublic)
{
    OIDvalue(LNusmUserPublic, IDusmUserPublic)
    OIDincludeST(STusmUserPublic)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_usmUserPublic, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserPublic) */

#if defined(I_usmUserStorageType)
{
    OIDvalue(LNusmUserStorageType, IDusmUserStorageType)
    OIDincludeST(STusmUserStorageType)
    INTEGER_TYPE, SR_READ_CREATE, I_usmUserStorageType, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserStorageType) */

#if defined(I_usmUserStatus)
{
    OIDvalue(LNusmUserStatus, IDusmUserStatus)
    OIDincludeST(STusmUserStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_usmUserStatus, 32,
    OIDaccess(usmUserEntry_get, usmUserEntry_test)
},
#endif	/* defined(I_usmUserStatus) */



#if defined(I_vacmContextName)
{
    OIDvalue(LNvacmContextName, IDvacmContextName)
    OIDincludeST(STvacmContextName)
    OCTET_PRIM_TYPE, SR_READ_ONLY, I_vacmContextName, 32,
    OIDaccess(vacmContextEntry_get, readOnly_test)
},
#endif	/* defined(I_vacmContextName) */



#if defined(I_vacmGroupName)
{
    OIDvalue(LNvacmGroupName, IDvacmGroupName)
    OIDincludeST(STvacmGroupName)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_vacmGroupName, 32,
    OIDaccess(vacmSecurityToGroupEntry_get, vacmSecurityToGroupEntry_test)
},
#endif	/* defined(I_vacmGroupName) */

#if defined(I_vacmSecurityToGroupStorageType)
{
    OIDvalue(LNvacmSecurityToGroupStorageType, IDvacmSecurityToGroupStorageType)
    OIDincludeST(STvacmSecurityToGroupStorageType)
    INTEGER_TYPE, SR_READ_CREATE, I_vacmSecurityToGroupStorageType, 32,
    OIDaccess(vacmSecurityToGroupEntry_get, vacmSecurityToGroupEntry_test)
},
#endif	/* defined(I_vacmSecurityToGroupStorageType) */

#if defined(I_vacmSecurityToGroupStatus)
{
    OIDvalue(LNvacmSecurityToGroupStatus, IDvacmSecurityToGroupStatus)
    OIDincludeST(STvacmSecurityToGroupStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_vacmSecurityToGroupStatus, 32,
    OIDaccess(vacmSecurityToGroupEntry_get, vacmSecurityToGroupEntry_test)
},
#endif	/* defined(I_vacmSecurityToGroupStatus) */



#if defined(I_vacmAccessContextMatch)
{
    OIDvalue(LNvacmAccessContextMatch, IDvacmAccessContextMatch)
    OIDincludeST(STvacmAccessContextMatch)
    INTEGER_TYPE, SR_READ_CREATE, I_vacmAccessContextMatch, 32,
    OIDaccess(vacmAccessEntry_get, vacmAccessEntry_test)
},
#endif	/* defined(I_vacmAccessContextMatch) */

#if defined(I_vacmAccessReadViewName)
{
    OIDvalue(LNvacmAccessReadViewName, IDvacmAccessReadViewName)
    OIDincludeST(STvacmAccessReadViewName)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_vacmAccessReadViewName, 32,
    OIDaccess(vacmAccessEntry_get, vacmAccessEntry_test)
},
#endif	/* defined(I_vacmAccessReadViewName) */

#if defined(I_vacmAccessWriteViewName)
{
    OIDvalue(LNvacmAccessWriteViewName, IDvacmAccessWriteViewName)
    OIDincludeST(STvacmAccessWriteViewName)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_vacmAccessWriteViewName, 32,
    OIDaccess(vacmAccessEntry_get, vacmAccessEntry_test)
},
#endif	/* defined(I_vacmAccessWriteViewName) */

#if defined(I_vacmAccessNotifyViewName)
{
    OIDvalue(LNvacmAccessNotifyViewName, IDvacmAccessNotifyViewName)
    OIDincludeST(STvacmAccessNotifyViewName)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_vacmAccessNotifyViewName, 32,
    OIDaccess(vacmAccessEntry_get, vacmAccessEntry_test)
},
#endif	/* defined(I_vacmAccessNotifyViewName) */

#if defined(I_vacmAccessStorageType)
{
    OIDvalue(LNvacmAccessStorageType, IDvacmAccessStorageType)
    OIDincludeST(STvacmAccessStorageType)
    INTEGER_TYPE, SR_READ_CREATE, I_vacmAccessStorageType, 32,
    OIDaccess(vacmAccessEntry_get, vacmAccessEntry_test)
},
#endif	/* defined(I_vacmAccessStorageType) */

#if defined(I_vacmAccessStatus)
{
    OIDvalue(LNvacmAccessStatus, IDvacmAccessStatus)
    OIDincludeST(STvacmAccessStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_vacmAccessStatus, 32,
    OIDaccess(vacmAccessEntry_get, vacmAccessEntry_test)
},
#endif	/* defined(I_vacmAccessStatus) */

#if defined(I_vacmViewSpinLock)
{
    OIDvalue(LNvacmViewSpinLock, IDvacmViewSpinLock)
    OIDincludeST(STvacmViewSpinLock)
    INTEGER_TYPE, SR_READ_WRITE, I_vacmViewSpinLock, 32,
    OIDaccess(vacmMIBViews_get, vacmMIBViews_test)
},
#endif	/* defined(I_vacmViewSpinLock) */



#if defined(I_vacmViewTreeFamilyMask)
{
    OIDvalue(LNvacmViewTreeFamilyMask, IDvacmViewTreeFamilyMask)
    OIDincludeST(STvacmViewTreeFamilyMask)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_vacmViewTreeFamilyMask, 32,
    OIDaccess(vacmViewTreeFamilyEntry_get, vacmViewTreeFamilyEntry_test)
},
#endif	/* defined(I_vacmViewTreeFamilyMask) */

#if defined(I_vacmViewTreeFamilyType)
{
    OIDvalue(LNvacmViewTreeFamilyType, IDvacmViewTreeFamilyType)
    OIDincludeST(STvacmViewTreeFamilyType)
    INTEGER_TYPE, SR_READ_CREATE, I_vacmViewTreeFamilyType, 32,
    OIDaccess(vacmViewTreeFamilyEntry_get, vacmViewTreeFamilyEntry_test)
},
#endif	/* defined(I_vacmViewTreeFamilyType) */

#if defined(I_vacmViewTreeFamilyStorageType)
{
    OIDvalue(LNvacmViewTreeFamilyStorageType, IDvacmViewTreeFamilyStorageType)
    OIDincludeST(STvacmViewTreeFamilyStorageType)
    INTEGER_TYPE, SR_READ_CREATE, I_vacmViewTreeFamilyStorageType, 32,
    OIDaccess(vacmViewTreeFamilyEntry_get, vacmViewTreeFamilyEntry_test)
},
#endif	/* defined(I_vacmViewTreeFamilyStorageType) */

#if defined(I_vacmViewTreeFamilyStatus)
{
    OIDvalue(LNvacmViewTreeFamilyStatus, IDvacmViewTreeFamilyStatus)
    OIDincludeST(STvacmViewTreeFamilyStatus)
    INTEGER_TYPE, SR_READ_CREATE, I_vacmViewTreeFamilyStatus, 32,
    OIDaccess(vacmViewTreeFamilyEntry_get, vacmViewTreeFamilyEntry_test)
},
#endif	/* defined(I_vacmViewTreeFamilyStatus) */



#if defined(I_snmpTargetAddrTMask)
{
    OIDvalue(LNsnmpTargetAddrTMask, IDsnmpTargetAddrTMask)
    OIDincludeST(STsnmpTargetAddrTMask)
    OCTET_PRIM_TYPE, SR_READ_CREATE, I_snmpTargetAddrTMask, 32,
    OIDaccess(snmpTargetAddrExtEntry_get, snmpTargetAddrExtEntry_test)
},
#endif	/* defined(I_snmpTargetAddrTMask) */

#if defined(I_snmpTargetAddrMMS)
{
    OIDvalue(LNsnmpTargetAddrMMS, IDsnmpTargetAddrMMS)
    OIDincludeST(STsnmpTargetAddrMMS)
    INTEGER_TYPE, SR_READ_CREATE, I_snmpTargetAddrMMS, 32,
    OIDaccess(snmpTargetAddrExtEntry_get, snmpTargetAddrExtEntry_test)
},
#endif	/* defined(I_snmpTargetAddrMMS) */
