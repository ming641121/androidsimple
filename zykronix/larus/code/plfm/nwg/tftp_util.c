/*
 *  Copyright(C) 2005, All Rights Reserved, by
 *  Ahoya Networks Inc. San Jose, CA
 *
 *  Abstract:
 *
 *  File Name: tftp_util.c
 *
 *  Description:
 *  This module implements the TFTP service for Ahoya product.
 *
 *  Note:
 *
 *  Author(s):
 *  Ahoya Engr.
 */

/* includes */

#include "vxWorks.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "ioLib.h"
#include "errno.h"
#include "sys/times.h"
#include "string.h"
#include "inetLib.h"
#include "fioLib.h"
#include "errnoLib.h"
#include "memLib.h"
#include "unistd.h"
#include "stdio.h"
#include "sys/socket.h"
#include "sockLib.h"
#include "hostLib.h"
#include "stdlib.h"
#include "ifLib.h"
#include "selectLib.h"
#include "rpc/rpc.h"
#include "xdr_nfs.h"
#include "dirent.h"
#include "sys/stat.h"
#include "nfsLib.h"
#include "nfsDrv.h"
#include "dosFsLib.h"
#include "rt11FsLib.h"
#include "netDrv.h"

#include "tss_types.h"
#include "sys_if.h"
#include "siw_if.h"
#include "nwg.h"
#include "nwg_if.h"
#include "tftpLib.h"
#include <ctype.h>

extern char* typeKeyword[];

/* globals */

BOOL 	tftpVerbose	= FALSE; 		/* verbose mode		*/
BOOL	tftpTrace	= FALSE;		/* packet tracing 	*/
int	tftpTimeout	= TFTP_TIMEOUT * 5;	/* total timeout (sec.)	*/
int	tftpReXmit	= TFTP_TIMEOUT;		/* rexmit value  (sec.) */

/* tftp task parameters */

int 	tftpTaskPriority = TFTP_TASK_PRIORITY;	/* tftp task priority   */
int 	tftpTaskOptions	 = TFTP_TASK_OPTIONS;	/* tftp task options    */
int 	tftpTaskStackSize= TFTP_TASK_STACKSIZE; /* tftp task stack size */

/* forward declarations */

STATUS		tftpXfer ();			/* Higher Level Routines */
STATUS		tftpTask ();
STATUS		tftpCopy ();

TFTP_DESC *	tftpInit ();			/* Lower Level Routines	*/
STATUS		tftpModeSet ();
STATUS  	tftpPeerSet ();
STATUS  	tftpPut (TFTP_DESC *, char *, FD_t *, SYS_LOG_FILTER_t *, int);
STATUS  	tftpGet (TFTP_DESC *, char *, FD_t *, int);
STATUS		tftpInfoShow ();
STATUS  	tftpQuit ();
STATUS_t        tftpdFilenameParse (U8_t *filename, TFTP_DESC *tftpDesc, int readOrWrite);
STATUS_t    IsValidImageFilename (U8_t *temp_filename);
int         ReadNextToken(char *buffer, char *token, int maxBytes);
int         WriteErrorMsgToOutFile(FD_t * fdp, char *badline, int errorType, 
								   int offset, bool_t firstError);

/* locals */

LOCAL 		tftpRequestCreate ();
LOCAL void 	tftpPacketTrace ();
LOCAL STATUS 	connectOverLoopback ();


/*******************************************************************************
*
* tftpXfer - transfer a file via TFTP using a stream interface
*
* This routine initiates a transfer to or from a remote file via TFTP.  It
* spawns a task to perform the TFTP transfer and returns a descriptor from
* which the data can be read (for "get") or to which it can be written (for
* "put") interactively.  The interface for this routine is similar to ftpXfer()
* in ftpLib.
*
* <pHost> is the server name or Internet address.  A non-zero value for <port>
* specifies an alternate TFTP server port number (zero means use default TFTP
* port number (69)).  <pFilename> is the remote filename. <pCommand> specifies
* the TFTP command.  The command can be either "put" or "get".
*
* The tftpXfer() routine returns a data descriptor, in <pDataDesc>, from
* which the TFTP data is read (for "get") or to which is it is written (for
* "put").  An error status descriptor gets returned in the variable
* <pErrorDesc>.  If an error occurs during the TFTP transfer, an error
* string can be read from this descriptor.  After returning successfully
* from tftpXfer(), the calling application is responsible for closing both
* descriptors.
*
* If there are delays in reading or writing the data descriptor, it is
* possible for the TFTP transfer to time out.
*
* INTERNAL
* tftpXfer() uses stream sockets connected over the loopback address for
* communication between the calling application and the task performing
* the tftpCopy.  It might be desirable to use some alternate method so TCP
* is no longer necessary for this module.
*
* EXAMPLE
* The following code demonstrates how tftpXfer() may be used:
*
* .CS
*     int  dataFd;
*     int  errorFd;
*     int  num;
*     char buf [512];
*
*     if (tftpXfer ("congo", 0, "/usr/fred", "get", "ascii", &dataFd,
*                   &errorFd) == ERROR)
*         return (ERROR);
*
*     while ((num = read (dataFd, buf, sizeof (buf))) > 0)
*         {
*         ....
*         }
*
*     close (dataFd);
*
*     if (read (errorFd, buf, sizeof (buf)) > 0)
*         {
*         printf ("YIKES! An error occurred!:%s\en", buf);
*         .....
*         }
*
*     close (errorFd);
* .CE
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
* S_tftpLib_INVALID_ARGUMENT
*
* SEE ALSO: ftpLib
*/

