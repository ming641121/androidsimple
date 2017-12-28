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
 * snmpio.c
 *
 *   snmpio.c contains routines which are commonly called by SNMP
 *   applications for network input and output.
 */



#include "sr_conf.h"

#include <stdio.h>

#include <string.h>



#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>         /*  needed for recvfrom and soclose */

#include <types.h>         /*  needed for recvfrom and soclose */

 
#include <sys/socket.h>

#include <netinet/in.h>


#include <errno.h>


#include <signal.h>

#include <sockLib.h>
#include <hostLib.h>
#include <arpa/inet.h>


#include "sr_proto.h"
#include "sr_type.h"

#include "sr_snmp.h"


/* this include ONLY standard include files */
#include "snmpio.h"

/*
 * Before you're tempted to put include files like netdb.h and
 * netinet/in.h here, don't.  some systems (ultrix, for example)
 * don't check for multiple inclusion of these and they are included in
 * include/snmpio.* which is where you should put them.
 */

#include "diag.h"
SR_FILENAME
extern SR_UINT32 sr_pkt_size;




int TimeOut
    SR_PROTOTYPE((void));
/*
 *  The following remain globalized as the command line utilities
 *  (through utility/common.c and utility/snmp_v2.c) reference them
 *  directly.
 */

char imagename[128]; /* place to hold name of image */
int fd;
int received_state;
SR_INT32 packet_len;
unsigned char *packet;
int seconds;


#define SNMPIO_INITIALIZE_IO 1
#define SNMPIO_INITIALIZE_TRAP_IO 2
#define SNMPIO_INITIALIZE_INFORM_IO 3

static struct sockaddr_in Sin;
static struct sockaddr_in from;

/* ----------------------------------------------------------------------
 *
 *  initialize_io - initialize SNMP io, TRAP io or INFORM io
 *
 *  Arguments: 
 *  i  (const char *) program_name
 *     Used for diagnostics.
 *  i  (const char *) name
 *     Host name for this host.
 *  i  (const int) action
 *     Action to take.  Note this module does not initialize Trap
 *     Receive IO, as that was sufficient different to leave alone.
 *
 *     .   SNMPIO_INITIALIZE_IO        Initialize SNMP IO
 *     .   SNMPIO_INITIALIZE_TRAP_IO   Initialize TRAP IO
 *     .   SNMPIO_INITIALIZE_INFORM_IO Initialize INFORM IO
 *
 *  History:
 *     InitializeIO and InitializeTrapIO were distinct subroutines
 *     with large amounts of code duplication.   When InitializeInformIO
 *     was needed, rather than duplicate code again, I merge the two
 *     and added what functionality I needed (buffer allocation
 *     for informs).  InitializeTrapRcvIO was sufficiently different
 *     I left it alone.
 */

static void
initialize_io(
    const char *program_name,
    const char *name,
    const int action)
{
    FNAME("initialize_io")

    if (action == SNMPIO_INITIALIZE_IO ||
        action == SNMPIO_INITIALIZE_INFORM_IO) {
        /* now must malloc packet since since might not be default */
        packet = (unsigned char *) malloc(sr_pkt_size);
        if (packet == NULL) {
            DPRINTF((APERROR,
                     "%s: malloc for packet of size %s failed\n",
                      Fname, sr_pkt_size));
            exit(-1);
        }
    } else {
        packet = NULL;
    }

    /* first, copy program name to the save area for use with error messages */
    strncpy(imagename,
            program_name,
            (size_t) MIN(sizeof(imagename) - 1, strlen(program_name)));
    /* make sure terminated properly in case it was long */
    imagename[sizeof(imagename) - 1] = '\0';

    /* set up timer for timeout */
    SetSigFunction(SIGALRM, (SigFunc) TimeOut);

    /* now, set up UDP connection */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	DPRINTF((APERROR, "%s:  unable to connect to socket.\n", imagename));
	exit(-1);
    }

    if ((Sin.sin_addr.s_addr = inet_addr((char *)name)) == -1) {
        Sin.sin_addr.s_addr = hostGetByName((char *)name);
        if (Sin.sin_addr.s_addr == SR_ERROR) {
            DPRINTF((APERROR, "%s:  host %s unknown.\n", Fname, name));
            exit(-1);
        }
    }
    Sin.sin_family = AF_INET;
    if (action == SNMPIO_INITIALIZE_IO) {
        Sin.sin_port = htons((short) GetSNMPPort());
    } else {                           /* TRAP, INFORM */
        Sin.sin_port = htons((short) GetSNMPTrapPort());
    }


}

/*
 * InitializeIO: does initialization and opens udp connection
 */
