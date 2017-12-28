/*
 *
 * Copyright (C) 2005 by Ahoya Networks Incorporated.
 *
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "ioLib.h"
#include "taskVarLib.h"
#include "shellLib.h"
#include "taskLib.h"
#include "tss_types.h"
#include "fioLib.h"
#include "ioLib.h"
#include "symLib.h"
#include "symbol.h"
#include "a_out.h"
#include "tss_if.h"
#include "cli_if.h"

#define INBUF_SIZE     300

extern STATUS_t commandPlugIn(pCMDSTRUCT_t);
extern U8_t *strskip(U8_t *s1, U8_t skipCh);
extern U8_t *nextOptArgGet(U8_t *argListPtr, U8_t *optPtr, U8_t *argPtr);
extern U8_t *str2val(U8_t  *text_str, U32_t *val);
extern U8_t *str2valnodot(U8_t  *text_str, U32_t *val);
extern U8_t *str2val64(U8_t  *text_str, U64_t *val);
extern STATUS_t SysFilterEvent(char *date, int slotNo, int trapType, int severity);
extern void TrapEventPrint();
extern void chpwd();
extern RET_t SccClearCfg();
extern STATUS_t EventClr();
extern void EventPrint(U8_t *date, U32_t pslot, U32_t severity);
extern RET_t FmConvertOutfileNameTo0Base(char *, char *);
extern STATUS FmTonePlanOutfileAction (UINT16 outfileIndex, char* outfileName);

STATUS_t printLineCheck(U8_t *printBuf, U8_t *dataBuf, U32_t *lineCnt);

#if (defined(SCC) && !defined(FLASH_BOOT)) /* CSCds15384 */
    static void lsevt(U8_t *);
#endif

STATUS_t clrcdcnf(U32_t one_based_cd);
STATUS_t chprotocol(U8_t *);
STATUS_t chsyslnmd(U8_t *);
STATUS_t onrambk(U8_t *inBuf);
STATUS_t addreds(U8_t *inBuf);
STATUS_t upgd(U8_t *inBuf);
STATUS_t upgdcmit(U8_t *inBuf);
STATUS_t upgdcancel(U8_t *inBuf);
STATUS_t dngd(U8_t *inBuf);
STATUS_t dngdcmit(U8_t *inBuf);
STATUS_t dngdcancel(U8_t *inBuf);
void lstraps();
extern STATUS_t TrapEventClr();
extern void SccResetNode();
STATUS_t resetcd(U8_t *);
STATUS_t resetnd();
STATUS_t deltoneplan(U8_t *);
STATUS_t viewtoneoutfile(U8_t *);
STATUS_t lstoneoutfiles();

#if (defined(SCC) && !defined(FLASH_BOOT)) /* CSCds15384 */
    static STATUS_t cliclrndcnf();
#endif

STATUS_t clrndcnf();
extern STATUS_t cnfkey (U8_t *key);
extern STATUS_t dspkey ();
extern STATUS_t updateflash (U32_t slot, U8_t *source);
STATUS_t updatefls (U8_t *bufs);
extern void dumpevt(char *outFile);
STATUS_t stopdisk(U8_t *);
STATUS_t dbbkup ();
STATUS_t dbrstr (U8_t *filename);
extern STATUS_t SetRoute (U8_t *ipaddr);
extern void version (void);
STATUS_t dumplog(char *outFile);

extern STATUS_t resetcd_(U8_t *cmdArg);
extern STATUS_t dbbkup_(U8_t *cmdArg);
extern STATUS_t dbrstr_(U8_t *cmdArg);
extern STATUS_t onrambk_(U8_t *cmdArg);
extern STATUS_t addreds_(U8_t *cmdArg);
extern STATUS_t upgd_(U8_t *cmdArg);
extern STATUS_t upgdcmit_(U8_t *cmdArg);
extern STATUS_t upgdcancel_(U8_t *cmdArg);
extern RET_t SmClearCardCfg(IN S32_t card);
extern STATUS_t dngdcmit_(U8_t *cmdArg);
extern STATUS_t dngdcancel_(U8_t *cmdArg);
extern STATUS_t chprotocol_(U8_t *cmdArg);
extern STATUS_t chsyslnmd_(U8_t *cmdArg);
extern STATUS_t deltoneplan_(U8_t *cmdArg);
extern BOOL isIPDCProtocolActive(RET_t *);
extern BOOL_t isDS0XConnExistOnCard(U32_t logicalSlot);


