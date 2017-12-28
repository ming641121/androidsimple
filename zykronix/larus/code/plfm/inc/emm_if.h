#ifndef		_EMM_IF_H_
#define   _EMM_IF_H_


 /*====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    interface to Emm devices
 *
 * Note:
 *
 * Author: Betta
 *
 *====================================================================
 * $Id: emm_if.h,v 1.13 2006/10/11 07:43:47 betta Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/emm_if.h,v $
 * $Log: emm_if.h,v $
 * Revision 1.13  2006/10/11 07:43:47  betta
 * *** empty log message ***
 *
 * Revision 1.12  2006/10/05 07:08:45  betta
 * *** empty log message ***
 *
 * Revision 1.11  2006/10/05 07:07:03  betta
 * *** empty log message ***
 *
 * Revision 1.10  2006/10/03 09:38:17  betta
 * *** empty log message ***
 *
 * Revision 1.9  2006/09/13 08:13:52  betta
 * *** empty log message ***
 *
 * Revision 1.8  2006/08/28 02:49:59  betta
 * *** empty log message ***
 *
 * Revision 1.7  2006/08/22 09:47:51  betta
 * flash access code added
 *
 * Revision 1.6  2006/08/22 01:24:06  betta
 * *** empty log message ***
 *
 * Revision 1.5  2006/08/15 02:43:19  betta
 * one filed of structure was removed
 *
 * Revision 1.4  2006/08/09 09:32:58  betta
 * *** empty log message ***
 *
 * Revision 1.3  2006/08/08 01:06:15  betta
 * *** empty log message ***
 *
 *====================================================================
 * $Endlog 
 */

#include "sr_snmp.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpdefs.h"
#include "snmppart.h"
#include "snmpsupp.h"
#include "emm_event.h"


#define majAlmHiThreshDefault   60
#define majAlmLoThreshDefault  -30
#define minAlmHiThreshDefault   50
#define minAlmLoThreshDefault  -20 
#define EMM_READ              1
#define EMM_WRITE             2

typedef int (*EMM_Read_Routine)();
typedef struct
{
  int emmShelf;
  int emmSensorType;
  int emmCurrentTp;
 
  int majAlmHiThresh; 
  int majAlmLoThresh;
  int minAlmHiThresh;
  int minAlmLoThresh;
  EMM_Read_Routine emmReadFunc;

  unsigned delayTime;
  SEM_ID emmSem;
  int formerEmmAlarmStatus;
  int formerMjAlmHiThresh; 
  int formerMjAlmLoThresh;
  int formerMnAlmHiThresh;
  int formerMnAlmLoThresh;
} emmSensor_t;
typedef struct
{
  int emmShelf;  
  int emmSensorType;
  EMM_Read_Routine emmReadFunc;
  unsigned delayTime;
  int formerEmmAlarmStatus;
} emmVolt_t;
               /* declaration of routine */
extern STATUS_t emmTask(void);
extern STATUS_t tmpEmmSet(stsEmmStatusEntry_t *emmEntry);
extern STATUS_t tmpEmmGet(stsEmmStatusEntry_t *emmEntry);
extern STATUS_t emmDataAccess(char *fname, int mode, unsigned sensorIdx);
extern int i2cReadTmpReg(int *);
extern int query_5VoltStatus(void);
extern int query_12VoltStatus(void);
extern int query_MainPwrAStatus(void);
extern int query_MainPwrBStatus(void);

            /* Globle valuables */
         
/* information for temperature sensor */
extern emmSensor_t tpSensor[];
/* information for power on the main shelf */
extern emmVolt_t voltState[];
#endif