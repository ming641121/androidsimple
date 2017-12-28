/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _AXSSTRAP_H_
#define _AXSSTRAP_H_

#include "axss_types.h"
#include "sr_conf.h"
#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "objectdb.h"
#include "snmpdefs.h"
#include "snmpsupp.h"
#include "snmppart.h"
#include "snmptype.h"
#include "trapobjs.h"


#define NULL_TRAPOID   NULL
#define OIDvalue(name, value) { name , (SR_UINT32 *) value }


static TRAP_OBJ_t stsSystemIpConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsSystemMgmtIpOption, IDstsSystemMgmtIpOption), NULL,   0, 0},
  {OIDvalue(LNstsSystemMgmtIpAddress, IDstsSystemMgmtIpAddress), NULL,   0, 0},
  {OIDvalue(LNstsSystemMgmtIpSubnetMask, IDstsSystemMgmtIpSubnetMask), NULL,   0, 0},
  {OIDvalue(LNstsSystemMgmtIpGatewayAddress, IDstsSystemMgmtIpGatewayAddress), NULL,   0, 0},
  {OIDvalue(LNstsSystemNtpIpOption, IDstsSystemNtpIpOption), NULL,   0, 0},
  {OIDvalue(LNstsSystemNtpIpAddress, IDstsSystemNtpIpAddress), NULL,   0, 0},
  {OIDvalue(LNstsSystemNtpIpSubnetMask, IDstsSystemNtpIpSubnetMask), NULL,   0, 0},
  {OIDvalue(LNstsSystemNtpIpGatewayAddress, IDstsSystemNtpIpGatewayAddress), NULL,   0, 0},
};

static TRAP_OBJ_t stsSystemConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsSystemDate, IDstsSystemDate), NULL,   0, 0},
  {OIDvalue(LNstsSystemTime, IDstsSystemTime), NULL,   0, 0},
  {OIDvalue(LNstsSystemTimeZone, IDstsSystemTimeZone), NULL,   0, 0},
  {OIDvalue(LNstsSystemMgmtIpSubnetMask, IDstsSystemMgmtIpSubnetMask), NULL,   0, 0},
  {OIDvalue(LNstsSystemAlarmAcoMode, IDstsSystemAlarmAcoMode), NULL,   0, 0},
  {OIDvalue(LNstsSystemAlarmAcoCutOff, IDstsSystemAlarmAcoCutOff), NULL,   0, 0},
  {OIDvalue(LNstsSystemAlarmEscalatingTime, IDstsSystemAlarmEscalatingTime), NULL,   0, 0},
};

static TRAP_OBJ_t stsSystemResetNotification_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsSystemReset, IDstsSystemReset), NULL,   0, 0},
};

static TRAP_OBJ_t stsSystemAlarmStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsSystemIntegratedAlarm, IDstsSystemIntegratedAlarm), NULL,   0, 0},
  {OIDvalue(LNstsSystemAlarmAcoLed, IDstsSystemAlarmAcoLed), NULL,   0, 0},
  {OIDvalue(LNstsSystemMinorLED, IDstsSystemMinorLED), NULL,   0, 0},
  {OIDvalue(LNstsSystemMajorLED, IDstsSystemMajorLED), NULL,   0, 0},
  {OIDvalue(LNstsSystemCriticalLED, IDstsSystemCriticalLED), NULL,   0, 0},
};

static TRAP_OBJ_t stsSystemSWVersionChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsSystemCurrentSWVersion, IDstsSystemCurrentSWVersion), NULL,   0, 0},
};

static TRAP_OBJ_t stsClockInputChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsClockInputManualSelect, IDstsClockInputManualSelect), NULL,   0, 0},
  {OIDvalue(LNstsClockInputOperMode, IDstsClockInputOperMode), NULL,   0, 0},
  {OIDvalue(LNstsClockInputSetSlip, IDstsClockInputSetSlip), NULL,   0, 0},
  {OIDvalue(LNstsClockInputSlipAlarmSetting, IDstsClockInputSlipAlarmSetting), NULL,   0, 0},
  {OIDvalue(LNstsClockInputSSMSelect, IDstsClockInputSSMSelect), NULL,   0, 0},
  {OIDvalue(LNstsClockInputValue, IDstsClockInputValue), NULL,   0, 0},
};

