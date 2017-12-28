#ifndef _TRAPTYPE_H_
#define _TRAPTYPE_H_

#include "axss_types.h"

#include "traprcd.h"

#define NULL_TL  NULL

#define DEFAULT_STRLEN     30
#define DEFAULT_OIDLEN     20
#define commStrCommString_TPLEN     MAX_COMMSTR_LEN
#define trapRegCommunityStr_TPLEN   MAX_TRAP_COMSTR_LEN
static TRAP_DATA_TP_t  stsSystemIpConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{   TYPE_DATA_32,     4,                                        0},
{   TYPE_DATA_32,     5,                                        0},
{   TYPE_DATA_32,     6,                                        0},
{   TYPE_DATA_32,     7,                                        0},
{   TYPE_DATA_32,     8,                                        0},
};

static TRAP_DATA_TP_t  stsSystemConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{       TYPE_STR,     9,                           DEFAULT_STRLEN},
{       TYPE_STR,    10,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,    11,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{   TYPE_DATA_32,    13,                                        0},
{   TYPE_DATA_32,    15,                                        0},
{   TYPE_DATA_32,    16,                                        0},
};

static TRAP_DATA_TP_t  stsSystemResetNotification_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,    23,                                        0},
};

static TRAP_DATA_TP_t  stsSystemAlarmStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,    12,                                        0},
{   TYPE_DATA_32,    14,                                        0},
{   TYPE_DATA_32,    17,                                        0},
{   TYPE_DATA_32,    18,                                        0},
{   TYPE_DATA_32,    19,                                        0},
};

static TRAP_DATA_TP_t  stsSystemSWVersionChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{       TYPE_STR,    21,                           DEFAULT_STRLEN},
};

static TRAP_DATA_TP_t  stsClockInputChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     4,                                        0},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     6,                                        0},
};

static TRAP_DATA_TP_t  stsClockInputSlipAlarmStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{   TYPE_DATA_32,     5,                                        0},
};

static TRAP_DATA_TP_t  stsClockInputStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
};

static TRAP_DATA_TP_t  stsClockInputSSMConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
};

static TRAP_DATA_TP_t  stsTopConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{   TYPE_DATA_32,     4,                                        0},
{   TYPE_DATA_32,     5,                                        0},
};

static TRAP_DATA_TP_t  stsTopDestConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
};

static TRAP_DATA_TP_t  stsPerfMonManualConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{   TYPE_DATA_32,     4,                                        0},
};

static TRAP_DATA_TP_t  stsCommStrConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{       TYPE_STR,     1,                           DEFAULT_STRLEN},
};

static TRAP_DATA_TP_t  stsShelfAdded_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
};

static TRAP_DATA_TP_t  stsShelfRemoved_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
};

static TRAP_DATA_TP_t  stsCardAdded_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
};

static TRAP_DATA_TP_t  stsCardRemoved_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
};

static TRAP_DATA_TP_t  stsCardStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     7,                                        0},
};

static TRAP_DATA_TP_t  stsOscillatorStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     8,                                        0},
};

static TRAP_DATA_TP_t  stsGpsStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{       TYPE_STR,     9,                           DEFAULT_STRLEN},
{       TYPE_STR,    10,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
};

static TRAP_DATA_TP_t  stsLinkConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{   TYPE_DATA_32,     4,                                        0},
{   TYPE_DATA_32,     5,                                        0},
{   TYPE_DATA_32,     6,                                        0},
{       TYPE_STR,     7,                           DEFAULT_STRLEN},
};

static TRAP_DATA_TP_t  stsLinkDown_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
};

static TRAP_DATA_TP_t  stsLinkUp_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
};

static TRAP_DATA_TP_t  stsAlarmThresholdConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
};

static TRAP_DATA_TP_t  stsLinkAlarmSettingConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
};

static TRAP_DATA_TP_t  stsLinkAlarmStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     3,                                        0},
};

static TRAP_DATA_TP_t  stsOscillatorAlarmSettingConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
};

static TRAP_DATA_TP_t  stsOscillatorAlarmStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     3,                                        0},
};

static TRAP_DATA_TP_t  stsOsPwrAlarmSettingConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
};

static TRAP_DATA_TP_t  stsOsPwrAlarmStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     3,                                        0},
};

static TRAP_DATA_TP_t  stsOutputAlarmSettingConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{   TYPE_DATA_32,     4,                                        0},
};

static TRAP_DATA_TP_t  stsOutputAlarmStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{   TYPE_DATA_32,     5,                                        0},
};

static TRAP_DATA_TP_t  stsOutputTiuSettingConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{   TYPE_DATA_32,     4,                                        0},
{   TYPE_DATA_32,     5,                                        0},
};

static TRAP_DATA_TP_t  stsOutputPortSettingConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
{       TYPE_STR,     4,                           DEFAULT_STRLEN},
};

static TRAP_DATA_TP_t  stsAlarmStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
};

