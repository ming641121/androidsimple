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

#include <signal.h>


#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>



#include <socket.h>

#include <sys/socket.h>

#include <sys/stat.h>


#include <fcntl.h>

#include <netinet/in.h>




#include <string.h>




#include <timers.h>
#include <net/inet.h>
#include <inetLib.h>
#include <taskLib.h>
#include <taskHookLib.h>





#include "sr_time.h"

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "sr_fdset.h"
#include "compat.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "mapctx.h"
#include "method.h"
#include "mibout.h"
#include "snmpd.h"
#include "mgmt.h"
#include "tmq.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "trap.h"
#include "sr_ntfy.h"
#include "snmpv2d.h"
#include "agtinit.h"
#include "fnames.h"
#include "geninit.h"
#if (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE))
#include "notify.h"
#endif	/* (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE)) */
#include "agtctx.h"


#include "diag.h"
SR_FILENAME

#include "sitedefs.h"




#include "mib_tabl.h"
#include "snmp-mib.h"


#ifdef SR_CONFIG_FP
#ifndef SR_USE_HARD_IO
const PARSER_RECORD_TYPE *snmpd_rt[66];
#else /* SR_USE_HARD_IO */
const PARSER_RECORD_TYPE *snmpd_rt[64];
#endif /* SR_USE_HARD_IO */
int snmpd_rt_count;
#endif /* SR_CONFIG_FP */

