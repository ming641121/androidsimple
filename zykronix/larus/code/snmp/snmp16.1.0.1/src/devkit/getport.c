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


#include "sr_conf.h"
#include <stdio.h>

#include <stdlib.h>

#include <stddef.h>


#include <sys/types.h>




#include <types.h>

#include <types/vxTypes.h>

#include <netinet/in.h>

#include <string.h>


#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME



#define SR_SNMP_ENV_NAME "SR_SNMP_TEST_PORT"
#define SR_TRAP_ENV_NAME "SR_TRAP_TEST_PORT"


#define SNMP_PORT 161
#define SNMP_TRAP_PORT 162


/*
 * GetSNMPPort()
 *
 * If the environment variable SR_SNMP_ENV_NAME is set, then use that
 * as the port to send to when sending SNMP requests. Otherwise, if the
 * function getservbyname(SR_SNMP_SVC_NAME, "udp") returns a valid port (from
 * /etc/services or equivalent database), use that.  Otherwise use the
 * #define SNMP_PORT if it exists, otherwise, use 161.
 *
 * Returns the port in HOST byte order.
 */

int
GetSNMPPort()
{
    FNAME("GetSNMPPort")
    static int      snmp_port;
    static int      called = 0;
    char           *temp;

    if (!called) {
        called = 1;
	if ((temp = (char *)getenv(SR_SNMP_ENV_NAME)) != NULL) {
	    snmp_port = atoi(temp);
	    DPRINTF((APTRACE, "%s: snmp port is %d from getenv(\"%s\")\n",
		     Fname, snmp_port, SR_SNMP_ENV_NAME));
	}
        else {

#ifdef SNMP_PORT
	    snmp_port = SNMP_PORT;
	    DPRINTF((APTRACE, "snmp port is %d from #define SNMP_PORT\n",
		     snmp_port));
#else
	    snmp_port = 161;
	    DPRINTF((APWARN, "snmp port is %d from constant\n", snmp_port));
#endif

	}
    }

    return snmp_port;
}


/*
 * GetSNMPTrapPort()
 *
 * If the environment variable SR_TRAP_ENV_NAME is set, then use that
 * as the port to send to when sending SNMP traps. Else, if the environment
 * variable SR_SNMP_ENV_NAME exits, use it's value + 1. Otherwise, if the
 * function getservbyname("snmp-trap", "udp") returns a valid port (from
 * /etc/services or equivalent database), use that.  Otherwise if the
 * #define SNMP_TRAP_PORT exits, use it's value, otherwise use 161.
 *
 * Returns the port in HOST byte order.
 */

int
GetSNMPTrapPort()
{
    FNAME("GetSNMPTrapPort")
    static int      trap_port;
    static int      called = 0;
    char           *temp;

    if (!called) {
        called = 1;
	if ((temp = (char *)getenv(SR_TRAP_ENV_NAME)) != NULL) {
	    trap_port = atoi(temp);
	    DPRINTF((APTRAP, "%s: trap port is %d from getenv(\"%s\")\n",
		     Fname, trap_port, SR_TRAP_ENV_NAME));
	}
	else if ((temp = (char *)getenv(SR_SNMP_ENV_NAME)) != NULL) {
	    trap_port = atoi(temp) + 1;
	    DPRINTF((APTRAP, "%s: trap port is %d from getenv(\"%s\") + 1\n",
		     Fname, trap_port, SR_SNMP_ENV_NAME));
	}
        else {

#ifdef SNMP_TRAP_PORT
	    trap_port = SNMP_TRAP_PORT;
	    DPRINTF((APTRAP, "trap port is %d from #define SNMP_TRAP_PORT\n",
		     trap_port));
#else				/* SNMP_TRAP_PORT */
	    trap_port = 162;
	    DPRINTF((APWARN, "trap port is %d from constant\n", trap_port));
#endif				/* SNMP_TRAP_PORT */

	}
    }

    return trap_port;
}

