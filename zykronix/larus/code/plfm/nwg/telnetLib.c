/* telnetLib.c - telnet server library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
04a,28may97,mjc  redesigned to allow user specified command interpreter and  
                 concurrent telnet connection support
03b,09aug94,dzb  fixed activeFlag race with cleanupFlag (SPR #2050).
                 made telnetdSocket global (SPR #1941).
		 added logFdFromRlogin (SPR #2212).
03a,02may94,ms   increased stack size for SIMHPPA.
02z,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
02y,01feb93,jdi  documentation cleanup for 5.1.
02x,18jul92,smb  Changed errno.h to errnoLib.h.
02w,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
02v,24apr92,rfs  Fixed flaky shell restart upon connection termination.
                 The functions telnetInTask() and telnetdExit() were changed.
                 This is fixing SPR #1427.  Also misc ANSI noise.
02u,13dec91,gae  ANSI cleanup.
02t,14nov91,jpb  moved remCurIdSetto shellLogout (shellLib.c).
02s,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed includes to have absolute path from h/
		  -fixed #else and #endif
		  -changed READ, WRITE and UPDATE to O_RDONLY O_WRONLY and ...
		  -changed VOID to void
		  -changed copyright notice
02r,01aug91,yao  fixed to pass 6 args to excJobAdd() call.
02q,13may91,shl  undo'ed 02o.
02p,30apr91,jdi	 documentation tweaks.
02o,29apr91,shl  added call to restore original machine name, user and
		 group ids (spr 916).
02n,05apr91,jdi	 documentation -- removed header parens and x-ref numbers;
		 doc review by dnw.
02m,24mar91,jdi  documentation cleanup.
02l,05oct90,shl  fixed telnetExit() to restore original user and password.
02k,02oct90,hjb  added a call to htons() where needed.
02j,08aug90,dnw  changed declaration of tnInput from void to int.
		 added forward declaration of setMode().
02i,07may90,shl  changed entry point of tTelnetd back to telnetd.
02h,18apr90,shl  added shell security.
		 changed telnetd name to tTelnetd.
02g,20aug89,gae  bumped telnetTaskStackSize from 1500 to 5000 for SPARC.
02f,29sep88,gae  documentation.
02e,06jun88,dnw  changed taskSpawn/taskCreate args.
02d,30may88,dnw  changed to v4 names.
02c,28may88,dnw  changed to use shellOrigStdSet (...) instead of shellSetOrig...
		 changed not to use shellTaskId global variable.
02b,01apr88,gae  made it work with I/O system revision.
02a,27jan88,jcf  made kernel independent.
01g,14dec87,dnw  fixed bug in telnetdIn() that caused system crashes.
01f,19nov87,dnw  changed telnetd to wait for shell to exist before accepting
		   remote connections.
01e,17nov87,ecs  lint.
01d,04nov87,ecs  documentation.
	     &   fixed bug in use of setsockopt().
	    dnw  changed to call shellLogoutInstall() instead of logoutInstall.
01c,24oct87,gae  changed setOrig{In,Out,Err}Fd() to shellSetOrig{In,Out,Err}().
		 made telnetdOut() exit on EOF from master pty fd.
		 made telnetInit() not sweat being called more than once.
		 added shellLock() to telnetd to get exclusive use of shell.
01g,20oct87,gae  added logging device for telnet shell; made telnetInit()
		   create pty device.
01f,05oct87,gae  made telnetdExit() from telnetdIn() - used by logout().
		 removed gratuitous standard I/O ioctl's.
		 made "disconnection" cleaner by having shell do restart.
01e,26jul87,dnw  changed default priority of telnet tasks from 100 to 2.
		 changed task priority and ids to be global variables so
		   they can be accessed from outside this module.
01d,04apr87,dnw  de-linted.
01c,27mar87,dnw  documentation
		 fixed bug causing control sequences from remote to be
		   misinterpreted.
		 added flushing of pty in case anything was left before
		   remote login.
01b,27feb87,dnw  changed to spawn telnet tasks UNBREAKABLE.
01a,20oct86,dnw  written.
*/

/*
DESCRIPTION
This library provides a remote login facility for VxWorks.  It uses the 
telnet protocol to enable users on remote systems to log in to VxWorks.
This telnet facility supports remote login to the VxWorks shell and the user 
specified command interpreters.  It also support concurrent telnet connections.

The telnet daemon provides a tty-like interface to the remote user through the 
use of the VxWorks pseudo-terminal driver, ptyDrv.

The telnet facility is initialized by calling telnetInit(), which is called
automatically when INCLUDE_TELNET is defined in configAll.h.

The trivial VxWorks telnet service that connect remote telnet user with the 
built-in VxWorks shell via standard telnet port is provided by module 
remShell.c.

This module have 2 levels of the API.

.SH "LOW LEVEL API:"
The telnet daemon  accepts remote telnet login requests and connect the command 
interpreter with the remote user by calling the command interpreter specific 
connection routine which gets a file descriptor of the pty device as one of its 
arguments.

The command interpreter connection routine should be declared as follows:

.CS
STATUS interpConn 
    (
    int     interpConnArg,
    int     ptySFd,
    int     exitArg,
    int *   disconnArg,
    char *  msgBuf
    )
.CE
.IP "<interpConnArg>"
an argument specified by user in the telnetServiceAdd() call;
.IP "<ptySFd>"
file descriptor of the pty device across which the command interpreter connects 
with the telnet;
.IP "<telnetdExit>"
a pointer to the routine that the command interpreter must call in response user
logout;
.IP "<exitArg>"
an argument to pass to the telnet logout service routine (telnetdExit());
.IP "<disconnArg>"
a pointer to a variable where argument for interpDisconn() may be returned, 
initialized to zero before interpConn() call;
.IP "<msgBuf>"
a pointer to a buffer where the error message may be returned when interpConn() 
terminates with ERROR, the telnet demon sends this message to the telnet client.
.LP
Each time a connection terminates telnet calls the command interpreter specific 
disconnection routine providing it with value returned by the corresponding 
interpConn() call.  This command interpreter disconnection routine is called 
independently of the reason of the connection termination: the connection 
closure by the remote host, or the logout command issued to a command 
interpreter by the remote user.

The command interpreter disconnection routine should be declared as follows:

.CS
void interpDisconn
    (
    int disconnArg
    )
.CE
.IP "<disconnArg>"
value returned by the corresponding interpConn() call
.LP
When the logout command is issued to a command interpreter, it must call the 
telnetdExit() routine to inform the telnet that the user is wishing to close 
connection.

To add such kind of the telnet service use telnetServiceAdd() call.

.SH "HIGH LEVEL API (telnet call):"
All that the user of the module must provide this is the routine that read data 
from the stdin and write data to the stdout.  When the telnet accepts remote 
telnet login requests it spawns the new task, causes the task's standard input 
and output to be redirected to the remote user, and calls the specified routine.
When the remote user disconnects this routine must just exit.  

If a telnet connection is broken by a remote host, standard I/O of the task 
that calls the routine provided by the module user, is redirected to an invalid 
file descriptor equal to the maximum number of file descriptors in the sistem 
(NUM_FILES defined in configAll.h).  Thus subsequent reads from and writes to 
standard I/O of that task will return error.

To add such kind of the telnet service use telnetCallAdd() call.

INCLUDE FILES: telnetLib.h

SEE ALSO:
ptyDrv, rlogLib
*/

