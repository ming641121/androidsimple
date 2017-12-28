/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file contains system wide Event definitions. All events must be
 *    defined here before they can be used in individual include files
 *    such as xxx_events.h
 *
 * Wanrning:
 * 
 *    Additions to this file, i.e. definitions of new events, MUST be made
 *    by the author of this file. Please contact him before making any 
 *    change to this file. There may be very serious ramifications on the
 *    system behaviour if this file is modified without first understanding
 *    the ramifications. Therefore, please consult with the owner of this
 *    file.
 *
 * Note:
 *    - Please be carefull when you modify this file. This file is included
 *	by every module in the system. Since it is created in the second
 *      phase of development, you may not see it directly in a specific 
 *      file, but it is indirectly in every module. So be careful!!!! 
 *
 * Author(s):
 
 *   
 */

/* added events for online diagnostics on 05/10/2001 by ysrini */

#ifndef AXSS_EVENTS_IF_H
#define AXSS_EVENTS_IF_H
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

#define   MAX_FATAL_PER_MOD     20
#define   MAX_MAJOR_PER_MOD     180
#define   MAX_MINOR_PER_MOD     100
#define   MAX_INFO_PER_MOD      100

#define   FATAL_START           (0)
#define   MAJOR_START           (MAX_FATAL_PER_MOD)
#define   MINOR_START           (MAJOR_START + MAX_MAJOR_PER_MOD)
#define   INFO_START            (MINOR_START + MAX_MINOR_PER_MOD)

#define   MAX_EV                (MAX_FATAL_PER_MOD +  \
                                 MAX_INFO_PER_MOD +  \
                                 MAX_MAJOR_PER_MOD +  \
                                 MAX_MINOR_PER_MOD)
       
#define  GET_EVENT_MOD(e)      ((e)/MAX_EV)
#define  EXTRACT_EVENT(e)      ((e) - (GET_EVENT_MOD(e) * MAX_EV))






/******************************************************************
 * All System Wide Fatal Events go in here 
 */
/* SYS Fatal Events */
#define _SYS_PCIERR                      (SYS_MOD * MAX_EV) + 1
#define _SYS_SAREXCEP                    (SYS_MOD * MAX_EV) + 2
#define _SYS_CTASKISDEAD                 (SYS_MOD * MAX_EV) + 3

#if 0
/* RTM Fatal Events  */
#define _RTM_RTM_FATAL                   (RTM_MOD * MAX_EV) + 1
#endif

/* ALM/ABM Fatal Events */
#define _ALBM_ABMMEMPRTYERR              (ALBM_MOD * MAX_EV) + 1
#define _ALBM_ATLINITFAIL                (ALBM_MOD * MAX_EV) + 2

/* DSE Fatal Events */
#define _TCG_ALLDSEFAIL                  (TCG_MOD * MAX_EV) + 1
#define _TCG_ALLMSMFAIL                  (TCG_MOD * MAX_EV) + 2
/* MRPC Fatal Events */
#define _MRPC_L2_RETX_THRES              (MRPC_MOD * MAX_EV) + 1
/* LAPD Q.921 Fatal events */
#define _LD_DB_CREATE_ERR                (LAPD_MOD * MAX_EV) + 1



/******************************************************************
 * All System Wide MAJOR Events go in here 
 */
/* Freedom  Major Events */
#define _FRDM_SEM_CREATE_FAILED    ((FRDM_MOD*MAX_EV)+MAJOR_START)+1
#define _FRDM_INIT_FAIL            ((FRDM_MOD*MAX_EV)+MAJOR_START)+2
#define _FRDM_RPD_INIT_FAIL        ((FRDM_MOD*MAX_EV)+MAJOR_START)+3
#define _FRDM_SFQ_INIT_FAIL        ((FRDM_MOD*MAX_EV)+MAJOR_START)+4
#define _FRDM_TPDLIST_INIT_FAIL    ((FRDM_MOD*MAX_EV)+MAJOR_START)+5

/* Comet HDLC Major Events */
#define _CMTHDLC_SEM_CREATE_FAILED    ((CMTHDLC_MOD*MAX_EV)+MAJOR_START)+1
#define _CMTHDLC_INIT_FAIL            ((CMTHDLC_MOD*MAX_EV)+MAJOR_START)+2
#define _CMTHDLC_RPD_INIT_FAIL        ((CMTHDLC_MOD*MAX_EV)+MAJOR_START)+3
#define _CMTHDLC_SFQ_INIT_FAIL        ((CMTHDLC_MOD*MAX_EV)+MAJOR_START)+4
#define _CMTHDLC_TPDLIST_INIT_FAIL    ((CMTHDLC_MOD*MAX_EV)+MAJOR_START)+5

/* IMage Downloader  Major Events */
#define _ID_INIT_FAIL              ((ID_MOD*MAX_EV)+MAJOR_START)+1
#define _ID_RCV_MSG_NULL           ((ID_MOD*MAX_EV)+MAJOR_START)+2

/* MRPC  Major Events */
#define _MRPC_SERVER_TABLE_SEM            ((MRPC_MOD*MAX_EV)+MAJOR_START)+1
#define _MRPC_FORWARD_Q_ERR               ((MRPC_MOD*MAX_EV)+MAJOR_START)+2
#define _MRPC_NEXT_LAYER_ERR              ((MRPC_MOD*MAX_EV)+MAJOR_START)+3
#define _MRPC_INIT_ERR                    ((MRPC_MOD*MAX_EV)+MAJOR_START)+4
#define _MRPC_NULL_RECV_ERR               ((MRPC_MOD*MAX_EV)+MAJOR_START)+5
#define _MRPC_TXID_FAIL                   ((MRPC_MOD*MAX_EV)+MAJOR_START)+6
#define _MRPC_FAILED_ALLOC_MSG_BUF        ((MRPC_MOD*MAX_EV)+MAJOR_START)+7
#define _MRPC_L2_TMR_FAIL                 ((MRPC_MOD*MAX_EV)+MAJOR_START)+8
#define _MRPC_L2_CB_SEM                   ((MRPC_MOD*MAX_EV)+MAJOR_START)+9
#define _MRPC_OUT_OF_SAR_BUFFS            ((MRPC_MOD*MAX_EV)+MAJOR_START)+10
#define _MRPC_MRPC_ADD_FUNC_FAILED        ((MRPC_MOD*MAX_EV)+MAJOR_START)+11
#define _MRPC_CLIENT_TABLE_SEM            ((MRPC_MOD*MAX_EV)+MAJOR_START)+12

/* NWG  Major Events */
#define _NWG_IOCTLRDFAIL           ((NWG_MOD*MAX_EV)+MAJOR_START)+1
#define _NWG_DSCRINITFAIL          ((NWG_MOD*MAX_EV)+MAJOR_START)+2

/* Shelf Manager  Major Events */
#define _SM_INIT_FAIL              ((SM_MOD*MAX_EV)+MAJOR_START)+1
#define _SM_RCV_MSG_NULL           ((SM_MOD*MAX_EV)+MAJOR_START)+2
#define _SM_READ_CFG_FILE_FAILED   ((SM_MOD*MAX_EV)+MAJOR_START)+3
#define _SM_FILE_CREATE_FAILED     ((SM_MOD*MAX_EV)+MAJOR_START)+4
#define _SM_READ_SHELF_CFG_FAILED  ((SM_MOD*MAX_EV)+MAJOR_START)+5
#define _SM_MRPC_ADD_PORT_FAILED   ((SM_MOD*MAX_EV)+MAJOR_START)+6
#define _SM_FILE_OPEN_FAILED       ((SM_MOD*MAX_EV)+MAJOR_START)+7
#define _SM_POLL_TIMER_FAIL        ((SM_MOD*MAX_EV)+MAJOR_START)+8
#define _SM_RED_TABLE_CORRUPT      ((SM_MOD*MAX_EV)+MAJOR_START)+9
#define _SM_WATCH_TIMER_FAIL       ((SM_MOD*MAX_EV)+MAJOR_START)+10
#define _SM_NULL_ADDR              ((SM_MOD*MAX_EV)+MAJOR_START)+11


/* System  Major Events */
#define _SYS_INITACKFAIL           ((SYS_MOD*MAX_EV)+MAJOR_START)+1
#define _SYS_BGNDTASKSUSPEND       ((SYS_MOD*MAX_EV)+MAJOR_START)+2
#define _SYS_TASKISDEAD            ((SYS_MOD*MAX_EV)+MAJOR_START)+3
#define _SYS_TASKSPAWNFAIL         ((SYS_MOD*MAX_EV)+MAJOR_START)+4
#define _SYS_BADDISK               ((SYS_MOD*MAX_EV)+MAJOR_START)+5
#define _SYS_CLKSYNFAIL            ((SYS_MOD*MAX_EV)+MAJOR_START)+6

/* SIW Major Events  */
#define _SIW_SEMDLKRESET           ((SIW_MOD*MAX_EV)+MAJOR_START)+1
#define _SIW_OBJCREATENOMEM        ((SIW_MOD*MAX_EV)+MAJOR_START)+2
#define _SIW_TASKSEMCRTFAIL        ((SIW_MOD*MAX_EV)+MAJOR_START)+3
#define _SIW_TASKSPAWNFAIL         ((SIW_MOD*MAX_EV)+MAJOR_START)+4
#define _SIW_MQCREATEFAIL          ((SIW_MOD*MAX_EV)+MAJOR_START)+5
#define _SIW_SEMCREATEFAIL         ((SIW_MOD*MAX_EV)+MAJOR_START)+6
#define _SIW_TMRCREATEFAIL         ((SIW_MOD*MAX_EV)+MAJOR_START)+7

/* TRM  Major Events */
#define _TRM_SOCK_INIT_FAIL        ((TRM_MOD*MAX_EV)+MAJOR_START)+1

/* BIM  Major Events */
#define _BIM_DB_FAIL               ((BIM_MOD*MAX_EV)+MAJOR_START)+1
#define _BIM_TBLENTRY_FAIL         ((BIM_MOD*MAX_EV)+MAJOR_START)+2
#define _BIM_TBLREAD_FAIL          ((BIM_MOD*MAX_EV)+MAJOR_START)+3
#define _BIM_TBLCMT_FAIL           ((BIM_MOD*MAX_EV)+MAJOR_START)+4
#define _BIM_RCV_MSG_FAIL          ((BIM_MOD*MAX_EV)+MAJOR_START)+5
#define _BIM_MSG_TYPE_FAIL         ((BIM_MOD*MAX_EV)+MAJOR_START)+6
#define _BIM_SAR_FAIL              ((BIM_MOD*MAX_EV)+MAJOR_START)+7
#define _BIM_LINK_FAIL             ((BIM_MOD*MAX_EV)+MAJOR_START)+8
#define _BIM_SOCK_INIT_FAIL        ((BIM_MOD*MAX_EV)+MAJOR_START)+9
#define _BIM_TRAP_PDU_FAIL         ((BIM_MOD*MAX_EV)+MAJOR_START)+10
#define _BIM_DBTBL_FAIL            ((BIM_MOD*MAX_EV)+MAJOR_START)+11
#define _BIM_SAR_FAIL_EV           ((BIM_MOD*MAX_EV)+MAJOR_START)+12

/* Card Manager  Major Events */
#define _CM_BOOT_STATE_TIMER_FAIL    ((CM_MOD*MAX_EV)+MAJOR_START)+1
#define _CM_INIT_FAIL                ((CM_MOD*MAX_EV)+MAJOR_START)+2
#define _CM_RCV_MSG_NULL             ((CM_MOD*MAX_EV)+MAJOR_START)+3
#define _CM_NULL_ADDR                ((CM_MOD*MAX_EV)+MAJOR_START)+4
#define _CM_FAIL_TO_Q_POST_STATUS    ((CM_MOD*MAX_EV)+MAJOR_START)+5
#define _CM_FAIL_TO_SPAWN_APP_TASKS  ((CM_MOD*MAX_EV)+MAJOR_START)+6
#define _CM_FAIL_TO_OPEN_FILE        ((CM_MOD*MAX_EV)+MAJOR_START)+7
#define _CM_FAIL_TO_READ_FILE        ((CM_MOD*MAX_EV)+MAJOR_START)+8
#define _CM_CKSUM_FAILED             ((CM_MOD*MAX_EV)+MAJOR_START)+9
#define _CM_IDLE_STATE_TIMER_FAIL    ((CM_MOD*MAX_EV)+MAJOR_START)+10
#define _CM_DSEFAIL                  ((CM_MOD*MAX_EV)+MAJOR_START)+11
#define _CM_POLL_TIMER_FAIL          ((CM_MOD*MAX_EV)+MAJOR_START)+12


/* Comet  Major Events */
#define _COMET_DBCREATFAL       ((COMET_MOD*MAX_EV)+MAJOR_START)+1
#define _COMET_INDCREATFAL      ((COMET_MOD*MAX_EV)+MAJOR_START)+2
#define _COMET_TBLCREATFAL      ((COMET_MOD*MAX_EV)+MAJOR_START)+3
#define _COMET_INDREGACCFAIL    ((COMET_MOD*MAX_EV)+MAJOR_START)+4


/* BSC DS1  Major Events */
#define _BDS1_DBCREATFAIL       ((BDS1_MOD*MAX_EV)+MAJOR_START)+1
#define _BDS1_INDCREATFAIL      ((BDS1_MOD*MAX_EV)+MAJOR_START)+2
#define _BDS1_TBLCREATFAIL      ((BDS1_MOD*MAX_EV)+MAJOR_START)+3

/* Core Redundancy Module  Major Events */
#define _CRM_INIT_FAIL                     ((CRM_MOD*MAX_EV)+MAJOR_START)+1
#define _CRM_RCV_MSG_NULL                  ((CRM_MOD*MAX_EV)+MAJOR_START)+2
#define _CRM_NULL_ADDR                     ((CRM_MOD*MAX_EV)+MAJOR_START)+3
#define _CRM_MRPC_ADD_PORT_FAILED          ((CRM_MOD*MAX_EV)+MAJOR_START)+4
#define _CRM_MRPC_ADD_FUNC_FAILED          ((CRM_MOD*MAX_EV)+MAJOR_START)+5
#define _CRM_PLFM_ACTV_STATE_TIMER_FAIL    ((CRM_MOD*MAX_EV)+MAJOR_START)+6
#define _CRM_MASTER_SYNC_STATE_TIMER_FAIL  ((CRM_MOD*MAX_EV)+MAJOR_START)+7
#define _CRM_SLAVE_SYNC_STATE_TIMER_FAIL   ((CRM_MOD*MAX_EV)+MAJOR_START)+8
#define _CRM_HW_BAD_STATE                  ((CRM_MOD*MAX_EV)+MAJOR_START)+9
#define _CRM_BAD_STATE_PEER                ((CRM_MOD*MAX_EV)+MAJOR_START)+10
#define _CRM_BAD_STATE_SM                  ((CRM_MOD*MAX_EV)+MAJOR_START)+11
#define _CRM_FILE_OPEN_FAILED              ((CRM_MOD*MAX_EV)+MAJOR_START)+12
#define _CRM_FILE_CREATE_FAILED            ((CRM_MOD*MAX_EV)+MAJOR_START)+13
#define _CRM_READ_CFG_FAILED               ((CRM_MOD*MAX_EV)+MAJOR_START)+14
#define _CRM_DMC_INIT_FAILED               ((CRM_MOD*MAX_EV)+MAJOR_START)+15
#define _CRM_FAIL_TO_SPAWN_DMCINIT_TASK    ((CRM_MOD*MAX_EV)+MAJOR_START)+16
#define _CRM_SW_BAD_STATE                  ((CRM_MOD*MAX_EV)+MAJOR_START)+17
#define _CRM_DBM_SYNC_FAILED               ((CRM_MOD*MAX_EV)+MAJOR_START)+18
#define _CRM_APPS_INIT_FAILED              ((CRM_MOD*MAX_EV)+MAJOR_START)+19
#define _CRM_HW_IN_BAD_STATE               ((CRM_MOD*MAX_EV)+MAJOR_START)+20
#define _CRM_REM_SCC_PRESENT_TIMER_FAIL    ((CRM_MOD*MAX_EV)+MAJOR_START)+21
#define _CRM_WATCH_TIMER_FAIL              ((CRM_MOD*MAX_EV)+MAJOR_START)+22

/* Clock Source Module  Major Events */
#define _CSM_HW_CHECK_FAILED               ((CSM_MOD*MAX_EV)+MAJOR_START)+1
#define _CSM_HW_SELECT_FAILED              ((CSM_MOD*MAX_EV)+MAJOR_START)+2

