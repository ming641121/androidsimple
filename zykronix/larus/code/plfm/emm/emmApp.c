 /*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: emmApp.c
 *
 *Define all Event objects for EMM
 *
 *Note:
 *
 *Author(s):
 * Betta 
 *
 *********************************************************************/

 /********************************************************************
 * $Id: emmApp.c,v 1.24 2006/10/05 07:06:04 betta Exp $
 * $Source: /cvsroot/larus5820/code/plfm/emm/emmApp.c,v $
 *********************************************************************/


#include <string.h>
#include <stdio.h>
#include "i2c.h"
#include "emm_if.h"
#include "trapdef.h"
#include "trap_if.h"

#include <sm.h>



#include "snmptrap.h"
#include "sys_snmp.h"

/* #define EMM_DEBUG 0 */


static int digCalcute(char *revBuf);

extern SYS_CONFIG_T sysConfig;




/*************************************************************************
function: 
  i2cReadTmpReg - get the temperature from the sensor.  
paramenter:
  int *tmp - the temperature value got from the sensor.
  												
return:
  OK -  get the temperature from the sensor successfully.
 ERROR - Fail to get the temperature from the sensor.
*/

int i2cReadTmpReg(int *tmp)
{
    unsigned char rxBuf[2];
    unsigned char regAddr = 0x00;
    if (i2cReadReg(0x90, &regAddr, 1, rxBuf, 2))
        return ERROR;
    *tmp = digCalcute(rxBuf);
    return OK;
}

/*************************************************************************
function: 
  digCalcute - transform the data received from the 
               high threshold temperature register, 
               low threshold temperature register and temperature register
               into digital number to get temperature value. 
paramenter:
  U8 revBuf - the pointer to address where stores two-byte character data 
              
  												
return:
  N/A 
*/


int digCalcute(char *revBuf)
{
    int tmp;
    tmp = revBuf[0];

    if (revBuf[0] & 0x80)
        tmp = -((~(int) tmp) + 1);
#if 0
    if (revBuf[1] & 0x80) {
        if (revBuf[0] & 0x80)
            tmp -= 0.5;
        else
            tmp += 0.5;
    }
#endif
    return tmp;
}



/******************************************************************************
function:
    emmTask - the task to monitor emm status.
              
paramenter: 
    void
RETURN: 
   	N/A
*/

