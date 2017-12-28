/*
****************************************************************
*                      Magic Pixel Inc.
*
*    Copyright 2004, Magic Pixel Inc., HsinChu, Taiwan
*                    All rights reserved.
*
*
*
* Filename:    REGFILE.H
*
* Programmer:    Alex Tsai
*                MPX E120 division
*
* Created: 10/27/2004
*
* Description: Structure definition for registers inside MP321
*
*
* Change History (most recent first):
*     <1>     10/27/2004    Alex Tsai    first file
****************************************************************
*/
#ifndef __REGFILE_H
#define __REGFILE_H

#define REGISTER_BASE    0xA8000000L
#define FLASH_BASE       0xBC000000L

/*
**************************************************
*     define the SIU registers
**************************************************
*/
#define SIU_BASE    (REGISTER_BASE + 0x00000000L)

typedef struct {
    volatile DWORD SIU_REG_00;          // 0_0000
    volatile DWORD SIU_REG_01;          // 0_0004
    volatile DWORD SIU_REG_02;          // 0_0008
    volatile DWORD SIU_REG_03;          // 0_000C
    volatile DWORD SIU_REG_04;          // 0_0010
    volatile DWORD SIU_REG_05;          // 0_0014
    volatile DWORD SIU_REG_06;          // 0_0018
    volatile DWORD SIU_REG_07;          // 0_001C
    volatile DWORD SIU_REG_08;          // 0_0020
    volatile DWORD SIU_REG_09;          // 0_0024
    volatile DWORD SIU_REG_10;          // 0_0028
    volatile DWORD SIU_REG_11;          // 0_002C
    volatile DWORD SIU_REG_12;          // 0_0030
    volatile DWORD SIU_REG_13;          // 0_0034
    volatile DWORD SIU_REG_14;          // 0_0038
    volatile DWORD SIU_REG_15;          // 0_003C
    volatile DWORD SIU_REG_16;          // 0_0040
    volatile DWORD Reserved1[47];       // 0_0044 ~ 0_00FC
    volatile DWORD I2C_WSTART_TIME;     // 0_0100
    volatile DWORD I2C_REG1;            // 0_0104
    volatile DWORD I2C_REG2;            // 0_0108
    volatile DWORD I2C_REG3;            // 0_010C
    volatile DWORD I2C_REG4;            // 0_0110
    volatile DWORD I2C_REG5;            // 0_0114
    volatile DWORD I2C_BUF0;            // 0_0118
    volatile DWORD I2C_BUF1;            // 0_011C
    volatile DWORD I2C_BUF2;            // 0_0120
    volatile DWORD I2C_BUF3;            // 0_0124
    volatile DWORD I2C_BUF4;            // 0_0128
    volatile DWORD Reserved2[950];      // 0_012C ~ 0_1000    
    volatile DWORD SIU_REG_30;          // 0_1004
    volatile DWORD SIU_REG_31;          // 0_1008
    volatile DWORD SIU_REG_32;          // 0_100C
    volatile DWORD SIU_REG_33;          // 0_1010
    volatile DWORD SIU_REG_34;          // 0_1014
    volatile DWORD SIU_REG_35;          // 0_1018
    volatile DWORD SIU_REG_36;          // 0_101C
    volatile DWORD SIU_REG_37;          // 0_1020
    volatile DWORD SIU_REG_38;          // 0_1024
    volatile DWORD SIU_REG_39;          // 0_1028
    volatile DWORD SIU_REG_40;          // 0_102C
    volatile DWORD SIU_REG_41;          // 0_1030
    volatile DWORD SIU_REG_42;          // 0_1034
    volatile DWORD SIU_REG_43;          // 0_1038
    volatile DWORD SIU_REG_44;          // 0_103C
    volatile DWORD SIU_REG_45;          // 0_1040
    volatile DWORD SIU_REG_46;          // 0_1044
    volatile DWORD SIU_REG_47;          // 0_1048
    volatile DWORD SIU_REG_48[12];      // 0_104C ~ 0_1078
    volatile DWORD Reserved3[33];       // 0_107C ~ 0_10FC
    volatile DWORD STEP0_R0;            // 0_1100
    volatile DWORD STEP0_R1;            // 0_1104
    volatile DWORD STEP0_R2;            // 0_1108
    volatile DWORD STEP0_R3;            // 0_110C
    volatile DWORD STEP0_R4;            // 0_1110
    volatile DWORD STEP0_R5;            // 0_1114
    volatile DWORD STEP0_R6;            // 0_1118
    volatile DWORD STEP0_R7;            // 0_111C
    volatile DWORD STEP1_R0;            // 0_1120
    volatile DWORD STEP1_R1;            // 0_1124
    volatile DWORD STEP1_R2;            // 0_1128
    volatile DWORD STEP1_R3;            // 0_112C
    volatile DWORD STEP1_R4;            // 0_1130
    volatile DWORD STEP1_R5;            // 0_1134
    volatile DWORD STEP1_R6;            // 0_1138
    volatile DWORD STEP1_R7;            // 0_113C
    volatile DWORD PWM0_R0;             // 0_1140
    volatile DWORD PWM0_R1;             // 0_1144
    volatile DWORD PWM0_R2;             // 0_1148
    volatile DWORD PWM0_R3;             // 0_114C
    volatile DWORD PWM0_R4;             // 0_1150
    volatile DWORD PWM0_R5;             // 0_1154
    volatile DWORD PWM0_R6;             // 0_1158
    volatile DWORD PWM0_R7;             // 0_115C
    volatile DWORD PWM1_R0;             // 0_1160
    volatile DWORD PWM1_R1;             // 0_1164
    volatile DWORD PWM1_R2;             // 0_1168
    volatile DWORD PWM1_R3;             // 0_116C
    volatile DWORD PWM1_R4;             // 0_1170
    volatile DWORD PWM1_R5;             // 0_1174
    volatile DWORD PWM1_R6;             // 0_1178
    volatile DWORD PWM1_R7;             // 0_117C
    volatile DWORD COUNT0_R0;           // 0_1180
    volatile DWORD COUNT0_R1;           // 0_1184
    volatile DWORD COUNT1_R0;           // 0_1188
    volatile DWORD COUNT1_R1;           // 0_118C
    volatile DWORD TRIGCTRL;            // 0_1190
    volatile DWORD INTRCTRL;            // 0_1194
    volatile DWORD TGPIO_DAT;           // 0_1198
    volatile DWORD TGPIO_CFG;           // 0_119C
    volatile DWORD Reserved4[152];      // 0_11A0 ~ 0_13FC
    volatile DWORD SIU_REG_80;          // 0_1400
    volatile DWORD SIU_REG_81;          // 0_1404 ~ 0_1408
    volatile DWORD SIU_REG_82;          // 0_1408 ~ 0_140C
    volatile DWORD SIU_REG_83;          // 0_140C ~ 0_1410
    volatile DWORD SIU_REG_84;          // 0_1410 ~ 0_1414
    volatile DWORD SIU_REG_85;          // 0_1414 ~ 0_1418
    volatile DWORD SIU_REG_86;          // 0_1418 ~ 0_141C
    volatile DWORD SIU_REG_87;          // 0_141C ~ 0_1420
    volatile DWORD SIU_REG_88;          // 0_1420 ~ 0_1424
    volatile DWORD SIU_REG_89;          // 0_1424 ~ 0_1428
    volatile DWORD SIU_REG_90;          // 0_1428 ~ 0_142C
    volatile DWORD SIU_REG_91;          // 0_142C ~ 0_1430
    volatile DWORD SIU_REG_92;          // 0_1430 ~ 0_1434
    volatile DWORD SIU_REG_93;          // 0_1434 ~ 0_1438
    volatile DWORD SIU_REG_94;          // 0_1438 ~ 0_143C
    volatile DWORD SIU_REG_95;          // 0_143C ~ 0_1440
    volatile DWORD SIU_REG_96;          // 0_1440 ~ 0_1444
    volatile DWORD Reserved5[239];      // 0_1444 ~ 0_17FC
    volatile DWORD SIU_PAGE_0;          // 0_1800 
    volatile DWORD SIU_PAGE_1;          // 0_1804
    volatile DWORD SIU_PAGE_2;          // 0_1808 
    volatile DWORD SIU_PAGE_3;          // 0_180C
    volatile DWORD SIU_PAGE_4;          // 0_1810 
    volatile DWORD SIU_PAGE_5;          // 0_1814 
    volatile DWORD SIU_PAGE_6;          // 0_1818 
    volatile DWORD SIU_PAGE_7;          // 0_181C 
    volatile DWORD SIU_PAGE_8;          // 0_1820 
    volatile DWORD SIU_PAGE_9;          // 0_1824 
    
    
} STSIUREG;

