#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define TYPE_DATA_32  0
#define TYPE_STR      1
#define TYPE_OID      2

#define BUFFER_SIZE   0x50000

typedef unsigned char  U8_t;
typedef unsigned short U16_t;
typedef unsigned long  U32_t;


U8_t *typeName[] = {
"TYPE_DATA_32",
"TYPE_STR",
"TYPE_OID"};



U8_t *strbchr(U8_t *str, U8_t ch);

U8_t *strcskip(U8_t *s1, U8_t ch);
U8_t *strpskip(U8_t *s1, U8_t *s2); 
U8_t *findTblEnd(U8_t *str);
U8_t *getTypenName(U8_t *str, U8_t *name, U8_t *type);
U8_t *fileGet(U8_t *fName, U32_t *len, U32_t offset);

U32_t debug = 0;

main(argc,argv)
int argc;
char *argv[];
{
    FILE  *tblFp, *tmpFp;
    FILE  *defFp;
    U32_t len;
    U32_t i;
    U8_t  tmpBuf[80];
    U32_t ttlcnt, ttlTbl;
    U8_t  namePtr[80], typePtr[80];
    U8_t  *lenStr;
    U8_t  *fileBuf, *defBuf, *typeStr;
    U8_t  *s1, *s2, *tblName;
    U32_t type;
    FILE  *test1;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == 'd')
                debug = 1;
        }
        else
        {
            fprintf(stdout, "Syntax error!\n");
            exit(-1);
        }
    } 

    if ((fileBuf = fileGet("snmptype.h", &len, 1)) == NULL)
    {
        fprintf(stdout, "Can not open %s\n", "snmptype.h");
        exit(-11);
    }

    if ((defBuf = fileGet("mibtlen.con", &len, 0)) == NULL)
    {
        fprintf(stdout, "Can not open %s\n", "mibtlen.con");
        exit(-11);
    }

    test1 = fopen("snmptype.h.keep", "w");
    fprintf(test1, fileBuf);
 
    if ((tblFp = fopen("mibtype.h", "w")) == NULL)
    {
        fprintf(stdout, "Open fail on %s\n",  "mibtype.h");
        exit(-1);

    }
    if ((tmpFp = fopen("mibtmp.h", "w")) == NULL)
    {
        fprintf(stdout, "Open fail on %s\n",  "mibtmp.h");
        exit(-1);

    }

    if ((defFp = fopen("mibtid.h", "w")) == NULL)
    {
        fprintf(stdout, "Open fail on %s\n",  "mibtid.h");
        exit(-1);

    }

    fprintf(tblFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

    fprintf(tblFp, "#ifndef _MIBTYPE_H_\n");
    fprintf(tblFp, "#define _MIBTYPE_H_\n\n");

    fprintf(tblFp, "#include \"mms_types.h\"\n\n");
    fprintf(tblFp, "#include \"mibrcd.h\"\n\n");

    fprintf(tblFp, defBuf);

    fprintf(defFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

    fprintf(defFp, "#ifndef _MIBTID_H_\n");
    fprintf(defFp, "#define _MIBTID_H_\n\n");
	    
    typeStr = &fileBuf[1];
    ttlTbl = 0;

if (debug) {
printf("%s\n, fileBuf = %x\n",fileBuf, fileBuf); 
}
    while (1)
    {
        /* find table */
        typeStr = strstr(typeStr, "struct");

if (debug) {
printf(" struct = %x\n", typeStr);
}

        if (!typeStr)
        {
            break;
        }

        /* skip word struct */
        typeStr += 6;

        /* find table name */
        s1 = strchr(typeStr, '{');
        *s1 = '\0';
        s1++;

        typeStr = strcskip(typeStr, ' ');
        if (!typeStr)
	{
            typeStr = findTblEnd(s1);
            if (!typeStr)
	    {
                exit(-1);
	    }
            continue;
	}

        tblName = strcskip(typeStr, '_');
        s2 = strstr(tblName, "_t");

        if (s2)
            *s2 = '\0';
if (debug) {
printf("tblName = %s\n", tblName);
}

        s2 = findTblEnd(s1);

if (debug) {
printf("tbl end = %x\n", s2);
}

        if (!s2)
	{
            exit(-1);
	}

        s2 = strcskip(s2, ' ');
        
        if (strncmp(tblName, s2, strlen(tblName)))
	{
            typeStr = s2;
            continue;
	}

        typeStr = s1;
        s1 = strstr(typeStr, " valid[");
if (debug) {
printf("tbl end = %x\n", s2);
}
        
        if (!s1 || s1 > s2)
	{
            typeStr = s2;
            continue;
	}

        ttlcnt = 0;
        while (1)
	{
            typeStr = getTypenName(typeStr, namePtr, typePtr);
if (debug) {
printf("name = %s type = %s\n", namePtr, typePtr);
}
            if (strncmp(namePtr, "valid[", 6))
	    {
                type = fieldType(typePtr);
                if (type == TYPE_STR ||
                    type == TYPE_OID)
	        {
	          /* find defined length or use default length */
                  if (strstr(defBuf, namePtr))
		  {
                      sprintf(tmpBuf, "%s_TPLEN", namePtr);
                      lenStr = tmpBuf;
		  }
                  else if (type == TYPE_STR)
		  {
                      lenStr = "DEFAULT_STRLEN";
		  }
                  else
                      lenStr = "DEFAULT_OIDLEN";

 	        }
                else
                      lenStr = "0";
                if (!ttlcnt)
                    fprintf(tblFp, "static MIB_DATA_TP_t  %s_TL[] =  {\n", tblName);
                fprintf(tblFp, "{%15s, %40s},\n",  typeName[type], lenStr);
                ttlcnt++;
	    }
            else
                break;
	}
if (debug) {
printf("tblName = %s ttlcnt = %u ttlTbl = %u\n", tblName, ttlcnt, ttlTbl);
}
        if (ttlcnt)
	{
            sprintf(tmpBuf, "DB_%s_TID", tblName);
            fprintf(defFp, "#define %40s     %u\n", tmpBuf, ttlTbl);
            fprintf(tmpFp, "  {%s_TL, %u},\n", tblName, ttlcnt);
            fprintf(tblFp, "};\n\n");
	}
        ttlTbl++;
        typeStr = s2;
    }
    fclose(tmpFp);
    free(defBuf);
    free(fileBuf);

    if (ttlTbl)
    {
        fprintf(tblFp, "\nMIB_TPLEN_RCD_t mibTpInfo[] =  {\n");

        if ((defBuf = fileGet("mibtmp.h", &len, 0)) == NULL)
        {
            fprintf(stdout, "Can not open %s\n", "mibtmp.h");
            exit(-11);
        }
        fprintf(tblFp, defBuf);
        fprintf(tblFp, "};\n");
        free(defBuf);
        remove("mibtmp.h");
        fprintf(tblFp, "U32_t mib2DbTid[%u];\n\n", ttlTbl);
    }


    fprintf(tblFp, "\n#endif\n");
    fclose(tblFp);

    fprintf(defFp, "extern U32_t mib2DbTid[];\n\n");
    fprintf(defFp, "#define  MIBID2TBLHDL(id) mib2DbTid[id]\n\n");

    fprintf(defFp, "\n#endif\n");
    fclose(defFp);
    parseTrapType();
    return(0);
}

parseTrapType()
{
    FILE  *tblFp;
    FILE  *defFp;
    FILE  *snmpFp;
    U32_t len;
    U32_t i;
    U8_t  tmpBuf[80];
    U32_t ttlcnt, ttlTbl;
    U8_t  namePtr[80], typePtr[80];
    U8_t  trapTblName[80], trapName[80], trapNum[80];
    U8_t  *lenStr;
    U8_t  *fileBuf, *defBuf, *trapBuf, *typeStr, *trapStr, *indxBuf;
    U8_t  *s1, *s2, *s3, *s4;
    U32_t type;
    U32_t val;
    U32_t mbrNum;
 

    if ((fileBuf = fileGet("snmptype.h", &len, 1)) == NULL)
    {
        fprintf(stdout, "Can not open %s\n", "snmptype.h");
        exit(-11);
    }

    if ((trapBuf = fileGet("mmstrap.c", &len, 0)) == NULL)
    {
        fprintf(stdout, "Can not open %s\n", "mmstrap.c");
        return 0;
    }

    if ((defBuf = fileGet("mibtlen.con", &len, 0)) == NULL)
    {
        fprintf(stdout, "Can not open %s\n", "mibtlen.con");
        exit(-11);
    }

    if ((indxBuf = fileGet("mibindex.tmp", &len, 0)) == NULL)
    {
        fprintf(stdout, "Can not open %s\n", "mibindex.tmp");
        exit(-11);
    }

    if ((tblFp = fopen("traptype.h", "w")) == NULL)
    {
        fprintf(stdout, "Open fail on %s\n",  "traptype.h");
        exit(-1);

    }

    if ((snmpFp = fopen("snmptrap.h", "w")) == NULL)
    {
        fprintf(stdout, "Open fail on %s\n",  "snmptrap.h");
        exit(-1);

    }

    fprintf(tblFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

    fprintf(tblFp, "#ifndef _TRAPTYPE_H_\n");
    fprintf(tblFp, "#define _TRAPTYPE_H_\n\n");

    fprintf(tblFp, "#include \"mms_types.h\"\n\n");
    fprintf(tblFp, "#include \"traprcd.h\"\n\n");

    fprintf(tblFp, "#define NULL_TL  NULL\n\n");
    fprintf(tblFp, defBuf);

    fprintf(snmpFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

    fprintf(snmpFp, "#ifndef _SNMPTRAP_H_\n");
    fprintf(snmpFp, "#define _SNMPTRAP_H_\n\n");

    fprintf(snmpFp, "#include \"sr_proto.h\"\n");
    fprintf(snmpFp, "#include \"userpart.h\"\n\n");


    typeStr = &fileBuf[1];
    ttlTbl = 0;
    trapStr = trapBuf;
    s1 = trapStr;

    while (s1)
    {
        /* find table */
        s1 = strstr(s1, "_TRAPOID[]");
        if (!s1)
	{
            break;
	}
        ttlcnt = 0;

        s3 = strbchr(s1, ' ');
        s3++;
        strncpy(trapTblName, s3, s1-s3);
        trapTblName[s1-s3] = '\0';
if (debug) {
printf("trapTblName = %s\n", trapTblName);
}
        fprintf(tblFp, "static TRAP_DATA_TP_t  %s_TL[] =  {\n", trapTblName);

        fprintf(snmpFp, "typedef\nstruct _%s_t   {\n", trapTblName);

        s2 = strstr(s1, "};");
       
        while (s1 < s2)
	{
            /* find table name */
            s1 = strstr(s1, "OIDvalue(LN");
            if (!s1 || s1 > s2)
                break;
            
	
            s1 += 11;
            s3 = strchr(s1, ',');
            strncpy(trapName, s1, s3-s1);
            trapName[s3-s1] = '\0';
if (debug) {
printf("trapName = %s\n", trapName);
}
            if ( (s3 = strstr(indxBuf, trapName)) )
	    {
if (debug) {
printf("trap %s is key\n", trapName);
}
                s1 = strstr(s1, "NULL,");
                s1 += 8;
                s4 = strbchr(s3, '{');
                val = 0;
                while (s4 && s4 < s3)
		{
                    s4 += 1;
                    s4 = strchr(s4, ',');
                    val++;
		}
                *s1 = val + '0';
	    }
                 
            if (!(s3 = strstr(typeStr, trapName)))
	    {
                fprintf(stdout, "trap %s not defined in mib\n", trapName);
                exit(-1);
	    }
            

            s3 = strbchr(s3, '\n');
            s4 = strbchr(s3, '{');
            mbrNum = 0;

            while (s4 <= s3)
	    {
                mbrNum++;
                s4 = strchr(s4, ';');
                if (!s4)
                    break;
                s4++;
	    }
            mbrNum--;
            

            s3 = getTypenName(s3, namePtr, typePtr);
if (debug) {
printf("name = %s type = %s\n", namePtr, typePtr);
}
            type = fieldType(typePtr);
	    
            if (type == TYPE_STR ||
                type == TYPE_OID)
	    {
	      /* find defined length or use default length */
              if (strstr(defBuf, namePtr))
	      {
                  sprintf(tmpBuf, "%s_TPLEN", namePtr);
                  lenStr = tmpBuf;
	      }
              else if (type == TYPE_STR)
	      {
                  lenStr = "DEFAULT_STRLEN";
	      }
              else
                  lenStr = "DEFAULT_OIDLEN";
	    
 	    }
            else
                  lenStr = "0";
	    
            fprintf(tblFp, "{%15s, %5u, %40s},\n",  typeName[type], mbrNum, lenStr);
            if (lenStr)
                fprintf(snmpFp, "     %15s   %40s;\n",  typePtr, namePtr);
            else
                fprintf(snmpFp, "     %15s   %40s;\n",  typePtr, namePtr);
            ttlcnt++; 
	}
        if (ttlcnt)
	{
            fprintf(tblFp, "};\n\n");
            fprintf(snmpFp, "} %s_t;\n\n", trapTblName);
	}
        s1 = s2;

    }
    free(defBuf);
    free(fileBuf);

    if ((defFp = fopen("mmstrap.c", "w")) == NULL)
    {
        fprintf(stdout, "Open fail on %s\n",  "mmstrap.c");
        exit(-1);

    }
    fprintf(defFp, trapBuf);
    fclose(defFp);

    fprintf(snmpFp, "\n#endif\n");
    fclose(snmpFp);

    s1 = strstr(trapStr, "trapOidTbl[]");
    if (!s1)
    {
        fprintf(tblFp, "\n#endif\n");
        fclose(tblFp);
        return(0);
    }

    free(indxBuf);
    s2 = strchr(s1, '{');
    s3 = strstr(s2, "};");
    while (s1 && s1 < s3)
    {
        s1 = strstr(s1, "TRAPOID,");
        if (s1)
	{
            strncpy(s1, "TL, NULL,", 9);
            
            s1 = strchr(s1+9, ',');
            s1 = strchr(s1+1, ',');
            s1 = strchr(s1+1, ',');
            s4 = strchr(s1, '}');
            while (*s4 != '\n')
	    {
                *s1++ = *s4++;
	    }
            memset(s1, ' ', s4-s1);
	}
    }
    
    s1 = strbchr(s3-10, '0');
    s1++;
    *s1++ = '}';
    memset(s1, ' ', s3-1-s1);

    s1 = strstr(s2, "#endif");
    *s1 = '\0';

    fprintf(tblFp, "\nTRAP_TPLEN_RCD_t trapTpInfo[] =  {\n");

    fprintf(tblFp, s2+2);

    fprintf(tblFp, "U32_t ttlTrapCnt = sizeof(trapTpInfo)/sizeof(TRAP_TPLEN_RCD_t) - 1;\n\n#endif");


    free(trapBuf);

    fclose(tblFp);

    return(0);
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
findTblEnd(U8_t *str)
{
    U8_t  *s1;
    U8_t  *s2;

    while (1)
    {
        s1 = strchr(str, '}');
        if (!s1)
            return(NULL);
        s2 = strchr(str, '{');
        if (!s2)
            return(s1+1);

        if (s2 < s1)
            str = s1 + 1;
        else
            return(s1 + 1);
    }
}


U8_t *
getTypenName(U8_t *str,
             U8_t *name,
             U8_t *type)
{
    U8_t *s1, *s2;

    while (1)
    {
        str = strchr(str, '\n');
        str++;
        str = strcskip(str, ' ');
        if (!strncmp(str, "#ifdef ", 7))
        {
if (debug) {
printf("getTypenName ifdef\n");
}
            str = strstr(str, "#endif");
	}
        else
            break;
    }
    s1 = str;
    str = strchr(str, ' ');
    strncpy(type, s1, str-s1);
    type[str-s1] = '\0';
    str = strcskip(str+1, ' ');
    if (*str == '*')
        str++;
    s2 = str;
    str = strchr(str, ';');
    s1 = str-1;
    str++;
    while (*s1 == ' ')
        s1--;
    s1++;
    strncpy(name, s2, s1-s2);
    name[s1-s2] = '\0';
    return(str);
}


fieldType(U8_t *typeStr)
{
    if (!strcmp(typeStr, "SR_UINT32") ||
        !strcmp(typeStr, "SR_INT32") ||
        !strcmp(typeStr, "long"))
        return(TYPE_DATA_32);
    else if (!strcmp(typeStr, "OctetString"))
        return(TYPE_STR);
    else if (!strcmp(typeStr, "OID"))
        return(TYPE_OID);

    return(TYPE_DATA_32);
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
        if ((fileBuf = malloc(BUFFER_SIZE)) == NULL)
        {
            fprintf(stdout, "Fail to get alloc mem for file %s!\n", fName);
            exit(-1);
        }
        *len = fread(&fileBuf[offset], 1, BUFFER_SIZE, fp);
        if (*len < 10)
	{
            free(fileBuf);
            fileBuf = NULL;
        }
        else
	{
            fileBuf[*len] = '\0';
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
