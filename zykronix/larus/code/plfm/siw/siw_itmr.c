/*
 * Copyright(C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA
 *
 * Author(s):
 *  
 */

#include "vxWorks.h"
#include "sysLib.h"
#include "intLib.h"
#include "semLib.h"
#include "errnoLib.h"
#include "axss_types.h"
#include "siw_if.h"
#include "sys_os.h"
#include "sys_if.h"
#include "stdio.h"
#include "taskLib.h"
#include "sysSymTbl.h"

/*
 * IMPLEMENTATION NOTES
 *
 * Lists
 *
 *   itimer_array:
 *   Timer nodes are allocated at this address.  To save space, the lists
 *   below are linked not by pointer but by 16-bit itimer_array index.
 *
 *   free_list:
 *   Points to the head of the free list of timer nodes.
 *
 *   delta_list:
 *   Points to the head of the interval timer delay queue.
 *
 * Counters
 *
 *   itmr_stoppedCount:
 *   Number of stopped nodes in delta_list.
 *
 *   itmr_freeSem:
 *   Counting semaphore for free list.
 *
 */


/* defines */

#define ITMR_ID_TO_INDEX(x)     ((x) & 0xffff)
#define ITMR_ID_TO_COOKIE(x)    (((x) >> 16) & 0xffff)
#define ITMR_NULL_NEXT          0xffff
#define ITMR_COOKIE_MIN         0x1
#define ITMR_COOKIE_MAX         0xffff

#define ITMR_COOKIE_CHANGE(x) \
    x = ((x < ITMR_COOKIE_MAX) ? (x + 1) : ITMR_COOKIE_MIN)

/* typedefs */

typedef struct {
FUNC_PTR_t   func;      /* function to be called when timer expires */
void *      parm;       /* parameter to be passed to above function */
U16_t       taskId;     /* SIW task ID */
U16_t       cookie;     /* rolling value (1 to 0xffff) */
U16_t       delta;      /* delay in ticks */
U16_t       next;       /* next node (itimer_array[] index) */
} SIW_ITMR_NODE_t;

/* forward declarations */

LOCAL U16_t SiwINodeShow(SIW_ITMR_NODE_t *node, U16_t sumDelta, S32_t level,
                         SIW_ITMRID_t iTmrId);
void ITmrTask (void);

/* locals */

volatile int    itimer_max = 0;

SIW_ITMR_NODE_t *itimer_array;
SIW_ITMR_NODE_t *delta_list;
SIW_ITMR_NODE_t *free_list;

unsigned        itmr_stoppedCount = 0;  /* # of stopped nodes in delta_list */
SEM_ID          itmr_freeSem;           /* counting semaphore for free list */
SEM_ID          itmr_startSem;          /* mutex semaphore for scheduling */

SEM_ID          semTickBroadcast;       /* XXX */

/********************************************************************
 *  Function:    SiwITmrLibInit()
 *
 *  Description: This routine initializes the SIW interval timer library.
 *               The amount of memory allocated is (maxTimers * 16) bytes.
 *
 *  In Parms:    U32_t maxTimers - maximum number of concurrent timers
 *
 *  Out Parms:   None.
 *
 *  Returns:     OK, or ERROR if an error occurs while allocating
 *               resources, the library has already been initialized,
 *               maxTimers is greater than SIW_ITMR_MAX_TIMER_MAX,
 *               or maxTimers is less than one.
 ********************************************************************/
