/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *
 *Note:
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: sm_if.h,v 1.3 2006/08/25 02:35:36 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/sm_if.h,v $
 *====================================================================
 * $Log: sm_if.h,v $
 * Revision 1.3  2006/08/25 02:35:36  cvsshuming
 * change header message
 *
 * $Endlog $
 *********************************************************************/


#ifndef _SM_IF_H_
#define _SM_IF_H_

#if SNMP_STUFF
#include "snmp_if.h"
#endif

/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */

#define SM_REL_1 "DB.01.00"
#define SM_REL_2 "DB.01.01"

#define ANNOUNCEMENT_SUPPORT 1
#define CARD_TRAP_ENABLE 1
#define SM_NODENAME_LEN 20
#define MAX_NV_READ_RETRIES 3

/* Integrated Alarm definitions for cardActiveRemovalMajor
 * and cardActiveRemovalClear 
 */ 
#define CARD_REMOVE_BITMAP_CLEAR      0 
#define CARD_REMOVE_BITMAP_MAJOR      2 
#define CARD_REMOVE_BITMAP_MASK       3

#define CARD_REMOVE_SHIFT 16

#define CARD_REMOVE_CLEAR     (CARD_REMOVE_BITMAP_CLEAR<<CARD_REMOVE_SHIFT)
#define CARD_REMOVE_MAJOR     (CARD_REMOVE_BITMAP_MAJOR<<CARD_REMOVE_SHIFT)
#define CARD_REMOVE_MASK      (CARD_REMOVE_BITMAP_MASK<<CARD_REMOVE_SHIFT)

#define SM_MAX_RESET_CNT  2  /* Max number of times to reset NMC before fail */

#define MGT_ETHER_SCAN_CYCLES 5         /* Test Mgt Ether Presence every 5 Ping cycles */
#define MGT_ETHER_SW_OVER_WAIT_TIMES  60 /* Wait for 60 ping cycles before switch over */


/* 
 * Following are the services published and offered by the Shelf Manager
 * Please see the definition (prototypes) of the functions associated
 * with these services. All clients should use these service numbers 
 * to request services from Shelf Manager remotely.
 */
#define  SM_GET_SHELF_STATUS         0
#define  SM_CARDS_REPORTING_STATUS   1
#define  LAPD_ASYNC_RESPONSE         2
#define  LAPD_L3_PRIMITIVES          3
#define  SM_ERROR_EVENT_LOG          4
#define  SM_REM_SCC_CHASSIS_IF_TYPE  5

/* 
 * All Of the Shelf/Chassis Manager States are defined below
 */
#define  SM_INIT_STATE                    1
#define  SM_ID_DNLD_STATE                 2
#define  SM_WAIT_IDLE_STATE               3
#define  SM_GO_DEFAULT_STATE              4
#define  SM_ACTIVE_STATE                  5
#define  SM_WAIT_ACTIVE_STATE             6
#define  SM_FAIL_STATE                    7
#define  SM_MISMATCH_STATE                8
#define  SM_CFG_CONFIRM_STATE             9
#define  SM_CARD_NOT_PRESENT_STATE        10
#define  SM_SECONDARY_STATE               11


/* 
 * All of Shelf Manager's IN Messages are defined here
 */
#define  SM_MSG_CARD_STATE_CHANGED_TO_IDLE        1
#define  SM_MSG_CARD_STATE_CHANGED_TO_ACTIVE      2
#define  SM_MSG_CARD_STATE_CHANGED_TO_FAIL        3
#define  SM_MSG_CFG_DOWNLOAD_DONE                 4
#define  SM_MSG_CFG_DOWNLOAD_FAILED               5
#define  SM_MSG_CFG_BUILD_DEFAULT                 6
#define  SM_MSG_DEFAULT_CFG_DONE                  7
#define  SM_MSG_IMAGE_DNLD_REQUEST                8
#define  SM_MSG_IMAGE_DNLD_COMPLETE               9
#define  SM_MSG_IMAGE_DNLD_FAILED                 10
#define  SM_MSG_CARD_REMOVED                      11
#define  SM_MSG_NEW_CARD_INSERTED                 12
#define  SM_MSG_POLL_TIMER_EXPIRED                13
#define  SM_MSG_BACKCARD_REMOVED                  14
#define  SM_MSG_NEW_BACKCARD_INSERTED             15

