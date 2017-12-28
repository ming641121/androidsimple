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
#include "axss_types.h"
#include "axss_if.h"
#include "sys_os.h"
#include "siw_if.h"
#include "clicmd_if.h"
#include "menu_objs.h"
#include "menutbl.h"

typedef STATUS_t (*MFUNC_t)(MENU_DATA_VAL_t *, U32_t *, U32_t *);
typedef STATUS_t (*PFUNC_t)(U8_t *, U8_t *);
typedef STATUS_t (*UPFUNC_t)(U32_t, MENU_DATA_VAL_t *);
typedef STATUS_t (*UFUNC_t)(MENU_DATA_ENTRY_t *pDtbl, U8_t *, MENU_DATA_VAL_t *);
typedef STATUS_t (*IFUNC_t)(MENU_DATA_ENTRY_t *pDtbl, U8_t *, MENU_DATA_VAL_t *);


/*
 *   Constant for screen limitation 
 */




#define AST_SEL                   2

#define EXACT                     1

#define RX_CTRL_C                 1

typedef struct {
      void             *cFunc;
      U8_t             funcType;
      U8_t             cmdType;
      U16_t            argCnt;
      U8_t             argBuf[INBUF_SIZE];
      MENU_DATA_VAL_t  dataVal;
} MENU_CMD_ARG_t;
   


/*
 *   Function prototype
 */
STATUS_t
menuCmdsParse(S32_t inFd, MENU_TBL_t *mTblPtr);

STATUS_t
menuDataTblParse(S32_t           inFd,
                 MENU_DATA_TBL_t *dTblPtr);

STATUS_t
menuDataParmParse(S32_t inFd, 
                 MENU_DATA_ENTRY_t *pDtbl, 
                 MENU_DATA_VAL_t *dataVal);

U32_t menuGetInData(S32_t             inFd, 
                    MENU_DATA_ENTRY_t *pDtbl, 
                    U8_t              *inBuf, 
                    MENU_DATA_VAL_t   *inData);

void menuIn2Val(MENU_DATA_ENTRY_t *pDtbl, 
                U8_t              *inBuf, 
                MENU_DATA_VAL_t   *inData);

STATUS_t menuCnvArg(MENU_CMD_ARG_t    *cmdArg,
                    U32_t             ttlFld,
                    MENU_DATA_VAL_t   *dataVal,
                    U32_t             strPos);

STATUS_t cnvChar2Val(U8_t inpCh, U32_t *val);

extern U32_t   accessLvl;

STATUS_t
menuMain(S32_t inFd)
{
    return(menuCmdsParse(inFd, &mainMenu));
}


STATUS_t
menuLogin(S32_t inFd)
{
    return(menuCmdsParse(inFd, &loginMenu));
}

