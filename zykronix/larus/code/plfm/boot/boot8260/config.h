/* ads827x/config.h - Motorola ADS827x-PCI board configuration header */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,12oct04,dtr  Revision change.
01e,19feb04,dtr  Fix HW RST CONF word.
01d,28jan04,dtr  Modify FLASH defines.
01c,08jan04,dtr  Adding INCLUDE_SECURITY_ENGINE.
01b,06jan04,dtr  Change PCI_MEM_MASK_SIZE to match LOCAL_MEM_SIZE.
01a,18dec03,dtr  adapted from ads826x config.h
*/

/*
This file contains the configuration parameters for the
Motorola MPC8272 ADS board.
*/

#ifndef	INCconfigh
#define	INCconfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* BSP version/revision identification, should be placed
 * before #include "configAll.h"
 */

#define BSP_VER_1_1		1
#define BSP_VER_1_2		1
#define BSP_VERSION		"1.2"
#define BSP_REV		 	"/1"

#define DEBUG           1
/* Define Clock Speed and source  */

#define	FREQ_33_MHZ	 	 33000000
#define	FREQ_40_MHZ	 	 40000000
#define	FREQ_66_MHZ	 	 66000000
#define FREQ_100_MHZ    100000000
/*
 * This define must be set to the value of the resonant oscillator
 * inserted in position U16 of the ADS8272 board.  Choose from above
 * list.
 */
#ifdef USE_BUS_100M /* For 100MHz now */
#define	OSCILLATOR_FREQ	FREQ_100_MHZ
#endif

#ifdef USE_BUS_66M
#define	OSCILLATOR_FREQ	FREQ_66_MHZ
#endif
 
#define	DIV_FACT_1	0
#define	DIV_FACT_16	1

#define BRGCLK_DIV_FACTOR	DIV_FACT_16

#define M8260_BRGC_DIVISOR	BRGCLK_DIV_FACTOR

#include "configAll.h"
#include "sys_memmap.h"


#define DEFAULT_BOOT_LINE \
"motfcc(0,0)host:target/config/ads827x/vxWorks h=10.0.0.50 e=10.0.0.200 u=vxworks pw=vxworks tn=ads827x"

#define INCLUDE_MMU_BASIC

#ifdef  INCLUDE_MMU_BASIC
#   define USER_I_MMU_ENABLE
#   define USER_D_MMU_ENABLE
#endif

#define INCLUDE_CACHE_SUPPORT

#ifdef  INCLUDE_CACHE_SUPPORT
#   define USER_D_CACHE_ENABLE
/* Does nothing about copyback vs writethrough in h/w, must use sysPhysMemDesc */
#   undef  USER_D_CACHE_MODE
#   define USER_D_CACHE_MODE  CACHE_COPYBACK

#   define USER_I_CACHE_ENABLE
#   undef  USER_I_CACHE_MODE
#   define USER_I_CACHE_MODE  CACHE_COPYBACK
#endif

/* Number of TTY definition */

#undef	NUM_TTY
#define	NUM_TTY		N_SIO_CHANNELS	/* defined in ads827x.h */


/* Optional timestamp support */

#define	INCLUDE_TIMESTAMP
#define	INCLUDE_AUX_CLK

/* optional TrueFFS support */
#ifdef TSS
#define  INCLUDE_TFFS
#else
#undef  INCLUDE_TFFS
#endif

#ifdef INCLUDE_TFFS
#    define INCLUDE_DOSFS	  /* dosFs file system */
#    define INCLUDE_SHOW_ROUTINES /* show routines for system facilities*/
#    define TFFS_BASE_ADRS    SYS_FLASH_BASE_ADRS
#endif
/* clock rates */

#define	SYS_CLK_RATE_MIN	1	/* minimum system clock rate */
#define	SYS_CLK_RATE_MAX	8000	/* maximum system clock rate */
#define	AUX_CLK_RATE_MIN	1	/* minimum auxiliary clock rate */
#define	AUX_CLK_RATE_MAX	8000	/* maximum auxiliary clock rate */

