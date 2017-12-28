#ifdef SCM
#define _ASMLANGUAGE

#include "vxWorks.h"
#include "asm.h"

#include "cacheLib.h"
#include "arch/ppc/cachePpcLib.h"
#include "config.h"
#include "regs.h"	
#include "sysLib.h"

#define FLASH_ADDR 0xB0000000
FUNC_EXPORT(reset_flash)
FUNC_EXPORT(flashid_command0)
FUNC_EXPORT(flashid_command1)
FUNC_EXPORT(flashid_command2)
FUNC_EXPORT(get_flashid)

FUNC_BEGIN(reset_flash)
   lwz	 r5, 0(r3)	
   lwz	 r6, 4(r3)
   lfdu  f0, 0(r3)  /* 0x00f000f000f000f0 */
    
   lis   r0, HIADJ(FLASH_ADDR)
   addi  r0, r0, LO(FLASH_ADDR)
   stfd  f0, 0(r0)   /* reset FLASH */
   stw   r5, 0(r3)
   stw   r6, 4(r3)
   
   blr			/* Return to caller */
FUNC_END(reset_flash)

FUNC_BEGIN(flashid_command0)
   lwz	 r5, 0(r3)	
   lwz	 r6, 4(r3)

   lfdu  f0, 0(r3) /* 0x00aa00aa00aa00aa */

   lis   r0, HIADJ(FLASH_ADDR)
   addi  r0, r0, LO(FLASH_ADDR)
   ori   r0, r0, 0x2aa8
   /* stfd  f0, 0(r0) */ /* write command */
/*
   stw   r5, 0(r3)
   stw   r6, 4(r3)
*/
   blr			/* Return to caller */
FUNC_END(flashid_command0)

FUNC_BEGIN(flashid_command1)
   lwz	 r5, 0(r3)	
   lwz	 r6, 4(r3)
   lfdu  f0, 0(r3)  /* 0x0055005500550055 */

   lis   r0, HIADJ(FLASH_ADDR)
   addi  r0, r0, LO(FLASH_ADDR)
   ori   r0, r0, 0x1550
   stfd  f0, 0(r0)  /* write command */

   stw   r5, 0(r3)
   stw   r6, 4(r3)

   blr			/* Return to caller */
FUNC_END(flashid_command1)

FUNC_BEGIN(flashid_command2)
   lwz	 r5, 0(r3)	
   lwz	 r6, 4(r3)
   lfdu  f0, 0(r3)  /* 0x0090009000900090 */

   lis   r0, HIADJ(FLASH_ADDR)
   addi  r0, r0, LO(FLASH_ADDR)
   ori   r0, r0, 0x2aa8
   stfd  f0, 0(r0)  /* write command */

   lis   r0, HIADJ(FLASH_ADDR)
   addi  r0, r0, LO(FLASH_ADDR)

/*   lfdu  f0, 0(r0)  
   stfd  f0, 0(r3) 
*/
   blr			/* Return to caller */
FUNC_END(flashid_command2)

FUNC_BEGIN(get_flashid)
   lwz	 r5, 0(r3)	
   lwz	 r6, 4(r3)
/*
   lfdu  f0, 0(r3)
   stfd  f0, 0(r3) 
*/
   stw   r5, 0(r3)
   stw   r6, 4(r3)

   blr
FUNC_END(get_flashid)

#endif