/* ------------------------------------------------------------
 * Name:         STATUS_t menuCmdsParse(S32_t inFd,
 *                                      U8_t *argListPtr)
 *
 * Description:  Menu Command Parser
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
menuCmdsParse(S32_t inFd, MENU_TBL_t *mTblPtr)
{
    MENU_DATA_TBL_t *pDtbl;
    U32_t           len;
    U8_t            inBuf[INBUF_SIZE];
    U32_t           val;
    U32_t           orgOpt;
    STATUS_t        rc;

   orgOpt = ioctl(inFd, FIOGETOPTIONS, 0);

   /* set to raw mode */
   ioctl(inFd, FIOOPTIONS, OPT_CRMOD);

   while (TRUE)
   {
      printf("%s\n", mTblPtr->menuBanner);
      printf("%s\n", mTblPtr->menuBody);

      while (TRUE)
      /* wrong character don't require repring menu banner */
      /* esc will reprint banner */
      {   

         /* read one character at a time */
         /* no echo required */
         len = read(inFd, inBuf, 1);

#ifdef DEBUG
printf("len %u inFd %x\n", len, inFd);
#endif
         if ((S32_t)len <= 0)
         /* I/O closed */
         {
	     /* restore original option */
            ioctl(inFd, FIOOPTIONS, orgOpt);
            return(FD_ERROR);
         }

         if (inBuf[0] == ESC_CHAR)
	 {
	     /* restore original option */
            ioctl(inFd, FIOOPTIONS, orgOpt);
            return(OK);
	 }

         if (inBuf[0] == EOL_CHAR)
            continue;

         if (cnvChar2Val(inBuf[0], &val) != OK)
         /* illegal character, ignore */
            continue;

         if (val < mTblPtr->lowBound ||
             val >  mTblPtr->highBound)
         /* not in range */
         {
            continue;
         }
         pDtbl = ((MENU_DATA_TBL_t **)mTblPtr->selList)[val];
         if (pDtbl && pDtbl->type == MENU_DATA_TBL_SEL &&
             pDtbl->accessLvl > accessLvl)
         {
             continue;
         }
         break;
      }

      pDtbl = ((MENU_DATA_TBL_t **)mTblPtr->selList)[val];
      if (pDtbl == NULL)
      /* return to upper layer */
      {
          /* restore original option */
         ioctl(inFd, FIOOPTIONS, orgOpt);
         return(OK);
      }

      if (pDtbl->type == MENU_ENTRY_SEL)
      {
         rc = menuCmdsParse(inFd, (MENU_TBL_t *)pDtbl);
         if (rc == USER_LOGIN || rc == FD_ERROR)
         {
            /* restore original option */
               ioctl(inFd, FIOOPTIONS, orgOpt);
            return(rc);
	 }
      }
      else
      {
         rc = menuDataTblParse(inFd, pDtbl);
         if (rc == FD_ERROR || rc == USER_LOGIN)
         {
            /* restore original option */
               ioctl(inFd, FIOOPTIONS, orgOpt);
            return(rc);
         }
      }
   }
}


/* ------------------------------------------------------------
 * Name:         STATUS_t menuDataTblParse(S32_t inFd,
 *                                      U8_t *argListPtr)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
menuDataTblParse(S32_t           inFd,
                 MENU_DATA_TBL_t *dTblPtr)
{
   MENU_DATA_ENTRY_t **pDtbl;
   MENU_DATA_VAL_t   dataVal[10];
   MENU_CMD_ARG_t    cmdArg;
   U32_t             rc;
   U32_t             i;
   U32_t             ttlFld = 0;
   U32_t             snmpData;

#ifdef DEBUG
printf("in menuDataTblParse\n");
#endif
   cmdArg.cFunc = (void *)dTblPtr->procFunc;
   cmdArg.cmdType = dTblPtr->cmdType;
   cmdArg.argCnt = 0;
   cmdArg.funcType = MENU_FUNC_PRC;

   pDtbl = ((MENU_DATA_ENTRY_t **)dTblPtr->selList);
   snmpData = dTblPtr->snmpData;
   if (dTblPtr->ttlData == 0)
   {
      if (dTblPtr->snmpData == FALSE)
      {
         return(((UPFUNC_t)dTblPtr->procFunc)(0, NULL));

      }
      else if (dTblPtr->cmdType == MENU_CMD_CFG)
      {
#ifdef SIMUL
         printf("CF %s %s\n", (U8_t *)dTblPtr->procFunc, (U8_t *)dTblPtr->selList);
         return(OK);
#else
         return(((UPFUNC_t)dTblPtr->procFunc)((U8_t *)dTblPtr->selList, NULL));
#endif

      }
      else
      {
#ifdef SIMUL
         printf("SH %s %s\n", (U8_t *)dTblPtr->procFunc, (U8_t *)dTblPtr->selList);
         return(OK);
#else
         return(((UPFUNC_t)dTblPtr->procFunc)((U8_t *)dTblPtr->selList, NULL));
#endif
      }
   }
#ifdef DEBUG
printf("ttlData %u pDtbl %x pDtbl[0] %x\n", dTblPtr->ttlData,
        pDtbl, pDtbl[0]);
#endif
   for (i = 0; i < dTblPtr->ttlData; i++, pDtbl++)
   /* get all data entry */
   {
      switch ((*pDtbl)->type)
      {
      case MENU_DATA_PARM_SEL:
         if ((rc = menuDataParmParse(inFd, *pDtbl, dataVal)) != OK)
	 {
            if (rc == ERROR)
               return(ERROR);
            return(OK);
	 }
         ttlFld++;
         break;
      case MENU_DSP_PARM_SEL:
         if ((rc = menuDataParmParse(inFd, *pDtbl, &cmdArg.dataVal)) != OK)
	 {
            if (rc == ERROR)
               return(ERROR);
            return(OK);
	 }
   
         cmdArg.cFunc = 
                 ((MENU_DSP_TBL_t *)(*pDtbl))->dFuncList;
         cmdArg.funcType = MENU_FUNC_LST;
         break;
      }
   }