/* Data Base Manager Module  Major Events */
#define _DBM_INIT_FAIL                  ((DBM_MOD*MAX_EV)+MAJOR_START)+1
#define _DBM_RCV_MSG_NULL               ((DBM_MOD*MAX_EV)+MAJOR_START)+2
#define _DBM_MRPC_ADD_PORT_FAILED       ((DBM_MOD*MAX_EV)+MAJOR_START)+3
#define _DBM_MRPC_ADD_FUNC_FAILED       ((DBM_MOD*MAX_EV)+MAJOR_START)+4
#define _DBM_FAIL_2Q_MSG                ((DBM_MOD*MAX_EV)+MAJOR_START)+5
#define _DBM_WRITE_LSEEK_FAIL           ((DBM_MOD*MAX_EV)+MAJOR_START)+6
#define _DBM_FILE_WRITE_FAIL            ((DBM_MOD*MAX_EV)+MAJOR_START)+7
#define _DBM_READ_LSEEK_FAIL            ((DBM_MOD*MAX_EV)+MAJOR_START)+8
#define _DBM_FILE_READ_FAIL             ((DBM_MOD*MAX_EV)+MAJOR_START)+9
#define _DBM_DB_FILE_WRITE_FAIL         ((DBM_MOD*MAX_EV)+MAJOR_START)+10
#define _DBM_DB_FILE_OPEN_FAIL          ((DBM_MOD*MAX_EV)+MAJOR_START)+11
#define _DBM_DB_FILE_READ_FAIL          ((DBM_MOD*MAX_EV)+MAJOR_START)+12
#define _DBM_FAIL_TO_OPEN_FILE          ((DBM_MOD*MAX_EV)+MAJOR_START)+13
#define _DBM_MKDIR_FAIL                 ((DBM_MOD*MAX_EV)+MAJOR_START)+14
#define _DBM_TBLCREATE_OUT_OF_MEM       ((DBM_MOD*MAX_EV)+MAJOR_START)+15
#define _DBM_ENTRY_TABLE_SEM            ((DBM_MOD*MAX_EV)+MAJOR_START)+16
#define _DBM_COMIT_OUT_OF_MEM           ((DBM_MOD*MAX_EV)+MAJOR_START)+17
#define _DBM_FAIL_TBLCREATE_FILE        ((DBM_MOD*MAX_EV)+MAJOR_START)+18
#define _DBM_COMIT_MSG_CKSUM_MISMATCH   ((DBM_MOD*MAX_EV)+MAJOR_START)+19
#define _DBM_NULL_ADDR                  ((DBM_MOD*MAX_EV)+MAJOR_START)+20
#define _DBM_PEER_COMIT_START_WO_END    ((DBM_MOD*MAX_EV)+MAJOR_START)+21
#define _DBM_PEER_COMIT_CONT_WO_START   ((DBM_MOD*MAX_EV)+MAJOR_START)+22
#define _DBM_PEER_COMIT_MISSING_BUFFERS ((DBM_MOD*MAX_EV)+MAJOR_START)+23
#define _DBM_REM_UNTAR_FAIL             ((DBM_MOD*MAX_EV)+MAJOR_START)+24
#define _DBM_SYNC_PEER_RESTORE_FAIL     ((DBM_MOD*MAX_EV)+MAJOR_START)+25
#define _DBM_FAIL_TO_SPAWN_DBMSERV_TASK ((DBM_MOD*MAX_EV)+MAJOR_START)+26
#define _DBM_PEER_CB_FN_FAILED          ((DBM_MOD*MAX_EV)+MAJOR_START)+27
#define _DBM_PEER_CREATE_DB_FAIL        ((DBM_MOD*MAX_EV)+MAJOR_START)+28
#define _DBM_PEER_CREATE_TBL_FAIL       ((DBM_MOD*MAX_EV)+MAJOR_START)+29
#define _DBM_PEER_CREATE_ENTRY_FAIL     ((DBM_MOD*MAX_EV)+MAJOR_START)+30
#define _DBM_PEER_DELETE_DB_FAIL        ((DBM_MOD*MAX_EV)+MAJOR_START)+31
#define _DBM_PEER_DELETE_TABLE_FAIL     ((DBM_MOD*MAX_EV)+MAJOR_START)+32
#define _DBM_PEER_DELETE_ENTRY_FAIL     ((DBM_MOD*MAX_EV)+MAJOR_START)+33
#define _DBM_PEER_COMIT_ENTRY_FAIL      ((DBM_MOD*MAX_EV)+MAJOR_START)+34
#define _DBM_UPGRADE_MISMATCH           ((DBM_MOD*MAX_EV)+MAJOR_START)+35
#define _DBM_DNGRADE_MISMATCH           ((DBM_MOD*MAX_EV)+MAJOR_START)+36
#define _DBM_SYNC_SAVE_TABLES_FAIL      ((DBM_MOD*MAX_EV)+MAJOR_START)+37
#define _DBM_SYNC_RESTORE_TABLES_FAIL   ((DBM_MOD*MAX_EV)+MAJOR_START)+38
#define _DBM_SYNC_REMOVE_OLD_TABLES_FAIL ((DBM_MOD*MAX_EV)+MAJOR_START)+39


/* File Manager Major Events */
#define _FM_ADD_PORT_FAILED     ((FM_MOD*MAX_EV)+MAJOR_START)+1
#define _FM_RCV_MSG_NULL        ((FM_MOD*MAX_EV)+MAJOR_START)+2
#define _FM_FMWRITE_FAIL        ((FM_MOD*MAX_EV)+MAJOR_START)+3
#define _FM_OUT_OF_FDPS         ((FM_MOD*MAX_EV)+MAJOR_START)+4
#define _FM_SEM_ERROR           ((FM_MOD*MAX_EV)+MAJOR_START)+5
#define _FM_WR_OVERRUN          ((FM_MOD*MAX_EV)+MAJOR_START)+6
#define _FM_WR_ILLEGAL_FT       ((FM_MOD*MAX_EV)+MAJOR_START)+7
#define _FM_NULL_ADDR           ((FM_MOD*MAX_EV)+MAJOR_START)+8
#define _FM_ILLEGAL_FDP         ((FM_MOD*MAX_EV)+MAJOR_START)+9
#define _FM_FAIL_2Q_MSG         ((FM_MOD*MAX_EV)+MAJOR_START)+10
#define _FM_FAIL_OPEN_HD_FILE   ((FM_MOD*MAX_EV)+MAJOR_START)+11
#define _FM_INIT_FAIL           ((FM_MOD*MAX_EV)+MAJOR_START)+12
#define _FM_ADD_MICS_FILES_RAM_FAIL  ((FM_MOD*MAX_EV)+MAJOR_START)+13

/* Environment Monitoring Module  Major Events */
#define _EMM_EMMMAJTRAP         ((EMM_MOD*MAX_EV)+MAJOR_START)+1

/* MSCP Major Events */
#define _MSCP_READ_TSK_FAILED   ((MSCP_MOD*MAX_EV)+MAJOR_START)+1
#define _MSCP_INIT              ((MSCP_MOD*MAX_EV)+MAJOR_START)+2
#define _MSCP_TSKQ_RD           ((MSCP_MOD*MAX_EV)+MAJOR_START)+3
#define _MSCP_SPAWN_FAILED      ((MSCP_MOD*MAX_EV)+MAJOR_START)+4
#define _MSCP_INVALID_PEER      ((MSCP_MOD*MAX_EV)+MAJOR_START)+5

/* MPC Major Events */
#define _MPC_DBM_CREATE_EVENT   ((MPC_MOD*MAX_EV)+MAJOR_START)+1
#define _MPC_INIT_EVENT         ((MPC_MOD*MAX_EV)+MAJOR_START)+2
#define _MPC_DS0_XCONN_ALRM     ((MPC_MOD*MAX_EV)+MAJOR_START)+3
#define _MPC_DS0_XCONN_ALRM_BULK     ((MPC_MOD*MAX_EV)+MAJOR_START)+4
#define _MPC_DS0_XCONN_ALRM_OP_FAIL  ((MPC_MOD*MAX_EV)+MAJOR_START)+5

/* PCC Major Events */
#define _PCC_DBM_CREATE_EVENT   ((PCC_MOD*MAX_EV)+MAJOR_START)+1
#define _PCC_INIT_EVENT         ((PCC_MOD*MAX_EV)+MAJOR_START)+2

/* PRC Major Events */
#define _PRC_DBM_CREATE_EVENT   ((PRC_MOD*MAX_EV)+MAJOR_START)+1
#define _PRC_INIT_EVENT         ((PRC_MOD*MAX_EV)+MAJOR_START)+2

/* ASX  Major Events */
#define _ASX_ASXINITFAIL        ((ASX_MOD*MAX_EV)+MAJOR_START)+1
#define _ASX_TASKSPAWNFAIL      ((ASX_MOD*MAX_EV)+MAJOR_START)+2

/* CBT  Major Events */
#define _CBT_CBTSANITY          ((CBT_MOD*MAX_EV)+MAJOR_START)+1
#define _CBT_CBTWRTRDCMP        ((CBT_MOD*MAX_EV)+MAJOR_START)+2
#define _CBT_TASKSPAWNFAIL      ((CBT_MOD*MAX_EV)+MAJOR_START)+3
#define _CBT_RECV_BIP8_FAIL     ((CBT_MOD*MAX_EV)+MAJOR_START)+4

/* ALBM  Major Events */
#define _ALBM_ABMINITFAIL       ((ALBM_MOD*MAX_EV)+MAJOR_START)+1
#define _ALBM_ALMINITFAIL       ((ALBM_MOD*MAX_EV)+MAJOR_START)+2
#define _ALBM_TASKSPAWNFAIL     ((ALBM_MOD*MAX_EV)+MAJOR_START)+3

/* TCG Major Events */
#define _TCG_DSEFAIL            ((TCG_MOD*MAX_EV)+MAJOR_START)+1
#define _TCG_MSMFAIL            ((TCG_MOD*MAX_EV)+MAJOR_START)+2

#if 0
/* RTM Major Events */
#define _RTM_RTM_MAJOR          ((RTM_MOD*MAX_EV)+MAJOR_START)+1
#define _RTM_INIT_FAIL  ((RTM_MOD*MAX_EV)+MAJOR_START)+2
#define _RTM_NULL_ADDR  ((RTM_MOD*MAX_EV)+MAJOR_START)+3
#define _RTM_RCV_MSG_FAIL       ((RTM_MOD*MAX_EV)+MAJOR_START)+4
#define _RTM_RCV_MSG_NULL       ((RTM_MOD*MAX_EV)+MAJOR_START)+5
#define _RTM_MRPC_ADD_PORT_FAILED       ((RTM_MOD*MAX_EV)+MAJOR_START)+6
#define _RTM_MRPC_ADD_FUNC_FAILED       ((RTM_MOD*MAX_EV)+MAJOR_START)+7
#define _RTM_FAIL_2Q_MSG        ((RTM_MOD*MAX_EV)+MAJOR_START)+8
#define _RTM_FILE_OPEN_FAIL     ((RTM_MOD*MAX_EV)+MAJOR_START)+9
#define _RTM_FILE_READ_FAIL     ((RTM_MOD*MAX_EV)+MAJOR_START)+10
#define _RTM_FILE_WRITE_FAIL    ((RTM_MOD*MAX_EV)+MAJOR_START)+11
#endif

/* LSP Major Events */
#define _LSP_INIT_EVENT            ((LSP_MOD*MAX_EV)+MAJOR_START)+1

/******************************************************************
 * Following are all MINOR Events defined 
 */
#define _FRDM_TPD_SEND_FAILED      ((FRDM_MOD*MAX_EV)+MINOR_START)+1
#define _FRDM_OUT_OF_TX_DESC       ((FRDM_MOD*MAX_EV)+MINOR_START)+2
#define _FRDM_RECV_ERR             ((FRDM_MOD*MAX_EV)+MINOR_START)+3
#define _FRDM_NULL_RECV_ERR        ((FRDM_MOD*MAX_EV)+MINOR_START)+4

/* Comet HDLC Minor events */
#define _CMTHDLC_TPD_SEND_FAILED      ((CMTHDLC_MOD*MAX_EV)+MINOR_START)+1
#define _CMTHDLC_OUT_OF_TX_DESC       ((CMTHDLC_MOD*MAX_EV)+MINOR_START)+2
#define _CMTHDLC_RECV_ERR             ((CMTHDLC_MOD*MAX_EV)+MINOR_START)+3
#define _CMTHDLC_NULL_RECV_ERR        ((CMTHDLC_MOD*MAX_EV)+MINOR_START)+4

/* TRM  Minor Events */
#define _TRM_DB_FAIL               ((TRM_MOD*MAX_EV)+MINOR_START)+1
#define _TRM_TBLENTRY_FAIL         ((TRM_MOD*MAX_EV)+MINOR_START)+2
#define _TRM_TBLCMT_FAIL           ((TRM_MOD*MAX_EV)+MINOR_START)+3
#define _TRM_RCV_MSG_FAIL          ((TRM_MOD*MAX_EV)+MINOR_START)+4
#define _TRM_MSG_TYPE_FAIL         ((TRM_MOD*MAX_EV)+MINOR_START)+5
#define _TRM_OBJ_INIT_FAIL         ((TRM_MOD*MAX_EV)+MINOR_START)+6
#define _TRM_TRAP_PDU_FAIL         ((TRM_MOD*MAX_EV)+MINOR_START)+7
#define _TRM_DBTBL_FAIL            ((TRM_MOD*MAX_EV)+MINOR_START)+8

/* Switch Path  Minor Evenets */
#define _SPATH_ALBMADDCON       ((SPATH_MOD*MAX_EV)+MINOR_START)+1
#define _SPATH_SARDEREGECN      ((SPATH_MOD*MAX_EV)+MINOR_START)+2
#define _SPATH_ALBMDELCON       ((SPATH_MOD*MAX_EV)+MINOR_START)+3
#define _SPATH_SARREGECN        ((SPATH_MOD*MAX_EV)+MINOR_START)+4

/* System  Minor Evenets */
#define _SYS_FAILRECVTASK       ((SYS_MOD*MAX_EV)+MINOR_START)+1
#define _SYS_ISRNOTCALLABLE     ((SYS_MOD*MAX_EV)+MINOR_START)+2
#define _SYS_NVRSEMTAKEFAIL     ((SYS_MOD*MAX_EV)+MINOR_START)+3
#define _SYS_NVRAMCKSMFAIL      ((SYS_MOD*MAX_EV)+MINOR_START)+4
#define _SYS_UNKNOWNCRDTYP      ((SYS_MOD*MAX_EV)+MINOR_START)+5
#define _SYS_EXCEPTION          ((SYS_MOD*MAX_EV)+MINOR_START)+6
#define _SYS_CPUISHOLD          ((SYS_MOD*MAX_EV)+MINOR_START)+7
#define _SYS_MEMSHORT           ((SYS_MOD*MAX_EV)+MINOR_START)+8
#define _SYS_INVALIDCOMETID     ((SYS_MOD*MAX_EV)+MINOR_START)+9

/* Cubit Id Minor Events */
#define _CUBE_CUBESTATUS        ((CUBE_MOD*MAX_EV)+MINOR_START)+1

/* ALBM  Minor Events */
#define _ALBM_OPENINGCONNFAIL   ((ALBM_MOD*MAX_EV)+MINOR_START)+1
#define _ALBM_OPENEGCONNFAIL    ((ALBM_MOD*MAX_EV)+MINOR_START)+2
#define _ALBM_TASKDELAYFAIL     ((ALBM_MOD*MAX_EV)+MINOR_START)+3
#define _ALBM_ABMERRSTATUS      ((ALBM_MOD*MAX_EV)+MINOR_START)+4
#define _ALBM_ALMPRTYERR        ((ALBM_MOD*MAX_EV)+MINOR_START)+5

/* OC3Q  Minor Events */
#define _OC3Q_OC3QSTATUS        ((OC3Q_MOD*MAX_EV)+MINOR_START)+1

/* RMG  Minor Events */
#define _RMG_RM_EVT_ERR_LOG     ((RMG_MOD*MAX_EV)+MINOR_START)+1

/* Alarm  Minor Events */
#define _ALARM_ALM_EVT_ERR_LOG  ((ALARM_MOD*MAX_EV)+MINOR_START)+1

/* ASX  Minor Events */
#define _ASX_TASKDELAYFAIL      ((ASX_MOD*MAX_EV)+MINOR_START)+1
#define _ASX_ASXERRSTATUS       ((ASX_MOD*MAX_EV)+MINOR_START)+2

/* CBT  Minor Events */
#define _CBT_TASKDELAYFAIL      ((CBT_MOD*MAX_EV)+MINOR_START)+1
#define _CBT_CBTERRSTATUS       ((CBT_MOD*MAX_EV)+MINOR_START)+2
#define _CBT_CBTGENERR          ((CBT_MOD*MAX_EV)+MINOR_START)+3

