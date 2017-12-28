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
FUNC_EXPORT(flashid_command)

pmanid: .double  0x0100000000000000
pdevid: .double  0
rstdat: .long    0x00f000f0
command0: .long  0x00aa00aa
command1: .long  0x00550055 
command2: .long  0x00900090

FUNC_BEGIN(reset_flash)
   /* save current value of MSR */
   mfmsr   r0

   /* Turn on FP */
   li      r1, _PPC_MSR_FP	
   sync
   mtmsr   r1		
   isync 
/*  
   lis     r7, HIADJ(FLASH_ADDR)
   addi    r7, r7, LO(FLASH_ADDR) 

   lis     r8, HIADJ(rstdat)
   addi    r8, r8, LO(rstdat)  
     
   lis     r9, HIADJ(rstdat)
   addi    r9, r9, LO(rstdat) 
   
   stw     r9, 4(r8)    
 
   li      r2, 0
   lfdux   f3, r8, r2
   stfd    f3, 0(r7)
   sync
*/
   /*  Restore MPU/MSR to a known state  */
   mtmsr   r0	/* restore saved value */
   mr      r3,  r8 
   isync
   blr			/* Return to caller */
FUNC_END(reset_flash)


FUNC_BEGIN(flashid_command)
   /* save current value of MSR */
   mfmsr   r0

   /* Turn on FP */
   li      r1, _PPC_MSR_FP		
   sync
   mtmsr   r1		
   isync 

   lis     r4,  HIADJ(FLASH_ADDR)
   addi    r4,  r4, LO(FLASH_ADDR) 
   ori     r6,  r4, 0x2aa8  /* 0x00000555<<3 */
   
   lis     r7,  HIADJ(command0)
   addi    r7,  r7, LO(command0) 
   lis     r8,  HIADJ(command0)
   addi    r8,  r8, LO(command0)
   stw     r8,  4(r7) 
   lis     r9,  HIADJ(command1)
   addi    r9,  r9, LO(command1) 
   lis     r10, HIADJ(command1)
   addi    r10, r10, LO(command1) 
   stw     r10, 4(r9) 
   lis     r11, HIADJ(command2)
   addi    r11, r11, LO(command2) 
   lis     r12, HIADJ(command2)
   addi    r12, r12, LO(command2) 
   stw     r12, 4(r11) 

   li      r13, 0
   lfdux   f3,  r7, r13
   stfd    f3,  0(r6)
   
   lis     r4,  HIADJ(FLASH_ADDR)
   addi    r4,  r4, LO(FLASH_ADDR) 
   ori     r2,  r4, 0x1550   /* 0x000002aa<<3 */

   lfsux   f3,  r9, r10
   stfd    f3,  0(r2)

   lfsux   f3,  r11, r12
   stfd    f3,  0(r6)

   lis     r4,  HIADJ(FLASH_ADDR)
   addi    r4,  r4, LO(FLASH_ADDR)
   stfsux  

   /*  Restore MPU/MSR to a known state  */
   mtmsr   r0	/* restore saved value */
   mr      r3,  r8  
   isync
   blr			/* Return to caller */
FUNC_END(flashid_command)

