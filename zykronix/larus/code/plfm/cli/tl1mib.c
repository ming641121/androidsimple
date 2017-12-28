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
 *
 *
 ********************************************************************
 * $Source: /cvsroot/larus5820/code/plfm/cli/tl1mib.c,v $
 *********************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "ioLib.h"
#include "axss_types.h"
#include "axss_if.h"
#include "tss_types.h"
#include "sys_os.h"
#include "siw_if.h"
#include "clicmd_if.h"


/*
 *   Constant for screen limitation 
 */


#include "tl1_objs.h"
#include "tl1tbl.h"

extern SHELL_VAR_t     *shellVarPtr;

typedef STATUS_t (*PFUNC_t)(U8_t *, U8_t *);
typedef STATUS_t (*UPFUNC_t)(U32_t, MENU_DATA_VAL_t *);
typedef STATUS_t (*IFUNC_t)(TL1_ARG_t *argLst, U8_t *, MENU_DATA_VAL_t *);
typedef STATUS_t (*UFUNC_t)(U8_t *, U32_t *, MENU_DATA_VAL_t *);
typedef STATUS_t (*MUFUNC_t)(U32_t, U8_t *, MENU_DATA_VAL_t *);

typedef struct {
      void             *cFunc;
      U8_t             funcType;
      U8_t             cmdType;
      U8_t             snmpData;
      U16_t            argCnt;
      U8_t             argBuf[INBUF_SIZE];
      U8_t             tidBuf[INBUF_SIZE];
      U8_t             ctagBuf[INBUF_SIZE];
      MENU_DATA_VAL_t  dataVal;
} MENU_CMD_ARG_t;


typedef struct {
   TL1_FIELD_PARM_t  *fldListPtr;
   U32_t             ttlData;
   MENU_DATA_VAL_t   dataVal[10];
} TL1_PARSE_INFO_t;


/*
 *   Function prototype
 */


U8_t *tl1CmdNextArg(U8_t *argListPtr, U8_t *argPtr, U8_t *delimer, U8_t *seperator);

STATUS_t
tl1CmdsParse(U8_t              *argListPtr,
             pTL1_GROUP_t      tl1CmdGrp);

STATUS_t tl1FieldParse(U8_t              *argStr,
                     TL1_PARSE_INFO_t  *parseInf,
                     MENU_CMD_ARG_t    *cmdArg);
STATUS_t 
tl1DataFieldParse(U8_t              *argStr,
                  TL1_PARSE_INFO_t  *parseInf,
                  MENU_CMD_ARG_t    *cmdArg);
STATUS_t 
tl1DataParmParse(TL1_ARG_t        *argLst,
                 U8_t             *dataStr,
                 U32_t            *ttlData,
                 MENU_DATA_VAL_t  *inData,
                 U8_t             *ctagBuf);

STATUS_t tl1In2Val(TL1_ARG_t          *argLst,
                U8_t              *inBuf, 
                MENU_DATA_VAL_t   *inData);

STATUS_t tl1CnvArg(MENU_CMD_ARG_t    *cmdArg,
                    U32_t             ttlFld,
                    MENU_DATA_VAL_t   *dataVal,
                    U32_t             strPos);
STATUS_t tl1GetInput(U32_t inFd, U8_t *inBuf, U8_t *prevCmdBuf);

U8_t *
tl1CmdNextArg(U8_t *argListPtr, 
              U8_t *argPtr,
              U8_t *delimer,
              U8_t *seperator);
STATUS_t
tl1MapStr2Num(U8_t           *dataStr, 
             pCLIUMAPSTRUC_t mapLst,
             U32_t           *val);

/******************************************************
this area is used by taipei team for TL1 testing only
******************************************************/

STATUS_t set_aco_com_aid(U8_t *inputstring, U32_t *input1, MENU_DATA_VAL_t *input2){

if (strncmp(inputstring, "COM", 3) == 0){
		return OK;
		
	}else{
		return ERROR;
	}
}

STATUS_t opr_aco_all_aid(U8_t *inputstring, U32_t *input1, MENU_DATA_VAL_t *input2){

if (strncmp(inputstring, "ALL", 3) == 0){
		return OK;
		
	}else{
		return ERROR;
	}
}

