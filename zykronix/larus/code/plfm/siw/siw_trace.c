/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_trace.c
 *
 *System Interface Wrapper code for Trace
 *
 *Note:
 *
 *Author(s):
 * 
 */

#include <intLib.h>
#include <symLib.h>
#include <sysSymTbl.h>
#include <stdio.h>
#include <ioLib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <msgQLib.h>

#include <axss_types.h>
#include <axss_if.h>
#include <siw_events.h>
#include <siw_trace.h>
#include <sys_if.h>
#include <fm_if.h>

#define TRACE_DIRECTORY        "C:"
#define TRACE_FILE             "TRACE.REC"
#define TRACE_MODULES          32
#define NUM_TRACE_MODULES      70
#define TRACE_MODULE_SIZE      12

char traceModuleTable[NUM_TRACE_MODULES][TRACE_MODULE_SIZE] = {
 "SYS", "SIW", "MRPC", "FM",  "ID",     "CM",  "SM",
 "WEB", "CRM", "DS1",  "PRC",  "PCC",   "PEC", "TCG",
 "DBM", "CSM", "PRC",  "MSCP", "ALARM", "ASX", "BIM",
 "HWPM", "TRAP", "PEC", "LOG", "DS1", "TCG", "RMG", 
 "EMM", "NWG", "TRM", "SPATH", "OC3Q", "CBT", "DSX",
 "DMC", "MPC", "ALBM", "FDRM", "COMET", "BDS1", "D3MX",
 "SONET", "OND", "LD", "PLFMLIB", "OC3",   /* traceModuleIndex = 47 */
};  

int                  traceModuleIndex = 47;
SIW_TRACE_ENTRY_t    siwTrcBuf[SIW_NUM_TRACE_BUFS][SIW_NUM_TRACE_ENTRIES];
SIW_TRACE_CONTROL_t  siwTrcBufCntl[SIW_NUM_TRACE_BUFS];
SIW_TRACE_ENTRY_t   *siwTrcBufPtr = NULL;
SIW_TRACE_CONTROL_t *siwTrcCntlPtr = NULL;
BOOL_t               siwTraceInit = FALSE;
U8_t                 siwTrcBufIndex = 0;
SIW_TRACE_OBJ_t     *trcModuleTable[TRACE_MODULES];
BOOL_t               siwTrcEnable = FALSE;
MSG_Q_ID             siwTrcMsgQ = NULL;
U32_t                trcModuleTableIndex = 0;
S32_t                siwTrcTask = 0;
U32_t                siwTrcMsgLost = 0;
U32_t                siwTrcCnt = 0;
BOOL_t               siwTraceDebug = TRUE;
BOOL_t               siwFuncTrace = TRUE;

/*************************************************************
 *  Function ptototype
 *************************************************************/
STATUS_t DbgTrcTask(void);
STATUS_t SiwTracePrint(void);
STATUS_t TracePrint(char *);
STATUS_t TraceDebugShow(void);
STATUS_t TraceDebugOff(void);
void     SiwFuncTraceEnable(void);
void     SiwFuncTraceDisable(void);
void     SiwAddModuleName(char *module);
void     SiwDebugTraceDisable(void);

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
 *  Out Parms:
 *  Returns:
 **********************************************************************/