#if defined(SCC)
#if !defined(FLASH_BOOT)

/*
 * Call Log Support and Function Declarations
 */
#define LS_CALL_LOG_ARG_NUM  5   /* Max. 5 Filter input for call log display*/
#define LS_CALL_LOG_ARG_SIZE 80  /* Buffer length for the filter options */

RET_t lsCallLog(U8_t *);
STATUS_t clrCallLog(VOID);
VOID callLogPrint(SYS_FILTER_INFO_t *);
RET_t checkCallLogFilter(U32_t, /* number of filter data input */
                         U8_t *, U8_t *, U8_t *, U8_t *, U8_t *, /*Filter data*/
                         SYS_FILTER_INFO_t *);

#endif /* !defined(FLASH_BOOT) */
#endif /* defined(SCC) */


CMDSTRUCT_t mgx8260ShCmdTable[] = {
#if defined(SCC)
#if !defined(FLASH_BOOT) 
/*
 *----------  ----------------  ---------- ------ ------ -------------- -----------------------
 * Cmd Name     Cmd Function       Type     Level  Log                   Help Text
 *----------  ----------------  ---------- ------ ------ -------------- -----------------------
 */
 {"chpwd"     , (CMDFUNC_t)chpwd         ,   AXSS_TYPE,  6,   NO_LOG, ALL_EXC_STATE, "Change user password"}, 
 {"clrndcnf"  , (CMDFUNC_t)cliclrndcnf   ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Clear the entire node configuration"}, 
 {"clrcdcnf"  , (CMDFUNC_t)clrcdcnf      , SHELL_TYPE,  1,      LOG, ALL_EXC_STATE, "Clear a card's configuration (clrcdcnf <card num>)"},
 {"version"   , (CMDFUNC_t)version       , SHELL_TYPE,  6,      LOG, ALL_EXC_STATE, "Software version"},
 {"lstraps"   , (CMDFUNC_t)lstraps       ,   AXSS_TYPE,  6,   NO_LOG, ALL_EXC_STATE, "List the entire trap log"}, 
 {"clrtraps"  , (CMDFUNC_t)TrapEventClr  ,   AXSS_TYPE,  2,      LOG, ALL_EXC_STATE, "Clear the entire trap log"}, 
 {"clrevt"    , (CMDFUNC_t)EventClr      ,   AXSS_TYPE,  2,      LOG, ALL_EXC_STATE, "Clear event log"}, 
 {"lsevt"     , (CMDFUNC_t)lsevt         ,   AXSS_TYPE,  6,   NO_LOG, ALL_EXC_STATE, "List events in event log"}, 
 {"resetnd"   , (CMDFUNC_t)resetnd       , SHELL_TYPE,  2,      LOG, ALL_EXC_STATE, "Reset node, including all processor and service modules"}, 
 {"resetcd"   , (CMDFUNC_t)resetcd       ,   AXSS_TYPE,  2,      LOG, ALL_EXC_STATE, "resetcd <card num> - Reset a card with confirmation"}, 
 {"lskey"     , (CMDFUNC_t)dspkey        , SHELL_TYPE,  2,      LOG, ALL_EXC_STATE, "Display key"},  
 {"chkey"     , (CMDFUNC_t)cnfkey        ,   AXSS_TYPE,  2,      LOG, ALL_EXC_STATE, "Modify key (chkey 6-char-string)"},
 {"chgw"      , (CMDFUNC_t)SetRoute      ,   AXSS_TYPE,  2,      LOG, ALL_EXC_STATE, "Change default gateway address"},
 {"onrambk"   , (CMDFUNC_t)onrambk       ,   AXSS_TYPE,  2,      LOG, ALL_EXC_STATE, "Enable RAM Backup for the card with confirmation"},
 {"addreds"   , (CMDFUNC_t)addreds       ,   AXSS_TYPE,  2,      LOG, ALL_EXC_STATE, "Add card redundancy with confirmation"},
 {"stopdisk"  , (CMDFUNC_t)stopdisk      ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Stop hard disk with confirmation"}, 
 {"dbbkup"    ,(CMDFUNC_t)dbbkup        ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Back-up configuration file with confirmation"}, 
 {"dbrstr"    , (CMDFUNC_t)dbrstr        ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Restored configuration from a file with confirmation"}, 
 {"dumplog"   , (CMDFUNC_t)dumplog       ,   AXSS_TYPE,  1,   NO_LOG, ALL_EXC_STATE, "Dump all log information to a file"}, 
 {"chprotocol", (CMDFUNC_t)chprotocol    ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Change call server protocol with confirmation"},
 {"chsyslnmd" , (CMDFUNC_t)chsyslnmd     ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Change chassis DS1 mode with confirmation"},
 {"upgd"      , (CMDFUNC_t)upgd          ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Non-graceful and graceful upgrade with confirmation"},
 {"upgdcmit"  , (CMDFUNC_t)upgdcmit      ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Graceful upgrade commit with confirmation"},
 {"upgdcancel", (CMDFUNC_t)upgdcancel    ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Graceful upgrade cancel with confirmation"},
 {"dngd"      , (CMDFUNC_t)dngd          ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Non-graceful and graceful downgrade with confirmation"},
 {"dngdcmit"  , (CMDFUNC_t)dngdcmit      ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Graceful downgrade commit with confirmation"},
 {"dngdcancel", (CMDFUNC_t)dngdcancel    ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Graceful downgrade cancel with confirmation"},      
 {"lscalllog"     , (CMDFUNC_t)lsCallLog ,   AXSS_TYPE,  6,   NO_LOG, ALL_EXC_STATE, "List system call event log"},
 {"clrcalllog"    , (CMDFUNC_t)clrCallLog,   AXSS_TYPE,  2,      LOG, ALL_EXC_STATE, "Clear system call event log"},
 {"deltoneplan"    , (CMDFUNC_t)deltoneplan, AXSS_TYPE,  2,      LOG, ALL_EXC_STATE, "deltoneplan <tone plan ID> - Delete a tone plan with confirmation"},
 {"viewtoneoutfile",(CMDFUNC_t)viewtoneoutfile, AXSS_TYPE, 6, NO_LOG, ALL_EXC_STATE, "viewtoneoutfile <tonefile>.tone.out - display the output file generated by tone parser"},
 {"lstoneoutfiles",(CMDFUNC_t)lstoneoutfiles, AXSS_TYPE, 6, NO_LOG, ALL_EXC_STATE, "lstoneoutfiles - display all output files generated by tone parser"},
#endif 
 {"updatefls",  (CMDFUNC_t)updatefls     ,   AXSS_TYPE,  1,      LOG, ALL_EXC_STATE, "Update flash for a slot with a file"},
#endif
 {NULL        , NULL          ,          0,  0,        0}};


STATUS_t
mgx8260CmdPlugIn()
{
    return(commandPlugIn(mgx8260ShCmdTable));
}

/* ------------------------------------------------------------
 * Name:         STATUS_t AreYouSure(
 *                           U8_t *prompt,
 *                           U32_t *sureStr)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
AreYouSure(U8_t *prompt,
           U8_t *sureStr)
{
    U32_t len;
    U8_t  inBuf[INBUF_SIZE];
    U8_t  *inPtr;

    printf(prompt);
    len = fioRdString(ioTaskStdGet(0, STD_IN), inBuf, INBUF_SIZE);
    if ((S32_t)len <= 0)
    {
        return(ERROR);
    }
    if (len <= 1)
        return(ERROR);
    inBuf[len-1] = '\0';
    
    inPtr = strskip(inBuf, ' ');
    if (!inPtr)
        return(ERROR);

    len = 0; 
    while (inPtr[len])
    {
        inPtr[len] = toupper(inPtr[len]);
        len++;
    }
    if (!strncmp(inPtr, sureStr, strlen(sureStr)))
        return(OK);
    return(ERROR);
}


STATUS_t
resetcd(U8_t *inBuf)
{
    if (inBuf != NULL)
    {
        if (AreYouSure("Are you sure to reset the card (Y/N)? ", "Y") == ERROR)
            return(ERROR);
        return(resetcd_(inBuf));
    }
    printf("Usage: resetcd slot_number (physical slot number 1-16) \n");
    return(ERROR);     
}

STATUS_t
resetnd()
{
    if (AreYouSure("Are you sure to reset the entire node (Y/N)? ", "Y") == ERROR)
        return(ERROR);
    
    SccResetNode();        
    return(OK);
}

static STATUS_t
cliclrndcnf()
{
    if (AreYouSure("Are you sure to clear the node configuration and reset the node(Y/N)? ", "Y") == ERROR)
        return(ERROR);

     return (clrndcnf());
}
    

STATUS_t
clrndcnf()
{
    S32_t ret;

    ret = SccClearCfg();
    if (-1 == ret) {
        printf("====>WARNING!!!!! Communications to the Peer SCC Failed.\n");
        printf("                  You MUST Manually Clear Configuration on \n");
        printf("                  peer SCC and then reset the Node\n");
        return(ERROR);
    }
    if (-2 == ret) {
        printf("This command is not allowed in present state of SCC \n");
        return(ERROR);
    }
    SccResetNode();        
    return(OK);
}


STATUS_t
dbbkup()
{
    if (AreYouSure("Are you sure to backup current node configuration(Y/N)? ", "Y") == ERROR)
        return(ERROR);

     return (dbbkup_(""));
}



STATUS_t
clrcdcnf(U32_t one_based_cd)
{
    U8_t inBuf[10];

    
    if (AreYouSure("Are you sure to clear the card configuration and reset the card (Y/N)? ", "Y") == ERROR)
        return(ERROR);

    if ((one_based_cd >= 1) && (one_based_cd <= 16))
    {
        RET_t ret;

        /* if ds0Xconn on the slot, block this action */
        
        if (isDS0XConnExistOnCard(p2lSlotNum(one_based_cd - 1)))
        {
            printf("There is ds0XConn on the card, clrcdcnf is not allowed. \n");
            return (ERROR);
        }

        ret = SmClearCardCfg(one_based_cd-1);
        if (ret != SM_OK)
        {
            if (ret == SM_SNMP_ILLEGAL_SLOT_NO_FOR_THIS_ACTION)
                printf("Illegal command! It can not be used when a redundant card is exist for this card.\n"); 
            else printf("clrcdcnf failed\n");
            return(ERROR);
        }
    }
    else
    {
        printf("The slot number provided (%d) is out of range\n", one_based_cd);
        return(ERROR);
    }

    sprintf(inBuf, "%d", one_based_cd);
    return(resetcd_(inBuf));
}




void lstraps()
{
    SysFilterEvent(0,0,0,0);
    TrapEventPrint();
}

STATUS_t
dumplog(char *outFile)
{  

    if (outFile == NULL)
    {
        printf("Usage: dumplog filename \n");
        return(ERROR);
    }
    else
    {
        dumpevt(outFile);
        return(OK);
    }
}

#define LSEVT_ARG_NUM  3
#define LSEVT_ARG_SIZE 80
#define LSEVT_SEV_ALL  0xf

/* ------------------------------------------------------------
 * Name:         static void lsevt(U8_t *)
 *
 * Description:  This function prints out event logs saved in IDE drive.
 *               If no arguments are passed, lsevt shows all saved events.
 *               Arguments can be passed to select the events of interest.
 *
 *               Usage: lsevt [ date [ pslot [ severity ]]]
 *                 date      dd/mm/yyyy (or 0 for all)
 *                 pslot     physical slot number 1-16 (or 0 for all)
 *                 severity  bit mask: fatal=8|major=4|minor=2|info=1
 *             
 *               For example, to show all fatal and major saved events:
 *                 -> lsevt 0 0 0xc
 * In Parms:     
 * Out Parms:    N/A
 *
 * ------------------------------------------------------------
 */
static void
lsevt(U8_t *inBuf)
{
    U8_t   argBuf[LSEVT_ARG_NUM][LSEVT_ARG_SIZE];
    U8_t * date       = NULL;           /* include all dates */
    U32_t  pslot      = 0;              /* include all physical slots */
    U32_t  severity   = LSEVT_SEV_ALL;  /* include all severities */
    U32_t  itmp       = 0;
    int    ix;

    /* initialize argBuf[] */
    for (ix = 0; ix < LSEVT_ARG_NUM; ix ++)
    {
        *argBuf[ix] = '\0';
    }

    /* copy up to 3 argments into argBuf[] */
    for (ix = 0; (inBuf != NULL) && (ix < LSEVT_ARG_NUM); ix ++)
    {
        inBuf = nextOptArgGet(inBuf, NULL, argBuf[ix]);
    }

    /* process arguments and call EventPrint() */

    if (strlen(argBuf[0]) && (str2val(argBuf[0], &itmp) == NULL))
    {
        date = argBuf[0];
    }
    if (((date != NULL) && ((strlen(date) != DATESTAMP_SIZE) ||
        (date[2] != '/') || (date[5] != '/'))) ||
        ((date == NULL) && (itmp != 0)) ||
        (strlen(argBuf[1]) && (str2val(argBuf[1], &pslot) == NULL)) ||
        (strlen(argBuf[2]) && (str2val(argBuf[2], &severity) == NULL)))
    {
        printf("Usage: lsevt [ date [ pslot [ severity ]]]\n");
        printf("  date      dd/mm/yyyy (or 0 for all)\n");
        printf("  pslot     physical slot number 1-16 (or 0 for all)\n");
        printf("  severity  bit mask: fatal=8|major=4|minor=2|info=1\n");
    }
    else
    {
        EventPrint(date, pslot - 1, severity);
    }
}




/*
 * Call Logging CLI  Support
 */

/*
 * The following indices into the argument input buffer
 */
#define CALL_LOG_INPUT_DATE_INDEX        0
#define CALL_LOG_INPUT_SRCENDPT_INDEX    1
#define CALL_LOG_INPUT_DESTENDPT_INDEX   2

#define UPFLS_ARG_NUM  2
#define UPFLS_ARG_SIZE 50

/* ------------------------------------------------------------
 * Name:         STATUS_t updatefls(U8_t *)
 *
 * Description:  This function is used for udpate boot flush.
 *
 *               Usage: updatefls [ slotNum [ fileName ]]
 *                 slotNum     physical slot number 1-16
 *                 fileName    the file used to flush boot
 *             
 * In Parms:     
 * Out Parms:    N/A
 *
 * ------------------------------------------------------------
 */
STATUS_t updatefls (U8_t *inBuf)
{
    U8_t   argBuf[UPFLS_ARG_NUM][UPFLS_ARG_SIZE];
    int    ix;
    int    slotNum;
    char*  fileName = NULL;
    STATUS_t    retCode;

    if (inBuf == NULL)
    {
        printf("Usage: updatefls slotNum(1 - 16) fileName \n");
        return(ERROR);
    }

    /* initialize argBuf[] */
    for (ix = 0; ix < UPFLS_ARG_NUM; ix ++)
    {
        *argBuf[ix] = '\0';
    }

    /* copy 2 argments into argBuf[] */
    for (ix = 0; (inBuf != NULL) && (ix < UPFLS_ARG_NUM); ix ++)
    {
        inBuf = nextOptArgGet(inBuf, NULL, argBuf[ix]);
    }

    if (str2val(argBuf[0], &slotNum) == NULL)
    {
        printf("Usage: updatefls slotNum(1 - 16) fileName \n");
        return(ERROR);
    }

    if (slotNum < 1 || slotNum > 16)
    {
        printf("updatefls: invalid slot number \n");
        return(ERROR);
    }


    fileName = argBuf[1];

    if (strlen(fileName) == 0)
    {
        printf("Usage: updatefls slotNum(1 - 16) fileName \n");
        return(ERROR);
    }


    retCode = updateflash(slotNum - 1, fileName);

        printf("%s\n", getErrMsgFromNum(retCode)); 

        return(retCode);

}