#ifdef DEBUG
printf("ttlFld %u\n", ttlFld);
#endif

   printf("\n\n");

   if (snmpData)
   {
      return(menuCnvArg(&cmdArg, ttlFld, dataVal, 0));
   }
   else
   {
      return(((UPFUNC_t)cmdArg.cFunc)(ttlFld, dataVal));
   }
}

/* ------------------------------------------------------------
 * Name:         STATUS_t menuDataParmParse(S32_t inFd,
 *                                          MENU_DATA_ENTRY_t *pDtbl, 
 *                                          MENU_DATA_VAL_t *dataVal)
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
menuDataParmParse(S32_t inFd, 
                 MENU_DATA_ENTRY_t *pDtbl, 
                 MENU_DATA_VAL_t *dataVal)
{
   U32_t             rc;
   U8_t              inBuf[INBUF_SIZE];

   rc = menuGetInData(inFd, pDtbl, inBuf, dataVal);
#ifdef DEBUG
printf("in menuDataParmParse return x\n", rc);
#endif
   if (rc == ERROR)
      return(ERROR);
   
   if (rc == RX_CTRL_C)
      return(RX_CTRL_C);
   switch (pDtbl->mapType)
   {
   case MAP_ARG_BITMAP:
   case MAP_ARG_NOMAP:
      /* all input had checked */
      /* should not have any error */
      menuIn2Val(pDtbl, inBuf, dataVal);
      break;
   case MAP_ARG_FUNC:
#ifdef SIMUL
      printf("U  %s %s\n", (U8_t *)pDtbl->mapFunc, inBuf);
#else
      ((UFUNC_t)pDtbl->mapFunc)(pDtbl, inBuf, dataVal);
#endif
      break;
   }
   return(OK);
  
}