void
SiwDbgTrace(const SIW_TRACE_OBJ_t *modObj, const char *function, U32_t trcId,
         U32_t arg1, U32_t arg2, U32_t arg3, U32_t arg4, U32_t arg5)
{
    U32_t             lockKey;
    SIW_TRACE_ENTRY_t *pTrcEnt = siwTrcBufPtr;
    int               i;
    BOOL_t            endString = FALSE;

    if (!siwTraceDebug)
    {
        return;
    }

    /* Lock interrupts */
    lockKey = intLock();

    /* Update Trace Entry */
    /* save trace count */
#if 0
    pTrcEnt->traceCount = siwTrcCnt;
#else
#ifndef SIM
    pTrcEnt->traceCount = CYCLES();
#else
    pTrcEnt->traceCount = 0;
#endif
#endif

    /* save Module name */
    for (i=0; i<TRACE_MODULE_NAME_SIZE-1; i++)
    {
        if (*(modObj->module + i) == '\0')
        {
            pTrcEnt->module[i] = ' ';
        }
        else
        {
            pTrcEnt->module[i] = *(modObj->module + i);
        }
    }
    pTrcEnt->module[i] = '\0';

    /* save Function name */
    for (i=0; i<TRACE_FUNCTION_NAME_SIZE-1; i++)
    {
        if (*(function + i) == '\0')
        {
            pTrcEnt->function[i] = ' ';
            endString = TRUE;
        }
        else
        {
            if (!endString)
            {
                pTrcEnt->function[i] = *(function + i);
            }
            else
            {
                pTrcEnt->function[i] = ' ';
            }
        }
    }
    pTrcEnt->function[i] = '\0';

    /* save trace ID */
    pTrcEnt->trcId = trcId;

    /* save parameters */
    pTrcEnt->arg1 = arg1;
    pTrcEnt->arg2 = arg2;
    pTrcEnt->arg3 = arg3;
    pTrcEnt->arg4 = arg4;
    pTrcEnt->arg5 = arg5;

    /* increment buffer header and pointer */
    siwTrcCntlPtr->head++;
    siwTrcBufPtr++;
    siwTrcCnt++;

    /* Check if need to wrap */
    if (siwTrcCntlPtr->head >= SIW_NUM_TRACE_ENTRIES)
    {
        siwTrcCntlPtr->wrapped = TRUE;
        siwTrcCntlPtr->head = 0;
        siwTrcCnt = 0;
        siwTrcBufPtr = &siwTrcBuf[siwTrcBufIndex][0];
    }
    if (siwTrcCntlPtr->wrapped)
    {
        siwTrcCntlPtr->tail++;
        if (siwTrcCntlPtr->tail >= SIW_NUM_TRACE_ENTRIES)
        {
            siwTrcCntlPtr->tail = 0;
        }
    }

    /* Check if print flag set */
    if ( modObj->printflag && siwTrcEnable == TRUE ) 
    {
        if ( msgQSend(siwTrcMsgQ, (char *)pTrcEnt, sizeof(SIW_TRACE_ENTRY_t),
                     NO_WAIT, MSG_PRI_NORMAL) == ERROR )
        {
            siwTrcMsgLost++;
        }
    }

    /* Unlock interrupts */
    intUnlock(lockKey);
}

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
void SiwFuncTrace(const char *function, U32_t arg1, U32_t arg2,
                  U32_t arg3, U32_t arg4, U32_t arg5)
{
    U32_t             lockKey;
    SIW_TRACE_ENTRY_t *pTrcEnt = siwTrcBufPtr;
    int               i;
    BOOL_t            endString = FALSE;

    if ((siwFuncTrace != TRUE) || (pTrcEnt == (SIW_TRACE_ENTRY_t *)NULL))
    {
        return;
    }

    /* Lock interrupts */
    lockKey = intLock();

    /* Update Trace Entry */
    /* save trace count */
#if 0
    pTrcEnt->traceCount = siwTrcCnt;
#else
#ifndef SIM
    pTrcEnt->traceCount = CYCLES();
#else
    pTrcEnt->traceCount = 0;
#endif
#endif

    /* save module name */
    strcpy(pTrcEnt->module, "FUN");

    /* save Function name */
    for (i=0; i<TRACE_FUNCTION_NAME_SIZE-1; i++)
    {
        if (*(function + i) == '\0')
        {
            pTrcEnt->function[i] = ' ';
            endString = TRUE;
        }
        else
        {
            if (!endString)
            {
                pTrcEnt->function[i] = *(function + i);
            }
            else
            {
                pTrcEnt->function[i] = ' ';
            }
        }
    }
    pTrcEnt->function[i] = '\0';

    /* save parameters */
    pTrcEnt->arg1 = arg1;
    pTrcEnt->arg2 = arg2;
    pTrcEnt->arg3 = arg3;
    pTrcEnt->arg4 = arg4;
    pTrcEnt->arg5 = arg5;

    /* increment buffer header and pointer */
    siwTrcCntlPtr->head++;
    siwTrcBufPtr++;
    siwTrcCnt++;

    /* Check if need to wrap */
    if (siwTrcCntlPtr->head >= SIW_NUM_TRACE_ENTRIES)
    {
        siwTrcCntlPtr->wrapped = TRUE;
        siwTrcCntlPtr->head = 0;
        siwTrcCnt = 0;
        siwTrcBufPtr = &siwTrcBuf[siwTrcBufIndex][0];
    }
    if (siwTrcCntlPtr->wrapped)
    {
        siwTrcCntlPtr->tail++;
        if (siwTrcCntlPtr->tail >= SIW_NUM_TRACE_ENTRIES)
        {
            siwTrcCntlPtr->tail = 0;
        }
    }

    /* Unlock interrupts */
    intUnlock(lockKey);
}

