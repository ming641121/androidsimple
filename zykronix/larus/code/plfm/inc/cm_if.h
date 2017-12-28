/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *    This header file contains the API for the Card Manager. In addition, 
 *    The card Manager message header and all messages are defined in here
 *
 *Note:
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: cm_if.h,v 1.3 2006/10/03 04:32:13 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/cm_if.h,v $
 *====================================================================
 * $Log: cm_if.h,v $
 * Revision 1.3  2006/10/03 04:32:13  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.2  2006/10/03 04:24:30  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/
#ifndef _CM_IF_H_
#define _CM_IF_H_

/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */

/* 
 * Following are the services published and offered by the Card Manager
 * Please see the definition (prototypes) of the functions associated
 * with these services. All clients should use these service numbers 
 * to request services from Card Manager remotely.
 */
#define  CM_SCC_MESSAGES             1
#define  CM_GET_CARD_STATE           2
#define  CM_RESET_CARD               3
#define  CM_GET_CARD_TYPE            4
#define  CM_PING_SERVICE             5
#define  CM_ENABLE_DB_REPLICATION    6
#define  CM_DISABLE_DB_REPLICATION   7
#define  CM_SET_IF_MODE              8
#define  CM_GET_INTG_ALARM           9
#define  CM_SET_TOD                  10
#define  CM_SET_CUBIT_ID             11
#define  CM_SET_CUBIT_ID_AND_SLOT    12
#define  CM_SET_FLIP_ONOFF           13
#define  CM_DBM_REBUILD_SC_TABLES    14
#define  CM_PERFORM_REVERSE_SYNC     15
#define  CM_SET_SLOT    	     16
#define  CM_UPDATE_INTERFACE_IP_ADDR 17
#define  CM_UPDATE_INTERFACE_MASK    18
#define  CM_UPDATE_PEER_SC_STATE    17    
#define  CM_UPDATE_PEER_SC_SLOT     18    
#define  CM_SET_PEER_SYNC_STATUS    19    
#define  CM_UPDATE_RED_PAIRS        20    
#define  CM_RESET_MRPC_L2           21

typedef enum {
    CM_POST_PASSED = 1,
    CM_POST_FAILED
} CM_POST_STATUS_t;


/* 
 * All of Card Manager's IN Messages are defined here
 */
#define  CM_MSG_IMAGE_DNLD_INITIATED      1
#define  CM_MSG_CFG_DNLD_INITIATED        2
#define  CM_MSG_COMPUTE_CKSUM_AND_EXECUTE 3
#define  CM_MSG_USE_DEFAULT_CFG           4
#define  CM_MSG_COMIT_CFG                 5
#define  CM_MSG_GOTO_MISMATCH_STATE       6
#define  CM_MSG_POST_PASSED               7
#define  CM_MSG_POST_PASSED_NO_ID         8
#define  CM_MSG_POST_FAILED               9
#define  CM_MSG_RESP                      10
#define  CM_MSG_NACK                      11
#define  CM_MSG_TRAP_EVENT                12
#define  CM_MSG_SECONDARY_STATE           13
#define  CM_MSG_GOTO_FAIL_STATE           14


#define VALID_POST_STATUS(f) \
     ((((f) == CM_POST_PASSED) || ((f) == CM_POST_FAILED)) ? TRUE : FALSE)

/* 
 * Following  is the list of all possible error codes returned by the
 * fm module API. 
 */
#define CM_OK                                    0
#define CM_ERROR                                -1
#define CM_ILLEGAL_FILE_NAME                    -2
#define CM_ILLEGAL_ADDRESS                      -3
#define CM_ILLEGAL_SLOT                         -4
#define CM_COULD_NOT_OPEN_FILE                  -5
#define CM_FAILED_TO_SPAWN_APP_ROOT_TASK        -6
#define CM_ILLEGAL_POST_STATUS                  -7
#define CM_ILLEGAL_ERR_TYPE                     -8



/* 
 * All Of the card Manager States are defined below
 */
#define  CM_INIT_STATE                    1
#define  CM_BOOT_STATE                    2
#define  CM_FAIL_STATE                    3
#define  CM_IDLE_STATE                    4
#define  CM_ACTIVE_STATE                  5
#define  CM_STANDBY_STATE                 6
#define  CM_MISMATCH_STATE                7
#define  CM_WAIT_COMIT_CFG_STATE          8
#define  CM_WAIT_DNLD_IMAGE_STATE         9

