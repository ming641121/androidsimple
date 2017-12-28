/*
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: emm_event.h
 *
 *Define all Event objects for serial Module
 *
 *Note:
 *
 *Author(s):
 *  
 *  betta.lin
 *
 *********************************************************************/

 /********************************************************************
 * $Id: emm_event.h,v 1.6 2006/09/13 08:08:44 betta Exp $
 * $Source: /cvsroot/larus5820/code/plfm/emm/emm_event.h,v $
 *====================================================================
 * $Log: emm_event.h,v $
 * Revision 1.6  2006/09/13 08:08:44  betta
 * *** empty log message ***
 *
 * Revision 1.5  2006/08/28 02:49:22  betta
 * *** empty log message ***
 *
 * Revision 1.4  2006/08/22 03:33:29  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.3  2006/08/22 01:23:10  betta
 * modified emmTask
 *
 * Revision 1.2  2006/08/21 04:14:21  cvsshuming
 * 1. add expansion shelf power monitoring
 * 2. add related trap
 * 3. add severity for existing trap.
 *
 *====================================================================
 * $Endlog $
 *********************************************************************/

#ifndef _TMP_EVENT_H_
#define _TMP_EVENT_H_

#include <siw_events.h>
#include "siw_trace.h"


SIW_EVENT_DEF(EMM, POWER_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "power %s is on the main board not available.");
SIW_EVENT_DEF(EMM, POWER_OK, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "power %s on the main board is ok now.");
     
          
SIW_EVENT_DEF(EMM, POWER_A_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "48-Volts power A in shelf [%d] is not available");
SIW_EVENT_DEF(EMM, POWER_B_FAIL, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "48-Volts power A in shelf [%d] is not available");
SIW_EVENT_DEF(EMM, POWER_A_OK, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "48-Volts power B in shelf [%d] is ok now");
SIW_EVENT_DEF(EMM, POWER_B_OK, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "48-Volts power B in shelf [%d] is ok now");


SIW_EVENT_DEF(EMM, SENSOR_OK, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The temperature sensor is now fuctioning well");
     
SIW_EVENT_DEF(EMM, mjAlm_Hi_Thresh, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The temperature is higher than major alarm high threshold temperature");    
SIW_EVENT_DEF(EMM, mjAlm_Lo_Thresh, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The temperature is higher than major alarm low threshold temperature");
SIW_EVENT_DEF(EMM, mnAlm_Hi_Thresh, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The temperature is higher than minor alarm high threshold temperature");
SIW_EVENT_DEF(EMM, mnAlm_Lo_Thresh, EVENT_MAJOR, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The temperature is higher than minor alarm low threshold temperature"); 
      
SIW_EVENT_DEF(EMM, MjAlm_Hi_Thresh_Change, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The major alarm high threshold temperature is Celsius %d degree now");    
SIW_EVENT_DEF(EMM, MjAlm_Lo_Thresh_Change, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The major alarm low threshold temperature is Celsius %d degree now");
SIW_EVENT_DEF(EMM, MnAlm_Hi_Thresh_Change, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The minor alarm high threshold temperature is Celsius %d degree now");
SIW_EVENT_DEF(EMM, MnAlm_Lo_Thresh_Change, EVENT_INFO, EVENT_NO_OPTION, EVENT_NO_ACTION,
     "The minor alarm low threshold temperature is Celsius %d degree now");       
              
         
/* temp sensor event log macros */
#define EMM_LOG0(e)		           		SiwEventLog(NULL, SIW_EVENT_SYM(EMM,e))
#define EMM_LOG1(e,v)              SiwEventLog(NULL, SIW_EVENT_SYM(EMM,e),(v))
#define EMM_LOG2(e,v1,v2)          SiwEventLog(NULL, SIW_EVENT_SYM(EMM,e),(v1),(v2))
#define EMM_LOG3(e,v1,v2,v3)       SiwEventLog(NULL, SIW_EVENT_SYM(EMM,e),(v1),(v2),(v3))
#define EMM_LOG4(e,v1,v2,v3,v4)    SiwEventLog(NULL, SIW_EVENT_SYM(EMM,e),(v1),(v2),(v3),(v4))
#define EMM_LOG5(e,v1,v2,v3,v4,v5) SiwEventLog(NULL, SIW_EVENT_SYM(EMM,e),(v1),(v2),(v3),(v4),(v5))

 
#endif  /* _EMM_EVENT_H_ */
