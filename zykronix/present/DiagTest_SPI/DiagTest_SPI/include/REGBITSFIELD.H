/*
****************************************************************
*                      Magic Pixel Inc.
*
*    Copyright 2004, Magic Pixel Inc., HsinChu, Taiwan
*                    All rights reserved.
*
*
*
* Filename:    REGBITSFIELD.H
*
* Programmer:    Alex Tsai
*                MPX E120 division
*
* Created: 10/27/2004
*
* Description: Field definition for registers inside MP321
*
*
* Change History (most recent first):
*     <1>     10/27/2004    Alex Tsai    first file
****************************************************************
*/
#ifndef __REG_BITS_FIELD_H
#define __REG_BITS_FIELD_H

/*
**************************************************
*          SIU registers
**************************************************
*/
    // SIU_REG_00
    #define DCLK_SELECT       0x0706
    #define MCLK_SELECT       0x0505
    #define PASS_DCLK         0x0404
    #define H_SYNC_POS        0x0303
    #define V_SYNC_POS        0x0202
    #define SIUINPUTENABLE    0x0101
    #define NOOUTPUT          0x0000
    // SIU_REG_01
    #define STOP              0x1F1F
    #define VIEW              0x1E1E
    #define SNAP              0x1D1D
    #define SNAPR             0x1C1C
    #define OPMODE            0x1B19
    #define SKIP_V_LFRAME     0x1818
    #define INTBEFORSNAP      0x1616
    #define INTBEFORSNAPDONE  0x1515
    #define MANUALEXP         0x1414
    #define MANUALEXPDONE     0x1313
    #define WAITHSYNCINFREX   0x1212
    #define WAITEXTSYNC       0x1111
    #define CLOSESHUTTERBEFORSNAP    0x1010
    #define FLASHENABLE       0x0A0A
    #define WAITSYNCINREYEFLASH      0x0909
    #define WAITSYNCINNORNFLASH      0x0808
    #define WITHMS           0x0000
    // SIU_REG_06
    #define FRAMEBETWEENFLASHTRIG    0x0C08
    #define FLASHREYECOUNT   0x0200
    // SIU_REG_14
    #define FRAMEBEFORCAPTURE 0x1F18
    #define WAITVSYNCTOINTRTRIG      0x1710
    #define VSYNCBEFORMSCLOSE 0x0F08
    #define VSYNCBEFORFLASHNORMAL    0x0700
    // SIU_REG_16
    #define INTERRUPT_FLAG    0x1610
    #define INTERRUPT_ENABLE  0x0200
    #define IM_FRAME_START    0x0004
    #define IM_WRITE_START    0x0002
    #define IM_WRITE_DONE     0x0001
    #define IC_FRAME_START    0x00040000
    #define IC_WRITE_START    0x00020000
    #define IC_WRITE_DONE     0x00010000
    // SIU_REG_30
    #define VVALIDTOLINEDELAY 0x1F18
    #define STARTOFBLACKHEIGHT       0x1710
    #define BLACKHEIGHT       0x0F08
    #define STARTOFWINDOWHEIGHT      0x0700
    // SIU_REG_31
    #define WINDOWHEIGHT      0x0D00
    // SIU_REG_32
    #define STARTOFBLACKWIDTH 0x1710
    #define BLACKWIDTH        0x0F08
    #define STARTOFWINDOWWIDTH       0x0700
    // SIU_REG_33
    #define HVALIDTOPIXELDELAY       0x1F10
    #define WINDOWWIDTH       0x0D00
    // SIU_REG_34
    #define BLACK_ENABLE      0x1010
    #define BLACK_TYPE        0x0808
    #define BLACK_FIXVALUE    0x0700
    // SIU_REG_35
    #define PIXELFORMAT       0x1818
    #define SHADE_ENABLE      0x1111
    #define SHADE_MAPSCALE    0x1010
    #define SHADE_LINEOFFSET  0x0F00
    // SIU_REG_36
    #define SHADE_HTOTAL      0x1710
    #define SHADE_HCOUNT      0x0F08
    #define SHADE_VCOUNT      0x0700
    // SIU_REG_37
    #define SHADE_ELINEOFFSET 0x1410
    #define SHADE_LINERATIO   0x0D08
    #define SHADE_BLINEOFFSET 0x0400
    // SIU_REG_38
    #define SHADE_ROWRATIO    0x0D08
    #define SHADE_BROWOFFSET  0x0400
    // SIU_REG_40
    #define COLOROFFSET0      0x1F18
    #define COLOROFFSET1      0x1710
    #define COLOROFFSET2      0x0F08
    #define COLOROFFSET3      0x0700
    // SIU_REG_41
    #define COLORGAIN0        0x1F18
    #define COLORGAIN1        0x1710
    #define COLORGAIN2        0x0F08
    #define COLORGAIN3        0x0700
    // SIU_REG_42
    #define H_DECRATIO        0x1110
    #define V_DECRATIO        0x0908
    #define DECIMATE_TYPE     0x0100
    // SIU_REG_43
    #define ENABLEWINDOW      0x1F1F
    #define WIDTHOFFSET       0x1B10
    #define HEIGHTOFFSET      0x0B00
    // SIR_REG_44
    #define WIDTH             0x1B10
    #define HEIGHT            0x0B00
    // SIU_REG_45
    #define CLEARFIFO         0x1818
    #define SIUOUT_DATAWIDTH  0x1010
    #define DMA_OUTPUT_ENABLE 0x0808
    #define FIELD1_OFFSET     0x0706
    #define FIELD2_OFFSET     0x0504
    #define FIELD3_OFFSET     0x0302
    #define FIELD4_OFFSET     0x0100
    // SIU_REG_46
    #define YCBCRFIELDMODE    0x1818
    #define YCBCRFIRSTFIELD0  0x1010
    #define YCBCR_YFIRST      0x0808
    #define YCBCR_SWITCHCBCR  0x0000
    // SIU_REG_48 ~ 59
    #define DEAD_VLD          0x1F1F
    #define DEAD_X            0x1D10
    #define DEAD_Y            0x0D00
    // I2C_REG1
    #define I2C_DADDR         0x1F19
    #define I2C_DRW           0x1818
    #define I2C_RADDR         0x1710
    // I2C_REG2
    #define I2C_D1            0x1F18
    #define I2C_D5            0x1710
    #define I2C_D6            0x0F08
    #define I2C_D3            0x0100
    // I2C_REG3
    #define I2C_NORSTART      0x0505
    #define I2C_NONACK        0x0404
    #define I2C_16BITMODE     0x0303
    #define I2C_SUS           0x0202
    #define I2C_DONE          0x0101
    // I2C_REG4
    #define I2C_WDATA         0x1F18 // Move in B Version
    #define I2C_WDATA_EXT     0x1710 // Move in B Version
    #define I2C_RDATA         0x0F08
    #define I2C_RDATA_EXT     0x0700
    // I2C_REG5
    #define I2C_START         0x0000
    // I2C_BUF0 ~ I2C_BUF4
    #define LASTTRAN          0x1A1A
    #define SYNCV             0x1919
    #define CONTW             0x1818
    #define RADDR             0x1710  
    #define W_DATA            0x0F08
    #define W_DATA_EXT        0x0700
    // SIU_REG_80
    #define STBENABLE         0x0202
    #define STBMODE           0x0100

