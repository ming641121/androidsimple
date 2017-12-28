/*Copyright(C) 2005, All Rights Reserved, by
 *Ahoya Networks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: siw_cards.c
 *
 *This module provides API to access card information.  
 *
 *Note:
 *
 *Author(s):
 * 
 */

#include <vxWorks.h>
#include <string.h>
#include <sysLib.h>

#include <axss_types.h>
/*#include <sys_piodefs.h>	*/
#include <siw_if.h>
#include <sys_if.h>
#include "siw_trace.h"
#include "siw_event.h"
#include <sm_if.h>
#include <cm_if.h>

#define SCC_SLOT_9            8
#define SCC_SLOT_10           9

extern S32_t Logical2PhysicalSlotNum(S32_t logicalSlot);
extern U32_t GetBCType (S32_t slot);
extern U32_t GetDc1Type (S32_t slot);
extern U32_t GetDc2Type (S32_t slot);

/************************************************************
 *  Function:    SiwCardState()
 *  Description: This function returns my card state.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     Card State
 *************************************************************/
S8_t SiwCardState(S32_t slot)
{
    return (GetCardState(Logical2PhysicalSlotNum(slot)));
}

/************************************************************
 *  Function:    SiwMyCardType()
 *  Description: This function returns my card type.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     Card Type
 *************************************************************/
U32_t SiwMyCardType()
{
    return (GetCardType(SiwMyPhySlotNo()));
}

/************************************************************
 *  Function:    SiwCardType()
 *  Description: This function returns the card type for the
 *               specific card.
 *
 *  In Parms:    U8_t nvRamId
 *  Out Parms:   None
 *  Returns:     Card Type
 *************************************************************/
S32_t SiwCardType(S32_t slot)
{
    return ((S32_t) GetCardType(Logical2PhysicalSlotNum(slot)));
}

/************************************************************
 *  Function:    SiwCardService()
 *  Description: This function returns my card service.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     Card Service
 *************************************************************/
S32_t SiwCardService(S32_t slot)
{
    S32_t  service = 0;
    service = GetCardService(Logical2PhysicalSlotNum(slot));

    return (service);
}

/************************************************************
 *  Function:    SiwIfService()
 *  Description: This function returns the service type for
 *               a card.
 *
 *  In Parms:    None
 *  Out Parms:   None
 *  Returns:     Card Service
 *************************************************************/
