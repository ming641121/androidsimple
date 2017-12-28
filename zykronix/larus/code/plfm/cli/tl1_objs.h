/*
 *
 * Copyright (C) 2006 by ahoya Networks Incorporated.
 *
 */

#ifndef _TL1_OBJS_H_
#define _TL1_OBJS_H_

#define  ARG_IGNORE               0
#define  ARG_TID                  1
#define  ARG_SID                  2
#define  ARG_PID				  3
#define  ARG_AID                  4
#define  ARG_CTAG                 5
#define  ARG_SUBCMD               6
#define  ARG_DATA                 7

#define  ARG_NULL                 0
#define  ARG_ULIST                1
#define  ARG_UFUNC                2
#define  ARG_SUBCMD_LIST          3


#define TL1_CFG_CMD               0
#define TL1_DSP_CMD               1
/* 
 * flags bit definitions
 */
/*
 * bit 0, mandatory or optional 
 *
 */
#define  TL1_FLAG_OPTIONAL          0
#define  TL1_FLAG_MANDATORY       0x1
#define  TL1_FLAG_IGNORE          0x2

/*
 * bit 1-2 for subcmd selection
 */
#define  TL1_FLAG_NULL_ALL        0x4
#define  TL1_FLAG_NULL_DATA       0x8

/*
 * bit 3 fix or flex format
 */
#define  TL1_FLEX_FORMAT          0x10

/*
 * bit 4 "All" acceptable
 */
#define  TL1_ALL_ALLOWED          0x20
#define  TL1_NO_RANGE_ALLOWED     0x40
/*
 * bit 5 for multi input selection
 */
#define TL1_MULTI_ALLOWED        0x80
/*
 * bit 6 for subcmd selection
 */
#define TL1_SUBCMD_KEY            0x100


/*
 * map type
 */
#define TL1_MAP_NOMAP             0
#define TL1_MAP_MAPCNV            1
#define TL1_MAP_MAPFUNC           2
#define TL1_MAP_MAPCNV_BITMAP     3
#define TL1_MAP_MAPCNV_FUNC       4

#define TL1_FUNC_PRC         1
#define TL1_FUNC_LST         2



typedef struct {
    U16_t            flags;
    U8_t             inType;                    /* string, number, or integer */
    U8_t             fldPos;                       /* position in cdl function */
    U8_t             mapType;                   /* bitmap, function, or map array list */
    void             *mapFunc;                  /* convert user selection to opt data */
    U8_t             *argStr;                   /* FLEX name */
} TL1_ARG_t;



typedef struct {
    U8_t             type;                      /* TID, AID, ... etc */
    U8_t             flags;                     /* Mandatory or Optional,  Fix or flex */
    U8_t             ttlData;                   /* num of data in this field if fix or float data or
                                                   num of sub cmd if this field is subcmd */
    U8_t             argType;                   /* user function, arg list or subcommand */
    void             *parmLst;                  /* function pointer if user function
                                                 * list of arg if fix or flex data format 
                                                 * list of subcmds if subcmd */
} TL1_FIELD_PARM_t;

typedef struct {
    U8_t             inType;                    /* string, number, or integer */
    U8_t             fldPos;                       /* position in cdl function */
    void             *data;                     /* convert user selection to opt data */
} TL1_DEFAULT_ARG_t;


typedef struct {
    U8_t             *name;
    U8_t             matchLen;
} TL1_SUBCMD_KEY_t;

typedef struct {
    TL1_SUBCMD_KEY_t *subKey;
    U8_t             numSubKey;
    U8_t             numOfFld;                   /* number of field */
    U8_t             cmdType;                    /* config or display */
    U8_t             snmpData;                   /* snmp function or user function */
    void             *procFunc;                  /* mib cdl func or user func */
    TL1_FIELD_PARM_t *fieldList;                 /* list of field */ 
} TL1_SUBCMD_GROUP_t, *pTL1_SUBCMD_GROUP_t;

typedef struct {
    U8_t             *name;                      /* cmd name */
    U8_t             numOfFld;                   /* number of field */
    U8_t             cmdType;                    /* config or display */
    U8_t             snmpData;                   /* snmp function or user function */
    void             *procFunc;                  /* mib cdl func or user func */
    TL1_FIELD_PARM_t *fieldList;                 /* list of field */
    U8_t             accessLvl; 
    TL1_DEFAULT_ARG_t *deftData;
} TL1_GROUP_t, *pTL1_GROUP_t;

#endif
