#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef unsigned char  U8_t;
typedef unsigned short U16_t;
typedef unsigned long  U32_t;


FILE *tblFp = stdout;
U8_t *fileName = "snmpoid.c";
U8_t  *fileBuf;
U8_t *undefName = "undefs.h";
U8_t  *undBuf;


main()
{
    FILE  *fp;
    FILE  *undFp;

    U32_t size, len;
    U8_t  *str, *endPtr;
    U8_t  nameBuf[200];
    U32_t curIndex = 0;

    if ((fp = fopen(fileName, "r")) == NULL)
    {
        fprintf(stdout, "Can not open %s!\n", fileName);
        exit(1);
    }

    if (fseek(fp, 0, SEEK_END))
    {
        fprintf(stdout, "Fail to get file size %s!\n", fileName);
        exit(1);
    }
    size = ftell(fp);

    if ((fileBuf = malloc(size + 2)) == NULL)
    {
        fprintf(stdout, "Fail to get alloc mem for file %s!\n", fileName);
        exit(1);
    }
    if (fseek(fp, 0, SEEK_SET))
    {
        fprintf(stdout, "Fail to move to beginning of file %s!\n", fileName);
        exit(1);
    }

    len = fread(&fileBuf[1], 1, size, fp);

    fclose(fp);
    fileBuf[0] = '\0';
    fileBuf[len] = '\0';

    if ((undFp = fopen(undefName, "r")) == NULL)
    {
        fprintf(stdout, "Can not open %s!\n", fileName);
        exit(1);
    }

    if (fseek(undFp, 0, SEEK_END))
    {
        fprintf(stdout, "Fail to get file size %s!\n", fileName);
        exit(1);
    }
    size = ftell(undFp);

    if ((undBuf = malloc(size + 2)) == NULL)
    {
        fprintf(stdout, "Fail to get alloc mem for file %s!\n", fileName);
        exit(1);
    }
    if (fseek(undFp, 0, SEEK_SET))
    {
        fprintf(stdout, "Fail to move to beginning of file %s!\n", fileName);
        exit(1);
    }

    len = fread(undBuf, 1, size, undFp);

    fclose(undFp);
    undBuf[len] = '\0';

    if ((tblFp = fopen("cliobjdef.h", "w")) == NULL)
    {
        fprintf(stdout, "Open fail on cliobjdef.h\n");
        exit(1);

    }
    fprintf(tblFp, "\
/*
 *
 * Copyright (C) 2006 by Ahoya Networks.
 *
 */\n\n");

    fprintf(tblFp, "#ifndef _CLIOBJDEF_H_\n");
    fprintf(tblFp, "#define _CLIOBJDEF_H_\n\n");
    /*
    fprintf(tblFp, "#define OBJINDEX    -1\n\n");
    */
   
    str = &fileBuf[1];
    strcpy(nameBuf, "OI");
    while (1)
    {
         if ((str = strstr(str, "#if defined(I_"))
            == NULL)
            break;
        
        str += 13;   /* skip "#if defined(I" */
        endPtr = strchr(str, ')');
        strncpy(&nameBuf[2], str, endPtr - str);
        nameBuf[2+endPtr - str] = '\0';

        if (!strstr(undBuf, &nameBuf[1]))
	{
	/*
        fprintf(tblFp, "#if defined(%s)\n", &nameBuf[1]);
        fprintf(tblFp, "#define %-43s (OBJINDEX+1)\n",
            nameBuf);
        fprintf(tblFp, "#undef  OBJINDEX\n");
        fprintf(tblFp, "#define OBJINDEX    %s\n",
            nameBuf);
        fprintf(tblFp, "#endif\n\n");

*/

            fprintf(tblFp, "#define %-43s %3u\n",
                nameBuf, curIndex);
            curIndex++;
	}
        str = endPtr;
    }

    fprintf(tblFp, "#endif /* _CLIOBJDEF_H_ */\n");

    fclose(tblFp);
}


