/*
 *Copyright(C) 2005, All Rights Reserved, by
 *ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_trace.h
 *
 *Defines for trace.
 *
 *Note:
 *
 *Author(s):
 * 
 */

#ifndef _SIW_TRACE_H_
#define _SIW_TRACE_H_

#include <axss_types.h>

/***********************************************************************
 *  Defines for trace buffers
 **********************************************************************/
#define SIW_NUM_TRACE_BUFS          2
#define SIW_NUM_TRACE_ENTRIES    20000

/***********************************************************************
 *  General Define of using in Event Log Entry
 **********************************************************************/
#define TRACE_MODULE_NAME_SIZE      4
#define TRACE_FUNCTION_NAME_SIZE   44 

/***********************************************************************
 *  Defines for trace level
 **********************************************************************/
#define TRACE_DISABLE               0
#define TRACE_LEVEL_1               1
#define TRACE_LEVEL_2               2
#define TRACE_LEVEL_3               3
#define TRACE_LEVEL_4               4
#define TRACE_LEVEL_5               5

/***********************************************************************
 *  Structure Objects for Module and Trace
 **********************************************************************/
typedef struct {
    const char *            module;
    BOOL_t                  funcTrace;
    U16_t                   level;
    U16_t                   printflag;
} SIW_TRACE_OBJ_t;

/***********************************************************************
 *  Typedef of Trace Buffer Entry structure used to store both function
 *  and debug trace for the platform.
 **********************************************************************/
typedef struct {
    U32_t       traceCount;                         /* Trace Count Number */
    char        module[TRACE_MODULE_NAME_SIZE];     /* Module Name */
    char        function[TRACE_FUNCTION_NAME_SIZE]; /* Function Name */
    U32_t       trcId;                              /* Trace ID */
    U32_t       arg1;
    U32_t       arg2;
    U32_t       arg3;
    U32_t       arg4;
    U32_t       arg5;
} SIW_TRACE_ENTRY_t;

/***********************************************************************
 *  Typedef of Trace control buffer
 **********************************************************************/
#define TRACE_FILENAME_SIZE 16

typedef struct {
    U16_t    head;
    U16_t    tail;
    BOOL_t   wrapped;
    char     fname[TRACE_FILENAME_SIZE];
} SIW_TRACE_CONTROL_t;

/***********************************************************************
 *  Function:      SiwDbgTrace()
 *  Description:   This function is used for saving the debugging Trace 
 *                 message.  This function shall not be directly called
 *                 by any tasks.
 *
 *  In Parms:      const SIW_TRACE_OBJ_t *modObj - pointer to the TRACE
 *                                                 module
 *                 const char *function - pointer to the function name
 *                 U32_t trcId - trace ID defined by user
 *                 U32_t arg1 - the first arg
 *                 U32_t arg2 - the second arg
 *                 U32_t arg3 - the third arg
 *                 U32_t arg4 - the fourth arg
 *                 U32_t arg5 - the fifth arg
 *  Out Parms:     None
 *  Returns:       None
 **********************************************************************/
extern void SiwDbgTrace(const SIW_TRACE_OBJ_t *modObj,
                        const char *function,
                        U32_t trcId,
                        U32_t arg1,
                        U32_t arg2,
                        U32_t arg3,
                        U32_t arg4,
                        U32_t arg5);

/***********************************************************************
 *  Function:      SiwFuncTrace()
 *  Description:   This function is used for saving the function Trace 
 *                 message.  This function shall not be directly called
 *                 by any tasks.
 *
 *  In Parms:      const char *function - pointer to the function name 
 *                 U32_t arg1 - the first arg
 *                 U32_t arg2 - the second arg
 *                 U32_t arg3 - the third arg
 *                 U32_t arg4 - the fourth arg
 *                 U32_t arg5 - the fifth arg
 *  Out Parms:     None
 *  Returns:       None
 **********************************************************************/
extern void SiwFuncTrace(const char *function,
                         U32_t arg1,
                         U32_t arg2,
                         U32_t arg3,
                         U32_t arg4,
                         U32_t arg5);

