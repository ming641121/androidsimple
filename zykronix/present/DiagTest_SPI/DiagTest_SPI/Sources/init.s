;;; Copyright ARM Ltd 2001. All rights reserved.
;
; This module performs ROM/RAM remapping (if required), initializes stack 
; pointers and interrupts for each mode, and finally branches to __main in 
; the C library (which eventually calls main()).
;
; On reset, the ARM core starts up in Supervisor (SVC) mode, in ARM state, 
; with IRQ and FIQ disabled.

        PRESERVE8

        AREA    Init, CODE, READONLY



; --- ensure no functions that use semihosting SWIs are linked in from the C library

                IMPORT __use_no_semihosting_swi
        
; --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs

Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F ; available on ARM Arch 4 and later

I_Bit           EQU     0x80 ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40 ; when F bit is set, FIQ is disabled


; --- System memory locations

;UART0_Base			EQU	0xA8030800
;ICU_BASE				EQU 0xA8030A00
UART0_Base			EQU	0xA8200000
ICU_BASE				EQU 0xA8240000

; --- Amount of memory (in bytes) allocated for stacks

Len_FIQ_Stack    EQU     0
Len_IRQ_Stack    EQU     256
Len_ABT_Stack    EQU     0
Len_UND_Stack    EQU     0
Len_SVC_Stack    EQU     1024
Len_USR_Stack    EQU     1024

; Add lengths >0 for FIQ_Stack, ABT_Stack, UND_Stack if you need them.
; Offsets will be loaded as immediate values.
; Offsets must be 8 byte aligned.

Offset_FIQ_Stack         EQU     0
Offset_IRQ_Stack         EQU     Offset_FIQ_Stack + Len_FIQ_Stack
Offset_ABT_Stack         EQU     Offset_IRQ_Stack + Len_IRQ_Stack
Offset_UND_Stack         EQU     Offset_ABT_Stack + Len_ABT_Stack
Offset_SVC_Stack         EQU     Offset_UND_Stack + Len_UND_Stack
Offset_USR_Stack         EQU     Offset_SVC_Stack + Len_SVC_Stack

		ALIGN
		EXPORT	Return_LR
Return_LR				DCD		0x00000000	; return address of program for diag
Str_Ask_USB_Testing		DCB		0x0D, 0x0A, "Process USB testing (Y/N) ?", 0

		ALIGN

; -------- subroutines ---------

; ----------------------------------------------
; Get_LED
;	input : None
;	return: R0 = LED value
;
;	This routine read the LED value
; ----------------------------------------------
		ALIGN
		EXPORT Get_LED
Get_LED
		;LDR		r0, = 0xA8020200	;GPIO base
		LDR		r0, = 0xA8070000	;GPIO base
		LDR		r0, [r0]
		BIC		r0, r0, #0xFFFFFFF0	; keep 0~15
		MOV		pc, lr

; ----------------------------------------------
; Set_LED
;	input : R0 = value to light LEDs (0~15)
;	return: None
;
;	This routine light up the LED by input value
; ----------------------------------------------
		ALIGN
		EXPORT Set_LED
Set_LED
		STMFD	sp!, {r0, r1, LR}	; push registers

		BIC		r0, r0, #0xFFFFFFF0	; keep 0~15
		ORR		r1, r0, #0x00FF0000
		;LDR		r0, = 0xA8020200	;GPIO base
		LDR		r0, = 0xA8070000	;GPIO base
		STR		r1, [r0]
		LDMFD	sp!, {r0, r1, PC}	; pop and return

; ----------------------------------------------
; Config_SRAM_To_3D
;	input : None
;	return: None
;
;	This routine check SRAM configuration and set to 3D if the original setting is not correct
;	If the SRAM configuration is changed, then reboot.
; ----------------------------------------------
		ALIGN
; Config the Cache/TCM/SRAM/3D memory of MP323
;
		EXPORT Config_SRAM_To_3D
