/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file contains the QMC BUFFER prototype
 *
 * Author(s):
 *
 */

#ifndef _SYS_MEMMAP_TSS_H_
#define _SYS_MEMMAP_TSS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *====================================================================
 *=                              define                              =
 *====================================================================
 */

#define FLASH_ADRS_8M		                          0xff800000	/* base address of 8Meg flash */
#define FLASH_ADRS_16M		                          0xff000000	/* base address of 16Meg flash */
#define FLASH_ADRS_32M		                          0xfe000000	/* base address of 32Meg flash */

#define BIT(x)                                        (1 << (x))

/* CS0: BOOT FLASH,   Size 512K, 8 bit port */
#define CS0_START									  0xFFF00000
#define CS0_SIZE 									  0x00080000
#define CS0_END										  (CS0_START + (CS0_SIZE - 1))


#define BOOT_BASE_ADRS                                CS0_START
#define BOOT_SIZE	       	                          CS0_SIZE
#define BOOT_BASE_MASK                                (0xFFF80000)

#if 1  /* TAIFAN Ref */
#define FLASH_BASE_ADRS		                          FLASH_ADRS_8M  /* this board has 8Meg flash */
#define FLASH_ADRS                                    0xffec0000
#define FLASH_BLOCK_SIZE                              0x40000
#define FLASH_BOOTLINE_OFFSET                         0x0
#define FLASH_WIDTH   		                          4
#define FLASH_SIZE	       	                          0x800000 /* 8 Mbytes total flash size */
#define FLASH_SIZE_WRITEABLE	                      256		/* reasonable write size */
#define SYS_FLASH_TYPE	       	                      FLASH_28F016	/* Intel flash device type */
#endif

/* CS1: Disk FLASH,   Size 16MB, 16 bit port */
#define CS1_START                                     0xB0000000
#define CS1_SIZE                                      0x01000000
#define CS1_END                                       (CS1_START + (CS1_SIZE - 1))

#define SYS_FLASH_BASE_ADRS                           CS1_START
#define SYS_FLASH_SIZE                                CS1_SIZE
#define SYS_BLOCK_SIZE                                0x20000	/* 128K Byte */
#define SYS_FLASH_BOOTLINE_OFFSET                     0x0
#define SYS_FLASH_WIDTH   		                      4
#define SYS_FLASH_SIZE_WRITEABLE	                  256		/* reasonable write size */
#define SYS_FLASH_TYPE	       	                      FLASH_28F016	/* Intel flash device type */
#define SYS_FLASH_SIZE_MASK                           0xFF000000


/* CS2: System SDRAM, Size 64MB, 64 bit port */
#define CS2_START                                     0x00000000
#define CS2_SIZE                                      0x04000000
#define CS2_END                                       (CS2_START + (CS2_SIZE - 1))

/* CS3: FPGA,         Size 4K,  8 bit port */
#define CS3_START                                     (0x40000000)
#define CS3_SIZE                                      (0x00001000)
#define CS3_END                                       (CS3_START + (CS3_SIZE - 1))
#define CS3_MASK                                      (0xFFFF8000)


/* CS4: BP,           Size 4K,  8 bit port */
#define CS4_START                                     (0x41000000)
#define CS4_SIZE                                      (0x00010000)
#define CS4_END                                       (CS4_START + (CS4_SIZE - 1))
#define CS4_MASK                                      (0xFFFF0000)


/* CS5: BITS,         Size 4K,  8 bit port */
#define CS5_START                                     (0x42000000)
#define CS5_SIZE                                      (0x00001000)
#define CS5_END                                       (CS5_START + (CS5_SIZE - 1))
#define CS5_MASK                                      (0xFFFF8000)

#define BITS_0_ADDR_BASE                              (CS5_START)
#define BITS_1_ADDR_BASE                              (CS5_START + 0x100)
#define BITS_2_ADDR_BASE                              (CS5_START + 0x200)
#define BITS_3_ADDR_BASE                              (CS5_START + 0x300)
#define BITS_4_ADDR_BASE                              (CS5_START + 0x400)

/* #define RTC_ADDR_BASE                                 (CS5_START + 0xF00) */


/* CS7: SMU,          Size 4K,  8 bit port */
#define CS7_START                                     (0x43000000)
#define CS7_SIZE                                      (0x00080000)
#define CS7_END                                       (CS7_START + (CS7_SIZE - 1))
#define CS7_MASK                                      (0xFFF80000)

/* temp added, need to confirm with Shinn */
#define  hwFatalEvent_TRAP				  1
#define  swFatalEvent_TRAP				  2
#define  hwMajorEvent_TRAP				  3
#define  swMajorEvent_TRAP				  4
#define  hwMinorEvent_TRAP				  5
#define  swMinorEvent_TRAP				  6