/*
**************************************************
*     define the IPU registers
**************************************************
*/
//#define IPU_BASE    (REGISTER_BASE + 0x00004000L)
#define IPU_BASE    (REGISTER_BASE + 0x00010000L)

typedef struct {
    volatile DWORD IP_CFG;              // 0_4000
    volatile DWORD IP_STAT;             // 0_4004
    volatile DWORD IP_IC;               // 0_4008
    volatile DWORD IP_IM;               // 0_400C
    volatile DWORD IP_IPR_0;            // 0_4010
    volatile DWORD IP_IPR_1;            // 0_4014
    volatile DWORD IP_IPR_2;            // 0_4018
    volatile DWORD IP_IPW0_0;           // 0_401C
    volatile DWORD IP_IPW0_1;           // 0_4020
    volatile DWORD IP_IPW0_2;           // 0_4024
    volatile DWORD IP_IPW0_3;           // 0_4028
    volatile DWORD IP_IPW0_4;           // 0_402C
    volatile DWORD IP_IPW_ICSC_0;       // 0_4030
    volatile DWORD IP_IPW_ICSC_1;       // 0_4034
    volatile DWORD IP_IPW_ICSC_2;       // 0_4038
    volatile DWORD IP_IPW_ICSC_3;       // 0_403C
    volatile DWORD IP_IPW_ICSC_4;       // 0_4040
    volatile DWORD IP_IPW_ICSC_5;       // 0_4044
    volatile DWORD IP_IPW1_0;           // 0_4048
    volatile DWORD IP_IPW1_1;           // 0_404C
    volatile DWORD IP_IPW1_2;           // 0_4050
    volatile DWORD IP_IPW1_3;           // 0_4054
    volatile DWORD IP_IPW1_4;           // 0_4058
    volatile DWORD IP_CLRINT;           // 0_405C
    volatile DWORD IP_CLRINT_1;         // 0_4060
    volatile DWORD IP_SLPC_0;           // 0_4064
    volatile DWORD IP_SLPC_1;           // 0_4068
    volatile DWORD IP_EDGEN;            // 0_406C
    volatile DWORD IP_ICSC_0;           // 0_4070
    volatile DWORD IP_ICSC_1;           // 0_4074
    volatile DWORD IP_ICSC_2;           // 0_4078
    volatile DWORD IP_ICSC_3;           // 0_407C
    volatile DWORD IP_ICSC_4;           // 0_4080
    volatile DWORD IP_ICSC_5;           // 0_4084
    volatile DWORD IP_WBGC_0;           // 0_4088
    volatile DWORD IP_WBGC_1;           // 0_408C
    volatile DWORD IP_CC_0;             // 0_4090
    volatile DWORD IP_CC_1;             // 0_4094
    volatile DWORD IP_CC_2;             // 0_4098
    volatile DWORD IP_CC_3;             // 0_409C
    volatile DWORD IP_CC_4;             // 0_40A0
    volatile DWORD IP_CC_5;             // 0_40A4
    volatile DWORD IP_CC_6;             // 0_40A8
    volatile DWORD IP_BC_0;             // 0_40AC
    volatile DWORD IP_BC_1;             // 0_40B0
    volatile DWORD IP_GC;               // 0_40B4
    volatile DWORD IP_RGBGM;            // 0_40B8
    volatile DWORD IP_RGMT_0;           // 0_40BC
    volatile DWORD IP_RGMT_1;           // 0_40C0
    volatile DWORD IP_RGMT_2;           // 0_40C4
    volatile DWORD IP_RGMT_3;           // 0_40C8
    volatile DWORD IP_RGMT_4;           // 0_40CC
    volatile DWORD IP_RGMT_5;           // 0_40D0
    volatile DWORD IP_RGMT_6;           // 0_40D4
    volatile DWORD IP_RGMT_7;           // 0_40D8
    volatile DWORD IP_GGMT_0;           // 0_40DC
    volatile DWORD IP_GGMT_1;           // 0_40E0
    volatile DWORD IP_GGMT_2;           // 0_40E4
    volatile DWORD IP_GGMT_3;           // 0_40E8
    volatile DWORD IP_GGMT_4;           // 0_40EC
    volatile DWORD IP_GGMT_5;           // 0_40F0
    volatile DWORD IP_GGMT_6;           // 0_40F4
    volatile DWORD IP_GGMT_7;           // 0_40F8
    volatile DWORD IP_BGMT_0;           // 0_40FC
    volatile DWORD IP_BGMT_1;           // 0_4100
    volatile DWORD IP_BGMT_2;           // 0_4104
    volatile DWORD IP_BGMT_3;           // 0_4108
    volatile DWORD IP_BGMT_4;           // 0_410C
    volatile DWORD IP_BGMT_5;           // 0_4110
    volatile DWORD IP_BGMT_6;           // 0_4114
    volatile DWORD IP_BGMT_7;           // 0_4118
    volatile DWORD IP_CSC_0;            // 0_411C
    volatile DWORD IP_CSC_1;            // 0_4120
    volatile DWORD IP_CSC_2;            // 0_4124
    volatile DWORD IP_CSC_3;            // 0_4128
    volatile DWORD IP_CSC_4;            // 0_412C
    volatile DWORD IP_CSC_5;            // 0_4130
    volatile DWORD IP_CLRADJ_0;         // 0_4134
    volatile DWORD IP_CLRADJ_1;         // 0_4138
    volatile DWORD IP_CLRADJ_2;         // 0_413C
    volatile DWORD IP_YGM;              // 0_4140
    volatile DWORD IP_YGMT_0;           // 0_4144
    volatile DWORD IP_YGMT_1;           // 0_4148
    volatile DWORD IP_YGMT_2;           // 0_414C
    volatile DWORD IP_YGMT_3;           // 0_4150
    volatile DWORD IP_YGMT_4;           // 0_4154
    volatile DWORD IP_YGMT_5;           // 0_4158
    volatile DWORD IP_YGMT_6;           // 0_415C
    volatile DWORD IP_YGMT_7;           // 0_4160
    volatile DWORD IP_MNSC_0;           // 0_4164
    volatile DWORD IP_MNSC_1;           // 0_4168
    volatile DWORD IP_MNSC_2;           // 0_416C
    volatile DWORD IP_MNSC_3;           // 0_4170
    volatile DWORD IP_MNSC_4;           // 0_4174
    volatile DWORD IP_SUBSC;            // 0_4178
    volatile DWORD IP_OVLY_0;           // 0_417C
    volatile DWORD IP_OVLY_1;           // 0_4180
    volatile DWORD IP_OVLY_2;           // 0_4184
    volatile DWORD IP_OVLY_3;           // 0_4188
    volatile DWORD Reserved1[29];       // 0_418C ~ 0_41FC
    volatile DWORD IP_IMST;             // 0_4200
    volatile DWORD IP_AECFG;            // 0_4204
    volatile DWORD IP_AEWNCFG_0;        // 0_4208
    volatile DWORD IP_AEWNCFG_1;        // 0_420C
    volatile DWORD IP_AEWNCFG_2;        // 0_4210
    volatile DWORD IP_AEWNCFG_3;        // 0_4214
    volatile DWORD IP_AEWNCFG_4;        // 0_4218
    volatile DWORD IP_AEWNCFG_5;        // 0_421C
    volatile DWORD IP_AEWNCFG_6;        // 0_4220
    volatile DWORD IP_AEWNCFG_7;        // 0_4224
    volatile DWORD IP_AEWNCFG_8;        // 0_4228
    volatile DWORD IP_AEWNCFG_9;        // 0_422C
    volatile DWORD IP_AWBCFG_0;         // 0_4230
    volatile DWORD IP_AWBCFG_1;         // 0_4234
    volatile DWORD IP_AWBCFG_2;         // 0_4238
    volatile DWORD IP_AWBCFG_3;         // 0_423C
    volatile DWORD IP_AWBCFG_4;         // 0_4240
    volatile DWORD IP_HSTCFG;           // 0_4244
    volatile DWORD IP_AFCFG;            // 0_4248
    volatile DWORD IP_AFWNCFG_0;        // 0_424C
    volatile DWORD IP_AFWNCFG_1;        // 0_4250
    volatile DWORD IP_AFWNCFG_2;        // 0_4254
    volatile DWORD IP_AFWNCFG_3;        // 0_4258
    volatile DWORD IP_AFWNCFG_4;        // 0_425C
    volatile DWORD IP_AFWNCFG_5;        // 0_4260
    volatile DWORD IP_AFWNCFG_6;        // 0_4264
    volatile DWORD IP_AFWNCFG_7;        // 0_4268
    volatile DWORD IP_AFWNCFG_8;        // 0_426C
    volatile DWORD IP_AFWNCFG_9;        // 0_4270
    volatile DWORD Reserved2[3];        // 0_4274 ~ 0_427C
    volatile DWORD IP_LAVG_00;          // 0_4280
    volatile DWORD IP_LAVG_01;          // 0_4284
    volatile DWORD IP_LAVG_02;          // 0_4288
    volatile DWORD IP_LAVG_03;          // 0_428C
    volatile DWORD IP_LAVG_04;          // 0_4290
    volatile DWORD IP_LAVG_10;          // 0_4294
    volatile DWORD IP_LAVG_11;          // 0_4298
    volatile DWORD IP_LAVG_12;          // 0_429C
    volatile DWORD IP_LAVG_13;          // 0_42A0
    volatile DWORD IP_LAVG_14;          // 0_42A4
    volatile DWORD IP_LAVG_20;          // 0_42A8
    volatile DWORD IP_LAVG_21;          // 0_42AC
    volatile DWORD IP_LAVG_22;          // 0_42B0
    volatile DWORD IP_LAVG_23;          // 0_42B4
    volatile DWORD IP_LAVG_24;          // 0_42B8
    volatile DWORD IP_LAVG_30;          // 0_42BC
    volatile DWORD IP_LAVG_31;          // 0_42C0
    volatile DWORD IP_LAVG_32;          // 0_42C4
    volatile DWORD IP_LAVG_33;          // 0_42C8
    volatile DWORD IP_LAVG_34;          // 0_42CC
    volatile DWORD IP_LAVG_40;          // 0_42D0
    volatile DWORD IP_LAVG_41;          // 0_42D4
    volatile DWORD IP_LAVG_42;          // 0_42D8
    volatile DWORD IP_LAVG_43;          // 0_42DC
    volatile DWORD IP_LAVG_44;          // 0_42E0
    volatile DWORD Reserved3[3];        // 0_42E4 ~ 0_42EC
    volatile DWORD IP_RGB_CNT;          // 0_42F0
    volatile DWORD IP_R_ACC;            // 0_42F4
    volatile DWORD IP_G_ACC;            // 0_42F8
    volatile DWORD IP_B_ACC;            // 0_42FC
    volatile DWORD IP_HST_0;            // 0_4300
    volatile DWORD IP_HST_1;            // 0_4304
    volatile DWORD IP_HST_2;            // 0_4308
    volatile DWORD IP_HST_3;            // 0_430C
    volatile DWORD IP_HST_4;            // 0_4310
    volatile DWORD IP_HST_5;            // 0_4314
    volatile DWORD IP_HST_6;            // 0_4318
    volatile DWORD IP_HST_7;            // 0_431C
    volatile DWORD Reserved4[4];        // 0_4320 ~ 0_432C
    volatile DWORD IP_FVACC_0;          // 0_4330
    volatile DWORD IP_FVACC_1;          // 0_4334
    volatile DWORD IP_FVACC_2;          // 0_4338
    volatile DWORD IP_FVACC_3;          // 0_433C
    volatile DWORD IP_FVACC_4;          // 0_4340
    volatile DWORD Reserved5[303];      // 0_4344 ~ 0_47FC
    volatile DWORD IP_OVLY_LUT[256];    // 0_4800 ~ 0_4BFC
} STIPUREG;

