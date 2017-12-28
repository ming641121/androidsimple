/* sysLib.c - Motorola ads 827x board system-dependent library */

/* Copyright 1984-2003 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,04oct04,dtr  Add support for latest security engine drivers
01g,28jan04,dtr  Use m82xxSio.c from target/src/drv/sio.
01f,08jan04,dtr  Add MMU mapping for security processor.
01e,08dec03,dtr  Modified for ads827x BSP.
01d,08oct03,dee  nvram routines check limits of nvram offset
01c,01oct03,dee  use sysHwInit to adjust mmu tables depending on size of SDRAM
01b,08jan03,dtr  Added support for PCI DMA and Error Handling.
                 Implemented workaround for PCI Bridge read errata.
01w,13jul02,dtr  Add support for PCI.
01v,08mar02,jnz  add support for ads8266
01u,12dec01,jrs  Add BAT table entry to correct mem map.
01t,10dec01,jrs  change copyright date
01s,30nov01,gls  fixed sysBatDesc definition (SPR #20321)
01r,17oct01,jrs  Upgrade to veloce
		 set M8260_SCCR to BRGCLK_DIV_FACTOR,
		 added global variable baudRateGenClk-set by sysBaudClkFreq(),
		 added sysCpmFreqGet() and sysBaudClkFreq() functions,
		 added 8260 Reset Configuration Table - SPR66989
		 changed INCLUDE_MOT_FCC to INCLUDE_MOTFCCEND - SPR #33914
01q,14mar00,ms_  add support for PILOT revision of board
01p,04mar00,mtl  minor changes in macros to be consistent
01o,18oct99,ms_  vxImmrGet must return only bits 14:0 (SPR 28533)
01n,18sep99,ms_  fix comment for ram on 60x bus
01m,16sep99,ms_  sysMotFccEnd.c is local to bsp after all...
01l,16sep99,ms_  fix include files path
01k,16sep99,ms_  some included files come from src/drv instead of locally
01j,16sep99,ms_  get miiLib.h from h/drv instead of locally
01i,13sep99,cn   added sysMiiOptRegsHandle () (SPR# 28305).
01g,08jun99,ms_  remove definition of M8260_SIUMCR that doesn't belong in here
01f,17apr99,ms_  unnesessary setting of MAMR
01e,17apr99,cn   added a temporary fix to initialize the boot line.
01d,17apr99,ms_  final cleanup for EAR
01c,14apr99,cn   added support for motFccEnd
01b,06apr99,ms_  reset the CPM in sysHwInit()
01a,28jan99,ms_  adapted from ads860/sysLib.c version 01j
*/

/*
DESCRIPTION
This library provides board-specific routines.  The chip drivers included are:

SEE ALSO:
.pG "Configuration"
*/

/* includes */

#include "vxWorks.h"
#include "vme.h"
#include "memLib.h"
#include "cacheLib.h"
#include "sysLib.h"
#include "config.h"
#include "string.h"
#include "intLib.h"
#include "logLib.h"
#include "stdio.h"
#include "taskLib.h"
#include "vxLib.h"
#include "tyLib.h"
#include "arch/ppc/mmu603Lib.h"
#include "arch/ppc/vxPpcLib.h"
#include "private/vmLibP.h"
#include "drv/mem/m8260Siu.h"
#include "drv/mem/m8260Memc.h"
#include "drv/timer/m8260Clock.h"
#include "drv/sio/m8260Cp.h"
#include "drv/sio/m8260CpmMux.h"
#include "drv/mem/m82xxDpramLib.h"
#include "miiLib.h"
#ifdef INCLUDE_PCI
#    include "drv/pci/pciConfigLib.h"
#    include "drv/pci/pciIntLib.h"
#endif /* INCLUDE_PCI */
#include <sntpsLib.h>
#include <time.h>

#include "sys_memmap.h"
#include "led_if.h"

#define  FORCE_DEFAULT_BOOT_LINE


extern int consoleFd;
/* globals */

/*
 * sysBatDesc[] is used to initialize the block address translation (BAT)
 * registers within the PowerPC 603/604 MMU.  BAT hits take precedence
 * over Page Table Entry (PTE) hits and are faster.  Overlap of memory
 * coverage by BATs and PTEs is permitted in cases where either the IBATs
 * or the DBATs do not provide the necessary mapping (PTEs apply to both
 * instruction AND data space, without distinction).
 *
 * The primary means of memory control for VxWorks is the MMU PTE support
 * provided by vmLib and cacheLib.  Use of BAT registers will conflict
 * with vmLib support.  User's may use BAT registers for i/o mapping and
 * other purposes but are cautioned that conflicts with cacheing and mapping
 * through vmLib may arise.  Be aware that memory spaces mapped through a BAT
 * are not mapped by a PTE and any vmLib() or cacheLib() operations on such
 * areas will not be effective, nor will they report any error conditions.
 *
 * Note: BAT registers CANNOT be disabled - they are always active.
 * For example, setting them all to zero will yield four identical data
 * and instruction memory spaces starting at local address zero, each 128KB
 * in size, and each set as write-back and cache-enabled.  Hence, the BAT regs
 * MUST be configured carefully.
 *
 * With this in mind, it is recommended that the BAT registers be used
 * to map LARGE memory areas external to the processor if possible.
 * If not possible, map sections of high RAM and/or PROM space where
 * fine grained control of memory access is not needed.  This has the
 * beneficial effects of reducing PTE table size (8 bytes per 4k page)
 * and increasing the speed of access to the largest possible memory space.
 * Use the PTE table only for memory which needs fine grained (4KB pages)
 * control or which is too small to be mapped by the BAT regs.
 *
 * All BATs point to PROM/FLASH memory so that end customer may configure
 * them as required.
 *
 * [Ref: chapter 7, PowerPC Microprocessor Family: The Programming Environments]
 */