#ifndef SR_USE_HARD_IO
static const SnmpType SNMP_ENGINE_ID_Type[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
static const SnmpType SNMP_ENGINE_ID_SRC_Type[] = {
    { INTEGER_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#endif /* SR_USE_HARD_IO */

#ifdef SR_CONFIG_FP
#ifndef SR_USE_HARD_IO
#define DEFAULT_SNMP_ENGINE_ID_SRC 1
ENGINE_ID engine_id;

static const PARSER_CONVERTER SNMP_ENGINE_ID_Converters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};

static const PARSER_CONVERTER SNMP_ENGINE_ID_SRC_Converters[] = {
    { ConvToken_integer, NULL },
    { NULL, NULL }
};

static const PARSER_RECORD_TYPE SNMP_ENGINE_ID_Record = {
    PARSER_SCALAR, "SNMP_ENGINE_ID ",
    SNMP_ENGINE_ID_Converters, SNMP_ENGINE_ID_Type,
    NULL, &engine_id.runtime_SNMP_ENGINE_ID, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , "#HELLO SNMP_ENGINE_ID ENTRY"
#endif  /* SR_NO_COMMENTS_IN_CONFIG */
};

static const PARSER_RECORD_TYPE SNMP_ENGINE_ID_SRC_Record = {
    PARSER_SCALAR, "SNMP_ENGINE_ID_SRC ",
    SNMP_ENGINE_ID_SRC_Converters, SNMP_ENGINE_ID_SRC_Type,
    NULL, &engine_id.runtime_SNMP_ENGINE_ID_SRC, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , "#HELLO SNMP_ENGINE_ID_SRC ENTRY"
#endif  /* SR_NO_COMMENTS_IN_CONFIG */
};

#endif /* SR_USE_HARD_IO */
#endif /* SR_CONFIG_FP */



void usage(char *progname);


int is_broadcast
    SR_PROTOTYPE((SR_UINT32 ipSrc));

int PostTimerHook
    SR_PROTOTYPE((TimerQueue *TmQ,
                  SR_UINT32 id,
                  void *info,
                  SR_INT32 invoke));

int
is_broadcast(ipSrc)
    SR_UINT32   ipSrc;
{
    /* %%% needs further work */
    if (((ipSrc & 0xff) == 0xff) || ((ipSrc & 0xff) == 0)) {
        return 1;
    }
    return 0;
}

int CancelTimerHook
    SR_PROTOTYPE((TimerQueue *TmQ,
                  SR_UINT32 id,
                  int anymore ));

extern char *snmpPortBindAddr;

#define PROXY_TRAP_BIND_ARG

#ifdef SR_DEBUG
LogBook *LogBk = NULL;
LogAppData logAppData = { SRLOG_FILE };
#define LOG_APP_DATA_ARG LogAppData *logAppData,
#else /* SR_DEBUG */
#define LOG_APP_DATA_ARG
#endif /* SR_DEBUG */

int process_command_line_option
    SR_PROTOTYPE((int *argc,
                  char **argv,
                  int *daemonize,
                  PROXY_TRAP_BIND_ARG
                  LOG_APP_DATA_ARG
                  int *s_port));

/* globals */

void
close_sockets(WIND_TCB *tcb);

SEM_ID semM;
char global_buf_inet_ntoa_b[INET_ADDR_LEN];



#ifdef SR_DEBUG
void MultiFunctionLogMessage
    SR_PROTOTYPE((LogEntry *log));
#endif /* SR_DEBUG */

/* POLL_INTERFACES and POLL_INTERVAL set in include/sr_conf.h */
#ifdef POLL_INTERFACES
void poll_interfaces(void);
time_t time(time_t *);
#endif				/* POLL_INTERFACES */

TransportInfo   snmpdTI;


TimerQueue *TimerQ;
extern int timerq_update;

time_t          time(time_t *);


RETSIGTYPE
quit(void)
{
    exit(1);
}

#ifdef TIMER_EXAMPLE
void 
test_time(SR_UINT32 TimeOutType, int TimerId, 
          SR_INT32 userData1, void *userData2)
{
    printf("test_timer\n");
    printf("TimeOutType: %u, TimerId: %d, userData1: %d, userData2: %s\n",
	     TimeOutType, TimerId, userData1, (char *)userData2);
}

void
hi(TimeOutDescriptor *none)
{
    printf("hi there!\n");
}
#endif /* TIMER_EXAMPLE */


#ifdef SR_DEBUG
int stderr_closed = 0;
int logfile_open = 0;
#endif /* SR_DEBUG */


int writeConfigFileFlag = FALSE;

SnmpLcd agt_snmp_lcd;


/*
 * For VxWorks the entry point will be unique so as not to confuse snmpd
 * with other "main" routines.  The VxWorks shell does no support argc/argv
 * All functions are called with up to ten arguments.  Unused arguments are
 * passed in with zero (0) value.
 *
 */
int
snmpd_main(arg1_in, arg2_in, arg3_in, arg4_in, arg5_in,
	   arg6_in, arg7_in, arg8_in, arg9_in, arg10_in)
    char           *arg1_in, *arg2_in, *arg3_in, *arg4_in, *arg5_in;
    char           *arg6_in, *arg7_in, *arg8_in, *arg9_in, *arg10_in;
{
    int             argc;
    char           *argv[11];
    unsigned char   *in_packet, *out_packet;
    TransportInfo   *SrcTransportInfo = NULL, DstTransportInfo;
#ifdef SR_DEBUG
    char tstr[32];
#endif /* SR_DEBUG */


#ifdef USE_NETWORK_SERVICES
    struct servent *SimpleServ = NULL;
#endif				/* USE_NETWORK_SERVICES */
    int             in_packet_len, out_packet_len;
    int             s_port;
    int             s_trap_port;
#ifdef SR_SNMPv1_ADMIN
    OID *oid_temp;
#endif /* SR_SNMPv1_ADMIN */

    int             daemonize = 0; 	/* don't daemonize by default */
    struct timeval  timeout_data, *timeout;
    SR_UINT32       curtime, howlong, bind_addr;
    fd_set          readfds;
    int             cc;
    int             maxfd;
#ifdef SR_DEBUG
#ifndef SR_USE_HARD_IO
    SR_INT32        tmpLogLevel;
    int             fd_tmp;
    char           *fullpath = NULL;
#endif /* SR_USE_HARD_IO */
#endif /* SR_DEBUG */

    /* Copies of argc and argv which can be modified 
       locally without confusing ps */
    int argcount;
    char **argvec;  

    /* timer checking related variables */
    int wrap = 0;
    TimerEvent *p = NULL;

#ifdef POLL_INTERFACES
    time_t polltime = time((time_t *) NULL);
#endif				/* POLL_INTERFACES */



    timeout = &timeout_data;
    timeout->tv_usec = 0;
    timeout->tv_sec = POLL_INTERVAL;
    FD_ZERO(&readfds);

#ifdef SR_DEBUG
/*
 * OpenLog() must be called before ANY function which invokes
 * DPRINTF or LOG!!
 */
    if((LogBk = OpenLog()) == NULL) {
	printf("%s: OpenLog() returned NULL\n", argv[0]);
	exit(1);
    }
#endif /* SR_DEBUG */

    SetSigFunction(SIGINT, (SigFunc) quit);

    /*
     * For VxWorks, "fake out" the argv/argc mechanism so that the bulk of
     * the standard snmpd command line parsing will remain intact.
     *
     */
    argc = 11;
    argv[0] = "snmpd"; /* %%% DLB should be actual image name? */
    argv[1] = arg1_in;		/* Brute force! */
    argv[2] = arg2_in;
    argv[3] = arg3_in;
    argv[4] = arg4_in;
    argv[5] = arg5_in;
    argv[6] = arg6_in;
    argv[7] = arg7_in;
    argv[8] = arg8_in;
    argv[9] = arg9_in;
    argv[10] = arg10_in;

#ifdef SR_DEBUG
    SetLogLevel(APERROR | APWARN);
    SetProgname(argv[0]);
    SetLogFunction(MultiFunctionLogMessage);
    SetLogUserData((void *) &logAppData);
#endif /* SR_DEBUG */

    argvec = malloc((argc+1)*SIZEOF_CHAR_P);
    if (!argvec) {
	DPRINTF((APERROR, "Out of memory.\n"));
	exit(1);
    };
    memcpy(argvec, argv, (argc+1)*SIZEOF_CHAR_P);
    argcount = argc;
#define ARGC argcount
#define ARGV argvec



    InitTimeNow();

    while (process_command_line_option(&ARGC,
				       ARGV,
                                       &daemonize,
#ifdef SR_DEBUG
                                       &logAppData,
#endif /* SR_DEBUG */
                                       &s_port));


    if (ARGV[1] != 0) { /* VxWorks convention, last arg = 0 */
	/* not all of the command line arguments were processed */
	usage(ARGV[0]);
	exit(1);
    }

    printf(STARTUP_BANNER_1);
    printf(STARTUP_BANNER_2);

    /* malloc packet buffers, sr_pkt_size might not be default */
    in_packet = (unsigned char *)malloc(sr_pkt_size);
    out_packet = (unsigned char *)malloc(sr_pkt_size);


#ifdef SR_DEBUG
    if (logfile_open) {
	/* some previous code has reopened stderr as a file */
	logAppData.operFlags &= ~SRLOG_STDERR;
	logAppData.operFlags |= SRLOG_FILE;
	logAppData.logfile_fp = stderr;
    }
    else {
	if (logAppData.operFlags & SRLOG_FILE) /* user request? */ {
#ifdef SR_USE_HARD_IO
	    DPRINTF((APERROR, "Can not open log file (no file system)\n"));
#else /* SR_USE_HARD_IO */
	    if (fullpath == NULL) {
		fullpath = GetLogFullPath("snmpd.log");
	    }
	    if (fullpath == NULL) {
		DPRINTF((APERROR, "Could not open log file\n"));
		logAppData.operFlags &= ~SRLOG_FILE;
	    }
	    else {
		if (logAppData.operFlags & SRLOG_APPEND) {
		    logAppData.logfile_fp = fopen(fullpath, "a");
		}
		else {
		    (void) unlink(fullpath);
		    fd_tmp = open(fullpath, O_WRONLY | O_CREAT, 0444);
		    if (fd_tmp == -1) {
			logAppData.operFlags &= ~SRLOG_FILE;
			logAppData.logfile_fp = NULL;
		    }
		    else {
			close(fd_tmp);
			logAppData.logfile_fp = fopen(fullpath, "w");
		    }
		}
		if (logAppData.logfile_fp == NULL) {
	   	    DPRINTF((APERROR, "Could not open log file %s\n",
			fullpath));
	   	    logAppData.operFlags &= ~SRLOG_FILE;
		}
		else {
                    tmpLogLevel = logAppData.operFlags;
                    logAppData.operFlags &= ~SRLOG_STDOUT;
                    logAppData.operFlags &= ~SRLOG_STDERR;
                    SetLogUserData((void *) &logAppData);
		    DPRINTF((APWARN, STARTUP_BANNER_1));
	   	    DPRINTF((APWARN, STARTUP_BANNER_2));
                    logAppData.operFlags = tmpLogLevel;
                    SetLogUserData((void *) &logAppData);
	  	    DPRINTF((APTRACE, "Successfully opened log file %s\n",
			fullpath));
		}
		free(fullpath);
	    }
#endif /* SR_USE_HARD_IO */
	}
    }

    /*
     * The SRLOG_FILE bit is overloaded.  Before this point, it is set
     * if by default or by user request a file should be opened.  After
     * this point, bit set means that a file has been opened successfully.
     */

    if ((GetLogLevel() & ~(APERROR | APWARN)) &&
	(!(logAppData.operFlags & SRLOG_TRACEFILE))) {
	/*
	 * log messages other than APERROR and APWARN are enabled
	 * and the override switch is not turned on to force those
	 * messages into the log file, so don't daemonize
	 */
        logAppData.operFlags |= SRLOG_STDERR;  /* implied */
	if (logAppData.operFlags & SRLOG_FILE) {
	    DPRINTF((APWARN|APERROR,
		"Only APERROR and APWARN messages are being printed to "
		"the log file (override with -log_tracefile)\n"));
	}
    }
#endif	/* SR_DEBUG */
    mib_oid_table = orig_mib_oid_table;

#ifdef SR_CONFIG_FP
    init_fnames();
#ifdef SR_GENERATE_CONFIGURATION
    GenerateAgentDefaults(fn_snmpd_file);
#endif /* SR_GENERATE_CONFIGURATION */
#endif /* SR_CONFIG_FP */



#ifdef SR_DEBUG
    if (stderr_closed) {
	logAppData.operFlags &= ~SRLOG_STDERR;
    }

    /*
     * The SRLOG_STDERR bit is overloaded.  Before this point, it is set
     * if by default or by user request the standard stream should be used.
     * After this point, bit set means that the standard stream is open.
     */
#endif	/* SR_DEBUG */

    /* get snmp and trap port values */
    s_port = GetSNMPPort();
    s_trap_port = GetSNMPTrapPort();




    if((TimerQ = InitTimerQueue(NULL)) == NULL) {
	exit(1);
    }

#define BINDTRIES 5

    /* set up network connection */
#ifdef SR_CLEAR_GLOBALS
    memset(&snmpdTI, 0, sizeof(TransportInfo));
#endif	/* SR_CLEAR_GLOBALS */
    snmpdTI.type = SR_IP_TRANSPORT;
    snmpdTI.t_ipPort = htons((short) s_port);

    if (snmpPortBindAddr != NULL) {
        bind_addr = inet_addr(snmpPortBindAddr);
	if (bind_addr == -1) {
	   DPRINTF((APERROR, "Cannot resolve snmp_bindaddr %s\n", snmpPortBindAddr));
	} else {
	   snmpdTI.t_ipAddr = bind_addr;
	   DPRINTF((APTRACE, "SNMP port bind restricted to address %s\n", 
                   snmpPortBindAddr));
        }
    }

    if (OpenTransport(&snmpdTI)) {
        DPRINTF((APERROR, "Cannot open SNMP transport\n"));
	exit(1);
    }

    if (BindTransport(&snmpdTI, BINDTRIES)) {
        DPRINTF((APERROR, "Cannot bind SNMP transport %s\n",
                FormatTransportString(tstr, sizeof(tstr), &snmpdTI)));
        DPRINTF((APERROR, "    is another agent running?\n"));
	exit(1);
    }

#if defined(SR_SNMPv3_ADMIN)
#endif	/* defined(SR_SNMPv3_ADMIN) */

    /* Add hook to delete sockets when snmpd dies */
    (void) taskDeleteHookAdd((FUNCPTR)close_sockets);

    /* Initialize semM semaphore */
    if ( (semM = semMCreate(  SEM_Q_PRIORITY
                            | SEM_INVERSION_SAFE ) ) == NULL ) {
        DPRINTF((APERROR, "Cannot init semaphore\n"));
        exit(1);
    }

    /* This initializes all of MIB-2 except for the system and snmp
     * groups.  The k_initialize() function is exported from k_$(os).c */
    if (k_initialize() == -1) {
        DPRINTF((APWARN, "k_initialize failed.  Continuing anyway.\n"));
    }

    initialize(&agt_snmp_lcd);

#ifdef SR_CONFIG_FP
    snmpd_rt_count = 0;
    system_rt(snmpd_rt, &snmpd_rt_count);
#ifndef SR_USE_HARD_IO
    snmpd_rt[snmpd_rt_count++] = &SNMP_ENGINE_ID_Record;
    snmpd_rt[snmpd_rt_count++] = &SNMP_ENGINE_ID_SRC_Record;
#endif /* SR_USE_HARD_IO */
    engine_rt(snmpd_rt, &snmpd_rt_count);
    snmpd_rt[snmpd_rt_count++] = NULL;

#ifndef SR_USE_HARD_IO
    engine_id.runtime_SNMP_ENGINE_ID_SRC = DEFAULT_SNMP_ENGINE_ID_SRC;
    engine_id.runtime_SNMP_ENGINE_ID = NULL;
#endif /* SR_USE_HARD_IO */
    Configure(snmpd_rt);
#ifndef SR_USE_HARD_IO
#if defined(SR_SNMPv3_ADMIN)
    updateEngineID(&engine_id);
#endif	/* defined(SR_SNMPv3_ADMIN) */
#endif /* SR_USE_HARD_IO */
#endif /* SR_CONFIG_FP */



#ifdef SR_CONFIG_FP
#ifndef SR_USE_HARD_IO
    AgentWriteConfigFile(fn_snmpd_file, snmpd_rt);
#endif /* SR_USE_HARD_IO */
#endif /* SR_CONFIG_FP */

    /* SrInitializeTraps() is called the first time a trap is sent */
    SendNotificationsSMIv2Params(coldStartTrap, NULL, NULL);

    /* daemonize used to be here, see above */
#ifdef TIMER_EXAMPLE
    {
	TimeOutDescriptor *tdp, tdp_data;

	tdp = &tdp_data;
	tdp->CallBack = hi;
	SetPeriodicTimeout(GetTimeNow(), 1500, (void *)tdp);
        SrSetPeriodicTimeout(GetTimeNow(), 1000, 0, 
                             (void *)test_time, 1, "test!!");
    }
#endif /* TIMER_EXAMPLE */


#ifdef SR_SNMPv1_ADMIN
    /* Force MakeOIDFromDot to be linked */
    oid_temp = MakeOIDFromDot("0.0");
#endif /* SR_SNMPv1_ADMIN */



    /* now the main loop... */
    /* Beyond this point use DPRINTF(()) for all message */
    while (1) {
	do {
            semTake(semM, WAIT_FOREVER);
	    CheckTimers(TimerQ);
            semGive(semM);

#ifdef POLL_INTERFACES
	    if ((time((time_t *) NULL) - polltime) >= POLL_INTERVAL) {
		poll_interfaces();
		polltime = time((time_t *) NULL);
	    }
#endif				/* POLL_INTERFACES */

	    SR_FD_SET(snmpdTI.fd, &readfds);
            maxfd = snmpdTI.fd;


#if defined(SR_SNMPv3_ADMIN)
#endif	/* defined(SR_SNMPv3_ADMIN) */

#if (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE))
        maxfd = SrSetInformFds(&readfds, maxfd);
#endif	/* (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE)) */


            
	    if (TimeQEmpty(TimerQ)) {
		timeout = NULL;	/* no timers pending, block indefinitely */
	    }
	    else {
                /*
                 * detect a clokc wrap, and adjust a timer value in 
                 * the timer queue.
                 */
                curtime = GetTimeNowWrap(&wrap);
                if ((wrap) || (timerq_update == 1)) {
                    for (p = TimerQ->head->next; 
                                         p != TimerQ->tail; p = p->next) {
                        if (p->type == CURRENT) {
                            p->when = 0;
                        }
                        else {
                            p->type = CURRENT;
                        }
                    }
                    timerq_update = 0;
                }
                p = TimerQ->head->next;
                if (p->type == WRAP) {
                    howlong = 
                        ((SR_UINT32)MAX_WRAP - curtime) + p->when;
                }
                else {
                    if (p->when <= curtime) {
                        /* Timer should already have expired; poll. */
                        howlong = 0;
                    }
                    else {
                        howlong = p->when - curtime;
                    }
                }
		timeout = &timeout_data;
		timeout->tv_sec = howlong / 100;
		timeout->tv_usec = (howlong % 100) * 10000;
		if (timeout->tv_sec > SR_SELECT_MAX_TIMEOUT_SEC) {
		    timeout->tv_sec = SR_SELECT_MAX_TIMEOUT_SEC;
		}
		if (timeout->tv_usec > SR_SELECT_MAX_TIMEOUT_USEC) {
		    timeout->tv_usec = SR_SELECT_MAX_TIMEOUT_USEC;
		}
	    }


	} while ((cc = select(maxfd+1,
                              (SELECT_BITMASK_TYPE)&readfds,
                              (SELECT_BITMASK_TYPE)NULL,
                              (SELECT_BITMASK_TYPE)NULL,
			      timeout)) == 0);

        semTake(semM, WAIT_FOREVER);


	if (cc == -1) {
	    DPRINTF((APWARN, "select returned -1 on snmp socket\n"));
            semGive(semM);
            continue;
	}


#if defined(SR_SNMPv3_ADMIN)
#endif	/* defined(SR_SNMPv3_ADMIN) */

#if (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE))
        cc = SrCheckInformFds(&readfds, cc);
        if (cc == 0) {
           semGive(semM);
            continue;
        }
#endif	/* (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE)) */

	if (FD_ISSET(snmpdTI.fd, &readfds)) /* snmp socket */
        {	
            SrcTransportInfo = &snmpdTI;
	}



        
        if (FD_ISSET(snmpdTI.fd, &readfds)) /* snmp socket */
        {

	in_packet_len = ReceiveFromTransport((char *)in_packet,
                                             sr_pkt_size,
                                             SrcTransportInfo);

	if (in_packet_len < 0) {
	    DPRINTF((APWARN, "Received Zero-Length packet from %s\n",
                     FormatTransportString(tstr, sizeof(tstr),
                                           SrcTransportInfo)));

	    /*
	     * Delete this comment, but don't put fromlen=sizeof(from) here.
	     * See comment in revision history above.  -DLB
	     */

           semGive(semM);
	    continue;
	}

        DstTransportInfo.type = SR_IP_TRANSPORT;
        DstTransportInfo.t_ipAddr = (SR_UINT32)0;
        DstTransportInfo.t_ipPort = (unsigned short)0;

	if (is_broadcast(SrcTransportInfo->t_ipAddr)) {
           semGive(semM);
	    continue;		/* Don't reply to the Broadcast Address */
	}

	DPRINTF((APPACKET, "in_packet_len = %d\n", in_packet_len));
	DPRINTF((APPACKET, "\nPacket from: %s\n",
                 FormatTransportString(tstr, sizeof(tstr), SrcTransportInfo)));


	out_packet_len = (int) SrDoMgmt(&agt_snmp_lcd,
                                        in_packet,
				        (unsigned short) in_packet_len,
				        SrcTransportInfo,
				        &DstTransportInfo,
				        out_packet,
				        sr_pkt_size);


	/*
	 * Send the response.
	 */

	if (out_packet_len > 0) {
            if (SendToTransport((char *)out_packet,
                                out_packet_len, SrcTransportInfo) < 0) {
		DPRINTF((APWARN, "Sendto failed.\n"));	/* %%% need errno */
	    }
	    else {
		snmpData.snmpOutPkts++;

#ifdef SR_CONFIG_FP
#ifndef SR_USE_HARD_IO
                if (writeConfigFileFlag == TRUE) {
                    DPRINTF((APCONFIG, "WRITING CONFIG FILE\n"));
                    AgentWriteConfigFile(fn_snmpd_file, snmpd_rt);
                    writeConfigFileFlag = FALSE;
                }
#endif /* SR_USE_HARD_IO */
#endif /* SR_CONFIG_FP */

	    }
	}
        semGive(semM);
    }

    }				/* while (1) */

    /* return 0;  never reached */
}				/* main */

#ifdef SR_DEBUG
void
MultiFunctionLogMessage(log)
    LogEntry *log;
{
    const LogAppData        *data;
    void                    *temp;
    static const LogAppData  defaultdata = {
	SRLOG_STDERR,
	0,
	NULL
    };

    if (log) {

	/*
	 * UserData in the LogEntry must be overwritten to pass
	 * the structure to other log functions.  Restore the
	 * old value from temp before returning from this function.
	 */
	temp = log->UserData;


	data = (LogAppData *) log->UserData;
	if (data == NULL) {
	    data = &defaultdata;
	}

        /* --------------- Standard I/O --------------- */
        if (data->operFlags & SRLOG_STDERR) {
            log->UserData = (void *) stderr;
            if (data->format == 0) {
                PrintShortLogMessage(log);
            }
            if (data->format == 1) {
                PrintLogMessage(log);
            }
        }
        if (data->operFlags & SRLOG_STDOUT) {
            log->UserData = (void *) stdout;
            if (data->format == 0) {
                PrintShortLogMessage(log);
            }
            if (data->format == 1) {
                PrintLogMessage(log);
            }
        }

        /* --------------- File I/O --------------- */
        if (data->operFlags & SRLOG_FILE) {
	    if (log->LogLevel & ~(APERROR | APWARN)) {
	        /* this log message is neither APERROR nor APWARN */
	        if (!(data->operFlags & SRLOG_TRACEFILE)) {
	            /* the -log_tracefile flag has not been specified */
		    /* do not print this message to a file */
		    log->UserData = temp;
	            return;
	        }
	    }
            log->UserData = (void *) data->logfile_fp;
            if (data->format == 0) {
                PrintShortLogMessage(log);
            }
            if (data->format == 1) {
                PrintLogMessage(log);
            }
        }


	/* restore the old value from temp */
	log->UserData = temp;
    }
}
#endif				/* SR_DEBUG */


/*
 * close_sockets --- Called by VxWorks when then snmp task is deleted
 *
 */
void
close_sockets(tcb)
    WIND_TCB       *tcb;
{
    int             snmpd_main();

    /* Close sockets *only* if we are being killed */
    if (tcb->entry == snmpd_main) {
	(void) close(snmpdTI.fd);
        (void) taskDeleteHookDelete((FUNCPTR)close_sockets);
#ifdef SR_TERMINATE_CODE
        terminate();
#endif /* SR_TERMINATE_CODE */
#ifdef SR_DEBUG
        CloseLog(LogBk);
#endif	/* SR_DEBUG */
    }
}


TimerQueue *
GetTimerQueue()
{
    return TimerQ;
}

#ifdef SR_DEBUG
LogBook *
GetLogBook()
{
    return LogBk;
}
#endif /* SR_DEBUG */

int
PostTimerHook(TimerQueue *TmQ, 
              SR_UINT32   id,
              void       *info,
              SR_INT32    invoke)
{
    TimeOutDescriptor *tdp = NULL;
    /* we are in the monolithic agent.  just call the callback from here */
    if (info != NULL) {
        tdp = (TimeOutDescriptor *) info;
        if (invoke == DONOT_CALL_CALLBACK) {
            (*(SrTodCallbackFunction)tdp->CallBack)(tdp->TimeOutType, 
                                                    tdp->TimerId, 
                                                    tdp->UserData1, 
                                                    tdp->UserData2);
            free(tdp);
            tdp = NULL;
        }
        else {
            (*tdp->CallBack)(tdp);
        }
    }
    return 1;
}

int
CancelTimerHook(TmQ, id, anymore)
     TimerQueue *TmQ;
     SR_UINT32 id;
     int anymore;
{
  return 1;	/* just for the compiler */
}

int
GetEmanateVersion()
{
#define MAX_SUPPORTED_EMANATE_VERSION 3
    return MAX_SUPPORTED_EMANATE_VERSION;
}