/*
**************************************************
*     define the IDU registers
**************************************************
*/
//#define IDU_BASE    (REGISTER_BASE + 0x00008000L)
#define IDU_BASE    (REGISTER_BASE + 0x00020000L)
typedef struct {
    volatile DWORD ID_DISP_ON_REG;      // 0_8000
    volatile DWORD ID_DISP_STAT_REG;    // 0_8004
    volatile DWORD ID_IC_REG;           // 0_8008
    volatile DWORD ID_IM_REG;           // 0_800C
    volatile DWORD ID_PSZ_REG;          // 0_8010
    volatile DWORD ID_PIF_REG;          // 0_8014
    volatile DWORD Reserved1[2];        // 0_8018 ~ 0_801C
    volatile DWORD ID_HTMG_REG;         // 0_8020
    volatile DWORD ID_VTMG_REG;         // 0_8024
    volatile DWORD Reserved2[2];        // 0_8028 ~ 0_802C
    volatile DWORD ID_MPU_TMG_REG;      // 0_8030
    volatile DWORD ID_MPU_CMD_REG;      // 0_8034
    volatile DWORD ID_MPU_PAR_REG;      // 0_8038
    volatile DWORD ID_MPU_CFG_REG;      // 0_803C
    volatile DWORD ID_MW_RU_REG;        // 0_8040
    volatile DWORD ID_MW_ON_REG;        // 0_8044
    volatile DWORD ID_MW_CFG_REG;       // 0_8048
    volatile DWORD ID_MW_VSP_REG;       // 0_804C
    volatile DWORD ID_MW_NDW_ON_REG;    // 0_8050
    volatile DWORD ID_MW_NDW_SA_REG;    // 0_8054
    volatile DWORD ID_MW_NDW_EA_REG;    // 0_8058
    volatile DWORD ID_MW_NDW_FILL_REG;  // 0_805C
    volatile DWORD Reserved3[4];        // 0_8060 ~ 0_806C
    volatile DWORD ID_SW_RU_REG;        // 0_8070
    volatile DWORD ID_SW_ON_REG;        // 0_8074
    volatile DWORD ID_SW_CFG_REG;       // 0_8078
    volatile DWORD ID_SW_VSP_REG;       // 0_807C
    volatile DWORD ID_SW_DSP_REG;       // 0_8080
    volatile DWORD ID_SW_DSZ_REG;       // 0_8084
    volatile DWORD Reserved4[2];        // 0_8088 ~ 0_808C
    volatile DWORD ID_ICSC_R_REG;       // 0_8090
    volatile DWORD ID_ICSC_G_REG;       // 0_8094
    volatile DWORD ID_ICSC_B_REG;       // 0_8098
    volatile DWORD Reserved5;           // 0_809C
    volatile DWORD ID_MIX_MODE_REG;     // 0_80A0
    volatile DWORD ID_MIX_BG_REG;       // 0_80A4
    volatile DWORD ID_MIX_KEY_REG;      // 0_80A8
    volatile DWORD ID_MIX_ALPHA_REG;    // 0_80AC
    volatile DWORD Reserved6[4];        // 0_80B0 ~ 0_80BC
    volatile DWORD ID_GM_CFG_REG;       // 0_80C0
    volatile DWORD ID_GMT_R0_REG;       // 0_80C4
    volatile DWORD ID_GMT_R1_REG;       // 0_80C8
    volatile DWORD ID_GMT_R2_REG;       // 0_80CC
    volatile DWORD ID_GMT_R3_REG;       // 0_80D0
    volatile DWORD ID_GMT_R4_REG;       // 0_80D4
    volatile DWORD ID_GMT_G0_REG;       // 0_80D8
    volatile DWORD ID_GMT_G1_REG;       // 0_80DC
    volatile DWORD ID_GMT_G2_REG;       // 0_80E0
    volatile DWORD ID_GMT_G3_REG;       // 0_80E4
    volatile DWORD ID_GMT_G4_REG;       // 0_80E8
    volatile DWORD ID_GMT_B0_REG;       // 0_80EC
    volatile DWORD ID_GMT_B1_REG;       // 0_80F0
    volatile DWORD ID_GMT_B2_REG;       // 0_80F4
    volatile DWORD ID_GMT_B3_REG;       // 0_80F8
    volatile DWORD ID_GMT_B4_REG;       // 0_80FC
    volatile DWORD Reserved7[4];        // 0_8100 ~ 0_810C
    volatile DWORD ID_DTHR_CFG_REG;     // 0_8110
    volatile DWORD ID_DTHR_PTRN_0_REG;  // 0_8114
    volatile DWORD ID_DTHR_PTRN_1_REG;  // 0_8118
    volatile DWORD Reserved8;           // 0_811C
    volatile DWORD ID_BYPASS_REG;       // 0_8120
    volatile DWORD Reserved9[183];      // 0_8124 ~ 0_83FC
    volatile DWORD ID_PALETTE[256];     // 0_8400 ~ 0_87FC
} STIDUREG;

