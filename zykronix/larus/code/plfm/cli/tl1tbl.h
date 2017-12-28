 /*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: tl1tbl.h
 *
 *
 *
 *Note:
 *
 *Author(s):
 * shin lin and wilson
 *******************************************************************
 * $Source: /cvsroot/larus5820/code/plfm/cli/tl1tbl.h,v $
 *********************************************************************/
 
/*********************************************************************
 * 2.1
 * SET-SID:[<TID>]:<CTAG>:<SID>; 
 *********************************************************************/

TL1_ARG_t set_sid_datlist[] = {
{TL1_FLAG_NULL_DATA, TL1_IN_STR, 1, TL1_MAP_NOMAP, NULL, (void *)"5820"}
};

TL1_FIELD_PARM_t set_sid_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  1, ARG_ULIST, (void *)set_sid_datlist}
};


#ifdef SIMUL
#define set_sid "set_sid"
#else
extern void set_sid();
#endif

TL1_GROUP_t set_sid_cmd =
{"SET-SID", 3, TL1_CFG_CMD, 1, (void *)set_sid, set_sid_parms, A1};

/*********************************************************************  
 * 15.1
 *  SCHED-PMREPT-LINK:[<TID>]:<AID>:<CTAG>::
 * [<OBSVINVL>],[<NUMOBSV>],[<NUMREPT>][,<MONTYPE>];
 *********************************************************************/
extern u_map_t stsPerfMonManualInputScanIntervalMAP;
extern u_map_t stsPerfMonInputMonTypeMAP;
TL1_ARG_t sched_pmrept_link_datlist[] = {
{TL1_FLAG_NULL_DATA, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsPerfMonManualInputScanIntervalMAP, (void *)"hundred-sec"},
{TL1_FLAG_NULL_DATA, TL1_IN_NUM, 3, TL1_MAP_NOMAP, NULL, (void *)10},
{TL1_FLAG_NULL_DATA, TL1_IN_NUM, 4, TL1_MAP_NOMAP, NULL, (void *)1},
{TL1_FLAG_NULL_DATA, TL1_IN_STR, 5, TL1_MAP_MAPCNV, (void *)&stsPerfMonInputMonTypeMAP, NULL}
};
extern u_map_t stsPerfMonInputCurrentMonMAP;

TL1_ARG_t sched_pmrept_link_aidlist[] = {                           
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV_BITMAP, (void *)&stsPerfMonInputCurrentMonMAP, NULL}
};

TL1_FIELD_PARM_t sched_pmrept_link_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)sched_pmrept_link_aidlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  3, ARG_ULIST, (void *)sched_pmrept_link_datlist}
};

#ifdef SIMUL
#define sched_pmrept_link "sched_pmrept_link"
#else
extern void sched_pmrept_link();
#endif

TL1_GROUP_t sched_pmrept_link_cmd =
{"SCHED-PMREPT-LINK", 5, TL1_CFG_CMD, 1, (void *)sched_pmrept_link, sched_pmrept_link_parms, A1};


/*********************************************************************
 * 11.3.1
 * SET-ATTR-EQPT:[<TID>]:<AID>:<CTAG>::[<NTFCNCDE>],[<CONDTYPE>];
 * 11.4.1
 * RTRV-ATTR-{EQPT|LINK}:[<TID>]:<AID>:<CTAG>::[<NTFCNCDE>],[,<CONDTYPE>];
 *********************************************************************/

/*
 * aid CLKn
*/

extern u_map_t stsOscillatorAlarmSettingClockMAP;
extern u_map_t stsOscillatorAlarmSettingTypeMAP;
extern u_map_t stsOscillatorAlarmSettingSeverityMAP;

TL1_ARG_t set_attr_eqpt_clk_datlist[] = {
{TL1_FLAG_NULL_ALL | TL1_ALL_ALLOWED | TL1_MULTI_ALLOWED, 
                     TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsOscillatorAlarmSettingTypeMAP, NULL},
{TL1_FLAG_NULL_DATA, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsOscillatorAlarmSettingSeverityMAP, (void *)"4"}
};


TL1_ARG_t set_attr_eqpt_clk_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsOscillatorAlarmSettingClockMAP, NULL},
};

TL1_FIELD_PARM_t set_attr_eqpt_clk_parms[] = {
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)set_attr_eqpt_clk_aidlist},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  2, ARG_ULIST, (void *)set_attr_eqpt_clk_datlist}
};

TL1_SUBCMD_KEY_t set_attr_eqpt_clk_keys[] = {
{"CLK", 3}};


TL1_ARG_t rtrv_attr_eqpt_clk_datlist[] = {
{TL1_FLAG_NULL_ALL | TL1_ALL_ALLOWED | TL1_MULTI_ALLOWED, 
                     TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsOscillatorAlarmSettingTypeMAP, NULL}
};


TL1_ARG_t rtrv_attr_eqpt_clk_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsOscillatorAlarmSettingClockMAP, NULL},
};

TL1_FIELD_PARM_t rtrv_attr_eqpt_clk_parms[] = {
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_attr_eqpt_clk_aidlist},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_attr_eqpt_clk_datlist}
};

TL1_SUBCMD_KEY_t rtrv_attr_eqpt_clk_keys[] = {
{"CLK", 3}};


/*
 * aid OUTm-n
 */
/* extern u_map_t stsOutputAlarmSettingPortMAP; */
extern u_map_t stsOutputAlarmSettingTypeMAP;
extern u_map_t stsOutputAlarmSettingSeverityMAP;
CLIUMAPSTRUC_t stsOutputAlarmSettingOutSlotMAP[] = {
{"out1", 1}, {"out2", 2}, {"out3", 3}, {"out4", 4},
{"out5", 5}, {"out6", 6}, {"out7", 7}, {"out8", 8},
{"out9", 9}, {"out10", 10}, {"out11", 11}, {"out12", 12},
{"out13", 13}, {"out14", 14}, {"out15", 15}, {"out16", 16},
MAPDATA_END};

TL1_ARG_t set_attr_eqpt_output_datlist[] = {
{TL1_FLAG_NULL_ALL | TL1_ALL_ALLOWED | TL1_MULTI_ALLOWED, 
                     TL1_IN_STR, 4, TL1_MAP_MAPCNV, (void *)&stsOutputAlarmSettingTypeMAP, NULL},
{TL1_FLAG_NULL_DATA, TL1_IN_STR, 5, TL1_MAP_MAPCNV, (void *)&stsOutputAlarmSettingSeverityMAP, (void *)"4"}
};

TL1_ARG_t set_attr_eqpt_output_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsOutputAlarmSettingOutSlotMAP, NULL},
};

TL1_ARG_t set_attr_eqpt_output_sidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_INT, 2, TL1_MAP_NOMAP, NULL, NULL},
};

TL1_ARG_t set_attr_eqpt_output_pidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_INT, 3, TL1_MAP_NOMAP, NULL, NULL},
};

TL1_FIELD_PARM_t set_attr_eqpt_output_parms[] = {
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)set_attr_eqpt_output_aidlist},
{ARG_SID, TL1_FLAG_OPTIONAL, 1, ARG_ULIST, (void *)set_attr_eqpt_output_sidlist},
{ARG_PID, TL1_FLAG_OPTIONAL, 1, ARG_ULIST, (void *)set_attr_eqpt_output_pidlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  2, ARG_ULIST, (void *)set_attr_eqpt_output_datlist}
};

TL1_SUBCMD_KEY_t set_attr_eqpt_output_keys[] = {
{"OUT", 3}};


TL1_ARG_t rtrv_attr_eqpt_output_datlist[] = {
{TL1_FLAG_NULL_ALL | TL1_ALL_ALLOWED | TL1_MULTI_ALLOWED, 
                     TL1_IN_STR, 4, TL1_MAP_MAPCNV, (void *)&stsOutputAlarmSettingTypeMAP, NULL}
};

TL1_ARG_t rtrv_attr_eqpt_output_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsOutputAlarmSettingOutSlotMAP, NULL},
};

TL1_ARG_t rtrv_attr_eqpt_output_sidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_INT, 2, TL1_MAP_NOMAP, NULL, NULL},
};

TL1_ARG_t rtrv_attr_eqpt_output_pidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_INT, 3, TL1_MAP_NOMAP, NULL, NULL},
};

TL1_FIELD_PARM_t rtrv_attr_eqpt_output_parms[] = {
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_attr_eqpt_output_aidlist},
{ARG_SID, TL1_FLAG_OPTIONAL, 1, ARG_ULIST, (void *)rtrv_attr_eqpt_output_sidlist},
{ARG_PID, TL1_FLAG_OPTIONAL, 1, ARG_ULIST, (void *)rtrv_attr_eqpt_output_pidlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  1, ARG_ULIST, (void *)rtrv_attr_eqpt_output_datlist}
};

TL1_SUBCMD_KEY_t rtrv_attr_eqpt_output_keys[] = {
{"OUT", 3}};


/*
 * aid OS/PWR
 */
extern u_map_t stsOsPwrAlarmSettingOsPwrMAP;
extern u_map_t stsOsPwrAlarmSettingSeverityMAP;

TL1_ARG_t set_attr_eqpt_ospwr_datlist[] = {
{TL1_FLAG_NULL_DATA, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsOsPwrAlarmSettingSeverityMAP, (void *)"4"}
};

TL1_ARG_t set_attr_eqpt_ospwr_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsOsPwrAlarmSettingOsPwrMAP, NULL},
};

TL1_ARG_t set_attr_eqpt_ospwr_sidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_INT, 2, TL1_MAP_NOMAP, NULL, NULL},
};

TL1_FIELD_PARM_t set_attr_eqpt_ospwr_parms[] = {
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)set_attr_eqpt_ospwr_aidlist},
{ARG_SID, TL1_FLAG_OPTIONAL, 1, ARG_ULIST, (void *)set_attr_eqpt_ospwr_sidlist},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  1, ARG_ULIST, (void *)set_attr_eqpt_ospwr_datlist}
};

TL1_SUBCMD_KEY_t set_attr_eqpt_ospwr_keys[] = {
{"PWR", 3}, {"OS", 2}};


TL1_ARG_t rtrv_attr_eqpt_ospwr_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsOsPwrAlarmSettingOsPwrMAP, NULL},
};

TL1_ARG_t rtrv_attr_eqpt_ospwr_sidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_INT, 2, TL1_MAP_NOMAP, NULL, NULL},
};

TL1_FIELD_PARM_t rtrv_attr_eqpt_ospwr_parms[] = {
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_attr_eqpt_ospwr_aidlist},
{ARG_SID, TL1_FLAG_OPTIONAL, 1, ARG_ULIST, (void *)rtrv_attr_eqpt_ospwr_sidlist},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL}
};

TL1_SUBCMD_KEY_t rtrv_attr_eqpt_ospwr_keys[] = {
{"PWR", 3}, {"OS", 2}};

/*
 * subcmd list
 */

#ifdef SIMUL
#define set_attr_eqpt_clk "set_attr_eqpt_clk"
#define set_attr_eqpt_output "set_attr_eqpt_output"
#define set_attr_eqpt_ospwr "set_attr_eqpt_ospwr"
#else
extern void set_attr_eqpt_clk();
extern void set_attr_eqpt_output();
extern void set_attr_eqpt_ospwr();
#endif


TL1_SUBCMD_GROUP_t set_attr_eqpt_subcmds[] =
{
{set_attr_eqpt_clk_keys, 1, 6, TL1_CFG_CMD, 1, set_attr_eqpt_clk, set_attr_eqpt_clk_parms},
{set_attr_eqpt_output_keys, 1, 6, TL1_CFG_CMD, 1, set_attr_eqpt_output, set_attr_eqpt_output_parms},
{set_attr_eqpt_ospwr_keys, 2, 6, TL1_CFG_CMD, 1, set_attr_eqpt_ospwr, set_attr_eqpt_ospwr_parms}
};

TL1_FIELD_PARM_t set_attr_eqpt_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,  TL1_FLAG_MANDATORY, 3, ARG_SUBCMD_LIST, (void *)set_attr_eqpt_subcmds},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL}
};


