//***********************************************
//
//  Copyright (c) 2005 Magic Pixel Inc.
//  All rights reserved
//
//***********************************************

#ifndef __M005DEF
#define __M005DEF

#define	OK		0x00
#define	Fail		0xFF


//***********************************************
//  Base addresses for standard memory-mapped peripherals
//***********************************************



//***********************************************
// Interrupt Controller IRQ register
//***********************************************
//#define INTCBase         0xA8030A00  // Interrupt Controller Base
#define INTCBase         0xA8240000  // Interrupt Controller Base
struct strc_intc
{
	volatile unsigned int	SRCPND;			// 0x00
	volatile unsigned int	reserved1;		// 0x04
	volatile unsigned int	INTMOD;		// 0x08
	volatile unsigned int	reserved2;		// 0x0c
	volatile unsigned int	INTMSK;			// 0x10
	volatile unsigned int	reserved3;		// 0x14
	volatile unsigned int	INTPND;			// 0x18
	volatile unsigned int	reserved4;		// 0x1c
	volatile unsigned int	INTPRT;			// 0x20
	volatile unsigned int	reserved5[4];	// 0x24~0x30
	volatile unsigned int	INTOFS;			// 0x34
	volatile unsigned int	EINTFIL;		// 0x38
};

#define INTC ((volatile struct strc_intc*) INTCBase)


#define	INTC_MP321_ICU	0x00000001	// arb0_req0
#define	INTC_G3D			0x00000002	// arb0_req1
#define	INTC_DMA2			0x00000004	// arb0_req2
#define	INTC_arb0_req3		0x00000008	// arb0_req3
#define	INTC_arb0_req4		0x00000010	// arb0_req4
#define	INTC_arb0_req5		0x00000020	// arb0_req5
#define	INTC_UGP0			0x00000040	// arb0_req6
#define	INTC_UGP1			0x00000080	// arb0_req7

#define	INTC_UART0			0x00000100	// arb1_req0
#define	INTC_UART1			0x00000200	// arb1_req1
#define	INTC_UART2			0x00000400	// arb1_req2
#define	INTC_UART3			0x00000800	// arb1_req3
#define	INTC_UART4			0x00001000	// arb1_req4
#define	INTC_SPI0			0x00002000	// arb1_req5
#define	INTC_SPI1			0x00004000	// arb1_req6
#define	INTC_USB			0x00008000	// arb1_req7

#define	INTC_GPIO0			0x00010000	// arb2_req0
#define	INTC_GPIO1			0x00020000	// arb2_req1
#define	INTC_GPIO2			0x00040000	// arb2_req2
#define	INTC_GPIO3			0x00080000	// arb2_req3
#define	INTC_GPIO4			0x00100000	// arb2_req4
#define	INTC_GPIO5			0x00200000	// arb2_req5
#define	INTC_GPIO6			0x00400000	// arb2_req6
#define	INTC_GPIO7			0x00800000	// arb2_req7

#define	INTC_arb3_req0		0x01000000	// arb3_req0
#define	INTC_arb3_req1		0x02000000	// arb3_req1
#define	INTC_HGPIO			0x04000000	// arb3_req2
#define	INTC_RTC_Alarm		0x08000000	// arb3_req3
#define	INTC_RTC_Tick		0x10000000	// arb3_req4
#define	INTC_UGP2			0x20000000	// arb3_req5
#define	INTC_UGP3			0x40000000	// arb3_req6
#define	INTC_UGP4			0x80000000	// arb3_req7


//***********************************************
// UART Structure maps to register offsets
//***********************************************
//#define Uart0Base		0xA8030800 // UART0 Controller Base
//#define Uart1Base		0xA8030830 // UART1 Controller Base
//#define Uart2Base		0xA8030860 // UART2 Controller Base
//#define Uart3Base		0xA8030890 // UART3 Controller Base
//#define Uart4Base		0xA80308C0 // UART4 Controller Base
#define Uart0Base		0xA8200000 // UART0 Controller Base
#define Uart1Base		0xA8210000 // UART1 Controller Base
#define Uart2Base		0xA8220000 // UART2 Controller Base
#define Uart3Base		0xA8230000 // UART3 Controller Base
//#define Uart4Base		0xA80308C0 // UART4 Controller Base
struct uart
{ volatile unsigned dr;   // @0x0
  volatile unsigned rsrecr;  // @0x4
  volatile unsigned lcrh; // @0x8
  volatile unsigned lcrm; // @0x0c
  volatile unsigned lcrl; // @0x10
  volatile unsigned cr;   // @0x14
  volatile unsigned fr;   // @0x18
  volatile unsigned iir;  // @0x1C
};