/*
**************************************************
*     define the JPG registers
**************************************************
*/
//#define JPG_BASE    (REGISTER_BASE + 0x0000C000L)
#define JPG_BASE    (REGISTER_BASE + 0x00000000L) //NOT USED
typedef struct {
    volatile DWORD JPEG_CONFIG;         // 0_C000
    volatile DWORD JPEG_CONTROL;        // 0_C004
    volatile DWORD IO_SEL;              // 0_C008
    volatile DWORD JPEG_BS_OFFSET;      // 0_C00C
    volatile DWORD ENC_BS_BYTENUM;      // 0_C010
    volatile DWORD Reserved1[59];       // 0_C014 ~ 0_C0FC
    volatile DWORD CODEC_CONFIG0;       // 0_C100
    volatile DWORD CODEC_CONFIG1;       // 0_C104
    volatile DWORD CODEC_CONFIG2;       // 0_C108
    volatile DWORD CODEC_CONFIG3;       // 0_C10C
    volatile DWORD CODEC_CONFIG4;       // 0_C110
    volatile DWORD CODEC_CONFIG5;       // 0_C114
    volatile DWORD CODEC_CONFIG6;       // 0_C118
    volatile DWORD CODEC_CONFIG7;       // 0_C11C
    volatile DWORD Reserved2[56];       // 0_C120 ~ 0_C1FC
    volatile DWORD Q_TAB0[64];          // 0_C200 ~ 0_C2FC
    volatile DWORD Q_TAB1[64];          // 0_C300 ~ 0_C3FC
    volatile DWORD ENC_HUFF_TAB[384];   // 0_C400 ~ 0_C9FC
    volatile DWORD MIN_TAB0[4];         // 0_CA00 ~ 0_CA0C
    volatile DWORD MIN_TAB1[4];         // 0_CA10 ~ 0_CA1C
    volatile DWORD MIN_TAB2[4];         // 0_CA20 ~ 0_CA2C
    volatile DWORD MIN_TAB3[4];         // 0_CA30 ~ 0_CA3C
    volatile DWORD Reserved3[48];       // 0_CA40 ~ 0_CAFC
    volatile DWORD BASE_TAB[64];        // 0_CB00 ~ 0_CBFC
} STJPGREG;

/*
**************************************************
*     define the MP4 registers
**************************************************
*/
//#define MP4_BASE    (REGISTER_BASE + 0x00010000L)
#define MP4_BASE    (REGISTER_BASE + 0x00000000L)

typedef struct {
    volatile DWORD MPEG4_STATUS;        // 1_0000
    volatile DWORD MPEG4_COMMAND;       // 1_0004
    volatile DWORD ENC_TIME_CONFIG;     // 1_0008
    volatile DWORD MASK_MP4_IRQ;        // 1_000C
    volatile DWORD CLEAR_MP4_IRQ;       // 1_0010
    volatile DWORD ACTIVATE_MP4;        // 1_0014
    volatile DWORD ENC_VOP_CONFIG0;     // 1_0018
    volatile DWORD ENC_VOP_CONFIG1;     // 1_001C
    volatile DWORD H263_CONFIG;         // 1_0020
    volatile DWORD BPKBUF_CONFIG;       // 1_0024
    volatile DWORD ENABLE_DEBLOCK;      // 1_0028
    volatile DWORD DEBLOCK_THRESH;      // 1_002C
    volatile DWORD BPK_ENC_BYTENUM;     // 1_0030
    volatile DWORD ENC_GOV_TIME;        // 1_0034
    volatile DWORD DEC_PARAMETER1;      // 1_0038
    volatile DWORD DEC_PARAMETER2;      // 1_003C
    volatile DWORD DEC_PARAMETER3;      // 1_0040
    volatile DWORD DEC_PARAMETER4;      // 1_0044
} STMP4REG;

