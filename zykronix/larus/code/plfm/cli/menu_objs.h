/*
 *
 * Copyright (C) 2006 by ahoya Networks Incorporated.
 *
 */
#ifndef _MENU_OBJS_H_
#define _MENU_OBJS_H_

#define MENU_ENTRY_SEL        0
#define MENU_DATA_TBL_SEL     1
#define MENU_DATA_PARM_SEL    2
#define MENU_DSP_PARM_SEL     3

#define MENU_CMD_CFG          0
#define MENU_CMD_DSP          1
#define MENU_CMD_NULL         2


#define MENU_DSP_BODY         0
#define MENU_DSP_FUNC         1

/*
 * bit 0 for multi input selection
 */
#define MENU_MULTI_IN     0x1

/*
 * bit 1 input '*' for all
 */
#define MENU_ALL_AST      0x2
/*
 * bit 2-3 input requirement
 */
#define MENU_NULL_IN_ERROR     4
#define MENU_NULL_IN_NOCHG   0x8
#define MENU_NULL_IN_UFUNC   0x10


#define MENU_SECUR_ECHO      0x20

#define MAP_ARG_FUNC          0
#define MAP_ARG_ARRAY         1
#define MAP_ARG_BITMAP        2
#define MAP_ARG_NOMAP         3

#define MENU_IN_TYPE_STR      1
#define MENU_IN_TYPE_NUM      2
#define MENU_IN_TYPE_INT      3
#define MENU_IN_TYPE_BITS     4

#define MENU_FUNC_PRC         1
#define MENU_FUNC_LST         2

/*  User Access Privilege Level Definition
 *
 */

#define AXS_PRI_1      1 
#define AXS_PRI_2      2
#define AXS_PRI_3      3
#define AXS_PRI_4      4
#define AXS_PRI_5      5
#define AXS_PRI_6      6
#define AXS_PRI_7      7          
  

typedef struct MENU_DSP_TBL_s {
     U8_t                 type;                /* menu selection or data selection */
     U8_t                 lowBound;            /* low bound of selection */
     U8_t                 highBound;           /* high bound of selection */
     U8_t                 menuDspType;         /* from menu or call function */
     U8_t                 *menuBanner;         /* Banner for this menu */
     U8_t                 *menuBody;           /* body of the menu */
     U16_t                flags;               /* multiple sel, all sel, no inp action */
     U8_t                 fldPos;              /* field position */
     U8_t                 inType;              /* data type i.e. string or number */
     U8_t                 mapType;             /* bitmap, function, or map array list */
     void                 *dFuncList;          /* function list */
} MENU_DSP_TBL_t;

typedef struct MENU_DATA_ENTRY_s {
     U8_t                 type;                /* menu selection or data selection */
     U8_t                 lowBound;            /* low bound of selection */
     U8_t                 highBound;           /* high bound of selection */
     U8_t                 menuDspType;         /* from menu or call function */
     U8_t                 *menuBanner;         /* Banner for this menu */
     U8_t                 *menuBody;           /* body of the menu */
     U16_t                flags;               /* multiple sel, all sel, no inp action */
     U8_t                 fldPos;              /* field position */
     U8_t                 inType;              /* data type i.e. string or number */
     U8_t                 mapType;             /* bitmap, function, or map array list */
     void                 *mapFunc;            /* convert user selection to opt data */
     void                 *inFunc;             /* if NULL_IN_UFUNC */ 
} MENU_DATA_ENTRY_t;

typedef struct MENU_DATA_TBL_s {
     U8_t                 type;                /* menu selection or data selection */
     U8_t                 ttlData;             /* total # of data entry */
     U8_t                 cmdType;             /* config or display */
     U8_t                 snmpData;            /* snmp function or user function */
     void                 *procFunc;
     void                 *selList;            /* selection of menu or list of data */
     U8_t                 accessLvl;
} MENU_DATA_TBL_t;

typedef struct MENU_TBL_s {
     U8_t                 type;                /* menu selection or data selection */
     U8_t                 lowBound;            /* low bound of selection */
     U8_t                 highBound;           /* high bound of selection */
     U8_t                 *menuBanner;         /* Banner for this menu */
     U8_t                 *menuBody;           /* body of the menu */
     void                 *selList;            /* selection of menu or list of data */
} MENU_TBL_t;


#endif