UINT32 sysBatDesc [2 * (_MMU_NUM_IBAT + _MMU_NUM_DBAT)] =
    {
#if 0 /* BSP_REMOVE org */
    /* I BAT 0 */
    ((FLASH_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_8M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((FLASH_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* I BAT 1 */

    0, 0,

    /* I BAT 2 */

    0, 0,

    /* I BAT 3 */

    0, 0,

    /* D BAT 0 */
    ((FLASH_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_8M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((FLASH_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* D BAT 1 */

    /* use DBAT1 to map CPM DPRAM and internal registers into data space */
    /* NOTE! the internal space cannot be cached and should be guarded */
    ((INTERNAL_MEM_MAP_ADDR & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_128K |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((INTERNAL_MEM_MAP_ADDR & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT | _MMU_LBAT_GUARDED),

    /* D BAT 2 */

    0,0,

    /* D BAT 3 */

    0,0
#else
    /* I BAT 0 */
    ((BOOT_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_512K |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((BOOT_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* I BAT 1 */

    0, 0,

    /* I BAT 2 */

    0, 0,

    /* I BAT 3 */

    0, 0,

    /* D BAT 0 */
    ((BOOT_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_512K |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((BOOT_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* D BAT 1 */

    /* use DBAT1 to map CPM DPRAM and internal registers into data space */
    /* NOTE! the internal space cannot be cached and should be guarded */
    ((INTERNAL_MEM_MAP_ADDR & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_128K |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((INTERNAL_MEM_MAP_ADDR & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT | _MMU_LBAT_GUARDED),

    /* D BAT 2 */
#ifdef TSS
    ((SYS_FLASH_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_256M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((SYS_FLASH_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),
#endif

#ifdef ADSL24V
    ((SYS_FLASH_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_4M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((SYS_FLASH_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),
#endif

    /* D BAT 3 */

    0,0
#endif
    };


/*
 * sysPhysMemDesc[] is used to initialize the Page Table Entry (PTE) array
 * used by the MMU to translate addresses with single page (4k) granularity.
 * PTE memory space should not, in general, overlap BAT memory space but
 * may be allowed if only Data or Instruction access is mapped via BAT.
 *
 * Address translations for local RAM, memory mapped PCI bus, the Board Control and
 * Status registers, the MPC8260 Internal Memory Map, and local FLASH RAM are set here.
 *
 * PTEs are held, strangely enough, in a Page Table.  Page Table sizes are
 * integer powers of two based on amount of memory to be mapped and a
 * minimum size of 64 kbytes.  The MINIMUM recommended Page Table sizes
 * for 32-bit PowerPCs are:
 *
 * Total mapped memory		Page Table size
 * -------------------		---------------
 *        8 Meg			     64 K
 *       16 Meg			    128 K
 *       32 Meg			    256 K
 *       64 Meg			    512 K
 *      128 Meg			      1 Meg
 * 	.				.
 * 	.				.
 * 	.				.
 *
 * [Ref: chapter 7, PowerPC Microprocessor Family: The Programming Environments]
 *
 */

PHYS_MEM_DESC sysPhysMemDesc [] =
    {

    {
    /* Vector Table and Interrupt Stack */
	/* From 0 to 0x10000 */
    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    RAM_LOW_ADRS - LOCAL_MEM_LOCAL_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    /* RAM on 60x bus - Must be second entry for Auto Sizing */
	/* From 0x10000 to the SDRAM SIZE */
    (void *) RAM_LOW_ADRS,
    (void *) RAM_LOW_ADRS,
    LOCAL_MEM_SIZE -  RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE |
    VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      |
    VM_STATE_CACHEABLE | VM_STATE_MEM_COHERENCY
    },

    {
    /* MPC8260 Internal Memory Map */

    (void *) INTERNAL_MEM_MAP_ADDR,
    (void *) INTERNAL_MEM_MAP_ADDR,
    INTERNAL_MEM_MAP_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

#ifdef TSS
    {
    /* CS1 : FLASH */

    (void *) CS1_START,
    (void *) CS1_START,
    CS1_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    /* CS3 : ADSL */

    (void *) CS3_START,
    (void *) CS3_START,
    CS3_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    /* CS4 : BP */

    (void *) CS4_START,
    (void *) CS4_START,
    CS4_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    /* CS5 : BITS */

    (void *) CS5_START,
    (void *) CS5_START,
    CS5_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    /* CS7 : SMU */

    (void *) CS7_START,
    (void *) CS7_START,
    CS7_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },
#endif

#ifdef INCLUDE_PCI
    {
    (void*)CPU_PCI_MEM_ADRS,
    (void*)CPU_PCI_MEM_ADRS,
    CPU_PCI_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    (void*)CPU_PCI_MEMIO_ADRS,
    (void*)CPU_PCI_MEMIO_ADRS,
    CPU_PCI_MEMIO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    (void*)CPU_PCI_IO_ADRS,
    (void*)CPU_PCI_IO_ADRS,
    CPU_PCI_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },
    {
    /* MPC8266 PCI Interrupt Controller  */
    (void *) PCI_BRIDGE_INTR_CTL_BA,
    (void *) PCI_BRIDGE_INTR_CTL_BA,
    PCI_BRIDGE_INTR_CTL_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    /* MPC8260 Internal Memory Map */

    (void *) PCI_AGENT_IMMR_BA,
    (void *) PCI_AGENT_IMMR_BA,
    PCI_AGENT_IMMR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },
#endif

#ifdef INCLUDE_SECURITY_ENGINE
    {
    (void *) SEC_ENG_BASE_ADRS,
    (void *) SEC_ENG_BASE_ADRS,
    SEC_ENG_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED

    }
#endif /* INCLUDE_SECURITY_ENGINE */
    };

int sysPhysMemDescNumEnt = NELEMENTS (sysPhysMemDesc);

int   sysBus      = BUS;                /* system bus type (VME_BUS, etc) */
int   sysCpu      = CPU;                /* system CPU type (PPC8260) */
char *sysBootLine = BOOT_LINE_ADRS;	/* address of boot line */
char *sysExcMsg   = EXC_MSG_ADRS;	/* catastrophic message area */
int   sysProcNum;			/* processor number of this CPU */
int   sysFlags;				/* boot flags */
char  sysBootHost [BOOT_FIELD_LEN];	/* name of host from which we booted */
char  sysBootFile [BOOT_FIELD_LEN];	/* name of file from which we booted */
BOOL  sysVmeEnable = FALSE;		/* by default no VME */
UINT32   baudRateGenClk;



/* forward declarations */
void sysCpmReset(void);
UINT32 sysChipRev(void);

uint32_t sysDecGet(void);

#ifdef INCLUDE_PCI
STATUS sysPciSpecialCycle (int busNo, UINT32 message);
STATUS sysPciConfigRead   (int busNo, int deviceNo, int funcNo,
			         int offset, int width, void * pData);
STATUS sysPciConfigWrite  (int busNo, int deviceNo, int funcNo,
			         int offset, int width, ULONG data);

#ifdef PCI_BRIDGE_READ_ERRATA_WORKAROUND
/* Partial Fix for Errata on read access to PCI bridge registers */
/* Uses IDMA2 to access instead of direct read.  */
	UINT8   pciBridgeRegisterReadByte(int);
	UINT16  pciBridgeRegisterReadWord(int);
	UINT32  pciBridgeRegisterReadLong(int);

#	undef  PCI_IN_BYTE
#	undef  PCI_IN_WORD
#	undef  PCI_IN_LONG
#	define PCI_IN_BYTE(x) pciBridgeRegisterReadByte(x)
#	define PCI_IN_WORD(x) pciBridgeRegisterReadWord(x)
#	define PCI_IN_LONG(x) pciBridgeRegisterReadLong(x)
#endif  /* PCI_BRIDGE_READ_ERRATA_WORKAROUND */

ULONG sysPciConfAddr = (PCI_CFG_ADR_REG | INTERNAL_MEM_MAP_ADDR);          /* PCI Configuration Address */
ULONG sysPciConfData = (PCI_CFG_DATA_REG | INTERNAL_MEM_MAP_ADDR);          /* PCI Configuration Data */


#include "pci/pciIntLib.c"           /* PCI int support */
#include "pci/pciConfigLib.c"        /* pci configuration library */

/* use show routine */
#ifdef INCLUDE_SHOW_ROUTINES
#include "pci/pciConfigShow.c"       /* pci configuration show routines */
#endif

/* use pci auto config */

#include "pci/pciAutoConfigLib.c"    /* automatic PCI configuration */
#include "sysBusPci.c"               /* pciAutoConfig BSP support file */
#include "mot82xxPci.c"

#endif /* INCLUDE_PCI */

UINT32 vxImmrGet();

#if 0 /* BSP use SMC for serial */
#include "sio/m8260Sio.c"
#else
#include "smc8260Sio.c"
#endif

#if 0 /* BSP use SMC for serial */
#include "sysSerial.c"
#else
#include "sysSmcSerial.c"
#endif

#include "m827xIntrCtl.c"
#include "timer/m8260Timer.c"
#define M82XX_SIO_FIXES

#ifdef INCLUDE_FLASH
#include "mem/flashMem.c"
#include "mem/nvRamToFlash.c"
#else
#include "mem/nullNvRam.c"
#endif

#if 0
#include "mem/m82xxDpramLib.c"
#endif

#ifdef  INCLUDE_MOTFCCEND
/* set the following array to a unique Ethernet hardware address */

/* last 5 nibbles are board specific, initialized in sysHwInit */

unsigned char sysFccEnetAddr [2][6] = {{0x08, 0x00, 0x3e, 0x33, 0x02, 0x01},
				       {0x08, 0x00, 0x3e, 0x33, 0x02, 0x02}};
STATUS sysFccEnetAddrGet (int unit, UCHAR * address);

#endif  /* INCLUDE_MOTFCCEND */

/* locals */
STATUS sysFccEnetDisable (UINT32 immrVal, UINT8 fccNum);
void sysControlLED ( int LED, int on);

/* defines */

#define ZERO    0
#define SYS_MODEL       "Motorola MPC8260 ADS"
#define SYS_MODEL_HIP4  "Motorola MPC82xx ADS - HIP4"
#define SYS_MODEL_HIP7  "Motorola MPC82xx ADS - HIP7"

/* needed to enable timer base */
#ifdef INCLUDE_PCI
#define      M8260_DPPC_MASK	0x0C000000 /* bits 4 and 5 */
#define      M8260_DPPC_VALUE	0x0C000000 /* bits (4,5) should be (1,0) */
#else
#define      M8260_DPPC_MASK	0x0C000000 /* bits 4 and 5 */
#define      M8260_DPPC_VALUE	0x08000000 /* bits (4,5) should be (1,0) */
#endif /*INCLUDE_PCI */

/* #include "drv/sio/m8260Sio.h" */
#ifdef  INCLUDE_MOTFCCEND
#   include "sysMotFcc2End.c"
#endif /* INCLUDE_MOTFCCEND */

#ifdef INCLUDE_FEI_END
#   include "sysFei82557End.c"
#endif /* INCLUDE_FEI_END */

#ifdef PCI_BRIDGE_READ_ERRATA_WORKAROUND
UCHAR	sysInByte(ULONG port)
{
return(pciBridgeRegisterReadByte(port));
}
USHORT	sysInWord(ULONG port)
{
return(pciBridgeRegisterReadWord(port));
}
ULONG	sysInLong(ULONG port)
{
return(pciBridgeRegisterReadLong(port));
}
#endif /*PCI_BRIDGE_READ_ERRATA_WORKAROUND */

FILE  *sysfwFp;
FILE  *sysConfigFp;

/******************************************************************************
*
* sysModel - return the model name of the CPU board
*
* This routine returns the model name of the CPU board.
*
* RETURNS: A pointer to the string.
*/

char * sysModel (void)
    {
    UINT device;

    if (((device = vxPvrGet()) & HIP4_MASK) == HIP4_ID)
        return(SYS_MODEL_HIP4);
    if (((device = vxPvrGet()) & HIP4_MASK) == HIP7_ID)
        return(SYS_MODEL_HIP7);

    return (SYS_MODEL);
    }

/******************************************************************************
*
* sysBspRev - return the bsp version with the revision eg 1.0/<x>
*
* This function returns a pointer to a bsp version with the revision.
* for eg. 1.0/<x>. BSP_REV defined in config.h is concatanated to
* BSP_VERSION and returned.
*
* RETURNS: A pointer to the BSP version/revision string.
*/

char * sysBspRev (void)
    {
    return (BSP_VERSION BSP_REV);
    }

/******************************************************************************
 *
 * sysHwMemInit - initialize and configure system memory.
 *
 * This routine is called before sysHwInit(). It performs memory auto-sizing
 * and updates the system's physical regions table, `sysPhysRgnTbl'. It may
 * include the code to do runtime configuration of extra memory controllers.
 *
 * NOTE: This routine should not be called directly by the user application.  It
 * cannot be used to initialize interrupt vectors.
 *
 * RETURNS: N/A
 */

void sysHwMemInit (void)
    {
    /* Call sysPhysMemTop() to do memory autosizing if available */

    sysPhysMemDesc[1].len = (UINT)(sysPhysMemTop () -  RAM_LOW_ADRS);
#ifdef INCLUDE_PCI
    sysPhysMemDesc[4].len = (UINT)(sysPhysMemTop ());
#endif
}

#if 1
/******************************************************************************
*
* sysHwInit - initialize the system hardware
*
* This routine initializes various feature of the MPC8260 ADS board. It sets up
* the control registers, initializes various devices if they are present.
*
* NOTE: This routine should not be called directly by the user.
*
* RETURNS: NA
*/

void sysHwInit (void)
    {
    int	immrVal = vxImmrGet();

    /* Issue a Software Reset Command to the CPM core */
    sysCpmReset ();

    /* put correct memory size in sysPhysMemDesc[1].len for
     * size of local memory
     */
    sysHwMemInit();

    /* Zero IO Port registers */
    /* reset the parallel ports */
#if 0
    * M8260_IOP_PADIR(immrVal) = 0x00000000;
    * M8260_IOP_PAPAR(immrVal) = 0x00000000;
    * M8260_IOP_PASO(immrVal) = 0x00000000;
    * M8260_IOP_PAODR(immrVal) = 0x00000000;
    * M8260_IOP_PADAT(immrVal) = 0x00000000;

    * M8260_IOP_PBDIR(immrVal) = 0x00000000;
    * M8260_IOP_PBPAR(immrVal) = 0x00000000;
    * M8260_IOP_PBSO(immrVal) = 0x00000000;
    * M8260_IOP_PBODR(immrVal) = 0x00000000;
    * M8260_IOP_PBDAT(immrVal) = 0x00000000;

    * M8260_IOP_PCDIR(immrVal) = 0x00000000;
    * M8260_IOP_PCPAR(immrVal) = 0x00000000;
    * M8260_IOP_PCSO(immrVal) = 0x00000000;
    * M8260_IOP_PCODR(immrVal) = 0x00000000;
    * M8260_IOP_PCDAT(immrVal) = 0x00000000;

    * M8260_IOP_PDDIR(immrVal) = 0x00000000;
    * M8260_IOP_PDPAR(immrVal) = 0x00000000;
    * M8260_IOP_PDSO(immrVal) = 0x00000000;
    * M8260_IOP_PDODR(immrVal) = 0x00000000;
    * M8260_IOP_PDDAT(immrVal) = 0x00000000;
#endif
    m82xxDpramLibInit();

    /* set the BRGCLK division factor */

    * M8260_SCCR(immrVal) = BRGCLK_DIV_FACTOR;

#if 0
    /* set DPPC in SIUMCR to 10 so that timer is enabled (TBEN) */
	
    * M8260_SIUMCR(immrVal) &= ~M8260_DPPC_MASK;  /* clear the dppc */

    * M8260_SIUMCR(immrVal) |= M8260_DPPC_VALUE;  /* or in the desired value */
#else
    * M8260_SIUMCR(immrVal) &= ~M8260_DPPC_VALUE;  /* EXTMC : 00 */
#endif

#if 0
    /* reset the Communications Processor */

    *M8260_CPCR(immrVal) = M8260_CPCR_RESET | M8260_CPCR_FLG;

    CACHE_PIPE_FLUSH();
#endif

    /* Get the Baud Rate Generator Clock  frequency */

    baudRateGenClk = sysBaudClkFreq();

#if 0
#ifdef INCLUDE_PCI

#ifdef PCI_BRIDGE_READ_ERRATA_WORKAROUND
    /* Setup to do IDMA for PCI Bridge reads */
    pciBridgeIdmaInit();
#endif / *PCI_BRIDGE_READ_ERRATA_WORKAROUND */

    /* config pci */
    if (pciConfigLibInit (PCI_MECHANISM_0,(ULONG) sysPciConfigRead,
                              (ULONG) sysPciConfigWrite,(ULONG) sysPciSpecialCycle) != OK)
        {
        sysToMonitor (BOOT_NO_AUTOBOOT);  /* BAIL */
        }

    /*  Initialize PCI interrupt library. */
    if ((pciIntLibInit ()) != OK)
        {
        sysToMonitor (BOOT_NO_AUTOBOOT);
        }

    if (*((char*)PCI_AUTO_CONFIG_ADRS)==FALSE)
        {
	mot82xxBridgeInit();
        if (PCI_CFG_TYPE == PCI_CFG_AUTO)
            sysPciAutoConfig();

        *((char*)PCI_AUTO_CONFIG_ADRS)=TRUE;
	}
#endif /* INCLUDE_PCI */
#endif

    /* Reset serial channels */

    sysSerialHwInit();

    /* Initialize interrupts */

    m8260IntrInit();

#if 0
    /* Initialize the Security Block */
    *M82XX_SECMR (immrVal) = 0xfffe0000;    
    *M82XX_SECBR (immrVal) = SEC_ENG_BASE_ADRS | 0x1;
#endif
    /*
     * The power management mode is initialized here. Reduced power mode
     * is activated only when the kernel is iddle (cf vxPowerDown).
     * Power management mode is selected via vxPowerModeSet().
     * DEFAULT_POWER_MGT_MODE is defined in config.h.
     */

    vxPowerModeSet (DEFAULT_POWER_MGT_MODE);


    }
#endif

#if 0
/***********************************************************************
*
* sysHwInit - initialize the system hardware
*
* This routine initializes various feature of the EST MPC8260 board. It sets
* up the control registers, initializes various devices if they are present.
*
* NOTE: This routine should not be called directly by the user.
*
* RETURNS: N/A
*/

void sysHwInit
    ( 
    void 
    )
    {
    volatile UINT32 temp;
    int             ix;
    ULONG         * pDPRAM1;

#ifdef INCLUDE_VWARE_LAUNCH
    char vwareMacAdrs[VWARE_MAC_ADRS_LEN];
#endif /* INCLUDE_VWARE_LAUNCH */

    /* Get the physical location of the IMMR register */
    int immrVal = vxImmrGet ();

    /* Issue a Software Reset Command to the CPM core */
    sysCpmReset ();

    /* Initialize DPRAM1 to zero.  */
    pDPRAM1 = (unsigned long *)immrVal;

    for (ix = 0; ix < 0x1000; ix++)
        *pDPRAM1++ = 0;

    /*  reset port A, B, C, and D */
    *M8260_IOP_PAPAR( immrVal ) = 0x00000000;
    *M8260_IOP_PADIR( immrVal ) = 0x00000000;
    *M8260_IOP_PAODR( immrVal ) = 0x00000000;

    *M8260_IOP_PBPAR( immrVal ) = 0x00000000;
    *M8260_IOP_PBDIR( immrVal ) = 0x00000000;
    *M8260_IOP_PBODR( immrVal ) = 0x00000000;

    *M8260_IOP_PCPAR( immrVal ) = 0x00000000;
    *M8260_IOP_PCDIR( immrVal ) = 0x00000000;
    *M8260_IOP_PCSO( immrVal )  = 0x00000000;

    *M8260_IOP_PDPAR( immrVal ) = 0x00000000;
    *M8260_IOP_PDDIR( immrVal ) = 0x00000000;
    *M8260_IOP_PDSO( immrVal )  = 0x00000000;

    /*
     * Initialize SICR.
     */
    *M8260_SICR( immrVal ) = 0x00000000;

    /*
     * Initialize interrupts - default interrupt level.
     */
    m8260IntrInit ();

    if (sysChipRev () == SILICON_REV_PREA1)
        {
        temp = *M8260_SIUMCR( immrVal );
#if 0  /* TAIFAN_MOD */
        temp &= 0xf3ffffff;    /* First clear DPPC ...   */
        temp |= 0x08000000;    /* ... then enable TBEN. */
#else
        temp &= ~(0x0C000000);    /* First clear DPPC ..., EXTMC = 00   */
#endif
        *M8260_SIUMCR( immrVal ) = temp;
        }

    /* 
     * Init the DPRAM lib 
     */

    m82xxDpramLibInit ();

    /*
     * Reset serial channels.
     */

    sysSerialHwInit ();
    
    /*
     * Setup port D for SMC1.
     */

    *M8260_IOP_PDPAR( immrVal ) |= (PD8 | PD9);
    *M8260_IOP_PDDIR( immrVal ) |=  PD9;

    /*
     * Setup port A for SMC2.
     */

    *M8260_IOP_PAPAR( immrVal ) |= (PA8 | PA9);
    *M8260_IOP_PADIR( immrVal ) |=  PA9;

#ifdef  FORCE_DEFAULT_BOOT_LINE
    strncpy (sysBootLine,DEFAULT_BOOT_LINE,strlen(DEFAULT_BOOT_LINE)+1);
#elif defined INCLUDE_VWARE_LAUNCH
    if (sysVwareBuildBootLine ((char*)&vwareMacAdrs))
	{
	sysEnetAddrSet(vwareMacAdrs[0], 
	               vwareMacAdrs[1], 
		       vwareMacAdrs[2], 
		       vwareMacAdrs[3],
		       vwareMacAdrs[4],
		       vwareMacAdrs[5]
		      );
	}
#endif /* FORCE_DEFAULT_BOOT_LINE */

#ifdef INCLUDE_MOTSCCEND
    /* make sure Ethernet is disabled */
    sysSccEnetDisable   ( 0 ) ;
    sysSccEnetIntDisable( 0 ) ;
#endif /* INLUCDE_MOTSCCEND */

#ifdef INCLUDE_MOTFCCEND
    sysFccEnetDisable (immrVal, 2);
#endif /* INCLUDE_MOTFCCEND */
    }
#endif

/*******************************************************************************
*
* sysPhysMemTop - get the address of the top of physical memory
*
* This routine returns the address of the first missing byte of memory,
* which indicates the top of memory.
*
* Determine installed memory by reading memory control registers
* and calculating if one or 2 chip selects are used for SDRAM.
* Use the address mask and valid bit to determine each bank size.
*
* RETURNS: The address of the top of physical memory.
*
* SEE ALSO: sysMemTop()
*/

char * sysPhysMemTop (void)
    {
    LOCAL char * physTop = NULL;

    physTop = (char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);

    return (physTop) ;
    }

/*******************************************************************************
*
* sysMemTop - get the address of the top of VxWorks memory
*
* This routine returns a pointer to the first byte of memory not
* controlled or used by VxWorks.
*
* The user can reserve memory space by defining the macro USER_RESERVED_MEM
* in config.h.  This routine returns the address of the reserved memory
* area.  The value of USER_RESERVED_MEM is in bytes.
*
* RETURNS: The address of the top of VxWorks memory.
*/

char * sysMemTop (void)
    {
    LOCAL char * memTop = NULL;

    memTop = sysPhysMemTop () - USER_RESERVED_MEM;

    return memTop;
    }


/******************************************************************************
*
* sysToMonitor - transfer control to the ROM monitor
*
* This routine transfers control to the ROM monitor.  Normally, it is called
* only by reboot()--which services ^X--and bus errors at interrupt level.
* However, in some circumstances, the user may wish to introduce a
* <startType> to enable special boot ROM facilities.
*
* RETURNS: Does not return.
*/

STATUS sysToMonitor
    (
     int startType	/* parameter passed to ROM to tell it how to boot */
    )
    {
    FUNCPTR pRom = (FUNCPTR) (ROM_TEXT_ADRS + 8);	/* Warm reboot */

    intLock();

    cacheDisable(INSTRUCTION_CACHE);
    cacheDisable(DATA_CACHE);

    sysAuxClkDisable();

    /* disable both RS232 ports on the board */
#if 0 /*BSP_RMV */
    *BCSR1 |= (BCSR1_RS232EN_1_L);
    *BCSR1 |= (BCSR1_RS232EN_2_L);
#endif
    sysSerialReset();		/* reset the serial device */

    vxMsrSet (0);

    (*pRom) (startType);	/* jump to bootrom entry point */

    return (OK);	/* in case we ever continue from ROM monitor */
    }

/******************************************************************************
*
* sysHwInit2 - additional system configuration and initialization
*
* This routine connects system interrupts and does any additional
* configuration necessary.
*
* RETURNS: NA
*/

void sysHwInit2 (void)
    {
    LOCAL BOOL configured = FALSE;
    int immrVal = vxImmrGet();

    CACHE_PIPE_FLUSH();


    if (!configured)
	{

	/* initialize serial interrupts */

	sysSerialHwInit2();

	* M8260_SCCR(immrVal) &= ~M8260_SCCR_TBS;
	CACHE_PIPE_FLUSH();

	configured = TRUE;

#ifdef INCLUDE_FEI_END
        sys557PciInit ();            /* Intel 82557/9 device */
#endif /* INCLUDE_FEI_END */

#ifdef INCLUDE_PCI_DMA
	pciDmaInit();
#endif /* INCLUDE_PCI_DMA */

#ifdef INCLUDE_PCI_ERROR_HANDLING
        pciErrorHandlingInit();
#endif /* INCLUDE_PCI_ERROR_HANDLING */

	}
    }

/******************************************************************************
*
* sysProcNumGet - get the processor number
*
* This routine returns the processor number for the CPU board, which is
* set with sysProcNumSet().
*
* RETURNS: The processor number for the CPU board.
*
* SEE ALSO: sysProcNumSet()
*/

int sysProcNumGet (void)
    {
    return (sysProcNum);
    }

/******************************************************************************
*
* sysProcNumSet - set the processor number
*
* This routine sets the processor number for the CPU board.  Processor numbers
* should be unique on a single backplane.
*
* Not applicable for the busless 8260Ads.
*
* RETURNS: NA
*
* SEE ALSO: sysProcNumGet()
*
*/

void sysProcNumSet
    (
    int 	procNum			/* processor number */
    )
    {
    sysProcNum = procNum;
    }

/******************************************************************************
*
* sysLocalToBusAdrs - convert a local address to a bus address
*
* This routine gets the VMEbus address that accesses a specified local
* memory address.
*
* Not applicable for the 8260Ads
*
* RETURNS: ERROR, always.
*
* SEE ALSO: sysBusToLocalAdrs()
*/

STATUS sysLocalToBusAdrs
    (
    int 	adrsSpace,	/* bus address space where busAdrs resides */
    char *	localAdrs,	/* local address to convert */
    char **	pBusAdrs	/* where to return bus address */
    )
    {
    return (ERROR);
    }

/******************************************************************************
*
* sysBusToLocalAdrs - convert a bus address to a local address
*
* This routine gets the local address that accesses a specified VMEbus
* physical memory address.
*
* Not applicable for the 8260Ads
*
* RETURNS: ERROR, always.
*
* SEE ALSO: sysLocalToBusAdrs()
*/

STATUS sysBusToLocalAdrs
    (
     int  	adrsSpace, 	/* bus address space where busAdrs resides */
     char *	busAdrs,   	/* bus address to convert */
     char **	pLocalAdrs 	/* where to return local address */
    )
    {
    return (ERROR);
    }

/******************************************************************************
*
* sysBusTas - test and set a location across the bus
*
* This routine does an atomic test-and-set operation across the backplane.
*
* Not applicable for the 8260Ads.
*
* RETURNS: FALSE, always.
*
* SEE ALSO: vxTas()
*/

BOOL sysBusTas
    (
     char *	adrs		/* address to be tested-and-set */
    )
    {
    return (FALSE);
    }

/******************************************************************************
*
* sysBusClearTas - test and clear
*
* This routine is a null function.
*
* RETURNS: NA
*/

void sysBusClearTas
    (
     volatile char * address	/* address to be tested-and-cleared */
    )
    {
    }

/*******************************************************************************
*
* vxImmrGet - return the current IMMR value
*
* This routine returns the current IMMR value
*
* RETURNS: current IMMR value
*
*/

UINT32  vxImmrGet (void)
    {

    return (INTERNAL_MEM_MAP_ADDR & IMMR_ISB_MASK);

    }

#ifdef INCLUDE_MOTFCCEND
/*******************************************************************************
*
* sysFccEnetAddrGet - get the hardware Ethernet address
*
* This routine provides the six byte Ethernet hardware address that will be
* used by each individual FCC device unit.  This routine must copy
* the six byte address to the space provided by <addr>.
*
* RETURNS: OK, or ERROR if the Ethernet address cannot be returned.
*/

STATUS sysFccEnetAddrGet
    (
    int         unit,      	/* base address of the on-chip RAM */
    UCHAR *     addr            /* where to copy the Ethernet address */
    )
    {
    bcopy ((char *) sysFccEnetAddr[unit], (char *) addr, sizeof (sysFccEnetAddr));

    return (OK);
    }

#endif	/* INCLUDE_MOTFCCEND */

/**************************************************************************
*
* sysFlashLED - flash one of the signal lamps
*
* This routine will flash the indicated signal lamp for about 1/4 second
*
* SEE ALSO:
*
* RETURNS: NA
*/
void sysFlashLED
    (
    int LED	/* bitmap of LED to flash */
    )
    {
#if 0 /*BSP_RMV */
    sysControlLED(LED, BCSR0_LED_ON);

    taskDelay(15);

    sysControlLED(LED, BCSR0_LED_OFF);
#endif

    }

/**************************************************************************
*
* sysControlLED - control one of the signal lamps
*
* This routine will turn the indicated signal lamp on or off, as
* requested
*
* SEE ALSO:
*
* RETURNS: NA
*/

void sysControlLED
    (
    int LED, 	/* bitmap of LED to control */
    int on	/* if TRUE turn LED ON; otherwise, turn LED OFF */
    )
    {

    CACHE_PIPE_FLUSH();			/* always before first read */
#if 0 /*BSP_RMV */
    if (on)
	*(UINT32 *)BCSR0 &= ~LED; /* clearing bit turns LED on */
    else
	*(UINT32 *)BCSR0 |= LED; /* setting bit turns LED off */
#endif
    }


#ifdef INCLUDE_PCI 	/* board level PCI routines */
/*******************************************************************************
*
* sysPciSpecialCycle - generate a special cycle with a message
*
* This routine generates a special cycle with a message.
*
* NOMANUAL
*
* RETURNS: OK
*/

STATUS sysPciSpecialCycle
    (
    int		busNo,
    UINT32	message
    )
    {
    int deviceNo	= 0x0000001f;
    int funcNo		= 0x00000007;

    PCI_OUT_LONG (sysPciConfAddr,
                  pciConfigBdfPack (busNo, deviceNo, funcNo) |
                  0x80000000);

    PCI_OUT_LONG (sysPciConfData, message);

    return (OK);
    }

/*******************************************************************************
*
* sysPciConfigRead - read from the PCI configuration space
*
* This routine reads either a byte, word or a long word specified by
* the argument <width>, from the PCI configuration space
* This routine works around a problem in the hardware which hangs
* PCI bus if device no 12 is accessed from the PCI configuration space.
*
* NOMANUAL
*
* RETURNS: OK, or ERROR if this library is not initialized
*/

STATUS sysPciConfigRead
    (
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,	  /* function number */
    int	offset,	  /* offset into the configuration space */
    int width,	  /* width to be read */
    void * pData /* data read from the offset */
    )
    {
    UINT8  retValByte = 0;
    UINT16 retValWord = 0;
    UINT32 retValLong = 0;
    STATUS retStat = ERROR;

    if ((busNo == 0) && (deviceNo == 12))
        return (ERROR);

    switch (width)
        {
        case 1:	/* byte */
            PCI_OUT_LONG (sysPciConfAddr,
                          pciConfigBdfPack (busNo, deviceNo, funcNo) |
                          (offset & 0xfc) | 0x80000000);

            retValByte = PCI_IN_BYTE (sysPciConfData + (offset & 0x3));
            *((UINT8 *)pData) = retValByte;
	    retStat = OK;
            break;

        case 2: /* word */
            PCI_OUT_LONG (sysPciConfAddr,
                          pciConfigBdfPack (busNo, deviceNo, funcNo) |
                          (offset & 0xfc) | 0x80000000);

	    retValWord = PCI_IN_WORD (sysPciConfData + (offset & 0x2));
            *((UINT16 *)pData) = retValWord;
	    retStat = OK;
	    break;

        case 4: /* long */
	    PCI_OUT_LONG (sysPciConfAddr,
		          pciConfigBdfPack (busNo, deviceNo, funcNo) |
		          (offset & 0xfc) | 0x80000000);
	    retValLong = PCI_IN_LONG (sysPciConfData);
            *((UINT32 *)pData) = retValLong;
	    retStat = OK;
            break;

        default:
            retStat = ERROR;
            break;
        }

    return (retStat);
    }

/*******************************************************************************
*
* sysPciConfigWrite - write to the PCI configuration space
*
* This routine writes either a byte, word or a long word specified by
* the argument <width>, to the PCI configuration space
* This routine works around a problem in the hardware which hangs
* PCI bus if device no 12 is accessed from the PCI configuration space.
*
* NOMANUAL
*
* RETURNS: OK, or ERROR if this library is not initialized
*/

STATUS sysPciConfigWrite
    (
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,	  /* function number */
    int	offset,	  /* offset into the configuration space */
    int width,	  /* width to write */
    ULONG data	  /* data to write */
    )
    {
    if ((busNo == 0) && (deviceNo == 12))
        return (ERROR);

    switch (width)
        {
        case 1:	/* byte */
            PCI_OUT_LONG (sysPciConfAddr,
                          pciConfigBdfPack (busNo, deviceNo, funcNo) |
                          (offset & 0xfc) | 0x80000000);
	    PCI_OUT_BYTE ((sysPciConfData + (offset & 0x3)), data);
            break;

        case 2: /* word */
            PCI_OUT_LONG (sysPciConfAddr,
                          pciConfigBdfPack (busNo, deviceNo, funcNo) |
                          (offset & 0xfc) | 0x80000000);
	    PCI_OUT_WORD ((sysPciConfData + (offset & 0x2)), data);
	    break;

        case 4: /* long */
	    PCI_OUT_LONG (sysPciConfAddr,
		          pciConfigBdfPack (busNo, deviceNo, funcNo) |
		          (offset & 0xfc) | 0x80000000);
	    PCI_OUT_LONG (sysPciConfData, data);
            break;

        default:
            return (ERROR);

        }
    return (OK);
    }
#endif /* INCLUDE_PCI */

typedef struct 
{
    unsigned int  length;
	unsigned int  checksum;
	unsigned char reserved[24];
} IMAGE_HEADER;

void serialLoadImage()
{
    int               oldOption;
	unsigned char 	* ptr = (unsigned char *)0xFFE0;
    unsigned int      length;
	unsigned int      checksum;
	unsigned int      i;
	unsigned int      tmp = 0;

    /* wait for the input */
	printf("Ready to Rx the image\r\n");
    
	oldOption = ioctl(STD_IN, FIOGETOPTIONS, 0);
	ioctl(STD_IN, FIOSETOPTIONS, OPT_RAW);

    for(i=0; i<0x20; i++)
	{
        *ptr = getchar();
		ptr ++;
	}

	length   = ((IMAGE_HEADER *) 0xFFE0)->length;
	checksum = ((IMAGE_HEADER *) 0xFFE0)->checksum;
		    
    printf("length: 0x%x\r\n", length);
    printf("checksum:0x%x\r\n", checksum);

    if((length < (unsigned int)0x300000) && (length > 0))
	{
        for(i=0; i<length; i++)
		{
		    *ptr = getchar();
			tmp += *ptr;
			ptr ++;
			if((i%100) == 0)
			    printf(".");
		}
		if(tmp == checksum)
		{
		    printf("checksum matched\r\n");
		}
		else
		{
		    printf("checksum not matched, file: 0x%x, calc: 0x%x\r\n", checksum, tmp);
		}
	}
	ioctl(STD_IN, FIOSETOPTIONS, oldOption);

    
    printf("Download length: 0x%x\r\n", length);
        
}

int startFm()
{
    FUNCPTR pRom = (FUNCPTR) (0x10000);	/* Warm reboot */

    intLock();

    cacheDisable(INSTRUCTION_CACHE);
    cacheDisable(DATA_CACHE);

    sysAuxClkDisable();

    sysSerialReset();		/* reset the serial device */

    vxMsrSet (0);

    (*pRom) ();	/* jump to bootrom entry point */

    return (OK);	/* in case we ever continue from ROM monitor */    
}

int readFwImgae()
{
    int len;

    if((sysfwFp = fopen("C:image/tssfw.bin", "r")) == NULL)
	{
	    printf("tssfw Open Failed\r\n");
	}
	len = fread((FUNCPTR) (0xFFE0), 1, 0x300000, sysfwFp);
	printf("Read sysFw Image: len %d\r\n", len);

    return OK;
}

#ifdef BOOT
int goFw()
{
    readFwImgae();

    startFm();
}
#endif

void bootOrFw()
{
#ifdef BOOT
    printf("Boot\n");
#else
    printf("FW\n");
#endif
}

void initPIO()
{
    int	immrVal = vxImmrGet();

    * M8260_IOP_PADIR(immrVal) = 0x00000000;
    * M8260_IOP_PAPAR(immrVal) = 0x00000000;
    * M8260_IOP_PASO(immrVal)  = 0x00000000;
    * M8260_IOP_PAODR(immrVal) = 0x00000000;
    * M8260_IOP_PADAT(immrVal) = 0x00000000;

    * M8260_IOP_PBDIR(immrVal) = 0x00000000;
    * M8260_IOP_PBPAR(immrVal) = 0x00000000;
    * M8260_IOP_PBSO(immrVal)  = 0x00000000;
    * M8260_IOP_PBODR(immrVal) = 0x00000000;
    * M8260_IOP_PBDAT(immrVal) = 0x00000000;

    * M8260_IOP_PCDIR(immrVal) = 0x00000000;
    * M8260_IOP_PCPAR(immrVal) = 0x00000000;
    * M8260_IOP_PCSO(immrVal)  = 0x00000000;
    * M8260_IOP_PCODR(immrVal) = 0x00000000;
    * M8260_IOP_PCDAT(immrVal) = 0x00000000;

    * M8260_IOP_PDDIR(immrVal) = 0x00000000;
    * M8260_IOP_PDPAR(immrVal) = 0x00000000;
    * M8260_IOP_PDSO(immrVal)  = 0x00000000;
    * M8260_IOP_PDODR(immrVal) = 0x00000000;
    * M8260_IOP_PDDAT(immrVal) = 0x00000000;

  
}

/***********************************************************************
*
* sysCpmReset - issues a CPM reset command
*
* This routine issues a CPM reset command
*
* RETURNS: N/A
*
*/
   
void sysCpmReset
    (
      void
    )
    {
    /* Get the location of the IMMR register.                               */
    int immrVal = vxImmrGet ();

    /* Wait for any previous commands to finish                             */
    while ( *M8260_CPCR( immrVal ) & M8260_CPCR_FLG )
	{}

    *M8260_CPCR( immrVal ) =  M8260_CPCR_RESET | M8260_CPCR_FLG;

    /* See if the command has been accepted.                                */
    while ( *M8260_CPCR( immrVal ) & M8260_CPCR_FLG )
        {}

    return;
    }


/***********************************************************************
*
* sysChipRev - determines revision of Chip installed
*
* This routine determines revision of Chip installed
*
* RETURNS: Chip revision
*
*/

UINT32 sysChipRev
    (
    void
    )
    {
    UINT32  immrRegAddr = vxImmrGet ();
    UINT32  immrValue;

    immrRegAddr += 0x101A8;
    immrValue = *(UINT32 *)immrRegAddr;
    immrValue &= MASKNUM_MASK;

    return (immrValue);    
    }

void sysMonLoop()
{
	tffsAppInit();
	initLED();

	/*
	original
	ledOp(LED_STATUS,         LED_ON, 0);
	
	ledOp(LED_STATUS_PLUS,         LED_ON, 0);
	ledOp(LED_STATUS_MINUS,         LED_ON, 0);
	*/
	ledOp(LED_SERIAL_PORT,    LED_ON, 0);
	ledOp(LED_IP_A_TRACK,     LED_ON, 0);
	ledOp(LED_CRITICAL_ALARM, LED_ON, 0);
	ledOp(LED_MAJOR_ALARM,    LED_ON, 0);
	ledOp(LED_MINOR_ALARM,    LED_ON, 0);

#if 0
    if((sysConfigFp = fopen("C:config/sysConfig.txt", "rb")) == NULL)
	{
	    printf("sysConfig.txt open failed\r\n");
	}
	len = fread((FUNCPTR) &sysConfig, 1, 20, sysConfigFp);
    printf("sysConfig File len:%d\r\n", len);
#endif


    for(;;)
	{
	    taskDelay(100);
	}
}

int sntpsDbg = 0;
extern 	struct timespec    sntpTime;
extern ULONG ppsTimeStamp;

STATUS sntpsClockHook( int request, void * pBuffer)
{
    ULONG currentTimeStamp = 0;
    /* clock_gettime(CLOCK_REALTIME, &time); */
    switch(request)
	{
	     case SNTPS_ID:
		     if(sntpsDbg)
		         printf("SNTPS_ID\n");
		     *(ULONG *)pBuffer = 1;
		 break;

         case SNTPS_RESOLUTION:
		     if(sntpsDbg)
		         printf("SNTPS_RESOLUTION\n");
		     *(ULONG *)pBuffer = (long)sntpTime.tv_nsec;
		 break;

         case SNTPS_TIME:
		     if(sntpsDbg)
			 {
		         printf("SNTPS_TIME\n");
			 }
		     ((SNTP_TIMESTAMP *)pBuffer)->seconds  = (ULONG)(sntpTime.tv_sec);
			 currentTimeStamp = sysTimestamp();
			 if(currentTimeStamp - ppsTimeStamp > 0x80000000)
			 {
			     sntpTime.tv_nsec = (ppsTimeStamp - currentTimeStamp)*10;
			 }
			 else
			 {
			     sntpTime.tv_nsec = (currentTimeStamp - ppsTimeStamp)*10;
			 }

		     ((SNTP_TIMESTAMP *)pBuffer)->fraction = (ULONG)sntpsNsecToFraction(sntpTime.tv_nsec);
		     if(sntpsDbg)
			 {
		         printf("SNTP Sec:0x%x, nSec:0x%x, fraction:0x%x\n", sntpTime.tv_sec, sntpTime.tv_nsec, ((SNTP_TIMESTAMP *)pBuffer)->fraction);
			 }

		 break;

         default:
		     return ERROR;
		 break;
	 }
	 return OK;
}


void  sys2msHandler()
{
    /* logMsg("Aux Clock\r\n", 0,0,0,0,0,0); */
}
STATUS sysAuxClockInit()
{
    sysAuxClkConnect(sys2msHandler, 0);
	sysAuxClkRateSet(500);
	sysAuxClkEnable();
}