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
 * -o snmp -separate_type_file 
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

#ifndef SR_SNMPSUPP_H
#define SR_SNMPSUPP_H


#ifdef __cplusplus
extern "C" {
#endif

#define I_sysDescr                                          0
#define I_sysObjectID                                       1
#define I_system_max                                        1

#define I_stsSystemId                                       0
#define I_stsSystemMgmtIpOption                             1
#define I_stsSystemMgmtIpAddress                            2
#define I_stsSystemMgmtIpSubnetMask                         3
#define I_stsSystemMgmtIpGatewayAddress                     4
#define I_stsSystemNtpIpOption                              5
#define I_stsSystemNtpIpAddress                             6
#define I_stsSystemNtpIpSubnetMask                          7
#define I_stsSystemNtpIpGatewayAddress                      8
#define I_stsSystemDate                                     9
#define I_stsSystemTime                                     10
#define I_stsSystemTimeZone                                 11
#define I_stsSystemIntegratedAlarm                          12
#define I_stsSystemAlarmAcoMode                             13
#define I_stsSystemAlarmAcoLed                              14
#define I_stsSystemAlarmAcoCutOff                           15
#define I_stsSystemAlarmEscalatingTime                      16
#define I_stsSystemMinorLED                                 17
#define I_stsSystemMajorLED                                 18
#define I_stsSystemCriticalLED                              19
#define I_stsSystemUpgradeSWAction                          20
#define I_stsSystemCurrentSWVersion                         21
#define I_stsSystemServiceState                             22
#define I_stsSystemReset                                    23
#define I_stsSystem_max                                     23

#define I_stsClockInputManualSelect                         0
#define I_stsClockInputOperMode                             1
#define I_stsClockInputSetSlip                              2
#define I_stsClockInputSlipState                            3
#define I_stsClockInputSlipAlarmSetting                     4
#define I_stsClockInputSlipAlarmSeverity                    5
#define I_stsClockInputValue                                6
#define I_stsClockInput_max                                 6

#define I_stsClockInputSSMSelect                            0
#define I_stsClockInputSSM_max                              0

#define I_stsTopSourceIpOption                              0
#define I_stsTopSourceIpAddress                             1
#define I_stsTopSourceIpSubnetMask                          2
#define I_stsTopSourceIpGatewayAddress                      3
#define I_stsTopPacketRate                                  4
#define I_stsTopInputSelect                                 5
#define I_stsTopTxPackets                                   6
#define I_stsTopTxBytes                                     7
#define I_stsTop_max                                        7

#define I_stsPerfMonManualInputSelect                       0
#define I_stsPerfMonManualInputScanInterval                 1
#define I_stsPerfMonManualInputNumReport                    2
#define I_stsPerfMonManualInputNumObservation               3
#define I_stsPerfMonManualInputAbortTest                    4
#define I_stsPerfMonManual_max                              4

#define I_stsReadOnlyCommStr                                0
#define I_stsReadWriteCommStr                               1
#define I_stsCommStr_max                                    1

#define I_stsClockInputStatusIndex                          0
#define I_stsClockInputState                                1
#define I_stsClockInputStatusEntry_max                      1

#define I_stsClockInputSSMIndex                             0
#define I_stsClockInputSSMInputSelect                       1
#define I_stsClockInputSSMValue                             2
#define I_stsClockInputSSMEntry_max                         2

#define I_stsCardShelf                                      0
#define I_stsCardIndex                                      1
#define I_stsCardType                                       2
#define I_stsCardDescr                                      3
#define I_stsCardSerial                                     4
#define I_stsCardHwVersion                                  5
#define I_stsCardCLEICode                                   6
#define I_stsCardOperStatus                                 7
#define I_stsCardReset                                      8
#define I_stsCardEntry_max                                  8

#define I_stsOscillatorIndex                                0
#define I_stsOscillatorModuleType                           1
#define I_stsOscillatorEFER                                 2
#define I_stsOscillatorESSD                                 3
#define I_stsOscillatorESSN                                 4
#define I_stsOscillatorFAGE                                 5
#define I_stsOscillatorTIMD                                 6
#define I_stsOscillatorSwitchState                          7
#define I_stsOscillatorStatus                               8
#define I_stsOscillatorRedStatus                            9
#define I_stsOscillatorEntry_max                            9

#define I_stsGpsIndex                                       0
#define I_stsGpsStatus                                      1
#define I_stsGpsState                                       2
#define I_stsGpsANTD                                        3
#define I_stsGpsAZEL                                        4
#define I_stsGpsRSTG                                        5
#define I_stsGpsBITS                                        6
#define I_stsGpsPAVG                                        7
#define I_stsGpsSIGQ                                        8
#define I_stsGpsSPOS                                        9
#define I_stsGpsTRMO                                        10
#define I_stsGpsUTCT                                        11
#define I_stsGpsVERS                                        12
#define I_stsGpsDETF                                        13
#define I_stsGpsEFER                                        14
#define I_stsGpsESSD                                        15
#define I_stsGpsESSN                                        16
#define I_stsGpsFAGE                                        17
#define I_stsGpsTIMD                                        18
#define I_stsGpsReset                                       19
#define I_stsGpsEntry_max                                   19

#define I_stsLinkIndex                                      0
#define I_stsLinkType                                       1
#define I_stsLinkEnable                                     2
#define I_stsLinkLineCode                                   3
#define I_stsLinkFMT                                        4
#define I_stsLinkTermination                                5
#define I_stsLinkMFCHK                                      6
#define I_stsLinkId                                         7
#define I_stsLinkEntry_max                                  7

#define I_stsShelfIndex                                     0
#define I_stsShelfSerial                                    1
#define I_stsShelfCLEICode                                  2
#define I_stsShelfReset                                     3
#define I_stsShelfEntry_max                                 3

#define I_stsAlarmThresholdCurrentMon                       0
#define I_stsAlarmThresholdType                             1
#define I_stsAlarmThresholdValue                            2
#define I_stsAlarmThresholdTimeInterval                     3
#define I_stsAlarmThresholdEntry_max                        3

#define I_stsLinkAlarmSettingLinkType                       0
#define I_stsLinkAlarmSettingType                           1
#define I_stsLinkAlarmSettingSevirity                       2
#define I_stsLinkAlarmSevirity                              3
#define I_stsLinkAlarmEntry_max                             3

#define I_stsOscillatorAlarmSettingClock                    0
#define I_stsOscillatorAlarmSettingType                     1
#define I_stsOscillatorAlarmSettingSeverity                 2
#define I_stsOscillatorAlarmSeverity                        3
#define I_stsOscillatorAlarmEntry_max                       3

#define I_stssPwrAlarmSettingShelf                          0
#define I_stsOsPwrAlarmSettingOsPwr                         1
#define I_stsOsPwrAlarmSettingSeverity                      2
#define I_stsOsPwrAlarmSeverity                             3
#define I_stsOsPwrAlarmEntry_max                            3

#define I_stsOutputAlarmSettingShelf                        0
#define I_stsOutputAlarmSettingSlot                         1
#define I_stsOutputAlarmSettingPort                         2
#define I_stsOutputAlarmSettingType                         3
#define I_stsOutputAlarmSettingSeverity                     4
#define I_stsOutputAlarmSeverity                            5
#define I_stsOutputAlarmEntry_max                           5

#define I_stsOutputPortSettingShelf                         0
#define I_stsOutputPortSettingSlot                          1
#define I_stsOutputPortSettingPort                          2
#define I_stsOutputPortSettingType                          3
#define I_stsOutputPortSettingName                          4
#define I_stsOutputPortSettingEntry_max                     4

#define I_stsOutputTiuSettingShelf                          0
#define I_stsOutputTiuSettingSlot                           1
#define I_stsOutputTiuSettingPort                           2
#define I_stsOutputTiuSettingLineCoding                     3
#define I_stsOutputTiuSettingFMT                            4
#define I_stsOutputTiuSettingTermination                    5
#define I_stsOutputTiuSettingEntry_max                      5

#define I_stsPerfMonInputCurrentMon                         0
#define I_stsPerfMonInputMonPeriod                          1
#define I_stsPerfMonInputMonBucket                          2
#define I_stsPerfMonInputMonType                            3
#define I_stsPerfMonInputMonValue                           4
#define I_stsPerfMonInputClockType                          5
#define I_stsPerfMonEntry_max                               5

#define I_stsTerminalIndex                                  0
#define I_stsTerminalBaudrate                               1
#define I_stsTerminalDataBits                               2
#define I_stsTerminalParity                                 3
#define I_stsTerminalStopBits                               4
#define I_stsTerminalEntry_max                              4

#define I_stsAlarmSource                                    0
#define I_stsAlarmSourceType                                1
#define I_stsAlarmSeverity                                  2
#define I_stsAlarmSrvEff                                    3
#define I_stsAlarmStatusEntry_max                           3

#define I_stsOutputShelf                                    0
#define I_stsOutputSlot                                     1
#define I_stsOutputRedState                                 2
#define I_stsOutputRedEnableState                           3
#define I_stsOutputRedSwitchState                           4
#define I_stsOutputCRCCHK                                   5
#define I_stsOutputCAS                                      6
#define I_stsOutputSSM                                      7
#define I_stsOutputSSMValue                                 8
#define I_stsOutputTimingFormat                             9
#define I_stsOutputCardEntry_max                            9

#define I_stsEmmShelf                                       0
#define I_stsEmmSensorType                                  1
#define I_stsEmmAlarmStatus                                 2
#define I_stsEmmSensorReading                               3
#define I_stsEmmMajorAlmHiThresh                            4
#define I_stsEmmMajorAlmLoThresh                            5
#define I_stsEmmMinorAlmHiThresh                            6
#define I_stsEmmMinorAlmLoThresh                            7
#define I_stsEmmStatusEntry_max                             7

#define I_stsAntIndex                                       0
#define I_stsAntStatus                                      1
#define I_stsAntSwitchState                                 2
#define I_stsAntEntry_max                                   2

#define I_stsTopDestNumber                                  0
#define I_stsTopDestIp                                      1
#define I_stsTopDestUdpPort                                 2
#define I_stsTopDestVlanId                                  3
#define I_stsTopDestEntry_max                               3

#define I_stsTrapRegMgrNumber                               0
#define I_stsTrapRegMgrIp                                   1
#define I_stsTrapRegMgrUdpPort                              2
#define I_stsTrapRegCommunityStr                            3
#define I_stsTrapRegState                                   4
#define I_stsTrapRegEntry_max                               4

#define I_stsClockPriIndex                                  0
#define I_stsClockPriSetting                                1
#define I_stsClockPriEntry_max                              1

#include "undefs.h"


#ifdef __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SR_SNMPSUPP_H */
