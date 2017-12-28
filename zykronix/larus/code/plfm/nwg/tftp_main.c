/*
/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *  Abstract:
 *
 *  File Name: tftp_main.c
 *
 *  Description: 
 *  This module implements the TFTP service for MMS-1600.  
 *
 *Author(s):
 */
 /********************************************************************
 * $Id: tftp_main.c,v 1.2 2006/10/03 04:27:17 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/nwg/tftp_main.c,v $
 *====================================================================
 * $Log: tftp_main.c,v $
 * Revision 1.2  2006/10/03 04:27:17  cvsshuming
 * *** empty log message ***
 *

 * $Endlog $
 *********************************************************************/
#include "vxWorks.h"
#include "netinet/in.h"
#include "sockLib.h"
#include "sys/socket.h"
#include "errnoLib.h"
#include "fcntl.h"
#include "ioLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "unistd.h"
#include "usrLib.h"
#include "iosLib.h"
#include "msgQLib.h"
#include "semLib.h"
#include "inetLib.h"

#include "tss_types.h"
#include "tss_if.h"
#include "sys_if.h"
#include "siw_if.h"
#include "nwg.h"
#include "nwg_if.h"
#include "tftpdLib.h"
#include "tftpLib.h"
#include "nwg_events.h"


/* EXTERNALS */
extern int             sysClkRateGet (void);
extern STATUS_t        tftpdFilenameParse (U8_t *, TFTP_DESC *, int);
extern S8_t            GetCardState (S32_t);
extern U8_t            IpServiceAvail;
extern BOOL tftpVerbose;
extern U32_t GetCardType (S32_t slot);

/* GLOBALS */

BOOL tftpdDebug			= FALSE;	/* TRUE: debugging messages */
int tftpdTaskPriority		= 55;
int tftpdTaskStackSize		= 12000;
int tftpdTaskId			= NONE;
int tftpdErrorSendTries		= 3;
int tftpdMaxConnections		= 10;
char *tftpdDirectoryDefault	= "/tftpboot";
int tftpdResponsePriority	= 68;
int maxConnections              = 10; /* max number of simultan. connects */

S32_t nTftpTasks                = 0;
SIW_SEMID_t                     tftpTaskMutex;
SIW_SEMID_t                     tftpTaskMutex;

FUNCPTR tftpdNameMunge = NULL;

/* LOCALS */

LOCAL SEM_ID	tftpdDirectorySem;	/* protection for the semaphore list */
LOCAL LIST    	tftpdDirectoryList;	/* access list, elements  TFTPD_DIR  */
LOCAL MSG_Q_ID	tftpdDescriptorQueue;	/* msg queue of available connection */
LOCAL TFTP_DESC *tftpdDescriptorPool;

/* PROTOTYPES */

static STATUS tftpdRequestVerify (TFTP_DESC *pReplyDesc, int opCode,
				  char *fileName);
static STATUS tftpdRequestDecode (TFTP_MSG *pTftpMsg, int *opCode,
				  char *fileName, char *mode);
static STATUS tftpdFileRead (char *fileName, TFTP_DESC *pReplyDesc);
static STATUS tftpdFileWrite (char *fileName, TFTP_DESC *pReplyDesc);
static STATUS tftpdDescriptorQueueInit (int nEntries);
static STATUS tftpdDescriptorQueueDelete (void);
static TFTP_DESC *tftpdDescriptorCreate (char *mode, BOOL connected,
					 int sock, u_short clientPort,
					 struct sockaddr_in *pClientAddr);
static STATUS tftpdDescriptorDelete (TFTP_DESC *descriptor);
#if 0 /* not currently being used */
  static STATUS tftpdDirectoryValidate (char *fileName);
#endif
static STATUS tftpdErrorSend (TFTP_DESC *pReplyDesc, int errorNum);


/******************************************************************************
*
* tftpdTask - TFTP server daemon task
*
* This routine processes incoming TFTP client requests by spawning a new
* task for each connection that is set up.
*
* This routine is called by tftpdInit().
*
* RETURNS:
* OK, or ERROR if the task returns unexpectedly.
*/

