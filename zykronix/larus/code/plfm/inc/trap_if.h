
#ifndef _TRAP_IF_H_
#define _TRAP_IF_H_
/*
 * TRAP_HEADER_t is commonly used by SNMP traps and MMS alarms/events.
 * If it is a SNMP trap, then the generator needs to call localTrapGen()
 * to form the message body.
 * If it is a MMS internal event/alarm, then the generator 
 * form the message body by their own format. However, only "trapLength" 
 * is needed in this case.
 */

enum {
  TRAP_CRITICAL = 1,
  TRAP_MAJOR,
  TRAP_MINOR,
  TRAP_NA,
  TRAP_NR,
  TRAP_CLEAR
};

typedef struct
{
  U32_t trapLength; /* length of trap info block, include header and body */
  U32_t trapSlot;
  U32_t trapSlotSeq;
  U32_t trapNumber;
  U32_t severity;
  U32_t catalog;
} TRAP_HEADER_t;


/****************************************************************************
 * Function:    TrapEventLog()
 * Descripion:  This routine generate trap event
 * In Parms:    IN U32_t trapNum         trap number
 *              IN U32_t *mibEntryPtr    point to the correspondent mib 
 *                                       structure data.
 *              IN SIW_EVENT_OBJ_t *event event structure
 * Out Parms:   None
 * Return Values: None
 */
/* STATUS_t TrapEventLog(IN U32_t trapNum, IN U32_t *mibEntryPtr,
             IN SIW_EVENT_OBJ_t *event, ...); */

STATUS_t TrapEventLog(IN U32_t trapNum, IN int trapType, IN int shelf, IN int slot, 
             IN int port, IN int severity, IN U32_t *mibEntryPtr,
             IN SIW_EVENT_OBJ_t *event, ...);

/****************************************************************************
 * Function:    TrapPack()
 * Descripion:  This routine pack to trap to trap message
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
U32_t TrapPack(IN U32_t trapNum, IN TRAP_HEADER_t *trapMsg, ...);

/****************************************************************************
 * Function:    TrapUnpack()
 * Descripion:  This routine pack to trap to trap message
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
U32_t
TrapUnpack(U32_t trapNum, U32_t *dataPtr, TRAP_HEADER_t *trapMsg);


/****************************************************************************
 * Function:    TrapPack()
 * Descripion:  This routine calculate the trap pack size for 
 *               specified trap num
 * In Parms:    None
 * Out Parms:   None
 * Return Values: None
 */
U32_t TrapPackLen(IN U32_t trapNum);


#endif