#include "vxWorks.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "ptyDrv.h"
#include "netinet/in.h"
#include "ioLib.h"
#include "taskLib.h"
#include "telnetLib.h"
#include "stdlib.h"
#include "unistd.h"
#include "errnoLib.h"
#include "string.h"
#include "stdio.h"
#include "fcntl.h"
#include "sockLib.h"
#include "logLib.h"
#include "shellLib.h"
#include "remLib.h"
#include "sysLib.h"
#include "iosLib.h"
#include "taskVarLib.h"
#include "configAll.h"
#include "tss_types.h"
#include "tnetptdef.h"
#include "inetLib.h" 
#include "tss_if.h"
#include <time.h>
#include <timers.h>


#define DEBUG


#define STDIN_BUF_SIZE		512
#define STDOUT_BUF_SIZE		512

#define sendToPty(fd,buf,len)	write(fd,buf,len)
#define sendToRem(fd,buf,len)	write(fd,buf,len)

/* telnet input states */

#define TS_DATA		0
#define TS_IAC		1
#define TS_CR		2
#define TS_BEGINNEG	3
#define TS_ENDNEG	4
#define TS_WILL		5
#define TS_WONT		6
#define TS_DO		7
#define TS_DONT		8

/* telnet pty device state */

#define TELNET_FREE	1
#define TELNET_BUSY	0

#define ERROR_MSG_LEN	260
#define PTY_NAME_LEN	20

typedef struct {
    U8_t  status;
    U8_t  index;
    U32_t mmsTaskId;
    U32_t client;
    U32_t slaveFd;

/* USER SESSEION */
    char  userName[11];
    U32_t accLvl;
    char  time[9];
    char  date[11];
    char  tnIPAddr[18];
/* USER SESSION */

} TELNET_SESS_INFO, *pTELNET_SESS_INFO;


#define  MIN_USER_TEL_SESSION_ENTRY   1
/* #define  __TRAPLOG_DEBUG__ */

/* global variables */

#if 0
#ifndef DEBUG
int telnetTaskOptions   = VX_SUPERVISOR_MODE | VX_UNBREAKABLE;
#else
int telnetTaskOptions   = VX_SUPERVISOR_MODE;
#endif

#if    (CPU_FAMILY == SIMHPPA) || (CPU_FAMILY == SIMSPARCSUNOS)
int telnetTaskStackSize = 10000;
#else  /* CPU_FAMILY == SIMHPPA */
int telnetTaskStackSize = 5000;
#endif /* CPU_FAMILY == SIMHPPA */
#endif
extern int telnetTaskOptions;
extern int telnetTaskStackSize;

extern int mmsShellStackSize;
extern int mmsShellPriority;
extern int mmsShellOptions;

extern MmsShell(S32_t inFd, S32_t outFd, S32_t exitAble);

extern U32_t tnconShellSessTimer(char *timePtr, char *datePtr);
#if 0
typedef
struct _userSessionEntry_t {
        SR_INT32        userSessionIndex;
        OctetString     *userSessionUserID;
        OctetString     *userSessionIpAddr;
        SR_INT32        userSessionAccLvl;
        OctetString     *userSessionLIDate;
        OctetString     *userSessionLITime;
        SR_INT32        userSessionType;
#ifdef U_userSessionEntry
U_userSessionEntry
#endif /* U_userSessionEntry */
        char            valid[1];
} userSessionEntry_t;

userSessionEntry_t *tnUserSessionInfoGet( int   conNum, 
                                          char  *userid,
                                          char  *ipaddr,
                                          int   acclvl,
                                          char  *timePtr, 
                                          char  *datePtr, 
                                          int   type);

LOCAL  userSessionEntry_t    tnUserSession;
#endif
/* local variables */

LOCAL char ptyTelnetName [] = "/pty/telnet";
LOCAL int telnetMaxConnNum;	/* Maximum number of telnet connections. */
LOCAL pTELNET_SESS_INFO telnetSessInf; /* array that indicates status of telnet session */