static TRAP_OBJ_t stsClockInputSlipAlarmStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsClockInputSetSlip, IDstsClockInputSetSlip), NULL,   0, 0},
  {OIDvalue(LNstsClockInputSlipState, IDstsClockInputSlipState), NULL,   0, 0},
  {OIDvalue(LNstsClockInputSlipAlarmSeverity, IDstsClockInputSlipAlarmSeverity), NULL,   0, 0},
};

static TRAP_OBJ_t stsClockInputStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsClockInputStatusIndex, IDstsClockInputStatusIndex), NULL,   0, 0},
  {OIDvalue(LNstsClockInputState, IDstsClockInputState), NULL,   0, 0},
};

static TRAP_OBJ_t stsClockInputSSMConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsClockInputSSMIndex, IDstsClockInputSSMIndex), NULL,   0, 0},
  {OIDvalue(LNstsClockInputSSMInputSelect, IDstsClockInputSSMInputSelect), NULL,   0, 0},
  {OIDvalue(LNstsClockInputSSMValue, IDstsClockInputSSMValue), NULL,   0, 0},
};

static TRAP_OBJ_t stsTopConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsTopSourceIpOption, IDstsTopSourceIpOption), NULL,   0, 0},
  {OIDvalue(LNstsTopSourceIpAddress, IDstsTopSourceIpAddress), NULL,   0, 0},
  {OIDvalue(LNstsTopSourceIpSubnetMask, IDstsTopSourceIpSubnetMask), NULL,   0, 0},
  {OIDvalue(LNstsTopSourceIpGatewayAddress, IDstsTopSourceIpGatewayAddress), NULL,   0, 0},
  {OIDvalue(LNstsTopPacketRate, IDstsTopPacketRate), NULL,   0, 0},
  {OIDvalue(LNstsTopInputSelect, IDstsTopInputSelect), NULL,   0, 0},
};

static TRAP_OBJ_t stsTopDestConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsTopDestNumber, IDstsTopDestNumber), NULL,   0, 0},
  {OIDvalue(LNstsTopDestIp, IDstsTopDestIp), NULL,   0, 0},
  {OIDvalue(LNstsTopDestUdpPort, IDstsTopDestUdpPort), NULL,   0, 0},
  {OIDvalue(LNstsTopDestVlanId, IDstsTopDestVlanId), NULL,   0, 0},
};

static TRAP_OBJ_t stsPerfMonManualConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsPerfMonManualInputSelect, IDstsPerfMonManualInputSelect), NULL,   0, 0},
  {OIDvalue(LNstsPerfMonManualInputScanInterval, IDstsPerfMonManualInputScanInterval), NULL,   0, 0},
  {OIDvalue(LNstsPerfMonManualInputNumReport, IDstsPerfMonManualInputNumReport), NULL,   0, 0},
  {OIDvalue(LNstsPerfMonManualInputNumObservation, IDstsPerfMonManualInputNumObservation), NULL,   0, 0},
  {OIDvalue(LNstsPerfMonManualInputAbortTest, IDstsPerfMonManualInputAbortTest), NULL,   0, 0},
};

static TRAP_OBJ_t stsCommStrConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsReadOnlyCommStr, IDstsReadOnlyCommStr), NULL,   0, 0},
  {OIDvalue(LNstsReadWriteCommStr, IDstsReadWriteCommStr), NULL,   0, 0},
};

static TRAP_OBJ_t stsShelfAdded_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsCardShelf, IDstsCardShelf), NULL,   0, 0},
};

static TRAP_OBJ_t stsShelfRemoved_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsCardShelf, IDstsCardShelf), NULL,   0, 0},
};

static TRAP_OBJ_t stsCardAdded_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsCardShelf, IDstsCardShelf), NULL,   0, 0},
  {OIDvalue(LNstsCardIndex, IDstsCardIndex), NULL,   0, 0},
  {OIDvalue(LNstsCardType, IDstsCardType), NULL,   0, 0},
};

static TRAP_OBJ_t stsCardRemoved_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsCardShelf, IDstsCardShelf), NULL,   0, 0},
  {OIDvalue(LNstsCardIndex, IDstsCardIndex), NULL,   0, 0},
  {OIDvalue(LNstsCardType, IDstsCardType), NULL,   0, 0},
};

