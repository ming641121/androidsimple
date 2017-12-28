/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: sm_utils.c
 *
 *Define all Event objects for SM Module
 *
 *Note:
 *
 *Author(s): shuming
 *
 *********************************************************************/

 /********************************************************************
 * $Id: sm_utils.c,v 1.14 2006/10/03 12:10:33 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/sm/sm_utils.c,v $
 *====================================================================
 * $Log: sm_utils.c,v $
 * Revision 1.14  2006/10/03 12:10:33  cvsshuming
 * add action function support in shelf manager for special card type, such as top, smu and imu
 *
 * Revision 1.13  2006/10/03 05:45:09  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.12  2006/10/03 02:43:50  cvsshuming
 * rename redundance and state to be redundancy_enable and redundancy_state
 *
 * Revision 1.11  2006/10/02 12:08:38  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.10  2006/10/02 07:26:22  cvsshuming
 * add smVariableInit
 *
 * Revision 1.9  2006/09/20 10:37:38  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.8  2006/09/15 09:24:25  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.7  2006/09/13 07:20:27  cvsshuming
 * for portalarmstatus
 *
 * Revision 1.6  2006/09/12 10:19:51  cvsshuming
 * update snmp action func
 *
 * Revision 1.5  2006/09/12 05:48:52  cvsshuming
 * mv snmp and some utils out of sm.c
 *
 *====================================================================
 * $Endlog $
 *********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <vxWorks.h>
#include <tss_types.h>
#include <sm.h>

U8_t ssmMessages[STRATUM_CLOCK_MAX] =
    { 0x20, 0x10, 0x30, 0x08, 0x44, 0x14, 0x0C };
STATUS_t userSSMEnable = FALSE;
U8_t userSSM;
U8_t outpSSM;



/**************************************************************************************
* function: void outpSSMUpdate()
*  input 
*		
*			
*  return 
*     void
***************************************************************************************/

void outpSSMUpdate()
{
    int shelf_index, slot, port;
    U8_t tmp;

    if (userSSMEnable) {
        outpSSM = userSSM;
    } else {

        /* checkitout. get clock info from CSM to determine which stratum signal level 
           ask frank. he will provide API for this.
         */
#ifdef CSM_READY
        tmp = CsmStratumLevelGet();
#else
        tmp = 1;
#endif
        outpSSM = ssmMessages[tmp];
    }

    for (shelf_index = 0; shelf_index < NUMBER_SHELVES; shelf_index++) {
        if (shelf_index == MAIN_SHELF) {
            for (slot = 0; slot < 8; slot++) {
                if ((outpCard.main[slot].status == OUTPUT_PRESENT)) {
                    if ((outpCard.main[slot].cardType == OUTPUT_FREQUENCY)
                        || (outpCard.main[slot].cardType == OUTPUT_TIU)) {
                        /* update SSM to FPGA */
                        outpCard.main[slot].reg1 = outpSSM;
                        *(U8_t *) OUTPUT_FPGA_W_REG_2(slot) = outpSSM;
                    }
                }
            }
        } else if (shelf_index == EXP1_SHELF) {
            for (slot = 0; slot < 16; slot++) {
                if ((outpCard.exp1[slot].status == OUTPUT_PRESENT)) {
                    if ((outpCard.exp1[slot].cardType == OUTPUT_FREQUENCY)
                        || (outpCard.exp1[slot].cardType == OUTPUT_TIU)) {
                        /* update SSM to FPGA */
                        outpCard.exp1[slot].reg1 = outpSSM;
                        expFPGAAccess(EXP1_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp1[slot].reg1,
                                      FPGA_WRITE);
                    }
                }
            }
        } else if (shelf_index == EXP2_SHELF) {
            for (slot = 0; slot < 16; slot++) {
                if ((outpCard.exp2[slot].status == OUTPUT_PRESENT)) {
                    if ((outpCard.exp2[slot].cardType == OUTPUT_FREQUENCY)
                        || (outpCard.exp2[slot].cardType == OUTPUT_TIU)) {
                        /* update SSM to FPGA */
                        outpCard.exp2[slot].reg1 = outpSSM;
                        expFPGAAccess(EXP2_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp2[slot].reg1,
                                      FPGA_WRITE);
                    }
                }
            }
        } else if (shelf_index == EXP3_SHELF) {
            for (slot = 0; slot < 16; slot++) {
                if ((outpCard.exp3[slot].status == OUTPUT_PRESENT)) {
                    if ((outpCard.exp3[slot].cardType == OUTPUT_FREQUENCY)
                        || (outpCard.exp3[slot].cardType == OUTPUT_TIU)) {
                        /* update SSM to FPGA */
                        outpCard.exp3[slot].reg1 = outpSSM;
                        expFPGAAccess(EXP3_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp3[slot].reg1,
                                      FPGA_WRITE);
                    }
                }
            }
        } else if (shelf_index == EXP4_SHELF) {
            for (slot = 0; slot < 16; slot++) {
                if ((outpCard.exp4[slot].status == OUTPUT_PRESENT)) {
                    if ((outpCard.exp4[slot].cardType == OUTPUT_FREQUENCY)
                        || (outpCard.exp4[slot].cardType == OUTPUT_TIU)) {
                        /* update SSM to FPGA */
                        outpCard.exp4[slot].reg1 = outpSSM;
                        expFPGAAccess(EXP4_SHELF, (0x10 + slot), 0x01,
                                      &outpCard.exp4[slot].reg1,
                                      FPGA_WRITE);
                    }
                }
            }
        }
    }
}


