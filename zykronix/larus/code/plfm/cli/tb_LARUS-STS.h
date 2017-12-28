/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSSYSTEMSCALAR_H_
#define _TB_STSSYSTEMSCALAR_H_

static CLIMIBENTITY_t stsSystemIdENT = {
    OIDvalue(LNstsSystemId, IDstsSystemId)
    NULL,
    "sysid",
    ARG_STR,
    0,
#ifdef stsSystemIdSYNTAX
    (void *)stsSystemIdSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemId",
    " The system ID is assigned by the administrator for this node.    DEFVAL {\"NodeName\"}"};

#ifndef stsSystemMgmtIpOptionSYNTAX
CLIUMAPSTRUC_t stsSystemMgmtIpOptionMAP[] = {
{"static", 1}, {"dhcp", 2}, MAPDATA_END};

#define stsSystemMgmtIpOptionSYNTAX  stsSystemMgmtIpOptionMAP
#endif

static CLIMIBENTITY_t stsSystemMgmtIpOptionENT = {
    OIDvalue(LNstsSystemMgmtIpOption, IDstsSystemMgmtIpOption)
    NULL,
    "sysmgipopt",
    ARG_ENUM,
    2,
#ifdef stsSystemMgmtIpOptionSYNTAX
    (void *)stsSystemMgmtIpOptionSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemMgmtIpOption",
    "Current option for proivioning of system management ip address.    DEFVAL {1}"};

static CLIMIBENTITY_t stsSystemMgmtIpAddressENT = {
    OIDvalue(LNstsSystemMgmtIpAddress, IDstsSystemMgmtIpAddress)
    NULL,
    "sysmgip",
    ARG_STR,
    0,
#ifdef stsSystemMgmtIpAddressSYNTAX
    (void *)stsSystemMgmtIpAddressSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemMgmtIpAddress",
    " The setting of the static management IP address for this node. If stsSystemMgmtIpOption is dhcp, then this IP will be discovered."};

static CLIMIBENTITY_t stsSystemMgmtIpSubnetMaskENT = {
    OIDvalue(LNstsSystemMgmtIpSubnetMask, IDstsSystemMgmtIpSubnetMask)
    NULL,
    "sysmgsubm",
    ARG_STR,
    0,
#ifdef stsSystemMgmtIpSubnetMaskSYNTAX
    (void *)stsSystemMgmtIpSubnetMaskSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemMgmtIpSubnetMask",
    " The setting of the static subnet mask for the management port. If stsSystemMgmtIpOption is dhcp, then this Subnet mask will be discovered."};

static CLIMIBENTITY_t stsSystemMgmtIpGatewayAddressENT = {
    OIDvalue(LNstsSystemMgmtIpGatewayAddress, IDstsSystemMgmtIpGatewayAddress)
    NULL,
    "sysmggwip",
    ARG_STR,
    0,
#ifdef stsSystemMgmtIpGatewayAddressSYNTAX
    (void *)stsSystemMgmtIpGatewayAddressSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemMgmtIpGatewayAddress",
    " The setting of the gateway address for the management port. If stsSystemMgmtIpOption is dhcp, then this gateway IP will be discovered."};

#ifndef stsSystemNtpIpOptionSYNTAX
CLIUMAPSTRUC_t stsSystemNtpIpOptionMAP[] = {
{"static", 1}, {"dhcp", 2}, MAPDATA_END};

#define stsSystemNtpIpOptionSYNTAX  stsSystemNtpIpOptionMAP
#endif

static CLIMIBENTITY_t stsSystemNtpIpOptionENT = {
    OIDvalue(LNstsSystemNtpIpOption, IDstsSystemNtpIpOption)
    NULL,
    "ntpipopt",
    ARG_ENUM,
    2,
#ifdef stsSystemNtpIpOptionSYNTAX
    (void *)stsSystemNtpIpOptionSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemNtpIpOption",
    "Current option for proivioning of system SNTP ip address.    DEFVAL {1}"};

static CLIMIBENTITY_t stsSystemNtpIpAddressENT = {
    OIDvalue(LNstsSystemNtpIpAddress, IDstsSystemNtpIpAddress)
    NULL,
    "ntpip",
    ARG_STR,
    0,
#ifdef stsSystemNtpIpAddressSYNTAX
    (void *)stsSystemNtpIpAddressSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemNtpIpAddress",
    " The setting of the static TOP IP address for this node. If stsSystemSntpIpOption is dhcp, then this IP will be discovered."};

static CLIMIBENTITY_t stsSystemNtpIpSubnetMaskENT = {
    OIDvalue(LNstsSystemNtpIpSubnetMask, IDstsSystemNtpIpSubnetMask)
    NULL,
    "ntpipsubm",
    ARG_STR,
    0,
#ifdef stsSystemNtpIpSubnetMaskSYNTAX
    (void *)stsSystemNtpIpSubnetMaskSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemNtpIpSubnetMask",
    " The setting of the static subnet mask for the TOP port. If stsSystemSntpIpOption is dhcp, then this Subnet mask will be discovered."};

static CLIMIBENTITY_t stsSystemNtpIpGatewayAddressENT = {
    OIDvalue(LNstsSystemNtpIpGatewayAddress, IDstsSystemNtpIpGatewayAddress)
    NULL,
    "ntpgwip",
    ARG_STR,
    0,
#ifdef stsSystemNtpIpGatewayAddressSYNTAX
    (void *)stsSystemNtpIpGatewayAddressSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemNtpIpGatewayAddress",
    " The setting of the gateway address for the TOP port. If stsSystemSntpIpOption is dhcp, then this gateway IP will be discovered."};

static CLIMIBENTITY_t stsSystemDateENT = {
    OIDvalue(LNstsSystemDate, IDstsSystemDate)
    NULL,
    "sysdate",
    ARG_STR,
    0,
#ifdef stsSystemDateSYNTAX
    (void *)stsSystemDateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemDate",
    "The current local date and time for the system in the format DAT, DAT Date in format YYMMDD"};

static CLIMIBENTITY_t stsSystemTimeENT = {
    OIDvalue(LNstsSystemTime, IDstsSystemTime)
    NULL,
    "systime",
    ARG_STR,
    0,
#ifdef stsSystemTimeSYNTAX
    (void *)stsSystemTimeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemTime",
    "The current local date and time for the system in the format, TM Time in format HHMMSS"};

#ifndef stsSystemTimeZoneSYNTAX
static U8_t stsSystemTimeZoneRANGE[] = "-48..52";

#define stsSystemTimeZoneSYNTAX  stsSystemTimeZoneRANGE
#endif

static CLIMIBENTITY_t stsSystemTimeZoneENT = {
    OIDvalue(LNstsSystemTimeZone, IDstsSystemTimeZone)
    NULL,
    "systmzone",
    ARG_ENUM,
    0,
#ifdef stsSystemTimeZoneSYNTAX
    (void *)stsSystemTimeZoneSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemTimeZone",
    " The Time Zone where this node is installed."};

#ifndef stsSystemIntegratedAlarmSYNTAX
CLIUMAPSTRUC_t stsSystemIntegratedAlarmMAP[] = {
{"clear", 1}, {"minor", 2}, {"major", 3}, {"critical", 4}, MAPDATA_END};

#define stsSystemIntegratedAlarmSYNTAX  stsSystemIntegratedAlarmMAP
#endif

static CLIMIBENTITY_t stsSystemIntegratedAlarmENT = {
    OIDvalue(LNstsSystemIntegratedAlarm, IDstsSystemIntegratedAlarm)
    NULL,
    "sysalm",
    ARG_ENUM,
    4,
#ifdef stsSystemIntegratedAlarmSYNTAX
    (void *)stsSystemIntegratedAlarmSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemIntegratedAlarm",
    "Current alarm severity of this system."};

#ifndef stsSystemAlarmAcoModeSYNTAX
CLIUMAPSTRUC_t stsSystemAlarmAcoModeMAP[] = {
{"manual", 1}, {"auto", 2}, MAPDATA_END};

#define stsSystemAlarmAcoModeSYNTAX  stsSystemAlarmAcoModeMAP
#endif

