/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file main System wide definitions, such as all module numbers
 *    etc.
 *
 * Note:
 *    - Please be carefull when you modify this file. This file is included
 *	by every module in the system.  
 *
 * Author(s):
 *
 * 
 */


#ifndef AXSS_IF_H
#define AXSS_IF_H
/*
 *====================================================================
 *=                            include                               =
 *====================================================================
 */

/* ABSOLUTELY NO INCLUDE FILES HERE, PLEASE */


/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */


/******************************************************************
* System wide Literals
*/

/* Maximum number of transactions that are supported from one Card in MRPC */
#if 1
#define    MAX_TRANSACTIONS    2048 /* Max client txns per card (for MRPC)*/
#else /* 0 */
#define    MAX_TRANSACTIONS    128 /* Max client txns per card (for MRPC)*/
#endif /* 0 */
#define    MAX_SERVICES   64  /* Max number of services per module for MRPC */
/* MRPC Layer 2 */
#define    MAX_RETX_TRANSACTIONS    20 /* Max Tx that can be re-Q'ed */


/* For Data Base Manager; these are system wide upper limits */
#define    MAX_DB_PER_SYSTEM   128              /* Total # of DBs in a system */
#define    MAX_DB_PER_TYPE     64               /* 64 for RAM and 64 for HD   */
#define    MAX_RAM_DB          MAX_DB_PER_TYPE  /* 64 for RAM */
#define    MAX_HD_DB           MAX_DB_PER_TYPE  /* 64 for HD */
/*
 * NOTE & WARNING: If the number of Tables in a given DB are changed (i.e.
 *                 increased or decreased, then DBM_MAX_TABLE_ID_BITS and
 *                 DBM_TABLE_ID_MASK MUST be adjusted correctly. Otherwise
 *                 the system will break apart. So, please make sure you know
 *                 what you are doing before touching any of these literals
 *                 (Sukhdev S Kapur)
 */
#define    MAX_TABLES_PER_DB          32        /* Max # Tables in one DB */
#define    DBM_MAX_TABLE_ID_BITS       5        /* # of bits reserved for it */
#define    DBM_TABLE_ID_MASK          0x1F      /* represents 5 bits */


/* For File Manager; these are system wide upper limits */
/* Max. Number of files in AXSS-1600  */
#define    MAX_FILES      ((MAX_DB_PER_SYSTEM * MAX_TABLES_PER_DB) + 64)

/* Maximum Lenght of RAM files in AXSS System */
#define    MAX_FILE_LENGTH    (64 * 1024 * 1024) /* 64M for now */

/* 
 * Maximum lenght of a file name in number of characters
 */
#define MAX_FILE_NAME_LENGTH      60
/* 
 * Maximum lenght a string carrying version number for Firmware
 */
#define MAX_VERSION_LENGTH       20

/* 
 *Interface Type Length Definition
 */
#define IF_ADDR_TYPE_LEN         10

/* 
 * Following are the path/file names for certain critical files in the
 * system, such as image files, etc..
 */
#define  IMAGE_PATH              "C:/IMAGE/"
#define  NMC_DIR_PATH            "C:/IMAGE/NMC/"
#define  NSC_DIR_PATH            "C:/IMAGE/NSC/"
#define  SCC_DIR_PATH            "C:/IMAGE/SCC/"
#define  BSC_DIR_PATH            "C:/IMAGE/BSC/"
#define  NSC_DIR_PATH            "C:/IMAGE/NSC/"
#define  HTML_DIR_PATH           "C:/IMAGE/html/"
#define  MISC_DIR_PATH           "C:/IMAGE/misc/"
#define  CONFIG_DIR_PATH         "C:/CONFIG/"
#define  DIAG_DIR_PATH           "C:/DIAG/"
#define  ANNOUNCEMENT_DIR_PATH   "C:/IMAGE/Announce/"
#define  TONE_PLAN_DIR_PATH      "C:/IMAGE/TonePlans/"
#define  NMC_IMAGE_FILE_NAME     "C:/IMAGE/NMC/nmc.fw"
#define  NSC_IMAGE_FILE_NAME     "C:/IMAGE/NSC/nsc.fw"
#define  SCC_IMAGE_FILE_NAME     "C:/IMAGE/SCC/scc.fw"
#define  BSC_IMAGE_FILE_NAME     "C:/IMAGE/BSC/bsc.fw"
#define  DIAG_FILE_NAME          "C:/DIAG/mms.diag"

