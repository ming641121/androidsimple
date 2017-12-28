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


#include <ctype.h>

#include <errno.h>


#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>


#include <string.h>


#include <netinet/in.h>


#include "sr_snmp.h"
#include "diag.h"
#include "sr_trans.h"
#include "prnt_lib.h"
SR_FILENAME

#include "pclo.h"




#ifdef SR_IP
SR_INT32 SrInitializeTraps(void);
extern TransportInfo ipTrapTI;
#endif /* SR_IP */

char *snmpPortBindAddr = NULL;



#ifdef SR_DEBUG
struct {
    char *name;
    SR_INT32 flag;
} LogFlags[] = {
    { "-apnone", 0 },
    { "-apwarn", APWARN },
    { "-aperror", APERROR },
    { "-aptrace", APTRACE },
    { "-apall", APTIMER|APTHREAD|APWARN|APTRACE|APERROR|APCONFIG|APPACKET|APTRAP|APACCESS|APVERBOSE },
    { "-aptimer", APTIMER },
    { "-apthread", APTHREAD },
    { "-apconfig", APCONFIG },
    { "-appacket", APPACKET },
    { "-aptrap", APTRAP },
    { "-apaccess", APACCESS },
    { "-apverbose", APVERBOSE },
    { "-apuser", APUSER },
    { "-apaudit", APAUDIT },
};

#define NLogFlags (sizeof(LogFlags)/sizeof(*LogFlags))
#endif /* SR_DEBUG */


void
usage(char *progname)
{
    (void) fprintf(stderr, "usage: %s [options]\n", progname);
    (void) fprintf(stderr, "\n");
    (void) fprintf(stderr, "options:\n");
    (void) fprintf(stderr, "\t-d                execute in the foreground window\n");
#ifdef SR_IP
    (void) fprintf(stderr, "\t-trap_send_port PORT   send SNMP trap/inform messages from port PORT\n");
#endif /* SR_IP */
#ifdef SR_DEBUG
    (void) fprintf(stderr, "\t-apnone           no log messages\n");
    (void) fprintf(stderr, "\t-apwarn           log warning messages\n");
    (void) fprintf(stderr, "\t-aperror          log error messages\n");
    (void) fprintf(stderr, "\t-apconfig         log config file i/o messages\n");
    (void) fprintf(stderr, "\t-appacket         log SNMP packet build/parse messages\n");
    (void) fprintf(stderr, "\t-aptrap           log trap/inform messages\n");
    (void) fprintf(stderr, "\t-apaccess         log agent processing messages\n");
    (void) fprintf(stderr, "\t-aptimer          log timer debug messages\n");
    (void) fprintf(stderr, "\t-apthread         log thread debug messages\n");
    (void) fprintf(stderr, "\t-apverbose        log verbose debug messages\n");
    (void) fprintf(stderr, "\t-apuser           log user messages\n");
    (void) fprintf(stderr, "\t-hexdump          dump packets in hex\n");
    (void) fprintf(stderr, "\t-vbdump           dump packets as varbinds\n");
    (void) fprintf(stderr, "\t-aptrace          trace packet in application\n");
    (void) fprintf(stderr, "\t-apaudit          audit SET processing in application\n");
    (void) fprintf(stderr, "\t-apall            log all messages\n");
    (void) fprintf(stderr, "\t-log_format 0     use traditional log message format\n");
    (void) fprintf(stderr, "\t-log_format 1     use new log message format\n");
    (void) fprintf(stderr, "\t-log_stdout       allow log messages to go to standard output\n");
    (void) fprintf(stderr, "\t-log_nostdout     disallow log messages to go to standard output\n");
    (void) fprintf(stderr, "\t-log_stderr       allow log messages to go to standard error\n");
    (void) fprintf(stderr, "\t-log_nostderr     disallow log messages to go to standard error\n");
    (void) fprintf(stderr, "\t-log_file         allow log messages to go to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_nofile       disallow log messages to go to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_tracefile    allow APTRACE messages to go to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_notracefile  disallow APTRACE messages to go to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_append       append log messages to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_noappend     discard previous contents of snmpd.log, if any\n");
#endif				/* SR_DEBUG */
    (void) fprintf(stderr, "\t-large_v1_pdus    "
                           "lift 484 byte restriction for v1/v2c PDUs\n");
    (void) fprintf(stderr, "\t-help             print this usage line\n");
    (void) fprintf(stderr, "\t-pkt_size value   use supplied value as max packet size\n"); 
    (void) fprintf(stderr, "\t-snmp_bindaddr x.x.x.x   bind to SNMP port on addr x.x.x.x\n");

}

