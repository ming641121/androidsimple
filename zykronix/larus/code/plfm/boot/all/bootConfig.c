/* bootConfig.c - system configuration module for boot ROMs */

/* Copyright 1984 - 2004 Wind River Systems, Inc. */

/*
modification history
--------------------
13f,05oct04,ijm  for ppp boot device, disconnect only if state is OPENED
13e,22sep04,dlk  Call netCoreSetup() instead of netCoreInstInit().
                 Call usrNetApplUtilInit() when INCLUDE_APPL_LOG_UTIL is
		 defined.
13d,20sep04,spm  updated hostname initialization: use separate routines
                 for host table setup and local hostname assigment
13c,17sep04,ijm  added vxWorks 5x equivalent PPP
13b,10sep04,kc   Fixed build problem when INCLUDE_NETWORK is not defined.
13a,31aug04,vvv  changed INCLUDE_SYSCTL to INCLUDE_NET_SYSCTL
12z,27aug04,spm  merged network stack changes from Base6 itn5, version 13v
                 (label COMP_WN_IPV6_BASE6_ITER5_TO_UNIFIED_PRE_MERGE):
                 replaced sysctl initialization with single routine
12y,07jul04,dlk  Remove SOCK_FD_MAX argument to sockLibInit().
12x,01jul04,niq  Move splnetInstInit to netCoreInstInit
12w,01jun04,rp   merged from iteration5
12v,19mar04,rp   updated to work with dual stack
12u,12feb03,pmr  fixed warning.
12t,24jan03,pmr  SPR 83157: remove relative include paths.
                 SPR 65782: validation of boot line in NVRAM.
12s,22jan03,wap  restore bootpLib and ipLib API compatibility
12r,13jan03,vvv  merged from branch wrn_velocecp, ver12u (SPR #83245, 83246,
		 84378)
12q,19jun02,elr  Moved close for ftp data sockets (SPR #77169)
12p,06jun02,pai  Removed redundant error messages from xxxLoad() routines.
12o,23may02,mas  corrected args passed to smNetInit() (SPR 71787)
12n,22may02,jkf  SPR#76700, pcmciaInit must be called before muxDevLoad
12m,22may02,sbs  changed default sys clock rate to a macro (SPR #1918)
12l,16may02,vvv  restored SLIP support
12k,10may02,wap  Reduce tBoot's priority to below tNetTask's before doing
                 netLoad() (SPR #76107)
12j,06may02,sbs  updating boot banner
12i,30apr02,wap  Correct arguments passed to dhcpcOptionAdd() for
                 little-endian targets (SPR #73769)
12h,24apr02,mas  fixed backplane NETIF params in usrBpInit() (SPR 76236)
12g,31mar02,jkf  SPR#74251, using bootLib.h macros for array sizes
12f,28mar02,jkf  SPR#74558, bootrom: command line input length increased.
12e,22mar02,zmm  Fix mask in 'd' instruction for little endian only (SPR 5602)
12d,14mar02,hdn  added X86 specific code for HTT in go () (spr 73738)
12c,13mar02,vvv  removed references to SLIP (SPR #72992)
12b,04mar02,pch  SPR 73609
12a,29jan02,vvv  allow icmpMaskGet to work if host and target are in different
                 subnets; added debug messages for netmask configuration
11z,17jan02,pcm  SPR 72642 - split excInit() into excInit() and excShowInit()
11y,10dec01,kab  SPR 71410 - stack overflow
11x,10dec01,sbs  Added version command and check for the length of bootline 
                 before saving to NVRAM. 
11w,10dec01,rec  Made 'd' command endian independent (SPR 5602).
11v,09dec01,jkf  SPR#62094, 62122, need devSplit() for booting from dosfs
11u,07dec01,jkf  SPR#70548, Booting from Partitioned disks fails with SCSI
11t,30nov01,nrv  including sysTffs.c when INCLUDE_TFFS is defined
11s,28nov01,tlc  Correct diab syntax error for MIPS architecture.
11r,11nov01,sbs  Added intLock() before jumping out of boot image (SPR 3704)
                 Removed '!' as an option to continue booting (SPR 7256)
11q,13nov01,yvp  Added include for private/excLibP.h
11p,27oct01,dat  Adding trap for misaligned data section SPR 9006
11o,09oct01,rae  merge from truestack ver11n.  Base 11j (SPR 25374)
11p,19oct01,vvv  fixed IP_MAX_UNITS declaration (SPR #70476)
11o,11oct01,aeg  updated initialization sequence of selectLib (SPR #31319).
11n,16sep01,dat  Use of WRS_ASM macro
11m,18jun01,mil  Cleaned up compiler warnings.
11l,27feb01,hk   update printBootLogo() copyright notice to cover 2001.
11l,07feb01,spm  added merge record for 30jan01 update from version 10z of
                 tor2_0_x branch (base 10y) and fixed modification history
11k,30jan01,ijm  merged SPR# 28602 fixes: proxy ARP services are obsolete
10q,30apr99,jpd  changed copyright year from 1998 to 1999.
10p,13nov98,jpd  conditionally add sysHwInit0() call before cacheLibInit(); make
		 cacheEnable call depend on INCLUDE_CACHE_SUPPORT (SPR #22707).
10z,01sep99,spm  allowed proxy client and server to use any device (SPR #8993)
11j,08dec00,rae  Fixed minor #ifdef problem
11i,06dec00,rae  usrRoot can't return ERROR
11h,17nov00,spm  fixed NPT merge to allow build without END device support
11g,14nov00,rae  removed unused argument from sockLibAdd()
11f,01nov00,ham  update muxDevStopAll call in response to muxLib.c's ver 03k.
11e,17oct00,niq  Integrating T3 DHCP
11e,28feb00,gnn  merged from NPT
11d,04oct99,pul  Modification for NPT: passing the right cookie to muxDevStart
11c,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
11b,29mar99,pul  removed mmuxMCastMapAdd()
11a,26mar99,sj   added NPT support: muxLibInit; muxMaxBinds
10z,19mar99,dat  removed bp driver interface SPR 25877, fixed bug in ver 10x.
11a,19apr00,zl   fixed unitNum default to 0 in mEnet(). Updated copyright year.
10y,17mar99,spm  added support for identical unit numbers (SPR #20913)
10x,16mar99,dat  SPR 25790, fixed startup of END and BSD drivers
10w,16mar99,spm  recovered orphaned code from tor1_0_1.sens1_1 (SPR #25770)
10v,01mar99,spm  bypassed incorrect cleanup after successful TFTP file 
                 transfer (SPR #24618)
10u,26feb99,dat  added FEI support (23818)
10t,19feb99,spm  fixed construction of device name for pcmcia (SPR #21676)
10s,02feb99,sgv  Added bitflag value to sockLibAdd
10r,28jan99,jmp  added TSFS_BOOT facility (SPR# 24466).
10q,20jan99,scb  usrBpInit() modified to use "sm=" before SM_ANCHOR_ADRS (23035)
10p,26jan99,jkf  removed x86 specific memAddToPool's from ataLoad,
                 ideLoad, pcmciaLoad, & tffsLoad.  Replaced with
                 BSP specific memAddToPool in sysHwInit2() (SPR#21338).
10o,13nov98,n_s  added call to muxDevStopAll in go. spr #23229
10n,11sep98,spm  corrected documentation for bootpGet and dhcpGet (SPR #20629)
10m,04sep98,ham  set subnetmask through ntohl() in bootpGet(),SPR#21909.
10l,03jun98,hdn  disabled system clock just before jumping to the entry point
		 for PentiumPro's Local APIC Timer.
10k,23apr98,yp   merged TrueFFS support
10j,01apr98,hdn  moved tffsLoad from usrTffs.c.
10i,04feb98,jpd  updated copyright date.
10h,19mar98,spm  corrected prototype for mEnet() routine (SPR #20174); changed
                 parameter name to match coding standards
10g,02feb98,spm  replaced invalid logMsg calls with printf statements
10f,06jan98,hdn  added support for TFFS.
10e,30nov97,vin  added network system pool
10d,20nov97,gnn  fixed spr#7265 to deal with devices with no attach routine.
10c,27oct97,vin  fixed call to endLoadFunc(..).
10b,03oct97,gnn  removed references to endDriver global
10b,23oct97,tam  fixed compilation warnings.
10a,26sep97,gnn  added muxAddrResFuncAdd call for ipAttach
09z,26sep97,vin  used NUM_XXX_MIN macros for system pool configuration.
09y,25sep97,gnn  SENS beta feedback fixes
09x,26aug97,vin  fixed warnings in muxIoctl
09w,26aug97,spm  major overhaul of DHCP client: reorganized code and changed
                 user interface to support multiple leases at runtime, improved
                 handling of address information, and allowed selection of UDP
                 ports; also fixed incorrect treatment of gateway field by
                 BOOTP/DHCP (SPR #9137)
09v,12aug97,gnn  changes necessitated by MUX/END update.
09u,15jul97,spm  fixed byte ordering of netmask from DHCP client (SPR #8739)
09t,01jul97,jag  removed stale code for RESOLVER (SPR #8868)
09j,15aug97,cdp  add casts etc. to stop compiler warnings.
09i,05feb97,cdp  remove ARM test hook (_func_armStartup).
09h,21jan97,jpd  Added support for Olicom and lnEbsa Ethernet drivers, made
                 cacheLib calls dependent upon INCLUDE_CACHE_SUPPORT being
                 defined.
09g,12dec96,cdp  added _func_armStartup for ARM platforms.
09s,02jun97,spm  changed DHCP option tags to prevent name conflicts (SPR #8667)
09r,20may97,gnn  fixed SPR 8627 so that multiple gn devices are supported.
09q,19may97,spm  included rngLib.h to fix DHCP client build error (SPR #8606)
09p,30apr97,spm  moved usrNetIfConfig for "gn" device to prevent DHCP error
09o,25apr97,gnn  changed INCLUDE_GN to INCLUDE_END
09n,18apr97,spm  corrected structure element name in BOOTP interface
09m,17apr97,gnn  added support for configNet style configuration of ENDs.
09l,07apr97,spm  changed BOOTP interface to DHCP style: all options supported
09k,29jan97,spm  made global variables for DHCP client unique
09j,29jan97,spm  removed parameters from dhcpBootBind()
09i,21jan97,gnn  added the new argument to muxDevLoad().
09h,20dec96,vin  fixed warnings muxDevLoad(). added AF_ROUTE.
09g,06mar97,mas  deleted unused variable 'char gateway[INET_ADDR_LEN];' from
		 bootLoad() (SPR 8113).
09g,18dec96,spm  fixed bug in unit number support and restored lost DHCP code
09f,17dec96,gnn  added stuff for new etherHooks.
09e,27nov96,spm  added support for DHCP client and network device unit numbers
09d,25nov96,vin	 added new cluster configuration.
09c,01oct96,spm  added bzero() call to checkInetAddrField (SPR 6326)
09b,22sep96,spm  Fixed SPR 7120: added support for gateways to BOOTP startup
09a,24jul96,vin  fetching netmask after usrNetIfConfig() mods for bsd4.4.
08z,21jun96,jmb  long modhist -- deleted entries prior to 1994.  SPR #6528
08y,19jun96,hdn  defined IDE_MEM_DOSFS to 0x200000.
08x,13jun96,hdn  added INCLUDE_ESMC for SMC91c9x Ethernet driver.
08w,05jun96,kkk  update copyright year from 1995 to 1996.
08v,05jun96,ism  bumped copyright date to 1996.
19v,19apr96,jds  changed the include of scsiLib.h to be done after config.h;
                 also added setting to asynchronous mode all target devices
08u,20mar96,hdn  made inclusion of pppLib.h and strmLib.h conditional.
09t,07mar96,gnn	 added SLIP_MTU to slipInit so that we could configure
	  	 the MTU.  (SPR #4652)
08s,06mar96,tpr  added #include "bootElfLib.h".
08r,06mar96,dat  removed __DATE__ from printBootLogo
08q,06mar96,tpr  Enabled the Instruction Cache for PowerPC.
08p,06dec95,vin  fixed usrStrmInit warning.
08o,29nov95,vin  added baudrate processing of boot string.
08n,06sep95,jag  deleted call to  fattachInit (), changed sadInit to 
		 autopushInit (), and changed iosStrmInit to strmInit (). 
08m,01aug95,dzb  changed STREAMS options to go through iosStrmInit ().
		 added usrStrmInit ().
                 PPP initialization failure message (SPR #4505).
08l,25jul95,dzb  tweaked socket library initialization.
08k,25jul95,dzb  added socket library init for BSD vs. STREAMS.
08j,18jul95,dzb  wait for PPP interface to come up in usrPPPInit().
08i,17jul95,dzb  fixed ordering of boot params search.
08h,26jun95,dzb  removed PPP_S_USEHOSTNAME option.
08g,21jun95,dzb  Added INCLUDE_PPP_CRYPT for unbundled crypt() support.
08f,20jun95,dzb  Added PPP support.
09b,28nov95,tpr	 re-ordered history following PPC merge.
09a,11oct95,dat	 new BSP revision id, modified signon banner printing
08z,29jun95,caf  added nicEvb driver entry.
08y,28jun95,caf  for PPC, called cacheTextUpdate() instead of cacheClear().
08x,26jun95,kvk  Removed references to _edata and _end for a clean 
		 bootrom_uncmp build.
08w,06jun95,caf  fixed edata and end for Green Hills/PowerPC.
08v,26jun95,ms	 updated for new serial drivers, removed WDB support.
08u,21jun95,ms	 changed copywrite to 1995
08t,01jun95,caf  added support for "ilac" driver.
08s,22may95,yao  adjust reference to edata, end to SVR4 ABI standard 
		 for PowerPC.
08r,22may95,p_m  added WDB agent support.
          + ms
08q,05apr95,kkk  changed edata & end to char arrays (spr# 3917)
08p,28mar95,kkk  made baud rate a macro in configAll.h
08o,19mar95,dvs  removed TRON references.
08n,08dec94,hdn  swapped 1st and 2nd parameters of fdDevCreate().
		 fixed bootHelp message; fd=... and ide=...
08m,21nov94,hdn  fixed a problem by swapping 1st and 2nd parameters of sscanf.
08l,20nov94,kdl  added hashLibInit() call if using dosFs.
08k,11nov94,dzb  added QU network interface.
08j,09nov94,jds  additions for scsi backward compatability ; scsi[12]IfInit()
08i,20oct94,hdn  used ideRawio() instead of using raw file system.
		 swapped 1st and 2nd parameter of ideLoad() and fdLoad().
08h,17aug94,dzb  fixed setting the gateway for a slip connection (SPR #2353).
                 added INCLUDE_NETWORK macros for scalability (SPR #1147).
 		 added CSLIP support.
08g,14jun94,caf  updated copyright notices.
08f,29may94,hdn  fixed more FTP bootrom bug in netLoad().
		 updated the copyright year 93 to 94.
		 disabled cache for i486 and Pentium.
08e,26may94,kdl  changed netLoad() to properly close FTP connections (SPR 3231).
08d,10may94,hdn  fixed the FTP bootrom bug (John's patch)
08c,09feb94,hdn  added support for if_elt 3COM EtherLink III driver.
		 added support for if_ene Eagle NE2000 driver.
*/

/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks boot ROM.
It is a stripped-down version of usrConfig.c, having no VxWorks shell or
debugging facilities.  Its primary function is to load an object module
over the network with RSH, FTP or TFTP.  Additionally, a simple set of
single letter commands is provided for displaying and modifying memory
contents. Use this module as a starting point for placing applications 
in ROM.
*/

#include "vxWorks.h"
#include "bootEcoffLib.h"
#include "bootElfLib.h"
#include "bootLib.h"
#include "bootLoadLib.h"
#include "cacheLib.h"
#include "cbioLib.h"
#include "ctype.h"
#include "dosFsLib.h"
#include "dpartCbio.h"
#include "errno.h"
#include "errnoLib.h"
#include "fcntl.h"
#include "fioLib.h"
#include "intLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "loadAoutLib.h"
#include "loadCoffLib.h"
#include "loadLib.h"
#include "logLib.h"
#include "memLib.h"
#include "msgQLib.h"
#include "pipeDrv.h"
#include "proxyLib.h"
#include "qLib.h"
#include "qPriBMapLib.h"
#include "rebootLib.h"
#include "rngLib.h"
#include "semLib.h"
#include "stdio.h"
#include "string.h"
#include "sysLib.h"
#include "sysSymTbl.h"
#include "taskHookLib.h"
#include "taskLib.h"
#include "tickLib.h"
#include "trcLib.h"
#include "unistd.h"
#include "usrFdiskPartLib.h"
#include "version.h"
#include "wdLib.h"

#include "private/excLibP.h"
#include "private/kernelLibP.h"
#include "private/workQLibP.h"

#include <netCore.h>
#include <netLib.h>
#include <ipLib.h>
#include <protos/tcpLib.h>
#include <protos/udpLib.h>
#include <arpa/inet.h>
#include <hostLib.h>
#include <ftpLib.h>
#include <tftpLib.h>
#include <private/ftpLibP.h>
#include <netDrv.h>
#include <remLib.h>
#include <sockFunc.h>
#include <sockLib.h>
#include <sys/mem_stru.h>
#include <sys/ds_conf.h>
#include <net/route.h>
#include <route/ipRouteLib.h>

/*
 * The BOOTCONFIG define must be before the inclusion of config.h, as
 * it affects the defaults for inclusion of some components such as TFTP.
 */

#define BOOTCONFIG

#include "config.h"
#include "scsiLib.h"

#ifdef   INCLUDE_NETWORK
#define  INCLUDE_BSD_SOCKET
#define  INCLUDE_SOCKLIB
#define  INCLUDE_DOMAIN_INIT
#define  INCLUDE_NET_SYSCTL
#define  INCLUDE_GTF
#define  INCLUDE_GTF_TIMER_START
#define  INCLUDE_NET_POOL
#define  INCLUDE_NETBUFLIB