/*******************************************************************************
*
* telnetPtyAlloc - allocate a pty device
*
* RETURNS: OK if a pty device was found, otherwise ERROR.
*/
LOCAL STATUS telnetPtyAlloc 
    (
    char *       ptyName
    )
    {
    int i;

    taskLock ();

    for (i = 0; i < telnetMaxConnNum; i++)
        if (telnetSessInf[i].status == TELNET_FREE)
            {
            
            telnetSessInf[i].userName[0] = '\0'; 
            telnetSessInf[i].time[0] = '\0'; 
            telnetSessInf[i].date[0] = '\0'; 
            telnetSessInf[i].accLvl  = 0;

            
            telnetSessInf[i].userName[0] = '\0'; 
            telnetSessInf[i].time[0] = '\0'; 
            telnetSessInf[i].date[0] = '\0'; 
            telnetSessInf[i].accLvl  = 0;

            telnetSessInf[i].status = TELNET_BUSY;
            taskUnlock ();

            sprintf (ptyName, "%s%d.", ptyTelnetName, i);
            return OK;
            }

    taskUnlock ();
    return ERROR;
    } /* telnetPtyAlloc */

/*******************************************************************************
*
* telnetPtyFree - free a pty device
*
* RETURNS: N/A
*/
LOCAL void telnetPtyFree
    (
    char * ptyName
    )
    {
    telnetSessInf[ atoi(ptyName + sizeof(ptyTelnetName) - 1) ].status = TELNET_FREE;
    } /* telnetPtyFree */

/*******************************************************************************
*
* setMode -
*
* RETURNS: N/A.
*/

LOCAL void setMode
    (
    int    ptyFd,		/* fd to local pseudo-terminal */
    BOOL * pRaw,		/* TRUE = raw mode enabled */
    BOOL * pEcho,		/* TRUE = echo enabled */
    int    telnetOption,
    BOOL   enable
    )
    {
    FAST int ioOptions;

    switch (telnetOption)
	{
	case TELOPT_BINARY: *pRaw  = enable; break;
	case TELOPT_ECHO:   *pEcho = enable; break;
	}

    if (*pRaw)
	ioOptions = 0;
    else
	{
	ioOptions = OPT_7_BIT | OPT_ABORT | OPT_TANDEM | OPT_LINE;
	if (*pEcho)
	    ioOptions |= OPT_ECHO | OPT_CRMOD;
	}

    (void) ioctl (ptyFd, FIOOPTIONS, ioOptions);
    } /* setMode */

/*******************************************************************************
*
* localDoOpt - offer/acknowledge local support of option
*
* This routine will try to enable or disable local support for the specified
* option.  If local support of the option is already in the desired mode, no
* action is taken.  If the request is to disable the option, the option will
* always be disabled.  If the request is to enable the option, the option
* will be enabled, IF we are capable of supporting it.  The remote is
* notified that we WILL/WONT support the option.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS localDoOpt
    (
    TBOOL    myOpts[],	/* current option settings - this side */
    BOOL *   pRaw,	/* TRUE = raw mode enabled */
    BOOL *   pEcho,	/* TRUE = echo enabled */
    FAST int option,	/* option to be enabled/disabled */
    BOOL     enable,	/* TRUE = enable option, FALSE = disable */
    int      remFd,	/* fd to remote */
    int      ptyFd,	/* fd to local pseudo-terminal ??? */
    BOOL     reqFromRem	/* TRUE = request is from remote */
    )
    {
    BOOL will = enable;

    if (myOpts [option] == enable)
	return (OK);

    switch (option)
	{
	case TELOPT_BINARY:
	case TELOPT_ECHO:
	    setMode (ptyFd, pRaw, pEcho, option, enable);
	    break;

	case TELOPT_SGA:
	    break;

	default:
	    will = FALSE;
	    break;
	}

    if ((myOpts [option] != will) || reqFromRem)
	{
	char msg[3];

	msg[0] = IAC;
	msg[1] = will ? WILL : WONT;
	msg[2] = option;

	sendToRem (remFd, msg, 3);

	myOpts [option] = will;
	}

    return ((will == enable) ? OK : ERROR);
    } /* localDoOpt */

/*******************************************************************************
*
* remDoOpt - request/acknowledge remote enable/disable of option
*
* This routine will try to accept the remote's enable or disable,
* as specified by "will", of the remote's support for the specified option.
* If the request is to disable the option, the option will always be disabled.
* If the request is to enable the option, the option will be enabled, IF we
* are capable of supporting it.  The remote is notified to DO/DONT support
* the option.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS remDoOpt
    (
    TBOOL    remOpts[],	/* current option settings - other side */
    BOOL *   pRaw,	/* TRUE = raw mode enabled */
    BOOL *   pEcho,	/* TRUE = echo enabled */
    FAST int option,    /* option to be enabled/disabled */
    BOOL     enable,	/* TRUE = enable option, FALSE = disable */
    int      remFd,	/* fd to remote */
    int      ptyFd,	/* fd to local pseudo-terminal ??? */
    BOOL     reqFromRem	/* TRUE = request is from remote */
    )
    {
    BOOL doOpt = enable;

    if (remOpts [option] == enable)
	return (OK);

    switch (option)
	{
	case TELOPT_BINARY:
	case TELOPT_ECHO:
	    setMode (ptyFd, pRaw, pEcho, option, enable);
	    break;

	case TELOPT_SGA:
	    break;

	default:
	    doOpt = FALSE;
	    break;
	}

    if ((remOpts [option] != doOpt) || reqFromRem)
	{
	char msg[3];

	msg[0] = IAC;
	msg[1] = doOpt ? DO : DONT;
	msg[2] = option;

	sendToRem (remFd, msg, 3);

	remOpts [option] = doOpt;
	}

    return ((enable == doOpt) ? OK : ERROR);
    } /* remDoOpt */

/*******************************************************************************
*
* tnInput - process input from remote
*
* RETURNS: state
*/