static TRAP_OBJ_t stsCardStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsCardShelf, IDstsCardShelf), NULL,   0, 0},
  {OIDvalue(LNstsCardIndex, IDstsCardIndex), NULL,   0, 0},
  {OIDvalue(LNstsCardType, IDstsCardType), NULL,   0, 0},
  {OIDvalue(LNstsCardOperStatus, IDstsCardOperStatus), NULL,   0, 0},
};

static TRAP_OBJ_t stsOscillatorStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsOscillatorIndex, IDstsOscillatorIndex), NULL,   0, 0},
  {OIDvalue(LNstsOscillatorModuleType, IDstsOscillatorModuleType), NULL,   0, 0},
  {OIDvalue(LNstsOscillatorStatus, IDstsOscillatorStatus), NULL,   0, 0},
};

static TRAP_OBJ_t stsGpsStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsGpsIndex, IDstsGpsIndex), NULL,   0, 0},
  {OIDvalue(LNstsGpsSPOS, IDstsGpsSPOS), NULL,   0, 0},
  {OIDvalue(LNstsGpsTRMO, IDstsGpsTRMO), NULL,   0, 0},
  {OIDvalue(LNstsGpsStatus, IDstsGpsStatus), NULL,   0, 0},
  {OIDvalue(LNstsGpsState, IDstsGpsState), NULL,   0, 0},
};

static TRAP_OBJ_t stsLinkConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsLinkIndex, IDstsLinkIndex), NULL,   0, 0},
  {OIDvalue(LNstsLinkType, IDstsLinkType), NULL,   0, 0},
  {OIDvalue(LNstsLinkEnable, IDstsLinkEnable), NULL,   0, 0},
  {OIDvalue(LNstsLinkLineCode, IDstsLinkLineCode), NULL,   0, 0},
  {OIDvalue(LNstsLinkFMT, IDstsLinkFMT), NULL,   0, 0},
  {OIDvalue(LNstsLinkTermination, IDstsLinkTermination), NULL,   0, 0},
  {OIDvalue(LNstsLinkMFCHK, IDstsLinkMFCHK), NULL,   0, 0},
  {OIDvalue(LNstsLinkId, IDstsLinkId), NULL,   0, 0},
};

static TRAP_OBJ_t stsLinkDown_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsLinkIndex, IDstsLinkIndex), NULL,   0, 0},
};

static TRAP_OBJ_t stsLinkUp_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsLinkIndex, IDstsLinkIndex), NULL,   0, 0},
};

static TRAP_OBJ_t stsAlarmThresholdConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsAlarmThresholdCurrentMon, IDstsAlarmThresholdCurrentMon), NULL,   0, 0},
  {OIDvalue(LNstsAlarmThresholdType, IDstsAlarmThresholdType), NULL,   0, 0},
  {OIDvalue(LNstsAlarmThresholdValue, IDstsAlarmThresholdValue), NULL,   0, 0},
  {OIDvalue(LNstsAlarmThresholdTimeInterval, IDstsAlarmThresholdTimeInterval), NULL,   0, 0},
};

static TRAP_OBJ_t stsLinkAlarmSettingConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsLinkAlarmSettingLinkType, IDstsLinkAlarmSettingLinkType), NULL,   0, 0},
  {OIDvalue(LNstsLinkAlarmSettingType, IDstsLinkAlarmSettingType), NULL,   0, 0},
  {OIDvalue(LNstsLinkAlarmSettingSevirity, IDstsLinkAlarmSettingSevirity), NULL,   0, 0},
};

static TRAP_OBJ_t stsLinkAlarmStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsLinkAlarmSettingLinkType, IDstsLinkAlarmSettingLinkType), NULL,   0, 0},
  {OIDvalue(LNstsLinkAlarmSettingType, IDstsLinkAlarmSettingType), NULL,   0, 0},
  {OIDvalue(LNstsLinkAlarmSevirity, IDstsLinkAlarmSevirity), NULL,   0, 0},
};

static TRAP_OBJ_t stsOscillatorAlarmSettingConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsOscillatorAlarmSettingClock, IDstsOscillatorAlarmSettingClock), NULL,   0, 0},
  {OIDvalue(LNstsOscillatorAlarmSettingType, IDstsOscillatorAlarmSettingType), NULL,   0, 0},
  {OIDvalue(LNstsOscillatorAlarmSettingSeverity, IDstsOscillatorAlarmSettingSeverity), NULL,   0, 0},
};