#ifdef INCLUDE_END
#define INCLUDE_MUX
#endif

#ifdef INCLUDE_ETHERNET
#define INCLUDE_MUX
#endif
#else /* INCLUDE_NETWORK not defined */
/* the following are needed because usrDepend.c is not compiled for bootrom */
#undef   INCLUDE_END
#undef   INCLUDE_ETHERNET
#undef   INCLUDE_MUX
#endif  /* INCLUDE_NETWORK */

#ifdef INCLUDE_BSD_SOCKET
#include <bsdSockLib.h>
#include <net/socketvar.h>
#endif

#ifdef INCLUDE_NET_SYSCTL
#include <sysctlLib.h>

IMPORT STATUS SysctlInstInit (void *);
#endif  /* INCLUDE_NET_SYSCTL */

#ifdef INCLUDE_MUX
#include <muxLib.h>
#endif

#if (defined(INCLUDE_DHCPC) || defined(INCLUDE_BOOTP))
#define INCLUDE_BPF
#endif /* INCLUDE_DHCPC.. */

#ifdef INCLUDE_BPF
#include <bpfDrv.h>
#endif

#ifdef INCLUDE_DHCPC
#include <dhcpcBootLib.h>
#include <dhcp/dhcpcCommonLib.h>
#include <inetLib.h>
#include "sysLib.h"
#endif /* INCLUDE_DHCPC */

#ifdef INCLUDE_BOOTP
#include <bootpLib.h>
#include <inetLib.h>
#endif /* INCLUDE_BOOTP */

#ifdef  INCLUDE_SM_NET
#ifndef  INCLUDE_SMEND
#warning  "INCLUDE_SM_NET requires INCLUDE_SMEND for shared memory driver!"
#endif /* !INCLUDE_SMEND */
#endif /* INCLUDE_SM_NET */

#ifdef INCLUDE_SM_NET
#include "drv/end/smEnd.h"
#endif /* INCLUDE_SM_NET */

#ifdef INCLUDE_APPL_LOG_UTIL
#include <applUtilLib.h>
void usrNetApplUtilInit (void);
#endif

/* defines */

#define STREQ(A, B) (strcmp(A, B) == 0 ? 1 : 0)
#define TIMEOUT		7	/* number of seconds before auto-boot */

/* maximum line length for cmd input to 'm' and 'mEnet' routine */
#define MAX_LINE        160

/* maximum line length for bootCmdLoop() command line input */
#define BOOT_CMD_MAX_LINE        BOOT_LINE_SIZE

#define RSHD		514	/* rshd service */
#define DEC		FALSE	/* getArg parameters */
#define HEX		TRUE
#define OPT		TRUE
#define MAX_ADR_SIZE 	6
#define DOS_ID_OFFSET                   3
#define FIRST_PARTITION_SECTOR_OFFSET   (0x1be + 8)
#define VXDOS                           "VXDOS"
#define VXEXT                           "VXEXT"


#ifndef    NUM_PARTITIONS_DISK_BOOT
#   define NUM_PARTITIONS_DISK_BOOT    1 /* one partition on the disk */
#endif  /* NUM_PARTITIONS_DISK_BOOT */

#ifndef    PARTITION_DISK_BOOT
#   define PARTITION_DISK_BOOT         1 /* partition mounted during boot */
#endif  /* PARTITION_DISK_BOOT */

/* DO NOT ADD ANYTHING BEFORE THE FIRST ROUTINE compressedEntry() */

void		usrInit ();
IMPORT void 	sysInitAlt ();

#if	(CPU_FAMILY==MIPS)
IMPORT void 	sysGpInit ();
#endif	/* (CPU_FAMILY==MIPS) */

#ifdef INCLUDE_DHCPC
LOCAL DHCP_LEASE_DATA dhcpLease;
#endif

/*******************************************************************************
*
* compressedEntry - compressed entry point after decompression 
*
* This routine is the entry point after the bootroms decompress, if
* compression is utilized.  This routine must be the first item of the
* text segment of this file.  With ANSI C, strings will appear in text
* segment so one should avoid entering strings, routines, or anything
* else that might displace this routine from base of the text segment.
*
* It is unwise to add functionality to this routine without due cause.
* We are in the prehistoric period of system initialization.  
*
* NOMANUAL
*/

void compressedEntry 
    (
    int startType
    )
    {

#if (CPU_FAMILY==MIPS)
    WRS_ASM (".extern _gp; la $gp,_gp");
#endif

#if	(CPU_FAMILY==I960)
    sysInitAlt (startType);		/* jump to the i960 entry point */
#else
    usrInit (startType);		/* all others procede below */
#endif
    }

/* Wind kernel configuration facility */

#undef	INCLUDE_SHOW_ROUTINES		/* keep out kernel show routines */
#undef	INCLUDE_SM_NET_SHOW
#include "usrKernel.c"			/* kernel configuration facility */


/* imports */

IMPORT char	edata [];		/* defined by the loader */
IMPORT char	end [];			/* defined by the loader */

#define FREE_MEM_START_ADRS FREE_RAM_ADRS

#ifdef  INCLUDE_INITIAL_MEM_ALLOCATION
#define MEM_POOL_START_ADRS \
	    (ROUND_UP(FREE_MEM_START_ADRS, (INITIAL_MEM_ALIGNMENT)) + \
	    (INITIAL_MEM_SIZE))
#else   /* INCLUDE_INITIAL_MEM_ALLOCATION */
#define MEM_POOL_START_ADRS FREE_MEM_START_ADRS
#endif  /* INCLUDE_INITIAL_MEM_ALLOCATION */



#ifdef  INCLUDE_TSFS_BOOT		/* boot via Target Server File System */

#if ((WDB_COMM_TYPE == WDB_COMM_SERIAL) && \
	(CONSOLE_TTY == NONE || CONSOLE_TTY == WDB_TTY_CHANNEL))
#define INCLUDE_TSFS_BOOT_VIO_CONSOLE	/* needed for Target Server Console */
#endif

#define INCLUDE_WDB			/* WDB agent needed for TSFS Boot */
#define INCLUDE_WDB_TSFS		/* target-server file system */

#undef  INCLUDE_WDB_BANNER		/* print banner after agent starts */
#undef 	INCLUDE_WDB_TTY_TEST		/* test serial line communcation */
#undef  INCLUDE_WDB_START_NOTIFY	/* notify the host of task creation */
#undef  INCLUDE_WDB_USER_EVENT		/* user events handling */
#undef  INCLUDE_WDB_CTXT		/* context control */
#undef  INCLUDE_WDB_FUNC_CALL		/* spawn function as separate task */
#undef  INCLUDE_WDB_GOPHER		/* gopher info gathering */
#undef  INCLUDE_WDB_EXIT_NOTIFY		/* notify the host of task exit */
#undef  INCLUDE_WDB_REG			/* get/set hardware registers */
#undef  INCLUDE_WDB_EVENTPOINTS		/* eventpoints handling */
#undef  INCLUDE_WDB_MEM			/* optional memory services */
#undef  INCLUDE_WDB_BP			/* breakpoint support */
#include "wdb/wdbEvtLib.h"
#include "usrWdb.c"    			/* WDB agent configuration */

#else	/* INCLUDE_TSFS_BOOT not defined */
#undef INCLUDE_WDB
#endif  /* INCLUDE_TSFS_BOOT */

LOCAL BOOT_PARAMS sysBootParams;
SYMTAB_ID       statSymTbl;  /* XXX strerror requires it */

/*
 * ################ NETWORK INITIALIZATION #######################
 */

#ifdef  INCLUDE_NETWORK /* hama */
#ifdef ETHERNET_ADR_SET
void		mEnet (char *);
void 		sysEnetAddrGet ();
void 		sysEnetAddrSet ();
#endif  /* ETHERNET_ADR_SET */

#ifdef INCLUDE_END
#include "configNet.h"

IMPORT END_TBL_ENTRY endDevTbl [];

typedef struct cookie_tbl
    {
    int unitNo;
    char devName [END_NAME_MAX];
    void * pCookie;
    } COOKIE_TBL;

COOKIE_TBL cookieTbl [32];

#endif 	/* INCLUDE_END */

#ifndef IP_MAX_UNITS 
#define IP_MAX_UNITS 1
#endif

/*
 * mBlk, clBlk configuration table for network stack *SYSTEM* pool.
 * Used for network stack system structures such as routes, sockets,
 * protocol control blocks, interface addresses, mulitcast addresses,
 * and multicast routing entries.
 */
M_CL_CONFIG sysMblkClBlkDesc =
    {
    /*
      no. mBlks        no. clBlks          memArea      memSize
      -----------      ----------          -------      -------
    */
    NUM_SYS_MBLKS_MIN, NUM_SYS_CLBLKS_MIN, PMA_SYSPOOL, PMS_SYSPOOL
    };
  
/*
 * network stack *SYSTEM* cluster pool configuration table
 * Used for network stack system structures such as routes, sockets,
 * protocol control blocks, interface addresses, mulitcast addresses,
 * and multicast routing entries.
 */
CL_DESC sysClDescTbl [] =
    {
    /*
      clusterSize      num             memArea       memSize
      -----------      ----            -------       -------
    */
    {SIZ_SYS_16,   NUM_SYS_16_MIN,   PMA_SYS_16,   PMS_SYS_16},
    {SIZ_SYS_32,   NUM_SYS_32_MIN,   PMA_SYS_32,   PMS_SYS_32},
    {SIZ_SYS_64,   NUM_SYS_64_MIN,   PMA_SYS_64,   PMS_SYS_64},
    {SIZ_SYS_128,  NUM_SYS_128_MIN,  PMA_SYS_128,  PMS_SYS_128},
    {SIZ_SYS_256,  NUM_SYS_256_MIN,  PMA_SYS_256,  PMS_SYS_256},
    {SIZ_SYS_512,  NUM_SYS_512_MIN,  PMA_SYS_512,  PMS_SYS_512},
    {SIZ_SYS_1024, NUM_SYS_1024_MIN, PMA_SYS_1024, PMS_SYS_1024}
    };

UINT sysClDescTblNumEnt = (NELEMENTS(sysClDescTbl));


/*
 * mBlk, clBlk configuration table for network stack *DATA* pool.
 * Only used for data transfer in the network stack.
 */
M_CL_CONFIG pktMblkClBlkDesc =
    {
    /*
      no. mBlks         no. clBlks         memArea      memSize
      -----------       ----------         -------      -------
    */
    NUM_DAT_MBLKS_MIN, NUM_DAT_CLBLKS_MIN, PMA_DATPOOL, PMS_DATPOOL
    };

/*
 * network stack *DATA* cluster pool configuration table
 * Only used for data transfer in the network stack.
 */
CL_DESC pktClDescTbl [] =
    {
    /*
      clusterSize  num          memArea        memSize
      -----------  ----         -------        -------
    */
    {  64,  NUM_DAT_64_MIN,    PMA_DAT_64,    PMS_DAT_64},
    { 128,  NUM_DAT_128_MIN,   PMA_DAT_128,   PMS_DAT_128},
    { 256,  NUM_DAT_256_MIN,   PMA_DAT_256,   PMS_DAT_256},
    { 512,  NUM_DAT_512_MIN,   PMA_DAT_512,   PMS_DAT_512},
    { 1024, NUM_DAT_1024_MIN,  PMA_DAT_1024,  PMS_DAT_1024},
    { 2048, NUM_DAT_2048_MIN,  PMA_DAT_2048,  PMS_DAT_2048},
    { 4096, NUM_DAT_4096_MIN,  PMA_DAT_4096,  PMS_DAT_4096},
    { 8192, NUM_DAT_8192_MIN,  PMA_DAT_8192,  PMS_DAT_8192},
    {16384, NUM_DAT_16384_MIN, PMA_DAT_16384, PMS_DAT_16384},
    {32768, NUM_DAT_32768_MIN, PMA_DAT_32768, PMS_DAT_32768},
    {65536, NUM_DAT_65536_MIN, PMA_DAT_65536, PMS_DAT_65536}
    };

UINT pktClDescTblNumEnt = (NELEMENTS(pktClDescTbl));


#include "../comps/src/net/coreip/usrNetBoot.c"		  /* INCLUDE_NET_BOOT */
#include "../comps/src/net/coreip/usrNetIpAttachCommon.c" /* INCLUDE_IPATTACH */
#include "../comps/src/net/coreip/usrNetBootConfig.c" /* INCLUDE_NET_BOOT_CONFIG */
#include "../comps/src/net/coreip/usrNetBsdSocket.c"	/* INCLUDE_BSD_SOCKET */

#ifdef INCLUDE_END
#include "../comps/src/net/coreip/usrNetEndLib.c"	/* INCLUDE_END */
#endif /* INCLUDE_END */

#ifdef INCLUDE_ETHERNET
#include "../comps/src/net/coreip/usrNetEther.c"	/* INCLUDE_ETHERNET */
#endif /* INCLUDE_ETHERNET */

#include "../comps/src/net/coreip/usrNetLoopback.c"	/* INCLUDE_LOOPBACK */
#include "../comps/src/net/coreip/usrNetIf.c"	        /* INCLUDE_IF */
#include "../comps/src/net/coreip/usrNetIpv4.c"		/* INCLUDE_IPV4 */
#include "../comps/src/net/coreip/usrNetIcmpv4.c"	/* INCLUDE_ICMPV4 */
#include "../comps/src/net/coreip/usrNetRawv4.c"	/* INCLUDE_RAWV4 */
#include "../comps/src/net/coreip/usrNetDaemon.c"	/* INCLUDE_NET_DAEMON */
#include "../comps/src/net/coreip/usrNetGtf.c"		/* INCLUDE_GTF */
#include "../comps/src/net/coreip/usrNetGtfTimerStart.c" /* INCLUDE_GTF_TIMER_START */
#include "../comps/src/net/coreip/usrNetRouteStorage.c" /* INCLUDE_ROUTE_STORAGE */
#include "../comps/src/net/coreip/usrNetRtSock.c"   /* INCLUDE_ROUTING_SOCKET */

#ifdef INCLUDE_UDPV4
#include "../comps/src/net/coreip/usrNetUdpv4.c"
#endif

#ifdef INCLUDE_TCPV4
#include "../comps/src/net/coreip/usrNetTcpv4.c"
#endif

#ifdef INCLUDE_IGMP
#include "../comps/src/net/coreip/usrNetIgmp.c"
#endif

#ifdef INCLUDE_HOST_TBL
#include "../comps/src/net/coreip/apps/usrNetHostTblCfg.c"
#endif

#ifdef INCLUDE_NET_REM_IO
#include "../comps/src/net/coreip/usrNetRemoteCfg.c"
#endif

#ifdef INCLUDE_BPF
#include "../comps/src/net/coreip/usrNetBpfDrv.c"
#endif

#ifdef INCLUDE_FTP
#include "../comps/src/net/coreip/apps/usrFtp.c"
#endif

/* forward declarations */

IMPORT int	loattach ();
IMPORT struct ifnet *	ifunit ();
IMPORT int	pcmciaattach ();
IMPORT int route_init ();
IMPORT STATUS hostInstInit (void *);

extern FUNCPTR _phostAdd;
extern NET_POOL_CONFIG_PARAMS netPoolDefaultConfigParams;

#ifdef INCLUDE_VXWORKS_5_X_EQUIV_PPP
#include "../comps/src/net/ppp/usrBasicPPPFrameworkInit.c"
#endif /* INCLUDE_VXWORKS_5_X_EQUIV_PPP */

#endif  /* INCLUDE_NETWORK */


/* global variables */

SYMTAB_ID	sysSymTbl;
int		consoleFd;		/* fd of initial console device */
char		consoleName [20];	/* console device name, eg. "/tyCo/0" */
int		sysStartType;		/* BOOT_CLEAR, BOOT_NO_AUTOBOOT, ... */

BOOL		scsiInitialized      = FALSE;
int		bootCmdTaskPriority  = 1;
int		bootCmdTaskOptions   = VX_SUPERVISOR_MODE;
int		bootCmdTaskStackSize = BOOT_CMD_STACK_SIZE;

/* Two magic cookies used to detect data section misalignment */

#define TRAP_VALUE_1	0x12348765
#define TRAP_VALUE_2	0x5a5ac3c3
LOCAL volatile UINT32	trapValue1	= TRAP_VALUE_1;
LOCAL volatile UINT32	trapValue2	= TRAP_VALUE_2;

/* forward declarations */

#ifdef __STDC__

void 		usrRoot (char *pMemPoolStart, unsigned memPoolSize);
void 		usrClock (void);
void		usrKernelInit (void);
LOCAL void	bootCmdLoop (void);
LOCAL char	autoboot (int timeout);
LOCAL void	printBootLogo (void);
LOCAL void	bootHelp (void);
LOCAL STATUS	bootLoad (char *bootString, FUNCPTR *pEntry);
LOCAL void	go (FUNCPTR entry);
LOCAL void	m (char *adrs);
LOCAL void	d (char *adrs, int nwords);
LOCAL void	bootExcHandler (int tid);
LOCAL void	skipSpace (char **strptr);
LOCAL void	printExcMsg (char *string);
LOCAL STATUS	getArg (char **ppString, int *pValue, BOOL defaultHex,
			BOOL optional);
LOCAL void	usrBootLineInit (int startType);
LOCAL STATUS	usrBootLineCrack (char *bootString, BOOT_PARAMS *pParams);

#ifdef  INCLUDE_NETWORK
LOCAL STATUS	netLoad (char *hostName, char *fileName, char *usr, 
			 char *passwd, FUNCPTR *pEntry);
LOCAL void	netifAdrsPrint (char *ifname);
LOCAL STATUS	checkInetAddrField (char *pInetAddr, BOOL subnetMaskOK);
#ifdef INCLUDE_BOOTP
LOCAL STATUS	bootpGet (char *pNetDev, char *pBootDevAddr, char *pBootFile,
			  char *pHostAddr, int *pMask);