STATUS tftpdTask ()
{
    int			serverSocket;	/* socket to use to communicate with
					 * the remote process */
    struct sockaddr_in	clientAddr;	/* process requesting TFTP
					 * connection */
    struct sockaddr_in	serverAddr;
    int			clientAddrLength = sizeof (struct sockaddr_in);
    TFTP_MSG 		requestBuffer;
    int			value;
    int			opCode;
    char		fileName [TFTP_SEGSIZE];
    char		mode [TFTP_SEGSIZE];
    TFTP_DESC		*pReplyDesc;
    int			replySocket;
    int                 initFail = 0;
    SIW_PRIORITY_t      pri;
    SIW_TASKINFO_t      info;
    STATUS_t            status;
    SIW_TID_t           tid = SIW_INVALID_TID;
    STATUS              stat;
    TFTP_MSG            tftpMsg;
    int                 sizeMsg;


    SiwTaskInitAck (OK);

#ifndef FLASH_BOOT
    SiwWaitCardActive (SiwMySlotNo());
#else
    while (!IpServiceAvail)
      taskDelay (100);
#endif

    serverSocket = socket (AF_INET, SOCK_DGRAM, 0);

    bzero ((char *) &serverAddr, sizeof (struct sockaddr_in));
    bzero ((char *) &clientAddr, sizeof (struct sockaddr_in));

    serverAddr.sin_family	= AF_INET;
    serverAddr.sin_port		= htons((u_short) TFTP_PORT);


    serverAddr.sin_addr.s_addr	= INADDR_ANY;

    while (bind (serverSocket, (SOCKADDR *) &serverAddr,
	      sizeof (struct sockaddr_in)) == ERROR)
    {
        if (!initFail)
        {
           SiwEventLog (NULL, SIW_EVENT_SYM(NWG, SCKBNDFAIL));
           initFail = 1;
        }
        taskDelay (100);
    }

    initFail = 0;
    while (tftpdDescriptorQueueInit (maxConnections) == ERROR)
    {
        if (!initFail)
        {
           initFail = 1;
           SiwEventLog (NULL, SIW_EVENT_SYM(NWG, DSCRINITFAIL));
        }
        taskDelay (100);
    }
     
     /* initialize the semaphores: tftpReadTaskMutex 
      * and tftpWriteTaskMutex
      */
    tftpTaskMutex = SiwSemCreate("tftpReadTaskMutex:tftp",
                                    SIW_SEM_BINARY,
                                    SIW_SEM_Q_PRIORITY,
                                    SIW_SEM_FULL,
                                    SIW_RECV_NO_ACTION);

    /*
     * Clean out any outstanding data on the TFTP port.
     */

    FOREVER
	{
        initFail = 0;
	while (ioctl (serverSocket, FIONREAD, (int) &value) == ERROR)
        {
            if (!initFail)
            {
               initFail = 1;
               SiwEventLog (NULL, SIW_EVENT_SYM(NWG, IOCTLRDFAIL));
            }
            taskDelay (100);
        }

	if (value == 0)                /* done - socket cleaned out */
	    break;

	recvfrom (serverSocket, (caddr_t) &requestBuffer,
		  sizeof (TFTP_MSG), 0, (SOCKADDR *) NULL,
		  (int *) NULL);
	}

    /*
     * The main loop.  Receive requests on the TFTP port, parse the request,
     * and spawn tasks to handle it.
     */

    FOREVER
	{

	/*
	 * Read a message from the TFTP port
	 */

	value = recvfrom (serverSocket, (char *) &requestBuffer, TFTP_SEGSIZE,
			  0, (struct sockaddr *) &clientAddr,
			  &clientAddrLength);

	/*
	 * If there's an error reading on the port, abort the server.
	 */

	if (value == ERROR)
	    {
	    printf ("TFTP:  could not read on TFTP port\n");
	    close (serverSocket);
	    tftpdDescriptorQueueDelete ();
	    break;
	    }

	/*
	 * Set up a socket to use for a reply, and get a port number for it.
	 */

	replySocket = socket (AF_INET, SOCK_DGRAM, 0);
	if (replySocket == ERROR)
	    {

	    /*
	     * XXX How should we deal with an error here?
	     */

	    continue;
	    }

	serverAddr.sin_port = htons((u_short) 0);
	if (bind (replySocket, (SOCKADDR *) &serverAddr,
		  sizeof (struct sockaddr_in)) == ERROR)
	    {

	    /*
	     * XXX How should we deal with an error here?
	     */

	    continue;
	    }

	if (tftpdRequestDecode (&requestBuffer, &opCode, (char *) fileName,
				(char *) mode) == ERROR)
	    {

	    /*
	     * We received something that doesn't look like a TFTP request.
	     * Ignore it.
	     */

	    close (replySocket);
	    continue;
	    }

	/*
	 * Get a reply descriptor.  This will pend until one is available.
	 */

	pReplyDesc = tftpdDescriptorCreate (mode, TRUE, replySocket,
					    clientAddr.sin_port, &clientAddr);
	if (pReplyDesc == NULL)
	    {

	    /*
	     * Couldn't create a reply descriptor.
	     */

	    close (replySocket);
	    continue;
	    }


	if (tftpdRequestVerify (pReplyDesc, opCode, fileName) != OK)
	    {

	    /*
	     * Invalid request, error packet already sent by tftpdRequestVerify
	     */

	    tftpdDescriptorDelete (pReplyDesc);
	    close (replySocket);
	    continue;
	    }

	if (tftpdDebug)
	    {
	    printf ("TFTP: Request: Opcode = %d, file = %s, client = %s\n",
		    opCode, fileName, pReplyDesc->serverName);
	    }

	switch (opCode)
	    {
	    case TFTP_RRQ:

		/*
		 * We've received a read request.
                 * Before spawn a task, check whether the number
                 * of tftp tasks existed in the system has reached 
                 * the limit.
                 */
                if ((stat = SiwSemTake (tftpTaskMutex, WAIT_FOREVER)) == OK) {
                    if (nTftpTasks < MAX_NUMBER_OF_TFTP_TASK) {
                        nTftpTasks++;
                        SiwSemGive(tftpTaskMutex);
                    }
                    else {
                        SiwSemGive(tftpTaskMutex);
                        printf("TFTPMain(): can not spawn more than %d tasks.\n",
                                           nTftpTasks); 
                        pReplyDesc->errNo = S_tftpLib_EXCEED_NUMOFTASK_LIMIT;
                        sizeMsg = tftpErrorCreate (&tftpMsg, pReplyDesc->errNo);
                        (void) tftpSend (pReplyDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL,
                                        0, 0, (int *) NULL);


                        break;
                    }
                } else {
                    /* Error from SiwSemTake() */
                    printf("TFTPMain(): Could not take read task mutex semaphore %x, %x", stat, errno);
                    pReplyDesc->errNo = S_tftpLib_TFTP_ERROR;
                    sizeMsg = tftpErrorCreate (&tftpMsg, pReplyDesc->errNo);
                    (void) tftpSend (pReplyDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL,
                                    0, 0, (int *) NULL);
                    break;
                }

                /* Spawn a tftpdFileRead task to process 
                 * the request.
                 */
	        taskSpawn ("tTftpRRQ", tftpdResponsePriority, 0,
			   10000, tftpdFileRead, (int) fileName,
			   (int) pReplyDesc, 0, 0, 0, 0, 0, 0, 0, 0);

		break;

	    case TFTP_WRQ:

		/*
		 * We've received a write request.  
                 * Before spawn a task, check whether the number
                 * of tasks existed in the system has reached 
                 * the limit.
                 */
                if ((stat = SiwSemTake (tftpTaskMutex, WAIT_FOREVER)) == OK) {
                    if (nTftpTasks < MAX_NUMBER_OF_TFTP_TASK) {
                        nTftpTasks++;
                        SiwSemGive(tftpTaskMutex);
                    }
                    else { 
                        SiwSemGive(tftpTaskMutex);
                        printf("TFTPMain(): can not spawn more than %d tasks.\n",
                                           nTftpTasks); 
                        pReplyDesc->errNo = S_tftpLib_EXCEED_NUMOFTASK_LIMIT;
                        sizeMsg = tftpErrorCreate (&tftpMsg, pReplyDesc->errNo);
                        (void) tftpSend (pReplyDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL,
                                        0, 0, (int *) NULL);
                        break;
                    }
                } else {
                    /* Error from SiwSemTake() */
                    printf("TFTPMain(): Could not take write task mutex semaphore %x", stat );
                    pReplyDesc->errNo = S_tftpLib_TFTP_ERROR;
                    sizeMsg = tftpErrorCreate (&tftpMsg, pReplyDesc->errNo);
                    (void) tftpSend (pReplyDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL,
                                    0, 0, (int *) NULL);
                    break;

                }

                /* Spawn a tftpdFileRead task to process 
                 * the request.
                 */
#if 1
                pri.applId     = SIW_PLATFORM_SW;
                pri.prioGroup  = SIW_PRI_GRP_NORMAL;
                pri.priority   = WEB_TASK_PRI;
                info.entryPt = (FUNC_PTR_t)tftpdFileWrite;
                info.stackSize = SIW_LARGE_STACK;
                info.arg1 = (int) fileName;
                info.arg2 = (int) pReplyDesc;
                info.waitAck = 0;

                /* this task will be deleted by itself */
                tid = SiwTaskSpawn("tTftpWRQ", &pri, &info,
                                    SIW_RECV_LOG_ERROR,
                                    SIW_RECV_LOG_ERROR,
                                    NULL,
                                    &status);

#else
	        taskSpawn ("tTftpWRQ", tftpdResponsePriority, 0,
			   10000, tftpdFileWrite, (int) fileName,
			   (int) pReplyDesc, 0, 0, 0, 0, 0, 0, 0, 0);
#endif
		break;
	    }
	} /* end FOREVER */

    printf ("TFTP:  aborting TFTP server\n");
    tftpdDescriptorQueueDelete ();
    close (serverSocket);
    return (ERROR);
}

