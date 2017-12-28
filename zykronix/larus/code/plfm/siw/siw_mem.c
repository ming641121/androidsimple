/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_mem.c
 *
 *System Interface Wrapper code for Memory Resources
 *
 *Note:
 *
 *Author(s):
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <memLib.h>
#include <intLib.h>
#include <errnoLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <axss_types.h>
#include <sys_memmap.h>
#include <sys_os.h>
#include <sys_if.h>
#include <siw_if.h>
#include "private/memPartLibP.h"
#include "siw_trace.h"
#include "siw_event.h"

/* 4 byte alignment for now */
#define SIW_MEM_MISALIGNED(x)   ((U32_t)(x) & 3)

#define MEM_TIMEOUT           1

/* Since the dynamic memory area is now in user reserved mem, I've moved all */
/* the defines related to it's size and number to the sys_memmap.h file. */
 
#define MEM_FREE              0
#define MEM_USED              1
#define STATIC_MAGIC_NUM      0x054a4053
#define SIW_EVENT_POOL_ID     NUM_DYN_POOLS - 1
#define SIW_EVENT_SIZE        99999

typedef struct {
 U32_t  bufSize;
 U32_t  bufNum;
 U32_t  memStartAddr;
 U32_t  memEndAddr;
 U16_t  threshold;  /* for dynamic memory threshold event */
 U16_t  triggered;  /* for dynamic memory threshold event */
 U16_t  high;       /* dynamic memory high water mark */
 U32_t **freeList;  /* free memory list */
 SEM_ID freeSem;    /* counting semaphore for free memory list */
 U32_t  freeHead;   /* index to head of free memory list */
 U32_t  freeTail;   /* index to tail of free memory list */
 SIW_TID_t failTask; /* ID of first task failing to allocate memory */
} DYN_MEM_POOL_t;

int    staticBufAllocCount = 0;
int    staticBufFreeCount = 0;
int    siwEventDynMemBlks = 0;
BOOL_t staticMemDebugOn = FALSE;

/* A place mark to show how much of the tasks trace cells are being used */
int lastUsedCell = 0;
/* An arrary listing the indexes of the most recently used trace cells */
int recentlyUsedCells [5] = { 0, 0, 0, 0, 0 };
#define RECENTLYUSEDCELLS_SIZE 5
#define DRAM_START_ADDR        0x10000

static char *staticMemStart = NULL;
static char *staticMemEnd = NULL;

extern BOOL_t trapDebug;
/*
#ifndef SIM
extern PART_ID isoMemPartId;
extern PART_ID dosFsMemPartId;
#endif
*/

/***********************************************************************
 *  Local static function declaration
 **********************************************************************/
void SiwMemDebugOn(int);
void SiwMemDebugOff(int);
void SiwStaticMemTrace(int);
void memTraceAdd (SIW_MEM_TYPE_t type, U32_t size, int taskId, U32_t poolId);
void memTraceFree (SIW_MEM_TYPE_t type, U32_t size, int taskId, U32_t poolId);

/********************************************************************
 * Function:     SiwAllocMem()                                      
 * Description:  This function allocates memory from the appropriate 
 *               memory partition based on size and memory type.    
 *               Memory type specifies the memory block will be     
 *               allocated from which memory pool.  A timeout       
 *               specifies a number of clock in ticks to wait till 
 *               the memory becomes available to be allocated.          
 *                                                                  
 * In Parms:     SIW_MEM_TYPE_t memType  -  type of memory allocation 
 *               U32_t          size     -  size of memory block to   
 *                                          allocate                  
 *               S32_t          timeout  -  clock ticks to wait for  
 *                                          memory                    
 * Out Parms:    None                                               
 * Returns:      A valid memory block pointer or NULL if failure    
 ********************************************************************/