#endif
#endif  /* INCLUDE_NETWORK */

#ifdef	INCLUDE_SCSI_BOOT
LOCAL STATUS	scsiLoad (int bootDevId, int bootDevLUN, char *fileName,
		          FUNCPTR *pEntry);
#endif	/* INCLUDE_SCSI_BOOT */

#ifdef  INCLUDE_FD
LOCAL STATUS    fdLoad (int drive, int type, char *fileName, FUNCPTR *pEntry);
#endif  /* INCLUDE_FD */

#ifdef  INCLUDE_IDE
LOCAL STATUS    ideLoad (int drive, int type, char *fileName, FUNCPTR *pEntry);
#endif  /* INCLUDE_IDE */

#ifdef	INCLUDE_ATA
LOCAL STATUS	ataLoad (int ctrl, int drive, char *fileName, FUNCPTR *pEntry);
#endif	/* INCLUDE_ATA */

#ifdef	INCLUDE_PCMCIA
LOCAL STATUS	pcmciaLoad (int sock, char *fileName, FUNCPTR *pEntry);
#endif	/* INCLUDE_PCMCIA */

#ifdef	INCLUDE_TFFS
LOCAL STATUS    tffsLoad (int drive, int removable, char *fileName, 
			  FUNCPTR *pEntry);
#endif	/* INCLUDE_TFFS */

#ifdef	INCLUDE_TSFS_BOOT
LOCAL STATUS	tsfsLoad (char * fileName, FUNCPTR * pEntry);
LOCAL void	wdbRebootEventGet (void * pNode, WDB_EVT_DATA *	pEvtData);
#endif	/* INCLUDE_TSFS_BOOT */

#else

void		usrRoot ();
void		usrClock ();
void		usrKernelInit ();
LOCAL void	bootCmdLoop ();
LOCAL char	autoboot ();
LOCAL void	printBootLogo ();
LOCAL void	bootHelp ();
LOCAL STATUS	bootLoad ();
LOCAL void	go ();
LOCAL void	m ();
LOCAL void	d ();
LOCAL void	bootExcHandler ();
LOCAL void	skipSpace ();
LOCAL void	printExcMsg ();
LOCAL STATUS	getArg ();
LOCAL void	usrBootLineInit ();
LOCAL STATUS	usrBootLineCrack ();

#ifdef  INCLUDE_NETWORK
LOCAL STATUS	netLoad ();
LOCAL void	netifAdrsPrint ();
LOCAL STATUS	checkInetAddrField ();
LOCAL STATUS	usrNetIfConfig ();
#ifdef INCLUDE_BOOTP
LOCAL STATUS	bootpGet ();
#endif
#endif  /* INCLUDE_NETWORK */

#ifdef	INCLUDE_SCSI_BOOT
LOCAL STATUS	scsiLoad();
#endif	/* INCLUDE_SCSI_BOOT */

#ifdef  INCLUDE_FD
LOCAL STATUS    fdLoad ();
#endif  /* INCLUDE_FD */

#ifdef  INCLUDE_IDE
LOCAL STATUS    ideLoad ();
#endif  /* INCLUDE_IDE */

#ifdef	INCLUDE_ATA
LOCAL STATUS	ataLoad ();
#endif	/* INCLUDE_ATA */

#ifdef	INCLUDE_PCMCIA
LOCAL STATUS	pcmciaLoad ();
#endif	/* INCLUDE_PCMCIA */

#ifdef	INCLUDE_TFFS
LOCAL STATUS    tffsLoad ();
#endif	/* INCLUDE_TFFS */

#ifdef	INCLUDE_TSFS_BOOT
LOCAL STATUS	tsfsLoad ();
LOCAL void	wdbRebootEventGet ();
#endif	/* INCLUDE_TSFS_BOOT */

#endif	/* __STDC__ */

/*******************************************************************************
*
* usrInit - user-defined system initialization routine
*
* This routine is called by the start-up code in romStart().  It is called
* before kernel multi-tasking is enabled, with the interrupts locked out.
*
* It starts by clearing BSS, so all variables are initialized to 0 as per
* the C specification.  Then it sets up exception vectors, initializes the
* hardware by calling sysHwInit(), and finally starts the kernel with the
* usrRoot() task to do the remainder of the initialization.
*
* NOMANUAL
*/

void usrInit
    (
    int startType
    )
    {
    /*
     * This trap will catch improper loading of the data section.
     * We check the magic cookie values to make sure the data section is
     * in the expected memory location. We do not want
     * to proceed further if the data segment is not correct.
     *
     * It should be easy to detect entry into the trap using an ICE, JTAG,
     * or logic analyzer. Without the trap, the processor is likely to run
     * away out of control.
     *
     * Data section misalignment can occur when there is a change in tool
     * chain, build rules, compiler, host utilites,  etc. 
     */

    while (trapValue1 != TRAP_VALUE_1 || trapValue2 != TRAP_VALUE_2)
	{
	/* infinite loop */;
	}
    
#if (CPU_FAMILY == SPARC)
    excWindowInit ();				/* SPARC window management */
#endif

#if	(CPU_FAMILY == MIPS)
    sysGpInit ();				/* MIPS global pointer */
#endif	/* (CPU_FAMILY == MIPS) */

#ifdef INCLUDE_SYS_HW_INIT_0
    /*
     * Perform any BSP-specific initialisation that must be done before
     * cacheLibInit() is called and/or BSS is cleared.
     */

    SYS_HW_INIT_0 ();
#endif /* INCLUDE_SYS_HW_INIT_0 */

    /* configure data and instruction cache if available and leave disabled */

#ifdef	INCLUDE_CACHE_SUPPORT
    /*
     * SPR 73609:  If a cache is not to be enabled, don't require
     * its mode to be defined.  Instead, default it to disabled.
     */
# if (!defined(USER_D_CACHE_ENABLE) && !defined(USER_D_CACHE_MODE))
#  define USER_D_CACHE_MODE CACHE_DISABLED
# endif	/* !USER_D_CACHE_ENABLE && !USER_D_CACHE_MODE */
# if (!defined(USER_I_CACHE_ENABLE) && !defined(USER_I_CACHE_MODE))
#  define USER_I_CACHE_MODE CACHE_DISABLED
# endif	/* !USER_I_CACHE_ENABLE && !USER_I_CACHE_MODE */

    cacheLibInit (USER_I_CACHE_MODE, USER_D_CACHE_MODE);
#endif	/* INCLUDE_CACHE_SUPPORT */

#if (CPU == SPARClite)
    cacheLib.textUpdateRtn = NULL;		/* XXX - mod hist 07u */
#endif

    /* don't assume bss variables are zero before this call */

    bzero (edata, end - edata);		/* zero out bss variables */

    sysStartType = startType;

    intVecBaseSet ((FUNCPTR *) VEC_BASE_ADRS);	/* set vector base table */

#if (CPU_FAMILY == AM29XXX)
    excSpillFillInit ();                        /* am29k stack cache managemt */
#endif

    excVecInit ();				/* install exception vectors */

    sysHwInit ();				/* initialize system hardware */

    usrKernelInit ();				/* configure the Wind kernel */


#if	(CPU==SPARC) || (CPU_FAMILY==I80X86)	/* XXX workaround for sun1e */
#undef USER_I_CACHE_ENABLE	/* XXX disable instruction cache */
#endif	/* (CPU==SPARC) || (CPU_FAMILY==I80X86)	*/

#ifdef	INCLUDE_CACHE_SUPPORT
#ifdef 	USER_I_CACHE_ENABLE
    cacheEnable (INSTRUCTION_CACHE);		/* enable instruction cache */
#endif	/* USER_I_CACHE_ENABLE */
#endif	/* INCLUDE_CACHE_SUPPORT */

    /* start the kernel specifying usrRoot as the root task */

    kernelInit ((FUNCPTR) usrRoot, ROOT_STACK_SIZE,
		(char *) MEM_POOL_START_ADRS,
		sysMemTop (), ISR_STACK_SIZE, INT_LOCK_LEVEL);
    }

/*******************************************************************************
*
* usrRoot - user-defined root task
*
* The root task performs any initialization that should be done
* subsequent to the kernel initialization.
*
* It initializes the I/O system, install drivers, create devices,
* sets up the network, etc., as necessary for the particular configuration.
* It may also create the system symbol table if one is to be included.
* Finally, it spawns the boot command loop task.
*
* NOMANUAL
*/

void usrRoot
    (
    char *      pMemPoolStart,          /* start of system memory partition */
    unsigned    memPoolSize             /* initial size of mem pool */
    )

    {
    char tyName [20];
    int ix;

    /* Initialize the memory pool before initializing any other package.
     * The memory associated with the root task will be reclaimed at the
     * completion of its activities.                                 
     */

    memInit (pMemPoolStart, memPoolSize);/* XXX select between memPartLibInit */

    /* set up system timer */

    sysClkConnect ((FUNCPTR) usrClock, 0);/* connect clock interrupt routine */
    sysClkRateSet (SYS_CLK_RATE); 	  /* set system clock rate */
    sysClkEnable ();			  /* start it */

    /*
     * The select library needs to be initialized before the tyLib module
     * since the _func_selWakeupListInit FUNCPTR is required (SPR #3314).
     * The installation of the select task delete hook is not performed
     * for boot ROMs.
     */

#ifdef INCLUDE_SELECT
    selectInit (NUM_FILES);
#endif  /* INCLUDE_SELECT */

    /* initialize I/O and file system */

    iosInit (NUM_DRIVERS, NUM_FILES, "/null");
    consoleFd = NONE;

    /* install driver for on-board serial ports and make devices */

#ifdef  INCLUDE_TYCODRV_5_2
#ifdef  INCLUDE_TTY_DEV
    if ((NUM_TTY > 0))
        {
        tyCoDrv ();                             /* install console driver */

        for (ix = 0; ix < NUM_TTY; ix++)        /* create serial devices */
            {
            sprintf (tyName, "%s%d", "/tyCo/", ix);

            (void) tyCoDevCreate (tyName, ix, 512, 512);

            if (ix == CONSOLE_TTY)
                strcpy (consoleName, tyName);   /* store console name */
            }

        consoleFd = open (consoleName, O_RDWR, 0);

        /* set baud rate */

        (void) ioctl (consoleFd, FIOBAUDRATE, CONSOLE_BAUD_RATE);
        (void) ioctl (consoleFd, FIOSETOPTIONS,
			OPT_ECHO | OPT_CRMOD | OPT_TANDEM | OPT_7_BIT);
        }
#endif  /* INCLUDE_TTY_DEV */

#else   /* !INCLUDE_TYCODRV_5_2 */
#ifdef  INCLUDE_TTY_DEV
    if ((NUM_TTY > 0))
        {
        ttyDrv();                               /* install console driver */

        for (ix = 0; ix < NUM_TTY; ix++)        /* create serial devices */
            {
#if (defined(INCLUDE_WDB) && (WDB_COMM_TYPE == WDB_COMM_SERIAL))
	    if (ix == WDB_TTY_CHANNEL)          /* don't use WDBs channel */
	    	continue;
#endif
            sprintf (tyName, "%s%d", "/tyCo/", ix);
            (void) ttyDevCreate (tyName, sysSerialChanGet(ix), 512, 512);

            if (ix == CONSOLE_TTY)              /* init the tty console */
                {
                strcpy (consoleName, tyName);
                consoleFd = open (consoleName, O_RDWR, 0);
                (void) ioctl (consoleFd, FIOBAUDRATE, CONSOLE_BAUD_RATE);
                (void) ioctl (consoleFd, FIOSETOPTIONS,
			OPT_ECHO | OPT_CRMOD | OPT_TANDEM | OPT_7_BIT);
                }
            }
        }
#endif  /* INCLUDE_TTY_DEV */


#ifdef INCLUDE_PC_CONSOLE
    pcConDrv ();
    for (ix = 0; ix < N_VIRTUAL_CONSOLES; ix++)
        {
        sprintf (tyName, "%s%d", "/pcConsole/", ix);
        (void) pcConDevCreate (tyName,ix, 512, 512);
        if (ix == PC_CONSOLE)           /* init the console device */
            {
            strcpy (consoleName, tyName);
            consoleFd = open (consoleName, O_RDWR, 0);
            (void) ioctl (consoleFd, FIOBAUDRATE, CONSOLE_BAUD_RATE);
            (void) ioctl (consoleFd, FIOSETOPTIONS,
			OPT_ECHO | OPT_CRMOD | OPT_TANDEM | OPT_7_BIT);
            }
        }
#endif  /* INCLUDE_PC_CONSOLE */

#endif  /* !INCLUDE_TYCODRV_5_2 */

#ifdef INCLUDE_WDB
    wdbConfig();		/* configure and initialize the WDB agent */
    vxBootFile[0] = NULL;	/* clear boot line set by wdbConfig() */

#if defined(INCLUDE_TSFS_BOOT) && defined(INCLUDE_TSFS_BOOT_VIO_CONSOLE)
    consoleFd = open ("/vio/0", O_RDWR, 0);	/* for Target Server Console */
#endif
#endif	/* INCLUDE_WDB */

    ioGlobalStdSet (STD_IN,  consoleFd);
    ioGlobalStdSet (STD_OUT, consoleFd);
    ioGlobalStdSet (STD_ERR, consoleFd);

    pipeDrv ();					/* install pipe driver */

#if     defined(INCLUDE_EXC_HANDLING) && defined(INCLUDE_EXC_TASK)
#ifdef  INCLUDE_EXC_SHOW
    excShowInit ();			/* init exception show routines */
#endif
    excInit ();				/* initialize exception handling */
#endif  /* defined(INCLUDE_EXC_HANDLING) && defined(INCLUDE_EXC_TASK) */

    excHookAdd ((FUNCPTR) bootExcHandler);	/* install exc handler */
    logInit (consoleFd, 5);			/* initialize logging */

#ifdef	INCLUDE_DOSFS
    hashLibInit ();				/* hashLib used by dosFS */
#endif

    /* initialize object module loader */

#if	defined(INCLUDE_AOUT)
    bootAoutInit ();				/* use a.out format */
#else	/* coff or ecoff */
#if	defined(INCLUDE_ECOFF)
    bootEcoffInit ();				/* use ecoff format */
#else	/* coff */
#if	defined(INCLUDE_COFF)
    bootCoffInit ();				/* use coff format */
#else   /* coff */
#if	defined(INCLUDE_ELF)
    bootElfInit ();				/* use elf format */
#endif
#endif 						/* mips cpp no elif */
#endif
#endif

#ifdef  INCLUDE_PCMCIA                                       
                                                             
    pcmciaInit ();                      /* init PCMCIA Lib */

#endif /* INCLUDE_PCMCIA */

#ifdef INCLUDE_MUX
    /* initialize the MUX */

    if (muxLibInit() == ERROR)
	return;                              /* can't return ERROR */
#endif /* INCLUDE_MUX */

    taskSpawn ("tBoot", bootCmdTaskPriority, bootCmdTaskOptions,
		bootCmdTaskStackSize, (FUNCPTR) bootCmdLoop,
		0,0,0,0,0,0,0,0,0,0);

    }

/*******************************************************************************
*
* usrClock - user defined system clock interrupt routine
*
* This routine is called at interrupt level on each clock interrupt.  It is
* installed a call to sysClkConnect().  It calls any other facilities that
* need to know about clock ticks, including the kernel itself.
*
* If the application needs anything to happen at clock interrupt level,
* it should be added to this routine.
*
* NOMANUAL
*/

void usrClock (void)

    {
    tickAnnounce ();	/* announce system tick to kernel */
    }

/*******************************************************************************
*
* bootCmdLoop - read and execute user commands forever (until boot)
*/