/***********************************************************************
 *  Function:     TraceDebugdbgSet
 *  Description:  This function sets the trace debugging capability
 *                for a module.
 *
 *  In Parms:     char *modNamePtr - point to the module name
 *                U8_t level - trace level
 *                U8_t printFlag - printing flag
 *                U8_t funcTrace - functional trace
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 **********************************************************************/
STATUS_t
TraceDebugSet(char *modNamePtr, U8_t level, U8_t printFlag, BOOL_t funcTrace)
{
    STATUS_t        rc;
    SIW_TRACE_OBJ_t *pModObj;
    char            symName[80];
    SYM_TYPE        symType;

    /* Validate level value */
    if ( level > TRACE_LEVEL_5 ) 
    {
        printf("\nLevel specified exceeds TRACE_LEVEL_5: %d\n", 
                TRACE_LEVEL_5);
        return(ERROR);
    }

    /* Validate printFlag */
    if ( printFlag > 1 ) 
    {
        printf("\nPrint flag must be zero or one.\n");
        return(ERROR);
    } 

    /* Locate module object */
    sprintf(symName, "TRACE_%s", modNamePtr);

    rc = symFindByName(sysSymTbl, symName, (char **)&pModObj, &symType);
    if ( rc == ERROR ) 
    {
        printf("\nUnable to locate module object for '%s'\n", modNamePtr);
        return(ERROR);
    }
    
    /* Set module object level and printflag */
    pModObj->level = level;
    pModObj->printflag = printFlag;
    pModObj->funcTrace = funcTrace;

    /* add to trace module table if printFlag is set */
    trcModuleTable[trcModuleTableIndex++] = pModObj;

    /* Return OK status */
    return(OK);
}

/***********************************************************************
 *  Function:     TraceDebugdbShowg
 *  Description:  This function turns on realtime displaying trace
 *                message on the screen.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 **********************************************************************/
STATUS_t
TraceDebugShow()
{
    if ( siwTrcEnable == FALSE )
    {
        /* Create message queue */
        siwTrcMsgQ = msgQCreate(1000, sizeof(SIW_TRACE_ENTRY_t), MSG_Q_FIFO);
        if ( siwTrcMsgQ == NULL )
        {
            printf("\nCreation of Trace Message Queue failed. Errno=%x\n",
                   errno);
            return(ERROR);
        }

        /* Create Trace task */
        siwTrcTask = taskSpawn("tDbgTrc", 2, 0, 0x1000, DbgTrcTask, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0);
        if ( siwTrcTask == ERROR )
        {
            printf("\nCreation of Debug Trace Task failed. Errno=%x\n",
                   errno);
            siwTrcTask = 0;
            msgQDelete(siwTrcMsgQ);
            siwTrcMsgQ = NULL;
            return(ERROR);
        }
    }

    /* Set Trace enable */
    siwTrcEnable = TRUE;
    siwTrcMsgLost = 0;

    /* Return OK status */
    return(OK);
}

/***********************************************************************
 *  Function:     TraceDebugOff()
 *  Description:  This function turns off the realtime trace message
 *                displaying on the screen.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 **********************************************************************/
