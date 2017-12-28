/*
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file is a local header file for File Manager
 *
 * Author(s):
 *
 *  
 */


#ifndef FM_H
#define FM_H

/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */
#define DBG     1

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

#define MAX_FM_IN_MESSAGES      32

extern char FM_PRINT_FLAG;
#define FM_DEBUG if (FM_PRINT_FLAG) printf

/*
 *====================================================================
 *=                             Typedefs                           =
 *====================================================================
 */


typedef struct fdTid
{
    NODE         node;
    SIW_TID_t    tid;
    S32_t        mode;
}FD_TID_t;


/* following structure for FLASH files */
#define FLASH_MEM_START_ADDRESS   (0x100)
#define MAX_FLASH_MEMORY     (2 * 1024 * 1024)   /* 2 Meg */
#define MAX_FLASH_FILE_SIZE  (1 * 1024 * 1024)
#define MAX_FLASH_FILES     (MAX_FLASH_MEMORY / MAX_FLASH_FILE_SIZE)
#define NUM_CUS_TONE_PLAN   MAX_CUST_TONE_PLAN_ID - MIN_CUST_TONE_PLAN_ID + 1

typedef struct flashRec
{
    BOOL_t taken; 
    U32_t  addr;
    U32_t  len;
} FLASH_REC_t;

typedef struct flashMem
{
    U32_t       nextAvail;
    FLASH_REC_t fRecord[MAX_FLASH_FILES];
} FLASH_MEM_t;

typedef struct {
        U8_t tonePlanFileName[MAX_FILE_NAME_LENGTH];
} TONE_PLAN_FILE_TABLE_t;

/*
 *====================================================================
 *=                             Macros                           =
 *====================================================================
 */

#define VALID_FILE_NAME(n) \
     ((((n) != NULL) &&    \
      ((strlen(n) > 0) && (strlen(n) <= MAX_FILE_NAME_LENGTH))) ? TRUE : FALSE)

#if 0 /* SSSSSSSS */
#define VALID_FILE_SIZE(s)  \
     ((((s) > 0) && ((s) <= MAX_FILE_LENGTH)) ? TRUE : FALSE)
#else
#define VALID_FILE_SIZE(s)  (TRUE)
#endif 

#define VALID_FILE_TYPE(t)  \
     ((((t) == RAM) || ((t) == HD) || ((t) == FLASH)) ? TRUE : FALSE)

#define VALID_FILE_FLAGS(f) \
     ((((f) == READ_ONLY) || ((f) == WRITE_ONLY) || ((f) == READ_WRITE)) ? TRUE : FALSE)

#define STAMP_FD(f)  ((f)->fdSignature = FD_SIGNATURE)

/* Following macros for tpackaging messages for remote communicaions with FM */

#define PACK_FM_ERR_MSG(d,e) {                      \
    (d)->msg = FM_NACK;                             \
    (d)->err = (e);                                 \
}

#define PACK_FM_RESP_MSG(d) {                       \
    (d)->msg = FM_RESP;                             \
    (d)->err = FM_OK;                               \
}

#define PACK_FM_ACK_MSG(d) {                        \
    (d)->msg = FM_ACK;                              \
    (d)->err = FM_OK;                               \
}

#define FM_HDR_PTR(i) ((FM_HDR_t *)(&(i)->data[0]))
#define FM_DATA_PTR(i) ((char *)(&(((MSG_t *)(i))->data[sizeof(FM_HDR_t)])))


/*
 *====================================================================
 *=                             Externs                           =
 *====================================================================
 */
#if 0 /* System Services creates the ID now */
extern SIW_MQID_t    fmMsgQId;
#endif  /* System Services creates the ID now */
extern U32_t allocFlashMem();
extern void freeFlashMem(U32_t address);
RET_t FmMain(int a0, int a1, int a2, int a3, int a4, 
	 int a5, int a6, int a7, int a8, int a9);

#endif  /* FM_H */