/*
**************************************************
*          IPU registers
**************************************************
*/
    // IP_CFG
    #define RGB_SRC_SEL       0x0C0C
    #define YUV_SRC_SEL       0x0808
    #define SC_SRC_SEL        0x0404
    #define SUBSC_SRC_SEL     0x0000
    // IP_IC
    #define IC_MN_END         0x00000008
    #define IC_IPR_END        0x00000004
    #define IC_IPW0_END       0x00000002
    #define IC_IPW1_END       0x00000001
    // IP_IM
    #define IM_MN_END         0x00000008
    #define IM_IPR_END        0x00000004
    #define IM_IPW0_END       0x00000002
    #define IM_IPW1_END       0x00000001
    // IP_IPR_0
    #define IPR_EN            0x1010
    #define IPR_SRC_SEL       0x0C0C
    #define IPR_FRMT          0x0908
    #define IPR_AUTO_TRG      0x0404
    #define IPR_TRG_SRC_SEL   0x0000
    // IP_IPR_1
    #define IPR_HSP           0x1B10
    #define IPR_VSP           0x0B00
    // IP_IPR_2
    #define IPR_HEP           0x1B10
    #define IPR_VEP           0x0B00
    // IP_IPW0_0
    #define IPW0_EN           0x0C0C
    #define IPW0_SRC_SEL      0x0808
    #define IPW0_OUT_FRMT     0x0100
    // IP_IPW0_1
    #define IPW0_CROP_HSP     0x1B10
    #define IPW0_CROP_VSP     0x0B00
    // IP_IPW0_3
    #define IPW0_OUT_HSP      0x1B10
    #define IPW0_OUT_VSP      0x0B00
    // IP_IPW0_4
    #define IPW0_OUT_HEP      0x1B10
    #define IPW0_OUT_VEP      0x0B00
    // IP_IPW1_0
    #define IPW1_EN           0x0C0C
    #define IPW1_SRC_SEL      0x0808
    #define IPW1_OUT_SEL      0x0404
    #define IPW1_OUT_FRMT     0x0100
    // IP_IPW1_1
    #define IPW1_CROP_HSP     0x1B10
    #define IPW1_CROP_VSP     0x0B00
    // IP_IPW1_3
    #define IPW1_OUT_HSP      0x1B10
    #define IPW1_OUT_VSP      0x0B00
    // IP_IPW1_4
    #define IPW1_OUT_HEP      0x1B10
    #define IPW1_OUT_VEP      0x0B00
    // IP_CLRINT
    #define IP_INTHLPF        0x0909
    #define IP_INTVLPF        0x0808
    #define IP_CLRINT_SEL     0x0504
    #define IP_SCH            0x0100
    // IP_SLPC_0
    #define IP_SLPC_EN        0x0000
    // IP_SLPC_1
    #define IP_SLPC_DNF       0x1818
    #define IP_SLPC_FP        0x1210
    #define IP_SLPC_FT        0x0F08
    #define IP_SLPC_PT        0x0700
    // IP_EDGEN
    #define IP_HAP            0x1F1C
    #define IP_VAP            0x1B18
    #define IP_KNEE           0x1710
    #define IP_CORING         0x0F08
    #define IP_EE_EEFS        0x0707
    #define IP_EE_AESV        0x0606
    #define IP_ED_EG          0x0500
    // IP_ICSC_0, IP_IPW_ICSC_0
    #define IP_ICSC_R0        0x1810
    #define IP_ICSC_RY        0x0900
    // IP_ICSC_1, IP_IPW_ICSC_1
    #define IP_ICSC_RCB       0x1910
    #define IP_ICSC_RCR       0x0900
    // IP_ICSC_2, IP_IPW_ICSC_2
    #define IP_ICSC_G0        0x1810
    #define IP_ICSC_GY        0x0900
    // IP_ICSC_3, IP_IPW_ICSC_3
    #define IP_ICSC_GCB       0x1910
    #define IP_ICSC_GCR       0x0900
    // IP_ICSC_4, IP_IPW_ICSC_4
    #define IP_ICSC_B0        0x1810
    #define IP_ICSC_BY        0x0900
    // IP_ICSC_5, IP_IPW_ICSC_5
    #define IP_ICSC_BCB       0x1910
    #define IP_ICSC_BCR       0x0900
    // IP_WBGC_0
    #define IP_WBGC_R         0x0800
    // IP_WBGC_1
    #define IP_WBGC_G         0x1810
    #define IP_WBGC_B         0x0800
    // IP_CC_0
    #define IP_CC_RMAX        0x1710
    #define IP_CC_GMAX        0x0F08
    #define IP_CC_BMAX        0x0700
    // IP_CC_1
    #define IP_CC_RR          0x0A00
    // IP_CC_2
    #define IP_CC_RG          0x1910
    #define IP_CC_RB          0x0900
    // IP_CC_3
    #define IP_CC_GR          0x0900
    // IP_CC_4
    #define IP_CC_GG          0x1A10
    #define IP_CC_GB          0x0900
    // IP_CC_5
    #define IP_CC_BR          0x0900
    // IP_CC_6
    #define IP_CC_BG          0x1910
    #define IP_CC_BB          0x0A00
    // IP_BC_0
    #define IP_BC_R           0x0800
    // IP_BC_1
    #define IP_BC_G           0x1810
    #define IP_BC_B           0x0800
    // IP_GC
    #define IP_GC_2X          0x1818
    #define IP_GC_R           0x1710
    #define IP_GC_G           0x0F08
    #define IP_GC_B           0x0700
    // IP_RGBGM
    #define IP_RGB_GM_ON      0x0000
    // IP_YGM
    #define IP_Y_GM_ON        0x0000
    // IP_RGMT_0, IP_GGMT_0, IP_BGMT_0, IP_YGMT_0
    #define IP_GMT_0          0x1710
    #define IP_GMT_1          0x0700
    // IP_RGMT_1, IP_GGMT_1, IP_BGMT_1, IP_YGMT_1
    #define IP_GMT_2          0x1710
    #define IP_GMT_3          0x0700
    // IP_RGMT_2, IP_GGMT_2, IP_BGMT_2, IP_YGMT_2
    #define IP_GMT_4          0x1710
    #define IP_GMT_5          0x0700
    // IP_RGMT_3, IP_GGMT_3, IP_BGMT_3, IP_YGMT_3
    #define IP_GMT_6          0x1710
    #define IP_GMT_7          0x0700
    // IP_RGMT_4, IP_GGMT_4, IP_BGMT_4, IP_YGMT_4
    #define IP_GMT_8          0x1710
    #define IP_GMT_9          0x0700
    // IP_RGMT_5, IP_GGMT_5, IP_BGMT_5, IP_YGMT_5
    #define IP_GMT_10         0x1710
    #define IP_GMT_11         0x0700
    // IP_RGMT_6, IP_GGMT_6, IP_BGMT_6, IP_YGMT_6
    #define IP_GMT_12         0x1710
    #define IP_GMT_13         0x0700
    // IP_RGMT_7, IP_GGMT_7, IP_BGMT_7, IP_YGMT_7
    #define IP_GMT_14         0x1710
    #define IP_GMT_15         0x0700
    // IP_CSC_0
    #define IP_CSC_YR         0x0800
    // IP_CSC_1
    #define IP_CSC_YG         0x1810
    #define IP_CSC_YB         0x0800
    // IP_CSC_2
    #define IP_CSC_CBR        0x0800
    // IP_CSC_3
    #define IP_CSC_CBG        0x1810
    #define IP_CSC_CBB        0x0800
    // IP_CSC_4
    #define IP_CSC_CRR        0x0800
    // IP_CSC_5
    #define IP_CSC_CRG        0x1810
    #define IP_CSC_CRB        0x0800
    // IP_CLRADJ_0
    #define IP_SP_Y           0x1510
    #define IP_SP_CB          0x0D08
    #define IP_SP_CR          0x0500
    // IP_CLRADJ_1
    #define IP_SP_XC          0x0F08
    #define IP_SP_STP         0x0300
    // IP_CLRADJ_2
    #define IP_HCI            0x1D1C
    #define IP_HCS            0x1A18
    #define IP_HCF            0x1710
    #define IP_HSI            0x0D0C
    #define IP_HSS            0x0A08
    #define IP_HSF            0x0700
    // IP_MNSC_0
    #define IP_MNSC_HUP       0x0909
    #define IP_MNSC_HDN       0x0808
    #define IP_MNSC_HSF       0x0700
    // IP_MNSC_1
    #define IP_MNSC_HTSZ      0x0A00
    // IP_MNSC_2
    #define IP_MNSC_VUP       0x0909
    #define IP_MNSC_VDN       0x0808
    #define IP_MNSC_VSF       0x0400
    // IP_MNSC_3
    #define IP_MNSC_VLW       0x1A10
    #define IP_MNSC_VTSZ      0x0A00
    // IP_MNSC_4
    #define IP_MNSC_HSM       0x1818
    #define IP_MNSC_HSR       0x1210
    #define IP_MNSC_VSM       0x0808
    #define IP_MNSC_VSR       0x0200
    // IP_SUBSC
    #define IP_SUBSC_HSR      0x1A18
    #define IP_SUBSC_VSR      0x1210
    #define IP_SUBSC_HDF      0x0F08
    #define IP_SUBSC_VDF      0x0700
    // IP_OVLY_0
    #define IP_OVLY_EN        0x0000
    // IP_OVLY_1
    #define IP_OVLY_KEY       0x0700
    // IP_OVLY_2
    #define IP_OVLY_HSP       0x1B10
    #define IP_OVLY_HSZ       0x0B00
    // IP_OVLY_3
    #define IP_OVLY_VSP       0x1B10
    #define IP_OVLY_VSZ       0x0B00
    // IP_IMST
    #define IP_IMST_MANU      0x0101
    #define IP_IMST_AUTO      0x0000
    // IP_AECFG
    #define IP_AEEN           0x0404
    #define IP_AESRC          0x0000
    // IP_AECFG_0
    #define IP_AEHSZ_0        0x1210
    #define IP_AEHSP_0        0x0A00
    // IP_AECFG_1
    #define IP_AEHSZ_1        0x1210
    #define IP_AEHSP_1        0x0A00
    // IP_AECFG_2
    #define IP_AEHSZ_2        0x1210
    #define IP_AEHSP_2        0x0A00
    // IP_AECFG_3
    #define IP_AEHSZ_3        0x1210
    #define IP_AEHSP_3        0x0A00
    // IP_AECFG_4
    #define IP_AEHSZ_4        0x1210
    #define IP_AEHSP_4        0x0A00
    // IP_AECFG_5
    #define IP_AEVSZ_0        0x1210
    #define IP_AEVSP_0        0x0A00
    // IP_AECFG_6
    #define IP_AEVSZ_1        0x1210
    #define IP_AEVSP_1        0x0A00
    // IP_AECFG_7
    #define IP_AEVSZ_2        0x1210
    #define IP_AEVSP_2        0x0A00
    // IP_AECFG_8
    #define IP_AEVSZ_3        0x1210
    #define IP_AEVSP_3        0x0A00
    // IP_AECFG_9
    #define IP_AEVSZ_4        0x1210
    #define IP_AEVSP_4        0x0A00
    // IP_AWBCFG_0
    #define IP_WB_EN          0x0C0C
    #define IP_WBSRCSEL       0x0808
    #define IP_WBSSH          0x0504
    #define IP_WBSSV          0x0100
    // IP_AWBCFG_1, IP_AWBCFG_3
    #define IP_WBRMULT0       0x0F08
    #define IP_WBBMULT0       0x0700
    // IP_AWBCFG_2, IP_AWBCFG_4
    #define IP_WBHSEL         0x1E18
    #define IP_WBLUMH         0x1710
    #define IP_WBLUML         0x0F08
    #define IP_WBSAT          0x0700
    // IP_HSTCFG
    #define IP_HSTSSH         0x0604
    // IP_AFCFG
    #define IP_AF_EN          0x0000
    // IP_AFCFG_0
    #define IP_AF_WNLX_0      0x1A10
    #define IP_AF_WNTY_0      0x0A00
    // IP_AFCFG_1
    #define IP_AF_WNRX_0      0x1A10
    #define IP_AF_WNBY_0      0x0A00
    // IP_AFCFG_2
    #define IP_AF_WNLX_1      0x1A10
    #define IP_AF_WNTY_1      0x0A00
    // IP_AFCFG_3
    #define IP_AF_WNRX_1      0x1A10
    #define IP_AF_WNBY_1      0x0A00
    // IP_AFCFG_4
    #define IP_AF_WNLX_2      0x1A10
    #define IP_AF_WNTY_2      0x0A00
    // IP_AFCFG_5
    #define IP_AF_WNRX_2      0x1A10
    #define IP_AF_WNBY_2      0x0A00
    // IP_AFCFG_6
    #define IP_AF_WNLX_3      0x1A10
    #define IP_AF_WNTY_3      0x0A00
    // IP_AFCFG_7
    #define IP_AF_WNRX_3      0x1A10
    #define IP_AF_WNBY_3      0x0A00
    // IP_AFCFG_8
    #define IP_AF_WNLX_4      0x1A10
    #define IP_AF_WNTY_4      0x0A00
    // IP_AFCFG_9
    #define IP_AF_WNRX_4      0x1A10
    #define IP_AF_WNBY_4      0x0A00

