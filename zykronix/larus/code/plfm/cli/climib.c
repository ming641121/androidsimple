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
#include "sr_conf.h"
#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#undef SR_OFFSET /* must following method.h */
#include "objectdb.h"
#include "makevb.h"
#include "cli_objs.h"
/* #include "snmp_util.h" */
#include "oid_lib.h"
#define SR_SNMPv2_PDU

/*
 *   Constant for screen limitation 
 */

#define MAX_DSP_COL      80

#define SHELL_SEM_WAIT_TIME       3

extern ObjectInfo      OidList[];
extern U32_t           serialNum;

U32_t           gSnmpInitFlag = 1;
extern SEM_ID          semM;
/*
 *   Function prototype
 */
STATUS_t DoMibCnf(U8_t *, OID *, Pdu *, pCLICFGTBL_t,
         pCLIMIBAPIGROUP_t);
STATUS_t mibTblShow(pCLIMIBDSPGROUP_t dspGrpPtr,
           OID   *keyInst, U32_t *lineNum);
STATUS_t mibObjShow(pMIBDSPENTITY_t dspObjPtr,
           VarBind *vb_ptr, U32_t   *lineNum);

STATUS_t mibTblFltShow(
	pCLIMIBDSPGROUP_t dspGrpPtr,
	OID               *keyInst, 
	U32_t             *lineNum,
        U8_t              *fltOptStr);


extern struct _Pdu *do_sets(
	struct _Pdu *inPdu,
	struct _ContextInfo *contextInfo,
	struct _OctetString *viewName,
	int payload);

U8_t *nextOptArgGet(U8_t *argListPtr, U8_t *optPtr, U8_t *argPtr);
U32_t optFindMibEnt(U8_t *optPtr, pCLICFGTBL_t mibCfgList,
              pCLIMIBENTITY_t *mibEntPtr);
STATUS_t arg2MibKeyInst(U8_t *argPtr, pCLIMIBENTITY_t mibEntPtr, 
              ObjectInfo *objPtr, OID *keyInst);
STATUS_t ParseArg2Val(U8_t *argPtr, pCLIMIBENTITY_t mibEntPtr, 
             ObjectInfo *objPtr, U32_t *val);
OID *MakeOIDFromText(U8_t *text_str);
OctetString *MakeIPFromText(U8_t *text_str);
U8_t *str2val(U8_t  *text_str, U32_t *val);
U8_t *str2valnodot(U8_t  *text_str, U32_t *val);
void cliMibCfgHelp(pCLICFGTBL_t mibCfgList);
void cliApiCfgHelp(pCLIMIBAPIGROUP_t apiCfgTbl);
void cliDspHelp(pCLIMIBDSPGROUP_t dspGrpPtr);
U8_t *strskip(U8_t *s1, U8_t skipCh);
STATUS_t printLineCheck(U8_t *printBuf, U8_t *dataBuf,
               U32_t *lineCnt);
U32_t nextInstGet(pCLIMIBENTITY_t mibObjPtr,
    OID               *keyInst);
void freeMibVar(pCLIMIBAPIOBJ_t apiObjPtr,
           U32_t  *val, U32_t  item);
VarBind *SnmpObjGet(U32_t searchType, OID *var);

extern void SysTimeShow(void);

/*
 * Local variables
 */
U8_t *argTypeStr[] = {
"num", "string", "map str", "func str"};

/* ------------------------------------------------------------
 * Name:         STATUS_t MibCnf(U8_t *argListPtr, 
 *                           pCLICFGTBL_t mibCfgList,
 *                           pCLIMIBAPIGROUP_t apiCfgTbl)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
MibCnf(U8_t *argListPtr, 
       pCLICFGTBL_t mibCfgList,
       pCLIMIBAPIGROUP_t apiCfgTbl)
{
    OID             keyInst[10];
    U32_t           instBuf[10][160];
    U32_t           tbl, key;
    U8_t            optStr[150], argStr[150];
    U8_t            *keyPtr, *endPtr;
    pCLIMIBCFGGROUP_t *mibTblList;
    U32_t           keyFound = FALSE;
    pCLIMIBENTITY_t mibObjPtr;
    Pdu             *tmp_pdu_ptr, *pdu_ptr;
    int             val;
    ObjectInfo      *object;
    U8_t            errmsg[200];


    if (gSnmpInitFlag == FALSE)
    {
        printf("CLI - commands are not accepted until the SNMP initialization process is finished\n");
        return (ERROR);
    }

    if (!argListPtr)
    {
        cliMibCfgHelp(mibCfgList);
        return(OK);
    }
    mibTblList = mibCfgList->mibTblList;
    for (tbl = 0; tbl < mibCfgList->tblCnt; 
        tbl++, mibTblList++)
    /* search for mib index(key) */
    {
        keyInst[tbl].length = 0;
        keyInst[tbl].oid_ptr = instBuf[tbl];

        for (key = 0; key < mibTblList[0]->keyLen; key++)
	{ 
            mibObjPtr = mibTblList[0]->mibKeyList[key];
            keyPtr = argListPtr;
            /* looking for table index */
            while ((keyPtr = strstr(keyPtr, mibObjPtr->opt)))
	    {
	        do {
                    keyPtr--;
                } while (keyPtr != argListPtr && *keyPtr == ' ');
                
                endPtr = nextOptArgGet(keyPtr, optStr, argStr);
                if (strcmp(optStr, mibObjPtr->opt))
                {
                    keyPtr = endPtr;
                    if (!keyPtr)
                        break;
                    continue;
                }
                if (argStr[0] == '\0')
	        {
                    printf("CLI - syntax error: missing option argument at -%s For help type %s <Enter>\n", optStr, mibCfgList->cfgName);
                    return(ERROR);
	        }
                if (argStr[0] == '.')
                {
                    if (apiCfgTbl)
                        printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, apiCfgTbl->name);
                    else
                        printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, mibCfgList->cfgName);
                    return(ERROR);
                }

                if (!mibObjPtr->objPtr)
		{
                    val = find_object(&(mibObjPtr->oid), EXACT);
                    if (val == -1)
	            {
                        printf("CLI - system error: illegal Object %s\n", mibObjPtr->objName);
                                return(ERROR);
	            }
                    
                    mibObjPtr->objPtr = &OidList[val];
		}

                object = mibObjPtr->objPtr;
 
                if (arg2MibKeyInst(argStr, mibObjPtr, object, &keyInst[tbl]) == ERROR)
	        {
                    if (apiCfgTbl)
                        printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, apiCfgTbl->name);
                    else
                        printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, mibCfgList->cfgName);
                    return(ERROR);
	        }

                memset(keyPtr, ' ', endPtr - keyPtr);
                keyFound = TRUE;
                break;
	    }/* end while */
            if (!keyPtr)
	    {
                if (!key)
                    break;
                else
	        {
                     if (apiCfgTbl)
                        printf("CLI - missing index %s\n For help type %s <Enter>\n", mibObjPtr->objName, apiCfgTbl->name);
                    else
                        printf("CLI - missing index %s\n For help type %s <Enter>\n", mibObjPtr->objName, mibCfgList->cfgName);
                    return(ERROR);
	        }
	    }
	}/* end for key */

        if (!mibTblList[0]->keyLen)
	{
            keyInst[tbl].length = 1;
            keyInst[tbl].oid_ptr[0] = 0;
            keyFound = TRUE;
	}
    }/* end for tbl */

    if (!keyFound)
    {
        if (apiCfgTbl)
            printf("CLI - index not specified\n For help type %s <Enter>\n", apiCfgTbl->name);
        else
            printf("CLI - index not specified\n For help type %s <Enter>\n", mibCfgList->cfgName);
        return(ERROR);
    }
    if ((pdu_ptr = (Pdu *) malloc(sizeof(Pdu))) == NULL)
    {
        printf("CLI - system error: no memory\n");
        return(ERROR);
    }
    memset(pdu_ptr, 0, sizeof(Pdu));
    pdu_ptr->type = SET_REQUEST_TYPE;

    if (DoMibCnf(argListPtr, keyInst, pdu_ptr, mibCfgList, apiCfgTbl) == ERROR)
    {
        FreePdu(pdu_ptr);
        return(ERROR);
    }

    if (!pdu_ptr->num_varbinds)
    {
        printf("CLI - Need additional parameter(s); the command did not execute!!!\n");
        FreePdu(pdu_ptr);
        return(ERROR);
    }

    if ((val = semTake(semM, SECONDS(SHELL_SEM_WAIT_TIME))) != OK)
    {
        if (val == ERROR)
            printf("CLI - SNMP Semaphare error!!!\n");
        else
            printf("CLI - SNMP Agent busy! Try again\n");
        return(ERROR);
    }

    tmp_pdu_ptr = do_sets(pdu_ptr, NULL, NULL, SR_SNMPv2_PDU_PAYLOAD);

    semGive(semM);
    if ((tmp_pdu_ptr) != NULL)
    {
        FreePdu(pdu_ptr);
        FreePdu(tmp_pdu_ptr);
        return(ERROR);
    }
    FreePdu(pdu_ptr);

    return(OK);
}