STATUS_t emmTask(void)
{

    stsEMMAlarmStatusChange_t emmStatus;
    stsEMMAlarmThresholdChange_t emmAlarmThreshold;
    U8_t tmp;
    int shelf_index;
    int err, i;
    char pwrString[][13] = {
        {"5-Volts"},
        {"12-Volts"},
        {"A"},
        {"B"},

    };

    i2cInit();

/* 
tpSensor[0] - 
information associated with 
the only one temperature sensor on the main board. */


    tpSensor[0].emmSem = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

    /* Read threashold temperature default value */
    if (emmDataAccess("tmpThresh.dat", EMM_READ, 0) == ERROR) {
        tpSensor[0].majAlmHiThresh = majAlmHiThreshDefault;
        tpSensor[0].majAlmLoThresh = majAlmLoThreshDefault;
        tpSensor[0].minAlmHiThresh = minAlmHiThreshDefault;
        tpSensor[0].minAlmLoThresh = minAlmHiThreshDefault;
        emmDataAccess("tmpThresh.dat", EMM_WRITE, 0);
    }


    emmStatus.stsEmmAlarmStatus = D_stsEmmAlarmStatus_cl;

    SiwTaskInitAck(OK);
/* 
tpSensor[0] - 
information associatted with 
the only one temperature sensor on the main board. */
    while (1) {
        if (tpSensor[0].delayTime == 0) {
            err = tpSensor[0].emmReadFunc(&tpSensor[0].emmCurrentTp);

        } else {

            tpSensor[0].delayTime--;
            goto queryVolts;
        }
        if (err == ERROR) {     /* Fail to read temperature sensor */

            if (tpSensor[0].delayTime == 0)
                tpSensor[0].delayTime = 5;

        } else {

            semTake(tpSensor[0].emmSem, WAIT_FOREVER);

            emmAlarmThreshold.stsSystemId.octet_ptr =
                sysConfig.stsSystemId.octet_ptr;
            emmAlarmThreshold.stsSystemId.length =
                sysConfig.stsSystemId.length;
            emmAlarmThreshold.stsEmmShelf = tpSensor[0].emmShelf;
            emmAlarmThreshold.stsEmmSensorType = tpSensor[0].emmSensorType;
            emmAlarmThreshold.stsEmmMajorAlmHiThresh =
                tpSensor[0].majAlmHiThresh;
            emmAlarmThreshold.stsEmmMajorAlmLoThresh =
                tpSensor[0].majAlmLoThresh;
            emmAlarmThreshold.stsEmmMinorAlmHiThresh =
                tpSensor[0].minAlmHiThresh;
            emmAlarmThreshold.stsEmmMinorAlmLoThresh =
                tpSensor[0].minAlmLoThresh;

            if (tpSensor[0].majAlmHiThresh != tpSensor[0].formerMjAlmHiThresh) {        /* Major high threshold temperatuere value was changed by action function */
                tpSensor[0].formerMjAlmHiThresh =
                    tpSensor[0].majAlmHiThresh;
                SiwEventLog(NULL,
                            SIW_EVENT_SYM(EMM, MjAlm_Hi_Thresh_Change),
                            tpSensor[0].majAlmHiThresh);

                TrapEventLog(stsEMMAlarmThresholdChange_TRAP,
                             D_stsEmmSensorType_temp_ambient,
                             D_stsEmmShelf_main, D_stsCardIndex_imu,
                             0, D_stsEmmAlarmStatus_mj,
                             (U32_t *) & emmAlarmThreshold,
                             SIW_EVENT_SYM(EMM, MjAlm_Hi_Thresh_Change),
                             tpSensor[0].majAlmHiThresh);
            }
            if (tpSensor[0].majAlmLoThresh !=
                tpSensor[0].formerMjAlmLoThresh) {
                /* Major low threshold temperatuere value was changed 
                   by action function */
                tpSensor[0].formerMjAlmLoThresh =
                    tpSensor[0].majAlmLoThresh;
                SiwEventLog(NULL,
                            SIW_EVENT_SYM(EMM, MjAlm_Lo_Thresh_Change),
                            tpSensor[0].majAlmLoThresh);

                TrapEventLog(stsEMMAlarmThresholdChange_TRAP,
                             D_stsEmmSensorType_temp_ambient,
                             D_stsEmmShelf_main, D_stsCardIndex_imu,
                             0, D_stsEmmAlarmStatus_mj,
                             (U32_t *) & emmAlarmThreshold,
                             SIW_EVENT_SYM(EMM, MjAlm_Lo_Thresh_Change),
                             tpSensor[0].majAlmLoThresh);
            }
            if (tpSensor[0].minAlmHiThresh !=
                tpSensor[0].formerMnAlmHiThresh) {
                /* Minor high threshold temperatuere value was changed
                   by action function */
                tpSensor[0].formerMnAlmHiThresh =
                    tpSensor[0].minAlmHiThresh;
                SiwEventLog(NULL,
                            SIW_EVENT_SYM(EMM, MnAlm_Hi_Thresh_Change),
                            tpSensor[0].minAlmHiThresh);

                TrapEventLog(stsEMMAlarmThresholdChange_TRAP,
                             D_stsEmmSensorType_temp_ambient,
                             D_stsEmmShelf_main, D_stsCardIndex_imu,
                             0, D_stsEmmAlarmStatus_mj,
                             (U32_t *) & emmAlarmThreshold,
                             SIW_EVENT_SYM(EMM, MnAlm_Hi_Thresh_Change),
                             tpSensor[0].minAlmHiThresh);
            }
            if (tpSensor[0].minAlmLoThresh !=
                tpSensor[0].formerMnAlmLoThresh) {
                /* Minor low threshold temperatuere value was changed
                   by action function */
                tpSensor[0].formerMnAlmLoThresh =
                    tpSensor[0].minAlmLoThresh;
                SiwEventLog(NULL,
                            SIW_EVENT_SYM(EMM, MnAlm_Lo_Thresh_Change),
                            tpSensor[0].minAlmLoThresh);

                TrapEventLog(stsEMMAlarmThresholdChange_TRAP,
                             D_stsEmmSensorType_temp_ambient,
                             D_stsEmmShelf_main, D_stsCardIndex_imu,
                             0, D_stsEmmAlarmStatus_mj,
                             (U32_t *) & emmAlarmThreshold,
                             SIW_EVENT_SYM(EMM, MnAlm_Lo_Thresh_Change),
                             tpSensor[0].minAlmLoThresh);
            }

            emmStatus.stsSystemId.octet_ptr =
                sysConfig.stsSystemId.octet_ptr;
            emmStatus.stsSystemId.length = sysConfig.stsSystemId.length;
            emmStatus.stsEmmShelf = tpSensor[0].emmShelf;
            emmStatus.stsEmmSensorType = tpSensor[0].emmSensorType;
            emmStatus.stsEmmSensorReading = tpSensor[0].emmCurrentTp;
            if (tpSensor[0].emmCurrentTp >= tpSensor[0].majAlmHiThresh) {
                /* the temperature is higher than 
                   major alarm high threshold temperature now. */

                if (tpSensor[0].formerEmmAlarmStatus != D_stsEmmAlarmStatus_mj) {
                    emmStatus.stsEmmAlarmStatus = D_stsEmmAlarmStatus_mj;
                    SiwEventLog(NULL, SIW_EVENT_SYM(EMM, mjAlm_Hi_Thresh));
                    TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                 D_stsEmmSensorType_temp_ambient,
                                 D_stsEmmShelf_main, D_stsCardIndex_imu,
                                 0, D_stsEmmAlarmStatus_mj,
                                 (U32_t *) & emmStatus, SIW_EVENT_SYM(EMM,
                                                                      mjAlm_Hi_Thresh));
                    tpSensor[0].formerEmmAlarmStatus = D_stsEmmAlarmStatus_mj;
                }
            } /* end of if (tpSensor[0].emmCurrentTp>majAlmHiThresh) */
            else if ((tpSensor[0].emmCurrentTp <=
                      tpSensor[0].majAlmLoThresh)) {
                /* the temperature reaches 
                   major alarm low threshold temperature now */


                if (tpSensor[0].formerEmmAlarmStatus != D_stsEmmAlarmStatus_mj) {
                    emmStatus.stsEmmAlarmStatus = D_stsEmmAlarmStatus_mj;
                    SiwEventLog(NULL, SIW_EVENT_SYM(EMM, mjAlm_Lo_Thresh));
                    TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                 D_stsEmmSensorType_temp_ambient,
                                 D_stsEmmShelf_main, D_stsCardIndex_imu,
                                 0, D_stsEmmAlarmStatus_mj,
                                 (U32_t *) & emmStatus, SIW_EVENT_SYM(EMM,
                                                                      mjAlm_Lo_Thresh));
                    tpSensor[0].formerEmmAlarmStatus = D_stsEmmAlarmStatus_mj;

                }
            }

            else if ((tpSensor[0].emmCurrentTp >=
                      tpSensor[0].minAlmHiThresh)
                     && (tpSensor[0].emmCurrentTp <
                         tpSensor[0].majAlmHiThresh)) {
                /* the temperature reaches 
                   minor alarm high threshold temperature now */


                if (tpSensor[0].formerEmmAlarmStatus != D_stsEmmAlarmStatus_mn) {
                    emmStatus.stsEmmAlarmStatus = D_stsEmmAlarmStatus_mn;
                    SiwEventLog(NULL, SIW_EVENT_SYM(EMM, mnAlm_Hi_Thresh));
                    TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                 D_stsEmmSensorType_temp_ambient,
                                 D_stsEmmShelf_main, D_stsCardIndex_imu,
                                 0, D_stsEmmAlarmStatus_mn,
                                 (U32_t *) & emmStatus, SIW_EVENT_SYM(EMM,
                                                                      mnAlm_Hi_Thresh));
                    tpSensor[0].formerEmmAlarmStatus = D_stsEmmAlarmStatus_mn;
                }
            } else
                if ((tpSensor[0].emmCurrentTp > tpSensor[0].majAlmLoThresh)
                    && (tpSensor[0].emmCurrentTp <=
                        tpSensor[0].minAlmLoThresh)) {
                /* the temperature reaches 
                   minor alarm low threshold temperature now */

                if (tpSensor[0].formerEmmAlarmStatus != D_stsEmmAlarmStatus_mn) {
                    emmStatus.stsEmmAlarmStatus = D_stsEmmAlarmStatus_mn;
                    SiwEventLog(NULL, SIW_EVENT_SYM(EMM, mnAlm_Lo_Thresh));
                    TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                 D_stsEmmSensorType_temp_ambient,
                                 D_stsEmmShelf_main, D_stsCardIndex_imu,
                                 0, D_stsEmmAlarmStatus_mn,
                                 (U32_t *) & emmStatus, SIW_EVENT_SYM(EMM,
                                                                      mnAlm_Lo_Thresh));
                    tpSensor[0].formerEmmAlarmStatus = D_stsEmmAlarmStatus_mn;
                }
            } else {            /* clear */
                /* From other threshold temperature 
                   to normal temperature now */
                if (tpSensor[0].formerEmmAlarmStatus != D_stsEmmAlarmStatus_cl) {
                    emmStatus.stsEmmAlarmStatus = D_stsEmmAlarmStatus_cl;
                    SiwEventLog(NULL, SIW_EVENT_SYM(EMM, SENSOR_OK));
                    TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                 D_stsEmmSensorType_temp_ambient,
                                 D_stsEmmShelf_main,
                                 D_stsCardIndex_imu, 0,
                                 D_stsEmmAlarmStatus_cl,
                                 (U32_t *) & emmStatus, SIW_EVENT_SYM(EMM,
                                                                      SENSOR_OK));
                    tpSensor[0].formerEmmAlarmStatus = D_stsEmmAlarmStatus_cl;

                }
            }                   /* else clear */

            semGive(tpSensor[0].emmSem);

        }                       /* err==OK */

      queryVolts:
        /*
           voltState[0] - information associatted with 5-V power on the main board.                                                                                                                                                                                                                                
           voltState[1] - information associatted with 12-V power on the main board.
           voltState[2] - information associatted with 48-V power A on the main board.  
           voltState[3] - information associatted with 48-V power B on the main board.     
         */
        for (i = 0; i < 4; i++) {
            emmStatus.stsSystemId.octet_ptr =
                sysConfig.stsSystemId.octet_ptr;
            emmStatus.stsSystemId.length = sysConfig.stsSystemId.length;
            emmStatus.stsEmmShelf = voltState[i].emmShelf;
            emmStatus.stsEmmSensorType = voltState[i].emmSensorType;
            emmStatus.stsEmmAlarmStatus = D_stsEmmAlarmStatus_cl;
            if (voltState[i].delayTime == 0) {
                err = voltState[i].emmReadFunc();

            } else {
                voltState[i].delayTime--;
                continue;
            }
            if (err == ERROR) {

                if (voltState[i].delayTime == 0)
                    voltState[i].delayTime = 5;

                if (voltState[i].formerEmmAlarmStatus != D_stsEmmAlarmStatus_mj) { /* power from OK to not OK  now */
                    emmStatus.stsEmmAlarmStatus = D_stsEmmAlarmStatus_mj;

                    SiwEventLog(NULL, SIW_EVENT_SYM(EMM, POWER_FAIL),
                                pwrString[i]);
                    TrapEventLog(stsEMMAlarmStatusChange_TRAP, voltState[i].emmSensorType,      /*ex:voltState[i].emmSensorType==D_stsEmmSensorType_volt5 */
                                 D_stsEmmShelf_main,
                                 D_stsCardIndex_imu, 0,
                                 D_stsEmmAlarmStatus_mj,
                                 (U32_t *) & emmStatus,
                                 SIW_EVENT_SYM(EMM, POWER_FAIL),
                                 pwrString[i]);


                    voltState[i].formerEmmAlarmStatus = D_stsEmmAlarmStatus_mj;
                }
                /* voltState[i].formerEmmAlarmStatus!=D_stsEmmAlarmStatus_mj */
            }
            /* err!=0 */
            else {              /* err==OK */


                if (voltState[i].formerEmmAlarmStatus != D_stsEmmAlarmStatus_cl) { /* power from not OK to OK  now */
                    emmStatus.stsEmmAlarmStatus = D_stsEmmAlarmStatus_cl;

                    SiwEventLog(NULL, SIW_EVENT_SYM(EMM, POWER_OK));
                    TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                 D_stsEmmSensorType_volt5,
                                 D_stsEmmShelf_main,
                                 D_stsCardIndex_imu, 0,
                                 D_stsEmmAlarmStatus_cl,
                                 (U32_t *) & emmStatus,
                                 SIW_EVENT_SYM(EMM, POWER_OK),
                                 pwrString[i]);

                    voltState[i].formerEmmAlarmStatus = D_stsEmmAlarmStatus_cl;
                }               /* voltState[i].formerEmmAlarmStatus!=D_stsEmmAlarmStatus_cl */
            }                   /* err==OK */
        }                       /* for (i = 0; i < 4; i++) */

        /* monitoring power status in expansion */

        for (shelf_index = 1; shelf_index < NUMBER_SHELVES; shelf_index++) {
            if (shelves[i].present == TRUE) {
                expFPGAAccess(shelf_index, 0x00, 0x00, &tmp, FPGA_READ);
                if ((tmp & 0x01) == 0x00) {

                    if (shelves[shelf_index].powerA != TRUE) {  /*power A from not OK to OK */
                        shelves[shelf_index].powerA = TRUE;
                        /* send alarm for ok status */
                        emmStatus.stsSystemId.octet_ptr =
                            sysConfig.stsSystemId.octet_ptr;
                        emmStatus.stsSystemId.length =
                            sysConfig.stsSystemId.length;
                        emmStatus.stsEmmShelf = shelf_index;
                        emmStatus.stsEmmSensorType =
                            D_stsEmmSensorType_volt48_A;
                        emmStatus.stsEmmAlarmStatus =
                            D_stsEmmAlarmStatus_cl;
                        SiwEventLog(NULL, SIW_EVENT_SYM(EMM, POWER_A_OK),
                                    shelf_index);
                        TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                     D_stsEmmSensorType_volt48_A,
                                     shelf_index + 1, D_stsCardIndex_imu,
                                     0, D_stsEmmAlarmStatus_cl,
                                     (U32_t *) & emmStatus,
                                     SIW_EVENT_SYM(EMM, POWER_A_OK),
                                     shelf_index);
                    }
                } else {        /*power A from OK to not OK */
                    if (shelves[shelf_index].powerA != FALSE) {
                        shelves[shelf_index].powerA = FALSE;
                        /* send alarm for fail status */
                        emmStatus.stsSystemId.octet_ptr =
                            sysConfig.stsSystemId.octet_ptr;
                        emmStatus.stsSystemId.length =
                            sysConfig.stsSystemId.length;
                        emmStatus.stsEmmShelf = shelf_index;
                        emmStatus.stsEmmSensorType =
                            D_stsEmmSensorType_volt48_A;
                        emmStatus.stsEmmAlarmStatus =
                            D_stsEmmAlarmStatus_mj;
                        SiwEventLog(NULL, SIW_EVENT_SYM(EMM, POWER_A_FAIL),
                                    shelf_index);
                        TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                     D_stsEmmSensorType_volt48_A,
                                     shelf_index + 1, D_stsCardIndex_imu,
                                     0, D_stsEmmAlarmStatus_mj,
                                     (U32_t *) & emmStatus,
                                     SIW_EVENT_SYM(EMM, POWER_A_FAIL),
                                     shelf_index);

#ifdef EMM_DEBUG
                        printf("in emm, shelf power A lost\n");
#endif
                    }

                }

                if ((tmp & 0x02) == 0x00) {

                    if (shelves[shelf_index].powerB != TRUE) {  /*power B from not OK to OK */
                        shelves[shelf_index].powerB = TRUE;
                        /* send alarm for ok status */
                        emmStatus.stsSystemId.octet_ptr =
                            sysConfig.stsSystemId.octet_ptr;
                        emmStatus.stsSystemId.length =
                            sysConfig.stsSystemId.length;
                        emmStatus.stsEmmShelf = shelf_index;
                        emmStatus.stsEmmSensorType =
                            D_stsEmmSensorType_volt48_B;
                        emmStatus.stsEmmAlarmStatus =
                            D_stsEmmAlarmStatus_cl;
                        SiwEventLog(NULL, SIW_EVENT_SYM(EMM, POWER_B_OK),
                                    shelf_index);
                        TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                     D_stsEmmSensorType_volt48_B,
                                     shelf_index + 1, D_stsCardIndex_imu,
                                     0, D_stsEmmAlarmStatus_cl,
                                     (U32_t *) & emmStatus,
                                     SIW_EVENT_SYM(EMM, POWER_B_OK),
                                     shelf_index);
                    }

                } else {        /*power B from OK to not OK */

                    if (shelves[shelf_index].powerB != FALSE) {
                        shelves[shelf_index].powerB = FALSE;
                        /* send alarm for fail status */
                        emmStatus.stsSystemId.octet_ptr =
                            sysConfig.stsSystemId.octet_ptr;
                        emmStatus.stsSystemId.length =
                            sysConfig.stsSystemId.length;
                        emmStatus.stsEmmShelf = shelf_index;
                        emmStatus.stsEmmSensorType =
                            D_stsEmmSensorType_volt48_B;
                        emmStatus.stsEmmAlarmStatus =
                            D_stsEmmAlarmStatus_mj;
                        SiwEventLog(NULL, SIW_EVENT_SYM(EMM, POWER_B_FAIL),
                                    shelf_index);
                        TrapEventLog(stsEMMAlarmStatusChange_TRAP,
                                     D_stsEmmSensorType_volt48_B,
                                     shelf_index + 1, D_stsCardIndex_imu,
                                     0, D_stsEmmAlarmStatus_mj,
                                     (U32_t *) & emmStatus,
                                     SIW_EVENT_SYM(EMM, POWER_B_FAIL),
                                     shelf_index);
#ifdef EMM_DEBUG
                        printf("in emm, shelf [%d] power B lost\n",
                               shelf_index);
#endif
                    }
                }
            }                   /* if (shelves[i].present == TRUE) */
        }                       /* for (shelf_index = 1; shelf_index < NUMBER_SHELVES; shelf_index++) */
        taskDelay(200);
    }
    return ERROR;
}