/******************************************************************************
*
* tftpdRequestVerify - ensure that an incoming TFTP request is valid
*
* Checks a TFTP request to make sure that the opcode is either
* a read or write request, and then checks to see if the file requested
* is in the access list.
*
* If there is an error, it sends an error packet to the offending client.
*
* RETURNS: OK, or ERROR if any of the conditions aren't met.
*/

LOCAL STATUS tftpdRequestVerify
    (
    TFTP_DESC	*pReplyDesc,
    int		opCode,
    char	*fileName
    )
    {

    /*
     * Need to check two things:
     *
     * 1.  The request itself needs to be valid, either a write request (WRQ)
     *     or a read request (RRQ).
     *
     * 2.  It needs to be to a valid directory.
     */

    if ((opCode != TFTP_RRQ) && (opCode != TFTP_WRQ))
	{

	/*
	 * Bad opCode sent to the server.
	 */

	tftpdErrorSend (pReplyDesc, EBADOP);
	return (ERROR);
	}

#if 0
    dirIsValid = tftpdDirectoryValidate (fileName);
    if (dirIsValid != OK)
	{

	/*
	 * Access was denied to the file that the client
	 * requested.
	 */

	tftpdErrorSend (pReplyDesc, errnoGet());
	return (ERROR);
	}
#endif

    return (OK);
    }