tl1Test(argc,argv)
int argc;
char *argv[];
{
    U8_t inBuf[200];
    U8_t prevCmdBuf[200];
    U8_t cmdBuf[200];
    U8_t *str;
    U32_t i;
    U8_t  seperator;
    U32_t           orgOpt;
    U32_t inFd;

    inFd = 1;
   

    prevCmdBuf[0] = '\0';
#ifndef SIMUL
    orgOpt = ioctl(inFd, FIOGETOPTIONS, 0);
    ioctl(inFd, FIOOPTIONS, OPT_CRMOD);
#endif



    while (1)
    {
#ifdef SIMUL
        i = read(inFd, inBuf, 200);
        inBuf[i-1] = '\0';
#else
        if (FD_ERROR == tl1GetInput(inFd, inBuf, prevCmdBuf))
           return(FD_ERROR);
        if (inBuf[0])
	{
           strcpy(prevCmdBuf, inBuf);
	}
        else
           continue;
#endif
        if (!strmatch(inBuf, "lout"))
	{
#ifndef SIMUL
           ioctl(inFd, FIOOPTIONS, orgOpt);
#endif
           return;
	}

        str = tl1CmdNextArg(inBuf, cmdBuf, ":;", &seperator);
#ifdef DEBUG
printf("cmd %s\n", cmdBuf);
#endif
        for (i = 0; tl1CmdTable[i]; i++)
	{
            if (!strmatch(tl1CmdTable[i]->name, cmdBuf))
            {
               if (tl1CmdsParse(str, tl1CmdTable[i]) == USER_LOGOUT)
               {
                   return;
               }
               break;
	    }
	}
        if (tl1CmdTable[i] == NULL)
	{
          tl1PrintErr("", "ICNV");
	}
    }
}


STATUS_t
tl1CmdsProc(U32_t inFd)
{
    U8_t inBuf[200];
    U8_t prevCmdBuf[200];
    U8_t cmdBuf[200];
    U8_t *str;
    U32_t i;
    U8_t  seperator;
    U32_t           orgOpt;
    U32_t rc;

    inFd = 1;
   
    initUserProfile();
    prevCmdBuf[0] = '\0';

    orgOpt = ioctl(inFd, FIOGETOPTIONS, 0);
    ioctl(inFd, FIOOPTIONS, OPT_CRMOD);

    while (1)
    {
        if (FD_ERROR == tl1GetInput(inFd, inBuf, prevCmdBuf))
           return(FD_ERROR);
        if (inBuf[0])
	{
           strcpy(prevCmdBuf, inBuf);
	}
        else
           continue;
        
        str = tl1CmdNextArg(inBuf, cmdBuf, ":;", &seperator);
#ifdef DEBUG
printf("cmd %s\n", cmdBuf);
#endif
        for (i = 0; tl1CmdTable[i]; i++)
	{
            if (!strmatch(tl1CmdTable[i]->name, cmdBuf))
            {
               rc = tl1CmdsParse(str, tl1CmdTable[i]);
               if (rc == USER_LOGOUT || rc == FD_ERROR)
               {
                  ioctl(inFd, FIOOPTIONS, orgOpt);
                  return(rc);
               }
               break;
	    }
	}
        if (tl1CmdTable[i] == NULL)
	{
          tl1PrintErr("", "ICNV");
	}
    }
}

STATUS_t 
tl1Login(U32_t inFd)
{
    U8_t inBuf[200];
    U8_t prevCmdBuf[200];
    U8_t cmdBuf[200];
    U8_t *str;
    U8_t  seperator;
    U32_t           orgOpt;
    U32_t           rc;

    inFd = 1;
   
    initUserProfile();
    prevCmdBuf[0] = '\0';

    orgOpt = ioctl(inFd, FIOGETOPTIONS, 0);
    ioctl(inFd, FIOOPTIONS, OPT_CRMOD);

    while (1)
    {
        if (FD_ERROR == tl1GetInput(inFd, inBuf, prevCmdBuf))
           return(FD_ERROR);
        if (inBuf[0])
	{
           strcpy(prevCmdBuf, inBuf);
	}
        else
           continue;
        
        str = tl1CmdNextArg(inBuf, cmdBuf, ":;", &seperator);
#ifdef DEBUG
printf("cmd %s\n", cmdBuf);
#endif
        if (!strmatch(act_user_cmd.name, cmdBuf))
        {
           rc = tl1CmdsParse(str, &act_user_cmd);
           if (rc != ERROR)
	   {
              ioctl(inFd, FIOOPTIONS, orgOpt);
              return(rc);
	   }
	}
        else
           tl1PrintErr("", "PLNA");
    }
}