STATUS_t
TraceDebugOff()
{
    U32_t i;

    /* Set Trace Enable Off */
    siwTrcEnable = FALSE;

    /* Delete Trace Task */
    if ( siwTrcTask != 0 ) 
    {
        taskDelete(siwTrcTask);
        siwTrcTask = 0;
    }

    /* Delete Message Queue */
    if ( siwTrcMsgQ != NULL ) 
    {
        msgQDelete(siwTrcMsgQ);
        siwTrcMsgQ = NULL;
    }

    /* turn off printflag for each module */
    for (i = 0; i < trcModuleTableIndex; i++)
    {
        trcModuleTable[i]->printflag = 0;
    }

    /* reset table index */
    trcModuleTableIndex = 0;

    return (OK);
}

/***********************************************************************
 *  Function:      DbgTrcTask
 *  Description:   This function displays a trace message on screen
 *                 after received.
 *
 *  In Parms:      None
 *  Out Parms:     None
 *  Returns:       ERROR
 **********************************************************************/
STATUS_t
DbgTrcTask()
{
    U32_t              len;
    SIW_TRACE_ENTRY_t  msgBuf;

    /* Loop forever */
    for (;;) 
    {
        len = msgQReceive(siwTrcMsgQ, (char *)&msgBuf, sizeof(msgBuf),
                          WAIT_FOREVER);

        if ( len == sizeof(SIW_TRACE_ENTRY_t) ) 
        {
            /* SiwTraceDbgPrint(&msgBuf); */
            printf("%8x %s-%s-%u %8x %8x %8x %8x %8x\n", 
                msgBuf.traceCount, msgBuf.module,
                msgBuf.function, msgBuf.trcId, msgBuf.arg1, msgBuf.arg2,
                msgBuf.arg3, msgBuf.arg4, msgBuf.arg5);

        }
        else 
        {
            printf("\nTrace Task received error. Errno=%x\n", errno);
            taskDelay(10);
        }
    }
  
    return (ERROR);
}

/***********************************************************************
 *  Function:     SiwTracePrint()
 *  Description:  This function shows all Trace records on screen from
 *                trace data buffer.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 **********************************************************************/
STATUS_t
SiwTracePrint()
{
    SIW_TRACE_ENTRY_t *pTrcEnt = siwTrcBufPtr - 1;
    int               lineNum = 0;
    U16_t             head, tail, i;

    /* check if there is any records saved */
    if (siwTrcCntlPtr->head == siwTrcCntlPtr->tail)
    {
        /* there is no any Trace records saved */
        return (ERROR);
    }

    /* setup header and tailer */
    head = siwTrcCntlPtr->head - 1;
    tail = siwTrcCntlPtr->tail;

    /* clear screen */
    ClrScrn();

    while (head != tail)
    {
        if (strcmp(pTrcEnt->module, "FUN") == 0)
        {
            printf("%8.8x ----%16.16s       %8.8x %8.8x %8.8x %8.8x %8.8x\n",
                    pTrcEnt->traceCount, pTrcEnt->function, 
                    pTrcEnt->arg1, pTrcEnt->arg2,
                    pTrcEnt->arg3, pTrcEnt->arg4, pTrcEnt->arg5);
        }
        else
        {
            printf("%8.8x %3.3s-%16.16s-%5.5x %8.8x %8.8x %8.8x %8.8x %8.8x\n", 
                    pTrcEnt->traceCount, pTrcEnt->module,
                    pTrcEnt->function, pTrcEnt->trcId, pTrcEnt->arg1, pTrcEnt->arg2,
                    pTrcEnt->arg3, pTrcEnt->arg4, pTrcEnt->arg5);
        }

        if (CheckScroll(&lineNum, MAX_NUM_LINES) == QUIT)
        {
            return (OK);
        }
 
        if (head == 0)
        {
            head = SIW_NUM_TRACE_ENTRIES - 1;
            pTrcEnt = &siwTrcBuf[siwTrcBufIndex][SIW_NUM_TRACE_ENTRIES - 1];
        }
        else
        {
            head--;
            pTrcEnt--;
        }
    }    

    for (i=0; i<2; i++)
    {
        if (strcmp(pTrcEnt->module, "FUN") == 0)
        {
            printf("%8.8x ----%16.16s       %8.8x %8.8x %8.8x %8.8x %8.8x\n",
                    pTrcEnt->traceCount, pTrcEnt->function,
                    pTrcEnt->arg1, pTrcEnt->arg2,
                    pTrcEnt->arg3, pTrcEnt->arg4, pTrcEnt->arg5);
        }
        else
        {
            printf("%8.8x %3.3s-%16.16s-%5.5x %8.8x %8.8x %8.8x %8.8x %8.8x\n", 
                    pTrcEnt->traceCount, pTrcEnt->module,
                    pTrcEnt->function, pTrcEnt->trcId, pTrcEnt->arg1, pTrcEnt->arg2,
                    pTrcEnt->arg3, pTrcEnt->arg4, pTrcEnt->arg5);
        }
        pTrcEnt--;
    }
 
    return (OK);
}

