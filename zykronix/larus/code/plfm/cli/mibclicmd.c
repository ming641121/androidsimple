/*
 *
 * Copyright (C) 2006 by Ahoya Networks Inc.
 *
 */

#include "mibclitbl.h"

extern STATUS_t MibCnf(U8_t *, pCLICFGTBL_t, pCLIMIBAPIGROUP_t);
extern STATUS_t MibApiCnf(U8_t *, pCLIMIBAPIGROUP_t, pCLICFGTBL_t);
extern STATUS_t MibApiDup(U8_t *, pCLIMIBAPIGROUP_t, pCLICFGTBL_t);
extern STATUS_t MibShow(U32_t, U8_t *, pCLIMIBDSPGROUP_t);
extern STATUS_t MibShowFlt(U8_t *, pCLIMIBAPIGROUP_t, pCLICFGTBL_t, pCLIMIBDSPGROUP_t);


/* Name:         stsSystemcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsSystemScalar
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsSystemcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsSystemcliCMTB;

    return(MibCnf(cmdArg, &stsSystemcliCMTB, NULL));
}


/* Name:         rtrv_mgmtip(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_mgmtip(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_mgmtipDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_mgmtipDSP));
}


/* Name:         rtrv_ntpip(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_ntpip(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_ntpipDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_ntpipDSP));
}


/* Name:         rtrv_dat(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_dat(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_datDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_datDSP));
}


/* Name:         rtrv_sysintg_alarm(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_sysintg_alarm(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_sysintg_alarmDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_sysintg_alarmDSP));
}


/* Name:         rtrv_aco_led(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_aco_led(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_aco_ledDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_aco_ledDSP));
}


/* Name:         rtrv_alarmescalating_time(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_alarmescalating_time(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_alarmescalating_timeDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_alarmescalating_timeDSP));
}


/* Name:         rtrv_system_led(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_system_led(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_system_ledDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_system_ledDSP));
}


/* Name:         rtrv_syssw_version(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_syssw_version(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_syssw_versionDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_syssw_versionDSP));
}


/* Name:         rtrv_sysservice_status(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_sysservice_status(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_sysservice_statusDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_sysservice_statusDSP));
}


/* Name:         set_sid(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_sid
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_sid(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsSystemcliCMTB;
    extern CLIMIBAPIGROUP_t  set_sidAPI;

    return(MibApiCnf(cmdArg, &set_sidAPI, &stsSystemcliCMTB));
}


/* Name:         set_mgmtip(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_mgmtip
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_mgmtip(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsSystemcliCMTB;
    extern CLIMIBAPIGROUP_t  set_mgmtipAPI;

    return(MibApiCnf(cmdArg, &set_mgmtipAPI, &stsSystemcliCMTB));
}


/* Name:         set_ntpip(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_ntpip
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_ntpip(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsSystemcliCMTB;
    extern CLIMIBAPIGROUP_t  set_ntpipAPI;

    return(MibApiCnf(cmdArg, &set_ntpipAPI, &stsSystemcliCMTB));
}


/* Name:         set_dat(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_dat
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_dat(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsSystemcliCMTB;
    extern CLIMIBAPIGROUP_t  set_datAPI;

    return(MibApiCnf(cmdArg, &set_datAPI, &stsSystemcliCMTB));
}


/* Name:         set_aco_com(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_aco_com
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_aco_com(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsSystemcliCMTB;
    extern CLIMIBAPIGROUP_t  set_aco_comAPI;

    return(MibApiCnf(cmdArg, &set_aco_comAPI, &stsSystemcliCMTB));
}


/* Name:         opr_aco_all(U8_t *cmdArg)
 *
 * Description:  Configure the mib opr_aco_all
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
opr_aco_all(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsSystemcliCMTB;
    extern CLIMIBAPIGROUP_t  opr_aco_allAPI;

    return(MibApiCnf(cmdArg, &opr_aco_allAPI, &stsSystemcliCMTB));
}


/* Name:         ed_alarmescalating_time(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_alarmescalating_time
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_alarmescalating_time(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsSystemcliCMTB;
    extern CLIMIBAPIGROUP_t  ed_alarmescalating_timeAPI;

    return(MibApiCnf(cmdArg, &ed_alarmescalating_timeAPI, &stsSystemcliCMTB));
}


/* Name:         set_syssw_upgrade(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_syssw_upgrade
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_syssw_upgrade(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsSystemcliCMTB;
    extern CLIMIBAPIGROUP_t  set_syssw_upgradeAPI;

    return(MibApiCnf(cmdArg, &set_syssw_upgradeAPI, &stsSystemcliCMTB));
}


/* Name:         reset_system(U8_t *cmdArg)
 *
 * Description:  Configure the mib reset_system
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
reset_system(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsSystemcliCMTB;
    extern CLIMIBAPIGROUP_t  reset_systemAPI;

    return(MibApiCnf(cmdArg, &reset_systemAPI, &stsSystemcliCMTB));
}


/* Name:         stsClockInputcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsClockInputScalar
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsClockInputcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsClockInputcliCMTB;

    return(MibCnf(cmdArg, &stsClockInputcliCMTB, NULL));
}


/* Name:         rtrv_clockinput_mode(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_clockinput_mode(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_clockinput_modeDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_clockinput_modeDSP));
}


/* Name:         rtrv_clockinput_manualselect(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_clockinput_manualselect(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_clockinput_manualselectDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_clockinput_manualselectDSP));
}


/* Name:         ed_clockinput_mode(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_clockinput_mode
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_clockinput_mode(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsClockInputcliCMTB;
    extern CLIMIBAPIGROUP_t  ed_clockinput_modeAPI;

    return(MibApiCnf(cmdArg, &ed_clockinput_modeAPI, &stsClockInputcliCMTB));
}


/* Name:         ed_clockinput_manualselect(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_clockinput_manualselect
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_clockinput_manualselect(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsClockInputcliCMTB;
    extern CLIMIBAPIGROUP_t  ed_clockinput_manualselectAPI;

    return(MibApiCnf(cmdArg, &ed_clockinput_manualselectAPI, &stsClockInputcliCMTB));
}


/* Name:         stsClockInputSSMcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsClockInputSSMScalar
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsClockInputSSMcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsClockInputSSMcliCMTB;

    return(MibCnf(cmdArg, &stsClockInputSSMcliCMTB, NULL));
}


/* Name:         stsTopcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsTopScalar
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsTopcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsTopcliCMTB;

    return(MibCnf(cmdArg, &stsTopcliCMTB, NULL));
}


/* Name:         rtrv_topip(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_topip(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_topipDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_topipDSP));
}


/* Name:         set_topip(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_topip
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_topip(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsTopcliCMTB;
    extern CLIMIBAPIGROUP_t  set_topipAPI;

    return(MibApiCnf(cmdArg, &set_topipAPI, &stsTopcliCMTB));
}


/* Name:         stsPerfMonManualcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsPerfMonManualScalar
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsPerfMonManualcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsPerfMonManualcliCMTB;

    return(MibCnf(cmdArg, &stsPerfMonManualcliCMTB, NULL));
}


/* Name:         rtrv_pmsched_link(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_pmsched_link(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_pmsched_linkDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_pmsched_linkDSP));
}


/* Name:         sched_pmrept_link(U8_t *cmdArg)
 *
 * Description:  Configure the mib sched_pmrept_link
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
sched_pmrept_link(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsPerfMonManualcliCMTB;
    extern CLIMIBAPIGROUP_t  sched_pmrept_linkAPI;

    return(MibApiCnf(cmdArg, &sched_pmrept_linkAPI, &stsPerfMonManualcliCMTB));
}


/* Name:         abort_smu_man_test(U8_t *cmdArg)
 *
 * Description:  Configure the mib abort_smu_man_test
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
abort_smu_man_test(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsPerfMonManualcliCMTB;
    extern CLIMIBAPIGROUP_t  abort_smu_man_testAPI;

    return(MibApiCnf(cmdArg, &abort_smu_man_testAPI, &stsPerfMonManualcliCMTB));
}


/* Name:         stsClockInputStatuscli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsClockInputStatusTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsClockInputStatuscli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsClockInputStatuscliCMTB;

    return(MibCnf(cmdArg, &stsClockInputStatuscliCMTB, NULL));
}


/* Name:         rtrv_clockinput_status(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_clockinput_status(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_clockinput_statusDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_clockinput_statusDSP));
}


/* Name:         rtrv_clockinput_ssm(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_clockinput_ssm(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_clockinput_ssmDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_clockinput_ssmDSP));
}


/* Name:         stsCardcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsCardTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsCardcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsCardcliCMTB;

    return(MibCnf(cmdArg, &stsCardcliCMTB, NULL));
}


/* Name:         rtrv_card(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_card(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_cardDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_cardDSP));
}


/* Name:         reset_card(U8_t *cmdArg)
 *
 * Description:  Configure the mib reset_card
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
reset_card(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsCardcliCMTB;
    extern CLIMIBAPIGROUP_t  reset_cardAPI;

    return(MibApiCnf(cmdArg, &reset_cardAPI, &stsCardcliCMTB));
}


/* Name:         stsOscillatorcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsOscillatorTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsOscillatorcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsOscillatorcliCMTB;

    return(MibCnf(cmdArg, &stsOscillatorcliCMTB, NULL));
}


/* Name:         rtrv_clock(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_osc(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_oscDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_oscDSP));
}


/* Name:         sw_dx_osc(U8_t *cmdArg)
 *
 * Description:  Configure the mib sw_dx_osc
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
sw_dx_osc(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsOscillatorcliCMTB;
    extern CLIMIBAPIGROUP_t  sw_dx_oscAPI;

    return(MibApiCnf(cmdArg, &sw_dx_oscAPI, &stsOscillatorcliCMTB));
}


/* Name:         stsGPScli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsGpsTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsGPScli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsGPScliCMTB;

    return(MibCnf(cmdArg, &stsGPScliCMTB, NULL));
}


/* Name:         rtrv_gps_antd(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_antd(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_antdDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_antdDSP));
}


/* Name:         rtrv_gps_azel(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_azel(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_azelDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_azelDSP));
}


/* Name:         rtrv_gps_bits(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_bits(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_bitsDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_bitsDSP));
}


/* Name:         rtrv_gps_pavg(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_pavg(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_pavgDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_pavgDSP));
}


/* Name:         rtrv_gps_sigq(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_sigq(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_sigqDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_sigqDSP));
}


/* Name:         rtrv_gps_spos(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_spos(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_sposDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_sposDSP));
}


/* Name:         rtrv_gps_trmo(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_trmo(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_trmoDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_trmoDSP));
}


/* Name:         rtrv_gps_utct(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_utct(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_utctDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_utctDSP));
}


/* Name:         rtrv_gps_vers(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_vers(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_versDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_versDSP));
}


/* Name:         rtrv_gps_detf(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_detf(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_detfDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_detfDSP));
}


/* Name:         rtrv_gps_efer(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_efer(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_eferDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_eferDSP));
}


/* Name:         rtrv_gps_essd(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_essd(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_essdDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_essdDSP));
}


/* Name:         rtrv_gps_essn(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_essn(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_essnDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_essnDSP));
}


/* Name:         rtrv_gps_fage(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_fage(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_fageDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_fageDSP));
}


/* Name:         rtrv_gps_timd(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_gps_timd(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_gps_timdDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_gps_timdDSP));
}


/* Name:         stsLinkcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsLinkTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsLinkcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsLinkcliCMTB;

    return(MibCnf(cmdArg, &stsLinkcliCMTB, NULL));
}


/* Name:         view_link(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
view_link(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t view_linkDSP;

    return(MibShow(EXACT, cmdArg, &view_linkDSP));
}


/* Name:         view_scan_channel(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
view_scan_channel(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t view_scan_channelDSP;

    return(MibShow(EXACT, cmdArg, &view_scan_channelDSP));
}


/* Name:         rtrv_link(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_link(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_linkDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_linkDSP));
}


/* Name:         set_link(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_link
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_link(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsLinkcliCMTB;
    extern CLIMIBAPIGROUP_t  set_linkAPI;

    return(MibApiCnf(cmdArg, &set_linkAPI, &stsLinkcliCMTB));
}


/* Name:         set_scan_channel(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_scan_channel
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_scan_channel(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsLinkcliCMTB;
    extern CLIMIBAPIGROUP_t  set_scan_channelAPI;

    return(MibApiCnf(cmdArg, &set_scan_channelAPI, &stsLinkcliCMTB));
}


/* Name:         ed_link(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_link
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_link(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsLinkcliCMTB;
    extern CLIMIBAPIGROUP_t  ed_linkAPI;

    return(MibApiCnf(cmdArg, &ed_linkAPI, &stsLinkcliCMTB));
}


/* Name:         stsShelfcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsShelfTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsShelfcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsShelfcliCMTB;

    return(MibCnf(cmdArg, &stsShelfcliCMTB, NULL));
}


/* Name:         rtrv_shelf(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_shelf(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_shelfDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_shelfDSP));
}


/* Name:         reset_shelf(U8_t *cmdArg)
 *
 * Description:  Configure the mib reset_shelf
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
reset_shelf(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsShelfcliCMTB;
    extern CLIMIBAPIGROUP_t  reset_shelfAPI;

    return(MibApiCnf(cmdArg, &reset_shelfAPI, &stsShelfcliCMTB));
}


/* Name:         stsAlarmThresholdcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsAlarmThresholdTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsAlarmThresholdcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsAlarmThresholdcliCMTB;

    return(MibCnf(cmdArg, &stsAlarmThresholdcliCMTB, NULL));
}


/* Name:         view_smu_thres(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_th_link(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_th_linkDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_th_linkDSP));
}


/* Name:         set_smu_thres(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_smu_thres
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_smu_thres(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsAlarmThresholdcliCMTB;
    extern CLIMIBAPIGROUP_t  set_smu_thresAPI;

    return(MibApiCnf(cmdArg, &set_smu_thresAPI, &stsAlarmThresholdcliCMTB));
}


/* Name:         set_th_link(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_th_link
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_th_link(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsAlarmThresholdcliCMTB;
    extern CLIMIBAPIGROUP_t  set_th_linkAPI;

    return(MibApiCnf(cmdArg, &set_th_linkAPI, &stsAlarmThresholdcliCMTB));
}


/* Name:         stsLinkAlarmSettingcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsLinkAlarmTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsLinkAlarmSettingcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsLinkAlarmSettingcliCMTB;

    return(MibCnf(cmdArg, &stsLinkAlarmSettingcliCMTB, NULL));
}


/* Name:         rtrv_attr_link(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_attr_link(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_attr_linkDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_attr_linkDSP));
}


/* Name:         set_attr_link(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_attr_link
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_attr_link(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsLinkAlarmSettingcliCMTB;
    extern CLIMIBAPIGROUP_t  set_attr_linkAPI;

    return(MibApiCnf(cmdArg, &set_attr_linkAPI, &stsLinkAlarmSettingcliCMTB));
}


/* Name:         stsOscillatorAlarmSettingcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsOscillatorAlarmTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsOscillatorAlarmSettingcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsOscillatorAlarmSettingcliCMTB;

    return(MibCnf(cmdArg, &stsOscillatorAlarmSettingcliCMTB, NULL));
}


/* Name:         rtrv_attr_eqpt_clk(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_attr_eqpt_clk(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_attr_eqpt_clkDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_attr_eqpt_clkDSP));
}


/* Name:         set_attr_eqpt_clk(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_attr_eqpt_clk
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_attr_eqpt_clk(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsOscillatorAlarmSettingcliCMTB;
    extern CLIMIBAPIGROUP_t  set_attr_eqpt_clkAPI;

    return(MibApiCnf(cmdArg, &set_attr_eqpt_clkAPI, &stsOscillatorAlarmSettingcliCMTB));
}


/* Name:         stsOsPwrAlarmSettingcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsOsPwrAlarmTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsOsPwrAlarmSettingcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsOsPwrAlarmSettingcliCMTB;

    return(MibCnf(cmdArg, &stsOsPwrAlarmSettingcliCMTB, NULL));
}


/* Name:         rtrv_attr_eqpt_ospwr(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_attr_eqpt_ospwr(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_attr_eqpt_ospwrDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_attr_eqpt_ospwrDSP));
}


/* Name:         set_attr_eqpt_ospwr(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_attr_eqpt_ospwr
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_attr_eqpt_ospwr(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsOsPwrAlarmSettingcliCMTB;
    extern CLIMIBAPIGROUP_t  set_attr_eqpt_ospwrAPI;

    return(MibApiCnf(cmdArg, &set_attr_eqpt_ospwrAPI, &stsOsPwrAlarmSettingcliCMTB));
}


/* Name:         stsOutputAlarmSettingcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsOutputAlarmTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsOutputAlarmSettingcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsOutputAlarmSettingcliCMTB;

    return(MibCnf(cmdArg, &stsOutputAlarmSettingcliCMTB, NULL));
}


/* Name:         rtrv_attr_eqpt_output(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_attr_eqpt_output(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_attr_eqpt_outputDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_attr_eqpt_outputDSP));
}


/* Name:         cur_outp_alm(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
cur_outp_alm(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t cur_outp_almDSP;

    return(MibShow(EXACT, cmdArg, &cur_outp_almDSP));
}


/* Name:         cur_outp_evt(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
cur_outp_evt(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t cur_outp_evtDSP;

    return(MibShow(EXACT, cmdArg, &cur_outp_evtDSP));
}


/* Name:         set_attr_eqpt_output(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_attr_eqpt_output
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_attr_eqpt_output(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsOutputAlarmSettingcliCMTB;
    extern CLIMIBAPIGROUP_t  set_attr_eqpt_outputAPI;

    return(MibApiCnf(cmdArg, &set_attr_eqpt_outputAPI, &stsOutputAlarmSettingcliCMTB));
}


/* Name:         stsOutputPortSettingcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsOutputPortSettingTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsOutputPortSettingcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsOutputPortSettingcliCMTB;

    return(MibCnf(cmdArg, &stsOutputPortSettingcliCMTB, NULL));
}


/* Name:         view_outp_name(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
view_outp_name(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t view_outp_nameDSP;

    return(MibShow(EXACT, cmdArg, &view_outp_nameDSP));
}


/* Name:         set_outp_name(U8_t *cmdArg)
 *
 * Description:  Configure the mib set_outp_name
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
set_outp_name(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsOutputPortSettingcliCMTB;
    extern CLIMIBAPIGROUP_t  set_outp_nameAPI;

    return(MibApiCnf(cmdArg, &set_outp_nameAPI, &stsOutputPortSettingcliCMTB));
}


/* Name:         stsOutputTiuSettingcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsOutputTiuSettingTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsOutputTiuSettingcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsOutputTiuSettingcliCMTB;

    return(MibCnf(cmdArg, &stsOutputTiuSettingcliCMTB, NULL));
}


/* Name:         stsPerfMoncli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsPerfMonTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsPerfMoncli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsPerfMoncliCMTB;

    return(MibCnf(cmdArg, &stsPerfMoncliCMTB, NULL));
}


/* Name:         view_scan_test(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
view_scan_test(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_pm_linkDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_pm_linkDSP));
}


/* Name:         stsOutputCardcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsOutputCardTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsOutputCardcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsOutputCardcliCMTB;

    return(MibCnf(cmdArg, &stsOutputCardcliCMTB, NULL));
}


/* Name:         rtrv_outp_ssm(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_outp_ssm(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_outp_ssmDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_outp_ssmDSP));
}


/* Name:         rtrv_outp_red(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_outp_red(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_outp_redDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_outp_redDSP));
}


/* Name:         ed_outp_ssm(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_outp_ssm
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_outp_ssm(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsOutputCardcliCMTB;
    extern CLIMIBAPIGROUP_t  ed_outp_ssmAPI;

    return(MibApiCnf(cmdArg, &ed_outp_ssmAPI, &stsOutputCardcliCMTB));
}


/* Name:         ed_outp_red(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_outp_red
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_outp_red(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsOutputCardcliCMTB;
    extern CLIMIBAPIGROUP_t  ed_outp_redAPI;

    return(MibApiCnf(cmdArg, &ed_outp_redAPI, &stsOutputCardcliCMTB));
}


/* Name:         stsTerminalcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsTerminalTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsTerminalcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsTerminalcliCMTB;

    return(MibCnf(cmdArg, &stsTerminalcliCMTB, NULL));
}


/* Name:         view_term(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
view_term(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t view_termDSP;

    return(MibShow(EXACT, cmdArg, &view_termDSP));
}


/* Name:         ed_term(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_term
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_term(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsTerminalcliCMTB;
    extern CLIMIBAPIGROUP_t  ed_termAPI;

    return(MibApiCnf(cmdArg, &ed_termAPI, &stsTerminalcliCMTB));
}


/* Name:         stsAlarmStatuscli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsAlarmStatusTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsAlarmStatuscli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsAlarmStatuscliCMTB;

    return(MibCnf(cmdArg, &stsAlarmStatuscliCMTB, NULL));
}


/* Name:         cur_clp_alm(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
cur_clp_alm(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t cur_clp_almDSP;

    return(MibShow(EXACT, cmdArg, &cur_clp_almDSP));
}


/* Name:         cur_clp_evt(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
cur_clp_evt(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t cur_clp_evtDSP;

    return(MibShow(EXACT, cmdArg, &cur_clp_evtDSP));
}


/* Name:         stsEmmStatuscli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsEmmStatusTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsEmmStatuscli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsEmmStatuscliCMTB;

    return(MibCnf(cmdArg, &stsEmmStatuscliCMTB, NULL));
}


/* Name:         rtrv_emm(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_emm(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_emmDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_emmDSP));
}


/* Name:         rtrv_emm_th(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_emm_th(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_emm_thDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_emm_thDSP));
}


/* Name:         ed_emm_th(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_emm_th
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_emm_th(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsEmmStatuscliCMTB;
    extern CLIMIBAPIGROUP_t  ed_emm_thAPI;

    return(MibApiCnf(cmdArg, &ed_emm_thAPI, &stsEmmStatuscliCMTB));
}


/* Name:         stsAntcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsAntTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsAntcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsAntcliCMTB;

    return(MibCnf(cmdArg, &stsAntcliCMTB, NULL));
}


/* Name:         rtrv_ant(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_ant(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_antDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_antDSP));
}


/* Name:         sw_ant(U8_t *cmdArg)
 *
 * Description:  Configure the mib sw_ant
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
sw_ant(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsAntcliCMTB;
    extern CLIMIBAPIGROUP_t  sw_antAPI;

    return(MibApiCnf(cmdArg, &sw_antAPI, &stsAntcliCMTB));
}


/* Name:         stsTopDestcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsTopDestTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsTopDestcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsTopDestcliCMTB;

    return(MibCnf(cmdArg, &stsTopDestcliCMTB, NULL));
}


/* Name:         stsTrapRegcli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsTrapRegTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsTrapRegcli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsTrapRegcliCMTB;

    return(MibCnf(cmdArg, &stsTrapRegcliCMTB, NULL));
}


/* Name:         rtrv_trap_reg(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_trap_reg(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_trap_regDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_trap_regDSP));
}


/* Name:         ed_trap_reg(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_trap_reg
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_trap_reg(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsTrapRegcliCMTB;
    extern CLIMIBAPIGROUP_t  ed_trap_regAPI;

    return(MibApiCnf(cmdArg, &ed_trap_regAPI, &stsTrapRegcliCMTB));
}


/* Name:         stsClockPricli(U8_t *cmdArg)
 *
 * Description:  Configure the mib table stsClockPriTable
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
stsClockPricli(U8_t *cmdArg)
{
    extern CLICFGTBL_t stsClockPricliCMTB;

    return(MibCnf(cmdArg, &stsClockPricliCMTB, NULL));
}


/* Name:         rtrv_clockinput_priority(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
rtrv_clockinput_priority(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t rtrv_clockinput_priorityDSP;

    return(MibShow(EXACT, cmdArg, &rtrv_clockinput_priorityDSP));
}


/* Name:         ed_clockinput_priority(U8_t *cmdArg)
 *
 * Description:  Configure the mib ed_clockinput_priority
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
ed_clockinput_priority(U8_t *cmdArg)
{
    extern CLICFGTBL_t       stsClockPricliCMTB;
    extern CLIMIBAPIGROUP_t  ed_clockinput_priorityAPI;

    return(MibApiCnf(cmdArg, &ed_clockinput_priorityAPI, &stsClockPricliCMTB));
}

