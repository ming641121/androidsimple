/* $Id: csm_api.h,v 1.2 2006/06/15 03:54:40 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_api.h,v $
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
 * $Log: csm_api.h,v $
 * Revision 1.2  2006/06/15 03:54:40  ffang
 * Check-in after integrated with siw/sys modules
 *
 * Revision 1.1  2006/05/19 03:52:48  ffang
 * Initial check-in
 *
 *====================================================================
 * $Endlog $
 */

#ifndef _CSM_API_H_
#define _CSM_API_H_

/*
 *====================================================================
 *=                        define's                                 =
 *====================================================================
 */

/*
 *====================================================================
 *=                        typedef's                                 =
 *====================================================================
 */

/* The enum of CSM clock input type */
typedef enum 
{
	CSM_INPUT_ERS1 = 0,
	CSM_INPUT_ERS2,
	CSM_INPUT_PRS,
	CSM_INPUT_BITS_1,
	CSM_INPUT_BITS_2,
    
    CSM_INPUT_CLK_MAX,
} CSM_INPUT_CLK_TYPE_e;

/* The enum of CSM stratum level for the associated input clock */
typedef enum 
{
    CSM_STRATUM_1 = 0,
    CSM_STRATUM_2,
    CSM_STRATUM_3,
    CSM_STRATUM_UNKNOWN,

    CSM_STRATUM_MAX,
} CSM_STRATUM_LEVEL_e;

/* The struct of CSM input clock */
typedef struct 
{
	CSM_INPUT_CLK_TYPE_e  clk;
	CSM_STRATUM_LEVEL_e   stratum;
} CSM_INPUT_CLK_s;

/*
 *====================================================================
 *=                        Macro's                                   =
 *====================================================================
 */

/*
 *====================================================================
 *=                        Extern's                                  =
 *====================================================================
 */

extern CSM_INPUT_CLK_s csmInpClk[CSM_INPUT_CLK_MAX];

extern U8_t 				CsmStratumLevelGet(void);
extern CSM_INPUT_CLK_TYPE_e CsmClockSourceGet(void);

#endif /* _CSM_API_H_ */
