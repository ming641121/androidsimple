/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file is a local header file for Card Manager
 *
 * Author(s):
 *
 *     
 */


#ifndef CM_H
#define CM_H

/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */
#define DEBUG printf

#ifdef  TRUE
#undef  TRUE
#define TRUE    1
#endif

#ifdef  FALSE
#undef  FALSE
#define FALSE   0
#endif

#ifdef  NULL
#undef  NULL
#define NULL   0
#endif

#define MAX_CM_IN_MESSAGES      32


/* 
 * following are the messages for the ID Task 
 */

/*
 *====================================================================
 *=                             Typedefs                           =
 *====================================================================
 */

typedef struct imgFileHdr 
{
    S32_t   len;
    U16_t   slot;
    U16_t   ckSum;
    char    version[MAX_VERSION_LENGTH];
    char    imageFileName[MAX_FILE_NAME_LENGTH];
}IMAGE_FILE_HDR_t;

/*
 *====================================================================
 *=                             Macros                           =
 *====================================================================
 */

#define   GET_IMAGE_START_ADDR() 0x12345

/*
 *====================================================================
 *=                             Externs                           =
 *====================================================================
 */
#if 0 /* System Services creates the ID now */
extern SIW_MQID_t    cmMsgQId;
#endif  /* System Services creates the ID now */

extern void CmComputeCkSumAndExecute();
extern U16_t checkSum(U32_t *addr, U32_t len);

#endif  /* CM_H */
