/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/*
 * This file contains routines which perform I/O to the world outside the
 * system.  For most platforms, that implies file I/O.
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>


#include <string.h>

#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME

#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"

#include <types.h>

#include <errno.h>



#include <errno.h>

char *
sr_fgets(buf, size, cfp)
char *buf;
int size;
ConfigFile *cfp;
{

/* Want to make sure we can read files that are in DOS format or in UNIX */
/* format. In DOS files lines end with CR/LF (ASCII 13, ASCII 10).       */
/* By checking for ASCII 13 (CR) at the next-to-last character we know   */
/* if it's a DOS file. If so, we overwrite the CR with newline (LF), and */
/* shorten the string up by one. This allows any agent to read any type  */
/* of file, which is a huge help for embedded systems like VxWorks that  */
/* might be compiled on DOS or on UNIX. The end result is that any file  */
/* like snmpd.cnf that is written out by the agent will be in UNIX       */
/* format.                                                               */

    int sz;

    if (fgets(buf, size, cfp->fp) == NULL) { /* Return NULL on EOF */
       return NULL;
    }
    sz = strlen(buf);
    if (sz < 2) { /* If string is < 2 must be UNIX format, so return */
       return buf;
    }
    if (buf[sz - 2] == 13) { 
       /* Check to see if next to last character in line is '\r' (ASCII 13) */
       /* If so left shift the '\n' over the '\r' and '\0' over the '\n' */
       /* Effectively removes the ^M from the line */

       buf[sz - 2] = '\n';
       buf[sz - 1] = '\0';
    }
    return buf;
}

int
sr_feof(cfp)
ConfigFile *cfp;
{
    return feof(cfp->fp);
}

int
sr_fprintf(cfp, buf)
ConfigFile *cfp;
const char *buf;
{
    return fputs(buf, cfp->fp);
}

void
write_config_log(logfilename, logmessage)
    const char *logfilename;
    const char *logmessage;
{
    FILE *fp;
    static int firstTry = 1;
    FNAME("write_config_log")

    if (logfilename == NULL) {
        return;
    }

    if (logmessage == NULL) {
        return;
    }

    fp = fopen(logfilename, "a");
    if (fp == NULL) {
        if (firstTry) {
            DPRINTF((APERROR, "%s: Can't open config log file %s: %s\n",
                Fname, logfilename, sys_errname(errno)));
            DPRINTF((APERROR, "%s: Writing config log output to console\n", 
                Fname));
            firstTry = 0;
        }
        DPRINTF((APERROR, (char *)logmessage));
        return;
    }
    fprintf(fp, logmessage);
    fclose(fp);
    return;
}

/* 
 * This routine opens a config file and keeps information about it.
 */
int
OpenConfigFile(filename, mode, cfgp)
const char *filename;
int mode;
ConfigFile **cfgp;
{
    int retval =  FS_OK;
    ConfigFile *tmpcfgp = NULL;
    char *modestr = NULL;
    FNAME("OpenConfigFile")

    
    if((tmpcfgp = (ConfigFile *) malloc(sizeof(ConfigFile))) == NULL) {
	DPRINTF((APWARN,
                  "%s: malloc failure - couldn't get ConfigFile struct\n",
                  Fname));
	retval = FS_ERROR;
    } else {
	/* save the filename and init the line count */
	strcpy(tmpcfgp->filename, filename);
	tmpcfgp->linecount = 0;

	/* determine the open mode */
	if(mode == FM_WRITE) {
	    modestr = "w";
	} else {
	    modestr = "r";
	}
	    
	/* open the file */
	if((tmpcfgp->fp = fopen(tmpcfgp->filename, modestr)) == NULL) {
            DPRINTF((APWARN, "%s: Can't open %s for %s: %s\n",
                Fname, tmpcfgp->filename,
                (mode == FM_WRITE) ? "writing" : "reading",
                sys_errname(errno)));
	    retval = FS_ERROR;
	}

    }
    /* If we can't open the file, clean up */
    if (retval == FS_ERROR) {
        CloseConfigFile(tmpcfgp);
        tmpcfgp = NULL;
    }

    /* pass back the pointer to the config structure */
    *cfgp   = tmpcfgp;
    tmpcfgp = NULL;
    
    return(retval);
}

/*
 * This routine closes configuration files and frees their info structs.
 */
void
CloseConfigFile(cfgp)
ConfigFile *cfgp;
{
    if(cfgp) {
	if(cfgp->fp) fclose(cfgp->fp);
	free((char *) cfgp);
        cfgp = NULL;
    }
    return;
}