/*
**************************************************
*     define the DRAI registers
**************************************************
*/
//#define DRAI_BASE    (REGISTER_BASE + 0x00014000L)
#define DRAI_BASE    (REGISTER_BASE + 0x00030000L)
typedef struct {
    volatile DWORD SIU_START_ROW0;      // 1_4000
    volatile DWORD IMG_BUF_POINTER;     // 1_4004
    volatile DWORD ISS1_START_ROW0;     // 1_4008
    volatile DWORD ISS2_START_ROW0;     // 1_400C
    volatile DWORD ISS3_START_ROW0;     // 1_4010
    volatile DWORD IDU_START_ROW;       // 1_4014
    volatile DWORD JPG_START_ROW;       // 1_4018
    volatile DWORD JPGB_START_ROW;      // 1_401C
    volatile DWORD MP4_BUF_POINTER;     // 1_4020
    volatile DWORD MP4_START_ROW0;      // 1_4024
    volatile DWORD MEW_START_ROW;       // 1_4028
    volatile DWORD DEB_START_ROW;       // 1_402C
    volatile DWORD MISC_FRAME_WIDTH0;   // 1_4030
    volatile DWORD MISC_FRAME_WIDTH1;   // 1_4034
    volatile DWORD ABUF_START_ROW;      // 1_4038
    volatile DWORD BBUF_START_ROW;      // 1_403C
    volatile DWORD CBUF_START_ROW;      // 1_4040
    volatile DWORD BSBUF_SETTING;       // 1_4044
    volatile DWORD ADG_MISC_SETTING;    // 1_4048
    volatile DWORD SIU_READ_START_ROW;  // 1_404C
    volatile DWORD SDRAM_POWER_DOWN;    // 1_4050
    volatile DWORD SDRAM_SETTING;       // 1_4054
    volatile DWORD MISC_FRAME_WIDTH2;   // 1_4058
    volatile DWORD CHASBUF_SETTING0;    // 1_405C
    volatile DWORD SIU_START_ROW1;      // 1_4060
    volatile DWORD ISS1_START_ROW1;     // 1_4064
    volatile DWORD ISS2_START_ROW1;     // 1_4068
    volatile DWORD ISS3_START_ROW1;     // 1_406C
    volatile DWORD MP4_START_ROW1;      // 1_4070
    volatile DWORD I2SBUF_START_ROW;    // 1_4074
    volatile DWORD USBBUF_START_ROW;    // 1_4078
    volatile DWORD MCIBUF_SETTING;      // 1_407C
    volatile DWORD AUD_START_ROW0;      // 1_4080
    volatile DWORD AUD_START_ROW1;      // 1_4084
    volatile DWORD AUD_START_ROW2;      // 1_4088
    volatile DWORD AUD_START_ROW3;      // 1_408C
    volatile DWORD AUDBUF_SETTING;      // 1_4090
    volatile DWORD ISS4_START_ROW;      // 1_4094
    volatile DWORD I2SBUF_SETTING0;     // 1_4098
    volatile DWORD CPU2HIU_BS_ADDR;     // 1_409C
    volatile DWORD HIU_BS_ADDR;         // 1_40A0
    volatile DWORD CPU2BPK_BS_ADDR;     // 1_40A4
    volatile DWORD BPK_BS_ADDR;         // 1_40A8
    volatile DWORD I2SBUF_SETTING1;     // 1_40AC
    volatile DWORD BPKPRG_START_ROW;    // 1_40B0
    volatile DWORD Reserved1[19];       // 1_40B4 ~ 1_40FC
    volatile DWORD RG_MD_IDLEDIR;       // 1_4100
    volatile DWORD RG_BT_BTC;           // 1_4104
    volatile DWORD Reserved2[14];       // 1_4108 ~ 1_413C   
    volatile DWORD RG_SIUY0_ext;        // 1_4140
    volatile DWORD RG_SIUUV0_ext;       // 1_4144
    volatile DWORD RG_SIUY1_ext;        // 1_4148
    volatile DWORD RG_SIUUV1_ext;       // 1_414C
    volatile DWORD Reserved3;           // 1_4150     
    volatile DWORD RG_IPU1Y0_ext;       // 1_4154
    volatile DWORD RG_IPU1UV0_ext;      // 1_4158
    volatile DWORD RG_IPU1Y1_ext;       // 1_415C
    volatile DWORD RG_IPU1UV1_ext;      // 1_4160
    volatile DWORD RG_IPU2Y0_ext;       // 1_4164
    volatile DWORD RG_IPU2UV0_ext;      // 1_4168
    volatile DWORD RG_IPU2Y1_ext;       // 1_416C
    volatile DWORD RG_IPU2UV1_ext;      // 1_4170
    volatile DWORD RG_IPU3Y0_ext;       // 1_4174
    volatile DWORD RG_IPU3UV0_ext;      // 1_4178
    volatile DWORD RG_IPU3Y1_ext;       // 1_417C
    volatile DWORD RG_IPU3UV1_ext;      // 1_4180
    volatile DWORD RG_IPU4_ext;         // 1_4184
    volatile DWORD RG_IDU1_ext;         // 1_4188
    volatile DWORD RG_IDU2_ext;         // 1_418C
    volatile DWORD RG_IDU3_ext;         // 1_4190
    volatile DWORD RG_IDU4_ext;         // 1_4194
    volatile DWORD RG_JPGY_ext;         // 1_4198
    volatile DWORD RG_JPGUV_ext;        // 1_419C
    volatile DWORD RG_JPGB_ext;         // 1_41A0
    volatile DWORD RG_I2S_EXT;          // 1_41A4
    volatile DWORD RG_MCI_ext;          // 1_41A8
    volatile DWORD RG_BSBUF_ext;        // 1_41AC
    volatile DWORD Reserved4[20];       // 1_41B0 ~ 1_41FC
    volatile DWORD RG_SDRAM_SETTING2;   // 1_4200
    volatile DWORD DRAI_INT;            // 1_4204
} STDRAIREG;

/*
**************************************************
*     define the USB registers
**************************************************
*/
//#define USB_BASE    (REGISTER_BASE + 0x00048000L)
#define USB_BASE    (REGISTER_BASE + 0x00600000L)
typedef struct {
    volatile DWORD Reserved0;           // 1_8000
    volatile DWORD D1SYNC;              // 1_8004
    volatile DWORD DEVICE_ADDRESS;      // 1_8008
    volatile DWORD Reserved1[13];       // 1_800c ~ 1_803c
    volatile DWORD IN_NEEDED;           // 1_8040
    volatile DWORD OUT_NEEDED;          // 1_8044
    volatile DWORD INOUT_ABORT;         // 1_8048
    volatile DWORD USEDMA;              // 1_804c
    volatile DWORD WHICH_EP;            // 1_8050
    volatile DWORD HOWMANY_TRAN;        // 1_8054
    volatile DWORD LASTTRAN_BYTE;       // 1_8058
    volatile DWORD MAX_PKT_SIZE;        // 1_805c
    volatile DWORD EP_NAKORSTALL;       // 1_8060
    volatile DWORD EP_ISO;              // 1_8064
    volatile DWORD FORCE_K;             // 1_8068
    volatile DWORD IRQ_EN;              // 1_806c
    volatile DWORD IRQ_STATUS;          // 1_8070
    volatile DWORD DMA_IDLE;            // 1_8074
    volatile DWORD TIMEOUT_LIMIT;       // 1_8078
    volatile DWORD FRAMENUM;            // 1_807c
    volatile DWORD NMH_EP;              // 1_8080
    volatile DWORD TRAN_COUNT;          // 1_8084
    volatile DWORD SETUP_CLEAR;         // 1_8088
    volatile DWORD EP0_TOGGLESET;       // 1_808c
    volatile DWORD EP0_toggleseten;     // 1_8090
    volatile DWORD DMA_ADDR_OFFSET;     // 1_8094
    volatile DWORD FORCE_SE0;           // 1_8098
    volatile DWORD Reserved2[9];        // 1_809c ~ 1_80bc
    volatile DWORD SETUP_DATA0;         // 1_80c0
    volatile DWORD SETUP_DATA4;         // 1_80c4
    volatile DWORD Reserved3[14];       // 1_80c8 ~ 1_80fc
    volatile DWORD INTERNALSRAM;        // 1_8100
} STUSBREG;

/*
**************************************************
*     define the HIU registers
**************************************************
*/
//#define HIU_BASE    (REGISTER_BASE + 0x0001C000L)
#define HIU_BASE    (REGISTER_BASE + 0x00050000L)
typedef struct {
    volatile DWORD HI_CMDF;     // 1_C000 = CMDF
    volatile DWORD HI_CINT;     // 1_C004 = CINT
    volatile DWORD HI_BINT;     // 1_C008 = SYST
    volatile DWORD HI_CPST;     // 1_C00c = CPST
    volatile DWORD HI_MQ;       // 1_C010
    volatile DWORD GPDAT0;      // 1_C014
    volatile DWORD GPDAT1;      // 1_C018
    volatile DWORD GPCFG0;      // 1_C01c
    volatile DWORD GPCFG1;      // 1_C020
    volatile DWORD GPINT0;      // 1_C024
    volatile DWORD GPINT1;      // 1_C028
    volatile DWORD GPINT2;      // 1_C02c
} STHIUREG;

