/*
 *
 * Copyright (C) 2005 by ahoya Networks Incorporated.
 *
 */

#ifndef _CLI_OBJS_H_
#define _CLI_OBJS_H_

#define ARG_ENUM      0
#define ARG_STR       1
#define ARG_CONV      2
#define ARG_FUNC      3


#define DSP_NUM                0
#define DSP_STRING             1
#define DSP_TABLE              2


typedef struct {
    U8_t  *mapStr;
    U32_t val;
} CLIUMAPSTRUC_t, *pCLIUMAPSTRUC_t;

typedef struct {
    OID        oid;
    ObjectInfo *objPtr;
    U8_t       *opt;
    U16_t      argType;
    U16_t      mapCnt;
    void       *mapFunc;
    U8_t       *argHelpMsg;
    U8_t       *objName;
    U8_t       *objDesc;
} CLIMIBENTITY_t, *pCLIMIBENTITY_t;

typedef struct {
    pCLIMIBENTITY_t *mibKeyList;
    pCLIMIBENTITY_t *mibList;
    U16_t           keyLen;
    U16_t           numOfMibEnt;
} CLIMIBCFGGROUP_t, *pCLIMIBCFGGROUP_t;

typedef struct {
    pCLIMIBENTITY_t apiObject;
    U8_t            seperator;
} CLIMIBAPIOBJ_t, *pCLIMIBAPIOBJ_t;


typedef struct {
    U8_t             *name;
    pCLIMIBAPIOBJ_t  apiList;
    U8_t             *optValStr;
} CLIMIBAPIGROUP_t, *pCLIMIBAPIGROUP_t;


typedef struct {
    pCLIMIBENTITY_t dspMibEntry;
    U8_t            *format;
    void            *dspFunc;
    U32_t           dspType;
} MIBDSPENTITY_t, *pMIBDSPENTITY_t;


typedef struct {
    U8_t              *name;
    pCLIMIBCFGGROUP_t mibCfgGroup;
    U16_t             numOfDspEnt;
    pMIBDSPENTITY_t   mibDspList;
    U8_t              *title;
} CLIMIBDSPGROUP_t, *pCLIMIBDSPGROUP_t;

typedef struct {
    U8_t              *cfgName;
    pCLIMIBCFGGROUP_t *mibTblList;
    U32_t             tblCnt;
} CLICFGTBL_t, *pCLICFGTBL_t;


typedef STATUS_t (*MIBARGFUNC_t)(pCLIMIBENTITY_t, U8_t *, U32_t *);
typedef STATUS_t (*DSPFUNC_t)(pCLIMIBDSPGROUP_t, OID *, U32_t *);

#endif