/******************************************************************************
function:
    query_5VoltStatus - query 5-V power status.
              
paramenter: 
    void
RETURN: 
   	0 : 5-V power works normally.
   -1 : 5-V power is absent now.
*/

int query_5VoltStatus(void)
{

    *M8260_IOP_PCPAR(immrVal) &= ~PC7;
    *M8260_IOP_PCDIR(immrVal) &= ~PC7;  /* input */
    if ((*M8260_IOP_PCDAT(immrVal)) & PC7)
        return OK;
    return ERROR;
}

/******************************************************************************
function:
    query_5VoltStatus - query 12-V power status.
              
paramenter: 
    void
RETURN: 
   	0 : 12-V power works normally.
   -1 : 12-V power is absent now.
*/

int query_12VoltStatus(void)
{

    *M8260_IOP_PCPAR(immrVal) &= ~PC6;
    *M8260_IOP_PCDIR(immrVal) &= ~PC6;  /* input */
    if ((*M8260_IOP_PCDAT(immrVal)) & PC6)
        return OK;
    return ERROR;
}

/******************************************************************************
function:
    query_MainPwrAStatus - query power A status on the main board.
              
paramenter: 
    void
RETURN: 
   	0 :  power A works normally.
   -1 :  power A is absent now.
*/

int query_MainPwrAStatus(void)
{

    *M8260_IOP_PCPAR(immrVal) &= ~PC0;
    *M8260_IOP_PCDIR(immrVal) &= ~PC0;  /* input */
    if ((*M8260_IOP_PCDAT(immrVal)) & PC0)
        return ERROR;
    return OK;
}