/*
 * DRAM refresh frequency - This macro defines the DRAM refresh frequency.
 * e.i: A DRAM with 1024 rows to refresh in 16ms:
 * DRAM_REFRESH_FREQ = 1024/ 16E-3 = 64E3 hz
 */

#define DRAM_REFRESH_FREQ	64000	/* 64 kHz */

#define M8260_SCCR_PCI_MODE_BIT 0x80

/* add on-chip drivers */
#define INCLUDE_SECURITY_ENGINE

#ifdef TSS
#undef  INCLUDE_PCI		/* include PCI library support */
#else
#undef  INCLUDE_PCI		/* don't include PCI library support */
#endif

#ifdef  INCLUDE_PCI
#define INCLUDE_PCI_AUTOCONF

/*
CPU Addr					PCI Addr
PCI_LOCAL_MEM_BUS	------------------------- PCI_MSTR_MEM_BUS
			-		        -
	      		-		        -
PCI_LOCAL_MEM_BUS +	------------------------- PCI_MSTR_MEM_BUS +
PCI_LOCAL_MEM_SIZE	----IMMR              	- PCI_MSTR_MEM_SIZE
      			-		        -
       			-		        -----PIMMR
	       		-		        -
		       	-		        -
CPU_PCI_MEM_ADRS	------------------------- PCI_MEM_ADRS
       		        -			-
		      	-			-
CPU_PCI_MEMIO_ADRS	------------------------- PCI_MEMIO_ADRS
	      	        -			-
		       	-		       	-
CPU_PCI_IO_ADRS		------------------------- PCI_IO_ADRS
		       	-	       		-
		       	-      			-
CPU_PCI_IO_ADRS +	------------------------- PCI_IO_ADRS +
CPU_PCI_IO_SIZE		-     			- PCI_IO_SIZE
		       	-     			-
		       	-     			-
		       	-     			-
		       	-------------------------- 4GBytes
*/



/* for custom sysPciAutoConfig.c */


/* PCI based addresses */
#define PCI_MEM_ADRS		0x50000000
#define PCI_MEM_SIZE		0x01000000 		/*16MB */
#define PCI_MEM_SIZE_MASK	PCI_SIZE_MASK_16M /* This should match PCI_MEM_SIZE */
#define PCI_MEMIO_ADRS		0x60000000
#define PCI_MEMIO_SIZE		0x01000000 		/*16MB */
#define PCI_MEMIO_SIZE_MASK PCI_SIZE_MASK_16M	/* This should match PCI_MEMIO_SIZE */
#define PCI_IO_ADRS	0x70000000
#define PCI_IO_SIZE	0x00100000	/* IO Space is not available */


/* CPU based addresses */
#define CPU_PCI_MEM_ADRS		0x80000000
#define CPU_PCI_MEM_SIZE		PCI_MEM_SIZE
#define CPU_PCI_MEMIO_ADRS		0x90000000
#define CPU_PCI_MEMIO_SIZE		PCI_MEMIO_SIZE
#define CPU_PCI_IO_ADRS			0xa0000000
#define CPU_PCI_IO_SIZE			PCI_IO_SIZE


/* CPU from PCI bus */
#define PCI_MSTR_MEM_BUS		0x00000000
#define PCI_MSTR_MEM_SIZE		PCI_LOCAL_MEM_SIZE
#define PCI_BRIDGE_PIMMR_BASE_ADRS      0x40000000


/* CPU Address that is visible from PCI */
#define PCI_LOCAL_MEM_BUS	LOCAL_MEM_LOCAL_ADRS
#define PCI_LOCAL_MEM_SIZE	LOCAL_MEM_SIZE

/* This should match LOCAL_MEM_SIZE - if not statically defined buffers
   need to be used for PCI devices */
