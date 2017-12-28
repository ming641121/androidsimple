/*
 *
 * Copyright (C) 2005 by Ahoya Networks Incorporated.
 *
 */
#include <in.h>
#include <net/inet.h>
#include <inetLib.h>
#include <telnetLib.h>

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
#include "signal.h"
#include "a_out.h"
#include "tss_if.h"
#include "cli_if.h"
#include "sys_os.h"
#include "clicmd_if.h"

/*
 *   Constant defined
 */

#define INBUF_SIZE     300
#define MAX_ARG_NUM    20

#define MAX_DSP_LINE     20

#define N_IMD       0x20
#define N_STR       0x40

/* #define __TRAPLOG__ */



extern SYMTAB_ID       sysSymTbl;              /* system symbol table id */
extern PART_ID         memSysPartId;

extern U8_t *nextOptArgGet(U8_t *argListPtr, U8_t *optPtr, U8_t *argPtr);
extern U8_t *strskip(U8_t *s1, U8_t skipCh);
extern U8_t *str2val(U8_t  *text_str, U32_t *val);

extern STATUS ptyDrv();

extern STATUS ptyDevCreate(char *name, int rdBufSize, int wrtBufSize);


typedef struct {
    U32_t status;
    U32_t axssTaskId;

    char  userName[11];
    U32_t accLvl;
} CONSOLE_SESS_INFO_t; 

CONSOLE_SESS_INFO_t consoleSessionInfo;    /* USER SESSION */

typedef struct {
         U32_t *addr;
         U32_t val;
         U8_t type;
} OPDATA_t, *pOPDATA_t;

void shellOutFlush(
    S32_t        masterFd,
    S32_t        slaveFd);

void shellCmdProc(U8_t *cmdStr, U8_t *argListPtr);
STATUS_t parseCmdArg(U8_t *argListPtr, pOPDATA_t result);
U8_t *parseArg(U8_t *argPtr, pOPDATA_t  result, int *status);
STATUS_t calcResult(U8_t ch, pOPDATA_t result, pOPDATA_t intData, U32_t assignFla);
STATUS_t validOp(U8_t ch, pOPDATA_t  result, pOPDATA_t  intData);
U8_t *argValGet(U8_t *argPtr, pOPDATA_t  result, int *status);
U8_t *argStrParse(U8_t *strPtr, pOPDATA_t result, U32_t *status);
U8_t *argStrGet(U8_t *argPtr, U8_t *buf, U32_t flag);

static void help();
void ttyredir();

extern STATUS_t SiwSemGiveForce(SIW_SEMID_t semId);



STATUS_t printLineCheck(
	U8_t *printBuf,
	U8_t *dataBuf,
	U32_t *lineCnt);


SYMTAB_ID       axssSymTblId = NULL;

SHELL_VAR_t     *shellVarPtr;
S32_t           accessLvl = 0;

U32_t           numOfCmd = 0;
S32_t           shellOutRedir = 0;

U32_t axssShellStackSize = 0x8000;
U32_t axssShellPriority = 62;
U32_t cmdShellPriority = 62;
U32_t consolePriority = 55;
U32_t axssShellOptions = VX_SUPERVISOR_MODE | VX_UNBREAKABLE;
U32_t menuSelected = 1;
S32_t        axssInFd;
S32_t        axssOutFd;

U8_t         auxTermName[50];