/* This is the maximum Number of Announcement files that AXSS will 
 * support.
 * WARNING: Do not increase the value of this literal. It is not trivial
 *          to increase the number of annoncement files
 */
#define MAX_ANNOUNCEMENT_FILES  100 
#define ANNOUNCEMENT_FILE_NAME  "C:/IMAGE/Announce/scc_"
#define MAKE_ANNOUNCEMENT_FILE_NAME(buf, id) \
    sprintf(buf, ANNOUNCEMENT_FILE_NAME"%d.%s", id, "wav")

/* This is the offset in SAR Buffer where Event Id is embedded for the 
 * SCC Error Handler to extract. Before this offset, Event Handler will
 * embed its Event Information. It better not cross this offset, otherwise,
 * it will end up over-writing the Event Id 
 * -SK
 */
#define ERROR_EVENT_ID_OFFSET  256


/* 
 * Hardware limits down here
 */
/*  Maximum number of Slots in a AXSS1600 */
#define    MAX_SLOTS      16  /* Max number of slots in AXSS1600 */

/* Maximum number of Card types in AXSS System; All card types must go here */
#define    MAX_CARD_TYPES  5  /* Max number of Card Types in AXSS1600 */
/*
 * Following are Front card types 
 * NOTE & WARNING: Please do not change the following card type assignments.
 *                 These are tied in with the HW definition. The things will
 *                 break very badly, if these are changed. So, HANDS OFF!!!!
 *                 BTW, feel free to add new ones, but do not re-assign these
 *                 numbers.
 *                 (Sukhdev Kapur)
 */
#define CARD_DIC    1
#define CARD_NMC    2
#define CARD_SCC    3
#define CARD_BSC    4
#define CARD_NSC    5
#define CARD_DMC    CARD_DIC  /* To accomodate Name Change */

#define FC_NMC      CARD_NMC
#define FC_DMC      CARD_DMC
#define FC_SCC      CARD_SCC
#define FC_BSC      CARD_BSC
#define FC_NSC      CARD_NSC

/*
 * Following are Back card types 
 */
#define BC_BLANK          1
#define BC_NMC_16T1E1     2
#define BC_RED_16T1E1     3
#define BC_DMC_6T3        4

#define BC_SCC_UI         5
#define BC_SCC_12T3       6
#define BC_SCC_12E3       7
#define BC_SCC_1GE        8
#define BC_SCC_4FE        9
#define BC_SCC_4OC3       10
#define BC_SCC_1OC12      11

#define BC_BSC_12T3       12
#define BC_BSC_12E3       13
#define BC_BSC_1GE        14
#define BC_BSC_4FE        15
#define BC_BSC_4OC3       16
#define BC_BSC_1OC12      17

/* a different back card is required for OC3 multi-mode line type */ 
#define BC_SCC_4OC3_MM    18      

/*
 * Following are Daughter card types 
 */
/* NarrowBand MultiService Modules (MSM) for NMC cards */
/* BroadBand Interface Modules (BIM) for SCC/BSC cards */
/* NOTE: These card types must be defined by Sukhdev   */
#define MSM_BLANK         0
#define MSM_DSP           1
#define MSM_CES           2
#define MSM_HDLC          3
#define MSM_DSPV          9
#define BIM_4FE           4
#define BIM_4OC3          5
#define BIM_12T3E3        6
#define BIM_OC12          7
#define BIM_1GE           8
#define BIM_CES_4T3E3    10