STATUS_t
SiwITmrLibInit (IN U32_t maxTimers)
{
    unsigned ii;
    int tid;

    if (itimer_max)
    {
        printf("ERROR - already initialized (itimer_max = %d)\n", itimer_max);
        return (ERROR);
    }

    /* check parameter */
    if (!(maxTimers) || (maxTimers > SIW_ITMR_MAX_TIMER_MAX))
    {
        printf("ERROR - parm check (maxTimers = %d)\n", maxTimers);
        return (ERROR);
    }

    /* allocate memory for array of timer nodes */
    itimer_array =  malloc(maxTimers * sizeof(SIW_ITMR_NODE_t));
    if (itimer_array == NULL)
    {
        printf("ERROR - malloc() returned NULL\n");
        /* TODO log error event */
        return (ERROR);
    }

    /* create counting semaphore for managing free list */
    itmr_freeSem = semCCreate(SEM_Q_PRIORITY, maxTimers);
    if (itmr_freeSem == NULL)
    {
        printf("ERROR - semCCreate() returned NULL\n");
        free((void *)itimer_array);
        /* TODO log error event */
        return (ERROR);
    }

    /* create mutual exclusion semaphore for starting timers */
    itmr_startSem = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE);
    if (itmr_startSem == NULL)
    {
        printf("ERROR - semMCreate() returned NULL\n");
        free((void *)itimer_array);
        /* TODO delete itmr_freeSem */
        /* TODO log error event */
        return (ERROR);
    }

    /* create binary semaphore for signaling clock tick */
    semTickBroadcast = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    if (semTickBroadcast == NULL)
    {
        printf("ERROR - semBCreate() returned NULL\n");
        /* TODO delete itmr_freeSem */
        /* TODO delete itmr_startSem */
        free((void *)itimer_array);
        /* TODO log error event */
        return (ERROR);
    }

    /* initialize free list */
    for (ii = 0; ii < maxTimers; ii++)
    {
        itimer_array[ii].func   = 0;
        itimer_array[ii].parm   = 0;
        itimer_array[ii].taskId = 0;
        itimer_array[ii].cookie = ITMR_COOKIE_MIN;
        itimer_array[ii].next = ii + 1;
    }
    itimer_array[maxTimers - 1].next = ITMR_NULL_NEXT;
    free_list = itimer_array;
    delta_list = (SIW_ITMR_NODE_t *)NULL;

    itimer_max = maxTimers;

    /* spawn tINTERVAL (uses itimer_max) */
    tid = taskSpawn("tINTERVAL", 3, 0, 10000, (FUNCPTR) ITmrTask,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (tid == ERROR)
    {
        printf("ERROR - taskSpawn() returned ERROR\n"); /* XXX */
        /* TODO delete itmr_freeSem */
        free((void *)itimer_array);
        /* TODO log error event */
        itimer_max = 0;
        return (ERROR);
    }

    /* successful return */
    return (OK);
}

/********************************************************************
*  Function:    SiwITmrPop()
********************************************************************/
LOCAL SIW_ITMR_NODE_t *
SiwITmrPop(SIW_ITMR_NODE_t **queue)
{
    SIW_ITMR_NODE_t *rv = (SIW_ITMR_NODE_t *)NULL;

    if (queue != (SIW_ITMR_NODE_t **)NULL)
    {
        rv = *queue;
        if (*queue)
        {
            if ((*queue)->next >= itimer_max)  /* includes ITMR_NULL_NEXT */
            {
                *queue = (SIW_ITMR_NODE_t *)NULL;
            }
            else
            {
                *queue = itimer_array + (*queue)->next; /* ptr arithmetic */
            }
        }
    }
    return (rv);
}