/*
**************************************************
*          IDU registers
**************************************************
*/
		// ID_DISP_ON_REG
		#define	ID_DISP_ON				0x0808
		#define ID_TRG_SEL				0x0605
		#define ID_AUTO_FRM_TX		0x0404
		#define	ID_SING_FRM_TX		0x0000
		// ID_DISP_STAT_REG
		#define	ID_VDO_BUSY				0x1010
		#define	ID_MPU_BUSY				0x0808
		#define	ID_DISP_BUSY			0x0000
		// ID_IC_REG
		#define	ID_IC_VDO_END			0x0101
		#define	ID_IC_MPU_END			0x0000
		// ID_IM_REG
		#define	ID_IM_VDO_END			0x0101
		#define	ID_IM_MPU_END			0x0000
		// ID_PSZ_REG
		#define	ID_PHSZ						0x1910
		#define	ID_PVSZ						0x0900
		// ID_PIF_REG
		#define ID_C601DS         0x0E0E
		#define ID_C656DS         0x0D0C
		#define ID_TV_FM          0x0909
		#define ID_TV_OUT         0x0808		
		#define	ID_DISP_OUT_BPP		0x0504
		#define	ID_DISP_IF_SEL		0x0100
		// ID_HTMG_REG
		#define	ID_OEP						0x1C1C
		#define	ID_HSP						0x1818
		#define	ID_HFPW						0x1710
		#define	ID_HSPW						0x0F08
		#define	ID_HBPW						0x0700
		// ID_VTMG_REG
		#define	ID_CLKP						0x1C1C
		#define	ID_VSP						0x1818
		#define	ID_VFPW						0x1710
		#define	ID_VSPW						0x0F08
		#define	ID_VBPW						0x0700
		// ID_MPU_CFG_REG
		#define	ID_MPU_PSEL				0x0606
		#define	ID_MPU_IF_SEL			0x0505
		#define	ID_MPU_BURST_WR		0x0404
		#define	ID_MPU_TX_SEL			0x0100
		// ID_MPU_TMG_REG
		#define	ID_MPU_TADS				0x1614
		#define	ID_MPU_TADH				0x1210
		#define	ID_MPU_TCSS				0x0E0C
		#define	ID_MPU_TCSH				0x0A08
		#define	ID_MPU_WRL				0x0604
		#define	ID_MPU_WRH				0x0200
		// ID_MPU_CMD_REG
		#define	ID_MPU_CMD				0x1100
		// ID_MPU_PAR_REG
		#define	ID_MPU_PAR				0x1100
		// ID_MW_RU_REG
		#define	ID_MW_RU					0x0000
		// ID_WM_ON_REG
		#define	ID_MW_ON					0x0000
		// ID_MW_CFG_REG
		#define	ID_MW_BFMD				0x0B0B
		#define	ID_MW_SRC					0x0908
		#define	ID_MW_BPP					0x0704
		#define	ID_MW_MD					0x0200
		// ID_MW_VSP_REG
		#define	ID_MW_VHSP				0x1A10
		#define	ID_MW_VVSP				0x0A00
		// ID_MW_NDW_ON_REG
		#define	ID_MW_NDW_ON			0x0000
		// ID_MW_NDW_SA_REG
		#define	ID_MW_NDW_HSA			0x1710
		#define	ID_MW_NDW_VSA			0x0A00
		// ID_MW_NDW_EA_REG
		#define	ID_MW_NDW_HEA			0x1710
		#define	ID_MW_NDW_VED			0x0A00
		// ID_MW_NDW_FILL_REG
		#define	ID_MW_NW_FILL			0x1F00
		// ID_SW_RU_REG
		#define	ID_SW_RU					0x0000
		// ID_SW_ON_REG
		#define	ID_SW_ON					0x0000
		// ID_SW_CFG_REG
		#define	ID_SW_BFMD				0x0B0B
		#define	ID_SW_SRC					0x0A08
		#define	ID_SW_BPP					0x0704
		#define	ID_SW_MD					0x0200
		// ID_SW_VSP_REG
		#define	ID_SW_VHSP				0x1A10
		#define	ID_SW_VVSP				0x0A00
		// ID_SW_DSP_REG
		#define	ID_SW_DHSP				0x1910
		#define	ID_SW_DVSP				0x0900
		// ID_SW_DSZ_REG
		#define	ID_SW_DHSZ				0x1910	// real define in register spec is SW_DHSZ
		#define	ID_SW_DVSZ				0x0900	// real define in register spec is SW_DVSZ
		// ID_ICSC_R_REG
		#define	ID_ICSC_R0				0x1E18
		#define	ID_ICSC_RY				0x1710
		#define	ID_ICSC_RCB				0x0F08
		#define	ID_ICSC_RCR				0x0700
		// ID_ICSC_G_REG
		#define	ID_ICSC_G0				0x1E18
		#define	ID_ICSC_GY				0x1710
		#define	ID_ICSC_GCB				0x0F08
		#define	ID_ICSC_GCR				0x0700
		// ID_ICSC_B_REG
		#define	ID_ICSC_B0				0x1E18
		#define	ID_ICSC_BY				0x1710
		#define	ID_ICSC_BCB				0x0F08
		#define	ID_ICSC_BCR				0x0700
		// ID_MIX_MODE_REG
		#define	ID_MIX_MODE				0x0100
		// ID_MIX_BG_REG
		#define	ID_MIX_BG_R				0x1510
		#define ID_MIX_BG_G				0x0D08
		#define	ID_MIX_BG_B				0x0500
		// ID_MIX_KEY_REG
		#define	ID_MIX_KEY_R			0x1510
		#define	ID_MIX_KEY_G			0x0D08
		#define	ID_MIX_KEY_B			0x0500
		// ID_MIX_ALPHA_REG
		#define	ID_MIX_KEY_ALPHA	0x0B08
		#define	ID_MIX_UNIV_ALPHA	0x0300
		// ID_GM_CFG_REG
		#define	ID_GM_SEL					0x0101
		#define	ID_GM_ON					0x0000
		// ID_GMT_R0_REG
		#define	ID_GMT_R_0				0x1D18
		#define	ID_GMT_R_1				0x1510
		#define	ID_GMT_R_2				0x0D08
		#define	ID_GMT_R_3				0x0500
		// ID_GMT_G0_REG
		#define	ID_GMT_G_0				0x1D18
		#define	ID_GMT_G_1				0x1510
		#define	ID_GMT_G_2				0x0D08
		#define	ID_GMT_G_3				0x0500
		// ID_GMT_B0_REG
		#define	ID_GMT_B_0				0x1D18
		#define	ID_GMT_B_1				0x1510
		#define	ID_GMT_B_2				0x0D08
		#define	ID_GMT_B_3				0x0500
		// ID_GMT_R1_REG
		#define	ID_GMT_R_4				0x1D18
		#define	ID_GMT_R_5				0x1510
		#define	ID_GMT_R_6				0x0D08
		#define	ID_GMT_R_7				0x0500
		// ID_GMT_G1_REG
		#define	ID_GMT_G_4				0x1D18
		#define	ID_GMT_G_5				0x1510
		#define	ID_GMT_G_6				0x0D08
		#define	ID_GMT_G_7				0x0500
		// ID_GMT_B1_REG
		#define	ID_GMT_B_4				0x1D18
		#define	ID_GMT_B_5				0x1510
		#define	ID_GMT_B_6				0x0D08
		#define	ID_GMT_B_7				0x0500
		// ID_GMT_R2_REG
		#define	ID_GMT_R_8				0x1D18
		#define	ID_GMT_R_9				0x1510
		#define	ID_GMT_R_10				0x0D08
		#define	ID_GMT_R_11				0x0500
		// ID_GMT_G2_REG
		#define	ID_GMT_G_8				0x1D18
		#define	ID_GMT_G_9				0x1510
		#define	ID_GMT_G_10				0x0D08
		#define	ID_GMT_G_11				0x0500
		// ID_GMT_B2_REG
		#define	ID_GMT_B_8				0x1D18
		#define	ID_GMT_B_9				0x1510
		#define	ID_GMT_B_10				0x0D08
		#define	ID_GMT_B_11				0x0500
		// ID_GMT_R3_REG
		#define	ID_GMT_R_12				0x1D18
		#define	ID_GMT_R_13				0x1510
		#define	ID_GMT_R_14				0x0D08
		#define	ID_GMT_R_15				0x0500
		// ID_GMT_G3_REG
		#define	ID_GMT_G_12				0x1D18
		#define	ID_GMT_G_13				0x1510
		#define	ID_GMT_G_14				0x0D08
		#define	ID_GMT_G_15				0x0500
		// ID_GMT_B3_REG
		#define	ID_GMT_B_12				0x1D18
		#define	ID_GMT_B_13				0x1510
		#define	ID_GMT_B_14				0x0D08
		#define	ID_GMT_B_15				0x0500
		// ID_GMT_R4_REG
		#define	ID_GMT_R_16				0x1E18
		// ID_GMT_G4_REG
		#define	ID_GMT_G_16				0x1E18
		// ID_GMT_B4_REG
		#define	ID_GMT_B_16				0x1E18
		// ID_DTHR_CFG_REG
		#define	ID_DTHR_ON				0x0000
		// ID_DTHR_PTRN_0_REG
		#define	ID_DTHR_PTRN_00		0x1F1C
		#define	ID_DTHR_PTRN_01		0x1B18
		#define	ID_DTHR_PTRN_02		0x1714
		#define	ID_DTHR_PTRN_03		0x1310
		#define	ID_DTHR_PTRN_10		0x0F0C
		#define	ID_DTHR_PTRN_11		0x0B08
		#define	ID_DTHR_PTRN_12		0x0704
		#define	ID_DTHR_PTRN_13		0x0300
		// ID_DTHR_PTRN_1_REG
		#define	ID_DTHR_PTRN_20		0x1F1C
		#define	ID_DTHR_PTRN_21		0x1B18
		#define	ID_DTHR_PTRN_22		0x1714
		#define	ID_DTHR_PTRN_23		0x1310
		#define	ID_DTHR_PTRN_30		0x0F0C
		#define	ID_DTHR_PTRN_31		0x0B08
		#define	ID_DTHR_PTRN_32		0x0704
		#define	ID_DTHR_PTRN_33		0x0300
		// ID_BYPASS_REG
		#define	ID_BYPASS_PSEL		0x0101
		#define	ID_BYPASS_ON			0x0000
		// ID_PALETTE
		#define	ID_PALETTE_A			0x0F0F
		#define	ID_PALETTE_R			0x0E0A
		#define	ID_PALETTE_G			0x0905
		#define	ID_PALETTE_B			0x0400