Config_SRAM_To_3D
		STMFD	sp!, {r1, r2, r3, LR}	; push registers

		LDR		r2, = 0xA8060000	;Boot Mode, Cache, TCM, On-Chip Memory Configuration Registers
		LDR		r1, [r2]
		AND		r3, r1, #0x00700000	; clear unnecessary bits and keep 3D memory configure bits
		CMP		r3, #0x00400000		; enable 4K bytes SRAM for 3D, another 16K Bytes external memory is used by G3D
		LDMEQFD	sp!, {r1, r2, r3, PC}	; pop and return

		BIC		r1, r1, #0x00700000	; keep other bits and set another 16K Bytes external memory is used by G3D
		ORR		r1, r1, #0x00400000	; enable 4K bytes SRAM for 3D
		STR		r1, [r2]			; Set !!

		LDR		r2, = 0xA8020800	;CFG_CPU_CLK in CKG
		LDR		r1, [r2]			; get original value
		ORR		r1, r1, #0x00000020	; ARM_reset is D[5]
		STR		r1, [r2]			; set D[5] = 1
		BIC		r1, r1, #0x00000020
		STR		r1, [r2]			; set D[5] = 0 to reset ARM

WaitCPUReset_for_3D
		B		WaitCPUReset_for_3D
		
; ----------------------------------------------
; Config_SRAM_To_System
;	input : None
;	return: None
;
;	This routine check SRAM configuration and release them to system if the original setting is for 3D.
;	If the SRAM configuration is changed, then reboot.
; ----------------------------------------------
		ALIGN
; Config the Cache/TCM/SRAM/3D memory of MP323
;
		EXPORT Config_SRAM_To_System
Config_SRAM_To_System
		STMFD	sp!, {r1, r2, r3, LR}	; push registers

		;LDR	r2, = 0xA8060000	;Boot Mode, Cache, TCM, On-Chip Memory Configuration Registers
		LDR		r2, = 0xA8630000	;Boot Mode, Cache, TCM, On-Chip Memory Configuration Registers
		LDR		r1, [r2]
		AND		r3, r1, #0x00700000	; clear unnecessary bits and keep 3D memory configure bits
		CMP		r3, #0x00100000		; disable 4K bytes SRAM for 3D, another 16K Bytes external memory is used by I-TCM
		LDMEQFD	sp!, {r1, r2, r3, PC}	; pop and return

		BIC		r1, r1, #0x00700000	; keep other bits and set another 16K Bytes external memory is used by I-TCM
		ORR		r1, r1, #0x00100000	; disable 4K bytes SRAM for 3D
		STR		r1, [r2]			; Set !!

		LDR		r2, = 0xA8020800	;CFG_CPU_CLK in CKG
		LDR		r1, [r2]			; get original value
		ORR		r1, r1, #0x00000020	; ARM_reset is D[5]
		STR		r1, [r2]			; set D[5] = 1
		BIC		r1, r1, #0x00000020
		STR		r1, [r2]			; set D[5] = 0 to reset ARM

WaitCPUReset_for_system
		B		WaitCPUReset_for_system
		

; ----------------------------------------------
; InputEmpty
;	input : None
;	return: r0 = 1 --> Input UART0 buffer empty
;	        r0 = 0 --> Input UART0 buffer is not empty
; ----------------------------------------------
		ALIGN
		EXPORT InputEmpty