TL1_GROUP_t set_attr_eqpt_cmd =
{"SET-ATTR-EQPT", 7, TL1_CFG_CMD, 1, NULL, set_attr_eqpt_parms, A1};


#ifdef SIMUL
#define rtrv_attr_eqpt_clk "rtrv_attr_eqpt_clk"
#define rtrv_attr_eqpt_output "rtrv_attr_eqpt_output"
#define rtrv_attr_eqpt_ospwr "rtrv_attr_eqpt_ospwr"
#else
extern void rtrv_attr_eqpt_clk();
extern void rtrv_attr_eqpt_output();
extern void rtrv_attr_eqpt_ospwr();
#endif

TL1_SUBCMD_GROUP_t rtrv_attr_eqpt_subcmds[] =
{
{rtrv_attr_eqpt_clk_keys, 1, 6, TL1_DSP_CMD, 1, rtrv_attr_eqpt_clk, rtrv_attr_eqpt_clk_parms},
{rtrv_attr_eqpt_output_keys, 1, 6, TL1_DSP_CMD, 1, rtrv_attr_eqpt_output, rtrv_attr_eqpt_output_parms},
{rtrv_attr_eqpt_ospwr_keys, 2, 6, TL1_DSP_CMD, 1, rtrv_attr_eqpt_ospwr, rtrv_attr_eqpt_ospwr_parms}
};

TL1_FIELD_PARM_t rtrv_attr_eqpt_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,  TL1_FLAG_MANDATORY, 3, ARG_SUBCMD_LIST, (void *)rtrv_attr_eqpt_subcmds},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_attr_eqpt_cmd =
{"RTRV-ATTR-EQPT", 7, TL1_DSP_CMD, 1, NULL, rtrv_attr_eqpt_parms, A1};


/*********************************************************************
 *  8
 *  RTRV-GPS:[<TID>]:<AID>:<CTAG>:$ANTD[,<DELAY>];
 *********************************************************************/

extern u_map_t stsGpsIndexMAP;
TL1_ARG_t rtrv_gps_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsGpsIndexMAP, NULL},
};


/*
 * subcmd list
 */

TL1_SUBCMD_KEY_t rtrv_gps_azel_keys[] = {
{"$AZEL", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_antd_keys[] = {
{"$ANTD", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_bits_keys[] = {
{"$BITS", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_detf_keys[] = {
{"$DETF", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_efer_keys[] = {
{"$EFER", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_essd_keys[] = {
{"$ESSD", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_essn_keys[] = {
{"$ESSN", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_fage_keys[] = {
{"$FAGE", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_pavg_keys[] = {
{"$PAVG", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_sigq_keys[] = {
{"$SIGQ", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_spos_keys[] = {
{"$SPOS", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_tfom_keys[] = {
{"$TFOM", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_timd_keys[] = {
{"$TIMD", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_trmo_keys[] = {
{"$TRMO", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_utct_keys[] = {
{"$UTCT", 5}};
TL1_SUBCMD_KEY_t rtrv_gps_vers_keys[] = {
{"$VERS", 5}};

#ifdef SIMUL
#define rtrv_gps_azel "rtrv_gps_azel"
#define rtrv_gps_antd "rtrv_gps_antd"
#define rtrv_gps_bits "rtrv_gps_bits"
#define rtrv_gps_detf "rtrv_gps_detf"
#define rtrv_gps_efer "rtrv_gps_efer"
#define rtrv_gps_essd "rtrv_gps_essd"
#define rtrv_gps_essn "rtrv_gps_essn"
#define rtrv_gps_fage "rtrv_gps_fage"
#define rtrv_gps_pavg "rtrv_gps_pavg"
#define rtrv_gps_sigq "rtrv_gps_sigq"
#define rtrv_gps_spos "rtrv_gps_spos"
#define rtrv_gps_tfom "rtrv_gps_tfom"
#define rtrv_gps_timd "rtrv_gps_timd"
#define rtrv_gps_trmo "rtrv_gps_trmo"
#define rtrv_gps_utct "rtrv_gps_utct"
#define rtrv_gps_vers "rtrv_gps_vers"
#else
extern STATUS_t rtrv_gps_azel();
extern STATUS_t rtrv_gps_antd();
extern STATUS_t rtrv_gps_bits();
extern STATUS_t rtrv_gps_detf();
extern STATUS_t rtrv_gps_efer();
extern STATUS_t rtrv_gps_essd();
extern STATUS_t rtrv_gps_essn();
extern STATUS_t rtrv_gps_fage();
extern STATUS_t rtrv_gps_pavg();
extern STATUS_t rtrv_gps_sigq();
extern STATUS_t rtrv_gps_spos();
extern STATUS_t rtrv_gps_tfom();
extern STATUS_t rtrv_gps_timd();
extern STATUS_t rtrv_gps_trmo();
extern STATUS_t rtrv_gps_utct();
extern STATUS_t rtrv_gps_vers();
#endif


TL1_SUBCMD_GROUP_t rtrv_gps_subcmds[] =
{
{rtrv_gps_azel_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_azel, NULL},
{rtrv_gps_antd_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_antd, NULL},
{rtrv_gps_bits_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_bits, NULL},
{rtrv_gps_detf_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_detf, NULL},
{rtrv_gps_efer_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_efer, NULL},
{rtrv_gps_essd_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_essd, NULL},
{rtrv_gps_essn_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_essn, NULL},
{rtrv_gps_fage_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_fage, NULL},
{rtrv_gps_pavg_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_pavg, NULL},
{rtrv_gps_sigq_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_sigq, NULL},
{rtrv_gps_spos_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_spos, NULL},
{rtrv_gps_tfom_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_tfom, NULL},
{rtrv_gps_timd_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_timd, NULL},
{rtrv_gps_trmo_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_trmo, NULL},
{rtrv_gps_utct_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_utct, NULL},
{rtrv_gps_vers_keys, 1, 0, TL1_DSP_CMD, 1, (void *)rtrv_gps_vers, NULL}
};

TL1_FIELD_PARM_t rtrv_gps_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_gps_aidlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  16, ARG_SUBCMD_LIST, (void *)rtrv_gps_subcmds}
};

TL1_GROUP_t rtrv_gps_cmd =
{"RTRV-GPS", 4, TL1_DSP_CMD, 1, NULL, rtrv_gps_parms, A1};




/*********************************************************************
 *  ED-SECU-USER:[<TID>]:<AID>:<CTAG>::[<NUID>],[<NPID>],    18.4
 *   [,<UAP>];
 *********************************************************************/
CLIUMAPSTRUC_t larusCapMAP[] = {
{"a1", 1}, {"a2", 2}, {"a3", 3}, {"a4", 4}, {"a5", 5}, 
{"a6", 6}, {"a7", 7}, {"a8", 8}, {"a9", 9}, MAPDATA_END};
extern STATUS_t getSecuUser(U32_t, U8_t *, MENU_DATA_VAL_t *);


TL1_ARG_t ed_user_secu_datlist[] = {
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 3, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 4, TL1_MAP_MAPCNV, (void *)larusCapMAP, NULL}
};

TL1_ARG_t ed_user_secu_aidlist[] = {
{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV_FUNC, (void *)getSecuUser, NULL}
};

TL1_FIELD_PARM_t ed_user_secu_parms[] = {
{ARG_TID,    TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,    TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)ed_user_secu_aidlist},
{ARG_CTAG,   TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_DATA,   TL1_FLAG_OPTIONAL,  3, ARG_ULIST, (void *)ed_user_secu_datlist}
};

extern STATUS_t ed_user_secu(U32_t, MENU_DATA_VAL_t *);

TL1_GROUP_t ed_user_secu_cmd =
{"ED-SECU-USER", 5, TL1_CFG_CMD, 0, (void *)ed_user_secu, ed_user_secu_parms, A1};

/*********************************************************************
 *  ACT-USER:[<TID>]:<AID>:<CTAG>::<PID>, 18.1
 *  
 *********************************************************************/

TL1_ARG_t act_user_datlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_ARG_t act_user_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_FIELD_PARM_t act_user_parms[] = {
{ARG_TID,    TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,    TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)act_user_aidlist},
{ARG_CTAG,   TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_DATA,   TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)act_user_datlist}
};

extern STATUS_t act_user(U32_t, MENU_DATA_VAL_t *);

TL1_GROUP_t act_user_cmd =
{"ACT-USER", 5, TL1_CFG_CMD, 0, (void *)act_user, act_user_parms, A1};

/*********************************************************************
 *  CANC-USER:[<TID>]:<AID>:<CTAG>;   18.2
 *   
 *********************************************************************/


TL1_ARG_t canc_user_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_FIELD_PARM_t canc_user_parms[] = {
{ARG_TID,    TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,    TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)canc_user_aidlist},
{ARG_CTAG,   TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
};

extern STATUS_t canc_user(U32_t, MENU_DATA_VAL_t *);

TL1_GROUP_t canc_user_cmd =
{"CANC-USER", 3, TL1_CFG_CMD, 0, (void *)canc_user, canc_user_parms, A1};

/*********************************************************************
 *  ENT-SECU-USER:[<TID>]:<AID>:<CTAG>::<PID>,<UAP>;  18.3
 *  
 *********************************************************************/

TL1_ARG_t ent_user_secu_datlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_MANDATORY, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)larusCapMAP, NULL}
};

TL1_ARG_t ent_user_secu_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_FIELD_PARM_t ent_user_secu_parms[] = {
{ARG_TID,    TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,    TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)ent_user_secu_aidlist},
{ARG_CTAG,   TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_DATA,   TL1_FLAG_MANDATORY,  2, ARG_ULIST, (void *)ent_user_secu_datlist}
};

extern STATUS_t ent_user_secu(U32_t, MENU_DATA_VAL_t *);

TL1_GROUP_t ent_user_secu_cmd =
{"ENT-SECU-USER", 5, TL1_CFG_CMD, 0, (void *)ent_user_secu, ent_user_secu_parms, A1};

/*********************************************************************
 *  ED-PID:[<TID>]::<CTAG>::[<OPID>],[<NPID>];
 *  
 *********************************************************************/

TL1_ARG_t ed_pid_datlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_MANDATORY, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, NULL}
};


TL1_FIELD_PARM_t ed_pid_parms[] = {
{ARG_TID,    TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_CTAG,   TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_DATA,   TL1_FLAG_OPTIONAL,  2, ARG_ULIST, (void *)ed_pid_datlist}
};

extern STATUS_t ed_pid(U32_t, MENU_DATA_VAL_t *);

TL1_GROUP_t ed_pid_cmd =
{"ED-PID", 5, TL1_CFG_CMD, 0, (void *)ed_pid, ed_pid_parms, A1};


/*********************************************************************
 *  RTRV-SECU-USER:[<TID>]:<AID>:<CTAG>;  18.5
 *   
 *********************************************************************/

TL1_ARG_t rtrv_user_secu_aidlist[] = {
{TL1_FLAG_MANDATORY| TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV_FUNC, (void *)getSecuUser, NULL}
};

TL1_FIELD_PARM_t rtrv_user_secu_parms[] = {
{ARG_TID,    TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,    TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_user_secu_aidlist},
{ARG_CTAG,   TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

extern STATUS_t rtrv_user_secu(U32_t, MENU_DATA_VAL_t *);

TL1_GROUP_t rtrv_user_secu_cmd =
{"RTRV-SECU-USER", 3, TL1_DSP_CMD, 0, (void *)rtrv_user_secu, rtrv_user_secu_parms, A1};

/*********************************************************************
 *  RTRV-USER:[<TID>]::<CTAG>;
 *   
 *********************************************************************/


TL1_FIELD_PARM_t rtrv_user_parms[] = {
{ARG_TID,    TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_CTAG,   TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

extern STATUS_t rtrv_user(U32_t, MENU_DATA_VAL_t *);

TL1_GROUP_t rtrv_user_cmd =
{"RTRV-USER", 3, TL1_DSP_CMD, 0, (void *)rtrv_user, rtrv_user_parms, A1};

/*********************************************************************
 *  DLT-SECU-USER:[<TID>]:<AID>:<CTAG>;  18.6
 *  
 *********************************************************************/

TL1_ARG_t dlt_user_secu_aidlist[] = {
{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV_FUNC, (void *)getSecuUser, NULL}
};

TL1_FIELD_PARM_t dlt_user_secu_parms[] = {
{ARG_TID,    TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID,    TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)dlt_user_secu_aidlist},
{ARG_CTAG,   TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

extern STATUS_t dlt_user_secu(U32_t, MENU_DATA_VAL_t *);

TL1_GROUP_t dlt_user_secu_cmd =
{"DLT-SECU-USER", 3, TL1_CFG_CMD, 0, (void *)dlt_user_secu, dlt_user_secu_parms, A1};





/********************************************************** 
 *  Taipei Team Added Data
 **********************************************************/

/********************************************************** 
 *  RTRV-HDR:[<TID>]:<CTAG>; 2.2
 **********************************************************/
#ifdef SIMUL
#define rtrv_hdr  "rtrv_hdr"
#else
extern STATUS_t rtrv_hdr();
#endif

TL1_FIELD_PARM_t rtrv_hdr_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_hdr_cmd =
  {"RTRV-HDR", 2, TL1_DSP_CMD, 0, (void *) rtrv_hdr, rtrv_hdr_parms, A1};
  

/********************************************************** 
 *  SET-MGMT-IP:[<TID>]:<CTAG>:<SB>; 2.3 
 **********************************************************/
#ifdef SIMUL
#define set_mgmtip  "set_mgmtip"
static CLIUMAPSTRUC_t stsSystemMgmtIpOptionMAP[] = {
{"STATIC", 1}, {"DHCP", 2}, MAPDATA_END};
#else
extern STATUS_t set_mgmtip();
extern u_map_t stsSystemMgmtIpOptionMAP;
#endif

/* section below is temporarily for testing purpose*/
TL1_ARG_t set_mgmtip_sb_datlist [] = {
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsSystemMgmtIpOptionMAP,"MODE"},
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, "IP"},
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 3, TL1_MAP_NOMAP, NULL, "SUBNET"},
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 4, TL1_MAP_NOMAP, NULL, "GATEWAY"}
};

TL1_FIELD_PARM_t set_mgmtip_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, 4, ARG_ULIST, (void *)set_mgmtip_sb_datlist}
};

TL1_GROUP_t set_mgmtip_cmd =
{"SET-MGMT-IP", 3, TL1_CFG_CMD, 1, (void *)set_mgmtip, set_mgmtip_parms, A1};

/********************************************************** 
 *  RTRV- MGMT-IP:[<TID>]:<CTAG>; 2.4
 **********************************************************/
#ifdef SIMUL
#define rtrv_mgmtip  "rtrv_mgmtip"
#else
extern STATUS_t rtrv_mgmtip();
#endif

     TL1_FIELD_PARM_t rtrv_mgmtip_parms[] = {
       {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
       {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
     };

TL1_GROUP_t rtrv_mgmtip_cmd =
  { "RTRV-MGMT-IP", 2, TL1_DSP_CMD, 1, (void *) rtrv_mgmtip, rtrv_mgmtip_parms, A1};  


/********************************************************** 
 *  SET-TOP-IP:[<TID>]:<CTAG>:<SB>; 2.11 
 **********************************************************/
#ifdef SIMUL
#define set_topip  "set_topip"
static CLIUMAPSTRUC_t stsTopSourceIpOptionMAP[] = {
{"STATIC", 1}, {"DHCP", 2}, MAPDATA_END};
#else
extern STATUS_t set_topip();
extern u_map_t stsTopSourceIpOptionMAP;
#endif

/* section below is temporarily for testing purpose*/
TL1_ARG_t set_topip_sb_datlist [] = {
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsTopSourceIpOptionMAP, "MODE"},
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, "IP"},
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 3, TL1_MAP_NOMAP, NULL, "SUBNET"},
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 4, TL1_MAP_NOMAP, NULL, "GATEWAY"}
};

TL1_FIELD_PARM_t set_topip_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, 4, ARG_ULIST, (void *)set_topip_sb_datlist}
};

TL1_GROUP_t set_topip_cmd =
{"SET-TOP-IP", 3, TL1_CFG_CMD, 1, (void *) set_topip, set_topip_parms, A1};

  
 /********************************************************** 
 *  RTRV-TOP-IP:[<TID>]:<CTAG>; 2.12
 **********************************************************/
#ifdef SIMUL
#define rtrv_topip  "rtrv_topip"

#else
extern STATUS_t rtrv_topip ();
#endif

TL1_FIELD_PARM_t rtrv_topip_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_topip_cmd =
  { "RTRV-TOP-IP", 2, TL1_DSP_CMD, 1, (void *) rtrv_topip, rtrv_topip_parms, A1 };


/********************************************************** 
 *  SET-NTP-IP:[<TID>]:<CTAG>:<SB>; 2.19 
 **********************************************************/
#ifdef SIMUL
#define set_ntpip  "set_ntpip"
static CLIUMAPSTRUC_t stsSystemNtpIpOptionMAP[] = {
{"STATIC", 1}, {"DHCP", 2}, MAPDATA_END};
#else
extern STATUS_t set_ntpip();
extern u_map_t stsSystemNtpIpOptionMAP;
#endif

/* section below is temporarily for testing purpose*/
TL1_ARG_t set_ntpip_sb_datlist [] = {
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsSystemNtpIpOptionMAP, "MODE"},
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, "IP"},
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 3, TL1_MAP_NOMAP, NULL, "SUBNET"},
{TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, TL1_IN_STR, 4, TL1_MAP_NOMAP, NULL, "GATEWAY"}
};

TL1_FIELD_PARM_t set_ntpip_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL|TL1_FLEX_FORMAT, 4, ARG_ULIST, (void *)set_ntpip_sb_datlist}
};

TL1_GROUP_t set_ntpip_cmd =
{"SET-NTP-IP", 3, TL1_CFG_CMD, 1, (void *) set_ntpip, set_ntpip_parms, A1};

/********************************************************** 
 *  RTRV-NTP-IP:[<TID>]:<CTAG>; 2.20
 **********************************************************/
#ifdef SIMUL
#define rtrv_ntpip  "rtrv_ntpip"
#else
extern STATUS_t rtrv_ntpip();
#endif

TL1_FIELD_PARM_t rtrv_ntpip_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_ntpip_cmd =
  { "RTRV-NTP-IP", 2, TL1_DSP_CMD, 1, (void *) rtrv_ntpip, rtrv_ntpip_parms, A1 };
  
 
 /********************************************************** 
 *  SET-DAT:[<TID>]:<CTAG>:<DAT>,<TM>[,<TMOFF>]; 2.27
 **********************************************************/
#ifdef SIMUL
#define set_dat "set_dat"
static CLIUMAPSTRUC_t stsSystemTimeZoneMAP[] = {
{"+12", 1}, {"+11", 2}, {"+10", 3}, {"+9", 4}, {"+8", 5}, 
{"+7", 6}, {"+6", 7}, {"+5", 8}, {"+4", 9}, {"+3", 10}, 
{"+2", 11}, {"+1", 12}, {"-128", 13}, {"-1", 14}, {"-2", 15}, 
{"-3", 16}, {"-4", 17}, {"-5", 18}, {"-6", 19}, {"-7", 20}, 
{"-8", 21}, {"-9", 22}, {"-10", 23}, {"-11", 24}, {"-12", 25}, MAPDATA_END};
#else
extern STATUS_t set_dat ();
#endif

TL1_ARG_t set_dat_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_NOMAP, NULL, NULL},
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, NULL},
  {TL1_FLAG_OPTIONAL,  TL1_IN_INT, 3, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_FIELD_PARM_t set_dat_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_MANDATORY, 3, ARG_ULIST, (void *) set_dat_datlist}
};

TL1_GROUP_t set_dat_cmd =
  { "SET-DAT", 3, TL1_CFG_CMD, 1, (void *) set_dat, set_dat_parms, A1 };
  
 
 /********************************************************** 
 *  RTRV-DAT:[<TID>]:<CTAG>; 2.28
 **********************************************************/
#ifdef SIMUL
#define rtrv_dat  "rtrv_dat"
#else
extern STATUS_t rtrv_dat ();
#endif

TL1_FIELD_PARM_t rtrv_dat_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_dat_cmd =
  { "RTRV-DAT", 2, TL1_DSP_CMD, 1, (void *) rtrv_dat, rtrv_dat_parms, A1 };
  
  
 /********************************************************** 
 *  RTRV-SYSTEM-ALARM:[<TID>]:<CTAG>; 2.29
 **********************************************************/
#ifdef SIMUL
#define rtrv_sysintg_alarm  "rtrv_sysintg_alarm"

#else
extern STATUS_t rtrv_sysintg_alarm ();
#endif

TL1_FIELD_PARM_t rtrv_sysintg_alarm_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_sysintg_alarm_cmd =
  { "RTRV-SYSTEM-ALARM", 2, TL1_DSP_CMD, 1, (void *) rtrv_sysintg_alarm, rtrv_sysintg_alarm_parms, A1};


/********************************************************** 
 *  SET-ACO-COM:[<TID>]:COM:<CTAG>::<ACOMODE>; 2.30
 **********************************************************/
#ifdef SIMUL
#define set_aco_com "set_aco_com"
extern STATUS_t  set_aco_com_aid();
static CLIUMAPSTRUC_t stsSystemAlarmAcoModeMAP[] = {
{"MAN", 1}, {"DELD", 2}, MAPDATA_END};
#else
extern STATUS_t  set_aco_com();
extern STATUS_t  set_aco_com_aid();
extern u_map_t stsSystemAlarmAcoModeMAP;
#endif

TL1_ARG_t set_aco_com_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *) &stsSystemAlarmAcoModeMAP, NULL}
};