#define UART0 ((volatile struct uart *) Uart0Base)
#define UART1 ((volatile struct uart *) Uart1Base)
#define UART2 ((volatile struct uart *) Uart2Base)

//***********************************************
// Received Status Register - RSR
//***********************************************
#define RSRECR_Overrun_Error   0x08
#define RSRECR_Break_Error     0x04
#define RSRECR_Parity_Error    0x02
#define RSRECR_Framing_Error   0x01

//***********************************************
// Line Control High Byte Register - LCRH
//***********************************************
#define LCRH_Word_Length_8  0x60
#define LCRH_Word_Length_7  0x40
#define LCRH_Word_Length_6  0x20
#define LCRH_Word_Length_5  0x00
#define LCRH_Fifo_Enabled   0x10
#define LCRH_2_Stop_Bits    0x08
#define LCRH_Even_Parity    0x04
#define LCRH_Parity_Enable  0x02
#define LCRH_Send_Break     0x01

//***********************************************
// Line Control Medium Byte Register - LCRM
// This register specifies the high byte of the Baud rate divisor
//***********************************************
#define LCRM_Baud_460800  0x00
#define LCRM_Baud_230400  0x00
#define LCRM_Baud_115200  0x00
#define LCRM_Baud_115200_HAPS  0x00
#define LCRM_Baud_76800   0x00
#define LCRM_Baud_57600   0x00
#define LCRM_Baud_38400   0x00
#define LCRM_Baud_38400_HAPS   0x00
#define LCRM_Baud_19200   0x00
#define LCRM_Baud_14400   0x00
#define LCRM_Baud_9600    0x00
#define LCRM_Baud_2400    0x01
#define LCRM_Baud_1200    0x02

//***********************************************
// Line Control Low Byte Register - LCRL
// This register specifies the low byte of the Baud rate divisor
//***********************************************
#define LCRL_Baud_460800  0x01
#define LCRL_Baud_230400  0x03
#define LCRL_Baud_115200  0x07
#define LCRL_Baud_115200_HAPS  0x01
#define LCRL_Baud_76800   0x0B
#define LCRL_Baud_57600   0x0F
//#define LCRL_Baud_38400   0x17
#define LCRL_Baud_38400_HAPS   0x05	// for 12MHz OSC at HAPS
#define LCRL_Baud_19200   0x2F
#define LCRL_Baud_14400   0x3F
#define LCRL_Baud_9600    0x5F
#define LCRL_Baud_2400    0x7F
#define LCRL_Baud_1200    0xFF

//***********************************************
// Control Register - CR
//***********************************************
#define CR_Loop_Back_En   0x80
#define CR_Timeout_Int_En 0x40
#define CR_TX_Int_Enable  0x20
#define CR_RX_Int_Enable  0x10
#define CR_ModStat_Int_En 0x08
#define CR_UART_Enable    0x01

//***********************************************
// Flag Register - FR
//***********************************************
#define FR_TX_Fifo_Empty  0x80
#define FR_RX_Fifo_Full   0x40
#define FR_TX_Fifo_Full   0x20
#define FR_RX_Fifo_Empty  0x10
#define FR_Busy           0x08
#define FR_Carrier_Detect 0x04
#define FR_Set_Ready      0x02
#define FR_Clear_To_Send  0x01

//***********************************************
// Interrupt Identification Register - IIR
//***********************************************
#define IIR_RX_Time_Out   0x08
#define IIR_TX            0x04
#define IIR_RX            0x02
#define IIR_Modem         0x01



#endif

