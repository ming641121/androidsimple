#include "tss_types.h"

#ifndef _CLI_IF_H_
#define _CLI_IF_H_


#define AXSS_TYPE    0
#define SHELL_TYPE  1

#define NO_LOG      0
#define LOG         1

#define BOOT_EXC_STATE      2
#define ACTIVE_EXC_STATE    4
#define STANDBY_EXC_STATE   8
#define MISMATCH_EXC_STATE  0x10
#define FAIL_EXC_STATE      0x20
#define UNKNOWN_EXC_STATE   0x80
#define ALL_EXC_STATE       0xff


typedef STATUS_t (*CMDFUNC_t)(U8_t *, ...);

typedef struct {
    U8_t       *cmdStr;
    CMDFUNC_t  command;
    U8_t       cmdType;
    U8_t       level;
    U8_t       logFlag;
    U32_t      excState;
    U8_t       *helpMsg;
} CMDSTRUCT_t, *pCMDSTRUCT_t;

extern void AxssShell(S32_t inFd, S32_t outFd, S32_t exitflag, U32_t loginAcc);
extern STATUS_t commandPlugIn(pCMDSTRUCT_t cmdTblPtr);
extern void commandUnPlug(pCMDSTRUCT_t cmdTblPtr);
extern U8_t *ShellUserNameGet();
extern U32_t ShellAccessLvlGet();


#endif