/* Following are CRM Messages */
#define  SM_MSG_CRM_STATE_CHANGED_TO_ACTIVE       16
#define  SM_MSG_CRM_STATE_CHANGED_TO_STANDBY      17
#define  SM_MSG_CRM_STATE_CHANGED_TO_FAIL         18
#define  SM_MSG_CRM_STATE_CHANGED_TO_MISMATCH     19
#define  SM_MSG_CRM_STATE_CHANGED_TO_F_B_MISMATCH 20
#define  SM_MSG_CRM_SWITCHOVER                    21

/* Following are Messages from other entities */
#define  SM_MSG_RM_FINISHED_RESYNC                22

/* used by PRC to change the state from admin down to admin up */
#define  SM_MSG_CARD_BECOME_ACTIVE                23
#define  SM_MSG_ON_LINE_DIAG                      24



/* following is the name of the system configration file. This is the file
 * is used by the Chassis/Shelf Manager to find out how the unverse looks like
#define  SM_MSG_CARD_INSERTED                     12
 * Therefore, if the system configuration needs to be chaged, the following
 * named file must be updated
 */
#define SYS_CONFIG_FILE   "C:smSysCfg.cfg"

/* 
 * Following  is the list of all possible error codes returned by the
 * fm module API. 
 */
#define SM_OK                                              0
#define SM_ERROR                                          -1
#define SM_ILLEGAL_SLOT                                   -2
#define SM_ILLEGAL_ADDRESS                                -3
#define SM_NMC_RED_NOT_AVAIL                              -4
#define SM_ILLEGAL_RED_TYPE                               -5
#define SM_ILLEGAL_BC_FOR_REDUNDANCY                      -6
#define SM_DIFF_SLOT_ALREADY_LINKED                       -7
#define SM_FW_IMAGES_DONT_MATCH                           -8
#define SM_RED_DEL_NOT_ALLOWED                            -9
#define SM_ILLEGAL_CARD_PAIR                              -10
#define SM_SWITCH_BACK_NOT_ALLOWED                        -11
#define SM_NO_DMC_PRESENT_FOR_DMC_RED                     -12
/* Following Addition Error codes for SNMP */
#define SM_SNMP_ILLEGAL_ACTION                            -13
#define SM_SNMP_ILLEGAL_IF_MODE                           -14
#define SM_SNMP_END_OF_SLOTS                              -15
#define SM_SNMP_ILLEGAL_Q_PROFILE                         -16
#define SM_SNMP_CARD_DOES_NOT_EXIST                       -17
#define SM_SNMP_SCC_SWITCHOVER_NOT_ALLOWED                -18
#define SM_SNMP_SERVICE_NOT_AVAIL_ON_SCC                  -19
#define SM_SNMP_SERVICE_NOT_AVAIL_ON_DMC                  -20
#define SM_SNMP_ILLEGAL_SLOT_NO_FOR_THIS_ACTION           -21
#define SM_SNMP_THIS_ACTION_NOT_ALLOWED_ON_STBY           -22
#define SM_ILLEGAL_IF_MODE                                -23
#define SM_SNMP_THIS_ACTION_NOT_ALLOWED_IN_CURRENT_STATE  -24
#define SM_SNMP_NMC_HAS_ACTIVE_CONN                       -25
#define SM_ILLEGAL_ERR_TYPE                               -26
#define SM_ILLEGAL_CARD_STATE                             -27
#define SM_SC_HAS_ACTIVE_CONFIG                           -28
#define SM_SNMP_ILLEGAL_IF_TYPE                           -29
#define SM_SNMP_ILLEGAL_E1_SET                            -30
#define SM_SNMP_LINE_EXIST                                -31
#define SM_SNMP_IF_TYPE_NO_CHANGE                         -32
#define SM_SNMP_SET_PEER_IFTYPE_FAIL                      -33
#define SM_FAILS_TO_UPDATE_SC_IP_ADDR                     -34 
#define SM_ILLEGAL_PRIMARY_CARD                           -35 
#define SM_ILLEGAL_UPGDCMIT                               -36
#define SM_ILLEGAL_SW_VERSION                             -37