LOCAL int tnInput
    (
    TBOOL      myOpts[],	/* current option settings - this side */
    TBOOL      remOpts[],	/* current option settings - other side */
    BOOL *     pRaw,		/* TRUE = raw mode enabled */
    BOOL *     pEcho,		/* TRUE = echo enabled */
    FAST int   state,		/* state of input stream */
    FAST int   remFd,		/* fd of socket to otherside */
    FAST int   ptyFd,		/* fd of pty to this side */
    FAST char *buf,		/* ptr to input chars */
    FAST int   n		/* number of chars input */
    )
    {
    char cc;
    int ci;

    while (--n >= 0)
	{
	cc = *buf++;			/* get next character */
	ci = (unsigned char) cc;	/* convert to int since many values
					 * are negative characters */
	switch (state)
	    {
	    case TS_CR:		/* doing crmod; ignore add'l linefeed */
		state = TS_DATA;
		if ((cc != EOS) && (cc != '\n'))
		    sendToPty (ptyFd, &cc, 1);	/* forward char */
		break;

	    case TS_DATA:	/* just pass data */
		if (ci == IAC)
		    state = TS_IAC;
		else
		    {
		    sendToPty (ptyFd, &cc, 1);	/* forward char */
		    if (!myOpts [TELOPT_BINARY] && (cc == '\r'))
			state = TS_CR;
		    }
		break;

	    case TS_IAC:

		switch (ci)
		    {
		    case BREAK:		/* interrupt from remote */
		    case IP:
			/* XXX interrupt (); */
			logMsg ("telnetInTask: interrupt\n", 0, 0, 0, 0, 0, 0);
			state = TS_DATA;
			break;

		    case AYT:		/* Are You There? */
			{
			static char aytAnswer [] = "\r\n[yes]\r\n";

			sendToRem (remFd, aytAnswer, sizeof (aytAnswer) - 1);
			state = TS_DATA;
			break;
			}

		    case EC:		/* erase character */
			sendToPty (ptyFd, "\b", 1);
			state = TS_DATA;
			break;

		    case EL:		/* erase line */
			sendToPty (ptyFd, "\025", 1);
			state = TS_DATA;
			break;

		    case DM:		/* data mark */
			state = TS_DATA;
			break;

		    case SB:		/* sub-option negotiation begin */
			state = TS_BEGINNEG;
			break;

		    case WILL: state = TS_WILL;	break;	/* remote will do opt */
		    case WONT: state = TS_WONT;	break;	/* remote wont do opt */
		    case DO:   state = TS_DO;	break;	/* req we do opt */
		    case DONT: state = TS_DONT;	break;	/* req we dont do opt */

		    case IAC:
			sendToPty (ptyFd, &cc, 1);	/* forward char */
			state = TS_DATA;
			break;
		    }
		break;

	    case TS_BEGINNEG:
		/* ignore sub-option stuff for now */
		if (ci == IAC)
		    state = TS_ENDNEG;
		break;

	    case TS_ENDNEG:
		state = (ci == SE) ? TS_DATA : TS_BEGINNEG;
		break;

	    case TS_WILL:		/* remote side said it will do opt */
		(void)remDoOpt (remOpts, pRaw, pEcho, ci, TRUE, remFd, ptyFd, 
                                TRUE);
		state = TS_DATA;
		break;

	    case TS_WONT:		/* remote side said it wont do opt */
		(void)remDoOpt (remOpts, pRaw, pEcho, ci, FALSE, remFd, ptyFd, 
                                TRUE);
		state = TS_DATA;
		break;

	    case TS_DO:			/* remote wants us to do opt */
		(void)localDoOpt (myOpts, pRaw, pEcho, ci, TRUE, remFd, ptyFd, 
                                          TRUE);
		state = TS_DATA;
		break;

	    case TS_DONT:		/* remote wants us to not do opt */
		(void)localDoOpt (myOpts, pRaw, pEcho, ci, FALSE, remFd, ptyFd, 
                                  TRUE);
		state = TS_DATA;
		break;

	    default:
		printErr ("telnetd: invalid state = %d\n", state);
		break;
	    }
	}

    return (state);
    } /* tnInput */

/*******************************************************************************
*
* telnetPutTask - stdout to socket process
*
* This routine gets spawned by the telnet daemon to move data between the
* client socket and the pseudo-terminal.  The task exits when the telnetdExit() 
* is called, or the client disconnects.
*
* NOMANUAL - but not LOCAL for i()
*/

void telnetPutTask
    (
    FAST int sock,      /* socket to copy output to */
    FAST int ptyMfd     /* pty Master fd */
    )
    {
    FAST int n;
    char buf [STDOUT_BUF_SIZE];

    while ((n = read (ptyMfd, buf, sizeof (buf))) > 0)
	{
	/* XXX should scan for IAC and double 'em to escape 'em */
	write (sock, buf, n);
	}
    close (sock);

    } /* telnetPutTask */

/*******************************************************************************
*
* telnetInTask - socket to stdin process
*
* This routine gets spawned by the telnet daemon to move data between the
* pseudo-terminal and the client socket.  The task exits when the telnetdExit() 
* is called, or the client disconnects.
*
* RETURNS: N/A.
*
* NOMANUAL - but not LOCAL for i()
*/