/* ------------------------------------------------------------
 * Name:         STATUS_t MibApiCnf(U8_t *argListPtr, 
 *                           pCLIMIBAPIGROUP_t apiCfgTbl,
 *                           pCLICFGTBL_t mibCfgList)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
MibApiCnf(U8_t              *argListPtr, 
          pCLIMIBAPIGROUP_t apiCfgTbl,
          pCLICFGTBL_t      mibCfgList)
{
    U8_t            optStr[500];
    U8_t            *endPtr;
    U32_t           len;
    pCLIMIBAPIOBJ_t apiObjPtr;
    

    if (gSnmpInitFlag == FALSE)
    {
        printf("CLI - commands are not accepted until the SNMP initialization process is finished\n");
        return (ERROR);
    }


    apiObjPtr = apiCfgTbl->apiList;
    if (!argListPtr && apiObjPtr->apiObject)
    {
        cliApiCfgHelp(apiCfgTbl);
        return(OK);
    }

    if (argListPtr)
        argListPtr = strskip(argListPtr, ' ');
    len = 0;
    while (1)
    {
        if (apiObjPtr->apiObject)
	{
            if (argListPtr && *argListPtr != '\0')
	    {
                endPtr = strchr(argListPtr, apiObjPtr->seperator);
        
                if (endPtr)
                    *endPtr = '\0';

                if (*argListPtr != '#')
		{
                    sprintf(&optStr[len], "-%s %s ", apiObjPtr->apiObject->opt, 
                          argListPtr);
                    len += strlen(&optStr[len]);
		}
                if (endPtr)
                    argListPtr = strskip(endPtr+1, ' ');
                else
                    argListPtr = endPtr;
	    }
            else
                break;
	}
        else if (argListPtr)
	{
            printf("CLI - too many parameters!
 For help type %s <Enter>\n", apiCfgTbl->name);
            return(ERROR);
	}
        else
            break;
        apiObjPtr++;
    }

    strcpy(&optStr[len], apiCfgTbl->optValStr);

    return(MibCnf(optStr, mibCfgList, apiCfgTbl));
}


/* ------------------------------------------------------------
 * Name:         STATUS_t MibApiDup(U8_t *argListPtr, 
 *                           pCLIMIBAPIGROUP_t apiCfgTbl,
 *                           pCLICFGTBL_t mibCfgList)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
MibApiDup(U8_t              *argListPtr, 
          pCLIMIBAPIGROUP_t apiCfgTbl,
          pCLICFGTBL_t      mibCfgList)
{
    U8_t            optStr[400];
    U8_t            argStr[40];
    U8_t            *endPtr;
    U32_t           len, len1;
    pCLIMIBAPIOBJ_t apiObjPtr;
    pCLIMIBENTITY_t mibObjPtr;
    pCLIMIBENTITY_t *mibInxPtr;
    int             tbl;
    OID             keyInst;
    U32_t           instBuf[50];
    int             val;

    apiObjPtr = apiCfgTbl->apiList;
    if (!argListPtr && apiObjPtr->apiObject)
    {
        cliApiCfgHelp(apiCfgTbl);
        return(OK);
    }
    if (apiObjPtr->apiObject)
    {
        tbl = optFindMibEnt(apiObjPtr->apiObject->opt,
            mibCfgList, &mibObjPtr);
        if (tbl == ERROR)
        {
            printf("CLI - illegal option string: %s
 For help type %s <Enter>\n", apiObjPtr->apiObject->opt, mibCfgList->cfgName);
            return(ERROR);
	}
    }
    else
    {
        nextOptArgGet(apiCfgTbl->optValStr, optStr, argStr);
        if (optStr[0] == '\0')
	{
            return(OK);
	}
        tbl = optFindMibEnt(optStr,
            mibCfgList, &mibObjPtr);
        if (tbl == ERROR)
        {
            printf("CLI - illegal option string: %s
 For help type %s <Enter>\n", optStr, mibCfgList->cfgName);
            return(ERROR);
	}
    }

        
    if (argListPtr)
        argListPtr = strskip(argListPtr, ' ');
    len = 0;
    while (1)
    {
        if (apiObjPtr->apiObject)
	{
            if (argListPtr && *argListPtr != '\0')
	    {
                endPtr = strchr(argListPtr, apiObjPtr->seperator);
        
                if (endPtr)
                    *endPtr = '\0';

                sprintf(&optStr[len], "-%s %s ", apiObjPtr->apiObject->opt, 
                      argListPtr);
                len += strlen(&optStr[len]);
                if (endPtr)
                    argListPtr = strskip(endPtr+1, ' ');
                else
                    argListPtr = endPtr;
	    }
            else
                break;
	}
        else if (argListPtr)
	{
            printf("CLI - too many parameters!
 For help type %s <Enter>\n", apiCfgTbl->name);
            return(ERROR);
	}
        else
            break;
        apiObjPtr++;
    }

    strcpy(&optStr[len], apiCfgTbl->optValStr);

    if (!(mibInxPtr = mibCfgList->mibTblList[tbl]->mibKeyList))
      /* scalar */
        return(MibCnf(optStr, mibCfgList, apiCfgTbl));

    keyInst.length = 0;
    keyInst.oid_ptr = instBuf;

    len += strlen(&optStr[len]);
    if ((val = semTake(semM, SECONDS(SHELL_SEM_WAIT_TIME))) != OK)
    {
        if (val == ERROR)
            printf("CLI - SNMP Semaphare error!!!\n");
        else
            printf("CLI - SNMP Agent busy! Try again\n");
        return(ERROR);
    }

    serialNum++;
    while (nextInstGet(mibObjPtr, &keyInst))
    {
        U32_t i;
        semGive(semM);
        len1 = len;
        for (i = 0; i < mibCfgList->mibTblList[tbl]->keyLen; i++)
	{
            sprintf(&optStr[len1], "-%s %u ", mibInxPtr[i]->opt, keyInst.oid_ptr[i]);
            len1 += strlen(&optStr[len1]);
	}
        if (MibCnf(optStr, mibCfgList, apiCfgTbl) == ERROR)
            return(ERROR);
        if ((val = semTake(semM, SECONDS(SHELL_SEM_WAIT_TIME))) != OK)
        {
            if (val == ERROR)
                printf("CLI - SNMP Semaphare error!!!\n");
            else
                printf("CLI - SNMP Agent busy! Try again\n");
            return(ERROR);
        }
    }
    semGive(semM);
    return (OK);
}


/* ------------------------------------------------------------
 * Name:         STATUS_t DoMibCnf(U8_t *argListPtr, 
 *                         OID               *keyInst,
 *                         Pdu               *pdu_ptr,
 *                         pCLICFGTBL_t      mibCfgList,
 *                         pCLIMIBAPIGROUP_t apiCfgTbl)
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
DoMibCnf(U8_t              *argListPtr,     
         OID               *keyInst,
         Pdu               *pdu_ptr,
         pCLICFGTBL_t      mibCfgList,
         pCLIMIBAPIGROUP_t apiCfgTbl)
{
    int             tbl, val;
    U8_t            optStr[150], argStr[150];
    pCLIMIBCFGGROUP_t *mibTblList;
    pCLIMIBENTITY_t mibObjPtr;
    VarBind         *vb_ptr;
    ObjectInfo      *object;
    U8_t            *str;

    mibTblList = mibCfgList->mibTblList;

    while (argListPtr)
    {
        str = nextOptArgGet(argListPtr, optStr, argStr);
        if (optStr[0] == '\0')
	{
            if (argStr[0] == '\0')
                return(OK);

            if ((str = strchr(argListPtr, ' ')))
                *str = '\0';
            printf("CLI - syntax error: missing option string at %s
 For help type %s <Enter>\n", argListPtr, mibCfgList->cfgName);
            return(ERROR);
	}
        if (argStr[0] == '\0')
	{
            printf("CLI - syntax error: missing option argument at %s
 For help type %s <Enter>\n", optStr, mibCfgList->cfgName);
            return(ERROR);
	}
        tbl = optFindMibEnt(optStr, mibCfgList, &mibObjPtr);
        if (tbl == ERROR)
	{
            printf("CLI - illegal option string: %s
 For help type %s <Enter>\n", optStr, mibCfgList->cfgName);
            return(ERROR);
	}

        if (mibCfgList->mibTblList[tbl]->keyLen &&
            !keyInst[tbl].length)
	{
            printf("CLI - table Index not specified for option %s
 For help type %s <Enter>\n", optStr, mibCfgList->cfgName);
            return(ERROR);
	}

        if (!mibObjPtr->objPtr)
	{
            val = find_object(&(mibObjPtr->oid), EXACT);
            if (val == -1)
	    {
                printf("CLI - system error: illegal Object %s\n", mibObjPtr->objName);
                return(ERROR);
	    }
	
            mibObjPtr->objPtr = &OidList[val];
	}
        object = mibObjPtr->objPtr;

        if (ParseArg2Val(argStr, mibObjPtr, object, &val) == ERROR)
	{
            if (apiCfgTbl)
                printf("CLI - illegal argument type for object %s
 For help type %s <Enter>\n", mibObjPtr->objName, apiCfgTbl->name);
            else
                printf("CLI - illegal argument type for object %s
 For help type %s <Enter>\n", mibObjPtr->objName, mibCfgList->cfgName);
            return(ERROR);
	}

        switch (object->oidtype)
	{
            case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
                vb_ptr = MakeVarBind(object, &keyInst[tbl], (void *)val);
                break;
            case OCTET_PRIM_TYPE:
            case OPAQUE_PRIM_TYPE:
            case IP_ADDR_PRIM_TYPE:
                vb_ptr = MakeVarBind(object, &keyInst[tbl], (void *)val);
                break;
            default:
                vb_ptr = MakeVarBind(object, &keyInst[tbl], (void *)&val);
                break;
	}

        if (LinkVarBind(pdu_ptr, vb_ptr) == -1)
	{
            printf("CLI - system error: LinkVarBind Fail!!!\n");
            return(ERROR);
	}
        pdu_ptr->num_varbinds++;
        if (str)
            argListPtr = strskip(str, ' ');
        else 
            argListPtr = NULL;

    }

    return(OK);
}




/* ------------------------------------------------------------
 * Name:         STATUS_t MibShow(U32_t searchType,
 *                           U8_t  *argListPtr,
 *                           pCLIMIBDSPGROUP_t dspGrpPtr)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
MibShow(U32_t searchType,
        U8_t  *argListPtr,
        pCLIMIBDSPGROUP_t dspGrpPtr)
{
    OID               keyInst;
    U32_t             instBuf[100];
    pCLIMIBCFGGROUP_t cfgTblPtr;

    pCLIMIBENTITY_t mibObjPtr;
    U32_t          lineNum = 0;
    U32_t          key;
    U8_t           argStr[120];
    int            val;
    ObjectInfo     *object;

 
    if (gSnmpInitFlag == FALSE)
    {
        printf("CLI - commands are not accepted until the SNMP initialization process is finished\n");
        return (ERROR);
    }

    keyInst.length = 0;
    keyInst.oid_ptr = instBuf;

    cfgTblPtr = dspGrpPtr->mibCfgGroup;

    if (searchType == EXACT)
    {
        if (!argListPtr && cfgTblPtr->keyLen)
	{
            cliDspHelp(dspGrpPtr);
            return(OK);
	}

        cfgTblPtr = dspGrpPtr->mibCfgGroup;

        for (key = 0; key < cfgTblPtr->keyLen; key++)
	{

            if (!argListPtr)
	    {
                cliDspHelp(dspGrpPtr);
                return(OK);
	    }

            mibObjPtr = cfgTblPtr->mibKeyList[key];

            /* looking for table index */
            argListPtr = nextOptArgGet(argListPtr, NULL, argStr);
            if (argStr[0] == '\0')
	    {
                printf("CLI - missing index %s For help type %s <Enter>\n", mibObjPtr->objName, dspGrpPtr->name);
                return(ERROR);
	    }
            if (argStr[0] == '.')
            {
                printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, dspGrpPtr->name);
                return(ERROR);
            }

            if (!mibObjPtr->objPtr)
	    {
                val = find_object(&(mibObjPtr->oid), EXACT);
                if (val == -1)
	        {
                    printf("CLI - system error: illegal Object %s\n", mibObjPtr->objName);
                    return(ERROR);
	        }
	    
                mibObjPtr->objPtr = &OidList[val];
	    }
           
            /* DEBUG */
            /* 
            printf("MibShow: find_object at %d OK \n", val); 
            */

            object = mibObjPtr->objPtr;
	    
            if (arg2MibKeyInst(argStr, mibObjPtr, object, &keyInst) == ERROR)
	    {
                printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, dspGrpPtr->name);
                return(ERROR);
	    }

            /* DEBUG */
            /* 
            printf("MibShow: arg2MibKeyInst OK \n"); 
            */

	}
        if (!cfgTblPtr->keyLen)
	/* scaler */
	{
            keyInst.length = 1;
            instBuf[0] = 0;
	}
    }
    else
    {
        keyInst.length = 1;
        instBuf[0] = 0;;
    }


    printf(dspGrpPtr->title);

    do
    {
        if (dspGrpPtr->mibCfgGroup->keyLen)
            mibObjPtr = dspGrpPtr->mibCfgGroup->mibKeyList[0];
        else
            mibObjPtr = dspGrpPtr->mibCfgGroup->mibList[0];

        if ((val = semTake(semM, SECONDS(SHELL_SEM_WAIT_TIME))) != OK)
        {
            if (val == ERROR)
                printf("CLI - SNMP Semaphare error!!!\n");
            else
                printf("CLI - SNMP Agent busy! Try again\n");
            return(ERROR);
        }
        serialNum++;
        if (searchType == NEXT)
        {

            if (nextInstGet(mibObjPtr, &keyInst) == FALSE)
	    {
                semGive(semM);
                return(OK);
	    }
        }

        if (mibTblShow(dspGrpPtr, &keyInst, &lineNum) == ERROR)
            return(ERROR);

        if (searchType == EXACT)
            return(OK);
    } while (1);
}