static TRAP_OBJ_t stsOscillatorAlarmStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsOscillatorAlarmSettingClock, IDstsOscillatorAlarmSettingClock), NULL,   0, 0},
  {OIDvalue(LNstsOscillatorAlarmSettingType, IDstsOscillatorAlarmSettingType), NULL,   0, 0},
  {OIDvalue(LNstsOscillatorAlarmSeverity, IDstsOscillatorAlarmSeverity), NULL,   0, 0},
};

static TRAP_OBJ_t stsOsPwrAlarmSettingConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstssPwrAlarmSettingShelf, IDstssPwrAlarmSettingShelf), NULL,   0, 0},
  {OIDvalue(LNstsOsPwrAlarmSettingOsPwr, IDstsOsPwrAlarmSettingOsPwr), NULL,   0, 0},
  {OIDvalue(LNstsOsPwrAlarmSettingSeverity, IDstsOsPwrAlarmSettingSeverity), NULL,   0, 0},
};

static TRAP_OBJ_t stsOsPwrAlarmStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstssPwrAlarmSettingShelf, IDstssPwrAlarmSettingShelf), NULL,   0, 0},
  {OIDvalue(LNstsOsPwrAlarmSettingOsPwr, IDstsOsPwrAlarmSettingOsPwr), NULL,   0, 0},
  {OIDvalue(LNstsOsPwrAlarmSeverity, IDstsOsPwrAlarmSeverity), NULL,   0, 0},
};

static TRAP_OBJ_t stsOutputAlarmSettingConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSettingShelf, IDstsOutputAlarmSettingShelf), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSettingSlot, IDstsOutputAlarmSettingSlot), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSettingPort, IDstsOutputAlarmSettingPort), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSettingType, IDstsOutputAlarmSettingType), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSettingSeverity, IDstsOutputAlarmSettingSeverity), NULL,   0, 0},
};

static TRAP_OBJ_t stsOutputAlarmStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSettingShelf, IDstsOutputAlarmSettingShelf), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSettingSlot, IDstsOutputAlarmSettingSlot), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSettingPort, IDstsOutputAlarmSettingPort), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSettingType, IDstsOutputAlarmSettingType), NULL,   0, 0},
  {OIDvalue(LNstsOutputAlarmSeverity, IDstsOutputAlarmSeverity), NULL,   0, 0},
};

static TRAP_OBJ_t stsOutputTiuSettingConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsOutputTiuSettingShelf, IDstsOutputTiuSettingShelf), NULL,   0, 0},
  {OIDvalue(LNstsOutputTiuSettingSlot, IDstsOutputTiuSettingSlot), NULL,   0, 0},
  {OIDvalue(LNstsOutputTiuSettingPort, IDstsOutputTiuSettingPort), NULL,   0, 0},
  {OIDvalue(LNstsOutputTiuSettingLineCoding, IDstsOutputTiuSettingLineCoding), NULL,   0, 0},
  {OIDvalue(LNstsOutputTiuSettingFMT, IDstsOutputTiuSettingFMT), NULL,   0, 0},
  {OIDvalue(LNstsOutputTiuSettingTermination, IDstsOutputTiuSettingTermination), NULL,   0, 0},
};

static TRAP_OBJ_t stsOutputPortSettingConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsOutputPortSettingShelf, IDstsOutputPortSettingShelf), NULL,   0, 0},
  {OIDvalue(LNstsOutputPortSettingSlot, IDstsOutputPortSettingSlot), NULL,   0, 0},
  {OIDvalue(LNstsOutputPortSettingPort, IDstsOutputPortSettingPort), NULL,   0, 0},
  {OIDvalue(LNstsOutputPortSettingType, IDstsOutputPortSettingType), NULL,   0, 0},
  {OIDvalue(LNstsOutputPortSettingName, IDstsOutputPortSettingName), NULL,   0, 0},
};

static TRAP_OBJ_t stsAlarmStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsAlarmSource, IDstsAlarmSource), NULL,   0, 0},
  {OIDvalue(LNstsAlarmSourceType, IDstsAlarmSourceType), NULL,   0, 0},
  {OIDvalue(LNstsAlarmSeverity, IDstsAlarmSeverity), NULL,   0, 0},
  {OIDvalue(LNstsAlarmSrvEff, IDstsAlarmSrvEff), NULL,   0, 0},
};