/*
 * Following is the threshold for the SM<->CM poll message lost, before a 
 * card is declared to be failed.
 */
#define SM_POLL_THRESHOLD   40
#define SM_POLL_TIMER       1  /* Number of Seconds */

#define NO_UPGARDE_DOWNGRADE 0

/* Following is the maximum NSC redundant pairs */
#define MAX_RED_PAIRS 13

/* Default version for crect_t structure */
#define DEFAULT_VERSION  0

#define FAB_VER_SIZE    10
#define SERIAL_NUM_SIZE 11

/* 
 * Message header for Shelf Manager.
 * Note: all messages to and from Shelf Manager will have this message
 *       header structure.
 */
typedef struct smMsgHdr
{
    U16_t    msg;
    U16_t    len;
    U16_t    version;
    U16_t    srcSlot;
} SM_MSG_HDR_t;


/* A card's record */

typedef struct cardInfo
{
    U8_t     srcImageFileName[MAX_FILE_NAME_LENGTH]; /* image lives on SCC HD*/
    U8_t     srcCfgFileName[MAX_FILE_NAME_LENGTH]; /* Config lives on SCC HD*/
    U8_t     dstImageFileName[MAX_FILE_NAME_LENGTH]; /* image lives on card */
    U8_t     dstFlashFileName[MAX_FILE_NAME_LENGTH]; /* Flash lives on card */
    U32_t    reserved[3];
} CARD_INFO_REC_t;

