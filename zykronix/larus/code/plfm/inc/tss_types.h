/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file main System wide definitions, such as all module numbers
 *    etc.
 *
 * Note:
 *    - Please be carefull when you modify this file. This file is included
 *	by every module in the system.  
 *
 * Author(s):
 *
 *  
 */


#ifndef AXSS_TYPES_H
#define AXSS_TYPES_H
/*
 *====================================================================
 *=                            include                               =
 *====================================================================
 */

/* ABSOLUTELY NO INCLUDE FILES HERE, PLEASE */


/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */

/* Following typedefs are system iwde typedefs. 
 */

typedef char                    S8_t;
typedef short                   S16_t;
typedef int                     S32_t;
typedef long long               S64_t;

typedef unsigned char           U8_t;
typedef unsigned short          U16_t;
typedef unsigned int            U32_t;
typedef unsigned long long      U64_t;

typedef int                     BOOL_t;
typedef int                     STATUS_t;
typedef void                    VOID_t;
typedef int                     RET_t;   /* most functions return RET_t */
typedef unsigned int            ADDRS_t;
typedef volatile unsigned char  VU8_t;
typedef volatile unsigned short VU16_t;
typedef volatile unsigned int   VU32_t;


typedef RET_t (*FUNC_PTR_t)();
typedef void  (*VOID_FUNC_PTR_t)();

/******************************************************************
 * System wide Macros
 ******************************************************************/
#define IN
#define OUT

/**********************************************************************
 *  VxWorks Return status values
 **********************************************************************/
#define OK              0
#define ERROR           (-1)

/***********************************************************************
 *  Defines for NULL, FALSE, and TRUE
 **********************************************************************/
#if     !defined(NULL)
#define NULL            0
#endif

#if     !defined(FALSE) || (FALSE!=0)
#define FALSE           0
#endif

#if     !defined(TRUE) || (TRUE!=1)
#define TRUE            1
#endif

#endif /* AXSS_TYPES_H */