/*******************************************************************************
 *  Macro:       SIW_DEBUG_TRACE()
 *  Description: This macro is used to log a trace message.
 *
 *  In Parms:    module - module identifier in uppercase
 *               function - function name
 *               trcId - trace ID
 *               trace_level - the level of Debug Trace
 *               arg1 - the first arg
 *               arg2 - the second arg
 *               arg3 - the third arg
 *               arg4 - the fourth arg
 *               arg5 - the fifth arg
 **********************************************************************/
#define SIW_DEBUG_TRACE(module, function, trcId, trace_level, \
                        arg1, arg2, arg3, arg4, arg5) \
{   register const SIW_TRACE_OBJ_t *modObj = NULL; \
    modObj = &(TRACE_ ## module); \
    if ( modObj->level >= trace_level ) \
    {  \
        SiwDbgTrace(modObj, #function, (U32_t) trcId, \
                    (U32_t)arg1, (U32_t)arg2, (U32_t)arg3, \
                    (U32_t)arg4, (U32_t)arg5); \
    } \
}

/********************************************************************************
 *  Macro:       SIW_FUNC_TRACE()
 *  Description: This macro is used to log a function trace.
 *
 *  In Parms:    module - module identifier in uppercase
 *               function - function name
 *               arg1 - the first arg
 *               arg2 - the second arg
 *               arg3 - the third arg
 *               arg4 - the fourth arg
 *               arg5 - the fifth arg
 **********************************************************************/
#define SIW_FUNC_TRACE(module, function, arg1, arg2, arg3, arg4, arg5) \
{   register const SIW_TRACE_OBJ_t *modObj = NULL; \
    modObj = &(TRACE_ ## module); \
    if ( modObj->funcTrace == TRUE ) \
    {  \
        SiwFuncTrace(#function, (U32_t)arg1, (U32_t)arg2, (U32_t)arg3, \
                     (U32_t)arg4, (U32_t)arg5); \
    } \
}

#ifdef DEFINE_TRACE
/***********************************************************************
 *  Macro:       SIW_MODULE_DEF()
 *  Description: This macro defines a Module object for Trace.
 *
 *  In Parms:    module -  module identifier in uppercase
 *               level - the level of Debug Trace
 *               printFlag - flag to enable trace printing on screen
 **********************************************************************/
#define SIW_MODULE_DEF(module, level) \
SIW_TRACE_OBJ_t TRACE_ ## module = \
          { #module, level, 0, TRUE }

#else
#define SIW_MODULE_DEF(module, level)  \
extern SIW_TRACE_OBJ_t TRACE_ ## module;
#endif  /* DEFINE_TRACE */

extern SIW_TRACE_ENTRY_t    siwTrcBuf[SIW_NUM_TRACE_BUFS][SIW_NUM_TRACE_ENTRIES];
extern SIW_TRACE_CONTROL_t  siwTrcBufCntl[SIW_NUM_TRACE_BUFS];
extern SIW_TRACE_ENTRY_t    *siwTrcBufPtr;
extern SIW_TRACE_CONTROL_t  *siwTrcCntlPtr;
extern BOOL_t               siwTraceInit;
extern U8_t                 siwTrcBufIndex;

/***********************************************************************
 *  Function:     TraceDump()
 *  Description:  This function dumps all Trace records from trace data
 *                buffer to IDE drive.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 **********************************************************************/
extern STATUS_t TraceDump(char *);

/***********************************************************************
 *  Function:     TraceDebugSet
 *  Description:  This function sets the trace debugging capability
 *                for a module.
 *
 *  In Parms:     char *modNamePtr - point to the module name
 *                U8_t level - trace level
 *                U8_t printFlag - printing flag
 *                BOOL_t funcTrace - functional trace
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 **********************************************************************/
extern STATUS_t TraceDebugSet(IN char *modNamePtr,
                              IN U8_t level,
                              IN U8_t printFlag,
                              IN BOOL_t funcTrace);

#endif /* _SIW_TRACE_H_ */