static TRAP_DATA_TP_t  stsOutputCardConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     5,                                        0},
{   TYPE_DATA_32,     6,                                        0},
{   TYPE_DATA_32,     7,                                        0},
{   TYPE_DATA_32,     8,                                        0},
{       TYPE_STR,     9,                           DEFAULT_STRLEN},
};

static TRAP_DATA_TP_t  stsEMMAlarmStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{   TYPE_DATA_32,     3,                                        0},
};

static TRAP_DATA_TP_t  stsEMMAlarmThresholdChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     4,                                        0},
{   TYPE_DATA_32,     5,                                        0},
{   TYPE_DATA_32,     6,                                        0},
{   TYPE_DATA_32,     7,                                        0},
};

static TRAP_DATA_TP_t  stsAntStatusChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
};

static TRAP_DATA_TP_t  stsTrapRegMgrConfigChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
{   TYPE_DATA_32,     2,                                        0},
{       TYPE_STR,     3,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     4,                                        0},
};

static TRAP_DATA_TP_t  stsClockPriChange_TL[] =  {
{       TYPE_STR,     0,                           DEFAULT_STRLEN},
{   TYPE_DATA_32,     0,                                        0},
{   TYPE_DATA_32,     1,                                        0},
};


TRAP_TPLEN_RCD_t trapTpInfo[] =  {
  {    1001,          stsSystemIpConfigChange_TL, NULL, 9},                                
  {    1002,            stsSystemConfigChange_TL, NULL, 8},                                
  {    1003,       stsSystemResetNotification_TL, NULL, 2},                                
  {    1004,       stsSystemAlarmStatusChange_TL, NULL, 6},                                
  {    1005,         stsSystemSWVersionChange_TL, NULL, 2},                                
  {    1101,              stsClockInputChange_TL, NULL, 7},                                
  {    1102, stsClockInputSlipAlarmStatusChange_TL, NULL, 4},                                      
  {    1103,        stsClockInputStatusChange_TL, NULL, 3},                                
  {    1104,     stsClockInputSSMConfigChange_TL, NULL, 4},                                
  {    1105,               stsTopConfigChange_TL, NULL, 7},                                
  {    1106,           stsTopDestConfigChange_TL, NULL, 5},                                
  {    1107,     stsPerfMonManualConfigChange_TL, NULL, 6},                                
  {    1108,           stsCommStrConfigChange_TL, NULL, 3},                                
  {    1201,                    stsShelfAdded_TL, NULL, 2},                                
  {    1202,                  stsShelfRemoved_TL, NULL, 2},                                
  {    1203,                     stsCardAdded_TL, NULL, 4},                                
  {    1204,                   stsCardRemoved_TL, NULL, 4},                                
  {    1205,              stsCardStatusChange_TL, NULL, 5},                                
  {    1301,        stsOscillatorStatusChange_TL, NULL, 4},                                
  {    1401,               stsGpsStatusChange_TL, NULL, 6},                                
  {    1501,              stsLinkConfigChange_TL, NULL, 9},                                
  {    1502,                      stsLinkDown_TL, NULL, 2},                                
  {    1503,                        stsLinkUp_TL, NULL, 2},                                
  {    1601,    stsAlarmThresholdConfigChange_TL, NULL, 5},                                 
  {    1701,  stsLinkAlarmSettingConfigChange_TL, NULL, 4},                                   
  {    1702,         stsLinkAlarmStatusChange_TL, NULL, 4},                                
  {    1801, stsOscillatorAlarmSettingConfigChange_TL, NULL, 4},                                         
  {    1802,   stsOscillatorAlarmStatusChange_TL, NULL, 4},                                  
  {    1901, stsOsPwrAlarmSettingConfigChange_TL, NULL, 4},                                    
  {    1902,        stsOsPwrAlarmStatusChange_TL, NULL, 4},                                
  {    2001, stsOutputAlarmSettingConfigChange_TL, NULL, 6},                                     
  {    2002,       stsOutputAlarmStatusChange_TL, NULL, 6},                                
  {    2003,  stsOutputTiuSettingConfigChange_TL, NULL, 7},                                   
  {    2004, stsOutputPortSettingConfigChange_TL, NULL, 6},                                    
  {    2101,             stsAlarmStatusChange_TL, NULL, 5},                                
  {    2201,        stsOutputCardConfigChange_TL, NULL, 9},                                
  {    2301,          stsEMMAlarmStatusChange_TL, NULL, 5},                                
  {    2302,       stsEMMAlarmThresholdChange_TL, NULL, 7},                                
  {    2401,               stsAntStatusChange_TL, NULL, 3},                                
  {    2501,        stsTrapRegMgrConfigChange_TL, NULL, 6},                                
  {    2502,                stsClockPriChange_TL, NULL, 3},                                
  {       0,                             NULL_TL, NULL, 0}                                
};

U32_t ttlTrapCnt = sizeof(trapTpInfo)/sizeof(TRAP_TPLEN_RCD_t) - 1;

#endif