/**************************************************************************************
* function: outpSSMSet
*  input 
*	int stratumClock
*			
*  return 
*     void
***************************************************************************************/
STATUS_t outpSSMSet(int stratumClock)
{
    if (stratumClock >= STRATUM_CLOCK_MAX) {
        printf("outpSSMSet: Wrong Stratum Clock %d\n", stratumClock);
        return ERROR;
    }

    userSSMEnable = TRUE;
    userSSM = ssmMessages[stratumClock];

    return OK;
}


/**************************************************************************************
* function:  
*  input 
*			 
*			
*  return 
*     void
***************************************************************************************/
U8_t outpSSMGet()
{
    return (outpSSM);
}

/**************************************************************************************
* function:  
*  input 
*			 
*			
*  return 
*     void
***************************************************************************************/
STATUS_t outpSSMClear()
{
    userSSMEnable = FALSE;
    userSSM = 6;

    return OK;
}



/**************************************************************************************
* function:  
*  input 
*			 
*			
*  return 
*     void
***************************************************************************************/
void expFPGAAccess(SHELF_t shelf_index, U8_t addrHigh, U8_t addrLow,
                   U8_t * data, U8_t command)
{                               /*shelves is global variable. */
    U8_t status;
    int i;

    if (shelf_index == EXP1_SHELF)
        *(U8_t *) EXP_FPGA_ADDR_SHELF = 1;
    else if (shelf_index == EXP2_SHELF)
        *(U8_t *) EXP_FPGA_ADDR_SHELF = 2;
    else if (shelf_index == EXP3_SHELF)
        *(U8_t *) EXP_FPGA_ADDR_SHELF = 3;
    else if (shelf_index == EXP4_SHELF)
        *(U8_t *) EXP_FPGA_ADDR_SHELF = 4;

    *(U8_t *) EXP_FPGA_ADDR_LSB = addrLow;
    *(U8_t *) EXP_FPGA_ADDR_MSB = addrHigh;
    if (command == FPGA_WRITE) {
        *(U8_t *) EXP_FPGA_ADDR_DATA = *data;
        *(U8_t *) EXP_FPGA_ADDR_COMMAND = command;
    } else if (command == FPGA_READ) {
        *(U8_t *) EXP_FPGA_ADDR_COMMAND = command;
        *data = 0;
        for (i = 0; i < 500000; i++) {  /* checkitout. is it good? */
            status = *(U8_t *) EXP_FPGA_ADDR_COMMAND;
            if ((status & 0x04) == 0x04) {
                *data = *(U8_t *) EXP_FPGA_ADDR_DATA;
                break;
            }
        }
    }
}




/**************************************************************************************
* function:  STATUS_t smDataAccess(char *fname, int mode, int dataType)
* description:
*	this routine read/write "fname" based on "dataType". the options are listed below.
*	for each dataType, the data is retrieve/save as defined in this routine.
*	if there is any change on global variable, this routine should be modified accordingly.
*	
*  input
*	Action mode: read/write
*	file name
*		dataType					fname			Global variable
*		-----------------------------------------------------------
*		SM_SHELF					smshelf.dat		shelves
*		SM_OUTPUT_CARD				smcard.dat		outpCard
*		SM_MAIN_OUTPUT_PORT			smmain.out		outpCard
*		SM_EXP1_OUTPUT_PORT			smexp1.out		outpCard
*		SM_EXP2_OUTPUT_PORT			smexp2.out		outpCard
*		SM_EXP3_OUTPUT_PORT			smexp3.out		outpCard
*		SM_EXP4_OUTPUT_PORT			smexp4.out		outpCard	
*  return 
*	STATUS_t 
*		OK
*		ERROR
*     
***************************************************************************************/