static TRAP_OBJ_t stsOutputCardConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsOutputShelf, IDstsOutputShelf), NULL,   0, 0},
  {OIDvalue(LNstsOutputSlot, IDstsOutputSlot), NULL,   0, 0},
  {OIDvalue(LNstsOutputRedState, IDstsOutputRedState), NULL,   0, 0},
  {OIDvalue(LNstsOutputCRCCHK, IDstsOutputCRCCHK), NULL,   0, 0},
  {OIDvalue(LNstsOutputCAS, IDstsOutputCAS), NULL,   0, 0},
  {OIDvalue(LNstsOutputSSM, IDstsOutputSSM), NULL,   0, 0},
  {OIDvalue(LNstsOutputSSMValue, IDstsOutputSSMValue), NULL,   0, 0},
  {OIDvalue(LNstsOutputTimingFormat, IDstsOutputTimingFormat), NULL,   0, 0},
};

static TRAP_OBJ_t stsEMMAlarmStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsEmmShelf, IDstsEmmShelf), NULL,   0, 0},
  {OIDvalue(LNstsEmmSensorType, IDstsEmmSensorType), NULL,   0, 0},
  {OIDvalue(LNstsEmmAlarmStatus, IDstsEmmAlarmStatus), NULL,   0, 0},
  {OIDvalue(LNstsEmmSensorReading, IDstsEmmSensorReading), NULL,   0, 0},
};

static TRAP_OBJ_t stsEMMAlarmThresholdChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsEmmShelf, IDstsEmmShelf), NULL,   0, 0},
  {OIDvalue(LNstsEmmSensorType, IDstsEmmSensorType), NULL,   0, 0},
  {OIDvalue(LNstsEmmMajorAlmHiThresh, IDstsEmmMajorAlmHiThresh), NULL,   0, 0},
  {OIDvalue(LNstsEmmMajorAlmLoThresh, IDstsEmmMajorAlmLoThresh), NULL,   0, 0},
  {OIDvalue(LNstsEmmMinorAlmHiThresh, IDstsEmmMinorAlmHiThresh), NULL,   0, 0},
  {OIDvalue(LNstsEmmMinorAlmLoThresh, IDstsEmmMinorAlmLoThresh), NULL,   0, 0},
};

static TRAP_OBJ_t stsAntStatusChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsAntIndex, IDstsAntIndex), NULL,   0, 0},
  {OIDvalue(LNstsAntStatus, IDstsAntStatus), NULL,   0, 0},
};

static TRAP_OBJ_t stsTrapRegMgrConfigChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsTrapRegMgrNumber, IDstsTrapRegMgrNumber), NULL,   0, 0},
  {OIDvalue(LNstsTrapRegMgrIp, IDstsTrapRegMgrIp), NULL,   0, 0},
  {OIDvalue(LNstsTrapRegMgrUdpPort, IDstsTrapRegMgrUdpPort), NULL,   0, 0},
  {OIDvalue(LNstsTrapRegCommunityStr, IDstsTrapRegCommunityStr), NULL,   0, 0},
  {OIDvalue(LNstsTrapRegState, IDstsTrapRegState), NULL,   0, 0},
};

static TRAP_OBJ_t stsClockPriChange_TRAPOID[] = {
  {OIDvalue(LNstsSystemId, IDstsSystemId), NULL,   0, 0},
  {OIDvalue(LNstsClockPriIndex, IDstsClockPriIndex), NULL,   0, 0},
  {OIDvalue(LNstsClockPriSetting, IDstsClockPriSetting), NULL,   0, 0},
};