/***********************************************************************
 *  Function:     TraceDump()
 *  Description:  This function dumps all Trace records from trace data
 *                buffer to IDE drive.
 *
 *  In Parms:     char *filename - the name of the file to save the 
 *                                 trace records
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 **********************************************************************/
STATUS_t
TraceDump(char *filename)
{
    DIR *               dd = NULL;
    U32_t               lockKey;
    int                 fd;
    int                 i;
    char                dstFname[TRACE_FILENAME_SIZE];
    char                srcFname[TRACE_FILENAME_SIZE];
    STATUS_t            status;
    SIW_TRACE_ENTRY_t   *strBuf = &siwTrcBuf[siwTrcBufIndex][0];
    SIW_TRACE_CONTROL_t *cntlPtr = &siwTrcBufCntl[siwTrcBufIndex];

    /* get the destination filename */
    memset(dstFname, 0, sizeof(dstFname)); 
    strcpy(dstFname, TRACE_DIRECTORY);
    i = strlen(TRACE_DIRECTORY);
    strcat(dstFname, "/S");
    sprintf(dstFname+i+2, "%d_", SiwMySlotNo());
    if (filename == NULL)
    {
        strcat(dstFname, TRACE_FILE); 
    }
    else
    {
        strcat(dstFname, filename);
    }

    /* get the source filename */
    strcpy(srcFname, siwTrcBufCntl[siwTrcBufIndex].fname);

    /* Lock interrupts */
    lockKey = intLock();

    /* switch trace buffer */
    if (siwTrcBufIndex == (SIW_NUM_TRACE_BUFS - 1))
    {
        siwTrcBufIndex = 0;
    }
    else
    {
        siwTrcBufIndex = SIW_NUM_TRACE_BUFS - 1;
    }
    
    /* Initialize Trace Buffer */
    siwTrcBufCntl[siwTrcBufIndex].head = 0;
    siwTrcBufCntl[siwTrcBufIndex].tail = 0;
    siwTrcBufCntl[siwTrcBufIndex].wrapped = FALSE;
    siwTrcBufPtr = &siwTrcBuf[siwTrcBufIndex][0];
    siwTrcCntlPtr = &siwTrcBufCntl[siwTrcBufIndex];
    siwTrcCnt = 0;
   
    /* Unlock interrupts */
    intUnlock(lockKey);

    /* save trace buffer to IDE drive */
#if 1  
    /* make sure the trace directory exists */
    if ((dd = opendir(TRACE_DIRECTORY)) == NULL)
        status = mkdir(TRACE_DIRECTORY);
    else
        closedir(dd);

    if ((fd = creat(dstFname, 0644)) == ERROR)
    {
        return ERROR;
    }
    close (fd);

    if ((fd = open(dstFname, O_WRONLY, 0644)) != ERROR)
    {
        /* save the control block */
        if (write(fd, (char *)cntlPtr, sizeof(SIW_TRACE_CONTROL_t)) == ERROR)
        {
            close(fd);
            printf("Failed to save Trace Buffer Control Block\n");
            return ERROR;
        }

        /* save the trace buffer */
        if (write(fd, (char *)strBuf, 
                  sizeof(SIW_TRACE_ENTRY_t) * SIW_NUM_TRACE_ENTRIES) == ERROR)
        {
            close(fd);
            printf("Failed to save Trace Buffer\n");
            return ERROR;
        }
        close(fd);
    }
#else
    /* dumping trace buffer into the IDE drive */
    
#endif

    return (OK);
}