typedef struct cRec
{
   /* Front card Info */
    U32_t       cType;                 /* Card's Type */
    U32_t       cSignature;
    U32_t       cSlotNum;              /* Card's slot NUmber */
    S32_t       cState;                /* Card's State */
    S32_t       cNative;               /* Card is Native or not */
    U32_t       cHwVersion;            /* Card's HW Version */
    U8_t        cSwVersion[MAX_VERSION_LENGTH]; /* Card's SW Version */
    U8_t        cFwVersion[MAX_VERSION_LENGTH]; /* Card's FW Version */
    U8_t        cFabVersion[FAB_VER_SIZE];       /* Card's Fab Version */
    U8_t        cSerialNum[SERIAL_NUM_SIZE];      /* Card's Serial Number */
    /* Failure/Reset/Mismatch reasons */
    U32_t       cMismatchReason;       /* Card's Mismatch Reason */
    U32_t       cFailReason;           /* Card's Fail Reason */
    U32_t       cResetReason;          /* Card's Reset Reason(sysResetReason)*/
    /* Back Card Info */
    U32_t       cBcType;               /* Card's BackCard Type */
    U32_t       cBcHwVersion;          /* Card's BackCard HwVersion */
    U8_t        cBcSerialNum[SERIAL_NUM_SIZE];  /* Card's BackCard SerialNum */
    /* Daughter Card Info */
    U32_t       cDc1Type;            /* Card's Daughter Card Type */
    U32_t       cDc1HwVersion;       /* Card's Daughter Card HwVersion */
    U8_t        cDc1SerialNum[SERIAL_NUM_SIZE];  /*  Daughter Card SerialNum */
    U32_t       cDc2Type;            /* Card's Daughter Card Type */
    U32_t       cDc2HwVersion;       /* Card's Daughter Card HwVersion */
    U8_t        cDc2SerialNum[SERIAL_NUM_SIZE]; /* Daughter Card SerialNum */
    /* Redundancy Info */
    U32_t       cRedType;              /* Card's Redundancy Type */
    U32_t       cRedPairs;             /* Card's Redundancy Pairs */
    /* Dmc Info     */
    S32_t       cDmcPresent;           /* Is DMC Present for this SCC */
    /* Misc. info */
    U32_t       cStatus;               /* Card's Status MASTER/SLAVE PRESENT,*/
    U32_t       cFlags;                /* Card's Flags; SWITCHOVE, etc.  */
    U32_t       cValid;                /* Card's recod is valid or not */
    S32_t       cCubitId;              /* Cubit Id for service Card */
    S32_t       cRamBackUp;            /* Card's RAM Back Up is Enabled/Dis */
    S32_t       cIfMode;               /* Card's Interface Mode */
    S32_t       cIfType;               /* Card's Interface Type (T1/E1,etc) */
    /* MRPC Layer 2 Synchronization stuff goes in here as well */
    S16_t       txSeq;                 /* Card's L2 Next TxSeq */
    S16_t       ackSeq;                /* Card's L2 Next Expected ACK */
    S16_t       expRxSeq;              /* Card's L2 Next Expected RxSeq */
    /* Communcation messages with this card */
    U32_t       cLastMsgSent;          /* Last message sent to card  */
    U32_t       cLastMsgRecvd;         /* Last message recvd from card  */
    U32_t       cLastEvtRecvd;         /* Last Event recvd from card  */
    U32_t       cAnmtFiles;            /* a flag for Annoncement file downloading */ 
    S32_t       cResetCnt;             /* Num of times NMC is reset */ 
    SIW_TID_t   cIdTid;                /* Task Id of ID Download Task */
    /* For SCC use only */
    S32_t       cBimMacAddrSize;         /* Size of BIM Mac Addresses */
    U8_t        cIpAddr[IP_ADDR_SIZE]; /* IP Address */
    U8_t        cIpAddr2[IP_ADDR_SIZE]; /* IP Address */
    U8_t        cAtmIpAddr[IP_ADDR_SIZE]; /* ATM IP Address */
    U8_t        cGatewayIpAddr[IP_ADDR_SIZE]; /* Gateway IP Address */
    U8_t        cIpAddrMask[IP_ADDR_SIZE];    /* IP Address Mask */
    U8_t        cIpAddr2Mask[IP_ADDR_SIZE];    /* IP Address Mask */
    U8_t        cAtmIpAddrMask[IP_ADDR_SIZE]; /* ATM IP Address Mask */
    U8_t        cFcMacAddr1[MAC_ADDR_SIZE]; /* MAC Address for Enet Port 1  */
    U8_t        cFcMacAddr2[MAC_ADDR_SIZE]; /* MAC Address for Enet Port 2  */
    U8_t        cBimMacAddrBase[MAC_ADDR_SIZE]; /* BIM MAC Address Base */
    U8_t        cTftpKey[TFTP_KEY_SIZE]; /* TFTP Password info */
    /* system fields */
    S32_t       systemRackNum;
    S32_t       systemNodeNum;
    S32_t       systemNodeType;
    S32_t       systemTimeZone;
    S16_t       cMgtEtherPresent;  /* status of Mgt ether lines connected */
    S16_t       cEtherPresent;     /* status of BIM ether lines connected */
    U8_t        systemNodeName[SM_NODENAME_LEN];
    /* System Time Stamp */
    struct timespec cTs;
    /* Reserved Fields for future Expansion */
    U16_t       cVersion;      /* crec_t struct version */
#define C_REC_RESV       119   /* change this # if use more cResv fields */
#define C_IPLP_ADDR      4             
#define C_IPLP_ADDRMASK  4+IP_ADDR_SIZE 
#define C_IPLP_BITMAP    C_IPLP_ADDRMASK+IP_ADDR_SIZE   /* It is 32 bit wide */
    U8_t        cResv[C_REC_RESV];
    U8_t        cAddSecRedProgressing;
    S16_t       cOnLineDiag;    /* status of on line diagnostic */      
    S32_t       cPeerSlotNum;        /* Secondary card's slot # */
    U32_t       cPeerState;       /* Secondary card's state */
    CARD_INFO_REC_t cCfg;
} CREC_t;