/* TCG Minor Events */
#define _TCG_DSPFAIL            ((TCG_MOD*MAX_EV)+MINOR_START)+1
#define _TCG_DSPC6FAIL          ((TCG_MOD*MAX_EV)+MINOR_START)+2
/* COMET  Minor Events */
#define _COMET_DBREADFAL        ((COMET_MOD*MAX_EV)+MINOR_START)+1
#define _COMET_DBWRITEFAL       ((COMET_MOD*MAX_EV)+MINOR_START)+2
#define _COMET_DBCMTFAL         ((COMET_MOD*MAX_EV)+MINOR_START)+3
#define _COMET_INDRMFAL         ((COMET_MOD*MAX_EV)+MINOR_START)+4
#define _COMET_BERTTASKFAL      ((COMET_MOD*MAX_EV)+MINOR_START)+5
#define _COMET_MAJALARM         ((COMET_MOD*MAX_EV)+MINOR_START)+6
#define _COMET_MINALARM         ((COMET_MOD*MAX_EV)+MINOR_START)+7
#define _COMET_PERFMAJALARM     ((COMET_MOD*MAX_EV)+MINOR_START)+8
#define _COMET_PERFMINALARM     ((COMET_MOD*MAX_EV)+MINOR_START)+9
#define _COMET_LOSON            ((COMET_MOD*MAX_EV)+MINOR_START)+10
#define _COMET_LOSOFF           ((COMET_MOD*MAX_EV)+MINOR_START)+11
#define _COMET_OOFON            ((COMET_MOD*MAX_EV)+MINOR_START)+12
#define _COMET_OOFOFF           ((COMET_MOD*MAX_EV)+MINOR_START)+13
#define _COMET_FRAMINGERR       ((COMET_MOD*MAX_EV)+MINOR_START)+14
#define _COMET_UASON            ((COMET_MOD*MAX_EV)+MINOR_START)+15
#define _COMET_UASOFF           ((COMET_MOD*MAX_EV)+MINOR_START)+16
#define _COMET_OOMFON            ((COMET_MOD*MAX_EV)+MINOR_START)+17
#define _COMET_OOMFOFF           ((COMET_MOD*MAX_EV)+MINOR_START)+18
#define _COMET_OOSMFON            ((COMET_MOD*MAX_EV)+MINOR_START)+19
#define _COMET_OOSMFOFF           ((COMET_MOD*MAX_EV)+MINOR_START)+20
#define _COMET_INDREGERR          ((COMET_MOD*MAX_EV)+MINOR_START)+21



/* BSC DS1  Minor Events */
#define _BDS1_DBREADFAIL       ((BDS1_MOD*MAX_EV)+MINOR_START)+1
#define _BDS1_DBWRITEFAIL      ((BDS1_MOD*MAX_EV)+MINOR_START)+2
#define _BDS1_DBCMTFAIL        ((BDS1_MOD*MAX_EV)+MINOR_START)+3
#define _BDS1_INDRMFAIL        ((BDS1_MOD*MAX_EV)+MINOR_START)+4
#define _BDS1_BERTTASKFAIL     ((BDS1_MOD*MAX_EV)+MINOR_START)+5
#define _BDS1_MAJALARM         ((BDS1_MOD*MAX_EV)+MINOR_START)+6
#define _BDS1_MINALARM         ((BDS1_MOD*MAX_EV)+MINOR_START)+7
#define _BDS1_PERFMAJALARM     ((BDS1_MOD*MAX_EV)+MINOR_START)+8
#define _BDS1_PERFMINALARM     ((BDS1_MOD*MAX_EV)+MINOR_START)+9
#define _BDS1_LOSON            ((BDS1_MOD*MAX_EV)+MINOR_START)+10
#define _BDS1_LOSOFF           ((BDS1_MOD*MAX_EV)+MINOR_START)+11
#define _BDS1_OOFON            ((BDS1_MOD*MAX_EV)+MINOR_START)+12
#define _BDS1_OOFOFF           ((BDS1_MOD*MAX_EV)+MINOR_START)+13
#define _BDS1_FRAMINGERR       ((BDS1_MOD*MAX_EV)+MINOR_START)+14
#define _BDS1_UASON            ((BDS1_MOD*MAX_EV)+MINOR_START)+15
#define _BDS1_UASOFF           ((BDS1_MOD*MAX_EV)+MINOR_START)+16
#define _BDS1_HWFAILURE        ((BDS1_MOD*MAX_EV)+MINOR_START)+17

/* Core Redundancy Module Minor Events */
#define _CRM_ILLEGAL_MSG        ((CRM_MOD*MAX_EV)+MINOR_START)+1
#define _CRM_STATE_ERROR        ((CRM_MOD*MAX_EV)+MINOR_START)+2
#define _CRM_STATE_ERR_RET      ((CRM_MOD*MAX_EV)+MINOR_START)+3
#define _CRM_RCV_MSG_FAIL       ((CRM_MOD*MAX_EV)+MINOR_START)+4
#define _CRM_SCC_FAILED         ((CRM_MOD*MAX_EV)+MINOR_START)+5

/* Clock Source Module Minor Events */
#define _CSM_INIT_FAILED        ((CSM_MOD*MAX_EV)+MINOR_START)+1
#define _CSM_DBM_CB_FAILED      ((CSM_MOD*MAX_EV)+MINOR_START)+2
#define _CSM_U_CONFIG_FAILED    ((CSM_MOD*MAX_EV)+MINOR_START)+3
#define _CSM_U_SELECT_FAILED    ((CSM_MOD*MAX_EV)+MINOR_START)+4
#define _CSM_CLK_SWITCH         ((CSM_MOD*MAX_EV)+MINOR_START)+5
#define _CSM_CLK_MAJOR_ALRM     ((CSM_MOD*MAX_EV)+MINOR_START)+6
#define _CSM_CLK_MINOR_ALRM     ((CSM_MOD*MAX_EV)+MINOR_START)+7
#define _CSM_CLK_ALRM_CLEAR     ((CSM_MOD*MAX_EV)+MINOR_START)+8

/* DS3 Module Minor Events */
#define _D3MX_D3MXSTATUS        ((D3MX_MOD*MAX_EV)+MINOR_START)+1

/* OC3 Module Minor Events */
#define _OC3_OC3STATUS          ((OC3_MOD*MAX_EV)+MINOR_START)+1
#define _OC3_OC3HWFAILURE       ((OC3_MOD*MAX_EV)+MINOR_START)+2

/* DataBase Module Minor Events */
#define _DBM_ILLEGAL_MSG        ((DBM_MOD*MAX_EV)+MINOR_START)+1

/* DMC Module Minor Events */
#define _DMC_ADD_ON_EMPTY_T3    ((DMC_MOD*MAX_EV)+MINOR_START)+1
#define _DMC_DBM_ADD_FAILED     ((DMC_MOD*MAX_EV)+MINOR_START)+2
#define _DMC_DBM_ADD_ON_EXIST_SRC_NUM   ((DMC_MOD*MAX_EV)+MINOR_START)+3
#define _DMC_DBM_HD_UPD_N_CMT   ((DMC_MOD*MAX_EV)+MINOR_START)+4
#define _DMC_DBM_DEL_NOT_CONN   ((DMC_MOD*MAX_EV)+MINOR_START)+5

/* DSX Module Minor Events */
#define _DSX_DSXSTATUS          ((DSX_MOD*MAX_EV)+MINOR_START)+1
#define _DSX_DSXMAJALARM        ((DSX_MOD*MAX_EV)+MINOR_START)+2
#define _DSX_DSXPERFMAJALARM    ((DSX_MOD*MAX_EV)+MINOR_START)+3
#define _DSX_DSXMINALARM        ((DSX_MOD*MAX_EV)+MINOR_START)+4
#define _DSX_DSXPERFMINALARM    ((DSX_MOD*MAX_EV)+MINOR_START)+5
#define _DSX_NOSYSMEM           ((DSX_MOD*MAX_EV)+MINOR_START)+6

/* SONET Module Minor Events */
#define _SONET_SNTSTATUS          ((SONET_MOD*MAX_EV)+MINOR_START)+1
#define _SONET_SNTMAJALARM        ((SONET_MOD*MAX_EV)+MINOR_START)+2
#define _SONET_SNTPERFMAJALARM    ((SONET_MOD*MAX_EV)+MINOR_START)+3
#define _SONET_SNTMINALARM        ((SONET_MOD*MAX_EV)+MINOR_START)+4
#define _SONET_SNTPERFMINALARM    ((SONET_MOD*MAX_EV)+MINOR_START)+5
#define _SONET_NOSYSMEM           ((SONET_MOD*MAX_EV)+MINOR_START)+6

/* Environmental Monitoring  Module Minor Events */
#define _EMM_TEMPREADFAIL       ((EMM_MOD*MAX_EV)+MINOR_START)+1
#define _EMM_VOLREADFAIL        ((EMM_MOD*MAX_EV)+MINOR_START)+2
#define _EMM_FANMISSED          ((EMM_MOD*MAX_EV)+MINOR_START)+3
#define _EMM_RPMREADFAIL        ((EMM_MOD*MAX_EV)+MINOR_START)+4
#define _EMM_DBCREATFAIL        ((EMM_MOD*MAX_EV)+MINOR_START)+5
#define _EMM_TBLCREATFAIL       ((EMM_MOD*MAX_EV)+MINOR_START)+6
#define _EMM_TBLUPDATFAIL       ((EMM_MOD*MAX_EV)+MINOR_START)+7
#define _EMM_EMMMINTRAP         ((EMM_MOD*MAX_EV)+MINOR_START)+8
#define _EMM_EMMTRAPFAIL        ((EMM_MOD*MAX_EV)+MINOR_START)+9
#define _EMM_NOSARMEM           ((EMM_MOD*MAX_EV)+MINOR_START)+10

/* MSCP Minor Events */
#define _MSCP_MSCP_LDAP_CFG     ((MSCP_MOD*MAX_EV)+MINOR_START)+1
#define _MSCP_FDSET_ERR         ((MSCP_MOD*MAX_EV)+MINOR_START)+2
#define _MSCP_SOCKOPT           ((MSCP_MOD*MAX_EV)+MINOR_START)+3
#define _MSCP_DB_INV_ELMT       ((MSCP_MOD*MAX_EV)+MINOR_START)+4
#define _MSCP_DYNMEM_ALLOC_ERR  ((MSCP_MOD*MAX_EV)+MINOR_START)+5
#define _MSCP_IPC_STAT_ALLOC    ((MSCP_MOD*MAX_EV)+MINOR_START)+6
#define _MSCP_IPC_MSG_ALLOC     ((MSCP_MOD*MAX_EV)+MINOR_START)+7
#define _MSCP_IPC_ERROR         ((MSCP_MOD*MAX_EV)+MINOR_START)+8
#define _MSCP_DBMERR            ((MSCP_MOD*MAX_EV)+MINOR_START)+9

/* MPC Minor Events */
#define _MPC_FSMILLEGAL_EVENT   ((MPC_MOD*MAX_EV)+MINOR_START)+1
#define _MPC_MSG_EVENT          ((MPC_MOD*MAX_EV)+MINOR_START)+2
#define _MPC_TMR_EVENT          ((MPC_MOD*MAX_EV)+MINOR_START)+3
#define _MPC_DBM_EVENT          ((MPC_MOD*MAX_EV)+MINOR_START)+4
#define _MPC_FSMTRUNK_EVENT     ((MPC_MOD*MAX_EV)+MINOR_START)+5
#define _MPC_MSGQSEND_EVENT     ((MPC_MOD*MAX_EV)+MINOR_START)+6
#define _MPC_MEMALLOC_EVENT     ((MPC_MOD*MAX_EV)+MINOR_START)+7
#define _MPC_SEM_EVENT          ((MPC_MOD*MAX_EV)+MINOR_START)+8

/* PCC Minor Events */
#define _PCC_FSMILLEGAL_EVENT   ((PCC_MOD*MAX_EV)+MINOR_START)+1
#define _PCC_MSG_EVENT          ((PCC_MOD*MAX_EV)+MINOR_START)+2
#define _PCC_TMR_EVENT          ((PCC_MOD*MAX_EV)+MINOR_START)+3
#define _PCC_DBM_EVENT          ((PCC_MOD*MAX_EV)+MINOR_START)+4
#define _PCC_FSMTRUNK_EVENT     ((PCC_MOD*MAX_EV)+MINOR_START)+5
#define _PCC_MSGQSEND_EVENT     ((PCC_MOD*MAX_EV)+MINOR_START)+6
#define _PCC_MEMALLOC_EVENT     ((PCC_MOD*MAX_EV)+MINOR_START)+7
#define _PCC_SEM_EVENT          ((PCC_MOD*MAX_EV)+MINOR_START)+8

/* PCC Minor Events */
#define _PCC_FSMILLEGAL_EVENT   ((PCC_MOD*MAX_EV)+MINOR_START)+1
#define _PCC_MSG_EVENT          ((PCC_MOD*MAX_EV)+MINOR_START)+2
#define _PCC_TMR_EVENT          ((PCC_MOD*MAX_EV)+MINOR_START)+3
#define _PCC_DBM_EVENT          ((PCC_MOD*MAX_EV)+MINOR_START)+4
#define _PCC_FSMTRUNK_EVENT     ((PCC_MOD*MAX_EV)+MINOR_START)+5
#define _PCC_MSGQSEND_EVENT     ((PCC_MOD*MAX_EV)+MINOR_START)+6
#define _PCC_MEMALLOC_EVENT     ((PCC_MOD*MAX_EV)+MINOR_START)+7
#define _PCC_SEM_EVENT          ((PCC_MOD*MAX_EV)+MINOR_START)+8

/* PRC Minor Events */
#define _PRC_SCSFSM_EVENT       ((PRC_MOD*MAX_EV)+MINOR_START)+1
#define _PRC_SUPFSM_EVENT       ((PRC_MOD*MAX_EV)+MINOR_START)+2
#define _PRC_TRAP_EVENT         ((PRC_MOD*MAX_EV)+MINOR_START)+3
#define _PRC_MSG_EVENT          ((PRC_MOD*MAX_EV)+MINOR_START)+4
#define _PRC_TMR_EVENT          ((PRC_MOD*MAX_EV)+MINOR_START)+5
#define _PRC_MSGQSEND_EVENT     ((PRC_MOD*MAX_EV)+MINOR_START)+6
#define _PRC_MEMALLOC_EVENT     ((PRC_MOD*MAX_EV)+MINOR_START)+7
#define _PRC_SEM_EVENT          ((PRC_MOD*MAX_EV)+MINOR_START)+8
#define _PRC_COTFSM_EVENT       ((PRC_MOD*MAX_EV)+MINOR_START)+9

#if 0
/* RTM Minor Events */
#define _RTM_RTM_MINOR          ((RTM_MOD*MAX_EV)+MINOR_START)+1
#define _RTM_ILLEGAL_MSG        ((RTM_MOD*MAX_EV)+MINOR_START)+2
#endif