/* ------------------------------------------------------------
 * Name:         void FreeMibShowObjs(pCLIMIBDSPGROUP_t dspGrpPtr,
 *                                    VarBind         **vb_list)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
void
FreeMibShowObjs(pCLIMIBDSPGROUP_t dspGrpPtr,
                VarBind         **vb_list)
{
    pMIBDSPENTITY_t dspObjPtr;
    U32_t  i;

    dspObjPtr = dspGrpPtr->mibDspList;
    for (i = 0; i < dspGrpPtr->numOfDspEnt; i++, dspObjPtr++)
    {
        if (vb_list[i])
            FreeVarBind(vb_list[i]);
    }
}

/* ------------------------------------------------------------
 * Name:         STATUS_t mibTblShow(pCLIMIBDSPGROUP_t dspGrpPtr,
 *                           OID   *keyInst,
 *                           U32_t *lineNum)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
mibTblShow(pCLIMIBDSPGROUP_t dspGrpPtr,
           OID   *keyInst,
           U32_t *lineNum)
{
    U32_t            i;
    U8_t             argStr[100];
    pMIBDSPENTITY_t  dspObjPtr;
    OID              *var;
    U32_t            val;
    VarBind          *vb_ptr;
#define MAX_TBL_OBJS    200
    VarBind          *vb_list[MAX_TBL_OBJS];

    dspObjPtr = dspGrpPtr->mibDspList;
    memset(vb_list, 0, sizeof(VarBind *) * MAX_TBL_OBJS);

    for (i = 0; i < dspGrpPtr->numOfDspEnt; i++, dspObjPtr++)
    {
        if (dspObjPtr->dspMibEntry)
        {
            if ((var = CatOID(&(dspObjPtr->dspMibEntry->oid), keyInst)) == NULL)
	    {
                semGive(semM);
                FreeMibShowObjs(dspGrpPtr, vb_list);
                return(ERROR);
	    }

            vb_ptr = SnmpObjGet(EXACT, var);
            FreeOID(var);
    
            if (vb_ptr == NULL)
	    {
                semGive(semM);
                FreeVarBind(vb_ptr);
                FreeMibShowObjs(dspGrpPtr, vb_list);
                return(ERROR);
	    }

            vb_list[i] = vb_ptr;
        }
    }
    semGive(semM);

    printf("\nTiemPo         ");
    SysTimeShow();
     
    dspObjPtr = dspGrpPtr->mibDspList;
    for (i = 0; i < dspGrpPtr->numOfDspEnt; i++, dspObjPtr++)
    {
        if (dspObjPtr->dspMibEntry) 
        {
            if ((dspObjPtr->dspType == DSP_STRING) && (dspObjPtr->dspFunc != NULL))
            {
                if (((DSPFUNC_t)(dspObjPtr->dspFunc))(dspGrpPtr, keyInst, (U32_t *)argStr) == ERROR)
    	        {
                    argStr[0] = '*';
                    argStr[1] = '\0';
    	        }
                if (printLineCheck(dspObjPtr->format, argStr, 
                    lineNum) == FALSE)
		{
                    FreeMibShowObjs(dspGrpPtr, vb_list);
                    return(ERROR);
		}
                printf(dspObjPtr->format, argStr);
            }
            else
            {
                if (mibObjShow(dspObjPtr, vb_list[i], lineNum) == ERROR)
	        {
                    FreeMibShowObjs(dspGrpPtr, vb_list);
                    return(ERROR);
	        }
            }
            FreeVarBind(vb_list[i]);
            vb_list[i] = NULL;
        }
        else
        {
            if (dspObjPtr->dspType == DSP_STRING)
            {
                if (((DSPFUNC_t)(dspObjPtr->dspFunc))(dspGrpPtr, keyInst, (U32_t *)argStr) == ERROR)
    	        {
                    argStr[0] = '*';
                    argStr[1] = '\0';
    	        }
                if (printLineCheck(dspObjPtr->format, argStr, 
                    lineNum) == FALSE)
		{
                    FreeMibShowObjs(dspGrpPtr, vb_list);
                    return(ERROR);
		}
                printf(dspObjPtr->format, argStr);
    	    }
            else if (dspObjPtr->dspType == DSP_NUM)
    	    {
                if (printLineCheck(dspObjPtr->format, NULL, 
                    lineNum) == FALSE)
		{
                    FreeMibShowObjs(dspGrpPtr, vb_list);
                    return(ERROR);
		}
                if (((DSPFUNC_t)(dspObjPtr->dspFunc))(dspGrpPtr, keyInst, &val) != ERROR)
                    printf(dspObjPtr->format, val);
                else
                    printf(dspObjPtr->format, -1);
    	    }
            else
    	    {
                printf(dspObjPtr->format);
                ((DSPFUNC_t)(dspObjPtr->dspFunc))(dspGrpPtr, keyInst, lineNum);
    	    }
	}

    } /* for i */

    return (OK);
}