/*
**************************************************
*     define the CKG registers
**************************************************
*/
//#define CKG_BASE    (REGISTER_BASE + 0x00020800L)
#define CKG_BASE    (REGISTER_BASE + 0x00130000L)
typedef struct {
    volatile DWORD CFG_CPU_CLK;         // 2_0000
    volatile DWORD CPUCK_SELECT;        // 2_0004
    volatile DWORD CFG_UART_CLK;        // 2_0008
    volatile DWORD SEL_UART_SRC;        // 2_000C
    volatile DWORD CFG_DRAI_CLK;        // 2_0010
    volatile DWORD DRAICK_SELECT;       // 2_0014
    volatile DWORD Reserved1[2];        // 2_0018 ~ 2_001C
    volatile DWORD CFG_HIU_CLK;         // 2_0020
    volatile DWORD HIUCK_SELECT;        // 2_0024
    volatile DWORD CFG_JPG_CLK;         // 2_0028
    volatile DWORD JPG_CLK_SELECT;      // 2_002C
    volatile DWORD CFG_MP4_CLK;         // 2_0030
    volatile DWORD Reserved2;           // 2_0034
    volatile DWORD CFG_SIU_CLK;         // 2_0038
    volatile DWORD SIUCK_SELECT;        // 2_003C
    volatile DWORD CFG_ISS_CLK;         // 2_0040
    volatile DWORD ISSCK_SELECT;        // 2_0044
    volatile DWORD CFG_IDU_CLK;         // 2_0048
    volatile DWORD IDUCK_SELECT;        // 2_004C
    volatile DWORD CFG_USB_CLK;         // 2_0050
    volatile DWORD SEL_USB_CLK;         // 2_0054
    volatile DWORD CFG_TIMER1_CLK;      // 2_0058
    volatile DWORD SEL_TIMER1_CLK;      // 2_005C
    volatile DWORD CFG_I2S_CLK;         // 2_0060
    volatile DWORD I2SCK_SELECT;        // 2_0064
    volatile DWORD CFG_ADC_CLK;         // 2_0068
    volatile DWORD Reserved3;           // 2_006C
    volatile DWORD CFG_TIMER2_CLK;      // 2_0070
    volatile DWORD SEL_TIMER2_CLK;      // 2_0074
    volatile DWORD CFG_TIMER3_CLK;      // 2_0078
    volatile DWORD SEL_TIMER3_CLK;      // 2_007C
    volatile DWORD CFG_TIMER4_CLK;      // 2_0080
    volatile DWORD SEL_TIMER4_CLK;      // 2_0084
    volatile DWORD CFG_TIMER5_CLK;      // 2_0088
    volatile DWORD SEL_TIMER5_CLK;      // 2_008C
    volatile DWORD CFG_MCI_CLK;         // 2_0090
    volatile DWORD MCICK_SELECT;        // 2_0094
    volatile DWORD PLL1_FREQUENCY;      // 2_0098
    volatile DWORD PLL1_CONFIG;         // 2_009C
    volatile DWORD PLL2_FREQUENCY;      // 2_00A0
    volatile DWORD PLL2_CONFIG;         // 2_00A4
    volatile DWORD CKGIN_SELECT;        // 2_00A8
    volatile DWORD SEL_WFS_CLK;         // 2_00AC
    volatile DWORD PLL3_FREQUENCY;         // 2_00b0
    volatile DWORD PLL3_CONFIG;            // 2_00b4
    volatile DWORD PLL4_FREQUENCY;         // 2_00b8
    volatile DWORD PLL4_CONFIG;            // 2_00bc
    volatile DWORD CFG_UART1_CLK;         // 2_00C0
    volatile DWORD SEL_UART1_SRC;            // 2_00C4
    volatile DWORD CFG_UART2_CLK;         // 2_00C8
    volatile DWORD SEL_UART2_SRC;            // 2_00CC
    volatile DWORD CFG_UART3_CLK;         // 2_00D0
    volatile DWORD SEL_UART3_SRC;            // 2_00D4
    volatile DWORD CFG_UART4_CLK;         // 2_00D8
    volatile DWORD SEL_UART4_SRC;            // 2_00DC
    volatile DWORD CFG_SPI1_CLK;           // 2_00E0
    volatile DWORD SPI1CK_SELECT;            // 2_00E4
    volatile DWORD CFG_SPI2_CLK;           // 2_00E8
    volatile DWORD SPI2CK_SELECT;            // 2_00EC
    volatile DWORD CFG_I2C_CLK;           // 2_00F0
    volatile DWORD I2CCK_SELECT;            // 2_00F4
    volatile DWORD CFG_I2S1_CLK;           // 2_00F8
    volatile DWORD I2S1CK_SELECT;            // 2_00FC
    volatile DWORD Reserved4          ;           // 2_0100
    volatile DWORD SEL_AHB_CLK;           // 2_0104H
    volatile DWORD Reserved5          ;           // 2_0108
    volatile DWORD SEL_APB_CLK;            // 2_010C
    volatile DWORD CFG_DMA321_CLK;    // 2_0110
    volatile DWORD CFG_DMA2_CLK;       // 2_0114
    volatile DWORD CFG_SDI_CLK;       // 2_0118
    volatile DWORD CFG_G3D_CLK;       // 2_011C
    volatile DWORD Reserved6[4] ;        // 2_0120~2_012C
    volatile DWORD SEL_GPIO_CLK;       // 2_0130
    volatile DWORD SEL_INT_CLK;       	// 2_0134
    volatile DWORD PLL5_FREQUENCY;       // 2_0138
    volatile DWORD PLL5_CONFIG;       	// 2_013C
    
} STCKGREG;

/*
**************************************************
*     define the ICU registers
**************************************************
*/
/*#define ICU_BASE    (REGISTER_BASE + 0x00020100L)

typedef struct {
    volatile DWORD ICU_STATUS;          // 2_0100
    volatile DWORD ICU_MASK;            // 2_0104
    volatile DWORD ICU_BB_INT;          // 2_0108
} STICUREG;*/
//#define ICU_BASE    (REGISTER_BASE + 0x00030A00L)
#define ICU_BASE    (REGISTER_BASE + 0x00240000L)
typedef struct {
    volatile DWORD SRCPND;               // 3_0A00
    volatile DWORD Reserved0;       // 3_0A04
    volatile DWORD INTMOD    ;            // 3_0A08
    volatile DWORD Reserved1;       // 3_0A0C
    volatile DWORD INTMSK;            // 3_0A10
    volatile DWORD Reserved2;       // 3_0A14
    volatile DWORD INPND;            // 3_0A18
    volatile DWORD Reserved3;       // 3_0A1C
    volatile DWORD INTPRT;             // 3_0A20
    volatile DWORD Reserved4[4];       // 3_0A24~3_0A30
    volatile DWORD INTOFS;             // 3_0A34
    volatile DWORD EINTFIL;             // 3_0A38
} STICUREG;
/*
**************************************************
*     define the GPIO registers
**************************************************
*/
//#define GPIO_BASE    (REGISTER_BASE + 0x00020200L)
#define GPIO_BASE    (REGISTER_BASE + 0x00070000L)
typedef struct {
    volatile DWORD GPDAT;               // 2_0200
    volatile DWORD GPCFG;               // 2_0204
    volatile DWORD SGPDAT;              // 2_0208
    volatile DWORD SGPCFG;              // 2_020C
    volatile DWORD VGPDAT0;             // 2_0210
    volatile DWORD VGPCFG0;             // 2_0214
    volatile DWORD VGPDAT1;             // 2_0218
    volatile DWORD VGPCFG1;             // 2_021C
    volatile DWORD AGPDAT;              // 2_0220
    volatile DWORD AGPCFG;              // 2_0224
    volatile DWORD FGPDAT0;             // 2_0228
    volatile DWORD FGPCFG0;             // 2_022C
    volatile DWORD FGPDAT1;             // 2_0230
    volatile DWORD FGPCFG1;             // 2_0234
    volatile DWORD UGPDAT;              // 2_0238
    volatile DWORD UGPCFG;              // 2_023C
    volatile DWORD EGPDAT;              // 2_0240
    volatile DWORD EGPCFG;              // 2_0244
    volatile DWORD GPINT0;              // 2_0248
    volatile DWORD GPINT1;              // 2_024C
    volatile DWORD FGPINT;              // 2_0250
    volatile DWORD SPIGPDAT;            // 2_0254
    volatile DWORD SPIGPCFG;            // 2_0258
    volatile DWORD NFGPDAT;             // 2_025C
    volatile DWORD CFGGPDAT;            // 2_0260
    volatile DWORD CFGGPCFG;            // 2_0264
} STGPIOREG;