/* SIW Minor Events */
#define _SIW_ISRNOTCALLABLE             ((SIW_MOD*MAX_EV)+MINOR_START)+1
#define _SIW_TBLSEMTAKEFAIL             ((SIW_MOD*MAX_EV)+MINOR_START)+2
#define _SIW_TASKTBLFULL                ((SIW_MOD*MAX_EV)+MINOR_START)+3
#define _SIW_TASKSEMGIVEFAIL            ((SIW_MOD*MAX_EV)+MINOR_START)+4
#define _SIW_TASKINITPTO                ((SIW_MOD*MAX_EV)+MINOR_START)+5
#define _SIW_TASKINITFAIL               ((SIW_MOD*MAX_EV)+MINOR_START)+6
#define _SIW_TASKIDDELETED              ((SIW_MOD*MAX_EV)+MINOR_START)+7
#define _SIW_TASKIDINVALID              ((SIW_MOD*MAX_EV)+MINOR_START)+8
#define _SIW_TASKDELFAIL                ((SIW_MOD*MAX_EV)+MINOR_START)+9
#define _SIW_TASKDELSAFE                ((SIW_MOD*MAX_EV)+MINOR_START)+10
#define _SIW_TASKNAMEFAIL               ((SIW_MOD*MAX_EV)+MINOR_START)+11
#define _SIW_TASKPRIOINVALID            ((SIW_MOD*MAX_EV)+MINOR_START)+12
#define _SIW_TASKPGRPINVALID            ((SIW_MOD*MAX_EV)+MINOR_START)+13
#define _SIW_TASKAPPLINVALID            ((SIW_MOD*MAX_EV)+MINOR_START)+14
#define _SIW_TASKSTKSIZEINV             ((SIW_MOD*MAX_EV)+MINOR_START)+15
#define _SIW_TASKSUSPACTINV             ((SIW_MOD*MAX_EV)+MINOR_START)+16
#define _SIW_TASKFAILACTINV             ((SIW_MOD*MAX_EV)+MINOR_START)+17
#define _SIW_TASKINITACKTO              ((SIW_MOD*MAX_EV)+MINOR_START)+18
#define _SIW_TASKWACKFAIL               ((SIW_MOD*MAX_EV)+MINOR_START)+19
#define _SIW_TASKDELMQOWN               ((SIW_MOD*MAX_EV)+MINOR_START)+20
#define _SIW_MQTBLFULL                  ((SIW_MOD*MAX_EV)+MINOR_START)+21
#define _SIW_MQIDINVALID                ((SIW_MOD*MAX_EV)+MINOR_START)+22
#define _SIW_MQIDDELETED                ((SIW_MOD*MAX_EV)+MINOR_START)+23
#define _SIW_MQSEMDELFAIL               ((SIW_MOD*MAX_EV)+MINOR_START)+24
#define _SIW_MQDELFAIL                  ((SIW_MOD*MAX_EV)+MINOR_START)+25
#define _SIW_MQSENDLENINV               ((SIW_MOD*MAX_EV)+MINOR_START)+26
#define _SIW_MQSENDFAIL                 ((SIW_MOD*MAX_EV)+MINOR_START)+27
#define _SIW_MQSEMGIVEFAIL              ((SIW_MOD*MAX_EV)+MINOR_START)+28
#define _SIW_MQRECVLENINV               ((SIW_MOD*MAX_EV)+MINOR_START)+29
#define _SIW_MQRECVFAIL                 ((SIW_MOD*MAX_EV)+MINOR_START)+30
#define _SIW_MQNUMMSGFAIL               ((SIW_MOD*MAX_EV)+MINOR_START)+31
#define _SIW_MQINFOPTRINV               ((SIW_MOD*MAX_EV)+MINOR_START)+32
#define _SIW_MQINFOFAIL                 ((SIW_MOD*MAX_EV)+MINOR_START)+33
#define _SIW_MQMAXINVALID               ((SIW_MOD*MAX_EV)+MINOR_START)+34
#define _SIW_MQMLENINVALID              ((SIW_MOD*MAX_EV)+MINOR_START)+35
#define _SIW_MQOWNERINVALID             ((SIW_MOD*MAX_EV)+MINOR_START)+36
#define _SIW_MQOWNERNOTEXIST            ((SIW_MOD*MAX_EV)+MINOR_START)+37
#define _SIW_MQSEMCRTFAIL               ((SIW_MOD*MAX_EV)+MINOR_START)+38
#define _SIW_MQFLSHRECVFAIL             ((SIW_MOD*MAX_EV)+MINOR_START)+39
#define _SIW_SEMTBLFULL                 ((SIW_MOD*MAX_EV)+MINOR_START)+40
#define _SIW_SEMIDINVALID               ((SIW_MOD*MAX_EV)+MINOR_START)+41
#define _SIW_SEMIDDELETED               ((SIW_MOD*MAX_EV)+MINOR_START)+42
#define _SIW_SEMDELUNOWN                ((SIW_MOD*MAX_EV)+MINOR_START)+43
#define _SIW_SEMDELFAIL                 ((SIW_MOD*MAX_EV)+MINOR_START)+44
#define _SIW_SEMTAKEFAIL                ((SIW_MOD*MAX_EV)+MINOR_START)+45
#define _SIW_SEMGIVEUNOWN               ((SIW_MOD*MAX_EV)+MINOR_START)+46
#define _SIW_SEMGIVEFAIL                ((SIW_MOD*MAX_EV)+MINOR_START)+47
#define _SIW_SEMFLUSHINV                ((SIW_MOD*MAX_EV)+MINOR_START)+48
#define _SIW_SEMFLUSHFAIL               ((SIW_MOD*MAX_EV)+MINOR_START)+49
#define _SIW_SEMRECVINV                 ((SIW_MOD*MAX_EV)+MINOR_START)+50
#define _SIW_SEMTYPEINVALID             ((SIW_MOD*MAX_EV)+MINOR_START)+51
#define _SIW_SEMOPTINVALID              ((SIW_MOD*MAX_EV)+MINOR_START)+52
#define _SIW_SEMSTATEINVALID            ((SIW_MOD*MAX_EV)+MINOR_START)+53
#define _SIW_SEMDLKLOGERR               ((SIW_MOD*MAX_EV)+MINOR_START)+54
#define _SIW_SEMDLKRESTAR               ((SIW_MOD*MAX_EV)+MINOR_START)+55
#define _SIW_TMERTBLFULL                ((SIW_MOD*MAX_EV)+MINOR_START)+56
#define _SIW_TMRIDINVALID               ((SIW_MOD*MAX_EV)+MINOR_START)+57
#define _SIW_TMRNOMEM                   ((SIW_MOD*MAX_EV)+MINOR_START)+58
#define _SIW_TMRFUNCIDINV               ((SIW_MOD*MAX_EV)+MINOR_START)+59
#define _SIW_TMRCNNTFAIL                ((SIW_MOD*MAX_EV)+MINOR_START)+60
#define _SIW_TMRSTMFAIL                 ((SIW_MOD*MAX_EV)+MINOR_START)+61
#define _SIW_TMRFUNCINVAL               ((SIW_MOD*MAX_EV)+MINOR_START)+62
#define _SIW_TMRMSGQINVAL               ((SIW_MOD*MAX_EV)+MINOR_START)+63
#define _SIW_TMRMSGSNDFAIL              ((SIW_MOD*MAX_EV)+MINOR_START)+64
#define _SIW_NOSTATICMEM                ((SIW_MOD*MAX_EV)+MINOR_START)+65
#define _SIW_MEMPTRINVALID              ((SIW_MOD*MAX_EV)+MINOR_START)+66
#define _SIW_WRNGMEMPTR                 ((SIW_MOD*MAX_EV)+MINOR_START)+67
#define _SIW_NOSYSMEM                   ((SIW_MOD*MAX_EV)+MINOR_START)+68
#define _SIW_WRNGMEMSIZE                ((SIW_MOD*MAX_EV)+MINOR_START)+69
#define _SIW_NODYNMEM                   ((SIW_MOD*MAX_EV)+MINOR_START)+70
#define _SIW_INVALBUFPTR                ((SIW_MOD*MAX_EV)+MINOR_START)+71
#define _SIW_INVALBUFPTRN               ((SIW_MOD*MAX_EV)+MINOR_START)+72
#define _SIW_DYNMEMCORRUPT              ((SIW_MOD*MAX_EV)+MINOR_START)+73

/* SM Minor Events */
#define _SM_ILLEGAL_MSG             ((SM_MOD*MAX_EV)+MINOR_START)+1
#define _SM_STATE_ERROR             ((SM_MOD*MAX_EV)+MINOR_START)+2
#define _SM_STATE_CHANGE_ERR        ((SM_MOD*MAX_EV)+MINOR_START)+3
#define _SM_STATE_ERR_RET           ((SM_MOD*MAX_EV)+MINOR_START)+4
#define _SM_RCV_MSG_FAIL            ((SM_MOD*MAX_EV)+MINOR_START)+5
#define _SM_PING_RESP_NULL          ((SM_MOD*MAX_EV)+MINOR_START)+6
#define _SM_RAM_DB_REPLICATION_FAIL ((SM_MOD*MAX_EV)+MINOR_START)+7
#define _SM_SM_RM_SYNC_ERR          ((SM_MOD*MAX_EV)+MINOR_START)+8


/* SNMP Minor Events */
#define _SNMP_LOGINALERT        ((SNMP_MOD*MAX_EV)+MINOR_START)+1

/* ID Minor Events */
#define _ID_ILLEGAL_MSG            ((ID_MOD*MAX_EV)+MINOR_START)+1
#define _ID_RCV_MSG_FAIL           ((ID_MOD*MAX_EV)+MINOR_START)+2

/* File Manager Minor Events */
#define _FM_RCV_MSG_FAIL        ((FM_MOD*MAX_EV)+MINOR_START)+1
#define _FM_ILLEGAL_MSG         ((FM_MOD*MAX_EV)+MINOR_START)+2
#define _FM_MRPC_ASYNC_FAIL     ((FM_MOD*MAX_EV)+MINOR_START)+3
#define _FM_ILLEGAL_FILE        ((FM_MOD*MAX_EV)+MINOR_START)+4
#define _FM_FMCREATE_FAIL       ((FM_MOD*MAX_EV)+MINOR_START)+5
#define _FM_FMDELETE_FAIL       ((FM_MOD*MAX_EV)+MINOR_START)+6
#define _FM_FMOPEN_FAIL         ((FM_MOD*MAX_EV)+MINOR_START)+7
#define _FM_FMCLOSE_FAIL        ((FM_MOD*MAX_EV)+MINOR_START)+8
#define _FM_FMREAD_FAIL         ((FM_MOD*MAX_EV)+MINOR_START)+9
#define _FM_FMRENAME_FAIL       ((FM_MOD*MAX_EV)+MINOR_START)+10
#define _FM_ADD_MICS_FILES_HD_FAIL       ((FM_MOD*MAX_EV)+MINOR_START)+11

/* MRPC Minor Events */
#define _MRPC_SEMID_ERR             ((MRPC_MOD*MAX_EV)+MINOR_START)+1
#define _MRPC_RECV_ERR              ((MRPC_MOD*MAX_EV)+MINOR_START)+2
#define _MRPC_ILLEGAL_SERV          ((MRPC_MOD*MAX_EV)+MINOR_START)+3
#define _MRPC_ILLEGAL_REQ           ((MRPC_MOD*MAX_EV)+MINOR_START)+4
#define _MRPC_UNKN_MSG              ((MRPC_MOD*MAX_EV)+MINOR_START)+5
#define _MRPC_CORRUPTED_TXID        ((MRPC_MOD*MAX_EV)+MINOR_START)+6
#define _MRPC_L2_TMR_ILLEGAL_SLOT   ((MRPC_MOD*MAX_EV)+MINOR_START)+7
#define _MRPC_MRPC_INVALID_PRIORITY ((MRPC_MOD*MAX_EV)+MINOR_START)+8
#define _MRPC_TX_SEM                ((MRPC_MOD*MAX_EV)+MINOR_START)+9
#define _MRPC_MRPC_OUT_OF_TASKS     ((MRPC_MOD*MAX_EV)+MINOR_START)+10
#define _MRPC_FAIL_TO_SPAWN_MRPCSERV_TASK ((MRPC_MOD*MAX_EV)+MINOR_START)+11
#define _MRPC_TCG_CONGESTED         ((MRPC_MOD*MAX_EV)+MINOR_START)+12

/* Card Manager  Minor Events */
#define _CM_STATE_ERROR         ((CM_MOD*MAX_EV)+MINOR_START)+1
#define _CM_RCV_MSG_FAIL        ((CM_MOD*MAX_EV)+MINOR_START)+2
#define _CM_ILLEGAL_MSG         ((CM_MOD*MAX_EV)+MINOR_START)+3
#define _CM_STATE_ERR_RET       ((CM_MOD*MAX_EV)+MINOR_START)+4
#define _CM_STATE_CHANGE_ERR    ((CM_MOD*MAX_EV)+MINOR_START)+5

/* Data Base  Manager  Minor Events */
#define _DBM_DELETE_HD_INVALID_ENTRY          ((DBM_MOD*MAX_EV)+MINOR_START)+1
#define _DBM_UPDATE_HD_INVALID_ENTRY          ((DBM_MOD*MAX_EV)+MINOR_START)+2
#define _DBM_READ_HD_INVALID_ENTRY            ((DBM_MOD*MAX_EV)+MINOR_START)+3
#define _DBM_COMIT_HD_INVALID_ENTRY           ((DBM_MOD*MAX_EV)+MINOR_START)+4
#define _DBM_PEER_DB_FAIL                     ((DBM_MOD*MAX_EV)+MINOR_START)+5
#define _DBM_PEER_TBL_FAIL                    ((DBM_MOD*MAX_EV)+MINOR_START)+6
#define _DBM_PEER_ENTRY_CREATE_FAIL           ((DBM_MOD*MAX_EV)+MINOR_START)+7
#define _DBM_PEER_ENTRY_UPDATE_FAIL           ((DBM_MOD*MAX_EV)+MINOR_START)+8
#define _DBM_DELETING_NON_EXISTANT_ENTRY      ((DBM_MOD*MAX_EV)+MINOR_START)+9
#define _DBM_DELETING_NON_EXISTANT_TABLE      ((DBM_MOD*MAX_EV)+MINOR_START)+10
#define _DBM_DELETING_NON_EXISTANT_DB         ((DBM_MOD*MAX_EV)+MINOR_START)+11
#define _DBM_PEER_DB_EXISTS_AT_DIFF_LOC       ((DBM_MOD*MAX_EV)+MINOR_START)+12
#define _DBM_PEER_TBL_EXISTS_AT_DIFF_LOC      ((DBM_MOD*MAX_EV)+MINOR_START)+13
#define _DBM_REM_DB_EXISTS_AT_DIFF_LOC        ((DBM_MOD*MAX_EV)+MINOR_START)+14
#define _DBM_REM_TBL_EXISTS_AT_DIFF_LOC       ((DBM_MOD*MAX_EV)+MINOR_START)+15
#define _DBM_REM_COMIT_START_WO_END           ((DBM_MOD*MAX_EV)+MINOR_START)+16
#define _DBM_REM_COMIT_CONT_WO_START          ((DBM_MOD*MAX_EV)+MINOR_START)+17
#define _DBM_REM_COMIT_MISSING_BUFFERS        ((DBM_MOD*MAX_EV)+MINOR_START)+18
#define _DBM_ILLEGAL_COMIT_MSG                ((DBM_MOD*MAX_EV)+MINOR_START)+19
#define _DBM_REM_CREATE_DB_FAIL               ((DBM_MOD*MAX_EV)+MINOR_START)+27
#define _DBM_REM_CREATE_TBL_FAIL              ((DBM_MOD*MAX_EV)+MINOR_START)+28
#define _DBM_REM_CREATE_ENTRY_FAIL            ((DBM_MOD*MAX_EV)+MINOR_START)+29
#define _DBM_REM_DELETE_DB_FAIL               ((DBM_MOD*MAX_EV)+MINOR_START)+30
#define _DBM_REM_DELETE_TABLE_FAIL            ((DBM_MOD*MAX_EV)+MINOR_START)+31
#define _DBM_REM_DELETE_ENTRY_FAIL            ((DBM_MOD*MAX_EV)+MINOR_START)+32
#define _DBM_REM_COMIT_ENTRY_FAIL             ((DBM_MOD*MAX_EV)+MINOR_START)+33
#define _DBM_SEND_PEER_CREATE_DB_FAIL         ((DBM_MOD*MAX_EV)+MINOR_START)+34
#define _DBM_SEND_PEER_CREATE_TBL_FAIL        ((DBM_MOD*MAX_EV)+MINOR_START)+35
#define _DBM_SEND_PEER_CREATE_ENTRY_FAIL      ((DBM_MOD*MAX_EV)+MINOR_START)+36
#define _DBM_SEND_PEER_DELETE_DB_FAIL         ((DBM_MOD*MAX_EV)+MINOR_START)+37
#define _DBM_SEND_PEER_DELETE_TBL_FAIL        ((DBM_MOD*MAX_EV)+MINOR_START)+38
#define _DBM_SEND_PEER_DELETE_ENTRY_FAIL      ((DBM_MOD*MAX_EV)+MINOR_START)+39
#define _DBM_SEND_PEER_COMIT_ENTRY_FAIL       ((DBM_MOD*MAX_EV)+MINOR_START)+40
#define _DBM_REM_RAM_CREATE_DB_FAIL           ((DBM_MOD*MAX_EV)+MINOR_START)+41
#define _DBM_REM_RAM_CREATE_TBL_FAIL          ((DBM_MOD*MAX_EV)+MINOR_START)+42
#define _DBM_REM_RAM_CREATE_ENTRY_FAIL        ((DBM_MOD*MAX_EV)+MINOR_START)+43
#define _DBM_REM_RAM_DELETE_DB_FAIL           ((DBM_MOD*MAX_EV)+MINOR_START)+44
#define _DBM_REM_RAM_DELETE_TBL_FAIL          ((DBM_MOD*MAX_EV)+MINOR_START)+45
#define _DBM_REM_RAM_DELETE_ENTRY_FAIL        ((DBM_MOD*MAX_EV)+MINOR_START)+46
#define _DBM_REM_RAM_UPDATE_ENTRY_FAIL        ((DBM_MOD*MAX_EV)+MINOR_START)+47
#define _DBM_REM_RAM_READ_ENTRY_FAIL          ((DBM_MOD*MAX_EV)+MINOR_START)+48
#define _DBM_REM_RAM_COMIT_ENTRY_FAIL         ((DBM_MOD*MAX_EV)+MINOR_START)+49
#define _DBM_SEND_REM_RAM_CREATE_DB_FAIL      ((DBM_MOD*MAX_EV)+MINOR_START)+50
#define _DBM_SEND_REM_RAM_CREATE_TBL_FAIL     ((DBM_MOD*MAX_EV)+MINOR_START)+51
#define _DBM_SEND_REM_RAM_CREATE_ENTRY_FAIL   ((DBM_MOD*MAX_EV)+MINOR_START)+52
#define _DBM_SEND_REM_RAM_DELETE_DB_FAIL      ((DBM_MOD*MAX_EV)+MINOR_START)+53
#define _DBM_SEND_REM_RAM_DELETE_TBL_FAIL     ((DBM_MOD*MAX_EV)+MINOR_START)+54
#define _DBM_SEND_REM_RAM_DELETE_ENTRY_FAIL   ((DBM_MOD*MAX_EV)+MINOR_START)+55
#define _DBM_REM_HD_CREATE_DB_FAIL            ((DBM_MOD*MAX_EV)+MINOR_START)+56
#define _DBM_REM_HD_CREATE_TBL_FAIL           ((DBM_MOD*MAX_EV)+MINOR_START)+57
#define _DBM_REM_HD_CREATE_ENTRY_FAIL         ((DBM_MOD*MAX_EV)+MINOR_START)+58
#define _DBM_REM_HD_DELETE_DB_FAIL            ((DBM_MOD*MAX_EV)+MINOR_START)+59
#define _DBM_REM_HD_DELETE_TBL_FAIL           ((DBM_MOD*MAX_EV)+MINOR_START)+60
#define _DBM_REM_HD_DELETE_ENTRY_FAIL         ((DBM_MOD*MAX_EV)+MINOR_START)+61
#define _DBM_REM_HD_UPDATE_ENTRY_FAIL         ((DBM_MOD*MAX_EV)+MINOR_START)+62
#define _DBM_REM_HD_READ_ENTRY_FAIL           ((DBM_MOD*MAX_EV)+MINOR_START)+63
#define _DBM_SEND_REM_HD_CREATE_DB_FAIL       ((DBM_MOD*MAX_EV)+MINOR_START)+64
#define _DBM_SEND_REM_HD_CREATE_TBL_FAIL      ((DBM_MOD*MAX_EV)+MINOR_START)+65
#define _DBM_SEND_REM_HD_CREATE_ENTRY_FAIL    ((DBM_MOD*MAX_EV)+MINOR_START)+66
#define _DBM_SEND_REM_HD_DELETE_DB_FAIL       ((DBM_MOD*MAX_EV)+MINOR_START)+67
#define _DBM_SEND_REM_HD_DELETE_TBL_FAIL      ((DBM_MOD*MAX_EV)+MINOR_START)+68
#define _DBM_SEND_REM_HD_DELETE_ENTRY_FAIL    ((DBM_MOD*MAX_EV)+MINOR_START)+69
#define _DBM_PEER_HD_UPDT_AND_CMT_ENTRY_FAIL  ((DBM_MOD*MAX_EV)+MINOR_START)+70
#define _DBM_SEND_PEER_UPDT_AND_CMT_FAIL      ((DBM_MOD*MAX_EV)+MINOR_START)+71
#define _DBM_SEND_REM_RAM_UPDATE_ENTRY_FAIL   ((DBM_MOD*MAX_EV)+MINOR_START)+72
#define _DBM_SEND_REM_RAM_READ_ENTRY_FAIL     ((DBM_MOD*MAX_EV)+MINOR_START)+73
#define _DBM_SEND_REM_HD_UPDT_AND_CMT_FAIL    ((DBM_MOD*MAX_EV)+MINOR_START)+74
#define _DBM_SEND_REM_HD_READ_ENTRY_FAIL      ((DBM_MOD*MAX_EV)+MINOR_START)+75
#define _DBM_SEND_REM_RAM_VALIDATE_ENTRY_FAIL ((DBM_MOD*MAX_EV)+MINOR_START)+76
#define _DBM_SEND_REM_HD_VALIDATE_ENTRY_FAIL  ((DBM_MOD*MAX_EV)+MINOR_START)+77
#define _DBM_SYNC_INIT_TABLES_FAIL            ((DBM_MOD*MAX_EV)+MINOR_START)+78
#define _DBM_FM_PEER_WR_FAIL                  ((DBM_MOD*MAX_EV)+MINOR_START)+79
#define _DBM_FM_SRC_FILE_OPEN_FAIL            ((DBM_MOD*MAX_EV)+MINOR_START)+80
#define _DBM_FM_DST_FILE_OPEN_FAIL            ((DBM_MOD*MAX_EV)+MINOR_START)+81
#define _DBM_DBM_FM_COPY_FAIL                 ((DBM_MOD*MAX_EV)+MINOR_START)+82
#define _DBM_SYNC_FAIL_CB_COPY                ((DBM_MOD*MAX_EV)+MINOR_START)+83
#define _DBM_SYNC_FAIL_L_2_P_CARD             ((DBM_MOD*MAX_EV)+MINOR_START)+84
#define _DBM_SYNC_CLEAN_TABLES_FAIL           ((DBM_MOD*MAX_EV)+MINOR_START)+85
#define _DBM_SYNC_GET_PEER_DBM_HDR            ((DBM_MOD*MAX_EV)+MINOR_START)+86
#define _DBM_SYNC_REBUILD_TABLES_FAIL         ((DBM_MOD*MAX_EV)+MINOR_START)+87
#define _DBM_SYNC_PEER_UNBLOCK_FAIL           ((DBM_MOD*MAX_EV)+MINOR_START)+88
#define _DBM_FAIL_CREATE_HD_STATIC_DB         ((DBM_MOD*MAX_EV)+MINOR_START)+89
#define _DBM_FAIL_CREATE_RAM_STATIC_DB        ((DBM_MOD*MAX_EV)+MINOR_START)+90
#define _DBM_RCV_MSG_FAIL                     ((DBM_MOD*MAX_EV)+MINOR_START)+91
#define _DBM_CONV_NULL_ENTRY                  ((DBM_MOD*MAX_EV)+MINOR_START)+92
#define _DBM_FAIL_TO_SPAWN_CLEAN_TASK         ((DBM_MOD*MAX_EV)+MINOR_START)+93