/* ------------------------------------------------------------
 * Name:         STATUS_t mibTblFltShow
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
mibTblFltShow(
	pCLIMIBDSPGROUP_t dspGrpPtr,
	OID               *keyInst,
	U32_t             *lineNum,
        U8_t              *fltOptStr)
{

    U32_t               i;
    U8_t                argStr[100];
    U8_t                optStr[100];
    pMIBDSPENTITY_t     dspObjPtr;
    OID                 *var;
    U32_t               val, argVal;
    VarBind             *vb_ptr;
#define MAX_TBL_OBJS    200
    VarBind             *vb_list[MAX_TBL_OBJS];
    U8_t                *str;
    U8_t                *strPtr;
    U16_t               checkOptDone = 0;
    int                 slot, line;    

    dspObjPtr = dspGrpPtr->mibDspList;
    memset(vb_list, 0, sizeof(VarBind *) * MAX_TBL_OBJS);

    for (i = 0; i < dspGrpPtr->numOfDspEnt; i++, dspObjPtr++)
    {
        if (dspObjPtr->dspMibEntry)
        {
            if ((var = CatOID(&(dspObjPtr->dspMibEntry->oid), keyInst)) == NULL)
	    {
                semGive(semM);
                FreeMibShowObjs(dspGrpPtr, vb_list);
                return(ERROR);
	    }

            vb_ptr = SnmpObjGet(EXACT, var);
            FreeOID(var);
    
            if (vb_ptr == NULL)
	    {
                semGive(semM);
                FreeVarBind(vb_ptr);
                FreeMibShowObjs(dspGrpPtr, vb_list);
                return(ERROR);
	    }
            vb_list[i] = vb_ptr;
        }
    }

    semGive(semM);

    /* check flt parameters */

    while (fltOptStr)
    {
        str = nextOptArgGet(fltOptStr, optStr, argStr);

        if (optStr[0] == '\0')
	{
            if (argStr[0] == '\0')
            {
                break;
            }
            if ((str = strchr(fltOptStr, ' ')))
                *str = '\0'; 
            printf("CLI - syntax error: flt missing option string");
            return(ERROR);
	}

        if (argStr[0] == '\0')
	{
            printf("CLI - syntax error: flt missing argument");
            return(ERROR);
	}

        if (strchr(argStr, '.') == NULL)
        {
            if ((str2val(argStr, &argVal)) == NULL)
            {
                printf("CLI - syntax error: flt wrong format \n");
                return(ERROR);
            }
        }
        else /* just for slot.line case */
        {
            slot = 0;
            line = 0;
            if ((strPtr = str2val(argStr, &slot)) == NULL)
            {
                printf("CLI - syntax error: flt wrong format \n");
                return(ERROR);
            }
            if ((strPtr = str2valnodot(strPtr, &line)) == NULL)
            {
                printf("CLI - syntax error: flt wrong format \n");
                return(ERROR);
            }
            argVal = (slot * 1000000) + (line * 1000);
        }

        if (strPtr == NULL)
        {
            printf("CLI - syntax error: flt wrong argument \n");
            return(ERROR);
        }

        dspObjPtr = dspGrpPtr->mibDspList;

        dspObjPtr ++; /* pass first DspEnt, it is key */

        checkOptDone = 0;

        for (i = 1; i < dspGrpPtr->numOfDspEnt; i++, dspObjPtr++)
        {
            if (strcmp(dspObjPtr->dspMibEntry->opt, optStr) == 0)
            {

                switch(vb_list[i]->value.type)
                {
                    case COUNTER_TYPE:
                    case GAUGE_TYPE:		
                    case TIME_TICKS_TYPE:
                    case INTEGER_TYPE:	

                        if (vb_list[i]->value.ul_value == argVal)
                        {
                            /* flt pass, get the next opt */
                            break;
                        }
                        else if (vb_list[i]->value.ul_value > 1000000) /* for slot.line case */
                        {
                            if (argVal == 0) /* no flt */
                            {
                                break;
                            }

                            if (argVal < 1000) /* only flt slot */
                            {
                                /* flt input as a slot number, such as 1, 5, ... */

                                if ((vb_list[i]->value.ul_value/1000000) == argVal)
                                    break;
                                else
                                    return OK;
                            }
  
                            if (((vb_list[i]->value.ul_value/1000000) == (argVal/1000000)) ||
                                (argVal/1000000 == 0)) /* slot OK */
                            {
                                if (((vb_list[i]->value.ul_value%1000000)/1000) == ((argVal%1000000)/1000) ||
                                    ((argVal%1000000)/1000 == 0))  /* line OK */ 
                                {
                                    break;
                                }
                                else
                                {
                                    /* flt slot pass, line not pass, do not show this obj */
                                    return OK;
                                }
                            }
                            else
                            {
                                /* flt slot not pass, do not show this obj */
                                return OK;
                            }
                        }
                        else
                        {
                            /* flt not pass, do not show this obj */
                            return OK;
                        }

                    default:
                        printf("CLI - syntax error: flt wrong arg type. \n");
                        return(ERROR);
                }
                checkOptDone = 1;
            }

            if (checkOptDone == 1)
                break;  

        } /* for i */

        if (checkOptDone == 0)
        {
            printf("CLI - syntax error: flt invalid opt: %s", optStr);
            return(ERROR);
        }

        if (str)
            fltOptStr = strskip(str, ' ');
        else 
            fltOptStr = NULL;

    } /* while fltOptStr */

    dspObjPtr = dspGrpPtr->mibDspList;

    for (i = 0; i < dspGrpPtr->numOfDspEnt; i++, dspObjPtr++)
    {
        if (dspObjPtr->dspMibEntry)
        {
            if ((dspObjPtr->dspType == DSP_STRING) && (dspObjPtr->dspFunc != NULL))
            {
                if (((DSPFUNC_t)(dspObjPtr->dspFunc))(dspGrpPtr, keyInst, (U32_t *)argStr) == ERROR)
    	        {
                    argStr[0] = '*';
                    argStr[1] = '\0';
    	        }
                if (printLineCheck(dspObjPtr->format, argStr, 
                    lineNum) == FALSE)
		{
                    FreeMibShowObjs(dspGrpPtr, vb_list);
                    return(ERROR);
		}
                printf(dspObjPtr->format, argStr);
            }
            else
            {
                if (mibObjShow(dspObjPtr, vb_list[i], lineNum) == ERROR)
	        {
                    FreeMibShowObjs(dspGrpPtr, vb_list);
                    return(ERROR);
	        }
            }
            FreeVarBind(vb_list[i]);
            vb_list[i] = NULL;
        }
        else
        {
            if (dspObjPtr->dspType == DSP_STRING)
            {
                if (((DSPFUNC_t)(dspObjPtr->dspFunc))(dspGrpPtr, keyInst, (U32_t *)argStr) == ERROR)
    	        {
                    argStr[0] = '*';
                    argStr[1] = '\0';
    	        }
                if (printLineCheck(dspObjPtr->format, argStr, 
                    lineNum) == FALSE)
		{
                    FreeMibShowObjs(dspGrpPtr, vb_list);
                    return(ERROR);
		}
                printf(dspObjPtr->format, argStr);
    	    }
            else if (dspObjPtr->dspType == DSP_NUM)
    	    {
                if (printLineCheck(dspObjPtr->format, NULL, 
                    lineNum) == FALSE)
		{
                    FreeMibShowObjs(dspGrpPtr, vb_list);
                    return(ERROR);
		}
                if (((DSPFUNC_t)(dspObjPtr->dspFunc))(dspGrpPtr, keyInst, &val) != ERROR)
                    printf(dspObjPtr->format, val);
                else
                    printf(dspObjPtr->format, -1);
    	    }
            else
    	    {
                printf(dspObjPtr->format);
                ((DSPFUNC_t)(dspObjPtr->dspFunc))(dspGrpPtr, keyInst, lineNum);
    	    }
	}

    } /* for i */
 
    return (OK);
}


/* ------------------------------------------------------------
 * Name:         STATUS_t nextInstGet(pCLIMIBENTITY_t mibObjPtr,
 *                           OID               *keyInst)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U32_t
nextInstGet(pCLIMIBENTITY_t mibObjPtr,
    OID               *keyInst)
{
    OID            *var;
    VarBind        *vb_ptr;
    int            i;

    if ((var = CatOID(&(mibObjPtr->oid), keyInst)) == NULL)
    {
        return(FALSE);
    }

    vb_ptr = SnmpObjGet(NEXT, var);
    FreeOID(var);

    if (vb_ptr == NULL)
    {
        return(FALSE);
    }

    if (CmpOIDClass(&(mibObjPtr->oid), vb_ptr->name))
    {
        keyInst->length = vb_ptr->name->length - mibObjPtr->oid.length;
        if (keyInst->length >= 0)
        {
            for (i = 0; i < keyInst->length; i++)
                keyInst->oid_ptr[i] = vb_ptr->name->oid_ptr[i + mibObjPtr->oid.length];
        }
        FreeVarBind(vb_ptr);
        return(FALSE);
    }
    
    keyInst->length = vb_ptr->name->length - mibObjPtr->oid.length;
    for (i = 0; i < keyInst->length; i++)
        keyInst->oid_ptr[i] = vb_ptr->name->oid_ptr[i + mibObjPtr->oid.length];
    FreeVarBind(vb_ptr);
    
    return(TRUE);
}
 
/* ------------------------------------------------------------
 * Name:         void freeMibVar(pCLIMIBAPIOBJ_t apiObjPtr,
 *                           U32_t  *val, 
 *                           U32_t  item)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
void
freeMibVar(pCLIMIBAPIOBJ_t apiObjPtr,
           U32_t  *val, 
           U32_t  item)
{

    while (item)
    {
        item--;
        switch(apiObjPtr->apiObject->objPtr->oidtype)
        {
        case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
            FreeOID((OID *)val[item]);
            break;
        case OCTET_PRIM_TYPE:
        case IP_ADDR_PRIM_TYPE:
        case OPAQUE_PRIM_TYPE:
            FreeOctetString((OctetString *)val[item]);
            (OctetString *)val[item] = NULL;
            break;
        }
    }
}
        


/* ------------------------------------------------------------
 * Name:         STATUS_t mibObjShow(pMIBDSPENTITY_t dspObjPtr,
 *                          VarBind *vb_ptr,
 *                          U32_t   *lineNum)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
mibObjShow(pMIBDSPENTITY_t dspObjPtr,
           VarBind *vb_ptr,
           U32_t   *lineNum)
{
    int  value;

    switch (vb_ptr->value.type) {
    case COUNTER_TYPE:		/* handle unsigned integers including
				 * COUNTER_32 */
    case GAUGE_TYPE:		/* includes GAUGE_32 */
    case TIME_TICKS_TYPE:
    case INTEGER_TYPE:		/* handle signed integers including */
        value = vb_ptr->value.ul_value;
        if (dspObjPtr->dspType == DSP_STRING)
	{
            U8_t             *str;
            pCLIUMAPSTRUC_t  mapPtr;
            U32_t            i;

            mapPtr = (pCLIUMAPSTRUC_t)(dspObjPtr->dspMibEntry->mapFunc);
            for (i = dspObjPtr->dspMibEntry->mapCnt; i; i--, mapPtr++)
	    {
                if ((int)mapPtr->val == value)
		{
                    str = mapPtr->mapStr;
                    break;
		}
	    }
            if (!i)
            {
                str = "*";
            }

            if (printLineCheck(dspObjPtr->format, str, 
               lineNum) == FALSE)
                return(ERROR);
            printf(dspObjPtr->format, str);
	}
        else
	{
            if (printLineCheck(dspObjPtr->format, NULL, 
                lineNum) == FALSE)
                return(ERROR);
            printf(dspObjPtr->format, value);
	}

	break;

    case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
    {
        OID   *oid;
        U8_t  buf[100];
        int   i;
        U32_t len = 0;

	oid = vb_ptr->value.oid_value;
        for (i = 0; i < oid->length; i++)
	{
            sprintf(&buf[len], "%u.", oid->oid_ptr[i]);
            len += strlen(&buf[len]);
	}
        buf[len-1] = '\0';
        if (printLineCheck(dspObjPtr->format, buf, 
            lineNum) == FALSE)
            return(ERROR);
        printf(dspObjPtr->format, buf);
    }
        break;
