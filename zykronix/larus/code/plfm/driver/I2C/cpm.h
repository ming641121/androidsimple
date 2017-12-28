/*  cpm.h - CPM header                  */
/*                                      */

/* CPM - Communication Processor Module */
/* Communication Processor register set */
#define	CPCR(base)	((VUINT32 *)((base) + 0x119C0)) /* Comm Proc Cmd*/

/* Command Register definitions (CPCR - 0x9C0) */    
#define CPM_CR_FLG          0x10000	    /* flag - command executing */ 
#define	CPM_CR_RESET		0x80000000	/* software reset command */
#define CPM_CR_OPCODE       0x000f	    /* command opcode */
#define	CPM_CR_CHANNEL_SPI          0X25400000   /* PAGE & SBC for SPI */	

/* CPCR - opcodes */
#define CPM_CR_OPCODE_INIT_RT   0x0		/* Init rx and tx */
#define CPM_CR_OPCODE_INIT_R    0x1		/* init rx only */
#define CPM_CR_OPCODE_INIT_T    0x2		/* init tx only */
#define CPM_CR_OPCODE_CLOSE     0x7		/* close rx buffer */    