/* NMG Minor Events */
#define _NWG_SCKBNDFAIL                       ((NWG_MOD*MAX_EV)+MINOR_START)+1

/* On-Line Diag Minor Events */
#define _OND_SCC_ALMCB_CHIP_MON_FAIL_OVER_TH         ((OND_MOD*MAX_EV)+MINOR_START)+1
#define _OND_SCC_ABMCB_CHIP_MON_FAIL_OVER_TH         ((OND_MOD*MAX_EV)+MINOR_START)+2
#define _OND_SCC_ALMSF_CHIP_MON_FAIL_OVER_TH         ((OND_MOD*MAX_EV)+MINOR_START)+3
#define _OND_SCC_ABMSF_CHIP_MON_FAIL_OVER_TH         ((OND_MOD*MAX_EV)+MINOR_START)+4
#define _OND_SCC_CUBITA_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+5
#define _OND_SCC_CUBITB_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+6
#define _OND_SCC_SAR_CHIP_MON_FAIL_OVER_TH           ((OND_MOD*MAX_EV)+MINOR_START)+7

#define _OND_DMC_M13MUX1_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+8
#define _OND_DMC_M13MUX2_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+9
#define _OND_DMC_M13MUX3_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+10
#define _OND_DMC_M13MUX4_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+11
#define _OND_DMC_M13MUX5_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+12
#define _OND_DMC_M13MUX6_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+13

#define _OND_BSC_M13MUX1_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+14
#define _OND_BSC_M13MUX2_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+15
#define _OND_BSC_BIMM13MUX3_CHIP_MON_FAIL_OVER_TH    ((OND_MOD*MAX_EV)+MINOR_START)+16
#define _OND_BSC_BIMM13MUX4_CHIP_MON_FAIL_OVER_TH    ((OND_MOD*MAX_EV)+MINOR_START)+17
#define _OND_BSC_BIMM13MUX5_CHIP_MON_FAIL_OVER_TH    ((OND_MOD*MAX_EV)+MINOR_START)+18
#define _OND_BSC_BIMM13MUX6_CHIP_MON_FAIL_OVER_TH    ((OND_MOD*MAX_EV)+MINOR_START)+19
#define _OND_BSC_BIMOCTAL_CHIP_MON_FAIL_OVER_TH      ((OND_MOD*MAX_EV)+MINOR_START)+20
#define _OND_BSC_OCTAL1_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+21
#define _OND_BSC_OCTAL2_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+22
#define _OND_BSC_OCTAL3_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+23
#define _OND_BSC_OCTAL4_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+24
#define _OND_BSC_OCTAL5_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+25
#define _OND_BSC_OCTAL6_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+26
#define _OND_BSC_OCTAL7_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+27
#define _OND_BSC_ALM_CHIP_MON_FAIL_OVER_TH           ((OND_MOD*MAX_EV)+MINOR_START)+28
#define _OND_BSC_ABM_CHIP_MON_FAIL_OVER_TH           ((OND_MOD*MAX_EV)+MINOR_START)+29
#define _OND_BSC_PCIAAL1_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+30
#define _OND_BSC_AAL1_1_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+31
#define _OND_BSC_AAL1_2_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+32
#define _OND_BSC_AAL1_3_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+33
#define _OND_BSC_AAL1_4_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+34
#define _OND_BSC_AAL1_5_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+35
#define _OND_BSC_AAL1_6_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+36
#define _OND_BSC_AAL1_7_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+37
#define _OND_BSC_BIMAAL1_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+38
#define _OND_BSC_FREEDM1_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+39
#define _OND_BSC_FREEDM2_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+40
#define _OND_BSC_SAR_CHIP_MON_FAIL_OVER_TH           ((OND_MOD*MAX_EV)+MINOR_START)+41

#define _OND_NSC_COMET1_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+42
#define _OND_NSC_COMET2_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+43
#define _OND_NSC_COMET3_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+44
#define _OND_NSC_COMET4_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+45
#define _OND_NSC_COMET5_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+46
#define _OND_NSC_COMET6_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+47
#define _OND_NSC_COMET7_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+48
#define _OND_NSC_COMET8_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+49
#define _OND_NSC_COMET9_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+50
#define _OND_NSC_COMET10_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+51
#define _OND_NSC_COMET11_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+52
#define _OND_NSC_COMET12_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+53
#define _OND_NSC_COMET13_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+54
#define _OND_NSC_COMET14_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+55
#define _OND_NSC_COMET15_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+56
#define _OND_NSC_COMET16_CHIP_MON_FAIL_OVER_TH       ((OND_MOD*MAX_EV)+MINOR_START)+57
#define _OND_NSC_MITEL1_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+58
#define _OND_NSC_MITEL2_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+59
#define _OND_NSC_MITEL3_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+60
#define _OND_NSC_AAL1_1_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+61
#define _OND_NSC_AAL1_2_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+62
#define _OND_NSC_AAL1_3_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+63
#define _OND_NSC_AAL1_4_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+64
#define _OND_NSC_CUBITA_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+65
#define _OND_NSC_CUBITB_CHIP_MON_FAIL_OVER_TH        ((OND_MOD*MAX_EV)+MINOR_START)+66
#define _OND_NSC_SAR_CHIP_MON_FAIL_OVER_TH           ((OND_MOD*MAX_EV)+MINOR_START)+67

#define _OND_SCC_MEM_TEST_FAIL_OVER_TH               ((OND_MOD*MAX_EV)+MINOR_START)+68
#define _OND_BSC_MEM_TEST_FAIL_OVER_TH               ((OND_MOD*MAX_EV)+MINOR_START)+69
#define _OND_NSC_MEM_TEST_FAIL_OVER_TH               ((OND_MOD*MAX_EV)+MINOR_START)+70

#define _OND_SCC_DATA_PATH_FAIL_OVER_TH              ((OND_MOD*MAX_EV)+MINOR_START)+71

#define _OND_TASKSPAWNFAIL                           ((OND_MOD*MAX_EV)+MINOR_START)+72
#define _OND_NOSYSMEM                                ((OND_MOD*MAX_EV)+MINOR_START)+73

#define _OND_SCCCARD_CHIP_MON_FAIL_OVER_TH           ((OND_MOD*MAX_EV)+MINOR_START)+74
#define _OND_BSCCARD_CHIP_MON_FAIL_OVER_TH           ((OND_MOD*MAX_EV)+MINOR_START)+75
#define _OND_NSCCARD_CHIP_MON_FAIL_OVER_TH           ((OND_MOD*MAX_EV)+MINOR_START)+76

/* LSP Minor Events */
#define _LSP_START_TONE_COLL_ERR                     ((LSP_MOD*MAX_EV)+MINOR_START)+1
#define _LSP_STOP_TONE_COLL_ERR                      ((LSP_MOD*MAX_EV)+MINOR_START)+2
#define _LSP_RX_DIGIT_ERR                            ((LSP_MOD*MAX_EV)+MINOR_START)+3
#define _LSP_RX_INTDIG_TMR_ERR                       ((LSP_MOD*MAX_EV)+MINOR_START)+4
#define _LSP_DC_EPFSM_ERR                            ((LSP_MOD*MAX_EV)+MINOR_START)+5
#define _LSP_MSG_ERR                                 ((LSP_MOD*MAX_EV)+MINOR_START)+6
#define _LSP_MEMALLOC_ERR                            ((LSP_MOD*MAX_EV)+MINOR_START)+7
#define _LSP_SEM_ERR                                 ((LSP_MOD*MAX_EV)+MINOR_START)+8
#define _LSP_MSGQSEND_ERR                            ((LSP_MOD*MAX_EV)+MINOR_START)+9
#define _LSP_DIGMAP_ERR                              ((LSP_MOD*MAX_EV)+MINOR_START)+10

/******************************************************************
 * Following are INFO Events System Wide 
 */
/* LAPD Q.921 Info events */
#define _LD_NOT_IMPLEM                ((LAPD_MOD*MAX_EV)+INFO_START)+1
#define _LD_ERR_PRINT                 ((LAPD_MOD*MAX_EV)+INFO_START)+2
#define _LD_RECV_ERR                  ((LAPD_MOD*MAX_EV)+INFO_START)+3
#define _LD_NULL_RECV_ERR             ((LAPD_MOD*MAX_EV)+INFO_START)+4
#define _LD_GEN_ERROR                 ((LAPD_MOD*MAX_EV)+INFO_START)+5
#define _LD_REBUILD_ERROR             ((LAPD_MOD*MAX_EV)+INFO_START)+6

/* Alarm Module Info Events */
#define _ALARM_ALM_EVT_CD_FAIL        ((ALARM_MOD*MAX_EV)+INFO_START)+1
#define _ALARM_ALM_EVT_EMM_TRAP_SENT    ((ALARM_MOD*MAX_EV)+INFO_START)+2
#define _ALARM_ALM_EVT_CLK_TRAP_SENT    ((ALARM_MOD*MAX_EV)+INFO_START)+3
#define _ALARM_ALM_EVT_SHELF_TRAP_SENT  ((ALARM_MOD*MAX_EV)+INFO_START)+4
#define _ALARM_ALM_EVT_SHELF_HIST_TRAP_SENT     ((ALARM_MOD*MAX_EV)+INFO_START)+5
#define _ALARM_ALM_EVT_CD_TRAP_SENT     ((ALARM_MOD*MAX_EV)+INFO_START)+6

/* BIM Module Info Events */
#define _BIM_ETHER_ADD              ((BIM_MOD*MAX_EV)+INFO_START)+1
#define _BIM_ETHER_DEL  ((BIM_MOD*MAX_EV)+INFO_START)+2
#define _BIM_ETHER_ACT  ((BIM_MOD*MAX_EV)+INFO_START)+3
#define _BIM_DN_FAIL    ((BIM_MOD*MAX_EV)+INFO_START)+4
#define _BIM_LINK_OK    ((BIM_MOD*MAX_EV)+INFO_START)+5
#define _BIM_TRAP_NUM_FAIL      ((BIM_MOD*MAX_EV)+INFO_START)+6
#define _BIM_TRAP_SEND_FAIL     ((BIM_MOD*MAX_EV)+INFO_START)+7
#define _BIM_TRAP_BUILD_FAIL    ((BIM_MOD*MAX_EV)+INFO_START)+8
#define _BIM_ETHER_MOD  ((BIM_MOD*MAX_EV)+INFO_START)+9

/* Card Manager  Module Info Events */
#define _CM_INFO                ((CM_MOD*MAX_EV)+INFO_START)+1
#define _CM_CM_TRAP_EVENT       ((CM_MOD*MAX_EV)+INFO_START)+2
#define _CM_INFO_POST_STATUS    ((CM_MOD*MAX_EV)+INFO_START)+3

/* Comet  Module Info Events */
#define _COMET_INVALIDLN        ((COMET_MOD*MAX_EV)+INFO_START)+1
#define _COMET_CHGLNTYPFAL      ((COMET_MOD*MAX_EV)+INFO_START)+2
#define _COMET_CHGLNCODFAL      ((COMET_MOD*MAX_EV)+INFO_START)+3
#define _COMET_SENDCODFAL       ((COMET_MOD*MAX_EV)+INFO_START)+4
#define _COMET_CHGLBCODFAL      ((COMET_MOD*MAX_EV)+INFO_START)+5
#define _COMET_CHGSIGMODFAL     ((COMET_MOD*MAX_EV)+INFO_START)+6
#define _COMET_CHGCLKSCRFAL     ((COMET_MOD*MAX_EV)+INFO_START)+7
#define _COMET_CHGFDLFAL        ((COMET_MOD*MAX_EV)+INFO_START)+8
#define _COMET_UNKWNOPTYP       ((COMET_MOD*MAX_EV)+INFO_START)+9
#define _COMET_INVLDHDLCID      ((COMET_MOD*MAX_EV)+INFO_START)+10
#define _COMET_UNKWNOPMOD       ((COMET_MOD*MAX_EV)+INFO_START)+11
#define _COMET_HDLCDISABLE      ((COMET_MOD*MAX_EV)+INFO_START)+12
#define _COMET_NOSYSMEM         ((COMET_MOD*MAX_EV)+INFO_START)+13
#define _COMET_BERTENABLE       ((COMET_MOD*MAX_EV)+INFO_START)+14
#define _COMET_BERTDISABLE      ((COMET_MOD*MAX_EV)+INFO_START)+15
#define _COMET_LNADDED          ((COMET_MOD*MAX_EV)+INFO_START)+16
#define _COMET_LNINITFAL        ((COMET_MOD*MAX_EV)+INFO_START)+17
#define _COMET_LNDELETED        ((COMET_MOD*MAX_EV)+INFO_START)+18
#define _COMET_LNDELFAIL        ((COMET_MOD*MAX_EV)+INFO_START)+19
#define _COMET_CHKRXYELFAL      ((COMET_MOD*MAX_EV)+INFO_START)+20
#define _COMET_CHKTXYELFAL      ((COMET_MOD*MAX_EV)+INFO_START)+21
#define _COMET_CHKRXREDFAL      ((COMET_MOD*MAX_EV)+INFO_START)+22
#define _COMET_CHKRXAISFAL      ((COMET_MOD*MAX_EV)+INFO_START)+23
#define _COMET_CHKTXAISFAL      ((COMET_MOD*MAX_EV)+INFO_START)+24
#define _COMET_NORXHDLC         ((COMET_MOD*MAX_EV)+INFO_START)+25
#define _COMET_NOTXHDLC         ((COMET_MOD*MAX_EV)+INFO_START)+26
#define _COMET_SETUPPATFAL      ((COMET_MOD*MAX_EV)+INFO_START)+27
#define _COMET_SETUPERFAL       ((COMET_MOD*MAX_EV)+INFO_START)+28
#define _COMET_ADDLINE          ((COMET_MOD*MAX_EV)+INFO_START)+29
#define _COMET_DELLINE          ((COMET_MOD*MAX_EV)+INFO_START)+30
#define _COMET_MODLINE          ((COMET_MOD*MAX_EV)+INFO_START)+31
#define _COMET_CLRALARM         ((COMET_MOD*MAX_EV)+INFO_START)+32
#define _COMET_EVENTTEST        ((COMET_MOD*MAX_EV)+INFO_START)+33
#define _COMET_LINEDEL          ((COMET_MOD*MAX_EV)+INFO_START)+34
#define _COMET_PERFCLRALARM     ((COMET_MOD*MAX_EV)+INFO_START)+35
#define _COMET_MODALARMSETTING   ((COMET_MOD*MAX_EV)+INFO_START)+36
#define _COMET_PAYLOADLOOPUP     ((COMET_MOD*MAX_EV)+INFO_START)+37
#define _COMET_LINELOOPUP        ((COMET_MOD*MAX_EV)+INFO_START)+38
#define _COMET_OTHERLOOPUP       ((COMET_MOD*MAX_EV)+INFO_START)+39
#define _COMET_LOOPDOWN          ((COMET_MOD*MAX_EV)+INFO_START)+40
#define _COMET_BERTSTART         ((COMET_MOD*MAX_EV)+INFO_START)+41
#define _COMET_BERTSTOP          ((COMET_MOD*MAX_EV)+INFO_START)+42
#define _COMET_MODECONFLICT      ((COMET_MOD*MAX_EV)+INFO_START)+43
#define _COMET_SENDBULKTRAP      ((COMET_MOD*MAX_EV)+INFO_START)+44
#define _COMET_INVALIDLNINCB      ((COMET_MOD*MAX_EV)+INFO_START)+45
#define _COMET_CONFIGTS16FAL      ((COMET_MOD*MAX_EV)+INFO_START)+46
#define _COMET_CONFIGFRMCRCFAL      ((COMET_MOD*MAX_EV)+INFO_START)+47
#define _COMET_INVALIDBITMAP     ((COMET_MOD*MAX_EV)+INFO_START)+48
#define _COMET_TRAPEVENTLOGFAL   ((COMET_MOD*MAX_EV)+INFO_START)+49
#define _COMET_INVALIDBITPOS   ((COMET_MOD*MAX_EV)+INFO_START)+50
#define _COMET_INVALIDDEVID   ((COMET_MOD*MAX_EV)+INFO_START)+51