/* Shelf Structure */
typedef struct sRec
{
#if 0
    U32_t   sState;
    U32_t   sStatus;
#endif
    CREC_t  slot[MAX_SLOTS];
} SREC_t;


/* 
 * Following for the NMC Redundancy 
 */

typedef enum
{
    RED_NONE = 0,
    RED_BACK_PLANE,
    RED_DMC
}RED_TYPE_t;

#if 1
typedef struct nmcRedOld
{
    S32_t    secCard;     /* Secondary Card */
    S32_t    switchBack;  /* SwitchBack Flag */
    BOOL_t   primary;     /* Is this card Primary(1) or Secondary(0) */
    S8_t     phyCId;      /* Physical Cubit Id */
    S8_t     priCId;      /* Primary  Cubit Id */
    S8_t     secCId;      /* Secondary Cubit Id */
    S8_t     transit;     /* Pad to even Boundary */
    S8_t     dummy[64];   /* For testing of upgrade/downgrade */
}NMC_RED_OLD_t;
#endif

typedef struct nmcRed
{
    S32_t    secCard;     /* Secondary Card */
    S32_t    switchBack;  /* SwitchBack Flag */
    BOOL_t   primary;     /* Is this card Primary(1) or Secondary(0) */
    S8_t     phyCId;      /* Physical Cubit Id */
    S8_t     priCId;      /* Primary  Cubit Id */
    S8_t     secCId;      /* Secondary Cubit Id */
    S8_t     transit;     /* Pad to even Boundary */
}NMC_RED_t;


typedef struct nmcRedMisc
{
    S32_t  redType;
    S32_t  redLinkedSlot;
}NMC_REDTYPE_t;

typedef struct bscRed
{
    S32_t    secCard;     /* Secondary Card */
    S32_t    switchBack;  /* SwitchBack Flag */
    BOOL_t   primary;     /* Is this card Primary(1) or Secondary(0) */
    U8_t     resvd[8];
}BSC_RED_t;

/*
 * Following for SM<->CM Ping/Polling protocol
 */
typedef struct smPollPing
{
    MSG_t  *pingResp;
    S32_t  pingCount;
    S32_t  pingThreshold;
    BOOL_t pingEnable;  /* In case need to disable polling a card */
    S32_t  pingFirstTime; /* For MRPC L2 Synchronization purposes */
} SM_POLL_t;


/*
 *====================================================================
 *=                         General Purpose Macros                   =
 *====================================================================
 */
#define SM_HDR_PTR(i) ((SM_MSG_HDR_t *)(&(i)->data[0]))
#define SM_DATA_PTR(i) ((char *)(&(((MSG_t *)(i))->data[sizeof(SM_MSG_HDR_t)])))

#define SM_PRINT_CARD_TYPE(s) {           \
    switch ((s)) {                        \
	case CARD_NMC:                    \
	    printf("NMC");                \
	break;                            \
	case CARD_NSC:                    \
	    printf("NSC");                \
	break;                            \
	case CARD_SCC:                    \
	    printf("SCC");                \
	break;                            \
	case CARD_DIC:                    \
	    printf("DMC");                \
	break;                            \
	case CARD_BSC:                    \
	    printf("BSC");                \
	break;                            \
	default:                          \
	    printf("UNKNOWN");            \
	break;                            \
    }                                     \
}