/* ------------------------------------------------------------
 * Name:         STATUS_t menuGetInData(S32_t             inFd,
 *                                      MENU_DATA_ENTRY_t *pDtbl,
 *                                      U8_t              *inBuf,
 *                                      MENU_DATA_VAL_t   *inData)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U32_t menuGetInData(S32_t             inFd, 
                    MENU_DATA_ENTRY_t *pDtbl, 
                    U8_t              *inBuf, 
                    MENU_DATA_VAL_t   *inData)
{
   U8_t     inpCh;
   U32_t    inChInx = 0;
   U32_t    lowBound;
   U32_t    highBound;
   U32_t    len;
   U32_t    val;
   U32_t    rc;

#ifdef DEBUG
printf("in menuGetInData pDtbl %x flag %x\n", pDtbl, pDtbl->flags);
#endif
   inBuf[0] = '\0';

   while (TRUE)
   /* esc need to reprint menu */
   /* ctl-c will back to prious menu */
   {
      printf("%s\n", pDtbl->menuBanner);
      if (pDtbl->menuDspType == MENU_DSP_BODY)
      {
          printf("%s", pDtbl->menuBody);
          lowBound = pDtbl->lowBound;
          highBound = pDtbl->highBound;
      }
      else
      {
         ((MFUNC_t)pDtbl->menuBody)(inData, &lowBound, &highBound);
      }

      if (!(pDtbl->flags & MENU_SECUR_ECHO))
         printf(inBuf);
      else
      {
         U8_t *str = inBuf;

         while (*str)
         {
            printf("*");
            str++;
         }
      }

      while (TRUE)
      {
         len = read(inFd, &inpCh, 1);
#ifdef DEBUG
printf("len %u inFd %x char %x\n", len, inFd, inpCh);
#endif
         if ((S32_t)len <= 0)
         {
            return(FD_ERROR);
         }

         if (inpCh == ESC_CHAR)
         /* esc cause table reprint */
         {
            break;
         }
         if (inpCh == CTRL_C)
         /* control-c will back to upper menu */
         {
            return(RX_CTRL_C);
         }

         if (((pDtbl->flags & MENU_MULTI_IN) || pDtbl->inType != MENU_IN_TYPE_NUM) &&
              inpCh == BK_SPACE)
         /* backspace */
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

         if (inpCh == EOL_CHAR && inChInx == 0 &&
             (pDtbl->flags & MENU_NULL_IN_UFUNC))
         {
#ifdef SIMUL
            printf("I  %s\n", (U8_t *)pDtbl->inFunc);
#else
            ((IFUNC_t)pDtbl->inFunc)(pDtbl, inBuf, inData);
#endif
            return(OK);
	 }

         if (pDtbl->inType == MENU_IN_TYPE_NUM)
         {
            if ((pDtbl->flags & MENU_MULTI_IN) && inpCh == '-')
            /* multiple selection available */
            {
               inBuf[inChInx++] = inpCh;
               inBuf[inChInx] = '\0';
               printf("%c", inpCh);
               continue;
            }

            if ((pDtbl->flags & MENU_ALL_AST) && inpCh == '*')
            /* all in * */
            {
               inBuf[inChInx++] = inpCh;
               inBuf[inChInx] = '\0';
               printf("%c", inpCh);
               if ((pDtbl->flags & MENU_MULTI_IN))
	       {
                  continue;
	       }
               return(OK);
            }

            if (inpCh != EOL_CHAR)
            {
               rc = cnvChar2Val(inpCh, &val);
               if (rc == ERROR)
               /* illegal character, ignore */
                  continue;

               if (val < lowBound ||
                   val >  highBound)
               /* out of range ignore */
               {
                  continue;
               }
               inBuf[inChInx++] = inpCh;
               inBuf[inChInx] = '\0';
               
               if ((pDtbl->flags & MENU_MULTI_IN))
               {
                  printf("%c", inpCh);
                  continue;
               }
               return(OK);
            }
            else if (inChInx == 0)
            /* no input provided */
            {
               if (pDtbl->flags & MENU_NULL_IN_ERROR)
               {
                  continue;
               }
               return(OK);
            }
            else
               return(OK);
         }
         else if (pDtbl->inType == MENU_IN_TYPE_INT)
         {
            if (inpCh == '-')
            /* negative */
            {
               if (inChInx == 0)
	       {
                  inBuf[inChInx++] = inpCh;
                  inBuf[inChInx] = '\0';
                  printf("%c", inpCh);
	       }
               continue;
            }

            if (inpCh != EOL_CHAR)
            {
               rc = cnvChar2Val(inpCh, &val);
               if (rc == ERROR)
               /* illegal character, ignore */
                  continue;

               inBuf[inChInx++] = inpCh;
               inBuf[inChInx] = '\0';
               printf("%c", inpCh);
               
            }
            else if (inChInx == 0)
            /* no input provided */
            {
               if ((pDtbl->flags & MENU_NULL_IN_ERROR))
               {
                  continue;
               }
               return(OK);
            }
            else
               return(OK);
         }
         else
         /* string input */
         {
            if (inpCh != EOL_CHAR)
            {
               if (highBound && inChInx >= highBound)
                  continue;
               if (isprint(inpCh))
               {
                  if ((pDtbl->flags & MENU_SECUR_ECHO))
                  {
                     if (inpCh == ' ' || inpCh == ',' || inpCh == ':' ||
                          inpCh == ';' || inpCh == '<' || inpCh == '>' || 
                          inpCh == '[' || inpCh == ']' || inpCh == '{' || 
                          inpCh == '}' || inpCh == '|' || inpCh == '+' ||
                          inpCh == '&' || inpCh == '=' || inpCh == '^' ||
                          inpCh == '/' || inpCh == '\\')
                             continue;
                     printf("*");
                  }
                  else
                     printf("%c", inpCh);

                  inBuf[inChInx++] = inpCh;
                  inBuf[inChInx] = '\0';
               }
            }
            else if (inChInx == 0)
            /* no input provided */
            {
               if ((pDtbl->flags & MENU_NULL_IN_ERROR))
               {
                  continue;
               }
               return(OK);
            }
            else
               return(OK);
         }
      }
   }
   return(OK);
}
/* ------------------------------------------------------------
 * Name:         void menuIn2Val(MENU_DATA_ENTRY_t *pDtbl, 
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
void menuIn2Val(MENU_DATA_ENTRY_t *pDtbl, 
                U8_t              *inBuf, 
                MENU_DATA_VAL_t   *inData)
{
   U32_t    inRangeMode = 0;
   U32_t    lastVal = pDtbl->lowBound;
   U32_t    fstDigit = TRUE;
   U32_t    i, j;
   U8_t     inpCh;
   U32_t    val;
   U32_t    bitMap, bits;
   U32_t    fldPos;
   U32_t    start, dif;

   if (pDtbl->fldPos)
      fldPos = pDtbl->fldPos - 1;
   else
      fldPos = 0;

#ifdef DEBUG
printf("in menuIn2Val fldPos %u\n", fldPos);
#endif
   inData[fldPos].type = pDtbl->type;
   inData[fldPos].ttlVal = 0;
   inData[fldPos].u.val[0] = 0;

   if (inBuf[0] == '\0')
   {
      return;
   }
   if (pDtbl->inType == MENU_IN_TYPE_STR)
   {
      for (i = 0; inpCh = inBuf[i]; i++)
      {
         inData[fldPos].u.str[i] = inpCh;
      }
      inData[fldPos].u.str[i] = '\0';
      inData[fldPos].ttlVal = 1;
      return;
   }
   else if (pDtbl->inType == MENU_IN_TYPE_INT)
   {
      inData[fldPos].ttlVal = 1;
      inData[fldPos].u.val[0] = atoi(inBuf);
      return;
   }

   bitMap = 0;
   for (i = 0; inpCh = inBuf[i]; i++)
   {
      if (inpCh == '-')
      {
         inRangeMode = TRUE;
         continue;
      }
      else if (inpCh == '*')
      {
         bits = 0;
         for (i = 0; i <= pDtbl->highBound -  pDtbl->lowBound; i++)
         {
            bits <<= 1;
            bits |= 1;
	 }
         bits <<= pDtbl->lowBound;
         bitMap |= bits;
         break;
      }
      cnvChar2Val(inpCh, &val);
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
      if (fstDigit)
      {
         dif = pDtbl->highBound - pDtbl->lowBound + 1;
         start = pDtbl->lowBound;
      }
      else
      {
         dif = pDtbl->highBound - lastVal;
         start = lastVal + 1;
      }

      for (j = 0, bits = 0; j < dif; j++)
      {
         bits <<= 1;
         bits |= 1;
      }
      bits <<= start;
      bitMap |= bits;
   }

   switch (pDtbl->mapType)
   {
   case MAP_ARG_BITMAP:
      inData[fldPos].type = MENU_IN_TYPE_BITS;
      inData[fldPos].u.val[0] = bitMap;
      inData[fldPos].ttlVal = 1;
      break;
   case MAP_ARG_NOMAP:
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
   return;
}
               

/* ------------------------------------------------------------
 * Name:         void menuCnvArg(MENU_DATA_ENTRY_t *pDtbl, 
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
STATUS_t menuCnvArg(MENU_CMD_ARG_t    *cmdArg,
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
         return(menuCnvArg(cmdArg, ttlFld-1, dataVal+1, 
                 strlen(cmdArg->argBuf)));
       
      }
      if (cmdArg->funcType == MENU_FUNC_PRC)
      {
         if (cmdArg->cmdType == MENU_CMD_CFG)
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
            if (cmdArg->cmdType == MENU_CMD_CFG)
	    {

#ifdef SIMUL
               printf("CF %s %s\n", (U8_t *)(((U8_t **)cmdArg->cFunc)[j]), cmdArg->argBuf);
#else
               if (((PFUNC_t)((PFUNC_t *)cmdArg->cFunc)[j])(cmdArg->argBuf, NULL) != OK)
                  return(ERROR);
#endif

	    }
            else
	    {

#ifdef SIMUL
               printf("SH %s %s\n", (U8_t *)(((U8_t **)cmdArg->cFunc)[j]), cmdArg->argBuf);
#else
               if (((PFUNC_t)((PFUNC_t *)cmdArg->cFunc)[j])(cmdArg->argBuf, NULL) != OK)
                  return(ERROR);
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
      case MENU_IN_TYPE_NUM:
         sprintf(&(cmdArg->argBuf[strPos]), "%u ", dataVal->u.val[i]);
         break;
      case MENU_IN_TYPE_INT:
         sprintf(&(cmdArg->argBuf[strPos]), "%d ", dataVal->u.val[i]);
         break;
      case MENU_IN_TYPE_BITS:
         sprintf(&(cmdArg->argBuf[strPos]), "0x%x ", dataVal->u.val[i]);
         break;
      case MENU_IN_TYPE_STR:
         sprintf(&(cmdArg->argBuf[strPos]), "%s ", dataVal->u.str);
         break;
      }
      
      if (ttlFld > 1)
      {
         if (menuCnvArg(cmdArg, ttlFld-1, dataVal+1, 
                 strlen(cmdArg->argBuf)) == ERROR)
            return(ERROR);
      }
      else if (cmdArg->funcType == MENU_FUNC_PRC)
      {
         if (cmdArg->cmdType == MENU_CMD_CFG)
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
            if (cmdArg->cmdType == MENU_CMD_CFG)
	    {

#ifdef SIMUL
               printf("CF %s %s\n", (U8_t *)(((U8_t **)cmdArg->cFunc)[j]), cmdArg->argBuf);
#else
               if (((PFUNC_t)((PFUNC_t *)cmdArg->cFunc)[j])(cmdArg->argBuf, NULL) != OK)
                  return(ERROR);
#endif

	    }
            else
	    {

#ifdef SIMUL
               printf("SH %s %s\n", (U8_t *)(((U8_t **)cmdArg->cFunc)[j]), cmdArg->argBuf);
#else
               if (((PFUNC_t)((PFUNC_t *)cmdArg->cFunc)[j])(cmdArg->argBuf, NULL) != OK)
                  return(ERROR);
#endif

	    }

	 }
         return(OK);
      }
   }
   return(OK);
}

STATUS_t cnvChar2Val(U8_t inpCh, U32_t *val)
{
    if (inpCh >= '0' && inpCh <= '9')
    {
       *val = inpCh - '0';
       return(OK);
    }
    if (inpCh >= 'A' && inpCh <= 'Z')
    {
       *val = inpCh - 'A' + 10;
       return(OK);
    }
    if (inpCh >= 'a' && inpCh <= 'z')
    {
       *val = inpCh - 'a' + 10;
       return(OK);
    }
    if (inpCh == '*')
    {
       return(AST_SEL);
    }
    return(ERROR);
}

   