STATUS_t tl1GetInput(U32_t inFd, U8_t *inBuf, U8_t *prevCmdBuf)
{
   U32_t inChInx = 0;
   U8_t  inpCh;
   int   len;

   while (1)
   {
      len = read(inFd, &inpCh, 1);
      if (len <= 0)
         return(FD_ERROR);
      if (inpCh == CTRL_U || inpCh == CTRL_C)
      /* command abort */
      {
         inBuf[0] = '\0';
         inChInx = 0;
         printf("\n");
         continue;
      }
      else if (inpCh == CTRL_R)
      /* command line retype */
      {
         if (!inChInx)
	 {
            if (prevCmdBuf[0])
	    {
               strcpy(inBuf, prevCmdBuf);
               inChInx = strlen(prevCmdBuf);
	    }
            printf("\n%s", inBuf);
	 }
         continue;
      }
      else if (inpCh == BK_SPACE)
      {
         if (inChInx)
         {
            inChInx--;
            inBuf[inChInx] = '\0';
            /* erase the character */
            printf("%c %c", BK_SPACE, BK_SPACE);
         }
         continue;
      }
      else if (inpCh == ';' || inpCh == ESC_CHAR)
      {
         inBuf[inChInx] = '\0';
         if (inpCh == ';')
	 {
            printf(";");
	 }
         printf("\n");
         return(OK);
      }
      else if(isprint(inpCh))
      {
         inBuf[inChInx++] = inpCh;
         printf("%c", inpCh);
      }
   }
   return(OK);
}