/*
**************************************************
*     define the UART registers
**************************************************
*/
//#define UART_BASE    (REGISTER_BASE + 0x00030800L)
#define UART_BASE    (REGISTER_BASE + 0x00200000L)
typedef struct {
    volatile DWORD UART_C;              // 2_0300
    volatile DWORD UART_INT;            // 2_0304
    volatile DWORD UARTDT;              // 2_0308
    volatile DWORD UAFFCN;              // 2_030C
} STUARTREG;

/*
**************************************************
*     define the TIMER registers
**************************************************
*/
#define TIMER0_BASE    (REGISTER_BASE + 0x00020400L)
#define TIMER1_BASE    (REGISTER_BASE + 0x00020420L)
#define TIMER2_BASE    (REGISTER_BASE + 0x00020440L)
#define TIMER3_BASE    (REGISTER_BASE + 0x00020460L)
#define TIMER4_BASE    (REGISTER_BASE + 0x00020480L)

typedef struct {
    volatile DWORD TM_C;                // TIMER_BASE + 0_0000
    volatile DWORD TM_V;                // TIMER_BASE + 0_0004
    volatile DWORD TC_A;                // TIMER_BASE + 0_0008
    volatile DWORD TC_B;                // TIMER_BASE + 0_000C
    volatile DWORD TPC;                 // TIMER_BASE + 0_0010
    volatile DWORD TMOFF;               // TIMER_BASE + 0_0014
    volatile DWORD Reserved[2];         // TIMER_BASE + 0_0018,C
} STTIMERREG;

/*
**************************************************
*     define the I2S registers
**************************************************
*/
//#define I2S_BASE    (REGISTER_BASE + 0x00020500L)
#define I2S0_BASE    (REGISTER_BASE + 0x00100000L)
#define I2S1_BASE    (REGISTER_BASE + 0x00110000L)
typedef struct {
    volatile DWORD I2S_EN_REG;          // 2_0500
    volatile DWORD I2S_CLR_REG;         // 2_0504
    volatile DWORD I2S_IC_REG;          // 2_0508
    volatile DWORD I2S_IM_REG;          // 2_050C
    volatile DWORD I2S_IFCFG_REG;       // 2_0510
    volatile DWORD I2S_FLEN_REG;        // 2_0514
    volatile DWORD I2S_DATCFG_REG;      // 2_0518
    volatile DWORD I2S_DMAADR_REG;      // 2_051C
    volatile DWORD I2S_BCLKCFG_REG;     // 2_0520
    volatile DWORD ADC_GAIN_STATUS_REG; // 2_0524
} STI2SREG;

/*
**************************************************
*     define the MCARD registers
**************************************************
*/
//#define MC_CFCOM_BASE (REGISTER_BASE + 0x00021000) // 0xa802_1000~0xa802_17ff
//#define MC_CFATR_BASE (REGISTER_BASE + 0x00020800) // 0xa802_1800~0xa802_1fff
//#define MCARD_BASE    (REGISTER_BASE + 0x00020700L)  // 0xa802_0700~0xa802_xxxx
#define MC_CFCOM_BASE (REGISTER_BASE + 0x00160000L) // 0xa816_0000~~0xa816_07ff
#define MC_CFATR_BASE (REGISTER_BASE + 0x00160800L) // 0xa816_0800~0xa816_0fff
#define MCARD_BASE    (REGISTER_BASE + 0x00120000L)  // 0xa802_0700~0xa802_xxxx
typedef struct {
    volatile DWORD MCARD_C;             // 2_0700
    volatile DWORD MC_DMARL;            // 2_0704
    volatile DWORD MC_RTM;              // 2_0708
    volatile DWORD MC_WTM;              // 2_070c
    volatile DWORD MC_WDT;              // 2_0710
    volatile DWORD res0[3];                // 2_0714, 2_0718, 2_071c
    volatile DWORD MC_CF_C;                  // 2_0720
    volatile DWORD MC_CF_IC;      		// 2_0024
    volatile DWORD MC_CF_SC; 			// 2_0028
    volatile DWORD res1[5];     		// 2_072c, 2_0730, 2_0734, 2_0738, 2_073c
    volatile DWORD MC_SM_C;             // 2_0740
    volatile DWORD MC_SM_IC;            // 2_0744
    volatile DWORD MC_SM_CMD;           // 2_0748
    volatile DWORD MC_SM_ADR;           // 2_074c
    volatile DWORD MC_SM_DAT;           // 2_0750
    volatile DWORD MC_SM_ECC1;          // 2_0754
    volatile DWORD MC_SM_ECC2;          // 2_0758
    volatile DWORD Reserved1[5];        // 2_075c
    volatile DWORD MC_SD_OP;            // 2_0770
    volatile DWORD MC_SD_IC;            // 2_0774
    volatile DWORD MC_SD_C;             // 2_0778
    volatile DWORD MC_SD_ARG;           // 2_077c
    volatile DWORD MC_SD_RSPA;          // 2_0780
    volatile DWORD MC_SD_RSPB;          // 2_0784
    volatile DWORD MC_SD_RSPC;          // 2_0788
    volatile DWORD MC_SD_RSPD;          // 2_078c
    volatile DWORD MC_SD_SC;            // 2_0790
    volatile DWORD Reserved2[3];        // 2_0794
    volatile DWORD MC_MS_CMD;           // 2_07a0
    volatile DWORD MC_MS_FIFO;          // 2_07a4
    volatile DWORD MC_MS_CRC;           // 2_07a8
    volatile DWORD MC_MS_T0;            // 2_07ac
    volatile DWORD MC_MS_T1;            // 2_07b0
    volatile DWORD MC_MS_STATUS;        // 2_07b4
    volatile DWORD MC_MS_IC;            // 2_07b8
    volatile DWORD Reserved3[6];        // 2_07bc
} STMCARDREG;


//#define ATA_BASE (REGISTER_BASE + 0x00020D00)
#define ATA_BASE (REGISTER_BASE + 0x00150000L)
typedef struct{
    volatile DWORD AtaAstsDctl;     //6  
    volatile DWORD res0;
    volatile DWORD AtaData;			//0
    volatile DWORD AtaErrorFeature; //1 
    volatile DWORD AtaSectorCnt;	//2
    volatile DWORD AtaLbaL;			//3
    volatile DWORD AtaLbaM;			//4
    volatile DWORD AtaLbaH;			//5
    volatile DWORD AtaDevice;		//6
    volatile DWORD AtaStatusCommand;//7
    volatile DWORD res1[8];
    volatile DWORD AtaDmaCtl;
    volatile DWORD AtaPioTim;
    volatile DWORD AtaMdmaTim;
    volatile DWORD AtaUdmaTim;
    volatile DWORD AtaSta;
    volatile DWORD AtaIc;
    volatile DWORD AtaCrc;
} ATA;