/* ------------------------------------------------------------
 * Name:         void axssShellInit()
 *                    
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
int axssShellInit()
{
    S32_t        masterFd;
    S32_t        slaveFd;

    initUserProfile();
    axssInFd = ioTaskStdGet(0, STD_IN);
    axssOutFd = ioTaskStdGet(0, STD_OUT);

    if (ptyDrv () == ERROR)
	{
	printErr ("axssShellInit: unable to initialize pty driver.\n");
	return(ERROR);
	}
    if (ptyDevCreate ("/pty/shellFlush.", 1024, 1024) == ERROR)
        {
        printErr ("axssShellInit: unable to create pty device\n");
        return(ERROR);
        }
    if ((masterFd = open ("/pty/shellFlush.M", O_RDWR, 0)) == ERROR)
    {
        printErr ("axssShellInit: error opening %s\n", "/pty/shellFlush0M");
        return(ERROR);
    }
    
    if ((slaveFd = open ("/pty/shellFlush.S", O_RDWR, 0)) == ERROR)
    {
        printErr ("shellOutFlush: error opening %s\n", "/pty/shellFlush0S");
        return(ERROR);
    }

    shellOrigStdSet (STD_IN, slaveFd);
    shellOrigStdSet (STD_OUT, slaveFd);
    shellOrigStdSet (STD_ERR, slaveFd);

    ioGlobalStdSet(STD_OUT, slaveFd);
    ioGlobalStdSet(STD_IN, slaveFd);
    ioGlobalStdSet(STD_ERR, slaveFd);

    excJobAdd (shellRestart, TRUE, 0, 0, 0, 0, 0);

    write(axssInFd, "\n", 1);

    ioctl(axssInFd, FIOOPTIONS, OPT_TERMINAL);

    strcpy(auxTermName, "tAuxTermd");
    if ((taskSpawn ( auxTermName, consolePriority,
                                  axssShellOptions,
                                  axssShellStackSize,
                                  (FUNCPTR)AxssShell,
                                  axssInFd, axssOutFd, 0,
                                  0, 0, 0, 0, 0, 0, 0)) == ERROR)
    {
        return(ERROR);
    }
/*
    shellOutRedir = axssOutFd;
*/
    if ((taskSpawn ( "tTermOut", consolePriority,
                                 axssShellOptions,
                                 0x800,
                                 (FUNCPTR)shellOutFlush,
                                 masterFd, slaveFd, 0,
                                 0, 0, 0, 0, 0, 0, 0)) == ERROR)
        return(ERROR);



    return(OK);


}

/* ------------------------------------------------------------
 * Name:         void shellOutFlush()
 *                    
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */

void
shellOutFlush(
    S32_t        masterFd,
    S32_t        slaveFd)
{
    U8_t         inBuf[INBUF_SIZE];
    U32_t        len;



    while(1)
    {
        len = read(masterFd, inBuf, INBUF_SIZE);
        if (shellOutRedir)
            write(shellOutRedir, inBuf, len);
    }

}


/* ------------------------------------------------------------
 * Name:         void AxssShell(S32_t inFd, S32_t outFd, S32_t exitflag)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
void
AxssShell(S32_t inFd, S32_t outFd, S32_t telnetFlag, U32_t sessionId)
{
    U8_t         argBuf[INBUF_SIZE];
    U8_t         *inPtr;
    U8_t         inBuf[INBUF_SIZE];
    U32_t        len;
    U8_t         *argListPtr;
    pCMDSTRUCT_t cmdRcdPtr;
    S8_t         cmdLvl;
    int          retVal;
    OPDATA_t     argArray[MAX_ARG_NUM];
    U8_t         *strPtr;
    U32_t        i;
    SHELL_VAR_t    shellVar;
    STATUS_t     rc;

    /* redirect the task std io */
    ioTaskStdSet(0, STD_IN, inFd);
    ioTaskStdSet(0, STD_OUT, outFd);
    ioTaskStdSet(0, STD_ERR, outFd);

    shellVarPtr = &shellVar;
    shellVarPtr->inLogin = 0;
    shellVarPtr->telnetFlag = telnetFlag;
    shellVarPtr->sessionId = sessionId;
    shellVarPtr->userName[0] = '\0';

    taskVarAdd(0, (U32_t *)&accessLvl);

    taskVarAdd(0, (U32_t *)&shellVarPtr);

    while (1)
    {
        if (!shellVarPtr->inLogin)
        {
            if (!menuSelected)
            {
               if (tl1Login(inFd) == FD_ERROR)
               {
                  if (shellVarPtr->telnetFlag)
                  {
                     return;
                  }
                  break;
               }
               shellVarPtr->inLogin = 1;
               if (accessLvl)
               {
                  if (tl1CmdsProc(inFd) == FD_ERROR)
                  {
                     shellVarPtr->inLogin = 0;
                     if (shellVarPtr->telnetFlag)
                     {
                        return;
                     }
                     break;
                  }
                  shellVarPtr->inLogin = 0;
                  if (shellVarPtr->telnetFlag)
                  {
                     telnetdExit(shellVarPtr->sessionId);
                     return;
                  }
                  continue;
               }
            }
            else
            {
               while ((rc = menuLogin(inFd)) != USER_LOGIN &&
                       rc != FD_ERROR)
                  ;
               if (rc == FD_ERROR)
               {
                  if (shellVarPtr->telnetFlag)
                  {
                     return;
                  }
                  break;
               }
               shellVarPtr->inLogin = 1;
               if (accessLvl)
               {
                  if (menuMain(inFd) == FD_ERROR)
                  {
                     if (shellVarPtr->telnetFlag)
                     {
                        return;
                     }
                     break;
                  }
                  shellVarPtr->inLogin = 0;
                  if (shellVarPtr->telnetFlag)
                  {
                     telnetdExit(shellVarPtr->sessionId);
                     return;
                  }
                  continue;
               }
            }
        }

        printf("->");
 
        len = fioRdString(inFd, inBuf, INBUF_SIZE);


        if ((S32_t)len <= 0)
	{
            if (shellVarPtr->telnetFlag)
            {
               return;
            }
            break;
	}
        if (len <= 1)
            continue;
        inBuf[len-1] = '\0';

        inPtr = strskip(inBuf, ' ');
        if (!inPtr)
           continue; 
        argListPtr = nextOptArgGet(inPtr, NULL, argBuf);


        shellCmdProc(argBuf, inPtr);
   }

}