/* 
   ';'  end of line
   'esc'  end of line
   'ctrl-c'  
*/
/* ------------------------------------------------------------
 * Name:         STATUS_t tl1CmdsParse(U8_t *argListPtr)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
tl1CmdsParse(U8_t              *argListPtr,
             pTL1_GROUP_t      tl1CmdGrp)
{
   TL1_PARSE_INFO_t  parseInf;
   MENU_CMD_ARG_t    cmdArg;
   U32_t             fld;
   STATUS_t          rc;
   U8_t              seperator = '\0';
   TL1_DEFAULT_ARG_t *dftData;

   cmdArg.funcType = TL1_FUNC_PRC;
   cmdArg.cmdType = tl1CmdGrp->cmdType;
   cmdArg.snmpData = tl1CmdGrp->snmpData;
   cmdArg.cFunc = tl1CmdGrp->procFunc;
   parseInf.fldListPtr = tl1CmdGrp->fieldList;
   parseInf.ttlData = 0;
   shellVarPtr->ctagBuf[0] = '\0';
   cmdArg.ctagBuf[0] = '\0';

   dftData = tl1CmdGrp->deftData;
   if (dftData)
   {
      while (dftData->data)
      {
         parseInf.dataVal[dftData->fldPos-1].type = dftData->inType;
         parseInf.dataVal[dftData->fldPos-1].ttlVal = 1;
         if (dftData->inType != TL1_IN_STR)
            parseInf.dataVal[dftData->fldPos-1].u.val[0] = (U32_t)dftData->data;
         else
            strcpy(parseInf.dataVal[dftData->fldPos-1].u.str, (U8_t *)dftData->data);
         parseInf.ttlData++;
         dftData++;
      }
   }
   for (fld = 0; fld < tl1CmdGrp->numOfFld; fld++, parseInf.fldListPtr++)
   {
      if (seperator == ';')
      /* last field processed */
         break;
      if (!argListPtr)
      {
          break;
      }
      argListPtr = tl1CmdNextArg(argListPtr, cmdArg.argBuf, ":;", &seperator);

      rc = tl1FieldParse(cmdArg.argBuf, &parseInf, &cmdArg); 
      if (rc == ERROR)
         return(ERROR);
   }
   for (; fld < tl1CmdGrp->numOfFld; fld++, parseInf.fldListPtr++)
   {
      cmdArg.argBuf[0] = '\0';

      rc = tl1FieldParse(cmdArg.argBuf, &parseInf, &cmdArg); 
      if (rc == ERROR)
         return(ERROR);
   }


   /*
    * call to process command
    */
   if (cmdArg.snmpData)
   {
      rc = tl1CnvArg(&cmdArg, parseInf.ttlData, parseInf.dataVal, 0);
      if (rc != ERROR)
      {
         if (cmdArg.cmdType == TL1_CFG_CMD)
         {
            printf("M <%s> COMPLD\n\"<command-specific information>\"\n;\n",
                   cmdArg.ctagBuf);
	 }
         else
	 {
            printf("M <%s> RTRV\n\"<command-specific information>\"\n;\n",
                   cmdArg.ctagBuf);

	 }
      }
      return(rc);
   }
   else
   {
#if 0
      printf("UF %s %u\n", (U8_t *)cmdArg.cFunc, parseInf.ttlData);
      return(OK);
#else

      return(((UPFUNC_t)cmdArg.cFunc)(parseInf.ttlData, parseInf.dataVal));
#endif
   }
}
/* ------------------------------------------------------------
 * Name:    STATUS_t tl1FieldParse(*cmdArg, 
 *                                 TL1_FIELD_PARM_t  *fldPtr, 
 *                                 U8_t              **argPtr, 
 *                                 U32_t             *ttlFld, 
 *                                 MENU_DATA_VAL_t   *dataVal)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t tl1FieldParse(U8_t              *argStr,
                     TL1_PARSE_INFO_t  *parseInf,
                     MENU_CMD_ARG_t    *cmdArg)
{
   TL1_FIELD_PARM_t  *fldPtr;

   fldPtr = parseInf->fldListPtr;
#ifdef DEBUG
printf("tl1FieldParse type %u flags %u argStr %s\n", fldPtr->type,
fldPtr->flags, argStr);
#endif
   if (fldPtr->flags & TL1_FLAG_MANDATORY)
   {
      if (argStr[0] == '\0')
      {
         tl1PrintErr(cmdArg->ctagBuf, "IPMS");
         return(ERROR);
      }
   }
   switch (fldPtr->type)
   {
   case ARG_NULL:
      if (argStr[0] != '\0')
      {
         tl1PrintErr(cmdArg->ctagBuf, "INUP");
         return(ERROR);
      }
      break;
   case ARG_TID:
      strcpy(cmdArg->tidBuf, argStr);
      break;
   case ARG_CTAG:
      strcpy(cmdArg->ctagBuf, argStr);
      strcpy(shellVarPtr->ctagBuf, argStr);
      break;
   case ARG_SID:
   case ARG_PID:
   case ARG_AID:
   case ARG_DATA:	   		
      return(tl1DataFieldParse(argStr, parseInf, cmdArg));
      break;		   		
   default:
      tl1PrintErr(cmdArg->ctagBuf, "IIPG");
      return(ERROR);
   }
   return(OK);
}



STATUS_t 
tl1DataFieldParse(U8_t              *argStr,
                  TL1_PARSE_INFO_t  *parseInf,
                  MENU_CMD_ARG_t    *cmdArg)
{
   TL1_FIELD_PARM_t *fldPtr;
   TL1_ARG_t        *argLst;
   U8_t             dataStr[100];
   U8_t             valStr[100];
   U8_t             *str;
   U8_t             seperator;
   TL1_SUBCMD_GROUP_t *scmdLst;
   U32_t            parm;
   U32_t            found = 0;
   U32_t            i;

   fldPtr = parseInf->fldListPtr;
#ifdef DEBUG
printf("tl1DataFieldParse argType %u\n", fldPtr->argType);
#endif
   switch (fldPtr->argType)
   {
      case ARG_ULIST:
         if ((fldPtr->flags & TL1_FLEX_FORMAT))
         {
            while (argStr)
	    {
               /* get arg data */
               argStr = tl1CmdNextArg(argStr, dataStr, ",", &seperator);
               str = tl1CmdNextArg(dataStr, valStr, "=", &seperator);
               if (seperator != '=' || !str)
	       {
                  tl1PrintErr(cmdArg->ctagBuf, "IIAC");
                  return(ERROR);
	       }
               argLst = (TL1_ARG_t *)fldPtr->parmLst;
               for (parm = 0; parm < fldPtr->ttlData; parm++, argLst++)
               {
                  if (argLst->argStr && !strmatch(argLst->argStr, valStr))
	          {
                     if (tl1DataParmParse(argLst, str, &parseInf->ttlData, parseInf->dataVal, cmdArg->ctagBuf) == ERROR)
                        return(ERROR);
                     break;
	          }
               }
	    }
      
         }
         else
	 {
            argLst = (TL1_ARG_t *)fldPtr->parmLst;
            for (parm = 0; parm < fldPtr->ttlData; parm++, argLst++)
            {
               /* get arg data */
               if (argStr)
                  argStr = tl1CmdNextArg(argStr, dataStr, ",", &seperator);
               else
                  dataStr[0] = '\0';
#ifdef DEBUG
printf("argStr %x dataStr %s\n", argStr, dataStr);
#endif
               if (tl1DataParmParse(argLst, dataStr, &parseInf->ttlData, parseInf->dataVal, cmdArg->ctagBuf) == ERROR)
                 return(ERROR);
	    }
	 }
         break;
            
      case ARG_UFUNC:
         return(((UFUNC_t)(fldPtr->parmLst))(argStr, &parseInf->ttlData, parseInf->dataVal));
      case ARG_SUBCMD_LIST:
         scmdLst = (TL1_SUBCMD_GROUP_t *)fldPtr->parmLst;
         for (parm = 0; parm < fldPtr->ttlData; parm++, scmdLst++)
	 {
            for (i = 0; i < scmdLst->numSubKey; i++)
	    {
               if (scmdLst->subKey[i].matchLen)
	       /* length match  */
	       {
                  if (!strnmatch(argStr, scmdLst->subKey[i].name, scmdLst->subKey[i].matchLen))
                  {
                      found = TRUE;
                      break;
		  }
	       }
               else
	       /* full string match  */
	       {
                  if (!strmatch(argStr, scmdLst->subKey[i].name))
                  {
                      found = TRUE;
                      break;
		  }
	       }
	    }
            if (found)
               break;
	 }
         if (!found)
	 {
            tl1PrintErr(cmdArg->ctagBuf, "IICT");
            return(ERROR);
	 }
         cmdArg->cmdType = scmdLst->cmdType;
         cmdArg->snmpData = scmdLst->snmpData;
         cmdArg->cFunc = scmdLst->procFunc;
         if (scmdLst->fieldList)
	 {
            parseInf->fldListPtr = scmdLst->fieldList;
            return(tl1FieldParse(argStr, parseInf, cmdArg));
	 }
         return(OK);
   }
   return(OK);         
}