#ifdef SR_SNMPv2_PDU
    case COUNTER_64_TYPE:	/* handle large unsigned integers */
        value = vb_ptr->value.uint64_value;
        if (printLineCheck(dspObjPtr->format, NULL, 
           lineNum) == FALSE)
            return(ERROR);
        printf(dspObjPtr->format, value);
	break;
#endif /* SR_SNMPv2_PDU */
    case OCTET_PRIM_TYPE:
    case OPAQUE_PRIM_TYPE:
    {
        OctetString  *os;
        U8_t  buf[100];

        os = vb_ptr->value.os_value;
        if (dspObjPtr->dspMibEntry->argType == ARG_ENUM)
	/* bit map type */
	{
             
            pCLIUMAPSTRUC_t pUmap;
            U32_t           len = 0;
            U32_t           i;

            value = 0;
            for (i = 0; i < os->length; i++)
	    {
               value |= (os->octet_ptr[i] << ((3-i)*8));
	    }
            if (!value)
	    {
               printf(dspObjPtr->format, "");
               break;
	    }
            pUmap = (pCLIUMAPSTRUC_t)dspObjPtr->dspMibEntry->mapFunc;
            for (i = 0; i < 32 && value; i++, value <<= 1)
	    {
               if (value & 0x80000000)
	       {
                  if (!len)
                     sprintf(buf,"%s", pUmap[i].mapStr);
                  else
                     sprintf(&buf[len],"&%s", pUmap[i].mapStr);
                  len += strlen(&buf[len]);
	       }
	    }
            printf(dspObjPtr->format, buf);
	}
        else
	{
           strncpy(buf, os->octet_ptr, os->length);
        
/* DEBUG */
/* 
printf("os->length = %d \n", os->length);
printf("os->octet_ptr[0] = %c and os->octet_ptr[1] = %c \n", os->octet_ptr[0], os->octet_ptr[1]); 
*/

           buf[os->length] = '\0';
           if (printLineCheck(dspObjPtr->format, buf, 
               lineNum) == FALSE)
               return(ERROR);
           printf(dspObjPtr->format, buf);
	}

    }
	break;
    case IP_ADDR_PRIM_TYPE:
    {
        OctetString  *os;
        U8_t  buf[100];
        int   i;
        U32_t len = 0;

        os = vb_ptr->value.os_value;
        for (i = 0; i < os->length; i++)
	{
            sprintf(&buf[len], "%u.", os->octet_ptr[i]);
            len += strlen(&buf[len]);
	}
        buf[len-1] = '\0';
        if (printLineCheck(dspObjPtr->format, buf, 
            lineNum) == FALSE)
            return(ERROR);
        printf(dspObjPtr->format, buf);

    }
        break;
    case NULL_TYPE:
	break;
    default:
	return (ERROR);
    };				/* end of switch */
   return(OK);

}




/* ------------------------------------------------------------
 * Name:         U8_t *strskip(U8_t *s1, U8_t skipCh)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *strskip(U8_t *s1, U8_t skipCh)
{
    register U8_t ch;

    while ((ch = *s1) && ch == skipCh)
        s1++;
    if (ch)
        return(s1);
    return(NULL);
}

/* ------------------------------------------------------------
 * Name:         U8_t *strccpy(U8_t *s1, U8_t *s2, U8_t endCh)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *strccpy(U8_t *s1, U8_t *s2, U8_t endCh)
{
    register U8_t ch;

    while ((ch = *s1 = *s2) && ch != endCh)
    {
        s1++;
        s2++;
    }
    if (ch)
    {
        *s1 = '\0';
        return(s2);
    }
    return(NULL);

}

/* ------------------------------------------------------------
 * Name:         U8_t *nextOptArgGet(U8_t *argListPtr, 
 *                         U8_t *optPtr, 
 *                         U8_t *argPtr)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *
nextOptArgGet(U8_t *argListPtr, 
              U8_t *optPtr, 
              U8_t *argPtr)
{
    U8_t   seperator;
    U32_t  pos = 0;
    U32_t  optFound = FALSE;

    if (optPtr)
        optPtr[0] = '\0';

    argPtr[0] = '\0';
    while (*argListPtr == ' ')
        argListPtr++;
    if (!(seperator = *argListPtr))
        return(NULL);
    if ( optPtr &&
        (seperator == '-' || seperator == '/') )
    {
        /* skip the leading space */
        argListPtr = strskip(argListPtr + 1, ' ');
        if (!argListPtr)
            return(NULL);
        optFound = TRUE;
    }
    
    if (optFound && !isdigit(*argListPtr))
    {
        seperator = '/';
        argListPtr = strccpy(optPtr, argListPtr, ' ');
        if (!argListPtr)
            return(NULL);
    
        /* skip the leading space */
        argListPtr = strskip(argListPtr + 1, ' ');
        if (!argListPtr)
            return(NULL);
    
        if (*argListPtr == '/')
            return(argListPtr);
    }
    
    
    if (seperator == '-')
    {
        argPtr[0] = '-';
        pos++;
    }
    argListPtr = strccpy(&argPtr[pos], argListPtr, ' ');

    return(argListPtr);
}

/* ------------------------------------------------------------
 * Name:         U32_t optFindMibEnt(U8_t  *optPtr, 
 *                         pCLICFGTBL_t    mibCfgList,
 *                         pCLIMIBENTITY_t *mibEntPtr)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U32_t
optFindMibEnt(U8_t            *optPtr, 
              pCLICFGTBL_t    mibCfgList,
              pCLIMIBENTITY_t *mibEntPtr)
{
    pCLIMIBENTITY_t *mibObjPtr;
    U32_t           tbl;
    U32_t           ent;

    for (tbl = 0; tbl < mibCfgList->tblCnt; tbl++)
    {
        mibObjPtr = mibCfgList->mibTblList[tbl]->mibList;

        for (ent = 0; ent < mibCfgList->mibTblList[tbl]->numOfMibEnt;
            ent++)
	{
            if (!strcmp(optPtr, mibObjPtr[ent]->opt))
	    {
                *mibEntPtr = mibObjPtr[ent];
                return(tbl);
	    }
	}
    }
    return(ERROR);
}


/* ------------------------------------------------------------
 * Name:         STATUS_t ParseArg2Val(U8_t *argPtr,
 *                           pCLIMIBENTITY_t mibEntPtr, 
 *                           ObjectInfo      *objPtr,
 *                           U32_t *val)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
ParseArg2Val(U8_t *argPtr,
             pCLIMIBENTITY_t mibEntPtr, 
             ObjectInfo      *objPtr,
             U32_t *val)
{
    pCLIUMAPSTRUC_t mapDataPtr;      
    U32_t           pos;
    U8_t            *strPtr;

    switch(mibEntPtr->argType)
    {
    case ARG_ENUM:

        if (argPtr[0] == '+' || argPtr[0] == '-')
	{
            if (argPtr[0] == '-' &&
                objPtr->oidtype != INTEGER_TYPE)
                return(ERROR);
            pos = 1;
            while (argPtr[pos] == ' ')
                pos++;
            if (argPtr[pos] != '\0')
                strPtr = str2val(&argPtr[1], val);
            else
                return(ERROR);
            if (argPtr[0] == '-')
                *val = -*val;
	}
        else if (objPtr->oidtype == OCTET_PRIM_TYPE)
	/* bit map */
        {
            U32_t value;

            strPtr = str2val(argPtr, val);
            value = 0;
*val;

#if 0
            for (pos = 0; pos < 31; pos++, *val >>= 1, value <<= 1)
            {
               if (*val & 1)
                  value |= 1;
	    }
            if (*val & 1)
               value |= 1;
#else
            value = (0x1000000) << (8 - *val);
#endif

            *val = (U32_t)MakeOctetString(&value, 1);
            if (*val == 0)
                return(ERROR);
	}
        else
            strPtr = str2val(argPtr, val);

        if (strPtr == NULL)
            return(ERROR);
        if (strPtr[0] != '\0')
            return(ERROR);
        return(OK);
    case ARG_STR:
        switch (objPtr->oidtype)
	{
        case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
	    *val = (U32_t)MakeOIDFromText(argPtr);
            if (*val == 0)
                return(ERROR);
            return(OK);
        case OCTET_PRIM_TYPE:
        case OPAQUE_PRIM_TYPE:
            *val = (U32_t)MakeOctetStringFromText(argPtr);
            if (*val == 0)
                return(ERROR);
            return(OK);
        case IP_ADDR_PRIM_TYPE:
	    *val = (U32_t)MakeIPFromText(argPtr);
            if (*val == 0)
                return(ERROR);
            return(OK);
	}
        return(ERROR);
    case ARG_CONV:
        mapDataPtr = (pCLIUMAPSTRUC_t)(mibEntPtr->mapFunc);
        for (pos = mibEntPtr->mapCnt; pos; pos--, mapDataPtr++)
	{
            if (!strcmp(argPtr, mapDataPtr->mapStr))
	    {
                *val = mapDataPtr->val;
                return(OK);
	    }
	}
        return(ERROR);
    case ARG_FUNC:
        return(((MIBARGFUNC_t)(mibEntPtr->mapFunc))(mibEntPtr, argPtr, val));
        break;
    }
    return(ERROR);
}