LOCAL void bootCmdLoop (void)

    {
    BOOT_PARAMS params;
    char line [BOOT_CMD_MAX_LINE];
    char *pLine;
    int nwords;
    int nbytes;
    int value;
    int adr;
    int adr2;
    FUNCPTR entry;
    char key = 0;

    /* flush standard input to get rid of any garbage;
     * E.g. the Heurikon HKV2F gets junk in USART if no terminal connected.
     */

    (void) ioctl (STD_IN, FIOFLUSH, 0 /*XXX*/);

    if (sysStartType & BOOT_CLEAR)
	printBootLogo ();

    usrBootLineInit (sysStartType);


    /* print out any new exception message -
     * the first byte is zeroed after printing so that we won't print
     * it again automatically.  However, 'e' command will still print out
     * the remainder. */

    printExcMsg (sysExcMsg);
    *sysExcMsg = EOS;		/* indicate exception message is old */

    /* start autoboot, unless no-autoboot specified */

    bootStringToStruct (BOOT_LINE_ADRS, &params);
    sysFlags = params.flags;

#if defined(INCLUDE_TSFS_BOOT) && defined(INCLUDE_TSFS_BOOT_VIO_CONSOLE)

    /* Wait for Target Server connection */

    while (!wdbTargetIsConnected())
    	taskDelay (sysClkRateGet());

    /*
     * disable the auto-boot mechanism, because the Target Server may not have
     * time to start its VIO Console before the end of the auto-boot coutdown.
     */

    sysStartType |= BOOT_NO_AUTOBOOT;
#endif

    if (!(sysStartType & BOOT_NO_AUTOBOOT) &&
	!(sysFlags & SYSFLG_NO_AUTOBOOT))
	{
	int timeout = TIMEOUT;

	if ((sysStartType & BOOT_QUICK_AUTOBOOT) ||
	    (sysFlags & SYSFLG_QUICK_AUTOBOOT))
	    {
	    timeout = 1;
	    }

	key = autoboot (timeout);	/* doesn't return if successful */
	}


    /* If we're here, either we aren't auto-booting, or we got an error
     * auto-booting, or the auto-booting was stopped. */

    /* put console in line mode */

    (void) ioctl (consoleFd, FIOSETOPTIONS, OPT_TERMINAL);

    /* read and execute the ROM commands */

    printf ("\n");

    FOREVER
	{
	if (key == '@')
	    {
	    line [0] = key;
	    line [1] = EOS;
	    key = 0;
	    }
	else
	    {
	    printf ("[VxWorks Boot]: ");
	    fioRdString (STD_IN, line, sizeof (line));
	    }

	adr = adr2 = 0;
	nwords = 0;

	/* take blanks off end of line */

	pLine = line + strlen (line) - 1;		/* point at last char */
	while ((pLine >= line) && (*pLine == ' '))
	    {
	    *pLine = EOS;
	    pLine--;
	    }

	pLine = line;
	skipSpace (&pLine);

	switch (*(pLine++))
	    {
	    case EOS:		/* blank line */
		break;

	    case 'd':		/* display */
		if ((getArg (&pLine, &adr, HEX, OPT) == OK) &&
		    (getArg (&pLine, &nwords, DEC, OPT) == OK))
		    d ((char *) adr, nwords);
		break;

	    case 'e':		/* exception */
		printExcMsg (sysExcMsg + 1);
		break;

	    case 'f':		/* fill */
		if ((getArg (&pLine, &adr, HEX, !OPT) == OK) &&
		    (getArg (&pLine, &nbytes, DEC, !OPT) == OK) &&
		    (getArg (&pLine, &value, DEC, !OPT) == OK))
		    {
		    bfillBytes ((char *) adr, nbytes, value);
		    }

		break;

	    case 't':		/* transpose(?) (running out of letters!) */
		if ((getArg (&pLine, &adr, HEX, !OPT) == OK) &&
		    (getArg (&pLine, &adr2, HEX, !OPT) == OK) &&
		    (getArg (&pLine, &nbytes, HEX, !OPT) == OK))
		    {
		    bcopy ((char *) adr, (char *) adr2, nbytes);
		    }
		break;

	    case 'm':		/* modify */
		if (getArg (&pLine, &adr, HEX, !OPT) == OK)
		    m ((char *) adr);
		break;

#ifdef	TARGET_HK_V2F
	    case 's':		/* system controller */
		{
		extern ULONG sysBCLSet ();

		if (getArg (&pLine, &value, DEC, !OPT) == OK)
		    {
		    if (value != 0)
			{
			(void) sysBCLSet ((ULONG)HK_BCL_SYS_CONTROLLER,
					  (ULONG)HK_BCL_SYS_CONTROLLER);
			printf ("System controller on.\n");
			}
		    else
			{
			(void) sysBCLSet ((ULONG)HK_BCL_SYS_CONTROLLER,
					  (ULONG)0);
			printf ("System controller off.\n");
			}
		    }
		break;
		}
#endif	/* TARGET_HK_V2F */

#if defined(TARGET_FRC_30) || defined(TARGET_FRC_31) || defined(TARGET_FRC_33)
	    case 's':		/* system controller */
		if (getArg (&pLine, &value, DEC, !OPT) == OK)
		    {
		    if (value != 0)
			{
			*FGA_CTL1 |= FGA_CTL1_SCON;
			printf ("System controller on.\n");
			}
		    else
			{
			*FGA_CTL1 &= ~FGA_CTL1_SCON;
			printf ("System controller off.\n");
			}
		    }
		break;
#endif	/* TARGET_FRC_30 || TARGET_FRC_31 || TARGET_FRC_33 */

	    case 'p':		/* print boot params */
		bootParamsShow (BOOT_LINE_ADRS);
		break;

	    case 'c':		/* change boot params */
		bootParamsPrompt (BOOT_LINE_ADRS);
                if (strlen(BOOT_LINE_ADRS) <= BOOT_LINE_SIZE)
                    {
                    if ((sysNvRamSet (BOOT_LINE_ADRS,
                        strlen (BOOT_LINE_ADRS) + 1, 0)) == ERROR)
                        printf("\nNOTE: Bootline not saved to NVRAM\n");
                    }
                else
                    printf("\nWARNING:Bootline longer than BOOT_LINE_SIZE "
			   "(%d bytes). Not saved to NVRAM\n",
			   (int) BOOT_LINE_SIZE);
                break;

	    case 'g':		/* go */
		{
		int entryAddr;
		if (getArg (&pLine, &entryAddr, HEX, !OPT) == OK)
		    {
		    entry = (FUNCPTR)entryAddr;
		    go (entry);
		    }
		}
		break;

#ifdef  INCLUDE_NETWORK
	    case 'n':
		netifAdrsPrint (pLine);
		break;

#ifdef ETHERNET_ADR_SET
	    case 'N':
		mEnet (pLine);
		break;
#endif  /* ETHERNET_ADR_SET */
#endif  /* INCLUDE_NETWORK */

	    case '?':			/* help */
            case 'h':			/* help */
		bootHelp ();
		break;

            case '@':			/* load and go with internal params */
	    case '$':			/* load and go with internal params */

		if (bootLoad (pLine, &entry) == OK)
		    {
		    go (entry);
		    }
		else
		    {
		    taskDelay (sysClkRateGet ());	/* pause a second */
		    reboot (BOOT_NO_AUTOBOOT);		/* something is awry */
		    }
		break;

	    case 'l':			/* load with internal params */

		if (bootLoad (pLine, &entry) == OK)
		    {
		    printf ("entry = 0x%x\n", (int) entry);
		    }
		else
		    {
		    taskDelay (sysClkRateGet ());	/* pause a second */
		    reboot (BOOT_NO_AUTOBOOT);		/* something is awry */
		    }
		break;

            case 'v':                   /* print version & logo */

                printBootLogo ();
                break;

	    default:
		printf ("Unrecognized command. Type '?' for help.\n");
		break;

            } /* switch */
        } /* FOREVER */
    }

/******************************************************************************
*
* autoboot - do automatic boot sequence
*
* RETURNS: Doesn't return if successful (starts execution of booted system).
*/

LOCAL char autoboot 
    (
    int timeout		/* timeout time in seconds */
    )
    {
    ULONG	autoBootTime;
    int		timeLeft;
    UINT	timeMarker;
    int		bytesRead = 0;
    FUNCPTR	entry;
    char	key;

    if (timeout > 0)
	{
	printf ("\nPress any key to stop auto-boot...\n");

	/* Loop looking for a char, or timeout after specified seconds */

	autoBootTime = tickGet () + sysClkRateGet () * timeout;
	timeMarker = tickGet () + sysClkRateGet ();
	timeLeft = timeout;

	printf ("%2d\r", timeLeft);

	 while ((tickGet () < autoBootTime) && (bytesRead == 0))
	    {
	    (void) ioctl (consoleFd, FIONREAD, (int) &bytesRead);

	    if (tickGet () == timeMarker)
		{
		timeMarker = tickGet () + sysClkRateGet ();
		printf ("%2d\r", --timeLeft);
		}
	    }
	}

    if (bytesRead == 0)    /* nothing typed so auto-boot */
	{
	/* put the console back in line mode so it echoes (so's you can bang
	 * on it to see if it's still alive) */

	(void) ioctl (consoleFd, FIOSETOPTIONS, OPT_TERMINAL);

	printf ("\nauto-booting...\n\n");

	if (bootLoad (BOOT_LINE_ADRS, &entry) == OK)
	    go (entry);				/* ... and never return */
	else
	    {
	    printf ("Can't load boot file!!\n");
	    taskDelay (sysClkRateGet ());	/* pause a second */
	    reboot (BOOT_NO_AUTOBOOT);		/* something is awry */
	    }
	}
    else
	{
	/* read the key that stopped autoboot */

	read (consoleFd, &key, 1);
	return (key & 0x7f);		/* mask off parity in raw mode */
	}

    return (ERROR);			/* for lint - can't really get here */
    }

/******************************************************************************
*
* printBootLogo - print initial boot banner page
*/

LOCAL void printBootLogo (void)

    {
    printf ("\n\n\n\n\n\n\n\n\n\n\n");
    printf ("%28s%s", "","VxWorks System Boot");
    printf ("\n\n\nCopyright 1984-2004  Wind River Systems, Inc.\n\n\n\n\n\n");
    printf ("CPU: %s\n", sysModel ());
    printf ("Version: %s\n", vxWorksVersion);
    printf ("BSP version: " BSP_VERSION BSP_REV "\n");
    printf ("Creation date: %s\n\n", creationDate);
    }

/*******************************************************************************
*
* bootHelp - print brief help list
*/

LOCAL void bootHelp (void)

    {
    static char *helpMsg[] =
	{
	"?",                      "- print this list",
	"@",                      "- boot (load and go)",
	"p",                      "- print boot params",
	"c",                      "- change boot params",
	"l",                      "- load boot file",
	"g adrs",                 "- go to adrs",
	"d adrs[,n]",             "- display memory",
	"m adrs",                 "- modify memory",
	"f adrs, nbytes, value",  "- fill memory",
	"t adrs, adrs, nbytes",   "- copy memory",
	"e",                      "- print fatal exception",
        "v",                      "- print boot logo with version",
#ifdef INCLUDE_NETWORK
	"n netif",	  	  "- print network interface device address",
#if defined(ETHERNET_ADR_SET)
	"N",			  "- set ethernet address",
#endif  /* ETHERNET_ADR_SET */
#endif  /* INCLUDE_NETWORK */
#if defined(TARGET_HK_V2F) || defined(TARGET_FRC_30) || \
    defined(TARGET_FRC_31) || defined(TARGET_FRC_33)
	"s [0/1]",                "- system controller 0 = off, 1 = on",
#endif	/* TARGET_HK_V2F/FRC_30/FRC_31/FRC_33 */
	"$dev(0,procnum)host:/file h=# e=# b=# g=# u=usr [pw=passwd] f=#", "",
	"                          tn=targetname s=script o=other", "",

#ifdef	INCLUDE_SCSI_BOOT
	"boot device: scsi=id,lun              file name: /sd0/vxWorks","",
#endif	/*INCLUDE_SCSI_BOOT*/

#ifdef	INCLUDE_FD
	"boot device: fd=drive,fdType          file name: /fd0/vxWorks","",
#endif	/* INCLUDE_FD */

#ifdef	INCLUDE_IDE
	"boot device: ide=drive,configType     file name: /ide0/vxWorks","",
#endif	/* INCLUDE_IDE */

#ifdef	INCLUDE_ATA
	"boot device: ata=ctrl,drive           file name: /ata0/vxWorks","",
#endif	/* INCLUDE_ATA */

#ifdef	INCLUDE_PCMCIA
	"boot device: pcmcia=sock              file name: /pcmcia0/vxWorks","",
#endif	/* INCLUDE_PCMCIA */

#ifdef	INCLUDE_TFFS
	"boot device: tffs=drive,removable     file name: /tffs0/vxWorks","",
#endif	/* INCLUDE_TFFS */

#ifdef INCLUDE_TSFS_BOOT
	"boot device: tsfs                     file name: /tgtsvr/vxWorks","",
#endif /*INCLUDE_TSFS_BOOT */

	"Boot flags:",		  "",
#if defined(TARGET_HK_V2F) || defined(TARGET_FRC_30) || \
    defined(TARGET_FRC_31) || defined(TARGET_FRC_33)
	"  0x01  - don't be system controller",	"",
#endif	/* TARGET_HK_V2F/FRC_30/FRC_31/FRC_33 */
	"  0x02  - load local system symbols",		"",
	"  0x04  - don't autoboot",			"",
	"  0x08  - quick autoboot (no countdown)",	"",
#ifdef  INCLUDE_NETWORK
	"  0x20  - disable login security",		"",
#ifdef  INCLUDE_DHCPC
	"  0x40  - use dhcp to get boot parameters",	"",
#else
#ifdef INCLUDE_BOOTP
	"  0x40  - use bootp to get boot parameters",	"",
#else
	"  0x40  - autoconfigure: NOT AVAILABLE (no method installed)",	"",
#endif /* INCLUDE_BOOTP */
#endif /* INCLUDE_DHCPC */
	"  0x80  - use tftp to get boot image",		"",
	"  0x100 - use proxy arp",			"",
#endif  /* INCLUDE_NETWORK */
	NULL
	};

    FAST char **pMsg;
#ifdef INCLUDE_END
    char       muxDevName[8];
    END_TBL_ENTRY * pDevTbl;
#endif /* INCLUDE_END */

    printf ("\n");

    for (pMsg = helpMsg; *pMsg != NULL; pMsg += 2)
	printf (" %-21s %s\n", *pMsg, *(pMsg + 1));

#ifdef  INCLUDE_NETWORK

    printf ("\navailable boot devices:");

#ifdef INCLUDE_END
    printf ("Enhanced Network Devices\n");

    for (pDevTbl = endDevTbl; pDevTbl->endLoadFunc != END_TBL_END; pDevTbl++)
        {
        if (!pDevTbl->processed)
            {
            muxDevName [0] = EOS;
            pDevTbl->endLoadFunc (muxDevName, NULL);
            printf (" %s%d", muxDevName, pDevTbl->unit);
            }
        }
#endif /* INCLUDE_END */
#endif  /* INCLUDE_NETWORK */

#ifdef	INCLUDE_SCSI_BOOT
    printf (" scsi");
#endif	/*INCLUDE_SCSI_BOOT*/

#ifdef  INCLUDE_FD
    printf (" fd");
#endif  /* INCLUDE_FD */

#ifdef  INCLUDE_IDE
    printf (" ide");
#endif  /* INCLUDE_IDE */

#ifdef  INCLUDE_ATA
    printf (" ata");
#endif  /* INCLUDE_ATA */

#ifdef  INCLUDE_TFFS
    printf (" tffs");
#endif  /* INCLUDE_TFFS */

#ifdef  INCLUDE_TSFS_BOOT
    printf (" tsfs");
#endif  /* INCLUDE_TSFS_BOOT */

    printf ("\n");
    }

#ifdef INCLUDE_NETWORK

#ifdef INCLUDE_BOOTP
/******************************************************************************
*
* bootpGet - get boot parameters via BOOTP.
*
* This routine retrieves a boot file name, host and target IP addresses, and 
* subnet mask from a BOOTP server, using the bootstrap protocol defined in
* RFC 1542. The IP address and subnet mask values will only be stored in the 
* boot parameters if not already specified. In order to use BOOTP, the boot 
* device indicated by <pNetDev> must be capable of sending broadcast messages. 
* Currently, only Ethernet devices and the shared-memory network drivers are 
* supported. To use the shared-memory drivers, the target IP address must 
* already be specified.
* .IP
* The routine is called when the SYSFLG_AUTOCONFIG boot flag is set and the 
* BOOTP client is included in the boot program. If the DHCP client is also
* included, that protocol is used instead.
*
* RETURNS: OK if successful, or ERROR otherwise.
*
* ERRNO: N/A
*
* SEE ALSO: bootpLib, RFC 1542, RFC 951
*/

LOCAL STATUS bootpGet 
    (
    char *pNetDev,		/* boot device */
    char *pBootDevAddr,		/* device address */
    char *pBootFile,		/* file name */
    char *pHostAddr,		/* host address */
    int  *pMask 		/* mask */
    )
    {
#ifndef INCLUDE_DHCPC
    struct bootpParams 		bootParams;     /* parameter descriptor */
    struct in_addr 		clntAddr; 	/* client address */
    struct in_addr 		hostAddr; 	/* server address */
    struct in_addr		subnetMask;	/* subnet mask */
    struct ifnet *              pIf;            /* network boot device */

    char	bootServer [INET_ADDR_LEN];/* boot server */

    subnetMask.s_addr 	      = 0;

    bzero ( (char *)&clntAddr, sizeof (struct in_addr));
    bzero ( (char *)&hostAddr, sizeof (struct in_addr));
    bzero (bootServer, INET_ADDR_LEN);
    bzero ((char *)&bootParams, sizeof (struct bootpParams));

    pIf = ifunit (pNetDev);
    if (pIf == NULL)
	return (ERROR);

    /* Need inet address to boot over the backplane */

    if ( (strncmp (pNetDev, "bp", 2) == 0) || 
            (strncmp (pNetDev, "sm", 2) == 0))
	{
	if (pBootDevAddr [0] == EOS)
	    return (ERROR);

        clntAddr.s_addr = inet_addr (pBootDevAddr);
        if (clntAddr.s_addr == (ULONG)ERROR)
            return (ERROR);
	}

    /* Set pointers to retrieve needed boot parameters. */

    bootParams.netmask = &subnetMask;

    printf ("Getting boot parameters via network interface %s", pNetDev);

    if (bootpLibInit (BOOTP_MAX_HDRSIZE) == ERROR)
        return (ERROR);

    if (bootpParamsGet (pIf, BOOTP_MAX_REQUESTS, &clntAddr, &hostAddr,
		        NULL, pBootFile, &bootParams) == ERROR)
	return (ERROR);

    inet_ntoa_b (hostAddr, bootServer);

    printf ("\nBootp Server:%s\n", bootServer);

    if (pBootFile [0] == EOS)
	return (ERROR);				/* no bootfile */

    printf ("    Boot file: %s\n", pBootFile);

    /* copies to params.had */

    if (pHostAddr [0] == EOS)			/* fill in host address */
	{
	strncpy (pHostAddr, bootServer, INET_ADDR_LEN);
	printf ("    Boot host: %s\n", pHostAddr);
	}

    /*
     * copies to pBootDevAddr (params.ead or params.bad) if not using bp or sm 
     * drivers and address is not already present.
     */

    if (pBootDevAddr [0] == EOS)		/* fill in inet address */
	{
        inet_ntoa_b (clntAddr, pBootDevAddr);
	printf ("    Boot device Addr (%s): %s\n", pNetDev, pBootDevAddr);
	}

    /* copies to netmask */

    if ((*pMask == 0) && (subnetMask.s_addr != 0))
	{
        *pMask = ntohl (subnetMask.s_addr);
	printf ("    Subnet mask: 0x%x\n", *pMask);
	}

    return (OK);
#else
    return (OK);            /* DHCP client used instead. */
#endif /* INCLUDE_DHCPC */
    }
#endif /* INCLUDE_BOOTP */