STATUS_t smDataAccess(char *fname, int mode, int dataType)
{
    FILE *fhd = NULL;
    int slot, port, i;
    STATUS_t status = OK;
    U8_t tmp[220];
    if (fname == NULL) {
        printf("smDataAccess: Invalid file name\n");
        return ERROR;
    }
    memset(tmp, 0, sizeof(tmp));
    if (mode == FLASH_READ) {
        if ((fhd = fopen(fname, "r")) == NULL)
            status = ERROR;

        else {
            switch (dataType) {

            case SM_SHELF:
                /* 5*(1+13+1+13)=130
                		#define SM_SHELF_SERIAL_LEN		13
						#define	SM_SHELF_CLEICODE_LEN	13 
                	byte		content
                	0			shelf 0 serial length
                	1-13		shelf 0 serial char
                	14			shelf 0 clei length
                	15-27		shelf 0 clei char
                	28			shelf 1 serial length
                	29-41		shelf 1 serial char
                	...
                */
                if (fread((char *) &tmp, sizeof(char), 130, fhd) != 130) {
                    status = ERROR;
                } else {
                    for (i = 0; i < NUMBER_SHELVES; i++) {

                        shelves[i].serial.length = tmp[i * 28];
                        shelves[i].cleicode.length = tmp[i * 28 + 13];

                        memcpy(shelves[i].serial.octet_ptr,
                               tmp + i * 28 + 1, shelves[i].serial.length);
                        memcpy(shelves[i].cleicode.octet_ptr,
                               tmp + i * 28 + 15,
                               shelves[i].cleicode.length);
                    }
                }
                fclose(fhd);
                break;
            case SM_OUTPUT_CARD:
            	/* 3x(8+16x4)= 216
                	byte		content
                	0-7			main shelf output card redundancy setting
                	8-23		exp1 shelf output card redundancy setting
                	24-39		exp2 shelf output card redundancy setting
                	40-55		exp3 shelf output card redundancy setting
                	56-71		exp4 shelf output card redundancy setting
                	72-143		main and exp output card cardType info
                	144-215		main and exp output card state

                	...
                */
                if (fread((char *) &tmp, sizeof(char), 216, fhd) != 216) {
                    status = ERROR;
                } else {
                    for (i = 0; i < 8; i++)
                        outpCard.main[i].redundancy_enable = tmp[i];
                    for (i = 8; i < 24; i++)
                        outpCard.exp1[i - 8].redundancy_enable = tmp[i];
                    for (i = 24; i < 40; i++)
                        outpCard.exp2[i - 24].redundancy_enable = tmp[i];
                    for (i = 40; i < 56; i++)
                        outpCard.exp3[i - 40].redundancy_enable = tmp[i];
                    for (i = 56; i < 72; i++)
                        outpCard.exp4[i - 56].redundancy_enable = tmp[i];
                    for (i = 72; i < 80; i++)
                        outpCard.main[i - 72].cardType = tmp[i];
                    for (i = 80; i < 96; i++)
                        outpCard.exp1[i - 80].cardType = tmp[i];
                    for (i = 96; i < 112; i++)
                        outpCard.exp2[i - 96].cardType = tmp[i];
                    for (i = 112; i < 128; i++)
                        outpCard.exp3[i - 112].cardType = tmp[i];
                    for (i = 128; i < 144; i++)
                        outpCard.exp4[i - 128].cardType = tmp[i];
                    for (i = 144; i < 152; i++)
                        outpCard.main[i - 144].redundancy_state = tmp[i];
                    for (i = 152; i < 168; i++)
                        outpCard.exp1[i - 152].redundancy_state = tmp[i];
                    for (i = 168; i < 184; i++)
                        outpCard.exp2[i - 168].redundancy_state = tmp[i];
                    for (i = 184; i < 200; i++)
                        outpCard.exp3[i - 184].redundancy_state = tmp[i];
                    for (i = 200; i < 216; i++)
                        outpCard.exp4[i - 200].redundancy_state = tmp[i];
                }
                fclose(fhd);
                break;
            case SM_MAIN_OUTPUT_PORT:
                if (fread((char *) &tmp, sizeof(char), 48, fhd) != 48) {
                    status = ERROR;
                } else {
                    for (slot = 0; slot < 8; slot++) {
                        outpCard.main[slot].reg0 = tmp[slot * 6 + 0];
                        outpCard.main[slot].reg1 = tmp[slot * 6 + 1];
                        outpCard.main[slot].reg2 = tmp[slot * 6 + 2];
                        outpCard.main[slot].reg3 = tmp[slot * 6 + 3];
                        outpCard.main[slot].reg4 = tmp[slot * 6 + 4];
                        outpCard.main[slot].reg5 = tmp[slot * 6 + 5];
                        if (outpCard.main[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.main[slot].reg2 & (1 << port))
                                    outpCard.main[slot].portType[port] =
                                        OUTPUT_T1;

                                else if (outpCard.main[slot].
                                         reg3 & (1 << port))
                                    outpCard.main[slot].portType[port] =
                                        OUTPUT_E1;

                                else if (outpCard.main[slot].
                                         reg4 & (1 << port))
                                    outpCard.main[slot].portType[port] =
                                        OUTPUT_SQW;

                                else if (outpCard.main[slot].
                                         reg5 & (1 << port))
                                    outpCard.main[slot].portType[port] =
                                        OUTPUT_CC;
                            }
                        } else if (outpCard.main[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.main[slot].reg2 & (1 << port))
                                    outpCard.main[slot].portType[port] =
                                        OUTPUT_1PPS;

                                else if (outpCard.main[slot].
                                         reg3 & (1 << port))
                                    outpCard.main[slot].portType[port] =
                                        OUTPUT_1PPM;

                                else if (outpCard.main[slot].
                                         reg4 & (1 << port))
                                    outpCard.main[slot].portType[port] =
                                        OUTPUT_1PPH;
                            }
                        } else {
                            for (port = 0; port < 8; port++)
                                outpCard.main[slot].portType[port] =
                                    OUTPUT_NULL;
                        }
                    }
                }
                fclose(fhd);
                break;
            case SM_EXP1_OUTPUT_PORT:
                if (fread((char *) &tmp, sizeof(char), 96, fhd) != 96) {
                    status = ERROR;
                } else {
                    for (slot = 0; slot < 16; slot++) {
                        outpCard.exp1[slot].reg0 = tmp[slot * 6 + 0];
                        outpCard.exp1[slot].reg1 = tmp[slot * 6 + 1];
                        outpCard.exp1[slot].reg2 = tmp[slot * 6 + 2];
                        outpCard.exp1[slot].reg3 = tmp[slot * 6 + 3];
                        outpCard.exp1[slot].reg4 = tmp[slot * 6 + 4];
                        outpCard.exp1[slot].reg5 = tmp[slot * 6 + 5];
                        if (outpCard.exp1[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.exp1[slot].reg2 & (1 << port))
                                    outpCard.exp1[slot].portType[port] =
                                        OUTPUT_T1;

                                else if (outpCard.exp1[slot].
                                         reg3 & (1 << port))
                                    outpCard.exp1[slot].portType[port] =
                                        OUTPUT_E1;

                                else if (outpCard.exp1[slot].
                                         reg4 & (1 << port))
                                    outpCard.exp1[slot].portType[port] =
                                        OUTPUT_SQW;

                                else if (outpCard.exp1[slot].
                                         reg5 & (1 << port))
                                    outpCard.exp1[slot].portType[port] =
                                        OUTPUT_CC;
                            }
                        } else if (outpCard.exp1[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.exp1[slot].reg2 & (1 << port))
                                    outpCard.exp1[slot].portType[port] =
                                        OUTPUT_1PPS;

                                else if (outpCard.exp1[slot].
                                         reg3 & (1 << port))
                                    outpCard.exp1[slot].portType[port] =
                                        OUTPUT_1PPM;

                                else if (outpCard.exp1[slot].
                                         reg4 & (1 << port))
                                    outpCard.exp1[slot].portType[port] =
                                        OUTPUT_1PPH;
                            }
                        } else {
                            for (port = 0; port < 8; port++)
                                outpCard.exp1[slot].portType[port] =
                                    OUTPUT_NULL;
                        }
                    }
                }
                fclose(fhd);
                break;
            case SM_EXP2_OUTPUT_PORT:
                if (fread((char *) &tmp, sizeof(char), 128, fhd) != 128) {
                    status = ERROR;
                } else {
                    for (slot = 0; slot < 16; slot++) {
                        outpCard.exp2[slot].reg0 = tmp[slot * 6 + 0];
                        outpCard.exp2[slot].reg1 = tmp[slot * 6 + 1];
                        outpCard.exp2[slot].reg2 = tmp[slot * 6 + 2];
                        outpCard.exp2[slot].reg3 = tmp[slot * 6 + 3];
                        outpCard.exp2[slot].reg4 = tmp[slot * 6 + 4];
                        outpCard.exp2[slot].reg5 = tmp[slot * 6 + 5];
                        if (outpCard.exp2[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.exp2[slot].reg2 & (1 << port))
                                    outpCard.exp2[slot].portType[port] =
                                        OUTPUT_T1;

                                else if (outpCard.exp2[slot].
                                         reg3 & (1 << port))
                                    outpCard.exp2[slot].portType[port] =
                                        OUTPUT_E1;

                                else if (outpCard.exp2[slot].
                                         reg4 & (1 << port))
                                    outpCard.exp2[slot].portType[port] =
                                        OUTPUT_SQW;

                                else if (outpCard.exp2[slot].
                                         reg5 & (1 << port))
                                    outpCard.exp2[slot].portType[port] =
                                        OUTPUT_CC;
                            }
                        } else if (outpCard.exp2[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.exp2[slot].reg2 & (1 << port))
                                    outpCard.exp2[slot].portType[port] =
                                        OUTPUT_1PPS;

                                else if (outpCard.exp2[slot].
                                         reg3 & (1 << port))
                                    outpCard.exp2[slot].portType[port] =
                                        OUTPUT_1PPM;

                                else if (outpCard.exp2[slot].
                                         reg4 & (1 << port))
                                    outpCard.exp2[slot].portType[port] =
                                        OUTPUT_1PPH;
                            }
                        } else {
                            for (port = 0; port < 8; port++)
                                outpCard.exp2[slot].portType[port] =
                                    OUTPUT_NULL;
                        }
                    }
                }
                fclose(fhd);
                break;
            case SM_EXP3_OUTPUT_PORT:
                if (fread((char *) &tmp, sizeof(char), 128, fhd) != 128) {
                    status = ERROR;
                } else {
                    for (slot = 0; slot < 16; slot++) {
                        outpCard.exp3[slot].reg0 = tmp[slot * 6 + 0];
                        outpCard.exp3[slot].reg1 = tmp[slot * 6 + 1];
                        outpCard.exp3[slot].reg2 = tmp[slot * 6 + 2];
                        outpCard.exp3[slot].reg3 = tmp[slot * 6 + 3];
                        outpCard.exp3[slot].reg4 = tmp[slot * 6 + 4];
                        outpCard.exp3[slot].reg5 = tmp[slot * 6 + 5];
                        if (outpCard.exp3[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.exp3[slot].reg2 & (1 << port))
                                    outpCard.exp3[slot].portType[port] =
                                        OUTPUT_T1;

                                else if (outpCard.exp3[slot].
                                         reg3 & (1 << port))
                                    outpCard.exp3[slot].portType[port] =
                                        OUTPUT_E1;

                                else if (outpCard.exp3[slot].
                                         reg4 & (1 << port))
                                    outpCard.exp3[slot].portType[port] =
                                        OUTPUT_SQW;

                                else if (outpCard.exp3[slot].
                                         reg5 & (1 << port))
                                    outpCard.exp3[slot].portType[port] =
                                        OUTPUT_CC;
                            }
                        } else if (outpCard.exp3[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.exp3[slot].reg2 & (1 << port))
                                    outpCard.exp3[slot].portType[port] =
                                        OUTPUT_1PPS;

                                else if (outpCard.exp3[slot].
                                         reg3 & (1 << port))
                                    outpCard.exp3[slot].portType[port] =
                                        OUTPUT_1PPM;

                                else if (outpCard.exp3[slot].
                                         reg4 & (1 << port))
                                    outpCard.exp3[slot].portType[port] =
                                        OUTPUT_1PPH;
                            }
                        } else {
                            for (port = 0; port < 8; port++)
                                outpCard.exp3[slot].portType[port] =
                                    OUTPUT_NULL;
                        }
                    }
                }
                fclose(fhd);
                break;
            case SM_EXP4_OUTPUT_PORT:
                if (fread((char *) &tmp, sizeof(char), 128, fhd) != 128) {
                    status = ERROR;
                } else {
                    for (slot = 0; slot < 16; slot++) {
                        outpCard.exp4[slot].reg0 = tmp[slot * 6 + 0];
                        outpCard.exp4[slot].reg1 = tmp[slot * 6 + 1];
                        outpCard.exp4[slot].reg2 = tmp[slot * 6 + 2];
                        outpCard.exp4[slot].reg3 = tmp[slot * 6 + 3];
                        outpCard.exp4[slot].reg4 = tmp[slot * 6 + 4];
                        outpCard.exp4[slot].reg5 = tmp[slot * 6 + 5];
                        if (outpCard.exp4[slot].cardType ==
                            OUTPUT_FREQUENCY) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.exp4[slot].reg2 & (1 << port))
                                    outpCard.exp4[slot].portType[port] =
                                        OUTPUT_T1;

                                else if (outpCard.exp4[slot].
                                         reg3 & (1 << port))
                                    outpCard.exp4[slot].portType[port] =
                                        OUTPUT_E1;

                                else if (outpCard.exp4[slot].
                                         reg4 & (1 << port))
                                    outpCard.exp4[slot].portType[port] =
                                        OUTPUT_SQW;

                                else if (outpCard.exp4[slot].
                                         reg5 & (1 << port))
                                    outpCard.exp4[slot].portType[port] =
                                        OUTPUT_CC;
                            }
                        } else if (outpCard.exp4[slot].cardType ==
                                   OUTPUT_TIMING) {
                            for (port = 0; port < 8; port++) {
                                if (outpCard.exp4[slot].reg2 & (1 << port))
                                    outpCard.exp4[slot].portType[port] =
                                        OUTPUT_1PPS;

                                else if (outpCard.exp4[slot].
                                         reg3 & (1 << port))
                                    outpCard.exp4[slot].portType[port] =
                                        OUTPUT_1PPM;

                                else if (outpCard.exp4[slot].
                                         reg4 & (1 << port))
                                    outpCard.exp4[slot].portType[port] =
                                        OUTPUT_1PPH;
                            }
                        } else {
                            for (port = 0; port < 8; port++)
                                outpCard.exp4[slot].portType[port] =
                                    OUTPUT_NULL;
                        }
                    }
                }
                fclose(fhd);
                break;
            default:
                break;
            }
        }
    } else if (mode == FLASH_WRITE) {
        if ((fhd = fopen(fname, "w")) == NULL)
            status = ERROR;

        else {
            switch (dataType) {
            case SM_SHELF:
                /* 5*(1+13+1+13)=130
                		#define SM_SHELF_SERIAL_LEN		13
						#define	SM_SHELF_CLEICODE_LEN	13 
                	byte		content
                	0			shelf 0 serial length
                	1-13		shelf 0 serial char
                	14			shelf 0 clei length
                	15-27		shelf 0 clei char
                	28			shelf 1 serial length
                	29-41		shelf 1 serial char
                	...
                */
                for (i = 0; i < NUMBER_SHELVES; i++) {
                    tmp[i * 28] = shelves[i].serial.length;
                    tmp[i * 28 + 14] = shelves[i].cleicode.length;

                    memcpy(tmp + i * 28 + 1, shelves[i].serial.octet_ptr,
                           shelves[i].serial.length);
                    memcpy(tmp + i * 28 + 15,
                           shelves[i].cleicode.octet_ptr,
                           shelves[i].cleicode.length);
                }
                fclose(fhd);
                break;

            case SM_OUTPUT_CARD:
                for (slot = 0; slot < 8; slot++)
                    tmp[slot] = outpCard.main[slot].redundancy_enable;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 8] = outpCard.exp1[slot].redundancy_enable;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 24] = outpCard.exp2[slot].redundancy_enable;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 40] = outpCard.exp3[slot].redundancy_enable;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 56] = outpCard.exp4[slot].redundancy_enable;
                for (slot = 0; slot < 8; slot++)
                    tmp[slot + 72] = outpCard.main[slot].cardType;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 80] = outpCard.exp1[slot].cardType;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 96] = outpCard.exp2[slot].cardType;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 112] = outpCard.exp3[slot].cardType;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 128] = outpCard.exp4[slot].cardType;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 144] = outpCard.main[slot].redundancy_state;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 152] = outpCard.exp1[slot].redundancy_state;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 168] = outpCard.exp2[slot].redundancy_state;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 184] = outpCard.exp3[slot].redundancy_state;
                for (slot = 0; slot < 16; slot++)
                    tmp[slot + 200] = outpCard.exp4[slot].redundancy_state;
                fwrite((char *) &tmp, sizeof(char), 216, fhd);
                fclose(fhd);
                break;
            case SM_MAIN_OUTPUT_PORT:
                for (slot = 0; slot < 8; slot++) {
                    tmp[slot * 6 + 0] = outpCard.main[slot].reg0;
                    tmp[slot * 6 + 1] = outpCard.main[slot].reg1;
                    tmp[slot * 6 + 2] = outpCard.main[slot].reg2;
                    tmp[slot * 6 + 3] = outpCard.main[slot].reg3;
                    tmp[slot * 6 + 4] = outpCard.main[slot].reg4;
                    tmp[slot * 6 + 5] = outpCard.main[slot].reg5;
                }
                fwrite((char *) &tmp, sizeof(char), 48, fhd);
                fclose(fhd);
                break;
            case SM_EXP1_OUTPUT_PORT:
                for (slot = 0; slot < 16; slot++) {
                    tmp[slot * 6 + 0] = outpCard.exp1[slot].reg0;
                    tmp[slot * 6 + 1] = outpCard.exp1[slot].reg1;
                    tmp[slot * 6 + 2] = outpCard.exp1[slot].reg2;
                    tmp[slot * 6 + 3] = outpCard.exp1[slot].reg3;
                    tmp[slot * 6 + 4] = outpCard.exp1[slot].reg4;
                    tmp[slot * 6 + 5] = outpCard.exp1[slot].reg5;
                }
                fwrite((char *) &tmp, sizeof(char), 96, fhd);
                fclose(fhd);
                break;
            case SM_EXP2_OUTPUT_PORT:
                for (slot = 0; slot < 16; slot++) {
                    tmp[slot * 6 + 0] = outpCard.exp2[slot].reg0;
                    tmp[slot * 6 + 1] = outpCard.exp2[slot].reg1;
                    tmp[slot * 6 + 2] = outpCard.exp2[slot].reg2;
                    tmp[slot * 6 + 3] = outpCard.exp2[slot].reg3;
                    tmp[slot * 6 + 4] = outpCard.exp2[slot].reg4;
                    tmp[slot * 6 + 5] = outpCard.exp2[slot].reg5;
                }
                fwrite((char *) &tmp, sizeof(char), 96, fhd);
                fclose(fhd);
                break;
            case SM_EXP3_OUTPUT_PORT:
                for (slot = 0; slot < 16; slot++) {
                    tmp[slot * 6 + 0] = outpCard.exp3[slot].reg0;
                    tmp[slot * 6 + 1] = outpCard.exp3[slot].reg1;
                    tmp[slot * 6 + 2] = outpCard.exp3[slot].reg2;
                    tmp[slot * 6 + 3] = outpCard.exp3[slot].reg3;
                    tmp[slot * 6 + 4] = outpCard.exp3[slot].reg4;
                    tmp[slot * 6 + 5] = outpCard.exp3[slot].reg5;
                }
                fwrite((char *) &tmp, sizeof(char), 96, fhd);
                fclose(fhd);
                break;
            case SM_EXP4_OUTPUT_PORT:
                for (slot = 0; slot < 16; slot++) {
                    tmp[slot * 6 + 0] = outpCard.exp4[slot].reg0;
                    tmp[slot * 6 + 1] = outpCard.exp4[slot].reg1;
                    tmp[slot * 6 + 2] = outpCard.exp4[slot].reg2;
                    tmp[slot * 6 + 3] = outpCard.exp4[slot].reg3;
                    tmp[slot * 6 + 4] = outpCard.exp4[slot].reg4;
                    tmp[slot * 6 + 5] = outpCard.exp4[slot].reg5;
                }
                fwrite((char *) &tmp, sizeof(char), 96, fhd);
                fclose(fhd);
                break;
            default:
                break;
            }
        }
    }
    return status;
}