/***********************************************************************
 *  Function:     TracePrint()
 *  Description:  This function shows all Trace records saved in IDE
 *                onto screen.
 *
 *  In Parms:     char *filename - Trace file name
 *  Out Parms:    None
 *  Returns:      OK or ERROR
 **********************************************************************/
STATUS_t
TracePrint(char *filename)
{
    DIR *               dd = NULL;
    SIW_TRACE_CONTROL_t cntlBlk;
    SIW_TRACE_ENTRY_t   trcEnt;
    S32_t               len;
    int                 fd, i, offset; 
    int                 traceRecSize = sizeof(SIW_TRACE_ENTRY_t);
    char                fname[TRACE_FILENAME_SIZE];

    /* get the filename */
    memset(fname, 0, sizeof(fname)); 
    strcpy(fname, TRACE_DIRECTORY);
    i = strlen(TRACE_DIRECTORY);
    strcat(fname, "/S");
    sprintf(fname+i+2, "%d_", SiwMySlotNo());

    if (filename == NULL)
    {
        strcat(fname, TRACE_FILE); 
    }
    else
    {
        strcat(fname, filename);
    }

    /* make sure the trace directory exists */
    if ((dd = opendir(TRACE_DIRECTORY)) == NULL)
    {
        return (ERROR);
    }
    else
    {
        closedir(dd);
    }

    if ((fd = open(fname, O_RDONLY, 0644)) == ERROR)
    {
        return (ERROR);
    }

    /* read Trace Buffer Control Block */
    len = read(fd, (char *)&cntlBlk, sizeof(SIW_TRACE_CONTROL_t));
    if (len != sizeof(SIW_TRACE_CONTROL_t))
    {
        close (fd);
        return (ERROR);
    }
    
    /* clear screen */
    ClrScrn();

    cntlBlk.head--;

    while (cntlBlk.head != cntlBlk.tail)
    {
        offset = traceRecSize * cntlBlk.head + sizeof(SIW_TRACE_CONTROL_t);
        lseek(fd, offset, SEEK_SET);           
        len = read(fd, (char *)&trcEnt, traceRecSize);
        if (len != traceRecSize)
        {
            close(fd);
            return (ERROR);
        }
        if (strcmp(trcEnt.module, "FUN") == 0)
        {
            printf("%8.8x ----%16.16s       %8.8x %8.8x %8.8x %8.8x %8.8x\n",
                    trcEnt.traceCount, trcEnt.function, 
                    trcEnt.arg1, trcEnt.arg2,
                    trcEnt.arg3, trcEnt.arg4, trcEnt.arg5);
        }
        else
        {
            printf("%8.8x %3.3s-%16.16s-%5.5x %8.8x %8.8x %8.8x %8.8x %8.8x\n", 
                    trcEnt.traceCount, trcEnt.module,
                    trcEnt.function, trcEnt.trcId, trcEnt.arg1, trcEnt.arg2,
                    trcEnt.arg3, trcEnt.arg4, trcEnt.arg5);
        }

        if (cntlBlk.head == 0)
        {
            cntlBlk.head = SIW_NUM_TRACE_ENTRIES - 1;
        }
        else
        {
            cntlBlk.head--;
        }
    }

    for (i=0; i<2; i++)
    {
        offset = traceRecSize * cntlBlk.head + sizeof(SIW_TRACE_CONTROL_t);
        lseek(fd, offset, SEEK_SET);
        len = read(fd, (char *)&trcEnt, traceRecSize);
        if (len != traceRecSize)
        {
            close(fd);
            return (ERROR);
        }
        if (strcmp(trcEnt.module, "FUN") == 0)
        {
            printf("%8.8x ----%16.16s       %8.8x %8.8x %8.8x %8.8x %8.8x\n",
                    trcEnt.traceCount, trcEnt.function, 
                    trcEnt.arg1, trcEnt.arg2,
                    trcEnt.arg3, trcEnt.arg4, trcEnt.arg5);
        }
        else
        {
            printf("%8.8x %3.3s-%16.16s-%5.5x %8.8x %8.8x %8.8x %8.8x %8.8x\n", 
                    trcEnt.traceCount, trcEnt.module,
                    trcEnt.function, trcEnt.trcId, trcEnt.arg1, trcEnt.arg2,
                    trcEnt.arg3, trcEnt.arg4, trcEnt.arg5);
        }
        cntlBlk.head--;
    }
    close (fd);
    return (OK);
}

