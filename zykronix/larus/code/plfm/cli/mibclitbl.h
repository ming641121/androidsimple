/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#ifndef _CLISNMPTBL_H_
#define _CLISNMPTBL_H_

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
#include "cli_objs.h"

#define MAPDATA_END    {NULL, 0}

#include "mibsynx.h"

#define OIDvalue(name, value) { name , (SR_UINT32 *) value },

extern ObjectInfo OidList[];

#include "tb_LARUS-STS.h"

static pCLIMIBCFGGROUP_t stsSystemcliTL[] = {
    &stsSystemScalarTBL};

CLICFGTBL_t stsSystemcliCMTB = {
   "stsSystemcli",
    stsSystemcliTL,
    1};

static CLIMIBAPIOBJ_t set_sidOPT[] = {
    {                                 &stsSystemIdENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_sidAPI = {
    "set_sid",
    set_sidOPT,
    ""};

static CLIMIBAPIOBJ_t set_mgmtipOPT[] = {
    {                       &stsSystemMgmtIpOptionENT, ' '},
    {                      &stsSystemMgmtIpAddressENT, ' '},
    {                   &stsSystemMgmtIpSubnetMaskENT, ' '},
    {               &stsSystemMgmtIpGatewayAddressENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_mgmtipAPI = {
    "set_mgmtip",
    set_mgmtipOPT,
    ""};

static CLIMIBAPIOBJ_t set_ntpipOPT[] = {
    {                        &stsSystemNtpIpOptionENT, ' '},
    {                       &stsSystemNtpIpAddressENT, ' '},
    {                    &stsSystemNtpIpSubnetMaskENT, ' '},
    {                &stsSystemNtpIpGatewayAddressENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_ntpipAPI = {
    "set_ntpip",
    set_ntpipOPT,
    ""};

static CLIMIBAPIOBJ_t set_datOPT[] = {
    {                               &stsSystemDateENT, ' '},
    {                               &stsSystemTimeENT, ' '},
    {                           &stsSystemTimeZoneENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_datAPI = {
    "set_dat",
    set_datOPT,
    ""};

static CLIMIBAPIOBJ_t set_aco_comOPT[] = {
    {                       &stsSystemAlarmAcoModeENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_aco_comAPI = {
    "set_aco_com",
    set_aco_comOPT,
    ""};

static CLIMIBAPIOBJ_t opr_aco_allOPT[] = {
    {                     &stsSystemAlarmAcoCutOffENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t opr_aco_allAPI = {
    "opr_aco_all",
    opr_aco_allOPT,
    ""};

static CLIMIBAPIOBJ_t ed_alarmescalating_timeOPT[] = {
    {                &stsSystemAlarmEscalatingTimeENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_alarmescalating_timeAPI = {
    "ed_alarmescalating_time",
    ed_alarmescalating_timeOPT,
    ""};

static CLIMIBAPIOBJ_t set_syssw_upgradeOPT[] = {
    {                    &stsSystemUpgradeSWActionENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_syssw_upgradeAPI = {
    "set_syssw_upgrade",
    set_syssw_upgradeOPT,
    ""};

static CLIMIBAPIOBJ_t reset_systemOPT[] = {
    {                              &stsSystemResetENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t reset_systemAPI = {
    "reset_system",
    reset_systemOPT,
    ""};

static pCLIMIBCFGGROUP_t stsClockInputcliTL[] = {
    &stsClockInputScalarTBL};

CLICFGTBL_t stsClockInputcliCMTB = {
   "stsClockInputcli",
    stsClockInputcliTL,
    1};

static CLIMIBAPIOBJ_t ed_clockinput_modeOPT[] = {
    {                       &stsClockInputOperModeENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_clockinput_modeAPI = {
    "ed_clockinput_mode",
    ed_clockinput_modeOPT,
    ""};

static CLIMIBAPIOBJ_t ed_clockinput_manualselectOPT[] = {
    {                   &stsClockInputManualSelectENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_clockinput_manualselectAPI = {
    "ed_clockinput_manualselect",
    ed_clockinput_manualselectOPT,
    ""};

static pCLIMIBCFGGROUP_t stsClockInputSSMcliTL[] = {
    &stsClockInputSSMScalarTBL,
    &stsClockInputSSMTableTBL};

CLICFGTBL_t stsClockInputSSMcliCMTB = {
   "stsClockInputSSMcli",
    stsClockInputSSMcliTL,
    2};

static pCLIMIBCFGGROUP_t stsTopcliTL[] = {
    &stsTopScalarTBL};

CLICFGTBL_t stsTopcliCMTB = {
   "stsTopcli",
    stsTopcliTL,
    1};

static CLIMIBAPIOBJ_t set_topipOPT[] = {
    {                        &stsTopSourceIpOptionENT, ' '},
    {                       &stsTopSourceIpAddressENT, ' '},
    {                    &stsTopSourceIpSubnetMaskENT, ' '},
    {                &stsTopSourceIpGatewayAddressENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_topipAPI = {
    "set_topip",
    set_topipOPT,
    ""};

static pCLIMIBCFGGROUP_t stsPerfMonManualcliTL[] = {
    &stsPerfMonManualScalarTBL};

CLICFGTBL_t stsPerfMonManualcliCMTB = {
   "stsPerfMonManualcli",
    stsPerfMonManualcliTL,
    1};

static CLIMIBAPIOBJ_t sched_pmrept_linkOPT[] = {
    {                 &stsPerfMonManualInputSelectENT, ' '},
    {           &stsPerfMonManualInputScanIntervalENT, ' '},
    {              &stsPerfMonManualInputNumReportENT, ' '},
    {         &stsPerfMonManualInputNumObservationENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t sched_pmrept_linkAPI = {
    "sched_pmrept_link",
    sched_pmrept_linkOPT,
    ""};

static CLIMIBAPIOBJ_t abort_smu_man_testOPT[] = {
    {              &stsPerfMonManualInputAbortTestENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t abort_smu_man_testAPI = {
    "abort_smu_man_test",
    abort_smu_man_testOPT,
    ""};

static pCLIMIBCFGGROUP_t stsClockInputStatuscliTL[] = {
    &stsClockInputStatusTableTBL};

CLICFGTBL_t stsClockInputStatuscliCMTB = {
   "stsClockInputStatuscli",
    stsClockInputStatuscliTL,
    1};

static pCLIMIBCFGGROUP_t stsCardcliTL[] = {
    &stsCardTableTBL};

CLICFGTBL_t stsCardcliCMTB = {
   "stsCardcli",
    stsCardcliTL,
    1};

static CLIMIBAPIOBJ_t reset_cardOPT[] = {
    {                                &stsCardShelfENT, ' '},
    {                                &stsCardIndexENT, ' '},
    {                                &stsCardResetENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t reset_cardAPI = {
    "reset_card",
    reset_cardOPT,
    ""};

static pCLIMIBCFGGROUP_t stsOscillatorcliTL[] = {
    &stsOscillatorTableTBL};

CLICFGTBL_t stsOscillatorcliCMTB = {
   "stsOscillatorcli",
    stsOscillatorcliTL,
    1};

static CLIMIBAPIOBJ_t sw_dx_oscOPT[] = {
    {                          &stsOscillatorIndexENT, ' '},
    {                    &stsOscillatorSwitchStateENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t sw_dx_oscAPI = {
    "sw_dx_osc",
    sw_dx_oscOPT,
    ""};

static pCLIMIBCFGGROUP_t stsGPScliTL[] = {
    &stsGpsTableTBL};

CLICFGTBL_t stsGPScliCMTB = {
   "stsGPScli",
    stsGPScliTL,
    1};

static pCLIMIBCFGGROUP_t stsLinkcliTL[] = {
    &stsLinkTableTBL};

CLICFGTBL_t stsLinkcliCMTB = {
   "stsLinkcli",
    stsLinkcliTL,
    1};

static CLIMIBAPIOBJ_t set_linkOPT[] = {
    {                                &stsLinkIndexENT, ' '},
    {                                   &stsLinkIdENT, ' '},
    {                                 &stsLinkTypeENT, ' '},
    {                                  &stsLinkFMTENT, ' '},
    {                             &stsLinkLineCodeENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_linkAPI = {
    "set_link",
    set_linkOPT,
    ""};

static CLIMIBAPIOBJ_t set_scan_channelOPT[] = {
    {                                &stsLinkIndexENT, ' '},
    {                               &stsLinkEnableENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_scan_channelAPI = {
    "set_scan_channel",
    set_scan_channelOPT,
    ""};

static CLIMIBAPIOBJ_t ed_linkOPT[] = {
    {                                &stsLinkIndexENT, ' '},
    {                               &stsLinkEnableENT, ' '},
    {                             &stsLinkLineCodeENT, ' '},
    {                                  &stsLinkFMTENT, ' '},
    {                          &stsLinkTerminationENT, ' '},
    {                                &stsLinkMFCHKENT, ' '},
    {                                   &stsLinkIdENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_linkAPI = {
    "ed_link",
    ed_linkOPT,
    ""};

static pCLIMIBCFGGROUP_t stsShelfcliTL[] = {
    &stsShelfTableTBL};

CLICFGTBL_t stsShelfcliCMTB = {
   "stsShelfcli",
    stsShelfcliTL,
    1};

static CLIMIBAPIOBJ_t reset_shelfOPT[] = {
    {                               &stsShelfIndexENT, ' '},
    {                               &stsShelfResetENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t reset_shelfAPI = {
    "reset_shelf",
    reset_shelfOPT,
    ""};

static pCLIMIBCFGGROUP_t stsAlarmThresholdcliTL[] = {
    &stsAlarmThresholdTableTBL};

CLICFGTBL_t stsAlarmThresholdcliCMTB = {
   "stsAlarmThresholdcli",
    stsAlarmThresholdcliTL,
    1};

static CLIMIBAPIOBJ_t set_smu_thresOPT[] = {
    {                 &stsAlarmThresholdCurrentMonENT, ' '},
    {                       &stsAlarmThresholdTypeENT, ' '},
    {                      &stsAlarmThresholdValueENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_smu_thresAPI = {
    "set_smu_thres",
    set_smu_thresOPT,
    ""};

static CLIMIBAPIOBJ_t set_th_linkOPT[] = {
    {                 &stsAlarmThresholdCurrentMonENT, ' '},
    {                       &stsAlarmThresholdTypeENT, ' '},
    {                      &stsAlarmThresholdValueENT, ' '},
    {               &stsAlarmThresholdTimeIntervalENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_th_linkAPI = {
    "set_th_link",
    set_th_linkOPT,
    ""};

static pCLIMIBCFGGROUP_t stsLinkAlarmSettingcliTL[] = {
    &stsLinkAlarmTableTBL};

CLICFGTBL_t stsLinkAlarmSettingcliCMTB = {
   "stsLinkAlarmSettingcli",
    stsLinkAlarmSettingcliTL,
    1};

static CLIMIBAPIOBJ_t set_attr_linkOPT[] = {
    {                 &stsLinkAlarmSettingLinkTypeENT, ' '},
    {                     &stsLinkAlarmSettingTypeENT, ' '},
    {                 &stsLinkAlarmSettingSevirityENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_attr_linkAPI = {
    "set_attr_link",
    set_attr_linkOPT,
    ""};

static pCLIMIBCFGGROUP_t stsOscillatorAlarmSettingcliTL[] = {
    &stsOscillatorAlarmTableTBL};

CLICFGTBL_t stsOscillatorAlarmSettingcliCMTB = {
   "stsOscillatorAlarmSettingcli",
    stsOscillatorAlarmSettingcliTL,
    1};

static CLIMIBAPIOBJ_t set_attr_eqpt_clkOPT[] = {
    {              &stsOscillatorAlarmSettingClockENT, ' '},
    {               &stsOscillatorAlarmSettingTypeENT, ' '},
    {           &stsOscillatorAlarmSettingSeverityENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_attr_eqpt_clkAPI = {
    "set_attr_eqpt_clk",
    set_attr_eqpt_clkOPT,
    ""};

static pCLIMIBCFGGROUP_t stsOsPwrAlarmSettingcliTL[] = {
    &stsOsPwrAlarmTableTBL};

CLICFGTBL_t stsOsPwrAlarmSettingcliCMTB = {
   "stsOsPwrAlarmSettingcli",
    stsOsPwrAlarmSettingcliTL,
    1};

static CLIMIBAPIOBJ_t set_attr_eqpt_ospwrOPT[] = {
    {                    &stssPwrAlarmSettingShelfENT, ' '},
    {                   &stsOsPwrAlarmSettingOsPwrENT, ' '},
    {                &stsOsPwrAlarmSettingSeverityENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_attr_eqpt_ospwrAPI = {
    "set_attr_eqpt_ospwr",
    set_attr_eqpt_ospwrOPT,
    ""};

static pCLIMIBCFGGROUP_t stsOutputAlarmSettingcliTL[] = {
    &stsOutputAlarmTableTBL};

CLICFGTBL_t stsOutputAlarmSettingcliCMTB = {
   "stsOutputAlarmSettingcli",
    stsOutputAlarmSettingcliTL,
    1};

static CLIMIBAPIOBJ_t set_attr_eqpt_outputOPT[] = {
    {                  &stsOutputAlarmSettingShelfENT, ' '},
    {                   &stsOutputAlarmSettingSlotENT, ' '},
    {                   &stsOutputAlarmSettingPortENT, ' '},
    {                   &stsOutputAlarmSettingTypeENT, ' '},
    {               &stsOutputAlarmSettingSeverityENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_attr_eqpt_outputAPI = {
    "set_attr_eqpt_output",
    set_attr_eqpt_outputOPT,
    ""};

static pCLIMIBCFGGROUP_t stsOutputPortSettingcliTL[] = {
    &stsOutputPortSettingTableTBL};

CLICFGTBL_t stsOutputPortSettingcliCMTB = {
   "stsOutputPortSettingcli",
    stsOutputPortSettingcliTL,
    1};

static CLIMIBAPIOBJ_t set_outp_nameOPT[] = {
    {                   &stsOutputPortSettingShelfENT, ' '},
    {                    &stsOutputPortSettingSlotENT, ' '},
    {                    &stsOutputPortSettingPortENT, ' '},
    {                    &stsOutputPortSettingTypeENT, ' '},
    {                    &stsOutputPortSettingNameENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t set_outp_nameAPI = {
    "set_outp_name",
    set_outp_nameOPT,
    ""};

static pCLIMIBCFGGROUP_t stsOutputTiuSettingcliTL[] = {
    &stsOutputTiuSettingTableTBL};

CLICFGTBL_t stsOutputTiuSettingcliCMTB = {
   "stsOutputTiuSettingcli",
    stsOutputTiuSettingcliTL,
    1};

static pCLIMIBCFGGROUP_t stsPerfMoncliTL[] = {
    &stsPerfMonTableTBL};

CLICFGTBL_t stsPerfMoncliCMTB = {
   "stsPerfMoncli",
    stsPerfMoncliTL,
    1};

static pCLIMIBCFGGROUP_t stsOutputCardcliTL[] = {
    &stsOutputCardTableTBL};

CLICFGTBL_t stsOutputCardcliCMTB = {
   "stsOutputCardcli",
    stsOutputCardcliTL,
    1};

static CLIMIBAPIOBJ_t ed_outp_ssmOPT[] = {
    {                              &stsOutputShelfENT, ' '},
    {                               &stsOutputSlotENT, ' '},
    {                           &stsOutputSSMValueENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_outp_ssmAPI = {
    "ed_outp_ssm",
    ed_outp_ssmOPT,
    ""};

static CLIMIBAPIOBJ_t ed_outp_redOPT[] = {
    {                              &stsOutputShelfENT, ' '},
    {                               &stsOutputSlotENT, ' '},
    {                     &stsOutputRedSwitchStateENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_outp_redAPI = {
    "ed_outp_red",
    ed_outp_redOPT,
    ""};

static pCLIMIBCFGGROUP_t stsTerminalcliTL[] = {
    &stsTerminalTableTBL};

CLICFGTBL_t stsTerminalcliCMTB = {
   "stsTerminalcli",
    stsTerminalcliTL,
    1};

static CLIMIBAPIOBJ_t ed_termOPT[] = {
    {                            &stsTerminalIndexENT, ' '},
    {                         &stsTerminalBaudrateENT, ' '},
    {                         &stsTerminalDataBitsENT, ' '},
    {                           &stsTerminalParityENT, ' '},
    {                         &stsTerminalStopBitsENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_termAPI = {
    "ed_term",
    ed_termOPT,
    ""};

static pCLIMIBCFGGROUP_t stsAlarmStatuscliTL[] = {
    &stsAlarmStatusTableTBL};

CLICFGTBL_t stsAlarmStatuscliCMTB = {
   "stsAlarmStatuscli",
    stsAlarmStatuscliTL,
    1};

static pCLIMIBCFGGROUP_t stsEmmStatuscliTL[] = {
    &stsEmmStatusTableTBL};

CLICFGTBL_t stsEmmStatuscliCMTB = {
   "stsEmmStatuscli",
    stsEmmStatuscliTL,
    1};

static CLIMIBAPIOBJ_t ed_emm_thOPT[] = {
    {                                 &stsEmmShelfENT, ' '},
    {                            &stsEmmSensorTypeENT, ' '},
    {                      &stsEmmMajorAlmHiThreshENT, ' '},
    {                      &stsEmmMajorAlmLoThreshENT, ' '},
    {                      &stsEmmMinorAlmHiThreshENT, ' '},
    {                      &stsEmmMinorAlmLoThreshENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_emm_thAPI = {
    "ed_emm_th",
    ed_emm_thOPT,
    ""};

static pCLIMIBCFGGROUP_t stsAntcliTL[] = {
    &stsAntTableTBL};

CLICFGTBL_t stsAntcliCMTB = {
   "stsAntcli",
    stsAntcliTL,
    1};

static CLIMIBAPIOBJ_t sw_antOPT[] = {
    {                                 &stsAntIndexENT, ' '},
    {                           &stsAntSwitchStateENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t sw_antAPI = {
    "sw_ant",
    sw_antOPT,
    ""};

static pCLIMIBCFGGROUP_t stsTopDestcliTL[] = {
    &stsTopDestTableTBL};

CLICFGTBL_t stsTopDestcliCMTB = {
   "stsTopDestcli",
    stsTopDestcliTL,
    1};

static pCLIMIBCFGGROUP_t stsTrapRegcliTL[] = {
    &stsTrapRegTableTBL};

CLICFGTBL_t stsTrapRegcliCMTB = {
   "stsTrapRegcli",
    stsTrapRegcliTL,
    1};

static CLIMIBAPIOBJ_t ed_trap_regOPT[] = {
    {                         &stsTrapRegMgrNumberENT, ' '},
    {                             &stsTrapRegMgrIpENT, ' '},
    {                        &stsTrapRegMgrUdpPortENT, ' '},
    {                      &stsTrapRegCommunityStrENT, ' '},
    {                             &stsTrapRegStateENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_trap_regAPI = {
    "ed_trap_reg",
    ed_trap_regOPT,
    ""};

static pCLIMIBCFGGROUP_t stsClockPricliTL[] = {
    &stsClockPriTableTBL};

CLICFGTBL_t stsClockPricliCMTB = {
   "stsClockPricli",
    stsClockPricliTL,
    1};

static CLIMIBAPIOBJ_t ed_clockinput_priorityOPT[] = {
    {                            &stsClockPriIndexENT, ' '},
    {                          &stsClockPriSettingENT, ' '},
    {                                            NULL,   0}
};

CLIMIBAPIGROUP_t ed_clockinput_priorityAPI = {
    "ed_clockinput_priority",
    ed_clockinput_priorityOPT,
    ""};



#endif