/* ------------------------------------------------------------
 * Name:         void shellCmdProc(U8_t *cmdStr, 
 *                           U8_t *argListPtr)
 *                           
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
void
shellCmdProc(U8_t *cmdStr, 
             U8_t *argListPtr)
{
    OPDATA_t  argArray[MAX_ARG_NUM];
    int       status;
    void      *symValue;
    U8_t      symType;
    U32_t     retVal;
    int       i;

    if (isalpha(*argListPtr))
    {
        U8_t *str1;

        str1 = strpbrk(cmdStr, "+-*/%&|^!=() ,");
        if (str1)
            *str1 = '\0';

        if (symFindByName(sysSymTbl, cmdStr, (char **)&symValue,
            (SYM_TYPE *)&symType) == ERROR)
        {
            printf("undefined symbol: %s\n", cmdStr);
            return;
	}
    }
    else
        symType = N_DATA;

    switch (symType & N_TYPE)
    {
    case N_TEXT:
        for (i = 0; i < MAX_ARG_NUM; i++)
	{
            argArray[i].addr = NULL;
            argArray[i].val = 0;
            argArray[i].type = N_UNDF;
	}

        argListPtr = strskip(argListPtr, ' ');
        argListPtr = strpbrk(argListPtr, "+-*/%&|^!=() ,");
        if (argListPtr)
            argListPtr = strskip(argListPtr, ' ');

        if (argListPtr)
            if (parseCmdArg(argListPtr, argArray) == ERROR)
                return;

        retVal = ((CMDFUNC_t)symValue)((char *)argArray[0].val, (char *)argArray[1].val, 
            (char *)argArray[2].val, (char *)argArray[3].val, (char *)argArray[4].val, 
            (char *)argArray[5].val, (char *)argArray[6].val, (char *)argArray[7].val, 
            (char *)argArray[8].val, (char *)argArray[9].val, (char *)argArray[10].val);
        printf("value = %u = 0x%x\n", retVal, retVal);
        break;
    case N_ABS:
    case N_DATA:
    case N_BSS:
    case N_COMM:
        argArray[0].addr = NULL;
        argArray[0].val = 0;
        argArray[0].type = N_UNDF;
        status = OK;

        argListPtr = parseArg(strskip(argListPtr, ' '), &argArray[0], &status);
        if (status == ERROR)
            return;
        if (argListPtr)
	{
            printf("syntax error\n");
            return;
	}

        if (argArray[0].addr)
	{
            if (symFindByValue(sysSymTbl, (UINT)argArray[0].addr, cmdStr, 
                (U32_t *)&i, (SYM_TYPE *)&symType) == ERROR)
            {
                printf("0x%x: value = %u = 0x%x\n", (U32_t)argArray[0].addr,
                   (U32_t)argArray[0].val, argArray[0].val);
    	    }
            else if (i == (int)argArray[0].addr)
            {
                printf("%s = 0x%x: value = %u = 0x%x\n", cmdStr, 
                   (U32_t)argArray[0].addr, argArray[0].val, argArray[0].val);
	    }
            else
	    {
                printf("%s + 0x%x = 0x%x: value = %u = 0x%x\n", cmdStr, 
                   (U32_t)argArray[0].addr - i,
                   (U32_t)argArray[0].addr, (U32_t)argArray[0].val, argArray[0].val);
	    }
	}
        else
            printf("value = %u = 0x%x\n", argArray[0].val, argArray[0].val);
        break;
    }

}