#define PCI_MSTR_MEM_SIZE_MASK	PCI_SIZE_MASK_64M

#ifndef PCI_CFG_TYPE
#	ifdef INCLUDE_PCI_AUTOCONF
#		define PCI_CFG_TYPE PCI_CFG_AUTO
#	else
#		define PCI_CFG_TYPE PCI_CFG_FORCE
#	endif /* INCLUDE_PCI_AUTOCONF */
#endif /* PCI_CFG_TYPE */

#endif /* INCLUDE_PCI */

#ifdef TSS
#define   INCLUDE_NETWORK
#else
#undef    INCLUDE_NETWORK
#endif


/* add necessary drivers */
#ifdef  INCLUDE_NETWORK
#	define INCLUDE_MOTFCCEND			/* include the FCC ethernet driver */
#   define INCLUDE_NET_SHOW             /* network info and status facilities */
#   define INCLUDE_NET_SYM_TBL          /* load symbol table from network */
#   define INCLUDE_TELNET               /* telnet-style remote login */
#   define INCLUDE_FTP_SERVER	        /* ftp server */
#   define INCLUDE_PING		            /* ping() utility */
#ifndef BOOT
#   define TELNETD_MAX_CLIENTS          10
#   define TELNETD_TASKFLAG             FALSE
#   define TELNETD_PARSER_CONTROL          telnetCmdParser
#   define INCLUDE_DHCPC		        /* dhcpc utility */
#   define INCLUDE_SNTPS                /* SNTP server */
#endif
#endif

#ifdef INCLUDE_DHCPC
#define DHCPC_SPORT 		67    /* Port monitored by DHCP servers. */
#define DHCPC_CPORT 		68    /* Port monitored by DHCP clients. */
#define DHCPC_MAX_LEASES 	4     /* Max. number of simultaneous leases */
#define DHCPC_OFFER_TIMEOUT 	5     /* Seconds to wait for multiple offers */
#define DHCPC_DEFAULT_LEASE 	3600  /* Desired lease length in seconds */
#define DHCPC_MIN_LEASE 	30    /* Minimum allowable lease length */
#define DHCPC_MAX_MSGSIZE       590   /* Allows minimum message on Ethernet */
#define DHCPC_DISCOVER_RETRIES	4     /* Max. number of DISCOVER retries */
#endif

#if defined (INCLUDE_SNTPC) || defined (INCLUDE_SNTPS)
#define SNTP_PORT 123

#ifdef INCLUDE_SNTPS
#define SNTPS_MODE SNTP_PASSIVE     /* SNTP_ACTIVE or SNTP_PASSIVE */
#define SNTPS_DSTADDR NULL         /* If NULL, uses subnet local broadcast. */
#define SNTPS_INTERVAL 64          /* Broadcast interval, in seconds. */
#define SNTPS_TIME_HOOK sntpsClockHook       /* Name of required clock access routine. */
#endif    /* SNTP server. */
#endif /* SNTP server or client. */

#ifdef INCLUDE_MOTFCCEND
#  ifndef INCLUDE_END
#    define INCLUDE_END  /* only END-style driver for FCC */
#  endif /* INCLUDE_END */
#endif /* INCLUDE_MOTFCCEND */

#ifdef INCLUDE_MOTFCCEND
#define INCLUDE_FCC1      /* primary */
#define INCLUDE_FCC2      
#undef  INCLUDE_FCC3      /* not available */
#undef  INCLUDE_FCC4      /* not available */
#endif /* INCLUDE_MOTFCCEND */

#undef INCLUDE_FEI_END
#undef INCLUDE_PCI_DMA
#undef INCLUDE_PCI_ERROR_HANDLING
#undef PCI_BRIDGE_READ_ERRATA_WORKAROUND

