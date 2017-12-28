/*
 *Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_event.h
 *
 *Define all Event objects for SIW Module
 *
 *Note:
 *
 *Author(s):
 * 
 */

#ifndef _SIW_EVENT_H_
#define _SIW_EVENT_H_

#include "siw_events.h"
#include "siw_trace.h"

SIW_MODULE_DEF(SIW, TRACE_LEVEL_1);

SIW_EVENT_DEF(SIW, ISRNOTCALLABLE, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Do not allow ISR to call: %s");

SIW_EVENT_DEF(SIW, TBLSEMTAKEFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to receive %s Table sem: %p, errno: %x");

SIW_EVENT_DEF(SIW, TASKTBLFULL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Task Table is full used: %d");

SIW_EVENT_DEF(SIW, TASKSEMGIVEFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to receive Task Sem: %p, SIW Task ID: %p");

SIW_EVENT_DEF(SIW, TASKINITPTO, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Time out to receive ACK from spawned task, SIW Task ID: %p");

SIW_EVENT_DEF(SIW, TASKINITFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The spawned task init failure, SIW Task ID: %p");

SIW_EVENT_DEF(SIW, TASKIDDELETED, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Task ID has been deleted: %p");

SIW_EVENT_DEF(SIW, TASKIDINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid Task ID: %p");

SIW_EVENT_DEF(SIW, TASKDELFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to delete task, Task ID(IN): %p, Task ID(Table): %p");

SIW_EVENT_DEF(SIW, TASKDELSAFE, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Task delete is not safe, Task ID: %p");

SIW_EVENT_DEF(SIW, TASKNAMEFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to retrieve the task name, Task ID: %p");

SIW_EVENT_DEF(SIW, TASKPRIOINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid task priority: %d");

SIW_EVENT_DEF(SIW, TASKPGRPINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid group task priority: %d");

SIW_EVENT_DEF(SIW, TASKAPPLINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid application task ID: %d");

SIW_EVENT_DEF(SIW, TASKSTKSIZEINV, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid task stack size: %d");

SIW_EVENT_DEF(SIW, TASKSUSPACTINV, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid task suspend action: %d");

SIW_EVENT_DEF(SIW, TASKFAILACTINV, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid task failed action: %d");

SIW_EVENT_DEF(SIW, OBJCREATENOMEM, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to create object: %s");

SIW_EVENT_DEF(SIW, TASKSEMCRTFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Sem creation failed for task synchronization");

SIW_EVENT_DEF(SIW, TASKSPAWNFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to spawn a task");

SIW_EVENT_DEF(SIW, TASKINITACKTO, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Sem timeout for parent waiting for ACK: SiwTID, %p, tblTID: %p, WTO: %d");

SIW_EVENT_DEF(SIW, TASKWACKFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Timeout for parent waiting for ACK, tblTID: %p");

SIW_EVENT_DEF(SIW, TASKDELMQOWN, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "In SysTaskDelete, the task still own the message queue, taskId: %p");

SIW_EVENT_DEF(SIW, MQTBLFULL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Message Queue Table is full: %d");

SIW_EVENT_DEF(SIW, MQIDINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid Message Queue ID: %p");

SIW_EVENT_DEF(SIW, MQIDDELETED, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Message Queue ID is deleted: %p");

SIW_EVENT_DEF(SIW, MQSEMDELFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Message Queue's Sem delete failure, SemId: %p, MsqId: %p");

SIW_EVENT_DEF(SIW, MQDELFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Message Queue delete failure, vxMsgQId: %p, MsqId: %p");

SIW_EVENT_DEF(SIW, MQSENDLENINV, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid message size to send, msgSize: %d, MsqId: %p");

SIW_EVENT_DEF(SIW, MQSENDFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to send message, tblMsgQId: %p, MsqId: %p");

SIW_EVENT_DEF(SIW, MQSEMGIVEFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "In MessageSend, failed to give Sem, semBId: %p, MsqId: %p");

SIW_EVENT_DEF(SIW, MQRECVLENINV, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Received invalid message size, msgSize: %d, MsqId: %p");

SIW_EVENT_DEF(SIW, MQRECVFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to receive message, tblMsgQId: %p, MsqId: %p");

SIW_EVENT_DEF(SIW, MQNUMMSGFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to get the number of message in queue, tblMsgQId: %p, MsqId: %p");

SIW_EVENT_DEF(SIW, MQINFOPTRINV, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid message queue info ptr, msgQInfoPtr: %p, MsqId: %p");

SIW_EVENT_DEF(SIW, MQINFOFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to get message queue info, tblMsgQId: %p, MsqId: %p, errno: %x");

SIW_EVENT_DEF(SIW, MQMAXINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid message queue size, maxMsgs: %d");

SIW_EVENT_DEF(SIW, MQMLENINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid message length, maxMsgLength: %d");

SIW_EVENT_DEF(SIW, MQOWNERINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid message queue owner, taskId: %p");

SIW_EVENT_DEF(SIW, MQOWNERNOTEXIST, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Message queue owner is not exit, taskId: %p");

SIW_EVENT_DEF(SIW, MQCREATEFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to create Message Queue, errno: %d");

SIW_EVENT_DEF(SIW, MQSEMCRTFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to create Message Queue Sem, errno: %d");

SIW_EVENT_DEF(SIW, MQFLSHRECVFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to flush message queue, msgQId: %p, errno: %d");

SIW_EVENT_DEF(SIW, SEMTBLFULL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Sem Table is full, maxEntries: %d");

SIW_EVENT_DEF(SIW, SEMIDINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid Sem ID, semId: %p");

SIW_EVENT_DEF(SIW, SEMIDDELETED, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Sem ID is deleted, semId: %p");

SIW_EVENT_DEF(SIW, SEMDELUNOWN, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Sem owned by deleted task, semId: %p, taskId: %p");

SIW_EVENT_DEF(SIW, SEMDELFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to delete Sem, vxSemId: %p, semId: %p, errno: %d");

SIW_EVENT_DEF(SIW, SEMTAKEFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to take Sem, vxSemId: %p, semId: %p, errno: %d");

SIW_EVENT_DEF(SIW, SEMGIVEUNOWN, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid operation to give Sem, semId: %p, taskId: %p");

SIW_EVENT_DEF(SIW, SEMGIVEFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to give Sem, tblSemId: %p, semId: %p, errno: %d");
SIW_EVENT_DEF(SIW, SEMFLUSHINV, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid to flush Sem, semId: %p");

SIW_EVENT_DEF(SIW, SEMFLUSHFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to flush Sem, tblSemId: %p, semId: %p, errno: %d");

SIW_EVENT_DEF(SIW, SEMRECVINV, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid deadlock action: %d");

SIW_EVENT_DEF(SIW, SEMTYPEINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid semType: %d");

SIW_EVENT_DEF(SIW, SEMOPTINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid Sem type&option, option: %d, semType: %d");

SIW_EVENT_DEF(SIW, SEMSTATEINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid init state: %d");

SIW_EVENT_DEF(SIW, SEMCREATEFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to create Sem, errno: %x");

SIW_EVENT_DEF(SIW, SEMDLKLOGERR, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Timeout for taking Sem -- vxSemId:%p, semId:%p, log event");

SIW_EVENT_DEF(SIW, SEMDLKRESTAR,EVENT_MINOR,EVENT_NO_OPTION,EVENT_TASK_SUSPEND,
     "Timeout for taking Sem -- vxSemId:%p, semId:%p, restart task");

SIW_EVENT_DEF(SIW, SEMDLKRESET, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Timeout for taking Sem -- vxSemId:%p, semId:%p, soft reset");

SIW_EVENT_DEF(SIW, TMERTBLFULL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Timer Table is full: %d");

SIW_EVENT_DEF(SIW, TMRCREATEFAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed to create Timer, errno: %x");

SIW_EVENT_DEF(SIW, TMRIDINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid Timer ID, timerId: %p logged by %s");

SIW_EVENT_DEF(SIW, TMRNOMEM, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "No available memory block for TimerFunction");

SIW_EVENT_DEF(SIW, TMRFUNCIDINV, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid TimerFunction ID, timerId: %p, tmrFuncId: %p");

SIW_EVENT_DEF(SIW, TMRCNNTFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed connect timer, timerId:%p, tmrFuncId:%p, errno:%x");

SIW_EVENT_DEF(SIW, TMRSTMFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Failed set timer, timerId:%p, tmrvalue:%d, errno:%x");

SIW_EVENT_DEF(SIW, TMRFUNCINVAL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid Timer Function received from the list");

SIW_EVENT_DEF(SIW, TMRMSGQINVAL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid Message Function received from the list");

SIW_EVENT_DEF(SIW, TMRMSGSNDFAIL, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "In Timer Function failed to send message: %p");

SIW_EVENT_DEF(SIW, NOSARBUF, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "There is no available Sar Buf: %p");

SIW_EVENT_DEF(SIW, NOSTATICMEM, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "There is no available static mem: %p");

SIW_EVENT_DEF(SIW, MEMPTRINVALID, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Try to free a out of range memPtr, memPtr: %p, taskId: %p");

SIW_EVENT_DEF(SIW, WRNGMEMPTR, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Free wrong %s memPtr, memPtr: %p, taskId: %p");

SIW_EVENT_DEF(SIW, NOSYSMEM, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "No available system memory for the size of: %d");

SIW_EVENT_DEF(SIW, WRNGMEMSIZE, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Request mem from Dyn-Pool with wrong size, size: %d, taskId: %p");

SIW_EVENT_DEF(SIW, NODYNMEM, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "There is no available dynamic mem from pool: %d, taskId: %p");

SIW_EVENT_DEF(SIW, INVALBUFPTR, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid SarBufPtr for chaining, fPtr: %p, nPtr: %p taskId: %p");

SIW_EVENT_DEF(SIW, INVALBUFPTRN, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Invalid fSarBufPtr for geeting next buf, fPtr: %p, taskId: %p");

SIW_EVENT_DEF(SIW, DYNMEMTHRESH, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Dynamic memory pool %d: %4d of %4d buffers allocated");

SIW_EVENT_DEF(SIW, DYNMEMCORRUPT, EVENT_MINOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "Detected dynamic mem corruption, adrs: %p, size: %d");

#endif  /* _SIW_EVENT_H_ */
