

#include <stdio.h>
#include "i2c.h"

#include "trapdef.h"
#include "emm_if.h"

/*
 Fields of emmSensor_t -
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
*/

emmSensor_t tpSensor[] = {
    {D_stsEmmShelf_main, D_stsEmmSensorType_temp_ambient, 25,
     majAlmHiThreshDefault, majAlmLoThreshDefault,
     minAlmHiThreshDefault, minAlmLoThreshDefault, i2cReadTmpReg, 0,
     NULL, D_stsEmmAlarmStatus_cl,
     majAlmHiThreshDefault, majAlmLoThreshDefault, minAlmHiThreshDefault,
     minAlmLoThreshDefault}

};



/*
 Fields of emmVolt_t -
  int emmShelf;
  int emmSensorType;
  EMM_Read_Routine emmReadFunc;
  unsigned delayTime;
  int formerEmmAlarmStatus;
*/

emmVolt_t voltState[] = {
    {D_stsEmmShelf_main, D_stsEmmSensorType_volt5, query_5VoltStatus, 0,
     D_stsEmmAlarmStatus_cl},
    {D_stsEmmShelf_main, D_stsEmmSensorType_volt12, query_12VoltStatus, 0,
     D_stsEmmAlarmStatus_cl},
    {D_stsEmmShelf_main, D_stsEmmSensorType_volt48_A, query_MainPwrAStatus,
     0,
     D_stsEmmAlarmStatus_cl},
    {D_stsEmmShelf_main, D_stsEmmSensorType_volt48_B, query_MainPwrBStatus,
     0,
     D_stsEmmAlarmStatus_cl},
};



/******************************************************************************
function:
tmpEmmGet - the task to get the mibs data information to the sensor device
              
paramenter: 
 stsEmmStatusEntry_t *emmEntry - the struct to store response data for temperature sensor query response  
  
 RETURNS: 
   		 OK    - if sensor is detected and get the temperature information
       ERROR - otherwise
*/

STATUS_t tmpEmmGet(stsEmmStatusEntry_t * emmEntry)
{
    if (emmEntry->stsEmmShelf < D_stsEmmShelf_main
        || emmEntry->stsEmmShelf > D_stsEmmShelf_expansion_4)
        return ERROR;
    if (emmEntry->stsEmmSensorType < D_stsEmmSensorType_temp_ambient
        || emmEntry->stsEmmSensorType > D_stsEmmSensorType_volt48_B)
        return ERROR;
    semTake(tpSensor[0].emmSem, WAIT_FOREVER);
    if (tpSensor[0].emmReadFunc(&tpSensor[0].emmCurrentTp) == OK) {

        if (tpSensor[0].emmCurrentTp >= tpSensor[0].majAlmHiThresh
            || tpSensor[0].emmCurrentTp <= tpSensor[0].majAlmLoThresh)
            emmEntry->stsEmmAlarmStatus = D_stsEmmAlarmStatus_mj;
        else if ((tpSensor[0].emmCurrentTp > tpSensor[0].majAlmLoThresh)
                 && (tpSensor[0].emmCurrentTp <=
                     tpSensor[0].minAlmLoThresh))
            emmEntry->stsEmmAlarmStatus = D_stsEmmAlarmStatus_mn;
        else if ((tpSensor[0].emmCurrentTp >= tpSensor[0].minAlmHiThresh)
                 && (tpSensor[0].emmCurrentTp <
                     tpSensor[0].majAlmHiThresh))
            emmEntry->stsEmmAlarmStatus = D_stsEmmAlarmStatus_mn;
        else
            emmEntry->stsEmmAlarmStatus = D_stsEmmAlarmStatus_cl;
        emmEntry->stsEmmSensorReading = tpSensor[0].emmCurrentTp;
        emmEntry->stsEmmMajorAlmHiThresh = tpSensor[0].majAlmHiThresh;
        emmEntry->stsEmmMajorAlmLoThresh = tpSensor[0].majAlmLoThresh;
        emmEntry->stsEmmMinorAlmHiThresh = tpSensor[0].minAlmHiThresh;
        emmEntry->stsEmmMinorAlmLoThresh = tpSensor[0].minAlmLoThresh;
    } else {
        semGive(tpSensor[0].emmSem);
        return ERROR;
    }
    semGive(tpSensor[0].emmSem);
    return OK;
}

/******************************************************************************
function:
tmpEmmSet - the task to set the mibs data information to the sensor device
              
paramenter: 
 stsEmmStatusEntry_t *emmEntry - the struct to store response data for temperature sensor query response  
  
 RETURNS: 
   		 OK    - if sensor is detected and get the temperature information
       ERROR - otherwise
*/
STATUS_t tmpEmmSet(stsEmmStatusEntry_t * emmEntry)
{
    semTake(tpSensor[0].emmSem, WAIT_FOREVER);
    if (VALID(I_stsEmmMajorAlmHiThresh, emmEntry->valid)) {
        tpSensor[0].majAlmHiThresh = emmEntry->stsEmmMajorAlmHiThresh;

    }
    if (VALID(I_stsEmmMajorAlmLoThresh, emmEntry->valid)) {
        tpSensor[0].majAlmLoThresh = emmEntry->stsEmmMajorAlmLoThresh;

    }
    if (VALID(I_stsEmmMinorAlmHiThresh, emmEntry->valid)) {
        tpSensor[0].minAlmHiThresh = emmEntry->stsEmmMinorAlmHiThresh;

    }
    if (VALID(I_stsEmmMinorAlmLoThresh, emmEntry->valid)) {
        tpSensor[0].minAlmLoThresh = emmEntry->stsEmmMinorAlmLoThresh;

    }
    emmDataAccess("tmpThresh.dat", EMM_WRITE, 0);

    semGive(tpSensor[0].emmSem);

    return OK;
}