#define  CM_MSG_TRAP_EVENT                12

/* System Trace reserved memory areas */
#define SYS_EXC_RESERVED_MEM  2048 /* reserved memory to capture exception */

/* resv mem to capture cpu utilization - 77k */
#define SYS_PROFILER_RESERVED_MEM  78848 

/* resv mem for memory trace info - with cell number set to 128 this is roughly 13412 bytes */ 
#define SYS_MEM_TRACE_RESV_MEM ( (2 * sizeof (MEM_TRACE_BUF_t)) + (3 * sizeof (int)) )

/* resv mem for reset reason info - 256 bytes */ 
#define SYS_RESET_REASON_RESV_MEM 256

/* resv mem for future used */
#define SYS_TRACE_FUTURE_RSV_MEM ((( 121 * 1024 ) - SYS_MEM_TRACE_RESV_MEM) - SYS_RESET_REASON_RESV_MEM)

#define SYS_TRACE_RESV_MEM  (SYS_EXC_RESERVED_MEM + SYS_PROFILER_RESERVED_MEM + \
                             SYS_MEM_TRACE_RESV_MEM + SYS_RESET_REASON_RESV_MEM + \
                             SYS_TRACE_FUTURE_RSV_MEM)

#define SYS_MEM_TRACE_OFFSET    (SYS_EXC_RESERVED_MEM + SYS_PROFILER_RESERVED_MEM)

#define SYS_RESET_REASON_OFFSET (SYS_EXC_RESERVED_MEM + SYS_PROFILER_RESERVED_MEM + \
                                 SYS_MEM_TRACE_RESV_MEM)

/* flag for SDRAM test in Diagnostic boot up define in sys_memmap.h */
#define BOOT_INTO_DIAG	         99    /* SCC=128MB BSC-NSC=64MB */ 
#define BOOT_INTO_DIAG_SHORT_MEM 100    /* SCC-BSC-NSC=64KB */

/* Dynamic memory size and number of butters */ 
#define DYN_BUF_1_SIZE        (64   + sizeof(SIW_MEM_HEADER_t))
#define DYN_BUF_2_SIZE        (128  + sizeof(SIW_MEM_HEADER_t))
#define DYN_BUF_3_SIZE        (256  + sizeof(SIW_MEM_HEADER_t))
#define DYN_BUF_4_SIZE        (1024 + sizeof(SIW_MEM_HEADER_t))
#define DYN_BUF_5_SIZE        (2048 + sizeof(SIW_MEM_HEADER_t))
#define DYN_BUF_6_SIZE        (4096 + sizeof(SIW_MEM_HEADER_t))
#define DYN_BUF_7_SIZE        (2048 + sizeof(SIW_MEM_HEADER_t))

#define DYN_BUF_1_NUM         460	/* 29K */
#define DYN_BUF_2_NUM         120	/* 15K */
#define DYN_BUF_3_NUM         470	/* 120K */
#define DYN_BUF_4_NUM         620	/* 620K */
#define DYN_BUF_5_NUM         220	/* 440K */
#define DYN_BUF_6_NUM         20	/* 80K */
#define DYN_BUF_7_NUM         120	/* 240K */

#define SYS_MEM_TRACE_OFFSET    (SYS_EXC_RESERVED_MEM + SYS_PROFILER_RESERVED_MEM)
#define SYS_EXC_RESERVED_MEM  2048 /* reserved memory to capture exception */
#define SYS_PROFILER_RESERVED_MEM  78848 
#define DYN_RESERVED_MEM ((DYN_BUF_1_SIZE * (DYN_BUF_1_NUM + 1)) + \
   (DYN_BUF_2_SIZE * (DYN_BUF_2_NUM + 1)) + (DYN_BUF_3_SIZE * (DYN_BUF_3_NUM + 1)) + \
   (DYN_BUF_4_SIZE * (DYN_BUF_4_NUM + 1)) + (DYN_BUF_5_SIZE * (DYN_BUF_5_NUM + 1)) + \
   (DYN_BUF_6_SIZE * (DYN_BUF_6_NUM + 1)) + (DYN_BUF_7_SIZE * (DYN_BUF_7_NUM + 1)))
/* end - temp added, need to confirm with Shinn */

/*
 *====================================================================
 *=                              typedef                             =
 *====================================================================
 */


/*
 *====================================================================
 *=                              macro                               =
 *====================================================================
 */


/*
 *====================================================================
 *=                              routine                             =
 *====================================================================
 */


#ifdef __cplusplus
}
#endif

#endif /* _SYS_MEMMAP_TSS_H_ */