/* ------------------------------------------------------------
 * Name:         STATUS_t parseCmdArg(U8_t *argListPtr, 
 *                           pOPDATA_t result)
 *                           
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
parseCmdArg(U8_t *argListPtr, 
            pOPDATA_t result)
{
    U8_t      *argPtr;
    int       status;

    argPtr = argListPtr;

    do
    {
        result->addr = NULL;
        result->val = 0;
        result->type = N_UNDF;
        status = OK;

        argPtr = parseArg(argPtr, result, &status);
        if (status == ERROR)
            return(ERROR);
        if (argPtr)
        { 
            if (*argPtr != ',')
                return(ERROR);
            argPtr = strskip(argPtr+1, ' ');
	}

        result++;
    } while (argPtr);
    return(OK);
}



/* ------------------------------------------------------------
 * Name:         U8_t *parseArg(U8_t       *argPtr,
 *                           pOPDATA_t  result,
 *                           int        *status)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *
parseArg(U8_t       *argPtr,
         pOPDATA_t  result,
         int        *status)
{
    U8_t      *strPtr;
    U8_t      ch;
    OPDATA_t  intData;


    intData.addr = NULL;
    intData.val = 0;
    intData.type = N_UNDF;
    /* get 1st value */
    if (!(strPtr = argValGet(argPtr, result, status)))
    {
        return(strPtr);
    }
    while (strPtr)
    {
        ch = *strPtr;
        if (ch == ',')
            return(strPtr);

        if (strchr("+-*/%^&|!=", ch))
	{
            if (*(strPtr+1) == '=')
	    {
                strPtr = parseArg(strPtr+2, &intData, status);
                if (*status == ERROR)
                    return(NULL);
                if (strPtr)
		{
		    *status = ERROR;
                    printf("bad assignment\n");
                    return(NULL);
		}
                *status = validOp('&', result, &intData);
                if (*status == ERROR)
                    return(NULL);
                if (ch == '!')
		{
                    result->val = (result->val != intData.val);
                    result->type = N_IMD;
                    result->addr = NULL;
		}
                else if (ch == '=')
		{
                    result->val = (result->val == intData.val);
                    result->type = N_IMD;
                    result->addr = NULL;
		}
                else
		{
                    calcResult(ch, result, &intData, TRUE);
                    if (result->addr)
                        *(result->addr) = result->val;
                    return(NULL);
		}
	    }
            else if (strchr("+-&|", *(strPtr+1)))
            {
                strPtr = parseArg(strPtr+2, &intData, status);
                if (*status == ERROR)
                    return(NULL);
                *status = validOp('&', result, &intData);
                if (*status == ERROR)
                    return(NULL);
                if (ch == '&')
                    result->val = (result->val && intData.val);
                else if (ch == '|')
                    result->val = (result->val || intData.val);
                else if (ch == '+')
                    result->val++;
                else
                    result->val--;
                result->addr = NULL;
            }
            else
	    {
                switch (ch)
	        {
	        case '+':
                case '-':
                    strPtr = parseArg(strPtr+1, &intData, status);
                    if (*status == ERROR)
                        return(NULL);
                    *status = calcResult(ch, result, &intData, FALSE);
                    if (*status == ERROR)
                        return(NULL);

                    break;
                case '*':
                case '/':
                case '%':
    	        case '^':
	        case '&':
	        case '|':
	        case '=':
                    strPtr = argValGet(strPtr+1, &intData, status);

                    if (*status == ERROR)
                        return(NULL);
                    *status = calcResult(ch, result, &intData, FALSE);
                    if (*status == ERROR)
                        return(NULL);
                    if (ch == '=' && result->addr)
                        *(result->addr) = result->val;
                    break;
	        case '!':
	            *status = ERROR;
                    printf("syntax error\n");
                    return(NULL);
		}
	    }
	}
        else if (ch == ')')
	{
            return(strPtr);
	}
        else
	{
	    *status = ERROR;
            printf("syntax error\n");
            return(NULL);

	}
    }
    return(strPtr);
}