typedef struct
{
	volatile BYTE Data;
	volatile BYTE ErrorFeature;
	volatile BYTE SectorCount;
	volatile BYTE Lba0;						// alias to sector number register
} CFATA;
typedef struct
{
	volatile BYTE Lba1;						// alias to cylinder low register
	volatile BYTE Lba2;						// alias to cylinder high register
	volatile BYTE Lba3;						// alias to drive and head register
	volatile BYTE StatusCommand;
	BYTE Res0[6];
	volatile BYTE AltStatus;
	BYTE Res1;	
} CFATA1;

/*
**************************************************
*     define the RTC  registers
**************************************************
*/
//#define RTC_BASE    (REGISTER_BASE + 0x00030B00L)
#define RTC_BASE    (REGISTER_BASE + 0x00250000L)
typedef struct {
    volatile DWORD RTC_WPROT0;             // 3_0B00
    volatile DWORD RTC_WPROT1;            // 3_0B04
    volatile DWORD RTC_WPROT2;            // 3_0B08
    volatile DWORD RTC_RSTN;                 // 3_B0C
    volatile DWORD RTC_CKSEL;               // 3_0B10
    volatile DWORD RTC_CCR;       	     // 3_0B14
    volatile DWORD RTC_CURVAL;            // 3_0B18
    volatile DWORD RTC_ALMVAL;           // 3_0B1C
    volatile DWORD RTC_INTR;               // 3_0B20
   
 /*   volatile DWORD ALM_EN;             // 3_0B00
    volatile DWORD ALM_SEC;            // 3_0B04
    volatile DWORD ALM_MIN;              // 3_0B08
    volatile DWORD ALM_HOUR;              // 3_B0C
    volatile DWORD ALM_DATE;              // 3_0B10
    volatile DWORD ALM_MONTH;       // 3_0B14
    volatile DWORD ALM_YEAR;             // 3_0B18
    volatile DWORD Reserved0;          // 3_0B1C
    volatile DWORD RTC_SEC;            // 3_0B20
    volatile DWORD RTC_MIN;           // 3_0B24
    volatile DWORD RTC_HOUR;           // 3_0B28
    volatile DWORD RTC_DATE;           // 3_0B2C
    volatile DWORD RTC_WEEK;          // 3_0B30
    volatile DWORD RTC_MONTH;          // 3_0B34
    volatile DWORD RTC_YEAR;        	// 3_0B38
    volatile DWORD Reserved1[2];          // 3_0B3C 3_0B40
    volatile DWORD CCR;            		// 3_0B44
    volatile DWORD RTC_RESET ;    	// 3_0B48
    volatile DWORD RTC_RND_RST;     // 3_0B4C
    volatile DWORD RTC_INTR;             // 3_0B50
    volatile DWORD W_PROTECT0;           // 3_B54
    volatile DWORD W_PROTECT1;          // 3_0B58
    volatile DWORD W_PROTECT2;          // 3_0B5C
    volatile DWORD RTC_TEST_MODE;          // 3_0B60
    volatile DWORD RTCK_FREQ_SEL;          // 3_0B64*/
  
} STRTCREG;

/*
**************************************************
*     define the SPI registers
**************************************************
*/

//#define SPI_BASE    (REGISTER_BASE + 0x00048400L)
#define SPI_BASE    (REGISTER_BASE + 0x00610000L)
typedef struct {
    volatile DWORD SPCON;                 // 4_8400
    volatile DWORD SPSTA;                 // 4_8404
    volatile DWORD SPTDAT;              // 4_8408
    volatile DWORD SPRDAT;              // 4_840C
  
} STSPIREG;

/*
**************************************************
*     define the DMA2 registers
**************************************************
*/

//#define DMA2_BASE    (REGISTER_BASE + 0x00030D00L)
#define DMA2_BASE    (REGISTER_BASE + 0x00260000L)
typedef struct {
    volatile DWORD DMA0_ISA;                 // 3_0D00
    volatile DWORD DMA0_IDA;                 // 3_0D04
    volatile DWORD DMA0_CFG0;              // 3_0D08
    volatile DWORD DMA0_CFG1;              // 3_0D0C
    volatile DWORD DMA0_SWT;              // 3_0D10
    volatile DWORD DMA0_STAT;              // 3_0D14
    volatile DWORD DMA0_CSA;              // 3_0D18
    volatile DWORD DMA0_CDA;              // 3_0D1C
    volatile DWORD Reserved0[4];               // 3_0D20~ 3_0D2C
    volatile DWORD DMA1_ISA;                 // 3_0D30
    volatile DWORD DMA1_IDA;                 // 3_0D34
    volatile DWORD DMA1_CFG0;              // 3_0D38
    volatile DWORD DMA1_CFG1;              // 3_0D3C
    volatile DWORD DMA1_SWT;              // 3_0D40
    volatile DWORD DMA1_STAT;              // 3_0D44
    volatile DWORD DMA1_CSA;              // 3_0D48
    volatile DWORD DMA1_CDA;              // 3_0D4C
    volatile DWORD Reserved1[4];               // 3_0D50~ 3_0D5C
    volatile DWORD DMA2_ISA;                 // 3_0D60
    volatile DWORD DMA2_IDA;                 // 3_0D64
    volatile DWORD DMA2_CFG0;              // 3_0D68
    volatile DWORD DMA2_CFG1;              // 3_0D6C
    volatile DWORD DMA2_SWT;              // 3_0D70
    volatile DWORD DMA2_STAT;              // 3_0D74
    volatile DWORD DMA2_CSA;              // 3_0D78
    volatile DWORD DMA2_CDA;              // 3_0D7C
    volatile DWORD Reserved2[4];               // 3_0D80~ 3_0D8C
    volatile DWORD DMA3_ISA;                 // 3_0D90
    volatile DWORD DMA3_IDA;                 // 3_0D94
    volatile DWORD DMA3_CFG0;              // 3_0D98
    volatile DWORD DMA3_CFG1;              // 3_0D9C
    volatile DWORD DMA3_SWT;              // 3_0DA0
    volatile DWORD DMA3_STAT;              // 3_0DA4
    volatile DWORD DMA3_CSA;              // 3_0DA8
    volatile DWORD DMA3_CDA;              // 3_0DAC
    volatile DWORD Reserved3[4];               // 3_0DB0~ 3_0DBC
    volatile DWORD DMA_CFG0;              // 3_0DC0
    volatile DWORD INTEN;              		// 3_0DC4
    volatile DWORD INTST;             		 // 3_0DC8
   
} STDMA2REG;
/*
**************************************************
*     define the Touch Screen Controller  registers
**************************************************
*/

//#define TSC_BASE    (REGISTER_BASE + 0x00030C00L)
#define TSC_BASE    (REGISTER_BASE + 0x001B0000L)
typedef struct {
    volatile DWORD TPC_CON;                   // 0xA8030C00
    volatile DWORD TPC_DRI;                    // 0xA8030C04
    volatile DWORD TPC_LB_WR;               // 0xA8030C08
    volatile DWORD TPC_HB_WR;              // 0xA8030C0C
    volatile DWORD TPC_LB_RD;               // 0xA8030C10
    volatile DWORD TPC_HB_RD;              // 0xA8030C14
} STTSCREG;

#endif    // __REGFILE_H