/* ------------------------------------------------------------
 * Name:         STATUS_t Value2Inst(ObjectInfo *objPtr,
 *                           U32_t      value,
 *                           OID        *inst)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
Value2Inst(ObjectInfo *objPtr,
           U32_t      value,
           OID        *inst)
{
    U32_t len = inst->length;

    switch (objPtr->oidtype) {
    case COUNTER_TYPE:		/* handle unsigned integers including
				 * COUNTER_32 */
    case GAUGE_TYPE:		/* includes GAUGE_32 */
    case TIME_TICKS_TYPE:
    case INTEGER_TYPE:		/* handle signed integers including */
        inst->oid_ptr[len] = value;
        inst->length = len + 1;
	break;
    case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
    {
        OID   *oid;
        int   i;

        oid = (OID *)value;
        for (i = 0; i < oid->length; i++)
            inst->oid_ptr[len++] = oid->oid_ptr[i];

        inst->length = len;
        FreeOID(oid);
    }
        break;
#ifdef SR_SNMPv2_PDU
    case COUNTER_64_TYPE:	/* handle large unsigned integers */
        inst->oid_ptr[len] = value;
        inst->length = len + 1;
	break;
#endif /* SR_SNMPv2_PDU */
    case OCTET_PRIM_TYPE:
    case IP_ADDR_PRIM_TYPE:
    case OPAQUE_PRIM_TYPE:
    {
        OctetString  *os;
        int          i;

        os = (OctetString *)value;
        for (i = 0; i < (int)os->length; i++)
            inst->oid_ptr[len++] = os->octet_ptr[i];

        inst->length = len;
        FreeOctetString(os);
    }
	break;
    case NULL_TYPE:
	break;
    default:
	return (ERROR);
    };				/* end of switch */
   return(OK);
}

/* ------------------------------------------------------------
 * Name:         STATUS_t arg2MibKeyInst(U8_t *argPtr, 
 *                           pCLIMIBENTITY_t mibEntPtr, 
 *                           ObjectInfo      *objPtr,
 *                           OID *keyInst)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
arg2MibKeyInst(U8_t *argPtr, 
               pCLIMIBENTITY_t mibEntPtr, 
               ObjectInfo      *objPtr,
               OID *keyInst)
{
    U32_t  val;

    if (ParseArg2Val(argPtr, mibEntPtr, objPtr, &val) == ERROR)
        return(ERROR);

    if (Value2Inst(objPtr, val, keyInst) == ERROR)
        return(ERROR); 

    return(OK);
}


/* ------------------------------------------------------------
 * Name:         STATUS_t cliDescPrint(pCLIMIBENTITY_t mibObjPtr, 
 *                           U32_t *lineNum)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
STATUS_t
cliDescPrint(pCLIMIBENTITY_t mibObjPtr, 
             U32_t *lineNum)
{
    U32_t ln;
    pCLIUMAPSTRUC_t  mapInfPtr;
    U32_t i;
    U32_t len, len1;

    if (printLineCheck("%s", mibObjPtr->objDesc, 
        lineNum) == FALSE)
        return(FALSE);
    ln = *lineNum + 2;
    if (mibObjPtr->argHelpMsg)
        printf("  %s: %s\n", mibObjPtr->objName, mibObjPtr->argHelpMsg);
    else if (mibObjPtr->mapFunc)
    {
        if (mibObjPtr->argType == ARG_CONV)
	{
            mapInfPtr = (pCLIUMAPSTRUC_t)mibObjPtr->mapFunc;
            printf("  %s: <%s>  %s", mibObjPtr->objName, argTypeStr[mibObjPtr->argType],
                  mapInfPtr->mapStr);
            len = strlen(mibObjPtr->objName) + 7;
            len += strlen(argTypeStr[mibObjPtr->argType]);
            len += strlen(mapInfPtr->mapStr);
            mapInfPtr++;
            for (i = 1; i < mibObjPtr->mapCnt; i++, mapInfPtr++)
	    {
                len1 = strlen(mapInfPtr->mapStr) + 2;
                if ((len  + len1) >= MAX_DSP_COL)
		{
                    printf(",\n      %s", mapInfPtr->mapStr);
                    len = len1 + 4;
		}
                else
		{
                    printf(", %s", mapInfPtr->mapStr);
                    len += len1;
		}
            }
            printf("\n");
	}
        else if (mibObjPtr->argType == ARG_FUNC)
	{
            printf("  %s: <%s>\n", mibObjPtr->objName, argTypeStr[mibObjPtr->argType]);

	}
        else if (mibObjPtr->mapCnt)
	{
            mapInfPtr = (pCLIUMAPSTRUC_t)mibObjPtr->mapFunc;
            printf("  %s: <%s>  %3u: %s", mibObjPtr->objName, argTypeStr[mibObjPtr->argType],
               mapInfPtr->val,  mapInfPtr->mapStr);
            len = strlen(mibObjPtr->objName) + 12;
            len += strlen(argTypeStr[mibObjPtr->argType]);
            len += strlen(mapInfPtr->mapStr);
            mapInfPtr++;
            for (i = 1; i < mibObjPtr->mapCnt; i++, mapInfPtr++)
	    {
                len1 = strlen(mapInfPtr->mapStr) + 7;
                if ((len  + len1) >= MAX_DSP_COL)
		{
                    printf(",\n      %3u: %s", mapInfPtr->val, mapInfPtr->mapStr);
                    len = len1 + 4;
		}
                else
		{
                    printf(", %3u: %s",  mapInfPtr->val, mapInfPtr->mapStr);
                    len += len1;
		}
	    }
            printf("\n");

	}
        else
            printf("  %s: <%s> %s\n", mibObjPtr->objName, argTypeStr[mibObjPtr->argType],
                 (U8_t *)mibObjPtr->mapFunc);
          

    }
    else
        printf("  %s: <%s>\n", mibObjPtr->objName, argTypeStr[mibObjPtr->argType]);
    printf("         %s\n", mibObjPtr->objDesc);
    *lineNum = ln;
    return(TRUE);
}

/* ------------------------------------------------------------
 * Name:         void cliMibCfgHelp(pCLICFGTBL_t mibCfgList)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
void
cliMibCfgHelp(pCLICFGTBL_t mibCfgList)
{
    pCLIMIBCFGGROUP_t mibCfgTbl;
    pCLIMIBENTITY_t   *mibObjPtr;
    U32_t  tbl, item;
    U32_t  len, len1;
    U32_t  lineNum = 0;

    printf("%s ", mibCfgList->cfgName);
    len = strlen(mibCfgList->cfgName) + 1;
    for (tbl = 0; tbl < mibCfgList->tblCnt; tbl++)
    {
        mibCfgTbl = mibCfgList->mibTblList[tbl];
        mibObjPtr = mibCfgTbl->mibKeyList;

        for (item = 0; item < mibCfgTbl->keyLen; item++)
	{
            len1 = strlen(mibObjPtr[item]->objName) +
                   strlen(mibObjPtr[item]->opt) + 5;
            if ((len + len1) >= MAX_DSP_COL)
	    {
                printf("\n    -%s <%s> ", mibObjPtr[item]->opt,
                    mibObjPtr[item]->objName);
                len = len1 + 4;
                lineNum++;
	    }
            else
	    {
                printf("-%s <%s> ", mibObjPtr[item]->opt,
                    mibObjPtr[item]->objName);
                len += len1;
	    }
	}

        mibObjPtr = mibCfgTbl->mibList;
        for (item = 0; item < mibCfgTbl->numOfMibEnt; item++)
	{
            len1 = strlen(mibObjPtr[item]->objName) +
                   strlen(mibObjPtr[item]->opt) + 5;
            if ((len + len1) >= MAX_DSP_COL)
	    {
                printf("\n    -%s <%s> ", mibObjPtr[item]->opt,
                    mibObjPtr[item]->objName);
                len = len1 + 4;
                lineNum++;
	    }
            else
	    {
                printf("-%s <%s> ", mibObjPtr[item]->opt,
                    mibObjPtr[item]->objName);
                len += len1;
	    }
	}
    }

    lineNum++;
    printf("\n");

    for (tbl = 0; tbl < mibCfgList->tblCnt; tbl++)
    {
        mibCfgTbl = mibCfgList->mibTblList[tbl];
        mibObjPtr = mibCfgTbl->mibKeyList;

        for (item = 0; item < mibCfgTbl->keyLen; item++)
	{
            if (cliDescPrint(mibObjPtr[item], &lineNum) == FALSE)
                return;
	}
        mibObjPtr = mibCfgTbl->mibList;

        for (item = 0; item < mibCfgTbl->numOfMibEnt; item++)
	{
            if (cliDescPrint(mibObjPtr[item], &lineNum) == FALSE)
                return;
	}
    }
}


/* ------------------------------------------------------------
 * Name:         void cliApiCfgHelp(pCLIMIBAPIGROUP_t apiCfgTbl)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
void
cliApiCfgHelp(pCLIMIBAPIGROUP_t apiCfgTbl)
{
    pCLIMIBAPIOBJ_t apiObjPtr;
    U32_t  len, len1;
    U32_t  lineNum = 0;

    apiObjPtr = apiCfgTbl->apiList;
    printf("%s ", apiCfgTbl->name);
    len = strlen(apiCfgTbl->name) + 1;
    while (apiObjPtr->apiObject)
    {
        len1 = strlen(apiObjPtr->apiObject->objName) + 3;
        if ((len + len1) >= MAX_DSP_COL)
	{
            printf("\n    <%s>%c", apiObjPtr->apiObject->objName,
               apiObjPtr->seperator);
            len = len1 + 4;
            lineNum++;
	}
        else
	{
            printf("<%s>%c", apiObjPtr->apiObject->objName,
               apiObjPtr->seperator);
            len += len1;
	}

        apiObjPtr++;
    }
    printf("\n");
    lineNum++;

    apiObjPtr = apiCfgTbl->apiList;
    while (apiObjPtr->apiObject)
    {
        if (cliDescPrint(apiObjPtr->apiObject, &lineNum) == FALSE)
            return;
        apiObjPtr++;
    }
}


/* ------------------------------------------------------------
 * Name:         void  cliDspHelp(pCLIMIBDSPGROUP_t dspGrpPtr)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
void
cliDspHelp(pCLIMIBDSPGROUP_t dspGrpPtr)
{
    pCLIMIBCFGGROUP_t mibCfgTbl;
    pCLIMIBENTITY_t   *mibObjPtr;
    U32_t  item;
    U32_t  len, len1;
    U32_t  lineNum = 0;

    printf("%s ", dspGrpPtr->name);
    len = strlen(dspGrpPtr->name) + 1;

    mibCfgTbl = dspGrpPtr->mibCfgGroup;
    mibObjPtr = mibCfgTbl->mibKeyList;
    
    for (item = 0; item < mibCfgTbl->keyLen; item++)
    {
        len1 = strlen(mibObjPtr[item]->objName) + 3;
        if ((len + len1) >= MAX_DSP_COL)
        {
            printf("\n    <%s> ", mibObjPtr[item]->objName);
            len = len1 + 4;
            lineNum++;
        }
        else
        {
            printf("<%s> ", mibObjPtr[item]->objName);
            len += len1;
        }
    }
    mibObjPtr = mibCfgTbl->mibKeyList;
    printf("\n");
    lineNum++;
    
    for (item = 0; item < mibCfgTbl->keyLen; item++)
    {
        if (cliDescPrint(mibObjPtr[item], &lineNum) == FALSE)
            return;
    }

}

/*
 * MakeOIDFromText:
 *
 *   This routine is used to create a library format Object Identifier
 *   data structure for use by calls to MakeVarBind() and
 *   MakeAuthentication() from hex text strings.  The format of these
 *   strings is a hex value (1 or more hex digits, upper or lower
 *   case) followed by a space, with more hex values and spaces to
 *   finish out the string.  For example, to create an Object
 *   Identifier consisting of three sub-identifiers (say, 1.21.51)
 *   text string could be "1 15 3d" or "0x1 0x15 0x3D".  The Object
 *   Identifier construct returned by this call can be free'ed by a
 *   call to FreeOID.  This is usually unnecessary as the construct is
 *   most often passed to another library routine for inclusion in a
 *   larger ASN.1 construct and that library's free counterpart will
 *   take care of memory recovery.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_obj_id_from_hex().
 */