/* ------------------------------------------------------------
 * Name:         STATUS_t calcResult(U8_t      ch, 
 *                           pOPDATA_t result, 
 *                           pOPDATA_t intData,
 *                           U32_t     assignFlag)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
calcResult(U8_t      ch, 
           pOPDATA_t result, 
           pOPDATA_t intData,
           U32_t     assignFlag)
{
    U8_t *str;

    if (validOp(ch, result, intData) == ERROR)
        return(ERROR);

    if (!assignFlag)
    {
        if (ch != '=')
	{
            result->addr = NULL;
            result->type = N_IMD;
	}
    }

    switch (ch)
    {
	case '+':
            if (result->type == N_STR)
	    {
                str = malloc(strlen((U8_t *)(result->val)) + 
                   strlen((U8_t *)(intData->val)) + 1);
                sprintf(str, "%s%s", (U8_t *)result->val, (U8_t *)intData->val);
                free((U8_t *)(result->val));
                free((U8_t *)(intData->val));
                result->val = (U32_t)str;
	    }
            else
                result->val += intData->val;
            break;
        case '-':
            result->val -= intData->val;
            break;
        case '*':
            result->val *= intData->val;
            break;
        case '/':
            result->val /= intData->val;
            break;
        case '%':
            result->val %= intData->val;
            break;
        case '&':
            result->val &= intData->val;
            break;
        case '|':
            result->val |= intData->val;
            break;
        case '^':
            result->val ^= intData->val;
            break;
        case '=':
            result->val = intData->val;
            break;
    }
    return(OK);
}

/* ------------------------------------------------------------
 * Name:         STATUS_t validOp(U8_t  ch, 
 *                           pOPDATA_t result, 
 *                           pOPDATA_t intData)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
validOp(U8_t      ch, 
        pOPDATA_t result, 
        pOPDATA_t intData)
{
    if (result->type == N_TEXT && ch == '=')
    {
        printf("illegal assignment.\n");
        if (intData->type == N_STR)
            free((U8_t *)(intData->val));
        return(ERROR);
        
    }
    if (result->type == N_STR || intData->type == N_STR)
    {
        if (result->type != intData->type)
	{
            if (result->type == N_STR)
	    {
                free((U8_t *)(result->val));
                printf("illegal string operation\n");
                return(ERROR);
	    }
            else if (ch != '=' || (result->type == N_IMD))
	    {
                free((U8_t *)(intData->val));
                printf("illegal string operation.\n");
                return(ERROR);
	    }
	}
        else if (ch != '+')
	{
            free((U8_t *)(result->val));
            free((U8_t *)(intData->val));
            printf("illegal string operation.\n");
            return(ERROR);
	}
    }
    return(OK);
}


/* ------------------------------------------------------------
 * Name:         U8_t *argValGet(U8_t *argPtr,
 *                           pOPDATA_t  result,
 *                           int       *status)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *
argValGet(U8_t *argPtr,
         pOPDATA_t  result,
         int        *status)
{
    U8_t  *strPtr;
    U8_t  ch;
    U32_t inx = 0;
    U32_t num[5];
    U32_t opch[5];
    U32_t *addr;

    strPtr = strskip(argPtr, ' ');
    ch = *strPtr;
    if (ch == '(')
    {
        strPtr = strskip(strPtr + 1, ' ');
        strPtr = parseArg(strPtr, result, status);
        if (*status == ERROR)
            return(NULL);

        if (!strPtr || *strPtr != ')')
	{
            *status = ERROR;
            printf("syntax error\n");
            return(NULL);
	}
        strPtr = strskip(strPtr + 1, ' ');
        return(strPtr);

    }
    if (ch == ')' || ch == ',')
    {
        *status = ERROR;
        printf("syntax error\n");
        return(NULL);
    }
    while (strchr("+-*&!", ch))
    {
        num[inx] = 1;
        opch[inx] = ch;
        
        while ((*strPtr == *(strPtr+1)))
	{
            num[inx]++;
            strPtr++;
	}
        if (((ch == '+' || ch == '-') && num[inx] > 2) ||
            ((ch == '!' || ch == '&') && num[inx] > 1))
	{
            *status = ERROR;
            printf("syntax error\n");
            return(NULL);
	}
        else if (inx >= 1)
	{
            if ((opch[inx] == '+' || opch[inx] == '-') &&
                (opch[inx-1] == '+' || opch[inx-1] == '-'))
	    {
                *status = ERROR;
                printf("syntax error\n");
                return(NULL);
	    }
	}
        inx++;

        strPtr++;
        strPtr = strskip(strPtr, ' ');
        if (*strPtr == '\0')
	{
            *status = ERROR;
            printf("syntax error\n");
            return(NULL);
	}
        if ((ch = *strPtr) == '(')
	{
            strPtr = strskip(strPtr + 1, ' ');
            strPtr = parseArg(strPtr, result, status);
            if (*status == ERROR)
                return(NULL);
            if (!strPtr || *strPtr != ')')
	    {
                *status = ERROR;
                printf("syntax error\n");
                return(NULL);
	    }
            strPtr = strskip(strPtr + 1, ' ');
            goto UNARY_OP;
	}
    }

    if (isdigit(*strPtr))
    {
        U8_t *str1;

        if ((str1 = strpbrk(strPtr, "+-*/%&|^!=() ,")))
	{
            ch = *str1;
            *str1 = '\0';
	}
        if (!(strPtr = str2val(strPtr, &(result->val))))
	{
            *status = ERROR;
            printf("invalid number: %s\n", strPtr);
            return(NULL);
	}
        if (str1)
	{
            *str1 = ch;
            strPtr = strskip(str1, ' ');
	}
        else
            strPtr = str1;
        result->type = N_IMD;
    }
    else if (*strPtr == '"')
    {
        if ((strPtr = argStrParse(strPtr, result, status)) == NULL)
            return(NULL);

    }
    else if (isalpha(*strPtr))
    {
        U8_t *str1;
        void *symValue;

        if ((str1 = strpbrk(strPtr, "+-*/%&|^!=() ,")))
	{
            ch = *str1;
            *str1 = '\0';
	}

        if (symFindByName(sysSymTbl, strPtr, (char **)&symValue, 
            (SYM_TYPE *)&(result->type)) == ERROR)
	{
            *status = ERROR;
            printf("undefined symbol: %s\n", strPtr);
            return(NULL);
	}
        if (str1)
	{
            *str1 = ch;
            strPtr = strskip(str1, ' ');
	}
        else
            strPtr = str1;
        result->addr = (U32_t *)symValue;
        switch (result->type & N_TYPE)
        {
        case N_TEXT:
            result->val = (U32_t)symValue;
            break;
        case N_ABS:
        case N_DATA:
        case N_BSS:
        case N_COMM:
            result->val = *(U32_t *)symValue;
            break;
        default:
            result->val = 0;
            break;
	}

    }
    else
    {
        *status = ERROR;
        printf("syntax error\n");
        return(NULL);
    }