int
kw_match(char *s, char *pat)
{

    int optional = 0;   /* Are we scanning optional part of the pattern */

    while ( 1 ) {
        while ( *s && *pat != '[' && *pat == *s ) {
            s++;
            pat++;
        }

        if ( *pat == '[' ) {
            /*
             * The remainder of the pattern is optional
             */
            optional = 1;
            pat++;
        }

        if ( *s == '\0' ) {
            /*
             * Done with keyword string, decide if we match
             */
            return  ( *pat == '\0' || optional );

        } else if ( *pat == '\0' ) {
            /*
             * End of pattern, but not keyword string
             */
            return 0;
        } else if (!( optional && *pat == *s ) ) {
            /*
             * Not the end of either and still no match
             */
            return 0;
        }
    }
}

int
process_command_line_option(
    int *argc,
    char **argv,
    int *daemonize,
#ifdef SR_DEBUG
    LogAppData *logAppData,
#endif /* SR_DEBUG */
    int *s_port
    )
{
    int num_to_eat;
#ifdef SR_DEBUG
    int i;
#endif /* SR_DEBUG */    
#ifdef SR_IP
    static int trap_send_port = 0;
#ifdef SR_DEBUG
    char tstr[32];
#endif /* SR_DEBUG */
#endif /* SR_IP */

    if (*argc < 2) {
        return 0;
    }
    if (argv[1] == 0) { /* VxWorks convention, last arg = 0 */
        return 0;
    }

#ifdef SR_DEBUG

    for (i = 0; i < NLogFlags; i++) {
        if (kw_match(argv[1], LogFlags[i].name)) {
            if (LogFlags[i].flag == 0) {
                SetLogLevel(LogFlags[i].flag);
            }
            else {
                SetLogLevel(GetLogLevel() | LogFlags[i].flag);
            }
            num_to_eat = 1;
            goto done;
        }
    }

    if (kw_match(argv[1], "-hex[dump")) {
	packdump(NULL, NULL, 0, NULL, NULL, 1);
        num_to_eat = 1;
	goto done;
    }

    if (kw_match(argv[1], "-vbdum[p")) {
        packdump(NULL, NULL, 0, NULL, NULL, 2);
        num_to_eat = 1;
        goto done;
    }



    if (kw_match(argv[1], "-log_fo[rmat")) {
	if (!isdigit((unsigned char)*argv[2])) {
	    usage(argv[0]);
	    exit(-1);
	}
	logAppData->format = (SR_INT32)(*argv[2] - '0');
	if (strlen(argv[2]) > 1 || logAppData->format > SRLOG_MAX_FORMAT) {
	    fprintf(stderr, "%s: unrecognized log format value\n", argv[2]);
	    exit(-1);
	}
	num_to_eat = 2;
	goto done;
    }
    if (kw_match(argv[1], "-log_stde[rr")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_STDERR;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_nostde[rr")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_STDERR;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_fi[le")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_FILE;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_nof[ile")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_FILE;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_t[racefile")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_TRACEFILE;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_not[racefile")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_TRACEFILE;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_a[ppend")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_APPEND;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_noa[ppend")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_APPEND;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_stdo[ut")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_STDOUT;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_nostdo[ut")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_STDOUT;
        }
        num_to_eat = 1;
        goto done;
    }
#endif /* SR_DEBUG */


    if (kw_match(argv[1], "-pkt[_size"))  {
        if ( (*argc < 3) || (!isdigit((unsigned char)*argv[2])) ) {
           usage(argv[0]);
           exit(-1);
          }
        
        sr_pkt_size = atoi(argv[2]);
        if (sr_pkt_size < 484)  {
            fprintf(stderr, "max packet size minimum value is 484 bytes. \n");
            fprintf(stderr, "setting max packet size to 484. \n");
           sr_pkt_size = 484;
        }
        if (sr_pkt_size > 2147483647)  {
            fprintf(stderr, "max packet size maximum value is 2147483647 bytes. \n");
            fprintf(stderr, "setting max packet size to 2147483647. \n"); 
           sr_pkt_size = 2147483647;
        }
        num_to_eat = 2;
        goto done;
     }

     if (kw_match(argv[1], "-snmp_bindaddr")) {
        if (*argc < 3) {
           usage(argv[0]);
           exit(-1);
        }

        snmpPortBindAddr = strdup(argv[2]);
        num_to_eat = 2;
        goto done;
     }


    if (kw_match(argv[1], "-d[aemonize")
        ) {
        if (daemonize != NULL) {
            *daemonize = 0;
        }
#ifdef SR_DEBUG
        logAppData->operFlags |= SRLOG_STDERR;
#endif /* SR_DEBUG */
        num_to_eat = 1;
        goto done;
    }

#ifdef SR_IP
#define BINDTRIES 5
    if (kw_match(argv[1], "-trap[_send_port")) {
        if (trap_send_port > 0) {
            DPRINTF((APERROR, "Cannot use -trap_send_port argument twice\n"));
            exit(1);
        } else {
            trap_send_port = atoi(argv[2]);
	    if (trap_send_port <= 0 || trap_send_port >= 65536) {
                DPRINTF((APERROR, "The trap send port can not be set to %d\n",
                         trap_send_port));
                exit(1);
            } else if (trap_send_port == GetSNMPPort()) {
                DPRINTF((APERROR, "Cannot send traps from the same port "
                         "as requests are received\n"));
                exit(1);
            } else {
                if (SrInitializeTraps()) {
                    exit(-1);
                }
                ipTrapTI.t_ipPort = htons((short) trap_send_port);
                if (BindTransport(&ipTrapTI, BINDTRIES)) {
                    DPRINTF((APERROR, "Cannot bind TRAP transport %s\n%s",
                        FormatTransportString(tstr, sizeof(tstr), &ipTrapTI),
                        (trap_send_port == 162) ?
                            "Is an SNMP manager running?\n" : ""));
                    exit(1);
                } else {
                    DPRINTF((APTRACE|APTRAP, "Traps will be sent from %s\n",
                        FormatTransportString(tstr, sizeof(tstr), &ipTrapTI)));
                }
            }
        }
        num_to_eat = 2;
        goto done;
    }

#endif /* SR_IP */

    if (kw_match(argv[1], "-p[ort")) {
        if (*argc < 3) {
            usage(argv[0]);
            exit(-1);
        }

        if (!isdigit((unsigned char)*argv[2])) {
            usage(argv[0]);
            exit(-1);
        }

        DPRINTF((APWARN, "%s is no longer supported, use SR_SNMP_TEST_PORT\n", argv[1]));
        num_to_eat = 2;
        goto done;
    }

    if (kw_match(argv[1], "-h[elp")) {
        usage(argv[0]);
        exit(0);
    }

    if (kw_match(argv[1], "-large_v1[_pdus")) {
	extern int use_large_v1_pdu;  /* see snmpd/shared/agtctxv[12].c */
	use_large_v1_pdu = 1;
        num_to_eat = 1;
        goto done;
    }


    return 0;

  done:
    memcpy(argv + 1, argv + num_to_eat + 1,
           sizeof(char *) * ((*argc) - num_to_eat));
    (*argc) -= num_to_eat;
    return 1;
}