/* Comet  Module Info Events */
#define _BDS1_INVALIDLN        ((BDS1_MOD*MAX_EV)+INFO_START)+1
#define _BDS1_CHGLNTYPFAIL     ((BDS1_MOD*MAX_EV)+INFO_START)+2
#define _BDS1_CHGLNCODFAIL     ((BDS1_MOD*MAX_EV)+INFO_START)+3
#define _BDS1_SENDCODFAIL      ((BDS1_MOD*MAX_EV)+INFO_START)+4
#define _BDS1_CHGLBCODFAIL     ((BDS1_MOD*MAX_EV)+INFO_START)+5
#define _BDS1_CHGSIGMODFAIL    ((BDS1_MOD*MAX_EV)+INFO_START)+6
#define _BDS1_CHGCLKSCRFAIL    ((BDS1_MOD*MAX_EV)+INFO_START)+7
#define _BDS1_CHGFDLFAIL       ((BDS1_MOD*MAX_EV)+INFO_START)+8
#define _BDS1_UNKWNOPTYP       ((BDS1_MOD*MAX_EV)+INFO_START)+9
#define _BDS1_INVLDHDLCID      ((BDS1_MOD*MAX_EV)+INFO_START)+10
#define _BDS1_UNKWNOPMOD       ((BDS1_MOD*MAX_EV)+INFO_START)+11
#define _BDS1_HDLCDISABLE      ((BDS1_MOD*MAX_EV)+INFO_START)+12
#define _BDS1_NOSYSMEM         ((BDS1_MOD*MAX_EV)+INFO_START)+13
#define _BDS1_BERTENABLE       ((BDS1_MOD*MAX_EV)+INFO_START)+14
#define _BDS1_BERTDISABLE      ((BDS1_MOD*MAX_EV)+INFO_START)+15
#define _BDS1_LNADDED          ((BDS1_MOD*MAX_EV)+INFO_START)+16
#define _BDS1_LNINITFAIL       ((BDS1_MOD*MAX_EV)+INFO_START)+17
#define _BDS1_LNDELETED        ((BDS1_MOD*MAX_EV)+INFO_START)+18
#define _BDS1_LNDELFAIL        ((BDS1_MOD*MAX_EV)+INFO_START)+19
#define _BDS1_CHKRXYELFAIL     ((BDS1_MOD*MAX_EV)+INFO_START)+20
#define _BDS1_CHKTXYELFAIL     ((BDS1_MOD*MAX_EV)+INFO_START)+21
#define _BDS1_CHKRXREDFAIL     ((BDS1_MOD*MAX_EV)+INFO_START)+22
#define _BDS1_CHKRXAISFAIL     ((BDS1_MOD*MAX_EV)+INFO_START)+23
#define _BDS1_CHKTXAISFAIL     ((BDS1_MOD*MAX_EV)+INFO_START)+24
#define _BDS1_NORXHDLC         ((BDS1_MOD*MAX_EV)+INFO_START)+25
#define _BDS1_NOTXHDLC         ((BDS1_MOD*MAX_EV)+INFO_START)+26
#define _BDS1_SETUPPATFAIL     ((BDS1_MOD*MAX_EV)+INFO_START)+27
#define _BDS1_SETUPERFAIL      ((BDS1_MOD*MAX_EV)+INFO_START)+28
#define _BDS1_ADDLINE          ((BDS1_MOD*MAX_EV)+INFO_START)+29
#define _BDS1_DELLINE          ((BDS1_MOD*MAX_EV)+INFO_START)+30
#define _BDS1_MODLINE          ((BDS1_MOD*MAX_EV)+INFO_START)+31
#define _BDS1_CLRALARM         ((BDS1_MOD*MAX_EV)+INFO_START)+32
#define _BDS1_EVENTTEST        ((BDS1_MOD*MAX_EV)+INFO_START)+33
#define _BDS1_LINEDEL          ((BDS1_MOD*MAX_EV)+INFO_START)+34
#define _BDS1_PERFCLRALARM     ((BDS1_MOD*MAX_EV)+INFO_START)+35
#define _BDS1_MODALARMSETTING   ((BDS1_MOD*MAX_EV)+INFO_START)+36
#define _BDS1_PAYLOADLOOPUP     ((BDS1_MOD*MAX_EV)+INFO_START)+37
#define _BDS1_LINELOOPUP        ((BDS1_MOD*MAX_EV)+INFO_START)+38
#define _BDS1_OTHERLOOPUP       ((BDS1_MOD*MAX_EV)+INFO_START)+39
#define _BDS1_LOOPDOWN          ((BDS1_MOD*MAX_EV)+INFO_START)+40
#define _BDS1_BERTSTART         ((BDS1_MOD*MAX_EV)+INFO_START)+41
#define _BDS1_BERTSTOP          ((BDS1_MOD*MAX_EV)+INFO_START)+42
#define _BDS1_MODECONFLICT      ((BDS1_MOD*MAX_EV)+INFO_START)+43
#define _BDS1_SENDBULKTRAP      ((BDS1_MOD*MAX_EV)+INFO_START)+44
#define _BDS1_INVALIDBITMAP     ((BDS1_MOD*MAX_EV)+INFO_START)+45
#define _BDS1_TRAPEVENTLOGFAL   ((BDS1_MOD*MAX_EV)+INFO_START)+46


/* Core Redundancy Info Events */
#define _CRM_INFO                     ((CRM_MOD*MAX_EV)+INFO_START)+1
#define _CRM_CRM_TRAP_EVENT           ((CRM_MOD*MAX_EV)+INFO_START)+2
#define _CRM_SCC_SWITCHOVER_START     ((CRM_MOD*MAX_EV)+INFO_START)+3
#define _CRM_SCC_SWITCHOVER_COMP      ((CRM_MOD*MAX_EV)+INFO_START)+4
#define _CRM_SCC_ON_LINE_DIAG_FAIL    ((CRM_MOD*MAX_EV)+INFO_START)+5
#define _CRM_PEER_WIN_NEGOTIATION     ((CRM_MOD*MAX_EV)+INFO_START)+6

/* Data Base Manager Info Events */
#define _DBM_INFO               ((DBM_MOD*MAX_EV)+INFO_START)+1

/* DMC Info Events */
#define _DMC_DMCADDMAP          ((DMC_MOD*MAX_EV)+INFO_START)+1
#define _DMC_DMCMODMAP  ((DMC_MOD*MAX_EV)+INFO_START)+2
#define _DMC_DMCMAJALARM        ((DMC_MOD*MAX_EV)+INFO_START)+3
#define _DMC_DMCDELMAP  ((DMC_MOD*MAX_EV)+INFO_START)+4

/* DSX Info Events */
#define _DSX_DSXADDLINE         ((DSX_MOD*MAX_EV)+INFO_START)+1
#define _DSX_DSXMODLINE         ((DSX_MOD*MAX_EV)+INFO_START)+2
#define _DSX_DSXCLRALARM        ((DSX_MOD*MAX_EV)+INFO_START)+3
#define _DSX_DSXPERFCLRALARM    ((DSX_MOD*MAX_EV)+INFO_START)+4
#define _DSX_DSXDELLINE         ((DSX_MOD*MAX_EV)+INFO_START)+5
#define _DSX_DSXMODALARMSETTING   ((DSX_MOD*MAX_EV)+INFO_START)+6
#define _DSX_DSXPAYLOADLOOPUP     ((DSX_MOD*MAX_EV)+INFO_START)+7
#define _DSX_DSXLINELOOPUP        ((DSX_MOD*MAX_EV)+INFO_START)+8
#define _DSX_DSXOTHERLOOPUP       ((DSX_MOD*MAX_EV)+INFO_START)+9
#define _DSX_DSXLOOPDOWN          ((DSX_MOD*MAX_EV)+INFO_START)+10



/* SONET Info Events */
#define _SONET_SNTADDLINE      ((SONET_MOD*MAX_EV)+INFO_START)+1
#define _SONET_SNTMODLINE      ((SONET_MOD*MAX_EV)+INFO_START)+2
#define _SONET_SNTCLRALARM     ((SONET_MOD*MAX_EV)+INFO_START)+3
#define _SONET_SNTPERFCLRALARM ((SONET_MOD*MAX_EV)+INFO_START)+4
#define _SONET_SNTDELLINE      ((SONET_MOD*MAX_EV)+INFO_START)+5
#define _SONET_SNTMODALARMSETTING   ((SONET_MOD*MAX_EV)+INFO_START)+6
#define _SONET_SNTLINELOOPUP        ((SONET_MOD*MAX_EV)+INFO_START)+7
#define _SONET_SNTSERIALLOOPUP      ((SONET_MOD*MAX_EV)+INFO_START)+8
#define _SONET_SNTPARALLELLOOPUP    ((SONET_MOD*MAX_EV)+INFO_START)+9
#define _SONET_SNTLOOPDOWN          ((SONET_MOD*MAX_EV)+INFO_START)+10

/* Environment Monitoring Module  Info Events */
#define _EMM_EMMCLRTRAP         ((EMM_MOD*MAX_EV)+INFO_START)+1

/* File Manager Module  Info Events */
#define _FM_INFO                ((FM_MOD*MAX_EV)+INFO_START)+1
#define _FM_START_REBUILD_MSG   ((FM_MOD*MAX_EV)+INFO_START)+2
#define _FM_FINISH_REBUILD_MSG  ((FM_MOD*MAX_EV)+INFO_START)+3

/* Image Downloader Module  Info Events */
#define _ID_INFO                ((ID_MOD*MAX_EV)+INFO_START)+1

/* MRPC Module  Info Events */
#define _MRPC_IN_MSG            ((MRPC_MOD*MAX_EV)+INFO_START)+1
#define _MRPC_FREE_STALE_TXN    ((MRPC_MOD*MAX_EV)+INFO_START)+2

/* RMG Module  Info Events */
#define _RMG_RM_USER_EVT_CD_ADD         ((RMG_MOD*MAX_EV)+INFO_START)+1
#define _RMG_RM_USER_EVT_CD_CFG_DEL     ((RMG_MOD*MAX_EV)+INFO_START)+2
#define _RMG_RM_USER_EVT_LINE_ADD       ((RMG_MOD*MAX_EV)+INFO_START)+3
#define _RMG_RM_USER_EVT_LINE_DELETE    ((RMG_MOD*MAX_EV)+INFO_START)+4
#define _RMG_RM_USER_EVT_LINE_MODIFY    ((RMG_MOD*MAX_EV)+INFO_START)+5
#define _RMG_RM_USER_EVT_PORT_ADD       ((RMG_MOD*MAX_EV)+INFO_START)+6
#define _RMG_RM_USER_EVT_PORT_DELETE    ((RMG_MOD*MAX_EV)+INFO_START)+7
#define _RMG_RM_USER_EVT_PORT_MODIFY    ((RMG_MOD*MAX_EV)+INFO_START)+8
#define _RMG_RM_USER_EVT_EP_ADD ((RMG_MOD*MAX_EV)+INFO_START)+9
#define _RMG_RM_USER_EVT_EP_DELETE      ((RMG_MOD*MAX_EV)+INFO_START)+10
#define _RMG_RM_USER_EVT_EP_MODIFY      ((RMG_MOD*MAX_EV)+INFO_START)+11
#define _RMG_RM_USER_EVT_CONN_ADD       ((RMG_MOD*MAX_EV)+INFO_START)+12
#define _RMG_RM_USER_EVT_CONN_DELETE    ((RMG_MOD*MAX_EV)+INFO_START)+13
#define _RMG_RM_USER_EVT_CONN_MODIFY    ((RMG_MOD*MAX_EV)+INFO_START)+14
#define _RMG_RM_USER_EVT_CONNPROF_SET   ((RMG_MOD*MAX_EV)+INFO_START)+15
#define _RMG_RM_USER_EVT_CALL_ADD       ((RMG_MOD*MAX_EV)+INFO_START)+16
#define _RMG_RM_USER_EVT_CALL_DELETE    ((RMG_MOD*MAX_EV)+INFO_START)+17
#define _RMG_RM_USER_EVT_CALL_ADD_SVC   ((RMG_MOD*MAX_EV)+INFO_START)+18
#define _RMG_RM_USER_EVT_CALL_DELETE_SVC        ((RMG_MOD*MAX_EV)+INFO_START)+19
#define _RMG_RM_USER_EVT_CALL_ADD_ASYNC ((RMG_MOD*MAX_EV)+INFO_START)+20
#define _RMG_RM_USER_EVT_CALL_DELETE_ASYNC      ((RMG_MOD*MAX_EV)+INFO_START)+21
#define _RMG_RM_USER_EVT_LEG_ADD        ((RMG_MOD*MAX_EV)+INFO_START)+22
#define _RMG_RM_USER_EVT_LEG_DELETE     ((RMG_MOD*MAX_EV)+INFO_START)+23
#define _RMG_RM_EVT_EP_ALLOCATED        ((RMG_MOD*MAX_EV)+INFO_START)+24
#define _RMG_RM_EVT_EP_FREED    ((RMG_MOD*MAX_EV)+INFO_START)+25
#define _RMG_RM_EVT_EP_ADDED    ((RMG_MOD*MAX_EV)+INFO_START)+26
#define _RMG_RM_EVT_EP_DELETED  ((RMG_MOD*MAX_EV)+INFO_START)+27
#define _RMG_RM_EVT_CONN_ALLOCATED      ((RMG_MOD*MAX_EV)+INFO_START)+28
#define _RMG_RM_EVT_CONN_FREED  ((RMG_MOD*MAX_EV)+INFO_START)+29
#define _RMG_RM_EVT_CONN_ADDED  ((RMG_MOD*MAX_EV)+INFO_START)+30
#define _RMG_RM_EVT_CONN_DELETED        ((RMG_MOD*MAX_EV)+INFO_START)+31
#define _RMG_RM_EVT_CONN_MODIFIED       ((RMG_MOD*MAX_EV)+INFO_START)+32
#define _RMG_RM_EVT_FR_EP_ADDED ((RMG_MOD*MAX_EV)+INFO_START)+33
#define _RMG_RM_EVT_FR_EP_DELETED       ((RMG_MOD*MAX_EV)+INFO_START)+34
#define _RMG_RM_EVT_FR_EP_MODIFIED      ((RMG_MOD*MAX_EV)+INFO_START)+35
#define _RMG_RM_EVT_CES_EP_ADDED        ((RMG_MOD*MAX_EV)+INFO_START)+36
#define _RMG_RM_EVT_CES_EP_DELETED      ((RMG_MOD*MAX_EV)+INFO_START)+37
#define _RMG_RM_EVT_CES_EP_MODIFIED     ((RMG_MOD*MAX_EV)+INFO_START)+38
#define _RMG_RM_EVT_VOICE_EP_ADDED      ((RMG_MOD*MAX_EV)+INFO_START)+39
#define _RMG_RM_EVT_VOICE_EP_DELETED    ((RMG_MOD*MAX_EV)+INFO_START)+40
#define _RMG_RM_EVT_VOICE_EP_MODIFIED   ((RMG_MOD*MAX_EV)+INFO_START)+41
#define _RMG_RM_EVT_ATM_EP_ADDED        ((RMG_MOD*MAX_EV)+INFO_START)+42
#define _RMG_RM_EVT_ATM_EP_DELETED      ((RMG_MOD*MAX_EV)+INFO_START)+43
#define _RMG_RM_EVT_ATM_EP_MODIFIED     ((RMG_MOD*MAX_EV)+INFO_START)+44
#define _RMG_RM_EVT_IP_EP_ADDED ((RMG_MOD*MAX_EV)+INFO_START)+45
#define _RMG_RM_EVT_IP_EP_DELETED       ((RMG_MOD*MAX_EV)+INFO_START)+46
#define _RMG_RM_EVT_IP_EP_MODIFIED      ((RMG_MOD*MAX_EV)+INFO_START)+47
#define _RMG_RM_EVT_LEG_ADDED   ((RMG_MOD*MAX_EV)+INFO_START)+48
#define _RMG_RM_EVT_LEG_DELETED ((RMG_MOD*MAX_EV)+INFO_START)+49
#define _RMG_RM_EVT_X_ADDED     ((RMG_MOD*MAX_EV)+INFO_START)+50
#define _RMG_RM_EVT_X_DELETED   ((RMG_MOD*MAX_EV)+INFO_START)+51
#define _RMG_RM_EVT_X_MODIFIED  ((RMG_MOD*MAX_EV)+INFO_START)+52
#define _RMG_RM_EVT_BSC_ATM_PROGED      ((RMG_MOD*MAX_EV)+INFO_START)+53
#define _RMG_RM_EVT_BSC_ATM_DEPROGED    ((RMG_MOD*MAX_EV)+INFO_START)+54
#define _RMG_RM_EVT_BSC_ATM_MODPROGED   ((RMG_MOD*MAX_EV)+INFO_START)+55
#define _RMG_RM_EVT_BSC_IP_PROGED       ((RMG_MOD*MAX_EV)+INFO_START)+56
#define _RMG_RM_EVT_BSC_IP_DEPROGED     ((RMG_MOD*MAX_EV)+INFO_START)+57
#define _RMG_RM_EVT_BSC_IP_MODPROGED    ((RMG_MOD*MAX_EV)+INFO_START)+58
#define _RMG_RM_EVT_SVC_HASH_ADDED      ((RMG_MOD*MAX_EV)+INFO_START)+59
#define _RMG_RM_EVT_SVC_HASH_DELETED    ((RMG_MOD*MAX_EV)+INFO_START)+60
#define _RMG_RM_EVT_X_IGR_ADD   ((RMG_MOD*MAX_EV)+INFO_START)+61
#define _RMG_RM_EVT_X_IGR_DEL   ((RMG_MOD*MAX_EV)+INFO_START)+62
#define _RMG_RM_EVT_X_IGR_MOD   ((RMG_MOD*MAX_EV)+INFO_START)+63
#define _RMG_RM_EVT_X_EGR_ADD   ((RMG_MOD*MAX_EV)+INFO_START)+64
#define _RMG_RM_EVT_X_EGR_DEL   ((RMG_MOD*MAX_EV)+INFO_START)+65
#define _RMG_RM_EVT_X_EGR_MOD   ((RMG_MOD*MAX_EV)+INFO_START)+66
#define _RMG_RM_EVT_ETHER_CNTL_ADD      ((RMG_MOD*MAX_EV)+INFO_START)+67
#define _RMG_RM_EVT_ETHER_CNTL_DEL      ((RMG_MOD*MAX_EV)+INFO_START)+68
#define _RMG_RM_USER_EVT_CD_DELETE      ((RMG_MOD*MAX_EV)+INFO_START)+69

