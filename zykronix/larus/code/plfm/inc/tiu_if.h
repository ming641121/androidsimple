/*
 *Copyright(C) 2006, All Rights Reserved, by
 *Ahoya Netowrks, Inc. San Jose, CA
 *
 *Abstract:
 *
 *File Name: tiu_if.h
 *
 * tiu framer maxim ds26502 code
 *
 *Note:
 *
 *Author(s):
 * shuming chang
 */
 
 
#ifndef _TIU_IF_H_
#define _TIU_IF_H_



#include <ds26502.h>
#include <semLib.h>



#define TIU_MAX_SHELF             5
#define TIU_MAX_SLOT              16
#define TIU_FRAMERS               2

#define TIU_FRAMER_BASE_ADDR      0x41002000
#define TIU_CFG_FILENAME					"tiu.cfg"
#define TIU_FLASH_READ              1
#define TIU_FLASH_WRITE             2

#define TIU_OUT_OF_SERVICE		0
#define TIU_IN_SERVICE            1

#define POLL_TIU_INTERVAL         5		  /* 50msc */

extern SEM_ID TiuTaskSemaphore;

typedef struct {
  S8_t         status;
  U8_t         alarmFlag;
  U8_t         redTimer;
  U8_t         redAlarm;
  U8_t         losAlarm;
  U8_t         raisAlarm;
  U8_t         yelAlarm;
  U8_t         lofAlarm;
  U8_t         losEvent;
  U8_t         lofEvent;
  U8_t         raisEvent;
  U8_t         raiEvent;
  U8_t         bvCountMan;
  U8_t         crcCountMan;
  U8_t         bvCountDef;
  U8_t         crcCountDef;
} TIU_STATUS_t;

typedef struct {
  int status;
  TE1_CONFIG_t lnCfg;
} TIU_FRAMER_t;


extern void tiuTask();
extern void tiuFramerInit();
extern STATUS_t tiuAdd(int shelf, int slot);
extern STATUS_t tiuRemove(int shelf, int slot);
extern STATUS_t tiuCheck(int shelf, int slot, int framer);
extern STATUS_t tiuFramerConfig(int shelf, int slot, int framer);
extern STATUS_t snmpTiuGet(stsOutputTiuSettingEntry_t *mibstructIn);
extern STATUS_t snmpTiuSet(stsOutputTiuSettingEntry_t *mibstructIn);


#endif /* _TIU_IF_H_ */