/********************************************************************
* function: STATUS_t SMUPresent(void)
* input parameter: void
* return
*  TRUE: if SMU Card exists
*  FALSE: if SMU card not exist
*********************************************************************/
STATUS_t SMUPresent(void)
{
    U8_t tmpData;
    tmpData = *(U8_t *) SMU_FRAMER_DEV_ID_ADDR;
    /*checkitout waiting david for hardware detection spec */
    tmpData &= SMU_FRAMER_DEV_ID_MASK;
    if (tmpData == SMU_FRAMER_DEV_ID_MASK) {
        return TRUE;
    } else {
        return ERROR;
    }
}




/********************************************************************
* function: portAlarmSummary
* input OUTPUT_CARD_PARA_t *thisCard;
* return
*  TRUE: if SMU Card exists
*  FALSE: if SMU card not exist
*********************************************************************/
int portAlarmSummary(OUTPUT_CARD_PARA_t * thisCard, int port)
{
    U8_t tmpSeverity = D_stsOutputAlarmSettingSeverity_nr;



    if ((thisCard->portAlarmSetting[port].outputAlarm_lossClock ==
         OUTPUT_ALARM_ON)
        && (thisCard->portAlarmSetting[port].outputAlarm_lossClock <
            tmpSeverity)) {
        tmpSeverity =
            thisCard->portAlarmSetting[port].outputAlarm_lossClock;
    }
    if ((thisCard->portAlarmStatus[port].outputAlarm_tiu_act ==
         OUTPUT_ALARM_ON)
        && (thisCard->portAlarmSetting[port].outputAlarm_tiu_act <
            tmpSeverity)) {
        tmpSeverity = thisCard->portAlarmSetting[port].outputAlarm_tiu_act;
    }
    if ((thisCard->portAlarmStatus[port].outputAlarm_tiu_los ==
         OUTPUT_ALARM_ON)
        && (thisCard->portAlarmSetting[port].outputAlarm_tiu_los <
            tmpSeverity)) {
        tmpSeverity = thisCard->portAlarmSetting[port].outputAlarm_tiu_los;
    }
    if ((thisCard->portAlarmStatus[port].outputAlarm_tiu_t_tie ==
         OUTPUT_ALARM_ON)
        && (thisCard->portAlarmSetting[port].outputAlarm_tiu_t_tie <
            tmpSeverity)) {
        tmpSeverity =
            thisCard->portAlarmSetting[port].outputAlarm_tiu_t_tie;
    }
    if ((thisCard->portAlarmStatus[port].outputAlarm_tiu_t_mtie ==
         OUTPUT_ALARM_ON)
        && (thisCard->portAlarmSetting[port].outputAlarm_tiu_t_mtie <
            tmpSeverity)) {
        tmpSeverity =
            thisCard->portAlarmSetting[port].outputAlarm_tiu_t_mtie;
    }
    if ((thisCard->portAlarmStatus[port].outputAlarm_tiu_aisp ==
         OUTPUT_ALARM_ON)
        && (thisCard->portAlarmSetting[port].outputAlarm_tiu_aisp <
            tmpSeverity)) {
        tmpSeverity =
            thisCard->portAlarmSetting[port].outputAlarm_tiu_aisp;
    }
    if ((thisCard->portAlarmStatus[port].outputAlarm_tiu_lof ==
         OUTPUT_ALARM_ON)
        && (thisCard->portAlarmSetting[port].outputAlarm_tiu_lof <
            tmpSeverity)) {
        tmpSeverity = thisCard->portAlarmSetting[port].outputAlarm_tiu_lof;
    }
    if ((thisCard->portAlarmStatus[port].outputAlarm_tiu_t_cs ==
         OUTPUT_ALARM_ON)
        && (thisCard->portAlarmSetting[port].outputAlarm_tiu_t_cs <
            tmpSeverity)) {
        tmpSeverity =
            thisCard->portAlarmSetting[port].outputAlarm_tiu_t_cs;
    }

    return tmpSeverity;



}