/******************************************************************************
*
* tftpdRequestDecode - break down a TFTP request
*
* Given a pointer to a TFTP message, this routine decodes the message
* and returns the message's opcode, file name, and mode.
*
* RETURNS:
* OK or ERROR.
*/


LOCAL STATUS tftpdRequestDecode
    (
    TFTP_MSG	*pTftpMsg,
    int	*	opCode,		/* pointer to the opCode to return */
    char	*fileName,	/* where to return filename */
    char	*mode		/* where to return mode */
    )
    {
    char	*strIndex;	/* index into pTftpMsg to get mode string */

    if (pTftpMsg == NULL)
	return (ERROR);

    if (opCode != NULL)
	*opCode = pTftpMsg->th_opcode;

    if (fileName != NULL)
	{
	strncpy (fileName, pTftpMsg->th.request, 128);
	fileName [127] = EOS;
	}

    if (mode != NULL)
	{

	/*
	 * Need to get the next string in the struct. Use the for loop to
	 * find the end of the first string.
	 */

	for (strIndex = pTftpMsg->th.request;
	     *strIndex != EOS;
	     strIndex++)
	    ;

	strncpy(mode, ++strIndex, 32);
	mode [31] = EOS;
	}

    return (OK);
    }

/******************************************************************************
*
* tftpdFileRead - handle a read request
*
* This routine constructs and executes the tftpPut() command to put the file
* to the remote system.  Normally this routine is the entry point for a task
* created by tftpdTask() in response to a read request.
*
* RETURNS: OK, or ERROR if the file requested could not be opened.
*/