#define SM_DISP_STATE(s) {                \
    switch ((s)) {                        \
	case SM_CARD_NOT_PRESENT_STATE:   \
	    printf("CARD_NOT_PRESENT");   \
	break;                            \
	case SM_INIT_STATE:               \
	    printf("INIT");               \
	break;                            \
	case SM_ID_DNLD_STATE:            \
	    printf("ID_DNLD");            \
	break;                            \
	case SM_FAIL_STATE:               \
	    printf("FAIL");               \
	break;                            \
	case SM_WAIT_IDLE_STATE:          \
	    printf("WAIT_IDLE");          \
	break;                            \
	case SM_ACTIVE_STATE:             \
	    printf("ACTIVE");             \
	break;                            \
	case SM_MISMATCH_STATE:           \
	    printf("MISMATCH");           \
	break;                            \
	case SM_GO_DEFAULT_STATE:         \
	    printf("GO_DEFAULT");         \
	break;                            \
	case SM_WAIT_ACTIVE_STATE:        \
	    printf("WAIT_ACTIVE");        \
	break;                            \
	case SM_CFG_CONFIRM_STATE:        \
	    printf("CFG_CONFIRM");        \
	break;                            \
	case SM_SECONDARY_STATE:          \
	    printf("SECONDARY");          \
	break;                            \
	default:                          \
	    printf("UNKNOWN");            \
	break;                            \
    }                                     \
}


#define DISP_SM_MSG(s) {                          \
    switch ((s)) {                                \
	case SM_MSG_CARD_STATE_CHANGED_TO_IDLE:   \
	    printf("CARD moved to IDLE State");   \
	break;                                    \
	case SM_MSG_CARD_STATE_CHANGED_TO_ACTIVE: \
	    printf("CARD moved to ACTIVE State"); \
	break;                                    \
	case SM_MSG_CARD_STATE_CHANGED_TO_FAIL  : \
	    printf("CARD moved to FAIL   State"); \
	break;                                    \
	case SM_MSG_CFG_DOWNLOAD_DONE:            \
	    printf("Config Building is Done ");   \
	break;                                    \
	case SM_MSG_CFG_DOWNLOAD_FAILED:          \
	    printf("Config Building is FAILED");  \
	break;                                    \
	case SM_MSG_DEFAULT_CFG_DONE:             \
	    printf("DEFAULT Cfg Build is Done");  \
	break;                                    \
	case SM_MSG_IMAGE_DNLD_REQUEST:           \
	    printf("BOOT Msg Receieved");         \
	break;                                    \
	case SM_MSG_IMAGE_DNLD_COMPLETE:          \
	    printf("Image Download is complete"); \
	break;                                    \
	case SM_MSG_IMAGE_DNLD_FAILED:            \
	    printf("Image Download is FAILED");   \
	break;                                    \
	case SM_MSG_CARD_REMOVED:                 \
	    printf("Card has been Removed");      \
	break;                                    \
	case SM_MSG_NEW_CARD_INSERTED:            \
	    printf("New Card is Inserted");       \
	break;                                    \
	default:                                  \
	    printf("UNKNOWN Message");            \
	break;                                    \
    }                                             \
}


/*
 *====================================================================
 *=                         external variables                       =
 *====================================================================
 */

extern SREC_t     shelf;
extern NMC_RED_t  nmcRed[MAX_SLOTS];
extern NMC_REDTYPE_t  nmcRedType;
extern SM_POLL_t smPing[MAX_SLOTS];



/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */
extern RET_t SmGetPeerCard( S32_t slot);
extern RET_t SmGetAllPrimInGrp(S32_t secSlot, OUT S32_t *primList, S16_t *listSize);
extern S32_t SmGetUpgradeDowngradeMode(S32_t card) ;
extern RET_t SmUpdateRedPairs(S32_t slot, S32_t redPairs);
extern STATUS_t SmCardAdminUp (S16_t card);
extern STATUS_t SmCardAdminDown (S16_t card);