TL1_FIELD_PARM_t set_aco_com_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY, 0, ARG_UFUNC, (void *)set_aco_com_aid},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)set_aco_com_datlist}
};

TL1_GROUP_t set_aco_com_cmd =
{"SET-ACO-COM", 5, TL1_CFG_CMD, 1, (void *)set_aco_com, set_aco_com_parms, A1};
  
  
 /********************************************************** 
 *  RTRV-ACO-LED:[<TID>]:<CTAG>; 2.31 
 **********************************************************/
#ifdef SIMUL
#define rtrv_aco_led  "rtrv_aco_led"
#else
extern STATUS_t rtrv_aco_led ();
#endif

TL1_FIELD_PARM_t rtrv_aco_led_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_aco_led_cmd =
  { "RTRV-ACO-LED", 2, TL1_DSP_CMD, 1, (void *) rtrv_aco_led,
  rtrv_aco_led_parms, A1
};


/********************************************************** 
 *  OPR-ACO-ALL:[<TID>]:ALL:<CTAG>; 2.32
 **********************************************************/
#ifdef SIMUL
#define opr_aco_all "opr_aco_all"
extern STATUS_t opr_aco_all_aid();
#else
extern STATUS_t opr_aco_all();
extern STATUS_t opr_aco_all_aid();
#endif

TL1_FIELD_PARM_t opr_aco_all_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY, 0, ARG_UFUNC, (void *)opr_aco_all_aid},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_DEFAULT_ARG_t opr_aco_all_def_dat[] = {
{ TL1_IN_NUM, 1, (void *)2},
{          0, 0, NULL}
};

TL1_GROUP_t opr_aco_all_cmd =
{"OPR-ACO-ALL", 3, TL1_CFG_CMD, 1, (void *)opr_aco_all, opr_aco_all_parms, A1, opr_aco_all_def_dat};

/********************************************************** 
 *  ED-ALARMESCALATION-TIME:[<TID>]:<CTAG>::<HOUR>; 2.33
 **********************************************************/
#ifdef SIMUL
#define ed_alarmescalating_time "ed_alarmescalating_time"
#else
extern STATUS_t ed_alarmescalating_time();
#endif

TL1_ARG_t ed_alarmescalating_time_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_INT, 1, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_FIELD_PARM_t ed_alarmescalating_time_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) ed_alarmescalating_time_datlist}
};