void *
SiwAllocMem(SIW_MEM_TYPE_t memType,
            U32_t          size,
            S32_t          timeout)
{
    void   *memPtr = NULL;
    S32_t  elapseTime = timeout;
    SIW_MEM_HEADER_t *siwMemPtr;
    BLOCK_HDR *blockHeader;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwAllocMem, memType, size, timeout, 0, 0);

    switch (memType) {
      /* get memory block from system memory partition */
      case SIW_STATIC: 
      case SIW_EVENT:
          size = SIW_EVENT_SIZE;
      case SIW_TMR:
      case SIW_DYNAMIC:

		if (size == 0)
        {
            memPtr = NULL;
            break;
        }

        for (;;)
        {
            /* alloc a memory from system memory partition */
            if ((memPtr = (void *)malloc(size + sizeof(SIW_MEM_HEADER_t))) != NULL)
            {
                /* Grab the memory block header */
                blockHeader = (BLOCK_HDR*)memPtr - 1;

                if ((U32_t)memPtr < (U32_t)staticMemStart)
                {
                    staticMemStart = memPtr;
                }

                staticBufAllocCount++;
                if (staticMemDebugOn)
                {
                    printf("Task(%s) allocate Static Memory Block(%d): %p\n",
                         SiwTaskName(SiwTaskIdSelf()), staticBufAllocCount, memPtr);
                }

                /* Grab the SIW mem header */
                siwMemPtr = (SIW_MEM_HEADER_t *)memPtr;

                /* save the memory type */
                siwMemPtr->memType = memType;

                /* save the magic number */
                siwMemPtr->poolId = 'J';
                siwMemPtr->memBlockId = '@';
                siwMemPtr->status = 'S';

                /* save the starting memory address for returning to caller */
                siwMemPtr->memPtr = (U32_t *)(memPtr + sizeof(SIW_MEM_HEADER_t));

                /* save the calling task ID */
                siwMemPtr->taskId = taskIdSelf();

                /* move memory pointer to the begining of user data area */
                memPtr += sizeof(SIW_MEM_HEADER_t);

                /* Add statistics based on this task using the system level allocated size*/
                memTraceAdd (SIW_STATIC, (U32_t)blockHeader->nWords * 2, siwMemPtr->taskId, 0);
                break;
            }
            else if (timeout == WAIT_FOREVER)
            {
                taskDelay(MEM_TIMEOUT);
            }
            else if (elapseTime > 0)
            {
                elapseTime -= MEM_TIMEOUT;
                taskDelay(MEM_TIMEOUT);
            }
            else
            {
                /* Log error event */
                SiwEventLog(NULL, SIW_EVENT_SYM(SIW, NOSTATICMEM), SiwTaskIdSelf());

                break;
            } 
        }
        break;

      default:
        /* Log error event */
    }

    return(memPtr);
}

/********************************************************************
 * Function:     SiwFreeMem()                                       
 * Description:  This function frees a memory block.  The memory    
 *               is returned to back to the memory pool it was      
 *               previously allocate from by SiwAllocMem().         
 *                                                                  
 * In Parms:     void *memPtr - pointer to memory block to free    
 * Out Parms:    None                                               
 * Returns:      OK or ERROR                                        
 ********************************************************************/
STATUS_t
SiwFreeMem(void *memPtr)
{
    U8_t memType;
    int  lockKey;
    SIW_MEM_HEADER_t *siwMemPtr;
    BLOCK_HDR *blockHeader;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwFreeMem, memPtr, 0, 0, 0, 0);

    /* Grab the SIW mem header */
    siwMemPtr = (SIW_MEM_HEADER_t *)((U32_t)memPtr - sizeof (SIW_MEM_HEADER_t));

    /* allocated? */
    if ( (siwMemPtr->status != MEM_USED) &&
         (siwMemPtr->status != 'S') )
    {
        /* Log error event */
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, WRNGMEMPTR), 
                    "(unallocated)", memPtr, SiwTaskIdSelf());
        return (ERROR);
    }

    lockKey = SiwIntLock();
    /* get the memory type */
    memType = *(U8_t *)((U32_t)memPtr - sizeof(SIW_MEM_HEADER_t));

    switch (memType) {
      /* free memory block to system memory partition */
      case SIW_STATIC:
      case SIW_TMR:
      case SIW_DYNAMIC:
      case SIW_EVENT: 
        if (siwMemPtr->memPtr != (U32_t *)memPtr) 
        {
            /* Log error event */
            SiwEventLog(NULL, SIW_EVENT_SYM(SIW, WRNGMEMPTR), "STATIC", memPtr, SiwTaskIdSelf());
        }
        else
        {
            memPtr = (void *)((U32_t)memPtr - sizeof(SIW_MEM_HEADER_t));
            blockHeader = (BLOCK_HDR*)memPtr - 1;
            siwMemPtr->poolId = 0;
            siwMemPtr->memBlockId = 0;
            siwMemPtr->status = 0;

            staticBufFreeCount++;
            if (staticMemDebugOn)
            {
                printf("Task(%s) Free Static Memory Block(%d): %p\n",
                       SiwTaskName(SiwTaskIdSelf()), staticBufFreeCount, memPtr);
            }
            memTraceFree (SIW_STATIC, (U32_t)blockHeader->nWords * 2, siwMemPtr->taskId, 0);
            free(memPtr);
        }
        break;

      default:
        /* Log error event */        
        SiwEventLog(NULL, SIW_EVENT_SYM(SIW, WRNGMEMPTR), 
                    "UNKNOWN", memPtr, SiwTaskIdSelf());
    }
    SiwIntUnlock(lockKey);

    return(OK);
}

/***********************************************************************
 *  Function:  SiwMemDebugOn()
 *  Description:  This function turns on the memory debeugging 
 *                feature.
 *
 *  In Parms:     int sel - selection to turn which memory trace
 *  Out Parms:    None
 *  Returns:      None
 ************************************************************************/
void SiwMemDebugOn(int sel)
{
    switch (sel) {
      case 1:   /* Static Memory Block */
        staticMemDebugOn = TRUE;
        break;
    }
}

/***********************************************************************
 *  Function:  SiwMemDebugOff()
 *  Description:  This function turns off the memory debeugging 
 *                feature.
 *
 *  In Parms:     int sel - selection to turn which memory trace
 *  Out Parms:    None
 *  Returns:      None
 ************************************************************************/
