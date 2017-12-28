/* tftpLib.h - vxWorks Trival File Transfer protocol header file */

/* Copyright 1984-1992 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,22sep92,rrr  added support for c++
01f,18sep92,jcf  added include of netinet/in.h.
01e,04jul92,jcf  cleaned up.
01d,15jun92,elh	 changed retransmission time.
01c,26may92,rrr  the tree shuffle
01b,29feb92,elh  ansified.
		 incorperated changes done by jmm for server.
01a,19Jul91,elh  written.
*/

#ifndef __INCtftpLibh
#define __INCtftpLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vwModNum.h"
#include "netinet/in.h"

/* defines */

/* TFTP constant values.  */

#define	TFTP_SEGSIZE		512		/* data segment size	*/
#define TFTP_ACK_SIZE		4		/* size of ack message	*/
#define TFTP_DATA_HDR_SIZE	4		/* header size 		*/
#define	TFTP_TIMEOUT		2		/* secs between rexmt's	*/
#define TFTP_PORT	 	69		/* tftp default port    */

#define TFTP_CLIENT		0
#define TFTP_SERVER		1


#define MAX_NUMBER_OF_TFTP_TASK  20 

/* TFTP Op Codes/packet types */

#define	TFTP_RRQ	01			/* read request		*/
#define	TFTP_WRQ	02			/* write request 	*/
#define	TFTP_DATA	03			/* data packet 		*/
#define	TFTP_ACK	04			/* acknowledgement 	*/
#define	TFTP_ERROR 	05			/* error packet 	*/

/*
  TFTP message formats are:


Type        Op #     Format without header

             2 bytes   string   1 byte  string  1 byte
            ------------------------------------------
TFTP_RRQ/  | [01|02]| filename |  0   | mode   |   0 |
TFTP_WRQ    ------------------------------------------

             2 bytes  2 bytes   n bytes
            ---------------------------
TFTP_DATA  |   03   | block # | data  |
            ---------------------------

             2 bytes   2 bytes
            ------------------
TFTP_ACK   |   04   | block #|
            ------------------

             2 bytes  2 bytes     string  1 byte
            -------------------------------------
TFTP_ERROR |  05    | ErrorCode | ErrMsg |   0  |
            -------------------------------------
*/

#if CPU_FAMILY==I960
#pragma align 1				/* don't to optimize alignments */
#endif /* CPU_FAMILY==I960 */

typedef struct fd
{
    NODE     node;
    U32_t    fdSignature; /* Signatures to validate FD;Don't trust if not mtch*/
    U32_t    fStart;   /* Start Address of file */
    S32_t    fMode;    /* Open for READ, WRITE, or Both */
    SIW_SEMID_t   fSemId;   /* Semaphore to prvent multiple writes */
    BOOL_t   semStatus; /* TRUE if sem was taken FALSE if sem was not taken */
    S32_t    fType;    /* What type of file, i.e. RAM, NVRAM, HD, FLASH*/
    S32_t    fLen;     /* Max. Length of the file */
    U32_t    fNextWr;    /* Next location where to write (append)  */
    S32_t    vxWFd;    /* This is VxWorks FD */
    S32_t    fStatus;  /* open/close count */
    LIST     tlist;    /* list of the tasks using this FD */
    char     fName[MAX_FILE_NAME_LENGTH];
} FD_t;



/* TFTP packet structure.  */

struct	tftpFormat
    {
    u_short blockOrError;
    char data [TFTP_SEGSIZE];
    };

typedef struct tftp_msg
    {
    u_short	th_opcode;			/* packet op code	*/
    union
	{
        char	 request [TFTP_SEGSIZE + 2];	/* request string 	*/
	struct   tftpFormat  misc;
     	} th;
    } TFTP_MSG;

#if CPU_FAMILY==I960
#pragma align 0				/* turn off alignment requirement */
#endif /* CPU_FAMILY==I960 */


/* useful defines to access the TFTP packet */

#define	th_request	th.request		/* request string 	*/
#define th_block	th.misc.blockOrError	/* block number		*/
#define th_error	th.misc.blockOrError	/* error code		*/
#define th_errMsg	th.misc.data		/* error message	*/
#define th_data		th.misc.data		/* data			*/


/* TFTP defined error codes.  */

#define	EUNDEF		0			/* not defined 		*/
#define	ENOTFOUND	1			/* file not found 	*/
#define	EACCESS		2			/* access violation	*/
#define	ENOSPACE	3			/* disk full		*/
#define	EBADOP		4			/* illegal operation 	*/
#define	EBADID		5			/* unknown transfer ID	*/
#define	EEXISTS		6			/* file already exists	*/
#define	ENOUSER		7			/* no such user		*/
#define EINVALIDPSWD    8                       /* Invalid password     */
#define EDISKWRERR      9                       /* Disk Write Error     */
#define ECHKSUMERR      10                      /* Checksum error       */
#define EFORMATERR      11                      /* File format error    */
#define ESTNDBYERR      12                      /* Standby update error */
#define EIMGMISMTC      13                      /* Image Mismatch       */
#define EREJECTTFTPANMT 14                      /* anmt file is still active*/
#define EEXCEEDANMTLIMIT      15                /* exceed total anmt duration */
#define EINVALIDANMTFILENAME  16                /* invalid anmt file name */
#define EINVALIDANMTFILEID    17                /* invalid anmt file index */
#define EEXCEEDTASKLIMIT      18                /* exceed total anmt duration */
#define EINVALIDIMGFILENAME   19                /* invalid image file name */
#define EILLEGALWRITE         20                /* illegal write op */
#define EILLEGALTONEPLANFILENAME   21           /* illegal tp file name */
#define EILLEGALTONEPLANWRITE      22           /* illegal write to tp */
#define EILLEGALTONEPLANID         23           /* illegal tp id */
#define EILLEGALTONEPLANVERSION    24           /* illegal tp version */
#define EILLEGALTONEPLAN           25           /* illegal tone plan def */