TL1_GROUP_t ed_alarmescalating_time_cmd =
  { "ED-ALARMESCALATION-TIME", 4, TL1_CFG_CMD, 1, (void *) ed_alarmescalating_time, ed_alarmescalating_time_parms, A1};


/********************************************************** 
 *  RTRV-ALARMESCALATION-TIME:[<TID>]:<CTAG>; 2.34
 **********************************************************/
#ifdef SIMUL
#define rtrv_alarmescalating_time  "rtrv_alarmescalating_time"
#else
extern STATUS_t rtrv_alarmescalating_time ();
#endif

TL1_FIELD_PARM_t rtrv_alarmescalating_time_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_alarmescalating_time_cmd =
  { "RTRV-ALARMESCALATION-TIME", 2, TL1_DSP_CMD, 1,
  (void *) rtrv_alarmescalating_time, rtrv_alarmescalating_time_parms, A1
};


/********************************************************** 
 *  RTRV-SYSTEM-LED:[<TID>]:<CTAG>; 2.35
 **********************************************************/
#ifdef SIMUL
#define rtrv_system_led  "rtrv_system_led"
#else
extern STATUS_t rtrv_system_led ();
#endif

TL1_FIELD_PARM_t rtrv_system_led_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_system_led_cmd =
  { "RTRV-SYSTEM-LED", 2, TL1_DSP_CMD, 1, (void *) rtrv_system_led, rtrv_system_led_parms, A1 };

/********************************************************** 
 *  SET-SYSSW-UPGRADE:[<TID>]:<CTAG>; 2.36
 **********************************************************/
#ifdef SIMUL
#define set_syssw_upgrade "set_syssw_upgrade"
#else
extern STATUS_t set_syssw_upgrade ();
#endif

TL1_FIELD_PARM_t set_syssw_upgrade_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_DEFAULT_ARG_t set_syssw_upgrade_def_dat[] = {
{ TL1_IN_NUM, 1, (void *)2},
{          0, 0, NULL}
};

TL1_GROUP_t set_syssw_upgrade_cmd =
  { "SET-SYSSW-UPGRADE", 2, TL1_CFG_CMD, 1, (void *) set_syssw_upgrade, set_syssw_upgrade_parms, A1, set_syssw_upgrade_def_dat};


/********************************************************** 
 *  RTRV-SYSSW-VERSION:[<TID>]:<CTAG>; 2.37
 **********************************************************/
#ifdef SIMUL
#define rtrv_syssw_version  "rtrv_syssw_version"
#else
extern STATUS_t rtrv_syssw_version ();
#endif

TL1_FIELD_PARM_t rtrv_syssw_version_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_syssw_version_cmd =
  { "RTRV-SYSSW-VERSION", 2, TL1_DSP_CMD, 1, (void *) rtrv_syssw_version, rtrv_syssw_version_parms, A1
};


/********************************************************** 
 *  RTRV-SYSSERVICE-STATUS:[<TID>]:<CTAG>; 2.38
 **********************************************************/
#ifdef SIMUL
#define rtrv_sysservice_status  "rtrv_sysservice_status"
#else
extern STATUS_t rtrv_sysservice_status ();
#endif

TL1_FIELD_PARM_t rtrv_sysservice_status_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_sysservice_status_cmd =
  { "RTRV-SYSSERVICE-STATUS", 2, TL1_DSP_CMD, 1,
  (void *) rtrv_sysservice_status, rtrv_sysservice_status_parms, A1
};


/*********************************************************************  
 *  RESET-SYSTEM:[<TID>]:<CTAG>:<TYPE>; 2.39
 *********************************************************************/
#ifdef SIMUL
#define reset_system "reset_system"
static CLIUMAPSTRUC_t stsSystemResetMAP[] = {
{"WARM", 1}, {"COLD", 2}, {"CLEARCONFIG", 3}, MAPDATA_END};
#else
extern STATUS_t reset_system ();
extern u_map_t stsSystemResetMAP;
#endif

TL1_ARG_t reset_system_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *) &stsSystemResetMAP, NULL}
};

TL1_FIELD_PARM_t reset_system_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) reset_system_datlist}
};

TL1_GROUP_t reset_system_cmd =
  {"RESET-SYSTEM", 3, TL1_CFG_CMD, 1, (void *) reset_system, reset_system_parms, A1};


/*********************************************************************  
 *  ED-CLOCKINPUT-MODE:[<TID>]:<CTAG>:<MODE>; 3.1
 *********************************************************************/
#ifdef SIMUL
#define ed_clockinput_mode "ed_clockinput_mode"
static CLIUMAPSTRUC_t stsClockInputOperModeMAP[] = {
{"PROTECTED", 1}, {"MANUAL", 2}, MAPDATA_END};
#else
extern STATUS_t ed_clockinput_mode ();
extern u_map_t stsClockInputOperModeMAP;
#endif	

TL1_ARG_t ed_clockinput_mode_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsClockInputOperModeMAP, NULL}
};

TL1_FIELD_PARM_t ed_clockinput_mode_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) ed_clockinput_mode_datlist}
};

TL1_GROUP_t ed_clockinput_mode_cmd =
{"ED-CLOCKINPUT-MODE", 3, TL1_CFG_CMD, 1, (void *) ed_clockinput_mode, ed_clockinput_mode_parms, A1 };


/********************************************************** 
 *  RTRV-CLOCKINPUT-MODE:[<TID>]:<CTAG>; 3.2
 **********************************************************/
#ifdef SIMUL
#define rtrv_clockinput_mode "rtrv_clockinput_mode"
#else
extern STATUS_t rtrv_clockinput_mode ();
#endif

TL1_FIELD_PARM_t rtrv_clockinput_mode_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_clockinput_mode_cmd =
  { "RTRV-CLOCKINPUT-MODE", 2, TL1_DSP_CMD, 1, (void *) rtrv_clockinput_mode, rtrv_clockinput_mode_parms, A1};


/********************************************************** 
 *  MANUALSELECT:[<TID>]:<CTAG>:<MANUALSELECT>; 3.3
 **********************************************************/
#ifdef SIMUL
#define ed_clockinput_manualselect "ed_clockinput_manualselect"
static CLIUMAPSTRUC_t stsClockInputManualSelectMAP[] = {
{"esr1", 1}, {"esr2", 2}, {"prs", 3}, {"link1", 4}, {"link2", 5}, MAPDATA_END};

#else
extern STATUS_t ed_clockinput_manualselect ();
extern u_map_t stsClockInputManualSelectMAP;
#endif

TL1_ARG_t ed_clockinput_manualselect_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsClockInputManualSelectMAP, NULL}
};

TL1_FIELD_PARM_t ed_clockinput_manualselect_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_OPTIONAL, 1, ARG_ULIST, (void *) ed_clockinput_manualselect_datlist}
};

TL1_GROUP_t ed_clockinput_manualselect_cmd =
  { "ED-CLOCKINPUT-MANUALSELECT", 3, TL1_CFG_CMD, 1, (void *) ed_clockinput_manualselect, ed_clockinput_manualselect_parms, A1 };


/********************************************************** 
 *  RTRV-CLOCKINPUT-MANUALSELECT:[<TID>]:<CTAG>; 3.4 
 **********************************************************/
#ifdef SIMUL
#define rtrv_clockinput_manualselect  "rtrv_clockinput_manualselect"
#else
extern STATUS_t rtrv_clockinput_manualselect ();
#endif

TL1_FIELD_PARM_t rtrv_clockinput_manualselect_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_clockinput_manualselect_cmd =
  { "RTRV-CLOCKINPUT-MANUALSELECT", 2, TL1_DSP_CMD, 1, (void *) rtrv_clockinput_manualselect, rtrv_clockinput_manualselect_parms, A1};


/********************************************************** 
 *  ED-CLOCKINPUT-PRIORITY:[<TID>]:<AID>:<CTAG>:<PRIORITY>; 3.5
 **********************************************************/
 
#ifdef SIMUL
#define ed_clockinput_priority "ed_clockinput_priority"
static CLIUMAPSTRUC_t stsClockPriIndexMAP[] = {
{"ESR", 1}, {"INPUT", 2}, {"PRS", 3},MAPDATA_END};
static CLIUMAPSTRUC_t stsClockPriSettingMAP[] = {
{"PRIORITY-1", 1}, {"PRIORITY-2", 2}, {"PRIORITY-3", 3},MAPDATA_END};
#else
extern STATUS_t ed_clockinput_priority ();
extern u_map_t stsClockPriIndexMAP;
extern u_map_t stsClockPriSettingMAP;
#endif

TL1_ARG_t ed_clockinput_priorityAid_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsClockPriIndexMAP, NULL}
};

TL1_ARG_t ed_clockinput_priority_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 2, TL1_MAP_MAPCNV,(void *) &stsClockPriSettingMAP, NULL}
};

TL1_FIELD_PARM_t ed_clockinput_priority_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) ed_clockinput_priorityAid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) ed_clockinput_priority_datlist}
};

TL1_GROUP_t ed_clockinput_priority_cmd =
{"ED-CLOCKINPUT-PRIORITY", 4, TL1_CFG_CMD, 1, (void *)ed_clockinput_priority, ed_clockinput_priority_parms, A1 };


/********************************************************** 
 * RTRV-CLOCKINPUT-PRIORITY:[<TID>]:<AID>:<CTAG>; 3.6 
 **********************************************************/

#ifdef SIMUL

#define rtrv_clockinput_priority  "rtrv_clockinput_priority"
#else
extern STATUS_t rtrv_clockinput_priority ();
extern u_map_t stsClockPriIndexMAP;
#endif