STATUS_t 
tl1DataParmParse(TL1_ARG_t        *argLst,
                 U8_t             *dataStr,
                 U32_t            *ttlData,
                 MENU_DATA_VAL_t  *inData,
                 U8_t             *ctagBuf)
{
   U32_t pos;   
   U32_t i;   
   U32_t allSet = 0; 
   U32_t val;
   U8_t  inpCh;

 
   pos = argLst->fldPos-1;
   inData[pos].ttlVal = 0;
#ifdef DEBUG
printf("tl1DataParmParse data %s pos %u mapType %u\n",
        dataStr, pos, argLst->mapType);
#endif
   if ((argLst->flags & TL1_FLAG_IGNORE))
   {
      return(OK);
   }
   if (dataStr[0] == '\0')
   {
      if ((argLst->flags & TL1_FLAG_MANDATORY))
      {
         tl1PrintErr(ctagBuf, "IDNV");
         return(ERROR);
      }
   
      if ((argLst->flags & TL1_FLAG_NULL_ALL))
      {
         allSet = TRUE;
   
      }
      else if ((argLst->flags & TL1_FLAG_NULL_DATA))
      {
         if (argLst->inType == TL1_IN_STR)
         {
            strcpy(inData[pos].u.str, argLst->argStr);
            inData[pos].type = TL1_IN_STR;
            inData[pos].ttlVal = 1;
            (*ttlData)++;
            return(OK);
         }
         else if (argLst->inType == TL1_IN_INT ||
                  argLst->inType == TL1_IN_NUM)
         {
            inData[pos].type = TL1_IN_INT;
            inData[pos].ttlVal = 1;
            inData[pos].u.val[0] = (int)argLst->argStr;
            (*ttlData)++;
            return(OK);
         }
      }
      else
         return(OK);
   }
   else 
   {
      if (!strmatch(dataStr, "all"))
      {
         if ((argLst->flags & TL1_ALL_ALLOWED))
         {
            allSet = TRUE;
         }
         else
	 {
            tl1PrintErr(ctagBuf, "IDNC");
            return(ERROR);
	 }
      }
   }
   
   switch (argLst->mapType)
   {
   case TL1_MAP_MAPCNV:
   case TL1_MAP_MAPCNV_BITMAP:
      if (!allSet)
      {
         if (!(argLst->flags & TL1_MULTI_ALLOWED))
	 {
            if (tl1MapStr2Num(dataStr, argLst->mapFunc, &val) == ERROR)
	    {
               tl1PrintErr(ctagBuf, "IDRG");
               return(ERROR);
	    }

            inData[pos].u.val[0] = val;
            inData[pos].ttlVal = 1;
            inData[pos].type = TL1_IN_NUM;
            (*ttlData)++;
           
	 }
         else
	 {
            if (tl1In2Val(argLst, dataStr, inData) == ERROR)
	    {
               tl1PrintErr(ctagBuf, "IDRG");
               return(ERROR);
	    }
            (*ttlData)++;
	 }            

      }
      else
      {
         pCLIUMAPSTRUC_t mapLst;

         mapLst = ((pCLIUMAPSTRUC_t)(argLst->mapFunc));
         if (argLst->mapType == TL1_MAP_MAPCNV)
	 {
            inData[pos].type = TL1_IN_NUM;
            i = 0;
	 }
         else
	 {
            inData[pos].type = TL1_IN_BITS;
            inData[pos].u.val[0] = 0;
            i = 1;
	 }
         while (mapLst->mapStr)
	 {
            if (argLst->mapType == TL1_MAP_MAPCNV)
	    {
               inData[pos].u.val[i++] = mapLst->val;
	    }	       	  
            else 
               inData[pos].u.val[0] |= (1 << mapLst->val);
            mapLst++;
	 }
         inData[pos].ttlVal = i;
         (*ttlData)++;
      }
      return(OK);   
   case TL1_MAP_MAPCNV_FUNC:
      if (!allSet)
      {
         if (!(argLst->flags & TL1_MULTI_ALLOWED))
	 {
            if (((MUFUNC_t)argLst->mapFunc)(0, dataStr, &inData[pos]) == ERROR)
	    {
               tl1PrintErr(ctagBuf, "IDRG");
               return(ERROR);
	    }

            inData[pos].ttlVal = 1;
            inData[pos].type = TL1_IN_NUM;
            (*ttlData)++;
	 }
         else
	 {
            if (tl1In2Val(argLst, dataStr, inData) == ERROR)
	    {
               tl1PrintErr(ctagBuf, "IDRG");
               return(ERROR);
	    }
            (*ttlData)++;
	 }            

      }
      else
      {
         if (((MUFUNC_t)argLst->mapFunc)(1, dataStr, &inData[pos]) == ERROR)
            return(ERROR);
         (*ttlData)++;
      }
      return(OK);   
   case TL1_MAP_MAPFUNC:
      ((UFUNC_t)argLst->mapFunc)(dataStr, ttlData, inData);
      return(OK);

   case TL1_MAP_NOMAP:
   if (argLst->inType == TL1_IN_STR)
   {
      for (i = 0; (inpCh = dataStr[i]); i++)
      {
         inData[pos].u.str[i] = inpCh;
      }
      inData[pos].type = TL1_IN_STR;
      inData[pos].u.str[i] = '\0';
      inData[pos].ttlVal = 1;
      (*ttlData)++;
      return(OK);
   }
   else if (argLst->inType == TL1_IN_INT ||
            argLst->inType == TL1_IN_NUM)
   {
      inData[pos].type = TL1_IN_INT;
      inData[pos].ttlVal = 1;
      inData[pos].u.val[0] = atoi(dataStr);
      (*ttlData)++;
      return(OK);
   }
   return(ERROR);
   }
   return(OK);
}