LOCAL STATUS tftpdFileRead
    (
    char	*fileName,		/* file to be sent */
    TFTP_DESC	*pReplyDesc 	/* where to send the file */
    )
    {
    FD_t        *requestFd;
    int		returnValue = OK;
    STATUS_t    ret;
    STATUS_t    stat;
    /*
     * XXX - X windows needs files that don't work with DOS - they have
     * more than three chars after a dot (as in fonts.alias).  If the
     * funcpointer is non-null, call the routine with a pointer to
     * filename, and the called routine should change the name to something
     * DOS understands.
     */

    if ((ret=tftpdFilenameParse ((U8_t *)fileName, pReplyDesc, TFTP_RRQ)) != OK)
    {
       if (tftpVerbose)
          printf ("Error parsing file name \n");

       if (ret == NWG_INVALID_PSWD)
          tftpdErrorSend (pReplyDesc, EINVALIDPSWD);
       else if (ret == NWG_ILLEGAL_FILE_NAME) 
		 tftpdErrorSend (pReplyDesc, EINVALIDIMGFILENAME);
       else
		 tftpdErrorSend (pReplyDesc, ENOTFOUND);
       returnValue = ERROR;
    }
      else {
		requestFd = FmOpen (pReplyDesc->localFile, O_RDONLY);

		if (requestFd == NULL)
		  {
			printf ("tftpdFileWrite: Cannot open file %s \n", pReplyDesc->localFile);
			SiwEventLog (NULL, SIW_EVENT_SYM(NWG, FOPENFAIL), pReplyDesc->localFile);
			tftpdErrorSend (pReplyDesc, ENOTFOUND);
			returnValue = ERROR;
		  }
		else
		  {		
			/*
			 * We call tftpPut from the server on a read request because the
			 * server is putting the file to the client
			 */
#if 0
			/* not the right place to do tone plan stuff -- only OPEN file here */
			
#endif			
			returnValue = tftpPut (pReplyDesc, pReplyDesc->localFile, requestFd, NULL, TFTP_SERVER);
			FmClose (requestFd);
		  }
   }
	
    /*
     * Close the socket, and delete the
     * tftpd descriptor.
     */

    if (returnValue == ERROR)
	{
	printf ("TFTP:  could not send client file \"%s\"\n",
		  fileName);
	}
    
    if ((stat = SiwSemTake (tftpTaskMutex, WAIT_FOREVER)) == OK) {
        if (--nTftpTasks < 0)
            nTftpTasks = 0;
        SiwSemGive(tftpTaskMutex);
    } else {
        /* Error from SiwSemTake() */
        printf("tftpdFileRead: fails to take the semphore.\n");
        returnValue = ERROR;
    }

    tftpdDescriptorDelete (pReplyDesc);


    return (returnValue);
    }


/******************************************************************************
*
* tftpdFileWrite - handle a write request
*
* This routine constructs and executes the tftpGet() command to get the file
* from the remote system.  Normally this routine is the entry point for a
* task created by tftpdTask() in response to a write request.
*
* RETURNS: OK, or ERROR if the file requested could not be opened.
*/

LOCAL STATUS tftpdFileWrite
    (
    char	*fileName,		/* file to be sent */
    TFTP_DESC	*pReplyDesc 	/* where to send the file */
    )
    {
    FD_t        *requestFd;
    int		returnValue = OK;
    U8_t        tmpFile[256];
    STATUS_t    ret;
    TFTP_MSG    tftpMsg;
    int         sizeMsg;
    STATUS      stat;

    if ((ret=tftpdFilenameParse ((U8_t *)fileName, pReplyDesc, TFTP_WRQ)) != OK)
    {
       if (tftpVerbose)
          printf ("Error parsing file name \n");
    
       if (ret == NWG_INVALID_PSWD)
          pReplyDesc->errNo =  EINVALIDPSWD;


       else if (ret == NWG_ILLEGAL_FILE_NAME)
		 pReplyDesc->errNo = S_tftpLib_ILLEGAL_IMG_FILE_NAME ;


       else
          pReplyDesc->errNo = ENOTFOUND;
       returnValue = ERROR;
    }
    else
    {
	  sprintf (tmpFile, "%s.tmp", pReplyDesc->localFile);
	  requestFd = FmOpen (tmpFile, O_RDWR | O_CREAT);
	  if (tftpVerbose)
              printf ("tftpFileWrite: fd[%x]\n", (U32_t)requestFd);
	
	  if ( (requestFd == NULL) || (returnValue == ERROR) )
	    {
	  	printf ("tftpdFileWrite: Cannot open file %s \n", pReplyDesc->localFile);
	  	SiwEventLog (NULL, SIW_EVENT_SYM(NWG, FOPENFAIL), pReplyDesc->localFile);
	  	pReplyDesc->errNo = ENOTFOUND;
	  	returnValue = ERROR;
	    }
	  else
	    {
	    /*
	     * We call tftpGet from the server on a read request because the
	     * server is putting the file to the client
	     */
	    printf ("tftpdFileWrite: now get the file\n");
	    
	    returnValue = tftpGet (pReplyDesc, tmpFile, requestFd,
				   TFTP_SERVER);
	    
	    FmClose (requestFd);
	    
	    if (returnValue == OK) 
	      {
				FmRename (tmpFile, pReplyDesc->localFile);
	      } /* returnValue == OK */
	    else
	      {
			errno = pReplyDesc->errNo; 
			printf ("TFTP: Failed to get file %s\n", pReplyDesc->localFile);
			FmDelete (tmpFile);
	      }
              }
      }

    /*
     * Close the socket, and delete the
     * tftpd descriptor.
     */
    if (returnValue == ERROR)
    {
        printf ("TFTP:  could not send \"%s\" to client\n", fileName);
        sizeMsg = tftpErrorCreate (&tftpMsg, pReplyDesc->errNo);
        (void) tftpSend (pReplyDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL,
                         0, 0, (int *) NULL);
    }
    else
    {
        tftpMsg.th_opcode = htons ((u_short)TFTP_ACK);
        tftpMsg.th_block  = htons ((u_short)pReplyDesc->block);
        sizeMsg = TFTP_ACK_SIZE;
        (void) tftpSend (pReplyDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL,
                         0, 0, (int *) NULL);
        SiwEventLog (NULL, SIW_EVENT_SYM(NWG, FDWNLOAD), pReplyDesc->localFile);
    }        

    tftpdDescriptorDelete (pReplyDesc);

    if ((stat = SiwSemTake (tftpTaskMutex, WAIT_FOREVER)) == OK) {
        if (--nTftpTasks < 0)
            nTftpTasks = 0;
        SiwSemGive(tftpTaskMutex);
    } else {
        /* Error from SiwSemTake() */
        printf("tftpdFileWrite fails to take the semphore.\n");
        returnValue = ERROR;
    }

    SiwTaskDelete (SIW_TID_SELF);
    exit (returnValue);
	return (returnValue);
    }