#ifdef INCLUDE_DHCPC
/******************************************************************************
*
* dhcpGet - get boot parameters with DHCP
*
* This routine retrieves a boot file name, host and target IP addresses, and
* subnet mask from a DHCP or BOOTP server, using the lease negotation process
* defined in RFC 1541. The IP address and subnet mask values will only be
* stored in the boot parameters if not already specified. The DHCP client will
* select the longest offered lease which exceeds the DHCPC_MIN_LEASE value.
* Any DHCP lease will be given preference over BOOTP replies. Unless a
* specific lease duration is provided in the target IP address entry, the
* client requests the lease length defined by DHCPC_DEFAULT_LEASE. The client
* will collect additional DHCP offers until the interval specified by
* DHCPC_OFFER_TIMEOUT expires.
* .IP
* The <pNetDev> argument indicates the network device which will be used to
* send and receive DHCP messages. The DHCP client only supports devices
* attached to the IP protocol with the MUX/END interface. The MTU size of the
* network interface must be large enough to receive an IP datagram of 576
* bytes and the device also must be capable of sending broadcast messages.
* Finally, the target IP address must already be specified to use the
* shared-memory driver.
* .IP
* This routine executes when the SYSFLG_AUTOCONFIG boot flag is set and the
* DHCP client is included in the boot program.
*
* NOTE
* The boot file to be loaded must also contain the DHCP client library in
* order to continue using the assigned target IP address. In addition, the
* DHCP server included with Windows NT does not supply boot file names. If
* that server is used to supply the boot parameters, the boot file name must
* be entered manually.
*
* RETURNS: OK if successful, or ERROR otherwise.
*
* ERRNO: N/A
*
* SEE ALSO: dhcpcBootLib, RFC 1541
*/

LOCAL STATUS dhcpGet
    (
    char *              pNetDev,        /* boot device */
    char *              pBootDevAddr,   /* device IP address */
    char *              pBootFile,      /* boot file name */
    char *              pHostAddr,      /* host IP address */
    int *               pMask,          /* target subnet mask */
    DHCP_LEASE_DATA *   pDhcpLease      /* lease times and addresses */
    )
    {
    STATUS              result;
    struct ifnet *      pIf;            /* pointer to network interface data */
    char                serverAddr [INET_ADDR_LEN];   /* DHCP server address */
    char                bootFile [BOOT_FILE_LEN];       /* name of boot file */
    int                 subnetMask;                     /* subnet mask */
    void *              pCookie;
    unsigned long       duration;

    struct dhcp_param bootParams;

    bzero (serverAddr, INET_ADDR_LEN);
    bzero ((char *) &bootParams, sizeof (struct dhcp_param));

    /*
     * Using pBootFile directly only works if all the DHCP servers supply a
     * bootfile. The Windows NT server does not, so we can't do this.
     */
    /* bootParams.file = pBootFile;  - Desired assignment to save memory. */

    bootParams.file = bootFile;
    bootParams.subnet_mask = (struct in_addr *) &subnetMask;

    pIf = ifunit (pNetDev);
    if (pIf == NULL)
        return (ERROR);

    printf ("Getting boot parameters via network interface %s.\n", pNetDev);

    /* Setup client to retrieve address information from a DHCP server. */

    pCookie = dhcpcBootInit (pIf, DHCPC_SPORT, DHCPC_CPORT, DHCPC_MAX_MSGSIZE,
                             DHCPC_OFFER_TIMEOUT, DHCPC_DEFAULT_LEASE,
                             DHCPC_MIN_LEASE);
    if (pCookie == NULL)
        {
        printf ("Error initializing DHCP client.\n");
        return (ERROR);
        }

#if (DHCPC_DISCOVER_RETRIES)
    dhcpcDiscoverRetries = DHCPC_DISCOVER_RETRIES;
#endif

    /* 
     * Set requested lease length to value from bootline, and
     * make sure we specify it in network byte order.
     */

    duration = htonl (pDhcpLease->lease_duration);
    dhcpcOptionAdd (pCookie, _DHCP_LEASE_TIME_TAG, sizeof (int),
                    (UCHAR *) &duration);

    if (pBootDevAddr [0] == EOS)
        {
        /* Attempt to retrieve address information from a DHCP server. */

        result = dhcpcBootBind ();
        if (result != OK)
            return (ERROR);
        }
    else
        {
        /*
         * Externally configured address. Get any additional parameters.
         * Ignore any failure (since the network device can be configured)
         * as long as a boot file is available.
         */

        result = dhcpcBootInformGet (pBootDevAddr);

        if (result != OK)
            {
            if (pBootFile[0] == EOS)
                return (ERROR);
            else
                return (OK);
            }
        }

    result = dhcpcBootParamsGet (&bootParams);
    if (result == ERROR)
        return (ERROR);

    /* Fill in configuration parameters for storage in bootline. */

    if (pBootDevAddr[0] == EOS)
        {
        /*
         * If the DHCP process established a lease (which includes an IP
         * address assignment), get the assigned address and timestamp
         * values. This information is not available if an address is
         * assigned externally. (A DHCP inform message is sent in that case).
         */

        bcopy ((char *) &bootParams.yiaddr, (char *) &pDhcpLease->yiaddr,
               sizeof (struct in_addr));
        pDhcpLease->lease_duration = bootParams.lease_duration;
        pDhcpLease->lease_origin = bootParams.lease_origin;
        }

    inet_ntoa_b (bootParams.server_id, serverAddr);
    printf ("\nDHCP Server:%s\n", serverAddr);

    if (pBootFile [0] == EOS && bootFile[0] == EOS)
        return (ERROR);                         /* no bootfile */

    if (bootFile[0] != EOS)                     /* Save new bootfile */
        {
        bcopy (bootFile, pBootFile, BOOT_FILE_LEN);
        printf ("    Boot file: %s\n", pBootFile);
        }

    if (pHostAddr [0] == EOS)                   /* fill in host address */
        {
        inet_ntoa_b (bootParams.siaddr, pHostAddr);
        printf ("    Boot host: %s\n", pHostAddr);
        }

    /*
     * Fill in the target's IP address, if needed. The status
     * variable indicates the source of the IP address as follows:
     *    DHCP_NATIVE - assigned by a DHCP server
     *    DHCP_BOOTP - issued by a BOOTP server
     *    DHCP_MANUAL - entered in boot parameters
     */

    if (pBootDevAddr [0] == EOS)                /* fill in inet address */
        {
        /*
         * Use the IP address from the DHCP protocol.
         * The status variable has already been set.
         */

        inet_ntoa_b (bootParams.yiaddr, pBootDevAddr);
        printf ("    Boot device Addr (%s): %s\n", pNetDev, pBootDevAddr);
        }

    if ((*pMask == 0) && (subnetMask != 0))
        {
        *pMask = ntohl (subnetMask);
        printf ("    Subnet mask: 0x%x\n", *pMask);
        }

    return (OK);
    }
#endif /* INCLUDE_DHCPC */

/*******************************************************************************
*
* usrNetPoolConfig - initialize the network memory pools
*
* This routine allocates memory for and initializes the network memory pools.
*
* RETURNS: N/A
*/

LOCAL void usrNetPoolConfig ()
    {
    netPoolDefaultConfigParams.cfgh.len = sizeof (NET_POOL_CONFIG_PARAMS);

    netPoolDefaultConfigParams.sysMblkClBlkConf = &sysMblkClBlkDesc;
    netPoolDefaultConfigParams.sysClDesc = &sysClDescTbl [0];
    netPoolDefaultConfigParams.sysClDescNum = sysClDescTblNumEnt;

    netPoolDefaultConfigParams.pktMblkClBlkConf = &pktMblkClBlkDesc;
    netPoolDefaultConfigParams.pktClDesc = &pktClDescTbl [0];
    netPoolDefaultConfigParams.pktClDescNum = pktClDescTblNumEnt;

    /* initialize _pNetDPool, _pNetSysPool */

    mbinit (&netPoolDefaultConfigParams);
    }

/*******************************************************************************
*
* usrNetInit - initializes static tables for network stack
*
* This routine initializes static tables for the network stack.
* 
* RETURNS: OK, or ERROR if initialization fails
*/

LOCAL STATUS usrNetInit ()
    {
    int status;

    /* set up the default configuration parameters */

    status = netCoreSetup (MAX_LINKHDR_CFG, MAX_PROTOHDR_CFG,
			   NMBCLUSTERS_CFG, IP_MAX_UNITS);
    if (status == ERROR)
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* usrNetworkInit - bootrom version of network initialization
*
* This routine initializes all network components that are necessary for
* booting vxWorks.
*
* The bootrom version of network initialization is slightly different from
* the project facility version of InitGroup usrNetworkInit or the one in
* usrNetwork.c regarding to the following:
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS usrNetworkInit (void)
    {
#ifdef INCLUDE_DHCPC
    char netDev [BOOT_DEV_LEN + 1];
#endif
#ifdef INCLUDE_BOOTP
    char netDev [BOOT_DEV_LEN + 1];
#endif

    /* by default print messages during network initialization */

    _func_printErr = (FUNCPTR) printf;

#ifdef INCLUDE_NET_SYSCTL
    SysctlInstInit (NULL);
#endif /* INCLUDE_NET_SYSCTL */

#ifdef INCLUDE_APPL_LOG_UTIL
    usrNetApplUtilInit ();
#endif

    usrNetBoot ();				/* INCLUDE_NET_BOOT */

    /* check if booting with shared memory driver */

#ifdef INCLUDE_SM_NET    
    if (strncmp (sysBootParams.bootDev, "sm", 2) == 0)
       {
       if (sysProcNumGet () == 0)
	  {
          printf ("Error: processor number must be non-zero to boot from sm\n");
	  return (ERROR);
          }

	pAddrString = sysBootParams.bad;
	backplaneBoot = TRUE;
        }
#endif /* INCLUDE_SM_NET */

    usrNetmaskGet ();                           /* INCLUDE_NETMASK_GET */

    if (usrNetInit (NULL) == ERROR)		/* INCLUDE_NET_INIT */
	return (ERROR);

    netBufLibInit ();				/* INCLUDE_NETBUFLIB */
#ifdef INCLUDE_NETBUFPOOL
    netBufPoolInit ();				/* INCLUDE_NETBUFPOOL */
#endif
#ifdef INCLUDE_LINKBUFPOOL
    linkBufPoolInit ();				/* INCLUDE_LINKBUFPOOL */
#endif

    if (usrNetDaemonInit (NET_JOB_NUM_CFG,	/* INCLUDE_NET_DAEMON */
		          NET_TASK_PRIORITY,
		          NET_TASK_OPTIONS,
		          NET_TASK_STACKSIZE) == ERROR)
        return (ERROR);

    if (usrNetGtfInit () == ERROR)		/* INCLUDE_GTF */
	return (ERROR);

    usrNetPoolConfig ();	                /* INCLUDE_NET_POOL */

    usrNetRouteStorageInit ();                  /* INCLUDE_ROUTE_STORAGE */

					/* InitGroup usrNetSocketInit */
    if (sockLibInit () == ERROR)		/* INCLUDE_SOCKLIB */
	return (ERROR);

    if (usrNetBsdSocket () == ERROR)		/* INCLUDE_BSD_SOCKET */
	return (ERROR);

					/* InitGroup usrNetProtoInit */
    usrNetIfInit ();                            /* INCLUDE_IF */
    usrNetIpv4Init ();				/* INCLUDE_IPV4 */
    usrNetIcmpv4Init ();			/* INCLUDE_ICMPV4 */

#ifdef INCLUDE_UDPV4
    usrNetUdpv4Init ();
#endif

#ifdef INCLUDE_TCPV4
    usrNetTcpv4Init ();
#endif

    usrNetRawv4Init ();			        /* INCLUDE_RAWV4 */

#ifdef INCLUDE_IGMP
    usrNetIgmpInit ();
#endif

    usrNetRtSockInit ();			/* INCLUDE_ROUTING_SOCKET */

					/* InitGroup usrNetMib2Init */
    if (m2IfInit (MIB2IF_TRAP_RTN,	        /* INCLUDE_MIB2_IF */
	          (void *) MIB2IF_TRAP_ARG) == ERROR)
        return (ERROR);

    domaininit (NULL);				/* INCLUDE_DOMAIN_INIT */
    route_init ();				/* INCLUDE_ROUTE */
    usrNetGtfTimerStart ();			/* INCLUDE_GTF_TIMER_START */

					/* InitGroup usrNetDevStart */
    usrNetLoopbackStart ();	                /* INCLUDE_LOOPBACK */

#ifdef INCLUDE_ETHERNET
    if (usrNetEther () == ERROR)		/* INCLUDE_ETHERNET */
	return (ERROR);
#endif

#ifdef INCLUDE_END
    usrNetEndLibInit ();			/* INCLUDE_END */
#endif

#ifdef INCLUDE_BPF
    if (usrNetBpfDrvInit () == ERROR)
	return (ERROR);
#endif

    if (strncmp (sysBootParams.bootDev, "ppp", 3) == 0)
        {
        /* booting via ppp */
#ifdef INCLUDE_VXWORKS_5_X_EQUIV_PPP
        usrBasicPPPFrameworkInit ();

        printf ("Attaching network interface ppp...\n");

        pppBootHandle = usrWindNetPPPInit (sysBootParams.bootDev,
                                           sysBootParams.ead,
                                           ((sysBootParams.gad[0] == EOS)?
                                            sysBootParams.had:
                                            sysBootParams.gad));
        if (pppBootHandle == NULL)
            return (ERROR);

        rebootHookAdd ((FUNCPTR) pppRebootHook);
#else
        printf ("\nError: PPP not included.\n");
        return (ERROR);
#endif /* INCLUDE_VXWORKS_5_X_EQUIV_PPP */
        }
    else
        {
        usrNetIpAttachCommon (pDevName, uNum,	/* INCLUDE_IPATTACH */
    			      "IPv4", ipAttach);

#ifdef INCLUDE_SM_NET
        /*
         * Shared memory slaves do not require an address from the boot
         * parameters. If the backplane address is not given, those devices
         * will retrieve an address from the shared memory master.
         */

        if (backplaneBoot) 
            {
            if (sysBootParams.bad [0] == EOS &&
                (smEndInetAddrGet ("sm", uNum, NONE, sysBootParams.bad) == OK))
                {
                printf ("SM backplane IP address: %s\n", sysBootParams.bad);
                }
     
            if (sysBootParams.bad [0] == EOS)
                {
                printf ("Error: no backplane address specified.\n");
                return (ERROR);
                }

            /* get gateway address if no specified */

            if ((sysBootParams.gad [0] == EOS) && !(sysFlags & SYSFLG_PROXY))
                {
                struct in_addr host;    /* Internet Address */
                struct in_addr backpl;  /* Internet Address */

                host.s_addr = inet_addr (sysBootParams.had);
                backpl.s_addr = inet_addr (sysBootParams.bad);

                if (in_netof(host) != in_netof(backpl))
                    {
                    /* Gateway address is available (assumed to be master) */

                    if (smEndInetAddrGet ("sm", uNum,
                                          0, sysBootParams.gad) == OK)
                        printf ("Gateway inet address: %s\n",
                                sysBootParams.gad);
                    }
                }
            }
#endif /* INCLUDE_SM_NET */
                                        /* InitGroup usrNetworkAddrInit */
        if (sysFlags & SYSFLG_AUTOCONFIG)
	    {
#ifdef INCLUDE_DHCPC
            sprintf (netDev, "%s%d", pDevName, uNum);

            if (dhcpGet (netDev, pAddrString, sysBootParams.bootFile, 
		         sysBootParams.had, &netmask, &dhcpLease) == ERROR)
                return (ERROR);
#else
#ifdef INCLUDE_BOOTP
            sprintf (netDev, "%s%d", pDevName, uNum);

            if (bootpGet (netDev, pAddrString, sysBootParams.bootFile, sysBootParams.had,
                         &netmask) == ERROR)
                return (ERROR);
#else
            if (_func_printErr)
	        (*_func_printErr) ("automatic address assignment requested but not included.\n");
            return (ERROR);
#endif
#endif
            }

        if (usrNetBootConfig (pDevName, uNum,	/* INCLUDE_NET_BOOT_CONFIG */
                              pAddrString, netmask,
                              sysBootParams.gad) == ERROR)
            return (ERROR);
        }

#ifdef INCLUDE_HOST_TBL
    usrNetHostTblSetup ();
#endif

/* InitGroup usrNetRemoteInit */

#ifdef INCLUDE_REMLIB
    remLibInit (RSH_STDERR_SETUP_TIMEOUT);
#endif

#ifdef INCLUDE_NET_REM_IO
    usrNetRemoteCreate ();			/* INCLUDE_NET_REMIO, NET_DRV */
#endif
					/* InitGroup usrNetAppInit */
#ifdef INCLUDE_FTP
    usrFtpInit ();
#endif
    return (OK);
    }

#endif /* INCLUDE_NETWORK */

/*******************************************************************************
*
* bootLoad - load a module into memory
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS bootLoad 
    (
    char *   bootString,
    FUNCPTR *pEntry
    )
    {
#ifdef  INCLUDE_SCSI_BOOT
#ifdef  INCLUDE_SCSI2
     SCSI_OPTIONS	options;
     UINT		which;
     int		devBusId;
#endif  /* INCLUDE_SCSI2 */
#endif  /* INCLUDE_SCSI_BOOT */
#ifdef  INCLUDE_NETWORK
    int 		result; 	/* classification of address string */
    unsigned long 	leaseLen; 	/* lease length field (optional) */
    char                buf [30];
    char 		bootDev [BOOT_DEV_LEN];
    char *		pBootAddr;
    IMPORT int          netTaskPriority;
    int                 oldTaskPriority;
    STATUS              status;

#endif  /* INCLUDE_NETWORK */

    /* copy bootString to low mem address, if specified */

    if ((bootString != NULL) && (*bootString != EOS))
	strcpy (BOOT_LINE_ADRS, bootString);

    /* interpret boot command */

    if (usrBootLineCrack (BOOT_LINE_ADRS, &sysBootParams) != OK)
	return (ERROR);

    /* Display boot parameters */

    bootParamsShow (BOOT_LINE_ADRS);

    /* set our processor number: may establish vme access, etc. */

    sysFlags = sysBootParams.flags;
    sysProcNumSet (sysBootParams.procNum);

#ifdef	INCLUDE_SCSI_BOOT

    /*
     * initialize either the SCSI1 or SCSI2 interface; initialize SCSI2 when
     * the SCSI2 interface is available.
     */

#ifndef INCLUDE_SCSI2
    scsi1IfInit ();
#else
    scsi2IfInit ();
#endif

    if (strncmp (sysBootParams.bootDev, "scsi", 4) == 0)
	{
	int bootDevId = NONE;
	int bootDevLUN = NONE;

#ifdef INCLUDE_SCSI2
        /* Set all devices to asynchronous data transfer */

        which = SCSI_SET_OPT_XFER_PARAMS;
        options.maxOffset = 0;
        options.minPeriod = SCSI_SYNC_XFER_MIN_PERIOD;

        for (devBusId = 0; devBusId < 8; devBusId++)
            {
            scsiTargetOptionsSet (pSysScsiCtrl, devBusId, &options, which);
            }
#endif /* INCLUDE_SCSI2 */

	/* check for absence of bus ID and LUN, in which case
	 * auto-configure and display results
	 */

	if (strlen (sysBootParams.bootDev) == 4)
	    {
	    if (!scsiInitialized)
		{
	    	if (sysScsiInit () == ERROR)
		    {
		    printErr ("Could not initialize SCSI.\n");
		    return (ERROR);
		    }
		scsiInitialized = TRUE;
		}

	    scsiAutoConfig (pSysScsiCtrl);
	    scsiShow (pSysScsiCtrl);

	    /* return ERROR to indicate that no file was loaded */
	    return (ERROR);
	    }

	sscanf (sysBootParams.bootDev, "%*4s%*c%d%*c%d", &bootDevId, &bootDevLUN);

	if (scsiLoad (bootDevId, bootDevLUN, sysBootParams.bootFile, pEntry) != OK)
	    {
	    printErr ("\nError loading file: errno = 0x%x.\n", errno);
	    return (ERROR);
	    }

	return (OK);
	}

#endif	/* INCLUDE_SCSI_BOOT */

#ifdef  INCLUDE_FD

    if (strncmp (sysBootParams.bootDev, "fd", 2) == 0)
	{
	int type = 0;
	int drive = 0;

	if (strlen (sysBootParams.bootDev) == 2)
	    return (ERROR);
	else
	    sscanf (sysBootParams.bootDev, "%*2s%*c%d%*c%d", &drive, &type);

	if (fdLoad (drive, type, sysBootParams.bootFile, pEntry) != OK)
	    {
	    printErr ("\nError loading file: errno = 0x%x.\n", errno);
	    return (ERROR);
	    }

	return (OK);
	}

#endif  /* INCLUDE_FD */

#ifdef	INCLUDE_IDE

    if (strncmp (sysBootParams.bootDev, "ide", 3) == 0)
	{
	int type = 0;
	int drive = 0;

	if (strlen (sysBootParams.bootDev) == 3)
	    return (ERROR);
	else
	    sscanf (sysBootParams.bootDev, "%*3s%*c%d%*c%d", &drive, &type);

	if (ideLoad (drive, type, sysBootParams.bootFile, pEntry) != OK)
	    {
	    printErr ("\nError loading file: errno = 0x%x.\n", errno);
	    return (ERROR);
	    }

	return (OK);
	}

#endif	/* INCLUDE_IDE */

#ifdef	INCLUDE_ATA

    if (strncmp (sysBootParams.bootDev, "ata", 3) == 0)
	{
	int ctrl  = 0;
	int drive = 0;

	if (strlen (sysBootParams.bootDev) == 3)
	    return (ERROR);
	else
	    sscanf (sysBootParams.bootDev, "%*3s%*c%d%*c%d", &ctrl, &drive);

	if (ataLoad (ctrl, drive, sysBootParams.bootFile, pEntry) != OK)
	    {
	    printErr ("\nError loading file: errno = 0x%x.\n", errno);
	    return (ERROR);
	    }

	return (OK);
	}

#endif	/* INCLUDE_ATA */

#ifdef	INCLUDE_PCMCIA

    if (strncmp (sysBootParams.bootDev, "pcmcia", 6) == 0)
	{
	int sock	= NONE;

	if (strlen (sysBootParams.bootDev) == 6)
	    return (ERROR);
	else
	    sscanf (sysBootParams.bootDev, "%*6s%*c%d", &sock);

	if (pcmciaLoad (sock, sysBootParams.bootFile, pEntry) == OK)
	    return (OK);

	/* fall through if the PC card is not a block device.
	 * let's try to boot it from an ethernet device.
	 */

	printErr ("\nCannot load from PCMCIA block device."
	          "\nTrying alternate PCMCIA devices ... ");
	}

#endif	/* INCLUDE_PCMCIA */

#ifdef  INCLUDE_TFFS

    if (strncmp (sysBootParams.bootDev, "tffs", 4) == 0)
	{
	int drive = 0;
	int removable = 0;

	if (strlen (sysBootParams.bootDev) == 4)
	    return (ERROR);
	else
	    sscanf (sysBootParams.bootDev, "%*4s%*c%d%*c%d", &drive, &removable);

	/* tffsLoad () should be after pcmciaInit () */

	if (tffsLoad (drive, removable, sysBootParams.bootFile, pEntry) != OK)
	    {
	    printErr ("\nError loading file: errno = 0x%x.\n", errno);
	    return (ERROR);
	    }

	return (OK);
	}

#endif  /* INCLUDE_TFFS */

#ifdef  INCLUDE_TSFS_BOOT

    if (strncmp (sysBootParams.bootDev, "tsfs", 4) == 0)
	{
	if (tsfsLoad (sysBootParams.bootFile, pEntry) != OK)
	    {
	    printErr ("\nError loading file: errno = 0x%x.\n", errno);
	    return (ERROR);
	    }

	return (OK);
	}

#endif  /* INCLUDE_TSFS_BOOT */

#ifndef  INCLUDE_NETWORK

    printf ("\nError loading file: networking code not present.\n");
    return (ERROR);
    }

