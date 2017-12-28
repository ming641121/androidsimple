#define _ASMLANGUAGE

#include "vxWorks.h"
#include "asm.h"

#include "cacheLib.h"
#include "arch/ppc/cachePpcLib.h"
#include "config.h"
#include "regs.h"	
#include "sysLib.h"

FUNC_EXPORT(reset_flash)
FUNC_EXPORT(flashid_command)

pmanid: .double  0xe000000000000000
pdevid: .double  0

FUNC_BEGIN(reset_flash)
   lis    r4, HIADJ(FLASH_ADDR)
   addi   r4, r4, LO(FLASH_ADDR) 
   mflr	  r9 

   bl     rstData0
   .double 0x00e0000000000000
rstData0:
   mflr	  r5
   lfd    f0, 0(r5) 
   isync

   bl     rstData1
   .double 0x0010000000000000
rstData1:
   mflr	  r5
   lfd    f1, 0(r5) 
   isync

   bl     rstData2
   .double 0x000000e000000000
rstData2:
   mflr	  r5
   lfd    f2, 0(r5) 
   isync

   bl     rstData3
   .double 0x0000001000000000
rstData3:
   mflr	  r5
   lfd    f3, 0(r5) 
   isync

   bl     rstData4
   .double 0x0000000000e00000
rstData4:
   mflr	  r5
   lfd    f4, 0(r5) 
   isync

   bl     rstData5
   .double 0x0000000000100000
rstData5:
   mflr	  r5
   lfd    f5, 0(r5) 
   isync

   bl     rstData6
   .double 0x00000000000000e0
rstData6:
   mflr	  r5
   lfd    f6, 0(r5) 
   isync

   bl     rstData7
   .double 0x0000000000000010
rstData7:
   mflr	  r5
   lfd    f7, 0(r5) 
   isync
   
   fadd   f8, f0, f1
   fadd   f9, f8, f2
   fadd   f10, f9, f3
   fadd   f11, f10, f4
   fadd   f12, f11, f5
   fadd   f13, f12, f6
   fadd   f14, f13, f7

   stfd   f14, 0(r4)
   
   lis   r0, 0x0
   addis r4, r0, HI(pmanid)
   ori   r4, r4, LO(pmanid)
   lfd    f0,  0(r4)
   stfd   f0, 0(r3)
   mtlr	  r9  
   blr
FUNC_END(reset_flash)


FUNC_BEGIN(flashid_command)
   lis    r4, HIADJ(FLASH_ADDR)
   addi   r4, r4, LO(FLASH_ADDR) 
   ori    r6, r4, 0x2aa8
   mflr	  r9 

   bl     getid0
   .double 0x0099009900990099
getid0:
   mflr	  r5
   lfd    f0, 0(r5) 
   isync

   bl     getid1
   .double 0x0011001100110011
getid1:
   mflr	  r5
   lfd    f1, 0(r5) 
   isync
   fadd   f2, f0, f1
   stfd   f2, 0(r6) 

   ori    r7, r4, 0x1550
   bl     getid2
   .double 0x0055005500550055
getid2:
   mflr	  r5
   lfd    f3, 0(r5) 
   isync
   stfd   f3, 0(r7) 
   
   bl     getid3
   .double 0x0090009000900090
getid3:
   mflr	  r5
   lfd    f4, 0(r5) 
   isync
   stfd   f4, 0(r6) 
   
   lfd    f5, 0(r4)
   lis    r4, HIADJ(FLASH_ADDR+8)
   addi   r4, r4, LO(FLASH_ADDR+8) 
   lfd    f6, 0(r4)

   stfd   f5, 0(r3)
/* stfd   f6, 0(r3+64) */
   mtlr	  r9 
   blr
FUNC_END(flashid_command)

