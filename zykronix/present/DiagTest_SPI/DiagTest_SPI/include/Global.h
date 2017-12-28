/*
****************************************************************
*                      Magic Pixel Inc.
*
*    Copyright 2004, Magic Pixel Inc., HsinChu, Taiwan
*                    All rights reserved.
*
*
*
* Filename:    GLOBAL.H
*
* Programmer:    Alex Tsai
*                MPX E120 division
*
* Created: 10/27/2004
*
* Description: Common header file
*
*        
* Change History (most recent first):
*     <1>     10/27/2004    Alex Tsai    first file
****************************************************************
*/
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "TypeDef.h"
#include "RegFile.h"
#include "RegBitsField.h"
#include "Macro.h"
#include "SysUtility.h"
// Global variables
#ifdef __MAIN_C
    #define EXT
#else
    #define EXT extern 
#endif

EXT STSIUREG *pstSiu;
EXT STIPUREG *pstIpu;
EXT STIDUREG *pstIdu;
EXT STJPGREG *pstJpg;
EXT STMP4REG *pstMp4;
EXT STDRAIREG *pstDrai;
EXT STUSBREG *pstUsb;
EXT STHIUREG *pstHiu;
EXT STCKGREG *pstCkg;
EXT STICUREG *pstIcu;
EXT STGPIOREG *pstGpio;
EXT STUARTREG *pstUart;
EXT STTIMERREG *pstTimer0, *pstTimer1, *pstTimer2, *pstTimer3, *pstTimer4;
EXT STI2SREG *pstI2s;
EXT STMCARDREG *pstMcard;
EXT STSYSTEMPARAMETER *pstSystemParam;
EXT DWORD _gdwFileSize ;
EXT STRTCREG *pstRTC;
EXT STSPIREG *pstSPI;
EXT STDMA2REG *pstDMA2;
#endif    // __GLOBAL_H