/*
**************************************************
*          JPG registers
**************************************************
*/
    // JPEG_CONFIG
    #define ENCODE            0x1717
    #define DOWN_SAMPLE       0x1616
    #define FORMAT            0x1514
    #define MCU_X             0x130A
    #define MCU_Y             0x0900
    // JPEG_CONTROL
    #define EN_JPEG_INT       0x0303
    #define CLR_INT           0x0202
    #define CODEC_INT         0x0101
    #define CODEC_START       0x0000
    // JPEG_BS_OFFSET
    #define BS_START_ADDR     0x1302
    #define BS_FIRST_BYTE     0x0100
    // CODEC_CONFIG0
    #define START_STOP        0x0000
    // CODEC_CONFIG1
    #define DE                0x0303
    #define RE                0x0202
    #define NCOL              0x0100
    // CODEC_CONFIG2
    #define NMCU              0x1900
    // CODEC_CONFIG3
    #define NRST              0x0F00
    // CODEC_CONFIG4 ~ 7
    #define NBLOCK            0x0704
    #define QT                0x0302
    #define HA                0x0101
    #define HD                0x0000

/*
**************************************************
*          MP4 registers
**************************************************
*/
    // MPEG4_STATUS
    #define DEC_SEQ           0x1919
    #define DEC_VOL           0x1818
    #define DEC_VOP           0x1717
    #define DATA_PAR_STREAM   0x1616
    #define BPK_ERR           0x1515
    #define BPK_ERR_STATUS    0x140D
    #define VLD_ERR           0x0C0C
    #define VOP_HEAD_DONE     0x0303
    #define VOP_END           0x0202
    #define VPK_END           0x0101
    #define VOP_END_INT       0x0000
    // MPEG4_COMMAND
    #define SEL_DRAM          0x0808
    #define VOP_TYPE_SEL      0x0707
    #define ENC_LAST_VOP      0x0606
    #define SEC_LAST_VOP      0x0505
    #define INSERT_GOV_HD     0x0404
    #define SEC_IS_VPK        0x0303
    #define IMPLEMENT_VPK     0x0202
    #define CCU_DEC_FUNC      0x0101
    #define CCU_ENC_FUNC      0x0000
    // ENC_TIME_CONFIG
    #define TIME_SEC          0x130C
    #define TIME_INCR         0x0B06
    #define ENC_FRM_RATE      0x0500
    // MASK_MP4_IRQ
    #define MASK_DEC_SEQ      0x00000080
    #define MASK_DEC_VOL      0x00000040
    #define MASK_DEC_VOP_END  0x00000020
    #define MASK_BPK_ERR      0x00000010
    #define MASK_VLD_ERR      0x00000008
    #define MASK_DEC_VOPHD_DONE   0x00000004
    #define MASK_ENC_VOP_END  0x00000002
    #define MASK_ENC_VPK_END  0x00000001
    // ENC_VOP_CONFIG0
    #define PKT_LIMIT         0x1C15
    #define PKT_THRD          0x140D
    #define TABLE_SEL         0x0C0B
    #define AC_PRED           0x0A0A
    #define MB_Y              0x0905
    #define MB_X              0x0400
    // ENC_VOP_CONFIG1
    #define USE_SAD           0x1B1B
    #define SAD_VALUE         0x1A13
    #define QUANT_STEP        0x120E
    #define DC_VLC_THR        0x0D0B
    #define FCODE_FORWARD     0x0A08
    #define ZERO_THRD         0x0700
    // H263_CONFIG
    #define H263_MBLMT        0x0804
    #define H263_SIZE_SEL     0x0301
    #define H263_MODE_ENABLE  0x0000
    // BPKBUF_CONFIG
    #define MAX_SB_CNT        0x160A
    #define MAX_MBB           0x0900
    // ENABLE_DEBLOCK
    #define EN_DEB_CHROMA     0x0101
    #define EN_DEB            0x0000
    // DEBLOCK_THRESH
    #define BASIC_THRD        0x100E
    #define QUANT_STEP_SIZE   0x0D0B
    #define STRONG_THRD       0x0A08
    #define FLAT_CHK_THRD     0x0700
    // DEC_PARAMETER1
    #define RVLC              0x0808
    #define DATA_PAR          0x0707
    #define RESYNC            0x0606
    #define H263_SOURCE       0x0503
    #define H263_MODE         0x0202
    #define VOP_TYPE          0x0101
    #define VOP_CODED         0x0000
    // DEC_PARAMETER2
    #define VOL_TIME_INC      0x0F00
    // DEC_PARAMETER3
    #define VOL_TIME_SEC      0x1710
    #define VOL_TIME_TIC      0x0F00
    // DEC_PARAMETER4
    #define FRAME_HEIGHT      0x1F10
    #define FRAME_WIDTH       0x0F00

