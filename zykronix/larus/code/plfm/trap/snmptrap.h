#ifndef _SNMPTRAP_H_
#define _SNMPTRAP_H_

#include "sr_proto.h"
#include "userpart.h"

typedef
struct _stsSystemIpConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                      stsSystemMgmtIpOption;
           SR_UINT32                     stsSystemMgmtIpAddress;
           SR_UINT32                  stsSystemMgmtIpSubnetMask;
           SR_UINT32              stsSystemMgmtIpGatewayAddress;
            SR_INT32                       stsSystemNtpIpOption;
           SR_UINT32                      stsSystemNtpIpAddress;
           SR_UINT32                   stsSystemNtpIpSubnetMask;
           SR_UINT32               stsSystemNtpIpGatewayAddress;
} stsSystemIpConfigChange_t;

typedef
struct _stsSystemConfigChange_t   {
         OctetString                                stsSystemId;
         OctetString                              stsSystemDate;
         OctetString                              stsSystemTime;
            SR_INT32                          stsSystemTimeZone;
           SR_UINT32                  stsSystemMgmtIpSubnetMask;
            SR_INT32                      stsSystemAlarmAcoMode;
            SR_INT32                    stsSystemAlarmAcoCutOff;
            SR_INT32               stsSystemAlarmEscalatingTime;
} stsSystemConfigChange_t;

typedef
struct _stsSystemResetNotification_t   {
         OctetString                                stsSystemId;
            SR_INT32                             stsSystemReset;
} stsSystemResetNotification_t;

typedef
struct _stsSystemAlarmStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                   stsSystemIntegratedAlarm;
            SR_INT32                       stsSystemAlarmAcoLed;
            SR_INT32                          stsSystemMinorLED;
            SR_INT32                          stsSystemMajorLED;
            SR_INT32                       stsSystemCriticalLED;
} stsSystemAlarmStatusChange_t;

typedef
struct _stsSystemSWVersionChange_t   {
         OctetString                                stsSystemId;
         OctetString                  stsSystemCurrentSWVersion;
} stsSystemSWVersionChange_t;

typedef
struct _stsClockInputChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                  stsClockInputManualSelect;
            SR_INT32                      stsClockInputOperMode;
            SR_INT32                       stsClockInputSetSlip;
            SR_INT32              stsClockInputSlipAlarmSetting;
            SR_INT32                     stsClockInputSSMSelect;
            SR_INT32                         stsClockInputValue;
} stsClockInputChange_t;

typedef
struct _stsClockInputSlipAlarmStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                       stsClockInputSetSlip;
            SR_INT32                     stsClockInputSlipState;
            SR_INT32             stsClockInputSlipAlarmSeverity;
} stsClockInputSlipAlarmStatusChange_t;

typedef
struct _stsClockInputStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                   stsClockInputStatusIndex;
            SR_INT32                         stsClockInputState;
} stsClockInputStatusChange_t;

typedef
struct _stsClockInputSSMConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                      stsClockInputSSMIndex;
            SR_INT32                stsClockInputSSMInputSelect;
            SR_INT32                      stsClockInputSSMValue;
} stsClockInputSSMConfigChange_t;

typedef
struct _stsTopConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                       stsTopSourceIpOption;
           SR_UINT32                      stsTopSourceIpAddress;
           SR_UINT32                   stsTopSourceIpSubnetMask;
           SR_UINT32               stsTopSourceIpGatewayAddress;
            SR_INT32                           stsTopPacketRate;
            SR_INT32                          stsTopInputSelect;
} stsTopConfigChange_t;

typedef
struct _stsTopDestConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                           stsTopDestNumber;
           SR_UINT32                               stsTopDestIp;
            SR_INT32                          stsTopDestUdpPort;
            SR_INT32                           stsTopDestVlanId;
} stsTopDestConfigChange_t;

typedef
struct _stsPerfMonManualConfigChange_t   {
         OctetString                                stsSystemId;
         OctetString                stsPerfMonManualInputSelect;
            SR_INT32          stsPerfMonManualInputScanInterval;
            SR_INT32             stsPerfMonManualInputNumReport;
            SR_INT32        stsPerfMonManualInputNumObservation;
            SR_INT32             stsPerfMonManualInputAbortTest;
} stsPerfMonManualConfigChange_t;