void telnetGetTask
    (
    int             ptySfd,	   /* pty Slave fd */
    FAST int        sock,	   /* socket to copy input from */
    int             ptyMfd,	   /* pty Master fd */
    int             outTaskId      /* out task id */
    )
    {
    int   n = 0;
    int   state = TS_DATA;
    char  buf [STDIN_BUF_SIZE];
    char  ptyName[PTY_NAME_LEN];
    TBOOL myOpts [256];		/* current option settings - this side */
    TBOOL remOpts [256];	/* current option settings - other side */
    BOOL  raw;			/* TRUE = raw mode enabled */
    BOOL  echo;			/* TRUE = echo enabled */
    int   optval;

    /* turn on KEEPALIVE so if the client crashes, we'll know about it */

    optval = 1;
    setsockopt (sock, SOL_SOCKET, SO_KEEPALIVE, 
                (char *) &optval, sizeof (optval));

    /* initialize modes and options and offer to do remote echo */

    raw  = FALSE;
    echo = TRUE;
    bzero ((char *) myOpts, sizeof (myOpts));
    bzero ((char *) remOpts, sizeof (remOpts));

    (void)localDoOpt (&myOpts[0], &raw, &echo, TELOPT_ECHO, TRUE, sock, ptyMfd, 
                      FALSE);

    (void)localDoOpt (&myOpts[0], &raw, &echo, TELOPT_SGA, TRUE, sock, ptyMfd, 
                      TRUE);

    /* Loop, reading from the socket and writing to the pty. */

    while ((n = read (sock, buf, sizeof (buf))) > 0)
        state = tnInput (&myOpts[0], &remOpts[0], &raw, &echo, state, sock, 
                         ptyMfd, buf, n);

    /* Exit and cleanup.  The above loop will exit when the socket is shut 
     * down.  The socket can be shut down as a result of the connection
     * terminating from the remote host, or as a result of the logout
     * command issued to a command interpreter serviced by this connection.  
     * When the logout command is used, the telnetdExit() routine below is 
     * called.  
     */

    if (ioctl (ptyMfd, FIOGETNAME, (int) &ptyName) == OK)
        {
        ptyName[strlen(ptyName) - 1] = '\0';
        telnetPtyFree(ptyName);
        }
    else
        logMsg ("Can't get pty name\n", 0, 0, 0, 0, 0, 0);

    taskDelete(outTaskId);
    close (sock);
    close (ptyMfd);
    close (ptySfd);

    } /* telnetInTask */

/*******************************************************************************
*
* telnetdExit - exit routine for telnet
*
* This is the support routine for logout.  It must be called from the command  
* interpreter as a result of the logout command being issued to it to inform 
* the telnet that the user wish to terminate connection.
*
* RETURNS: N/A.
*/

void telnetdExit
    (
    FAST int exitArg
    )
    {
    /* In the current implementation the <exitArg> is the fd of the socket used 
     * to establish the telnet connection.
     * Shuting down the socket coses the telnetInTask() above to exit. 
     */

    write (exitArg, "\n", 1);
    shutdown (exitArg, 2);
    } /* telnetdExit */

/*******************************************************************************
*
* telnetd - VxWorks telnet daemon
*
* This routine enables remote users to log in to the VxWorks over the network 
* via the telnet protocol.  It is spawned by telnetSeviceAdd(), which should be
* called to add a telnet service.
*
* The telnet daemon requires the existence of pseudo-terminal devices, which 
* must be created by telnetInit() before telnetd() is spawned.  The telnetd()
* routine creates two additional processes, `telnetInTask' and `telnetPutTask',
* whenever a remote user is logged in.  These processes exit when the remote
* connection is terminated.
*
* RETURNS: N/A
*/

void telnetTask
    (
    int             serviceNum    /* port number of the service */
    )
    {
    int telnetTaskPriority;	/* priority of telnet tasks */
    int telnetPutTaskId;	/* task ID of telnetPutTask task */
    int telnetInTaskId;		/* task ID of telnetInTask task */
    int mmsShellTaskId;
    struct sockaddr_in myAddress;
    struct sockaddr_in clientAddress;
    int clientAddrLen;
    int client;
    int masterFd;
    int slaveFd;
    int sd;
    int socket_opt = 1;

    int disconnArg;

    int  connNum = 0;		/* number of telnet connection */
    char ptyName [PTY_NAME_LEN];
    char charBuf [ERROR_MSG_LEN];
    char *msg = "\r\nSorry, trouble to establish connection.\r\n";

    taskPriorityGet (0, &telnetTaskPriority); /* for In and Out tasks */

    /* open a socket and wait for a client */
 
    sd = socket (AF_INET, SOCK_STREAM, 0);

    if ( setsockopt (sd, SOL_SOCKET, SO_DEBUG, (void *)(&socket_opt), 4) == ERROR)
       printf ("Telnet: could not set socket option\n");

    bzero ((char *) &myAddress, sizeof (myAddress));
    myAddress.sin_family = AF_INET;
    myAddress.sin_port   = htons (serviceNum);

    if (bind (sd, (struct sockaddr *) &myAddress, sizeof (myAddress)) == ERROR)
	{
	printErr ("telnetd: port %d bind failed.\n", serviceNum);
	return;
	}

    listen (sd, 1);

    FOREVER
	{
	errnoSet (OK);		/* clear errno for pretty i() display */

        disconnArg = 0;
        client     = 0;
        masterFd   = 0;
        slaveFd    = 0;

	/* now accept connection */

	clientAddrLen = sizeof (clientAddress);
	client = accept (sd, (struct sockaddr *)&clientAddress, &clientAddrLen);

	if (client == ERROR)
	    {
	    printErr ("telnetd: port %d accept failed - status = 0x%x\n",
		      serviceNum, errnoGet ());
	    continue;
	    }

	/* create the pseudo terminal:
	 * the master side is connected to the socket to the
	 * remote machine - two processes telnetInTask & telnetPutTask
	 * handle input and output.
	 */

        if (telnetPtyAlloc (ptyName) != OK)
            {
            printErr ("telnetd: pty device allocation failed.\n");
	    write (client, msg, strlen (msg));
	    close (client);
            continue;
            }

        connNum = atoi(ptyName + sizeof(ptyTelnetName ) - 1);

        strcpy (charBuf, ptyName);
        strcat(charBuf,"M");
	if ((masterFd = open (charBuf, O_RDWR, 0)) == ERROR)
	    {
	    printErr ("telnetd: error opening %s\n", charBuf);
	    write (client, msg, strlen (msg));
	    goto telnetd_error;
	    }

        charBuf[strlen(charBuf) - 1] = 'S';
	if ((slaveFd = open (charBuf, O_RDWR, 0)) == ERROR)
	    {
	    printErr ("telnetd: error opening %s\n", charBuf);
	    write (client, msg, strlen (msg));
	    goto telnetd_error;
	    }

        /* setup the slave device to act like a terminal */

        (void) ioctl (slaveFd, FIOOPTIONS, OPT_TERMINAL);

        /* flush out pty device */

        (void) ioctl (slaveFd, FIOFLUSH, 0 /*XXX*/);

        /* Connect the slave pty device with a command interpreter */
        sprintf(charBuf, "tmmsShell%d", connNum);
        if ((mmsShellTaskId = taskNameToId(charBuf)) != ERROR)
	{
            taskDelete(mmsShellTaskId);
            taskDelay(1);
	}

        if ((mmsShellTaskId = taskSpawn ( charBuf, mmsShellPriority,
                                           mmsShellOptions,
                                           mmsShellStackSize,
                                           (FUNCPTR)MmsShell,
                                           slaveFd, slaveFd, client,
                                           0, 0, 0, 0, 0, 0, 0)) == ERROR)
        {
            printErr ("telnetd: error spawning %s child - status = 0x%x\n",
                      charBuf, errnoGet ());
            goto telnetd_error;
        }

        telnetSessInf[atoi(ptyName + sizeof(ptyTelnetName) - 1)].mmsTaskId = mmsShellTaskId;

        telnetSessInf[atoi(ptyName + sizeof(ptyTelnetName) - 1)].client = client;
        telnetSessInf[atoi(ptyName + sizeof(ptyTelnetName) - 1)].slaveFd = slaveFd;

        inet_ntoa_b(clientAddress.sin_addr, telnetSessInf[atoi(ptyName + sizeof(ptyTelnetName) - 1)].tnIPAddr);


	/* spawn the output process which transfers data from the master pty
	 * to the socket. */

        sprintf(charBuf, "tTnetdOut%d", connNum);
        if ((telnetPutTaskId = taskNameToId(charBuf)) != ERROR)
	{
            taskDelete(telnetPutTaskId);
            taskDelay(1);
	}
	if ((telnetPutTaskId = taskSpawn ( charBuf, telnetTaskPriority,
					   telnetTaskOptions,
					   telnetTaskStackSize,
					   (FUNCPTR)telnetPutTask, 
                        		   client, masterFd, 
					   0, 0, 0, 0, 0, 0, 0, 0)) == ERROR)
        {
            printErr ("telnetd: error spawning %s child - status = 0x%x\n",
		      charBuf, errnoGet ());
            goto telnetd_error;
        }

	/* spawn the input process which transfers data from the client socket
	 * to the master pty. */

        sprintf(charBuf, "tTnetdIn%d", connNum);
        if ((telnetInTaskId = taskNameToId(charBuf)) != ERROR)
	{
            taskDelete(telnetInTaskId);
            taskDelay(1);
	}
	if ((telnetInTaskId =  taskSpawn ( charBuf, telnetTaskPriority,
					   telnetTaskOptions,
					   telnetTaskStackSize,
					   (FUNCPTR)telnetGetTask, 
                        		   slaveFd, client, masterFd, 
					   telnetPutTaskId, 0, 0, 0, 0, 0, 0)) != ERROR)
            continue;

	printErr ("telnetd: error spawning %s child - status = 0x%x\n",
		  charBuf, errnoGet ());
	taskDelete (telnetPutTaskId);
	/*
        taskDelete (mmsShellTaskId);
	*/

telnetd_error:		/* try to do a tidy clean-up */
        telnetPtyFree(ptyName);
        if (client != 0)
	    close (client);
        if (masterFd != 0)
	    close (masterFd);
        if (slaveFd != 0)
	    close (slaveFd);
	}
    } /* telnetd */


