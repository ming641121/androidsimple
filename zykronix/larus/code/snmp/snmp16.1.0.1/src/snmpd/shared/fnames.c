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

#ifdef SR_CONFIG_FP
#include "sr_conf.h"

#include <stdio.h>



#include <stdlib.h>

#include <string.h>

#include "sr_proto.h"
#include "sr_type.h"
#include "fnames.h"
#include "diag.h"
SR_FILENAME

char _fn_snmpd_file[] = "snmpd.cnf";
char *fn_snmpd_file = NULL;
char _fn_snmpd_junk_file[] = "snmpd.jnk";
char *fn_snmpd_junk_file = NULL;

void
init_fnames()
{
    char           *prefix = NULL;
    char           *source = NULL;
#ifdef SR_DEBUG
    const char     *Fname = "init_fnames";
#endif /* SR_DEBUG */
    int            namelen;



    if ((prefix = getenv("SR_AGT_CONF_DIR")) == NULL) {
	prefix = SR_AGT_CONF_FILE_PREFIX;
        source = "(default)";
    }
    else {
        source = "from getenv(\"SR_AGT_CONF_DIR\")";
    }
    if (prefix[strlen(prefix) - 1] == '/') {
	prefix[strlen(prefix) - 1] = '\0';
    }

    DPRINTF((APCONFIG, "%s: searching for configuration files in %s %s\n",
             Fname, prefix, source));

/*
 *  Malloc the file name buffers to prevent overflow on long
 *  environment string.
 */
    namelen = strlen(_fn_snmpd_file) + strlen(prefix) + 2;
    fn_snmpd_file = (char *) malloc((size_t) namelen);
    namelen = strlen(_fn_snmpd_junk_file) + strlen(prefix) + 2;
    fn_snmpd_junk_file = (char *) malloc((size_t) namelen);
    if (fn_snmpd_file == NULL || fn_snmpd_junk_file == NULL) {
	DPRINTF((APWARN, 
	   "%s: Failed to malloc space for configuration file name\n", Fname));
        return;
    }
    sprintf(fn_snmpd_file, "%s/%s", prefix, _fn_snmpd_file);
    sprintf(fn_snmpd_junk_file, "%s/%s", prefix, _fn_snmpd_junk_file);



}
#endif /* SR_CONFIG_FP */