/******************************************************************************
*
* tftpdDescriptorQueueInit - set up pool of available descriptors
*
* This routine sets up the system for managing a pool of available reply
* descriptors.  A piece of contiguous memory, large enough to hold an
* array of <nEntries> descriptors, is allocated (with malloc()).
* Pointers to each element in the array are written into a message
* queue.  The routine tftpdDescriptorCreate() reads pointers out of this
* queue, and tftpdDescriptorDelete() writes them back in when the
* descriptor space is no longer needed.
*
* RETURNS: OK, or ERROR if either out of memory or the queue could not
* be allocated.
*/

LOCAL STATUS tftpdDescriptorQueueInit
    (
    int	nEntries	/* maximum number of descriptors in
			 * use at any time */
    )
    {
    TFTP_DESC *theAddress;

    /*
     * If nEntries is 0, set it to the default number of connections
     */

    if (nEntries == 0)
        nEntries = tftpdMaxConnections;

    /*
     * Create the message queue
     */

    if ((tftpdDescriptorQueue = (MSG_Q_ID) msgQCreate (nEntries,
						       sizeof (TFTP_DESC *),
						       MSG_Q_FIFO)) == NULL)
        {
	return (ERROR);
	}

    /*
     * Allocate space for the descriptors.
     */

    tftpdDescriptorPool = (TFTP_DESC *) malloc (sizeof (TFTP_DESC) * nEntries);
    if (tftpdDescriptorPool == NULL)
	{
	msgQDelete (tftpdDescriptorQueue);
        return (ERROR);
	}

    while (nEntries--)
	{
	theAddress = &(tftpdDescriptorPool [nEntries]);
	msgQSend (tftpdDescriptorQueue, (char *) &theAddress,
		  sizeof (TFTP_DESC *), WAIT_FOREVER, MSG_PRI_NORMAL);
	}

    return (OK);
    }

/******************************************************************************
*
* tftpdDescriptorQueueDelete - delete pool of available descriptors
*
* This routine deletes the memory and message queues allocated by
* tftpdDescriptorQueueInit.
*
* RETURNS: OK, or ERROR if either memory could not be freed or if the
* message queue could not be deleted.
*/

LOCAL STATUS tftpdDescriptorQueueDelete
    (
    void
    )
    {
    STATUS returnValue = OK;

    if (msgQDelete (tftpdDescriptorQueue) == ERROR)
	{
	printf ("TFTP:  could not delete message queue\n");
	returnValue = ERROR;
	}

    /*
     * free() is now void according to ANSI, so can't check the return value
     */

    free (tftpdDescriptorPool);

    return (returnValue);
    }