void
packdump(
    char *message,
    unsigned char *packet,
    SR_UINT32 length,
    TransportInfo *ti,
    void *temp,
    int flag)
{
#ifdef SR_DEBUG
    SR_UINT32 i, pos;
    char *buffer;
    char buf[32];
    Pdu *pdu_ptr = NULL;
    VarBind *var_bind;
    SnmpMessage *snmp_msg;
    static unsigned int option = 0x00000000;

    if ((flag == 0) && (option != 0x00000000)){
        buffer = (char *)malloc(256);
        if (buffer == NULL) {
            return;
        }
        buffer[0]='\0';

        if (ti == NULL) {
            strcpy(buf, "<unknown>");
        } else {
            if (FormatTransportString(buf, 32, ti) == NULL) {
                strcpy(buf, "<unknown>");
            }
        }
        if (message != NULL) {
            printf("\n%s, addr = %s, length = %lu:\n", message, buf,
                    (unsigned long)length);
        }

        if (option & HEXDUMP){
            strcpy(buffer, "  ");
            for (i = 0, pos = 1; i < length; i++) {
                sprintf(buf, "%2.2x ", (unsigned int)packet[i]);
                strcat(buffer, buf);
                if ((pos % 16) == 0) {
                    pos = 1;
                    printf("%s\n", buffer);
                    strcpy(buffer, "  ");
                }
                else {
                    pos++;
                }
            }
            if (pos > 1) {
                printf("%s\n", buffer);
            }
        }

        free(buffer);

        if (option & VBDUMP){
            if (!strncmp(message, "Incoming Packet", 15)) {
                snmp_msg = (SnmpMessage *)temp;
                if (snmp_msg->packlet == NULL) {
                   return;
                }
                if ((pdu_ptr = SrParsePdu(snmp_msg->packlet->octet_ptr, 
                    snmp_msg->packlet->length)) != NULL) {
	            PrintVarBindList(pdu_ptr->var_bind_list);
	         }
             }
             else {
                 var_bind = (VarBind *)temp;
	         PrintVarBindList(var_bind);
	     }
         }
    }
    else{
	if (flag == 1)
	    option = option | HEXDUMP;
        if (flag == 2)
	    option = option | VBDUMP;
    }

    if (pdu_ptr != NULL) {
        FreePdu(pdu_ptr);
        pdu_ptr=NULL;
    }
#endif /* SR_DEBUG */
}