UNARY_OP:
    while (inx)
    {
        inx--;
        switch (opch[inx])
	{
	case '+':
            if (num[inx] == 2)
                result->val++;
            break;
	case '-':
            if (num[inx] == 2)
                result->val--;
            else
                result->val = -result->val;
            break;
	case '!':
            result->val = !result->val;
            break;
        case '*':
            addr = (U32_t *)&(result->val);
            while (num[inx]--)
                addr = (U32_t *)*addr;
            result->addr = addr;
            result->val = *addr;
            break;
        case '&':
            if (!result->addr)
	    {
                *status = ERROR;
                printf("invalid application of 'address of' operator.\n");
                return(NULL);
	    }
            break;
	}
    }
    return(strPtr);
}    

/* ------------------------------------------------------------
 * Name:         U8_t *argStrParse(U8_t *strPtr, 
 *                           pOPDATA_t  result,
 *                           U32_t      *status)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *argStrParse(U8_t *strPtr, 
                pOPDATA_t  result,
                U32_t      *status)
{
    U8_t *str1;
    U8_t  tmpBuf[INBUF_SIZE];

    str1 = argStrGet(strPtr+1, tmpBuf, TRUE);
    if (!str1)
    {
        *status = ERROR;
        printf("invalid string: %s\n", strPtr);
        return(NULL);
    }
    if (!(result->val = (U32_t)malloc(strlen(tmpBuf)+1)))
    {
        *status = ERROR;
        printf("no memory!\n");
        return(NULL);
    }
    strcpy((U8_t *)result->val, tmpBuf);
    result->type = N_STR;
    if (*str1 == '\0')
        return(NULL);
    return(str1);
}

/* ------------------------------------------------------------
 * Name:         U8_t *argStrGet(U8_t *argPtr, 
 *                           U8_t *buf,
 *                           U32_t flag)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *
argStrGet(U8_t *argPtr, 
          U8_t *buf,
          U32_t flag)
{
    U8_t *strPtr;
    U8_t ch;

    strPtr = argPtr;
    while (strPtr)
    {
        ch = *buf = *strPtr++;
        if (ch == '\\')
	{
            ch = *strPtr++;

            switch (ch)
            {
              case 'a':
                *buf = '\a';
                break;
              case 'b':
                *buf = '\b';
                break;
              case 'f':
                *buf = '\f';
                break;
              case 'n':
                *buf = '\n';
                break;
              case 'r':
                *buf = '\r';
                break;
              case 't':
                *buf = ' ';
                break;
              case 'v':
                *buf = '\v';
                break;
              case '\\':
                break;
              case '"':
                *buf = '"';
                break;
              default:
                return(NULL);
            }
            strPtr++;
	}
        else if (ch == '"')
	{
            if (flag)
	    {
                *buf = '\0';
                while (*strPtr == ' ')
                    strPtr++;
                return(strPtr);
	    }
	}
        buf++;
        if (ch == '\0')
	{
            *buf = '\0';
            if (flag)
            {
                return(NULL);
            }
            else
                return(strPtr-1);
	}
    }
    return(NULL);
}


typedef struct {
    pCMDSTRUCT_t *val;
    U8_t   *match;
    U32_t  count;
} SYMNAMETBL_t, *pSYMNAMETBL_t;

/* ------------------------------------------------------------
 * Name:         static U32_t helpPrint(U8_t  *name,
 *                           U32_t    val,
 *                           SYM_TYPE type,
 *                           U32_t    level,
 *                           U16_t    group)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
static U32_t
helpPrint(U8_t     *name,
          U32_t    val,
          SYM_TYPE type,
          U32_t    symNameTbl,
          U16_t    group)
{
    if (type < accessLvl)
        return(TRUE);

    if (((pSYMNAMETBL_t)symNameTbl)->match)
    {
        if (!strstr(((pCMDSTRUCT_t)val)->cmdStr, ((pSYMNAMETBL_t)symNameTbl)->match))
	{
            if (!strstr(((pCMDSTRUCT_t)val)->helpMsg, ((pSYMNAMETBL_t)symNameTbl)->match))
                return(TRUE);
	}
    }


    ((pSYMNAMETBL_t)symNameTbl)->val[((pSYMNAMETBL_t)symNameTbl)->count] = (pCMDSTRUCT_t)val;
    ((pSYMNAMETBL_t)symNameTbl)->count++;
    return(TRUE);
}

int symcmp(U8_t *s1, U8_t *s2)
{
    return(strcmp((*(pCMDSTRUCT_t *)s2)->cmdStr, 
             (*(pCMDSTRUCT_t *)s1)->cmdStr));
}

/* ------------------------------------------------------------
 * Name:         static void help()
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */


static void help(U8_t *match)
{
    SYMNAMETBL_t symNameTbl;
    U32_t        i;
    U32_t        lineNum = 0;
    U8_t         buf[400];
    

    if (!axssSymTblId)
        return;

    if (match)
    {
        i = strlen(match);
        while (match[i-1] == ' ')
            i--;
        match[i] = '\0';
    }

    symNameTbl.val = (pCMDSTRUCT_t *)malloc(sizeof(pCMDSTRUCT_t) * numOfCmd + 20);
    symNameTbl.count = 0;
    symNameTbl.match = match;
    symEach(axssSymTblId, (FUNCPTR)helpPrint, (U32_t)&symNameTbl);
    qsort((void *)symNameTbl.val, symNameTbl.count, sizeof(pCMDSTRUCT_t), (FUNC_PTR_t)symcmp);
    for (i = symNameTbl.count; i; i--)
    {
        sprintf(buf, "%-18s%s\n", symNameTbl.val[i-1]->cmdStr, symNameTbl.val[i-1]->helpMsg);
        if (printLineCheck("%s", buf, &lineNum) == FALSE)
                break;
        printf("%s", buf);
    }
    free(symNameTbl.val);

}

/* ------------------------------------------------------------
 * Name:         static void axssShellRestart()
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
static void axssShellRestart()
{    
    taskSpawn ( auxTermName, axssShellPriority,
                                           axssShellOptions,
                                           axssShellStackSize,
                                           (FUNCPTR)AxssShell,
                                           axssInFd, axssOutFd, 
                                           shellVarPtr->telnetFlag,
                                           shellVarPtr->sessionId, 
                                           0, 0, 0, 0, 0, 0);    
    consoleSessionInfo.status = 0; /* USER SESSION */
}