/* The following constant defines card state for all cards in AXSS chassis */
#define CARD_REMOVED        1
#define CARD_BOOT           2
#define CARD_ACTIVE         3
#define CARD_STANDBY        4
#define CARD_MISMATCH       5
#define CARD_FAIL           6
#define CARD_SECONDARY      7
#define CARD_UNKNOWN_STATE  8

/* The following constant defines interface types for AXSS chassis */
#define UNKNOWN_IF_TYPE     0
#define DS1_IF_TYPE         1
#define E1_IF_TYPE          2
#define DS3_IF_TYPE         3
#define E3_IF_TYPE          4
#define ENET_IF_TYPE        5
#define OC3_IF_TYPE         6
#define OC12_IF_TYPE        7
#define DMC_DS3_IF_TYPE     8

/* when shipping to Europe, default value should set to be E1_IF_TYPE*/
#define DEFAULT_IF_TYPE     1 

/* definitions for on-line diagnostic */
#define ON_LINE_DIAG_ON     0
#define ON_LINE_DIAG_OFF    1 

/* 
 * Following are the Mismatch Reasons for various card
 * configurations
 */
 /* WARNING: Please note that if you add a new literal here, you must
  *          inform SNMP person to add this into their MIB. If not done
  *          so, this will not be reflected to the user. You will be sorry
  *          for your this mistake. So, please be careful. If you do not 
  *          undertand these comments, please consult with the owner of this
  *          file. -Sukhdev Kapur
  */
#define MISMATCH_NONE	   0   /* No Mismatch; all OK */
#define MISMATCH_CONFIG	   1   /* Config Mismatches with HW */
#define MISMATCH_FC_BC 	   2   /* FC and BC Mismatch        */
#define MISMATCH_DC_BC 	   3   /* Duaghter Card and BC Mismatch  */
#define MISMATCH_PEER  	   4   /* Applies to SCC; Peer's HW Mismatch */
#define MISMATCH_DMC  	   5   /* DMC's Configuration Mismatches w/Hw */
#define MISMATCH_NO_BC	   6   /* NO backcard */
#define MISMATCH_NO_DC 	   7   /* none or invalid daughter cards */
#define MISMATCH_ETH_PORT  8   /* Mismatch 'cause of Ethernet Port */
#define MISMATCH_CHASSIS_CONFIG 9 /*Mismatch 'cause of E1 type NSC is existed  */
#define MISMATCH_PEER_SWFW 10  /*Mismatch 'cause SCCs have diff. SW and FW images */
 /* WARNING: Please note that if you add a new literal previous group, you must
  *          inform SNMP person to add this into their MIB. If not done
  *          so, this will not be reflected to the user. You will be sorry
  *          for your this mistake. So, please be careful. If you do not 
  *          undertand these comments, please consult with the owner of this
  *          file. -Sukhdev Kapur
  */


/* 
 * Following are the values of Interface Mode typese set by SNMP
 */
#define IF_MODE_BACKCARD      0  /* Back Card specified */
#define IF_MODE_BACKPLANE     1  /* Back Plane specified */
#define IF_MODE_NO_BACKCARD   2  /* No Back Card specified */


/* 
 * Following are the values Failure Reasons. This one is for SNMP
 */
 /* WARNING: Please note that if you add a new literal here, you must
  *          inform SNMP person to add this into their MIB. If not done
  *          so, this will not be reflected to the user. You will be sorry
  *          for your this mistake. So, please be careful. If you do not 
  *          undertand these comments, please consult with the owner of this
  *          file. -Sukhdev Kapur
  */