/* Integrated Alarm defines */
#define IA_BITMAP_CLEAR      0
#define IA_BITMAP_MINOR      1
#define IA_BITMAP_MAJOR      2
#define IA_BITMAP_MASK       3

#define IA_LINE_SHIFT        0
#define IA_EMMTEMP_SHIFT     8
#define IA_EMMVOLT_SHIFT     10

#define IA_LINE_CLEAR        (IA_BITMAP_CLEAR<<IA_LINE_SHIFT)
#define IA_LINE_MINOR        (IA_BITMAP_MINOR<<IA_LINE_SHIFT)
#define IA_LINE_MAJOR        (IA_BITMAP_MAJOR<<IA_LINE_SHIFT)
#define IA_LINE_MASK         (IA_BITMAP_MASK<<IA_LINE_SHIFT)

#define IA_EMMTEMP_CLEAR     (IA_BITMAP_CLEAR<<IA_EMMTEMP_SHIFT)
#define IA_EMMTEMP_MINOR     (IA_BITMAP_MINOR<<IA_EMMTEMP_SHIFT)
#define IA_EMMTEMP_MAJOR     (IA_BITMAP_MAJOR<<IA_EMMTEMP_SHIFT)
#define IA_EMMTEMP_MASK      (IA_BITMAP_MASK<<IA_EMMTEMP_SHIFT)

#define IA_EMMVOLT_CLEAR     (IA_BITMAP_CLEAR<<IA_EMMVOLT_SHIFT)
#define IA_EMMVOLT_MINOR     (IA_BITMAP_MINOR<<IA_EMMVOLT_SHIFT)
#define IA_EMMVOLT_MAJOR     (IA_BITMAP_MAJOR<<IA_EMMVOLT_SHIFT)
#define IA_EMMVOLT_MASK      (IA_BITMAP_MASK<<IA_EMMVOLT_SHIFT)

#define CM_POLL_TIMER       1  /* Number of Seconds */

/* 
 * following is for the Application Root Task
 */
#define  APP_ROOT_TASK_ID           SIW_APPL_SW
#define  APP_ROOT_TASK_PRIORITY     10
#define  APP_ROOT_TASK_STACK_SIZE   SIW_DEFAULT_STACK 

/* 
 * Message header for card Manager.
 * Note: all messages to and from Card Manager will have this message
 *       header structure.
 */
typedef struct cmMsgHdr
{
    U16_t    msg;
    U16_t    len;
    U16_t    version;
    U16_t    srcSlot;
    U32_t    cFlags;
    U32_t    cIfMode;
    U32_t    cRamBackUp;
    S8_t     cubitId;
    U32_t    resv1;
    U32_t    resv2;
    U32_t    resv3;
    U32_t    resv4;
    U32_t    resv5;
    struct timespec cTs;
    char     IpAddr[IP_ADDR_SIZE];
    char     AtmIpAddr[IP_ADDR_SIZE];
    char     IpAddr2[IP_ADDR_SIZE];
} CM_MSG_HDR_t;

/*
 * Firmware image file Header Structure
 */
typedef struct fileHdr 
{
    S32_t   len;
    U32_t   offset;
    U32_t   cardType;
    U16_t   slot;
    U16_t   ckSum;
    char    version[MAX_VERSION_LENGTH];
    char    fileName[MAX_FILE_NAME_LENGTH];
    U16_t   pad;   /* pad to 68 byte boundary */
}FILE_HDR_t;


#if 0 /* SSSSS Make one generic file header */ 
/*
 * Configuration file Header Structure
 */
typedef struct cfgFileHdr 
{
    S32_t   len;
    U32_t   offset;
    U32_t   cardType;
    U16_t   slot;
    U16_t   ckSum;
    char    version[MAX_VERSION_LENGTH];
    char    fileName[MAX_FILE_NAME_LENGTH];
    U16_t   pad;   /* pad to 68 byte boundary */
}CFG_FILE_HDR_t;

#endif  /*0  SSSSS Make one generic file header */ 

typedef FILE_HDR_t IMAGE_FILE_HDR_t;
typedef FILE_HDR_t CFG_FILE_HDR_t;


