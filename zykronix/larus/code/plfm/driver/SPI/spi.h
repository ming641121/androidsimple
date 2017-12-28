/*
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: spi.h - the header of driver for MPC8248 SPI connect to MX25L2005     
 *  
 *
 *
 * Note: EEPROM is indeed flash memory. which only allow bite wise from 1 to 0
 *
 *Author(s):
 * marvin, betta and shuming
 *
 */
 


 /*====================================================================
 * $Log: spi.h,v $
 * Revision 1.6  2006/09/20 08:23:53  cvsshuming
 * have eeprom access much faster and also fix some error and warning
 *
 * Revision 1.5  2006/07/19 16:48:25  cvsshuming
 * use U32_t instead of int for portability
 *
 * Revision 1.4  2006/07/13 09:05:47  cvsshuming
 * fix write boundary error bug
 *
 * Revision 1.3  2006/07/11 10:30:33  cvsshuming
 * add log and remove typo
 *
 *
 *
 *====================================================================
 * $Endlog $
 */
 
#ifndef __SPI_h__
#define __SPI_h__

#define _PackedType
 
#define U8  unsigned char
#define U16 unsigned short
#define U32 unsigned long
#define U64 unsigned long long
#define BASE_ADDR   INTERNAL_MEM_MAP_ADDR 
#define BASE_SPI 0x89FC
#define SPI_BASE_ADDR (BASE_ADDR + BASE_SPI)
#define DPRAM_SPI_BD BASE_ADDR

#define BASE_SPI_RXBD 0x00
#define BASE_SPI_TXBD 0x20

#define SPI_RXBL_MAX	100	/* maximum RX buffer length */
#define SPI_TXBL_MAX	100	/* maximum TX buffer length */
#define SPI_DRV_NBDS	1
#define SPI_CLK_SPD     25000000
#define SPI_INT_CLR	    0xff	/* clear interrupt status */
#define SPI_INT_DSBL	0x0	    /* disable all interrupts */

#define SPI_IOCTL_W     0       /* write */
#define SPI_IOCTL_R     1       /* read */
#define SPI_IOCTL_O     2       /* read/or/write */
#define SPI_IOCTL_A     3       /* read/and/write */
#define SPI_IOCTL_AO    4       /* read/and/or/write */

/* SPI regiter set */
#define SPIMOD    (volatile U16 *)(INTERNAL_MEM_MAP_ADDR + 0x11AA0) /* SPI Mode Reg */
#define	SPIE     (volatile U8 *) (INTERNAL_MEM_MAP_ADDR + 0x11AA6) /* SPI Event Reg */   
#define	SPIM     (volatile U8 *) (INTERNAL_MEM_MAP_ADDR + 0x11AAA) /* SPI Mask Reg */ 	
#define	SPICOM    (volatile U8 *) (INTERNAL_MEM_MAP_ADDR + 0x11AAD) /* SPI Command Reg */	

/* SPI mode register bit definitions */
#define  CPM_SPI_MODE_LOOP    0x4000  /* loop mode */
#define  CPM_SPI_MODE_CI      0x2000  /* clock invert */ 
#define  CPM_SPI_MODE_CP      0x1000  /* clock phase */
#define  CPM_SPI_MODE_DIV16   0x0800  /* divide by 16 */
#define  CPM_SPI_MODE_REV1    0x0400  /* MSB mode */
#define  CPM_SPI_MODE_MS      0x0200  /* master */
#define  CPM_SPI_MODE_EN      0x0100  /* enable SPI */ 
#define  CPM_SPI_MODE_LEN8    0x0070  /* 8-bit length */
#define  CPM_SPI_MODE_PM0     0x0000  /* prescale divide ratio */       

/* SPI event/mask register bit definitions */
#define CPM_SPI_ER_MME        0x0020  /* transmit error */
#define CPM_SPI_ER_TXE        0x0010  /* transmit error */
#define CPM_SPI_ER_BSY        0x0004  /* busy condition */
#define CPM_SPI_ER_TXB        0x0002  /* transmit buffer */
#define CPM_SPI_ER_RXB        0x0001  /* receive buffer */

/* SPI command register bit definitions */
#define CPM_SPI_COMMAND_STR  0x0080  /* start transmit */

/* SPI receive buffer descriptor control/status */
#define CPM_SPI_R_CS_E     0x8000   /* empty */
#define CPM_SPI_R_CS_W     0x2000   /* wrap, final BD in table */
#define CPM_SPI_R_CS_I     0x1000   /* interrupt */
#define CPM_SPI_R_CS_L     0x0800   /* buffer closed on start/stop/error */
#define CPM_SPI_R_CS_CM    0x0200   /* continuous mode */
#define CPM_SPI_R_CS_OV    0x0002   /* overrun */
#define CPM_SPI_R_CS_ME    0x0001   /* mutiple-master error */

/* SPI transmit buffer descriptor control/status */
#define CPM_SPI_T_CS_R       0x8000  /* ready */
#define CPM_SPI_T_CS_W       0x2000  /* wrap, final BD in table */
#define CPM_SPI_T_CS_I       0x1000  /* interrupt */
#define CPM_SPI_T_CS_L       0x0800  /* last buffer of message */
#define CPM_SPI_T_CS_CM      0x0200  /* continuous mode */
#define CPM_SPI_T_CS_UN      0x0002  /* underrun */
#define CPM_SPI_T_CS_ME      0x0001  /* mutiple-master error */