/*******************************************************************************
*
* telnetInit - initialize the telnet support
*
* This routine initializes the telnet facility, which supports remote login
* to the VxWorks shell and the user specified command interpreters via the 
* telnet protocol.  It creates so many pty devices as necessary to support 
* number of telnet connection defined by its variable <maxConnNum>.  
*
* It is called automatically when INCLUDE_TELNET is defined in configAll.h.
*
* RETURNS: N/A
*/

void MmsTelnetInit 
    (
    int maxConnNum
    )
    {
    static BOOL done;	/* FALSE = not done */
    char nameBuf[PTY_NAME_LEN];
    int i;
#ifdef SCC
    int  telnetdId;	/* task ID of telnetd task */
#endif

    if (done)
	{
	printErr ("telnetInit: already initialized.\n");
	return;
	}

    if (ptyDrv () == ERROR)
	{
	printErr ("telnetInit: unable to initialize pty driver.\n");
	return;
	}

    telnetMaxConnNum = maxConnNum;
    telnetSessInf = malloc (sizeof (TELNET_SESS_INFO) * telnetMaxConnNum);
    for (i = 0; i < telnetMaxConnNum; i++)
        {
        sprintf (nameBuf, "%s%d.", ptyTelnetName, i);

        if (ptyDevCreate (nameBuf, 1024, 32 * 1024) == ERROR)
	    {
	    printErr ("telnetInit: unable to create pty device %d.\n", i);
	    return;
	    }
        telnetSessInf[i].status = TELNET_FREE;
        }

#ifdef SCC
    telnetdId = taskSpawn ("tTnetd", 51,
			   telnetTaskOptions, telnetTaskStackSize,
			   (FUNCPTR)telnetTask, 23, 0,
                           0, 0, 0, 0, 0, 0, 0, 0);
#endif

    taskSpawn ("tTnetSmd", 51,
			   telnetTaskOptions, telnetTaskStackSize,
			   (FUNCPTR)telnetTask, 
                           SM_TELNET_FIRST_PORT+SiwMyPhySlotNo(), 0,
                           0, 0, 0, 0, 0, 0, 0, 0);
    done = TRUE;
    } /* telnetInit */

/*******************************************************************************
*
* 
*
* 
*/
STATUS telnetShellId
    (
    int  taskId
    )
    {
    int i;

    for (i = 0; i < telnetMaxConnNum; i++)
    {
        if (telnetSessInf[i].status == TELNET_BUSY &&
            (int)telnetSessInf[i].mmsTaskId == taskId)
                return(i);
    }
    return ERROR;
    } /* telnetPtyAlloc */