#if 0   /* Re-map Boot Msg over to the Card Record */
/* Boot Message */
typedef struct bm
{
    U32_t    cardType;
    U32_t    slotNum;
    U32_t    cardState;
    U32_t    hwVersion;
    U32_t    swVersion;
    U32_t    bcType;
    U32_t    bcHwVersion;
    U32_t    dc1Type;
    U32_t    dc1HwVersion;
    U32_t    dc2Type;
    U32_t    dc2HwVersion;
    U32_t    reserved;
}BM_t;
#else   /* Re-map Boot Msg over to the Card Record */
    typedef CREC_t BM_t;
#endif   /* Re-map Boot Msg over to the Card Record */

typedef struct dsx1lnstinfo_t {
    U32_t dsx1LineIndex;
    U32_t dsx1LineStatus;
} dsx1LnStInfo_t;

/*
 *====================================================================
 *=                         General Purpose Macros                   =
 *====================================================================
 */
#define CM_MSG_HDR_PTR(i) ((CM_MSG_HDR_t *)(&(i)->data[0]))
#define CM_DATA_PTR(i) ((char *)(&(((MSG_t *)(i))->data[sizeof(CM_MSG_HDR_t)])))

#define   FW_FLASH_FILE_NAME "flash_image"
#define   FW_IMAGE_FILE_NAME "fm_image"
#define   GET_FW_IMAGE_LEN() (2048 * 1000) /* 2 Meg for now */
#define   GET_FW_IMAGE_START_ADDR() \
      ((U32_t)SiwAllocMem(SIW_STATIC, GET_FW_IMAGE_LEN(), WAIT_FOREVER))

#define DISP_CM_MSG(s) {                         \
    switch ((s)) {                               \
	case CM_MSG_IMAGE_DNLD_INITIATED:        \
	    printf("Image download Initiated");  \
	break;                                   \
	case CM_MSG_CFG_DNLD_INITIATED:          \
	    printf("Config Download Initiated ");\
	break;                                   \
	case CM_MSG_COMPUTE_CKSUM_AND_EXECUTE:   \
	    printf("Compute CkSum & Execute");   \
	break;                                   \
	case CM_MSG_USE_DEFAULT_CFG:             \
	    printf("Go Default");                \
	break;                                   \
	case CM_MSG_COMIT_CFG:                   \
	    printf("Commit Configuration");      \
	break;                                   \
	case CM_MSG_GOTO_MISMATCH_STATE:         \
	    printf("Go to MISMATCH State ");     \
	break;                                   \
	case CM_MSG_POST_PASSED:                 \
	    printf("POST is Passed");            \
	break;                                   \
	case CM_MSG_POST_FAILED:                 \
	    printf("POST is FAILED");            \
	break;                                   \
	case CM_MSG_POST_PASSED_NO_ID:           \
	    printf("POST Passed; NO Image Dnld Needed"); \
	break;                                   \
	default:                                 \
	    printf("UNKNOWN Message");           \
	break;                                   \
    }                                            \
}
/*
 *====================================================================
 *=                         external variables                       =
 *====================================================================
 */


extern S32_t cmMyLogSlotNum;

/*
 *====================================================================
 *=                          external routines                       =
 *====================================================================
 */

extern U16_t checkSum(U32_t *addr, U32_t len);

/****************************************************************************
 * Function:    CmInitialPostStatus()
 * Descripion:  This routine Is an API to be used by system Services to
 *              present the Initial status of POST (Power On Self Tests)   
 *              This routine prepares a message and sends it to Card Manager
 *              to take an appropriate action.
 * In Parms:    CM_POST_STATUS_t status  - CM_POST_PASSED or CM_POST_FAILED 
 *              S32_t startType          - SOFT_REBBOT_NO_ID, etc...
 * Out Parms:   None 
 * Return Values:
 *              CM_OK                  - If all works out OK
 *              CM_ILLEGAL_POST_STATUS - status was not CM_POST_PASSED or 
 *                                                      CM_POST_FAILED
 *              CM_ILLEGAL_ADDRESS     - Could not allocate MSG Buffer    
 *              CM_ERROR               - Could not enque  Message
 */
extern RET_t CmInitialPostStatus(IN CM_POST_STATUS_t status, S32_t startType);


/****************************************************************************
 * Function:    CmGetRemCardState()
 * Descripion:  This routine is a wrapper around CmGetCardState(). It calls
 *              CmGetCardState(), allocates a message buffer and
 *              Packages the response and returns. It is assumed that MRPC
 *              will do its job and deliver the response to the rightful
 *              owner.
 *              NOTE: This function should be called only if the state of a 
 *                    remote card is needed. For local card's state call
 *                    CmGetCardState()
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              CM_ERROR
 *              Any error returned by lower level routine
 */