S32_t SiwIfService(S32_t slot)
{
    S32_t  service = 0;
    U32_t  backcard = 0;
    U32_t  dc1      = 0;
    U32_t  dc2      = 0;

    backcard = GetBCType (Logical2PhysicalSlotNum(slot));
    dc1 = GetDc1Type (Logical2PhysicalSlotNum(slot));
    dc2 = GetDc2Type (Logical2PhysicalSlotNum(slot));

    if (SiwCardState (slot) != CARD_REMOVED)
    {
       switch (SiwCardType (slot))
       {
          case CARD_SCC:
             if ( ((backcard == BC_SCC_12T3) || (backcard == BC_SCC_12E3)) &&
                  (dc1 == BIM_12T3E3) )
                service = DS3_IF_TYPE;
             else if ( (backcard == BC_SCC_4FE) && (dc1 == BIM_4FE) )
                service = ENET_IF_TYPE;
             else if ( (backcard == BC_SCC_4OC3) && (dc1 == BIM_4OC3) )
                service = OC3_IF_TYPE;
             else if ( (backcard == BC_SCC_1OC12) && (dc1 == BIM_OC12) )
                service = OC12_IF_TYPE;
             else
                service = UNKNOWN_IF_TYPE;
             break;

          case CARD_DMC:
             if (backcard == BC_DMC_6T3)
                service = DMC_DS3_IF_TYPE;
             else
                service = UNKNOWN_IF_TYPE;
             break;

          case CARD_BSC:
             if ( ((backcard == BC_BSC_12T3) || (backcard == BC_BSC_12E3)) &&
                  ((dc1 == BIM_12T3E3) || (dc1 == BIM_CES_4T3E3)) )
                service = DS3_IF_TYPE;
             else if ( (backcard == BC_BSC_4FE) && (dc1 == BIM_4FE) )
                service = ENET_IF_TYPE;
             else if ( (backcard == BC_BSC_4OC3) && (dc1 == BIM_4OC3) )
                service = OC3_IF_TYPE;
             else if ( (backcard == BC_BSC_1OC12) && (dc1 == BIM_OC12) )
                service = OC12_IF_TYPE;
             else
                service = UNKNOWN_IF_TYPE;
             break;
 
          case CARD_NSC:
          case CARD_NMC:
#ifdef SCC
#ifndef FLASH_BOOT
             if ( (shelf.slot[slot].cIfMode == IF_MODE_BACKPLANE) ||
#else
             if (
#endif /*FLASH_BOOT*/
#else 
#ifdef NMC
             if ( (CmIfMode() == IF_MODE_BACKPLANE) ||
#else
             if (
#endif /*NMC*/
#endif /*SCC*/
                  (backcard == BC_NMC_16T1E1) ||
                  (backcard == BC_RED_16T1E1) )
                service = DS1_IF_TYPE;
             else
                service = UNKNOWN_IF_TYPE;
             break;

          default:
                service = UNKNOWN_IF_TYPE;
                break;
       }
 
    }
    else
       service = UNKNOWN_IF_TYPE;

    return (service);

}

/***********************************************************************
 *  Function:    SiwWaitCardActive()
 *  Description: This function is used to wait for the card state to
 *               enter the ACTIVE state. 
 *
 *  In Parms:    U8_t slot - slot number
 *  Out Parms:   None
 *  Returns:     OK or ERROR if unable to wait for card becoming Active
 **********************************************************************/
STATUS_t
SiwWaitCardActive(U8_t slot)
{
    STATUS_t  rc = OK;
    SIW_CARD_STATE_t currState;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwWaitCardActive, 0, 0, 0, 0, 0);


    for (;;)
    {
        currState = GetCardState(Logical2PhysicalSlotNum(slot));
        if ( currState == CARD_ACTIVE)
        {
            break;
        }
        else
        {
            taskDelay(2);
        }
    }

    return(rc);
}

/***********************************************************************
 *  Function:  SiwWaitCardReady()
 *  Description: This function is used to wait for the card state to
 *               become. This function will block the calling task until
 *               the card is ready for processing.
 *
 *  In Parms:    U8_t slot - slot number
 *  Out Parms:   None
 *  Returns:     OK or ERROR if unable to wait for card ready
 **********************************************************************/
STATUS_t
SiwWaitCardReady(U8_t slot)
{
    STATUS_t  rc = OK;
    SIW_CARD_STATE_t currState;

    /* Call function trace */
    SIW_FUNC_TRACE(SIW, SiwWaitCardReady, 0, 0, 0, 0, 0);


    for (;;)
    {
        currState = GetCardState(Logical2PhysicalSlotNum(slot));
        if ( (currState == CARD_ACTIVE) || (currState == CARD_STANDBY) )
        {
            break;
        }
        else
        {
            taskDelay(2);
        }
    }
 
    return(rc);
}

/*******************************************************************
 *  Function:     SiwMySlotNo()
 *  Description:  This function returns the slot number of the
 *                current card.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      slot number 
 *******************************************************************/
S32_t SiwMySlotNo()
{
#ifdef AHOYA
#ifdef SIM
    S32_t slotNo = 0;
    int   i = 0;
    int   j;

    for (;;)
    {
        if (sysBootLine[i++] == ',')
        {
            for (j=i; j<i+2; j++)
            {
                if (sysBootLine[j] != ')')
                {
                    slotNo *= 10;
                    slotNo += (S32_t)sysBootLine[j] & 0xF;
                }
            }
            break;
        }
    }
    return (slotNo - 1);
#else
#ifdef SCC
    return (8); /* Always return the SCC logical slot number is zero based */
#else
    if (VALID_SLOT(cmMyLogSlotNum)) 
        return(cmMyLogSlotNum);
    else
        return ((SYS_SLOT_ID())-1);
#endif
#endif
#endif /* AHOYA */
	return 0;
}


/*******************************************************************
 *  Function:     SiwMyPhySlotNo()
 *  Description:  This function returns the physical slot number of the
 *                current card.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      slot number 
 *******************************************************************/
S32_t SiwMyPhySlotNo()
{
#ifdef AHOYA
#ifdef SIM
    S32_t slotNo = 0;
    int   i = 0;
    int   j;

    for (;;)
    {
        if (sysBootLine[i++] == ',')
        {
            for (j=i; j<i+2; j++)
            {
                if (sysBootLine[j] != ')')
                {
                    slotNo *= 10;
                    slotNo += (S32_t)sysBootLine[j] & 0xF;
                }
            }
            break;
        }
    }
    return (slotNo - 1);
#else
    return ((SYS_SLOT_ID())-1);
#endif
#endif /* AHOYA	*/
	return 0;
}
/*******************************************************************
 *  Function:     SiwStandbySlotNo()
 *  Description:  This function returns the slot number of standby 
 *                card.
 *
 *  In Parms:     None
 *  Out Parms:    None
 *  Returns:      slot number
 *******************************************************************/
S32_t SiwStandbySlotNo()
{
    S32_t slotNo = -1;

    if (VALID_SCC_SLOT(SiwMyPhySlotNo())) {
        slotNo = (SiwMyPhySlotNo() ^ 1);
    }

    return (slotNo);
}

/* AHOYA added - stub func */
S32_t Logical2PhysicalSlotNum(S32_t logicalSlot)
{
	return 	logicalSlot;
}

U32_t GetBCType (S32_t slot)
{
	return 0;
}

U32_t GetDc1Type (S32_t slot)
{
	return 0;
}

U32_t GetDc2Type (S32_t slot)
{
	return 0;
}

U32_t GetCardType (S32_t slot)
{
	return 0;
}

S32_t GetCardService (S32_t slot)
{
	return 0;
}

S8_t GetCardState (S32_t slot)
{
    return 0;
}