void SiwMemDebugOff(int sel)
{
    switch (sel) {
      case 1:   /* Static Memory Block */
        staticMemDebugOn = FALSE;
        break;
    }
}

/***********************************************************************
 *  Function:     SiwStaticMemTrace()
 *  Description:  This function shows all allocated memory blocks in  
 *                static memory pool.
 *
 *  In Parms:     int tid - vxWorks Task ID
 *  Out Parms:    None
 *  Returns:      None
 ************************************************************************/
void SiwStaticMemTrace(int tid)
{
    U32_t memStart = (U32_t)staticMemStart;
    int  *magicPtr;
    int  count = 0;
    char buf[200], *cptr;
    int  len;
    int  num = 0;
   

    while (memStart < (U32_t)staticMemEnd)
    {
        magicPtr = (int *)memStart;   
    
        if (*magicPtr == STATIC_MAGIC_NUM)
        {
            SIW_MEM_HEADER_t *memPtr = (SIW_MEM_HEADER_t *)memStart;
            num++;

            if (((unsigned)memPtr->taskId < DRAM_START_ADDR) || 
                ((unsigned)memPtr->taskId > (DRAM_START_ADDR + SysGetDramSize())))
            {
                printf("%d TaskId[0x%8.8x]  MemPtr[0x%8.8x] is corrupted\n",
                        count++, memPtr->taskId, (U32_t)memPtr->memPtr);
                memStart += sizeof(SIW_MEM_HEADER_t);
            }
            else if ((memPtr->taskId == tid) || (tid == 0))
            {
                printf("%d  Task[%10.10s]   MemPtr[0x%8.8x]\n",
                        count++, taskName(memPtr->taskId), (U32_t)memPtr->memPtr);
                memStart += sizeof(SIW_MEM_HEADER_t);
            }
            else
            {
                memStart += sizeof(int);
            }

            if (num >= 30)
            {
                num = 0;

                /* if one screen is not enough */
                do {
                       printf( "\nType <CR> to continue, Q<CR> to stop: " );
                } while ((len = read(STD_IN, buf, sizeof(buf))) == 0);

                buf[len] = 0;
                cptr = &buf[0];
                while( *cptr == ' ')
                {
                    cptr++;
                }

                /* check if quit */
                if( (*cptr ==  'q') || (*cptr == 'Q') )
                {
                    break;
                }
            }
        }
        else
        {
            memStart += sizeof(int);
        }
    }
}

/***********************************************************************
 *  Function:     pauseAndContinue
 *  Description:  This function pauses and waits for a certain keystroke
 *                before returning 1 if the keystroke for quit has been
 *                striked, 0 for anything else
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 ************************************************************************/
int pauseAndContinue ()
{
    int  len;
    char buf[200], *cptr;

    do {
        printf( "\nType <CR> to continue, Q<CR> to stop: " );
    } while ((len = read(STD_IN, buf, sizeof(buf))) == 0);
    
    buf[len] = 0;
    cptr = &buf[0];
    while( *cptr == ' ')
    {
        cptr++;
    }
        
    /* check if quit */
    if( (*cptr ==  'q') || (*cptr == 'Q') )
    {
        return (1);
    }

    return (0);
}

/***********************************************************************
 *  Function:     lsMemShow
 *  Description:  This function shows detailed information about all
 *                memory pools as well as task based memory information.
 *
 *  In Parms:     Level of information: 0 for basic and 1 for expanded
 *                2 for everything
 *  Out Parms:    None
 *  Returns:      None
 ************************************************************************/
STATUS_t
lsMemShow(int level)
{
    return (OK);
}

/***********************************************************************
 *  Function:     lsMemTrace
 *  Description:  This function shows detailed information about all
 *                memory pools as well as task based memory information.
 *                This command displays post mortem information.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      None
 ************************************************************************/
STATUS_t
lsMemTrace()
{
    return (OK);
}

/********************************************************************
 *  Function:     MemTraceCellFind
 *  Description:  This function returns the index to the cell that
 *                matches the taskId
 *  In Parms:     int taskId
 *  Out Parms:    None
 *  Returns:      None
 *********************************************************************/
int memTraceCellFind (int taskId, MEM_TRACE_BUF_t *memTrace)
{
    return (1);
}

/***********************************************************************
 *  Function:     memTraceAdd
 *  Description:  This function saves task based memory information as
 *                given by the inputs into the reserved mem trace area.
 *                Not this only works for SIW_DYNAMIC and SIW_STATIC
 *                memory and only if said task uses SiwAllocMem exclusively
 ************************************************************************/
void memTraceAdd (SIW_MEM_TYPE_t type, U32_t size, int taskId, U32_t poolId)
{
    return;
}

/***********************************************************************
 *  Function:     memTraceFree
 *  Description:  This function saves task based memory information as
 *                given by the inputs into the reserved mem trace area.
 *                Not this only works for SIW_DYNAMIC and SIW_STATIC
 *                memory and only if said task uses SiwAllocMem exclusively
 ************************************************************************/
void memTraceFree (SIW_MEM_TYPE_t type, U32_t size, int taskId, U32_t poolId)
{
}

