/* $Id: csm_expcd.c,v 1.1 2006/06/16 03:29:36 ffang Exp $
 * $Source: /cvsroot/larus5820/code/plfm/csm/csm_expcd.c,v $
 *====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    This file contains the routines of Expansion Driver
 *
 * Note:
 *
 * Author(s):
 *
 *====================================================================
 * $Log: csm_expcd.c,v $
 * Revision 1.1  2006/06/16 03:29:36  ffang
 * Initial check-in
 *
 *====================================================================
 * $Endlog $
 */

#include "stdio.h"
#include <string.h>
#include "sys_os.h"
#include "csm_if.h"


/*========================================================================
 * Function:     ExpCdRead
 *
 * Description:  Routine to read expansion card
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
U8_t
ExpCdRead (VU32_t * addr)
{
	return 0;
}

/*========================================================================
 * Function:     ExpCdWrite
 *
 * Description:  Routine to write expansion card
 *
 * In Parms:     none
 * Out Parms:    none
 *
 * Returns:      OK if success, ERROR otherwise
 */
void
ExpCdWrite (VU32_t * addr, U8_t data)
{

}