/******************************************************************************
function:
    query_MainPwrAStatus - query power B status on the main board.
              
paramenter: 
    void
RETURN: 
   	0 :  power B works normally.
   -1 :  power B is absent now.
*/


int query_MainPwrBStatus(void)
{

    *M8260_IOP_PCPAR(immrVal) &= ~PC1;
    *M8260_IOP_PCDIR(immrVal) &= ~PC1;  /* input */
    if ((*M8260_IOP_PCDAT(immrVal)) & PC1)
        return ERROR;
    return OK;
}

/******************************************************************************
function:
    emmDataAccess - Access alarm threshold temperature value via flash.
              
paramenter: 
    char *fname - file name storing alarm threshold temperature value.
    int mode - IO mode (R/W)
    unsigned sensorIdx - element index of tpSensor[] array.
RETURN: 
   	OK    : Access sucessfully.
    ERROR : Fail to access.
*/
STATUS_t emmDataAccess(char *fname, int mode, unsigned sensorIdx)
{
    FILE *fhd = NULL;

    STATUS_t status = OK;
    int tmp[4];

    if (fname == NULL) {
        return ERROR;
    }

    if (mode == EMM_READ) {
        if ((fhd = fopen(fname, "r")) == NULL)
            status = ERROR;

        else {
            if (fread((char *) &tmp, sizeof(int), 4, fhd) != 4)
                status = ERROR;
            else {
                tpSensor[sensorIdx].majAlmHiThresh = tmp[0];
                tpSensor[sensorIdx].majAlmLoThresh = tmp[1];
                tpSensor[sensorIdx].minAlmHiThresh = tmp[2];
                tpSensor[sensorIdx].minAlmLoThresh = tmp[3];
            }
        }
        fclose(fhd);
    } else if (mode == EMM_WRITE) {

        if ((fhd = fopen(fname, "w")) == NULL)
            status = ERROR;
        else {
            tmp[0] = tpSensor[sensorIdx].majAlmHiThresh;
            tmp[1] = tpSensor[sensorIdx].majAlmLoThresh;
            tmp[2] = tpSensor[sensorIdx].minAlmHiThresh;
            tmp[3] = tpSensor[sensorIdx].minAlmLoThresh;
        }
        fwrite((char *) &tmp, sizeof(char), 4, fhd);
        fclose(fhd);
    }

    return status;
}