/*******************************************************************************
*
* 
*
* 
*/
int
tnConShellIdGet(int conNum)
{
   if (telnetSessInf[conNum].status == TELNET_BUSY)
   {
       return(telnetSessInf[conNum].mmsTaskId);
   }
   return(-1);


}


/*******************************************************************************
*
* 
*
* 
*/
void
tnConShellIdSet(int conNum, int id)
{
   if (telnetSessInf[conNum].status == TELNET_BUSY)
       telnetSessInf[conNum].mmsTaskId = id;
}


/*******************************************************************************
*
* 
*
* 
*/
int tnconShellClientGet(int conNum)
{
   if (telnetSessInf[conNum].status == TELNET_BUSY)
       return(telnetSessInf[conNum].client);
   return(-1);
}

#if 0
/*******************************************************************************
*
* 
*
* 
*/
int tnconShellSlaveGet(int conNum)
{
   if (telnetSessInf[conNum].status == TELNET_BUSY)
       return(telnetSessInf[conNum].slaveFd);
   return(-1);
}


/* USER SESSION */ /* Programmer: Frank Shiau*/
/* ------------------------------------------------------------
 * Name        : U32_t tnconShellSessSet(U32_t taskid,
 *                                       char *name,
 *                                       U32_t acclvl)
 *              
 * Description :  
 *
 * In Parms    : taskid, name, acclvl      
 * Out Parms   : OK or ERROR(-1)  
 *
 * ------------------------------------------------------------
 */
U32_t
tnconShellSessSet(U32_t taskid, char *name, U32_t acclvl)
{
    int i;
    char timePtr[9];
    char datePtr[11];

#if (defined(SCC) && !defined(FLASH_BOOT))
    static  userSessionEntry_t    userSessionEntryData;
    userSessionEntry_t            *resultPtr;
#endif

   for (i = 0; i < telnetMaxConnNum; i++)
   {  
      if ((telnetSessInf[i].status == TELNET_BUSY) &&
          ((U32_t)(telnetSessInf[i].mmsTaskId) == taskid))
      {

         bzero( telnetSessInf[i].userName, 
               sizeof(telnetSessInf[i].userName) );

         bzero( telnetSessInf[i].time, 
                sizeof(telnetSessInf[i].time) );

         bzero( telnetSessInf[i].date, 
                sizeof(telnetSessInf[i].date) );

         strncpy( telnetSessInf[i].userName, name, strlen(name) );
         telnetSessInf[i].userName[10] = '\0';

         telnetSessInf[i].accLvl = acclvl;

         tnconShellSessTimer(timePtr ,datePtr);

         timePtr[8] = '\0';
         datePtr[10] = '\0';

         strncpy(telnetSessInf[i].time, timePtr, 9);
         strncpy(telnetSessInf[i].date, datePtr, 11);

#if (defined(SCC) && !defined(FLASH_BOOT))

         SiwEventLog( NULL, 
                      SIW_EVENT_SYM(SNMP, USER_LOGIN),
                      telnetSessInf[i].userName,
                      telnetSessInf[i].tnIPAddr,
                      "TELNET");

#ifdef __TRAPLOG_DEBUG__
printf("**** Before Telnet Login to Trap Log\n");
#endif

            memset( (char *) &userSessionEntryData, 
                    0, 
                    sizeof(userSessionEntry_t) );

            resultPtr = tnUserSessionInfoGet(i,
                                             telnetSessInf[i].userName,
                                             telnetSessInf[i].tnIPAddr,
                                             telnetSessInf[i].accLvl,
                                             telnetSessInf[i].time,
                                             telnetSessInf[i].date,
                                             1);

            memcpy( &userSessionEntryData, 
                    resultPtr, 
                    sizeof(userSessionEntry_t)); 

            TrapEventLog( shelfUserLogin_TRAP, 
                          (U32_t *)&userSessionEntryData, 
                          SIW_EVENT_SYM(SNMP, USER_LOGIN_TR),
                          telnetSessInf[i].userName,
                          telnetSessInf[i].tnIPAddr,
                          "TELNET");

#ifdef __TRAPLOG_DEBUG__
printf("**** After Telnet Login to Trap Log\n");
#endif

#endif
         return OK;
      }  
   }
   return(ERROR);
}


/* ------------------------------------------------------------
 * Name        : U32_t tnconShellSessGet(int conNum, 
 *                                       char *userid, 
 *                                       U32_t *acclvl, 
 *                                       char *logintime, 
 *                                       char *logindate, 
 *                                       char *ipaddr)
 *              
 * Description :  
 *
 * In Parms    : conNum, userid, acclvl, logintime, logindate, ipaddr    
 * Out Parms   : OK(1) or ERROR(-1) 
 *
 * ------------------------------------------------------------
 */
U32_t
tnconShellSessGet(int conNum, char *userid, U32_t *acclvl, char *logintime, char *logindate , char *ipaddr)
{
   if (telnetSessInf[conNum].status == TELNET_BUSY)
   {
       strncpy(userid, telnetSessInf[conNum].userName, 11);

       strncpy(logintime, telnetSessInf[conNum].time, 9);
       strncpy(logindate, telnetSessInf[conNum].date, 11);

       *acclvl = telnetSessInf[conNum].accLvl;

       strncpy(ipaddr , telnetSessInf[conNum].tnIPAddr,18);

       return(1);
   }
   else
   {
       return(ERROR);
   }
}


/* ------------------------------------------------------------
 * Name        : U32_t tnconShellSessTimer(char *timePtr,
 *                                         char *datePtr)
 *              
 * Description :  
 *
 * In Parms    : timePtr, datePtr     
 * Out Parms   : NULL, ret  
 *
 * ------------------------------------------------------------
 */