STATUS_t tl1In2Val(TL1_ARG_t          *argLst,
                U8_t              *inBuf, 
                MENU_DATA_VAL_t   *inData)
{
   U32_t    inRangeMode = 0;
   U32_t    lastVal;
   U32_t    fstDigit = TRUE;
   U32_t    i, j;
   U8_t     inpCh;
   U32_t    val;
   U32_t    bitMap, bits;
   U32_t    fldPos;
   U32_t    start, dif;
   U8_t     valBuf[100];
   MENU_DATA_VAL_t   dataVal;

   if (argLst->mapType != TL1_MAP_MAPCNV_FUNC)
      lastVal = ((pCLIUMAPSTRUC_t)(argLst->mapFunc))->val;

   if (argLst->fldPos)
      fldPos = argLst->fldPos - 1;
   else
      fldPos = 0;

#ifdef DEBUG
printf("in menuIn2Val fldPos %u\n", fldPos);
#endif
   inData[fldPos].type = TL1_IN_NUM;
   inData[fldPos].ttlVal = 0;
   inData[fldPos].u.val[0] = 0;


   bitMap = 0;
   if ((inpCh = *inBuf) == '&')
   {
      inBuf++;
   }

   while (inBuf && *inBuf)
   {
      if (inpCh == '&' && *inBuf == '&')
      {
         if (argLst->mapType != TL1_MAP_MAPCNV_FUNC)
	 {
            inRangeMode = TRUE;
            inBuf++;
	 }
         else
	 {
            return(ERROR);
	 }
      }
   
      inBuf = tl1CmdNextArg(inBuf, valBuf, "&", &inpCh);
      if (valBuf[0] == '\0')
         continue;
      
      if (argLst->mapType == TL1_MAP_MAPCNV_FUNC)
      {
         if (((MUFUNC_t)argLst->mapFunc)(0, valBuf, &dataVal) == ERROR)
         {
            return(ERROR);
         }
         val = dataVal.u.val[0];
      }
      else if (tl1MapStr2Num(valBuf, argLst->mapFunc, &val) == ERROR)
      {
         return(ERROR);
      }
#ifdef DEBUG
printf("valBuf %s val %u\n", valBuf, val);
#endif

      if (inRangeMode)
      {
         if (val > lastVal)
         {

            dif = val - lastVal;
            start = lastVal;
         }
         else if (val < lastVal)
         {
            dif = lastVal - val;
            start = val;
	 }

         for (j = 0, bits = 0; j <= dif; j++)
         {
            bits <<= 1;
            bits |= 1;
	 }
         bits <<= start;
         bitMap |= bits;
         inRangeMode = 0;
      }
      else
      {
         bitMap |= (1 << val);
      }

      fstDigit = FALSE;
      lastVal = val;
   }
   if (inRangeMode)
   {
      pCLIUMAPSTRUC_t mapLst;

      mapLst = ((pCLIUMAPSTRUC_t)(argLst->mapFunc));
      if (fstDigit)
      {
         while (mapLst->mapStr)
	 {
            bitMap |= (1 << mapLst->val);
            mapLst++;
	 }
      }
      else
      {
         while (mapLst->mapStr && mapLst->val != lastVal)
	 {
             mapLst++;
	 }
         while (mapLst->mapStr)
	 {
            bitMap |= (1 << mapLst->val);
            mapLst++;
	 }
      }
   }

   switch (argLst->mapType)
   {
   case TL1_MAP_MAPCNV_BITMAP:
      inData[fldPos].type = TL1_IN_BITS;
      inData[fldPos].u.val[0] = bitMap;
      inData[fldPos].ttlVal = 1;
      break;
   default:
      for (i = 0, j = 0, bits = 1; i < 32; i++, bits <<= 1)
      {
         if ((bitMap & bits))
         {
            inData[fldPos].u.val[j++] = i;
         }
      }
      inData[fldPos].ttlVal = j;
      break;
   }
   return(OK);
}