/********************************************************************
* function: initShelfMangerGlobalVariable
* 	this function is to malloc the octet string of global variable 
*	used in shelf manager and expansion shelf. this init should be done only once. so
*	the static local variable is used here. note. some check for malloc return value
* 	is needed, but missing.
* return
*  OK
*  ERROR 
*********************************************************************/
STATUS_t smVariableInit()
{
    int slot,i;

    static U8_t malloc_counter=0;




	printf("smdebug: init the global variable [%d]\n", malloc_counter);
	if (malloc_counter ==0){
	    /* zero out */
	    memset(&outpCard, 0, sizeof(OUTPUT_CARD_t));
	    memset(&shelves, 0, sizeof(SHELFS_t)*NUMBER_SHELVES);
	    
	    /* shelves variable */
	    for (i = 1; i < NUMBER_SHELVES; i++) {
	        shelves[i].serial.octet_ptr 	= malloc(SM_SHELF_SERIAL_LEN);
	        shelves[i].cleicode.octet_ptr 	= malloc(SM_SHELF_CLEICODE_LEN);
	        
	    }
	    /* output card in main shelf */
	    for (slot = 0; slot < 8; slot++) {
	    	
	            outpCard.main[slot].cardDescr.octet_ptr =
	                malloc(SM_CARD_DESCRIPTON_LEN);
	            outpCard.main[slot].cardHwVersion.octet_ptr =
	                malloc(SM_CARD_HW_VERSION_LEN);
	            outpCard.main[slot].cardSerial.octet_ptr =
	                malloc(SM_CARD_SERIAL_LEN);
	            outpCard.main[slot].cardCLEICode.octet_ptr =
	                malloc(SM_CARD_CLEICODE_LEN);
	
	
	    }
	    /* output card in all expansion shelf*/
	    for (slot = 0; slot < 16; slot++) { 
	            outpCard.exp1[slot].cardDescr.octet_ptr =
	                malloc(SM_CARD_DESCRIPTON_LEN);
	            outpCard.exp1[slot].cardHwVersion.octet_ptr =
	                malloc(SM_CARD_HW_VERSION_LEN);
	            outpCard.exp1[slot].cardSerial.octet_ptr =
	                malloc(SM_CARD_SERIAL_LEN);
	            outpCard.exp1[slot].cardCLEICode.octet_ptr =
	                malloc(SM_CARD_CLEICODE_LEN);
	 
	 
	 
	            outpCard.exp2[slot].cardDescr.octet_ptr =
	                malloc(SM_CARD_DESCRIPTON_LEN);
	            outpCard.exp2[slot].cardHwVersion.octet_ptr =
	                malloc(SM_CARD_HW_VERSION_LEN);
	            outpCard.exp2[slot].cardSerial.octet_ptr =
	                malloc(SM_CARD_SERIAL_LEN);
	            outpCard.exp2[slot].cardCLEICode.octet_ptr =
	                malloc(SM_CARD_CLEICODE_LEN);
	 
	 
	            outpCard.exp3[slot].cardDescr.octet_ptr =
	                malloc(SM_CARD_DESCRIPTON_LEN);
	            outpCard.exp3[slot].cardHwVersion.octet_ptr =
	                malloc(SM_CARD_HW_VERSION_LEN);
	            outpCard.exp3[slot].cardSerial.octet_ptr =
	                malloc(SM_CARD_SERIAL_LEN);
	            outpCard.exp3[slot].cardCLEICode.octet_ptr =
	                malloc(SM_CARD_CLEICODE_LEN);
	
	
	            outpCard.exp4[slot].cardDescr.octet_ptr =
	                malloc(SM_CARD_DESCRIPTON_LEN);
	            outpCard.exp4[slot].cardHwVersion.octet_ptr =
	                malloc(SM_CARD_HW_VERSION_LEN);
	            outpCard.exp4[slot].cardSerial.octet_ptr =
	                malloc(SM_CARD_SERIAL_LEN);
	            outpCard.exp4[slot].cardCLEICode.octet_ptr =
	                malloc(SM_CARD_CLEICODE_LEN);
	                
	    }
	    /* those special card */
    	
	            outpCard.smu.cardDescr.octet_ptr =
	                malloc(SM_CARD_DESCRIPTON_LEN);
	            outpCard.smu.cardHwVersion.octet_ptr =
	                malloc(SM_CARD_HW_VERSION_LEN);
	            outpCard.smu.cardSerial.octet_ptr =
	                malloc(SM_CARD_SERIAL_LEN);
	            outpCard.smu.cardCLEICode.octet_ptr =
	                malloc(SM_CARD_CLEICODE_LEN);

	            outpCard.top.cardDescr.octet_ptr =
	                malloc(SM_CARD_DESCRIPTON_LEN);
	            outpCard.top.cardHwVersion.octet_ptr =
	                malloc(SM_CARD_HW_VERSION_LEN);
	            outpCard.top.cardSerial.octet_ptr =
	                malloc(SM_CARD_SERIAL_LEN);
	            outpCard.top.cardCLEICode.octet_ptr =
	                malloc(SM_CARD_CLEICODE_LEN);
	                
	            outpCard.imu.cardDescr.octet_ptr =
	                malloc(SM_CARD_DESCRIPTON_LEN);
	            outpCard.imu.cardHwVersion.octet_ptr =
	                malloc(SM_CARD_HW_VERSION_LEN);
	            outpCard.imu.cardSerial.octet_ptr =
	                malloc(SM_CARD_SERIAL_LEN);
	            outpCard.imu.cardCLEICode.octet_ptr =
	                malloc(SM_CARD_CLEICODE_LEN);


    }
    malloc_counter++;
	
	return OK;
}
