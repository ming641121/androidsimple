/* romInit.s - Motorola ads827x ROM initialization module */

/* Copyright 1984-2003 Wind River Systems, Inc. */
	.data
	.globl  copyright_wind_river
	.long   copyright_wind_river

/*
modification history
--------------------
01a,23dec03,dtr  create from ads826x/romInit.s
*/

/*
DESCRIPTION
This module contains the entry code for the VxWorks bootrom.
The entry point romInit, is the first code executed on power-up.
It sets the BOOT_COLD parameter to be passed to the generic
romStart() routine.

The routine sysToMonitor() jumps to the location 8 bytes
past the beginning of romInit, to perform a "warm boot".
This entry point allows a parameter to be passed to romStart().

*/

#define	_ASMLANGUAGE
#include "vxWorks.h"
#include "asm.h"
#include "cacheLib.h"
#include "config.h"
#include "regs.h"	
#include "sysLib.h"
#include "drv/timer/m8260Clock.h"
#include "drv/mem/m8260Siu.h"
#include "drv/mem/m8260Memc.h"
#include "sys_memmap.h"
#include "m8260Pio.h"

	/* internals */

	FUNC_EXPORT(_romInit)		/* start of system code */
	FUNC_EXPORT(romInit)		/* start of system code */

	/* externals */

	FUNC_IMPORT(romStart)	/* system initialization routine */

/*
* A flash based Hard Reset Config Word can be used to boot the board by
* changing jumper JP3.  However the reset word must be located at the
* base of flash memory.  The directives below configure a hard reset
* config word at ROM_BASE_ADRS, which is usually 0xFFF00000.
*
* The following defines what the hard reset config word should look like,
* but the table is located at the wrong address.
* The following must be programmed into the first 32 bytes of flash
* to use it as a hard reset configuration word.
*/
	.text                       /* byte 0 (MSByte) of the configuration master's */
	.fill   1,1,HRCW_BYTE_0     /* Hard Reset Configuration Word */
	.fill   7,1,0
	.fill   1,1,HRCW_BYTE_1
	.fill   7,1,0
	.fill   1,1,HRCW_BYTE_2
	.fill   7,1,0
	.fill   1,1,HRCW_BYTE_3		/* This is the LSByte */
	.fill   231,1,0     		/* The rest of the space are filled with zeros */

	.align 2

/******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*
*
* romInit
*		(
*		int startType	/@ only used by 2nd entry point @/
*		)

*/

FUNC_BEGIN(_romInit)
FUNC_BEGIN(romInit)

	b	  	cold		/* jump to the cold boot initialization */
	nop
	mr		r31, r3		/* save	passed parameter in r31 */
	b	  	warm		/* jump to the warm boot initialization */
						/* r3 should have BOOT_WARM value */

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 1984-2003 Wind River Systems, Inc."
	.align 2

cold:
	li		r31, BOOT_COLD	/* set cold boot as start type */
							/* saved in r31 to later pass as */
							/* argument to romStart() */