#else  /* INCLUDE_NETWORK */

    /* verify unsupported booting devices */

    if (strncmp (sysBootParams.bootDev, "sl", 2) == 0)
        {
	printf ("booting via slip is unsupported.\n");
        return (ERROR);
        }

    strncpy (bootDev, sysBootParams.bootDev, sizeof (bootDev));

    /* attach and configure boot interface */

    pBootAddr = sysBootParams.ead;

#ifdef INCLUDE_SM_NET
    if ((strncmp (sysBootParams.bootDev, "bp", 2) == 0) ||
        (strncmp (sysBootParams.bootDev, "sm", 2) == 0))
        {
        backplaneBoot = TRUE;
        pBootAddr = sysBootParams.bad;
        }
#endif /* INCLUDE_SM_NET */

    /* Save requested lease length, if any. Ignore lease origin value. */

    result = bootLeaseExtract (pBootAddr, &leaseLen, NULL);
    if (result < 0)
        {
        printf ("Error reading target address information.\n");
        return (ERROR);
        }

    /* Handle any lease information attached to the address entry. */

    if (result == 2)
        {
        /* 
         * The current address contains both a duration value and a start
	 * time, indicating that it was assigned by a DHCP server.
	 */

	/* Handle finite and infinite address assignment. */

        if (sysFlags & SYSFLG_AUTOCONFIG)
            *pBootAddr = EOS;    /* Remove for later replacement. */
        else
            {
            /* 
             * Technically, this address is invalid since it contains
             * a finite interval that requires DHCP for verification
             * and the automatic configuration flag is not set.
             * However, this situation can only occur if caused 
             * deliberately by the user. So, just ignore the timing
             * information and assign the address permanently.
             */

            result = 0;    /* Prevents restoration of time values. */
            }
    	}

#ifdef INCLUDE_DHCPC
    /* Set the DHCP lease information, if needed. */

    if (sysFlags & SYSFLG_AUTOCONFIG)
        {
        /* Save the requested lease length if entered by the user. */

        if (result == 1)
            dhcpLease.lease_duration = leaseLen;
        else
            {
            /*
             * The lease length is either not present or left over from
             * an earlier lease. Use the default value.
             */

            dhcpLease.lease_duration = DHCPC_DEFAULT_LEASE;
            }
        dhcpLease.lease_origin = 0;
        }
#endif /* INCLUDE_DHCPC */

    /*
     * Initialize network stack. Once usrNetworkInit is completed,
     * the bootrom can start loading the vxWorks image via network.
     * Note that MUX is already initialized in usrRoot().
     */

    if (usrNetworkInit() == ERROR)
	return (ERROR);

    if (netmask != 0)               /* reconstruct address with mask */
	{
	sprintf (buf, ":%x", netmask);
	strcat  (pBootAddr, buf);
	}

    /*
     * If a value was specified for the requested lease length and the
     * address information was not replaced by automatic configuration,
     * restore that value for use by later reboots.
     */

    if (!(sysFlags & SYSFLG_AUTOCONFIG) && result == 1)
        {
        /*
         * The value is ignored if <result> is 2, since that is only
         * possible at this point for permanent DHCP assignments,
         * which need no special processing by later reboots.
         */

        if (netmask == 0)   /* Create empty netmask field. */
            sprintf (buf, "::%lx", leaseLen);
        else                /* Append requested lease length to netmask. */
            sprintf(buf, ":%lx", leaseLen);

        strcat (pBootAddr, buf);
        }
 
#ifdef INCLUDE_DHCPC
    /*
     * If the target IP address was assigned by a DHCP server, append
     * the lease times. The presence of those fields in the address string
     * will cause the runtime image to renew the corresponding lease.
     */

    if (sysFlags & SYSFLG_AUTOCONFIG)
        {
        /* Add lease origin and lease duration if needed. */

        if (dhcpcBindType == DHCP_NATIVE)
            {
            if (netmask == 0)       /* Create empty netmask field. */
                sprintf (buf, "::%lx:%lx", dhcpLease.lease_duration,
                         dhcpLease.lease_origin);
            else            /* Append lease timestamps to netmask. */
                sprintf (buf, ":%lx:%lx", dhcpLease.lease_duration,
                         dhcpLease.lease_origin);
            strcat (pBootAddr, buf);
            }
        }
#endif    /* INCLUDE_DHCPC */

    bootStructToString (BOOT_LINE_ADRS, &sysBootParams);

    /* load specified file */

    taskPriorityGet (0, &oldTaskPriority);
    taskPrioritySet (0, netTaskPriority + 1);

    if (netLoad (sysBootParams.had, sysBootParams.bootFile, sysBootParams.usr,
		 sysBootParams.passwd, pEntry) != OK)
	{
	printf ("\nError loading file: errno = 0x%x.\n", errno);
	status = ERROR;
	}
    else
	status = OK;

    taskPrioritySet (0, oldTaskPriority);

#ifdef INCLUDE_VXWORKS_5_X_EQUIV_PPP
    if ((strncmp (sysBootParams.bootDev, "ppp", 3) == 0) &&
        ((pppBootHandle->status & PPP_LCP_OPENED) != 0))
        {
        if (ppp5xDisconnect (pppBootHandle) == ERROR ||
            pppBootHandle->syncSem == NULL)
            return (ERROR);

        if (semTake (pppBootHandle->syncSem,
            PPP_DISCONNECT_DELAY * sysClkRateGet()) == ERROR)
            {
            printf ("PPP: timeout: could not disconnect the link.\n");
            return (ERROR);
            }

        printf ("PPP: link disconnected OK\n");

        }
#endif /* INCLUDE_VXWORKS_5_X_EQUIV_PPP */

    return (status);
    }

/*******************************************************************************
*
* netLoad - downLoad a file from a remote machine via the network.
*
* The remote shell daemon on the machine 'host' is used to download
* the given file to the specified previously opened network file descriptor.
* The remote userId should have been set previously by a call to iam().
* If the file does not exist, the error message from the Unix 'host'
* is printed to the VxWorks standard error fd and ERROR is returned.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS netLoad 
    (
    char *hostName,
    char *fileName,
    char *usr,
    char *passwd,
    FUNCPTR *pEntry
    )
    {
    int fd;
    int errFd;		/* for receiving standard error messages from Unix */
    BOOL bootFtp = (passwd[0] != EOS);
    BOOL bootRsh = FALSE;

#ifdef INCLUDE_REMLIB
    char command [BOOT_FILE_LEN + BOOT_HOST_LEN];
#endif

    printf ("Loading... ");

#ifdef INCLUDE_TFTP_CLIENT
    if (sysFlags & SYSFLG_TFTP)		/* use tftp to get image */
        {
	if (tftpXfer (hostName, 0, fileName, "get", "binary", &fd,
		      &errFd) == ERROR)
	    return (ERROR);
	}

   else
#endif
       {
	if (bootFtp)
	    {
#ifdef INCLUDE_FTP
	    if (ftpXfer (hostName, usr, passwd, "", "RETR %s", "", fileName,
		         &errFd, &fd) == ERROR)
#endif
		return (ERROR);
	    }
	else
	    {
	    bootRsh = TRUE;
#ifdef INCLUDE_REMLIB
	    sprintf (command, "cat %s", fileName);

	    fd = rcmd (hostName, RSHD, usr, usr, command, &errFd);
	    if (fd == ERROR)
#endif
		return (ERROR);
	    }
	}

    if (bootLoadModule (fd, pEntry) != OK)
	goto readErr;

#ifdef INCLUDE_TFTP_CLIENT
    /*
     * Successful TFTP transfers don't need any cleanup. The tftpXfer()
     * routine closes all file descriptors once the data has been
     * retrieved from the remote host.
     */

    if (sysFlags & SYSFLG_TFTP) /* used tftp to get image - just exit */
	{

	/* 
	 * Empty the data socket so that the entire file is received from
	 * TFTP server - allows for graceful close on server side 
	 */

        while ((iosFdValue(fd) != ERROR)
	       && (read (fd, command, sizeof (command)) > 0));
        return (OK);
	}
#endif


    if (bootRsh == FALSE)
	{
#ifdef INCLUDE_FTP
	/* Empty the Data Socket before close. PC FTP server hangs otherwise */

	while ((read (fd, command, sizeof (command))) > 0);

        /* close the data socket before checking for a ftp reply (SPR #77169) */

        close (fd);

	if (bootFtp)
	    (void) ftpCommand (errFd, "QUIT",0,0,0,0,0,0);
#endif
	}
    else 
        close (fd);

    close (errFd);
    return (OK);

readErr:
    /* check standard error on Unix */

    if (bootRsh == FALSE)
	{
#ifdef INCLUDE_FTP
	/* Empty the Data Socket before close. PC FTP server hangs otherwise */

	while ((read (fd, command, sizeof (command))) > 0);

        /* close the data socket before checking for a ftp reply (SPR #77169) */

        close (fd);

	if (bootFtp)
	    {
	    (void) ftpReplyGet (errFd, FALSE); /* error message on std. err */
	    (void) ftpCommand (errFd, "QUIT",0,0,0,0,0,0);
	    }
#endif /* INCLUDE_FTP */
	}
    else
	{
	char buf [BOOT_FILE_LEN + BOOT_HOST_LEN];
	int errBytesRecv = fioRead (errFd, buf, sizeof (buf));

	if (errBytesRecv > 0)
	    {
	    /* print error message on standard error fd */

	    buf [errBytesRecv] = EOS;
	    printf ("\n%s:%s: %s\n", hostName, fileName, buf);
	    }

        close (fd);

	}

    close (errFd);

    return (ERROR);
    }

#endif  /* INCLUDE_NETWORK */

#if     (defined (INCLUDE_SCSI_BOOT) || defined (INCLUDE_FD) || \
	 defined (INCLUDE_IDE) || defined (INCLUDE_ATA) || \
	 defined (INCLUDE_TFFS))

#define	SPIN_UP_TIMEOUT	45	/* max # of seconds to wait for spinup */

/******************************************************************************
*
* devSplit - split the device name from a full path name
*
* This routine returns the device name from a valid UNIX-style path name
* by copying until two slashes ("/") are detected.  The device name is
* copied into <devName>.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void devSplit 
    (
    FAST char *fullFileName,	/* full file name being parsed */
    FAST char *devName		/* result device name */
    )
    {
    FAST int nChars = 0;

    if (fullFileName != NULL)
	{
	char *p0 = fullFileName;
	char *p1 = devName;

	while ((nChars < 2) && (*p0 != EOS))
	    {
	    if (*p0 == '/')
		nChars++;

	    *p1++ = *p0++;
	    }
	*p1 = EOS;
	}
    else
	{
	(void) strcpy (devName, "");
	}
    }

#endif  /* (defined (INCLUDE_SCSI_BOOT) || (INCLUDE_FD) || (INCLUDE_IDE)) */

#ifdef  INCLUDE_SCSI_BOOT

/******************************************************************************
*
* scsiLoad - load a vxWorks image from a local SCSI disk
*
* RETURNS: OK, or ERROR if file can not be loaded.
*/