/* SPI I/O control/status word definitions */
#define SPI_IO_S_ERROR	(1<<16)	/* error of some type, see remaining bits */
#define SPI_IO_S_R_OV	(1<<17)	/* receiver overrun */
#define SPI_IO_S_R_BSY	(1<<18)	/* receiver busy */
#define SPI_IO_S_T_ME	(1<<19)	/* mutiple-master error */
#define SPI_IO_S_T_UN	(1<<20)	/* transmitter underrun */
#define SPI_IO_S_T_TO	(1<<21)	/* transmitter timeout */
#define SPI_IO_S_R_TO	(1<<22)	/* receiver timeout */
#define SPI_IO_S_X_BUF	(1<<23)	/* transfer size exceeds internal buffer size */
#define SPI_IO_S_X_CNT	(1<<24)	/* transfer size does not equal the expected */

/* read/write data operation */
#define SPI_IO_C_RDATA	(1<<0)	
#define SPI_IO_C_WDATA	(1<<1)

/* SYNC, ISYNC macro */ 
#define IO_SYNC  WRS_ASM(" sync") /* Macro for all i/o operations to use */
#define IO_ISYNC  WRS_ASM(" isync") /* Macro for all i/o operations to use */

/* bits setting */
#define W31 1
#define W30 2
#define W29 4
#define W28 8

#define W27 0x10
#define W26 0x20
#define W25 0x40
#define W24 0x80

#define W23 0x100
#define W22 0x200
#define W21 0x400
#define W20 0x800

#define W19 0x1000
#define W18 0x2000
#define W17 0x4000
#define W16 0x8000

#define W15 0x10000
#define W14 0x20000
#define W13 0x40000
#define W12 0x80000

#define W11 0x100000
#define W10 0x200000
#define W9  0x400000
#define W8  0x800000

#define W7  0x1000000
#define W6  0x2000000
#define W5  0x4000000
#define W4  0x8000000

#define W3  0x10000000
#define W2  0x20000000
#define W1  0x40000000
#define W0  0x80000000

/* SPI mode register setting */
#define HW15 1
#define HW14 2
#define HW13 4
#define HW12 8

#define HW11 0x10
#define HW10 0x20
#define HW9  0x40
#define HW8  0x80

#define HW7  0x100
#define HW6  0x200
#define HW5  0x400
#define HW4  0x800

#define HW3  0x1000
#define HW2  0x2000
#define HW1  0x4000
#define HW0  0x8000

#define ERROR  (-1)
#define PD31    (0x00000001)
#define PD30    (0x00000002)
#define PD29    (0x00000004)
#define PD28    (0x00000008)
#define PD27    (0x00000010)
#define PD26    (0x00000020)
#define PD25    (0x00000040)
#define PD24    (0x00000080)
#define PD23    (0x00000100)
#define PD22    (0x00000200)
#define PD21    (0x00000400)
#define PD20    (0x00000800)
#define PD19    (0x00001000)
#define PD18    (0x00002000)
#define PD17    (0x00004000)
#define PD16    (0x00008000)
#define PD15    (0x00010000)
#define PD14    (0x00020000)
#define PD13    (0x00040000)
#define PD12    (0x00080000)
#define PD11    (0x00100000)
#define PD10    (0x00200000)
#define PD9     (0x00400000)
#define PD8     (0x00800000)
#define PD7     (0x01000000)
#define PD6     (0x02000000)
#define PD5     (0x04000000)
#define PD4     (0x08000000)

enum {		/* to define SPISectorErase mode */    
    SPIERASE_MODE_SECTOR=0,
    SPIERASE_MODE_ADDRESS
};

#define WRITE_PROGRAM_PAGE_SIZE 256
#define READ_PAGE_SIZE 4
#define WRITE_STATUS_TEST_COUNT	10
#define SPI_TRX_SIZE			260


/* buffer descriptor*/
typedef struct buffers_descriptor
{
	U16 status;
	U16 length;
	U8  *pointer;
}bd;

/* SPI I/O command packet */
typedef struct spi_cmd_pkt
{
    U16 csword;                /* status/control word */
    U16  dataAdrs;              /* data transfer address */
    U16 dataSize;              /* data transfer size */
    U16 dataSizeActual;        /* data transfer size (actual) */
} _PackedType SPI_CMD_PKT;

/* SPI_PARAM */
typedef  struct  
{
    volatile U16  rbase;          /* Rx BD Base Address */
    volatile U16  tbase;          /* Tx BD Base Address */
    volatile U8   rfcr;           /* Rx function code */
    volatile U8   tfcr;           /* Tx function code */
    volatile U16  mrblr;          /* Rx buffer length */
    volatile U32  rstate;         /* Rx internal state */
    volatile U32  rptr;           /* Rx internal data pointer */
    volatile U16  rbptr;          /* Rx BD Pointer */
    volatile U16  rcount;         /* Rx internal byte count */
    volatile U32  rtemp;          /* Rx temp */
    volatile U32  tstate;         /* Tx internal state */
    volatile U32  tptr;           /* Tx internal data pointer */
    volatile U16  tbptr;          /* Tx BD pointer */
    volatile U16  tcount;         /* Tx byte count */
    volatile U32  ttemp;          /* Tx temp */
    volatile U8   reserved[8];
} _PackedType SPI_PARAM;






extern STATUS_t EEPROM_write(U32 targetAddrInEEPROM,U8 *SourceAddress, U32_t len);
extern STATUS_t EEPROM_read(U8 *targetAddress,U32_t srcAddrInEEPROM,U32_t len);
extern STATUS_t EEPROM_ChipErase();
extern STATUS_t EEPROM_GetID();


#endif /* SPI */