/********************************************************************
*  Function:    SiwITmrInsert()
********************************************************************/
LOCAL STATUS_t
SiwITmrInsert(SIW_ITMR_NODE_t *node, SIW_ITMR_NODE_t **queue)
{
    if (!node ||!queue)    /* NULL checking */
    {
        printf("SiwITmrInsert: unexpected NULL %p %p\n", node, queue); /*XXX*/
        return (ERROR);
    }
    /* make sure node is terminated properly */
    node->next = ITMR_NULL_NEXT;

    if (*queue == (SIW_ITMR_NODE_t *)NULL)
    {
        /* queue size 0 */
        *queue = node;
    }
    else if ((*queue)->delta >= node->delta)
    {
        /* insert at head of queue */
        (*queue)->delta -= node->delta;
        node->next = *queue - itimer_array;     /* ptr arithmetic */
        *queue = node;
    }
    else if ((*queue)->next >= itimer_max)
    {
        /* insert at tail of queue size 1 */
        node->delta -= (*queue)->delta;
        (*queue)->next = node - itimer_array;   /* ptr arithmetic */
    }
    else
    {
        /* insert after head of queue size > 1 */
        SIW_ITMR_NODE_t *back = *queue;
        SIW_ITMR_NODE_t *front = itimer_array + back->next; /* ptr arithmetic */
        U16_t sumdelta = back->delta + front->delta;
        U16_t tempsum;

        /* search for insertion point */
        while ((sumdelta < node->delta) && (front->next < itimer_max))
        {
            back = front;
            front = itimer_array + front->next;     /* ptr arithmetic */
            sumdelta += front->delta;
        }

        if (sumdelta >= node->delta)
        {
            /* insert between front and back */
            back->next = node - itimer_array;       /* ptr arithmetic */
            node->next = front - itimer_array;      /* ptr arithmetic */
            tempsum = sumdelta - front->delta;
            front->delta = (sumdelta - node->delta);
            node->delta -= tempsum;
        }
        else
        {
            /* insert at tail */
            front->next = node - itimer_array;      /* ptr arithmetic */
            node->delta -= sumdelta;
        }
    }
    return (OK);
}

/********************************************************************
 *  Function:    free_stopped_timers()
 ********************************************************************/
LOCAL int
free_stopped_timers(SIW_ITMR_NODE_t *node)
{
    int rv = 0;
    int lock;
    int sumDelta = 0;
    unsigned count;
    SIW_ITMR_NODE_t *prev = NULL;

    for (count = 1; node && itmr_stoppedCount; count++)
    {
        sumDelta += node->delta;

        if ((sumDelta > 1) && (count > 1))
        {
            if (node->func == NULL)
            {
                lock = intLock();           /* LOCK INTERRUPTS */

                rv++;
                itmr_stoppedCount--;
                
                /* remove node from list */
                prev->next = node->next;
                if (node->next < itimer_max)
                {
                    itimer_array[node->next].delta += node->delta;
                }
                
                /* insert node in free queue */
                node->delta = 0;
                SiwITmrInsert(node, &free_list);
                semGive(itmr_freeSem);
                intUnlock(lock);            /* UNLOCK INTERRUPTS */
            }
        }

        /* advance to next node */
        prev = node;
        if (node->next == ITMR_NULL_NEXT)
        {
            break;
        }
        else
        {
            node = itimer_array + node->next;   /* ptr arithmetic */
        }
    }
    return (rv);    /* return number of nodes recycled */
}

/********************************************************************
 *  Function:    SiwITmrStart()
 *
 *  Description: This routine adds an interval timer to the SIW timer
 *               queue.  The specified callback routine will be called
 *               in the context of the timer task after the specified
 *               number  of  milliseconds +/- 100 milliseconds have elapsed.
 *               SiwITmrStart() must not be called from interrupt level.
 *               Although any delay between 1000 and 60000 ms may be
 *               specified, the last two digits are not significant.
 *               So, for example, two timers with delays of 4002 and
 *               4036 are not distinguishable and may run in either order.
 *
 *  In Parms:    U32_t delay         - delay in msec (1000 - 60000)
 *               FUNC_PTR_t pRoutine - callback routine
 *               void *parm          - parameter to pass to callback routine
 *
 *  Out Parms:   U32_t *error - error number
 *
 *  Errors:      Possible values for error include:
 *               0 (zero) - no error
 *               EAGAIN   - service temporarily unavailable
 *               ENOMEM   - no interval timers are available
 *               EINVAL   - invalid parameter
 *
 *  Returns:     The ID of the started interval timer, or NULL on error
 ********************************************************************/