/*
**************************************************
*          DRAI registers
**************************************************
*/
    // SIU_START_ROW0
    #define SIU_SROW0_Y       0x130A
    #define SIU_SROW0_CBCR    0x0900
    // IMG_BUF_POINTER
    #define ISS3_BP           0x0303
    #define ISS2_BP           0x0202
    #define ISS1_BP           0x0101
    #define SIU_BP            0x0000
    // ISS1_START_ROW0
    #define ISS1_SROW0_Y      0x130A
    #define ISS1_SROW0_CBCR   0x0900
    // ISS2_START_ROW0
    #define ISS2_SROW0_Y      0x130A
    #define ISS2_SROW0_CBCR   0x0900
    // ISS3_START_ROW0
    #define ISS3_SROW0_Y      0x130A
    #define ISS3_SROW0_CBCR   0x0900
    // IDU_START_ROW
    #define IDU_SROW3         0x1F18
    #define IDU_SROW2         0x1710
    #define IDU_SROW1         0x0F08
    #define IDU_SROW0         0x0700
    // JPG_START_ROW
    #define JPG_SROW_Y        0x130A
    #define JPG_SROW_CBCR     0x0900
    // MP4_BUF_POINTER
    #define RF_BP             0x0101
    #define ROW_BP            0x0000
    // MP4_START_ROW0
    #define MP4_SROW0_Y       0x130A
    #define MP4_SROW0_CBCR    0x0900
    // MEW_START_ROW
    #define MP4R_SR1_Y        0x1F18
    #define MP4R_SR1_CBCR     0x1710
    #define MP4R_SR0_Y        0x0F08
    #define MP4R_SR0_CBCR     0x0700
    // DEB_START_ROW
    #define DEB_SR1           0x0F08
    #define DEB_SR0           0x0700
    // MISC_FRAME_WIDTH0
    #define ISS3_FW           0x1F18
    #define ISS2_FW           0x1710
    #define ISS1_FW           0x0F08
    #define MP4_FW            0x0700
    // MISC_FRAME_WIDTH1
    #define IDU3_FW           0x1F18
    #define IDU2_FW           0x1710
    #define IDU1_FW           0x0F08
    #define IDU0_FW           0x0700
    // BSBUF_SETTING
    #define BS_CHASING1       0x1E1B
    #define DS_WITH_BPK       0x1A1A
    #define BS_CHASING0       0x1916
    #define BS_PRESET         0x1512
    #define BS_SR             0x1108
    #define BS_QUOTA          0x0700
    // SDRAM_POWER_DOWN
    #define FDRAM_DPMODE      0x0303
    #define FDRAM_MRS         0x0202
    #define FDRAM_PDMODE      0x0101
    #define FDRAM_SRMODE      0x0000
    // SDRAM_SETTING
    #define SDRAM_CLK_PHASE   0x1E1B
    #define ENDIAN            0x1A1A
    #define ROWPERBANK        0x1918
    #define B32               0x1717
    #define SFRFR_PERIOD      0x160F
    #define ATRFR_GAPE        0x0E0B
    #define AFRFR_PERIOD      0x0A03
    #define CAS               0x0202
    #define TRCD              0x0101
    #define TRP               0x0000
    // MISC_FRAME_WIDTH2
    #define ISS4_BW           0x0F08
    #define SIU_BW            0x0700
    // CHASBUF_SETTING0
    #define CHASING_CFG       0x1510
    #define CB2_QUOTA         0x0F08
    #define CB1_QUOTA         0x0700
    // SIU_START_ROW1
    #define SIU_SROW1_Y       0x130A
    #define SIU_SROW1_CBCR    0x0900
    // ISS1_START_ROW1
    #define ISS1_SR1_Y        0x130A
    #define ISS1_SR1_CBCR     0x0900
    // ISS2_START_ROW1
    #define ISS2_SR1_Y        0x130A
    #define ISS2_SR1_CBCR     0x0900
    // ISS3_START_ROW1
    #define ISS3_SR1_Y        0x130A
    #define ISS3_SR1_CBCR     0x0900
    // MP4_START_ROW1
    #define MP4_SR1_Y         0x130A
    #define MP4_SR1_CBCR      0x0900
    // I2SBUF_START_ROW
    #define I2S_SR            0x0900
    // USBBUF_START_ROW
    // MCIBUF_SETTING
    #define MCI_PRESET        0x0A0A
    #define MCI_SR            0x0900
    // AUDBUF_SETTING
    #define AUD_BFQUOTA       0x0904
    #define AUD_BFCONFIG      0x0300
    #define AUD_START_ROW    0x0900
	
    // I2SBUF_SETTING0
    #define I2S_CHASING       0x1B1A
    #define I2S_PRESET        0x1919
    #define UPDATE_I2S_CPUADDR    0x1818
    #define I2S_CPUADDR       0x1708
    #define I2S_BFQUOTA       0x0700
    // CPU2HIU_BS_ADDR
    #define UPDATE_HIU_CPUADDRESS 0x0F0F
    #define HIU_CPUADDRESS    0x0E00
    // HIU_BS_ADDR
    #define UPDATE_HIU_BFADDRESS  0x0F0F
    #define HIU_BFADDRESS     0x0E00
    // CPU2BPK_BS_ADDR
    #define UPDATE_BPK_CPUADDRESS 0x0F0F
    #define BPK_CPUADDRESS    0x0E00
    // BPK_BS_ADDR
    #define UPDATE_BPK_BFADDRESS  0x0F0F
    #define BPK_BFADDRESS     0x0E00
    // I2SBUF_SETTING1
    #define UPDATE_I2S_BFADDRESS  0x1010
    #define I2S_BFADDRESS     0x0F00
    // RG_BT_BTC
    #define RG_BT_ADH         0x1614
    #define RG_BT_CEH         0x1210
    #define RG_BT_CMDW        0x0F0C
    #define RG_BT_CES         0x0A08
    #define RG_BT_ADS         0x0604
    #define RG_BT_OZD         0x0200
    // RG_SIUY0_ext
    #define SIUY0_ROW_ext     0x1410
    #define SIUY0_COL_ext     0x0800
    // RG_SIUUV0_ext
    #define SIUUV0_ROW_ext    0x1410
    #define SIUUV0_COL_ext    0x0800
    // RG_SIUY1_ext
    #define SIUY1_ROW_ext     0x1410
    #define SIUY1_COL_ext     0x0800
    // RG_SIUUV1_ext
    #define SIUUV1_ROW_ext    0x1410
    #define SIUUV1_COL_ext    0x0800
    // RG_IPU1Y0_ext
    #define IPU1Y0_ROW_ext    0x1410
    #define IPU1Y0_COL_ext    0x0800
    // RG_IPU1UV0_ext
    #define IPU1UV0_ROW_ext   0x1410
    #define IPU1UV0_COL_ext   0x0800
    // RG_IPU1Y1_ext
    #define IPU1Y1_ROW_ext    0x1410
    #define IPU1Y1_COL_ext    0x0800
    // RG_IPU1UV1_ext
    #define IPU1UV1_ROW_ext   0x1410
    #define IPU1UV1_COL_ext   0x0800
    // RG_IPU2Y0_ext
    #define IPU2Y0_ROW_ext    0x1410
    #define IPU2Y0_COL_ext    0x0800
    // RG_IPU2UV0_ext
    #define IPU2UV0_ROW_ext   0x1410
    #define IPU2UV0_COL_ext   0x0800
    // RG_IPU2Y1_ext
    #define IPU2Y1_ROW_ext    0x1410
    #define IPU2Y1_COL_ext    0x0800
    // RG_IPU2UV1_ext
    #define IPU2UV1_ROW_ext   0x1410
    #define IPU2UV1_COL_ext   0x0800
    // RG_IPU3Y0_ext
    #define IPU3Y0_ROW_ext    0x1410
    #define IPU3Y0_COL_ext    0x0800
    // RG_IPU3UV0_ext
    #define IPU3UV0_ROW_ext   0x1410
    #define IPU3UV0_COL_ext   0x0800
    // RG_IPU3Y1_ext
    #define IPU3Y1_ROW_ext    0x1410
    #define IPU3Y1_COL_ext    0x0800
    // RG_IPU3UV1_ext
    #define IPU3UV1_ROW_ext   0x1410
    #define IPU3UV1_COL_ext   0x0800
    // RG_IPU4_ext
    #define IPU4_ROW_ext      0x1410
    #define IPU4_COL_ext      0x0800
    // RG_IDU1_ext
    #define IDU1_ROW_ext      0x1610
    #define IDU1_COL_ext      0x0800
    // RG_IDU2_ext
    #define IDU2_ROW_ext      0x1610
    #define IDU2_COL_ext      0x0800 
    // RG_IDU3_ext
    #define IDU3_ROW_ext      0x1610
    #define IDU3_COL_ext      0x0800 
    // RG_IDU4_ext
    #define IDU4_ROW_ext      0x1610
    #define IDU4_COL_ext      0x0800 
    // RG_JPGY_ext
    #define JPGY_ROW_ext      0x1410
    #define JPGY_COL_ext      0x0800        
    // RG_JPGUV_ext
    #define JPGUV_ROW_ext     0x1410
    #define JPGUV_COL_ext     0x0800        
    // RG_JPGB_ext
    #define JPGB_ROW_ext      0x1410
    #define JPGB_COL_ext      0x0800                              
    //RG_I2S_EXT
    #define I2S_ROW_EXT       0x1410
    #define I2S_COL_EXT       0x0800
    //RG_MCI_EXT
    #define MCI_ROW_EXT       0x1410
    #define MCI_COL_EXT       0x0800
    //RG_BSBUF_EXT
    #define BSBUF_ROW_ext     0x1410
    #define BSBUF_COL_ext     0x0800
    //RG_SDRAM_SETTING2
    #define SDRAM_EMRSEN      0x1F1F
    #define SDRAM_EMRSV       0x1E10
    #define SDCLKOFF_EN       0x0A0A
    #define HWSELFREF_EN      0x0909
    #define PCHPWRDN          0x0808
    #define tRCmin            0x0604
    #define tRDL              0x0303
    #define tRCDH             0x0202
    #define COL128            0x0000
    // DRAI_INT
    #define DRAI_IMASK        0x1510
    #define DRAI_ICAUSE       0x0500
    #define I2SBUF_END        0x00000001
    #define I2SHBUF_END       0x00000002
    #define MCIBUF_END        0x00000004
    #define MCIHBUF_END       0x00000008
    #define HIUBSBUF_END      0x00000010
    #define HIUBSHBUF_END     0x00000020