TRAP_OIDTBL_t trapOidTbl[] = {
  {    1001,          stsSystemIpConfigChange_TRAPOID,  9,      "stsSystemIpConfigChange"},
  {    1002,            stsSystemConfigChange_TRAPOID,  8,        "stsSystemConfigChange"},
  {    1003,       stsSystemResetNotification_TRAPOID,  2,   "stsSystemResetNotification"},
  {    1004,       stsSystemAlarmStatusChange_TRAPOID,  6,   "stsSystemAlarmStatusChange"},
  {    1005,         stsSystemSWVersionChange_TRAPOID,  2,     "stsSystemSWVersionChange"},
  {    1101,              stsClockInputChange_TRAPOID,  7,          "stsClockInputChange"},
  {    1102, stsClockInputSlipAlarmStatusChange_TRAPOID,  4, "stsClockInputSlipAlarmStatusChange"},
  {    1103,        stsClockInputStatusChange_TRAPOID,  3,    "stsClockInputStatusChange"},
  {    1104,     stsClockInputSSMConfigChange_TRAPOID,  4, "stsClockInputSSMConfigChange"},
  {    1105,               stsTopConfigChange_TRAPOID,  7,           "stsTopConfigChange"},
  {    1106,           stsTopDestConfigChange_TRAPOID,  5,       "stsTopDestConfigChange"},
  {    1107,     stsPerfMonManualConfigChange_TRAPOID,  6, "stsPerfMonManualConfigChange"},
  {    1108,           stsCommStrConfigChange_TRAPOID,  3,       "stsCommStrConfigChange"},
  {    1201,                    stsShelfAdded_TRAPOID,  2,                "stsShelfAdded"},
  {    1202,                  stsShelfRemoved_TRAPOID,  2,              "stsShelfRemoved"},
  {    1203,                     stsCardAdded_TRAPOID,  4,                 "stsCardAdded"},
  {    1204,                   stsCardRemoved_TRAPOID,  4,               "stsCardRemoved"},
  {    1205,              stsCardStatusChange_TRAPOID,  5,          "stsCardStatusChange"},
  {    1301,        stsOscillatorStatusChange_TRAPOID,  4,    "stsOscillatorStatusChange"},
  {    1401,               stsGpsStatusChange_TRAPOID,  6,           "stsGpsStatusChange"},
  {    1501,              stsLinkConfigChange_TRAPOID,  9,          "stsLinkConfigChange"},
  {    1502,                      stsLinkDown_TRAPOID,  2,                  "stsLinkDown"},
  {    1503,                        stsLinkUp_TRAPOID,  2,                    "stsLinkUp"},
  {    1601,    stsAlarmThresholdConfigChange_TRAPOID,  5, "stsAlarmThresholdConfigChange"},
  {    1701,  stsLinkAlarmSettingConfigChange_TRAPOID,  4, "stsLinkAlarmSettingConfigChange"},
  {    1702,         stsLinkAlarmStatusChange_TRAPOID,  4,     "stsLinkAlarmStatusChange"},
  {    1801, stsOscillatorAlarmSettingConfigChange_TRAPOID,  4, "stsOscillatorAlarmSettingConfigChange"},
  {    1802,   stsOscillatorAlarmStatusChange_TRAPOID,  4, "stsOscillatorAlarmStatusChange"},
  {    1901, stsOsPwrAlarmSettingConfigChange_TRAPOID,  4, "stsOsPwrAlarmSettingConfigChange"},
  {    1902,        stsOsPwrAlarmStatusChange_TRAPOID,  4,    "stsOsPwrAlarmStatusChange"},
  {    2001, stsOutputAlarmSettingConfigChange_TRAPOID,  6, "stsOutputAlarmSettingConfigChange"},
  {    2002,       stsOutputAlarmStatusChange_TRAPOID,  6,   "stsOutputAlarmStatusChange"},
  {    2003,  stsOutputTiuSettingConfigChange_TRAPOID,  7, "stsOutputTiuSettingConfigChange"},
  {    2004, stsOutputPortSettingConfigChange_TRAPOID,  6, "stsOutputPortSettingConfigChange"},
  {    2101,             stsAlarmStatusChange_TRAPOID,  5,         "stsAlarmStatusChange"},
  {    2201,        stsOutputCardConfigChange_TRAPOID,  9,    "stsOutputCardConfigChange"},
  {    2301,          stsEMMAlarmStatusChange_TRAPOID,  5,      "stsEMMAlarmStatusChange"},
  {    2302,       stsEMMAlarmThresholdChange_TRAPOID,  7,   "stsEMMAlarmThresholdChange"},
  {    2401,               stsAntStatusChange_TRAPOID,  3,           "stsAntStatusChange"},
  {    2501,        stsTrapRegMgrConfigChange_TRAPOID,  6,    "stsTrapRegMgrConfigChange"},
  {    2502,                stsClockPriChange_TRAPOID,  3,            "stsClockPriChange"},
  {       0,                             NULL_TRAPOID,  0,                           NULL}
};

#endif