SIW_ITMRID_t
SiwITmrStart (IN U32_t delay, IN FUNC_PTR_t pRoutine,
              IN void *parm, OUT U32_t *error)
{
    SIW_ITMR_NODE_t *   node;

    if (error)
    {
        *error = 0;
    }

    if (!itimer_max)
    {
        printf("ERROR - not initialized\n");    /* XXX */
        if (error)
        {
            *error = ENOMEM;
        }
        return (0);
    }
    /* TODO check for interrupt level */

    /* check parameters */
    if ((delay < SIW_ITMR_DELAY_MIN) || (delay > SIW_ITMR_DELAY_MAX) ||
        (pRoutine == (FUNC_PTR_t)NULL))
    {
        if (error)
        {
            *error = EINVAL;
        }
        return ((SIW_ITMRID_t)NULL);
    }

    /* mutual exclusion semaphore protects from multiple writers */
    if (semTake(itmr_startSem, 1) != OK)
    {
        if (error)
        {
            *error = EAGAIN;
        }
        printf("interval timer temporarily unavailable\n");    /* XXX */
        return ((SIW_ITMRID_t)NULL);
    }

    /* timer resource is managed using counting semaphore */
    if ((semTake(itmr_freeSem, NO_WAIT) == OK) ||
        ((free_stopped_timers(delta_list) > 0) &&
         (semTake(itmr_freeSem, NO_WAIT) == OK)))
    {
        /* get node from head of free list */
        node = SiwITmrPop(&free_list);

        if (node == NULL)
        {
            /* XXX should not get here */
            printf("SiwITmrStart: NULL free list unexpected\n");
        }
        else
        {
            /* fill in node structure */
            node->func = pRoutine;
            node->parm = parm;
            node->taskId = (U16_t)((U32_t)SiwTaskIdSelf());  /* 32->16 bit squeeze */
            node->delta = delay / (1000 / sysClkRateGet());

            /* insert node in timer queue */
            SiwITmrInsert(node, &delta_list);
        }
    }
    else
    {
        printf("no interval timer available\n");    /* XXX */
        if (error)
        {
            *error = ENOMEM;
        }
        /* TODO log error event */
        semGive(itmr_startSem);                     /* drop lock */
        return ((SIW_ITMRID_t)NULL);
    }

    semGive(itmr_startSem);                         /* drop lock */
    /* return cookie in upper 16 bits, index in lower 16 bits */
    return ((node->cookie << 16) + (node - itimer_array));
}

/********************************************************************
 *  Function:    SiwITmrStop()
 *
 *  Description: This routine stops a currently running interval timer.
 *               An ID that has been passed to SiwITmrStop() is no
 *               longer valid and cannot be 'restarted'.  The memory
 *               used for the timer will be recycled automatically.
 *               SiwITmrStop() may be called from interrupt level.
 *
 *  In Parms:    SIW_ITMRID_t iTmrId - ID of SIW interval timer to stop
 *
 *  Out Parms:   None
 *
 *  Returns:     OK, or ERROR if iTmrId was invalid on entry.
 ********************************************************************/
STATUS_t
SiwITmrStop (IN SIW_ITMRID_t iTmrId)
{
    U16_t index;
    SIW_ITMR_NODE_t *   node;
    STATUS_t            rv = ERROR;

    /* find node */

    if ((index = ITMR_ID_TO_INDEX(iTmrId)) >= itimer_max)
    {
        return (ERROR);
    }
    node = itimer_array + index;    /* pointer arithmetic */

    /* check validity */

    if ((node->func) && (node->cookie == ITMR_ID_TO_COOKIE(iTmrId)))
    {
        int lock = intLock();       /* LOCK INTERRUPTS */

        /* check validity again (with interrupts locked) */

        if ((node->func) && (node->cookie == ITMR_ID_TO_COOKIE(iTmrId)))
        {
            /* mark node invalid and change its (non-zero) cookie */
            node->func = (FUNC_PTR_t)NULL;
            ITMR_COOKIE_CHANGE(node->cookie);

            /* increment counter (protected by intLock) */
            ++itmr_stoppedCount; 
            rv = OK;
        }
        intUnlock(lock);            /* UNLOCK INTERRUPTS */
    }
    return (rv);
}