typedef
struct _stsCommStrConfigChange_t   {
         OctetString                                stsSystemId;
         OctetString                         stsReadOnlyCommStr;
         OctetString                        stsReadWriteCommStr;
} stsCommStrConfigChange_t;

typedef
struct _stsShelfAdded_t   {
         OctetString                                stsSystemId;
            SR_INT32                               stsCardShelf;
} stsShelfAdded_t;

typedef
struct _stsShelfRemoved_t   {
         OctetString                                stsSystemId;
            SR_INT32                               stsCardShelf;
} stsShelfRemoved_t;

typedef
struct _stsCardAdded_t   {
         OctetString                                stsSystemId;
            SR_INT32                               stsCardShelf;
            SR_INT32                               stsCardIndex;
            SR_INT32                                stsCardType;
} stsCardAdded_t;

typedef
struct _stsCardRemoved_t   {
         OctetString                                stsSystemId;
            SR_INT32                               stsCardShelf;
            SR_INT32                               stsCardIndex;
            SR_INT32                                stsCardType;
} stsCardRemoved_t;

typedef
struct _stsCardStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                               stsCardShelf;
            SR_INT32                               stsCardIndex;
            SR_INT32                                stsCardType;
            SR_INT32                          stsCardOperStatus;
} stsCardStatusChange_t;

typedef
struct _stsOscillatorStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                         stsOscillatorIndex;
            SR_INT32                    stsOscillatorModuleType;
            SR_INT32                        stsOscillatorStatus;
} stsOscillatorStatusChange_t;

typedef
struct _stsGpsStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                                stsGpsIndex;
         OctetString                                 stsGpsSPOS;
         OctetString                                 stsGpsTRMO;
            SR_INT32                               stsGpsStatus;
            SR_INT32                                stsGpsState;
} stsGpsStatusChange_t;

typedef
struct _stsLinkConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                               stsLinkIndex;
            SR_INT32                                stsLinkType;
            SR_INT32                              stsLinkEnable;
            SR_INT32                            stsLinkLineCode;
            SR_INT32                                 stsLinkFMT;
            SR_INT32                         stsLinkTermination;
            SR_INT32                               stsLinkMFCHK;
         OctetString                                  stsLinkId;
} stsLinkConfigChange_t;

typedef
struct _stsLinkDown_t   {
         OctetString                                stsSystemId;
            SR_INT32                               stsLinkIndex;
} stsLinkDown_t;

typedef
struct _stsLinkUp_t   {
         OctetString                                stsSystemId;
            SR_INT32                               stsLinkIndex;
} stsLinkUp_t;

typedef
struct _stsAlarmThresholdConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                stsAlarmThresholdCurrentMon;
            SR_INT32                      stsAlarmThresholdType;
            SR_INT32                     stsAlarmThresholdValue;
            SR_INT32              stsAlarmThresholdTimeInterval;
} stsAlarmThresholdConfigChange_t;

typedef
struct _stsLinkAlarmSettingConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                stsLinkAlarmSettingLinkType;
            SR_INT32                    stsLinkAlarmSettingType;
            SR_INT32                stsLinkAlarmSettingSevirity;
} stsLinkAlarmSettingConfigChange_t;

typedef
struct _stsLinkAlarmStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                stsLinkAlarmSettingLinkType;
            SR_INT32                    stsLinkAlarmSettingType;
            SR_INT32                       stsLinkAlarmSevirity;
} stsLinkAlarmStatusChange_t;

typedef
struct _stsOscillatorAlarmSettingConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32             stsOscillatorAlarmSettingClock;
            SR_INT32              stsOscillatorAlarmSettingType;
            SR_INT32          stsOscillatorAlarmSettingSeverity;
} stsOscillatorAlarmSettingConfigChange_t;

typedef
struct _stsOscillatorAlarmStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32             stsOscillatorAlarmSettingClock;
            SR_INT32              stsOscillatorAlarmSettingType;
            SR_INT32                 stsOscillatorAlarmSeverity;
} stsOscillatorAlarmStatusChange_t;

typedef
struct _stsOsPwrAlarmSettingConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                   stssPwrAlarmSettingShelf;
            SR_INT32                  stsOsPwrAlarmSettingOsPwr;
            SR_INT32               stsOsPwrAlarmSettingSeverity;
} stsOsPwrAlarmSettingConfigChange_t;