#ifndef INCLUDE_PCI
#  undef INCLUDE_PCI_DMA
#  undef INCLUDE_PCI_ERROR_HANDLING
#  undef INCLUDE_FEI_END
#  undef PCI_BRIDGE_READ_ERRATA_WORKAROUND
#endif

#ifdef TSS	/* BSP_ADD */
#undef  INCLUDE_FLASH
#endif

#if 0 /* BSP remove */
#ifdef INCLUDE_FLASH
#define FLASH_ADRS_8M		0xff800000	/* base address of 8Meg flash */
#define FLASH_ADRS_16M		0xff000000	/* base address of 16Meg flash */
#define FLASH_ADRS_32M		0xfe000000	/* base address of 32Meg flash */
#define FLASH_BASE_ADRS		FLASH_ADRS_8M  /* this board has 8Meg flash */
#define FLASH_ADRS              0xffec0000
#define FLASH_BLOCK_SIZE        0x40000
#define FLASH_BOOTLINE_OFFSET   0x0
#define FLASH_WIDTH   		4
#define FLASH_SIZE	       	0x800000 /* 8 Mbytes total flash size */
#define FLASH_SIZE_WRITEABLE	256		/* reasonable write size */
#define SYS_FLASH_TYPE	       	FLASH_28F016	/* Intel flash device type */
#endif
#endif /* BSP remove */

/* Memory addresses */

#if 0 /* ORG BSP_RMV */
#define LOCAL_MEM_SIZE		0x04000000		/* 64 Mbyte memory available */
#define LOCAL_MEM_LOCAL_ADRS	0x00000000		/* Base of RAM */
#endif

#ifdef TSS
#define LOCAL_MEM_SIZE		    0x08000000		/* 128 Mbyte memory available */
#define LOCAL_MEM_LOCAL_ADRS	0x00000000		/* Base of RAM */
#endif

#ifdef ADSL24V
#define LOCAL_MEM_SIZE		    0x01000000		/* 16 Mbyte memory available */
#define LOCAL_MEM_LOCAL_ADRS	0x00000000		/* Base of RAM */
#endif
/*
 * The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
 * in config.h, MakeSkel, Makefile, and Makefile.*
 * All definitions for these constants must be identical.
 */

#define ROM_BASE_ADRS   0xfff00000		/* base address of ROM */
#define ROM_TEXT_ADRS  	ROM_BASE_ADRS + 0x100
#define ROM_SIZE       	0x080000		 /* ROM space */
#define ROM_WARM_ADRS   (ROM_TEXT_ADRS+8) /* warm reboot entry */

#ifdef TSS
/* RAM address for ROM boot */
#define RAM_HIGH_ADRS  	(LOCAL_MEM_LOCAL_ADRS + 0x01000000)

#ifdef BOOT
/* RAM address for sys image */
#define RAM_LOW_ADRS	(LOCAL_MEM_LOCAL_ADRS + 0x00800000)
#else
#define RAM_LOW_ADRS	(LOCAL_MEM_LOCAL_ADRS + 0x00010000)
#endif

#define USER_RESERVED_MEM    0x00000000	/* user reserved memory size */
#endif

#ifdef ADSL24V
/* RAM address for ROM boot */
#define RAM_HIGH_ADRS  	(LOCAL_MEM_LOCAL_ADRS + 0x00800000)

/* RAM address for sys image */
#ifdef BOOT
#define RAM_LOW_ADRS	(LOCAL_MEM_LOCAL_ADRS + 0x00400000)
#else
#define RAM_LOW_ADRS	(LOCAL_MEM_LOCAL_ADRS + 0x00010000)
#endif

#define USER_RESERVED_MEM    0x00000000	/* user reserved memory size */
#endif

/* Hard Reset Configuration Words */

