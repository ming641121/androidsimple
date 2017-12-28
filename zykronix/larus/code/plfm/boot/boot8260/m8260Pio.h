/* m8260Pio.h - Motorola MPC8260 PIO header file */

/* Copyright 2005 Ahoya Networks Inc. */

#ifndef _M8260_PIO_H_
#define _M8260_PIO_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef  _ASMLANGUAGE
#define CAST(x)
#else /* _ASMLANGUAGE */
typedef volatile UCHAR VCHAR;   /* shorthand for volatile UCHAR */
typedef volatile INT32 VINT32; /* volatile unsigned word */
typedef volatile INT16 VINT16; /* volatile unsigned halfword */
typedef volatile INT8 VINT8;   /* volatile unsigned byte */
typedef volatile UINT32 VUINT32; /* volatile unsigned word */
typedef volatile UINT16 VUINT16; /* volatile unsigned halfword */
typedef volatile UINT8 VUINT8;   /* volatile unsigned byte */
#define CAST(x) (x)
#endif  /* _ASMLANGUAGE */


/*
 * MPC8260 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR
 * register. Also note that in the MPC8260, the IMMR is not a special
 * purpose register, but it is memory mapped.
 */

/* DIRECTION */

#define M8260_PDIRA(base)       (CAST(VUINT32 *)((base) + 0x10D00))

#define M8260_PDIRB(base)       (CAST(VUINT32 *)((base) + 0x10D20))

#define M8260_PDIRC(base)       (CAST(VUINT32 *)((base) + 0x10D40))

#define M8260_PDIRD(base)       (CAST(VUINT32 *)((base) + 0x10D60))
  
/* DATA */

#define M8260_PDATA(base)       (CAST(VUINT32 *)((base) + 0x10D10))

#define M8260_PDATB(base)       (CAST(VUINT32 *)((base) + 0x10D30))

#define M8260_PDATC(base)       (CAST(VUINT32 *)((base) + 0x10D50))

#define M8260_PDATD(base)       (CAST(VUINT32 *)((base) + 0x10D70))
 
#ifdef __cplusplus
}
#endif

#endif /* _M8260_PIO_H_ */