void
InitializeIO(
    const char *program_name,
    const char *name)
{
    initialize_io(program_name, name, SNMPIO_INITIALIZE_IO);
    return;
}

/*
 * SendRequest: This is now a macro in ../include/snmpio.h
 */

/*
 * SrSendRequest:
 */
int
SrSendRequest(
    int sock,
    const unsigned char *packet,
    const SR_INT32 packet_length)
{
    unsigned char   *out_pkt;

    /* allocate the packet */
    out_pkt = (unsigned char *) malloc((size_t)sr_pkt_size);
    if (out_pkt == NULL) {
	DPRINTF((APWARN, "%s:  couldn't get send buffer\n", imagename));
	return(FALSE);
    }

    /*
     * copy the data into the buffer
     *
     * we have to cast packet_length to be an size_t to avoid trouble
     * with long/int sizes on some machines
     */
    memcpy(out_pkt, packet, (size_t) packet_length);

    /* for debug PrintPacket(packet, packet_length); */
    if (sendto(sock, (char *)out_pkt, (int) packet_length, 0,
             (struct sockaddr *)&Sin, sizeof(Sin)) < 0) {
	DPRINTF((APWARN, "%s:  send.\n", imagename));
	free((char *) out_pkt);
	return (FALSE);
    }



    /* free the packet buffer */
    free((char *) out_pkt);
    return (TRUE);
}


/*
 * GetResponse:
 */
int
GetResponse()
{



    int fromlen = sizeof(from);


    fd_set readmask;
    struct timeval timeout;

    int             cc;


    received_state = SR_WAITING;


    FD_ZERO(&readmask);
    SR_FD_SET(fd, &readmask);
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    if (timeout.tv_sec > SR_SELECT_MAX_TIMEOUT_SEC) {
        timeout.tv_sec = SR_SELECT_MAX_TIMEOUT_SEC;
    }
    if (timeout.tv_usec > SR_SELECT_MAX_TIMEOUT_USEC) {
        timeout.tv_usec = SR_SELECT_MAX_TIMEOUT_USEC;
    }
    cc = select((fd + 1),
                (SELECT_BITMASK_TYPE)&readmask,
                (SELECT_BITMASK_TYPE)0,
                (SELECT_BITMASK_TYPE)0,
                &timeout);
    if (cc > 0) {
        packet_len = (SR_INT32) recvfrom(fd, (char *) packet, sr_pkt_size, 0,
				 (struct sockaddr *)&from, &fromlen);
        if (packet_len > 0) {
	    return (SR_RECEIVED);
	}
    }
    return (SR_TIMEOUT);



}

/*
 * CloseUp: 
 */
void
CloseUp()
{






}


/*
 * TimeOut: Handle timing out.
 */
int
TimeOut()
{
    received_state = SR_TIMEOUT;

/*
 *  On sysv systems, signal handling is automatically cleared when
 *  the signal is received, so it has to be reasserted here.
 *  This is also true of AIX, which, for some reason does not have
 *  SYSV set.
 */

#if defined(SYSV)
    SetSigFunction(SIGALRM, (SigFunc) TimeOut);
#endif	/* defined(SYSV) */

    return (0);
}

/*
 * MakeReqId: 
 */
SR_INT32
MakeReqId()
{
    time_t        ltime;

    time(&ltime);
    return (SR_INT32) (ltime & 0x7fff);
}



/* 
 * InitializeTrapIO: does initialization and opens udp connection 
 */
void
InitializeTrapIO(program_name, name)
    const char *program_name;
    const char *name;
{
    initialize_io(program_name, name, SNMPIO_INITIALIZE_TRAP_IO);
    return;
}

/* 
 * InitializeTrapRcvIO: does initialization and opens udp connection 
 */
int
InitializeTrapRcvIO(program_name)
    const char *program_name;
{
    int             i;
    int status = 0;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	DPRINTF((APWARN, "%s: socket() failed\n", program_name));
	return -1;
    }

    /* clear out and initialize the socket's address */
    memset(&Sin, 0, sizeof(Sin));
    Sin.sin_family = AF_INET;	/* this will be an internet socket */
    Sin.sin_port = htons((short) GetSNMPTrapPort());
    for (i = 0;
         (((status = bind(fd, (struct sockaddr *)&Sin, sizeof(Sin))) < 0) &&
           (i < 5));
         i++)
    {
	perror("traprcv:  bind");
    }
    if (status < 0) {
        return -1;
    }



    return 0;    /* normal return */
}



/*
 * InitializeInformIO: does initialization and opens udp port
 */
void
InitializeInformIO(
    const char *program_name,
    const char *name)
{
    initialize_io(program_name, name, SNMPIO_INITIALIZE_INFORM_IO);
    return;
}