/* ------------------------------------------------------------
 * Name:         void tl1CnvArg(MENU_DATA_ENTRY_t *pDtbl, 
 *                               U8_t              *inBuf,
 *                               MENU_DATA_VAL_t   *inData)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t tl1CnvArg(MENU_CMD_ARG_t    *cmdArg,
                    U32_t             ttlFld,
                    MENU_DATA_VAL_t   *dataVal,
                    U32_t             strPos)
{
   U32_t i, j;

   if (dataVal->ttlVal == 0)
   {
      sprintf(&(cmdArg->argBuf[strPos]), "# ");
      if (ttlFld > 1)
      {
         return(tl1CnvArg(cmdArg, ttlFld-1, dataVal+1, 
                 strlen(cmdArg->argBuf)));
       
      }
      if (cmdArg->funcType == TL1_FUNC_PRC)
      {
         if (cmdArg->cmdType == TL1_CFG_CMD)
	 {
#ifdef SIMUL
            printf("CF %s %s\n", (U8_t *)cmdArg->cFunc, cmdArg->argBuf);
            return(OK);
#else
            return(((PFUNC_t)cmdArg->cFunc)(cmdArg->argBuf, NULL));
#endif
	 }
         else
         {
#ifdef SIMUL
            printf("SH %s %s\n", (U8_t *)cmdArg->cFunc, cmdArg->argBuf);
            return(OK);
#else
            return(((PFUNC_t)cmdArg->cFunc)(cmdArg->argBuf, NULL));
#endif
         }
      }
      else
      {
         for (j = 0; j < cmdArg->dataVal.ttlVal; j++)
	 {
            if (cmdArg->cmdType == TL1_CFG_CMD)
	    {

#ifdef SIMUL
               printf("CF %s %s\n", (U8_t *)(((U8_t **)cmdArg->cFunc)[j]), cmdArg->argBuf);
#else
               if (((PFUNC_t)((PFUNC_t *)cmdArg->cFunc)[j])(cmdArg->argBuf, NULL) != OK)
	       {
                  tl1PrintErr(cmdArg->ctagBuf, "SROF");
                  return(ERROR);
	       }
#endif

	    }
            else
	    {

#ifdef SIMUL
               printf("SH %s %s\n", (U8_t *)(((U8_t **)cmdArg->cFunc)[j]), cmdArg->argBuf);
#else
               if (((PFUNC_t)((PFUNC_t *)cmdArg->cFunc)[j])(cmdArg->argBuf, NULL) != OK)
	       {
                  tl1PrintErr(cmdArg->ctagBuf, "SROF");
                  return(ERROR);
	       }
#endif

	    }

	 }
         return(OK);
      }
   }
   cmdArg->argCnt++;

   for (i = 0; i < dataVal->ttlVal; i++)
   {
      switch (dataVal->type)
      {
      case TL1_IN_NUM:
         sprintf(&(cmdArg->argBuf[strPos]), "%u ", dataVal->u.val[i]);
         break;
      case TL1_IN_INT:
         sprintf(&(cmdArg->argBuf[strPos]), "%d ", dataVal->u.val[i]);
         break;
      case TL1_IN_BITS:
         sprintf(&(cmdArg->argBuf[strPos]), "0x%x ", dataVal->u.val[i]);
         break;
      case TL1_IN_STR:
         sprintf(&(cmdArg->argBuf[strPos]), "%s ", dataVal->u.str);
         break;
      }
      
      if (ttlFld > 1)
      {
         if (tl1CnvArg(cmdArg, ttlFld-1, dataVal+1, 
                 strlen(cmdArg->argBuf)) == ERROR)
            return(ERROR);
      }
      else if (cmdArg->funcType == TL1_FUNC_PRC)
      {
         if (cmdArg->cmdType == TL1_CFG_CMD)
	 {
#ifdef SIMUL
            printf("CF %s %s\n", (U8_t *)cmdArg->cFunc, cmdArg->argBuf);
            return(OK);
#else
            return(((PFUNC_t)cmdArg->cFunc)(cmdArg->argBuf, NULL));
#endif
	 }
         else
         {
#ifdef SIMUL
            printf("SH %s %s\n", (U8_t *)cmdArg->cFunc, cmdArg->argBuf);
            return(OK);
#else
            return(((PFUNC_t)cmdArg->cFunc)(cmdArg->argBuf, NULL));
#endif
         }
      }
      else
      {
         for (j = 0; j < cmdArg->dataVal.ttlVal; j++)
	 {
            if (cmdArg->cmdType == TL1_CFG_CMD)
	    {

#ifdef SIMUL
               printf("CF %s %s\n", (U8_t *)(((U8_t **)cmdArg->cFunc)[j]), cmdArg->argBuf);
#else
               if (((PFUNC_t)((PFUNC_t *)cmdArg->cFunc)[j])(cmdArg->argBuf, NULL) != OK)
	       {
                  tl1PrintErr(cmdArg->ctagBuf, "SROF");
                  return(ERROR);
	       }
#endif

	    }
            else
	    {

#ifdef SIMUL
               printf("SH %s %s\n", (U8_t *)(((U8_t **)cmdArg->cFunc)[j]), cmdArg->argBuf);
#else
               if (((PFUNC_t)((PFUNC_t *)cmdArg->cFunc)[j])(cmdArg->argBuf, NULL) != OK)
	       {
                  tl1PrintErr(cmdArg->ctagBuf, "SROF");
                  return(ERROR);
	       }
#endif

	    }

	 }
         return(OK);
      }
   }
   return(OK);
}



/* ------------------------------------------------------------
 * Name:         U8_t *tl1CmdNextArg(U8_t *argListPtr, 
 *                         U8_t *argPtr,
 *                         U8_t seperator)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *
tl1CmdNextArg(U8_t *argListPtr, 
              U8_t *argPtr,
              U8_t *delimer,
              U8_t *seperator)
{
    register U8_t ch;
   U8_t      *str;
 

    argPtr[0] = '\0';
    while (*argListPtr == ' ')
        argListPtr++;

    if (!(*argListPtr))
        return(NULL);


    while ((ch = *argPtr = *argListPtr) )
    {
        for (str = delimer; *str && ch != *str; str++)
            ;
        if (!*str)
        /* no match */
        {
           argPtr++;
           argListPtr++;
        }
        else
            break;
    }
    *seperator = ch;
    if (ch)
    {
        argPtr--;
        while (*argPtr == ' ')
           argPtr--;
        argPtr++;

        *argPtr = '\0';
        return(argListPtr+1);
    }
    return(NULL);

}