/*
**************************************************
*          USB registers
**************************************************
*/
    // D1SYNC
    // DEVICE_ADDRESS
    // IN_NEEDED
    // OUT_NEEDED
    // INOUT_ABORT
    // USEDMA
    // WHICH_EP
    // HOWMANY_TRAN
    // LASTTRAN_BYTE
    // MAX_PKT_SIZE
    // EP_NAKORSTALL
    // EP_ISO
    // FORCE_K
    // IRQ_EN // IRQ_STATUS
    #define USB_OUT_SUS         0x00000800
    #define USB_SETUP_SUS       0x00000400
    #define USB_SETUP_ERR       0x00000200
    #define USB_OUT_ERR         0x00000100
    #define USB_OUT_NMH         0x00000080
    #define USB_IN_SUS          0x00000040
    #define USB_IN_ERR          0x00000020
    #define USB_IN_NMH          0x00000010
    #define USB_SOF             0x00000008
    #define USB_SUSP            0x00000004
    #define USB_RESU            0x00000002
    #define USB_REST            0x00000001
    // DMA_IDLE
    // TIMEOUT_LIMIT
    // FRAMENUM
    // NMH_EP
    // TRAN_COUNT
    // SETUP_CLEAR
    // EP0_TOGGLESET
    // EP0_toggleseten
    // DMA_ADDR_OFFSET
    // FORCE_SE0
    // SETUP_DATA0
    #define DATA0       0x1F18
    #define DATA1       0x1710
    #define DATA2       0x0F08
    #define DATA3       0x0700
    // SETUP_DATA4
    #define DATA4       0x1F18
    #define DATA5       0x1710
    #define DATA6       0x0F08
    #define DATA7       0x0700
    // INTERNALSRAM

