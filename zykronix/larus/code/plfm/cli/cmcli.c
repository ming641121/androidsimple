#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef unsigned char  U8_t;
typedef unsigned short U16_t;
typedef unsigned long  U32_t;


#define MAX_API_ARG    15

#define UNKNOWN_TYPE     0
#define CMD_TYPE         1
#define DSP_TYPE         2
#define KEY_TYPE         3
#define OPT_TYPE         4
#define API_TYPE         5
#define FLTDSP_TYPE      6  /* yshen */

#define CONST_OPT        0
#define VAR_OPT          1

#define SYNTAX_UNKNOWN         0
#define SYNTAX_SEQUENCE        1
#define SYNTAX_INTEGER         2
#define SYNTAX_INTEGER_MAP     3
#define SYNTAX_INTEGER_RANGE   4
#define SYNTAX_STRING          5

#define ACCESS_UNKNOWN         0
#define ACCESS_READ_WRITE      1
#define ACCESS_READ_ONLY       2
#define ACCESS_NOT_ACCESSIBLE  3

#define ARG_ENUM      0
#define ARG_STR       1
#define ARG_CONV      2
#define ARG_FUNC     3

#define ARG_DEFAULT            0
#define ARG_MAP                1
#define ARG_UMAP               2
#define ARG_FUNC               3

#define DSP_COLUMN             0
#define DSP_ROW                1

#define DSP_NUM                0
#define DSP_STRING             1
#define DSP_TABLE              2

#define DEFAULT_LEVEL          1
#define DEFAULT_LOGFLAG        0

#define API_ARG_SZ             61


#define ACTIVE_EXC_STATE    4
#define STANDBY_EXC_STATE   8
#define ALL_EXC_STATE       0xff

U8_t *snmpExactNext[] = {
  "NEXT", "EXACT"};

U8_t *dspTypeStr[] = {
      "DSP_NUM", "DSP_STRING", "DSP_TABLE"};

typedef struct {
    U8_t  name[60];
    U8_t  optStr[40];
    U8_t  helpMsg[400];
    U8_t  *objMibTbl;
    U32_t inType;
    U32_t mibType;
    U32_t accessTp;
} MIBENTRYINF_t, *pMIBENTRYINF_t;

typedef struct {
    U32_t  strPos;
    U32_t  datPos;
    U8_t  dspStr[100];
    U8_t  optStr[40];
    U8_t  fmtStr[40];
} MIBENTDSP_t, *pMIBENTDSP_t;

typedef struct {
    U8_t  name[60];
    U8_t  helpMsg[400];
    U8_t  banner[400];
    MIBENTDSP_t  dspEnt[100];
    U32_t cmdType;
    U32_t accessLvl;
    U32_t logFlag;
    U32_t excState;
    U32_t count;
} MIBDSPCMD_t, *pMIBDSPCMD_t;

typedef struct {
    U8_t   optStr[40];
    U8_t   sepCh;
    U8_t   argType;
    U32_t  argNum;
    U8_t   fmtStr[40];
} MIBENTAPI_t, *pMIBENTAPI_t;

typedef struct {
    U8_t  name[60];
    U8_t  helpMsg[400];
    MIBENTAPI_t  apiEnt[100];
    U32_t cmdType;
    U32_t accessLvl;
    U32_t logFlag;
    U32_t excState;
    U32_t count;
} MIBAPICMD_t, *pMIBAPICMD_t;

U8_t *strbchr(U8_t *str,  U8_t ch);
U8_t *strcskip(U8_t *s1, U8_t ch);
U8_t *strpskip(U8_t *s1, U8_t *s2);

U8_t *parseMibTable(U8_t *str, U32_t flag);
U8_t *strcskip(U8_t *s1, U8_t ch);
U8_t *clistrkey(U8_t *buf, U8_t *keyword);
U8_t *cliArgGet(U8_t *buf);
U8_t *clifieldGet(U8_t *buf, 
            U8_t *fieldBuf,
            U8_t *name,
            U8_t *errMsg1,
            U8_t *errMsg2);
U8_t *findObjEnt(U8_t *buf, U8_t *mibName);
U8_t *parseOpt(U8_t *buf, pMIBENTRYINF_t mibInfPtr);
U8_t *parseDsp(U8_t *buf, pMIBDSPCMD_t dspCmdPtr);
U8_t *parseApi(U8_t *buf, pMIBAPICMD_t apiCmdPtr);
U8_t *makeArgStruct(U8_t *str, pMIBENTRYINF_t mibInfPtr, U32_t *argCnt);
U8_t *fileGet(U8_t *fName, U32_t *len, U32_t offset);
U8_t *cmdFieldGet(U8_t *str, U8_t *name, U32_t *accessLvl, 
            U32_t *logFlag, U32_t *excFlag, U8_t  *helpStr);
U8_t *parseFltDsp(U8_t *buf, pMIBAPICMD_t apiCmdPtr, pMIBDSPCMD_t dspCmdPtr);

MIBDSPCMD_t mibDspCmd[100];
U32_t  numOfDspCmd;
MIBAPICMD_t mibApiCmd[100];
U32_t  numOfApiCmd;

/* FLT DISPLAY */
MIBDSPCMD_t mibFltDspCmd[100]; /* yshen */
U32_t  numOfFltDspCmd;
MIBAPICMD_t mibFltCmd[100];
U32_t  numOfFltCmd;



U32_t cnfCmdAccessLvl = DEFAULT_LEVEL;
U32_t cnfCmdLogFlag = DEFAULT_LOGFLAG;
U32_t cnfCmdExcState = 0;

U8_t   cnfCmdName[100];
U8_t   cnfCmdHelpMsg[400];
U8_t   mibTableName[100];
MIBENTRYINF_t  mibEntRecd[100];
MIBENTRYINF_t  mibIndexRecd[10];
U32_t  numOfMibEntry;
U32_t  numOfIndexEntry;
U32_t  numOfCfgCmd;

U8_t   *argTypeStr[] = {
"ARG_ENUM", "ARG_STR", "ARG_CONV", "ARG_FUNC"
};


U8_t  *logFlagStr[] = {
  "NO_LOG", "LOG"};

/*
    &OidList[OI_%s],
    */

U8_t   *mibEntryStr = "static CLIMIBENTITY_t %sENT = {
    OIDvalue(LN%s, ID%s)
    NULL,
    \"%s\",
    %s,
    %u,
    (void *)%s,
    %s,
    \"%s\",
    \"%s\"};\n\n";

U8_t   *mibTableStr = "CLIMIBCFGGROUP_t %sTBL = {
    %s%s,
    %sLST,
    %u,
    %u};\n\n";

U8_t   *mibDspTblStr = "CLIMIBDSPGROUP_t %sDSP = {
  \"%s\",
  &%sTBL,
  %u,
  %sDENT,
  \"%s\"};\n\n";

U8_t *apiObjName = "static CLIMIBAPIOBJ_t %sOPT[] = {";
U8_t *apiObjStr = "    {%45sENT, '%c'},";
U8_t *apicmdtbl = "CLIMIBAPIGROUP_t %sAPI = {
    \"%s\",
    %sOPT,
    \"%s\"};\n\n";

U8_t  *cliCfgFuncStr = "
/* Name:         %s(U8_t *cmdArg)
 *
 * Description:  Configure the mib table %s
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
%s(U8_t *cmdArg)
{
    extern CLICFGTBL_t %sCMTB;

    return(MibCnf(cmdArg, &%sCMTB, NULL));
}\n\n";

U8_t  *cliApiFuncStr = "
/* Name:         %s(U8_t *cmdArg)
 *
 * Description:  Configure the mib %s
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
%s(U8_t *cmdArg)
{
    extern CLICFGTBL_t       %sCMTB;
    extern CLIMIBAPIGROUP_t  %sAPI;

    return(MibApiCnf(cmdArg, &%sAPI, &%sCMTB));
}\n\n";

U8_t  *cliDupFuncStr = "
/* Name:         %s(U8_t *cmdArg)
 *
 * Description:  Configure and duplicate the mib %s
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
%s(U8_t *cmdArg)
{
    extern CLICFGTBL_t       %sCMTB;
    extern CLIMIBAPIGROUP_t  %sAPI;

    return(MibApiDup(cmdArg, &%sAPI, &%sCMTB));
}\n\n";


U8_t  *cliDspFuncStr = "
/* Name:         %s(U8_t *cmdArg)
 *
 * Description:  Show mib objects instance
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
%s(U8_t *cmdArg)
{
    extern CLIMIBDSPGROUP_t %sDSP;

    return(MibShow(%s, cmdArg, &%sDSP));
}\n\n";


/* yshen */

U8_t  *cliDspFltFuncStr = "
/* Name:         %s(U8_t *cmdArg)
 *
 * Description:  Show mib objects by flt
 *
 * In Parms:     U8_t  *cmdArg
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
%s(U8_t *cmdArg)
{
    extern CLICFGTBL_t       %sCMTB;
    extern CLIMIBAPIGROUP_t  %sAPI;
    extern CLIMIBDSPGROUP_t  %sDSP;

    return(MibShowFlt(cmdArg, &%sAPI, &%sCMTB, &%sDSP));
}\n\n";

U8_t  *clishFuncStr = "
/* Name:         MibCmdPlugIn()
 *
 * Description:  plug in the mib command
 *
 * In Parms:     None
 * Out Parms:    OK or ERROR
 *
 */
STATUS_t
MibCmdPlugIn()
{
    extern STATUS_t commandPlugIn(pCMDSTRUCT_t);

    return(commandPlugIn(mibCmdTable));
}\n\n";



U8_t *shExtCmdStr = "extern STATUS_t %s(U8_t *);\n";
U8_t *shCmdTblStr = "    {%-27s, %-25s,   MMS_TYPE, %2u, %8s, 0x%04x, \"%s\"},\n";
U8_t *shellTblStr = "CMDSTRUCT_t mibCmdTable[] = {\n";


FILE *tblFp = stdout;

U8_t *fileName = "frPortTable.my";

U8_t  *fileBuf;
U8_t *clitblName = "mibclitbl.h";
U8_t *apitblName = "mibclitbl.h";
U8_t *cliCmdName = "mibclicmd.c";
U8_t *shellCmdName = "mibclicmdtbl.c";
U8_t mibTblFileName[80];

FILE *keyFp;

U32_t debug = 0;

main(argc,argv)
int argc;
char *argv[];
{
    U32_t len;
    U8_t  *str;
    U32_t i;
    U8_t  *tmpFile;

    fileName = NULL;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == 'd')
                debug = 1;
        }
        else if (!fileName)
            fileName = argv[i];
        else
        {
            fprintf(stdout, "Syntax error!\n");
            exit(-1);
        }
    } 

    if (!fileName)
    {
        fprintf(stdout, "Input file name not specified!!\n");
        exit(-1);
    }

    if ((fileBuf = fileGet(fileName, &len, 1)) == NULL)
    {
        fprintf(stdout, "Can not open %s\n", fileName);
        exit(-1);
    }

    if ((keyFp = fopen("mibindex.tmp", "a+")) == NULL)
    {
        
        if ((keyFp = fopen("mibindex.tmp", "w")) == NULL)
        {
            fprintf(stdout, "Can not open mibindex.tmp\n");
            exit(-1);
        }
    }

    sprintf(mibTblFileName, "tb_%s", fileName);
    str = strchr(mibTblFileName, '.');
    if (str)
        *str = '\0';
    strcat(mibTblFileName, ".h");

    remove(mibTblFileName);

if (debug) {
printf("mibTblFileName = %s\n", mibTblFileName);
}
    i = 1;
    str = &fileBuf[1];
    tmpFile = fileBuf;

    parseMibTrap(str);
    while (str)
    {
        str = parseMibTable(str, i);
        fileBuf = tmpFile;
        i = 0;
if (debug) {
printf("call return with str = %x\n", str);
}
    }
    free(tmpfile);
    fclose(keyFp);
    return(0);
}