/******************************************************************************
*
* tftpdDescriptorCreate - build a tftp descriptor to use with tftpLib
*
* The routines in tftpLib, tftpPut() and tftpGet() in particular, expect to
* be passed a pointer to a struct of type TFTP_DESC that contains the
* information about the connection to the host.  This is a convenience
* routine to allocate space for a TFTP_DESC struct, fill in the elements,
* and return a pointer to it.
*
* This routine pends until a descriptor is available.
*
* RETURNS:
* A pointer to a newly allocated TFTP_DESC struct, or NULL on failure.
*/

LOCAL TFTP_DESC *tftpdDescriptorCreate
    (
    char	*mode,			/* mode 		*/
    BOOL	connected,		/* state		*/
    int		sock,			/* socket number	*/
    u_short	clientPort,		/* client port number	*/
    struct sockaddr_in *pClientAddr 	/* client address	*/
    )
    {
    TFTP_DESC	*pTftpDesc;		/* pointer to the struct to return */
    char	clientName [INET_ADDR_LEN];

    if (msgQReceive (tftpdDescriptorQueue, (char *) &pTftpDesc,
		     sizeof (pTftpDesc), WAIT_FOREVER) == ERROR)
	{

	/*
	 * Couldn't get a descriptor from the queue, return an error
	 */

	return (NULL);
	}

    /*
     * Copy the arguments into the appropriate elements of the struct
     */

    strncpy (pTftpDesc->mode, mode, 31);
    pTftpDesc->mode [31] = EOS;
    pTftpDesc->connected = connected;

    /*
     * clientName and serverName are reversed, because the routines
     * that use this descriptor are expecting to look at the universe
     * from the client side.
     */


    inet_ntoa_b (pClientAddr->sin_addr, clientName);

    strncpy (pTftpDesc->serverName, clientName, 128);
    pTftpDesc->serverName [127] = EOS;

    bcopy ((char *) pClientAddr, (char *) &pTftpDesc->serverAddr,
	   sizeof (struct sockaddr_in));
    pTftpDesc->sock = sock;
    pTftpDesc->serverPort = clientPort;

    /*
     * We've filled the struct, now return a pointer to it
     */

    return (pTftpDesc);
    }

/******************************************************************************
*
* tftpdDescriptorDelete - delete a reply descriptor
*
* This routine returns the space for a reply descriptor back to the
* descriptor pool.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS tftpdDescriptorDelete
    (
    TFTP_DESC *descriptor
    )
    {

    close (descriptor->sock);

    return (msgQSend (tftpdDescriptorQueue, (char *) &descriptor,
		      sizeof (TFTP_DESC *), WAIT_FOREVER, MSG_PRI_NORMAL));
    }

/******************************************************************************
*
* tftpdDirectoryAdd - add a directory to the access list
*
* This routine adds the specified directory name to the access list 
* for the TFTP server.
*
* RETURNS: N/A
*/

STATUS tftpdDirectoryAdd
    (
    char	*fileName	/* name of directory to add to access list */
    )
    {
    TFTPD_DIR	*newNode;

    /*
     * Allocate memory for the node
     */

    newNode = (TFTPD_DIR *) malloc (sizeof (TFTPD_DIR));
    if (newNode == NULL)
	{
	return (ERROR);
	}

    /*
     * Copy the filename into the node
     */

    strncpy (newNode->dirName, fileName, MAX_FILENAME_LENGTH);
    newNode->dirName [MAX_FILENAME_LENGTH - 1] = EOS;

    /*
     * Add the node to the list
     */

    semTake (tftpdDirectorySem, WAIT_FOREVER);
    lstAdd (&tftpdDirectoryList, (NODE *) newNode);
    semGive (tftpdDirectorySem);

    return (OK);
    }

/******************************************************************************
*
* tftpdDirectoryRemove - delete a directory from the access list
*
* This routine deletes the specified directory name from the access list 
* for the TFTP server.
*
* RETURNS: N/A
*/

STATUS tftpdDirectoryRemove
    (
    char	*fileName	/* name of directory to add to access list */
    )
    {
    TFTPD_DIR	*dirNode;

    for (dirNode = (TFTPD_DIR *) lstFirst (&tftpdDirectoryList);
	 dirNode != NULL;
	 dirNode = (TFTPD_DIR *) lstNext ((NODE *) dirNode))
	{

	/*
	 * if the name of the file matches the name in the current
	 * element, delete the element
	 */

	if (strcmp (dirNode->dirName, fileName))
	    {
	    semTake (tftpdDirectorySem, WAIT_FOREVER);
	    lstDelete (&tftpdDirectoryList, (NODE *) dirNode);
	    semGive (tftpdDirectorySem);

	    /*
	     * Also need to free the memory
	     */

	    free (dirNode);

	    return (OK);
	    }
	}

    /*
     * If we got to this point, then there was no match.  Return error.
     */

    return (ERROR);
    }