#define FAIL_REASON_NONE                0  /* No Failure specified */
#define FAIL_HW_MSM                     1   /* MSM failed */
#define FAIL_HW_SAR                     2   /* SAR failed */
#define FAIL_PCIA_ERR_INT               3   /* PCI-A error interrupt */
#define FAIL_ASX_FAILURE                4   /* ASX failure */
#define FAIL_ALBM_FAILURE               5   /* ALBM failure */
#define FAIL_CUBIT_FAILURE              6   /* cubit failure */
#define FAIL_BUS_CYCLE_TIMEOUT          7   /* bus cycle timeout failure */
#define FAIL_HARD_DRV_FAILURE           8   /* hard disk failed */
#define FAIL_MGMT_ETH_FAILURE           9   /* mgmt ethernet port failed */
#define FAIL_DMC_INIT                   10  /* DMC initialization failed */
#define FAIL_SERIAL_PORT_FAILURE        11  /* SERIAL port failed */
#define FAIL_BKGD_TASK_STARV            12  /* background task starvation */
#define FAIL_KEY_TASK_FAILURE           13  /* key task failed */
#define FAIL_REASON_SW                  14  /* General Software Failure */
#define FAIL_REASON_HW                  15  /* General Hardware Failure */
#define FAIL_HEARTBEAT_LOST             16  /* SM<->CM Hearbeat Lost    */
#define FAIL_ID_FAILED                  17  /* Image DownLoad Failed    */
#define FAIL_PLFM_ACTIVE                18  /* platform active timer expired */
#define FAIL_APP_INIT                   19  /* appl init failed */
#define FAIL_CFG_FAILED                 20  /* Config Download failed */
#define FAIL_REM_CARD_REPORT_FAILURE    21  /* Remote Card reported failed */
#define FAIL_PEER_SCC_FORCED_FAILURE    22  /* Rem SCC forcing us 2 mv to fail*/
#define FAIL_COMMUNICATION_LOST         23  /* Active SCC to Standyby SCC Comm Lost */
#define FAIL_ON_LINE_DIAG               24  /* On line diagnostic fails    */
 /* WARNING: Please note that if you add a new literal to previous group, you must
  *          inform SNMP person to add this into their MIB. If not done
  *          so, this will not be reflected to the user. You will be sorry
  *          for your this mistake. So, please be careful. If you do not 
  *          undertand these comments, please consult with the owner of this
  *          file. -Sukhdev Kapur
  */

/* 
 * Failure types - to be used by the applications when invoking
 * ErrHandler() api to report errors
 */
#define ERROR_FATAL     1  /* Fatal Error */
#define ERROR_MAJOR     2  /* Major Error */
#define ERROR_MINOR     3  /* Minor Error */
#define ERROR_INFO      4  /* Info Error */



/******************************************************************
* All System wide Modules
*/
/* 
 * All system wide module numbers go in here. One number per module.
 * Irregardless of which subsystem contains this module, it should be
 * deifned in here. MRCP uses it for RPC Communications
 */

#define    MAX_MODULES    64  /* Max number of modules in AXSS1600 */

/*
 * NOTE: If you ever add a module numebr which exceeds MAX_MODULES(defined
 *       above), make sure you update the value of MAX_MODULES; otherwise
 * 	 the communications between the modules will break down
 */