/*
**************************************************
*          HIU registers
**************************************************
*/

    //HIU_INT
    #define ICCMDREQ         0x0000    
    #define IMCMDREQ         0x0404
    //SYST    
    #define MEMTYPE          0x0000
    #define RD_DLY           0x0401         
    #define WR_DLY           0x0805 

/*
**************************************************
*          CKG registers
**************************************************
*/
    // CFG_CPU_CLK
    #define BIU_RESET         0x0404
    #define CPU_PSTOP1        0x0303
    #define CPU_RESET         0x0202
    #define CPU_TSTOP         0x0101
    #define CPU_PSTOP0        0x0000
    // CFG_UART_CLK
    #define UART_CCOUNT       0x0B02
    #define UART_RESET        0x0101
    #define UART_PSTOP        0x0000
    // CFG_DRAI_CLK
    #define DRAI_PSTOP1       0x0404
    #define DRAI_RESET        0x0303
    #define DRAI_TSTOP        0x0202
    #define DRAI_PSTOP0       0x0101
    #define DRAI_PSTOP_1      0x0000
    // CFG_HIU_CLK
    #define HIU_RESET         0x0202
    #define HIU_TSTOP         0x0101
    #define HIU_PSTOP         0x0000
    // CFG_JPG_CLK
    #define JPG_RESET         0x0101
    #define JPG_PSTOP         0x0000
    // JPG_CLK_SELECT
    #define JPG_FCLK          0x0505
    #define JPG_MCLK          0x0404
    #define JPG_CSELECT       0x0300
    // CFG_MP4_CLK
    #define MP4_RESET         0x0101
    #define MP4_PSTOP         0x0000
    // CFG_SIU_CLK
    #define SIU_CCOUNT        0x0B02    // Delete in B Version
    #define CIS_CKSEL         0x0707
    #define SIU_CKSEL         0x0604
    #define CIS_PSTOP	        0x0303
    #define I2C_PSTOP         0x0202
    #define SIU_RESET         0x0101
    #define SIU_PSTOP         0x0000
    // SIUCK_SELECT
    #define SIU_CSOURCE       0x0404    // Move in B Version
    #define SIU_CSELECT       0x0300
    // CFG_ISS_CLK
    #define SCL_PSTOP         0x0404
    #define IPU_SOURCE        0x0303
    #define IPUS_PSTOP        0x0202
    #define IPU_RESET         0x0101
    #define IPU_PSTOP         0x0000
    // ISSCK_SELECT
    #define SCL_CSELECT       0x0704
    #define IPU_CSELECT       0x0300
    // CFG_IDU_CLK
    #define IDU_CDIV2         0x0F0C
    #define IDU_CDIV1         0x0B08
    #define IDU_RESET         0x0101
    #define IDU_PSTOP         0x0000
    // CFG_USB_CLK
    #define USB_CCOUNT        0x0502
    #define USB_RESET         0x0101
    #define USB_PSTOP         0x0000
    // CFG_TIMER1_CLK
    #define TIM1_CCOUNT       0x0F02
    #define TIM1_RESET        0x0101
    #define TIM1_PSTOP        0x0000
    // CFG_I2S_CLK
    #define I2S_CCOUNT        0x0B02
    #define I2S_RESET         0x0101
    #define I2S_PSTOP         0x0000
    // I2SCK_SELECT
    #define I2S_CSELECT       0X0000
    // CFG_ADC_CLK
    #define ADCK1M_PHAS       0x0808
    #define ADC_CKSRCSEL      0x0707
    #define ADC_CLKDIV        0x0602
    #define ADC_CKEN          0x0101
    #define ADC_PWRDN         0x0000
    // CFG_TIMER2_CLK
    #define TIM2_CCOUNT       0x0F02
    #define TIM2_RESET        0x0101
    #define TIM2_PSTOP        0x0000
    // CFG_TIMER3_CLK
    #define TIM3_CCOUNT       0x0F02
    #define TIM3_RESET        0x0101
    #define TIM3_PSTOP        0x0000
    // CFG_TIMER4_CLK
    #define TIM4_CCOUNT       0x0F02
    #define TIM4_RESET        0x0101
    #define TIM4_PSTOP        0x0000
    // CFG_TIMER5_CLK
    #define TIM5_CCOUNT       0x0F02
    #define TIM5_RESET        0x0101
    #define TIM5_PSTOP        0x0000
    // CFG_MCI_CLK
    #define MCI_RESET         0x0101
    #define MCI_PSTOP         0x0000
    // MCICK_SELECT
    #define MCI_CCOUNT        0x0902
    #define MCI_CSELECT       0x0100
    // PLL1_FREQUENCY
    #define PLL1_N            0x0F08
    #define PLL1_M            0x0700
    // PLL2_FREQUENCY
    #define PLL2_N            0x0F08
    #define PLL2_M            0x0700
    // CKGIN_SELECT
    #define CKG_SEL2          0x0101
    #define CKG_SEL1          0x0000

/*
**************************************************
*          ICU registers
**************************************************
*/
    // ICU_STATUS & ICU_MASK
    #define MCI_IRQ_MASK      0x00010000
    #define USB_IRQ_MASK      0x00008000
    #define TIMER_4_IRQ_MASK  0x00004000
    #define TIMER_3_IRQ_MASK  0x00002000
    #define TIMER_2_IRQ_MASK  0x00001000
    #define TIMER_1_IRQ_MASK  0x00000800
    #define TIMER_0_IRQ_MASK  0x00000400
    #define GPIO_IRQ_MASK     0x00000200
    #define I2S_IRQ_MASK      0x00000100
    #define SIU_IRQ_MASK      0x00000080
    #define ISS_IRQ_MASK      0x00000040
    #define IDU_IRQ_MASK      0x00000020
    #define JPG_IRQ_MASK      0x00000010
    #define MP4_IRQ_MASK      0x00000008
    #define DRAI_IRQ_MASK     0x00000004
    #define HIU_IRQ_MASK      0x00000002
    #define UART_IRQ_MASK     0x00000001

/*
**************************************************
*          GPIO registers
**************************************************
*/
    // GPINT0 & GPINT1
    #define GPINT_MODE        0x1F18
    #define GPINT_POLARITY    0x1710
    #define GPINT_ENABLE      0x0F08
    #define GPINT_CAUSE       0x0700
    #define GPIO_IC_0         0x00000001
    #define GPIO_IC_1         0x00000002
    #define GPIO_IC_2         0x00000004
    #define GPIO_IC_3         0x00000008
    #define GPIO_IC_4         0x00000010
    #define GPIO_IC_5         0x00000020
    #define GPIO_IC_6         0x00000040
    #define GPIO_IC_7         0x00000080