#ifdef _BSP_ORG_
#define HRCW_BYTE_0	0x0C	/* 32 bit boot port, MSR(IP) = 1 */
#define HRCW_BYTE_1	0x72	/* IMMR[0-14] = 0x0F00_0000, no L2 cache signals*/
#define HRCW_BYTE_2   	0x36	/* bit[20:21] LBPC = 01, set local bus as PCI bus */
#define HRCW_BYTE_3    	0x5A	/* bit[26] ALD_EN = 1 */
#endif

#ifdef TSS_ORG
#ifdef USE_BUS_100M
#define HRCW_BYTE_0	    0x04	/* 8 bit boot port, MSR(IP) = 1 */
#define HRCW_BYTE_1	    0x72	/* IMMR[0-14] = 0x0F00_0000, no L2 cache signals*/
#define HRCW_BYTE_2   	0x36	/* bit[20:21] LBPC = 01, set local bus as PCI bus */
#define HRCW_BYTE_3    	0x0A	/* bit[26] ALD_EN = 1 */
#endif
#endif /* TSS_ORG */

#ifdef TSS
#ifdef USE_BUS_100M
#define HRCW_BYTE_0	    0x04	/* EARB: 0, EXMC: 0, CDIS: 0, EBM: 0, BPS: 01, 8 bit boot port, MSR(IP) = 1, CIP: 0,  ISPS: 0*/
#define HRCW_BYTE_1	    0x02	/* BAC: 00, EXTMC: 00, PLLBP: 0, ISB: 010 IMMR[0-14] = 0x0F00_0000, no L2 cache signals*/
#define HRCW_BYTE_2   	0x32	/* BMS: 0, BBD: 0, MMR: 11, Reserved: 00, CPUC: 10*/
#define HRCW_BYTE_3    	0x0A	/* CS6PC: 00, ALD_EN: 0, PCI_MODCK: 0, MODCK_H: 1010*/
#endif
#endif /* TSS */

#ifdef ADSL24V
#ifdef USE_BUS_66M
#define HRCW_BYTE_0	    0x04	/* 8 bit boot port, MSR(IP) = 1 */
#define HRCW_BYTE_1	    0x72	/* IMMR[0-14] = 0x0F00_0000, no L2 cache signals*/
#define HRCW_BYTE_2   	0x36	/* bit[20:21] LBPC = 01, set local bus as PCI bus */
#define HRCW_BYTE_3    	0x47	/* bit[26] ALD_EN = 0, PCI HOST MODE, 0111_011 66M, 200M, 266M*/
#endif

#ifdef USE_BUS_100M
#define HRCW_BYTE_0	    0x04	/* 8 bit boot port, MSR(IP) = 1 */
#define HRCW_BYTE_1	    0x72	/* IMMR[0-14] = 0x0F00_0000, no L2 cache signals*/
#define HRCW_BYTE_2   	0x36	/* bit[20:21] LBPC = 01, set local bus as PCI bus */
#define HRCW_BYTE_3    	0x4A	/* bit[26] ALD_EN = 0, PCI HOST MODE, 1010_100 100M, 200M, 400M*/
#endif
#endif /* ADSL24V */

/*
 * The hard reset configuration word (above) defines the initial location
 * of the Internal Memory Map.  If you want to change this address during
 * bsp boot time (to match the memory map defined by the Motorola ADS8266-PCI
 * documentation) define the IMMAP_REMAP macro below.  The actual value
 * for the INTERNAL_MEM_MAP_ADDR is then defined in ads828x.h
 */
#undef IMMAP_REMAP

/*
 * Default power management mode - selected via vxPowerModeSet() in
 * sysHwInit().
 */

#define DEFAULT_POWER_MGT_MODE  VX_POWER_MODE_DISABLE

#define INCLUDE_USER_APPL

#ifndef BOOT
#define USER_APPL_INIT         	SysTaskMonitor()
#else
#define USER_APPL_INIT         	SysBootMonitor()
#endif

#include "ads827x.h"			/* include the ads826x params */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* INCconfigh */
#if defined(PRJ_BUILD)
	#include "prjParams.h"
#endif