typedef
struct _stsOsPwrAlarmStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                   stssPwrAlarmSettingShelf;
            SR_INT32                  stsOsPwrAlarmSettingOsPwr;
            SR_INT32                      stsOsPwrAlarmSeverity;
} stsOsPwrAlarmStatusChange_t;

typedef
struct _stsOutputAlarmSettingConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                 stsOutputAlarmSettingShelf;
            SR_INT32                  stsOutputAlarmSettingSlot;
            SR_INT32                  stsOutputAlarmSettingPort;
            SR_INT32                  stsOutputAlarmSettingType;
            SR_INT32              stsOutputAlarmSettingSeverity;
} stsOutputAlarmSettingConfigChange_t;

typedef
struct _stsOutputAlarmStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                 stsOutputAlarmSettingShelf;
            SR_INT32                  stsOutputAlarmSettingSlot;
            SR_INT32                  stsOutputAlarmSettingPort;
            SR_INT32                  stsOutputAlarmSettingType;
            SR_INT32                     stsOutputAlarmSeverity;
} stsOutputAlarmStatusChange_t;

typedef
struct _stsOutputTiuSettingConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                   stsOutputTiuSettingShelf;
            SR_INT32                    stsOutputTiuSettingSlot;
            SR_INT32                    stsOutputTiuSettingPort;
            SR_INT32              stsOutputTiuSettingLineCoding;
            SR_INT32                     stsOutputTiuSettingFMT;
            SR_INT32             stsOutputTiuSettingTermination;
} stsOutputTiuSettingConfigChange_t;

typedef
struct _stsOutputPortSettingConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                  stsOutputPortSettingShelf;
            SR_INT32                   stsOutputPortSettingSlot;
            SR_INT32                   stsOutputPortSettingPort;
            SR_INT32                   stsOutputPortSettingType;
         OctetString                   stsOutputPortSettingName;
} stsOutputPortSettingConfigChange_t;

typedef
struct _stsAlarmStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                             stsAlarmSource;
            SR_INT32                         stsAlarmSourceType;
            SR_INT32                           stsAlarmSeverity;
            SR_INT32                             stsAlarmSrvEff;
} stsAlarmStatusChange_t;

typedef
struct _stsOutputCardConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                             stsOutputShelf;
            SR_INT32                              stsOutputSlot;
            SR_INT32                          stsOutputRedState;
            SR_INT32                            stsOutputCRCCHK;
            SR_INT32                               stsOutputCAS;
            SR_INT32                               stsOutputSSM;
            SR_INT32                          stsOutputSSMValue;
         OctetString                      stsOutputTimingFormat;
} stsOutputCardConfigChange_t;

typedef
struct _stsEMMAlarmStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                                stsEmmShelf;
            SR_INT32                           stsEmmSensorType;
            SR_INT32                          stsEmmAlarmStatus;
            SR_INT32                        stsEmmSensorReading;
} stsEMMAlarmStatusChange_t;

typedef
struct _stsEMMAlarmThresholdChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                                stsEmmShelf;
            SR_INT32                           stsEmmSensorType;
            SR_INT32                     stsEmmMajorAlmHiThresh;
            SR_INT32                     stsEmmMajorAlmLoThresh;
            SR_INT32                     stsEmmMinorAlmHiThresh;
            SR_INT32                     stsEmmMinorAlmLoThresh;
} stsEMMAlarmThresholdChange_t;

typedef
struct _stsAntStatusChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                                stsAntIndex;
            SR_INT32                               stsAntStatus;
} stsAntStatusChange_t;

typedef
struct _stsTrapRegMgrConfigChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                        stsTrapRegMgrNumber;
           SR_UINT32                            stsTrapRegMgrIp;
            SR_INT32                       stsTrapRegMgrUdpPort;
         OctetString                     stsTrapRegCommunityStr;
            SR_INT32                            stsTrapRegState;
} stsTrapRegMgrConfigChange_t;

typedef
struct _stsClockPriChange_t   {
         OctetString                                stsSystemId;
            SR_INT32                           stsClockPriIndex;
            SR_INT32                         stsClockPriSetting;
} stsClockPriChange_t;


#endif