#define BOOT_MOD             0
#define SYS_MOD              1
#define MRPC_MOD             2       /* MRPC Module                  */
#define FM_MOD               3       /* File Manager Module          */
#define LOG_MOD              4
#define ID_MOD               5       /* Image Download Module        */
#define CM_MOD               6       /* Card Manager Module          */
#define SM_MOD               7       /* Shelf/Chassis Manager Module */
#define WEB_MOD              8       /* Web Manager Module */
#define SNMPD_MOD            9       /* SNMP Agent Module */
#define CRM_MOD              10      /* SCC (core) redundancy Module (CRM) */
#define HWPM_MOD             11      /* HW Poll Module */
#define DS1_MOD              12      /* COMET (T1/E1) Module */
#define PRC_MOD              13      /* Protocol Resource Control */
#define PCC_MOD              14      /* Protocol Channel Control */
#define PEC_MOD              15      /* Protocol Event Control */
#define TCG_MOD              16      /* telephony control group */
#define DBM_MOD              17      /* DataBase Manager Module */
#define ALBM_MOD             18      /* ABM/ALM Module */
#define RMG_MOD              19      /* RMG Module */
#define ALARM_MOD            20      /* ALARM Module */
#define MSCP_MOD             21      /* MSCP Module */
#define BIM_MOD              22      /* BIM Module */
#define CSM_MOD              23      /* CSM Module */
#define EMM_MOD              24      /* EMM Module */
#define TFTP_MOD             25      /* TFTP Module */
#define FRDM_MOD             26      /* FREEDM Module */
#define NWG_MOD              27      /* NWG Module */
#define TRM_MOD              28      /* TRM Module */
#define COMET_MOD            29      /* COMET Module */
#define SPATH_MOD            30      /* SPATH  Module */
#define OC3Q_MOD             31      /* OC3  Module */
#define ASX_MOD              32      /* ASX  Module */
#define SIW_MOD              33      /* SIW  Module */
#define CBT_MOD              34      /* SIW  Module */
#define SNMP_MOD             35      /* SNMP  Module */
#define PLFM_MOD             36      /* PLFM  Module */
#define D3MX_MOD             37      /* D3Mx  Module */
#define CUBE_MOD             38      /* CUBE  Module */
#define DMC_MOD              39      /* DMC  Module */
#define DSX_MOD              40      /* DSX  Module */
#define RTM_MOD              41      /* RTM  Module */
#define BDS1_MOD             42      /* BSC DS1  Module */
#define LAPD_MOD             43      /* LAPD Q.921  Module */
#define CLI_MOD              44      /* Cli Shell  Module */
#define MGCP_MOD             45      /* Cli Shell  Module */
#define MPC_MOD              46      /* MGCP Platform control */
#define IBP_MOD              47      /* ISDN backhaul module on SCC */
#define RUDP_MOD             48      /* RUDP module on SCC */
#define IBPMUX_MOD           49      /* ISDN Backhaul Mux module on SCC */
#define SONET_MOD            50      /* SONET Module */
#define OC3_MOD              51      /* OC3 Module */
#define SPM_MOD              52      /* SPM Moudle */
#define OND_MOD              53      /* On-Line Diagnostic */
#define MPCDBM_MOD           54      /* MPC database manager operations task*/
#define CMTHDLC_MOD          55      /* Comet HDLC Driver Module */
#define PCCDBM_MOD           56      /* Comet HDLC Driver Module */
#define LSP_MOD              57      /* LSP Module               */
#define OSPF_MOD             58      /* OSPF Module               */
#define RMEPRSP_MOD          59      /* OSPF Module               */
#define DBMAPPSYNC_MOD       60      /* DBmAPPSYNC Module         */
/* NOTE: MAX_DEF_MODS should not exceed MAX_Module */
#define MAX_DEF_MODS         61      /* Max. Number of modules defined */

/******************************************************************
*     System wide Macros
*/
/* These are some useful macros that can be used by any SW code.
 * Please add more cool stuff here....
 */
#define GET_SCC_SLOT_NUM() (8) /* For now always use slot #9 */
#define VALID_MODULE(m) ((((m) < 0) || ((m) >= MAX_MODULES)) ? FALSE : TRUE)
#define VALID_SCC_SLOT(s) ((((s) == 8) || ((s) == 9)) ? TRUE : FALSE)
#define VALID_BSC_SLOT(s) ((((s) >= 10) || ((s) <= 15)) ? TRUE : FALSE)
#ifdef SCC
#define PEER_SCC_PRESENT() \
((crm.peer.cStatus == CARD_PRESENT) ? TRUE : FALSE )

#endif /* SCC */