U32_t
tnconShellSessTimer(char *timePtr ,char *datePtr)
{
    time_t curTime;
    int ret;
    struct tm timeBuf1, timeBuf2;

    /* Get the Time related stuff from VxWorks */

    curTime = time(NULL);
    if ((int)curTime == ERROR)
    {
       printf("Problem reading the current time.\n");
       return NULL;
    } 


   /* printf("Time = %s\n", ctime(&curTime)); */

    ret = gmtime_r(&curTime, &timeBuf1);
    if (ret != OK)
    {
       printf("Problem converting to broken-time.\n");
       return NULL;
    }

    ret = strftime(timePtr, 9, "%H:%M:%S", &timeBuf1);
    ret = gmtime_r(&curTime, &timeBuf2);
    ret = strftime(datePtr, 11, "%m/%d/%Y", &timeBuf2);

    return(ret);

}

/* ------------------------------------------------------------
 * Name        : U32_t ipAddrtoString(U32_t ipaddr
 *                                    char *pString)
 *              
 * Description :  
 *
 * In Parms    : iadd, pString     
 * Out Parms   : OK  
 *
 * ------------------------------------------------------------
 */
U32_t
ipAddrtoString(U32_t ipaddr, char *pString)
{
   struct in_addr iaddr;
   iaddr.s_addr = ipaddr;
   inet_ntoa_b(iaddr, pString);
   return(OK);
}


/* ------------------------------------------------------------
 * Name        : U32_t tnconShellLogoutGet(int taskid, 
 *                                         int conNum, 
 *                                         char *userid, 
 *                                         U32_t *acclvl, 
 *                                         char *logintime, 
 *                                         char *logindate , 
 *                                         char *ipaddr)
 *              
 * Description :  
 *
 * In Parms    : taskid, userid, ipaddr    
 * Out Parms   : OK(1) or ERROR(-1) 
 *
 * ------------------------------------------------------------
 */
U32_t
tnconShellLogoutGet(U32_t  taskid, 
                    int    *conNum, 
                    char   *userid, 
                    U32_t  *acclvl, 
                    char   *logintime, 
                    char   *logindate , 
                    char   *ipaddr)
{
   int i;

   for (i = 0; i < telnetMaxConnNum; i++)
   {
      if ( (telnetSessInf[i].mmsTaskId == taskid) )
     /*      if ((telnetSessInf[i].status == BUSY) &&
          (telnetSessInf[i].mmsTaskId == taskid))  */
      {

         strncpy(userid, telnetSessInf[i].userName, 11);

         strncpy(ipaddr, telnetSessInf[i].tnIPAddr,18);

         *acclvl = telnetSessInf[i].accLvl;

         *conNum = i;

         strncpy(logintime, telnetSessInf[i].time, 9);

         strncpy(logindate, telnetSessInf[i].date, 11);

         return(OK);
      }
   }


   /* Abnormal logout */
   strcpy(userid, "Unknown");
   strcpy(ipaddr, "Unknown");
   *acclvl = 0;
   *conNum = 0;
   strcpy(logintime, "Unknown");
   strcpy(logindate, "Unknown");   

   return(ERROR);
      
   
}



/* ------------------------------------------------------------
 * Name        : userSessionEntry_t *
 *               tnUserSessionInfoGet(SR_INT32 userSessionIndex)
 *
 * Description : Fill in the sturcture for the Lougout Trap
 *               
 *
 * In Parms    : clientIndex
 *              
 * Out Parms   : userSessionEntrydata
 *
 * ------------------------------------------------------------
 */
userSessionEntry_t *
tnUserSessionInfoGet( int   conNum, 
                      char  *userid,
                      char  *ipaddr,
                      int   acclvl,
                      char  *timePtr, 
                      char  *datePtr, 
                      int   type)
{

    static OctetString sessionUserid;
    static OctetString sessionIpaddr;
    static OctetString sessionLogindate;
    static OctetString sessionLogintime;

    static char        sessionUseridStr[11];
    static char        sessionIpaddrStr[18];
    static char        sessionLogintimeStr[9];
    static char        sessionLogindateStr[11];

#ifdef __TRAPLOG_DEBUG__

printf("**** Before tnUserSessionInfoGet()\n");
printf(" Index: %d, Name:%s, IP:%s Lvl:%d, TM:%s, DT:%s, type:%d.\n",
         conNum,
         userid,
         ipaddr,
         acclvl,
         timePtr,
         datePtr,
         type);

#endif


    tnUserSession.userSessionIndex = 
        conNum + MIN_USER_TEL_SESSION_ENTRY; /*telnet:1~10, console:21 */


    tnUserSession.userSessionUserID = &sessionUserid;
    sessionUserid.octet_ptr = &sessionUseridStr[0];
    sessionUserid.length = 
        strlen(userid);
    bzero(sessionUseridStr, 11);
    strncpy( sessionUseridStr,
             userid,
             sessionUserid.length);


    tnUserSession.userSessionIpAddr = &sessionIpaddr;
    sessionIpaddr.octet_ptr = &sessionIpaddrStr[0];
    sessionIpaddr.length = strlen(ipaddr);
    bzero(sessionIpaddrStr, 18);
    strncpy(sessionIpaddrStr, ipaddr, strlen(ipaddr));


    tnUserSession.userSessionAccLvl = acclvl;


    tnUserSession.userSessionLITime = &sessionLogintime;
    sessionLogintime.octet_ptr = &sessionLogintimeStr[0];
    sessionLogintime.length = strlen(timePtr);
    bzero(sessionLogintimeStr, 9);
    strncpy(sessionLogintimeStr, timePtr, strlen(timePtr));


    tnUserSession.userSessionLIDate = &sessionLogindate;
    sessionLogindate.octet_ptr = &sessionLogindateStr[0];
    sessionLogindate.length = strlen(datePtr);
    bzero(sessionLogindateStr, 11);
    strncpy(sessionLogindateStr, datePtr, strlen(datePtr));


    tnUserSession.userSessionType = type;

#ifdef __TRAPLOG_DEBUG__ 
printf("**** After tnUserSessionInfoGet()\n");
#endif

    return(&tnUserSession);
}


/* USER SESSION */
#endif