InputEmpty
		LDR		r0, = UART0_Base	; Base address of UART0
		LDR		r0, [r0, #0x18]
		TST		r0, #0x10			; check RxFE
		MOVEQ	r0, #0
		MOVNE	r0, #1
		MOV		pc, lr


; ---- Read a char from UART0 for retarget.c -------------------
;	input : None
;	return: r0 contain the returned char
; ----------------------------------------------
		ALIGN
		EXPORT receive_char
receive_char
		STMFD	sp!, {r1, r2, LR}	; push registers
		LDR		r1, = UART0_Base
receive_char_Check_Rx_FIFO
		LDR		r2, [r1, #0x18]
		TST		r2, #0x10			; check RXFE
		BNE		receive_char_Check_Rx_FIFO
receive_char_Rx_FIFO_Not_Empty
		LDRB	r0, [r1]			; get data
		LDMFD	sp!, {r1, r2, PC}	; pop and return

; ---- Write a char to UART0 for retarget.c -------------------
;	input : r0 point to a char (byte) to be written
;	return: None
; ----------------------------------------------
		ALIGN
		EXPORT sendchar
sendchar
		STMFD	sp!, {r0-r2, LR}	; push registers
		LDR		r1, = UART0_Base
sendchar_Check_Tx_FIFO
		LDR		r2, [r1, #0x18]
		TST		r2, #0x20			; check Tx FIFO full?
		BEQ		sendchar_Tx_FIFO_OK
sendchar_Wait_Tx_Empty
		LDR		r2, [r1, #0x18]
		TST		r2, #0x80			; check Tx FIFO empty?
		BEQ		sendchar_Wait_Tx_Empty
sendchar_Tx_FIFO_OK
		LDRB	r0, [r0]
		STR		r0, [r1, #0]		; write
		LDMFD	sp!, {r0-r2, PC}	; pop and return

; ---- Write a char to UART0 -------------------
;	input : r0 is the data to be written
;	return: None
; ----------------------------------------------
Write_Char
		STMFD	sp!, {r1, r2, LR}	; push registers
		LDR		r1, = UART0_Base
Check_Tx_FIFO
		LDR		r2, [r1, #0x18]
		TST		r2, #0x20			; check Tx FIFO full?
		BEQ		Tx_FIFO_OK
Wait_Tx_Empty
		LDR		r2, [r1, #0x18]
		TST		r2, #0x80			; check Tx FIFO empty?
		BEQ		Wait_Tx_Empty
Tx_FIFO_OK
		STR		r0, [r1, #0]		; write
		LDMFD	sp!, {r1, r2, PC}	; pop and return


; ---- Write a null terminate string to output ---------
;	input : r0 is the pointer to the written data
;	return: None
; ------------------------------------------------------
		EXPORT	Write_String
Write_String
		STMFD	sp!, {r0, r1, LR}
		MOV		r1, r0
Write_String_Next_Char
		LDRB	r0, [r1], #1
		CMP		r0, #0
		LDMEQFD	sp!, {r0, r1, PC}		; pop and return
		BL		Write_Char				; call Write_Char
		B		Write_String_Next_Char

; ---- Translate nibble into ASCII and print out  ---------
;	input : r0[3:0] is the value to be printed
;	return: None
; ---------------------------------------------------------
Write_Nibble
		STMFD	sp!, {r0, LR}
		BIC		r0, r0, #0xFFFFFFF0
		ADD		r0, r0, #0x30
		CMP		r0, #0x39
		BLE		Write_Nibble_PrintASCII
		ADD		r0, r0, #0x07
Write_Nibble_PrintASCII
		BL		Write_Char
		LDMFD	sp!, {r0, PC}

; ---- Translate Byte into ASCII and print out  ---------
;	input : r0[7:0] is the value to be printed
;	return: None
; ---------------------------------------------------------
Write_Byte
		STMFD	sp!, {r0, r1, LR}
		MOV		r1, r0
		MOV		r0, r1, LSR #4
		BL		Write_Nibble
		MOV		r0, r1
		BL		Write_Nibble
		LDMFD	sp!, {r0, r1, PC}

; ---- Translate HWord into ASCII and print out  ---------
;	input : r0[15:0] is the value to be printed
;	return: None
; ---------------------------------------------------------
Write_HWord
		STMFD	sp!, {r0, r1, r2, LR}
		MOV		r2, #12
		MOV		r1, r0
Write_HWord_NextNibble
		MOV		r0, r1, LSR r2
		BL		Write_Nibble
		SUBS	r2, r2, #4
		BGE		Write_HWord_NextNibble
		LDMFD	sp!, {r0, r1, r2, PC}

; ---- Translate Word into ASCII and print out  ---------
;	input : r0[31:0] is the value to be printed
;	return: None
; ---------------------------------------------------------
Write_Word
		STMFD	sp!, {r0, r1, r2, LR}
		MOV		r2, #28
		MOV		r1, r0
Write_Word_NextNibble
		MOV		r0, r1, LSR r2
		BL		Write_Nibble
		SUBS	r2, r2, #4
		BGE		Write_Word_NextNibble
		LDMFD	sp!, {r0, r1, r2, PC}

; ---- Write New Line  ------------------------------------
;	input : None
;	return: None
; ---------------------------------------------------------
		ALIGN
Str_New_Line						DCB		0x0D, 0x0A, 0
		ALIGN
Write_New_Line
		STMFD	sp!, {r0, LR}
		LDR		r0, =Str_New_Line
		BL		Write_String
		LDMFD	sp!, {r0, PC}


		ALIGN

        EXPORT  Reset_Handler

Reset_Handler

	IF :DEF: For_Diag
;		IMPORT	Return_LR
		LDR	r0, = Return_LR
		STR	lr, [r0]
	ENDIF

	IF :LNOT::DEF: For_Diag

; Config the Cache/TCM/SRAM/3D memory of MP323
;

;;;;		LDR		r2, = 0xA8060000	;Boot Mode, Cache, TCM, On-Chip Memory Configuration Registers
;;;;		
;;;;		LDR		r1, = 0x02156743
;;;;		STR		r1, [r2, #0x04]		; priority
;;;;		LDR		r1, = 0x00000010
;;;;		STR		r1, [r2, #0x0C]		; set to fix priority
;;;;		

; --- Initialize stack pointer registers

; Enter each mode in turn and set up the stack pointer

        IMPORT  top_of_stacks       ; located by scatter-file
        LDR     r0, =top_of_stacks  ; top of stack region

;       MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit ; No interrupts
;       SUB     sp, r0, #Offset_FIQ_Stack

        MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_IRQ_Stack

;       MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit ; No interrupts
;       SUB     sp, r0, #Offset_ABT_Stack

;       MSR     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit ; No interrupts
;       SUB     sp, r0, #Offset_UND_Stack

        MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_SVC_Stack

; ********** Don't touch R0 before enter USER mode **********

; --- Initialize memory system
    IF :DEF: CACHE
        IMPORT  Clock_Speed     ; in CMclocks.s
        IMPORT  Cache_Init      ; in the core-specific files e.g. 940T.s
        BL      Clock_Speed
        BL      Cache_Init
    ENDIF


; --- Initialize critical IO devices
        ; ...

	IF :DEF: For_NOR
; --- Set SDRAM_SETTING register
	 ;LDR		r2, = 0xA8030004
		LDR		r2, = 0xA8170004
		LDR		r1, = 0x02E04037					; for 10MHz, 12MHz
		STR		r1, [r2]
; --- Set SDRAM_POWER_DOWN register --- power up
	 ;LDR		r2, = 0xA8030000
		LDR		r2, = 0xA8170000
		LDR		r1, = 0x00000024					; wrap-8
;		LDR		r1, = 0x00001400					; wrap-8, for DDR
		STR		r1, [r2]
	ENDIF

;
; --------- Set MP323 ICU ----
		LDR		r2, = ICU_BASE
		LDR		r1, = 0x00000000
		STR		r1, [r2, #0x08]				; Set all interrupt mode is 0 --> IRQ mode

		LDR		r1, = 0x00000000
		STR		r1, [r2, #0x20]				; set INTPRT to fixed priority as default

		LDR		r1, = 0x00000000
		STR		r1, [r2, #0x38]				; Set external EINTFIL as default.

		LDR		r1, = 0xFFFFFFFF
		STR		r1, [r2, #0x0]				; Clear SRCPND register

		LDR		r1, = 0xFFFFFFFF
		STR		r1, [r2, #0x18]				; Clear INTPND register

		LDR		r1, = 0xFFFFFEFF
		STR		r1, [r2, #0x10]				; Set all interrupt mask is 1 --> mask out, except UART0=0 --> int available

; --- Set SDRAM_POWER_DOWN register --- power up
	;LDR		r2, = 0xA8030000
		LDR		r2, = 0xA8170000
;		LDR		r1, = 0x00001404					; For DDR MRS
;		STR		r1, [r2]

; --- Now change to User mode and set up User mode stack, if needed
        MSR     CPSR_c, #Mode_USR:OR:F_Bit           ; IRQs now enabled
        SUB     sp, r0, #Offset_USR_Stack

; ********** R0 is released to change **********

; --- Config CKG for CPU/AHB/APB ---
		;LDR		r0, = 0xA8020800			; CKG Base Address
		LDR		r0, = 0xA8130000			; CKG Base Address
		LDR		r1, = 0x00000000			; AHB clock = CPUCK
		STR		r1, [r0, #0x104]
		LDR		r1, = 0x00000001			; APB clock = AHBCK/2
		STR		r1, [r0, #0x10C]
		
		LDR		r1, [r0]					; Get value of CFG_CPU_CLK register
		AND		r1, r1, #0xFFFFFFFD			; Set bit2(CPU_tstop) = 0
		STR		r1, [r0]					; temporarily stop CPU clock to align AHB & APB setting.
											; Bit 2 will return to default value by itself.
		
		LDR		r1, = 0x0000000F			; PLL1~4 as OSCI pad
		STR		r1, [r0, #0xA8]
			
; --- Config CKG for UART ---
		LDR		r1, = 0x0
		STR		r1, [r0, #0x0C]				; select UART0 src = PLL4
		LDR		r1, = 2_00000101			; reset UART0, turn on UART0, divide clk by 2
		STR		r1, [r0, #0x08]
		LDR		r1, = 2_00000111			; release reset of UART0, turn on UART0, divide clk by 2
		STR		r1, [r0, #0x08]

; --- Config CKG for IDU ---
		LDR		r1, = 0x01
		STR		r1, [r0, #0x4C]				; select IDU src = PLL2
		LDR		r1, = 0x17
		STR		r1, [r0, #0x48]
		AND		r1, r1, #0xFFFFFFFD
		STR		r1, [r0, #0x48]				; reset IDU clock
		ORR		r1, r1, #0x02
		STR		r1, [r0, #0x48]				; release reset of IDU clock

; --- Config CKG for DMA321 ---
		LDR		r1, = 0x01					; enable DMA321 clock & S/W reset it
		STR		r1, [r0, #0x110]
		LDR		r1, = 0x03					; enable DMA321 clock & release the S/W reset
		STR		r1, [r0, #0x110]

; --- Config CKG for I2S ---
		LDR		r1, = 0x05
		STR		r1, [r0, #0x60]				; turn on I2S clock, S/W reset I2S clock, diveder N = 1
		LDR		r1, = 0x07
		STR		r1, [r0, #0x60]				; turn on I2S clock, relese S/W reset I2S clock, diveder N = 1
		LDR		r1, = 0x0
		STR		r1, [r0, #0x64]				; I2S clock comes from WFS clock, not inverted
		; --- Config clock for I2S (WFS)
		LDR		r1, = 0x00000001
		STR		r1, [r0, #0xAC]				; (SEL_WFS_CLK) = PLL1

; --- Config CKG for I2C ---
		LDR		r1, = 0x00000001			; turn on I2C clock, S/W reset I2C clock
		STR		r1, [r0, #0xF0]
		LDR		r1, = 0x00000003			; turn on I2C clock, release S/W reset I2C clock
		STR		r1, [r0, #0xF0]
		LDR		r1, = 0x0000003F			; I2C clock source = PLL2/4, divide by (0xF+1) = 196KHz
		STR		r1, [r0, #0xF4]

; --- Config CKG for G3D ---
		LDR		r1, = 0x07
		STR		r1, [r0, #0x11C]
		AND		r1, r1, #0xFFFFFFFD
		STR		r1, [r0, #0x11C]				; reset G3D clock
		ORR		r1, r1, #0x02
		STR		r1, [r0, #0x11C]				; release reset of G3D clock


; --------- Set IOs -------------
; --- Set UART0 GPIO ---
		;LDR		r0, = 0xA8020200
		LDR		r0, = 0xA8070000

		MOV		r1, #0x03
		STR		r1, [r0, #0x3C]			; set UART0 Tx/Rx as UART0

		MOV		r1, #0
		STR		r1, [r0, #0x04]			; set DGPIO config

		LDR		r1, = 0x00FF0001
		STR		r1, [r0]				; Set LED on

; --- Set IDU GPIO ---
;		LDR		r0, = 0xA8020200		; GPIO base
		LDR		r1, = 0xFFC				; LRSTx/LRDx/LWRx/LDI/LCS2x/LCS1x/LOE/LHS/LVS/LCLK enable
		STR		r1, [r0, #0x1C]			; LRST default is low, enable it will set it to high

; --- Set SIU (I2C) GPIO ---
;		LDR		r0, = 0xA8020200		; IO BASE
		LDR		r1, = 0x00000003
		STR		r1, [r0, #0x0C]			; enable SSDATA, SSCLK

; --- Set Audio (I2S) GPIO ---
;		LDR		r0, = 0xA8020200		; IO Base
		LDR		r1, = 0x0000000F
		STR		r1, [r0, #0x24]			; enable ASDO, ASDI, ALRCLK, ABCLK

; 
; 
; --- Add your test ASM code here
;
;

; Because the HAPS UART_CLK is set to equal APB_CLK, uses 12MHz by above setting
;BAUD_LOW	EQU	0x03				; for  8MHz OSC... ( 8M/3.25/16/38400)-1 = 3.006410256
;BAUD_LOW	EQU	0x02				; for  6MHz OSC... ( 6M/3.25/16/38400)-1 = 2.004807692
;BAUD_LOW	EQU	0x04				; for 10MHz OSC... (10M/3.25/16/38400)-1 = 4.008012821
BAUD_LOW	EQU	0x05				; for 12MHz OSC... (12M/3.25/16/38400)-1 = 5.009615385
;BAUD_LOW	EQU	0x01				; for 12MHz OSC... (12M/3.25/16/115200)-1 = 1.003205128

		LDR		r0, = UART0_Base	; Base address of UART0
		
		MOV		r2, #BAUD_LOW		; Baud Rate register low
		STR		r2, [r0, #0x10]		; 38400 bps, the value is measured by LA
		
		MOV		r1, #0
		STR		r1, [r0, #0x0C]		; Baud Rate register high

		MOV		r1, #0x70			; 8N1, enable FIFO
		STR		r1, [r0, #0x08]		; Line control register

		MOV		r1, #0x00			; Error clear register
		STR		r1, [r0, #0x04]		; set to clear errors

;		MOV		r1, #0x51			; enable bit at bit0 , enable Rx & RxTi interrupt
		MOV		r1, #0x01			; enable bit at bit0 , disable Rx & RxTi interrupt
		STR		r1, [r0, #0x14]		; enable UART0



	ENDIF


        IMPORT  __main

; --- Now enter the C code
        B       __main   ; note use B not BL, because an application will never return this way

        END