LOCAL STATUS scsiLoad 
    (
    int     bootDevId,
    int     bootDevLUN,
    char    *fileName,
    FUNCPTR *pEntry
    )
    {
    int fd;
    SCSI_PHYS_DEV *pScsiPhysBootDev;
    BLK_DEV       *pScsiBlkBootDev;
    char bootDir  [BOOT_FILE_LEN];
    int           ix;
#ifdef INCLUDE_SCSI2
    SCSI_OPTIONS  options;
    UINT          which;
#endif /* INCLUDE_SCSI2 */
    CBIO_DEV_ID pCbio;


    if (!scsiInitialized)		/* skip if this is a retry */
	{
	if (sysScsiInit () == ERROR)
	    {
	    printErr ("Could not initialize SCSI.\n");
	    return (ERROR);
	    }
	scsiInitialized = TRUE;
	}

    taskDelay (sysClkRateGet ());	/* delay 1 second after reset */

    if ((bootDevId  < SCSI_MIN_BUS_ID) ||
	(bootDevId  > SCSI_MAX_BUS_ID) ||
	(bootDevLUN < SCSI_MIN_LUN)    ||
	(bootDevLUN > SCSI_MAX_LUN))
	{
	printErr ("SCSI device parameters < busId = %d, lun = %d > ",
		  bootDevId, bootDevLUN);
	printErr ("are out of range (0-7).\n");
	printErr ("Check boot device format:\n");
	printErr ("    scsi=<busId>,<lun>  e.g.  scsi=2,0\n");
	return (ERROR);
	}

#ifdef INCLUDE_SCSI2
    /* Set all devices to asynchronous data transfer */

    which = SCSI_SET_OPT_XFER_PARAMS;
    options.maxOffset = 0;
    options.minPeriod = SCSI_SYNC_XFER_MIN_PERIOD;
    scsiTargetOptionsSet (pSysScsiCtrl, bootDevId, &options, which);
#endif /* INCLUDE_SCSI2 */

    /* create device handle for TEST UNIT READY commands */

    if ((pScsiPhysBootDev = scsiPhysDevCreate (pSysScsiCtrl, bootDevId,
					       bootDevLUN, 128, 0, 0,
					       0xffff, 512))
	== NULL)
	{
	printErr ("scsiPhysDevCreate failed.\n");
	return (ERROR);
	}

    /* issue a couple fo TEST UNIT READY commands to clear reset execption */

    scsiTestUnitRdy (pScsiPhysBootDev);
    scsiTestUnitRdy (pScsiPhysBootDev);

    /* issue a TEST UNIT READY every second for SPIN_UP_TIMEOUT seconds,
     * or until device returns OK status.
     */

    if (scsiTestUnitRdy (pScsiPhysBootDev) != OK)
        {
        printf ("Waiting for disk to spin up...");

        for (ix = 0; ix < SPIN_UP_TIMEOUT; ix++)
            {
            if (scsiTestUnitRdy (pScsiPhysBootDev) == OK)
                {
                printf (" done.\n");
                break;
		}
            else
		{
                if (ix != (SPIN_UP_TIMEOUT - 1))
                    printf (".");
                else
                    {
                    printf (" timed out.\n");
                    return (ERROR);
		    }
                taskDelay (sysClkRateGet ());
		}
	    }
	}

    /* delete temporary device handle */

    scsiPhysDevDelete (pScsiPhysBootDev);

    printf ("Attaching to scsi device... ");

    /* recreate a device handle, with polling for actual device parameters */

    taskDelay (sysClkRateGet ());

    if ((pScsiPhysBootDev = scsiPhysDevCreate (pSysScsiCtrl, bootDevId,
                                               bootDevLUN, 0, -1, 0, 0, 0))
         == NULL)
	{
        printErr ("scsiPhysDevCreate failed.\n");
        return (ERROR);
        }

    /*-------------------------------------------------------------------------
     *
     * Configuration of an OMTI3500
     *
     *-----------------------------------------------------------------------*/

    if ((strncmp (pScsiPhysBootDev->devVendorID, "SMS", 3) == 0) &&
	(strncmp (pScsiPhysBootDev->devProductID, "OMTI3500", 8) == 0))
	{
	char modeData [4];	/* array for floppy MODE SELECT data */

	/* zero modeData array, then set byte 1 to "medium code" (0x1b).
	 * NOTE: MODE SELECT data is highly device-specific.  If your device
	 * requires configuration via MODE SELECT, please consult the device's
	 * Programmer's Reference for the relevant data format.
	 */

	bzero (modeData, sizeof (modeData));
	modeData [1] = 0x1b;

	/* issue a MODE SELECT cmd to correctly configure floppy controller */

	scsiModeSelect (pScsiPhysBootDev, 1, 0, modeData, sizeof (modeData));

	/* delete and re-create the SCSI_PHYS_DEV so that INQUIRY will return
	 * the new device parameters, i.e., correct number of blocks
	 */

	scsiPhysDevDelete (pScsiPhysBootDev);

	/* recreate a device handle, polling for actual device parameters */

	if ((pScsiPhysBootDev = scsiPhysDevCreate (pSysScsiCtrl, bootDevId,
						   bootDevLUN, 0, -1, 0, 0, 0))
	    == NULL)
	    {
	    printErr ("scsiPhysDevCreate failed.\n");
	    return (ERROR);
	    }
	}
    /*-------------------------------------------------------------------------
     *
     * END of OMTI3500 configuration
     *
     *-----------------------------------------------------------------------*/

    /* create a block device spanning entire disk (non-distructive!) */

    if ((pScsiBlkBootDev = scsiBlkDevCreate (pScsiPhysBootDev, 0, 0)) == NULL)
	{
        printErr ("scsiLoad: scsiBlkDevCreate failed.\n");
        return (ERROR);
	}

    dosFsInit (NUM_DOSFS_FILES);        /* initialize DOS-FS */

    /* split off boot device from boot file */

    devSplit (fileName, bootDir);

    /* now support booting from partitions on SCSI devices */

    pCbio = dpartDevCreate((CBIO_DEV_ID) pScsiBlkBootDev,
                           NUM_PARTITIONS_DISK_BOOT,
                           usrFdiskPartRead);

    if (NULL == pCbio)
        {
        printErr ("scsiLoad: dpartDevCreate returned NULL.\n");
        return (ERROR);
	}

    /* initialize the boot block device as a dosFs device named <bootDir> */

    if (ERROR == dosFsDevCreate(bootDir,
                                dpartPartGet(pCbio,PARTITION_DISK_BOOT),
                                20, NONE))
        {
        printErr ("scsiLoad: dosFsDevCreate returned ERROR.\n");
        return (ERROR);
	}
	
    printErr ("done.\n");

    /* load the boot file */

    printErr ("Loading %s...", fileName);

    fd = open (fileName, O_RDONLY, 0);

    if (fd == ERROR)
	{
        printErr ("\nCannot open \"%s\".\n", fileName);
        return (ERROR);
	}

    if (bootLoadModule (fd, pEntry) != OK)
        goto readErr;

    close (fd);
    return (OK);

readErr:

    close (fd);
    return (ERROR);
    }

#endif	/* INCLUDE_SCSI_BOOT */

#ifdef	INCLUDE_FD

#include "usrFd.c"

/******************************************************************************
*
* fdLoad - load a vxWorks image from a local floppy disk
*
* RETURNS: OK, or ERROR if file can not be loaded.
*/

LOCAL STATUS fdLoad 
    (
    int     drive,
    int     type,
    char    *fileName,
    FUNCPTR *pEntry
    )
    {
    int fd;

    if (fdDrv (FD_INT_VEC, FD_INT_LVL) != OK)
	{
	printErr ("Could not initialize.\n");
	return (ERROR);
	}

    printf ("Attaching to floppy disk device... ");

    dosFsInit (NUM_DOSFS_FILES);        /* initialize DOS-FS */

    if (usrFdConfig (drive, type, fileName) == ERROR)
	{
        printErr ("usrFdConfig failed.\n");
        return (ERROR);
	}

    printErr ("done.\n");

    /* load the boot file */

    printErr ("Loading %s...", fileName);

    if ((fd = open (fileName, O_RDONLY, 0)) == ERROR)
	{
        printErr ("\nCannot open \"%s\".\n", fileName);
        return (ERROR);
	}

    if (bootLoadModule (fd, pEntry) != OK)
        goto fdLoadErr;

    close (fd);
    return (OK);

fdLoadErr:

    close (fd);
    return (ERROR);
    }

#endif	/* INCLUDE_FD */

#ifdef	INCLUDE_IDE

#define	IDE_MEM_DOSFS	0x200000
#include "usrIde.c"

/******************************************************************************
*
* ideLoad - load a vxWorks image from a local IDE disk
*
* RETURNS: OK, or ERROR if file can not be loaded.
*/

LOCAL STATUS ideLoad 
    (
    int     drive,
    int     type,
    char    *fileName,
    FUNCPTR *pEntry
    )
    {
    int fd;

    if (ideDrv (IDE_INT_VEC, IDE_INT_LVL, type) == ERROR)
	{
	printErr ("Could not initialize.\n");
	return (ERROR);
	}

    printf ("Attaching to IDE disk device... ");

    dosFsInit (NUM_DOSFS_FILES);        /* initialize DOS-FS */

    if (usrIdeConfig (drive, fileName) == ERROR)
	{
        printErr ("usrIdeConfig failed.\n");
        return (ERROR);
	}

    printErr ("done.\n");

    /* load the boot file */

    printErr ("Loading %s...", fileName);

    if ((fd = open (fileName, O_RDONLY, 0)) == ERROR)
	{
        printErr ("\nCannot open \"%s\".\n", fileName);
        return (ERROR);
	}

    if (bootLoadModule (fd, pEntry) != OK)
        goto ideLoadErr;

    close (fd);
    return (OK);

ideLoadErr:

    close (fd);
    return (ERROR);
    }

#endif	/* INCLUDE_IDE */

#ifdef	INCLUDE_ATA

#define	ATA_MEM_DOSFS	0x200000
#include "usrAta.c"

/******************************************************************************
*
* ataLoad - load a vxWorks image from a local ATA disk
*
* RETURNS: OK, or ERROR if file can not be loaded.
*/

LOCAL STATUS ataLoad 
    (
    int     ctrl,
    int     drive,
    char    *fileName,
    FUNCPTR *pEntry
    )
    {
    IMPORT ATA_RESOURCE ataResources[];
    ATA_RESOURCE *pAtaResource	= &ataResources[ctrl];
    int fd;
    char tmp[BOOT_FILE_LEN];

    if (ataDrv (ctrl, pAtaResource->drives, pAtaResource->intVector,
		pAtaResource->intLevel, pAtaResource->configType,
		pAtaResource->semTimeout, pAtaResource->wdgTimeout) == ERROR)
	{
	printErr ("Could not initialize.\n");
	return (ERROR);
	}

    printf ("Attaching to ATA disk device... ");

    dosFsInit (NUM_DOSFS_FILES);        /* initialize DOS-FS */

    devSplit (fileName, tmp);

    if (usrAtaConfig (ctrl, drive, tmp) == ERROR)
	{
        printErr ("usrAtaConfig failed.\n");
        return (ERROR);
	}

    printErr ("done.\n");

    /* load the boot file */

    printErr ("Loading %s...", fileName);

    if ((fd = open (fileName, O_RDONLY, 0)) == ERROR)
	{
        printErr ("\nCannot open \"%s\".\n", fileName);
        return (ERROR);
	}

    if (bootLoadModule (fd, pEntry) != OK)
        goto ataLoadErr;

    close (fd);
    return (OK);

ataLoadErr:

    close (fd);
    return (ERROR);
    }

#endif	/* INCLUDE_ATA */

#ifdef	INCLUDE_PCMCIA

#define	PCMCIA_MEM_DOSFS	0x200000
#include "usrPcmcia.c"

/******************************************************************************
*
* pcmciaLoad - load a vxWorks image from a PCMCIA disk device
*
* RETURNS: OK, or ERROR if file can not be loaded.
*/

LOCAL STATUS pcmciaLoad 
    (
    int     sock,
    char    *fileName,
    FUNCPTR *pEntry
    )
    {
    int fd;

    printf ("Attaching to PCMCIA block device... ");

    dosFsInit (NUM_DOSFS_FILES);        /* initialize DOS-FS */

    if (usrPcmciaConfig (sock, fileName) != OK)
        return (ERROR);

    printErr ("done.\n");

    /* load the boot file */

    printErr ("Loading %s...", fileName);

    if ((fd = open (fileName, O_RDONLY, 0)) == ERROR)
	{
        printErr ("\nCannot open \"%s\".\n", fileName);
        return (ERROR);
	}

    if (bootLoadModule (fd, pEntry) != OK)
        goto pcmciaLoadErr;

    close (fd);
    return (OK);

pcmciaLoadErr:

    close (fd);
    return (ERROR);
    }

#endif	/* INCLUDE_PCMCIA */

#ifdef	INCLUDE_TFFS

#define	TFFS_MEM_DOSFS	0x200000
#include "usrTffs.c"
#include "sysTffs.c"      /* the BSP stub file, in the BSP directory */ 

/******************************************************************************
*
* tffsLoad - load a vxWorks image from a TFFS Flash disk
*
* RETURNS: OK, or ERROR if file can not be loaded.
*
* NOMANUAL
*/

LOCAL STATUS tffsLoad 
    (
    int     drive,		/* TFFS drive number (0 - (noOfDrives-1)) */
    int     removable,		/* 0 - nonremovable flash media */
    char    * fileName,		/* file name to download */
    FUNCPTR * pEntry
    )
    {
    int fd;

    if (tffsDrv () != OK)
	{
	printErr ("Could not initialize.\n");
	return (ERROR);
	}

    printf ("Attaching to TFFS... ");

    dosFsInit (NUM_DOSFS_FILES);        /* initialize DOS-FS */

    if (usrTffsConfig (drive, removable, fileName) == ERROR)
	{
        printErr ("usrTffsConfig failed.\n");
        return (ERROR);
	}

    printErr ("done.\n");

    /* load the boot file */

    printErr ("Loading %s...", fileName);

    if ((fd = open (fileName, O_RDONLY, 0)) == ERROR)
	{
        printErr ("\nCannot open \"%s\".\n", fileName);
        return (ERROR);
	}

    if (bootLoadModule (fd, pEntry) != OK)
        goto tffsLoadErr;

    close (fd);
    return (OK);

tffsLoadErr:

    close (fd);
    return (ERROR);
    }

#endif	/* INCLUDE_TFFS */

#ifdef INCLUDE_TSFS_BOOT
/******************************************************************************
*
* tsfsLoad - load a vxWorks image from a Target Server File System (TSFS).
*
* RETURNS: OK, or ERROR if file can not be loaded.
*
* NOMANUAL
*/

LOCAL STATUS tsfsLoad 
    (
    char    * fileName,		/* file name to download */
    FUNCPTR * pEntry
    )
    {
    int fd;
    WDB_EVT_NODE rebootEventNode;
    char corefile [strlen (fileName) + 8];

    /* add a leading slash if the filename is a relatif path */

    if (fileName[0] != '/' && fileName[0] != '\\')
	sprintf (corefile, "/tgtsvr/%s", fileName);
    else
	sprintf (corefile, "/tgtsvr%s", fileName);

    printf ("Booting using TSFS...\nMake sure that your");
    printf (" Target Server is started with -R[oot] option.\n");

#ifndef INCLUDE_TSFS_BOOT_VIO_CONSOLE
    printf ("Waiting for Target Server connection...");

    /* wait for Target Server connection */

    while (!wdbTargetIsConnected())
    	taskDelay (sysClkRateGet());

    printf (" Done.\n");
#endif	/* INCLUDE_TSFS_BOOT_VIO_CONSOLE */

    /* open the core file via tsfs */

    printErr ("\nLoading %s...\n", corefile);

    if ((fd = open (corefile, O_RDONLY, 0)) == ERROR)
	{
        printErr ("\nCannot open \"%s\".\n", corefile);
        return (ERROR);
	}

    /* load the the core file */

    if (bootLoadModule (fd, pEntry) != OK)
        goto tsfsLoadErr;

    close (fd);

#if (WDB_COMM_TYPE != WDB_COMM_SERIAL)

    /* Notify the host of the target reboot */

    wdbEventNodeInit (&rebootEventNode, wdbRebootEventGet, NULL, NULL);
    wdbEventPost (&rebootEventNode);

    /* let some time to WDB to post the event */

    taskDelay (sysClkRateGet() / 10);
#endif	/* WDB_COMM_TYPE != WDB_COMM_SERIAL */

    return (OK);

tsfsLoadErr:

    close (fd);
    return (ERROR);
    }

/******************************************************************************
*
* wdbRebootEventGet - dummy eventGet routine to force the Target Server restart
*
* suspend the WDB task, so the Target Server will get a RPC_SYSTEMERROR
* will trying to get an event, so it will restart and try to re-attach to
* the target.
*/

LOCAL void wdbRebootEventGet
    (
    void *		pNode,
    WDB_EVT_DATA *	pEvtData
    )
    {
    taskSuspend (0);
    }

#endif	/* INCLUDE_TSFS_BOOT */

#ifdef  INCLUDE_NETWORK

/******************************************************************************
*
* netifAdrsPrint - print MAC address of a network interface
*/

LOCAL void netifAdrsPrint 
    (
    char *ifname		/* interface name */
    )
    {
    IMPORT struct ifnet *ifunit ();
    char *buf;
    char  devName [10];
    int i, value;

    if (ifname == NULL || *ifname == EOS)
	{
	printf ("Interface not specified\n");
	return;
	}

    while (*ifname == ' ')
	ifname++;       /* skip leading blanks */

    if (*ifname == EOS)
	{
	printf ("Interface not specified\n");
	return;
	}

    /* Search for unit number of network device. */
    i = 0;
    while (!isdigit((int)ifname[i]) && !isspace((int)ifname[i]) && ifname[i] != EOS)
       i++;

    if (ifname[i] == EOS)          /* No unit number given - use 0. */
       value = 0;
 
    buf = &ifname[i];
    if (bootScanNum (&buf, &value, FALSE) != OK)  /* No unit number - use 0. */
       value = 0;

    ifname[i] = EOS;
    sprintf (devName, "%s%d", ifname, value);

    if (strncmp (devName, "bp", 2) == 0)
	{
	/* address for backplane is just processor number */

	printf ("Address for device \"%s\" == 00:00:00:00:00:%02x\n",
		devName,  sysProcNumGet ());
	return;
	}
    }

#endif  /* INCLUDE_NETWORK */