extern S32_t CmGetRemCardState(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmGetRemCardInfo(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmRemEnableDbReplication(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmRemDisableDbReplication(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmSetRemIfMode(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmRemCardIntgAlarm(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmSetRemTOD(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmRemUpdateInterfaceIpAddr(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmRemUpdateInterfaceMask(IN MSG_t *inMsg, OUT MSG_t **outMsg);
/* VS */
extern S32_t CmUpdatePeerScState(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmUpdatePeerScSlot(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmSetDbmBlockScRepl(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmRemCardRemoved(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmUpdateRedPairs(IN MSG_t *inMsg, OUT MSG_t **outMsg);
extern S32_t CmResetMrpcL2(IN MSG_t *inMsg, OUT MSG_t **outMsg);

/****************************************************************************
 * Function:    CmResetRemCard()
 * Descripion:  This routine is a wrapper around CmResetCard(). 
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              NONE
 */
extern S32_t CmResetRemCard(IN MSG_t *inMsg, OUT MSG_t **outMsg);


/****************************************************************************
 * Function:    CmGetRemoteCardInfo()
 * Descripion:  This routine sends an MRPC message to requested card to get its
 *              state and prints the infomation  
 * In Parms:    S32_t   slot - The card number in question 
 * Out Parms:   None 
 * Return Values:
 *              the state of remote card
 */
extern S32_t CmGetRemoteCardInfo(S32_t slot);

/****************************************************************************
 * Function:    GetHwCardInfo()
 * Descripion:  This routine reads Hardware specific information about a given 
 *              card. This information includes Front Card type, Back Card
 *              type, BIM or MSM card information.
 *              All of this information is stored in CREC_t structure, which
 *              is provided by the user. 
 * In Parms:    S32_t   slot - The card number in question 
 * Out Parms:   CREC_t  *cRec - card Record 
 * Return Values:
 *               OK (0) ot ERROR (-1)
 */
extern RET_t GetHwCardInfo(CREC_t *cRec);

/****************************************************************************
 * Function:    GetCardState ()
 * Descripion:  This routine returns the card state for a perpective card
 *
 * In Parms:    slot number
 * Out Parms:   none
 * Return Values:
 *               OK (0) ot ERROR (-1)
 */
extern S8_t GetCardState (S32_t slot);

/****************************************************************************
 * Function:    GetCardType ()
 * Descripion:  This routine returns the card type for a perpective card
 *
 * In Parms:    slot number
 * Out Parms:   none
 * Return Values:
 *               OK (0) ot ERROR (-1)
 */
extern U32_t GetCardType (S32_t slot);

/****************************************************************************
 * Function:    GetCardService ()
 * Descripion:  This routine returns the service for a perpective card
 *
 * In Parms:    slot number
 * Out Parms:   none
 * Return Values:
 *               OK (0) ot ERROR (-1)
 */
extern S32_t GetCardService (S32_t slot);
extern S32_t BscFlipBcRelay(MSG_t *, MSG_t **);
extern RET_t setBscBcRelay(S32_t onOff);


#ifdef SCC
extern void lsCards (void);
#endif

#ifdef BSC
extern U32_t  CmDc1Type ();
extern U32_t  CmDc2Type ();
#endif

#ifdef NMC
extern U32_t  CmType ();
extern S32_t  CmService ();
extern U32_t  CmBCType ();
extern U32_t  CmDc1Type ();
extern U32_t  CmDc2Type ();
extern BOOL_t CmRedundancy ();
extern RET_t  CmSetFailReason(U32_t reason);
extern S32_t  CmIfMode();
extern S32_t  CmSetRemCubitId(MSG_t *, MSG_t **);
extern S32_t  CmSetRemCIdAndSlot(MSG_t *, MSG_t **);
#endif
extern S32_t  CmPerformReverseDbSync(MSG_t *, MSG_t **);

extern RET_t CmSendMsg2SmMsgQ(S32_t message);
extern S8_t CmState ();
extern RET_t CmErrHandler(S32_t slot, S32_t eType, S32_t errNo, MSG_t *msg);
extern RET_t CmSetFailReason(U32_t reason);
extern U32_t CmType ();
extern S32_t CmService ();
extern U32_t CmBCType ();

#endif /* _CM_IF_H_ */