extern RET_t SmFlipBscBcRelay(IN S32_t slot, S32_t onOff);
extern RET_t SmSnmpGetRedSecondary();
extern RET_t SmSnmpGetNextRed(S32_t pri, S32_t sec, S32_t *redPri, S32_t *redSec);
extern RET_t addred(S32_t pri, S32_t sec);
extern RET_t delred(S32_t pri, S32_t sec);
extern RET_t SmSetServiceCardTOD(S32_t target, struct timespec *ts);
extern RET_t SmSetImageFileName(S32_t card, char *name);
extern RET_t SmBackCardRemoved(IN S32_t slot);
extern RET_t SmBackCardInserted(IN S32_t slot);


extern RET_t SmSnmpGetRedCard(S32_t slot);
extern BOOL_t SmSnmpIsCardSecondary(S32_t slot);
extern RET_t SmCardRemoved(IN S32_t slot);
extern RET_t SmNewCardInserted(IN S32_t slot);
extern S32_t SmIfType (U32_t slot);
extern S32_t GetSystemNodeNum();
extern char * GetSystemNodeName(char *nodeName);
extern BOOL_t SmIsRedCardAvail(S32_t slot);
extern BOOL_t IsCardCongested(IN S32_t card, S32_t priority);
extern RET_t SmPollTimerExpired();
extern RET_t SmResetCard(IN S32_t slot);
extern RET_t SmGetCardFwVersion(IN S32_t slot, OUT char *fwVersion);
extern S8_t  SmState (IN S32_t slot);
extern U32_t SmType (IN S32_t slot);
extern S32_t SmService (IN S32_t slot);
extern U32_t SmBCType (IN S32_t slot);
extern U32_t SmDc1Type (IN S32_t slot);
extern U32_t SmDc2Type (IN S32_t slot);
extern RET_t SmEnablePolling ();
extern RET_t SmEnableCardPolling (IN S32_t card);
extern RET_t SmDisablePolling ();
extern RET_t SmDisableCardPolling (IN S32_t card, S32_t callernum);
extern RET_t SetPollingThreshold(S32_t new);
extern BOOL_t SmL2ResyncInProgress(IN S32_t slot);
extern RET_t SmL2ResyncDone(IN S32_t slot);
extern RET_t SmSetFailReason(S32_t card, U32_t reason);
extern RET_t updateShelfConfig(S32_t card);
extern S32_t SmIfMode (U32_t);
extern RET_t SmRemSccSetChassisIfType(IN MSG_t *, OUT MSG_t **);
extern RET_t SmSetChassisIfType (S32_t);
extern S32_t SmGetChassisIfType ();
extern RET_t SmSetServiceCardCubitId (S32_t, U8_t);
extern RET_t SmSetServiceCardCIdAndSlot (S32_t);
extern S32_t l2pSlotNum(S32_t);
extern S32_t p2lSlotNum(S32_t);
extern S32_t getPeerServiceCardNo (S32_t slot);
extern BOOL_t ok2SendScDb(S32_t idx, S32_t peer);
#if 0
extern RET_t SmSendTrap(IN U32_t trap, U32_t slot, U32_t fcType,
                        U32_t bcType, U32_t status);
#endif /* 0 */
#if SNMP_STUFF
extern RET_t SmCardSnmpAction(logicalCardEntry_t *data, SNMP_ACCESS_t action);
#endif
extern RET_t dispCardInfo(IN CREC_t *rec);
extern RET_t SccErrHandler (IN S32_t slot, S32_t eType, S32_t errNo);
extern RET_t SmErrHandler (IN S32_t slot, S32_t eType, S32_t errNo);
extern RET_t SmSendMsgToSm(IN S32_t slot, S32_t message);
extern RET_t SmNotifyRmSyncDone(IN S32_t slot);
extern void  SmWaitSmReady();

#endif /* _SM_IF_H_ */