static CLIMIBENTITY_t stsSystemAlarmAcoModeENT = {
    OIDvalue(LNstsSystemAlarmAcoMode, IDstsSystemAlarmAcoMode)
    NULL,
    "sysalmacomd",
    ARG_ENUM,
    2,
#ifdef stsSystemAlarmAcoModeSYNTAX
    (void *)stsSystemAlarmAcoModeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemAlarmAcoMode",
    "This object to set the alarm cutoff mode
for an equipment unit or facility in the system. The mode describes the
source of the command and the degree to which the system is permitted
to provide an automatic ACO function.
Valid values are:
manual - Alarm cutoff introduced by OS command or by manual operations at the IMC.
auto -  Alarm cutoff introduced automatically at the IMC but delayed for about 5 seconds to allow a brief audible announcement of the trouble."};

#ifndef stsSystemAlarmAcoLedSYNTAX
CLIUMAPSTRUC_t stsSystemAlarmAcoLedMAP[] = {
{"on", 1}, {"off", 2}, MAPDATA_END};

#define stsSystemAlarmAcoLedSYNTAX  stsSystemAlarmAcoLedMAP
#endif

static CLIMIBENTITY_t stsSystemAlarmAcoLedENT = {
    OIDvalue(LNstsSystemAlarmAcoLed, IDstsSystemAlarmAcoLed)
    NULL,
    "sysalmacoled",
    ARG_ENUM,
    2,
#ifdef stsSystemAlarmAcoLedSYNTAX
    (void *)stsSystemAlarmAcoLedSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemAlarmAcoLed",
    "This object indicates the current status of alarm audio.
              Alarm audio is turning on once there is an alarm in this system. 
              Alarm audio can be turn off manually by system administrator by pushing 
              acknowledged button located in the system module."};

#ifndef stsSystemAlarmAcoCutOffSYNTAX
CLIUMAPSTRUC_t stsSystemAlarmAcoCutOffMAP[] = {
{"no-change", 1}, {"cutoff", 2}, MAPDATA_END};

#define stsSystemAlarmAcoCutOffSYNTAX  stsSystemAlarmAcoCutOffMAP
#endif

static CLIMIBENTITY_t stsSystemAlarmAcoCutOffENT = {
    OIDvalue(LNstsSystemAlarmAcoCutOff, IDstsSystemAlarmAcoCutOff)
    NULL,
    "sysalmacocut",
    ARG_ENUM,
    2,
#ifdef stsSystemAlarmAcoCutOffSYNTAX
    (void *)stsSystemAlarmAcoCutOffSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemAlarmAcoCutOff",
    "This object indicates the current status of alarm audio.
              Alarm audio is turning off once there is an alarm in this system. 
              Alarm audio can be turn off manually by system administrator by pushing 
              acknowledged button located in the system module or setting the cutoff(2) value."};

#ifndef stsSystemAlarmEscalatingTimeSYNTAX
static U8_t stsSystemAlarmEscalatingTimeRANGE[] = "1..360";

#define stsSystemAlarmEscalatingTimeSYNTAX  stsSystemAlarmEscalatingTimeRANGE
#endif

static CLIMIBENTITY_t stsSystemAlarmEscalatingTimeENT = {
    OIDvalue(LNstsSystemAlarmEscalatingTime, IDstsSystemAlarmEscalatingTime)
    NULL,
    "sysalmestime",
    ARG_ENUM,
    0,
#ifdef stsSystemAlarmEscalatingTimeSYNTAX
    (void *)stsSystemAlarmEscalatingTimeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemAlarmEscalatingTime",
    "This object indicates the time in hours for the alarm escalation occurs. 
              System will raise one level of the alarm severity for system alarm."};

#ifndef stsSystemMinorLEDSYNTAX
CLIUMAPSTRUC_t stsSystemMinorLEDMAP[] = {
{"off", 1}, {"on", 2}, MAPDATA_END};

#define stsSystemMinorLEDSYNTAX  stsSystemMinorLEDMAP
#endif

static CLIMIBENTITY_t stsSystemMinorLEDENT = {
    OIDvalue(LNstsSystemMinorLED, IDstsSystemMinorLED)
    NULL,
    "sysminled",
    ARG_ENUM,
    2,
#ifdef stsSystemMinorLEDSYNTAX
    (void *)stsSystemMinorLEDSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemMinorLED",
    "Current Minor LED state on the system module of this system."};

#ifndef stsSystemMajorLEDSYNTAX
CLIUMAPSTRUC_t stsSystemMajorLEDMAP[] = {
{"off", 1}, {"on", 2}, MAPDATA_END};

#define stsSystemMajorLEDSYNTAX  stsSystemMajorLEDMAP
#endif

static CLIMIBENTITY_t stsSystemMajorLEDENT = {
    OIDvalue(LNstsSystemMajorLED, IDstsSystemMajorLED)
    NULL,
    "sysmajled",
    ARG_ENUM,
    2,
#ifdef stsSystemMajorLEDSYNTAX
    (void *)stsSystemMajorLEDSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemMajorLED",
    "Current Major LED state on the system module of this system."};

#ifndef stsSystemCriticalLEDSYNTAX
CLIUMAPSTRUC_t stsSystemCriticalLEDMAP[] = {
{"off", 1}, {"on", 2}, MAPDATA_END};

#define stsSystemCriticalLEDSYNTAX  stsSystemCriticalLEDMAP
#endif

static CLIMIBENTITY_t stsSystemCriticalLEDENT = {
    OIDvalue(LNstsSystemCriticalLED, IDstsSystemCriticalLED)
    NULL,
    "syscriled",
    ARG_ENUM,
    2,
#ifdef stsSystemCriticalLEDSYNTAX
    (void *)stsSystemCriticalLEDSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemCriticalLED",
    "Current Critical LED state on the system module of this system."};

#ifndef stsSystemUpgradeSWActionSYNTAX
CLIUMAPSTRUC_t stsSystemUpgradeSWActionMAP[] = {
{"no-action", 1}, {"upgrade", 2}, MAPDATA_END};

#define stsSystemUpgradeSWActionSYNTAX  stsSystemUpgradeSWActionMAP
#endif

static CLIMIBENTITY_t stsSystemUpgradeSWActionENT = {
    OIDvalue(LNstsSystemUpgradeSWAction, IDstsSystemUpgradeSWAction)
    NULL,
    "sysswact",
    ARG_ENUM,
    2,
#ifdef stsSystemUpgradeSWActionSYNTAX
    (void *)stsSystemUpgradeSWActionSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemUpgradeSWAction",
    "Upgrade SW image on this system."};

static CLIMIBENTITY_t stsSystemCurrentSWVersionENT = {
    OIDvalue(LNstsSystemCurrentSWVersion, IDstsSystemCurrentSWVersion)
    NULL,
    "sysswver",
    ARG_ENUM,
    0,
#ifdef stsSystemCurrentSWVersionSYNTAX
    (void *)stsSystemCurrentSWVersionSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemCurrentSWVersion",
    "The current software version running in the system."};

#ifndef stsSystemServiceStateSYNTAX
CLIUMAPSTRUC_t stsSystemServiceStateMAP[] = {
{"init", 1}, {"inService", 2}, {"outOfService", 3}, MAPDATA_END};

#define stsSystemServiceStateSYNTAX  stsSystemServiceStateMAP
#endif

static CLIMIBENTITY_t stsSystemServiceStateENT = {
    OIDvalue(LNstsSystemServiceState, IDstsSystemServiceState)
    NULL,
    "syssvcst",
    ARG_ENUM,
    3,
#ifdef stsSystemServiceStateSYNTAX
    (void *)stsSystemServiceStateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemServiceState",
    "To indicate the state of service."};

#ifndef stsSystemResetSYNTAX
CLIUMAPSTRUC_t stsSystemResetMAP[] = {
{"noAction", 1}, {"warmReset", 2}, {"coldReset", 3}, {"configReset", 4}, MAPDATA_END};

#define stsSystemResetSYNTAX  stsSystemResetMAP
#endif

static CLIMIBENTITY_t stsSystemResetENT = {
    OIDvalue(LNstsSystemReset, IDstsSystemReset)
    NULL,
    "sysreset",
    ARG_ENUM,
    4,
#ifdef stsSystemResetSYNTAX
    (void *)stsSystemResetSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsSystemReset",
    " warmReset - Restarts system. Clears history alarm and PM.
coldReset - Restarts system through power cycle. Clears history alarm and PM.
configReset - Restarts system. Initializes user setting to default
value. Clears history alarm and PM."};

static pCLIMIBENTITY_t stsSystemScalarLST[] = {
                  &stsSystemIdENT,         &stsSystemMgmtIpOptionENT,
       &stsSystemMgmtIpAddressENT,     &stsSystemMgmtIpSubnetMaskENT,
&stsSystemMgmtIpGatewayAddressENT,          &stsSystemNtpIpOptionENT,
        &stsSystemNtpIpAddressENT,      &stsSystemNtpIpSubnetMaskENT,
 &stsSystemNtpIpGatewayAddressENT,                 &stsSystemDateENT,
                &stsSystemTimeENT,             &stsSystemTimeZoneENT,
        &stsSystemAlarmAcoModeENT,
      &stsSystemAlarmAcoCutOffENT,
 &stsSystemAlarmEscalatingTimeENT,      &stsSystemUpgradeSWActionENT,                &stsSystemResetENT};

CLIMIBCFGGROUP_t stsSystemScalarTBL = {
    NULL,
    stsSystemScalarLST,
    0,
    17};

static MIBDSPENTITY_t rtrv_mgmtipDENT[] = {
    {&stsSystemMgmtIpOptionENT, " MGMTIP OPTION :%s\n", NULL, DSP_STRING},
    {&stsSystemMgmtIpAddressENT, " MGMTIP ADDRESS:%s\n", NULL, DSP_STRING},
    {&stsSystemMgmtIpSubnetMaskENT, " MGMTIP MASK   :%s\n", NULL, DSP_STRING},
    {&stsSystemMgmtIpGatewayAddressENT, " MGMTIP GATEWAY:%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_mgmtipDSP = {
  "rtrv_mgmtip",
  &stsSystemScalarTBL,
  4,
  rtrv_mgmtipDENT,
  ""};

static MIBDSPENTITY_t rtrv_ntpipDENT[] = {
    {&stsSystemNtpIpOptionENT, " NTPIP  OPTION :%s\n", NULL, DSP_STRING},
    {&stsSystemNtpIpAddressENT, " NTPIP  ADDRESS:%s\n", NULL, DSP_STRING},
    {&stsSystemNtpIpSubnetMaskENT, " NTPIP  MASK   :%s\n", NULL, DSP_STRING},
    {&stsSystemNtpIpGatewayAddressENT, " NTPIP  GATEWAY:%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_ntpipDSP = {
  "rtrv_ntpip",
  &stsSystemScalarTBL,
  4,
  rtrv_ntpipDENT,
  ""};

static MIBDSPENTITY_t rtrv_datDENT[] = {
    {&stsSystemTimeZoneENT, " UTC OFFSET=%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_datDSP = {
  "rtrv_dat",
  &stsSystemScalarTBL,
  1,
  rtrv_datDENT,
  ""};

static MIBDSPENTITY_t rtrv_sysintg_alarmDENT[] = {
    {&stsSystemIntegratedAlarmENT, " SYSTEM INTEGRATED ALARM:%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_sysintg_alarmDSP = {
  "rtrv_sysintg_alarm",
  &stsSystemScalarTBL,
  1,
  rtrv_sysintg_alarmDENT,
  ""};

static MIBDSPENTITY_t rtrv_aco_ledDENT[] = {
    {&stsSystemAlarmAcoLedENT, " ACOLED:%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_aco_ledDSP = {
  "rtrv_aco_led",
  &stsSystemScalarTBL,
  1,
  rtrv_aco_ledDENT,
  ""};

static MIBDSPENTITY_t rtrv_alarmescalating_timeDENT[] = {
    {&stsSystemAlarmEscalatingTimeENT, " ESCALATING TIME:", NULL, DSP_TABLE}};

CLIMIBDSPGROUP_t rtrv_alarmescalating_timeDSP = {
  "rtrv_alarmescalating_time",
  &stsSystemScalarTBL,
  1,
  rtrv_alarmescalating_timeDENT,
  ""};

static MIBDSPENTITY_t rtrv_system_ledDENT[] = {
    {&stsSystemCriticalLEDENT, " CRITICAL LED:%s\n", NULL, DSP_STRING},
    {&stsSystemMajorLEDENT, " MAJOR    LED:%s\n", NULL, DSP_STRING},
    {&stsSystemMinorLEDENT, " MINOR    LED:%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_system_ledDSP = {
  "rtrv_system_led",
  &stsSystemScalarTBL,
  3,
  rtrv_system_ledDENT,
  ""};

static MIBDSPENTITY_t rtrv_syssw_versionDENT[] = {
    {&stsSystemCurrentSWVersionENT, " SYSTEM SW VERSION:%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_syssw_versionDSP = {
  "rtrv_syssw_version",
  &stsSystemScalarTBL,
  1,
  rtrv_syssw_versionDENT,
  ""};

static MIBDSPENTITY_t rtrv_sysservice_statusDENT[] = {
    {&stsSystemServiceStateENT, " SYSTEM SERVICE STATUS:%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_sysservice_statusDSP = {
  "rtrv_sysservice_status",
  &stsSystemScalarTBL,
  1,
  rtrv_sysservice_statusDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSCLOCKINPUTSCALAR_H_
#define _TB_STSCLOCKINPUTSCALAR_H_

#ifndef stsClockInputManualSelectSYNTAX
CLIUMAPSTRUC_t stsClockInputManualSelectMAP[] = {
{"esr1", 1}, {"esr2", 2}, {"prs", 3}, {"link1", 4}, {"link2", 5}, MAPDATA_END};

#define stsClockInputManualSelectSYNTAX  stsClockInputManualSelectMAP
#endif

static CLIMIBENTITY_t stsClockInputManualSelectENT = {
    OIDvalue(LNstsClockInputManualSelect, IDstsClockInputManualSelect)
    NULL,
    "clkinmansel",
    ARG_ENUM,
    5,
#ifdef stsClockInputManualSelectSYNTAX
    (void *)stsClockInputManualSelectSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputManualSelect",
    "To provision the manual clock selected from external signal 1, 2, or prs or input 1 or 2 ."};

#ifndef stsClockInputOperModeSYNTAX
CLIUMAPSTRUC_t stsClockInputOperModeMAP[] = {
{"protected", 1}, {"manual", 2}, MAPDATA_END};

#define stsClockInputOperModeSYNTAX  stsClockInputOperModeMAP
#endif

static CLIMIBENTITY_t stsClockInputOperModeENT = {
    OIDvalue(LNstsClockInputOperMode, IDstsClockInputOperMode)
    NULL,
    "clkinopmod",
    ARG_ENUM,
    2,
#ifdef stsClockInputOperModeSYNTAX
    (void *)stsClockInputOperModeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputOperMode",
    "To provision the clock mode.
protected - once the original active clock recovered, system will make it active.
manual - system will not switch the clock even the original active clock recovered."};

#ifndef stsClockInputSetSlipSYNTAX
CLIUMAPSTRUC_t stsClockInputSetSlipMAP[] = {
{"refAb", 1}, {"frame", 2}, MAPDATA_END};

#define stsClockInputSetSlipSYNTAX  stsClockInputSetSlipMAP
#endif

static CLIMIBENTITY_t stsClockInputSetSlipENT = {
    OIDvalue(LNstsClockInputSetSlip, IDstsClockInputSetSlip)
    NULL,
    "clkinslip",
    ARG_ENUM,
    2,
#ifdef stsClockInputSetSlipSYNTAX
    (void *)stsClockInputSetSlipSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputSetSlip",
    "sets the slip rate between either the A and B
clock reference signals that will be shown on the IMC seven-segment
display or the active reference signal and one of the five input signals
monitored by the SMU. The latter is the default setting when the IMC is
initialized."};

#ifndef stsClockInputValueSYNTAX
CLIUMAPSTRUC_t stsClockInputValueMAP[] = {
{"prs", 1}, {"stu", 2}, {"st2", 3}, {"st3", 4}, {"sic", 5}, {"duc", 6}, MAPDATA_END};

#define stsClockInputValueSYNTAX  stsClockInputValueMAP
#endif

static CLIMIBENTITY_t stsClockInputValueENT = {
    OIDvalue(LNstsClockInputValue, IDstsClockInputValue)
    NULL,
    "clkinvalue",
    ARG_ENUM,
    6,
#ifdef stsClockInputValueSYNTAX
    (void *)stsClockInputValueSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputValue",
    "Oscillator type"};

static pCLIMIBENTITY_t stsClockInputScalarLST[] = {
    &stsClockInputManualSelectENT,         &stsClockInputOperModeENT,
         &stsClockInputSetSlipENT,                              NULL};

CLIMIBCFGGROUP_t stsClockInputScalarTBL = {
    NULL,
    stsClockInputScalarLST,
    0,
    3};

static MIBDSPENTITY_t rtrv_clockinput_modeDENT[] = {
    {&stsClockInputOperModeENT, "CLOCK INPUT MODE=%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_clockinput_modeDSP = {
  "rtrv_clockinput_mode",
  &stsClockInputScalarTBL,
  1,
  rtrv_clockinput_modeDENT,
  ""};

static MIBDSPENTITY_t rtrv_clockinput_manualselectDENT[] = {
    {&stsClockInputManualSelectENT, "CLOCK INPUT MANUAL SELECT=%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_clockinput_manualselectDSP = {
  "rtrv_clockinput_manualselect",
  &stsClockInputScalarTBL,
  1,
  rtrv_clockinput_manualselectDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSCLOCKINPUTSSMSCALAR_H_
#define _TB_STSCLOCKINPUTSSMSCALAR_H_

#ifndef stsClockInputSSMSelectSYNTAX
CLIUMAPSTRUC_t stsClockInputSSMSelectMAP[] = {
{"e1-sa4-ssm", 1}, {"e1-sa5-ssm", 2}, {"e1-sa6-ssm", 3}, {"e1-sa7-ssm", 4}, {"e1-sa8-ssm", 5}, {"t1-ssm", 6}, MAPDATA_END};

#define stsClockInputSSMSelectSYNTAX  stsClockInputSSMSelectMAP
#endif

static CLIMIBENTITY_t stsClockInputSSMSelectENT = {
    OIDvalue(LNstsClockInputSSMSelect, IDstsClockInputSSMSelect)
    NULL,
    "clkinssmsel",
    ARG_ENUM,
    6,
#ifdef stsClockInputSSMSelectSYNTAX
    (void *)stsClockInputSSMSelectSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputSSMSelect",
    "sets the SSM."};

static pCLIMIBENTITY_t stsClockInputSSMScalarLST[] = {
       &stsClockInputSSMSelectENT};

CLIMIBCFGGROUP_t stsClockInputSSMScalarTBL = {
    NULL,
    stsClockInputSSMScalarLST,
    0,
    1};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSTOPSCALAR_H_
#define _TB_STSTOPSCALAR_H_

#ifndef stsTopSourceIpOptionSYNTAX
CLIUMAPSTRUC_t stsTopSourceIpOptionMAP[] = {
{"static", 1}, {"dhcp", 2}, MAPDATA_END};

#define stsTopSourceIpOptionSYNTAX  stsTopSourceIpOptionMAP
#endif

static CLIMIBENTITY_t stsTopSourceIpOptionENT = {
    OIDvalue(LNstsTopSourceIpOption, IDstsTopSourceIpOption)
    NULL,
    "topipopt",
    ARG_ENUM,
    2,
#ifdef stsTopSourceIpOptionSYNTAX
    (void *)stsTopSourceIpOptionSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopSourceIpOption",
    "Current option for proivioning of system TOP ip address.    DEFVAL {1}"};

static CLIMIBENTITY_t stsTopSourceIpAddressENT = {
    OIDvalue(LNstsTopSourceIpAddress, IDstsTopSourceIpAddress)
    NULL,
    "topip",
    ARG_STR,
    0,
#ifdef stsTopSourceIpAddressSYNTAX
    (void *)stsTopSourceIpAddressSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopSourceIpAddress",
    " The setting of the static TOP IP address for this node. If stsTopSourceIpOption is dhcp, then this IP will be discovered."};

static CLIMIBENTITY_t stsTopSourceIpSubnetMaskENT = {
    OIDvalue(LNstsTopSourceIpSubnetMask, IDstsTopSourceIpSubnetMask)
    NULL,
    "topipsubm",
    ARG_STR,
    0,
#ifdef stsTopSourceIpSubnetMaskSYNTAX
    (void *)stsTopSourceIpSubnetMaskSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopSourceIpSubnetMask",
    " The setting of the static subnet mask for the TOP port. If stsTopSourceIpOption is dhcp, then this Subnet mask will be discovered."};

static CLIMIBENTITY_t stsTopSourceIpGatewayAddressENT = {
    OIDvalue(LNstsTopSourceIpGatewayAddress, IDstsTopSourceIpGatewayAddress)
    NULL,
    "topgwip",
    ARG_STR,
    0,
#ifdef stsTopSourceIpGatewayAddressSYNTAX
    (void *)stsTopSourceIpGatewayAddressSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopSourceIpGatewayAddress",
    " The setting of the gateway address for the TOP port. If stsTopSourceIpOption is dhcp, then this gateway IP will be discovered."};

#ifndef stsTopPacketRateSYNTAX
static U8_t stsTopPacketRateRANGE[] = "10..1000";

#define stsTopPacketRateSYNTAX  stsTopPacketRateRANGE
#endif

static CLIMIBENTITY_t stsTopPacketRateENT = {
    OIDvalue(LNstsTopPacketRate, IDstsTopPacketRate)
    NULL,
    "toppktrate",
    ARG_ENUM,
    0,
#ifdef stsTopPacketRateSYNTAX
    (void *)stsTopPacketRateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopPacketRate",
    " The setting of the packet rate per second of TOP."};

#ifndef stsTopInputSelectSYNTAX
CLIUMAPSTRUC_t stsTopInputSelectMAP[] = {
{"e1", 1}, {"t1", 2}, {"cc", 3}, MAPDATA_END};

#define stsTopInputSelectSYNTAX  stsTopInputSelectMAP
#endif

static CLIMIBENTITY_t stsTopInputSelectENT = {
    OIDvalue(LNstsTopInputSelect, IDstsTopInputSelect)
    NULL,
    "topinsel",
    ARG_ENUM,
    3,
#ifdef stsTopInputSelectSYNTAX
    (void *)stsTopInputSelectSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopInputSelect",
    " The setting of the input select of TOP."};

#ifndef stsTopTxPacketsSYNTAX
static U8_t stsTopTxPacketsRANGE[] = "0..1000000";

#define stsTopTxPacketsSYNTAX  stsTopTxPacketsRANGE
#endif

static CLIMIBENTITY_t stsTopTxPacketsENT = {
    OIDvalue(LNstsTopTxPackets, IDstsTopTxPackets)
    NULL,
    "toptxpkt",
    ARG_ENUM,
    0,
#ifdef stsTopTxPacketsSYNTAX
    (void *)stsTopTxPacketsSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopTxPackets",
    " The count of TOP transmitted packets."};

#ifndef stsTopTxBytesSYNTAX
static U8_t stsTopTxBytesRANGE[] = "0..1000000";

#define stsTopTxBytesSYNTAX  stsTopTxBytesRANGE
#endif

static CLIMIBENTITY_t stsTopTxBytesENT = {
    OIDvalue(LNstsTopTxBytes, IDstsTopTxBytes)
    NULL,
    "toptxbyte",
    ARG_ENUM,
    0,
#ifdef stsTopTxBytesSYNTAX
    (void *)stsTopTxBytesSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopTxBytes",
    " The count of TOP transmitted bytes."};

static pCLIMIBENTITY_t stsTopScalarLST[] = {
         &stsTopSourceIpOptionENT,         &stsTopSourceIpAddressENT,
     &stsTopSourceIpSubnetMaskENT,  &stsTopSourceIpGatewayAddressENT,
             &stsTopPacketRateENT,             &stsTopInputSelectENT,
                             NULL};

CLIMIBCFGGROUP_t stsTopScalarTBL = {
    NULL,
    stsTopScalarLST,
    0,
    6};

static MIBDSPENTITY_t rtrv_topipDENT[] = {
    {&stsTopSourceIpOptionENT, " TOPIP OPTION :%s\n", NULL, DSP_STRING},
    {&stsTopSourceIpAddressENT, " TOPIP ADDRESS:%s\n", NULL, DSP_STRING},
    {&stsTopSourceIpSubnetMaskENT, " TOPIP MASK   :%s\n", NULL, DSP_STRING},
    {&stsTopSourceIpGatewayAddressENT, " TOPIP GATEWAY:%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_topipDSP = {
  "rtrv_topip",
  &stsTopScalarTBL,
  4,
  rtrv_topipDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSPERFMONMANUALSCALAR_H_
#define _TB_STSPERFMONMANUALSCALAR_H_

#ifndef stsPerfMonManualInputSelectSYNTAX
static  char *stsPerfMonManualInputSelectBITMAPSTR[] = {
       "link1", "link2", "link3", "link4", "link5"};

};

static  BITMAP stsPerfMonManualInputSelectBITMAP = {
    NULL,
    NULL,
    NULL,
    stsPerfMonManualInputSelectBITMAPSTR};

#define stsPerfMonManualInputSelectSYNTAX  &stsPerfMonManualInputSelectBITMAP
#endif

static CLIMIBENTITY_t stsPerfMonManualInputSelectENT = {
    OIDvalue(LNstsPerfMonManualInputSelect, IDstsPerfMonManualInputSelect)
    NULL,
    "perfmansel",
    ARG_ENUM,
    4,
#ifdef stsPerfMonManualInputSelectSYNTAX
    (void *)stsPerfMonManualInputSelectSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonManualInputSelect",
    "Current monitor link number. To specify the link(s) for the monitoring. 
                 The default is '00'H. No selection."};

#ifndef stsPerfMonManualInputScanIntervalSYNTAX
CLIUMAPSTRUC_t stsPerfMonManualInputScanIntervalMAP[] = {
{"one-sec", 1}, {"ten-sec", 2}, {"hundred-sec", 3}, {"thousand-sec", 4}, {"tenThousand-sec", 5}, {"hundredThousand-sec", 6}, MAPDATA_END};

#define stsPerfMonManualInputScanIntervalSYNTAX  stsPerfMonManualInputScanIntervalMAP
#endif

static CLIMIBENTITY_t stsPerfMonManualInputScanIntervalENT = {
    OIDvalue(LNstsPerfMonManualInputScanInterval, IDstsPerfMonManualInputScanInterval)
    NULL,
    "perfmaninval",
    ARG_ENUM,
    6,
#ifdef stsPerfMonManualInputScanIntervalSYNTAX
    (void *)stsPerfMonManualInputScanIntervalSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonManualInputScanInterval",
    "Specify the scan interval."};

#ifndef stsPerfMonManualInputNumReportSYNTAX
static U8_t stsPerfMonManualInputNumReportRANGE[] = "1..100";

#define stsPerfMonManualInputNumReportSYNTAX  stsPerfMonManualInputNumReportRANGE
#endif

static CLIMIBENTITY_t stsPerfMonManualInputNumReportENT = {
    OIDvalue(LNstsPerfMonManualInputNumReport, IDstsPerfMonManualInputNumReport)
    NULL,
    "perfnumrept",
    ARG_ENUM,
    0,
#ifdef stsPerfMonManualInputNumReportSYNTAX
    (void *)stsPerfMonManualInputNumReportSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonManualInputNumReport",
    "Specify the number of reports"};

#ifndef stsPerfMonManualInputNumObservationSYNTAX
static U8_t stsPerfMonManualInputNumObservationRANGE[] = "1..100";

#define stsPerfMonManualInputNumObservationSYNTAX  stsPerfMonManualInputNumObservationRANGE
#endif

static CLIMIBENTITY_t stsPerfMonManualInputNumObservationENT = {
    OIDvalue(LNstsPerfMonManualInputNumObservation, IDstsPerfMonManualInputNumObservation)
    NULL,
    "perfnumobs",
    ARG_ENUM,
    0,
#ifdef stsPerfMonManualInputNumObservationSYNTAX
    (void *)stsPerfMonManualInputNumObservationSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonManualInputNumObservation",
    "Specify the number of observations"};

#ifndef stsPerfMonManualInputAbortTestSYNTAX
CLIUMAPSTRUC_t stsPerfMonManualInputAbortTestMAP[] = {
{"noAction", 1}, {"abort", 2}, MAPDATA_END};

#define stsPerfMonManualInputAbortTestSYNTAX  stsPerfMonManualInputAbortTestMAP
#endif

static CLIMIBENTITY_t stsPerfMonManualInputAbortTestENT = {
    OIDvalue(LNstsPerfMonManualInputAbortTest, IDstsPerfMonManualInputAbortTest)
    NULL,
    "perfmanabort",
    ARG_ENUM,
    2,
#ifdef stsPerfMonManualInputAbortTestSYNTAX
    (void *)stsPerfMonManualInputAbortTestSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonManualInputAbortTest",
    " This object allows users to abort the current performance monitoring manual test."};

static pCLIMIBENTITY_t stsPerfMonManualScalarLST[] = {
  &stsPerfMonManualInputSelectENT, &stsPerfMonManualInputScanIntervalENT,
&stsPerfMonManualInputNumReportENT, &stsPerfMonManualInputNumObservationENT,
&stsPerfMonManualInputAbortTestENT};

CLIMIBCFGGROUP_t stsPerfMonManualScalarTBL = {
    NULL,
    stsPerfMonManualScalarLST,
    0,
    5};

static MIBDSPENTITY_t rtrv_pmsched_linkDENT[] = {
    {&stsPerfMonManualInputSelectENT, " %s", NULL, DSP_STRING},
    {&stsPerfMonManualInputScanIntervalENT, ":%s", NULL, DSP_STRING},
    {&stsPerfMonManualInputNumReportENT, "%u", NULL, DSP_NUM},
    {&stsPerfMonManualInputNumObservationENT, "%u\n", NULL, DSP_NUM}};

CLIMIBDSPGROUP_t rtrv_pmsched_linkDSP = {
  "rtrv_pmsched_link",
  &stsPerfMonManualScalarTBL,
  4,
  rtrv_pmsched_linkDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSCLOCKINPUTSTATUSTABLE_H_
#define _TB_STSCLOCKINPUTSTATUSTABLE_H_

#ifndef stsClockInputStatusIndexSYNTAX
CLIUMAPSTRUC_t stsClockInputStatusIndexMAP[] = {
{"esr1", 1}, {"esr2", 2}, {"prs", 3}, {"link1", 4}, {"link2", 5}, MAPDATA_END};

#define stsClockInputStatusIndexSYNTAX  stsClockInputStatusIndexMAP
#endif

static CLIMIBENTITY_t stsClockInputStatusIndexENT = {
    OIDvalue(LNstsClockInputStatusIndex, IDstsClockInputStatusIndex)
    NULL,
    "clkinstidx",
    ARG_ENUM,
    5,
#ifdef stsClockInputStatusIndexSYNTAX
    (void *)stsClockInputStatusIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputStatusIndex",
    "It contains external signal receiver 1, external signal receiver 2,or prs or input 1 and input 2."};

#ifndef stsClockInputStateSYNTAX
CLIUMAPSTRUC_t stsClockInputStateMAP[] = {
{"active", 1}, {"standby", 2}, {"down", 3}, {"recovering", 4}, MAPDATA_END};

#define stsClockInputStateSYNTAX  stsClockInputStateMAP
#endif

static CLIMIBENTITY_t stsClockInputStateENT = {
    OIDvalue(LNstsClockInputState, IDstsClockInputState)
    NULL,
    "clkinst",
    ARG_ENUM,
    4,
#ifdef stsClockInputStateSYNTAX
    (void *)stsClockInputStateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputState",
    "This object allows users to view this clock module status."};

static pCLIMIBENTITY_t stsClockInputStatusTableLST[] = {
                             NULL};

static pCLIMIBENTITY_t stsClockInputStatusTableINDX[] = {
     &stsClockInputStatusIndexENT};

CLIMIBCFGGROUP_t stsClockInputStatusTableTBL = {
    stsClockInputStatusTableINDX,
    stsClockInputStatusTableLST,
    1,
    0};

static MIBDSPENTITY_t rtrv_clockinput_statusDENT[] = {
    {&stsClockInputStatusIndexENT, " %s", NULL, DSP_STRING},
    {&stsClockInputStateENT, ": %s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_clockinput_statusDSP = {
  "rtrv_clockinput_status",
  &stsClockInputStatusTableTBL,
  2,
  rtrv_clockinput_statusDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSCLOCKINPUTSSMTABLE_H_
#define _TB_STSCLOCKINPUTSSMTABLE_H_

#ifndef stsClockInputSSMIndexSYNTAX
CLIUMAPSTRUC_t stsClockInputSSMIndexMAP[] = {
{"link1", 1}, {"link2", 2}, MAPDATA_END};

#define stsClockInputSSMIndexSYNTAX  stsClockInputSSMIndexMAP
#endif

static CLIMIBENTITY_t stsClockInputSSMIndexENT = {
    OIDvalue(LNstsClockInputSSMIndex, IDstsClockInputSSMIndex)
    NULL,
    "clkinssmidx",
    ARG_ENUM,
    2,
#ifdef stsClockInputSSMIndexSYNTAX
    (void *)stsClockInputSSMIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputSSMIndex",
    "This is the index of indicating input 1 and input 2."};

#ifndef stsClockInputSSMInputSelectSYNTAX
CLIUMAPSTRUC_t stsClockInputSSMInputSelectMAP[] = {
{"e1-sa4-ssm", 1}, {"e1-sa5-ssm", 2}, {"e1-sa6-ssm", 3}, {"e1-sa7-ssm", 4}, {"e1-sa8-ssm", 5}, {"t1-ssm", 6}, MAPDATA_END};

#define stsClockInputSSMInputSelectSYNTAX  stsClockInputSSMInputSelectMAP
#endif

static CLIMIBENTITY_t stsClockInputSSMInputSelectENT = {
    OIDvalue(LNstsClockInputSSMInputSelect, IDstsClockInputSSMInputSelect)
    NULL,
    "clkinssminsel",
    ARG_ENUM,
    6,
#ifdef stsClockInputSSMInputSelectSYNTAX
    (void *)stsClockInputSSMInputSelectSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputSSMInputSelect",
    "This second index indetifies the SSM type."};

#ifndef stsClockInputSSMValueSYNTAX
CLIUMAPSTRUC_t stsClockInputSSMValueMAP[] = {
{"prs", 1}, {"stu", 2}, {"st2", 3}, {"st3", 4}, {"sic", 5}, {"duc", 6}, MAPDATA_END};

#define stsClockInputSSMValueSYNTAX  stsClockInputSSMValueMAP
#endif

static CLIMIBENTITY_t stsClockInputSSMValueENT = {
    OIDvalue(LNstsClockInputSSMValue, IDstsClockInputSSMValue)
    NULL,
    "clkinssmval",
    ARG_ENUM,
    6,
#ifdef stsClockInputSSMValueSYNTAX
    (void *)stsClockInputSSMValueSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockInputSSMValue",
    "This object does not allow users to specify this SSM value.
prs - primary reference (stratum 1)
stu - sync traceability unknown
st2 - stratum 2 reference
st3 - stratum 3 reference
sic - sonet minimum clock traceable
dus - don't use for sync"};

static pCLIMIBENTITY_t stsClockInputSSMTableLST[] = {
                             NULL};

static pCLIMIBENTITY_t stsClockInputSSMTableINDX[] = {
        &stsClockInputSSMIndexENT,   &stsClockInputSSMInputSelectENT};

CLIMIBCFGGROUP_t stsClockInputSSMTableTBL = {
    stsClockInputSSMTableINDX,
    stsClockInputSSMTableLST,
    2,
    0};

static MIBDSPENTITY_t rtrv_clockinput_ssmDENT[] = {
    {&stsClockInputSSMIndexENT, " %s", NULL, DSP_STRING},
    {&stsClockInputSSMInputSelectENT, ": %s", NULL, DSP_STRING},
    {&stsClockInputSSMValueENT, ", %s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_clockinput_ssmDSP = {
  "rtrv_clockinput_ssm",
  &stsClockInputSSMTableTBL,
  3,
  rtrv_clockinput_ssmDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSCARDTABLE_H_
#define _TB_STSCARDTABLE_H_

#ifndef stsCardShelfSYNTAX
CLIUMAPSTRUC_t stsCardShelfMAP[] = {
{"main", 1}, {"expansion-1", 2}, {"expansion-2", 3}, {"expansion-3", 4}, {"expansion-4", 5}, MAPDATA_END};

#define stsCardShelfSYNTAX  stsCardShelfMAP
#endif

static CLIMIBENTITY_t stsCardShelfENT = {
    OIDvalue(LNstsCardShelf, IDstsCardShelf)
    NULL,
    "cdshelf",
    ARG_ENUM,
    5,
#ifdef stsCardShelfSYNTAX
    (void *)stsCardShelfSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsCardShelf",
    "Output shelf number. Shelf 0 is the main chassis."};

#ifndef stsCardIndexSYNTAX
CLIUMAPSTRUC_t stsCardIndexMAP[] = {
{"output1", 1}, {"output2", 2}, {"output3", 3}, {"output4", 4}, {"output5", 5}, {"output6", 6}, {"output7", 7}, {"output8", 8}, {"output9", 9}, {"output10", 10}, {"output11", 11}, {"output12", 12}, {"output13", 13}, {"output14", 14}, {"output15", 15}, {"output16", 16}, {"esr1", 17}, {"esr2", 18}, {"smu", 19}, {"oscillator1", 20}, {"oscillator2", 21}, {"top", 22}, {"imu", 23}, MAPDATA_END};

#define stsCardIndexSYNTAX  stsCardIndexMAP
#endif

static CLIMIBENTITY_t stsCardIndexENT = {
    OIDvalue(LNstsCardIndex, IDstsCardIndex)
    NULL,
    "cdidx",
    ARG_ENUM,
    23,
#ifdef stsCardIndexSYNTAX
    (void *)stsCardIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsCardIndex",
    "Index into cardTable. In 5820 system index 17 to 24 are built in models if existed."};

#ifndef stsCardTypeSYNTAX
CLIUMAPSTRUC_t stsCardTypeMAP[] = {
{"cardTypeSmu", 1}, {"cardTypeOutputFrequency", 2}, {"cardTypeOutputIrig-B", 3}, {"cardTypeOutputTiming", 4}, {"cardTypeOutputTiu", 5}, {"cardTypeOutputF5F10", 6}, {"cardTypeOutputTop", 7}, {"cardTypeGpsCdma", 8}, {"cardTypeGpsFrequency", 9}, {"cardTypeGpsUmts", 10}, {"cardTypeGpsGsm", 11}, {"cardTypeClkRb", 12}, {"cardTypeClkCrystalSt2", 13}, {"cardTypeClkCrystalSt3", 14}, {"cardTypeClkCrystal", 15}, {"cardTypeImu", 16}, MAPDATA_END};

#define stsCardTypeSYNTAX  stsCardTypeMAP
#endif

static CLIMIBENTITY_t stsCardTypeENT = {
    OIDvalue(LNstsCardType, IDstsCardType)
    NULL,
    "cdtype",
    ARG_ENUM,
    16,
#ifdef stsCardTypeSYNTAX
    (void *)stsCardTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsCardType",
    "card type."};

static CLIMIBENTITY_t stsCardDescrENT = {
    OIDvalue(LNstsCardDescr, IDstsCardDescr)
    NULL,
    "cddescr",
    ARG_STR,
    0,
#ifdef stsCardDescrSYNTAX
    (void *)stsCardDescrSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsCardDescr",
    "Text description of this card."};

static CLIMIBENTITY_t stsCardSerialENT = {
    OIDvalue(LNstsCardSerial, IDstsCardSerial)
    NULL,
    "cdseri",
    ARG_STR,
    0,
#ifdef stsCardSerialSYNTAX
    (void *)stsCardSerialSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsCardSerial",
    "The serial number of this card, or zero if
      unavailable."};

static CLIMIBENTITY_t stsCardHwVersionENT = {
    OIDvalue(LNstsCardHwVersion, IDstsCardHwVersion)
    NULL,
    "cdhwver",
    ARG_STR,
    0,
#ifdef stsCardHwVersionSYNTAX
    (void *)stsCardHwVersionSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsCardHwVersion",
    "Hardware revision level of this card, or an
    empty string if unavailable."};

static CLIMIBENTITY_t stsCardCLEICodeENT = {
    OIDvalue(LNstsCardCLEICode, IDstsCardCLEICode)
    NULL,
    "cdclei",
    ARG_STR,
    0,
#ifdef stsCardCLEICodeSYNTAX
    (void *)stsCardCLEICodeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsCardCLEICode",
    "CLEI code of this card, or an empty string if
      unavailable."};

#ifndef stsCardOperStatusSYNTAX
CLIUMAPSTRUC_t stsCardOperStatusMAP[] = {
{"active", 1}, {"standby", 2}, {"failed", 3}, {"notPresent", 4}, {"misMatch", 5}, MAPDATA_END};

#define stsCardOperStatusSYNTAX  stsCardOperStatusMAP
#endif

static CLIMIBENTITY_t stsCardOperStatusENT = {
    OIDvalue(LNstsCardOperStatus, IDstsCardOperStatus)
    NULL,
    "cdopst",
    ARG_ENUM,
    5,
#ifdef stsCardOperStatusSYNTAX
    (void *)stsCardOperStatusSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsCardOperStatus",
    "The operational status of the card.
       stsCardOperStatus is up when a card is
       recognized by the device and is enabled for
       operation. stsCardOperStatus is failed if the
       card is not recognized by the device, or if
       it is not enabled for operation.
       stsCardOperStatus is standby if the card is
       enabled and acting as a standby slave"};

#ifndef stsCardResetSYNTAX
CLIUMAPSTRUC_t stsCardResetMAP[] = {
{"noAction", 1}, {"reset", 2}, MAPDATA_END};

#define stsCardResetSYNTAX  stsCardResetMAP
#endif

static CLIMIBENTITY_t stsCardResetENT = {
    OIDvalue(LNstsCardReset, IDstsCardReset)
    NULL,
    "cdreset",
    ARG_ENUM,
    2,
#ifdef stsCardResetSYNTAX
    (void *)stsCardResetSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsCardReset",
    "This object is used to reset this card"};

static pCLIMIBENTITY_t stsCardTableLST[] = {
                 &stsCardResetENT};

static pCLIMIBENTITY_t stsCardTableINDX[] = {
                 &stsCardShelfENT,                  &stsCardIndexENT};

CLIMIBCFGGROUP_t stsCardTableTBL = {
    stsCardTableINDX,
    stsCardTableLST,
    2,
    1};

static MIBDSPENTITY_t rtrv_cardDENT[] = {
    {&stsCardShelfENT, " %s", NULL, DSP_STRING},
    {&stsCardIndexENT, ": SLOT", NULL, DSP_TABLE},
    {&stsCardTypeENT, ": %s", NULL, DSP_STRING},
    {&stsCardDescrENT, ", %s", NULL, DSP_STRING},
    {&stsCardSerialENT, ", %s", NULL, DSP_STRING},
    {&stsCardHwVersionENT, ", %s", NULL, DSP_STRING},
    {&stsCardCLEICodeENT, ", %s", NULL, DSP_STRING},
    {&stsCardOperStatusENT, ", %s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_cardDSP = {
  "rtrv_card",
  &stsCardTableTBL,
  8,
  rtrv_cardDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSOSCILLATORTABLE_H_
#define _TB_STSOSCILLATORTABLE_H_

#ifndef stsOscillatorIndexSYNTAX
CLIUMAPSTRUC_t stsOscillatorIndexMAP[] = {
{"oscillator1", 1}, {"oscillator2", 2}, MAPDATA_END};

#define stsOscillatorIndexSYNTAX  stsOscillatorIndexMAP
#endif

static CLIMIBENTITY_t stsOscillatorIndexENT = {
    OIDvalue(LNstsOscillatorIndex, IDstsOscillatorIndex)
    NULL,
    "oscidx",
    ARG_ENUM,
    2,
#ifdef stsOscillatorIndexSYNTAX
    (void *)stsOscillatorIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorIndex",
    "Oscillator index"};

#ifndef stsOscillatorModuleTypeSYNTAX
CLIUMAPSTRUC_t stsOscillatorModuleTypeMAP[] = {
{"rb", 1}, {"crystalSt2", 2}, {"crystalSt3", 3}, {"crystal", 4}, MAPDATA_END};

#define stsOscillatorModuleTypeSYNTAX  stsOscillatorModuleTypeMAP
#endif

static CLIMIBENTITY_t stsOscillatorModuleTypeENT = {
    OIDvalue(LNstsOscillatorModuleType, IDstsOscillatorModuleType)
    NULL,
    "oscmodtype",
    ARG_ENUM,
    4,
#ifdef stsOscillatorModuleTypeSYNTAX
    (void *)stsOscillatorModuleTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorModuleType",
    "Oscillator module"};

static CLIMIBENTITY_t stsOscillatorEFERENT = {
    OIDvalue(LNstsOscillatorEFER, IDstsOscillatorEFER)
    NULL,
    "oscefer",
    ARG_STR,
    0,
#ifdef stsOscillatorEFERSYNTAX
    (void *)stsOscillatorEFERSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorEFER",
    "Estimator Frequency Error, EFER outputs the most recently calculated frequency error estimate.
This object is supported in 5850, but not 5820."};

static CLIMIBENTITY_t stsOscillatorESSDENT = {
    OIDvalue(LNstsOscillatorESSD, IDstsOscillatorESSD)
    NULL,
    "oscessd",
    ARG_STR,
    0,
#ifdef stsOscillatorESSDSYNTAX
    (void *)stsOscillatorESSDSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorESSD",
    "Estimator Standard Deviation, ESSD outputs the most recently calculated standard deviation of the
estimator error (actual/estimated time error).
This object is supported in 5850, but not 5820."};

static CLIMIBENTITY_t stsOscillatorESSNENT = {
    OIDvalue(LNstsOscillatorESSN, IDstsOscillatorESSN)
    NULL,
    "oscessn",
    ARG_STR,
    0,
#ifdef stsOscillatorESSNSYNTAX
    (void *)stsOscillatorESSNSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorESSN",
    "Estimator Sample Number, The ESSN command outputs the current estimator sample. One sample is
taken per second.
This object is supported in 5850, but not 5820."};

static CLIMIBENTITY_t stsOscillatorFAGEENT = {
    OIDvalue(LNstsOscillatorFAGE, IDstsOscillatorFAGE)
    NULL,
    "oscfage",
    ARG_STR,
    0,
#ifdef stsOscillatorFAGESYNTAX
    (void *)stsOscillatorFAGESYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorFAGE",
    "Frequency Age, FAGE outputs the frequency aging. This is a measurement of how quickly
the on-board VCXO frequency is changing.
This object is supported in 5850, but not 5820."};

static CLIMIBENTITY_t stsOscillatorTIMDENT = {
    OIDvalue(LNstsOscillatorTIMD, IDstsOscillatorTIMD)
    NULL,
    "osctimd",
    ARG_STR,
    0,
#ifdef stsOscillatorTIMDSYNTAX
    (void *)stsOscillatorTIMDSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorTIMD",
    "Time Difference between GPS 1PPS and Oscillator 1PPS,TIMD outputs the time difference between the GPS 1PPS and oscillator
1PPS in nanoseconds relative to coordinated universal time (UTC).
This object is supported in 5850, but not 5820."};

#ifndef stsOscillatorSwitchStateSYNTAX
CLIUMAPSTRUC_t stsOscillatorSwitchStateMAP[] = {
{"noAction", 1}, {"switchOver", 2}, MAPDATA_END};

#define stsOscillatorSwitchStateSYNTAX  stsOscillatorSwitchStateMAP
#endif

static CLIMIBENTITY_t stsOscillatorSwitchStateENT = {
    OIDvalue(LNstsOscillatorSwitchState, IDstsOscillatorSwitchState)
    NULL,
    "oscswst",
    ARG_ENUM,
    2,
#ifdef stsOscillatorSwitchStateSYNTAX
    (void *)stsOscillatorSwitchStateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorSwitchState",
    "This object allows users to provision this Oscillator module to be switch over manually. 
                 The previous active one will be switched over to standby."};

#ifndef stsOscillatorStatusSYNTAX
CLIUMAPSTRUC_t stsOscillatorStatusMAP[] = {
{"holdoversync", 1}, {"aqrsync", 2}, {"frngsync", 3}, {"lkdsync", 4}, {"absent", 5}, MAPDATA_END};

#define stsOscillatorStatusSYNTAX  stsOscillatorStatusMAP
#endif

static CLIMIBENTITY_t stsOscillatorStatusENT = {
    OIDvalue(LNstsOscillatorStatus, IDstsOscillatorStatus)
    NULL,
    "oscst",
    ARG_ENUM,
    5,
#ifdef stsOscillatorStatusSYNTAX
    (void *)stsOscillatorStatusSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorStatus",
    "This object allows users to view this Oscillator module status."};

#ifndef stsOscillatorRedStatusSYNTAX
CLIUMAPSTRUC_t stsOscillatorRedStatusMAP[] = {
{"active", 1}, {"standby", 2}, MAPDATA_END};

#define stsOscillatorRedStatusSYNTAX  stsOscillatorRedStatusMAP
#endif

static CLIMIBENTITY_t stsOscillatorRedStatusENT = {
    OIDvalue(LNstsOscillatorRedStatus, IDstsOscillatorRedStatus)
    NULL,
    "oscredst",
    ARG_ENUM,
    2,
#ifdef stsOscillatorRedStatusSYNTAX
    (void *)stsOscillatorRedStatusSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorRedStatus",
    "This object allows users to view this Oscillator module redundant status."};

static pCLIMIBENTITY_t stsOscillatorTableLST[] = {
     &stsOscillatorSwitchStateENT,                              NULL};

static pCLIMIBENTITY_t stsOscillatorTableINDX[] = {
           &stsOscillatorIndexENT};

CLIMIBCFGGROUP_t stsOscillatorTableTBL = {
    stsOscillatorTableINDX,
    stsOscillatorTableLST,
    1,
    1};

static MIBDSPENTITY_t rtrv_oscDENT[] = {
    {&stsOscillatorIndexENT, " %s", NULL, DSP_STRING},
    {&stsOscillatorStatusENT, ":", NULL, DSP_TABLE}};

CLIMIBDSPGROUP_t rtrv_oscDSP = {
  "rtrv_osc",
  &stsOscillatorTableTBL,
  2,
  rtrv_oscDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSGPSTABLE_H_
#define _TB_STSGPSTABLE_H_

#ifndef stsGpsIndexSYNTAX
CLIUMAPSTRUC_t stsGpsIndexMAP[] = {
{"gps1", 1}, {"gps2", 2}, MAPDATA_END};

#define stsGpsIndexSYNTAX  stsGpsIndexMAP
#endif

static CLIMIBENTITY_t stsGpsIndexENT = {
    OIDvalue(LNstsGpsIndex, IDstsGpsIndex)
    NULL,
    "gpsidx",
    ARG_ENUM,
    2,
#ifdef stsGpsIndexSYNTAX
    (void *)stsGpsIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsIndex",
    "GPS module index"};

#ifndef stsGpsStatusSYNTAX
CLIUMAPSTRUC_t stsGpsStatusMAP[] = {
{"ok", 1}, {"noSignal", 2}, {"poorSignal", 3}, {"nonAvailable", 4}, {"atennaShort", 5}, MAPDATA_END};

#define stsGpsStatusSYNTAX  stsGpsStatusMAP
#endif

static CLIMIBENTITY_t stsGpsStatusENT = {
    OIDvalue(LNstsGpsStatus, IDstsGpsStatus)
    NULL,
    "gpsst",
    ARG_ENUM,
    5,
#ifdef stsGpsStatusSYNTAX
    (void *)stsGpsStatusSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsStatus",
    "GPS current status"};

static CLIMIBENTITY_t stsGpsANTDENT = {
    OIDvalue(LNstsGpsANTD, IDstsGpsANTD)
    NULL,
    "gpsantd",
    ARG_STR,
    0,
#ifdef stsGpsANTDSYNTAX
    (void *)stsGpsANTDSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsANTD",
    "ANTD outputs of the antenna cable delay., +99999 THRU -99999 in nanoseconds."};

static CLIMIBENTITY_t stsGpsAZELENT = {
    OIDvalue(LNstsGpsAZEL, IDstsGpsAZEL)
    NULL,
    "gpsazel",
    ARG_STR,
    0,
#ifdef stsGpsAZELSYNTAX
    (void *)stsGpsAZELSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsAZEL",
    "Azimuth and Elevation, AZEL outputs the satellite number followed by its elevation in degrees
above the horizon and azimuth in degrees relative to true north. The twelve
satellites are the ones currently visible to the GPS receiver."};

static CLIMIBENTITY_t stsGpsRSTGENT = {
    OIDvalue(LNstsGpsRSTG, IDstsGpsRSTG)
    NULL,
    "gpsrstg",
    ARG_STR,
    0,
#ifdef stsGpsRSTGSYNTAX
    (void *)stsGpsRSTGSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsRSTG",
    "RSTG."};

static CLIMIBENTITY_t stsGpsBITSENT = {
    OIDvalue(LNstsGpsBITS, IDstsGpsBITS)
    NULL,
    "gpsbits",
    ARG_STR,
    0,
#ifdef stsGpsBITSSYNTAX
    (void *)stsGpsBITSSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsBITS",
    "Built-in Test Status, the BITS command outputs the status of various built-in tests."};

static CLIMIBENTITY_t stsGpsPAVGENT = {
    OIDvalue(LNstsGpsPAVG, IDstsGpsPAVG)
    NULL,
    "gpspavg",
    ARG_STR,
    0,
#ifdef stsGpsPAVGSYNTAX
    (void *)stsGpsPAVGSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsPAVG",
    "Position Average Status, The PAVG command retrieves the average latitude, longitude, and altitude."};

static CLIMIBENTITY_t stsGpsSIGQENT = {
    OIDvalue(LNstsGpsSIGQ, IDstsGpsSIGQ)
    NULL,
    "gpssigq",
    ARG_STR,
    0,
#ifdef stsGpsSIGQSYNTAX
    (void *)stsGpsSIGQSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsSIGQ",
    "Signal Quality, The SIGQ command outputs eight sets of satellite signal quality from 0 to
9, where 0 is the worst and 9 is the best signal strength. Signal levels
greater than three are considered adequate for normal operation."};

static CLIMIBENTITY_t stsGpsSPOSENT = {
    OIDvalue(LNstsGpsSPOS, IDstsGpsSPOS)
    NULL,
    "gpsspos",
    ARG_STR,
    0,
#ifdef stsGpsSPOSSYNTAX
    (void *)stsGpsSPOSSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsSPOS",
    "Set Position, SPOS retrieves or sets the latitude, longitude, and altitude."};

static CLIMIBENTITY_t stsGpsTRMOENT = {
    OIDvalue(LNstsGpsTRMO, IDstsGpsTRMO)
    NULL,
    "gpstrmo",
    ARG_STR,
    0,
#ifdef stsGpsTRMOSYNTAX
    (void *)stsGpsTRMOSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsTRMO",
    "Time Recovery Mode, TRMO allows the user to select or read back the Time Recovery mode of
operation. If mode option K is added, the receiver will be put into Known
mode. This is only recommended if the current position (SPOS) is correct
or the user is about to set the position. The unit will enter GPS Track
sooner in Known mode than in Position Average mode. Do not use mode
option D; it is only for a station that is not stationary. Use mode option P
if mode option K is entered and the current position is not correct."};

static CLIMIBENTITY_t stsGpsUTCTENT = {
    OIDvalue(LNstsGpsUTCT, IDstsGpsUTCT)
    NULL,
    "gpsutct",
    ARG_STR,
    0,
#ifdef stsGpsUTCTSYNTAX
    (void *)stsGpsUTCTSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsUTCT",
    "Coordinated Universal Time, The UTCT command retrieves UTC time."};

static CLIMIBENTITY_t stsGpsVERSENT = {
    OIDvalue(LNstsGpsVERS, IDstsGpsVERS)
    NULL,
    "gpsvers",
    ARG_STR,
    0,
#ifdef stsGpsVERSSYNTAX
    (void *)stsGpsVERSSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsVERS",
    "Get Current Firmware Version, VERS outputs the current version numbers."};

static CLIMIBENTITY_t stsGpsDETFENT = {
    OIDvalue(LNstsGpsDETF, IDstsGpsDETF)
    NULL,
    "gpsdetf",
    ARG_STR,
    0,
#ifdef stsGpsDETFSYNTAX
    (void *)stsGpsDETFSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsDETF",
    "Determine Frequency, DETF determines the frequency of the GPS input (should be 1.544 MHz).
Use this command when the antenna is moved. Precede this command with RSTG,C.
This object is supported in 5850, but not 5820."};

static CLIMIBENTITY_t stsGpsEFERENT = {
    OIDvalue(LNstsGpsEFER, IDstsGpsEFER)
    NULL,
    "gpsefer",
    ARG_STR,
    0,
#ifdef stsGpsEFERSYNTAX
    (void *)stsGpsEFERSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsEFER",
    "Estimator Frequency Error, EFER outputs the most recently calculated frequency error estimate.
This object is supported in 5850, but not 5820."};

static CLIMIBENTITY_t stsGpsESSDENT = {
    OIDvalue(LNstsGpsESSD, IDstsGpsESSD)
    NULL,
    "gpsessd",
    ARG_STR,
    0,
#ifdef stsGpsESSDSYNTAX
    (void *)stsGpsESSDSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsESSD",
    "Estimator Standard Deviation, ESSD outputs the most recently calculated standard deviation of the
estimator error (actual/estimated time error).
This object is supported in 5850, but not 5820."};

static CLIMIBENTITY_t stsGpsESSNENT = {
    OIDvalue(LNstsGpsESSN, IDstsGpsESSN)
    NULL,
    "gpsessn",
    ARG_STR,
    0,
#ifdef stsGpsESSNSYNTAX
    (void *)stsGpsESSNSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsESSN",
    "Estimator Sample Number, The ESSN command outputs the current estimator sample. One sample is
taken per second.
This object is supported in 5850, but not 5820."};

static CLIMIBENTITY_t stsGpsFAGEENT = {
    OIDvalue(LNstsGpsFAGE, IDstsGpsFAGE)
    NULL,
    "gpsfage",
    ARG_STR,
    0,
#ifdef stsGpsFAGESYNTAX
    (void *)stsGpsFAGESYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsFAGE",
    "Frequency Age, FAGE outputs the frequency aging. This is a measurement of how quickly
the on-board VCXO frequency is changing.
This object is supported in 5850, but not 5820."};

static CLIMIBENTITY_t stsGpsTIMDENT = {
    OIDvalue(LNstsGpsTIMD, IDstsGpsTIMD)
    NULL,
    "gpstimd",
    ARG_STR,
    0,
#ifdef stsGpsTIMDSYNTAX
    (void *)stsGpsTIMDSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsGpsTIMD",
    "Time Difference between GPS 1PPS and Oscillator 1PPS,TIMD outputs the time difference between the GPS 1PPS and oscillator
1PPS in nanoseconds relative to coordinated universal time (UTC).
This object is supported in 5850, but not 5820."};

static pCLIMIBENTITY_t stsGpsTableLST[] = {
                   &stsGpsSPOSENT,
                   &stsGpsTRMOENT,                              NULL};

static pCLIMIBENTITY_t stsGpsTableINDX[] = {
                  &stsGpsIndexENT};

CLIMIBCFGGROUP_t stsGpsTableTBL = {
    stsGpsTableINDX,
    stsGpsTableLST,
    1,
    2};

static MIBDSPENTITY_t rtrv_gps_antdDENT[] = {
    {&stsGpsANTDENT, " $ANTD,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_antdDSP = {
  "rtrv_gps_antd",
  &stsGpsTableTBL,
  1,
  rtrv_gps_antdDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_azelDENT[] = {
    {&stsGpsAZELENT, " $AZEL,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_azelDSP = {
  "rtrv_gps_azel",
  &stsGpsTableTBL,
  1,
  rtrv_gps_azelDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_bitsDENT[] = {
    {&stsGpsBITSENT, " $BITS,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_bitsDSP = {
  "rtrv_gps_bits",
  &stsGpsTableTBL,
  1,
  rtrv_gps_bitsDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_pavgDENT[] = {
    {&stsGpsPAVGENT, " $PAVG,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_pavgDSP = {
  "rtrv_gps_pavg",
  &stsGpsTableTBL,
  1,
  rtrv_gps_pavgDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_sigqDENT[] = {
    {&stsGpsSIGQENT, " $SIGQ,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_sigqDSP = {
  "rtrv_gps_sigq",
  &stsGpsTableTBL,
  1,
  rtrv_gps_sigqDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_sposDENT[] = {
    {&stsGpsSPOSENT, " $SPOS,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_sposDSP = {
  "rtrv_gps_spos",
  &stsGpsTableTBL,
  1,
  rtrv_gps_sposDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_trmoDENT[] = {
    {&stsGpsTRMOENT, " $TRMO,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_trmoDSP = {
  "rtrv_gps_trmo",
  &stsGpsTableTBL,
  1,
  rtrv_gps_trmoDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_utctDENT[] = {
    {&stsGpsUTCTENT, " $UTCT,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_utctDSP = {
  "rtrv_gps_utct",
  &stsGpsTableTBL,
  1,
  rtrv_gps_utctDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_versDENT[] = {
    {&stsGpsVERSENT, " $VERS,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_versDSP = {
  "rtrv_gps_vers",
  &stsGpsTableTBL,
  1,
  rtrv_gps_versDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_detfDENT[] = {
    {&stsGpsDETFENT, " $DETF,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_detfDSP = {
  "rtrv_gps_detf",
  &stsGpsTableTBL,
  1,
  rtrv_gps_detfDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_eferDENT[] = {
    {&stsGpsEFERENT, " $EFER,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_eferDSP = {
  "rtrv_gps_efer",
  &stsGpsTableTBL,
  1,
  rtrv_gps_eferDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_essdDENT[] = {
    {&stsGpsESSDENT, " $ESSD,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_essdDSP = {
  "rtrv_gps_essd",
  &stsGpsTableTBL,
  1,
  rtrv_gps_essdDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_essnDENT[] = {
    {&stsGpsESSNENT, " $ESSN,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_essnDSP = {
  "rtrv_gps_essn",
  &stsGpsTableTBL,
  1,
  rtrv_gps_essnDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_fageDENT[] = {
    {&stsGpsFAGEENT, " $FAGE,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_fageDSP = {
  "rtrv_gps_fage",
  &stsGpsTableTBL,
  1,
  rtrv_gps_fageDENT,
  ""};

static MIBDSPENTITY_t rtrv_gps_timdDENT[] = {
    {&stsGpsTIMDENT, " $TIMD,%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_gps_timdDSP = {
  "rtrv_gps_timd",
  &stsGpsTableTBL,
  1,
  rtrv_gps_timdDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSLINKTABLE_H_
#define _TB_STSLINKTABLE_H_

#ifndef stsLinkIndexSYNTAX
CLIUMAPSTRUC_t stsLinkIndexMAP[] = {
{"link1", 1}, {"link2", 2}, {"link3", 3}, {"link4", 4}, {"link5", 5}, MAPDATA_END};

#define stsLinkIndexSYNTAX  stsLinkIndexMAP
#endif

static CLIMIBENTITY_t stsLinkIndexENT = {
    OIDvalue(LNstsLinkIndex, IDstsLinkIndex)
    NULL,
    "lnkidx",
    ARG_ENUM,
    5,
#ifdef stsLinkIndexSYNTAX
    (void *)stsLinkIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkIndex",
    "Type identify this input link."};

#ifndef stsLinkTypeSYNTAX
CLIUMAPSTRUC_t stsLinkTypeMAP[] = {
{"e1", 1}, {"t1", 2}, {"cc", 3}, {"f2", 4}, {"j1", 5}, MAPDATA_END};

#define stsLinkTypeSYNTAX  stsLinkTypeMAP
#endif

static CLIMIBENTITY_t stsLinkTypeENT = {
    OIDvalue(LNstsLinkType, IDstsLinkType)
    NULL,
    "lnktype",
    ARG_ENUM,
    5,
#ifdef stsLinkTypeSYNTAX
    (void *)stsLinkTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkType",
    "Identify the input link type."};

#ifndef stsLinkEnableSYNTAX
CLIUMAPSTRUC_t stsLinkEnableMAP[] = {
{"yes", 1}, {"no", 2}, MAPDATA_END};

#define stsLinkEnableSYNTAX  stsLinkEnableMAP
#endif

static CLIMIBENTITY_t stsLinkEnableENT = {
    OIDvalue(LNstsLinkEnable, IDstsLinkEnable)
    NULL,
    "lnkenable",
    ARG_ENUM,
    2,
#ifdef stsLinkEnableSYNTAX
    (void *)stsLinkEnableSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkEnable",
    "Cab be configured on link 3, 4 and 5."};

#ifndef stsLinkLineCodeSYNTAX
CLIUMAPSTRUC_t stsLinkLineCodeMAP[] = {
{"ami", 1}, {"b8zs", 2}, {"hdb3", 3}, {"zbtsi", 4}, {"jbzs", 5}, {"other", 6}, MAPDATA_END};

#define stsLinkLineCodeSYNTAX  stsLinkLineCodeMAP
#endif

static CLIMIBENTITY_t stsLinkLineCodeENT = {
    OIDvalue(LNstsLinkLineCode, IDstsLinkLineCode)
    NULL,
    "lnklncode",
    ARG_ENUM,
    6,
#ifdef stsLinkLineCodeSYNTAX
    (void *)stsLinkLineCodeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkLineCode",
    "for T1 link only. default is AMI."};

#ifndef stsLinkFMTSYNTAX
CLIUMAPSTRUC_t stsLinkFMTMAP[] = {
{"t1-d4", 1}, {"t1-esf", 2}, {"j1-d4", 3}, {"j1-esf", 4}, {"e1", 5}, {"e1-crc", 6}, {"e1-mf", 7}, {"e1-crc-mf", 8}, {"f2", 9}, {"cc", 10}, {"cc-400", 11}, MAPDATA_END};

#define stsLinkFMTSYNTAX  stsLinkFMTMAP
#endif

static CLIMIBENTITY_t stsLinkFMTENT = {
    OIDvalue(LNstsLinkFMT, IDstsLinkFMT)
    NULL,
    "lnkfmt",
    ARG_ENUM,
    11,
#ifdef stsLinkFMTSYNTAX
    (void *)stsLinkFMTSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkFMT",
    "default is t1-d4."};

#ifndef stsLinkTerminationSYNTAX
CLIUMAPSTRUC_t stsLinkTerminationMAP[] = {
{"ohm-75", 1}, {"ohm-100", 2}, {"ohm-110", 3}, {"ohm-120", 4}, MAPDATA_END};

#define stsLinkTerminationSYNTAX  stsLinkTerminationMAP
#endif

static CLIMIBENTITY_t stsLinkTerminationENT = {
    OIDvalue(LNstsLinkTermination, IDstsLinkTermination)
    NULL,
    "lnkterm",
    ARG_ENUM,
    4,
#ifdef stsLinkTerminationSYNTAX
    (void *)stsLinkTerminationSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkTermination",
    "default is ohm-100."};

#ifndef stsLinkMFCHKSYNTAX
CLIUMAPSTRUC_t stsLinkMFCHKMAP[] = {
{"enable", 1}, {"disable", 2}, MAPDATA_END};

#define stsLinkMFCHKSYNTAX  stsLinkMFCHKMAP
#endif

static CLIMIBENTITY_t stsLinkMFCHKENT = {
    OIDvalue(LNstsLinkMFCHK, IDstsLinkMFCHK)
    NULL,
    "lnkmfchk",
    ARG_ENUM,
    2,
#ifdef stsLinkMFCHKSYNTAX
    (void *)stsLinkMFCHKSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkMFCHK",
    "Used for link1 through link5. If MFCHK is enabled,
it will force a counter MF error. Otherwise, it will ignore a MF error."};

static CLIMIBENTITY_t stsLinkIdENT = {
    OIDvalue(LNstsLinkId, IDstsLinkId)
    NULL,
    "lnkid",
    ARG_STR,
    0,
#ifdef stsLinkIdSYNTAX
    (void *)stsLinkIdSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkId",
    " The Link ID is assigned by the administrator for this link. Identification code of the primary or secondary external
timing link. This is one to twenty characters. Only letters, digits, and hyphens are allowed. Special characters, including space and quotes, are not allowed.
The first character may not be a hyphen. Only applicable to PLNK and SLNK"};

static pCLIMIBENTITY_t stsLinkTableLST[] = {
                  &stsLinkTypeENT,                 &stsLinkEnableENT,
              &stsLinkLineCodeENT,                    &stsLinkFMTENT,
           &stsLinkTerminationENT,                  &stsLinkMFCHKENT,
                    &stsLinkIdENT};

static pCLIMIBENTITY_t stsLinkTableINDX[] = {
                 &stsLinkIndexENT};

CLIMIBCFGGROUP_t stsLinkTableTBL = {
    stsLinkTableINDX,
    stsLinkTableLST,
    1,
    7};

static MIBDSPENTITY_t view_linkDENT[] = {
    {&stsLinkIndexENT, " %s", NULL, DSP_STRING},
    {&stsLinkTypeENT, ":%s", NULL, DSP_STRING},
    {&stsLinkFMTENT, ":FMT =%s", NULL, DSP_STRING},
    {&stsLinkLineCodeENT, ":LINECODE=%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t view_linkDSP = {
  "view_link",
  &stsLinkTableTBL,
  4,
  view_linkDENT,
  ""};

static MIBDSPENTITY_t view_scan_channelDENT[] = {
    {&stsLinkIndexENT, " %s", NULL, DSP_STRING},
    {&stsLinkEnableENT, ":ENABLE=%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t view_scan_channelDSP = {
  "view_scan_channel",
  &stsLinkTableTBL,
  2,
  view_scan_channelDENT,
  ""};

static MIBDSPENTITY_t rtrv_linkDENT[] = {
    {&stsLinkIndexENT, " %s", NULL, DSP_STRING},
    {&stsLinkIdENT, ":%s", NULL, DSP_STRING},
    {&stsLinkTypeENT, ":%s", NULL, DSP_STRING},
    {&stsLinkFMTENT, ":FMT =%s", NULL, DSP_STRING},
    {&stsLinkLineCodeENT, ":LINECODE=%s", NULL, DSP_STRING},
    {&stsLinkEnableENT, ",SCAN=%s", NULL, DSP_STRING},
    {&stsLinkTerminationENT, ",TERM=%s", NULL, DSP_STRING},
    {&stsLinkMFCHKENT, ",MKCHK=%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_linkDSP = {
  "rtrv_link",
  &stsLinkTableTBL,
  8,
  rtrv_linkDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSSHELFTABLE_H_
#define _TB_STSSHELFTABLE_H_

#ifndef stsShelfIndexSYNTAX
CLIUMAPSTRUC_t stsShelfIndexMAP[] = {
{"main", 1}, {"expansion-1", 2}, {"expansion-2", 3}, {"expansion-3", 4}, {"expansion-4", 5}, MAPDATA_END};

#define stsShelfIndexSYNTAX  stsShelfIndexMAP
#endif

static CLIMIBENTITY_t stsShelfIndexENT = {
    OIDvalue(LNstsShelfIndex, IDstsShelfIndex)
    NULL,
    "shfidx",
    ARG_ENUM,
    5,
#ifdef stsShelfIndexSYNTAX
    (void *)stsShelfIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsShelfIndex",
    "Output shelf number. index value 100 is the main chassis."};

static CLIMIBENTITY_t stsShelfSerialENT = {
    OIDvalue(LNstsShelfSerial, IDstsShelfSerial)
    NULL,
    "shfser",
    ARG_STR,
    0,
#ifdef stsShelfSerialSYNTAX
    (void *)stsShelfSerialSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsShelfSerial",
    "The serial number of this shelf, or zero if
      unavailable."};

static CLIMIBENTITY_t stsShelfCLEICodeENT = {
    OIDvalue(LNstsShelfCLEICode, IDstsShelfCLEICode)
    NULL,
    "shfclei",
    ARG_STR,
    0,
#ifdef stsShelfCLEICodeSYNTAX
    (void *)stsShelfCLEICodeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsShelfCLEICode",
    "CLEI code of this shelf, or an empty string if
                           unavailable."};

#ifndef stsShelfResetSYNTAX
CLIUMAPSTRUC_t stsShelfResetMAP[] = {
{"noAction", 1}, {"reset", 2}, MAPDATA_END};

#define stsShelfResetSYNTAX  stsShelfResetMAP
#endif

static CLIMIBENTITY_t stsShelfResetENT = {
    OIDvalue(LNstsShelfReset, IDstsShelfReset)
    NULL,
    "shfreset",
    ARG_ENUM,
    2,
#ifdef stsShelfResetSYNTAX
    (void *)stsShelfResetSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsShelfReset",
    "This object is used to reset shelf"};

static pCLIMIBENTITY_t stsShelfTableLST[] = {
                &stsShelfResetENT};

static pCLIMIBENTITY_t stsShelfTableINDX[] = {
                &stsShelfIndexENT};

CLIMIBCFGGROUP_t stsShelfTableTBL = {
    stsShelfTableINDX,
    stsShelfTableLST,
    1,
    1};

static MIBDSPENTITY_t rtrv_shelfDENT[] = {
    {&stsShelfIndexENT, " %s", NULL, DSP_STRING},
    {&stsShelfSerialENT, ": SERIAL=%s", NULL, DSP_STRING},
    {&stsShelfCLEICodeENT, ": CLEI=%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_shelfDSP = {
  "rtrv_shelf",
  &stsShelfTableTBL,
  3,
  rtrv_shelfDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSALARMTHRESHOLDTABLE_H_
#define _TB_STSALARMTHRESHOLDTABLE_H_

#ifndef stsAlarmThresholdCurrentMonSYNTAX
CLIUMAPSTRUC_t stsAlarmThresholdCurrentMonMAP[] = {
{"link1", 1}, {"link2", 2}, {"link3", 3}, {"link4", 4}, {"link5", 5}, MAPDATA_END};

#define stsAlarmThresholdCurrentMonSYNTAX  stsAlarmThresholdCurrentMonMAP
#endif

static CLIMIBENTITY_t stsAlarmThresholdCurrentMonENT = {
    OIDvalue(LNstsAlarmThresholdCurrentMon, IDstsAlarmThresholdCurrentMon)
    NULL,
    "almthrcurmon",
    ARG_ENUM,
    5,
#ifdef stsAlarmThresholdCurrentMonSYNTAX
    (void *)stsAlarmThresholdCurrentMonSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAlarmThresholdCurrentMon",
    "Current monitor number."};

#ifndef stsAlarmThresholdTypeSYNTAX
CLIUMAPSTRUC_t stsAlarmThresholdTypeMAP[] = {
{"cvl", 1}, {"cvp", 2}, {"cs", 3}, {"jitter", 4}, {"tie", 5}, {"mtie", 6}, MAPDATA_END};

#define stsAlarmThresholdTypeSYNTAX  stsAlarmThresholdTypeMAP
#endif

static CLIMIBENTITY_t stsAlarmThresholdTypeENT = {
    OIDvalue(LNstsAlarmThresholdType, IDstsAlarmThresholdType)
    NULL,
    "almthrtype",
    ARG_ENUM,
    6,
#ifdef stsAlarmThresholdTypeSYNTAX
    (void *)stsAlarmThresholdTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAlarmThresholdType",
    "Monitor alarm threshold setting type.
CVL - Coding violation count - Line (BPV)
CVP - Coding violation count - Path (CRC)
CS - Controlled slip count
JITTER - Peak to peak JITTER in unit interval
TIE - Time interval error in nanoseconds
MTIE - Maximum time interval error in nanoseconds"};

#ifndef stsAlarmThresholdValueSYNTAX
static U8_t stsAlarmThresholdValueRANGE[] = "0..10000";

#define stsAlarmThresholdValueSYNTAX  stsAlarmThresholdValueRANGE
#endif

static CLIMIBENTITY_t stsAlarmThresholdValueENT = {
    OIDvalue(LNstsAlarmThresholdValue, IDstsAlarmThresholdValue)
    NULL,
    "almthrval",
    ARG_ENUM,
    0,
#ifdef stsAlarmThresholdValueSYNTAX
    (void *)stsAlarmThresholdValueSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAlarmThresholdValue",
    "Threshold level to set. When this value is reached, a trap will be sent out. Below are the default values:

CVL - 16
CVP - 16
CS - 1
JITTER - 5 UI (T1)
1 - UI (E1)
TIE (Stratum 1) - 0.503 UI (T1), 0.668 UI (E1) - use 503 and 668 as the integer values. 
MTIE (Stratum 1) - 0.503 UI (T1), 0.668 UI (E1) - use 503 and 668 as the integer values."};

#ifndef stsAlarmThresholdTimeIntervalSYNTAX
CLIUMAPSTRUC_t stsAlarmThresholdTimeIntervalMAP[] = {
{"one-Sec", 1}, {"ten-Sec", 2}, {"hundred-Sec", 3}, {"thousand-Sec", 4}, {"tenThousand-Sec", 5}, {"hundredThousand-Sec", 6}, {"one-hour", 7}, MAPDATA_END};

#define stsAlarmThresholdTimeIntervalSYNTAX  stsAlarmThresholdTimeIntervalMAP
#endif

static CLIMIBENTITY_t stsAlarmThresholdTimeIntervalENT = {
    OIDvalue(LNstsAlarmThresholdTimeInterval, IDstsAlarmThresholdTimeInterval)
    NULL,
    "almthrtmintval",
    ARG_ENUM,
    7,
#ifdef stsAlarmThresholdTimeIntervalSYNTAX
    (void *)stsAlarmThresholdTimeIntervalSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAlarmThresholdTimeInterval",
    "Time interval setting for the corresponding alarm threshold value. Below are the default values.

CVL - ten-Sec
CVP - ten-Sec
CS - one-Hour
JITTER - 1-Sec
TIE (Stratum 1) - one-Hour
MTIE (Stratum 1) - one-Hour
wander - hundred-Sec"};

static pCLIMIBENTITY_t stsAlarmThresholdTableLST[] = {
       &stsAlarmThresholdValueENT, &stsAlarmThresholdTimeIntervalENT};

static pCLIMIBENTITY_t stsAlarmThresholdTableINDX[] = {
  &stsAlarmThresholdCurrentMonENT,         &stsAlarmThresholdTypeENT};

CLIMIBCFGGROUP_t stsAlarmThresholdTableTBL = {
    stsAlarmThresholdTableINDX,
    stsAlarmThresholdTableLST,
    2,
    2};

static MIBDSPENTITY_t rtrv_th_linkDENT[] = {
    {&stsAlarmThresholdCurrentMonENT, "LINK=%s", NULL, DSP_STRING},
    {&stsAlarmThresholdTypeENT, ":TYPE=%s", NULL, DSP_STRING},
    {&stsAlarmThresholdValueENT, ":THRES=%u", NULL, DSP_NUM},
    {&stsAlarmThresholdTimeIntervalENT, ":INTERVAL=%u\n", NULL, DSP_NUM}};

CLIMIBDSPGROUP_t rtrv_th_linkDSP = {
  "rtrv_th_link",
  &stsAlarmThresholdTableTBL,
  4,
  rtrv_th_linkDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSLINKALARMTABLE_H_
#define _TB_STSLINKALARMTABLE_H_

#ifndef stsLinkAlarmSettingLinkTypeSYNTAX
CLIUMAPSTRUC_t stsLinkAlarmSettingLinkTypeMAP[] = {
{"link1", 1}, {"link2", 2}, {"link3", 3}, {"link4", 4}, {"link5", 5}, {"f5F10", 6}, {"plnk", 7}, {"slnk", 8}, MAPDATA_END};

#define stsLinkAlarmSettingLinkTypeSYNTAX  stsLinkAlarmSettingLinkTypeMAP
#endif

static CLIMIBENTITY_t stsLinkAlarmSettingLinkTypeENT = {
    OIDvalue(LNstsLinkAlarmSettingLinkType, IDstsLinkAlarmSettingLinkType)
    NULL,
    "lnkalmsetlnktype",
    ARG_ENUM,
    8,
#ifdef stsLinkAlarmSettingLinkTypeSYNTAX
    (void *)stsLinkAlarmSettingLinkTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkAlarmSettingLinkType",
    "Link type for the alarm setting"};

#ifndef stsLinkAlarmSettingTypeSYNTAX
CLIUMAPSTRUC_t stsLinkAlarmSettingTypeMAP[] = {
{"act", 1}, {"los", 2}, {"aisp", 3}, {"lof", 4}, {"cvl", 5}, {"cvp", 6}, {"cs", 7}, {"jitter", 8}, {"tie", 9}, {"mtie", 10}, MAPDATA_END};

#define stsLinkAlarmSettingTypeSYNTAX  stsLinkAlarmSettingTypeMAP
#endif

static CLIMIBENTITY_t stsLinkAlarmSettingTypeENT = {
    OIDvalue(LNstsLinkAlarmSettingType, IDstsLinkAlarmSettingType)
    NULL,
    "lnkalmsettype",
    ARG_ENUM,
    10,
#ifdef stsLinkAlarmSettingTypeSYNTAX
    (void *)stsLinkAlarmSettingTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkAlarmSettingType",
    "Alarm setting type. 
ACT (NR, NSA) Link in active state
AISP (MN, SA) Alarm indication signal detected - Path. N/A to F2 and F5F10 link types.
LOF (MN, SA) Loss of frame. N/A to F2 and F5F10 link types.
LOS (MN, SA) Loss of signal
T-CS (NA, NSA) Threshold violation for MONTYPE CS. N/A to F2, F5F10, PLNK and SLNK link types.
T-TIE (NA, NSA) Threshold violation for MONTYPE TIE. N/A to F5F10, PLNK and SLNK link types.
T-MTIE (NA, NSA) Threshold violation for MONTYPE MTIE. N/A to F5F10, PLNK and SLNK link types."};

#ifndef stsLinkAlarmSettingSeviritySYNTAX
CLIUMAPSTRUC_t stsLinkAlarmSettingSevirityMAP[] = {
{"cr", 1}, {"mj", 2}, {"mn", 3}, {"na", 4}, {"nr", 5}, MAPDATA_END};

#define stsLinkAlarmSettingSeviritySYNTAX  stsLinkAlarmSettingSevirityMAP
#endif

static CLIMIBENTITY_t stsLinkAlarmSettingSevirityENT = {
    OIDvalue(LNstsLinkAlarmSettingSevirity, IDstsLinkAlarmSettingSevirity)
    NULL,
    "lnkalmsetsev",
    ARG_ENUM,
    5,
#ifdef stsLinkAlarmSettingSeviritySYNTAX
    (void *)stsLinkAlarmSettingSeviritySYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsLinkAlarmSettingSevirity",
    "Threshold level to set. When this value is reached, a trap will be sent out.

cr - Critical alarm
mj - Major alarm
mn - Minor alarm
na - Not alarmed; report as event
nr - Not reported"};

static pCLIMIBENTITY_t stsLinkAlarmTableLST[] = {
  &stsLinkAlarmSettingSevirityENT};

static pCLIMIBENTITY_t stsLinkAlarmTableINDX[] = {
  &stsLinkAlarmSettingLinkTypeENT,       &stsLinkAlarmSettingTypeENT};

CLIMIBCFGGROUP_t stsLinkAlarmTableTBL = {
    stsLinkAlarmTableINDX,
    stsLinkAlarmTableLST,
    2,
    1};

static MIBDSPENTITY_t rtrv_attr_linkDENT[] = {
    {&stsLinkAlarmSettingLinkTypeENT, " %s", NULL, DSP_STRING},
    {&stsLinkAlarmSettingSevirityENT, ",LINK:%s", NULL, DSP_STRING},
    {&stsLinkAlarmSettingTypeENT, ",%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_attr_linkDSP = {
  "rtrv_attr_link",
  &stsLinkAlarmTableTBL,
  3,
  rtrv_attr_linkDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSOSCILLATORALARMTABLE_H_
#define _TB_STSOSCILLATORALARMTABLE_H_

#ifndef stsOscillatorAlarmSettingClockSYNTAX
CLIUMAPSTRUC_t stsOscillatorAlarmSettingClockMAP[] = {
{"oscillator1", 1}, {"oscillator2", 2}, {"all", 3}, MAPDATA_END};

#define stsOscillatorAlarmSettingClockSYNTAX  stsOscillatorAlarmSettingClockMAP
#endif

static CLIMIBENTITY_t stsOscillatorAlarmSettingClockENT = {
    OIDvalue(LNstsOscillatorAlarmSettingClock, IDstsOscillatorAlarmSettingClock)
    NULL,
    "oscalmsetclk",
    ARG_ENUM,
    3,
#ifdef stsOscillatorAlarmSettingClockSYNTAX
    (void *)stsOscillatorAlarmSettingClockSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorAlarmSettingClock",
    "Oscillator for the alarm setting. 
                 all means both, can only apply frngsync as value of stsOscillatorAlarmSettingType."};

#ifndef stsOscillatorAlarmSettingTypeSYNTAX
CLIUMAPSTRUC_t stsOscillatorAlarmSettingTypeMAP[] = {
{"aqrsync", 1}, {"frngsync", 2}, {"hldovrsync", 3}, {"lkdsync", 4}, {"act", 5}, MAPDATA_END};

#define stsOscillatorAlarmSettingTypeSYNTAX  stsOscillatorAlarmSettingTypeMAP
#endif

static CLIMIBENTITY_t stsOscillatorAlarmSettingTypeENT = {
    OIDvalue(LNstsOscillatorAlarmSettingType, IDstsOscillatorAlarmSettingType)
    NULL,
    "oscalmsettype",
    ARG_ENUM,
    5,
#ifdef stsOscillatorAlarmSettingTypeSYNTAX
    (void *)stsOscillatorAlarmSettingTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorAlarmSettingType",
    "Alarm setting type. 
aqrsync (NR, NSA) - Acquire synchronization mode
frngsync (NR, NSA) - Free running synchronization mode.
hldovrsync (MJ, SA) - Holdover synchronization mode
lkdsync (NR, NSA) - Lock synchronization mode
act (NR, NSA) - Unit in active state"};

#ifndef stsOscillatorAlarmSettingSeveritySYNTAX
CLIUMAPSTRUC_t stsOscillatorAlarmSettingSeverityMAP[] = {
{"cr", 1}, {"mj", 2}, {"mn", 3}, {"na", 4}, {"nr", 5}, MAPDATA_END};

#define stsOscillatorAlarmSettingSeveritySYNTAX  stsOscillatorAlarmSettingSeverityMAP
#endif

static CLIMIBENTITY_t stsOscillatorAlarmSettingSeverityENT = {
    OIDvalue(LNstsOscillatorAlarmSettingSeverity, IDstsOscillatorAlarmSettingSeverity)
    NULL,
    "oscalmsetsev",
    ARG_ENUM,
    5,
#ifdef stsOscillatorAlarmSettingSeveritySYNTAX
    (void *)stsOscillatorAlarmSettingSeveritySYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOscillatorAlarmSettingSeverity",
    "Alarm level to set.

cr - Critical alarm
mj - Major alarm.
mn - Minor alarm
na - Not alarmed; report as event
nr - Not reported"};

static pCLIMIBENTITY_t stsOscillatorAlarmTableLST[] = {
&stsOscillatorAlarmSettingSeverityENT};

static pCLIMIBENTITY_t stsOscillatorAlarmTableINDX[] = {
&stsOscillatorAlarmSettingClockENT, &stsOscillatorAlarmSettingTypeENT};

CLIMIBCFGGROUP_t stsOscillatorAlarmTableTBL = {
    stsOscillatorAlarmTableINDX,
    stsOscillatorAlarmTableLST,
    2,
    1};

static MIBDSPENTITY_t rtrv_attr_eqpt_clkDENT[] = {
    {&stsOscillatorAlarmSettingClockENT, " %s", NULL, DSP_STRING},
    {&stsOscillatorAlarmSettingSeverityENT, ",EQPT:%s", NULL, DSP_STRING},
    {&stsOscillatorAlarmSettingTypeENT, ",%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_attr_eqpt_clkDSP = {
  "rtrv_attr_eqpt_clk",
  &stsOscillatorAlarmTableTBL,
  3,
  rtrv_attr_eqpt_clkDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSOSPWRALARMTABLE_H_
#define _TB_STSOSPWRALARMTABLE_H_

#ifndef stssPwrAlarmSettingShelfSYNTAX
CLIUMAPSTRUC_t stssPwrAlarmSettingShelfMAP[] = {
{"main", 1}, {"expansion-1", 2}, {"expansion-2", 3}, {"expansion-3", 4}, {"expansion-4", 5}, MAPDATA_END};

#define stssPwrAlarmSettingShelfSYNTAX  stssPwrAlarmSettingShelfMAP
#endif

static CLIMIBENTITY_t stssPwrAlarmSettingShelfENT = {
    OIDvalue(LNstssPwrAlarmSettingShelf, IDstssPwrAlarmSettingShelf)
    NULL,
    "pwralmsetshf",
    ARG_ENUM,
    5,
#ifdef stssPwrAlarmSettingShelfSYNTAX
    (void *)stssPwrAlarmSettingShelfSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stssPwrAlarmSettingShelf",
    "Shelf number. index value 100 is the main chassis."};

#ifndef stsOsPwrAlarmSettingOsPwrSYNTAX
CLIUMAPSTRUC_t stsOsPwrAlarmSettingOsPwrMAP[] = {
{"os", 1}, {"pwrA", 2}, {"pwrB", 3}, MAPDATA_END};

#define stsOsPwrAlarmSettingOsPwrSYNTAX  stsOsPwrAlarmSettingOsPwrMAP
#endif

static CLIMIBENTITY_t stsOsPwrAlarmSettingOsPwrENT = {
    OIDvalue(LNstsOsPwrAlarmSettingOsPwr, IDstsOsPwrAlarmSettingOsPwr)
    NULL,
    "ospwralmset",
    ARG_ENUM,
    3,
#ifdef stsOsPwrAlarmSettingOsPwrSYNTAX
    (void *)stsOsPwrAlarmSettingOsPwrSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOsPwrAlarmSettingOsPwr",
    "OS and power alarm types for the alarm setting"};

#ifndef stsOsPwrAlarmSettingSeveritySYNTAX
CLIUMAPSTRUC_t stsOsPwrAlarmSettingSeverityMAP[] = {
{"cr", 1}, {"mj", 2}, {"mn", 3}, {"na", 4}, {"nr", 5}, MAPDATA_END};

#define stsOsPwrAlarmSettingSeveritySYNTAX  stsOsPwrAlarmSettingSeverityMAP
#endif

static CLIMIBENTITY_t stsOsPwrAlarmSettingSeverityENT = {
    OIDvalue(LNstsOsPwrAlarmSettingSeverity, IDstsOsPwrAlarmSettingSeverity)
    NULL,
    "ospwralmsetsev",
    ARG_ENUM,
    5,
#ifdef stsOsPwrAlarmSettingSeveritySYNTAX
    (void *)stsOsPwrAlarmSettingSeveritySYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOsPwrAlarmSettingSeverity",
    "Alarm level to set. 

cr - Critical alarm
mj - Major alarm
mn - Minor alarm
na - Not alarmed; report as event
nr - Not reported"};

static pCLIMIBENTITY_t stsOsPwrAlarmTableLST[] = {
 &stsOsPwrAlarmSettingSeverityENT};

static pCLIMIBENTITY_t stsOsPwrAlarmTableINDX[] = {
     &stssPwrAlarmSettingShelfENT,     &stsOsPwrAlarmSettingOsPwrENT};

CLIMIBCFGGROUP_t stsOsPwrAlarmTableTBL = {
    stsOsPwrAlarmTableINDX,
    stsOsPwrAlarmTableLST,
    2,
    1};

static MIBDSPENTITY_t rtrv_attr_eqpt_ospwrDENT[] = {
    {&stssPwrAlarmSettingShelfENT, " %s", NULL, DSP_STRING},
    {&stsOsPwrAlarmSettingSeverityENT, ",EQPT:%s", NULL, DSP_STRING},
    {&stsOsPwrAlarmSettingOsPwrENT, ",%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_attr_eqpt_ospwrDSP = {
  "rtrv_attr_eqpt_ospwr",
  &stsOsPwrAlarmTableTBL,
  3,
  rtrv_attr_eqpt_ospwrDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSOUTPUTALARMTABLE_H_
#define _TB_STSOUTPUTALARMTABLE_H_

#ifndef stsOutputAlarmSettingShelfSYNTAX
CLIUMAPSTRUC_t stsOutputAlarmSettingShelfMAP[] = {
{"main", 1}, {"expansion-1", 2}, {"expansion-2", 3}, {"expansion-3", 4}, {"expansion-4", 5}, MAPDATA_END};

#define stsOutputAlarmSettingShelfSYNTAX  stsOutputAlarmSettingShelfMAP
#endif

static CLIMIBENTITY_t stsOutputAlarmSettingShelfENT = {
    OIDvalue(LNstsOutputAlarmSettingShelf, IDstsOutputAlarmSettingShelf)
    NULL,
    "outalmsetshf",
    ARG_ENUM,
    5,
#ifdef stsOutputAlarmSettingShelfSYNTAX
    (void *)stsOutputAlarmSettingShelfSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputAlarmSettingShelf",
    "Output shelf number. index value 100 is the main chassis."};

#ifndef stsOutputAlarmSettingSlotSYNTAX
static U8_t stsOutputAlarmSettingSlotRANGE[] = "1..16";

#define stsOutputAlarmSettingSlotSYNTAX  stsOutputAlarmSettingSlotRANGE
#endif

static CLIMIBENTITY_t stsOutputAlarmSettingSlotENT = {
    OIDvalue(LNstsOutputAlarmSettingSlot, IDstsOutputAlarmSettingSlot)
    NULL,
    "outalmsetslot",
    ARG_ENUM,
    0,
#ifdef stsOutputAlarmSettingSlotSYNTAX
    (void *)stsOutputAlarmSettingSlotSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputAlarmSettingSlot",
    "Slot number of this shelf. Shelf 0 can only support 8 slots. The rest of shelf can support up to 16 slots."};

#ifndef stsOutputAlarmSettingPortSYNTAX
static U8_t stsOutputAlarmSettingPortRANGE[] = "1..8";

#define stsOutputAlarmSettingPortSYNTAX  stsOutputAlarmSettingPortRANGE
#endif

static CLIMIBENTITY_t stsOutputAlarmSettingPortENT = {
    OIDvalue(LNstsOutputAlarmSettingPort, IDstsOutputAlarmSettingPort)
    NULL,
    "outalmsetport",
    ARG_ENUM,
    0,
#ifdef stsOutputAlarmSettingPortSYNTAX
    (void *)stsOutputAlarmSettingPortSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputAlarmSettingPort",
    "Port number located at this slot."};

#ifndef stsOutputAlarmSettingTypeSYNTAX
CLIUMAPSTRUC_t stsOutputAlarmSettingTypeMAP[] = {
{"lossClock", 1}, {"tiu-act", 2}, {"tiu-los", 3}, {"tiu-t-tie", 4}, {"tiu-t-mtie", 5}, {"tiu-aisp", 6}, {"tiu-lof", 7}, {"tiu-t-cs", 8}, MAPDATA_END};

#define stsOutputAlarmSettingTypeSYNTAX  stsOutputAlarmSettingTypeMAP
#endif

static CLIMIBENTITY_t stsOutputAlarmSettingTypeENT = {
    OIDvalue(LNstsOutputAlarmSettingType, IDstsOutputAlarmSettingType)
    NULL,
    "outalmsettype",
    ARG_ENUM,
    8,
#ifdef stsOutputAlarmSettingTypeSYNTAX
    (void *)stsOutputAlarmSettingTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputAlarmSettingType",
    "Alarm setting type. 
syncclk (MJ, SA) Synchronization channel failure"};

#ifndef stsOutputAlarmSettingSeveritySYNTAX
CLIUMAPSTRUC_t stsOutputAlarmSettingSeverityMAP[] = {
{"cr", 1}, {"mj", 2}, {"mn", 3}, {"na", 4}, {"nr", 5}, MAPDATA_END};

#define stsOutputAlarmSettingSeveritySYNTAX  stsOutputAlarmSettingSeverityMAP
#endif

static CLIMIBENTITY_t stsOutputAlarmSettingSeverityENT = {
    OIDvalue(LNstsOutputAlarmSettingSeverity, IDstsOutputAlarmSettingSeverity)
    NULL,
    "outalmsetsev",
    ARG_ENUM,
    5,
#ifdef stsOutputAlarmSettingSeveritySYNTAX
    (void *)stsOutputAlarmSettingSeveritySYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputAlarmSettingSeverity",
    "Alarm level to set.

cr - Critical alarm
mj - Major alarm
mn - Minor alarm
na - Not alarmed; report as event
nr - Not reported"};

static pCLIMIBENTITY_t stsOutputAlarmTableLST[] = {
&stsOutputAlarmSettingSeverityENT};

static pCLIMIBENTITY_t stsOutputAlarmTableINDX[] = {
   &stsOutputAlarmSettingShelfENT,     &stsOutputAlarmSettingSlotENT,
    &stsOutputAlarmSettingPortENT,     &stsOutputAlarmSettingTypeENT};

CLIMIBCFGGROUP_t stsOutputAlarmTableTBL = {
    stsOutputAlarmTableINDX,
    stsOutputAlarmTableLST,
    4,
    1};

static MIBDSPENTITY_t rtrv_attr_eqpt_outputDENT[] = {
    {&stsOutputAlarmSettingShelfENT, " %s", NULL, DSP_STRING},
    {&stsOutputAlarmSettingSlotENT, ",OUT:%u", NULL, DSP_NUM},
    {&stsOutputAlarmSettingPortENT, "-%u", NULL, DSP_NUM},
    {&stsOutputAlarmSettingSeverityENT, ",EQPT:%s", NULL, DSP_STRING},
    {&stsOutputAlarmSettingTypeENT, ",%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_attr_eqpt_outputDSP = {
  "rtrv_attr_eqpt_output",
  &stsOutputAlarmTableTBL,
  5,
  rtrv_attr_eqpt_outputDENT,
  ""};

static MIBDSPENTITY_t cur_outp_almDENT[] = {
    {&stsOutputAlarmSettingShelfENT, " %s", NULL, DSP_STRING},
    {&stsOutputAlarmSettingSlotENT, ":OUT%u", NULL, DSP_NUM},
    {&stsOutputAlarmSettingPortENT, "-%u", NULL, DSP_NUM},
    {&stsOutputAlarmSettingTypeENT, ":%s", NULL, DSP_STRING},
    {&stsOutputAlarmSettingSeverityENT, "=%s", NULL, DSP_STRING},
    {&stsOutputAlarmSettingSeverityENT, ",%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t cur_outp_almDSP = {
  "cur_outp_alm",
  &stsOutputAlarmTableTBL,
  6,
  cur_outp_almDENT,
  ""};

static MIBDSPENTITY_t cur_outp_evtDENT[] = {
    {&stsOutputAlarmSettingShelfENT, " %s", NULL, DSP_STRING},
    {&stsOutputAlarmSettingSlotENT, ":OUT%u", NULL, DSP_NUM},
    {&stsOutputAlarmSettingPortENT, "-%u", NULL, DSP_NUM},
    {&stsOutputAlarmSettingTypeENT, ":%s", NULL, DSP_STRING},
    {&stsOutputAlarmSettingSeverityENT, "=%s", NULL, DSP_STRING},
    {&stsOutputAlarmSettingSeverityENT, ",%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t cur_outp_evtDSP = {
  "cur_outp_evt",
  &stsOutputAlarmTableTBL,
  6,
  cur_outp_evtDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSOUTPUTPORTSETTINGTABLE_H_
#define _TB_STSOUTPUTPORTSETTINGTABLE_H_

#ifndef stsOutputPortSettingShelfSYNTAX
CLIUMAPSTRUC_t stsOutputPortSettingShelfMAP[] = {
{"main", 1}, {"expansion-1", 2}, {"expansion-2", 3}, {"expansion-3", 4}, {"expansion-4", 5}, MAPDATA_END};

#define stsOutputPortSettingShelfSYNTAX  stsOutputPortSettingShelfMAP
#endif

static CLIMIBENTITY_t stsOutputPortSettingShelfENT = {
    OIDvalue(LNstsOutputPortSettingShelf, IDstsOutputPortSettingShelf)
    NULL,
    "outptsetshf",
    ARG_ENUM,
    5,
#ifdef stsOutputPortSettingShelfSYNTAX
    (void *)stsOutputPortSettingShelfSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputPortSettingShelf",
    "Output shelf number. index value 100 is the main chassis."};

#ifndef stsOutputPortSettingSlotSYNTAX
static U8_t stsOutputPortSettingSlotRANGE[] = "1..16";

#define stsOutputPortSettingSlotSYNTAX  stsOutputPortSettingSlotRANGE
#endif

static CLIMIBENTITY_t stsOutputPortSettingSlotENT = {
    OIDvalue(LNstsOutputPortSettingSlot, IDstsOutputPortSettingSlot)
    NULL,
    "outptsetslot",
    ARG_ENUM,
    0,
#ifdef stsOutputPortSettingSlotSYNTAX
    (void *)stsOutputPortSettingSlotSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputPortSettingSlot",
    "Slot number of this shelf. Shelf 0 can only support 8 slots. The rest of shelf can support up to 16 slots."};

#ifndef stsOutputPortSettingPortSYNTAX
static U8_t stsOutputPortSettingPortRANGE[] = "1..8";

#define stsOutputPortSettingPortSYNTAX  stsOutputPortSettingPortRANGE
#endif

static CLIMIBENTITY_t stsOutputPortSettingPortENT = {
    OIDvalue(LNstsOutputPortSettingPort, IDstsOutputPortSettingPort)
    NULL,
    "outptsetport",
    ARG_ENUM,
    0,
#ifdef stsOutputPortSettingPortSYNTAX
    (void *)stsOutputPortSettingPortSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputPortSettingPort",
    "Port number located at this slot."};

#ifndef stsOutputPortSettingTypeSYNTAX
CLIUMAPSTRUC_t stsOutputPortSettingTypeMAP[] = {
{"t1", 1}, {"e1", 2}, {"cc", 3}, {"sqw", 4}, {"i-rigb", 5}, {"onePps", 6}, {"onePpm", 7}, {"onePph", 8}, {"tiu", 9}, {"f5", 10}, {"f10", 11}, MAPDATA_END};

#define stsOutputPortSettingTypeSYNTAX  stsOutputPortSettingTypeMAP
#endif

static CLIMIBENTITY_t stsOutputPortSettingTypeENT = {
    OIDvalue(LNstsOutputPortSettingType, IDstsOutputPortSettingType)
    NULL,
    "outptsettype",
    ARG_ENUM,
    11,
#ifdef stsOutputPortSettingTypeSYNTAX
    (void *)stsOutputPortSettingTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputPortSettingType",
    "Output port setting type."};

static CLIMIBENTITY_t stsOutputPortSettingNameENT = {
    OIDvalue(LNstsOutputPortSettingName, IDstsOutputPortSettingName)
    NULL,
    "outptsetname",
    ARG_STR,
    0,
#ifdef stsOutputPortSettingNameSYNTAX
    (void *)stsOutputPortSettingNameSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputPortSettingName",
    "Port name to set."};

static pCLIMIBENTITY_t stsOutputPortSettingTableLST[] = {
     &stsOutputPortSettingTypeENT,      &stsOutputPortSettingNameENT};

static pCLIMIBENTITY_t stsOutputPortSettingTableINDX[] = {
    &stsOutputPortSettingShelfENT,      &stsOutputPortSettingSlotENT,
     &stsOutputPortSettingPortENT};

CLIMIBCFGGROUP_t stsOutputPortSettingTableTBL = {
    stsOutputPortSettingTableINDX,
    stsOutputPortSettingTableLST,
    3,
    2};

static MIBDSPENTITY_t view_outp_nameDENT[] = {
    {&stsOutputPortSettingShelfENT, " %s", NULL, DSP_STRING},
    {&stsOutputPortSettingSlotENT, ",OUT:%u", NULL, DSP_NUM},
    {&stsOutputPortSettingPortENT, "-%u", NULL, DSP_NUM},
    {&stsOutputPortSettingTypeENT, ",%s", NULL, DSP_STRING},
    {&stsOutputPortSettingNameENT, ",%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t view_outp_nameDSP = {
  "view_outp_name",
  &stsOutputPortSettingTableTBL,
  5,
  view_outp_nameDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSOUTPUTTIUSETTINGTABLE_H_
#define _TB_STSOUTPUTTIUSETTINGTABLE_H_

#ifndef stsOutputTiuSettingShelfSYNTAX
CLIUMAPSTRUC_t stsOutputTiuSettingShelfMAP[] = {
{"main", 1}, {"expansion-1", 2}, {"expansion-2", 3}, {"expansion-3", 4}, {"expansion-4", 5}, MAPDATA_END};

#define stsOutputTiuSettingShelfSYNTAX  stsOutputTiuSettingShelfMAP
#endif

static CLIMIBENTITY_t stsOutputTiuSettingShelfENT = {
    OIDvalue(LNstsOutputTiuSettingShelf, IDstsOutputTiuSettingShelf)
    NULL,
    "outtiusetshf",
    ARG_ENUM,
    5,
#ifdef stsOutputTiuSettingShelfSYNTAX
    (void *)stsOutputTiuSettingShelfSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputTiuSettingShelf",
    "Output shelf number. index value 100 is the main chassis."};

#ifndef stsOutputTiuSettingSlotSYNTAX
static U8_t stsOutputTiuSettingSlotRANGE[] = "1..16";

#define stsOutputTiuSettingSlotSYNTAX  stsOutputTiuSettingSlotRANGE
#endif

static CLIMIBENTITY_t stsOutputTiuSettingSlotENT = {
    OIDvalue(LNstsOutputTiuSettingSlot, IDstsOutputTiuSettingSlot)
    NULL,
    "outtiusetslot",
    ARG_ENUM,
    0,
#ifdef stsOutputTiuSettingSlotSYNTAX
    (void *)stsOutputTiuSettingSlotSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputTiuSettingSlot",
    "Slot number of this shelf. Shelf 0 can only support 8 slots. The rest of shelf can support up to 16 slots."};

#ifndef stsOutputTiuSettingPortSYNTAX
static U8_t stsOutputTiuSettingPortRANGE[] = "1..8";

#define stsOutputTiuSettingPortSYNTAX  stsOutputTiuSettingPortRANGE
#endif

static CLIMIBENTITY_t stsOutputTiuSettingPortENT = {
    OIDvalue(LNstsOutputTiuSettingPort, IDstsOutputTiuSettingPort)
    NULL,
    "outtiusetport",
    ARG_ENUM,
    0,
#ifdef stsOutputTiuSettingPortSYNTAX
    (void *)stsOutputTiuSettingPortSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputTiuSettingPort",
    "Port number located at this slot."};

#ifndef stsOutputTiuSettingLineCodingSYNTAX
CLIUMAPSTRUC_t stsOutputTiuSettingLineCodingMAP[] = {
{"ami", 1}, {"b8zs", 2}, {"hdb3", 3}, {"other", 4}, MAPDATA_END};

#define stsOutputTiuSettingLineCodingSYNTAX  stsOutputTiuSettingLineCodingMAP
#endif

static CLIMIBENTITY_t stsOutputTiuSettingLineCodingENT = {
    OIDvalue(LNstsOutputTiuSettingLineCoding, IDstsOutputTiuSettingLineCoding)
    NULL,
    "outtiusetlncode",
    ARG_ENUM,
    4,
#ifdef stsOutputTiuSettingLineCodingSYNTAX
    (void *)stsOutputTiuSettingLineCodingSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputTiuSettingLineCoding",
    "Line Coding setting type."};

#ifndef stsOutputTiuSettingFMTSYNTAX
CLIUMAPSTRUC_t stsOutputTiuSettingFMTMAP[] = {
{"t1-d4", 1}, {"t1-esf", 2}, {"j1-d4", 3}, {"j1-esf", 4}, MAPDATA_END};

#define stsOutputTiuSettingFMTSYNTAX  stsOutputTiuSettingFMTMAP
#endif

static CLIMIBENTITY_t stsOutputTiuSettingFMTENT = {
    OIDvalue(LNstsOutputTiuSettingFMT, IDstsOutputTiuSettingFMT)
    NULL,
    "outtiusetfmt",
    ARG_ENUM,
    4,
#ifdef stsOutputTiuSettingFMTSYNTAX
    (void *)stsOutputTiuSettingFMTSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputTiuSettingFMT",
    "FMT to set."};

#ifndef stsOutputTiuSettingTerminationSYNTAX
CLIUMAPSTRUC_t stsOutputTiuSettingTerminationMAP[] = {
{"ohm-75", 1}, {"ohm-100", 2}, {"ohm-110", 3}, {"ohm-120", 4}, MAPDATA_END};

#define stsOutputTiuSettingTerminationSYNTAX  stsOutputTiuSettingTerminationMAP
#endif

static CLIMIBENTITY_t stsOutputTiuSettingTerminationENT = {
    OIDvalue(LNstsOutputTiuSettingTermination, IDstsOutputTiuSettingTermination)
    NULL,
    "outtiusetterm",
    ARG_ENUM,
    4,
#ifdef stsOutputTiuSettingTerminationSYNTAX
    (void *)stsOutputTiuSettingTerminationSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputTiuSettingTermination",
    "Termination to set."};

static pCLIMIBENTITY_t stsOutputTiuSettingTableLST[] = {
&stsOutputTiuSettingLineCodingENT,        &stsOutputTiuSettingFMTENT,
&stsOutputTiuSettingTerminationENT};

static pCLIMIBENTITY_t stsOutputTiuSettingTableINDX[] = {
     &stsOutputTiuSettingShelfENT,       &stsOutputTiuSettingSlotENT,
      &stsOutputTiuSettingPortENT};

CLIMIBCFGGROUP_t stsOutputTiuSettingTableTBL = {
    stsOutputTiuSettingTableINDX,
    stsOutputTiuSettingTableLST,
    3,
    3};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSPERFMONTABLE_H_
#define _TB_STSPERFMONTABLE_H_

#ifndef stsPerfMonInputCurrentMonSYNTAX
CLIUMAPSTRUC_t stsPerfMonInputCurrentMonMAP[] = {
{"link1", 1}, {"link2", 2}, {"link3", 3}, {"link4", 4}, {"link5", 5}, MAPDATA_END};

#define stsPerfMonInputCurrentMonSYNTAX  stsPerfMonInputCurrentMonMAP
#endif

static CLIMIBENTITY_t stsPerfMonInputCurrentMonENT = {
    OIDvalue(LNstsPerfMonInputCurrentMon, IDstsPerfMonInputCurrentMon)
    NULL,
    "perfcurmon",
    ARG_ENUM,
    5,
#ifdef stsPerfMonInputCurrentMonSYNTAX
    (void *)stsPerfMonInputCurrentMonSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonInputCurrentMon",
    "Current monitor number."};

#ifndef stsPerfMonInputMonPeriodSYNTAX
CLIUMAPSTRUC_t stsPerfMonInputMonPeriodMAP[] = {
{"one-hour", 1}, {"one-day", 2}, {"hundred-Sec", 3}, MAPDATA_END};

#define stsPerfMonInputMonPeriodSYNTAX  stsPerfMonInputMonPeriodMAP
#endif

static CLIMIBENTITY_t stsPerfMonInputMonPeriodENT = {
    OIDvalue(LNstsPerfMonInputMonPeriod, IDstsPerfMonInputMonPeriod)
    NULL,
    "perfper",
    ARG_ENUM,
    3,
#ifdef stsPerfMonInputMonPeriodSYNTAX
    (void *)stsPerfMonInputMonPeriodSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonInputMonPeriod",
    "Accumulation period of the performance monitoring. This
may be 100 second, 1-DAY or 1-HR."};

#ifndef stsPerfMonInputMonBucketSYNTAX
static U8_t stsPerfMonInputMonBucketRANGE[] = "0..72";

#define stsPerfMonInputMonBucketSYNTAX  stsPerfMonInputMonBucketRANGE
#endif

static CLIMIBENTITY_t stsPerfMonInputMonBucketENT = {
    OIDvalue(LNstsPerfMonInputMonBucket, IDstsPerfMonInputMonBucket)
    NULL,
    "perfbuc",
    ARG_ENUM,
    0,
#ifdef stsPerfMonInputMonBucketSYNTAX
    (void *)stsPerfMonInputMonBucketSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonInputMonBucket",
    "Number of the register to be retrieved. If stsPerfMonInputMonPeriod is 1-HR,
this may be a value from 0 to 72 (3 days). 0 means the (incomplete)
hour now in progress, 1 means the most recently completed
hour, and so on. If stsPerfMonInputMonPeriod is 1-DAY, this may be a value
from 0 to 7. 0 means the (incomplete) day now in progress;
if a complete day is not available, then the current day's
hourly data are displayed. 1 means the most recent
complete day, 2 the day previous to that, and so on.
REGNO may be a single value or a group. If stsPerfMonInputMonPeriod is 100-Sec, this may be a value
from 0 to 30. 0 means the (incomplete) 100 second period now in progress"};

#ifndef stsPerfMonInputMonTypeSYNTAX
CLIUMAPSTRUC_t stsPerfMonInputMonTypeMAP[] = {
{"cvl", 1}, {"cvp", 2}, {"fcs", 3}, {"esp", 4}, {"jitter", 5}, {"sesp", 6}, {"tie", 7}, {"mtie", 8}, MAPDATA_END};

#define stsPerfMonInputMonTypeSYNTAX  stsPerfMonInputMonTypeMAP
#endif

static CLIMIBENTITY_t stsPerfMonInputMonTypeENT = {
    OIDvalue(LNstsPerfMonInputMonType, IDstsPerfMonInputMonType)
    NULL,
    "perftype",
    ARG_ENUM,
    8,
#ifdef stsPerfMonInputMonTypeSYNTAX
    (void *)stsPerfMonInputMonTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonInputMonType",
    "This object specifies the input monitor type"};

#ifndef stsPerfMonInputMonValueSYNTAX
static U8_t stsPerfMonInputMonValueRANGE[] = "-1..1000000";

#define stsPerfMonInputMonValueSYNTAX  stsPerfMonInputMonValueRANGE
#endif

static CLIMIBENTITY_t stsPerfMonInputMonValueENT = {
    OIDvalue(LNstsPerfMonInputMonValue, IDstsPerfMonInputMonValue)
    NULL,
    "perfvalue",
    ARG_ENUM,
    0,
#ifdef stsPerfMonInputMonValueSYNTAX
    (void *)stsPerfMonInputMonValueSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonInputMonValue",
    "-1 means not applicable to current monitor type value in stsPerfMonInputClockType."};

#ifndef stsPerfMonInputClockTypeSYNTAX
CLIUMAPSTRUC_t stsPerfMonInputClockTypeMAP[] = {
{"unknown", 1}, {"t1", 2}, {"e1", 3}, {"cc", 4}, {"f2", 5}, {"f5", 6}, {"f10", 7}, MAPDATA_END};

#define stsPerfMonInputClockTypeSYNTAX  stsPerfMonInputClockTypeMAP
#endif

static CLIMIBENTITY_t stsPerfMonInputClockTypeENT = {
    OIDvalue(LNstsPerfMonInputClockType, IDstsPerfMonInputClockType)
    NULL,
    "perfclktype",
    ARG_ENUM,
    7,
#ifdef stsPerfMonInputClockTypeSYNTAX
    (void *)stsPerfMonInputClockTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsPerfMonInputClockType",
    "This object specifies the input monitor clock type"};

static pCLIMIBENTITY_t stsPerfMonTableLST[] = {
                             NULL};

static pCLIMIBENTITY_t stsPerfMonTableINDX[] = {
    &stsPerfMonInputCurrentMonENT,      &stsPerfMonInputMonPeriodENT,
     &stsPerfMonInputMonBucketENT,        &stsPerfMonInputMonTypeENT};

CLIMIBCFGGROUP_t stsPerfMonTableTBL = {
    stsPerfMonTableINDX,
    stsPerfMonTableLST,
    4,
    0};

static MIBDSPENTITY_t rtrv_pm_linkDENT[] = {
    {&stsPerfMonInputCurrentMonENT, " %s", NULL, DSP_STRING},
    {&stsPerfMonInputMonPeriodENT, ":%s", NULL, DSP_STRING},
    {&stsPerfMonInputClockTypeENT, ":%s", NULL, DSP_STRING},
    {&stsPerfMonInputMonBucketENT, ":Bucket=%u", NULL, DSP_NUM},
    {&stsPerfMonInputMonTypeENT, ",%s", NULL, DSP_STRING},
    {&stsPerfMonInputMonValueENT, "=%u\n", NULL, DSP_NUM}};

CLIMIBDSPGROUP_t rtrv_pm_linkDSP = {
  "rtrv_pm_link",
  &stsPerfMonTableTBL,
  6,
  rtrv_pm_linkDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSOUTPUTCARDTABLE_H_
#define _TB_STSOUTPUTCARDTABLE_H_

#ifndef stsOutputShelfSYNTAX
CLIUMAPSTRUC_t stsOutputShelfMAP[] = {
{"main", 1}, {"expansion-1", 2}, {"expansion-2", 3}, {"expansion-3", 4}, {"expansion-4", 5}, MAPDATA_END};

#define stsOutputShelfSYNTAX  stsOutputShelfMAP
#endif

static CLIMIBENTITY_t stsOutputShelfENT = {
    OIDvalue(LNstsOutputShelf, IDstsOutputShelf)
    NULL,
    "outshf",
    ARG_ENUM,
    5,
#ifdef stsOutputShelfSYNTAX
    (void *)stsOutputShelfSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputShelf",
    "Output shelf number. Shelf 0 is the main chassis."};

#ifndef stsOutputSlotSYNTAX
static U8_t stsOutputSlotRANGE[] = "1..16";

#define stsOutputSlotSYNTAX  stsOutputSlotRANGE
#endif

static CLIMIBENTITY_t stsOutputSlotENT = {
    OIDvalue(LNstsOutputSlot, IDstsOutputSlot)
    NULL,
    "outslot",
    ARG_ENUM,
    0,
#ifdef stsOutputSlotSYNTAX
    (void *)stsOutputSlotSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputSlot",
    "Slot number of this shelf. Shelf 0 can only support 8 slots. The rest of shelf can support up to 16 slots."};

#ifndef stsOutputRedStateSYNTAX
CLIUMAPSTRUC_t stsOutputRedStateMAP[] = {
{"active", 1}, {"standby", 2}, MAPDATA_END};

#define stsOutputRedStateSYNTAX  stsOutputRedStateMAP
#endif

static CLIMIBENTITY_t stsOutputRedStateENT = {
    OIDvalue(LNstsOutputRedState, IDstsOutputRedState)
    NULL,
    "outredst",
    ARG_ENUM,
    2,
#ifdef stsOutputRedStateSYNTAX
    (void *)stsOutputRedStateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputRedState",
    "Output card current card status. if there is no redundancy, both pair will show active."};

#ifndef stsOutputRedSwitchStateSYNTAX
CLIUMAPSTRUC_t stsOutputRedSwitchStateMAP[] = {
{"noAction", 1}, {"switchCard", 2}, MAPDATA_END};

#define stsOutputRedSwitchStateSYNTAX  stsOutputRedSwitchStateMAP
#endif

static CLIMIBENTITY_t stsOutputRedSwitchStateENT = {
    OIDvalue(LNstsOutputRedSwitchState, IDstsOutputRedSwitchState)
    NULL,
    "outredswst",
    ARG_ENUM,
    2,
#ifdef stsOutputRedSwitchStateSYNTAX
    (void *)stsOutputRedSwitchStateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputRedSwitchState",
    "Provisioning the switch over action for the redundancy pair. Only slot pair 1 and 2, 3 and 4,5and 6, 7 and 8, 9 and 10, 11 and 12, 13 and 14, 15 and 16 can be redundancy pair."};

#ifndef stsOutputCRCCHKSYNTAX
CLIUMAPSTRUC_t stsOutputCRCCHKMAP[] = {
{"enable", 1}, {"disable", 2}, MAPDATA_END};

#define stsOutputCRCCHKSYNTAX  stsOutputCRCCHKMAP
#endif

static CLIMIBENTITY_t stsOutputCRCCHKENT = {
    OIDvalue(LNstsOutputCRCCHK, IDstsOutputCRCCHK)
    NULL,
    "outcrcchk",
    ARG_ENUM,
    2,
#ifdef stsOutputCRCCHKSYNTAX
    (void *)stsOutputCRCCHKSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputCRCCHK",
    "Provisioning output card CRC check status"};

#ifndef stsOutputCASSYNTAX
CLIUMAPSTRUC_t stsOutputCASMAP[] = {
{"enable", 1}, {"disable", 2}, MAPDATA_END};

#define stsOutputCASSYNTAX  stsOutputCASMAP
#endif

static CLIMIBENTITY_t stsOutputCASENT = {
    OIDvalue(LNstsOutputCAS, IDstsOutputCAS)
    NULL,
    "outcas",
    ARG_ENUM,
    2,
#ifdef stsOutputCASSYNTAX
    (void *)stsOutputCASSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputCAS",
    "Provisioning output card CAS enable/disable status"};

#ifndef stsOutputSSMSYNTAX
CLIUMAPSTRUC_t stsOutputSSMMAP[] = {
{"sa4", 1}, {"sa5", 2}, {"sa6", 3}, {"sa7", 4}, {"sa8", 5}, MAPDATA_END};

#define stsOutputSSMSYNTAX  stsOutputSSMMAP
#endif

static CLIMIBENTITY_t stsOutputSSMENT = {
    OIDvalue(LNstsOutputSSM, IDstsOutputSSM)
    NULL,
    "outssm",
    ARG_ENUM,
    5,
#ifdef stsOutputSSMSYNTAX
    (void *)stsOutputSSMSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputSSM",
    "Provisioning output card SSM."};

#ifndef stsOutputSSMValueSYNTAX
CLIUMAPSTRUC_t stsOutputSSMValueMAP[] = {
{"prs", 1}, {"stu", 2}, {"st2", 3}, {"st3", 4}, {"sic", 5}, {"duc", 6}, MAPDATA_END};

#define stsOutputSSMValueSYNTAX  stsOutputSSMValueMAP
#endif

static CLIMIBENTITY_t stsOutputSSMValueENT = {
    OIDvalue(LNstsOutputSSMValue, IDstsOutputSSMValue)
    NULL,
    "outssmval",
    ARG_ENUM,
    6,
#ifdef stsOutputSSMValueSYNTAX
    (void *)stsOutputSSMValueSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputSSMValue",
    "This object allows users to specify this SSM value.
prs - primary reference (stratum 1)
stu - sync traceability unknown
st2 - stratum 2 reference
st3 - stratum 3 reference
sic - sonet minimum clock traceable
dus - don't use for sync"};

static CLIMIBENTITY_t stsOutputTimingFormatENT = {
    OIDvalue(LNstsOutputTimingFormat, IDstsOutputTimingFormat)
    NULL,
    "outtimefmt",
    ARG_ENUM,
    0,
#ifdef stsOutputTimingFormatSYNTAX
    (void *)stsOutputTimingFormatSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsOutputTimingFormat",
    "Provisioning output card timing format, default is UTC format."};

static pCLIMIBENTITY_t stsOutputCardTableLST[] = {
      &stsOutputRedSwitchStateENT,
              &stsOutputCRCCHKENT,                  &stsOutputCASENT,
                 &stsOutputSSMENT,             &stsOutputSSMValueENT,
        &stsOutputTimingFormatENT};

static pCLIMIBENTITY_t stsOutputCardTableINDX[] = {
               &stsOutputShelfENT,                 &stsOutputSlotENT};

CLIMIBCFGGROUP_t stsOutputCardTableTBL = {
    stsOutputCardTableINDX,
    stsOutputCardTableLST,
    2,
    6};

static MIBDSPENTITY_t rtrv_outp_ssmDENT[] = {
    {&stsOutputShelfENT, " %s", NULL, DSP_STRING},
    {&stsOutputSlotENT, ":%s", NULL, DSP_STRING},
    {&stsOutputSSMENT, ":%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_outp_ssmDSP = {
  "rtrv_outp_ssm",
  &stsOutputCardTableTBL,
  3,
  rtrv_outp_ssmDENT,
  ""};

static MIBDSPENTITY_t rtrv_outp_redDENT[] = {
    {&stsOutputShelfENT, " %s", NULL, DSP_STRING},
    {&stsOutputSlotENT, ":Slot=%s", NULL, DSP_STRING},
    {&stsOutputRedStateENT, ":State=%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_outp_redDSP = {
  "rtrv_outp_red",
  &stsOutputCardTableTBL,
  3,
  rtrv_outp_redDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSTERMINALTABLE_H_
#define _TB_STSTERMINALTABLE_H_

#ifndef stsTerminalIndexSYNTAX
CLIUMAPSTRUC_t stsTerminalIndexMAP[] = {
{"terminal1", 1}, MAPDATA_END};

#define stsTerminalIndexSYNTAX  stsTerminalIndexMAP
#endif

static CLIMIBENTITY_t stsTerminalIndexENT = {
    OIDvalue(LNstsTerminalIndex, IDstsTerminalIndex)
    NULL,
    "termidx",
    ARG_ENUM,
    1,
#ifdef stsTerminalIndexSYNTAX
    (void *)stsTerminalIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTerminalIndex",
    "Terminal index number."};

#ifndef stsTerminalBaudrateSYNTAX
CLIUMAPSTRUC_t stsTerminalBaudrateMAP[] = {
{"b1200", 1}, {"b2400", 2}, {"b4800", 3}, {"b9600", 4}, {"b19200", 5}, {"b38400", 6}, MAPDATA_END};

#define stsTerminalBaudrateSYNTAX  stsTerminalBaudrateMAP
#endif

static CLIMIBENTITY_t stsTerminalBaudrateENT = {
    OIDvalue(LNstsTerminalBaudrate, IDstsTerminalBaudrate)
    NULL,
    "termbr",
    ARG_ENUM,
    6,
#ifdef stsTerminalBaudrateSYNTAX
    (void *)stsTerminalBaudrateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTerminalBaudrate",
    "Specify the baud rate."};

#ifndef stsTerminalDataBitsSYNTAX
CLIUMAPSTRUC_t stsTerminalDataBitsMAP[] = {
{"seven", 1}, {"eight", 2}, MAPDATA_END};

#define stsTerminalDataBitsSYNTAX  stsTerminalDataBitsMAP
#endif

static CLIMIBENTITY_t stsTerminalDataBitsENT = {
    OIDvalue(LNstsTerminalDataBits, IDstsTerminalDataBits)
    NULL,
    "termdb",
    ARG_ENUM,
    2,
#ifdef stsTerminalDataBitsSYNTAX
    (void *)stsTerminalDataBitsSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTerminalDataBits",
    "Specify the data bits."};

#ifndef stsTerminalParitySYNTAX
CLIUMAPSTRUC_t stsTerminalParityMAP[] = {
{"even", 1}, {"odd", 2}, {"none", 3}, MAPDATA_END};

#define stsTerminalParitySYNTAX  stsTerminalParityMAP
#endif

static CLIMIBENTITY_t stsTerminalParityENT = {
    OIDvalue(LNstsTerminalParity, IDstsTerminalParity)
    NULL,
    "termpar",
    ARG_ENUM,
    3,
#ifdef stsTerminalParitySYNTAX
    (void *)stsTerminalParitySYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTerminalParity",
    "Specify the parity."};

#ifndef stsTerminalStopBitsSYNTAX
CLIUMAPSTRUC_t stsTerminalStopBitsMAP[] = {
{"one", 1}, {"two", 2}, MAPDATA_END};

#define stsTerminalStopBitsSYNTAX  stsTerminalStopBitsMAP
#endif

static CLIMIBENTITY_t stsTerminalStopBitsENT = {
    OIDvalue(LNstsTerminalStopBits, IDstsTerminalStopBits)
    NULL,
    "termsb",
    ARG_ENUM,
    2,
#ifdef stsTerminalStopBitsSYNTAX
    (void *)stsTerminalStopBitsSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTerminalStopBits",
    "Specify the stop bits."};

static pCLIMIBENTITY_t stsTerminalTableLST[] = {
          &stsTerminalBaudrateENT,           &stsTerminalDataBitsENT,
            &stsTerminalParityENT,           &stsTerminalStopBitsENT};

static pCLIMIBENTITY_t stsTerminalTableINDX[] = {
             &stsTerminalIndexENT};

CLIMIBCFGGROUP_t stsTerminalTableTBL = {
    stsTerminalTableINDX,
    stsTerminalTableLST,
    1,
    4};

static MIBDSPENTITY_t view_termDENT[] = {
    {&stsTerminalBaudrateENT, " Local Terminal (CID)::BAUD=%u", NULL, DSP_NUM},
    {&stsTerminalDataBitsENT, ",DBITS= %u", NULL, DSP_NUM},
    {&stsTerminalParityENT, ",PAR= %s", NULL, DSP_STRING},
    {&stsTerminalStopBitsENT, ":,SBITS=%u\n", NULL, DSP_NUM}};

CLIMIBDSPGROUP_t view_termDSP = {
  "view_term",
  &stsTerminalTableTBL,
  4,
  view_termDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSALARMSTATUSTABLE_H_
#define _TB_STSALARMSTATUSTABLE_H_

#ifndef stsAlarmSourceSYNTAX
CLIUMAPSTRUC_t stsAlarmSourceMAP[] = {
{"clk1", 1}, {"clk2", 2}, {"pwrA", 3}, {"pwrB", 4}, {"os", 5}, {"f10", 6}, {"link1", 7}, {"link2", 8}, {"link3", 9}, {"link4", 10}, {"link5", 11}, {"esr1", 12}, {"esr2", 13}, MAPDATA_END};

#define stsAlarmSourceSYNTAX  stsAlarmSourceMAP
#endif

static CLIMIBENTITY_t stsAlarmSourceENT = {
    OIDvalue(LNstsAlarmSource, IDstsAlarmSource)
    NULL,
    "almsrc",
    ARG_ENUM,
    13,
#ifdef stsAlarmSourceSYNTAX
    (void *)stsAlarmSourceSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAlarmSource",
    "Source of current alarm status"};

#ifndef stsAlarmSourceTypeSYNTAX
CLIUMAPSTRUC_t stsAlarmSourceTypeMAP[] = {
{"eqpt", 1}, {"link", 2}, {"com", 3}, MAPDATA_END};

#define stsAlarmSourceTypeSYNTAX  stsAlarmSourceTypeMAP
#endif

static CLIMIBENTITY_t stsAlarmSourceTypeENT = {
    OIDvalue(LNstsAlarmSourceType, IDstsAlarmSourceType)
    NULL,
    "almsrctype",
    ARG_ENUM,
    3,
#ifdef stsAlarmSourceTypeSYNTAX
    (void *)stsAlarmSourceTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAlarmSourceType",
    "Alarm Source type."};

#ifndef stsAlarmSeveritySYNTAX
CLIUMAPSTRUC_t stsAlarmSeverityMAP[] = {
{"cr", 1}, {"mj", 2}, {"mn", 3}, {"na", 4}, {"nr", 5}, {"cl", 6}, MAPDATA_END};

#define stsAlarmSeveritySYNTAX  stsAlarmSeverityMAP
#endif

static CLIMIBENTITY_t stsAlarmSeverityENT = {
    OIDvalue(LNstsAlarmSeverity, IDstsAlarmSeverity)
    NULL,
    "almsev",
    ARG_ENUM,
    6,
#ifdef stsAlarmSeveritySYNTAX
    (void *)stsAlarmSeveritySYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAlarmSeverity",
    "Current alarm level.

cr - Critical alarm
mj - Major alarm
mn - Minor alarm
na - Not alarmed; report as event
nr - Not reported
cl - No alarm or alarm cleared. Also used by trap."};

#ifndef stsAlarmSrvEffSYNTAX
CLIUMAPSTRUC_t stsAlarmSrvEffMAP[] = {
{"sa", 1}, {"nsa", 2}, MAPDATA_END};

#define stsAlarmSrvEffSYNTAX  stsAlarmSrvEffMAP
#endif

static CLIMIBENTITY_t stsAlarmSrvEffENT = {
    OIDvalue(LNstsAlarmSrvEff, IDstsAlarmSrvEff)
    NULL,
    "almsrveff",
    ARG_ENUM,
    2,
#ifdef stsAlarmSrvEffSYNTAX
    (void *)stsAlarmSrvEffSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAlarmSrvEff",
    "Has the value service effect or non-service effect.

SA Service-affecting condition. Immediate action required. The IMC defines major alarms as service-affecting.
NSA Nonservice-affecting condition. The IMC defines minor alarms and events as nonservice-affecting."};

static pCLIMIBENTITY_t stsAlarmStatusTableLST[] = {
                             NULL};

static pCLIMIBENTITY_t stsAlarmStatusTableINDX[] = {
               &stsAlarmSourceENT};

CLIMIBCFGGROUP_t stsAlarmStatusTableTBL = {
    stsAlarmStatusTableINDX,
    stsAlarmStatusTableLST,
    1,
    0};

static MIBDSPENTITY_t cur_clp_almDENT[] = {
    {&stsAlarmSourceENT, " %s", NULL, DSP_STRING},
    {&stsAlarmSourceTypeENT, ":%s", NULL, DSP_STRING},
    {&stsAlarmSeverityENT, "= %s", NULL, DSP_STRING},
    {&stsAlarmSrvEffENT, ",%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t cur_clp_almDSP = {
  "cur_clp_alm",
  &stsAlarmStatusTableTBL,
  4,
  cur_clp_almDENT,
  ""};

static MIBDSPENTITY_t cur_clp_evtDENT[] = {
    {&stsAlarmSourceENT, " %s", NULL, DSP_STRING},
    {&stsAlarmSourceTypeENT, ":%s", NULL, DSP_STRING},
    {&stsAlarmSeverityENT, "= %s", NULL, DSP_STRING},
    {&stsAlarmSrvEffENT, ",%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t cur_clp_evtDSP = {
  "cur_clp_evt",
  &stsAlarmStatusTableTBL,
  4,
  cur_clp_evtDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSEMMSTATUSTABLE_H_
#define _TB_STSEMMSTATUSTABLE_H_

#ifndef stsEmmShelfSYNTAX
CLIUMAPSTRUC_t stsEmmShelfMAP[] = {
{"main", 1}, {"expansion-1", 2}, {"expansion-2", 3}, {"expansion-3", 4}, {"expansion-4", 5}, MAPDATA_END};

#define stsEmmShelfSYNTAX  stsEmmShelfMAP
#endif

static CLIMIBENTITY_t stsEmmShelfENT = {
    OIDvalue(LNstsEmmShelf, IDstsEmmShelf)
    NULL,
    "emmshf",
    ARG_ENUM,
    5,
#ifdef stsEmmShelfSYNTAX
    (void *)stsEmmShelfSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsEmmShelf",
    "Output shelf number. Shelf 0 is the main chassis."};

#ifndef stsEmmSensorTypeSYNTAX
CLIUMAPSTRUC_t stsEmmSensorTypeMAP[] = {
{"temp-ambient", 1}, {"volt5", 2}, {"volt12", 3}, {"volt48-A", 4}, {"volt48-B", 5}, MAPDATA_END};

#define stsEmmSensorTypeSYNTAX  stsEmmSensorTypeMAP
#endif

static CLIMIBENTITY_t stsEmmSensorTypeENT = {
    OIDvalue(LNstsEmmSensorType, IDstsEmmSensorType)
    NULL,
    "emmsentype",
    ARG_ENUM,
    5,
#ifdef stsEmmSensorTypeSYNTAX
    (void *)stsEmmSensorTypeSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsEmmSensorType",
    "Sensor type."};

#ifndef stsEmmAlarmStatusSYNTAX
CLIUMAPSTRUC_t stsEmmAlarmStatusMAP[] = {
{"cr", 1}, {"mj", 2}, {"mn", 3}, {"na", 4}, {"nr", 5}, {"cl", 6}, MAPDATA_END};

#define stsEmmAlarmStatusSYNTAX  stsEmmAlarmStatusMAP
#endif

static CLIMIBENTITY_t stsEmmAlarmStatusENT = {
    OIDvalue(LNstsEmmAlarmStatus, IDstsEmmAlarmStatus)
    NULL,
    "emmalmst",
    ARG_ENUM,
    6,
#ifdef stsEmmAlarmStatusSYNTAX
    (void *)stsEmmAlarmStatusSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsEmmAlarmStatus",
    "Current alarm level.

cr - Critical alarm
mj - Major alarm
mn - Minor alarm
na - Not alarmed; report as event
nr - Not reported
cl - No alarm or alarm cleared; also used in trap"};

#ifndef stsEmmSensorReadingSYNTAX
static U8_t stsEmmSensorReadingRANGE[] = "-50..100";

#define stsEmmSensorReadingSYNTAX  stsEmmSensorReadingRANGE
#endif

static CLIMIBENTITY_t stsEmmSensorReadingENT = {
    OIDvalue(LNstsEmmSensorReading, IDstsEmmSensorReading)
    NULL,
    "emmsenread",
    ARG_ENUM,
    0,
#ifdef stsEmmSensorReadingSYNTAX
    (void *)stsEmmSensorReadingSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsEmmSensorReading",
    "  The current Emm sensor reading."};

#ifndef stsEmmMajorAlmHiThreshSYNTAX
static U8_t stsEmmMajorAlmHiThreshRANGE[] = "-50..100";

#define stsEmmMajorAlmHiThreshSYNTAX  stsEmmMajorAlmHiThreshRANGE
#endif

static CLIMIBENTITY_t stsEmmMajorAlmHiThreshENT = {
    OIDvalue(LNstsEmmMajorAlmHiThresh, IDstsEmmMajorAlmHiThresh)
    NULL,
    "emmmajalmhi",
    ARG_ENUM,
    0,
#ifdef stsEmmMajorAlmHiThreshSYNTAX
    (void *)stsEmmMajorAlmHiThreshSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsEmmMajorAlmHiThresh",
    "  The setting value of the majoralarm high threshold of this sensor. 
                   Not applicable to voltage-mvolt."};

#ifndef stsEmmMajorAlmLoThreshSYNTAX
static U8_t stsEmmMajorAlmLoThreshRANGE[] = "-50..100";

#define stsEmmMajorAlmLoThreshSYNTAX  stsEmmMajorAlmLoThreshRANGE
#endif

static CLIMIBENTITY_t stsEmmMajorAlmLoThreshENT = {
    OIDvalue(LNstsEmmMajorAlmLoThresh, IDstsEmmMajorAlmLoThresh)
    NULL,
    "emmmajalmlo",
    ARG_ENUM,
    0,
#ifdef stsEmmMajorAlmLoThreshSYNTAX
    (void *)stsEmmMajorAlmLoThreshSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsEmmMajorAlmLoThresh",
    "  The setting value of the major alarm low threshold of this sensor.
                  Not applicable to voltage-mvolt."};

#ifndef stsEmmMinorAlmHiThreshSYNTAX
static U8_t stsEmmMinorAlmHiThreshRANGE[] = "-50..100";

#define stsEmmMinorAlmHiThreshSYNTAX  stsEmmMinorAlmHiThreshRANGE
#endif

static CLIMIBENTITY_t stsEmmMinorAlmHiThreshENT = {
    OIDvalue(LNstsEmmMinorAlmHiThresh, IDstsEmmMinorAlmHiThresh)
    NULL,
    "emmminalmhi",
    ARG_ENUM,
    0,
#ifdef stsEmmMinorAlmHiThreshSYNTAX
    (void *)stsEmmMinorAlmHiThreshSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsEmmMinorAlmHiThresh",
    "  The setting value of the minor alarm high threshold of this sensor.
                   Not applicable to voltage-mvolt."};

#ifndef stsEmmMinorAlmLoThreshSYNTAX
static U8_t stsEmmMinorAlmLoThreshRANGE[] = "-50..100";

#define stsEmmMinorAlmLoThreshSYNTAX  stsEmmMinorAlmLoThreshRANGE
#endif

static CLIMIBENTITY_t stsEmmMinorAlmLoThreshENT = {
    OIDvalue(LNstsEmmMinorAlmLoThresh, IDstsEmmMinorAlmLoThresh)
    NULL,
    "emmminalmlo",
    ARG_ENUM,
    0,
#ifdef stsEmmMinorAlmLoThreshSYNTAX
    (void *)stsEmmMinorAlmLoThreshSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsEmmMinorAlmLoThresh",
    "  The setting value of the minor alarm low threshold of this sensor.
                  Not applicable to voltage-mvolt."};

static pCLIMIBENTITY_t stsEmmStatusTableLST[] = {
       &stsEmmMajorAlmHiThreshENT,        &stsEmmMajorAlmLoThreshENT,
       &stsEmmMinorAlmHiThreshENT,        &stsEmmMinorAlmLoThreshENT};

static pCLIMIBENTITY_t stsEmmStatusTableINDX[] = {
                  &stsEmmShelfENT,              &stsEmmSensorTypeENT};

CLIMIBCFGGROUP_t stsEmmStatusTableTBL = {
    stsEmmStatusTableINDX,
    stsEmmStatusTableLST,
    2,
    4};

static MIBDSPENTITY_t rtrv_emmDENT[] = {
    {&stsEmmShelfENT, " %s", NULL, DSP_STRING},
    {&stsEmmSensorTypeENT, ": %s", NULL, DSP_STRING},
    {&stsEmmSensorReadingENT, ": %s", NULL, DSP_STRING},
    {&stsEmmAlarmStatusENT, ": %s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_emmDSP = {
  "rtrv_emm",
  &stsEmmStatusTableTBL,
  4,
  rtrv_emmDENT,
  ""};

static MIBDSPENTITY_t rtrv_emm_thDENT[] = {
    {&stsEmmShelfENT, " %s", NULL, DSP_STRING},
    {&stsEmmSensorTypeENT, ": %s", NULL, DSP_STRING},
    {&stsEmmMajorAlmHiThreshENT, ",MJ-HI-TH=", NULL, DSP_TABLE},
    {&stsEmmMajorAlmLoThreshENT, ",MJ-LO-TH=", NULL, DSP_TABLE},
    {&stsEmmMinorAlmHiThreshENT, ",MN-HI-TH=", NULL, DSP_TABLE},
    {&stsEmmMinorAlmLoThreshENT, ",MN-LO-TH=", NULL, DSP_TABLE}};

CLIMIBDSPGROUP_t rtrv_emm_thDSP = {
  "rtrv_emm_th",
  &stsEmmStatusTableTBL,
  6,
  rtrv_emm_thDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSANTTABLE_H_
#define _TB_STSANTTABLE_H_

#ifndef stsAntIndexSYNTAX
static U8_t stsAntIndexRANGE[] = "1..2";

#define stsAntIndexSYNTAX  stsAntIndexRANGE
#endif

static CLIMIBENTITY_t stsAntIndexENT = {
    OIDvalue(LNstsAntIndex, IDstsAntIndex)
    NULL,
    "antidx",
    ARG_ENUM,
    0,
#ifdef stsAntIndexSYNTAX
    (void *)stsAntIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAntIndex",
    "Ant module index"};

#ifndef stsAntStatusSYNTAX
CLIUMAPSTRUC_t stsAntStatusMAP[] = {
{"active", 1}, {"standby", 2}, {"failure", 3}, {"absent", 4}, MAPDATA_END};

#define stsAntStatusSYNTAX  stsAntStatusMAP
#endif

static CLIMIBENTITY_t stsAntStatusENT = {
    OIDvalue(LNstsAntStatus, IDstsAntStatus)
    NULL,
    "antst",
    ARG_ENUM,
    4,
#ifdef stsAntStatusSYNTAX
    (void *)stsAntStatusSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAntStatus",
    "Ant current status"};

#ifndef stsAntSwitchStateSYNTAX
CLIUMAPSTRUC_t stsAntSwitchStateMAP[] = {
{"noAction", 1}, {"switchAnt", 2}, MAPDATA_END};

#define stsAntSwitchStateSYNTAX  stsAntSwitchStateMAP
#endif

static CLIMIBENTITY_t stsAntSwitchStateENT = {
    OIDvalue(LNstsAntSwitchState, IDstsAntSwitchState)
    NULL,
    "antswst",
    ARG_ENUM,
    2,
#ifdef stsAntSwitchStateSYNTAX
    (void *)stsAntSwitchStateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsAntSwitchState",
    "Provisioning this Ant pair to switch over. The other Ant (if existed) will change its active/standby state accordingly."};

static pCLIMIBENTITY_t stsAntTableLST[] = {
            &stsAntSwitchStateENT};

static pCLIMIBENTITY_t stsAntTableINDX[] = {
                  &stsAntIndexENT};

CLIMIBCFGGROUP_t stsAntTableTBL = {
    stsAntTableINDX,
    stsAntTableLST,
    1,
    1};

static MIBDSPENTITY_t rtrv_antDENT[] = {
    {&stsAntIndexENT, " %s", NULL, DSP_STRING},
    {&stsAntStatusENT, ": %s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_antDSP = {
  "rtrv_ant",
  &stsAntTableTBL,
  2,
  rtrv_antDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSTOPDESTTABLE_H_
#define _TB_STSTOPDESTTABLE_H_

#ifndef stsTopDestNumberSYNTAX
static U8_t stsTopDestNumberRANGE[] = "1..28";

#define stsTopDestNumberSYNTAX  stsTopDestNumberRANGE
#endif

static CLIMIBENTITY_t stsTopDestNumberENT = {
    OIDvalue(LNstsTopDestNumber, IDstsTopDestNumber)
    NULL,
    "topdestnum",
    ARG_ENUM,
    0,
#ifdef stsTopDestNumberSYNTAX
    (void *)stsTopDestNumberSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopDestNumber",
    "A device ID of this TOP entry in the table."};

static CLIMIBENTITY_t stsTopDestIpENT = {
    OIDvalue(LNstsTopDestIp, IDstsTopDestIp)
    NULL,
    "topdestip",
    ARG_STR,
    0,
#ifdef stsTopDestIpSYNTAX
    (void *)stsTopDestIpSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopDestIp",
    "IP address of the destination."};

#ifndef stsTopDestUdpPortSYNTAX
static U8_t stsTopDestUdpPortRANGE[] = "0..65535";

#define stsTopDestUdpPortSYNTAX  stsTopDestUdpPortRANGE
#endif

static CLIMIBENTITY_t stsTopDestUdpPortENT = {
    OIDvalue(LNstsTopDestUdpPort, IDstsTopDestUdpPort)
    NULL,
    "topudpport",
    ARG_ENUM,
    0,
#ifdef stsTopDestUdpPortSYNTAX
    (void *)stsTopDestUdpPortSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopDestUdpPort",
    "UDP port number of the destination."};

#ifndef stsTopDestVlanIdSYNTAX
static U8_t stsTopDestVlanIdRANGE[] = "1..32";

#define stsTopDestVlanIdSYNTAX  stsTopDestVlanIdRANGE
#endif

static CLIMIBENTITY_t stsTopDestVlanIdENT = {
    OIDvalue(LNstsTopDestVlanId, IDstsTopDestVlanId)
    NULL,
    "topvlanid",
    ARG_ENUM,
    0,
#ifdef stsTopDestVlanIdSYNTAX
    (void *)stsTopDestVlanIdSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTopDestVlanId",
    "VLAN ID for this TOP"};

static pCLIMIBENTITY_t stsTopDestTableLST[] = {
                 &stsTopDestIpENT,             &stsTopDestUdpPortENT,
             &stsTopDestVlanIdENT};

static pCLIMIBENTITY_t stsTopDestTableINDX[] = {
             &stsTopDestNumberENT};

CLIMIBCFGGROUP_t stsTopDestTableTBL = {
    stsTopDestTableINDX,
    stsTopDestTableLST,
    1,
    3};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSTRAPREGTABLE_H_
#define _TB_STSTRAPREGTABLE_H_

#ifndef stsTrapRegMgrNumberSYNTAX
static U8_t stsTrapRegMgrNumberRANGE[] = "1..10";

#define stsTrapRegMgrNumberSYNTAX  stsTrapRegMgrNumberRANGE
#endif

static CLIMIBENTITY_t stsTrapRegMgrNumberENT = {
    OIDvalue(LNstsTrapRegMgrNumber, IDstsTrapRegMgrNumber)
    NULL,
    "trapmgrnum",
    ARG_ENUM,
    0,
#ifdef stsTrapRegMgrNumberSYNTAX
    (void *)stsTrapRegMgrNumberSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTrapRegMgrNumber",
    "A unique number used as an index to the table."};

static CLIMIBENTITY_t stsTrapRegMgrIpENT = {
    OIDvalue(LNstsTrapRegMgrIp, IDstsTrapRegMgrIp)
    NULL,
    "trapmgrip",
    ARG_STR,
    0,
#ifdef stsTrapRegMgrIpSYNTAX
    (void *)stsTrapRegMgrIpSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTrapRegMgrIp",
    "IP address of the trap receiver manager."};

#ifndef stsTrapRegMgrUdpPortSYNTAX
static U8_t stsTrapRegMgrUdpPortRANGE[] = "0..65535";

#define stsTrapRegMgrUdpPortSYNTAX  stsTrapRegMgrUdpPortRANGE
#endif

static CLIMIBENTITY_t stsTrapRegMgrUdpPortENT = {
    OIDvalue(LNstsTrapRegMgrUdpPort, IDstsTrapRegMgrUdpPort)
    NULL,
    "trapmgrudpport",
    ARG_ENUM,
    0,
#ifdef stsTrapRegMgrUdpPortSYNTAX
    (void *)stsTrapRegMgrUdpPortSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTrapRegMgrUdpPort",
    "UDP port number on which the manager receives traps from an agent."};

static CLIMIBENTITY_t stsTrapRegCommunityStrENT = {
    OIDvalue(LNstsTrapRegCommunityStr, IDstsTrapRegCommunityStr)
    NULL,
    "trapcommstr",
    ARG_STR,
    0,
#ifdef stsTrapRegCommunityStrSYNTAX
    (void *)stsTrapRegCommunityStrSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTrapRegCommunityStr",
    "This object is the community string(up to 20 characters)to be 
              carried by the traps sent to the registered manager. Each mamanger 
              can have different community string."};

#ifndef stsTrapRegStateSYNTAX
CLIUMAPSTRUC_t stsTrapRegStateMAP[] = {
{"enabled", 1}, {"disabled", 2}, MAPDATA_END};

#define stsTrapRegStateSYNTAX  stsTrapRegStateMAP
#endif

static CLIMIBENTITY_t stsTrapRegStateENT = {
    OIDvalue(LNstsTrapRegState, IDstsTrapRegState)
    NULL,
    "trapst",
    ARG_ENUM,
    2,
#ifdef stsTrapRegStateSYNTAX
    (void *)stsTrapRegStateSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsTrapRegState",
    "The state of the Trap IP. Trap will not sent to this trap manager if disabled"};

static pCLIMIBENTITY_t stsTrapRegTableLST[] = {
              &stsTrapRegMgrIpENT,          &stsTrapRegMgrUdpPortENT,
       &stsTrapRegCommunityStrENT,               &stsTrapRegStateENT};

static pCLIMIBENTITY_t stsTrapRegTableINDX[] = {
          &stsTrapRegMgrNumberENT};

CLIMIBCFGGROUP_t stsTrapRegTableTBL = {
    stsTrapRegTableINDX,
    stsTrapRegTableLST,
    1,
    4};

static MIBDSPENTITY_t rtrv_trap_regDENT[] = {
    {&stsTrapRegMgrNumberENT, " %s", NULL, DSP_STRING},
    {&stsTrapRegMgrIpENT, ":IPADDR=%s", NULL, DSP_STRING},
    {&stsTrapRegMgrUdpPortENT, ",UDP=%u", NULL, DSP_NUM},
    {&stsTrapRegCommunityStrENT, ",COMMUNITY=%s", NULL, DSP_STRING},
    {&stsTrapRegStateENT, ",STATE=%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_trap_regDSP = {
  "rtrv_trap_reg",
  &stsTrapRegTableTBL,
  5,
  rtrv_trap_regDENT,
  ""};

#endif
/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _TB_STSCLOCKPRITABLE_H_
#define _TB_STSCLOCKPRITABLE_H_

#ifndef stsClockPriIndexSYNTAX
CLIUMAPSTRUC_t stsClockPriIndexMAP[] = {
{"esr", 1}, {"input", 2}, {"prs", 3}, MAPDATA_END};

#define stsClockPriIndexSYNTAX  stsClockPriIndexMAP
#endif

static CLIMIBENTITY_t stsClockPriIndexENT = {
    OIDvalue(LNstsClockPriIndex, IDstsClockPriIndex)
    NULL,
    "clkpri",
    ARG_ENUM,
    3,
#ifdef stsClockPriIndexSYNTAX
    (void *)stsClockPriIndexSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockPriIndex",
    "To provision the clock input under protected mode.
external signal receiver - this is the default mode.
input - users can choose it for giving the high priority to input 1 or input2 as the clock input under protected mode."};

#ifndef stsClockPriSettingSYNTAX
CLIUMAPSTRUC_t stsClockPriSettingMAP[] = {
{"priority-1", 1}, {"priority-2", 2}, {"priority-3", 3}, MAPDATA_END};

#define stsClockPriSettingSYNTAX  stsClockPriSettingMAP
#endif

static CLIMIBENTITY_t stsClockPriSettingENT = {
    OIDvalue(LNstsClockPriSetting, IDstsClockPriSetting)
    NULL,
    "clkpriset",
    ARG_ENUM,
    3,
#ifdef stsClockPriSettingSYNTAX
    (void *)stsClockPriSettingSYNTAX,
#else
    NULL,
#endif
    NULL,
    "stsClockPriSetting",
    "1 means highest priority, 3 means lowest priority"};

static pCLIMIBENTITY_t stsClockPriTableLST[] = {
           &stsClockPriSettingENT};

static pCLIMIBENTITY_t stsClockPriTableINDX[] = {
             &stsClockPriIndexENT};

CLIMIBCFGGROUP_t stsClockPriTableTBL = {
    stsClockPriTableINDX,
    stsClockPriTableLST,
    1,
    1};

static MIBDSPENTITY_t rtrv_clockinput_priorityDENT[] = {
    {&stsClockPriIndexENT, " %s", NULL, DSP_STRING},
    {&stsClockPriSettingENT, ":%s\n", NULL, DSP_STRING}};

CLIMIBDSPGROUP_t rtrv_clockinput_priorityDSP = {
  "rtrv_clockinput_priority",
  &stsClockPriTableTBL,
  2,
  rtrv_clockinput_priorityDENT,
  ""};

#endif
