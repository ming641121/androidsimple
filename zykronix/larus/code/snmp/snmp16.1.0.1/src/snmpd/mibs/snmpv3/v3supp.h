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

#ifndef SR_V3SUPP_H
#define SR_V3SUPP_H


#ifdef __cplusplus
extern "C" {
#endif


#define I_snmpEngineID                                      0
#define I_snmpEngineBoots                                   1
#define I_snmpEngineTime                                    2
#define I_snmpEngineMaxMessageSize                          3
#define I_snmpEngine_max                                    3

#define I_snmpUnknownSecurityModels                         0
#define I_snmpInvalidMsgs                                   1
#define I_snmpUnknownPDUHandlers                            2
#define I_snmpMPDStats_max                                  2

#define I_snmpTargetSpinLock                                0
#define I_snmpUnavailableContexts                           1
#define I_snmpUnknownContexts                               2
#define I_snmpTargetObjects_max                             2

#define I_snmpTargetAddrName                                0
#define I_snmpTargetAddrTDomain                             1
#define I_snmpTargetAddrTAddress                            2
#define I_snmpTargetAddrTimeout                             3
#define I_snmpTargetAddrRetryCount                          4
#define I_snmpTargetAddrTagList                             5
#define I_snmpTargetAddrParams                              6
#define I_snmpTargetAddrStorageType                         7
#define I_snmpTargetAddrRowStatus                           8
#define I_snmpTargetAddrTMask                               9
#define I_snmpTargetAddrMMS                                 10
#define I_snmpTargetAddrEntry_max                           10

#define I_snmpTargetParamsName                              0
#define I_snmpTargetParamsMPModel                           1
#define I_snmpTargetParamsSecurityModel                     2
#define I_snmpTargetParamsSecurityName                      3
#define I_snmpTargetParamsSecurityLevel                     4
#define I_snmpTargetParamsStorageType                       5
#define I_snmpTargetParamsRowStatus                         6
#define I_snmpTargetParamsEntry_max                         6

#define I_snmpNotifyName                                    0
#define I_snmpNotifyTag                                     1
#define I_snmpNotifyType                                    2
#define I_snmpNotifyStorageType                             3
#define I_snmpNotifyRowStatus                               4
#define I_snmpNotifyEntry_max                               4

#define I_snmpNotifyFilterProfileName                       0
#define I_snmpNotifyFilterProfileStorType                   1
#define I_snmpNotifyFilterProfileRowStatus                  2
#define I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName 3
#define I_snmpNotifyFilterProfileEntry_max                  3

#define I_snmpNotifyFilterSubtree                           0
#define I_snmpNotifyFilterMask                              1
#define I_snmpNotifyFilterType                              2
#define I_snmpNotifyFilterStorageType                       3
#define I_snmpNotifyFilterRowStatus                         4
#define I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName 5
#define I_snmpNotifyFilterEntry_max                         5

#define I_snmpProxyName                                     0
#define I_snmpProxyType                                     1
#define I_snmpProxyContextEngineID                          2
#define I_snmpProxyContextName                              3
#define I_snmpProxyTargetParamsIn                           4
#define I_snmpProxySingleTargetOut                          5
#define I_snmpProxyMultipleTargetOut                        6
#define I_snmpProxyStorageType                              7
#define I_snmpProxyRowStatus                                8
#define I_snmpProxyEntry_max                                8

#define I_usmStatsUnsupportedSecLevels                      0
#define I_usmStatsNotInTimeWindows                          1
#define I_usmStatsUnknownUserNames                          2
#define I_usmStatsUnknownEngineIDs                          3
#define I_usmStatsWrongDigests                              4
#define I_usmStatsDecryptionErrors                          5
#define I_usmStats_max                                      5

#define I_usmUserSpinLock                                   0
#define I_usmUser_max                                       0

#define I_usmUserEngineID                                   0
#define I_usmUserName                                       1
#define I_usmUserSecurityName                               2
#define I_usmUserCloneFrom                                  3
#define I_usmUserAuthProtocol                               4
#define I_usmUserAuthKeyChange                              5
#define I_usmUserOwnAuthKeyChange                           6
#define I_usmUserPrivProtocol                               7
#define I_usmUserPrivKeyChange                              8
#define I_usmUserOwnPrivKeyChange                           9
#define I_usmUserPublic                                     10
#define I_usmUserStorageType                                11
#define I_usmUserStatus                                     12
#define I_usmTargetTag                                      13
#define I_usmUserEntry_max                                  13

#define I_vacmContextName                                   0
#define I_vacmContextEntry_max                              0

#define I_vacmSecurityModel                                 0
#define I_vacmSecurityName                                  1
#define I_vacmGroupName                                     2
#define I_vacmSecurityToGroupStorageType                    3
#define I_vacmSecurityToGroupStatus                         4
#define I_vacmSecurityToGroupEntry_max                      4

#define I_vacmAccessContextPrefix                           0
#define I_vacmAccessSecurityModel                           1
#define I_vacmAccessSecurityLevel                           2
#define I_vacmAccessContextMatch                            3
#define I_vacmAccessReadViewName                            4
#define I_vacmAccessWriteViewName                           5
#define I_vacmAccessNotifyViewName                          6
#define I_vacmAccessStorageType                             7
#define I_vacmAccessStatus                                  8
#define I_vacmAccessEntryIndex_vacmGroupName                9
#define I_vacmAccessEntry_max                               9

#define I_vacmViewSpinLock                                  0
#define I_vacmMIBViews_max                                  0

#define I_vacmViewTreeFamilyViewName                        0
#define I_vacmViewTreeFamilySubtree                         1
#define I_vacmViewTreeFamilyMask                            2
#define I_vacmViewTreeFamilyType                            3
#define I_vacmViewTreeFamilyStorageType                     4
#define I_vacmViewTreeFamilyStatus                          5
#define I_vacmViewTreeFamilyEntry_max                       5


#include "undefs.h"


#ifdef __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SR_V3SUPP_H */