/* mode structure */

typedef struct tftp_mode
    {
    char *m_name;				/* mode name		*/
    char *m_mode;				/* mode type		*/
    } TFTP_MODE;

/* error structure */

typedef struct tftp_errmsg
    {
    int   e_code;				/* error code		*/
    char  *e_msg;				/* error message	*/
    } TFTP_ERRMSG;

/* TFTP descriptor */

typedef struct tftp_desc
    {
    char 	mode [32];			/* mode 		*/
    BOOL	connected;			/* state 		*/
    char    	serverName [128];		/* server ascii name	*/
    struct sockaddr_in serverAddr;		/* server address 	*/
    int 	sock;				/* socket 		*/
    u_short	serverPort;			/* server port number	*/
    char        localFile [128];                /* local file name      */
    u_char      fileType;                       /* file type            */
    u_short     fileId;                         /* Announcement file ID */
    u_int       errNo;
    int         block;
    } TFTP_DESC;

/* TFTP task defaults */
#define UNKNOWN_FILE            1
#define FW_FILE                 2
#define FLASH_FILE              3

#define	TFTP_TASK_PRIORITY	70		/* TFTP task priority	*/
#define TFTP_TASK_OPTIONS	0		/* TFTP task options	*/
#define TFTP_TASK_STACKSIZE	7000		/* TFTP stack size 	*/

/* tftpLib error values */

#define S_tftpLib_INVALID_ARGUMENT	(M_tftpLib | 1)
#define S_tftpLib_INVALID_DESCRIPTOR	(M_tftpLib | 2)
#define S_tftpLib_INVALID_COMMAND	(M_tftpLib | 3)
#define S_tftpLib_INVALID_MODE		(M_tftpLib | 4)
#define S_tftpLib_UNKNOWN_HOST		(M_tftpLib | 5)
#define S_tftpLib_NOT_CONNECTED		(M_tftpLib | 6)
#define S_tftpLib_TIMED_OUT		(M_tftpLib | 7)
#define S_tftpLib_TFTP_ERROR		(M_tftpLib | 8)
#define S_tftpLib_FW_CHKSUM_ERROR       (M_tftpLib | 9)
#define S_tftpLib_READ_ERROR            (M_tftpLib | 10)
#define S_tftpLib_WRITE_ERROR           (M_tftpLib | 11)
#define S_tftpLib_FORMAT_ERROR          (M_tftpLib | 12)
#define S_tftpLib_IMAGE_MISMATCH        (M_tftpLib | 13)
#define S_tftpLib_EXCEED_NUMOFTASK_LIMIT  (M_tftpLib | 14)
#define S_tftpLib_ILLEGAL_IMG_FILE_NAME   (M_tftpLib | 15)
#define S_tftpLib_ILLEGAL_WRITE           (M_tftpLib | 16)

/* Tone plan error codes */
#define MAX_ERROR_MSG_LENGTH   512


#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	tftpXfer (char * pHost, int port, char * pFilename,
			  char * pCommand, char * pMode, int * pDataDesc,
    			  int * pErrorDesc);
extern STATUS 	tftpCopy (char * pHost,	int port, char * pFilename,
			  char * pCommand, char * pMode, int fd);
extern TFTP_DESC *tftpInit (void);
extern STATUS 	tftpModeSet (TFTP_DESC * pTftpDesc, char * pMode);
extern STATUS 	tftpPeerSet (TFTP_DESC * pTftpDesc, char * pHostname,
			     int port);
extern STATUS 	tftpGet (TFTP_DESC *pTftpDesc, char *pFilename, FD_t *fd,
		         int clientOrServer);
extern STATUS 	tftpInfoShow (TFTP_DESC * pTftpDesc);
extern STATUS 	tftpQuit (TFTP_DESC * pTftpDesc);
extern int 	tftpSend (TFTP_DESC *pTftpDesc, TFTP_MSG *pTftpMsg, int sizeMsg,
		          TFTP_MSG *pTftpReply, int opReply, int blockReply,
		          int *pPort);
extern int 	tftpErrorCreate (TFTP_MSG * pTftpMsg, int errorNum);

#else   /* __STDC__ */

extern STATUS 	tftpXfer ();
extern STATUS 	tftpCopy ();
extern TFTP_DESC *tftpInit ();
extern STATUS 	tftpModeSet ();
extern STATUS 	tftpPeerSet ();
extern STATUS 	tftpPut ();
extern STATUS 	tftpGet ();
extern STATUS 	tftpInfoShow ();
extern STATUS 	tftpQuit ();
extern int 	tftpSend ();
extern int 	tftpErrorCreate ();

#endif  /* __STDC__ */


#ifdef __cplusplus
}
#endif

#endif /* __INCtftpLibh */