TL1_ARG_t rtrv_clockinput_priorityAid_datlist[] = {
  {TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsClockPriIndexMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_clockinput_priority_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST,(void *) rtrv_clockinput_priorityAid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_clockinput_priority_cmd =
  { "RTRV-CLOCKINPUT-PRIORITY", 3, TL1_DSP_CMD, 1, (void *) rtrv_clockinput_priority, rtrv_clockinput_priority_parms, A1 };


/********************************************************** 
 *  SET-SLIP-REFAB:[<TID>]::<CTAG>; 3.7.1 
 **********************************************************/
 
/********************************************************** 
#ifdef SIMUL
#define set_slip_refab  "set_slip_refab"
#else
extern STATUS_t set_slip_refab();
#endif

TL1_FIELD_PARM_t set_slip_refab_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t set_slip_refab_cmd =
  { "SET-SLIP-REFAB", 3, TL1_CFG_CMD, 1, (void *) set_slip_refab, set_slip_refab_parms, A1  };

 **********************************************************/

  
/********************************************************** 
 *  SET-SLIP-FRAME:[<TID>]::<CTAG>; 3.7.2 
 **********************************************************/
/********************************************************** 
#ifdef SIMUL
#define set_slip_frame  "set_slip_frame"
#else
extern STATUS_t set_slip_frame();
#endif

TL1_FIELD_PARM_t set_slip_frame_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t set_slip_frame_cmd =
  { "SET-SLIP-FRAME", 3, TL1_CFG_CMD, 1, (void *) set_slip_frame, set_slip_frame_parms, A1 };

 **********************************************************/
 
 /********************************************************** 
 *  RTRV-CLOCKINPUT-STATUS:[<TID>]:<AID>:<CTAG>; 4.1 
 **********************************************************/
#ifdef SIMUL
static CLIUMAPSTRUC_t stsClockInputStatusIndexMAP[] = {
{"ESR1", 1}, {"ESR2", 2}, {"INPUT1", 3}, {"INPUT2", 4}, {"PRS", 5}, MAPDATA_END};
#define rtrv_clockinput_status  "rtrv_clockinput_status"
#else
extern STATUS_t rtrv_clockinput_status ();
extern u_map_t stsClockInputStatusIndexMAP;
#endif

TL1_ARG_t rtrv_clockinput_statusAid_datlist[] = {
{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsClockInputStatusIndexMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_clockinput_status_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST,
   (void *) rtrv_clockinput_statusAid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_clockinput_status_cmd =
  { "RTRV-CLOCKINPUT-STATUS", 3, TL1_DSP_CMD, 1,
(void *) rtrv_clockinput_status, rtrv_clockinput_status_parms, A1 };


/********************************************************** 
 *  RTRV-CLOCKINPUT-SSM:[<TID>]:<AID>:<CTAG>; 5.1
 **********************************************************/
#ifdef SIMUL
#define rtrv_clockinput_ssm  "rtrv_clockinput_ssm"
static CLIUMAPSTRUC_t stsClockInputSSMIndexMAP[] = {
{"INPUT1", 1}, {"INPUT2", 2}, MAPDATA_END};
#else
extern STATUS_t rtrv_clockinput_ssm ();
extern u_map_t stsClockInputSSMIndexMAP;
extern u_map_t stsClockInputSSMInputSelectMAP;
#endif

TL1_ARG_t rtrv_clockinput_ssmAid_datlist[] = {
{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsClockInputSSMIndexMAP, NULL}
};

TL1_ARG_t rtrv_clockinput_ssm_datalist[] = {
{TL1_FLAG_NULL_ALL|TL1_ALL_ALLOWED|TL1_MULTI_ALLOWED, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsClockInputSSMInputSelectMAP, NULL},
};


TL1_FIELD_PARM_t rtrv_clockinput_ssm_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_clockinput_ssmAid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_MANDATORY,  1, ARG_ULIST, (void *) rtrv_clockinput_ssm_datalist }
};

TL1_GROUP_t rtrv_clockinput_ssm_cmd =
  { "RTRV-CLOCKINPUT-SSM", 4, TL1_DSP_CMD, 1, (void *) rtrv_clockinput_ssm, rtrv_clockinput_ssm_parms, A1};


/********************************************************** 
 *  RTRV-CARD:[<TID>]:<SID>:<AID>:<CTAG>; 6.1
 **********************************************************/
#ifdef SIMUL
#define rtrv_card  "rtrv_card"
static CLIUMAPSTRUC_t stsCardShelfMAP[] = {
{"MAIN", 1}, {"EXP1", 2}, {"EXP2", 3}, {"EXP3", 4}, MAPDATA_END};
static CLIUMAPSTRUC_t stsCardIndexMAP[] = {
{"Output1", 1}, {"Output2", 2}, {"Output3", 3}, {"Output4", 4}, {"Output5", 5}, 
{"Output6", 6}, {"Output7", 7}, {"Output8", 8}, {"Output9", 9}, {"Output10", 10}, 
{"Output11", 11}, {"Output12", 12}, {"Output13", 13}, {"Output14", 14}, {"Output15", 15}, 
{"Output16", 16}, {"ESR1", 17}, {"ESR2", 18}, {"SMU", 19}, {"OSC1", 20}, 
{"OSC2", 21}, {"TOP", 22}, {"IMU", 23}, MAPDATA_END};
#else
extern STATUS_t rtrv_card ();
extern u_map_t stsCardShelfMAP;
extern u_map_t stsCardIndexMAP;
#endif


TL1_ARG_t rtrv_cardSid_datlist[] = {
{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsCardShelfMAP, NULL}
};

TL1_ARG_t rtrv_cardAid_datlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 2, TL1_MAP_MAPCNV,(void *) &stsCardIndexMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_card_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_SID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_cardSid_datlist},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_cardAid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_card_cmd =
  { "RTRV-CARD", 4, TL1_DSP_CMD, 1, (void *) rtrv_card, rtrv_card_parms, A1 };
  

/********************************************************** 
 *  RESET-CARD:[<TID>]:<SID>:<AID>:<CTAG>; 6.2
 **********************************************************/
#ifdef SIMUL
#define reset_card "reset_card"
#else
extern STATUS_t reset_card ();
extern u_map_t stsCardShelfMAP;
extern u_map_t stsCardIndexMAP;
#endif

TL1_ARG_t reset_cardSid_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *) &stsCardShelfMAP, NULL}
};

TL1_ARG_t reset_cardAid_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *) &stsCardIndexMAP, NULL}
};

TL1_FIELD_PARM_t reset_card_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_SID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) reset_cardSid_datlist},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) reset_cardAid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
};

TL1_DEFAULT_ARG_t reset_card_def_dat[] = {
{ TL1_IN_NUM, 3, (void *)2},
{          0, 0, NULL}
};

TL1_GROUP_t reset_card_cmd =  {"RESET-CARD", 3, TL1_CFG_CMD, 1, (void *) reset_card, reset_card_parms, A1, reset_card_def_dat};


 /********************************************************** 
 *  RTRV-OSC:[<TID>]:<AID>:<CTAG>; 7.1
 **********************************************************/
#ifdef SIMUL
#define rtrv_osc "rtrv_osc"
static CLIUMAPSTRUC_t stsOscillatorIndexMAP[] = {
{"OSC1", 1}, {"OSC2", 2}, MAPDATA_END};

#else
extern STATUS_t rtrv_osc();
extern u_map_t stsOscillatorIndexMAP;
#endif

TL1_ARG_t rtrv_osc_aid_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *) &stsOscillatorIndexMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_osc_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_osc_aid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_osc_cmd = {"RTRV-OSC", 3, TL1_DSP_CMD, 1, (void *) rtrv_osc, rtrv_osc_parms, A1 }; 
  
 
 /********************************************************** 
 *  SW-DX-OSC:[<TID>]:<AID>:<CTAG>; 7.2
 **********************************************************/
#ifdef SIMUL
#define sw_dx_osc "sw_dx_osc"
#else
extern STATUS_t sw_dx_osc ();
extern u_map_t stsOscillatorIndexMAP;
#endif

TL1_ARG_t sw_dx_osc_aid_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *) &stsOscillatorIndexMAP, NULL}
};

TL1_FIELD_PARM_t sw_dx_osc_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) sw_dx_osc_aid_datlist},  
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_DEFAULT_ARG_t sw_dx_osc_def_dat[] = {
{ TL1_IN_NUM, 2, (void *)2},
{          0, 0, NULL}
};

TL1_GROUP_t sw_dx_osc_cmd =
  { "SW-DX-OSC", 3, TL1_CFG_CMD, 1, (void *) sw_dx_osc, sw_dx_osc_parms, A1, sw_dx_osc_def_dat }; 


 /********************************************************** 
*  ED-LINK:[<TID>]:<AID>:<CTAG>:::<SB>; 9.1
 **********************************************************/
#ifdef SIMUL
#define ed_link "ed_link"
static CLIUMAPSTRUC_t stsLinkIndexMAP[] = {
{"MONn", 1}, {"PLNK", 2}, {"SLNK", 3}, MAPDATA_END};
static CLIUMAPSTRUC_t stsLinkFMTMAP[] = {
{"SF", 1}, {"ESF", 2}, MAPDATA_END};
static CLIUMAPSTRUC_t stsLinkLineCodeMAP[] = {
{"AMI", 1}, {"B8ZS", 2}, MAPDATA_END};
static CLIUMAPSTRUC_t stsLinkMFCHKMAP[] = {
{"MON1", 1}, {"MON2", 2}, {"MON3", 3}, {"MON4", 4}, {"MON5", 5}, MAPDATA_END};
static CLIUMAPSTRUC_t stsLinkEnableMAP[] = {
{"ON", 1}, {"OFF", 2}, MAPDATA_END};
#else
extern STATUS_t  ed_link();
extern u_map_t stsLinkIndexMAP;
extern u_map_t stsLinkFMTMAP;
extern u_map_t stsLinkLineCodeMAP;
extern u_map_t stsLinkMFCHKMAP;
extern u_map_t stsLinkEnableMAP;
extern u_map_t stsLinkTerminationMAP;
#endif


TL1_ARG_t ed_link_sb_datlist[] = {
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 4, TL1_MAP_MAPCNV, (void *)&stsLinkFMTMAP, "FMT"},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsLinkLineCodeMAP, "LINECDE"},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 5, TL1_MAP_MAPCNV, (void *)&stsLinkTerminationMAP, "TERM"},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 6, TL1_MAP_MAPCNV, (void *)&stsLinkMFCHKMAP, "CRCCHK"},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 7, TL1_MAP_NOMAP, NULL, "LNKID"},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsLinkEnableMAP, "SCAN"}
};

TL1_ARG_t ed_link_aid_datlist [] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsLinkIndexMAP, NULL}
};

TL1_FIELD_PARM_t ed_link_parms [] = {
{ARG_TID,  TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) ed_link_aid_datlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_MANDATORY|TL1_FLEX_FORMAT, 6, ARG_ULIST, (void *) ed_link_sb_datlist}
};

TL1_DEFAULT_ARG_t ed_link_def_dat[] = {
{ TL1_IN_NUM, 7, (void *)1},
{          0, 0, NULL}
};

TL1_GROUP_t ed_link_cmd =
{"ED-LINK", 6, TL1_CFG_CMD, 1, (void *) ed_link, ed_link_parms, A1, ed_link_def_dat };
 
 
 /********************************************************** 
*  RTRV-LINK:[<TID>]:<AID>:<CTAG>;9.2
 **********************************************************/
#ifdef SIMUL
#define rtrv_link "rtrv_link"
#else
extern STATUS_t rtrv_link ();
extern u_map_t stsLinkIndexMAP;
#endif

TL1_ARG_t rtrv_link_aid_datlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsLinkIndexMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_link_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_link_aid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_link_cmd =
  { "RTRV-LINK", 3, TL1_DSP_CMD, 1, (void *) rtrv_link, rtrv_link_parms, A1 }; 
 

/********************************************************** 
*  RESET-SHELF:[<TID>]:<SID>:<CTAG>;10.1
 **********************************************************/
#ifdef SIMUL
#define reset_shelf "reset_shelf"
static CLIUMAPSTRUC_t stsShelfIndexMAP[] = {
{"Main", 1}, {"EXP1", 2}, {"EXP2", 3}, {"EXP3", 4}, MAPDATA_END};
#else
extern STATUS_t reset_shelf ();
extern u_map_t stsShelfIndexMAP;
#endif

TL1_ARG_t reset_shelf_datlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsShelfIndexMAP, NULL}
};

TL1_FIELD_PARM_t reset_shelf_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_SID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) reset_shelf_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
};

TL1_DEFAULT_ARG_t reset_shelf_def_dat[] = {
{ TL1_IN_NUM, 2, (void *)2},
{          0, 0, NULL}
};

TL1_GROUP_t reset_shelf_cmd =
  { "RESET-SHELF", 3, TL1_CFG_CMD, 1, (void *) reset_shelf,
reset_shelf_parms, A1, reset_shelf_def_dat };
  
  /********************************************************** 
*  RTRV-SHELF:[<TID>]:<SID>:<CTAG>;10.2
 **********************************************************/
#ifdef SIMUL
#define rtrv_shelf "rtrv_shelf"
#else
extern STATUS_t rtrv_shelf();
extern u_map_t stsShelfIndexMAP;
#endif

TL1_ARG_t rtrv_shelf_datlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsShelfIndexMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_shelf_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_SID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_shelf_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_shelf_cmd =
  { "RTRV-SHELF", 3, TL1_DSP_CMD, 1, (void *) rtrv_shelf, rtrv_shelf_parms, A1 };

 
/*********************************************************************
SET-TH-LINK:[<TID>]:<AID>:<CTAG>::[<MONTYPE>],<THLEV>,,[<TMPER>];   11.1
 *********************************************************************/

#ifdef SIMUL
#define set_th_link "set_th_link"
static CLIUMAPSTRUC_t stsAlarmThresholdCurrentMonMAP[] = {
{"MON1", 1}, {"MON2", 2}, {"MON3", 3}, {"MON4", 4}, {"MON5", 5}, MAPDATA_END};
static CLIUMAPSTRUC_t stsAlarmThresholdTypeMAP[] = {
{"CVL", 1}, {"CVP", 2}, {"CS", 3}, {"JITTER", 4}, {"TIE", 5}, {"MTIE", 6}, MAPDATA_END};

static CLIUMAPSTRUC_t stsAlarmThresholdTimeIntervalMAP[] = {
{"1-SEC", 1}, {"10-SEC", 2}, {"1-HR", 3}, {"1-DAY", 4}, MAPDATA_END};
#else
extern STATUS_t  set_th_link();
extern u_map_t stsAlarmThresholdCurrentMonMAP;
extern u_map_t stsAlarmThresholdTypeMAP;
extern u_map_t stsAlarmThresholdTimeIntervalMAP;
#endif