#if 0   /* Function currently is not being used */
/******************************************************************************
*
* tftpdDirectoryValidate - confirm that file requested is in valid directory
*
* This routine checks that the file requested is in a directory that
* matches an entry in the directory access list.  The validation
* procedure is:
*
*     1.  If the requested file matches the directory exactly,
*         deny access.  This prevents opening devices rather
* 	  than files.
*
*     2.  If the directory and the first part of the file
*         are equal, permission is granted.
*
*     Examples:
*
* 		File		Directory	Result
* 		/mem		/mem		rejected by #1
* 		/mem/foo	/mem		granted
* 		/stuff/foo	/mem		rejected by #2
* 						(first four chars of file
* 						don't match "/mem")
*
* RETURNS: OK, or ERROR if access is not allowed.
*/

LOCAL STATUS tftpdDirectoryValidate
    (
    char *fileName
    )
    {
    DEV_HDR	*deviceHdr;
    char 	fullPathName [MAX_FILENAME_LENGTH];
    char 	tmpString [MAX_FILENAME_LENGTH];
    TFTPD_DIR	*dirNode;
    STATUS 	returnValue = ERROR;

    /*
     * Use ioFullFileNameGet to get the complete name, including
     * the device, of the requested file.
     */

    ioFullFileNameGet (fileName, &deviceHdr, fullPathName);
    strcpy (tmpString, deviceHdr->name);
    strcat (tmpString, fullPathName);
    strcpy (fullPathName, tmpString);

    /*
     * If the access list is empty, there are no restrictions.  Return OK.
     */

    if (lstCount (&tftpdDirectoryList) == 0)
	return (OK);

    semTake (tftpdDirectorySem, WAIT_FOREVER);

    for (dirNode = (TFTPD_DIR *) lstFirst (&tftpdDirectoryList);
	 dirNode != NULL;
	 dirNode = (TFTPD_DIR *) lstNext ((NODE *) dirNode))
	{

	/*
	 * If the filename is exactly the same as the directory, break the loop
	 * and return an error (rejected by #1)
	 */

	if (strcmp (fullPathName, dirNode->dirName) == 0)
	    {
	    returnValue = ERROR;
	    break;
	    }

	/*
	 * If the first part of the filename is exactly equal
	 * to the directory name, return OK (#2).
	 */

	if (strncmp (dirNode->dirName, fullPathName,
		     strlen (dirNode->dirName)) == 0)
	    {
	    returnValue = OK;
	    break;
	    }
	}
    semGive (tftpdDirectorySem);

    if (returnValue == ERROR)
	errnoSet (EACCESS);
    return (returnValue);
    }
#endif /* Function currently not used */

/******************************************************************************
*
* tftpdErrorSend - send an error to a client
*
* Given a client connection and an error number, this routine builds an error
* packet and sends it to the client.
*
* RETURNS:
* OK, or ERROR if tftpSend() fails.  Note that no further action is required if
* tftpSend() fails.
*/

LOCAL STATUS tftpdErrorSend
    (
    TFTP_DESC	*pReplyDesc,	/* client to send to */
    int		errorNum		/* error to send */
    )
    {
    TFTP_MSG	errMsg;
    int		errSize;
    int		repeatCount = tftpdErrorSendTries;
    STATUS	returnValue = OK;

    if (tftpVerbose)
       printf ("tftpdErrorSend: errorNum = %d\n", errorNum);

    errSize = tftpErrorCreate (&errMsg, errorNum);

    /*
     * Try to send the error message a few times.
     */

    while (repeatCount--)
	{
	returnValue = tftpSend (pReplyDesc, &errMsg, errSize,
				(TFTP_MSG *) 0, 0, 0, (int *) 0);
	/*
	 * If we didn't get an error, break out of the loop.  Otherwise,
	 * wait one second and retry
	 */

	if (returnValue != ERROR)
	    {
	    break;
	    }
	else
	    {
	    taskDelay (sysClkRateGet ());
	    }
	}
    return (returnValue);
    }