STATUS_t
tl1MapStr2Num(U8_t           *dataStr, 
             pCLIUMAPSTRUC_t mapLst,
             U32_t           *val)
{
   while (mapLst->mapStr)
   {
      if (!strmatch(dataStr, mapLst->mapStr))
      {
         *val = mapLst->val;
         return(OK);
      }
      mapLst++;
   }
   return(ERROR);
}

void 
tl1PrintErr(U8_t *ctagBuf, U8_t *errCode)
{
   printf("M <%s> DENY\n  <%s>\n /* \"Description of error\" */\n;\n",
           ctagBuf, errCode);

}

U32_t strmatch(U8_t   *s1,
                U8_t   *s2)
{
   U8_t  ch1, ch2;

   while (*s1 && *s2)
   { 
      if ((ch1 = toupper(*s1)) != (ch2 = toupper(*s2)))
         return(ch1 - ch2);
      s1++;
      s2++;
   }
   return(*s1-*s2);
}

U32_t strnmatch(U8_t   *s1,
                 U8_t   *s2,
                 U32_t  cnt)
{
   U8_t  ch1, ch2;

   while (*s1 && *s2 && cnt)
   { 
      if ((ch1 = toupper(*s1)) != (ch2 = toupper(*s2)))
         return(ch1 - ch2);
      s1++;
      s2++;
      cnt--;
   }
   if (!cnt)
      return(0);
   return(*s1-*s2);
}
   