OID *
MakeOIDFromText(text_str)
U8_t  *text_str;
{
    OID            *oid_ptr;
    int             i;
    U8_t           *temp_ptr;
    U32_t          value;

    if ((oid_ptr =
	 MakeOID(NULL, ((int)strlen((char *) text_str) / 2) + 1)) == NULL) {
	return (NULL);
    }

    for (i = 0, temp_ptr = text_str; (i < 40) && (temp_ptr[0] != '\0'); i++) 
    {
        if ((temp_ptr = str2val(temp_ptr, &value)) == NULL)
	{
            FreeOID(oid_ptr);
            return NULL;
	}

	oid_ptr->oid_ptr[i] = value;

	while (temp_ptr[0] == ' ') {
	    temp_ptr++;
        }
    }

    oid_ptr->length = i;
    return (oid_ptr);
}

/*
 * MakeIPFromText:
 *
 *   This routine is used to create a library format Object Identifier
 *   data structure for use by calls to MakeVarBind() and
 *   MakeAuthentication() from hex text strings.  The format of these
 *   strings is a hex value (1 or more hex digits, upper or lower
 *   case) followed by a space, with more hex values and spaces to
 *   finish out the string.  For example, to create an Object
 *   Identifier consisting of three sub-identifiers (say, 1.21.51)
 *   text string could be "1 15 3d" or "0x1 0x15 0x3D".  The Object
 *   Identifier construct returned by this call can be free'ed by a
 *   call to FreeOID.  This is usually unnecessary as the construct is
 *   most often passed to another library routine for inclusion in a
 *   larger ASN.1 construct and that library's free counterpart will
 *   take care of memory recovery.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_obj_id_from_hex().
 */

OctetString *
MakeIPFromText(text_str)
U8_t  *text_str;
{
    OctetString    *os_ptr;
    int             i;
    U8_t           *temp_ptr;
    U32_t          value;

    if ((os_ptr = 
	 MakeOctetString(NULL, 40)) == NULL) {
	return (NULL);
    }

    for (i = 0, temp_ptr = text_str; (temp_ptr[0] != '\0'); i++) 
    {
        if ((temp_ptr = str2val(temp_ptr, &value)) == NULL)
	{
            FreeOctetString(os_ptr);
            return NULL;
	}
        if (value > 0xff)
	{
            FreeOctetString(os_ptr);
            return NULL;
	}
	os_ptr->octet_ptr[i] = value;

	while (temp_ptr[0] == ' ') {
	    temp_ptr++;
        }
    }
    if (i < 1)
    {
        FreeOctetString(os_ptr);
        return NULL;
    }

    os_ptr->length = i;
    return (os_ptr);
}

/* ------------------------------------------------------------
 * Name:        U8_t *str2val(U8_t  *text_str,
 *                       U32_t *val)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *
str2val(U8_t  *text_str,
        U32_t *val)
{
    U32_t base;
    U32_t value;

    if ((text_str[0] == '0'))
    { 
        if ((text_str[1] == 'x') || (text_str[1] == 'X')) 
        {
            text_str += 2;
            base = 16;
        }
        else if ((text_str[1] == 'b') || (text_str[1] == 'B')) 
        {
            text_str += 2;
            base = 2;
        }
        else 
            base = 8;
    }
    else if (!isdigit(text_str[0]) && (text_str[1] == '\0'))
    /*
     * If input is "." or non-digit char 
     */
    {
        return (NULL);
    }
    else
    {
        base = 10;
    }

    value = 0;
    while (1) {
        if (base < 10)
        {
            if ((text_str[0] >= '0') && (text_str[0] <= (base + '0'))) 
            {
                value = (value * base) + text_str[0] - '0';
                text_str++;
                continue;
    	    }
    
        }
        else if ((text_str[0] >= '0') && (text_str[0] <= '9')) 
        {
            value = (value * base) + text_str[0] - '0';
            text_str++;
            continue;
        } 
        else if (base == 16)
        {
            if ((text_str[0] >= 'a') && (text_str[0] <= 'f')) {
                value = (value * 16) + text_str[0] - 'a' + 10;
                text_str++;
                continue;
            } else
            if ((text_str[0] >= 'A') && (text_str[0] <= 'F')) {
                value = (value * 16) + text_str[0] - 'A' + 10;
                text_str++;
                continue;
            } 
        } 
    
        if (text_str[0] == '\0') 
        {
            *val = value;
            return(text_str);
        }

        if (text_str[0] != ' ' && text_str[0] != '.') {
            return NULL;
        } else {
            if (text_str[0] == '.')
                text_str++;
    
            while (text_str[0] == ' ')
                text_str++;
            *val = value;

            return(text_str);
        }
    }
}
/* ------------------------------------------------------------
 * Name:        U8_t *str2valnodot(U8_t  *text_str,
 *                       U32_t *val)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
U8_t *
str2valnodot(U8_t  *text_str,
        U32_t *val)
{
    U32_t base;
    U32_t value;


    if ((text_str[0] == '0'))
    { 
        if ((text_str[1] == 'x') || (text_str[1] == 'X')) 
        {
            text_str += 2;
            base = 16;
        }
        else if ((text_str[1] == 'b') || (text_str[1] == 'B')) 
        {
            text_str += 2;
            base = 2;
        }
        else 
            base = 8;
    }
    else if (!isdigit(text_str[0]) && (text_str[1] == '\0'))
    /*
     * If input is "." or non-digit char 
     */
    {
        return (NULL);
    }
    else
    {
        base = 10;
    }

    value = 0;
    while (1) {
        if (base < 10)
        {
            if ((text_str[0] >= '0') && (text_str[0] <= (base + '0'))) 
            {
                value = (value * base) + text_str[0] - '0';
                text_str++;
                continue;
    	    }
    
        }
        else if ((text_str[0] >= '0') && (text_str[0] <= '9')) 
        {
            value = (value * base) + text_str[0] - '0';
            text_str++;
            continue;
        } 
        else if (base == 16)
        {
            if ((text_str[0] >= 'a') && (text_str[0] <= 'f')) {
                value = (value * 16) + text_str[0] - 'a' + 10;
                text_str++;
                continue;
            } else
            if ((text_str[0] >= 'A') && (text_str[0] <= 'F')) {
                value = (value * 16) + text_str[0] - 'A' + 10;
                text_str++;
                continue;
            } 
        } 
    
        if (text_str[0] == '\0') 
        {
            *val = value;
            return(text_str);
        }
        if (text_str[0] != ' ') {

            return NULL;
        } else {
            while (text_str[0] == ' ')
                text_str++;
            *val = value;

            return(text_str);
        }
    }
}