#define _RMG_RM_USER_EVT_CALL_MODIFY        ((RMG_MOD*MAX_EV)+INFO_START)+70
#define _RMG_RM_USER_EVT_CALL_MODIFY_SVC    ((RMG_MOD*MAX_EV)+INFO_START)+71
#define _RMG_RM_USER_EVT_CALL_MODIFY_ASYNC  ((RMG_MOD*MAX_EV)+INFO_START)+72
#define _RMG_RM_USER_EVT_LEG_MODIFY         ((RMG_MOD*MAX_EV)+INFO_START)+73
#define _RMG_RM_EVT_CELLBUS_BW_FULL    ((RMG_MOD*MAX_EV)+INFO_START)+74

/* Shelf Manager Info Events */
#define _SM_INFO                ((SM_MOD*MAX_EV)+INFO_START)+1
#define _SM_SM_TRAP_EVENT       ((SM_MOD*MAX_EV)+INFO_START)+2
#define _SM_SM_SWITCHOVER_START ((SM_MOD*MAX_EV)+INFO_START)+3
#define _SM_SM_SWITCHOVER_COMP  ((SM_MOD*MAX_EV)+INFO_START)+4
#define _SM_SC_POLL_FAILED      ((SM_MOD*MAX_EV)+INFO_START)+5
#define _SM_SC_POLL_RESET_CARD  ((SM_MOD*MAX_EV)+INFO_START)+6
#define _SM_SPURIOUS_POLL       ((SM_MOD*MAX_EV)+INFO_START)+7
#define _SM_ON_LINE_DIAG_FAIL   ((SM_MOD*MAX_EV)+INFO_START)+8

/* SYS Info Events */
#define _SYS_STARTUP            ((SYS_MOD*MAX_EV)+INFO_START)+1
#define _SYS_BOOTFAIL   ((SYS_MOD*MAX_EV)+INFO_START)+2
#define _SYS_BGNDTASKRESTART    ((SYS_MOD*MAX_EV)+INFO_START)+3
#define _SYS_RECVTASK   ((SYS_MOD*MAX_EV)+INFO_START)+4
#define _SYS_EVENTTEST  ((SYS_MOD*MAX_EV)+INFO_START)+5
#define _SYS_VERSION    ((SYS_MOD*MAX_EV)+INFO_START)+6
#define _SYS_RSTCBT             ((SYS_MOD*MAX_EV)+INFO_START)+7

/* TRM Info Events */
#define _TRM_TRAP_NUM_FAIL      ((TRM_MOD*MAX_EV)+INFO_START)+1
#define _TRM_TRAP_BUILD_FAIL    ((TRM_MOD*MAX_EV)+INFO_START)+2
#define _TRM_TRAP_SEND_FAIL     ((TRM_MOD*MAX_EV)+INFO_START)+3

/* MSCP Info Events */
#define _MSCP_TMR_CREATE        ((MSCP_MOD*MAX_EV)+INFO_START)+1
#define _MSCP_TMR_FUNC_ADD              ((MSCP_MOD*MAX_EV)+INFO_START)+2
#define _MSCP_HEALTH_CHECK      ((MSCP_MOD*MAX_EV)+INFO_START)+3
#define _MSCP_CALL_REJECTED     ((MSCP_MOD*MAX_EV)+INFO_START)+4
#define _MSCP_RCV_INVSOCK       ((MSCP_MOD*MAX_EV)+INFO_START)+5
#define _MSCP_MULTI_OCCUR       ((MSCP_MOD*MAX_EV)+INFO_START)+6
#define _MSCP_VEND_SKIP ((MSCP_MOD*MAX_EV)+INFO_START)+7
#define _MSCP_ENC_ERROR ((MSCP_MOD*MAX_EV)+INFO_START)+8
#define _MSCP_DEC_ERROR ((MSCP_MOD*MAX_EV)+INFO_START)+9
#define _MSCP_SOCK_PKT_RCV_BIG  ((MSCP_MOD*MAX_EV)+INFO_START)+10
#define _MSCP_SOCK_PKT_RCV_SMALL        ((MSCP_MOD*MAX_EV)+INFO_START)+11
#define _MSCP_ESC_FUNC  ((MSCP_MOD*MAX_EV)+INFO_START)+12
#define _MSCP_TSKQ_MSG_LEN      ((MSCP_MOD*MAX_EV)+INFO_START)+13
#define _MSCP_TSKQ_MSG_TYPE     ((MSCP_MOD*MAX_EV)+INFO_START)+14
#define _MSCP_NO_ROUTE_MSG_TYPE ((MSCP_MOD*MAX_EV)+INFO_START)+15
#define _MSCP_PDU_DEC   ((MSCP_MOD*MAX_EV)+INFO_START)+16
#define _MSCP_MSG_ENC   ((MSCP_MOD*MAX_EV)+INFO_START)+17
#define _MSCP_NO_ROUTE_TXN_ID   ((MSCP_MOD*MAX_EV)+INFO_START)+18
#define _MSCP_NO_ROUTE_CORR_ID  ((MSCP_MOD*MAX_EV)+INFO_START)+19
#define _MSCP_CON_SUCCESS       ((MSCP_MOD*MAX_EV)+INFO_START)+20
#define _MSCP_ORIG_ID   ((MSCP_MOD*MAX_EV)+INFO_START)+21
#define _MSCP_GENERR            ((MSCP_MOD*MAX_EV)+INFO_START)+22
#define _MSCP_SOCKERR           ((MSCP_MOD*MAX_EV)+INFO_START)+23
#define _MSCP_UNSOLICITED_ASUP  ((MSCP_MOD*MAX_EV)+INFO_START)+24

/* MPC Info Events */
#define _MPC_FLOWCTRL_EVENT     ((MPC_MOD*MAX_EV)+INFO_START)+1
#define _MPC_RTM_EVENT          ((MPC_MOD*MAX_EV)+INFO_START)+2
#define _MPC_FSMRTE_EVENT       ((MPC_MOD*MAX_EV)+INFO_START)+3
#define _MPC_PORTTRANS_EVENT    ((MPC_MOD*MAX_EV)+INFO_START)+4
#define _MPC_MGCPERR_EVENT      ((MPC_MOD*MAX_EV)+INFO_START)+5
#define _MPC_DELAY_EVENT        ((MPC_MOD*MAX_EV)+INFO_START)+6
#define _MPC_VCI_EVENT          ((MPC_MOD*MAX_EV)+INFO_START)+7
#define _MPC_DBG_EVENT1         ((MPC_MOD*MAX_EV)+INFO_START)+8
#define _MPC_DBG_EVENT2         ((MPC_MOD*MAX_EV)+INFO_START)+9
#define _MPC_DBG_EVENT3         ((MPC_MOD*MAX_EV)+INFO_START)+10
#define _MPC_DBG_EVENT4         ((MPC_MOD*MAX_EV)+INFO_START)+11
#define _MPC_DBG_EVENT5         ((MPC_MOD*MAX_EV)+INFO_START)+12
#define _MPC_DBG_EVENT6         ((MPC_MOD*MAX_EV)+INFO_START)+13
#define _MPC_DS0_XCONN_ADD      ((MPC_MOD*MAX_EV)+INFO_START)+14
#define _MPC_DS0_XCONN_DEL      ((MPC_MOD*MAX_EV)+INFO_START)+15
#define _MPC_DS0_XCONN_ALRM_CLR         ((MPC_MOD*MAX_EV)+INFO_START)+16
#define _MPC_DS0_XCONN_ALRM_CLR_BULK    ((MPC_MOD*MAX_EV)+INFO_START)+17
#define _MPC_DS0_XCONN_ALRM_CLR_OP_FAIL ((MPC_MOD*MAX_EV)+INFO_START)+18

/* PCC Info Events */
#define _PCC_FLOWCTRLADD_EVENT  ((PCC_MOD*MAX_EV)+INFO_START)+1
#define _PCC_FLOWCTRLDEL_EVENT  ((PCC_MOD*MAX_EV)+INFO_START)+2
#define _PCC_RTM_EVENT          ((PCC_MOD*MAX_EV)+INFO_START)+3
#define _PCC_FSMRTE_EVENT       ((PCC_MOD*MAX_EV)+INFO_START)+4
#define _PCC_PORTTRANS_EVENT    ((PCC_MOD*MAX_EV)+INFO_START)+5
#define _PCC_TCALLADD_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+6
#define _PCC_ICALLADD_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+7
#define _PCC_TCALLDEL_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+8
#define _PCC_ICALLDEL_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+9
#define _PCC_IADDFAIL_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+10
#define _PCC_TADDFAIL_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+11
#define _PCC_TDELFAIL_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+12
#define _PCC_IDELFAIL_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+13
#define _PCC_DELFAIL_EVENT      ((PCC_MOD*MAX_EV)+INFO_START)+14
#define _PCC_ICALLMOD_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+15
#define _PCC_IMODFAIL_EVENT     ((PCC_MOD*MAX_EV)+INFO_START)+16
#define _PCC_TCALLFAIL_EVENT    ((PCC_MOD*MAX_EV)+INFO_START)+17
#define _PCC_ICALLFAIL_EVENT    ((PCC_MOD*MAX_EV)+INFO_START)+18

/* PLFMLIB Info Events */
#define _PLFMLIB_MEM_EVENT      ((PLFM_MOD*MAX_EV)+INFO_START)+1
#define _PLFMLIB_DBM_EVENT      ((PLFM_MOD*MAX_EV)+INFO_START)+2
#define _PLFMLIB_HASH_EVENT     ((PLFM_MOD*MAX_EV)+INFO_START)+3
#define _PLFMLIB_ILLEGAL_EVENT  ((PLFM_MOD*MAX_EV)+INFO_START)+4

/* PRC Info Events */
#define _PRC_FLOWCTRLADD_EVENT  ((PRC_MOD*MAX_EV)+INFO_START)+1
#define _PRC_FLOWCTRLDEL_EVENT  ((PRC_MOD*MAX_EV)+INFO_START)+2
#define _PRC_DBM_EVENT          ((PRC_MOD*MAX_EV)+INFO_START)+3
#define _PRC_PORTTRANS_EVENT    ((PRC_MOD*MAX_EV)+INFO_START)+4
#define _PRC_DUPSCS_EVENT       ((PRC_MOD*MAX_EV)+INFO_START)+5
#define _PRC_DUPCOT_EVENT       ((PRC_MOD*MAX_EV)+INFO_START)+6
#define _PRC_STNADD_EVENT       ((PRC_MOD*MAX_EV)+INFO_START)+7
#define _PRC_STNDEL_EVENT       ((PRC_MOD*MAX_EV)+INFO_START)+8
#define _PRC_STNFAIL_EVENT      ((PRC_MOD*MAX_EV)+INFO_START)+9


/* RTM Info Events */
#define _RTM_RTM_INFO           ((RTM_MOD*MAX_EV)+INFO_START)+1
#define _RTM_RTM_DEBUG          ((RTM_MOD*MAX_EV)+INFO_START)+2

/* SIW Info Events */
#define _SIW_NOSARBUF           ((SIW_MOD*MAX_EV)+INFO_START)+1
#define _SIW_DYNMEMTHRESH       ((SIW_MOD*MAX_EV)+INFO_START)+2

/* NWG Info Events */
#define _NWG_FOPENFAIL          ((NWG_MOD*MAX_EV)+INFO_START)+1
#define _NWG_FDWNLOAD           ((NWG_MOD*MAX_EV)+INFO_START)+2
#define _NWG_CFGBKUP            ((NWG_MOD*MAX_EV)+INFO_START)+3
#define _NWG_CFGRST             ((NWG_MOD*MAX_EV)+INFO_START)+4
#define _NWG_UPFLS              ((NWG_MOD*MAX_EV)+INFO_START)+5
#define _NWG_UPFLSFAIL          ((NWG_MOD*MAX_EV)+INFO_START)+6