#define VALID_SCC_CARD(c) (((c) == FC_SCC) ? TRUE : FALSE)
#define VALID_DMC_CARD(c) (((c) == FC_DMC) ? TRUE : FALSE)
#define VALID_DMC_BC(c) (((c) == BC_DMC_6T3) ? TRUE : FALSE)

#define VALID_SCC_BC(s) \
      ((((s) == BC_SCC_UI) || ((s) == BC_SCC_12T3) || ((s) == BC_SCC_12E3) || \
       ((s) == BC_SCC_4FE) || ((s) == BC_SCC_1GE) || ((s) == BC_SCC_4OC3) || \
       ((s) == BC_SCC_4OC3_MM)) ? TRUE : FALSE)
#define VALID_SCC_DC(s) \
      ((((s) == BIM_4FE) || ((s) == BIM_4OC3) || ((s) == BIM_12T3E3) || \
       ((s) == BIM_OC12) || ((s) == BIM_1GE) ) ? TRUE : FALSE)

#define VALID_SCC_DC_BC(d, b) \
      (((((d) == BIM_4FE) && ((b) == BC_SCC_4FE)) ||     \
        (((d) == BIM_4OC3) && ((b) == BC_SCC_4OC3)) ||   \
        (((d) == BIM_4OC3) && ((b) == BC_SCC_4OC3_MM)))  \
       ? TRUE : FALSE)

#define VALID_SERVICE_CARD(c) \
	 ((((c) == FC_NMC) || ((c) == FC_BSC) || ((c) == FC_NSC)) ? TRUE : FALSE)

#define VALID_NMC_BC(s) ((((s) == BC_BLANK) || ((s) == BC_NMC_16T1E1) || \
                        ((s) == BC_RED_16T1E1)) ? TRUE : FALSE)
#define VALID_NMC_DC(s) ((((s) == MSM_DSP) || ((s) == MSM_DSPV) || \
                        ((s) == MSM_CES) || ((s) == MSM_HDLC)) ? TRUE : FALSE)

#define VALID_NSC_BC(s) ((((s) == BC_BLANK) || ((s) == BC_NMC_16T1E1) || \
                        ((s) == BC_RED_16T1E1)) ? TRUE : FALSE)
#define VALID_NSC_DC(s) (((s) == MSM_DSP) || ((s) == MSM_DSPV) || \
         ((s) == MSM_CES) || ((s) == MSM_HDLC) || ((s) == MSM_BLANK))

#define VALID_BSC_BC(s) (((s) == BC_BSC_12T3) || ((s) == BC_BSC_12E3) || \
                         ((s) == BC_BSC_1GE) || ((s) == BC_BSC_4FE) || \
                         ((s) == BC_BSC_4OC3) || ((s) == BC_BSC_1OC12) || \
                         ((s) == BC_DMC_6T3))
#if 1
#define VALID_BSC_DC(s) (((s) == BIM_4FE) || ((s) == BIM_4OC3) || ((s) == BIM_CES_4T3E3) || \
                         ((s) == BIM_12T3E3) || ((s) == BIM_OC12) || \
                         ((s) == BIM_1GE) || ((s) == MSM_BLANK))
#else /* 1 */
#define VALID_BSC_DC(s) (1)
#endif /* 1 */

#define VALID_SLOT(s) (((U32_t)(s) >= MAX_SLOTS) ? FALSE : TRUE)
#define VALID_SERVICE(s) ((((s) < 0) || ((s) >= MAX_SERVICES)) ? FALSE : TRUE)
#define VALID_ADDRESS(a) (((a) == NULL) ? FALSE : TRUE)
#define VALID_SEMID(s) (((s) == NULL) ? FALSE : TRUE)
#define VALID_FILE_NAME(n) \
     ((((n) != NULL) &&    \
      ((strlen(n) > 0) && (strlen(n) <= MAX_FILE_NAME_LENGTH))) ? TRUE : FALSE)