/* ------------------------------------------------------------
 * Name:         static void axssShellRestart()
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
void axssShellExcRestart(U32_t accessLvl)
{

    taskDelete(taskNameToId(auxTermName));
    taskSpawn ( auxTermName, axssShellPriority,
                                           axssShellOptions,
                                           axssShellStackSize,
                                           (FUNCPTR)AxssShell,
                                           axssInFd, axssOutFd, 
                                           shellVarPtr->telnetFlag,
                                           shellVarPtr->sessionId, 
                                           0, 0, 0, 0, 0, 0);
    
    consoleSessionInfo.status = 0; /* USER SESSION */
}


void axssShellExc(U32_t conNum)
{
   if (taskIdSelf() == taskNameToId("tAuxTermd"))
        excJobAdd (axssShellExcRestart, accessLvl, 0, 0, 0, 0, 0);
}




/* ------------------------------------------------------------
 * Name:         static void ()
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
void ttyredir(U32_t flag)
{

    if (flag)
        shellOutRedir = ioTaskStdGet(0, STD_OUT);
    else
        shellOutRedir = 0;
}

void bye()
{
   shellVarPtr->inLogin = 0;
   if (shellVarPtr->telnetFlag)
   {
      telnetdExit(shellVarPtr->sessionId);
      taskDelete(taskIdSelf());
   }
}

/* ------------------------------------------------------------
 * Name:         STATUS_t printLineCheck(U8_t *printBuf,
 *                           U8_t *dataBuf,
 *                           U32_t *lineCnt)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
printLineCheck(U8_t *printBuf,
               U8_t *dataBuf,
               U32_t *lineCnt)
{
    U32_t lineNum = *lineCnt;

    if (lineNum >= MAX_DSP_LINE)
    {
        U32_t fd;
        U8_t  buf[251];
        S32_t len;

        printf("\n\nType <CR> to continue, Q<CR> to stop: ");
        fd = ioTaskStdGet(0, STD_IN);
        while ((len = fioRdString(fd, buf, 250)) != EOF)
	{
            if (buf[len-1] == EOS)
	    {
                if (toupper(buf[0]) == 'Q')
                    return(FALSE);
                else
                    break;
	    }
	}
        if (len == EOF)
            return(FALSE);
        lineNum = 0;
    }

    while (*printBuf)
    {
        if (*printBuf == '\n')
	{
           lineNum++;
	}
        printBuf++;
    }
    if (dataBuf)
    {
        while (*dataBuf)
        {
            if (*dataBuf == '\n')
	    {
               lineNum++;
	    }
            dataBuf++;
        }
    }
    *lineCnt = lineNum;
    return(TRUE);

}


U32_t ShellAccessLvlGet()
{
    return(accessLvl);
}


U8_t *ShellUserNameGet()
{
    return(shellVarPtr->userName);
}








STATUS_t telnetCmdParser(int telnetdEvent, U32_t sessionId, int ioFd)
{
   U8_t buf[100];
   U32_t tid;
   
   tid = taskIdSelf();

#if 0
printf("telnetCmdParser tid 0x%x telnetdEvent %u sessionId %u ioFd %u\n",
        tid, telnetdEvent, sessionId, ioFd);
#endif

   if (telnetdEvent == REMOTE_INIT)
   {
      printf("telnetCmdParser REMOTE_INIT\n");
      return(OK);
   }
   if (telnetdEvent == REMOTE_START)
   {
      printf("telnetCmdParser REMOTE_START\n");
      sprintf(buf, "tcmd%x", sessionId);
      if ((taskSpawn ( buf, cmdShellPriority,
                                  axssShellOptions,
                                  axssShellStackSize,
                                  (FUNCPTR)AxssShell,
                                  ioFd, ioFd, 1,
                                  sessionId, 0, 0, 0, 0, 0, 0)) == ERROR)
    {
        return(ERROR);
    }
      return(OK);
   }
   if (telnetdEvent == REMOTE_STOP)
   {
      ttyredir (0);
      return(OK);
   }
   return(OK);
}