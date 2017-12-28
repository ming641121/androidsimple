/*
****************************************************************
*                      Magic Pixel Inc.
*
*    Copyright 2004, Magic Pixel Inc., HsinChu, Taiwan
*                    All rights reserved.
*
*
*
* Filename:    MACRO.h
*
* Programmer:    Alex Tsai
*                MPX E120 division
*
* Created: 10/28/2004
*
* Description: Macro definition
*
*        
* Change History (most recent first):
*     <1>     10/28/2004    Alex Tsai    first file
****************************************************************
*/
#ifndef __MACRO_H
#define __MACRO_H

#define SETREGBITS(x, y, z) SetRegBitsUtil((DWORD *)x, y, z)
#define GETREGBITS(x, y)    GetRegBitsUtil((DWORD *)x, y)

#endif    // __MACRO_H