/*
**************************************************
*          I2S registers
**************************************************
*/

    // 0x2_0500	I2S_EN_REG	    I2SU enable register	R/W
    #define I2SU_EN                 0x0000
    #define ADC_EN		    0x0101
    #define ADC_GAIN		    0x0302
    #define ADC_TB		    0x0f0e
    // 0x2_0504	I2S_CLR_REG	    I2SU status clear register	R/W
    #define I2S_CLR                 0x0000
    // 0x2_0508	I2S_IC_REG	    I2SU interrupt cause status register	R
    #define I2S_IC_FOR              0x00000001
    #define I2S_IC_FUR              0x00000002
    // 0x2_050c	I2S_IM_REG	    I2SU interrupt mask configuration register	R/W
    #define I2S_IM_FOR              0x00000001
    #define I2S_IM_FUR              0x00000002
    // 0x2_0510	I2S_IFCFG_REG	I2SU interface configuration register	R/W
    #define I2S_STEREO_CODEC        0x0000
    #define I2S_LEFT_JUST           0x0101
    #define I2S_DATA_DELAY          0x0202
    #define I2S_FRAME_SYNC_POL      0x0303
    #define I2S_SHORT_FRAME_SYNC    0x0404
    #define I2S_NON_STOP_FS         0x0505
    #define I2S_MS                  0x0808
    #define I2S_TR                  0x0909
    // 0x2_0514	I2S_FLEN_REG	I2SU Frame length configuration register	R/W
    #define I2S_FRAME_LEN           0x0800
    // 0x2_0518	I2S_DATCFG_REG	I2SU PCM data format configuration register	R/W
    #define I2S_SIGNBIT_INV         0x0000
    #define I2S_BIG_ENDIAN          0x0101
    #define I2S_16BIT_PCM           0x0202
    #define I2S_MONO_CHAN_SEL       0x0303
    #define I2S_STEREO_CHAN_REV     0x0404
    #define I2S_STEREO_PCM          0x0505
    // 0x2_051c	I2S_DMAADR_REG	I2SU DMA transfer current address	R
    #define I2S_DMAADR              0x1300    
    // 0x2_0520	I2S_BCLKCFG_REG	I2SU bit clock configuration register	R/W
    #define I2S_BCLK_POL            0x0000
    #define I2S_BCLK_OE             0x0404
    // 0x2_0524 ADC_GAIN_STATUS_REG
    #define ADC_GAIN_VALUE          0x0300
    #define ADC_TB_VALUE            0x0f0e
/*
**************************************************
*          MCARD registers
**************************************************
*/
    // MCARD_C
    #define FFEMP                   0x0A0A
    #define MSSL                    0x0605
    #define MCDMADR                 0x0404
    #define MCDMAS                  0x0302
    #define MCFFEN                  0x0101
    #define MCARDEN                 0x0000
    // MC_DMARL
    #define LIMITEN                 0x1717
    #define DLENGTH                 0x1600
    // MC_RTM
    #define MCR_TRS                 0x1A18
    #define MCR_HLD                 0x1110
    #define MCR_PLW                 0x0C08
    #define MCR_ST                  0x0200
    // MC_WTM
    #define MCW_TRS                 0x1B18
    #define MCW_HLD                 0x1110
    #define MCW_PLW                 0x0B08
    #define MCW_ST                  0x0200
    // MC_WDT
    #define WDTEN                   0x1515
    #define WDT_CNT                 0x1400
    // MC_SM_C
    #define SMCESEL                 0x0807
    #define SMADLG                  0x0605
    #define SMCEMSK                 0x0404
    #define SMTYPE                  0x0303
    #define SMECCEN                 0x0202
    #define SM_WP                   0x0101
    #define SCDMD                   0x0000
    // MC_SM_IC
    #define MD_CDX                  0x10000000
    #define MD_SMTO                 0x08000000
    #define MD_SFTCE                0x04000000
    #define MD_ECB                  0x02000000
    #define MD_SRB                  0x01000000
    #define PL_CDX                  0x00100000
    #define PL_SMTO                 0x00080000
    #define PL_SFTCE                0x00040000
    #define PL_ECB                  0x00020000
    #define PL_SRB                  0x00010000

    #define IM_CDX                  0x00001000
    #define IM_SMTO                 0x00000800
    #define IM_SFTCE                0x00000400
    #define IM_ECB                  0x00000200
    #define IM_SRB                  0x00000100
    #define IC_CDX                  0x00000010
    #define IC_SMTO                 0x00000008
    #define IC_SFTCE                0x00000004
    #define IC_ECB                  0x00000002
    #define IC_SRB                  0x00000001
    // MC_SM_CMD
    // MC_SM_ADR
    // MC_SM_DAT
    // MC_SM_ECC1
    // MC_SM_ECC2
    // MC_SD_OP
    #define RADTC                   0x0202
    #define WADTC                   0x0101
    #define NADTC                   0x0000
    // MC_SD_IC
    #define IM_WXF                  0x00008000
    #define IM_SDTO                 0x00004000
    #define IM_PGMF                 0x00002000
    #define IM_CRC7F                0x00001000
    #define IM_CRC16F               0x00000800
    #define IM_RSPF                 0x00000400
    #define IM_TREND                0x00000200
    #define IM_RSPDONE              0x00000100
    #define IC_WXF                  0x00000080
    #define IC_SDTO                 0x00000040
    #define IC_PGMF                 0x00000020
    #define IC_CRC7F                0x00000010
    #define IC_CRC16F               0x00000008
    #define IC_RSPF                 0x00000004
    #define IC_TREND                0x00000002
    #define IC_RSPDONE              0x00000001
    // MC_SD_C
    #define DBUS                    0x0F0E
    #define CD                      0x0D0D
    #define FSMBUSY                 0x0C0C
    #define R1B                     0x0B0B
    #define ROD                     0x0909
    #define STOPCMD_B               0x0808
    #define LED_MODE                0x0707
    #define INDEX                   0x0500
    // MC_SD_ARG
    // MC_SD_RSPA
    // MC_SD_RSPB
    // MC_SD_RSPC
    // MC_SD_RSPD
    // MC_SD_SC
    // MC_MS_CMD
    #define DATA_LEN                0x0F08
    #define PAGE_MOD                0x0707
    #define BY_DMA                  0x0606
    #define TPC_CMD                 0x0300
    // MC_MS_FIFO
    // MC_MS_CRC
    // MC_MS_T0
    // MC_MS_T1
    // MC_MS_STATUS
    #define CMDNK                   0x00200000
    #define BREQ                    0x00100000
    #define ERR                     0x00080000
    #define CED                     0x00040000
    #define INVALIDCMD              0x00020000

    #define SDIO                    0x100D
    #define BS                      0x0C0B

    #define TIMEOUT1                0x00000400
    #define TIMEOUT0                0x00000200
    #define CRCERROR                0x00000100
    #define CMDEND                  0x00000080
    #define CMDINT                  0x00000040
    #define INVALIDACC              0x00000020
    #define FIFOEMPTY               0x00000010
    #define FIFOFULL                0x00000008
    #define FIFOIDLE                0x00000004
    #define HRDREQ                  0x00000002
    #define HWRREQ                  0x00000001
    // MC_MS_IC
    #define IMOD                    0x1D18
    #define IPOL                    0x1510
    #define IMASK                   0x0D08
    #define INT_INVALIDCMD_MASK     0x00002000
    #define INT_TIMEOUT1_MASK       0x00001000
    #define INT_CMDEND_MASK         0x00000800
    #define INT_CMDINT_MASK         0x00000400
    #define INT_HRDREQ_MASK         0x00000200
    #define INT_HWRREQ_MASK         0x00000100
    #define ICAUSE                  0x0500
    #define INT_INVALIDCMD_CAUSE    0x00000020
    #define INT_TIMEOUT1_CAUSE      0x00000010
    #define INT_CMDEND_CAUSE        0x00000008
    #define INT_CMDINT_CAUSE        0x00000004
    #define INT_HRDREQ_CAUSE        0x00000002
    #define INT_HWRREQ_CAUSE        0x00000001
#endif	// ifndef __REG_BITS_FIELD_H