/*******************************************************************************
*
* go - start at specified address
*/

LOCAL void go 
    (
    FUNCPTR entry
    )
    {
    printf ("Starting at 0x%x...\n\n", (int) entry);

    taskDelay (sysClkRateGet ());	/* give the network a moment to close */

#ifdef INCLUDE_END
    /* Stop all ENDs to restore to known state for interrupts and DMA */

    (void) muxDevStopAll (0);
#endif  /* INCLUDE_END */


#if	(CPU_FAMILY == PPC)
    cacheTextUpdate ((void *) (LOCAL_MEM_LOCAL_ADRS),	/* cache coherency */
		     (size_t) (sysMemTop() - LOCAL_MEM_LOCAL_ADRS));
#else
#ifdef	INCLUDE_CACHE_SUPPORT
    cacheClear (DATA_CACHE, NULL, ENTIRE_CACHE);	/* push cache to mem */
#endif	/* INCLUDE_CACHE_SUPPORT */
#endif	/* (CPU_FAMILY == PPC) */

#if	(CPU_FAMILY == I80X86)
    sysClkDisable ();			/* disable the system clock interrupt */
    sysIntLock ();			/* lock the used/owned interrupts */
#   if defined (SYMMETRIC_IO_MODE) || defined (VIRTUAL_WIRE_MODE)
    {
    extern void loApicEnable ();
    loApicEnable (FALSE);		/* disable the LOAPIC */
    }
#       if defined (SYMMETRIC_IO_MODE)
    {
    extern BOOL	sysBp;			/* TRUE for BP, FALSE for AP */
    extern void ioApicEnable ();
    if (sysBp)
        ioApicEnable (FALSE);		/* disable the IO APIC */
    }
#       endif /* (SYMMETRIC_IO_MODE) */
#   endif /* (SYMMETRIC_IO_MODE) || (VIRTUAL_WIRE_MODE) */
#endif	/* (CPU_FAMILY == I80X86) */

    /* Lock interrupts before jumping out of boot image. The interrupts
     * enabled during system initialization in sysHwInit()
     */

    intLock(); 

    (entry) ();		/* go to entry point - never to return */
    }

/*******************************************************************************
*
* m - modify memory
*
* This routine prompts the user for modifications to memory, starting at the
* specified address.  It prints each address, and the current contents of
* that address, in turn.  The user can respond in one of several ways:
*
*	RETURN   - No change to that address, but continue
*		   prompting at next address.
*	<number> - Set the contents to <number>.
*	. (dot)	 - No change to that address, and quit.
*	<EOF>	 - No change to that address, and quit.
*
* All numbers entered and displayed are in hexadecimal.
* Memory is treated as 16-bit words.
*/

LOCAL void m 
    (
    char *adrs		/* address to change */
    )
    {
    char line [MAX_LINE + 1];	/* leave room for EOS */
    char *pLine;		/* ptr to current position in line */
    int value;			/* value found in line */
    char excess;

    /* round down to word boundary */

    for (adrs = (char *) ((int) adrs & 0xfffffffe);	/* start on even addr */
         ;						/* FOREVER */
	 adrs = (char *) (((short *) adrs) + 1))	/* bump as short ptr */
	{
	/* prompt for substitution */

	printf ("%06x:  %04x-", (int) adrs, (*(short *)adrs) & 0x0000ffff);

	/* get substitution value:
	 *   skip empty lines (CR only);
	 *   quit on end of file or invalid input;
	 *   otherwise put specified value at address */

	if (fioRdString (STD_IN, line, MAX_LINE) == EOF)
	    break;

	line [MAX_LINE] = EOS;	/* make sure input line has EOS */

	/* skip leading spaces*/

	for (pLine = line; isspace ((UINT) * pLine); ++pLine)
	    ;

	if (*pLine == EOS)			/* skip field if just CR */
	    continue;

	if (sscanf (pLine, "%x%1s", &value, &excess) != 1)
	    break;				/* quit if not number */

	* (short *) adrs = value;		/* assign new value */
	}

    printf ("\n");
    }
/*******************************************************************************
*
* d - display memory
*
* Display contents of memory, starting at adrs.  Memory is displayed in
* words.  The number of words displayed defaults to 64.  If
* nwords is non-zero, that number of words is printed, rounded up to
* the nearest number of full lines.  That number then becomes the default.
*/

LOCAL void d 
    (
    FAST char *adrs,	/* address to display */
    int	       nwords	/* number of words to print. */
    )			/* If 0, print 64 or last specified. */
    {
    static char *last_adrs;
    static int dNbytes = 128;
    char ascii [17];
    FAST int nbytes;
    FAST int byte;

    ascii [16] = EOS;			/* put an EOS on the string */

    nbytes = 2 * nwords;

    if (nbytes == 0)
	nbytes = dNbytes;	/* no count specified: use current byte count */
    else
	dNbytes = nbytes;	/* change current byte count */

    if (adrs == 0)
	adrs = last_adrs;	/* no address specified: use last address */

    adrs = (char *) ((int) adrs & ~1);	/* round adrs down to word boundary */


    /* print leading spaces on first line */

    bfill ((char *) ascii, 16, '.');

    printf ("%06x:  ", (int) adrs & ~0xf);

    for (byte = 0; byte < ((int) adrs & 0xf); byte++)
	{
	printf ("  ");
	if (byte & 1)
	    printf (" ");	/* space between words */
	if (byte == 7)
	    printf (" ");	/* extra space between words 3 and 4 */

	ascii [byte] = ' ';
	}


    /* print out all the words */

    while (nbytes-- > 0)
	{
	if (byte == 16)
	    {
	    /* end of line:
	     *   print out ascii format values and address of next line */

	    printf ("  *%16s*\n%06x:  ", ascii, (int) adrs);

	    bfill ((char *) ascii, 16, '.');	/* clear out ascii buffer */
	    byte = 0;				/* reset word count */
	    }


#if  _BYTE_ORDER == _BIG_ENDIAN
    printf ("%02x", *adrs & 0x000000ff);
#else  /* _BYTE_ORDER == _LITTLE_ENDIAN  */
    /* swap odd and even bytes */
    if ( (long)adrs & 1)
        {
        printf ("%02x", *((char *)((long)adrs & 0xfffffffe)) & 0x000000ff);
        }
    else
        {
        printf ("%02x", *((char *)((long)adrs + 1)) & 0x000000ff);
        }
#endif /* _BYTE_ORDER == _BIG_ENDIAN  */

	if (byte & 1)
	    printf (" ");	/* space between words */
	if (byte == 7)
	    printf (" ");	/* extra space between words 3 and 4 */

	if (* adrs == ' ' ||
			(isascii ((UINT) * adrs) && isprint ((UINT) * adrs)))
	    ascii [byte] = (UINT) * adrs;

	adrs++;
	byte++;
	}


    /* print remainder of last line */

    for (; byte < 16; byte++)
	{
	printf ("  ");
	if (byte & 1)
	    printf (" ");	/* space between words */
	if (byte == 7)
	    printf (" ");	/* extra space between words 3 and 4 */

	ascii [byte] = ' ';
	}

    printf ("  *%16s*\n", ascii);	/* print out ascii format values */

    last_adrs = adrs;
    }
/*******************************************************************************
*
* bootExcHandler - bootrom exception handling routine
*/

LOCAL void bootExcHandler 
    (
    int tid		/* task ID */
    )
    {
    REG_SET regSet;       /* task's registers */

    /* get registers of task to be traced */

    if (taskRegsGet (tid, &regSet) != ERROR)
        {
        trcStack (&regSet, (FUNCPTR) NULL, tid);
        taskRegsShow (tid);
        }
    else
        printf ("bootExcHandler: exception caught but no valid task.\n");

    taskDelay (sysClkRateGet ());       /* pause a second */

    reboot (BOOT_NO_AUTOBOOT);
    }
/*******************************************************************************
*
* skipSpace - advance pointer past white space
*
* Increments the string pointer passed as a parameter to the next
* non-white-space character in the string.
*/

LOCAL void skipSpace 
    (
    FAST char **strptr	/* pointer to pointer to string */
    )
    {
    while (isspace ((UINT) ** strptr))
	++ * strptr;
    }
/*******************************************************************************
*
* printExcMsg - print exception message
*
* Avoid printing possible control characters in exception message area.
*/

LOCAL void printExcMsg 
    (
    char *string
    )
    {
    printf ("\n");

    while (isascii ((UINT) * string) && (isprint ((UINT) * string) ||
						isspace ((UINT) * string)))
	printf ("%c", * string++);

    printf ("\n");
    }

/******************************************************************************
*
* getArg - get argument from command line
*
* This routine gets the next numerical argument from the command line.
* If the argument is not optional, then an error is reported if no argument
* is found.  <ppString> will be updated to point to the new position in the
* command line.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS getArg 
    (
    FAST char **ppString,	/* ptr to ptr to current position in line */
    int *	pValue,		/* ptr where to return value */
    BOOL	defaultHex,	/* TRUE = arg is hex (even w/o 0x) */
    BOOL	optional	/* TRUE = ok if end of line */
    )
    {
    skipSpace (ppString);


    /* if nothing left, complain if arg is not optional */

    if (**ppString == EOS)
	{
	if (!optional)
	    {
	    printf ("missing parameter\n");
	    return (ERROR);
	    }
	else
	    return (OK);
	}


    /* scan arg */

    if (bootScanNum (ppString, pValue, defaultHex) != OK)
	{
	printf ("invalid parameter\n");
	return (ERROR);
	}

    skipSpace (ppString);

    /* if we encountered ',' delimiter, step over it */

    if (**ppString == ',')
	{
	++*ppString;
	return (OK);
	}

    /* if end of line, scan is ok */

    if (**ppString == EOS)
	return (OK);

    /* we got stopped by something else */

    printf ("invalid parameter\n");
    return (ERROR);
    }


/* The following routines are common to bootConfig and usrConfig and will
 * eventually be merged
 */

/******************************************************************************
*
* usrBootLineInit - initialize system boot line
*
* Initializes system boot line as per specified start type.
* If this is a COLD boot, i.e., with CLEAR option to clear memory,
* then the boot line is initialized from non-volatile RAM, if any,
* otherwise from the compiled in default boot line.
*/

LOCAL void usrBootLineInit 
    (
    int startType
    )
    {
    if (startType & BOOT_CLEAR)
	{
	/* this is a cold boot so get the default boot line */

	if ((sysNvRamGet (BOOT_LINE_ADRS, BOOT_LINE_SIZE, 0) == ERROR) ||
	    (*BOOT_LINE_ADRS == EOS) || (*BOOT_LINE_ADRS == (char) -1))
	    {
	    /* either no non-volatile RAM or empty boot line */

	    strcpy (BOOT_LINE_ADRS, DEFAULT_BOOT_LINE);
	    }
	}
    }
/******************************************************************************
*
* usrBootLineCrack - interpret and verify boot line
*
* This routine interprets the specified boot line and checks the validity
* of certain parameters.  If there are errors, a diagnostic message is
* printed.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS usrBootLineCrack 
    (
    char *	 bootString,
    BOOT_PARAMS *pParams
    )
    {
    FAST char *	pS;

    pS = bootStringToStruct (bootString, pParams);

    if (*pS != EOS)
	{
	bootParamsErrorPrint (bootString, pS);
	return (ERROR);
	}

#ifdef  INCLUDE_NETWORK
    /* check inet addresses */

    if ((checkInetAddrField (pParams->ead, TRUE) != OK) ||
	(checkInetAddrField (pParams->bad, TRUE) != OK) ||
	(checkInetAddrField (pParams->had, FALSE) != OK) ||
	(checkInetAddrField (pParams->gad, FALSE) != OK))
	{
	return (ERROR);
	}
#endif  /* INCLUDE_NETWORK */

    return (OK);
    }

#ifdef  INCLUDE_NETWORK

/******************************************************************************
*
* checkInetAddrField - check for valid inet address in boot field
*/

LOCAL STATUS checkInetAddrField 
    (
    char *pInetAddr,
    BOOL subnetMaskOK
    )
    {
    char inetAddr [30];

    /* 
     * The bzero() call corrects SPR 6326. The calls to bootNetmaskExtract()
     * and inet_addr() did not delimit the input string with a '\0'. When
     * inet_addr attempted to print the invalid address, the system would
     * crash or hang.
     */

    bzero (inetAddr, sizeof(inetAddr));

    if (*pInetAddr == EOS)
	return (OK);

    strncpy (inetAddr, pInetAddr, sizeof (inetAddr) - 1);

    if (subnetMaskOK)
	{
	if (bootNetmaskExtract (inetAddr, &netmask) < 0)
	    {
	    printf ("Error: invalid netmask in boot field \"%s\".\n", inetAddr);
	    return (ERROR);
	    }
	}

    if (inet_addr (inetAddr) == (ULONG) ERROR)
	{
	printf ("Error: invalid inet address in boot field \"%s\".\n",inetAddr);
	return (ERROR);
	}

    return (OK);
    }

#ifdef ETHERNET_ADR_SET
/*******************************************************************************
*
* mEnet - modify the last three bytes of the ethernet address
*
* RETURNS: void
*
* NOMANUAL
*/

void mEnet 
    (
    char * 	pNum   /* Boot line, including unit number. */
    )
    {
    uchar_t byte [MAX_ADR_SIZE];	/* array to hold new Enet addr */
    uchar_t curArr [MAX_ADR_SIZE];	/* array to hold current Enet addr */
    char line [MAX_LINE + 1];
    char *pLine;		/* ptr to current position in line */
    int value;			/* value found in line */
    char excess;
    char *buf;
    int unitNum;
    int ix;

    /* Search for unit number of network device. */

    buf = pNum;
    if (bootScanNum (&buf, &unitNum, FALSE) != OK)  /* Use 0 if no unit #. */
       unitNum = 0;

    sysEnetAddrGet (unitNum, curArr);
    printf ("Current Ethernet Address is: ");

#if _BYTE_ORDER == _BIG_ENDIAN
    printf ("%02x:%02x:%02x:%02x:%02x:%02x\n", curArr[5], 
	    curArr[4], curArr[3], curArr[2], 
	    curArr[1], curArr[0]);
    byte[5] = ((ENET_DEFAULT & 0x0000ff00) >> 8);
    byte[4] = ((ENET_DEFAULT & 0x00ff0000) >> 16);
    byte[3] = ((ENET_DEFAULT & 0xff000000) >> 24);
    byte[2] = curArr[2];
    byte[1] = curArr[1];
    byte[0] = curArr[0];
#else  /* _BYTE_ORDER == _LITTLE_ENDIAN  */
    printf ("%02x:%02x:%02x:%02x:%02x:%02x\n", curArr[0], 
	    curArr[1], curArr[2], curArr[3], 
	    curArr[4], curArr[5]);
    byte[5] = ((ENET_DEFAULT & 0x00ff0000) >> 16);
    byte[4] = ((ENET_DEFAULT & 0x0000ff00) >> 8);
    byte[3] = (ENET_DEFAULT & 0x000000ff);
    byte[2] = curArr[3];
    byte[1] = curArr[4];
    byte[0] = curArr[5];
#endif /* _BYTE_ORDER == _BIG_ENDIAN */

    printf ("Modify only the last 3 bytes (board unique portion) of Ethernet Address.\n");
    printf ("The first 3 bytes are fixed at manufacturer's default address block.\n");

    for (ix = 5; ix > 2; ix--)
        printf ("%02x- %02x\n", byte[ix], byte[ix]);


    /* start on fourth byte of enet addr */
    for (ix = 2; ix >= 0; ix --)	
	{
	/* prompt for substitution */

	printf ("%02x- ", byte[ix]);

	/* get substitution value:
	 *   skip empty lines (CR only);
	 *   quit on end of file or invalid input;
	 *   otherwise put specified value at address */

	if (fioRdString (STD_IN, line, MAX_LINE) == EOF)
	    break;

	line [MAX_ADR_SIZE] = EOS;	/* make sure input line has EOS */

	for (pLine = line; isspace ((int) *pLine); ++pLine)	/* skip leading spaces*/
	    ;

	if (*pLine == EOS)			/* skip field if just CR */
	    continue;

	if (sscanf (pLine, "%x%1s", &value, &excess) != 1)
	    break;				/* quit if not number */

	byte[ix]  = (uchar_t)value;		/* assign new value */
	}

    printf ("\n");

    sysEnetAddrSet (byte[5], byte[4], byte[3], byte[2], byte[1], byte[0]);

    printf ("New Ethernet Address is: ");
#if _BYTE_ORDER == _BIG_ENDIAN
    for (ix = 5; ix > 0; ix--)
        printf ("%02x:", byte[ix]);
    printf ("%02x\n", byte[0]);
#else  /* _BYTE_ORDER == _LITTLE_ENDIAN */
    for (ix = 5; ix > 0; ix--)
        printf ("%02x:", byte[ix]);
    printf ("%02x\n", byte[0]);
#endif /* _BYTE_ODER == _BIG_ENDIAN */
    }
#endif  /* ETHERNET_ADR_SET */

#endif  /* INCLUDE_NETWORK */

#ifdef INCLUDE_APPL_LOG_UTIL

extern STATUS applUtilInstInit (void *);

/****************************************************************************
*
* usrNetApplUtilInit() - initialize the application/stack logging utility
*
* RETURNS: N/A
*/

void usrNetApplUtilInit (void)
    {
    APPL_UTIL_CFG * pCfg;
    APPL_UTIL_CFG cfg;

    pCfg = &cfg;

    pCfg->cfgh.len = sizeof (*pCfg);
    pCfg->fd = APPL_LOG_FD_CFG;
    pCfg->level = APPL_LOG_LEVEL_CFG;
    pCfg->fdKern = APPL_KERN_LOG_FD_CFG;
    pCfg->levelKern = APPL_KERN_LOG_LEVEL_CFG;

    applUtilInstInit (pCfg);
    }

#endif /* INCLUDE_APPL_LOG_UTIL */