STATUS tftpXfer
    (
    char *		pHost,			/* host name or address */
    int			port,			/* port number		*/
    char *		pFilename,		/* remote filename 	*/
    char *		pCommand,		/* TFTP command 	*/
    char *		pMode,			/* TFTP transfer mode 	*/
    int	*		pDataDesc,		/* return data desc.	*/
    int	*		pErrorDesc 		/* return error desc.	*/
    )

    {
    int			dataSock;		/* data socket 		*/
    int 		errorSock;		/* error socket 	*/

    if ((pDataDesc == NULL) || (pErrorDesc == NULL))
        {
	errno = S_tftpLib_INVALID_ARGUMENT;
	return (ERROR);
	}

    if (connectOverLoopback (pDataDesc, &dataSock) == ERROR)
	return (ERROR);

    if (connectOverLoopback (pErrorDesc, &errorSock) == ERROR)
       {
       (void) close (*pDataDesc);
       (void) close (dataSock);
       return (ERROR);
       }

    if (taskSpawn ("tTftpTask", tftpTaskPriority, tftpTaskOptions,
	           tftpTaskStackSize, tftpTask, (int)pHost, port,
		   (int) pFilename, (int) pCommand, (int) pMode, dataSock,
		   errorSock, 0, 0, 0) == ERROR)
	{
	close (*pDataDesc);			/* close up both ends of */
	close (*pErrorDesc);			/* both sockets 	 */
	close (dataSock);
	close (errorSock);
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* tftpTask - perform a TFTP file transfer
*
* This task calls tftpXfer() to perform a TFTP file transfer.  If the transfer
* fails, it writes an error status message to <errorFd>.  tftpTask closes
* both <errorFd> and <fd> on completion.
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* NOMANUAL
*/

STATUS tftpTask
    (
    char *		pHost,			/* host name or address	*/
    int			port,			/* optional port number	*/
    char *		pFilename,		/* remote filename 	*/
    char *		pCommand,		/* TFTP command 	*/
    char *		pMode,			/* TFTP transfer mode 	*/
    int			fd,			/* data fd		*/
    int			errorFd 		/* error fd		*/
    )

    {
    int			status;
    char		errorMsg [100];

    if ((status =
	 tftpCopy (pHost, port, pFilename, pCommand, pMode, fd)) == ERROR)
	{
	(void) sprintf (errorMsg, "tftp transfer failed: error 0x%x\n", errno);
    	(void) write (errorFd, errorMsg, strlen (errorMsg));
	}

    (void) close (errorFd);
    (void) close (fd);
    return (status);
    }

/*******************************************************************************
*
* tftpCopy - transfer a file via TFTP
*
* This routine transfers a file using the TFTP protocol to or from a remote
* system.
* <pHost> is the remote server name or Internet address.  A non-zero value
* for <port> specifies an alternate TFTP server port (zero means use default
* TFTP port number (69)).  <pFilename> is the remote filename. <pCommand>
* specifies the TFTP command, which can be either "put" or "get".
* <pMode> specifies the mode of transfer, which can be "ascii",
* "netascii",  "binary", "image", or "octet".
*
* <fd> is a file descriptor from which to read/write the data from or to
* the remote system.  For example, if the command is "get", the remote data
* will be written to <fd>.  If the command is "put", the data to be sent is
* read from <fd>.  The caller is responsible for managing <fd>.  That is, <fd>
* must be opened prior to calling tftpCopy() and closed up on completion.
*
* EXAMPLE
* The following sequence gets an ASCII file "/folk/vw/xx.yy" on host
* "congo" and stores it to a local file called "localfile":
*
* .CS
*     -> fd = open ("localfile", 1001, 0666)
*     -> tftpCopy ("congo", 0, "/folk/vw/xx.yy", "get", "ascii", fd)
*     -> close (fd)
* .CE
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
* S_tftpLib_INVALID_COMMAND
*
* SEE ALSO: ftpLib
*/

STATUS tftpCopy
    (
    char *		pHost,			/* host name or address	*/
    int			port,			/* optional port number	*/
    char *		pFilename,		/* remote filename 	*/
    char *		pCommand,		/* TFTP command 	*/
    char *		pMode,			/* TFTP transfer mode 	*/
    int	 		fd 			/* fd to put/get data   */
    )

    {
    TFTP_DESC *		pTftpDesc;		/* TFTP descriptor	*/
    int			status;			/* return status 	*/

    if ((pTftpDesc = tftpInit ()) == NULL)	/* get tftp descriptor  */
	return (ERROR);
    						/* set peer and mode */

    if ((tftpPeerSet (pTftpDesc, pHost, port) == ERROR) ||
        ((pMode != NULL) && (tftpModeSet (pTftpDesc, pMode) == ERROR)))
	{
	(void) tftpQuit (pTftpDesc);
	return (ERROR);
	}

    if (strcmp (pCommand, "get") == 0)		/* get or put the file */
	{
#if 0
	status = tftpGet (pTftpDesc, pFilename, fd, TFTP_CLIENT);
#endif
	}

    else if (strcmp (pCommand, "put") == 0)
	{
#if 0
	status =  tftpPut (pTftpDesc, pFilename, fd, NULL, TFTP_CLIENT);
#endif
	}

    else
	{
	errno = S_tftpLib_INVALID_COMMAND;
	status = ERROR;
	}

    (void) tftpQuit (pTftpDesc);		/* close tftp session */
    return (status);
    }

/*******************************************************************************
*
* tftpInit - initialize a TFTP session
*
* This routine initializes a TFTP session by allocating and initializing a TFTP
* descriptor.  It sets the default transfer mode to "netascii".
*
* RETURNS: A pointer to a TFTP descriptor if successful, otherwise NULL.
*/

TFTP_DESC * tftpInit (void)

    {
    TFTP_DESC *		pTftpDesc;		/* TFTP descriptor     */
    struct sockaddr_in	localAddr;   		/* local address       */

    if ((pTftpDesc = (TFTP_DESC *) calloc (1, sizeof (TFTP_DESC))) == NULL)
        return (NULL);

				    		/* default mode is netascii */
    (void) strcpy (pTftpDesc->mode, "netascii");
    pTftpDesc->connected  = FALSE;
    						/* set up a datagram socket */

    if ((pTftpDesc->sock = socket (AF_INET, SOCK_DGRAM, 0)) == ERROR)
	{
	free ((char *) pTftpDesc);
    	return (NULL);
        }

    bzero ((char *) &localAddr, sizeof (struct sockaddr_in));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons ((u_short) 0);

    if (bind (pTftpDesc->sock, (SOCKADDR *) &localAddr,
	      sizeof (struct sockaddr_in)) == ERROR)
	{
	free ((char *) pTftpDesc);
	(void) close (pTftpDesc->sock);
    	return (NULL);
	}

    return (pTftpDesc);				/* return descriptor */
    }

/*******************************************************************************
*
* tftpModeSet - set the TFTP transfer mode
*
* This routine sets the transfer mode associated with the TFTP descriptor
* <pTftpDesc>.  <pMode> specifies the transfer mode, which can be
* "netascii", "binary", "image", or "octet".  Although recognized, these
* modes actually translate into either octet or netascii.
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
*  S_tftpLib_INVALID_DESCRIPTOR
*  S_tftpLib_INVALID_ARGUMENT
*  S_tftpLib_INVALID_MODE
*/

STATUS tftpModeSet
    (
    TFTP_DESC *		pTftpDesc,		/* TFTP descriptor 	*/
    char *		pMode 			/* TFTP transfer mode  	*/
    )

    {
    char *		sep = " ";		/* separator character	*/
    TFTP_MODE *		pTftpModes;		/* pointer to modes	*/
    LOCAL TFTP_MODE 	tftpModes [] = 		/* available modes	*/
	{
	    { "ascii",		"netascii" },
	    { "netascii",	"netascii" },
	    { "binary",		"octet"	   },
	    { "image",		"octet"	   },
	    { "octet",		"octet"	   },
	    { NULL,		NULL	   }
	};

    if (pTftpDesc == NULL)			/* validate arguments */
	{
	errno = S_tftpLib_INVALID_DESCRIPTOR;
	return (ERROR);
	}

    if (pMode == NULL)
	{
	errno = S_tftpLib_INVALID_ARGUMENT;
    	return (ERROR);
    	}

    /* look for actual mode type from the passed mode */

    for (pTftpModes = tftpModes; pTftpModes->m_name != NULL; pTftpModes++)
	{
	if (strcmp (pMode, pTftpModes->m_name) == 0)
	    {
	    (void) strcpy (pTftpDesc->mode, pTftpModes->m_mode);
	    if (tftpVerbose)
		printf ("mode set to %s.\n", pTftpDesc->mode);
	    return (OK);
	    }
	}
    						/* invalid mode, print error */
    errno = S_tftpLib_INVALID_MODE;
    printf ("%s: unknown mode\n", pMode);
    printf ("valid modes: [");

    for (pTftpModes = tftpModes; pTftpModes->m_name != NULL; pTftpModes++)
	{
	printf ("%s%s", sep, pTftpModes->m_name);
	if (*sep == ' ')
	    sep = " | ";
	}
    printf (" ]\n");
    return (ERROR);
    }

/*******************************************************************************
*
* tftpPeerSet - set the TFTP server address
*
* This routine sets the TFTP server (peer) address associated with the TFTP
* descriptor <pTftpDesc>.  <pHostname> is either the TFTP server name
* (e.g., "congo") or the server Internet address (e.g., "90.3").  A non-zero
* value for <port> specifies the server port number (zero means use
* the default TFTP server port number (69)).
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
*  S_tftpLib_INVALID_DESCRIPTOR
*  S_tftpLib_INVALID_ARGUMENT
*  S_tftpLib_UNKNOWN_HOST
*/

STATUS tftpPeerSet
    (
    TFTP_DESC *		pTftpDesc,		/* TFTP descriptor	*/
    char *		pHostname, 		/* server name/address	*/
    int    		port			/* port number		*/
    )

    {
    if (pTftpDesc == NULL)			/* validate arguments */
	{
	errno = S_tftpLib_INVALID_DESCRIPTOR;
	return (ERROR);
	}

    if ((port < 0) || (pHostname == NULL))
	{
	errno = S_tftpLib_INVALID_ARGUMENT;
	return (ERROR);
	}

    bzero ((char *) &pTftpDesc->serverAddr, sizeof (struct sockaddr_in));
    pTftpDesc->serverAddr.sin_family = AF_INET;

						/* get server address */
    if ((((int)(pTftpDesc->serverAddr.sin_addr.s_addr =
		hostGetByName (pHostname))) == ERROR) &&

        (((int)(pTftpDesc->serverAddr.sin_addr.s_addr =
	        inet_addr (pHostname))) == ERROR))
	{
	pTftpDesc->connected = FALSE;
	printf ("tftpPeerSet %s: unknown host.\n", pHostname);
	errno = S_tftpLib_UNKNOWN_HOST;
	return (ERROR);
	}
						/* set port and hostname */
    (void) strcpy (pTftpDesc->serverName, pHostname);
    pTftpDesc->serverPort = (port != 0) ? htons ((u_short) port) :
					  htons ((u_short) TFTP_PORT);
    pTftpDesc->connected = TRUE;

    if (tftpVerbose)
	printf("Connected to %s [%d]\n", pHostname,
	       ntohs (pTftpDesc->serverPort));
    return (OK);
    }

/*******************************************************************************
*
* tftpPut - put a file to a remote system
*
* This routine puts data from a local file (descriptor) to a file on the remote
* system.  <pTftpDesc> is a pointer to the TFTP descriptor.  <pFilename> is
* the remote filename.  <fd> is the file descriptor from which it gets the
* data.  A call to tftpPeerSet() must be made prior to calling this routine.
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
*  S_tftpLib_INVALID_DESCRIPTOR
*  S_tftpLib_INVALID_ARGUMENT
*  S_tftpLib_NOT_CONNECTED
*/

STATUS tftpPut
    (
    TFTP_DESC *		pTftpDesc,		/* TFTP descriptor       */
    char      *		pFilename,		/* remote filename       */
    FD_t                *fd,			/* file descriptor       */
    SYS_LOG_FILTER_t    *cookie,                /* call log fd */
    int			clientOrServer 		/* which side is calling */
    )

    {
    int			size;			/* size of data to send */
    TFTP_MSG		tftpMsg;  		/* TFTP message         */
    TFTP_MSG		tftpAck;  		/* TFTP ack message     */
    int			port;			/* return port number   */
    int			block;			/* data block number    */
    BOOL		convert;		/* convert to ascii	*/
    char		charTemp;		/* temp char holder	*/
    S32_t               offset;

    int                   status;

    if (pTftpDesc == NULL)			/* validate arguments */
	{
	errno = S_tftpLib_INVALID_DESCRIPTOR;
	return (ERROR);
	}

    if (pFilename == NULL)
	{
	errno = S_tftpLib_INVALID_ARGUMENT;
	return (ERROR);
	}

    if (!pTftpDesc->connected)			/* must be connected */
	{
	printf ("tftpPut: No target machine specified.\n");
	errno = S_tftpLib_NOT_CONNECTED;
	return (ERROR);
	}
    						/* initialize variables */
    
    convert = (strcmp (pTftpDesc->mode, "netascii") == 0) ? TRUE : FALSE;

    charTemp = '\0';
    bzero ((char *) &tftpMsg, sizeof (TFTP_MSG));
    bzero ((char *) &tftpAck, sizeof (TFTP_MSG));
    pTftpDesc->serverAddr.sin_port = pTftpDesc->serverPort;
    block = 0;
    offset = 0;

    if (tftpVerbose)
 	printf ("putting to %s:%s [%s]\n",  pTftpDesc->serverName,
		pFilename, pTftpDesc->mode);

    /*
     *  If we're a client, then
     *  create and send write request message.  Get ACK back with
     *  block 0, and a new port number (TID).  If we're a server,
     *  this isn't necessary.
     */

    if (clientOrServer == TFTP_CLIENT)
    {
	size = tftpRequestCreate (&tftpMsg, TFTP_WRQ, pFilename,
				  pTftpDesc->mode);

	if (tftpVerbose)
	    printf("Sending WRQ to port %d\n", pTftpDesc->serverPort);

	if ((tftpSend (pTftpDesc, &tftpMsg, size, &tftpAck, TFTP_ACK, block,
		       &port) == ERROR))
        {
           errno = S_tftpLib_TFTP_ERROR;
           return (ERROR);
        }

	pTftpDesc->serverAddr.sin_port = port;
    }

   {
      do
	{
	/*
	 *  Read data from file - converting to ascii if necessary.
	 *  All data packets, except the last, must have 512 bytes of data.
	 *  The TFTP server sees packets with less than 512 bytes of
	 *  data as end of transfer.
	 */
	if (convert)
        {
            errno = S_tftpLib_INVALID_MODE;
            size = ERROR; /* we don't support ASCII file format */
        }
	else 
    {
	  size = FmRead (fd, tftpMsg.th_data, offset, TFTP_SEGSIZE);
	  if (size == ERROR)
		errno = S_tftpLib_READ_ERROR;
	}
	
	if (size == ERROR) 		/* if error, send message and bail */
	    {
	    size = tftpErrorCreate (&tftpMsg, errno);
	    (void) tftpSend (pTftpDesc, &tftpMsg, size, (TFTP_MSG *) NULL,
			     0, 0, (int *) NULL);
	    return (ERROR);
	    }

        offset += size;

	/* send data message and get an ACK back with same block no. */
	block++;
	tftpMsg.th_opcode = htons ((u_short) TFTP_DATA);
	tftpMsg.th_block  = htons ((u_short) block);

        if (tftpSend (pTftpDesc, &tftpMsg, size + TFTP_DATA_HDR_SIZE,
		      &tftpAck, TFTP_ACK, block, (int *) NULL) == ERROR)
        {
            errno = S_tftpLib_TFTP_ERROR;
	    return (ERROR);
        }

	} while (size == TFTP_SEGSIZE);
    }

    return (OK);
    }

/*******************************************************************************
*
* tftpGet - get a file from a remote system
*
* This routine gets a file from a remote system via TFTP.  <pFilename> is the
* filename.  <fd> is the file descriptor to which the data is written.
* <pTftpDesc> is a pointer to the TFTP descriptor.  The tftpPeerSet() routine
* must be called prior to calling this routine.
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
*  S_tftpLib_INVALID_DESCRIPTOR
*  S_tftpLib_INVALID_ARGUMENT
*  S_tftpLib_NOT_CONNECTED
*/

STATUS tftpGet
    (
    TFTP_DESC *		pTftpDesc,		/* TFTP descriptor       */
    char *		pFilename,		/* remote filename       */
    FD_t                *fd,			/* file descriptor       */
    int			clientOrServer 		/* which side is calling */
    )

    {
    TFTP_MSG		tftpMsg;		/* TFTP message		*/
    TFTP_MSG		tftpReply;		/* TFTP DATA		*/
    int			sizeMsg;		/* size to send		*/
    int			port;			/* return port		*/
    int	*		pPort;			/* port pointer		*/
    int			block;			/* block expected	*/
    char *		pBuffer; 		/* pointer to buffer	*/
    unsigned short *	tmpBuffer; 		/* pointer to buffer	*/
    BOOL		convert;		/* convert from ascii 	*/
    int			numBytes;		/* number of bytes 	*/
    int			sizeReply;		/* number of data bytes */
    char		charTemp;		/* temp char holder 	*/
    STATUS		errorVal = OK;		/* error value 		*/
    int         i;
    U32_t               checksum;
    U32_t               imageChksum;
    FILE_HDR_t          *imageHdr;
    char                version[MAX_VERSION_LENGTH];
    char                imageName[MAX_FILE_NAME_LENGTH];

    if (pTftpDesc == NULL)			/* validate arguments */
	{
	pTftpDesc->errNo = S_tftpLib_INVALID_DESCRIPTOR;
	return (ERROR);
	}

    if ((fd == NULL) || (pFilename == NULL))
	{
	pTftpDesc->errNo = S_tftpLib_INVALID_ARGUMENT;
	return (ERROR);
	}

    if (!pTftpDesc->connected)			/* return if not connected  */
	{
	printf ("tftpGet: No target machine specified.\n");
	pTftpDesc->errNo = S_tftpLib_NOT_CONNECTED;
	return (ERROR);
	}
    						/* initialize variables */
    bzero ((char *) &tftpMsg, sizeof (TFTP_MSG));
    bzero ((char *) &tftpReply, sizeof (TFTP_MSG));
    pPort = &port;
    convert = (strcmp (pTftpDesc->mode, "netascii") == 0) ? TRUE : FALSE;
    charTemp = '\0';
    pTftpDesc->serverAddr.sin_port = pTftpDesc->serverPort;
    block = 1;

    if (tftpVerbose)
	printf ("getting from %s:%s [%s]\n", pTftpDesc->serverName,
		pFilename, pTftpDesc->mode);

    /*
     * If we're a server, then the first message is an ACK with block = 0.
     * If we're a client, then it's an RRQ.
     */

    if (clientOrServer == TFTP_SERVER)
	{
	tftpMsg.th_opcode = htons ((u_short)TFTP_ACK);
	tftpMsg.th_block  = htons ((u_short)(0));
	sizeMsg = TFTP_ACK_SIZE;
	}
    else
	{

	/* formulate a RRQ message */

	sizeMsg = tftpRequestCreate (&tftpMsg, TFTP_RRQ, pFilename,
				     pTftpDesc->mode);
	}

    FOREVER

	{

	/* send the RRQ or ACK - expect back DATA */

        sizeReply = tftpSend (pTftpDesc, &tftpMsg, sizeMsg, &tftpReply,
	                      TFTP_DATA, block, pPort);

	if (sizeReply == ERROR)			/* if error then bail */
        {
            pTftpDesc->errNo = S_tftpLib_TFTP_ERROR;
	    return (ERROR);
        }

	sizeReply -= TFTP_DATA_HDR_SIZE;	/* calculate data size */

	/*
	 *  The first block returns a new server port number (TID) to use.
	 *  From now on, we use this port and tell tftpSend to validate it.
	 */

	if (block == 1)
	    {
	    pTftpDesc->serverAddr.sin_port = port;
	    pPort = NULL;
            if ( (pTftpDesc->fileType == FW_FILE) || 
                 (pTftpDesc->fileType == FLASH_FILE) )
               checksum = 0;
	    }

	/* write data to file, converting to ascii if necessary */

	if (convert)
        {
            pTftpDesc->errNo = S_tftpLib_INVALID_MODE;
            errorVal = ERROR;  /* we don't support ASCII file format */
        }
	else
	    {
	    int	bytesWritten;

	    for (bytesWritten = 0, pBuffer = tftpReply.th_data,
		 errorVal = OK; bytesWritten < sizeReply;
		 bytesWritten += numBytes, pBuffer += numBytes)

		{
if (tftpVerbose)
  printf ("tftpGet: writing %d bytes\n", (sizeReply - bytesWritten));
		   if ((numBytes = FmWrite (fd, pBuffer, 0,
				       (sizeReply - bytesWritten), TRUE)) <= 0)
		   {
		       errorVal = ERROR;
                       pTftpDesc->errNo = S_tftpLib_WRITE_ERROR;
                       if (tftpVerbose)
                          printf ("Byte written = %x size = %d\n", 
                                        numBytes, (sizeReply - bytesWritten));
		       break;
		   }
if (tftpVerbose)
  printf ("tftpGet: wrote %d bytes\n", numBytes);

                   if ( (pTftpDesc->fileType == FW_FILE) ||
                        (pTftpDesc->fileType == FLASH_FILE) )
                   {
                      tmpBuffer = (unsigned short *) pBuffer;
                      if (block == 1)
                      {
                          imageHdr = (FILE_HDR_t *) (pBuffer+16);
                          if (tftpVerbose)
                          {
                             printf ("Length         : %x\n", imageHdr->len);
                             printf ("Offset         : %x\n", imageHdr->offset);
                             printf ("Card type      : %x\n", imageHdr->cardType);
                             printf ("Checksum       : %x\n", imageHdr->ckSum);
                             bzero (version, MAX_VERSION_LENGTH);
                             strncpy (version, imageHdr->version,MAX_VERSION_LENGTH);
                             bzero (imageName, MAX_FILE_NAME_LENGTH);
                             strncpy (imageName, imageHdr->fileName, MAX_FILE_NAME_LENGTH);
                             printf ("Version        : %s\n", version);
                             printf ("Name           : %s\n", imageName);
                          }
                          imageChksum = (U32_t) imageHdr->ckSum;
                          imageHdr->ckSum = 0;
                          if ( ((!strncmp(pFilename,SCC_DIR_PATH,strlen(SCC_DIR_PATH))) && 
                                (imageHdr->cardType != CARD_SCC)) ||
                               ((!strncmp(pFilename,NMC_DIR_PATH,strlen(NMC_DIR_PATH))) &&
                                (imageHdr->cardType != CARD_NMC)) ||
                               ((!strncmp(pFilename,NSC_DIR_PATH,strlen(NSC_DIR_PATH))) &&
                                (imageHdr->cardType != CARD_NSC)) ||
                               ((!strncmp(pFilename,BSC_DIR_PATH,strlen(BSC_DIR_PATH))) &&
                                (imageHdr->cardType != CARD_BSC)) )
                          { 
                             pTftpDesc->errNo = S_tftpLib_IMAGE_MISMATCH;
                             errorVal = ERROR;
                             return (ERROR);
                          }
                      }
                      for (i = 0; i < numBytes; i += 2, tmpBuffer++)
                      {
                          checksum += *tmpBuffer;
                      }
                   }
		}
	    }

	if (errorVal == ERROR)		/* if error send message and bail */
	{
            if (tftpVerbose)
               printf ("tftpGet: Error value[%d]\n", pTftpDesc->errNo);
#if 0
	    sizeMsg = tftpErrorCreate (&tftpMsg, pTftpDesc->errNo);
	    (void) tftpSend (pTftpDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL,
			     0, 0, (int *) NULL);
#endif
	    return (ERROR);
	}

	/* create ACK message */

	tftpMsg.th_opcode = htons ((u_short)TFTP_ACK);
	tftpMsg.th_block  = htons ((u_short)(block));
	sizeMsg = TFTP_ACK_SIZE;

	/*
	 * if last packet received was less than 512 bytes then end
	 * of transfer. Send final ACK, then we're outta here.
	 */

	if (sizeReply < TFTP_SEGSIZE)
	{
            if ( (pTftpDesc->fileType == FW_FILE) ||
                 (pTftpDesc->fileType == FLASH_FILE) )
            {
                while (checksum & 0xffff0000)
                   checksum = (checksum>>16) + (checksum & 0xffff);

                checksum = ~checksum & 0xffff;

                printf ("*** Firmware image (%s) checksum = %x\n", pFilename, checksum);
                if (checksum != imageChksum)
                {
                   if (tftpVerbose)
                      printf ("tftpGet: Checksum error C[%x] I[%x]\n", 
                                        checksum,imageChksum);
                   pTftpDesc->errNo = S_tftpLib_FW_CHKSUM_ERROR;
                   errorVal = ERROR;
                }
            }

            if (errorVal == ERROR)
            {
#if 0
               sizeMsg = tftpErrorCreate (&tftpMsg, pTftpDesc->errNo);
#endif
               pTftpDesc->errNo = errno;
            }
            else
            {
               pTftpDesc->errNo = 0;
               pTftpDesc->block = block;
            }

#if 0 /* moving this to after update standby (tftp_main.c) */
            if (tftpVerbose)
               printf ("Sending: OP[%d] BLK[%d] SZ[%d] ER[%x]\n", 
                        tftpMsg.th_opcode, tftpMsg.th_block, sizeMsg, tftpMsg.th_error);

    	    (void) tftpSend (pTftpDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL,
			     0, 0, (int *) NULL);
#endif
    	    return (errorVal);
	}

	block++;
	}  /* end forever loop */
    }

/*******************************************************************************
*
* tftpInfoShow - get TFTP status information
*
* This routine prints information associated with TFTP descriptor <pTftpDesc>.
*
* EXAMPLE
* A call to tftpInfoShow() might look like:
*
* .CS
*     -> tftpInfoShow (tftpDesc)
*            Connected to yuba [69]
*            Mode: netascii  Verbose: off  Tracing: off
*            Rexmt-interval: 5 seconds, Max-timeout: 25 seconds
*     value = 0 = 0x0
*     ->
* .CE
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
* S_tftpLib_INVALID_DESCRIPTOR
*/

STATUS tftpInfoShow
    (
    TFTP_DESC *		pTftpDesc		/* TFTP descriptor */
    )

    {
    if (pTftpDesc == NULL)
	{
	errno = S_tftpLib_INVALID_DESCRIPTOR;
	return (ERROR);
	}

    if (pTftpDesc->connected == TRUE)
	printf ("\tConnected to %s [%d]\n", pTftpDesc->serverName,
		ntohs (pTftpDesc->serverPort));
    else
	printf ("\tNot connected\n");

    printf ("\tMode: %s  Verbose: %s  Tracing: %s\n", pTftpDesc->mode,
	tftpVerbose ? "on" : "off", tftpTrace ? "on" : "off");
    printf ("\tRexmt-interval: %d seconds, Max-timeout: %d seconds\n",
	tftpReXmit, tftpTimeout);

    return (OK);
    }

/*******************************************************************************
*
* tftpQuit - quit a TFTP session
*
* This routine closes a TFTP session associated with the TFTP descriptor
* <pTftpDesc>.
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
* S_tftpLib_INVALID_DESCRIPTOR
*/

STATUS tftpQuit
    (
    TFTP_DESC *		pTftpDesc			/* TFTP descriptor */
    )

    {
    if (pTftpDesc == NULL)
	{
	errno = S_tftpLib_INVALID_DESCRIPTOR;
	return (ERROR);
	}

    (void) close (pTftpDesc->sock);			/* close up shop */
    free ((char *) pTftpDesc);
    return (OK);
    }

/*******************************************************************************
*
* tftpSend - send a TFTP message to the remote system
*
* This routine sends <sizeMsg> bytes of the passed message <pTftpMsg> to the
* remote system associated with the TFTP descriptor <pTftpDesc>.  If
* <pTftpReply> is not NULL, tftpSend() tries to get a reply message with a
* block number <blockReply> and an opcode <opReply>.  If <pPort> is NULL,
* the reply message must come from the same port to which the message
* was sent.  If <pPort> is not NULL, the port number from which the reply
* message comes is copied to this variable.
*
* INTERNAL
* This routine does the lock step acknowledgement and protocol processing for
* TFTP.  Basically, it sends a message to the server and (waits for and)
* receives the next message.  This is implemented via two main loops.  The
* outer loop sends the message to the TFTP server, the inner loop tries to
* get the next message.  If the reply message does not arrive in a specified
* amount of time, the inner loop is broken out to in order to retransmit
* the send message.
*
* RETURNS: The size of the reply message, or ERROR.
*
* ERRNO
*  S_tftpLib_TIMED_OUT
*  S_tftpLib_TFTP_ERROR
*/

int tftpSend
    (
    TFTP_DESC *		pTftpDesc,		/* TFTP descriptor	*/
    TFTP_MSG *		pTftpMsg,		/* TFTP send message	*/
    int			sizeMsg,		/* send message size  	*/
    TFTP_MSG *		pTftpReply,		/* TFTP reply message	*/
    int			opReply,		/* reply opcode   	*/
    int			blockReply,		/* reply block number	*/
    int *		pPort 			/* return port number	*/
    )

    {
    int 		timeWait = 0;		/* time waited		*/
    int			maxWait;		/* max time to wait	*/
    struct timeval	reXmitTimer;		/* retransmission time	*/
    fd_set 		readFds;		/* select read fds	*/
    struct sockaddr_in  peer;			/* peer			*/
    int 		peerlen; 		/* peer len		*/
    int			num;			/* temp variable	*/
    int			amount;			/* amount in socket 	*/

    /* set up the timeout values */

    bzero ((char *) &reXmitTimer, sizeof (struct timeval));
    maxWait = (tftpTimeout < 0) ? (TFTP_TIMEOUT * 5) : tftpTimeout;
    reXmitTimer.tv_sec = min ((tftpReXmit < 0) ? (TFTP_TIMEOUT) : tftpReXmit,
			      maxWait);

    FOREVER 					/* send loop */
	{
	if (tftpTrace)
	    tftpPacketTrace ("sent", pTftpMsg, sizeMsg);

						/* send message to server */
	if (sendto (pTftpDesc->sock, (caddr_t) pTftpMsg, sizeMsg, 0,
		    (SOCKADDR *) &pTftpDesc->serverAddr,
		     sizeof (struct sockaddr_in)) != sizeMsg)
	    return (ERROR);

	if (pTftpReply == NULL)			/* return if no reply desired */
	    return (0);

	FOREVER 				/* receive loop */
	    {
	    FD_ZERO (&readFds);
	    FD_SET  (pTftpDesc->sock, &readFds);
						/* wait for reply message */

	    if ((num = select (FD_SETSIZE, &readFds, (fd_set *) NULL,
			     (fd_set *) NULL, &reXmitTimer)) == ERROR)
		return (ERROR);

	    if (num == 0) 			/* select timed out */
	        {
	        timeWait += reXmitTimer.tv_sec;
	        if (timeWait >= maxWait)	/* return error, if waited */
						/* for too long 	   */
		    {
		    printf ("Transfer Timed Out.\n");
		    pTftpDesc->errNo = S_tftpLib_TIMED_OUT;
		    return (ERROR);
		    }

		/* break out to send loop in order to retransmit */
		break;
	    	}

	    if (ioctl (pTftpDesc->sock, FIONREAD, (int)&amount) == ERROR)
		return (ERROR);

	    if (amount == 0)			/* woke up but no data	*/
		continue;			/* just go back to sleep*/

	    peerlen = sizeof (struct sockaddr_in);
	    if ((num = recvfrom (pTftpDesc->sock, (caddr_t) pTftpReply,
			         sizeof (TFTP_MSG), 0, (SOCKADDR *) &peer,
			         &peerlen)) == ERROR)
	    	return (ERROR);

	    if (tftpTrace)
		tftpPacketTrace ("received", pTftpReply, num);

	    /* just return if we received an error message */

	    if (ntohs (pTftpReply->th_opcode) == TFTP_ERROR)
		{
		printf ("Error code %d: %s\n", ntohs (pTftpReply->th_error),
			  pTftpReply->th_errMsg);
	    	pTftpDesc->errNo = S_tftpLib_TFTP_ERROR;
	    	return (ERROR);
	    	}
	    /*
	     *  ignore message if it does not have the correct opcode
	     *  or it is not from the port we expect.
	     */

	    if (((pPort == NULL) &&
	         (peer.sin_port != pTftpDesc->serverAddr.sin_port)) ||
	        (opReply != ntohs (pTftpReply->th_opcode)))
	        continue;

	    /* if got right block number, then we got the right packet! */

	    if (ntohs (pTftpReply->th_block) == blockReply)
	        {
	        if (pPort != NULL)
		    *pPort = peer.sin_port;
	        return (num);
		}
	    /*
	     * somehow we didn't get the right block, lets flush out the
	     * the socket, and try resynching if things look salvageable.
	     */

	    FOREVER
		{
	   	 TFTP_MSG	trash;

	    	if (ioctl (pTftpDesc->sock, FIONREAD, (int)&amount) == ERROR)
		    return (ERROR);

	    	if (amount == 0)		/* done - socket cleaned out */
		    break;

	        (void) recvfrom (pTftpDesc->sock, (caddr_t) &trash, sizeof
			   	 (TFTP_MSG), 0, (SOCKADDR *) NULL, (int *) NULL);
	        }

	    /* break out to send loop in order to resynch by reresending */

	    if (ntohs (pTftpReply->th_block) == (blockReply - 1))
	        break;

	    return (ERROR);			/* things look bad - return */
	    }
        }
    }

/*******************************************************************************
*
* tftpRequestCreate - create a TFTP read/write request message
*
* This routine creates a TFTP read/write request message. <pTftpMsg> is the
* message to be filled in.  <opcode> specifies the request (either TFTP_RRQ
* or TFTP_WRQ), <pFilename> specifies the filename and <pMode> specifies the
* mode.  The format of a TFTP read/write request message is:
*
*	2 bytes |   string | 1 byte | string | 1 byte
*	----------------------------------------------
*	OpCode  | filename |   0    | Mode   |   0
*
* RETURNS: size of message
*/

LOCAL int tftpRequestCreate
    (
    TFTP_MSG *		pTftpMsg,		/* TFTP message pointer	*/
    int       		opCode,			/* request opCode 	*/
    char *		pFilename,		/* remote filename 	*/
    char *		pMode 			/* TFTP transfer mode	*/
    )

    {
    FAST char *		cp;			/* character pointer 	*/

    pTftpMsg->th_opcode = htons ((u_short) opCode);

    cp = pTftpMsg->th_request;			/* fill in file name	*/
    (void) strcpy (cp, pFilename);
    cp += strlen (pFilename);
    *cp++ = '\0';

    (void) strcpy (cp, pMode);	    		/* fill in mode		*/
    cp += strlen (pMode);
    *cp++ = '\0';

    return (cp - (char *) pTftpMsg);		/* return size of message */
    }

/*******************************************************************************
*
* tftpErrorCreate - create a TFTP error message
*
* This routine creates a TFTP error message.  <pTftpMsg> is the TFTP message
* to be filled in.  <errorNum> is a TFTP defined error number.  The format of
* a TFTP error message is:
*
*	2 bytes | 2 bytes   | string | 1 byte
*	-------------------------------------
*	OpCode  | ErrorCode | ErrMsg |   0
*
* RETURNS: Size of message.
*
* NOMANUAL
*/

int tftpErrorCreate
    (
    TFTP_MSG *		pTftpMsg,		/* TFTP error message	*/
    int			errorNum 		/* error number 	*/
    )

    {
    TFTP_ERRMSG	*	pError;			/* pointer to error	*/
    LOCAL TFTP_ERRMSG 	tftpErrors [] =		/* TFTP	defined errors	*/
	{
	    { EUNDEF,	        "Undefined error code"		   },
	    { ENOTFOUND,        "Invalid file name" 	           },
	    { EACCESS,	        "Access violation"		   },
	    { ENOSPACE,	        "Disk full or allocation exceeded" },
	    { EBADOP,	        "Illegal TFTP operation"	   },
	    { EBADID,	        "Unknown transfer ID"		   },
	    { EEXISTS,	        "File already exists"		   },
	    { ENOUSER,	        "No such user"			   },
	    { EINVALIDPSWD,	"Invalid user password (key)"	   },
	    { EDISKWRERR,	"Disk access error"		   },
	    { ECHKSUMERR,	"Image checksum error"		   },
	    { EIMGMISMTC,	"Image mismatched"		   },
            { EEXCEEDTASKLIMIT,  "The number of tftp task has reached the system limit."},
	    { EINVALIDIMGFILENAME,      "Invalid Image File Name" },
	    { -1,	NULL				   }
    	};

    if (tftpVerbose)
       printf ("tftpErrorCreate: error number %x\n", errorNum);
    switch (errorNum)
	{
	case S_nfsLib_NFSERR_NOENT:
	case S_dosFsLib_FILE_NOT_FOUND:
	case S_rt11FsLib_FILE_NOT_FOUND:
	case S_netDrv_NO_SUCH_FILE_OR_DIR:
	    errorNum = ENOTFOUND;
	    break;
       
        case S_tftpLib_READ_ERROR:
            errorNum = EACCESS;
            break;

        case S_tftpLib_WRITE_ERROR:
            errorNum = EDISKWRERR;
            break;

        case S_tftpLib_FW_CHKSUM_ERROR:
            errorNum = ECHKSUMERR;
            break;
        
        case S_tftpLib_FORMAT_ERROR:
            errorNum = EFORMATERR;
            break;
        


        case S_tftpLib_EXCEED_NUMOFTASK_LIMIT:
            errorNum = EEXCEEDTASKLIMIT;
            break;

        case S_tftpLib_IMAGE_MISMATCH:
            errorNum = EIMGMISMTC;
            break;

	case S_tftpLib_ILLEGAL_IMG_FILE_NAME:
	    errorNum = EINVALIDIMGFILENAME;
	    break;

	case S_tftpLib_ILLEGAL_WRITE:
	    errorNum = EILLEGALWRITE;
	    break;

	}
						/* locate the passed error */
    for (pError = tftpErrors; pError->e_code >= 0; pError++)
	{
    	if (pError->e_code == errorNum)
	    {
    	    break;
	    }
	}

    if (pError->e_code < 0) 	    		/* if not found, use EUNDEF */
	pError = tftpErrors;
    						/* fill in error message */
    pTftpMsg->th_opcode = htons ((u_short) TFTP_ERROR);
    pTftpMsg->th_error  = htons ((u_short) pError->e_code);
    (void) strcpy (pTftpMsg->th_errMsg, pError->e_msg);

    return (strlen (pError->e_msg) + 5);	/* return size of message */
    }

/*******************************************************************************
*
* tftpPacketTrace - trace a TFTP packet
*
* tftpPacketTrace prints out diagnostic information about a TFTP packet.
* Tracing is enabled when the global variable tftpTrace is TRUE.  <pMsg> is a
* pointer to a diagnostic message that gets printed with each trace.
* <pTftpMsg> is the TFTP packet of <size> bytes.
*
* RETURNS: N/A
*/

LOCAL void tftpPacketTrace
    (
    char *		pMsg,			/* diagnostic message 	*/
    TFTP_MSG *		pTftpMsg,		/* TFTP packet 		*/
    int 		size 			/* packet size 		*/
    )

    {
    u_short		op;			/* message op code 	*/
    char *		cp;			/* temp char pointer 	*/
    LOCAL char *	tftpOpCodes [] =	/* ascii op codes 	*/
	{
	"#0",
	"RRQ", 					/* read request		*/
 	"WRQ",  				/* write request	*/
 	"DATA", 				/* data message		*/
	"ACK", 					/* ack message		*/
	"ERROR"  				/* error message	*/
	};

    op = ntohs (pTftpMsg->th_opcode);

    if (op < TFTP_RRQ || op > TFTP_ERROR) 	/* unknown op code */
	{
	printf ("%s opcode=%x\n", pMsg, op);
	return;
	}
    printf ("{%15s} %s %s ", taskName(0), pMsg, tftpOpCodes [op]);

    switch (op)
	{
	/* for request messages, display filename and mode */

	case TFTP_RRQ:
	case TFTP_WRQ:
	    cp = index (pTftpMsg->th_request, '\0');
	    printf ("<file=%s, mode=%s>\n", pTftpMsg->th_request, cp + 1);
	    break;

	/* for the data messages, display the block number and bytes */

	case TFTP_DATA:
	    printf ("<block=%d, %d bytes>\n", ntohs (pTftpMsg->th_block),
	            size - TFTP_DATA_HDR_SIZE);
	    break;

	/* for the ack messages, display the block number */

	case TFTP_ACK:
	    printf ("<block=%d>\n", ntohs (pTftpMsg->th_block));
	    break;

	/* for error messages display the error code and message */

	case TFTP_ERROR:
	    printf ("<code=%d, msg=%s>\n", ntohs (pTftpMsg->th_error),
	            pTftpMsg->th_errMsg);
	    break;
	}
    }

/*******************************************************************************
*
* connectOverLoopback - connect two stream sockets over the loopback address
*
* This routine creates two stream sockets and connects them over the loopback
* address.  It places the two socket descriptors in <pSock1> and <pSock2>.
*
* RETURNS: OK if successful, otherwise ERROR.
*/

LOCAL STATUS connectOverLoopback
    (
    int	*		pSock1,			/* socket 1 		*/
    int	*		pSock2 			/* socket 2 		*/
    )

    {
    struct sockaddr_in	serverAddr;		/* server address 	*/
    char 		loopAddr [24];		/* loop back address 	*/
    int			serverSock;		/* server socket 	*/
    struct sockaddr_in	clientAddr;		/* client address	*/
    int			addrlen;		/* address length	*/
    int			peerlen;		/* peer address length	*/
    struct timeval	timeOut;		/* connect timeout 	*/

    /* set up server address over loopback. */

    bzero ((char *) &serverAddr, sizeof (struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = ntohs ((u_short) 0);

    if ((ifAddrGet ("lo0", loopAddr) == ERROR) ||
        (((int)(serverAddr.sin_addr.s_addr = inet_addr (loopAddr))) == ERROR) ||
         (serverSock = socket (AF_INET, SOCK_STREAM, 0)) == ERROR)
	return (ERROR);

    if ((*pSock1 = socket (AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
	(void) close (serverSock);
	return (ERROR);
	}

    bzero ((char *) &timeOut, sizeof (struct timeval));
    timeOut.tv_sec = 5;
    timeOut.tv_usec = 0;

    peerlen = addrlen = sizeof (struct sockaddr_in);

    /* make connection */

    if ((bind (serverSock, (SOCKADDR *) &serverAddr,
	       sizeof (struct sockaddr_in)) == ERROR) ||
        (getsockname (serverSock, (SOCKADDR *) &serverAddr,
		     &addrlen) == ERROR) ||
        (listen (serverSock, 5) == ERROR) ||
	(connectWithTimeout (*pSock1, (SOCKADDR *) &serverAddr, sizeof
			     (struct sockaddr_in), &timeOut) == ERROR) ||
        ((*pSock2 = accept (serverSock, (SOCKADDR *) &clientAddr,
			    &peerlen)) == ERROR))
	{
	(void) close (serverSock);
	(void) close (*pSock1);
	return (ERROR);
	}

    (void) close (serverSock);
    return (OK);
    }

#define BASE_NAME_START      0
#define BASE_NAME_STOP       2
#define DASH_LOC             3
#define REVISION_START       4
#define REVISION_STOP        12 
#define FIRST_DOT_LOC        13 
#define SUBSR_START          14 
#define SUBSR_STOP           16
#define SECOND_DOT_LOC       17
#define KEY_START            18
#define KEY_STOP             23 
#define BASE_NAME_SIZE       3
#define KEY_SIZE             6
#define REVISION_SIZE        9
#define SUBSR_SIZE           3
#define EXT_DOT_LOC          3          
#define EXT_SIZE             2          
#define IMG_KEY_START        7          
#define IMG_FILE_SIZE        6 
#define MAX_COUNTRY_NAME_SIZE  14
#define DB_DIR_LEN_1         10
#define DB_DIR_LEN_2         11
#define DB_DIR_PATH_1        "CONFIG/DB/"
#define DB_DIR_PATH_2        "/CONFIG/DB/"
#define CALL_LOG_DIR         "C:/LOG/"
#define CALL_LOG_FILE_NAME   "call.log"

STATUS_t tftpdFilenameParse (U8_t *filename, TFTP_DESC *tftpDesc, int readOrWrite)
{
   U8_t   Key[KEY_SIZE+1];
   U8_t   *tempPtr;
   U8_t   i;
   U8_t   temp_name[MAX_FILE_NAME_LENGTH];
   U8_t   Extension[EXT_SIZE+1]; 
   U8_t   temp_filename[MAX_FILE_NAME_LENGTH];
   U8_t   dbPtr[MAX_FILE_NAME_LENGTH];   

   strncpy (temp_name, filename, MAX_FILE_NAME_LENGTH);
   strncpy (dbPtr, filename, MAX_FILE_NAME_LENGTH);   
   tempPtr = filename;
   for (i = 0; i < strlen(filename); i++)
      filename[i] = (U8_t) tolower ((int) filename[i]);

   if (filename == NULL)
   {
       /* Return error */
       printf ("tftpdFilenameParse: NULL file name \n");
       return (ERROR);
   }
   else
   {
	 bzero (Key, KEY_SIZE+1);
	 bzero (Extension, EXT_SIZE+1);
	 bzero(temp_filename, MAX_FILE_NAME_LENGTH);

	 if (readOrWrite == TFTP_WRQ) {
	   strncpy (Extension, &filename[EXT_DOT_LOC + 1], EXT_SIZE);

	   if (filename[EXT_DOT_LOC] == '.') 
             strncpy (Key, &filename[IMG_KEY_START], KEY_SIZE);
	   else
             Key[0] = '\0';
	   
	   printf ("Extension                   : %s\n", Extension);
	   printf ("Key                         : %s\n", Key);

	   if (!strcmp(Extension, "fw")) {
#ifndef FLASH_BOOT
#if 0
	     if (strncmp ((char *)(&crm.this.cTftpKey), Key, TFTP_KEY_SIZE))
	     {
	       printf ("tftpdFilenameParse: Wrong TFTP key [%s] [%s]\n", 
				   (char *)(&crm.this.cTftpKey), Key);
	       return (NWG_INVALID_PSWD);
	     }
#endif
	     strncpy(temp_filename, filename, IMG_FILE_SIZE);
#endif

#ifdef FLASH_BOOT
	     strcpy(temp_filename, filename);
#endif
	   } else {
	     strcpy(temp_filename, filename);
	   } 
	 } else {
	   strcpy(temp_filename, filename);
	 }
	 printf ("Filename                   : %s\n", temp_filename);

	   sprintf (tftpDesc->localFile, "%snmc.fw", NMC_DIR_PATH);
	   tftpDesc->fileType = FW_FILE;
	   printf ("tftpdFilenameParse: Local file name = %s\n", tftpDesc->localFile);
   }
   return (OK);
}

#define MAX_DIR_PATH_LENGTH  12
#define IMAGE_FILE_LENGTH    17

STATUS_t IsValidImageFilename (U8_t *temp_filename) 
{
   U8_t   BaseName[BASE_NAME_SIZE+1];
   U8_t   Subscript[SUBSR_SIZE+1];
   U8_t   *tempPtr;
   U8_t   temp_name[MAX_FILE_NAME_LENGTH];
   U8_t   dirpath[MAX_DIR_PATH_LENGTH];
   U8_t   dirlen;

   bzero(dirpath, MAX_DIR_PATH_LENGTH);
   bzero(temp_name, MAX_FILE_NAME_LENGTH);
   bzero (BaseName, BASE_NAME_SIZE+1);
   bzero (Subscript, SUBSR_SIZE+1);

   printf("IsValidImageFile(): temp_filename is %s\n", temp_filename);
   tempPtr = strrchr(temp_filename, '/');
   if (tempPtr != NULL) {
     tempPtr++;
     strcpy(temp_name, tempPtr);
     printf("IsValidImageFile(): temp_name is %s\n", temp_name);

     dirlen = strlen(temp_filename) - strlen(tempPtr);
     if (dirlen >= MAX_DIR_PATH_LENGTH) {
       strncpy(dirpath, temp_filename, dirlen - 1);
       dirpath[dirlen - 1] = '\0';
     }
     else 
       strncpy(dirpath, temp_filename, dirlen);
     printf("IsValidImageFile(): dirpath is %s\n", dirpath);
     
     strncpy (BaseName, &tempPtr[BASE_NAME_START], BASE_NAME_SIZE);   
     strncpy (Subscript, &tempPtr[SUBSR_START], SUBSR_SIZE);
     
     printf("IsValidImageFile(): BaseName is %s\n", BaseName);
     printf("IsValidImageFile(): Subscript is %s\n", Subscript);
#if 1
     if ((!(strcmp(temp_name, "scc.fw"))) || 
	 ((! (strcmp(BaseName, "scc") && strcmp(BaseName, "SCC"))) &&
	  (! (strcmp(Subscript, "fls"))) &&
	  (strlen(temp_name) == IMAGE_FILE_LENGTH))) {
       if ((!(strcmp(dirpath, SCC_DIR_PATH))) || 
	   (!(strcmp(dirpath, "/IMAGE/SCC/"))) ||
	   (!(strcmp(dirpath, "IMAGE/SCC/")))) {
	 printf("IsValidImageFile(): Illegal SCC image file \n");
	 PERR("IsValidImageFile(): Invalid image file name.\n");
	 return(ERROR);
       }
     } else if ((!(strcmp(temp_name, "bsc.fw"))) || 
		((! (strcmp(BaseName, "bsc") && strcmp(BaseName, "BSC"))) &&
		 (! (strcmp(Subscript, "fls"))) &&
		 (strlen(temp_name) == IMAGE_FILE_LENGTH))) {
       if ((!(strcmp(dirpath, BSC_DIR_PATH))) || 
	   (!(strcmp(dirpath, "/IMAGE/BSC/"))) ||
	   (!(strcmp(dirpath, "IMAGE/BSC/")))) {
	 printf("IsValidImageFile(): Illegal BSC image file \n");
	 PERR("IsValidImageFile(): Invalid image file name.\n");
	 return(ERROR);
       }
     } else if ((!(strcmp(temp_name, "nsc.fw"))) || 
		((! (strcmp(BaseName, "nsc") && strcmp(BaseName, "NSC"))) &&
		 (! (strcmp(Subscript, "fls"))) &&
		 (strlen(temp_name) == IMAGE_FILE_LENGTH))) {
       if ((!(strcmp(dirpath, NSC_DIR_PATH))) || 
	   (!(strcmp(dirpath, "/IMAGE/NSC/"))) ||
	   (!(strcmp(dirpath, "IMAGE/NSC/")))) {
	 printf("IsValidImageFile(): Illegal NSC image file \n");
	 PERR("IsValidImageFile(): Invalid image file name.\n");
	 return(ERROR);
       }
     } 
#endif
     return(OK);
   } else {
     return(OK);
   }
}

#define MAX_TOKEN_LEN     20
int bufIndex;