TL1_ARG_t set_th_link_aidlist[] = {
{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsAlarmThresholdCurrentMonMAP, NULL}
};
TL1_ARG_t set_th_link_datalist[] = {
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsAlarmThresholdTypeMAP, NULL},
{TL1_FLAG_MANDATORY, TL1_IN_STR, 3, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_IGNORE,   TL1_IN_STR, 0, TL1_MAP_NOMAP, NULL,  NULL},
{TL1_FLAG_NULL_DATA, TL1_IN_STR, 4, TL1_MAP_MAPCNV, (void *)&stsAlarmThresholdTimeIntervalMAP, (void *) "one-hour"}
};

TL1_FIELD_PARM_t set_th_link_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL,0, ARG_NULL,NULL },
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) set_th_link_aidlist },
{ARG_CTAG, TL1_FLAG_MANDATORY,0,ARG_NULL, NULL },
{ARG_IGNORE, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_MANDATORY, 4, ARG_ULIST, (void *) set_th_link_datalist }
};

TL1_GROUP_t set_th_link_cmd =
{"SET-TH-LINK", 5, TL1_CFG_CMD, 1,(void *) set_th_link, set_th_link_parms, A1};


/*********************************************************************
*RTRV-TH-LINK:[<TID>]:<AID>:<CTAG>[::[<MONTYPE>],[,<TMPER>]];   11.2
 *********************************************************************/

#ifdef SIMUL
#define rtrv_th_link "rtrv_th_link"
#else
extern STATUS_t rtrv_th_link();
extern u_map_t stsAlarmThresholdCurrentMonMAP;
extern u_map_t stsAlarmThresholdTypeMAP;
extern u_map_t stsAlarmThresholdTimeIntervalMAP;
#endif

TL1_ARG_t rtrv_th_aidlist[] = {
{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsAlarmThresholdCurrentMonMAP, NULL}
};

TL1_ARG_t rtrv_th_datalist[] = {
{TL1_FLAG_NULL_ALL|TL1_ALL_ALLOWED|TL1_MULTI_ALLOWED, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsAlarmThresholdTypeMAP, NULL},
{TL1_FLAG_IGNORE, TL1_IN_STR, 0, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_NULL_DATA, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsAlarmThresholdTimeIntervalMAP, (void *) "1-HR" }
};

TL1_FIELD_PARM_t rtrv_th_link_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_th_aidlist },
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  3, ARG_ULIST, (void *) rtrv_th_datalist }
};

TL1_GROUP_t rtrv_th_link_cmd =
{"RTRV-TH-LINK", 5, TL1_DSP_CMD, 1,(void *) rtrv_th_link, rtrv_th_link_parms, A1};


/*********************************************************************
SET-ATTR-LINK:[<TID>]:<AID>:<CTAG>::[<NTFCNCDE>],[<CONDTYPE>];   11.3.2
 *********************************************************************/
#ifdef SIMUL
#define set_attr_link "set_attr_link"
static CLIUMAPSTRUC_t stsLinkAlarmSettingSevirityMAP[] = {
{"CR", 1}, {"MJ", 2}, {"MN", 3}, {"NA", 4}, {"NR", 5}, MAPDATA_END};
#else
extern STATUS_t set_attr_link();
extern u_map_t stsLinkAlarmSettingLinkTypeMAP;
extern u_map_t stsLinkAlarmSettingSevirityMAP;
extern u_map_t stsLinkAlarmSettingTypeMAP;
#endif

TL1_ARG_t set_attr_link_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingLinkTypeMAP, NULL}
};

TL1_ARG_t set_attr_link_datalist[] = {
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingTypeMAP, NULL},
{TL1_FLAG_NULL_ALL|TL1_ALL_ALLOWED|TL1_MULTI_ALLOWED, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingSevirityMAP, NULL}
};

TL1_FIELD_PARM_t set_attr_link_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL,   0, ARG_NULL, NULL },
{ARG_AID,  TL1_FLAG_MANDATORY,  1, ARG_ULIST, (void *) set_attr_link_aidlist},
{ARG_CTAG, TL1_FLAG_MANDATORY,  0, ARG_NULL, NULL },
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL },
{ARG_DATA, TL1_FLAG_OPTIONAL,   2, ARG_ULIST, (void *) set_attr_link_datalist}
};

TL1_GROUP_t set_attr_link_cmd =
{"SET-ATTR-LINK", 5, TL1_CFG_CMD, 1,(void *) set_attr_link, set_attr_link_parms, A1};


/*********************************************************************
*RTRV-ATTR-LINK:[<TID>]:<AID>:<CTAG>::[<NTFCNCDE>][,<CONDTYPE>];   11.4.1
 *********************************************************************/
#ifdef SIMUL
#define rtrv_attr_link "rtrv_attr_link"
#else
extern STATUS_t rtrv_attr_link();
extern u_map_t stsLinkAlarmSettingLinkTypeMAP;
extern u_map_t stsLinkAlarmSettingSevirityMAP;
extern u_map_t stsLinkAlarmSettingTypeMAP;
#endif

TL1_ARG_t rtrv_attr_link_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingLinkTypeMAP, NULL}
};

TL1_ARG_t rtrv_attr_link_datalist[] = {
{TL1_FLAG_NULL_ALL|TL1_ALL_ALLOWED|TL1_MULTI_ALLOWED, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingSevirityMAP, NULL},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingTypeMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_attr_link_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_attr_link_aidlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  2, ARG_ULIST, (void *) rtrv_attr_link_datalist}
};

TL1_GROUP_t rtrv_attr_link_cmd =
{"RTRV-ATTR-LINK", 5, TL1_DSP_CMD, 1,(void *) rtrv_attr_link, rtrv_attr_link_parms, A1};



/*********************************************************************
*RTRV-COND-LINK:[<TID>]:<AID>:<CTAG>::[<CONDTYPE>],,[,<TMPER>];   11.5.1
 *********************************************************************/
/*********************************************************************
#ifdef SIMUL
#define rtrv_cond_link "rtrv_cond_link"
#else
extern STATUS_t rtrv_cond_link();
extern u_map_t stsLinkAlarmSettingLinkTypeMAP;
extern u_map_t stsLinkAlarmSettingTypeMAP
extern u_map_t stsAlarmThresholdTimeIntervalMAP;
#endif

TL1_ARG_t rtrv_cond_link_aidlist[] = {
{TL1_FLAG_MANDATORY|TL1_MULTI_ALLOWED|TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingLinkTypeMAP, NULL}
};

TL1_ARG_t rtrv_cond_link_datalist[] = {
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingTypeMAP, NULL},
{TL1_FLAG_IGNORE, TL1_IN_STR, 0, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_IGNORE, TL1_IN_STR, 0, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsAlarmThresholdTimeIntervalMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_cond_link_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_cond_link_aidlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  4, ARG_ULIST, (void *) rtrv_cond_link_datalist}
};

TL1_GROUP_t rtrv_cond_link_cmd =
{"RTRV-COND-LINK", 5, TL1_DSP_CMD, 1,(void *) rtrv_cond_link, rtrv_cond_link_parms, A1};

  *********************************************************************/
 
 /*********************************************************************
*RTRV-COND-COM:[<TID>]:COM:<CTAG>::[<CONDTYPE>],,[,<TMPER>];   11.5.2
 *********************************************************************/
 /*********************************************************************
#ifdef SIMUL
#define rtrv_cond_com "rtrv_cond_com"
extern STATUS_t  rtrv_cond_com_aid();
#else
extern STATUS_t rtrv_cond_com();
extern STATUS_t  rtrv_cond_com_aid();
extern u_map_t stsLinkAlarmSettingTypeMAP;
extern u_map_t stsAlarmThresholdTimeIntervalMAP;
#endif

TL1_ARG_t rtrv_cond_com_datalist[] = {
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingTypeMAP, NULL},
{TL1_FLAG_IGNORE, TL1_IN_STR, 0, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_IGNORE, TL1_IN_STR, 0, TL1_MAP_NOMAP, NULL, NULL},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsAlarmThresholdTimeIntervalMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_cond_com_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY, 0, ARG_UFUNC, (void *)rtrv_cond_com_aid},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  4, ARG_ULIST, (void *) rtrv_cond_com_datalist}
};

TL1_GROUP_t rtrv_cond_com_cmd =
{"RTRV-COND-COM", 5, TL1_DSP_CMD, 1,(void *) rtrv_cond_com, rtrv_cond_com_parms, A1};

  *********************************************************************/

/*********************************************************************
*RTRV-ALM-LINK:[<TID>]:<AID>:<CTAG>::[<NTFCNCDE>],[<CONDTYPE>][,<SRVEFF>];  11.6.1
 *********************************************************************/
/*********************************************************************
#ifdef SIMUL
#define rtrv_alm_link "rtrv_alm_link"
static CLIUMAPSTRUC_t stsAlarmSrvEffMAP[] = {
{"SA", 1}, {"NSA", 2}, MAPDATA_END};
#else
extern STATUS_t rtrv_alm_link();
extern u_map_t stsLinkAlarmSettingLinkTypeMAP;
extern u_map_t stsLinkAlarmSettingSevirityMAP;
extern u_map_t stsLinkAlarmSettingTypeMAP;
extern u_map_t stsAlarmSrvEffMAP;
#endif

TL1_ARG_t rtrv_alm_link_aidlist[] = {
{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingLinkTypeMAP, NULL}
};

TL1_ARG_t rtrv_alm_link_datalist[] = {
{TL1_FLAG_NULL_ALL|TL1_ALL_ALLOWED|TL1_MULTI_ALLOWED, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingSevirityMAP, NULL},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingTypeMAP, NULL},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 4, TL1_MAP_MAPCNV, (void *)&stsAlarmSrvEffMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_alm_link_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_alm_link_aidlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  3, ARG_ULIST, (void *) rtrv_alm_link_datalist}
};

TL1_GROUP_t rtrv_alm_link_cmd =
{"RTRV-ALM-LINK", 5, TL1_DSP_CMD, 1,(void *) rtrv_alm_link, rtrv_alm_link_parms, A1};

 *********************************************************************/

/*********************************************************************
*RTRV-ALM-ALL:[<TID>]:ALL:<CTAG>::[<NTFCNCDE>],[<CONDTYPE>][,<SRVEFF>];  11.6.2
 *********************************************************************/
/*********************************************************************
#ifdef SIMUL
#define rtrv_alm_all "rtrv_alm_all"
extern STATUS_t  rtrv_alm_all_aid();
#else
extern STATUS_t rtrv_alm_all();
extern STATUS_t rtrv_alm_all_aid();
extern u_map_t stsLinkAlarmSettingSevirityMAP;
extern u_map_t stsLinkAlarmSettingTypeMAP;
extern u_map_t stsAlarmSrvEffMAP;
#endif

TL1_ARG_t rtrv_alm_all_datalist[] = {
{TL1_FLAG_NULL_ALL|TL1_ALL_ALLOWED|TL1_MULTI_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingSevirityMAP, NULL},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *)&stsLinkAlarmSettingTypeMAP, NULL},
{TL1_FLAG_OPTIONAL, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsAlarmSrvEffMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_alm_all_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY, 0, ARG_UFUNC, (void *)rtrv_alm_all_aid},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_IGNORE, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_OPTIONAL,  3, ARG_ULIST, (void *) rtrv_alm_all_datalist}
};

TL1_GROUP_t rtrv_alm_all_cmd =
{"RTRV-ALM-ALL", 5, TL1_DSP_CMD, 1,(void *) rtrv_alm_all, rtrv_alm_all_parms, A1};

 *********************************************************************/


 /********************************************************** 
*  RTRV-ANT:[<TID>]:<AID>:<CTAG>;12.1
 **********************************************************/
#ifdef SIMUL
#define rtrv_ant "rtrv_ant"
static CLIUMAPSTRUC_t stsAntIndexMAP[] = {
{"ANT1", 1}, {"ANT2", 2}, MAPDATA_END};
#else
extern STATUS_t  rtrv_ant();
#endif

