/* $Id: csm_hw.h,v 1.4 2006/08/03 04:27:28 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_hw.h,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of CSM Data Base
 *    Management for redundancy
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Log: csm_hw.h,v $
 * Revision 1.4  2006/08/03 04:27:28  ffang
 * unit test with FPGA
 *
 * Revision 1.3  2006/08/03 02:56:56  ffang
 * add unit test routines for dbm and events
 *
 * Revision 1.2  2006/07/14 04:28:16  ffang
 * sync recheck-in
 *
 * Revision 1.1  2006/05/16 04:09:49  ffang
 * *** empty log message ***
 *
 *====================================================================
 * $Endlog $
 */

#ifndef _CSM_HW_H_
#define _CSM_HW_H_

#include "sys_memmap_tss.h"

/*
 *====================================================================
 *=                          address map                             =
 *====================================================================
 */
#define  CSM_FPGA_BASE_ADDR			0x40000000	   
#define  CSM_FPGA_BASE_0			(U8_t)(*(VU8_t *)(CSM_FPGA_BASE_ADDR+0))
#define  CSM_FPGA_BASE_1			(U8_t)(*(VU8_t *)(CSM_FPGA_BASE_ADDR+1))
#define  CSM_FPGA_BASE_2			(U8_t)(*(VU8_t *)(CSM_FPGA_BASE_ADDR+2))
#define  CSM_FPGA_BASE_3			(U8_t)(*(VU8_t *)(CSM_FPGA_BASE_ADDR+3))
#define  CSM_FPGA_BASE_4			(U8_t)(*(VU8_t *)(CSM_FPGA_BASE_ADDR+4))
#define  CSM_FPGA_BASE_5			(U8_t)(*(VU8_t *)(CSM_FPGA_BASE_ADDR+5))
#define  CSM_FPGA_BASE_6			(U8_t)(*(VU8_t *)(CSM_FPGA_BASE_ADDR+6))
#define  CSM_FPGA_BASE_7			(U8_t)(*(VU8_t *)(CSM_FPGA_BASE_ADDR+7))
#define  CSM_FPGA_BASE_8			(U8_t)(*(VU8_t *)(CSM_FPGA_BASE_ADDR+8))


#define  FRAMER_BASE_ADDR			(VU32_t *) 0x42000016

#define CSM_BIT_0					0
#define CSM_BIT_1					1
#define CSM_BIT_2					2
#define CSM_BIT_3					3
#define CSM_BIT_4					4
#define CSM_BIT_5					5
#define CSM_BIT_6					6
#define CSM_BIT_7					7

#define CSM_BIT_SET(__val, __bit)	((__val) |= (1 << (__bit)))	
#define CSM_BIT_CLEAR(__val, __bit) ((__val) &= ~(1 << (__bit)))		

/*
 *====================================================================
 *=                        selection register                        =
 *====================================================================
 */


/*
 *====================================================================
 *=                        control register                          =
 *====================================================================
 */


/*
 *====================================================================
 *=                        status register                           =
 *====================================================================
 */


/*
 *====================================================================
 *=                       bit combinations                           =
 *====================================================================
 */


/*
 *====================================================================
 *=                        PIO stuff                                 =
 *====================================================================
 */

#endif /* _CSM_HW_H_ */