U8_t *
parseMibTable(U8_t *str, U32_t flag)
{
    U8_t  *start, *end;
    U8_t  *objTbl;
    U32_t i;
    U32_t cliType;
    U32_t scalarType = 0;
    U8_t  tmpBuf[80];

    if ((start = clistrkey(str, "MMS_CLI_START")) == NULL)
    {
        if (flag)
            fprintf(stdout, "CLI Specifier MMS_CLI_START not found!!\n");
        return(NULL);
    }

    if ((end = clistrkey(start, "MMS_CLI_END")) == NULL)
    {
        fprintf(stdout, "CLI terminator MMS_CLI_END not found!!\n");
        exit(-1);
    }

    str = clifieldGet(start, mibTableName, "", "mib table", "mib table");

if (debug) {
printf("mibTableName = %s\n", mibTableName);
}


    if ((objTbl = findObjEnt(&fileBuf[1], mibTableName)) == NULL)
    {
        scalarType = 1;
	/*
        fprintf(stdout, "Mib table %s not found!!\n", mibTableName);
        exit(-1);
	*/
    }
    else
    {
if (debug) {
printf("findObjEnt = %x\n", objTbl);
}
        if (mibObjSyntax(objTbl) != SYNTAX_SEQUENCE)
        {
            fprintf(stdout, "%s is not a Mib table!!\n", mibTableName);
            exit(-1);
        }
    }
    numOfMibEntry = 0;
    numOfIndexEntry = 0;
    numOfDspCmd = 0;
    numOfApiCmd = 0;
    numOfCfgCmd = 0;
    numOfFltCmd = 0;
    numOfFltDspCmd = 0;

    if ((tblFp = fopen(mibTblFileName, "a+")) == NULL)
    {
        fprintf(stdout, "Open fail on %s\n",  mibTblFileName);
        exit(-1);

    }

    fprintf(tblFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

    for (i = 0; (tmpBuf[i] = toupper(mibTableName[i])); i++)
        ;
    fprintf(tblFp, "#ifndef _TB_%s_H_\n", tmpBuf);
    fprintf(tblFp, "#define _TB_%s_H_\n\n", tmpBuf);
#if 0
    fprintf(tblFp, "#include \"sr_conf.h\"\n");
    fprintf(tblFp, "#include \"sr_snmp.h\"\n");
    fprintf(tblFp, "#include \"comunity.h\"\n");
    fprintf(tblFp, "#include \"v2clssc.h\"\n");
    fprintf(tblFp, "#include \"context.h\"\n");
    fprintf(tblFp, "#include \"method.h\"\n");
    fprintf(tblFp, "#include \"objectdb.h\"\n");
    /*
    fprintf(tblFp, "#include \"mibout.h\"\n\n");
    */
    fprintf(tblFp, "#include \"frPortTabledefs.h\"\n");
    fprintf(tblFp, "#include \"frPortTablesupp.h\"\n");
    fprintf(tblFp, "#include \"frPortTablepart.h\"\n");
    fprintf(tblFp, "#include \"frPortTabletype.h\"\n");
    fprintf(tblFp, "#include \"cli_objs.h\"\n\n");

    fprintf(tblFp, "extern ObjectInfo OidList[];\n\n");

#endif
    while (str && str < end)
    {
        str = cliArgGet(str);
        cliType = cliKeyword(str);
        switch (cliType)
	{
	case CMD_TYPE:
            if (numOfCfgCmd)
	    {
                fprintf(stdout, "DUPLICATE CMD definition!!\n");
                exit(-1);
	    }
            str = cliArgGet(str);
            if (*str == '{')
	    {
                str = cmdFieldGet(str, cnfCmdName, &cnfCmdAccessLvl, 
                      &cnfCmdLogFlag, &cnfCmdExcState, cnfCmdHelpMsg);
    
                if (*str != '}')
		{
                    fprintf(stdout, "Missing CMD %s terminator!!\n", cnfCmdName);
                    exit(-1);
		}
                numOfCfgCmd++;
	    }
            else
	    {
                fprintf(stdout, "No CMD Argument!!\n");
                exit(-1);
	    }
            break;
	case DSP_TYPE:
            str = parseDsp(str, &mibDspCmd[numOfDspCmd++]);
            break;
	case OPT_TYPE:
            str = parseOpt(str, &mibEntRecd[numOfMibEntry++]);
            break;
	case KEY_TYPE:
            if (scalarType)
	    {
                fprintf(stdout, "Mib table %s not found!!\n", mibTableName);
                exit(-1);
	    }
            str = parseOpt(str, &mibIndexRecd[numOfIndexEntry++]);
            break;
	case API_TYPE:
            str = parseApi(str, &mibApiCmd[numOfApiCmd++]);
            break;

	case FLTDSP_TYPE:
            str = parseFltDsp(str, &mibFltCmd[numOfFltCmd++], &mibFltDspCmd[numOfFltDspCmd++]);
            if (numOfFltCmd != numOfFltDspCmd)
            {
                fprintf(stdout, "parseFltDsp() error\n");
                exit(-1);
            }
            break;

	}
    }

    /* make a new structure */

    makeMibTable();
    /* normal DISPLAY */
    for (i = 0; i < numOfDspCmd; i++)
    {
        makeDspTable(&mibDspCmd[i], 0);
    }

    /* flt DISPALY */

    for (i = 0; i < numOfFltDspCmd; i++)
    {
        makeDspTable(&mibFltDspCmd[i], 1);
    }

    fprintf(tblFp, "#endif\n");
    fclose(tblFp);

    makeSnmpCli(cnfCmdName, mibTableName);

    makeCliCmd();

    makeShellCmdTbl();

    if (numOfApiCmd)
        makeSnmpApi();

    if (numOfFltCmd)
        makeSnmpFlt(); /* new for flt yshen */
    return(end);
}

parseMibTrap(U8_t *str)
{
    U8_t  tmpBuf[80];
    U8_t  tmpBuf1[80];
    FILE  *fp, *trapfp, *trapdefFp;
    U32_t len;
    U32_t ttlItem, ttlTbl, genItem, genTrapItem;
    U8_t  *s1, *s2, *s3, *s4, *s5, *s6;
    U8_t  trapTblName[80], trapName[80], trapNum[80];
    U8_t  *genBuf = NULL;
    U8_t  trapSevrity[80], trapCatg[80];
    U32_t val;
    U32_t webType;

    
    ttlTbl = 0;
    while (1)
    {
        str = strstr(str, "TRAP-TYPE");
        if (!str)
            break;

        s1 = strbchr(str, '\n');
        s1 = strpskip(s1, "\n\t ");
        s2 = strpbrk(s1, "\n\t{ ");

        if (s1 == str)
	{
            str = s2;
            continue;
	}

        strncpy(trapTblName, s1, s2-s1);
        trapTblName[s2-s1] = '\0';
if (debug) {
printf("trapTblName = %s\n", trapTblName);
}
        str = strstr(str, "VARIABLES");
        if (!str)
	{
            break;
	}

        s1 = strchr(str, '{');
        s3 = strchr(s1, '}');

        if (!s1 || !s3)
            return;
        s1++;

        if (!ttlTbl)
	{
            U8_t *str1, *str2, *str3;
            U8_t nameBuf[80];


            genBuf = fileGet("trap.gen", &len, 0);
if (debug) {
printf("genBuf = %x\n", genBuf);
}

            trapfp = fopen("mmstrap.tbl", "w");
/*
            fp = fopen("mmstrap.h", "r");
            if (!fp)
	    */
	    {
                fp = fopen("mmstrap.c", "w");
                fprintf(fp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

                fprintf(fp, "#ifndef _MMSTRAP_H_\n");
                fprintf(fp, "#define _MMSTRAP_H_\n\n");

                fprintf(fp, "#include \"mms_types.h\"\n");
                fprintf(fp, "#include \"sr_conf.h\"\n");
                fprintf(fp, "#include \"sr_snmp.h\"\n");
                fprintf(fp, "#include \"comunity.h\"\n");
                fprintf(fp, "#include \"v2clssc.h\"\n");
                fprintf(fp, "#include \"context.h\"\n");
                fprintf(fp, "#include \"method.h\"\n");
                fprintf(fp, "#include \"objectdb.h\"\n");
			
                fprintf(fp, "#include \"snmpdefs.h\"\n");
                fprintf(fp, "#include \"snmpsupp.h\"\n");
                fprintf(fp, "#include \"snmppart.h\"\n");
                fprintf(fp, "#include \"snmptype.h\"\n");

                fprintf(fp, "#include \"trapobjs.h\"\n\n\n");

                fprintf(fp, "#define NULL_TRAPOID   NULL\n");
                fprintf(fp, "#define OIDvalue(name, value) { name , (SR_UINT32 *) value }\n\n\n");
                genTrapItem = 0;

                trapdefFp = fopen("trapdef.h", "w");
                fprintf(trapdefFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

                fprintf(trapdefFp, "#ifndef _TRAPDEF_H_\n");
                fprintf(trapdefFp, "#define _TRAPDEF_H_\n\n");

                str1 = fileGet("trapcatg", &len, 0);
                fprintf(trapdefFp, "%s\n\n", str1);
                free(str1);



              if (genBuf)
	      {
                str1 = genBuf;
                while (str1)
		{   
                    str1 = strpskip(str1, "\n\t, ");
                    if (!str1)
                        break;
                    str2 = strpbrk(str1, "\n\t, ");
                    if (str2)
		    {
                        strncpy(nameBuf, str1, str2 - str1);
                        nameBuf[str2 - str1] = '\0';
		    }
                    else
                        strcpy(nameBuf, str1);
                    if (!genTrapItem)
                        fprintf(fp, "static TRAP_OBJ_t generalTrapOid[] = {\n");

                    fprintf(fp, "  {OIDvalue(LN%s, ID%s), NULL,   0, 0, 0,      0 },\n", nameBuf, nameBuf);
if (debug) {
printf("nameBuf = %s %x %u\n", nameBuf, str1, *str2);
}
                    genTrapItem++;
                    str1 = str2;
		}
                if (genTrapItem)
		{
                    fprintf(fp, "};\n\nTRAP_OIDTBL_t generalTrapTbl = {\n");
                    fprintf(fp, "  %8s, %40s,   %u, 0, 0, NULL, 0};\n\n", "0", "generalTrapOid", genTrapItem);
		}
                else
		{
                    fprintf(fp, "TRAP_OIDTBL_t generalTrapTbl {\n");
                    fprintf(fp, "  %8s, %40s,   %u};\n\n", "0", "NULL", genTrapItem);
		}
	      }
	    }
	    /*
            else
	    {
                fclose(fp);
                fp = fopen("mmstrap.h", "a+");
	    }
	    */
	}


        ttlItem = 0;
        genItem = 0;

        s4 = strstr(s3, "::=");
        s5 = strstr(s3, "DESCRIPTION");
        while (1)
	{

            s1 = strpskip(s1, "\n\t ,");
            if (!s1 || *s1 == '}')
                break;
            
            s2 = strpbrk(s1, "\n\t,} ");
        
            strncpy(trapName, s1, s2-s1);
            trapName[s2-s1] = '\0';
if (debug) 
{
    printf("trapName = %s\n", trapName);
}
            genItem++;
            if (genBuf && strstr(genBuf, trapName) && 
                genItem <= genTrapItem)
	    {
                s1 = s2;
                continue;
	    }

            if (!ttlItem)
                fprintf(fp, "static TRAP_OBJ_t %s_TRAPOID[] = {\n", trapTblName);
            s6 = strstr(s5, trapName);
            if (!s6  || s6 > s4)
                fprintf(fp, "  {OIDvalue(LN%s, ID%s), NULL,   0, 0, 0,     0},\n", trapName, trapName);
            else
	    {
                s6 = strpbrk(s6, " (");
                s6 = strcskip(s6, ' ');
                if (*s6 == '(')
		{
                    s6 = strcskip(s6+1, ' ');
                    if (*s6 == '*')
                    {
                        fprintf(fp, "  {OIDvalue(LN%s, ID%s), NULL,   0, 0, 1,      0},\n", trapName, trapName);
if (debug)
{
    printf(" get * \n");
}
                    } /* end '*' */
                    else if (*s6 == '#')
                    {
                        if (sscanf(s6+1, "%d", &val) != 1)
		        {
                            fprintf(fp, "  {OIDvalue(LN%s, ID%s), NULL,   0, 0, 0,     0},\n", trapName, trapName);
if (debug)
{
    printf(" get # only \n");
}
		        }
                        else
                        {
                            fprintf(fp, "  {OIDvalue(LN%s, ID%s), NULL,   0, 0, 3, %5d},\n", trapName, trapName, val);
if (debug)
{
    printf( " get #num \n");
}                       
                        }
                    } /* end '#' */
                    else 
		    {
                        if (sscanf(s6, "%d", &val) != 1)
		        {
                            fprintf(fp, "  {OIDvalue(LN%s, ID%s), NULL,   0, 0, 0,     0},\n", trapName, trapName);
if (debug)
{
     printf ( " get none \n");
}
		        }
                        else
                        {
                            fprintf(fp, "  {OIDvalue(LN%s, ID%s), NULL,   0, 0, 2, %5d},\n", trapName, trapName, val);
if (debug)
{
    printf( " get num %d \n", val);
}                       }
		    }
		}
                else
                {
                    fprintf(fp, "  {OIDvalue(LN%s, ID%s), NULL,   0, 0, 0,     0},\n", trapName, trapName);
if (debug)
{
    printf( "not web ? \n");
}
                }
	    }

            ttlItem++;
            s1 = s2;
	} /* end  while 1 */


        s1 = strstr(s3, "::=");
        s2 = strstr(s3, "DESCRIPTION");
        s6 = strstr(s2, "shelfIntegratedAlarm");
        if (!s2 || s2 > s1)
	{
            strcpy(trapSevrity, "INFO");
            strcpy(trapCatg, "CARD");
	}
        else
	{
            s4 = strstr(s2, "SEVERITY");
            s5 = strstr(s2, "CATEGORY");
            if (!s4 || s4 > s1)
	    {
                strcpy(trapSevrity, "INFO");
	    }
            else
	    {
                s4 = strpskip(s4+8, "\n\t= ");
                s2 = strpbrk(s4, "\n\t\" ");
                strncpy(trapSevrity, s4, s2-s4);
                trapSevrity[s2-s4] = '\0';

	    }
            if (!s5 || s5 > s1)
	    {
                strcpy(trapCatg, "CARD");
	    }
            else
	    {
                s5 = strpskip(s5+8, "\n\t= ");
                s2 = strpbrk(s5, "\n\t\" ");
                strncpy(trapCatg, s5, s2-s5);
                trapCatg[s2-s5] = '\0';
	    }
      
            if (!(!s6 || s6 > s1))
	    {
                s6 = strpbrk(s6, " (");
                s6 = strcskip(s6, ' ');
                if (*s6 == '(')
		{
                    s6 = strcskip(s6+1, ' ');
                    if (*s6 == '*')
		    {
                        webType = 1;
if (debug)
{
    printf("* webType is %d \n", webType);
}
                        val = 0;
		    }
                    else if (*s6 == '#')
                    {
                        webType = 3;
if (debug)
{
    printf("# webType is %d \n", webType);
}
                        val = 0;
                    }
                    else
		    {
                        webType = 2;
                        if (sscanf(s6, "%d", &val) != 1)
		        {
                            webType = 0;
		        }
if (debug)
{
    printf("num webType is %d \n", webType);
}
		    }
		}
                else
		{
                    webType = 0;
if (debug)
{
    printf("else: webType is %d \n", webType);
}
                    val = 0;
		}
	    }
            else
	    {
                    webType = 0;
                    val = 0;
	    }
	}
        
        s1 += 3;
        s1 = strpskip(s1, "\n\t ");
        s2 = strpbrk(s1, "\n\t ");
        strncpy(trapNum, s1, s2-s1);
        trapNum[s2-s1] = '\0';
if (debug) {
printf("trapNum = %s\n", trapNum);
}
        if (ttlItem)
	{
            fprintf(fp, "};\n\n");
            sprintf(tmpBuf, "%s_TRAPOID", trapTblName);
            sprintf(tmpBuf1, "\"%s\"", trapTblName);

            fprintf(trapfp, "  {%8s, %40s, %2u, %10s, %15s, %u, %5d, %30s, 0},\n", trapNum, tmpBuf, ttlItem, trapSevrity, trapCatg, webType, val, tmpBuf1);
	}
        else
	{
            sprintf(tmpBuf1, "\"%s\"", trapTblName);
            fprintf(trapfp, "  {%8s, %40s, %2u, %10s, %15s, %u, %5d, %30s, 0},\n", trapNum, "NULL_TRAPOID", 0, trapSevrity, trapCatg, 0, 0, tmpBuf1);
	}

        fprintf(trapdefFp, "#define %s_TRAP    %s\n", trapTblName, trapNum);

        ttlTbl++;
        str = s3;
    }
    if (ttlTbl)
    {
        fprintf(trapfp, "  {%8s, %40s, %2u, %10u, %15u, %u, %5d, %30s, 0}\n", "0", "NULL_TRAPOID", 0, 0, 0, 0, 0, "NULL");
        fclose(trapfp);
        if (genBuf)
            free(genBuf);
        genBuf = fileGet("mmstrap.tbl", &len, 0);
        remove("mmstrap.tbl");
        fprintf(fp, "TRAP_OIDTBL_t trapOidTbl[] = {\n%s};\n\n", genBuf);
        fprintf(fp, "#endif\n");
        fclose(fp);
        fprintf(trapdefFp, "#endif\n");
        fclose(trapdefFp);
    }

}

U8_t *
clistrcpy(U8_t *s1,
          U8_t *s2)
{
    U8_t ch;

    while (1)
    {
        ch = *s1 = *s2;
        if (ch == '<' || ch == '>' ||
            ch == '#' || ch == ' ' ||
            ch == '{' || ch == '}' ||
            ch == '\n' || ch == '\0')
	{
            *s1 = '\0';
            return(s2);
	}
        s1++;
        s2++;
    }
}

U8_t *
clistrpcpy(U8_t *s1,
           U8_t *s2,
           U8_t tCh,
           U8_t *stopStr)
{
    U8_t  ch;
    U32_t i;
    U32_t len = strlen(stopStr);

    while (1)
    {
        ch = *s1 = *s2;
        if (!ch)
            return(NULL);

        if (ch == tCh)
	{
            *s1 = '\0';
            return(s2);
	}
        for (i = 0; i < len; i++)
	{
            if (ch == stopStr[i])
	    {
                return(NULL);
	    }
	}
        s1++;
        s2++;
    }
}

U8_t *
strbchr(U8_t *str, 
        U8_t ch)
{

    str--;
    while (*str && *str != ch)
        str--;
    return(str);
}


U8_t *
strcskip(U8_t *s1,
         U8_t ch)
{
    while (*s1 == ch)
        s1++;
    if (*s1)
        return(s1);
    return(NULL);
}

U8_t *
strpskip(U8_t *s1,
         U8_t *s2)
{
    U32_t len = strlen(s2);
    U32_t i;

    while (1)
    {
        for (i = 0; i < len; i++)
	{
            if (*s1 == s2[i])
	    {
                s1++;
                break;
	    }
	}
        if (i == len)
	{
            if (*s1)
                return(s1);
            return(NULL);
	}
    }
}

U8_t *                    
clistrkey(U8_t *buf, 
          U8_t *keyword)
{
    U8_t *str;

    while (1)
    {
        if ((buf = strstr(buf, keyword)) == NULL)
        {
            return(NULL);
        }
        str = strbchr(buf, '\n');

        if ((str = strpskip(str+1, " \t")) == NULL)
            return(NULL);

        if (str[0] != '-' || str[1] != '-')
	{
            buf += strlen(keyword);
            continue;
	}

        	
        while ((str = strpbrk(str, "#M")))
	{
            if (*str == '#')
	    {
                if ((buf = strchr(buf, '\n')) == NULL)
                    return(NULL);
                break;
	    }
            if (str == buf)
                return(buf);
            if ((str = strpbrk(str, "# ")) == NULL)
                return(NULL);
            if (str == buf)
                return(buf);
            if (str > buf)
	    {
                buf += strlen(keyword);
                break;
	    }
	}
        if (str == NULL)
            return(NULL);
    }
}

U8_t *
cliArgGet(U8_t *buf)
{
    U8_t ch = *buf;

    buf++;
    if (!strchr("<>{}[];", ch))
    /* skip the current arg */
    {
        buf = strpbrk(buf, " \n<>{}#[];");
        if (buf == NULL)
            return(NULL);
    }

    while (1)
    {
        if ((buf = strpskip(buf, " \t")) == NULL)
            return(NULL);
        if (*buf == '\n')
	{
	    do {
	    /* skip all empty line */
                buf++;
                buf = strpskip(buf, " \t");
	    }
            while (*buf == '\n');

            if (buf[0] != '-' || buf[1] != '-')
                return(NULL);

            if ((buf = strcskip(buf, '-')) == NULL)
                return(NULL);
	}
        else if (*buf == '#')
	{
            if ((buf = strchr(buf, '\n')) == NULL)
                return(NULL);
            buf++;
	}
        else
            return(buf);
    }
}


U8_t *
clifieldGet(U8_t *buf, 
            U8_t *fieldBuf,
            U8_t *name,
            U8_t *errMsg1,
            U8_t *errMsg2)
{
    U8_t  *str;

    str = cliArgGet(buf);

    if (*str != '<')
    {
        fprintf(stdout, "missing opening < for %s %s option!!\n", errMsg1, name);
        exit(-1);
    }

    str = cliArgGet(str);

    str = clistrcpy(fieldBuf, str);
if (debug) {
printf("clifieldGet %x %c fieldBuf = %s\n", str, *str, fieldBuf);
}
    str--;

    if ((str = cliArgGet(str)) == NULL)
    {
        fprintf(stdout, "Missing closing > for %s %s!!\n",  errMsg2, name);
        exit(-1);
    }
    if (*str != '>')
    {
        fprintf(stdout, "Missing closing > for %s %s field %s!!\n", errMsg2, name, fieldBuf);
        exit(-1);
    }
    return(str);
}

U8_t *
clifieldStrGet(U8_t *buf, 
            U8_t *fieldBuf,
            U8_t *name,
            U8_t *errMsg1,
            U8_t *errMsg2)
{
    U8_t  *str;

if (debug) {
printf("clifieldGet 0 %x\n", buf);
}
    str = cliArgGet(buf);

if (debug) {
printf("clifieldGet 1 %x %c\n", str, *str);
}

    if (*str != '<')
    {
        fprintf(stdout, "missing opening < for %s %s option!!\n", errMsg1, name);
        exit(-1);
    }

    str = cliArgGet(str);
if (debug) {
printf("clifieldGet 2 %x %c\n", str, *str);
}

    if (*str == '"')
    {
        U8_t ch;

        str++;
        while (1)
	{
            ch = *fieldBuf = *str++;
            if (ch == '"')
	    {
                *fieldBuf = '\0';
                break;
	    }
            if (ch == '\n' || ch == '\0')
	    {
                fprintf(stdout, "missing string terminator %s!!\n", name);
                exit(-1);
	    }
            fieldBuf++;
	}
    }
if (debug) {
printf("clifieldGet 3 %x %c fieldBuf = %s\n", str, *str, fieldBuf);
}
    str--;

    if ((str = cliArgGet(str)) == NULL)
    {
        fprintf(stdout, "Missing closing > for %s %s!!\n",  errMsg2, name);
        exit(-1);
    }
if (debug) {
printf("clifieldGet 4 %x %c\n", str, *str);
}
    if (*str != '>')
    {
        fprintf(stdout, "Missing closing > for %s %s field %s!!\n", errMsg2, name, fieldBuf);
        exit(-1);
    }
    return(str);
}

U8_t *
cliStrGet(U8_t *buf, 
            U8_t *fieldBuf,
            U8_t *name,
            U8_t *errMsg1,
            U8_t *errMsg2)
{
    U8_t  *str;

    str = buf;

if (debug) {
printf("clifieldGet 1 %x %c\n", str, *str);
}


    if (*str == '"')
    {
        U8_t ch;

        str++;
        while (1)
	{
            ch = *fieldBuf = *str++;
            if (ch == '"')
	    {
                *fieldBuf = '\0';
                break;
	    }
            if (ch == '\n' || ch == '\0')
	    {
                fprintf(stdout, "missing string terminator %s!!\n", name);
                exit(-1);
	    }
            fieldBuf++;
	}
    }

if (debug) {
printf("clifieldGet 4 %x %c\n", str, *str);
}
    return(str);
}

cliKeyword(U8_t *arg)
{
    if (!strncmp(arg, "CMD", 3))
        return(CMD_TYPE);
    else if (!strncmp(arg, "DSP", 3))
        return(DSP_TYPE);
    else if (!strncmp(arg, "OPT", 3))
        return(OPT_TYPE);
    else if (!strncmp(arg, "KEY", 3))
        return(KEY_TYPE);
    else if (!strncmp(arg, "API", 3))
        return(API_TYPE);
    else if (!strncmp(arg, "FLTDSP", 6)) /* yshen */
        return(FLTDSP_TYPE);
    else
        return(UNKNOWN_TYPE);

}

U8_t *
dspPosGet(U8_t *str, 
          U32_t *pos)
{
    if (sscanf(str, "@%u:", pos) != 1)
    {
        fprintf(stdout, "Illegal @ format!!\n");
        exit(-1);
    }
    str = strchr(str, ':');
    if (str != NULL)
        str++;
    return(str);

}

U8_t *
findObjEnt(U8_t *buf, 
           U8_t *mibName)
{
    U8_t *str;

    str = buf;
    while (1)
    {

        buf = strstr(buf, mibName);
        if (!buf)
        {
            return(NULL);
        }

        str = strbchr(buf, '\n');

        if ((str = strpskip(str+1, " \t")) == NULL)
	{
            return(NULL);
	}

        if (str != buf)
	{
            buf++;
            continue;
	}

        str = strpbrk(buf, " \t");
        if (!str)
            return(NULL);
 
        if ((str = strpskip(str, " \t")) == NULL)
	{
            return(NULL);
	}
        if (!strncmp(str, "OBJECT-TYPE", 11))
            return(buf);
        buf = str;
    }
    return(NULL);
}

mibObjSyntax(U8_t *objTbl)
{
    U8_t *str;

    str = strstr(objTbl, "SYNTAX");
    if (str == NULL)
        return(NULL);
    str = strpbrk(str, " \t");
    if ((str = strpskip(str, " \t")) == NULL)
    {
        return(NULL);
    }    

    if (!strncmp(str, "SEQUENCE", 8))
    {
        return(SYNTAX_SEQUENCE);

    }
    else if (!strncmp(str, "INTEGER", 7))
    {
        str += 7;
        str = strpskip(str, " \t\n");
        if (*str == '{')
            return(SYNTAX_INTEGER_MAP);
        else if (*str == '(')
            return(SYNTAX_INTEGER_RANGE);
        else
            return(SYNTAX_INTEGER);
    }
    else if (!strncmp(str, "DisplayString", 13) ||
           !strncmp(str, "IpAddress", 9))
    {
        return(SYNTAX_STRING);
    }
    else if (!strncmp(str, "Gauge", 5))
    {
        return(SYNTAX_INTEGER);

    }
    else if (!strncmp(str, "Counter", 7))
    {
        return(SYNTAX_INTEGER);

    }
    else if (!strncmp(str, "OBJECT IDENTIFIER", 17))
    {
        return(SYNTAX_STRING);
    }
    else
    {
        return(SYNTAX_UNKNOWN);
    }
}

U8_t *
mibObjMap(U8_t *objTbl,
          U32_t *mapCnt)
{
    U8_t *str;
    U8_t argBuf[80];
    U32_t val;
    U32_t cnt = 0;
    U8_t *s1, ch;

    str = strstr(objTbl, "SYNTAX");
    if (str == NULL)
        return(NULL);
    str = strpbrk(str, " \t");
    if ((str = strpskip(str, " \t")) == NULL)
    {
        return(NULL);
    }
    if (!strncmp(str, "INTEGER", 7))
    {
        str += 7;
        str = strpskip(str, " \t\n");
        if (*str == '{')
	{
            str++;
            while (1)
	    {
                str = strpskip(str, " \t\n");
                if (*str == '}')
                    break;

                s1 = argBuf;
                while (1)
		{
                    ch = *s1 = *str;
                    if (!ch || ch == ' ' || ch == '\t' || 
                        ch == '\n' || ch == '(')
		    {
                        *s1 = '\0';
                        break;
		    }
                    s1++;
                    str++;
		}
                if (ch != '(')
                     str = strpskip(str, " \t\n");
                if (*str == '(')

                if (sscanf(str, "(%u)", &val) != 1)
		{
                    return(NULL);
		}

                if (cnt++)
                    fprintf(tblFp, ", {\"%s\", %u}", argBuf, val);
                else
                    fprintf(tblFp, "{\"%s\", %u}", argBuf, val);
                str = strchr(str, ')');
                str = strpbrk(str, " \t\n");
                str++;
	    }
	}
        fprintf(tblFp, "};\n\n");
        *mapCnt = cnt;
        return(str);
    }
    return(NULL);
}

U8_t *
mibObjDesc(U8_t *objTbl,
           U8_t *descBuf)
{
    U8_t *str, *s1, *s2, *s3;
    U8_t ch;

    str = strstr(objTbl, "DESCRIPTION");
    if (str == NULL)
        return(NULL);
    str = strpbrk(str, " \t\n");
    if ((str = strpskip(str, " \t\n")) == NULL)
    {
        return(NULL);
    }
    s1 = descBuf;
    if (*str == '"')
    {
        str++;
        while (1)
	{
            ch = *s1 = *str;
            if (ch == '\n')
	    {
                *s1 = '\n';
            }
            else if (ch == '\t')
	    {
                *s1++ = ' ';
                *s1++ = ' ';
                *s1++ = ' ';
                *s1 = ' ';
	    }
            else if (ch == '"')
	    {
                while (*(s1 - 1) == ' ' || *(s1 - 1) == '\n')
                    s1--;

                s2 = strstr(str, "DEFVAL");
                s3 = strstr(str, "::=");
                if (s2 && (U32_t)s2 < (U32_t)s3)
		{
                    *s1++ = ' ';
                    *s1++ = ' ';
                    *s1++ = ' ';
                    *s1++ = ' ';
                    while ((ch = *s2) && ch != '\n')
		    {
                        if (ch == '"')
                            *s1++ = '\\';
                        *s1++ = *s2++;
		    }
                }
                *s1 = '\0';


                return(str);
	    }
            s1++;
            str++;
        }
    }
}  


U8_t *
mibObjRange(U8_t *objTbl,
            U8_t *argBuf)
{
    U8_t *str;
    U32_t cnt = 0;
    U8_t *s1, ch;

    str = strstr(objTbl, "SYNTAX");
    if (str == NULL)
        return(NULL);
    str = strpbrk(str, " \t");
    if ((str = strpskip(str, " \t")) == NULL)
    {
        return(NULL);
    }
    if (!strncmp(str, "INTEGER", 7))
    {
        str += 7;
        str = strpskip(str, " \t\n");
if (debug) {
printf("mibObjRange 0\n");
}
        if (*str == '(')
	{
if (debug) {
printf("mibObjRange 1\n");
}
            str++;
            str = strpskip(str, " \t\n");
            s1 = argBuf;
            while (1)
	    {
                ch = *s1 = *str;
                if (!ch || ch == ' ' || ch == '\t' || 
                    ch == '\n' || ch == ')')
	        {
                    *s1 = '\0';
                    return(str);
	        }
                s1++;
                str++;
	    }

	}
    }
    return(NULL);
}

mibObjAccess(U8_t *objTbl)
{
    U8_t *str;

    str = strstr(objTbl, "ACCESS");
    if (str == NULL)
        return(NULL);
    str = strpbrk(str, " \t");
    if ((str = strpskip(str, " \t")) == NULL)
    {
        return(NULL);
    }
    if (!strncmp(str, "not-accessible", 8))
        return(ACCESS_NOT_ACCESSIBLE);
    else if (!strncmp(str, "read-write", 10))
        return(ACCESS_READ_WRITE);
    else if (!strncmp(str, "read-only", 9))
        return(ACCESS_READ_ONLY);
    else
        return(ACCESS_UNKNOWN);
}

U8_t *
parseOpt(U8_t *buf, 
         pMIBENTRYINF_t mibInfPtr)
{
    U8_t  *str;
    U8_t  *objTbl;
    U8_t  tmpBuf[50];
    U32_t cnt = 0;
    U32_t syntaxType;
    U8_t  descBuf[5000];

    str = cliArgGet(buf);
    if (*str != '{')
    {
        fprintf(stdout, "missing opening { for OPT!!\n");
        exit(-1);
    }
    str = clifieldGet(str,  mibInfPtr->name,  mibInfPtr->name, "option str", 
        "option str");

    if ((objTbl = findObjEnt(&fileBuf[1], mibInfPtr->name)) == NULL)
    {
        fprintf(stdout, "Mib entry %s not found!!\n",  mibInfPtr->name);
        exit(-1);
    }


    mibInfPtr->accessTp = mibObjAccess(objTbl);

    mibInfPtr->objMibTbl = objTbl;

    str = clifieldGet(str,  mibInfPtr->optStr,  mibInfPtr->name, "option str", 
        "option str");

    str = cliArgGet(str);

    strcpy(tmpBuf, "NULL");
    mibInfPtr->inType = ARG_DEFAULT;
    if (*str == '<')
    {
        str--;
        str = clifieldGet(str, tmpBuf,  mibInfPtr->name, 
            "input type", "input type");

        if (!strncmp(tmpBuf, "ARG_DEFAULT", 10))
	{
            mibInfPtr->inType = ARG_DEFAULT;
	}
        else if (!strncmp(tmpBuf, "ARG_MAP", 7))
	{
            mibInfPtr->inType = ARG_MAP;
	}
        else if (!strncmp(tmpBuf, "ARG_UMAP", 8))
	{
            mibInfPtr->inType = ARG_UMAP;
	}
        else if (!strncmp(tmpBuf, "ARG_FUNC", 8))
	{
            mibInfPtr->inType = ARG_FUNC;
	}
        else
	{
            fprintf(stdout, "Illegal argument type %s\n", tmpBuf);
            exit(-1);
	}

        if (mibInfPtr->inType == ARG_FUNC)
	{
            str = clifieldGet(str,  tmpBuf,  mibInfPtr->name, "arg func", 
                "arg func");
            fprintf(tblFp, "extern MIBARGFUNC_t %s(pCLIMIBENTITY_t, U8_t *, U32_t *);\n\n", tmpBuf);
	}
        else if (mibInfPtr->inType == ARG_UMAP)
	{
            fprintf(tblFp, "static CLIUMAPSTRUC_t %sUMAP[] = {\n", mibInfPtr->name);
            str = makeArgStruct(str, mibInfPtr, &cnt);
            sprintf(tmpBuf, "%sUMAP", mibInfPtr->name);
	}
        str = cliArgGet(str);
    }
    if (*str == '<')
    /* help string */
    {
        str--;
        mibInfPtr->helpMsg[0] = '"';
        str = clifieldStrGet(str, &mibInfPtr->helpMsg[1], mibInfPtr->name, 
        "OPT argument help message", "OPT argument help message");
        mibInfPtr->helpMsg[strlen(mibInfPtr->helpMsg)] = '"';
        mibInfPtr->helpMsg[strlen(mibInfPtr->helpMsg)+1] = '\0';
        str = cliArgGet(str);

    }
    else
        strcpy(mibInfPtr->helpMsg, "NULL");

    if (*str != '}')
    {
        fprintf(stdout, "Missing closing } for %s  mib %s!!\n", 
            "OPT", mibInfPtr->name);
        exit(-1);
    
    }

    mibObjDesc(objTbl, descBuf);    
    syntaxType = mibObjSyntax(objTbl);

    if (mibInfPtr->inType != ARG_FUNC &&
        mibInfPtr->inType != ARG_UMAP)
    {
        if (mibInfPtr->inType == ARG_MAP && 
            syntaxType != SYNTAX_INTEGER_MAP)
        {
            fprintf(stdout, "Illegal argument type %s\n", tmpBuf);
            exit(-1);
        }
    
if (debug) {
printf("syntaxType = %u\n", syntaxType);
}
    
        if (mibInfPtr->inType == ARG_MAP)
            mibInfPtr->mibType = ARG_CONV;
        else if (syntaxType == SYNTAX_STRING)
            mibInfPtr->mibType = ARG_STR;
        else
            mibInfPtr->mibType = ARG_ENUM;


        if (syntaxType == SYNTAX_INTEGER_MAP)
        {
            fprintf(tblFp, "static CLIUMAPSTRUC_t %sMAP[] = {\n", 
                mibInfPtr->name);
            if ((mibObjMap(objTbl, &cnt)) == NULL)
            {
                fprintf(stdout, "Illegal argument type %s\n", tmpBuf);
                exit(-1);
            }
            sprintf(tmpBuf, "%sMAP", mibInfPtr->name);        
        }
        else if (syntaxType == SYNTAX_INTEGER_RANGE)
        {
      
            if (mibObjRange(objTbl, tmpBuf))
    	    {
                fprintf(tblFp, "static U8_t %sRANGE[] = \"%s\";\n\n", 
                    mibInfPtr->name, tmpBuf);
                sprintf(tmpBuf, "%sRANGE", mibInfPtr->name);
    	    }        
        }
    }
    else if (mibInfPtr->inType == ARG_FUNC)
        mibInfPtr->mibType = ARG_FUNC;
    else
        mibInfPtr->mibType = ARG_CONV;

    /* make a new structure */
    makeMibEntity(mibInfPtr, tmpBuf, cnt, descBuf);
}

U8_t *
parseDsp(U8_t *buf,
         pMIBDSPCMD_t dspCmdPtr)
{
    U8_t  *str;
    pMIBENTDSP_t dspEntPtr;
    U8_t  dspType[40];
    U8_t  strBuf[400];
    U32_t curPos, pos, fmtInx;
    U32_t len, j;
    U8_t  *fmtBuf;

    str = cliArgGet(buf);
    if (*str != '{')
    {
        fprintf(stdout, "missing opening { for OPT!!\n");
        exit(-1);
    }


    dspEntPtr = dspCmdPtr->dspEnt;
    dspCmdPtr->count = 0;

    str = cmdFieldGet(str, dspCmdPtr->name, &dspCmdPtr->accessLvl, 
                      &dspCmdPtr->logFlag, &dspCmdPtr->excState, dspCmdPtr->helpMsg);

    str--;

    str = clifieldGet(str, dspType, dspCmdPtr->name, "DSP CMD TYPE", 
        "DSP CMD TYPE");

    if (dspType[0] == '\0')
        dspCmdPtr->cmdType = DSP_ROW;
    else if (!strncmp(dspType, "column", 6))
        dspCmdPtr->cmdType = DSP_COLUMN;
    else if (!strncmp(dspType, "row", 3))
        dspCmdPtr->cmdType = DSP_ROW;
    else
    {
        fprintf(stdout, "DSP CMD Type must be row or column %s!!\n", 
         dspCmdPtr->name);
        exit(-1);

    }


    str = cliArgGet(str);

    dspCmdPtr->banner[0] = '\0';
    curPos = 0;
    fmtInx = 0;
    fmtBuf = dspCmdPtr->banner;

    if (*str == '<')
    {
        str++;
        str = strpskip(str, " \t");
        while (1)
	{
            if (*str == '>')
                break;
            if (*str == '@')
                str = dspPosGet(str, &pos);
            else
                pos = -1;
	
            if (*str == '"')
	    {
                str++;
                str = clistrpcpy(strBuf, str, '"', "\n");
                if (!str)
	        {
                    fprintf(stdout, "DSP banner string need be in the same line %s!!\n",
                    dspCmdPtr->name);
                    exit(-1);
	        }
            }
            else
    	    {
                 fprintf(stdout, "Missing field seperator for %s banner string!!\n", 
                 dspCmdPtr->name);
                 exit(-1);
	    }
            str++;
            if ((str = strpskip(str, " \t")) == NULL)
	    {
                 fprintf(stdout, "Illegal %s banner format\n", 
                 dspCmdPtr->name);
                 exit(-1);
	    }
            if (*str != ';' && *str != '>')
	    {
                 fprintf(stdout, "Illegal %s banner field seperator  at %s\n", 
                 dspCmdPtr->name, strBuf);
                 exit(-1);
	    }
            if (*str == ';')
                str = cliArgGet(str);

            if (pos != -1)
	    {
                if (pos < curPos)
		{
                    fmtBuf[fmtInx++] = '\n';
                    curPos = 0;
		}
                if (pos > curPos)
	        {
                    for (j = pos - curPos; j; j--)
                        fmtBuf[fmtInx++] = ' ';
                    curPos = pos;
	        }
            }
            strcpy(&fmtBuf[fmtInx], strBuf);
            len = strlen(strBuf);
            curPos += len;
            fmtInx += len;
        }
    }

    
    /*    
    str = clifieldStrGet(str, dspCmdPtr->banner, dspCmdPtr->name, 
        "DSP CMD Banner", "DSP CMD Banner");
	*/

if (debug) {
printf("parseDsp banner = %s\n", dspCmdPtr->banner);
}

    str = cliArgGet(str);
    if (*str == '<')
    {
        str++;
        while (1)
	{
            if (*str == '>')
                break;
            if (*str == '}' || *str == '{')
	    {
                fprintf(stdout, "Illegal termination for DSP %s!!\n", 
                 dspCmdPtr->name);
                exit(-1);
	    }
            str = strpskip(str, " \t");
            if (*str != '[')
	    {
                fprintf(stdout, "Illegal field Brace for DSP %s!!\n",
                     dspCmdPtr->name);
                exit(-1);

	    }
            str++;
            str = strpskip(str, " \t");

            if (*str != ';')
	    {
                if (*str == '@')
                    str = dspPosGet(str, &(dspEntPtr->strPos));
                else
                    dspEntPtr->strPos = -1;

                if (*str == '"')
		{
                    str++;
                    str = clistrpcpy(dspEntPtr->dspStr, str, '"', "\n");
                    if (!str)
		    {
                        fprintf(stdout, "DSP string need be in the same line %s!!\n",
                        dspCmdPtr->name);
                        exit(-1);
		    }
                }
                else
		{
                     fprintf(stdout, "Missing field seperator for %s string!!\n", 
                     dspCmdPtr->name);
                     exit(-1);
		}
                str++;
                if ((str = strpskip(str, " \t")) == NULL)
		{
                     fprintf(stdout, "Missing 1 field seperator for %s string!!\n", 
                     dspCmdPtr->name);
                     exit(-1);

		}
if (debug) {
printf("parseDsp dsp = %s %x %c\n", dspEntPtr->dspStr, dspEntPtr->strPos, *str);
}

                if (*str != ';')
		{
                     fprintf(stdout, "Missing field seperator for %s string!!\n", 
                     dspCmdPtr->name);
                     exit(-1);

		}
	    }
            else
	    {
                dspEntPtr->strPos = -1;
                dspEntPtr->dspStr[0] = '\0';
	    }
            str = cliArgGet(str);
        
            if (*str == '@')
                str = dspPosGet(str, &(dspEntPtr->datPos));
            else
                dspEntPtr->datPos = -1;

            str = clistrpcpy(dspEntPtr->optStr, str, ';', "\n[]<>{}");
            if (!str)
	    {
                fprintf(stdout, "Illegal char in option field %s\n", 
                        dspCmdPtr->name);
                exit(-1);	    
	    }
if (debug) {
printf("parseDsp opt = %s %x\n", dspEntPtr->optStr, dspEntPtr->datPos);
}
            str = cliArgGet(str);
        
            if (*str != '%')
	    {
                fprintf(stdout, "Illegal format char for DSP %s\n", 
                        dspCmdPtr->name);
                exit(-1);	    

	    }

            str = clistrpcpy(dspEntPtr->fmtStr, str, ']', "\n[]<>{}");
            if (!str)
	    {
                fprintf(stdout, "Illegal format char for DSP %s\n", 
                    dspCmdPtr->name);
                exit(-1);	    
	    }
if (debug) {
printf("parseDsp fmt = %s\n", dspEntPtr->fmtStr);
}
            str = cliArgGet(str);
            dspEntPtr++;
            dspCmdPtr->count++;
	}
    }
    else
    {
        fprintf(stdout, "DSP %s missing argument list!!\n", 
         dspCmdPtr->name);
        exit(-1);
    }
    return(str);
}


U8_t *
parseApi(U8_t *buf,
         pMIBAPICMD_t apiCmdPtr)
{
    U8_t  *str;
    pMIBENTAPI_t apiEntPtr;
    U8_t  apiType[40];

    str = cliArgGet(buf);
    if (*str != '{')
    {
        fprintf(stdout, "missing opening { for OPT!!\n");
        exit(-1);
    }


    apiEntPtr = apiCmdPtr->apiEnt;
    apiCmdPtr->count = 0;

    str = cmdFieldGet(str, apiCmdPtr->name, &apiCmdPtr->accessLvl, 
                      &apiCmdPtr->logFlag, &apiCmdPtr->excState, apiCmdPtr->helpMsg);

    if (debug) {
        printf("parseApi %c\n", *str);
    }
    str--;

    str = clifieldGet(str, apiType, apiCmdPtr->name, "API CMD TYPE", 
        "API CMD TYPE");

    if (debug) {
printf("parseApi type = %s *c \n", apiType, *str);
    }

    if (apiType[0] == '\0')
        apiCmdPtr->cmdType = DSP_ROW;
    else if (!strncmp(apiType, "column", 6))
        apiCmdPtr->cmdType = DSP_COLUMN;
    else if (!strncmp(apiType, "row", 3))
        apiCmdPtr->cmdType = DSP_ROW;
    else
    {
        fprintf(stdout, "API CMD Type must be row or column %s!!\n", 
         apiCmdPtr->name);
        exit(-1);

    }

    str = cliArgGet(str);
    if (debug) {
printf("parseApi 1 %c\n", *str);
    }
    if (*str == '<')
    {
        str++;
        while (1)
	{
            if (*str == '>')
                break;
            if (*str == '}' || *str == '{')
	    {
                fprintf(stdout, "Illegal termination for API %s!!\n", 
                 apiCmdPtr->name);
                exit(-1);
	    }
            str = strpskip(str, " \t");
            if (*str != '[')
	    {
                fprintf(stdout, "Illegal field Brace for API %s!!\n",
                     apiCmdPtr->name);
                exit(-1);

	    }
            str++;
            str = strpskip(str, " \t");

            if (*str != ';')
	    {
                apiEntPtr->sepCh = *str;
                str++;
                str = strpskip(str, " \t");

if (debug) {
printf("parseApi seperator = %c\n", apiEntPtr->sepCh);
}

                if (*str != ';')
		{
                     fprintf(stdout, "Missing field seperator for %s string!!\n", 
                     apiCmdPtr->name);
                     exit(-1);

		}
	    }
            else 
                apiEntPtr->sepCh = ' ';
            str++;
        
            str = clistrpcpy(apiEntPtr->optStr, str, ';', "\n[]<>{}");
            if (!str)
	    {
                fprintf(stdout, "Illegal char in option field %s\n", 
                        apiCmdPtr->name);
                exit(-1);	    
	    }


            str = cliArgGet(str);
        
            if (*str != '%')
	    {
                U8_t *str1, ch;

                apiEntPtr->argType = CONST_OPT;
                str1 = strpbrk(str, "\n[]<>{}\t ");
                if (str1)
		{
                    ch = *str1;
                    *str1 = '\0';
		}
                strcpy(apiEntPtr->fmtStr, str);
                if (str1)
		{
                    *str1 = ch;
		}
                str = str1;

	    }
            else
	    {
                str++;
                if (isdigit(*str))
		{
                    apiEntPtr->argNum = strtol((char*)str, (char**)&str, 10);
                
                    str = strpskip(str, " \t");
                    apiEntPtr->argType = VAR_OPT;
		}
                else
                {
                    fprintf(stdout, "Illegal  format!!\n");
                    exit(-1);
                }
	    }

            if (!str)
	    {
                fprintf(stdout, "Illegal format char for API %s\n", 
                    apiCmdPtr->name);
                exit(-1);	    
	    }

if (debug) {
printf("parseApi fmt = %s\n", apiEntPtr->fmtStr);
}
            str = cliArgGet(str);
            apiEntPtr++;
            apiCmdPtr->count++;
	}
    }
    else
    {
        fprintf(stdout, "API %s missing argument list!!\n", 
         apiCmdPtr->name);
        exit(-1);
    }
}


pMIBENTRYINF_t
mibOptGetEnt(U8_t *optStr)
{
    U32_t i;

    for (i = 0; i < numOfMibEntry; i++)
    {
if (debug) {
printf("mibOptGetEnt %s %s\n", optStr, mibEntRecd[i].optStr);
}
        if (!strcmp(optStr, mibEntRecd[i].optStr))
            return(&mibEntRecd[i]);
    }
    for (i = 0; i < numOfIndexEntry; i++)
    {
if (debug) {
printf("mibOptGetEnt %s %s\n", optStr, mibIndexRecd[i].optStr);
}
        if (!strcmp(optStr, mibIndexRecd[i].optStr))
            return(&mibIndexRecd[i]);
    }
    return(NULL);

}


U8_t *
makeArgStruct(U8_t *str,
              pMIBENTRYINF_t mibInfPtr,
              U32_t *argCnt)
{
    U8_t argBuf[50];
    U32_t val;
    U32_t cnt = 0;

    str = cliArgGet(str);
    if (*str == '<')
    {
        str++;
        while (1)
	{
            if (*str == '>')
                break;
            if (*str == '}' || *str == '{')
	    {
                fprintf(stdout, "Illegal termination for OPT %s Map!!\n", 
                 mibInfPtr->name);
                exit(-1);
	    }
            str = strpskip(str, " \t");


            if (*str == '"')
	    {
                str = clistrpcpy(argBuf, str + 1, '"', "\n");
                if (!str)
	        {
                    fprintf(stdout, "Opt %s Map string need be in the same line!!\n",
                    mibInfPtr->name);
                    exit(-1);
	        }
            }
            else
	    {
                fprintf(stdout, "Need \" for OPT %s Map string!!\n", 
                    mibInfPtr->name);
                exit(-1);
	    }

if (debug) {
printf("parseArg Opt = %s %s,\n", mibInfPtr->name, argBuf);
}

            str++;
            if ((str = strpskip(str, " \t")) == NULL)
	    {
                 fprintf(stdout, "Missing field seperator for OPT %s Map!!\n", 
                 mibInfPtr->name);
                 exit(-1);
	    }
            if (*str == ':')
	    {
                str++;
                if (sscanf(str, "%u;", &val) != 1)
		{
                 fprintf(stdout, "Missing field value for OPT %s Map!!\n", 
                 mibInfPtr->name);
                 exit(-1);
                
		}
                
                str = strpbrk(str, "<>;{}");
                if (*str == ';')
                    str = cliArgGet(str);
	    }
            else
	    {
                 fprintf(stdout, "Missing field seperator for OPT %s Map!!\n", 
                 mibInfPtr->name);
                 exit(-1);
	    }
            if (cnt++)
                fprintf(tblFp, ", {\"%s\", %u}", argBuf, val);
            else
                fprintf(tblFp, "{\"%s\", %u}", argBuf, val);
	}
        fprintf(tblFp, "};\n\n");
        *argCnt = cnt;
        return(str);
    }
    else
    {
        fprintf(stdout, "Missing Map description for OPT %s Map!!\n", 
            mibInfPtr->name);
        exit(-1);
    }
}

makeMibTable()
{
    U32_t i;
    U8_t  buf[100];
    U32_t cnt = 0;

    if (numOfMibEntry)
    {
        fprintf(tblFp, "static pCLIMIBENTITY_t %sLST[] = {\n", mibTableName);
   
        for (i = 0; i < numOfMibEntry; i++)
        {
            if (mibEntRecd[i].accessTp != ACCESS_READ_ONLY)
	    {
                cnt++;
                buf[0] = '&';
                strcpy(&buf[1], mibEntRecd[i].name);
                if (i == (numOfMibEntry - 1))
                   fprintf(tblFp, "%30sENT};\n\n",  buf);
                else if (!(i & 1))	
                    fprintf(tblFp, "%30sENT, ",  buf);
                else		
                    fprintf(tblFp, "%30sENT,\n",  buf);
	    }
            else if (i == (numOfMibEntry - 1))
                fprintf(tblFp, "%33s};\n\n", "NULL");
	}
    }
    else
        return;

    if (numOfIndexEntry)
    {
        fprintf(tblFp, "static pCLIMIBENTITY_t %sINDX[] = {\n", mibTableName);
        fprintf(keyFp, "%s = {\n", mibTableName);
        for (i = 0; i < numOfIndexEntry; i++)
        {
            buf[0] = '&';
            strcpy(&buf[1], mibIndexRecd[i].name);
            if (i == (numOfIndexEntry - 1))
	    {
                fprintf(tblFp, "%30sENT};\n\n",  buf);
                fprintf(keyFp, "%30s};\n\n",  &buf[1]);
	    }
            else if (!(i & 1))	
	    {
                fprintf(tblFp, "%30sENT, ",  buf);
                fprintf(keyFp, "%30s,\n",  &buf[1]);
	    }
            else		
	    {
                fprintf(tblFp, "%30sENT,\n",  buf);
                fprintf(keyFp, "%30s,\n",  &buf[1]);
	    }
	}
        fprintf(tblFp, mibTableStr, mibTableName, mibTableName, "INDX",
                 mibTableName, numOfIndexEntry, cnt);
    }
    else
        fprintf(tblFp, mibTableStr, mibTableName, "NULL", "",
                  mibTableName, numOfIndexEntry, cnt);
}

/* make a new structure */
makeMibEntity(pMIBENTRYINF_t mibEntPtr,
              U8_t  *arg,
              U32_t cnt,
              U8_t  *descBuf)
{
    fprintf(tblFp, mibEntryStr, mibEntPtr->name, mibEntPtr->name,
        mibEntPtr->name, mibEntPtr->optStr, argTypeStr[mibEntPtr->mibType], 
        cnt, arg, mibEntPtr->helpMsg, mibEntPtr->name, descBuf);
}
         
#define MAX_UFUNC_NAME   25
#define MAX_OBJ_NAME     30     
#define MAX_FLT_DSP_ENT  10 
   
makeDspTable(pMIBDSPCMD_t mibDspTblPtr, int isFlt)
{
    U32_t curPos;
    pMIBENTDSP_t dspRecPtr;
    U32_t i, j, k;
    U8_t  fmtBuf[200];
    U32_t len;
    U32_t fmtInx;
    pMIBENTRYINF_t mibEInfoPtr;
    U32_t dspType;
    U8_t  fltUfunc[MAX_FLT_DSP_ENT][MAX_UFUNC_NAME];
    U8_t  fltObjName[MAX_FLT_DSP_ENT][MAX_OBJ_NAME];
    int   catchUfuncBitMap = 0;

    curPos = 0;
    dspRecPtr = mibDspTblPtr->dspEnt;

    for (i = 0; i < mibDspTblPtr->count; i++, dspRecPtr++)
    {
        fmtInx = 0;
        if (!strncmp(dspRecPtr->optStr, "UFUNC ", 6))
	{
            if (isFlt == 0)
            {
                fprintf(tblFp, "extern DSPFUNC_t %s(pCLIMIBDSPGROUP_t, OID *, U32_t *);\n\n", 
                     &dspRecPtr->optStr[6]);
            }
            else
            {
                if (strchr(dspRecPtr->optStr, ':') != NULL)
                {
                    /* catch UFUNC name */
                    for (j = 0; j < MAX_UFUNC_NAME; j++)
                    {
                        if (dspRecPtr->optStr[6 + j] != ':')
                            fltUfunc[i][j] = dspRecPtr->optStr[6 + j];
                        else
                            break;
                    }
                    fltUfunc[i][j] = '\0';
                    fprintf(tblFp, "extern DSPFUNC_t %s(pCLIMIBDSPGROUP_t, OID *, U32_t *);\n\n", 
                            fltUfunc[i]);   

                    /* catch Object name */
                    for (k = 0; k < MAX_OBJ_NAME; k++)
                    {
                        if (dspRecPtr->optStr[7 + j + k] != ':')
                            fltObjName[i][k] = dspRecPtr->optStr[7 + j + k];
                        else
                            break;
                    }
                    fltObjName[i][k] = '\0'; 

                    catchUfuncBitMap = (catchUfuncBitMap | (0x1 << i));
                }
                else
                {
                    fprintf(tblFp, "extern DSPFUNC_t %s(pCLIMIBDSPGROUP_t, OID *, U32_t *);\n\n", 
                         &dspRecPtr->optStr[6]);
                }
            }
	} /* UFUNC control */
    }

    dspRecPtr = mibDspTblPtr->dspEnt;
    fprintf(tblFp, "static MIBDSPENTITY_t %sDENT[] = {\n", mibDspTblPtr->name);
    for (i = 0; i < mibDspTblPtr->count; i++, dspRecPtr++)
    {
        U8_t fmt;

        fmtInx = 0;
        fmt = dspRecPtr->fmtStr[strlen(dspRecPtr->fmtStr)-1];
        if (fmt == 'u' || fmt == 'x' || fmt == 'd' || fmt == 'i' ||
            fmt == 'o' || fmt == 'X')
        {
            dspType = DSP_NUM;
        }
        else if (fmt == 's')
        {
            dspType = DSP_STRING;
        }
        else
            dspType = DSP_TABLE;

        if (!strncmp(dspRecPtr->optStr, "UFUNC ", 6))
	{
            mibEInfoPtr = NULL;
	}
        else
	{
            mibEInfoPtr = mibOptGetEnt(dspRecPtr->optStr);
            if (mibEInfoPtr == NULL)
    	    {
                fprintf(stdout, "%u DSP %s OPT %s not exist!!\n", i,
                mibDspTblPtr->name, dspRecPtr->optStr);
                exit(-1);
	    }
	}

        if (dspRecPtr->strPos != -1)
	{
            if (dspRecPtr->strPos > curPos)
	    {
                for (j = dspRecPtr->strPos - curPos; j; j--)
                    fmtBuf[fmtInx++] = ' ';
                curPos = dspRecPtr->strPos;
	    }
	    /*
            else if (dspRecPtr->strPos < curPos)
	    {
                fmtBuf[fmtInx++] = '\\';
                fmtBuf[fmtInx++] = 'n';
                fmtBuf[fmtInx] = '\0';
                curPos = 0;
	    }
	    */
        }
        strcpy(&fmtBuf[fmtInx], dspRecPtr->dspStr);
        len = strlen(dspRecPtr->dspStr);
        curPos += len;
        fmtInx += len;

        if (dspRecPtr->datPos != -1)
	{
            if (dspRecPtr->datPos > curPos)
	    {
                for (j = dspRecPtr->datPos - curPos; j; j--)
                    fmtBuf[fmtInx++] = ' ';
                curPos = dspRecPtr->datPos;
	    }
            else if (dspRecPtr->datPos < curPos)
	    {
                fmtBuf[fmtInx++] = '\\';
                fmtBuf[fmtInx++] = 'n';
                fmtBuf[fmtInx] = '\0';
                curPos = 0;
	    }
	}

         
        if (dspType != DSP_TABLE)
	{
            if (isdigit(dspRecPtr->fmtStr[1]))
                len = atol(&dspRecPtr->fmtStr[1]);
            else
                len = 5;

            strcpy(&fmtBuf[fmtInx],  dspRecPtr->fmtStr);
            curPos += len;
	}

        
        if (i < (mibDspTblPtr->count - 1) &&
            dspRecPtr[1].strPos != -1)
	{
            if (dspRecPtr[1].strPos < curPos)
	    {
                fmtInx = strlen(fmtBuf);
                fmtBuf[fmtInx++] = '\\';
                fmtBuf[fmtInx++] = 'n';
                fmtBuf[fmtInx] = '\0';
                curPos = 0;
	    }
        }
        else if (i >= (mibDspTblPtr->count - 1))
	{
          if (dspType != DSP_TABLE)
	  {
            fmtInx = strlen(fmtBuf);
            fmtBuf[fmtInx++] = '\\';
            fmtBuf[fmtInx++] = 'n';
            fmtBuf[fmtInx] = '\0';
	  }
            curPos = 0;
	}
        
        if (mibEInfoPtr)
	{
            if (i != (mibDspTblPtr->count-1))
               fprintf(tblFp, "    {&%sENT, \"%s\", NULL, %s},\n", 
               mibEInfoPtr->name, fmtBuf, dspTypeStr[dspType]);
            else
	    {
               fprintf(tblFp, "    {&%sENT, \"%s\", NULL, %s}};\n\n", 
                   mibEInfoPtr->name, fmtBuf, dspTypeStr[dspType]);
	    }
	}
        else
	{
            if (i != (mibDspTblPtr->count-1))
            {
                if ((isFlt == 1) && ((catchUfuncBitMap >> i) & 0x1))
                {

                    fprintf(tblFp, "    {&%sENT, \"%s\", %s, %s},\n", 
                       (char*)fltObjName[i], fmtBuf, (char*)fltUfunc[i], dspTypeStr[dspType]);
                }
                else
                    fprintf(tblFp, "    {NULL, \"%s\", %s, %s},\n", 
                       fmtBuf, &dspRecPtr->optStr[6], dspTypeStr[dspType]);
            }
            else
            {
                if ((isFlt == 1) && ((catchUfuncBitMap >> i) & 0x1))
                {
                    fprintf(tblFp, "    {&%sENT, \"%s\", %s, %s}};\n\n", 
                       (char*)fltObjName[i], fmtBuf, (char*)fltUfunc[i], dspTypeStr[dspType]);
                }
                else
                    fprintf(tblFp, "    {NULL, \"%s\", %s, %s}};\n\n", 
                       fmtBuf, &dspRecPtr->optStr[6], dspTypeStr[dspType]);
            }
	}
    }

    fprintf(tblFp, mibDspTblStr, mibDspTblPtr->name, mibDspTblPtr->name, 
             mibTableName,
             mibDspTblPtr->count, mibDspTblPtr->name, mibDspTblPtr->banner);

}




U8_t *
updateCnt(U8_t *start,
          U32_t incFlag)
{
    U8_t  *str;
    U32_t cnt;

    str = strstr(start, "TL,\n  ");
    if (str)
    {
        str += 6;
        while (*str == ' ')
            str++;
        cnt = atoi(str);
        if (incFlag)
            cnt++;
        else
            cnt--;
        *str = '\0';
        fprintf(tblFp, "%s%u", start, cnt);
        while (*str != '}')
            str++;
    }
    return(str);
}

U8_t *
addTlEnt(U8_t *start,
         U8_t *curPtr)
{
    curPtr = strchr(curPtr, '}');
    *curPtr = '\0';
    fprintf(tblFp, "%s,
    &%sTBL}", start, mibTableName);
    return(updateCnt(curPtr+1, 1));
}

U8_t *
removeTl(U8_t *start, 
         U8_t *curPtr)
{
    U8_t *str;

    /* back to previous entry */
    while (*curPtr != ',' && *curPtr != '{')
        curPtr--;
    if (*curPtr == ',')
    /* previously entry exist */
    {
        *curPtr = '\0';
        curPtr++;
        fprintf(tblFp, start);
        while (*curPtr != ',' && *curPtr != '}')
	/* skip removed entry */
            curPtr++;
        return(updateCnt(curPtr, 0));

    }
    else if (*curPtr == '{')
      /* first entry in the list */
    {
        str = curPtr + 1;
        while (*str != ',' && *str != '}')
	/* skip removed entry */
            str++;

        if (*str == '}')
	/* single entry */
        /* remove whole table */
	{
            while (*curPtr != '\n')
                curPtr--;
            curPtr++;
            *curPtr = '\0';
            curPtr++;
            fprintf(tblFp, start);
            curPtr = strstr(curPtr, "CLICFGTBL_t ");
            if (curPtr)
                curPtr = strstr(curPtr, "};\n");
            if (!curPtr)
                return(NULL);
            return(curPtr + 4);
	}
        return(updateCnt(str, 0));
    }
    return(NULL);
}


makeSnmpCli(U8_t *cnfCmdName, 
            U8_t *mibTableName)
{
    FILE  *fp;
    U32_t len;
    U8_t  *str, *s1, *s2;
    U8_t  buf1[80], buf2[80];

    fileBuf = fileGet(clitblName, &len, 0);
    if (fileBuf)
    {
        len -= 5;
        while (fileBuf[len] != '\n')
            len--;
        while (fileBuf[len] == '\n')
            len--;
        len++;
        fileBuf[len] = '\0';
    }

    if ((tblFp = fopen(clitblName, "w")) == NULL)
    {
        fprintf(stdout, "Can not open %s!\n", clitblName);
        exit(-1);
    }
if (debug) {
printf("file open %x fileBuf %x\n", tblFp, fileBuf);
}
    if (fileBuf)
    {
        str = strstr(fileBuf, "extern ObjectInfo OidList");
        str = strstr(str, "#include");
        if (!str)
	{
            free(fileBuf);
            fclose(tblFp);
            return(0);
	}

        str--;
        *str = '\0';
        str++;
        fprintf(tblFp, fileBuf);

        if (strstr(str, mibTblFileName) == NULL)
            fprintf(tblFp, "\n#include \"%s\"\n", mibTblFileName);
        else
            fprintf(tblFp, "\n");

        sprintf(buf1, "static pCLIMIBCFGGROUP_t %sTL", 
             cnfCmdName);
        s1 = strstr(str, buf1);

        sprintf(buf2, "&%sTBL", mibTableName);

        s2 = strstr(str, buf2);

if (debug) {
printf("s1 = %x s2 = %x\n", s1, s2);
}
        if (s1)
	{
            if (s2)
	    {
                if (s1 > s2)
                {
                    str = removeTl(str, s2);
                    str = addTlEnt(str, s1);
	        }
                else
		{
                    s1 = strchr(s1, '}');
                    if (s1 < s2)
		    {
                        str = addTlEnt(str, s1);
                        str = removeTl(str, s2);
		    }
		}
	    }
            else
	    {
                str = addTlEnt(str, s1);
	    }
            
            fprintf(tblFp, "%s\n\n#endif", str);
            fclose(tblFp);
            free(fileBuf);
            return(0);
	}
        else if (s2)
	{
            str = removeTl(str, s2);
	}
        fprintf(tblFp, str);
    }
    else
    {
        fprintf(tblFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

        fprintf(tblFp, "#ifndef _CLISNMPTBL_H_\n");
        fprintf(tblFp, "#define _CLISNMPTBL_H_\n\n");

        fprintf(tblFp, "#include \"mms_types.h\"\n");
        fprintf(tblFp, "#include \"sr_conf.h\"\n");
        fprintf(tblFp, "#include \"sr_snmp.h\"\n");
        fprintf(tblFp, "#include \"comunity.h\"\n");
        fprintf(tblFp, "#include \"v2clssc.h\"\n");
        fprintf(tblFp, "#include \"context.h\"\n");
        fprintf(tblFp, "#include \"method.h\"\n");
        fprintf(tblFp, "#include \"objectdb.h\"\n");
        /*
        fprintf(tblFp, "#include \"mibout.h\"\n\n");
        */
        fprintf(tblFp, "#include \"snmpdefs.h\"\n");
        fprintf(tblFp, "#include \"snmpsupp.h\"\n");
        fprintf(tblFp, "#include \"snmppart.h\"\n");
        fprintf(tblFp, "#include \"snmptype.h\"\n");
        fprintf(tblFp, "#include \"cli_objs.h\"\n\n");
        fprintf(tblFp, "#define OIDvalue(name, value) { name , (SR_UINT32 *) value },\n\n");

        fprintf(tblFp, "extern ObjectInfo OidList[];\n\n");
        fprintf(tblFp, "#include \"%s\"\n\n", mibTblFileName);

    }

    fprintf(tblFp, "static pCLIMIBCFGGROUP_t %sTL[] = {
    &%sTBL};\n\n", cnfCmdName, mibTableName);

    fprintf(tblFp, "CLICFGTBL_t %sCMTB = {
   \"%s\",
    %sTL,
    1};\n\n#endif\n", cnfCmdName, cnfCmdName, cnfCmdName);
    fclose(tblFp);

    if (fileBuf)
        free(fileBuf);

}

makeSnmpApi()
{
    FILE  *fp;
    U32_t size, len;
    U8_t  *str, *s1, *s2;
    U32_t i;

    fileBuf = fileGet(apitblName, &len, 0);

    if ((tblFp = fopen(apitblName, "w")) == NULL)
    {
        fprintf(stdout, "Can not open %s!\n", apitblName);
        exit(-1);
    }


    if (fileBuf)
    {

if (debug) {
printf("file open %x fileBuf %x\n", tblFp, fileBuf);
printf("makeSnmpApi numOfApiCmd = %u\n", numOfApiCmd);
}

        for (i = 0; i < numOfApiCmd; i++)
	{
            len -= 5;
            while (fileBuf[len] != '\n')
                len--;
            while (fileBuf[len] == '\n')
                len--;
            len += 3;
            fileBuf[len] = '\0';
            adjustApiTbl(fileBuf, &mibApiCmd[i]);

            free(fileBuf);
            if (i != (numOfApiCmd -1) )
	    {
                fileBuf = fileGet(apitblName, &len, 0);

               if ((tblFp = fopen(apitblName, "w")) == NULL)
               {
                   fprintf(stdout, "Can not open %s!\n", apitblName);
                   exit(-1);
               }
	    }
	}
    }
    else
    {
        fprintf(tblFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

        fprintf(tblFp, "#ifndef _CLISNMPTBL_H_\n");
        fprintf(tblFp, "#define _CLISNMPTBL_H_\n\n");

        fprintf(tblFp, "#include \"mms_types.h\"\n");
        fprintf(tblFp, "#include \"sr_conf.h\"\n");
        fprintf(tblFp, "#include \"sr_snmp.h\"\n");
        fprintf(tblFp, "#include \"comunity.h\"\n");
        fprintf(tblFp, "#include \"v2clssc.h\"\n");
        fprintf(tblFp, "#include \"context.h\"\n");
        fprintf(tblFp, "#include \"method.h\"\n");
        fprintf(tblFp, "#include \"objectdb.h\"\n");
        /*
        fprintf(tblFp, "#include \"mibout.h\"\n\n");
        */
        fprintf(tblFp, "#include \"snmpdefs.h\"\n");
        fprintf(tblFp, "#include \"snmpsupp.h\"\n");
        fprintf(tblFp, "#include \"snmppart.h\"\n");
        fprintf(tblFp, "#include \"snmptype.h\"\n");
        fprintf(tblFp, "#include \"cli_objs.h\"\n\n");
        fprintf(tblFp, "#define OIDvalue(name, value) { name , (SR_UINT32 *) value },\n\n");

        fprintf(tblFp, "extern ObjectInfo OidList[];\n\n");
        fprintf(tblFp, "#include \"%s\"\n\n", mibTblFileName);

if (debug) {
printf(" new file \n");
}

        for (i = 0; i < numOfApiCmd; i++)
	{
            makeApiTbl(&mibApiCmd[i]);
	}

        fprintf(tblFp, "\n\n#endif");
        fclose(tblFp);

    }

}

makeCliCmd()
{
    FILE  *fp;
    U32_t size, len;
    U32_t i;
    U8_t  tmpBuf[100];

    fileBuf = fileGet(cliCmdName, &len, 0);

    if ((tblFp = fopen(cliCmdName, "w")) == NULL)
    {
        fprintf(stdout, "Can not open %s!\n", cliCmdName);
        exit(-1);
    }

if (debug) {
printf("file open %x fileBuf %x\n", tblFp, fileBuf);
}
    if (fileBuf)
    {
        fprintf(tblFp, fileBuf);

        sprintf(tmpBuf, " %s(", cnfCmdName);        
        if (numOfCfgCmd &&
            !strstr(fileBuf, tmpBuf))
	{
            fprintf(tblFp, cliCfgFuncStr,
            cnfCmdName, mibTableName,
            cnfCmdName, cnfCmdName, cnfCmdName);
        }

        for (i = 0; i < numOfDspCmd; i++)
	{
            sprintf(tmpBuf, " %s(", mibDspCmd[i].name);        
            if (!strstr(fileBuf, tmpBuf))
	    {
            fprintf(tblFp, cliDspFuncStr,
            mibDspCmd[i].name,
            mibDspCmd[i].name,
            mibDspCmd[i].name,
            snmpExactNext[mibDspCmd[i].cmdType],
            mibDspCmd[i].name);
	    }
	}
        for (i = 0; i < numOfApiCmd; i++)
	{
            sprintf(tmpBuf, " %s(", mibApiCmd[i].name);        
            if (!strstr(fileBuf, tmpBuf))
	    {

            if (mibApiCmd[i].cmdType == DSP_ROW)
                fprintf(tblFp, cliApiFuncStr,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                cnfCmdName,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                cnfCmdName);
            else
                fprintf(tblFp, cliDupFuncStr,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                cnfCmdName,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                cnfCmdName);
	    }
	}

        for (i = 0; i < numOfFltCmd; i++)
	{
            sprintf(tmpBuf, " %s(", mibFltCmd[i].name);        
            if (!strstr(fileBuf, tmpBuf))
	    {
            fprintf(tblFp, cliDspFltFuncStr,
            mibFltCmd[i].name,
            mibFltCmd[i].name,
            cnfCmdName,
            mibFltCmd[i].name,
            mibFltCmd[i].name,
            mibFltCmd[i].name,
            cnfCmdName,
            mibFltCmd[i].name);
	    }
	}

        free(fileBuf);
    }
    else
    {
        fprintf(tblFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

        fprintf(tblFp, "#include \"%s\"\n\n", clitblName);
        fprintf(tblFp, "extern STATUS_t MibCnf(U8_t *, pCLICFGTBL_t, pCLIMIBAPIGROUP_t);\n");
        fprintf(tblFp, "extern STATUS_t MibApiCnf(U8_t *, pCLIMIBAPIGROUP_t, pCLICFGTBL_t);\n");
        fprintf(tblFp, "extern STATUS_t MibApiDup(U8_t *, pCLIMIBAPIGROUP_t, pCLICFGTBL_t);\n");
        fprintf(tblFp, "extern STATUS_t MibShow(U32_t, U8_t *, pCLIMIBDSPGROUP_t);\n");
        fprintf(tblFp, "extern STATUS_t MibShowFlt(U8_t *, pCLIMIBAPIGROUP_t, pCLICFGTBL_t, pCLIMIBDSPGROUP_t);\n\n");

        if (numOfCfgCmd)
	{
            fprintf(tblFp, cliCfgFuncStr,
            cnfCmdName, mibTableName,
            cnfCmdName, cnfCmdName, cnfCmdName);
        }

        for (i = 0; i < numOfDspCmd; i++)
	{
            fprintf(tblFp, cliDspFuncStr,
            mibDspCmd[i].name,
            mibDspCmd[i].name,
            mibDspCmd[i].name,
            snmpExactNext[mibDspCmd[i].cmdType],
            mibDspCmd[i].name);
	}
        for (i = 0; i < numOfApiCmd; i++)
	{
            if (mibApiCmd[i].cmdType == DSP_ROW)
                fprintf(tblFp, cliApiFuncStr,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                cnfCmdName,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                cnfCmdName);
            else
                fprintf(tblFp, cliDupFuncStr,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                cnfCmdName,
                mibApiCmd[i].name,
                mibApiCmd[i].name,
                cnfCmdName);
	}

        for (i = 0; i < numOfFltCmd; i++)
	{
            fprintf(tblFp, cliDspFltFuncStr,
            mibFltCmd[i].name,
            mibFltCmd[i].name,
            cnfCmdName,
            mibFltCmd[i].name,
            mibFltCmd[i].name,
            mibFltCmd[i].name,
            cnfCmdName,
            mibFltCmd[i].name);
	}
    }
    fclose(tblFp);
}


makeShellCmdTbl()
{
    U8_t  *str;
    U32_t len;
    U32_t i;
    U8_t  buf[100], buf_1[100];
    U8_t  tmpBuf[100];
    U8_t  *curBuf;

    fileBuf = fileGet(shellCmdName, &len, 0);

    if ((tblFp = fopen(shellCmdName, "w")) == NULL)
    {
        fprintf(stdout, "Can not open %s!\n", shellCmdName);
        exit(-1);
    }

if (debug) {
printf("file open %x fileBuf %x\n", tblFp, fileBuf);
}
    if (fileBuf)
    {
        str = strstr(fileBuf, "extern");
        if (!str)
	{
            free(fileBuf);
            goto NEWFILE;
	}
        str = strbchr(str, '\n');
        *str = '\0';
        fprintf(tblFp, fileBuf);
        fprintf(tblFp, "\n");
        str++;
        curBuf = str;

        sprintf(tmpBuf, " %s(", cnfCmdName);
        if (numOfCfgCmd &&
            !strstr(curBuf, tmpBuf))
	{
            fprintf(tblFp, shExtCmdStr, cnfCmdName);
        }

        for (i = 0; i < numOfDspCmd; i++)
	{
            sprintf(tmpBuf, " %s(", mibDspCmd[i].name);
            if (!strstr(curBuf, tmpBuf))
	    {
                fprintf(tblFp, shExtCmdStr, mibDspCmd[i].name);
	    }
	}
        for (i = 0; i < numOfApiCmd; i++)
	{
            sprintf(tmpBuf, " %s(", mibApiCmd[i].name);
            if (!strstr(curBuf, tmpBuf))
	    {
                fprintf(tblFp, shExtCmdStr, mibApiCmd[i].name);
	    }
	}

        for (i = 0; i < numOfFltCmd; i++)
	{
            sprintf(tmpBuf, " %s(", mibFltCmd[i].name);
            if (!strstr(curBuf, tmpBuf))
	    {
                fprintf(tblFp, shExtCmdStr, mibFltCmd[i].name);
	    }
	}

        str = strstr(curBuf, shellTblStr);
        str = strchr(str, '\n');
        *str = '\0';
        fprintf(tblFp, curBuf);
        fprintf(tblFp, "\n");
        str++;
        curBuf = str;

        sprintf(buf, "\"%s\"", cnfCmdName);
        sprintf(buf_1, "(CMDFUNC_t)");
        sprintf(&buf_1[11], cnfCmdName);

        if (numOfCfgCmd &&
            !strstr(curBuf, buf))
	{
            fprintf(tblFp, shCmdTblStr, buf,
                  buf_1, cnfCmdAccessLvl,
                  logFlagStr[cnfCmdLogFlag],
                  cnfCmdExcState,
                  cnfCmdHelpMsg);
        }


        for (i = 0; i < numOfDspCmd; i++)
	{
            sprintf(buf, "\"%s\"", mibDspCmd[i].name);
            sprintf(buf_1, "(CMDFUNC_t)");
            sprintf(&buf_1[11], mibDspCmd[i].name);

            if (!strstr(curBuf, buf))
	    {
                fprintf(tblFp, shCmdTblStr, buf,
                  buf_1, mibDspCmd[i].accessLvl,
                  logFlagStr[mibDspCmd[i].logFlag],
                  mibDspCmd[i].excState,
                  mibDspCmd[i].helpMsg);

	    }
	}
        for (i = 0; i < numOfApiCmd; i++)
	{
            sprintf(buf, "\"%s\"", mibApiCmd[i].name);
            sprintf(buf_1, "(CMDFUNC_t)");
            sprintf(&buf_1[11], mibApiCmd[i].name);

            if (!strstr(curBuf, buf))
	    {
                fprintf(tblFp, shCmdTblStr, buf,
                  buf_1, mibApiCmd[i].accessLvl,
                  logFlagStr[mibApiCmd[i].logFlag],
                  mibApiCmd[i].excState,
                  mibApiCmd[i].helpMsg);
	    }
	}

        for (i = 0; i < numOfFltCmd; i++)
	{
            sprintf(buf, "\"%s\"", mibFltCmd[i].name);
            sprintf(buf_1, "(CMDFUNC_t)");
            sprintf(&buf_1[11], mibFltCmd[i].name);
            if (!strstr(curBuf, buf))
	    {
                fprintf(tblFp, shCmdTblStr, buf,
                  buf_1, mibFltCmd[i].accessLvl,
                  logFlagStr[mibFltCmd[i].logFlag],
                  mibFltCmd[i].excState,
                  mibFltCmd[i].helpMsg);
	    }
	}




        fprintf(tblFp, curBuf);
        free(fileBuf);
    }
    else
    {
NEWFILE:
        fprintf(tblFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

        fprintf(tblFp, "#include \"cli_if.h\"\n\n");
	/*
        fprintf(tblFp, "#ifndef _CLIMIBTBL_H_\n");
        fprintf(tblFp, "#define _CLIMIBTBL_H_\n\n");
	*/

        if (numOfCfgCmd)
	{
            fprintf(tblFp, shExtCmdStr, cnfCmdName);
        }

        for (i = 0; i < numOfDspCmd; i++)
	{
            fprintf(tblFp, shExtCmdStr, mibDspCmd[i].name);
	}
        for (i = 0; i < numOfApiCmd; i++)
	{
            fprintf(tblFp, shExtCmdStr, mibApiCmd[i].name);
	}
        for (i = 0; i < numOfFltCmd; i++)
	{
            fprintf(tblFp, shExtCmdStr, mibFltCmd[i].name);
	}

        fprintf(tblFp, "\n\n\n");

        fprintf(tblFp, "%s", shellTblStr);



        if (numOfCfgCmd)
	{
            sprintf(buf, "\"%s\"", cnfCmdName);
            sprintf(buf_1, "(CMDFUNC_t)");
            sprintf(&buf_1[11], cnfCmdName);
            fprintf(tblFp, shCmdTblStr, buf,
                  buf_1, cnfCmdAccessLvl,
                  logFlagStr[cnfCmdLogFlag],
                  cnfCmdExcState,
                  cnfCmdHelpMsg);
        }

        for (i = 0; i < numOfDspCmd; i++)
	{
            sprintf(buf, "\"%s\"", mibDspCmd[i].name);
            sprintf(buf_1, "(CMDFUNC_t)");
            sprintf(&buf_1[11], mibDspCmd[i].name);

            fprintf(tblFp, shCmdTblStr, buf, 
            buf_1, mibDspCmd[i].accessLvl,
            logFlagStr[mibDspCmd[i].logFlag],
                  mibDspCmd[i].excState,
                  mibDspCmd[i].helpMsg);
	}

        for (i = 0; i < numOfApiCmd; i++)
	{
            sprintf(buf, "\"%s\"", mibApiCmd[i].name);
            sprintf(buf_1, "(CMDFUNC_t)");
            sprintf(&buf_1[11], mibApiCmd[i].name);

            fprintf(tblFp, shCmdTblStr, buf,
            buf_1, mibApiCmd[i].accessLvl,
            logFlagStr[mibApiCmd[i].logFlag],
                  mibApiCmd[i].excState,
                  mibApiCmd[i].helpMsg);
	}

        for (i = 0; i < numOfFltCmd; i++)
	{
            sprintf(buf, "\"%s\"", mibFltCmd[i].name);
            sprintf(buf_1, "(CMDFUNC_t)");
            sprintf(&buf_1[11], mibFltCmd[i].name);

            fprintf(tblFp, shCmdTblStr, buf,
            buf_1, mibFltCmd[i].accessLvl,
            logFlagStr[mibFltCmd[i].logFlag],
                  mibFltCmd[i].excState,
                  mibFltCmd[i].helpMsg);
	}

        i = 0;
        fprintf(tblFp, "    {%-27s, %-25s,          0,  0, %8s}};\n\n%s\n\n\n",
            "NULL","NULL", "0", clishFuncStr);
    }
    fclose(tblFp);
}


makeApiTbl(pMIBAPICMD_t apiCmdPtr)
{
    pMIBENTRYINF_t mibEInfoPtr;
    U32_t  j, k = 1;
    U8_t   constBuf[100];
    U8_t   buf[100];
    MIBENTAPI_t    oldApi[50];
    U32_t numOfArg = 0;

    fprintf(tblFp, apiObjName, apiCmdPtr->name);
    fprintf(tblFp, "\n");
    constBuf[0] = '\0';


{
    printf(apiObjName, apiCmdPtr->name);
    printf("apiCmdPtr->count = %u\n", apiCmdPtr->count);
}
    for (j = 0; j < apiCmdPtr->count; j++)
    {
        mibEInfoPtr = mibOptGetEnt(apiCmdPtr->apiEnt[j].optStr);
        if (mibEInfoPtr == NULL)
        {
            fprintf(stdout, "%u API %s OPT %s not exist!!\n",
            apiCmdPtr->name, apiCmdPtr->apiEnt[j].optStr);
            exit(-1);
        }
        if (apiCmdPtr->apiEnt[j].argType == VAR_OPT)
        {
            if (numOfArg > (apiCmdPtr->apiEnt[j].argNum-1))
            {
                sprintf(oldApi[apiCmdPtr->apiEnt[j].argNum-1].optStr, "&%sENT", mibEInfoPtr->name);
                oldApi[apiCmdPtr->apiEnt[j].argNum-1].sepCh = apiCmdPtr->apiEnt[j].sepCh;
	    }
            else
	    {
                for (k = numOfArg; k <apiCmdPtr->apiEnt[j].argNum-1; k++)
		{
                    strcpy(oldApi[k].optStr, "NULL");
                    oldApi[k].sepCh = 0;
	        }
		
                sprintf(oldApi[apiCmdPtr->apiEnt[j].argNum-1].optStr, "&%sENT", mibEInfoPtr->name);
                oldApi[apiCmdPtr->apiEnt[j].argNum-1].sepCh = apiCmdPtr->apiEnt[j].sepCh;
                numOfArg = apiCmdPtr->apiEnt[j].argNum;
	    }
        }
        else
        {
            sprintf(&constBuf[strlen(constBuf)], "-%s %s ",
                 apiCmdPtr->apiEnt[j].optStr,
                 apiCmdPtr->apiEnt[j].fmtStr);
        }
    }
    for (k = 0; k < numOfArg; k++)
    {
        if (strncmp(oldApi[k].optStr, "NULL", 4))
            fprintf(tblFp, "    {%48s, '%c'},", oldApi[k].optStr, 
                oldApi[k].sepCh);
        else
            fprintf(tblFp, "    {%48s,   0},", "NULL");
    
        fprintf(tblFp, "\n");
    
    }
    fprintf(tblFp, "    {%48s,   0}\n};\n\n", "NULL");


    fprintf(tblFp, apicmdtbl, apiCmdPtr->name, apiCmdPtr->name, 
        apiCmdPtr->name, constBuf);
}



adjustApiTbl(U8_t *fileBuf,
             pMIBAPICMD_t apiCmdPtr)
{
    U8_t  buf[100];
    U8_t   constBuf[100];
    U8_t  *str, *str1, *curBuf;
    U32_t j, k = 0;
    pMIBENTRYINF_t mibEInfoPtr;
    U32_t numOfArg;
    MIBENTAPI_t    oldApi[50];
    U32_t inx = 0;
    U8_t   *tmp;

    sprintf(buf, apiObjName, apiCmdPtr->name);

    str = strstr(fileBuf, buf);
    if (str)
    {
if (debug) {
printf("adjustApiTbl Opt list exist %x %u\n", str, strlen(fileBuf));
}
        constBuf[0] = '\0';
        /* advance string point to first member */
        str = strchr(str, '{');
        str = strchr(str+1, '{');
	/* back to beginning of line */
        str = strbchr(str, '\n');
        *str = '\0';
        fprintf(tblFp, "%s\n", fileBuf);
        str++;

        str1 = strstr(str, "};");
        numOfArg = (str1 - str + 2)/API_ARG_SZ - 1;
if (debug) {
printf("adjustApiTbl numOfArg = %u %u\n", numOfArg, (str1 - str + 2));
}

        for (inx = 0; inx < numOfArg; inx++)
	{
	    str = strchr(str, '{');
            str = strpskip(str+1, " ");
            tmp = strchr(str, ',');
            *tmp = '\0';
            strcpy(oldApi[inx].optStr, str);
            str = strpskip(tmp+1, " ");
            if (*str == '\'')
                str++;
            oldApi[inx].sepCh = *str;
            str = strchr(str, '\n')+1;

if (debug) {
printf("adjustApiTbl %s %x\n", oldApi[inx].optStr, oldApi[inx].sepCh);
}

	}

        for (j = 0; j < apiCmdPtr->count; j++)
        {
if (debug) {
printf("adjustApiTbl Opt = %s\n", apiCmdPtr->apiEnt[j].optStr);
}
            mibEInfoPtr = mibOptGetEnt(apiCmdPtr->apiEnt[j].optStr);
            if (mibEInfoPtr == NULL)
            {
                fprintf(stdout, "%u API %s OPT %s not exist!!\n",
                apiCmdPtr->name, apiCmdPtr->apiEnt[j].optStr);
                exit(-1);
            }
            if (apiCmdPtr->apiEnt[j].argType == VAR_OPT)
            {
                U8_t *tmp;

                for (k = 0; k < numOfArg; k++)
		{
                    if (!strncmp(&(oldApi[k].optStr[1]), mibEInfoPtr->name,
                        strlen(mibEInfoPtr->name)))
		    {
if (debug) {
printf("adjustApiTbl match k = %u name = %s\n", k, mibEInfoPtr->name);
}
                        if ((k + 1) != apiCmdPtr->apiEnt[j].argNum)
			{
                            strcpy(oldApi[k].optStr, "NULL");
                            oldApi[k].sepCh = 0;
			}
                        break;
                      
		     }
		}
if (debug) {
printf("adjustApiTbl argNum =  %u\n", apiCmdPtr->apiEnt[j].argNum);
}
                if (numOfArg > (apiCmdPtr->apiEnt[j].argNum-1))
		{
if (debug) {
printf("adjustApiTbl k = %u\n", k);
}
                        sprintf(oldApi[apiCmdPtr->apiEnt[j].argNum-1].optStr, "&%sENT", mibEInfoPtr->name);
                        oldApi[apiCmdPtr->apiEnt[j].argNum-1].sepCh = apiCmdPtr->apiEnt[j].sepCh;
		}
                else
		{
if (debug) {
printf("adjustApiTbl oor k = %u\n", k);
}
                    for (k = numOfArg; k <apiCmdPtr->apiEnt[j].argNum-1; k++)
		    {
                        strcpy(oldApi[k].optStr, "NULL");
                        oldApi[k].sepCh = 0;
	            }

                    sprintf(oldApi[apiCmdPtr->apiEnt[j].argNum-1].optStr, "&%sENT", mibEInfoPtr->name);
                    oldApi[apiCmdPtr->apiEnt[j].argNum-1].sepCh = apiCmdPtr->apiEnt[j].sepCh;
                    numOfArg = apiCmdPtr->apiEnt[j].argNum;
		}

            }
            else
            {
                sprintf(&constBuf[strlen(constBuf)], "-%s %s ",
                     apiCmdPtr->apiEnt[j].optStr,
                     apiCmdPtr->apiEnt[j].fmtStr);
if (debug) {
printf("adjustApiTbl constBuf = %s\n", constBuf);
}
            }
        }

        while (numOfArg)
	{
            if (strncmp(oldApi[numOfArg-1].optStr, "NULL", 4))
                break;

            numOfArg--;

	}
        for (k = 0; k < numOfArg; k++)
	{
            if (strncmp(oldApi[k].optStr, "NULL", 4))
                fprintf(tblFp, "    {%48s, '%c'},", oldApi[k].optStr, 
                    oldApi[k].sepCh);
            else
                fprintf(tblFp, "    {%48s,   0},", "NULL");

            fprintf(tblFp, "\n");

	}
        fprintf(tblFp, "    {%48s,   0}\n};\n\n", "NULL");

        curBuf = str1+4;

if (debug) {
printf("adjustApiTbl fileBuf len = %u\n", strlen(fileBuf));
}
        if (constBuf[0] != '\0')
	{
            sprintf(buf, "%sAPI", apiCmdPtr->name);
            str = strstr(curBuf, buf);
            if (!str)
            {
                fprintf(stdout, "%s %s Table and Entry not match!!\n",
                apiCmdPtr->name, buf);
                exit(-1);
    
            }
            sprintf(buf, "%sOPT", apiCmdPtr->name);
            str = strstr(str, buf);
            if (!str)
            {
                fprintf(stdout, "%s %s Table and Entry not match!!\n",
                apiCmdPtr->name, buf);
                exit(-1);
            }

            /* search for addition option string */
            str1 = strchr(str, '"');
            str = strchr(str1+1, '"');
            if (!str)
            {
                fprintf(stdout, "%u Table and Entry not match!!\n",
                apiCmdPtr->name);
                exit(-1);
            }
            *str = '\0';
if (debug) {
printf("adjustApiTbl fileBuf len = %u constBuf len = %u str len = %u\n",
    strlen(fileBuf), strlen(constBuf), strlen(str+1));
}
            if (!strstr(str1, constBuf)) 
                fprintf(tblFp, "%s%s\"%s", curBuf, constBuf, str+1);
            else
                fprintf(tblFp, "%s\"%s", curBuf, str+1);
	}
        else
            fprintf(tblFp, "%s", curBuf);
    }
    else
    {
        fprintf(tblFp, "%s", fileBuf);
        makeApiTbl(apiCmdPtr);	
    }
    fprintf(tblFp, "\n\n#endif");
    fclose(tblFp);
}


U8_t *
fileGet(U8_t *fName,
       U32_t *len,
       U32_t offset)
{
    FILE *fp;
    U8_t *fileBuf;


    if ((fp = fopen(fName, "r")) != NULL)
    {
        if ((fileBuf = malloc(0x20000)) == NULL)
        {
            fprintf(stdout, "Fail to get alloc mem for file %s!\n", fName);
            exit(-1);
        }
        *len = fread(&fileBuf[offset], 1, 0x20000, fp);
        if (*len < 10)
	{
            free(fileBuf);
            fileBuf = NULL;
        }
        else
	{
            fileBuf[*len] = '\0';
	}
        if (*len >= 0x20000)
        {
            free(fileBuf);
            fileBuf = NULL;
            printf("%s size: %x, increase malloc on fileGet", fName, *len);
            exit(-1);
        }
        fclose(fp);
if (debug) {
printf("file name = %s len = %x\n", fName, *len);
}

    }
    else
        fileBuf = NULL;

    return(fileBuf);
}

U8_t *
cmdFieldGet(U8_t *str, 
            U8_t *name, 
            U32_t *accessLvl, 
            U32_t *logFlag,
            U32_t *excFlag,
            U8_t  *helpStr)
{
    U8_t ch;
    U8_t *str1;

    str = cliArgGet(str);
    if (*str != '<')
    {
        fprintf(stdout, "Missing name field!!\n");
        exit(-1);

    }
    str = cliArgGet(str);
    str1 = strpbrk(str, "<>{}[]; \t");
    if (!str1)
    {
        fprintf(stdout, "Illegal name field terminator!!\n");
        exit(-1);
    }

    ch = *str1;
    *str1 = '\0';
    strcpy(name, str);
    *str1 = ch;
    str = strpskip(str1, " \t");


    if (*str == '>')
    {
        *accessLvl = DEFAULT_LEVEL;
        *logFlag = DEFAULT_LOGFLAG;
        return(cliArgGet(str));
    }
    if (*str != ';')
    {
        fprintf(stdout, "Illegal name field seperator for %s!!\n", name);
        exit(-1);
    }

    str++;
    *accessLvl = strtol(str, NULL, 10);

    str = cliArgGet(str);

if (debug) {
printf("ch = %c %u\n", *str, *accessLvl);
}

    if (*str != ';' && *str != '>')
    {
        fprintf(stdout, "Illegal name field seperator for %s!!\n", name);
        exit(-1);
    }
    if (*str == ';')
    {
        str++;
        str = strpskip(str, " \t");
        if (!strncmp(str, "LOG", 3))
            *logFlag = 1;
        else if (!strncmp(str, "NOLOG", 5))
            *logFlag = 0;
        str = cliArgGet(str);
    }

    if (*str == '>')
    {
        *excFlag = ACTIVE_EXC_STATE;
        helpStr[0] = '\0';
    }
    else
    {
        if (*str == ';')
        {
            str++;
            str = strpskip(str, " \t");
            *excFlag = 0;
            while (*str && *str != ';' && *str != '>')
	    {
                if (*str == 'A')
                    *excFlag |= ACTIVE_EXC_STATE;
                else if (*str == 'S')
                    *excFlag |= STANDBY_EXC_STATE;
                else if (*str == 'E')
                    *excFlag = ALL_EXC_STATE;
                str++;
            }
            if (!*excFlag)
                *excFlag = ACTIVE_EXC_STATE;
	}

        if (*str == '>')
	{
            helpStr[0] = '\0';
	}
        else
	{
            if (*str == ';')
            {
                str = cliArgGet(str);

                str = cliStrGet(str, helpStr, name, 
                    "CMD Help String", "CMD Help String");
if (debug)
printf("*****\n%s\n", helpStr);
	    }
	}
    }
    if (*str != '>')
    {
        fprintf(stdout, "Illegal name field terminator for %s!!\n", name);
        exit(-1);
    }

    return(cliArgGet(str));
}

/* yshen */

makeSnmpFlt()
{
    FILE  *fp;
    U32_t size, len;
    U8_t  *str, *s1, *s2;
    U32_t i;

    fileBuf = fileGet(apitblName, &len, 0);

    if ((tblFp = fopen(apitblName, "w")) == NULL)
    {
        fprintf(stdout, "Can not open %s!\n", apitblName);
        exit(-1);
    }


    if (fileBuf)
    {

        for (i = 0; i < numOfFltCmd; i++)
	{
            len -= 5;
            while (fileBuf[len] != '\n')
                len--;
            while (fileBuf[len] == '\n')
                len--;
            len += 3;
            fileBuf[len] = '\0';
            adjustApiTbl(fileBuf, &mibFltCmd[i]);

            free(fileBuf);
            if (i != (numOfFltCmd -1) )
	    {
                fileBuf = fileGet(apitblName, &len, 0);

               if ((tblFp = fopen(apitblName, "w")) == NULL)
               {
                   fprintf(stdout, "Can not open %s!\n", apitblName);
                   exit(-1);
               }
	    }
	}
    }
    else
    {
        fprintf(tblFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

        fprintf(tblFp, "#ifndef _CLISNMPTBL_H_\n");
        fprintf(tblFp, "#define _CLISNMPTBL_H_\n\n");

        fprintf(tblFp, "#include \"mms_types.h\"\n");
        fprintf(tblFp, "#include \"sr_conf.h\"\n");
        fprintf(tblFp, "#include \"sr_snmp.h\"\n");
        fprintf(tblFp, "#include \"comunity.h\"\n");
        fprintf(tblFp, "#include \"v2clssc.h\"\n");
        fprintf(tblFp, "#include \"context.h\"\n");
        fprintf(tblFp, "#include \"method.h\"\n");
        fprintf(tblFp, "#include \"objectdb.h\"\n");
        /*
        fprintf(tblFp, "#include \"mibout.h\"\n\n");
        */
        fprintf(tblFp, "#include \"snmpdefs.h\"\n");
        fprintf(tblFp, "#include \"snmpsupp.h\"\n");
        fprintf(tblFp, "#include \"snmppart.h\"\n");
        fprintf(tblFp, "#include \"snmptype.h\"\n");
        fprintf(tblFp, "#include \"cli_objs.h\"\n\n");
        fprintf(tblFp, "#define OIDvalue(name, value) { name , (SR_UINT32 *) value },\n\n");

        fprintf(tblFp, "extern ObjectInfo OidList[];\n\n");
        fprintf(tblFp, "#include \"%s\"\n\n", mibTblFileName);

if (debug) {
printf(" new file \n");
}

        for (i = 0; i < numOfFltCmd; i++)
	{
            makeApiTbl(&mibFltCmd[i]);
	}

        fprintf(tblFp, "\n\n#endif");
        fclose(tblFp);

    }

}

U8_t *
parseFltDsp(
	U8_t            *buf, 
	pMIBAPICMD_t    apiCmdPtr,
	pMIBDSPCMD_t    dspCmdPtr)

{
    U8_t  *str;
    pMIBENTAPI_t apiEntPtr;
    U8_t  apiType[40];
    pMIBENTDSP_t dspEntPtr;
    U8_t  strBuf[400];
    U32_t curPos, pos, fmtInx;
    U32_t len, j;
    U8_t  *fmtBuf;

    str = cliArgGet(buf);
    if (*str != '{')
    {
        fprintf(stdout, "missing opening { for OPT!!\n");
        exit(-1);
    }


    apiEntPtr = apiCmdPtr->apiEnt;
    apiCmdPtr->count = 0;

    dspEntPtr = dspCmdPtr->dspEnt;
    dspCmdPtr->count = 0;

    str = cmdFieldGet(str, apiCmdPtr->name, &apiCmdPtr->accessLvl, 
                      &apiCmdPtr->logFlag, &apiCmdPtr->excState, apiCmdPtr->helpMsg);

    strcpy(dspCmdPtr->name, apiCmdPtr->name);
    dspCmdPtr->accessLvl = apiCmdPtr->accessLvl;
    dspCmdPtr->logFlag = apiCmdPtr->logFlag;
    dspCmdPtr->excState = apiCmdPtr->excState;
    strcpy(dspCmdPtr->helpMsg, apiCmdPtr->helpMsg);

    str--;

    str = clifieldGet(str, apiType, apiCmdPtr->name, "FLT DSP CMD TYPE", 
        "FLT DSP CMD TYPE");

    if (!strncmp(apiType, "column", 6))
        apiCmdPtr->cmdType = DSP_COLUMN;
    else
    {
        fprintf(stdout, "FLT DSP CMD Type must be column %s!!\n", 
                apiCmdPtr->name);
        exit(-1);

    }

    dspCmdPtr->cmdType = DSP_COLUMN;

    /* parse FLT part (follow API) */

    str = cliArgGet(str);

    if (*str == '<')
    {
        str++;
        while (1)
	{
            if (*str == '>')
                break;
            if (*str == '}' || *str == '{')
	    {
                fprintf(stdout, "Illegal termination for FLT DSP %s!!\n", 
                 apiCmdPtr->name);
                exit(-1);
	    }
            str = strpskip(str, " \t");
            if (*str != '[')
	    {
                fprintf(stdout, "Illegal field Brace for FLT DSP %s!!\n",
                     apiCmdPtr->name);
                exit(-1);

	    }
            str++;
            str = strpskip(str, " \t");

            if (*str != ';')
	    {
                apiEntPtr->sepCh = *str;
                str++;
                str = strpskip(str, " \t");

                if (*str != ';')
		{
                     fprintf(stdout, "Missing field seperator for %s string!!\n", 
                     apiCmdPtr->name);
                     exit(-1);

		}
	    }
            else 
                apiEntPtr->sepCh = ' ';
            str++;
        
            str = clistrpcpy(apiEntPtr->optStr, str, ';', "\n[]<>{}");
            if (!str)
	    {
                fprintf(stdout, "Illegal char in option field %s\n", 
                        apiCmdPtr->name);
                exit(-1);	    
	    }


            str = cliArgGet(str);
        
            if (*str != '%')
	    {
                U8_t *str1, ch;

                apiEntPtr->argType = CONST_OPT;
                str1 = strpbrk(str, "\n[]<>{}\t ");
                if (str1)
		{
                    ch = *str1;
                    *str1 = '\0';
		}
                strcpy(apiEntPtr->fmtStr, str);
                if (str1)
		{
                    *str1 = ch;
		}
                str = str1;

	    }
            else
	    {
                str++;
                if (isdigit(*str))
		{
                    apiEntPtr->argNum = strtol((char*)str, (char**)&str, 10);
                
                    str = strpskip(str, " \t");
                    apiEntPtr->argType = VAR_OPT;
		}
                else
                {
                    fprintf(stdout, "Illegal  format!!\n");
                    exit(-1);
                }
	    }

            if (!str)
	    {
                fprintf(stdout, "Illegal format char for API %s\n", 
                    apiCmdPtr->name);
                exit(-1);	    
	    }

            str = cliArgGet(str);
            apiEntPtr++;
            apiCmdPtr->count++;
	}
    }
    else
    {
        fprintf(stdout, "FLT DSP %s missing argument list!!\n", 
         apiCmdPtr->name);
        exit(-1);
    }

    /* parse DISPLAY part (follow DSP) */

    str = cliArgGet(str);

    dspCmdPtr->banner[0] = '\0';
    curPos = 0;
    fmtInx = 0;
    fmtBuf = dspCmdPtr->banner;

    if (*str == '<')
    {
        str++;
        str = strpskip(str, " \t");
        while (1)
	{
            if (*str == '>')
                break;
            if (*str == '@')
                str = dspPosGet(str, &pos);
            else
                pos = -1;
	
            if (*str == '"')
	    {
                str++;
                str = clistrpcpy(strBuf, str, '"', "\n");
                if (!str)
	        {
                    fprintf(stdout, "DSP banner string need be in the same line %s!!\n",
                    dspCmdPtr->name);
                    exit(-1);
	        }
            }
            else
    	    {
                 fprintf(stdout, "Missing field seperator for %s banner string!!\n", 
                 dspCmdPtr->name);
                 exit(-1);
	    }
            str++;

            if ((str = strpskip(str, " \t")) == NULL)
	    {
                 fprintf(stdout, "Illegal %s banner format\n", 
                 dspCmdPtr->name);
                 exit(-1);
	    }

            if (*str != ';' && *str != '>')
	    {
                 fprintf(stdout, "Illegal %s banner field seperator  at %s\n", 
                 dspCmdPtr->name, strBuf);
                 exit(-1);
	    }

            if (*str == ';')
                str = cliArgGet(str);

            if (pos != -1)
	    {
                if (pos < curPos)
		{
                    fmtBuf[fmtInx++] = '\n';
                    curPos = 0;
		}
                if (pos > curPos)
	        {
                    for (j = pos - curPos; j; j--)
                        fmtBuf[fmtInx++] = ' ';
                    curPos = pos;
	        }
            }

            strcpy(&fmtBuf[fmtInx], strBuf);
            len = strlen(strBuf);
            curPos += len;
            fmtInx += len;
        }
    }

    str = cliArgGet(str);

    if (*str == '<')
    {
        str++;
        while (1)
	{

            if (*str == '>')
                break;
            if (*str == '}' || *str == '{')
	    {
                fprintf(stdout, "Illegal termination for DSP %s!!\n", 
                 dspCmdPtr->name);
                exit(-1);
	    }

            str = strpskip(str, " \t");
            if (*str != '[')
	    {
                fprintf(stdout, "Illegal field Brace for DSP %s!!\n",
                     dspCmdPtr->name);
                exit(-1);

	    }
            str++;

            str = strpskip(str, " \t");

            if (*str != ';')
	    {
                if (*str == '@')
                    str = dspPosGet(str, &(dspEntPtr->strPos));
                else
                    dspEntPtr->strPos = -1;

                if (*str == '"')
		{
                    str++;
                    str = clistrpcpy(dspEntPtr->dspStr, str, '"', "\n");
                    if (!str)
		    {
                        fprintf(stdout, "DSP string need be in the same line %s!!\n",
                        dspCmdPtr->name);
                        exit(-1);
		    }

                }
                else
		{
                     fprintf(stdout, "Missing field seperator for %s string!!\n", 
                     dspCmdPtr->name);
                     exit(-1);
		}
                str++;
                if ((str = strpskip(str, " \t")) == NULL)
		{
                     fprintf(stdout, "Missing 1 field seperator for %s string!!\n", 
                     dspCmdPtr->name);
                     exit(-1);

		}

                if (*str != ';')
		{
                     fprintf(stdout, "Missing field seperator for %s string!!\n", 
                     dspCmdPtr->name);
                     exit(-1);

		}
	    }
            else
	    {
                dspEntPtr->strPos = -1;
                dspEntPtr->dspStr[0] = '\0';
	    }
            str = cliArgGet(str);
        
            if (*str == '@')
                str = dspPosGet(str, &(dspEntPtr->datPos));
            else
                dspEntPtr->datPos = -1;

            str = clistrpcpy(dspEntPtr->optStr, str, ';', "\n[]<>{}");
            if (!str)
	    {
                fprintf(stdout, "Illegal char in option field %s\n", 
                        dspCmdPtr->name);
                exit(-1);	    
	    }

            str = cliArgGet(str);
        
            if (*str != '%')
	    {
                fprintf(stdout, "Illegal format char for DSP %s\n", 
                        dspCmdPtr->name);
                exit(-1);	    

	    }

            str = clistrpcpy(dspEntPtr->fmtStr, str, ']', "\n[]<>{}");
            if (!str)
	    {
                fprintf(stdout, "Illegal format char for DSP %s\n", 
                    dspCmdPtr->name);
                exit(-1);	    
	    }

            str = cliArgGet(str);
            dspEntPtr++;
            dspCmdPtr->count++;

	} /* while */

    }
    else
    {
        fprintf(stdout, "FLT DSP %s missing argument list!!\n", 
        dspCmdPtr->name);
        exit(-1);
    }
}