warm:
	li		r1, BOOT_COLD	/* need to see if a warm or cold */
	xor.	r1, r1, r31		/* start to jump around IMMR stuff */
	bne		start 			/* jump if not BOOT_COLD */

	/*
	 * initialize the IMMR register before any non-core register
	 * modifications. The default IMMR base address was 0x0F000000,
	 * as originally programmed in the Hard Reset Configuration Word.
	 * ONLY IF A COLD BOOT
	 */

	/* put desired internal memory map base address in r4 */
	lis		r4, HIADJ (INTERNAL_MEM_MAP_ADDR)
	addi	r4, r4, LO (INTERNAL_MEM_MAP_ADDR)

	/* get the reset value of IMM base address */
	lis		r8, HIADJ (RST_INTERNAL_MEM_MAP_ADDR + PQII_REG_BASE_OFF)
	addi	r8, r8, LO (RST_INTERNAL_MEM_MAP_ADDR + PQII_REG_BASE_OFF)
	stw		r4, IMMR_OFFSET(r8)		/* IMMR now at bsp specified value */
	isync

	/* SYPCR - turn off the system protection stuff */

	lis		r5, HIADJ (M8260_SYPCR_SWTC | M8260_SYPCR_BMT | \
                       M8260_SYPCR_PBME | M8260_SYPCR_LBME | M8260_SYPCR_SWP)
	addi	r5, r5, LO (M8260_SYPCR_SWTC | M8260_SYPCR_BMT | \
                        M8260_SYPCR_PBME | M8260_SYPCR_LBME | M8260_SYPCR_SWP)
	lis 	r6, HIADJ (M8260_SYPCR (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_SYPCR (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

start:
	/* now that IMMR is set, use dual ported ram for a temp stack */
	lis		sp, HIADJ (INTERNAL_MEM_MAP_ADDR + DPRAM1_SIZE)
	addi	sp,	sp, LO (INTERNAL_MEM_MAP_ADDR + DPRAM1_SIZE)

	/*
	 * Map the bank 0 to the Boot Rom : 0xFFF00000, 512K, 8bit port, valid
	 */

	/* load the base register 0 */
	lis		r5, HIADJ ((BOOT_BASE_ADRS & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((BOOT_BASE_ADRS & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR0 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR0 (INTERNAL_MEM_MAP_ADDR))

	/* load the option register 0 */
	stw 	r5, 0(r6)
	lis	r5, HIADJ((BOOT_BASE_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((BOOT_BASE_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_SCY_7_CLK)
	lis 	r6, HIADJ (M8260_OR0 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR0 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

#if 0 /* MARVIN_DEBUG */
	/* Direction C14, C15 out */
	lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

    /* Data,  */
    lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)
#endif

#ifdef SCM
	/* CS1 : FLASH
	 * Map the bank 1 to the Flash : 0xB0000000, 128M, 64bit port, valid
	 * SYS_FLASH_BASE_ADRS: 0xB0000000
	 * SYS_FLASH_SIZE_MASK: 0xF8000000
	 */

	/* load the option register 1 */
	lis	r5, HIADJ((SYS_FLASH_SIZE_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV1 | M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((SYS_FLASH_SIZE_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV1 | M8260_OR_SCY_7_CLK)

	lis 	r6, HIADJ (M8260_OR1 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR1 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 1 */
	lis		r5, HIADJ ((SYS_FLASH_BASE_ADRS & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_64 | M8260_BR_V)
	addi	r5,	r5, LO ((SYS_FLASH_BASE_ADRS & M8260_BR_BA_MSK) | \
						M8260_BR_PS_64 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR1 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR1 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


    /* CS3 : RTC, Size 32B, 8 bit port */

	/* load the option register 3 */
	lis	r5, HIADJ((CS3_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV1 | M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((CS3_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV1 | M8260_OR_SCY_7_CLK)
	lis 	r6, HIADJ (M8260_OR3 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR3 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 3 */
	lis		r5, HIADJ ((CS3_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS3_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR3 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR3 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


	/* CS5 : CPLD, Size 4K, 8 bit port */

	/* load the option register 5 */
	lis	r5, HIADJ((CS5_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((CS5_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	lis 	r6, HIADJ (M8260_OR5 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR5 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 5 */
	lis		r5, HIADJ ((CS5_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS5_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR5 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR5 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


    /* CS7 : FPGA */

	/* load the option register 7 */
	lis	r5, HIADJ((CS7_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((CS7_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	lis 	r6, HIADJ (M8260_OR7 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR7 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 7 */
	lis		r5, HIADJ ((CS7_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS7_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR7 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR7 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)
#endif


#ifdef ADSL24V

/* CS1 : FLASH */

	/*
	 * Map the bank 1 to the Flash : 0xB0000000, 4M, 16bit port, valid
	 * SYS_FLASH_BASE_ADRS: 0xB0000000
	 * SYS_FLASH_SIZE_MASK: 0xFFC00000
	 */

	/* load the option register 1 */
	lis	r5, HIADJ((SYS_FLASH_SIZE_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_1 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV4 | M8260_OR_SCY_10_CLK)
	addi	r5,	r5, LO ((SYS_FLASH_SIZE_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_1 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV4 | M8260_OR_SCY_10_CLK)
	lis 	r6, HIADJ (M8260_OR1 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR1 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 1 */
	lis		r5, HIADJ ((SYS_FLASH_BASE_ADRS & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_16 | M8260_BR_V)
	addi	r5,	r5, LO ((SYS_FLASH_BASE_ADRS & M8260_BR_BA_MSK) | \
						M8260_BR_PS_16 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR1 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR1 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


/* CS3 : ADSL */

	/* load the option register 3 */
	lis	r5, HIADJ((CS3_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK | M8260_OR_TRLX)



	addi	r5,	r5, LO ((CS3_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK | M8260_OR_TRLX)
	lis 	r6, HIADJ (M8260_OR3 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR3 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 3 */
	lis		r5, HIADJ ((CS3_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS3_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR3 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR3 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


/* CS4 : VF */

	/* load the option register 4 */
	lis	r5, HIADJ((CS4_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((CS4_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	lis 	r6, HIADJ (M8260_OR4 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR4 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)


	/* load the base register 4 */
	lis		r5, HIADJ ((CS4_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_16 | M8260_BR_V)
	addi	r5,	r5, LO ((CS4_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_16 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR4 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR4 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


/* CS5 : CPLD */

	/* load the option register 5 */
	lis	r5, HIADJ((CS5_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((CS5_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	lis 	r6, HIADJ (M8260_OR5 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR5 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 5 */
	lis		r5, HIADJ ((CS5_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS5_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR5 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR5 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


/* CS7 : FPGA */

	/* load the option register 7 */
	lis	r5, HIADJ((CS7_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((CS7_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	lis 	r6, HIADJ (M8260_OR7 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR7 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 7 */
	lis		r5, HIADJ ((CS7_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS7_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR7 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR7 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

#endif

#ifdef TSS
	/* CS1 : FLASH
	 * Map the bank 1 to the Flash : 0xB0000000, 16MB, 16bit port, valid
	 * SYS_FLASH_BASE_ADRS: 0xB0000000
	 * SYS_FLASH_SIZE_MASK: 0xFF000000
	 */

	/* load the option register 1 */
	lis	r5, HIADJ((SYS_FLASH_SIZE_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV1 | M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((SYS_FLASH_SIZE_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV1 | M8260_OR_SCY_7_CLK)

	lis 	r6, HIADJ (M8260_OR1 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR1 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 1 */
	lis		r5, HIADJ ((SYS_FLASH_BASE_ADRS & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_16 | M8260_BR_V)
	addi	r5,	r5, LO ((SYS_FLASH_BASE_ADRS & M8260_BR_BA_MSK) | \
						M8260_BR_PS_16 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR1 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR1 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

/* CS3 : FPGA */

	/* load the option register 3 */
	lis	r5, HIADJ((CS3_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK | M8260_OR_TRLX)



	addi	r5,	r5, LO ((CS3_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK | M8260_OR_TRLX)
	lis 	r6, HIADJ (M8260_OR3 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR3 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 3 */
	lis		r5, HIADJ ((CS3_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS3_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR3 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR3 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


/* CS4 : BP */

	/* load the option register 4 */
	lis	r5, HIADJ((CS4_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK | M8260_OR_TRLX)
	addi	r5,	r5, LO ((CS4_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK | M8260_OR_TRLX)
	lis 	r6, HIADJ (M8260_OR4 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR4 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)


	/* load the base register 4 */
	lis		r5, HIADJ ((CS4_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS4_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR4 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR4 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


/* CS5 : BITS */

	/* load the option register 5 */
	/* load the option register 5 */
	lis	r5, HIADJ((CS5_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV1 | M8260_OR_SCY_7_CLK | M8260_OR_TRLX)
	addi	r5,	r5, LO ((CS5_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV1 | M8260_OR_SCY_7_CLK | M8260_OR_TRLX)
	lis 	r6, HIADJ (M8260_OR5 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR5 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 5 */
	lis		r5, HIADJ ((CS5_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS5_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR5 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR5 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


/* CS7 : SMU */

	/* load the option register 7 */
	lis	r5, HIADJ((CS7_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	addi	r5,	r5, LO ((CS7_MASK & M8260_OR_AM_MSK) | \
				M8260_OR_EHTR_8 | M8260_OR_CSNT_EARLY | \
				M8260_OR_ACS_DIV2 | M8260_OR_SCY_7_CLK)
	lis 	r6, HIADJ (M8260_OR7 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_OR7 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* load the base register 7 */
	lis		r5, HIADJ ((CS7_START & M8260_BR_BA_MSK) | \
					   M8260_BR_PS_8 | M8260_BR_V)
	addi	r5,	r5, LO ((CS7_START & M8260_BR_BA_MSK) | \
						M8260_BR_PS_8 | M8260_BR_V)
	lis 	r6, HIADJ (M8260_BR7 (INTERNAL_MEM_MAP_ADDR))
	addi	r6,	r6, LO (M8260_BR7 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)


#endif
	/*
	 * Initialize all necessary core registers before continuing
	 *
	 * 1. Clear MSR to disable all exceptions, no mmu, no fpu etc
	 * 2. Clear out the SPRGs
	 * 3. Clear out the Segment Registers
	 * 4. Clear Instruction BAT registers
	 * 5. Clear Data BAT registers
	 * 6. Initialize FPU registers
	 *
	 */
	bl		sysClearMSR
	bl		sysClearSPRGs
	bl		sysClearSRs
	bl		sysClearIBATs
	bl		sysClearDBATs
	bl		sysClearFPRegs


	/* program the SCCR: normal operation */

	lis 	r5, HIADJ (M8260_SCCR_DFBRG_16)
	addi	r5, r5, LO (M8260_SCCR_DFBRG_16)

	lis 	r6, HIADJ (M8260_SCCR (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_SCCR (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

#if 0  /* TAIFAN_REMOVE */
#if DEBUG
	li		r3,1
	bl		flashRedLed
#endif
#endif


	/* program the BCR */

	lis     r5, 0x100c
	lis 	r6, HIADJ (M8260_BCR (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_BCR (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0 (r6)

	/* program the PPC_ACR */

	addi	r5, 0, 0x02
	lis 	r6, HIADJ (M8260_PPC_ACR (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PPC_ACR (INTERNAL_MEM_MAP_ADDR))
	stb 	r5, 0(r6)

	/* program the PPC_ALRH */
#ifdef INCLUDE_PCI
	addis   r5, 0, 0x3012
	ori		r5, r5, 0x6745
#else
	addis   r5, 0, 0x0126
	ori		r5, r5, 0x7893
#endif INCLUDE_PCI
	lis 	r6, HIADJ (M8260_PPC_ALRH (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PPC_ALRH (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

	/* program the TESCR1 */

	addis   r5, 0, 0x0000
	ori	r5, r5, 0x4000
	lis 	r6, HIADJ (M8260_TESCR1 (INTERNAL_MEM_MAP_ADDR))	
	addi	r6, r6, LO (M8260_TESCR1 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

	/* program the LTESCR1 */

	addis   r5, 0, 0x0000
	ori	r5, r5, 0x0000
	lis 	r6, HIADJ (M8260_LTESCR1 (INTERNAL_MEM_MAP_ADDR))	
	addi	r6, r6, LO (M8260_LTESCR1 (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

#if 0 /* MARVIN_DEBUG */
	/* Direction C14, C15 out */
	lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

    /* Data,  */
    lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)
#endif
	
SdramInit:

	/* program the MPTPR */
#if 0 /* ORG */
	addi    r5,0,0x2800	 /* MPTPR[PTP] */
    lis     r6, HIADJ (M8260_MPTPR (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_MPTPR (INTERNAL_MEM_MAP_ADDR))
	sth     r5, 0x0 (r6)      /* store upper half-word */

	/* program the PSRT */

	addi    r5,0,0x13
    lis     r6, HIADJ (M8260_PSRT (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_PSRT (INTERNAL_MEM_MAP_ADDR))
	stb     r5, 0x0 (r6)      /* store byte - bits[24-31] */
#endif

#ifdef USE_BUS_100M
	/* (PSRT + 1)*(PTP + 1)/100M < 15.625u */
	/* PSRT = 0x2F, PTP = 0x1F, 48*32/100M = 15.36u */

	addi    r5,0,0x1F00	 /* MPTPR[PTP] */
    lis     r6, HIADJ (M8260_MPTPR (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_MPTPR (INTERNAL_MEM_MAP_ADDR))
	sth     r5, 0x0 (r6)      /* store upper half-word */

	/* program the PSRT */

	addi    r5,0,0x2F
    lis     r6, HIADJ (M8260_PSRT (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_PSRT (INTERNAL_MEM_MAP_ADDR))
	stb     r5, 0x0 (r6)      /* store byte - bits[24-31] */
#endif

#ifdef USE_BUS_66M
	/* (PSRT + 1)*(PTP + 1)/100M < 15.625u */
	/* PSRT = 0x1F, PTP = 0x1F, 32*32/66M = 15.36u */

	addi    r5,0,0x1F00	 /* MPTPR[PTP] */
    lis     r6, HIADJ (M8260_MPTPR (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_MPTPR (INTERNAL_MEM_MAP_ADDR))
	sth     r5, 0x0 (r6)      /* store upper half-word */

	/* program the PSRT */

	addi    r5,0,0x1F
    lis     r6, HIADJ (M8260_PSRT (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_PSRT (INTERNAL_MEM_MAP_ADDR))
	stb     r5, 0x0 (r6)      /* store byte - bits[24-31] */
#endif

#ifdef ADSL24V
	/* load OR2 */

    lis     r5, HIADJ (0xff000000 | \
                       M8260_OR_SDRAM_BPD_4 | \
	                   M8260_OR_SDRAM_ROWST_10| \
	                   M8260_OR_SDRAM_NUMR_11 )
    addi    r5, r5, LO (0xff000000 | \
                       M8260_OR_SDRAM_BPD_4 | \
	                   M8260_OR_SDRAM_ROWST_10| \
	                   M8260_OR_SDRAM_NUMR_11 )  
    lis     r6, HIADJ (M8260_OR2 (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_OR2 (INTERNAL_MEM_MAP_ADDR))
	stw	    r5, 0 (r6)

	/* load BR2 */

	lis	    r5, HIADJ (M8260_BR_MS_SDRAM_60X | M8260_BR_V) 
	addi	r5, r5, LO (M8260_BR_MS_SDRAM_60X | M8260_BR_V)
    lis     r6, HIADJ (M8260_BR2 (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_BR2 (INTERNAL_MEM_MAP_ADDR))
	stw	    r5, 0 (r6)

	/* The following describes the bit settings of the PSDMR register */
	/* On board SDRAM supporting page base interleaving.
	 * Refresh services are off for now
	 * SDAM  = b010
	 * BSMA  = b011 
	 * SDA10 = b010
	 * 8-clock refresh recovery time
	 * precharge-to-activate interval is 3-clock time
	 * activate-to-read/write interval is 3-clock time
	 * Burst lenght is 4
	 * last data out to precharge is 2 clock
	 * write recovery time is 2 clock
	 * no external address multiplexing
	 * normal timing for the control lines
	 * CAS latency is 3
	 */

	addis   r5,0,0x826b  
	ori     r5,r5,0x3ca3 
    lis     r6, HIADJ (M8260_PSDMR (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_PSDMR (INTERNAL_MEM_MAP_ADDR))
	stw	    r5, 0 (r6)

	/* OP selects the Precharge all banks command */

	addis   r5,0,0xaa6b
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
	addis   r0,0,0
 
	/* do a single write to an arbitrary location */

	addi    r5,0,0x00FF      /* Load 0x000000FF into r5 */
	stb     r5,0(r0)         /* Write 0xFF to address 0 - bits [24-31] */

	/* issue a "CBR Refresh" command to SDRAM */
	
	addis   r5,0,0x8a6b
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
 
	addi	r7,0,0x0008
	mtspr	9,r7             /* Load spr CTR with 8 */
	addi 	r8,0,0x00FF      /* Load 0x000000FF into r8 */

SdramWrLoop:
	/* Loop 8 times, writing 0xFF to address 0 */

	stb  	r8,0(r0)        	/* Write 0xFF to address 0 */
	bc   	16,0,SdramWrLoop	/* Decrement CTR, and possibly branch */
 
	/* issue a "Mode Register Write" command to SDRAM */

	addis   r5,0,0x9a6b
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
 
	/* do a single write to an arbitrary location */

	addi    r8,0,0x00FF      /* Load 0x000000FF into r8 */
	stb     r8,0(r0)         /* Write 0xFF to address 0 - bits [24-31] */

	/* enable refresh services and put SDRAM into normal operation */
	
	addis   r5,0,0xc26b
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
#endif

#ifdef SCM
	/* load OR2 */

    /* 128MB                                   */
	/* 4 internal bank                         */
	/* ROWST 0101, row start at A5 in PBI mode */
	/* 13 row                                  */
#if 0
    lis     r5, HIADJ (0xf8000a00 | \				   
                       M8260_OR_SDRAM_BPD_4 | \		   
	                   M8260_OR_SDRAM_NUMR_13 )		   
    addi    r5, r5, LO (0xf8000a00 | \				   
                       M8260_OR_SDRAM_BPD_4 | \		   
	                   M8260_OR_SDRAM_NUMR_13 )  	   
#endif
    lis     r5, HIADJ (0xf8002b00)		   
    addi    r5, r5, LO (0xf8002b00)  	   

    lis     r6, HIADJ (M8260_OR2 (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_OR2 (INTERNAL_MEM_MAP_ADDR))
	stw	    r5, 0 (r6)

	/* load BR2 */

	lis	    r5, HIADJ (M8260_BR_MS_SDRAM_60X | M8260_BR_V) 
	addi	r5, r5, LO (M8260_BR_MS_SDRAM_60X | M8260_BR_V)
    lis     r6, HIADJ (M8260_BR2 (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_BR2 (INTERNAL_MEM_MAP_ADDR))
	stw	    r5, 0 (r6)

	/* The following describes the bit settings of the PSDMR register */
	/* On board SDRAM supporting page base interleaving.
	 * Refresh services are off for now
	 * SDAM  = b011
	 * BSMA  = b001 
	 * SDA10 = b011
	 * 8-clock refresh recovery time
	 * precharge-to-activate interval is 3-clock time
	 * activate-to-read/write interval is 3-clock time
	 * Burst lenght is 4
	 * last data out to precharge is 2 clock
	 * write recovery time is 2 clock
	 * no external address multiplexing
	 * normal timing for the control lines
	 * CAS latency is 3
	 */

	/* OP: 000, selects the Normal Operation */

	addis   r5,0,0x832f  
	ori     r5,r5,0x3ca3 
    lis     r6, HIADJ (M8260_PSDMR (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_PSDMR (INTERNAL_MEM_MAP_ADDR))
	stw	    r5, 0 (r6)

	/* OP: 101, selects the Precharge all banks command */

	addis   r5,0,0xab2f
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
	addis   r0,0,0
 
	/* do a single write to an arbitrary location */

	addi    r5,0,0x00FF      /* Load 0x000000FF into r5 */
	stb     r5,0(r0)         /* Write 0xFF to address 0 - bits [24-31] */

	/* OP: 001, issue one "CBR Refresh" command to SDRAM */
	addis   r5,0,0x8b2f
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)

	addi	r7,0,0x0008
	mtspr	9,r7             /* Load spr CTR with 8 */
	addi 	r8,0,0x00FF      /* Load 0x000000FF into r8 */

SdramWrLoop:
	/* Loop 8 times, writing 0xFF to address 0 */

	stb  	r8,0(r0)        	/* Write 0xFF to address 0 */
	bc   	16,0,SdramWrLoop	/* Decrement CTR, and possibly branch */

	/* OP: 011, issue a "Mode Register Write" command to SDRAM */

	addis   r5,0,0x9b2f
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
 
	/* do a single write to an arbitrary location */

	addi    r8,0,0x00FF      /* Load 0x000000FF into r8 */
	stb     r8,0(r0)         /* Write 0xFF to address 0 - bits [24-31] */

	/* RFEN: 1, OP: 000, enable refresh services and put SDRAM into normal operation */
	
	addis   r5,0,0xc32f
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
#endif

#ifdef TSS
	/* load OR2 */

    /* 128MB                                    */
	/* 4 internal bank                         */
	/* ROWST 0101, row start at A5 in PBI mode */
	/* 13 row                                  */
#if 0
    lis     r5, HIADJ (0xf8000a00 | \				   
                       M8260_OR_SDRAM_BPD_4 | \		   
	                   M8260_OR_SDRAM_NUMR_13 )		   
    addi    r5, r5, LO (0xf8000a00 | \				   
                       M8260_OR_SDRAM_BPD_4 | \		   
	                   M8260_OR_SDRAM_NUMR_13 )
#endif
    lis     r5, HIADJ (0xf8002b00)		   
    addi    r5, r5, LO (0xf8002b00)  	   

    lis     r6, HIADJ (M8260_OR2 (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_OR2 (INTERNAL_MEM_MAP_ADDR))
	stw	    r5, 0 (r6)

	/* load BR2 */

	lis	    r5, HIADJ (M8260_BR_MS_SDRAM_60X | M8260_BR_V) 
	addi	r5, r5, LO (M8260_BR_MS_SDRAM_60X | M8260_BR_V)
    lis     r6, HIADJ (M8260_BR2 (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_BR2 (INTERNAL_MEM_MAP_ADDR))
	stw	    r5, 0 (r6)

	/* The following describes the bit settings of the PSDMR register */
	/* On board SDRAM supporting page base interleaving.
	 * Refresh services are off for now
	 * SDAM  = b011
	 * BSMA  = b001 
	 * SDA10 = b011
	 * 8-clock refresh recovery time
	 * precharge-to-activate interval is 3-clock time
	 * activate-to-read/write interval is 3-clock time
	 * Burst lenght is 4
	 * last data out to precharge is 2 clock
	 * write recovery time is 2 clock
	 * no external address multiplexing
	 * normal timing for the control lines
	 * CAS latency is 3
	 */

	/* OP: 000, selects the Normal Operation */

	addis   r5,0,0x832f  
	ori     r5,r5,0x3ca3 
    lis     r6, HIADJ (M8260_PSDMR (INTERNAL_MEM_MAP_ADDR))
    addi    r6, r6, LO (M8260_PSDMR (INTERNAL_MEM_MAP_ADDR))
	stw	    r5, 0 (r6)

	/* OP: 101, selects the Precharge all banks command */

	addis   r5,0,0xab2f
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
	addis   r0,0,0
 
	/* do a single write to an arbitrary location */

	addi    r5,0,0x00FF      /* Load 0x000000FF into r5 */
	stb     r5,0(r0)         /* Write 0xFF to address 0 - bits [24-31] */

	/* OP: 001, issue one "CBR Refresh" command to SDRAM */
	addis   r5,0,0x8b2f
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)

	addi	r7,0,0x0008
	mtspr	9,r7             /* Load spr CTR with 8 */
	addi 	r8,0,0x00FF      /* Load 0x000000FF into r8 */

SdramWrLoop:
	/* Loop 8 times, writing 0xFF to address 0 */

	stb  	r8,0(r0)        	/* Write 0xFF to address 0 */
	bc   	16,0,SdramWrLoop	/* Decrement CTR, and possibly branch */

	/* OP: 011, issue a "Mode Register Write" command to SDRAM */

	addis   r5,0,0x9b2f
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
 
	/* do a single write to an arbitrary location */

	addi    r8,0,0x00FF      /* Load 0x000000FF into r8 */
	stb     r8,0(r0)         /* Write 0xFF to address 0 - bits [24-31] */

	/* RFEN: 1, OP: 000, enable refresh services and put SDRAM into normal operation */
	
	addis   r5,0,0xc32f
	ori     r5,r5,0x3ca3
	stw	r5, 0 (r6)
#endif

#if 0 /* MARVIN_DEBUG */
	/* Direction C14, C15 out */
	lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

    /* Data,  */
    lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)
#endif

#if 0
debugGreen1:

	/* Direction C14, C15 out */
	lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

    /* Data,  */
    lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

	bl      debugGreen1
#endif

#if 0
#if DEBUG
	li		r3, 1
	bl		flashGreenLed
#endif
#endif

#if 0
debugGreen1:

	/* Direction C14, C15 out */
	lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

    /* Data,  */
    lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

	/* do a single write to an arbitrary location */
	addi	r0, 0, 0x1000
	mtspr	9, r0			/* Load CTR with 4K */

writeSdramNTimes:
	addi    r8,0,0x00FF      /* Load 0x000000FF into r8 */
	stb     r8,0(r0)         /* Write 0xFF to address 0 - bits [24-31] */
	bc		16, 0, writeSdramNTimes	/* */

    /* Data,  */
    lis 	r5, HIADJ (0x00000000)
	addi	r5, r5, LO (0x00000000)

	lis 	r6, HIADJ (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

	/* do a single write to an arbitrary location */
	addi	r0, 0, 0x1000
	mtspr	9, r0			/* Load CTR with 4K */

writeSdramNTimes1:
	addi    r8,0,0x00FF      /* Load 0x000000FF into r8 */
	stb     r8,0(r0)         /* Write 0xFF to address 0 - bits [24-31] */
	bc		16, 0, writeSdramNTimes1	/* */

	bl      debugGreen1
#endif



	/* invalidate TLBs: loop on all TLB entries using r7 as an index */

	addi	r0, 0, 0x0020
	mtspr	9, r0			/* Load CTR with 32 */
	addi	r7, 0, 0		/* Use r7 as the tlb index */

tlb_write_loop:

	tlbie	r7					/* invalidate the tlb entry */
	sync
	addi   	r7, r7, 0x1000			/* increment the index */
	bc		16, 0, tlb_write_loop	/* Decrement CTR, then branch if the */
						/* decremented CTR is not equal to 0 */

		/* Turn off data and instruction cache control bits */

	mfspr   r7, HID0
	isync
	sync				/* synchronize */
	andi.   r7, r7, 0x3FFF		/* Clear DCE and ICE bits */
	mtspr   HID0, r7
	isync
	sync				/* synchronize */

#if 0 /* TAIFAN_RMV_FIRST */
#ifdef INCLUDE_PCI
		/* Set the pciAutoconfig check to FALSE */
	xor		r5, r5, r5				/* Zero r5 */
	lis		r6, HIADJ (PCI_AUTO_CONFIG_ADRS)
	addi		r6, r6, LO (PCI_AUTO_CONFIG_ADRS)
	stw		r5, 0(r6)
	stw		r5, 0(r6)
#endif /* INCLUDE_PCI */

#ifdef INCLUDE_SECURITY_ENGINE

        lis     r5, HIADJ(SEC_ENG_SIZE_MASK) 
        addi    r5, r5, LO(SEC_ENG_SIZE_MASK)
	lis 	r6, HIADJ (M82XX_SECMR (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M82XX_SECMR (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

        lis     r5, HIADJ(SEC_ENG_BASE_ADRS | 0x1) 
        addi    r5, r5, LO(SEC_ENG_BASE_ADRS | 0x1) 
	lis 	r6, HIADJ (M82XX_SECBR (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M82XX_SECBR (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

#endif /* INCLUDE_SECURITY_ENGINE */
#endif /* TAIFAN_RMV_FIRST */
	/* Get the board revision number but do nothing for now */

    /* Turn on data cache control bits */

	mfspr   r7, HID0
	isync
	sync				/* synchronize */
	andi.   r7, r7, 0xBFFF		/* enable ICE bits */
	mtspr   HID0, r7
	isync
	sync				/* synchronize */

	/* initialize the stack pointer */

	lis 	sp, HIADJ (STACK_ADRS)
	addi	sp, sp, LO (STACK_ADRS)

	/* go to C entry point */

	addi	sp, sp, -FRAMEBASESZ		/* get frame stack */

	/*
	 * restore startType from r31
	 * calculate C entry point: routine - entry point + ROM base
	 * routine	= romStart
	 * entry point	= romInit	= R7
	 * ROM base	= ROM_TEXT_ADRS = R8
	 * C entry point: romStart - R7 + R8
	 */

	mr		r3,r31		/* load startType to parameter for 'C' func */

	lis		r7, HIADJ (romInit)
	addi	r7, r7, LO (romInit)

	lis		r8, HIADJ (ROM_TEXT_ADRS)
	addi	r8, r8, LO (ROM_TEXT_ADRS)

	lis 	r6, HIADJ (romStart)
	addi	r6, r6, LO (romStart)	/* load R6 with C entry point */

	sub 	r6, r6, r7		/* routine - entry point */
	add 	r6, r6, r8		/* + ROM base */

	mtspr   LR, r6			/* save destination address*/
							/* into LR register */
#if 1  /* MARVIN_DEBUG */
	/* Direction C14, C15 out */
	lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

    /* Data,  */
    lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)
#endif
	blr						/* jump to the C entry point */

#if 0 /* TAIFAN_DEBUG */
debugGreen1:

	/* Direction C14, C15 out */
	lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDIRC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

    /* Data,  */
    lis 	r5, HIADJ (0x00030000)
	addi	r5, r5, LO (0x00030000)

	lis 	r6, HIADJ (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	addi	r6, r6, LO (M8260_PDATC (INTERNAL_MEM_MAP_ADDR))
	stw 	r5, 0(r6)

	bl      debugGreen1
#endif