/* ------------------------------------------------------------
 * Name:        VarBind *SnmpObjGet(U32_t searchType,
 *                       OID *var)
 *
 * Description:  
 *
 * In Parms:     
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */
VarBind *
SnmpObjGet(U32_t searchType,
           OID *var)
{
    U32_t payload;
    U32_t retCode;

#ifdef SR_SNMPv2_PDU
    payload = SR_SNMPv2_PDU_PAYLOAD;
#else  /* SR_SNMPv2_PDU */
    payload = SR_SNMPv1_PDU_PAYLOAD;
#endif /* SR_SNMPv2_PDU */


    if (searchType == EXACT) {
        retCode = GetExactObjectInstance(var, NULL, NULL, serialNum, payload);
        return(retCode);
    } else {
        retCode = GetNextObjectInstance(var, NULL, NULL, serialNum, payload);
        return(retCode);
    }
}

/* ------------------------------------------------------------
 * Name:         STATUS_t MibShowFlt()
 *
 * Description:  Flt show mib objects
 *
 * In Parms:     argListPtr
 *               apiCfgTbl (for flt)
 *               dspGrpPtr (for display)
 *          
 * Out Parms:    Ok or ERROR
 *
 * ------------------------------------------------------------
 */

STATUS_t
MibShowFlt(
	U8_t               *argListPtr,
	pCLIMIBAPIGROUP_t  apiCfgTbl,
        pCLICFGTBL_t       mibCfgList,
	pCLIMIBDSPGROUP_t  dspGrpPtr)
{

    U8_t                fltOptStr[500];
    U8_t                *endPtr;
    U32_t               len;
    pCLIMIBAPIOBJ_t     apiObjPtr;
    OID                 keyInst[10];
    U32_t               instBuf[10][160];
    U32_t               tbl, key;
    U8_t                optStr[150], argStr[150];
    U8_t                *keyPtr;
    pCLIMIBCFGGROUP_t   *mibTblList;
    U32_t               keyFound = FALSE;
    pCLIMIBENTITY_t     mibObjPtr;
    ObjectInfo          *object;
    OID                 dspKeyInst;
    U32_t               dspInstBuf[100];
    pCLIMIBCFGGROUP_t   cfgTblPtr;
    U32_t               lineNum = 0;
    int                 val;
    int                 keyIdx;
    int                 indexFltOK = 0;

    
    apiObjPtr = apiCfgTbl->apiList;

    if (!argListPtr)
    {
        MibShow(NEXT, argListPtr, dspGrpPtr);
        return(OK);
    }
    else if (!strcmp(argListPtr, "help"))
    {
        cliApiCfgHelp(apiCfgTbl);
        return(OK);
    }    

    if (argListPtr)
        argListPtr = strskip(argListPtr, ' ');

    len = 0;

    while (1)
    {
        if (apiObjPtr->apiObject)
	{
            if (argListPtr && *argListPtr != '\0')
	    {
                endPtr = strchr(argListPtr, apiObjPtr->seperator);
        
                if (endPtr)
                    *endPtr = '\0';

                if (*argListPtr != '#')
		{
                    sprintf(&fltOptStr[len], "-%s %s ", apiObjPtr->apiObject->opt, 
                          argListPtr);
                    len += strlen(&fltOptStr[len]);
		}
                if (endPtr)
                    argListPtr = strskip(endPtr+1, ' ');
                else
                    argListPtr = endPtr;
	    }
            else
                break;
	}
        else if (argListPtr)
	{
            printf("CLI - too many parameters! 
                    For help type %s <Enter>\n", apiCfgTbl->name);
            return(ERROR);
	}
        else
            break;
        apiObjPtr++;
    }

    strcpy(&fltOptStr[len], apiCfgTbl->optValStr);

    if (!fltOptStr)
    {
        cliMibCfgHelp(mibCfgList);
        return(OK);
    }

    mibTblList = mibCfgList->mibTblList;

    for (tbl = 0; tbl < mibCfgList->tblCnt; tbl++, mibTblList++)

    /* search for mib index(key) */
    {
        keyInst[tbl].length = 0;
        keyInst[tbl].oid_ptr = instBuf[tbl];

        for (key = 0; key < mibTblList[0]->keyLen; key++)
	{ 
            mibObjPtr = mibTblList[0]->mibKeyList[key];
            keyPtr = fltOptStr;
            /* looking for table index */
            while ((keyPtr = strstr(keyPtr, mibObjPtr->opt)))
	    {
	        do 
                {
                    keyPtr--;
                } while (keyPtr != fltOptStr && *keyPtr == ' ');
                
                endPtr = nextOptArgGet(keyPtr, optStr, argStr);

                if (strcmp(optStr, mibObjPtr->opt))
                {
                    keyPtr = endPtr;
                    if (!keyPtr)
                        break;
                    continue;
                }

                if (argStr[0] == '\0')
	        {
                    printf("CLI - syntax error: missing option argument at -%s For help type %s <Enter>\n", optStr, mibCfgList->cfgName);
                    return(ERROR);
	        }
                if (argStr[0] == '.')
                {
                    if (apiCfgTbl)
                        printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, apiCfgTbl->name);
                    else
                        printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, mibCfgList->cfgName);
                    return(ERROR);
                }

                if (!mibObjPtr->objPtr)
		{
                    val = find_object(&(mibObjPtr->oid), EXACT);
                    if (val == -1)
	            {
                        printf("CLI - system error: illegal Object %s\n", mibObjPtr->objName);
                                return(ERROR);
	            }                  
                    mibObjPtr->objPtr = &OidList[val];
		}

                object = mibObjPtr->objPtr;
 
                if (arg2MibKeyInst(argStr, mibObjPtr, object, &keyInst[tbl]) == ERROR)
	        {
                    if (apiCfgTbl)
                        printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, apiCfgTbl->name);
                    else
                        printf("CLI - illegal argument type for object %s For help type %s <Enter>\n", mibObjPtr->objName, mibCfgList->cfgName);
                    return(ERROR);
	        }

                memset(keyPtr, ' ', endPtr - keyPtr);
                keyFound = TRUE;
                break;
	    }/* end while */

            if (!keyPtr)
	    {
                if (!key)
                    break;
                else
	        {
                    if (apiCfgTbl)
                        printf("CLI - missing index %s\n For help type %s <Enter>\n", mibObjPtr->objName, apiCfgTbl->name);
                    else
                        printf("CLI - missing index %s\n For help type %s <Enter>\n", mibObjPtr->objName, mibCfgList->cfgName);
                    return(ERROR);
	        }
	    }

	}/* end for key */

        if (!mibTblList[0]->keyLen)
	{
            keyInst[tbl].length = 1;
            keyInst[tbl].oid_ptr[0] = 0;
            keyFound = TRUE;
	}
    }/* end for tbl */

    if (!keyFound)
    {
        if (apiCfgTbl)
            printf("CLI - index not specified\n For help type %s <Enter>\n", apiCfgTbl->name);
        else
            printf("CLI - index not specified\n For help type %s <Enter>\n", mibCfgList->cfgName);
        return(ERROR);
    }

    dspKeyInst.length = 1;
    dspKeyInst.oid_ptr = dspInstBuf;
    cfgTblPtr = dspGrpPtr->mibCfgGroup;
    dspInstBuf[0] = 0;; 
    printf(dspGrpPtr->title);

    do
    {
        if (dspGrpPtr->mibCfgGroup->keyLen)
            mibObjPtr = dspGrpPtr->mibCfgGroup->mibKeyList[0];
        else
            mibObjPtr = dspGrpPtr->mibCfgGroup->mibList[0];

        if ((val = semTake(semM, SECONDS(SHELL_SEM_WAIT_TIME))) != OK)
        {
            if (val == ERROR)
                printf("CLI - SNMP Semaphare error!!!\n");
            else
                printf("CLI - SNMP Agent busy! Try again\n");
            return(ERROR);
        }

        serialNum++;

        if (nextInstGet(mibObjPtr, &dspKeyInst) == FALSE)
	{
            semGive(semM);
            return(OK);
	}
       
        /* check dspKeyInst */

        for (keyIdx = 0; keyIdx < dspKeyInst.length; keyIdx++)
        {
            indexFltOK = 0;

            if (keyInst[0].oid_ptr[keyIdx] == 0)
            {
                indexFltOK = 1;
            }
            else if (dspKeyInst.oid_ptr[keyIdx] <= 1000000)
            {
                if (dspKeyInst.oid_ptr[keyIdx] == keyInst[0].oid_ptr[keyIdx])
                {
                    indexFltOK = 1;
                }
            }
            else
            {
                if (((dspKeyInst.oid_ptr[keyIdx]/1000000) == (keyInst[0].oid_ptr[keyIdx]/1000000)) ||
                    (keyInst[0].oid_ptr[keyIdx]/1000000 == 0))             /* slot OK */
                {
                    if (((dspKeyInst.oid_ptr[keyIdx]%1000000)/1000) == ((keyInst[0].oid_ptr[keyIdx]%1000000)/1000) ||
                        ((keyInst[0].oid_ptr[keyIdx]%1000000)/1000 == 0))  /* line OK */ 
                    {
                        indexFltOK = 1;
                    }
                }
            }
            if (indexFltOK == 0) /* Do not need to check next index */
                break;
        }
               
        if (indexFltOK == 1)
        {
            if (mibTblFltShow(dspGrpPtr, &dspKeyInst, &lineNum, fltOptStr) == ERROR)
                return(ERROR);
        }

        indexFltOK = 0; /* start to check next object */

    } while (1);

}



