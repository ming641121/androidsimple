/*
****************************************************************
*                      Magic Pixel Inc.
*
*    Copyright 2004, Magic Pixel Inc., HsinChu, Taiwan
*                    All rights reserved.
*
*
*
* Filename:    TYPEDEF.H
*
* Programmer:    Wesley
*                MPX E120 division
*
* Created: 10/27/2004
*
* Description: Global type definition for typical types
*
* Variable prefix:
*    [p][s]typeVariableName
*    p: for pointer variable
*    s: for signed variable
*
*    type:
*        BYTE    b
*        WORD    w
*        DWORD   dw
*        LONG    l
*        BOOL    bool
*
*    Example:
*        swHuffBase - a signed WORD variable
*        pdwImageBuffer - a pointer to a DWORD variable
*
*	Exception:
*		1. For the user defined structure type, leave the 'type' empty
*    	   and only have the [p] option.
*
* Change History (most recent first):
*     <1>     10/27/2004    Wesley    first file
*     <2>     11/17/2004    Wesley    add exception 1 for structure
****************************************************************
*/

#ifndef __TYPEDEF_NEW_H
#define __TYPEDEF_NEW_H

typedef unsigned long long LONG;
typedef	unsigned int       DWORD;
typedef unsigned short     WORD;
typedef unsigned char      BYTE;
typedef unsigned char      BOOL;

typedef long long SLONG;
typedef int       SDWORD;
typedef short     SWORD;
typedef char      SBYTE;

#define TRUE  1
#define FALSE 0
//#define NULL  0

#endif    // __TYPEDEF_NEW_H