/***********************************************************************
 *  Function:     SiwFuncTraceDisable()
 *  Description:  This function disables the SIW_FUNC_TRACE().
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 **********************************************************************/
void
SiwFuncTraceDisable()
{
    SIW_TRACE_OBJ_t *pModObj;
    char            symName[80];
    SYM_TYPE        symType;
    STATUS_t        rc;
    int             i;

    for (i=0; i<traceModuleIndex; i++)
    {
        /* Locate module object */
        sprintf(symName, "TRACE_%s", traceModuleTable[i]);

        /* retrieve Module Object from symbol table */
        rc = symFindByName(sysSymTbl, symName, (char **)&pModObj, &symType);
        if (rc != OK)
        {
            continue;
        }
     
        /* disable Function Trace for the module */
        pModObj->funcTrace = FALSE;
    }
}


/***********************************************************************
 *  Function:     SiwFuncTraceEnable()
 *  Description:  This function enables the SIW_FUNC_TRACE().
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 **********************************************************************/
void
SiwFuncTraceEnable()
{
    SIW_TRACE_OBJ_t *pModObj;
    char            symName[80];
    SYM_TYPE        symType;
    STATUS_t        rc;
    int             i;

    for (i=0; i<traceModuleIndex; i++)
    {
        /* Locate module object */
        sprintf(symName, "TRACE_%s", traceModuleTable[i]);

        /* retrieve Module Object from symbol table */
        rc = symFindByName(sysSymTbl, symName, (char **)&pModObj, &symType);
        if (rc != OK)
        {
            continue;
        }
     
        /* disable Function Trace for the module */
        pModObj->funcTrace = TRUE;
    }
}

/***********************************************************************
 *  Function:     SiwDebugTraceDisable()
 *  Description:  This function disables the SIW_DEBUG_TRACE().
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 **********************************************************************/
void
SiwDebugTraceDisable()
{
    SIW_TRACE_OBJ_t *pModObj;
    char            symName[80];
    SYM_TYPE        symType;
    STATUS_t        rc;
    int             i;

    for (i=0; i<traceModuleIndex; i++)
    {
        /* Locate module object */
        sprintf(symName, "TRACE_%s", traceModuleTable[i]);

        /* retrieve Module Object from symbol table */
        rc = symFindByName(sysSymTbl, symName, (char **)&pModObj, &symType);
        if (rc != OK)
        {
            continue;
        }
     
        /* disable Function Trace for the module */
        pModObj->level = 0;
    }
}


/***********************************************************************
 *  Function:     SiwAddModuleName()
 *  Description:  This function enables the SIW_FUNC_TRACE().
 *
 *  In Parms:     char *module - module name
 *  Out Parms:    None
 *  Returns:      None
 **********************************************************************/
void
SiwAddModuleName(char *module)
{
    if (module == NULL)
    {
        return; 
    }
    if (strlen(module) >= TRACE_MODULE_SIZE)
    {
        return; 
    }

    strcpy(traceModuleTable[traceModuleIndex++], module);
}

void
SiwModuleShow()
{
    SIW_TRACE_OBJ_t *pModObj;
    char            symName[80];
    SYM_TYPE        symType;
    STATUS_t        rc;
    int             i;

    for (i=0; i<traceModuleIndex; i++)
    {
        /* Locate module object */
        sprintf(symName, "TRACE_%s", traceModuleTable[i]);
        printf("%d -- %s: ", i+1, symName);
        /* retrieve Module Object from symbol table */
        rc = symFindByName(sysSymTbl, symName, (char **)&pModObj, &symType);
        if (rc != OK)
        {
            printf("Not Found\n");
            continue;
        }

        if (pModObj->funcTrace)
        {
            printf("TRUE\n");
        }
        else
        {
            printf("FALSE\n");
        }
    }
}