#define VALID_IF_MODE(m) (((m) ==IF_MODE_BACKPLANE) ||  \
         ((m) == IF_MODE_BACKCARD) || ((m) == IF_MODE_NO_BACKCARD))
/* 
 * Following macro should be used to get ticks for whole seconds only
 */
#define SECONDS(s) (sysClkRateGet() * (s))


/******************************************************************
* Typedefs
*/
/* Following definition of system wide buffers */


typedef struct l2Hdr
{
    S16_t   txSeq;     /* Tx Seq Number */
    S16_t   ackSeq;    /* ACK Seq Number */
    U8_t    msgAck;    /* Msg/Ack Flag one bit for Ack and 1 bit for Msg  */
    U8_t    pri;       /* Priority of the Msg */
    U8_t    sarIntr;   /* SAR interrupt flag */
    U8_t    resv8;     /* Reserved 8 bits */
}L2_HDR_t;


typedef struct mrpcHrd 
{
   U16_t    portNum;
   U16_t    srcSlotNum;
   U16_t    txId;
   U8_t     req;  /* SYNC or ASYNC */
   U8_t     msg;
   S32_t    err;
   U8_t     congInd[4];   /* MRPC congestion indication */
   L2_HDR_t l2;
} MRPC_HDR_t;


#define MAX_BUFF_LEN   1964
#define MAX_PAD_TO_2032   8
typedef struct buffer 
{
   U32_t               fe:1;
   U32_t               hold:1;
   U32_t               hi:1;
   U32_t               no:13;         /* length of the data in this buffer */
   U32_t               v_110:1;
   U32_t               rsv1:2;
   U32_t               no13:1;
   U32_t               csm:1;
   U32_t               fnum:11;
   struct buffer       *next;          /* next buffer in buffer chain */
   U8_t                *dataPtr;       /* 1st data byte address */
   U32_t               usedCnt:8;      /* buffer used count
                                        * the sarFrameBufRet put the 
                                        * buffer to the free pool if
                                        * usedCnt == 0
                                        * This can be used to simulate
                                        * the buffer copy or retransmission
                                        * After the buffer is transmitted by
                                        * the sarFrameBufTx() function,
                                        * the sarFrameBufRet() is called to 
                                        * return the buffer to the free pool
                                        * if the application needs to hold the
                                        * buffer for retransmission, it should 
                                        * call sarFrameBufCntUp() which
                                        * increment the usedCnt by 1
                                        */
   U32_t               reserved_1:3;
   U32_t               edmaFreeSel:3; 
   U32_t               edmaBuffFree:1;
   U32_t               edmaBuffLarge:1;
   U32_t               edmaEgrBfsNum:16;
   U32_t               frLength:16;     /* length of the frame 
                                         * equal to length of data 
                                         * if no buffer chainning
                                         */
   U32_t               reserved_2:1;  
   U32_t               frEfci:1;
   U32_t               frClp:1;
   U32_t               ConNum:13;
   struct buffer       *frBfdTailPtr;    /* point to end of frame buffer */
   MRPC_HDR_t          mrpcHdr;
   U8_t                data[MAX_BUFF_LEN];
   U32_t               sscopTrailer;
   U8_t                reserved1[MAX_PAD_TO_2032];
} BUFFER_t;

typedef BUFFER_t MSG_t;

#define MSG_BUFFER_SIZE   sizeof (MSG_t)

#ifndef SIM
/* this routine is contributed by slin */
#define CYCLES()  ({  \
        int __z ;  __asm__ ("mftb %0"  : "=r" (__z)); \
        __z; })
#else
#define CYCLES()  (0)
#endif

typedef struct mgcpIpdcCfg
{
    S32_t  ipdcActive;
}MGCP_IPDC_CFG_t;

typedef struct ifTypeCfg
{
    S32_t  t1Active;
}IFTYPE_CFG_t;

typedef struct 
{
    S32_t  SccUpgradeActive;
}SCC_IN_UPGRADE_t;

#endif /* AXSS_IF_H */