/* TCG Info Events */
#define _TCG_INVALIDLN          ((TCG_MOD*MAX_EV)+INFO_START)+1
#define _TCG_PORTDEL            ((TCG_MOD*MAX_EV)+INFO_START)+2
#define _TCG_INSERTHASHFAIL     ((TCG_MOD*MAX_EV)+INFO_START)+3
#define _TCG_REMHASHFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+4
#define _TCG_LKUPHASHFAIL       ((TCG_MOD*MAX_EV)+INFO_START)+5
#define _TCG_HASHUPDATEFAIL     ((TCG_MOD*MAX_EV)+INFO_START)+6
#define _TCG_DBCREATFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+7
#define _TCG_TBLCREATFAIL       ((TCG_MOD*MAX_EV)+INFO_START)+8
#define _TCG_DBWRITEFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+9
#define _TCG_DBREADFAIL         ((TCG_MOD*MAX_EV)+INFO_START)+10
#define _TCG_FSMEVENTFAIL       ((TCG_MOD*MAX_EV)+INFO_START)+11
#define _TCG_ADDCHANFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+12
#define _TCG_DELCHANFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+13
#define _TCG_DELCONNFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+14
#define _TCG_OPREQFAIL          ((TCG_MOD*MAX_EV)+INFO_START)+15
#define _TCG_ANNCFAIL           ((TCG_MOD*MAX_EV)+INFO_START)+16
#define _TCG_SETRMTFAIL         ((TCG_MOD*MAX_EV)+INFO_START)+17
#define _TCG_INVLDEVENT         ((TCG_MOD*MAX_EV)+INFO_START)+18
#define _TCG_SETDEFFAIL         ((TCG_MOD*MAX_EV)+INFO_START)+19
#define _TCG_OPREQHASHFAIL      ((TCG_MOD*MAX_EV)+INFO_START)+20
#define _TCG_SLPBKFAIL          ((TCG_MOD*MAX_EV)+INFO_START)+21
#define _TCG_ELPBKFAIL          ((TCG_MOD*MAX_EV)+INFO_START)+22
#define _TCG_TESTFAIL           ((TCG_MOD*MAX_EV)+INFO_START)+23
#define _TCG_RSTFAIL            ((TCG_MOD*MAX_EV)+INFO_START)+24
#define _TCG_OUTOFSRVFAIL       ((TCG_MOD*MAX_EV)+INFO_START)+25
#define _TCG_BLOCKFAIL          ((TCG_MOD*MAX_EV)+INFO_START)+26
#define _TCG_UNBLOCKFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+27
#define _TCG_ALLOCFAIL          ((TCG_MOD*MAX_EV)+INFO_START)+28
#define _TCG_PORTADDED          ((TCG_MOD*MAX_EV)+INFO_START)+29
#define _TCG_PORTDELETED        ((TCG_MOD*MAX_EV)+INFO_START)+30
#define _TCG_PORTMODIFY         ((TCG_MOD*MAX_EV)+INFO_START)+31
#define _TCG_PORTCFGGET         ((TCG_MOD*MAX_EV)+INFO_START)+32
#define _TCG_NSCAAL1ADDERR      ((TCG_MOD*MAX_EV)+INFO_START)+33
#define _TCG_NSCAAL1DELERR      ((TCG_MOD*MAX_EV)+INFO_START)+34
#define _TCG_C5RESET            ((TCG_MOD*MAX_EV)+INFO_START)+35
#define _TCG_C6FAIL             ((TCG_MOD*MAX_EV)+INFO_START)+36
#define _TCG_DELPORTFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+37
#define _TCG_MODCHANFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+38
#define _TCG_MITELADDFAIL       ((TCG_MOD*MAX_EV)+INFO_START)+39
#define _TCG_MITELDELFAIL       ((TCG_MOD*MAX_EV)+INFO_START)+40
#define _TCG_SIGNALADDFAIL      ((TCG_MOD*MAX_EV)+INFO_START)+41
#define _TCG_SIGNALDELFAIL      ((TCG_MOD*MAX_EV)+INFO_START)+42
#define _TCG_DSPCHANADDFAIL     ((TCG_MOD*MAX_EV)+INFO_START)+43
#define _TCG_DSPCHANDELFAIL     ((TCG_MOD*MAX_EV)+INFO_START)+44
#define _TCG_DSESENTFAIL        ((TCG_MOD*MAX_EV)+INFO_START)+45
#define _TCG_BSCINVLDEVENT      ((TCG_MOD*MAX_EV)+INFO_START)+46

/* CLI Module  Info Events */
#define _CLI_USER_COMMAND       ((CLI_MOD*MAX_EV)+INFO_START)+1

/* MGCP Module Info Events */
#define _MGCP_MEMALLOC          ((MGCP_MOD*MAX_EV)+INFO_START)+1
#define _MGCP_GENERR            ((MGCP_MOD*MAX_EV)+INFO_START)+2
#define _MGCP_IPC_ERROR         ((MGCP_MOD*MAX_EV)+INFO_START)+3
#define _MGCP_MGCP_SWOVER       ((MGCP_MOD*MAX_EV)+INFO_START)+4
#define _MGCP_DBMERR            ((MGCP_MOD*MAX_EV)+INFO_START)+5

/* RUDP Module Info Events */
#define _RUDP_INVEVNT           ((RUDP_MOD*MAX_EV)+INFO_START)+1
#define _RUDP_INV_ACKSEQ        ((RUDP_MOD*MAX_EV)+INFO_START)+2
#define _RUDP_TXWNDOFULL        ((RUDP_MOD*MAX_EV)+INFO_START)+3
#define _RUDP_TXBUFOVERRIDE     ((RUDP_MOD*MAX_EV)+INFO_START)+4
#define _RUDP_RXBUFOVERRIDE     ((RUDP_MOD*MAX_EV)+INFO_START)+5
#define _RUDP_INVSEQDATA        ((RUDP_MOD*MAX_EV)+INFO_START)+6
#define _RUDP_DUPDATA           ((RUDP_MOD*MAX_EV)+INFO_START)+7
#define _RUDP_INVSEQNUL         ((RUDP_MOD*MAX_EV)+INFO_START)+8
#define _RUDP_DUPNUL            ((RUDP_MOD*MAX_EV)+INFO_START)+9
#define _RUDP_INVSEQRST         ((RUDP_MOD*MAX_EV)+INFO_START)+10
#define _RUDP_INVSEQTCS         ((RUDP_MOD*MAX_EV)+INFO_START)+11
#define _RUDP_TCSINVCONID       ((RUDP_MOD*MAX_EV)+INFO_START)+13
#define _RUDP_TCSINXFERINVCONID ((RUDP_MOD*MAX_EV)+INFO_START)+14
#define _RUDP_STRPRNT           ((RUDP_MOD*MAX_EV)+INFO_START)+15
#define _RUDP_INVSEGLEN         ((RUDP_MOD*MAX_EV)+INFO_START)+16
#define _RUDP_DYNMEMALLOC       ((RUDP_MOD*MAX_EV)+INFO_START)+17
#define _RUDP_INVBUFIDX         ((RUDP_MOD*MAX_EV)+INFO_START)+18
#define _RUDP_BUFOVERRIDE       ((RUDP_MOD*MAX_EV)+INFO_START)+19
#define _RUDP_SOCKWRFAILED      ((RUDP_MOD*MAX_EV)+INFO_START)+20

/* IBPMUX Module events */
#define _IBPMUX_GENERR      	((IBPMUX_MOD*MAX_EV)+INFO_START)+1
#define _IBPMUX_INITERR      	((IBPMUX_MOD*MAX_EV)+INFO_START)+2
#define _IBPMUX_DBMERR      	((IBPMUX_MOD*MAX_EV)+INFO_START)+3
#define _IBPMUX_PARSEERR      	((IBPMUX_MOD*MAX_EV)+INFO_START)+4
#define _IBPMUX_SIWERR      	((IBPMUX_MOD*MAX_EV)+INFO_START)+5
#define _IBPMUX_RUDP_BUF      	((IBPMUX_MOD*MAX_EV)+INFO_START)+6
#define _IBPMUX_RUDP_SRVCS      ((IBPMUX_MOD*MAX_EV)+INFO_START)+7
#define _IBPMUX_BSM_TMR      	((IBPMUX_MOD*MAX_EV)+INFO_START)+8
#define _IBPMUX_BSMDBG          ((IBPMUX_MOD*MAX_EV)+INFO_START)+9
#define _IBPMUX_RUDPDBG         ((IBPMUX_MOD*MAX_EV)+INFO_START)+10

/* Session Manager Module Info Events */
#define _BSM_GENEVNT            ((IBPMUX_MOD*MAX_EV)+INFO_START)+11
#define _BSM_SSWOVER            ((IBPMUX_MOD*MAX_EV)+INFO_START)+12
#define _BSM_SSWOVER_FAIL       ((IBPMUX_MOD*MAX_EV)+INFO_START)+13
#define _BSM_GSWOVER            ((IBPMUX_MOD*MAX_EV)+INFO_START)+14
#define _BSM_GSWOVER_FAIL       ((IBPMUX_MOD*MAX_EV)+INFO_START)+15

/* OSPF events */
#define _OSPF_NBRSTATE      	((OSPF_MOD*MAX_EV)+INFO_START)+1
#define _OSPF_IFSTATE       	((OSPF_MOD*MAX_EV)+INFO_START)+2
#define _OSPF_GEN            	((OSPF_MOD*MAX_EV)+INFO_START)+3
#define _OSPF_RTADD            	((OSPF_MOD*MAX_EV)+INFO_START)+4
#define _OSPF_RTDEL            	((OSPF_MOD*MAX_EV)+INFO_START)+5
#define _OSPF_MEMALLOC      	((OSPF_MOD*MAX_EV)+INFO_START)+6
#define _OSPF_IPIFERR      	    ((OSPF_MOD*MAX_EV)+INFO_START)+7
#define _OSPF_PKTERR        	((OSPF_MOD*MAX_EV)+INFO_START)+8
#define _OSPF_NBRERR          	((OSPF_MOD*MAX_EV)+INFO_START)+9
#define _OSPF_IFERR           	((OSPF_MOD*MAX_EV)+INFO_START)+10
#define _OSPF_LSAERR           	((OSPF_MOD*MAX_EV)+INFO_START)+11
#define _OSPF_SIWERR           	((OSPF_MOD*MAX_EV)+INFO_START)+12
#define _OSPF_DBMERR           	((OSPF_MOD*MAX_EV)+INFO_START)+13

/* On-Line Diag Info events */
#define _OND_SCC_ALMCB_CHIP_MON_FIRST_FAIL      ((OND_MOD*MAX_EV)+INFO_START)+1
#define _OND_SCC_ABMCB_CHIP_MON_FIRST_FAIL      ((OND_MOD*MAX_EV)+INFO_START)+2
#define _OND_SCC_ALMSF_CHIP_MON_FIRST_FAIL      ((OND_MOD*MAX_EV)+INFO_START)+3
#define _OND_SCC_ABMSF_CHIP_MON_FIRST_FAIL      ((OND_MOD*MAX_EV)+INFO_START)+4
#define _OND_SCC_CUBITA_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+5
#define _OND_SCC_CUBITB_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+6
#define _OND_SCC_SAR_CHIP_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+7

#define _OND_DMC_M13MUX1_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+8
#define _OND_DMC_M13MUX2_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+9
#define _OND_DMC_M13MUX3_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+10
#define _OND_DMC_M13MUX4_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+11
#define _OND_DMC_M13MUX5_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+12
#define _OND_DMC_M13MUX6_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+13

#define _OND_BSC_M13MUX1_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+14
#define _OND_BSC_M13MUX2_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+15
#define _OND_BSC_BIMM13MUX3_CHIP_MON_FIRST_FAIL ((OND_MOD*MAX_EV)+INFO_START)+16
#define _OND_BSC_BIMM13MUX4_CHIP_MON_FIRST_FAIL ((OND_MOD*MAX_EV)+INFO_START)+17
#define _OND_BSC_BIMM13MUX5_CHIP_MON_FIRST_FAIL ((OND_MOD*MAX_EV)+INFO_START)+18
#define _OND_BSC_BIMM13MUX6_CHIP_MON_FIRST_FAIL ((OND_MOD*MAX_EV)+INFO_START)+19
#define _OND_BSC_BIMOCTAL_CHIP_MON_FIRST_FAIL   ((OND_MOD*MAX_EV)+INFO_START)+20
#define _OND_BSC_OCTAL1_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+21
#define _OND_BSC_OCTAL2_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+22
#define _OND_BSC_OCTAL3_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+23
#define _OND_BSC_OCTAL4_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+24
#define _OND_BSC_OCTAL5_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+25
#define _OND_BSC_OCTAL6_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+26
#define _OND_BSC_OCTAL7_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+27
#define _OND_BSC_ALM_CHIP_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+28
#define _OND_BSC_ABM_CHIP_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+29
#define _OND_BSC_PCIAAL1_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+30
#define _OND_BSC_AAL1_1_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+31
#define _OND_BSC_AAL1_2_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+32
#define _OND_BSC_AAL1_3_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+33
#define _OND_BSC_AAL1_4_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+34
#define _OND_BSC_AAL1_5_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+35
#define _OND_BSC_AAL1_6_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+36
#define _OND_BSC_AAL1_7_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+37
#define _OND_BSC_BIMAAL1_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+38
#define _OND_BSC_FREEDM1_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+39
#define _OND_BSC_FREEDM2_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+40
#define _OND_BSC_SAR_CHIP_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+41

#define _OND_NSC_COMET1_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+42
#define _OND_NSC_COMET2_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+43
#define _OND_NSC_COMET3_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+44
#define _OND_NSC_COMET4_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+45
#define _OND_NSC_COMET5_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+46
#define _OND_NSC_COMET6_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+47
#define _OND_NSC_COMET7_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+48
#define _OND_NSC_COMET8_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+49
#define _OND_NSC_COMET9_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+50
#define _OND_NSC_COMET10_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+51
#define _OND_NSC_COMET11_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+52
#define _OND_NSC_COMET12_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+53
#define _OND_NSC_COMET13_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+54
#define _OND_NSC_COMET14_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+55
#define _OND_NSC_COMET15_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+56
#define _OND_NSC_COMET16_CHIP_MON_FIRST_FAIL    ((OND_MOD*MAX_EV)+INFO_START)+57
#define _OND_NSC_MITEL1_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+58
#define _OND_NSC_MITEL2_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+59
#define _OND_NSC_MITEL3_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+60
#define _OND_NSC_AAL1_1_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+61
#define _OND_NSC_AAL1_2_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+62
#define _OND_NSC_AAL1_3_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+63
#define _OND_NSC_AAL1_4_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+64
#define _OND_NSC_CUBITA_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+65
#define _OND_NSC_CUBITB_CHIP_MON_FIRST_FAIL     ((OND_MOD*MAX_EV)+INFO_START)+66
#define _OND_NSC_SAR_CHIP_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+67

#define _OND_SCC_MEM_TEST_FIRST_FAIL            ((OND_MOD*MAX_EV)+INFO_START)+68
#define _OND_BSC_MEM_TEST_FIRST_FAIL            ((OND_MOD*MAX_EV)+INFO_START)+69
#define _OND_NSC_MEM_TEST_FIRST_FAIL            ((OND_MOD*MAX_EV)+INFO_START)+70

#define _OND_SCC_DATA_PATH_FIRST_FAIL           ((OND_MOD*MAX_EV)+INFO_START)+71

#define _OND_SCCCARD_CHIP_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+72
#define _OND_BSCCARD_CHIP_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+73
#define _OND_NSCCARD_CHIP_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+74
#define _OND_SCC_MEM_TEST_PARITY_FAIL           ((OND_MOD*MAX_EV)+INFO_START)+75
#define _OND_SCC_CELLBUSA_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+76
#define _OND_SCC_CELLBUSB_MON_FIRST_FAIL        ((OND_MOD*MAX_EV)+INFO_START)+77
#define _OND_NSC_MEM_TEST_PARITY_FAIL           ((OND_MOD*MAX_EV)+INFO_START)+78
#define _OND_BSC_MEM_TEST_PARITY_FAIL           ((OND_MOD*MAX_EV)+INFO_START)+79
#define _OND_NSC_DSP_C5_MON_FIRST_FAIL          ((OND_MOD*MAX_EV)+INFO_START)+80
#define _OND_NSC_DSP_C6_MON_FIRST_FAIL          ((OND_MOD*MAX_EV)+INFO_START)+81



/* SNMP Module Info Events */
#define _SNMP_USER_LOGIN                ((SNMP_MOD*MAX_EV) + INFO_START) + 1
#define _SNMP_USER_LOGOUT               ((SNMP_MOD*MAX_EV) + INFO_START) + 2   
#define _SNMP_ISDNCHAN_ADD              ((SNMP_MOD*MAX_EV) + INFO_START) + 3
#define _SNMP_ISDNCHAN_DELETE           ((SNMP_MOD*MAX_EV) + INFO_START) + 4
#define _SNMP_OBJECTADD                 ((SNMP_MOD*MAX_EV) + INFO_START) + 5
#define _SNMP_OBJECTDELETE              ((SNMP_MOD*MAX_EV) + INFO_START) + 6
#define _SNMP_OBJECTMODIFY              ((SNMP_MOD*MAX_EV) + INFO_START) + 7
#define _SNMP_DS1SETDEFPAR_FAIL         ((SNMP_MOD*MAX_EV) + INFO_START) + 8
#define _SNMP_USER_LOGIN_TR             ((SNMP_MOD*MAX_EV) + INFO_START) + 9
#define _SNMP_USER_NL_LOGOUT_TR         ((SNMP_MOD*MAX_EV) + INFO_START) + 10
#define _SNMP_USER_TT_LOGOUT_TR         ((SNMP_MOD*MAX_EV) + INFO_START) + 11
#define _SNMP_DSX1_RELATED_TBL_MODIFY   ((SNMP_MOD*MAX_EV) + INFO_START) + 12

/* LSP Module Info Events */
#define _LSP_GET_TIME_EVENT             ((LSP_MOD*MAX_EV)+INFO_START)+1

/******************************************************************
 * Following Static Table is used to specialized functions. Each event
 * can have a specific Error Handler function.
 */

typedef  struct 
{
    S32_t            evId;       /* Event ID (from above) */
    S32_t            evType;     /* Event Type (from axss_if.h) */
    VOID_FUNC_PTR_t  evFunc;     /* Error Handler function */
} EV_TABLE_ENTRY_t;


#if 0
EV_TABLE_ENTRY_t   eventsTable[] = 
{
    /* Fatal Events/Errors */

    /* Major Events/Errors */
    {_MRPC_L2_RETX_THRES, ERROR_MAJOR, NULL},

    /* Leave this last one alone */
    {0, 0, NULL},
};
#endif

#endif /* AXSS_EVENTS_IF_H */