TL1_ARG_t rtrv_ant_aid_datlist[] = {
{TL1_FLAG_NULL_DATA, TL1_IN_STR, 1, TL1_MAP_NOMAP, NULL, NULL}	
};

TL1_FIELD_PARM_t rtrv_ant_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_ant_aid_datlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_ant_cmd =
{"RTRV-ANT", 3, TL1_DSP_CMD, 1, (void *)rtrv_ant, rtrv_ant_parms, A1};
 
 
 /********************************************************** 
*  SW-ANT:[<TID>]:<AID>:<CTAG>;12.2
 **********************************************************/
#ifdef SIMUL
#define sw_ant "sw_ant"
#else
extern STATUS_t sw_ant ();
#endif

TL1_ARG_t sw_ant_aid_datlist[] = {
{TL1_FLAG_NULL_DATA, TL1_IN_NUM, 1, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_FIELD_PARM_t sw_ant_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)sw_ant_aid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_DEFAULT_ARG_t sw_ant_def_dat[] = {
{ TL1_IN_NUM, 2, (void *)2},
{          0, 0, NULL}
};

TL1_GROUP_t sw_ant_cmd = { "SW-ANT", 3, TL1_CFG_CMD, 1, (void *) sw_ant, sw_ant_parms, A1, sw_ant_def_dat};


/********************************************************** 
*  ED-ANT:[<TID>]:<AID>:<CTAG>::<SB>; 12.3
 **********************************************************/
#ifdef SIMUL
#define ed_ant "ed_ant"
static CLIUMAPSTRUC_t stsAntStateMAP[] = {
{"ENABLE", 1}, {"DISABLE", 2}, MAPDATA_END};
#else
extern STATUS_t ed_ant ();
#endif

TL1_ARG_t ed_ant_aid_datlist[] = {
	{TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_ARG_t ed_ant_datlist[] = {
	{TL1_FLAG_MANDATORY, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, NULL}
};


TL1_FIELD_PARM_t ed_ant_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) ed_ant_aid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_IGNORE, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_MANDATORY,  1, ARG_ULIST, (void *)ed_ant_datlist},
};

TL1_GROUP_t ed_ant_cmd =  { "ED-ANT", 5, TL1_CFG_CMD, 1, (void *) ed_ant, ed_ant_parms, A1 };

/********************************************************** 
*  RTRV-EMM:[<TID>]:<SID>:<AID>:<CTAG>;13.1
 **********************************************************/
#ifdef SIMUL
#define rtrv_emm "rtrv_emm"
static CLIUMAPSTRUC_t stsEmmShelfMAP[] = {
{"MAIN", 1}, {"EXP1", 2}, {"EXP2", 3}, {"EXP3", 4}, MAPDATA_END};
static CLIUMAPSTRUC_t stsEmmUnitIdMAP[] = {
{"EMM1", 1}, {"EMM2", 2}, {"EMM3", 3}, {"EMM4", 4}, {"EMM5", 5}, 
{"EMM6", 6}, {"EMM7", 7}, {"EMM8", 8}, {"EMM9", 9}, {"EMM10", 10}, 
{"EMM11", 11}, {"EMM12", 12}, {"EMM13", 13}, {"EMM14", 14}, {"EMM15", 15}, 
{"EMM16", 16}, {"EMM17", 17}, {"EMM18", 18}, {"EMM19", 19}, {"EMM20", 20}, MAPDATA_END};
#else
extern STATUS_t rtrv_emm();
extern u_map_t stsEmmShelfMAP;
extern u_map_t stsEmmSensorTypeMAP;
#endif

TL1_ARG_t rtrv_emm_siddatlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsEmmShelfMAP, NULL}
};

TL1_ARG_t rtrv_emm_aiddatlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 2, TL1_MAP_MAPCNV,(void *) &stsEmmSensorTypeMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_emm_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_SID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_emm_siddatlist },
{ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_emm_aiddatlist },
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
};

TL1_GROUP_t rtrv_emm_cmd =
{"RTRV-EMM", 4, TL1_DSP_CMD, 1,(void *)rtrv_emm, rtrv_emm_parms, A1};


/********************************************************** 
*  ED-EMM-TH:[<TID>]:<SID>:<AID>:<CTAG>:<SB>;13.2
 **********************************************************/
#ifdef SIMUL
#define ed_emm_th "ed_emm_th"
#else
extern STATUS_t ed_emm_th();
extern u_map_t stsEmmShelfMAP ;
#if 0 /* TAIFAN_MOD */
extern u_map_t stsEmmSensorTypeMAP;
#endif
#endif


TL1_ARG_t ed_emm_th_siddatlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsEmmShelfMAP, NULL}
};

TL1_ARG_t ed_emm_th_aiddatlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 2, TL1_MAP_MAPCNV,(void *) &stsEmmSensorTypeMAP, NULL}
};

TL1_ARG_t ed_emm_th_sbdatlist[] ={
{TL1_FLEX_FORMAT, TL1_IN_INT, 3, TL1_MAP_NOMAP, NULL, "MJ-HI-TH"},
{TL1_FLEX_FORMAT, TL1_IN_INT, 4, TL1_MAP_NOMAP, NULL, "MJ-LO-TH"},
{TL1_FLEX_FORMAT, TL1_IN_INT, 5, TL1_MAP_NOMAP, NULL, "MN-HI-TH"},
{TL1_FLEX_FORMAT, TL1_IN_INT, 6, TL1_MAP_NOMAP, NULL, "MN-LO-TH"}
};

TL1_FIELD_PARM_t ed_emm_th_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_SID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)ed_emm_th_siddatlist},
{ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)ed_emm_th_aiddatlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_MANDATORY | TL1_FLEX_FORMAT, 4, ARG_ULIST, (void *)ed_emm_th_sbdatlist}
};

TL1_GROUP_t ed_emm_th_cmd =
{"ED-EMM-TH", 5, TL1_CFG_CMD, 1,(void *)ed_emm_th, ed_emm_th_parms, A1};


/********************************************************** 
*  RTRV-EMM-TH:[<TID>]:<SID>:<AID>:<CTAG>;13.3
 **********************************************************/
#ifdef SIMUL
#define rtrv_emm_th "rtrv_emm_th"
#else
extern STATUS_t rtrv_emm_th();
extern u_map_t stsEmmShelfMAP ;
extern u_map_t stsEmmSensorTypeMAP;
#endif

TL1_ARG_t rtrv_emm_th_siddatlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsEmmShelfMAP, NULL}
};

TL1_ARG_t rtrv_emm_th_aiddatlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 2, TL1_MAP_MAPCNV,(void *) &stsEmmSensorTypeMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_emm_th_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL,0,ARG_NULL,NULL},
{ARG_SID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_emm_th_siddatlist},
{ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_emm_th_aiddatlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
};

TL1_GROUP_t rtrv_emm_th_cmd =
{"RTRV-EMM-TH", 4, TL1_DSP_CMD, 1,(void *)rtrv_emm_th, rtrv_emm_th_parms, A1};


/********************************************************** 
*  ED-TRAP-REG:[<TID>]:<AID>:<CTAG>:<SB>; 14.1
**********************************************************/
#ifdef SIMUL
#define ed_trap_reg "ed_trap_reg"
static CLIUMAPSTRUC_t stsTrapRegStateMAP[] = {
{"ENABLED", 1}, {"DISABLED", 2}, MAPDATA_END};
#else
extern STATUS_t  ed_trap_reg();
extern u_map_t stsTrapRegStateMAP;
#endif