LOCAL U16_t
SiwINodeShow (SIW_ITMR_NODE_t *node, U16_t sumDelta, S32_t level,
              SIW_ITMRID_t iTmrId)
{
    SIW_ITMRID_t tmpId;

    if (node == (SIW_ITMR_NODE_t *)NULL)
    {
        printf("SiwINodeShow: NULL unexpected\n");
    }
    else
    {
        tmpId = (node->cookie << 16) + (node - itimer_array);
        sumDelta += node->delta;
        /* NULL func indicates stopped timer */
        if (node->func && ((iTmrId == 0) || (iTmrId == tmpId)))
        {
            printf("    ID: 0x%08x\n", tmpId);
            if (level)
            {
                STATUS rc;
                char symName[80];
                SYM_TYPE symType;
                int symVal;
                char *pName;

                printf("  func: %p", node->func);
                symName[0] = 0;
                rc = symFindByValue (sysSymTbl, (int)(node->func),
                                     symName, &symVal, &symType);
                if ((symVal == (int)(node->func)) && symName[0])
                {
                    printf("  (%s)", symName);
                }
                printf("\n  parm: %p", node->parm);
                symName[0] = 0;
                rc = symFindByValue (sysSymTbl, (int)(node->parm),
                                     symName, &symVal, &symType);
                if ((symVal == (int)(node->parm)) && symName[0])
                {
                    symName[79] = 0;
                    printf("  (%s)", symName);
                }
                printf("\n  task: %d", node->taskId);
                pName = SiwTaskName((SIW_TID_t)((U32_t)node->taskId));
                if (pName)
                {
                    printf("  (%s)", pName);
                }
                printf("\n");
            }
            printf(" delay: %d ms\n\n", ((sumDelta + 5) / 10) * 100);
        }
    }
    return (sumDelta);
}

LOCAL void
SiwIQueueShow(SIW_ITMR_NODE_t *queue, S32_t level, SIW_ITMRID_t iTmrId)
{
    U16_t sum = 0;

    while (queue)
    {
        sum = SiwINodeShow(queue, sum, level, iTmrId);
        
        if (queue->next == ITMR_NULL_NEXT)
        {
            break;
        }
        else
        {
            queue = itimer_array + queue->next;     /* ptr arithmetic */
        }
    }
}

/********************************************************************
 *  Function:    ITmrTask()
 ********************************************************************/
void
ITmrTask (void)
{
    SIW_ITMR_NODE_t *   node;

    if (!itimer_max)
    {
        printf("itimer_max is NULL\n");
    }
    else if (!semTickBroadcast)
    {
        printf("semTickBroadcast is NULL\n");
    }
    else
    {
        FOREVER
        {
            /* wait for tick */
            semTake(semTickBroadcast, WAIT_FOREVER);

            /* process delta_list */
            if (delta_list && (delta_list->delta > 0))
            {
                delta_list->delta --;
            }

            while (delta_list && (delta_list->delta == 0))
            {
                /* get node from head of delta list */
                node = SiwITmrPop(&delta_list);

                if (node->func != NULL)
                {
                    /* active timer */
                    (*node->func) (node->parm); /* invoke callback function */
                    node->func = NULL;
                }
                else
                {
                    int lock = intLock();   /* LOCK INTERRUPTS */

                    /* stopped timer */
                    --itmr_stoppedCount;    /* update counter */
                    intUnlock(lock);        /* UNLOCK INTERRUPTS */
                }

                /* recycle node to free list */
                ITMR_COOKIE_CHANGE(node->cookie);
                SiwITmrInsert(node, &free_list);
                semGive(itmr_freeSem);
            }
        }
    }
    printf("ITmrTask exiting\n");   /* XXX */
    exit (1);
}

/********************************************************************
 *  Function:    SiwITmrShow()
 *
 *  Description: This function prints SIW interval timer information.
 *
 *  In Parms:    SIW_ITMRID_t iTmrId - ID of interval timer to show,
 *                                     or 0 for summary of all timers
 *               S32_t level         - level of information (0 is default)
 *
 *  Out Parms:   None
 *
 *  Returns:     OK, or ERROR if iTmrId is neither 0 nor a valid timer ID.
 ********************************************************************/
STATUS_t
SiwITmrShow (IN SIW_ITMRID_t iTmrId, IN S32_t level)
{
    SiwIQueueShow(delta_list, level, iTmrId);
    return (OK);
}