TL1_ARG_t ed_trap_reg_Aiddatlist [] = {
{TL1_FLAG_MANDATORY, TL1_IN_INT, 1, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_ARG_t ed_trap_reg_sbdatlist[]={
{TL1_FLEX_FORMAT, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, "IPADDR"},
{TL1_FLEX_FORMAT, TL1_IN_INT, 3, TL1_MAP_NOMAP, NULL, "UDP"},
{TL1_FLEX_FORMAT|TL1_FLAG_NULL_DATA, TL1_IN_STR, 4, TL1_MAP_NOMAP, NULL, "COMMUNITY"},
{TL1_FLEX_FORMAT, TL1_IN_STR, 5, TL1_MAP_MAPCNV, (void *)&stsTrapRegStateMAP, "STATE"},
};

TL1_FIELD_PARM_t ed_trap_reg_parms [] = {
{ARG_TID, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
/* {ARG_AID, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},*/
{ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)ed_trap_reg_Aiddatlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
{ARG_DATA, TL1_FLAG_MANDATORY|TL1_FLEX_FORMAT, 4, ARG_ULIST, (void *)ed_trap_reg_sbdatlist}
};

TL1_GROUP_t ed_trap_reg_cmd =
{"ED-TRAP-REG", 4, TL1_CFG_CMD, 1, (void *) ed_trap_reg, ed_trap_reg_parms, A1};


/********************************************************** 
 *  RTRV-TRAP-REG:[<TID>]:<AID>:<CTAG>; 14.2
 **********************************************************/
#ifdef SIMUL
#define rtrv_trap_reg  "rtrv_trap_reg"
#else
extern STATUS_t rtrv_trap_reg();
#endif

TL1_ARG_t rtrv_trap_regAid_datlist[] = {
  {TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_FIELD_PARM_t rtrv_trap_reg_parms[] = {
{ARG_TID, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
{ARG_AID, TL1_FLAG_MANDATORY,  1, ARG_ULIST, (void *)rtrv_trap_regAid_datlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_trap_reg_cmd =
{"RTRV-TRAP-REG", 3, TL1_DSP_CMD, 1, (void *)rtrv_trap_reg, rtrv_trap_reg_parms, A1};


/********************************************************** 
 *  RTRV-PMSCHED-LINK:[<TID>]:<AID>:<CTAG>; 15.2 
 **********************************************************/
#ifdef SIMUL
#define rtrv_pmsched_link  "rtrv_pmsched_link"
#else
extern STATUS_t rtrv_pmsched_link();
extern u_map_t stsPerfMonInputCurrentMonMAP;
#endif

TL1_ARG_t rtrv_pmsched_linkAid_datlist[] = {
{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *)&stsPerfMonInputCurrentMonMAP, NULL}
};

TL1_FIELD_PARM_t rtrv_pmsched_link_parms[] = {
{ARG_TID,  TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
{ARG_AID,  TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *)rtrv_pmsched_linkAid_datlist},
{ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t rtrv_pmsched_link_cmd =
{"RTRV-PMSCHED-LINK", 3, TL1_DSP_CMD, 1, (void *)rtrv_pmsched_link, rtrv_pmsched_link_parms, A1};

/********************************************************** 
 *  RTRV-PM-LINK:[<TID>]:<AID>:<CTAG>::[<MONTYPE>],,,,[<TMPER>][,<REGNO>]; 15.4
**********************************************************/

#ifdef SIMUL
#define rtrv_pm_link  "rtrv_pm_link"
static CLIUMAPSTRUC_t stsPmLinkIndexMAP[] = {
{"MONn", 1}, {"ALL", 2}, MAPDATA_END};

static CLIUMAPSTRUC_t stsPerfMonInputMonTypeMAP[] = {
{"CVL", 1}, {"CVP", 2}, {"FCS", 3}, {"ESP", 4}, {"JITTER", 5}, {"SESP", 6}, {"TIE", 7}, {"MTIE", 8}, MAPDATA_END};

static CLIUMAPSTRUC_t stsPerfMonInputMonPeriodMAP[] = {
{"1-HR", 1}, {"1-DAY", 2}, {"100-SECS", 3}, MAPDATA_END};

static CLIUMAPSTRUC_t stsPerfMonInputMonRegnoMAP[] = {
{"0", 1}, {"1", 2}, {"2", 3}, {"3", 4}, {"4", 5},
{"5", 6}, {"6", 7}, {"7", 8}, {"8", 9}, {"9", 10},
{"10", 11}, {"11", 12}, {"12", 13}, {"13", 14}, {"14", 15},
{"15", 16}, {"16", 17}, {"17", 18}, {"18", 19}, {"19", 20},
{"20", 21}, {"21", 22}, {"22", 23}, {"23", 24}, {"24", 25}, MAPDATA_END};
#else
extern STATUS_t rtrv_pm_link();
extern u_map_t stsPerfMonInputCurrentMonMAP;
extern u_map_t stsPerfMonInputMonTypeMAP;
extern u_map_t stsPerfMonInputMonPeriodMAP;
#endif

TL1_ARG_t rtrv_pm_link_aid_datlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsPerfMonInputCurrentMonMAP, NULL}
};

TL1_ARG_t rtrv_pm_link_sb_datlist[] = {
  {TL1_FLAG_OPTIONAL, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *) &stsPerfMonInputMonTypeMAP, NULL},
  {TL1_FLAG_IGNORE, TL1_IN_STR, 0, TL1_MAP_NOMAP, NULL, NULL},
  {TL1_FLAG_IGNORE, TL1_IN_STR, 0, TL1_MAP_NOMAP, NULL, NULL},
  {TL1_FLAG_IGNORE, TL1_IN_STR, 0, TL1_MAP_NOMAP, NULL, NULL},
  {TL1_FLAG_NULL_DATA, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsPerfMonInputMonPeriodMAP, (void *)"one-hour"},
  {TL1_FLAG_NULL_DATA, TL1_IN_STR, 4, TL1_MAP_NOMAP, NULL, (void *)"0"}
};

TL1_FIELD_PARM_t rtrv_pm_link_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) rtrv_pm_link_aid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_IGNORE, TL1_FLAG_OPTIONAL,  0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_OPTIONAL, 6, ARG_ULIST, (void *) rtrv_pm_link_sb_datlist}
};

TL1_GROUP_t rtrv_pm_link_cmd =
  { "RTRV-PM-LINK", 5, TL1_DSP_CMD, 1, (void *) rtrv_pm_link, rtrv_pm_link_parms, A1 };
  

/********************************************************** 
 *  ED-TIMING-OUTPUT:[<TID>]:<SID>:<AID>:<CTAG>:<SB>; 16.1
 **********************************************************/
#ifdef SIMUL
#define ed_timing_output "ed_timing_output"
static CLIUMAPSTRUC_t stsOutputCRCCHKMAP[] = {
{"ENABLE", 1}, {"DISABLE", 2}, MAPDATA_END};
static CLIUMAPSTRUC_t stsOutputCASMAP[] = {
{"ENABLE", 1}, {"DISABLE", 2}, MAPDATA_END};
static CLIUMAPSTRUC_t stsOutputSSMMAP[] = {
{"SA4", 1}, {"SA5", 2}, {"SA6", 3}, {"SA7", 4}, {"SA8", 5}, MAPDATA_END};
static CLIUMAPSTRUC_t stsOutputSSMValueMAP[] = {
{"PRS", 1}, {"STU", 2}, {"ST2", 3}, {"ST3", 4}, {"SIC", 5}, {"DUC", 6}, MAPDATA_END};

#else
extern STATUS_t  ed_timing_output();
/*
extern u_map_t stsCardShelfMAP;
extern u_map_t stsCardIndexMAP;
*/
extern u_map_t  stsOutputShelfMAP;
extern u_map_t  stsOutputCRCCHKMAP;
extern u_map_t  stsOutputCASMAP;
extern u_map_t  stsOutputSSMMAP;
extern u_map_t  stsOutputSSMValueMAP;
#endif
/*
TL1_ARG_t ed_timing_outputSid_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *) &stsCardShelfMAP, NULL}
};
TL1_ARG_t ed_timing_outputAid_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *) &stsCardIndexMAP, NULL}
};
*/

TL1_ARG_t ed_timing_outputSid_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 1, TL1_MAP_MAPCNV, (void *) &stsOutputShelfMAP, NULL}
};

TL1_ARG_t ed_timing_outputAid_datlist[] = {
  {TL1_FLAG_MANDATORY, TL1_IN_STR, 2, TL1_MAP_NOMAP, NULL, NULL}
};

TL1_ARG_t ed_timing_output_sb_datlist[] = {
{TL1_FLAG_MANDATORY | TL1_FLEX_FORMAT, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *)&stsOutputCRCCHKMAP, "CRCCHK"},
{TL1_FLAG_MANDATORY | TL1_FLEX_FORMAT, TL1_IN_STR, 4, TL1_MAP_MAPCNV , (void *)&stsOutputCASMAP, "CAS"},
{TL1_FLAG_MANDATORY | TL1_FLEX_FORMAT, TL1_IN_STR, 5, TL1_MAP_MAPCNV, (void *)&stsOutputSSMMAP, "SSM"},
{TL1_FLAG_MANDATORY | TL1_FLEX_FORMAT, TL1_IN_STR, 6, TL1_MAP_MAPCNV, (void *)&stsOutputSSMValueMAP, "SSMVALUE"}
};

TL1_FIELD_PARM_t ed_timing_output_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_SID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) ed_timing_outputSid_datlist},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) ed_timing_outputAid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_MANDATORY | TL1_FLEX_FORMAT , 4, ARG_ULIST, (void *) ed_timing_output_sb_datlist}
};

TL1_GROUP_t ed_timing_output_cmd =  {"ED-TIMING-OUTPUT", 5, TL1_CFG_CMD, 1, (void *) ed_timing_output, ed_timing_output_parms, A1 };



  
/********************************************************** 
 *  ED-TERM:[<TID>]:<AID>:<CTAG>:<SB>; 17.1
**********************************************************/
#ifdef SIMUL
#define ed_term  "ed_term"
static CLIUMAPSTRUC_t stsTerminalIndexMAP[] = {
{"CID", 1}, MAPDATA_END};
static CLIUMAPSTRUC_t stsTerminalBaudrateMAP[] = {
{"1200", 1}, {"2400", 2}, {"4800", 3}, {"9600", 4}, {"19200", 5}, {"38400", 6}, MAPDATA_END};
static CLIUMAPSTRUC_t stsTerminalDataBitsMAP[] = {
{"7", 1}, {"8", 2}, MAPDATA_END};
static CLIUMAPSTRUC_t stsTerminalParityMAP[] = {
{"EVEN", 1}, {"ODD", 2}, {"NONE", 3}, MAPDATA_END};
static CLIUMAPSTRUC_t stsTerminalStopBitsMAP[] = {
{"1", 1}, {"2", 2}, MAPDATA_END};
#else
extern STATUS_t ed_term ();
extern u_map_t stsTerminalIndexMAP;
extern u_map_t stsTerminalBaudrateMAP;
extern u_map_t stsTerminalDataBitsMAP;
extern u_map_t stsTerminalParityMAP;
extern u_map_t stsTerminalStopBitsMAP;
#endif

TL1_ARG_t ed_term_aid_datlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsTerminalIndexMAP, NULL}
};


TL1_ARG_t ed_term_sb_datlist[] = {
  {TL1_FLAG_OPTIONAL | TL1_FLEX_FORMAT, TL1_IN_STR, 2, TL1_MAP_MAPCNV, (void *) &stsTerminalBaudrateMAP, "BAUD"},
  {TL1_FLAG_OPTIONAL | TL1_FLEX_FORMAT, TL1_IN_STR, 3, TL1_MAP_MAPCNV, (void *) &stsTerminalDataBitsMAP, "DBITS"},
  {TL1_FLAG_OPTIONAL | TL1_FLEX_FORMAT, TL1_IN_STR, 4, TL1_MAP_MAPCNV, (void *) &stsTerminalParityMAP, "PARITY"},
  {TL1_FLAG_OPTIONAL | TL1_FLEX_FORMAT, TL1_IN_STR, 5, TL1_MAP_MAPCNV, (void *) &stsTerminalStopBitsMAP, "SBITS"}
};

TL1_FIELD_PARM_t ed_term_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) ed_term_aid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL},
  {ARG_DATA, TL1_FLAG_OPTIONAL | TL1_FLEX_FORMAT, 4, ARG_ULIST, (void *) ed_term_sb_datlist}
};

TL1_GROUP_t ed_term_cmd =
  { "ED-TERM", 4, TL1_CFG_CMD, 1, (void *) ed_term, ed_term_parms, A1 };

 
  /********************************************************** 
 *  RTRV-TERM:[<TID>]:<AID>:<CTAG>; 17.2
 **********************************************************/
#ifdef SIMUL
#define view_term  "view_term"
#else
extern STATUS_t view_term ();
extern u_map_t stsTerminalIndexMAP;
#endif
TL1_ARG_t view_term_aid_datlist[] = {
	{TL1_FLAG_MANDATORY | TL1_MULTI_ALLOWED | TL1_ALL_ALLOWED, TL1_IN_STR, 1, TL1_MAP_MAPCNV,(void *) &stsTerminalIndexMAP, NULL}
};


TL1_FIELD_PARM_t view_term_parms[] = {
  {ARG_TID, TL1_FLAG_OPTIONAL, 0, ARG_NULL, NULL},
  {ARG_AID, TL1_FLAG_MANDATORY, 1, ARG_ULIST, (void *) view_term_aid_datlist},
  {ARG_CTAG, TL1_FLAG_MANDATORY, 0, ARG_NULL, NULL}
};

TL1_GROUP_t view_term_cmd =
  { "RTRV-TERM", 3, TL1_DSP_CMD, 1, (void *) view_term, view_term_parms, A1 };


/*********************************************************************
********  tl1CmdTable
*********************************************************************/



TL1_GROUP_t *tl1CmdTable[] = {
&set_sid_cmd, &sched_pmrept_link_cmd, &set_attr_eqpt_cmd,
&rtrv_attr_eqpt_cmd, &rtrv_gps_cmd, &ed_user_secu_cmd, 
&act_user_cmd, &canc_user_cmd, &ent_user_secu_cmd, 
&ed_pid_cmd, &rtrv_user_secu_cmd, &rtrv_user_cmd, 
&dlt_user_secu_cmd, 
&rtrv_mgmtip_cmd, &rtrv_topip_cmd, &set_ntpip_cmd, &rtrv_ntpip_cmd, &set_dat_cmd, &rtrv_dat_cmd, 
&rtrv_sysintg_alarm_cmd, &rtrv_aco_led_cmd, &ed_alarmescalating_time_cmd, 
&rtrv_alarmescalating_time_cmd, &rtrv_system_led_cmd, &set_syssw_upgrade_cmd, 
&rtrv_syssw_version_cmd, &rtrv_sysservice_status_cmd, &reset_system_cmd, &ed_clockinput_mode_cmd,
&rtrv_clockinput_mode_cmd, &ed_clockinput_manualselect_cmd, &rtrv_clockinput_manualselect_cmd,
&rtrv_clockinput_priority_cmd, &rtrv_clockinput_status_cmd, &rtrv_clockinput_ssm_cmd, 
&rtrv_card_cmd, &reset_card_cmd, &sw_dx_osc_cmd, &rtrv_link_cmd, &reset_shelf_cmd, 
&rtrv_shelf_cmd, &sw_ant_cmd, &ed_term_cmd, &view_term_cmd,
&rtrv_hdr_cmd, &set_mgmtip_cmd, &set_topip_cmd, &set_aco_com_cmd,
&opr_aco_all_cmd, &ed_clockinput_priority_cmd, &rtrv_osc_cmd,
&ed_link_cmd, &rtrv_ant_cmd, &ed_ant_cmd, &rtrv_emm_cmd,
&ed_emm_th_cmd, &rtrv_emm_th_cmd, &ed_trap_reg_cmd, &set_th_link_cmd, &rtrv_th_link_cmd, 
&rtrv_trap_reg_cmd, &rtrv_pmsched_link_cmd, &rtrv_pm_link_cmd, &ed_timing_output_cmd, 
&rtrv_attr_link_cmd, &set_attr_link_cmd, NULL};

/*********************************************************************
********  tl1CmdTable Resversed
&set_slip_refab_cmd, &set_slip_frame_cmd, 
&rtrv_cond_link_cmd, &rtrv_cond_com_cmd,
&rtrv_alm_link_cmd, &rtrv_alm_all_cmd, 
*********************************************************************/


