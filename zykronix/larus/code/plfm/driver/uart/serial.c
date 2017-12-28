/*
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: serial.h
 *
 *Define all Event objects for serial Module
 *
 *Note:
 *
 *Author(s):
 *  shuming.chang
 *  Klim and Betta 
 *
 *********************************************************************
 * $Id: serial.c,v 1.57 2006/10/05 06:27:24 klim Exp $
 * $Source: /cvsroot/larus5820/code/plfm/driver/uart/serial.c,v $
 *
 *********************************************************************/

#include <string.h>
#include <stdio.h>
#include "serial_if.h"
#include "uartdrv.h"
#include <stdlib.h>
#include "logLib.h"
#include <tss_types.h>
#include <trapdef.h>
#include "drv/parallel/m8260IOPort.h"
#include "config.h"

#define gpsDevNum 3
#define signallimit 80
extern int getRedState(int stsCardIndex, int whichlink);

/* API scc API */
static void idxCalculate(char *cmdString, int len, int *idx, DEV_TYPE dev);

/*  API - gps */
static int gpsSatelliteInfo(serialGpsGpwav_t * gpwavData, int port);
static int gpsSelfTestResult(serialGpsGptst_t * gptstData, int port);
static int gpsConductSelfTest(int port);
static int gpsSignalStrength(int port, int range);
static int gpsCableDelay(int port, int range);

/*static int gpsRePara(int port);*/
static int gptstParsing(char *string2Parsed, serialGpsGptst_t * gptstData);
static int gpwavParsing(char *string2Parsed, serialGpsGpwav_t * gpwavData);
static int gpggaParsing(char *string2Parsed, serialGpsGpgga_t * gpggaData);
static int gpzdaParsing(char *string2Parsed, serialGpsGpzda_t * gpzdaData);
static int gpgsvParsing(char *string2Parsed, serialGpsGpgsv_t * gpgsvData);

/* jupiter 30 */
static int gpgll30Parsing(char *string2Parsed,
                          serialGpsGpgll_t * gpgllData);
static int gpmss30Parsing(char *string2Parsed,
                          serialGpsGpmss_t * gpmssData);
static int gpgga30Parsing(char *string2Parsed,
                          serialGpsGpgga_t * gpggaData);

/* jupiter pico t*/
static int picoteaParsing(char *string2Parsed, serialGpsGpgga_t * eaData);
static int picotazParsing(char *string2Parsed,
                          serialGpsPicoANTD_t * azData);
static int picotbbParsing(char *string2Parsed,
                          serialGpsPicoAZEL_t * bbData);

/* set the position */
static int xxggaParsing(char *string2Parsed, serialGpsXXGGA_t * xxggaData);

/*static int spos30Parsing(char *string2Parsed, serialGpsSPOS30_t * spos30Data);
static int gpclrParsing(int string2Parsed, serialGpsGpclr_t * gpclrData);*/
static int sccGPGGA(serialGpsGpgga_t * my_gpggaData, int port);
static int sccGPZDA(serialGpsGpzda_t * my_gpzdaData, int port);
static int GPZDA30(serialGpsGpzda_t * my_gpzdaData, int port);
static int sccGPGSV(serialGpsGpgsv_t * my_gpgsvData, int port);
static int sccXXGGA(char *string2Parsed, serialGpsXXGGA_t * xxggaData,
                    int port);
static int XXGGA30(char *string2Parsed, serialGpsSPOS30_t * spos30Data,
                   int port);
static int sccGTReset(void *input, int portNum, stsGpsEntry_t * entry);
static int GTReset(serialGpsGpclr_t * my_gpclrData, int port);

/* jupiter 30 */
static int GPGGA30(serialGpsGpgga_t * my_gpggaData, int port);
static int GPGSV30(serialGpsGpgsv_t * my_gpgsvData, int port);
static int GPGLL30(serialGpsGpgll_t * my_gpgllData, int port);
static int GPMSS30(serialGpsGpmss_t * my_gpmssData, int port);

static int gpsReset30(void *input, int portNum, stsGpsEntry_t * entry);
static int Reset30(serialGpsSPOS30_t * my_gpclrData, int port);

/* jupiter pico t*/
static int EApicot(serialGpsGpgga_t * my_eapicotData, int port);
static int AZpicot(serialGpsPicoANTD_t * my_azpicotData, int port);
static int BBpicot(serialGpsPicoAZEL_t * my_bbpicotData, int port);

/* API - CDMA */
static int fltstatPasing(char *respond);
static int cdmaStatus(int port);
static int cdmaTime(int port, serialCDMAtime_t * CDMAtimeData);
static int cdmaSetPSSWidth(int wid, int port);
static int cdmaAckPasing(char *rxBuf);
static int cdmaReset(int port);
static int cdmaVer(int port, char *ver);
static int cdmaLeap(int port, int curleap, int futleap);
static int CDMAtimeParsing(char *string2Parsed,
                           serialCDMAtime_t * CDMAtimeData);

/* API - Rubidium\Crystal */
static int idPasing(char *string2Parsed, serialID_t * idData);
static int rb_or_cryID(int port, serialID_t * idData, DEV_TYPE devType);
static int statusPasing(char *string2Parsed, int *statusData);
static int rb_or_cryStatus(int port, int *statusData);
static int snPasing(char *string2Parsed, int snData);
static int rb_or_crySerNum(int port, long *snData);
static int rb_or_cryViewTimeConst(int port, long *vtData);
static int rb_or_crySetTC(int port, char *cmdString, int *tcData);
static int rb_or_crySetTW(int port, char *cmdString, int *twData);
static int rb_or_cryRawPA(int port, char *cmdString, int *raData);

/*static int rb_or_crySetTrack(int port, int *trackStatus);*/
static int rb_or_cryTrackFreeRun(int port, int *trackStatus);
static int TR3Pasing(char *string2Parsed, int *trackStatus);

/*static int rb_or_crySetSync(int port, int *syncStatus);*/
static int rb_or_crySetSyncFreeRun(int port, int *syncStatus);
static int rb_or_cryBeatSec(int port, serialBS_t * bsData);
static int rb_or_cryStopBeatSec(int port, serialBS_t * bsData);
static int rb_or_cryPPSBeatSec(int port, serialBS_t * bsData);
static int SY3Pasing(char *string2Parsed, int *syncStatus);
static int BT3Pasing(char *string2Parsed, serialBS_t * btData);
static int VTPasing(char *string2Parsed, long vtData);
static int TCPasing(char *string2Parsed, int tcData);
static int TWPasing(char *string2Parsed, int twData);
static int RAPasing(char *string2Parsed, int raData);
static int rb_or_cryReset(int port);

static int gpsdummy();
static int oscdummy();
static int gpsnullget(stsGpsEntry_t * entry);
static int oscnullget(stsOscillatorEntry_t * entry);
static int gpsGetGT(void *input, int portNum, stsGpsEntry_t * entry);
static int gpsGet30(void *input, int portNum, stsGpsEntry_t * entry);
static int gpsGetPico(void *input, int portNum, stsGpsEntry_t * entry);
static int gpsSetGT(void *input, int portNum, stsGpsEntry_t * entry);
static int gpsSet30(void *input, int portNum, stsGpsEntry_t * entry);
static int gpsSetPico(void *input, int portNum, stsGpsEntry_t * entry);
static int oscGet1(void *input, int portNum, stsOscillatorEntry_t * entry);
static int oscSet1(void *input, int portNum, stsOscillatorEntry_t * entry);
static int getGpsRedState(int portNum);

/*static STATUS_t tlDataAccess(stsGpsEntry_t *input, char *fname, int mode);*/
int serialDebug = 0;
static char positionReadGT[60] =
    "latitude:4888.0770,N,longtitude:12345.4700,E";
static char positionRead30[50];
static int oscStatus = 0;
static unsigned flagexist_scc1 = 0;
static unsigned flagexist_smc1 = 0;
static unsigned flagexist_scc3 = 0;     /* a flag to identify the device is exist */
static unsigned flagexist_scc4 = 0;
unsigned devIdxScc1 = 0;
static unsigned devIdxSmc1 = 0;
unsigned devIdxScc3 = 0;
static unsigned devIdxScc4 = 0;
static unsigned markdebug = 0;

#ifdef SERIAL_DEBUG
/* API scc API */
void sccSetBaudRate9600(int port);
void sccSetBaudRate4800(int port);
void test_SelfTestResult();
void scc1RbID();
void scc3RbID();
void scc1RbStatus();
void scc3RbStatus();
void Scc1SetTrack();
void Scc3SetTrack();
void scc1SetSync();
void scc3SetSync();
void scc1RbReset();
void scc3RbReset();

#endif

/* osc function maping table */
/* 
	structure filed : 
	#1.osc query function,
	#2.osc get function,
	#3.osc set function,
    #4.parameter module     
*/
oscDevOperation_t oscModule[] = {
    {
     rb_or_cryID, oscGet1, oscSet1, (void *) &osc1getset_t},    /* osc1 */
    {
     rb_or_cryID, oscGet1, oscSet1, (void *) &osc2getset_t},    /* osc2 */
    {
     oscdummy, NULL, NULL, NULL}        /* NULL for the last row */
};


/* for the Null array use */
int oscdummy()
{

    return NO_OSC_DEVICE;
}


/* gps function maping table */
/* 
	structure filed : 
	#1.gps query function,
	#2.gps get function,
	#3.gps set function,
    #4.parameter module     
*/
gpsDevOperation_t gpsModule[] = {
    {
     sccGPGGA, gpsGetGT, gpsSetGT, sccGTReset,(void *) &gpsGT8031getset_t},        /* GT8031 */
    {
     GPGGA30, gpsGet30, gpsSet30, gpsReset30,(void *) &gps30getset_t},     /* jupiter 30 */
    {
     EApicot, gpsGetPico, gpsSetPico, NULL,(void *) &gpsPicogetset_t},       /* jupiter pico t */
    {
     gpsdummy, NULL, NULL, NULL, NULL}        /* NULL for the last row */
};


/* for the Null array use */
int gpsdummy()
{
    return NO_GPS_DEVICE;
}


/* serial command maping table */
/* 
	structure filed : 
	#1.command string header,
	#2.command API name,
	#3.parse respose string function,
    #4.serial device type    
*/
UART_APP_t uartApp[] = {

    /* jupiter pico t */
    {
     "@@Ea0*54\r\n", EApicot, picoteaParsing, gpsDev}
    ,
    {
     "@@Az0*0b\r\n", AZpicot, picotazParsing, gpsDev}
    ,
    {
     "@@Bb0*10\r\n", BBpicot, picotbbParsing, gpsDev}
    ,
    /* jupiter 30 */
    {
     "$PSRF103,00,01,00,01*25\r\n", GPGGA30, gpgga30Parsing, gpsDev}
    ,
    {
     "$PSRF103,03,01,00,01*26\r\n", GPGSV30, gpgsvParsing, gpsDev}
    ,
    {
     "$PSRF103,01,01,00,01*24\r\n", GPGLL30, gpgll30Parsing, gpsDev}
    ,
    {
     "$PSRF103,06,01,00,01*23\r\n", GPMSS30, gpmss30Parsing, gpsDev}
    ,
    {
     "$PSRF104,,\r\n", XXGGA30, NULL, gpsDev}
    ,
    /* GT8031 */
    {
     "$PFEC,GPint,GGA00\r\n", sccGPGGA, gpggaParsing, gpsDev}
    ,
    {
     "$PFEC,GPint,ZDA00\r\n", sccGPZDA, gpzdaParsing, gpsDev}
    ,
    {
     "$PSRF103,08,01,00,01*2d\r\n", GPZDA30, gpzdaParsing, gpsDev}
    ,
    {
     "$PFEC,GPint,GSV00\r\n", sccGPGSV, gpgsvParsing, gpsDev}
    ,
    {
     "$XXGGA,,\r\n", sccXXGGA, NULL, gpsDev}
    ,
    {
     "$PFEC,GPint,tst00\r\n", gpsSelfTestResult, gptstParsing, gpsDev}
    ,
    {
     "$PFEC,GPint,wav00\r\n", gpsSatelliteInfo, gpwavParsing, gpsDev}
    ,
    {
     "$PFEC,GPtrq,0\r\n", gpsConductSelfTest, NULL, gpsDev}
    ,
    {
     "$PFEC,GPset,S", gpsSignalStrength, NULL, gpsDev}
    ,
    {
     "$PFEC,GPset,t", gpsCableDelay, NULL, gpsDev}
    ,
    {
     "FLTSTAT\r\n", cdmaStatus, fltstatPasing, cdmaDev}
    ,
    {
     "TIME\r\n", cdmaTime, CDMAtimeParsing, cdmaDev}
    ,
    {
     "CTIME=OFF\r\n", cdmaCTIME, NULL, cdmaDev}
    ,
    {
     "PPSWIDTH", cdmaSetPSSWidth, cdmaAckPasing, cdmaDev}
    ,
    {
     "RESET\r\n", cdmaReset, cdmaAckPasing, cdmaDev}
    ,
    {
     "VER\r\n", cdmaVer, NULL, cdmaDev}
    ,
    {
     "LEAP", cdmaLeap, cdmaAckPasing, cdmaDev}
    ,
    {
     "ID\r\n", rb_or_cryID, idPasing, sroDev}
    ,
    {
     "ID\r\n", rb_or_cryID, idPasing, sxoDev}
    ,
    {
     "ST\r\n", rb_or_cryStatus, statusPasing, sroDev}
    ,
    {
     "SN\r\n", rb_or_crySerNum, snPasing, sroDev}
    ,
    {
     "TR3\r\n", rb_or_crySetTrack, TR3Pasing, sroDev}
    ,
    {
     "TR0\r\n", rb_or_cryTrackFreeRun, TR3Pasing, sroDev}
    ,
    {
     "SY3\r\n", rb_or_crySetSync, SY3Pasing, sroDev}
    ,
    {
     "SY0\r\n", rb_or_crySetSyncFreeRun, SY3Pasing, sroDev}
    ,
    {
     "BT3\r\n", rb_or_cryBeatSec, BT3Pasing, sroDev}
    ,
    {
     "BT0\r\n", rb_or_cryStopBeatSec, BT3Pasing, sroDev}
    ,
    {
     "BT1\r\n", rb_or_cryPPSBeatSec, BT3Pasing, sroDev}
    ,
    {
     "VT\r\n", rb_or_cryViewTimeConst, VTPasing, sroDev}
    ,
    {
     "TC\r\n", rb_or_crySetTC, TCPasing, sroDev}
    ,
    {
     "TW\r\n", rb_or_crySetTW, TWPasing, sroDev}
    ,
    {
     "RA\r\n", rb_or_cryRawPA, RAPasing, sroDev}
    ,
    {
     "RESET\r\n", rb_or_cryReset, NULL, sroDev}
};


#ifdef SERIAL_DEBUG
/*********************************************************************************/
/*************************************
* function: gpsdebugread
*
**************************************/
int gpsdebugread1()
{

    stsGpsEntry_t input;
    input.stsGpsIndex = 1;

    gpsGet(&input);

    if (markdebug == 0) {
        printf("1-index: %d\n", input.stsGpsIndex);
        printf("2-status: %d\n", input.stsGpsStatus);
        printf("3-state: %d\n", input.stsGpsState);
        printf("4-str:%s, the length of it:%d\n",
               input.stsGpsANTD->octet_ptr, input.stsGpsANTD->length);
        printf("5-str:%s, the length of it:%d\n",
               input.stsGpsAZEL->octet_ptr, input.stsGpsAZEL->length);
        printf("6-reset:%s, the length of it:%d\n",
               input.stsGpsRSTG->octet_ptr, input.stsGpsRSTG->length);
        printf("7-self-test results:%s, the length of it:%d\n",
               input.stsGpsBITS->octet_ptr, input.stsGpsBITS->length);
        printf("8-position average status:%s, the length of it:%d\n",
               input.stsGpsPAVG->octet_ptr, input.stsGpsPAVG->length);
        printf("9-signal quality:%s, the length of it:%d\n",
               input.stsGpsSIGQ->octet_ptr, input.stsGpsSIGQ->length);
        printf("10-after set the position:%s, the length of it:%d\n",
               input.stsGpsSPOS->octet_ptr, input.stsGpsSPOS->length);
        printf("11-time recovery mode:%s, the length of it:%d\n",
               input.stsGpsTRMO->octet_ptr, input.stsGpsTRMO->length);
        printf("12-UTC:%s, the length of it:%d\n",
               input.stsGpsUTCT->octet_ptr, input.stsGpsUTCT->length);
        printf("13-Get current Firmware:%s, the length of it:%d\n",
               input.stsGpsVERS->octet_ptr, input.stsGpsVERS->length);
        printf("14-str:%s, the length of it:%d\n",
               input.stsGpsDETF->octet_ptr, input.stsGpsDETF->length);
        printf("15-str:%s, the length of it:%d\n",
               input.stsGpsEFER->octet_ptr, input.stsGpsEFER->length);
        printf("16-str:%s, the length of it:%d\n",
               input.stsGpsESSD->octet_ptr, input.stsGpsESSD->length);
        printf("17-str:%s, the length of it:%d\n",
               input.stsGpsESSN->octet_ptr, input.stsGpsESSN->length);
        printf("18-str:%s, the length of it:%d\n",
               input.stsGpsFAGE->octet_ptr, input.stsGpsFAGE->length);
        printf("19-str:%s, the length of it:%d\n",
               input.stsGpsTIMD->octet_ptr, input.stsGpsTIMD->length);
        printf("20-reset:%d\n", input.stsGpsReset);

    }
    return (0);
}


/*************************************
* function: gpsdebugwrite
*
*
**************************************/
int gpsdebugwrite1()
{
    stsGpsEntry_t input;
    input.stsGpsIndex = 1;
    gpsSet(&input);
    printf("SPOS is:%s, the length of it:%d\n",
           input.stsGpsSPOS->octet_ptr, input.stsGpsSPOS->length);
    return (0);
}


/*************************************
* function: 
* oscdebugread - Read the oscillator response which stored in the mib structure
*
**************************************/
int oscdebugread()
{
    stsOscillatorEntry_t myptr;
    myptr.stsOscillatorIndex = 1;
    oscGet(&myptr);
    if (markdebug == 1) {
        printf("1-the index: %d\n", myptr.stsOscillatorIndex);
        printf("2-the module: %d\n", myptr.stsOscillatorModuleType);
        printf("3-the est. freq error:%s, the length of it:%d\n",
               myptr.stsOscillatorEFER->octet_ptr,
               myptr.stsOscillatorEFER->length);
        printf("4-the est.stand. dev.:%s, the length of it:%d\n",
               myptr.stsOscillatorESSD->octet_ptr,
               myptr.stsOscillatorESSD->length);
        printf("5-est.samp.#:%s, the length of it:%d\n",
               myptr.stsOscillatorESSN->octet_ptr,
               myptr.stsOscillatorESSN->length);
        printf("6-freq. age:%s, the length of it:%d\n",
               myptr.stsOscillatorFAGE->octet_ptr,
               myptr.stsOscillatorFAGE->length);
        printf("7-time diff.:%s, the length of it:%d\n",
               myptr.stsOscillatorTIMD->octet_ptr,
               myptr.stsOscillatorTIMD->length);
        printf("8-active: %d\n", myptr.stsOscillatorSwitchState);
        printf("9-sta.: %d\n", myptr.stsOscillatorStatus);
        printf("10-redundant status:%d\n", myptr.stsOscillatorRedStatus);
    }
    return (0);
}


/*************************************
* function: 
* oscdebugwrite - Write the oscillator response which stored in the mib structure
*
**************************************/
int oscdebugwrite()
{
    stsOscillatorEntry_t myptr;
    myptr.stsOscillatorIndex = 1;
    oscSet(&myptr);
    if (markdebug == 1) {
        printf("result of the index: %d\n", myptr.stsOscillatorIndex);
        printf("result of the status: %d\n",
               myptr.stsOscillatorModuleType);
        printf("result of the est. freq error: %s\n",
               (char *) myptr.stsOscillatorEFER);
        printf("result of the est.stand. dev.: %s\n",
               (char *) myptr.stsOscillatorESSD);
        printf("est.samp.#: %s\n", (char *) myptr.stsOscillatorESSN);
        printf("freq. age: %s\n", (char *) myptr.stsOscillatorFAGE);
        printf("time diff.: %s\n", (char *) myptr.stsOscillatorTIMD);
        printf("active: %d\n", myptr.stsOscillatorSwitchState);
        printf("sta.: %d\n", myptr.stsOscillatorStatus);
        printf("redundant status:%d\n", myptr.stsOscillatorRedStatus);
    }
    return (0);
}


/******************************************************************************
function:
sccSetBaudRate9600/sccSetBaudRate4800 - The routine to set baudrate to 
9600/4800 bps through scc1 or scc3.
paramenter:
  int port port number (1 for scc1, and 3 for scc3) 														

 RETURNS: N/A 
*/
void sccSetBaudRate9600(int port)
{
    m8260SccChan1.baudRate = 9600;
    m8260SioIoctl(&m8260SccChan1, SIO_BAUD_SET, m8260SccChan1.baudRate);
}


void sccSetBaudRate4800(int port)
{
    m8260SccChan1.baudRate = 4800;
    m8260SioIoctl(&m8260SccChan1, SIO_BAUD_SET, m8260SccChan1.baudRate);
}
#endif


int gpsdevquery(int portNum)
{
    serialGpsGpgga_t gpgga;
    int err;
    int idx = 0;
    int *pIdx;
    if (portNum == SCC1PORT) {
        pIdx = &devIdxScc1;
    } else if (portNum == SCC3PORT)
        pIdx = &devIdxScc3;
    while (1) {
        err = gpsModule[idx].gpsmodulequery(&gpgga, portNum);
        if (err == GPS_FIX_INVALID || err == OK) {      /* ok */
            *pIdx = idx;
            if (portNum == SCC1PORT)
                flagexist_scc1 = FLAG_EXIST;
            else {
                flagexist_scc3 = FLAG_EXIST;
            }
            break;
        } else if (err == NO_GPS_DEVICE) {      /* query all devices in the system */
            if (markdebug == 1) {
                printf("no device\n");
            }
            break;
        } else {
            idx++;
            continue;
        }
    }
    return err;
}


/******************************************************************************
function:
gpsGet - the task to get out the mibs data information from the gps device
              
paramenter: 
 stsGpsEntry_t *input - the struct to store response data for gps query response  
  
 RETURNS:    		 
 				OK    - if gps is detected and get the signal information
        ERROR - otherwise
            		
*/
STATUS_t gpsGet(stsGpsEntry_t * input)
{
    int portNum;
    int *fgExist;
    int result;
    int err;
    int *pIdx;
    static OctetString stsGpsANTD[2];
    static OctetString stsGpsAZEL[2];
    static OctetString stsGpsRSTG[2];
    static OctetString stsGpsBITS[2];
    static OctetString stsGpsPAVG[2];
    static OctetString stsGpsSIGQ[2];
    static OctetString stsGpsSPOS[2];
    static OctetString stsGpsTRMO[2];
    static OctetString stsGpsUTCT[2];
    static OctetString stsGpsVERS[2];
    static OctetString stsGpsDETF[2];
    static OctetString stsGpsEFER[2];
    static OctetString stsGpsESSD[2];
    static OctetString stsGpsESSN[2];
    static OctetString stsGpsFAGE[2];
    static OctetString stsGpsTIMD[2];
    input->stsGpsANTD = &stsGpsANTD[input->stsGpsIndex-1];
    input->stsGpsAZEL = &stsGpsAZEL[input->stsGpsIndex-1];
    input->stsGpsRSTG = &stsGpsRSTG[input->stsGpsIndex-1];
    input->stsGpsBITS = &stsGpsBITS[input->stsGpsIndex-1];
    input->stsGpsPAVG = &stsGpsPAVG[input->stsGpsIndex-1];
    input->stsGpsSIGQ = &stsGpsSIGQ[input->stsGpsIndex-1];
    input->stsGpsSPOS = &stsGpsSPOS[input->stsGpsIndex-1];
    input->stsGpsTRMO = &stsGpsTRMO[input->stsGpsIndex-1];
    input->stsGpsUTCT = &stsGpsUTCT[input->stsGpsIndex-1];
    input->stsGpsVERS = &stsGpsVERS[input->stsGpsIndex-1];
    input->stsGpsDETF = &stsGpsDETF[input->stsGpsIndex-1];
    input->stsGpsEFER = &stsGpsEFER[input->stsGpsIndex-1];
    input->stsGpsESSD = &stsGpsESSD[input->stsGpsIndex-1];
    input->stsGpsESSN = &stsGpsESSN[input->stsGpsIndex-1];
    input->stsGpsFAGE = &stsGpsFAGE[input->stsGpsIndex-1];
    input->stsGpsTIMD = &stsGpsTIMD[input->stsGpsIndex-1];
    input->stsGpsSPOS->octet_ptr = positionReadGT;
    input->stsGpsSPOS->length = strlen(positionReadGT);
    if (input->stsGpsIndex == D_stsGpsIndex_esr1) {
        portNum = SCC1PORT;
        fgExist = &flagexist_scc1;
        pIdx = &devIdxScc1;
    } else if (input->stsGpsIndex == D_stsGpsIndex_esr2) {
        portNum = SCC3PORT;
        fgExist = &flagexist_scc3;
        pIdx = &devIdxScc3;
    } else {
        if (markdebug == 1) {
            printf("no Index input\n");
        }
        return ERROR;
    }
    if (*fgExist == NO_FLAG_EXIST) {
        result = gpsdevquery(portNum);
        if (result != NO_GPS_DEVICE) {
            *fgExist = FLAG_EXIST;
            gpsModule[*pIdx].gpsmoduleget(gpsModule[*pIdx].
                                          gpsmoduleparameter, portNum,
                                          input);
        } 
        else {
            gpsnullget(input);
            if (markdebug == 1) {
                printf("no device\n");
            }
            return ERROR;
        }
    } 
    else {                    /* *fgExist ==1 */
        err =
            gpsModule[*pIdx].gpsmoduleget(gpsModule[*pIdx].
                                          gpsmoduleparameter, portNum,
                                          input);
        if (err == -2) {
            *fgExist = 0;
            return ERROR;
        }
    }
    return OK;
}



/******************************************************************************
function:
gpsSet - the task to set the mibs data information to the gps device
              
paramenter: 
 stsGpsEntry_t *input - the struct to store response data for gps query response  
  
 RETURNS:  				
 				OK    - if gps is detected and set the signal information
        ERROR - otherwise 
*/
STATUS_t gpsSet(stsGpsEntry_t * input)
{
    serialGpsXXGGA_t xxggaData;
    int portNum;
    int *fgExist;
    int result;
    int err;
    int *pIdx;
    int parsing;
    char writebuffer1[50];
    static OctetString stsGpsANTD[2];
    static OctetString stsGpsAZEL[2];
    static OctetString stsGpsRSTG[2];
    static OctetString stsGpsBITS[2];
    static OctetString stsGpsPAVG[2];
    static OctetString stsGpsSIGQ[2];
    static OctetString stsGpsSPOS[2];
    static OctetString stsGpsTRMO[2];
    static OctetString stsGpsUTCT[2];
    static OctetString stsGpsVERS[2];
    static OctetString stsGpsDETF[2];
    static OctetString stsGpsEFER[2];
    static OctetString stsGpsESSD[2];
    static OctetString stsGpsESSN[2];
    static OctetString stsGpsFAGE[2];
    static OctetString stsGpsTIMD[2];
    input->stsGpsANTD = &stsGpsANTD[input->stsGpsIndex-1];
    input->stsGpsAZEL = &stsGpsAZEL[input->stsGpsIndex-1];
    input->stsGpsRSTG = &stsGpsRSTG[input->stsGpsIndex-1];
    input->stsGpsBITS = &stsGpsBITS[input->stsGpsIndex-1];
    input->stsGpsPAVG = &stsGpsPAVG[input->stsGpsIndex-1];
    input->stsGpsSIGQ = &stsGpsSIGQ[input->stsGpsIndex-1];
    input->stsGpsSPOS = &stsGpsSPOS[input->stsGpsIndex-1];
    input->stsGpsTRMO = &stsGpsTRMO[input->stsGpsIndex-1];
    input->stsGpsUTCT = &stsGpsUTCT[input->stsGpsIndex-1];
    input->stsGpsVERS = &stsGpsVERS[input->stsGpsIndex-1];
    input->stsGpsDETF = &stsGpsDETF[input->stsGpsIndex-1];
    input->stsGpsEFER = &stsGpsEFER[input->stsGpsIndex-1];
    input->stsGpsESSD = &stsGpsESSD[input->stsGpsIndex-1];
    input->stsGpsESSN = &stsGpsESSN[input->stsGpsIndex-1];
    input->stsGpsFAGE = &stsGpsFAGE[input->stsGpsIndex-1];
    input->stsGpsTIMD = &stsGpsTIMD[input->stsGpsIndex-1];
    input->stsGpsSPOS->octet_ptr = positionReadGT;
    input->stsGpsSPOS->length = strlen(positionReadGT);
/*
	positionReadGT = input->stsGpsSPOS->octet_ptr;
	*/
    input->stsGpsSPOS->octet_ptr =
        "latitude:5555.0770,N,longtitude:98765.1100,E";	
   /* tlDataAccess(input, "spos.dat", ESR_WRITE);*/
    memset(writebuffer1, 0, 50);
    strcpy(writebuffer1, input->stsGpsSPOS->octet_ptr);
    parsing = xxggaParsing(writebuffer1, &xxggaData);
    if (parsing == 0 || parsing == 7) {
        strcpy(positionReadGT, input->stsGpsSPOS->octet_ptr);
        if (markdebug == 1) {
            printf("the octet string is:%s\n",
                   input->stsGpsSPOS->octet_ptr);
        }
    }
    else {
        if (markdebug == 1) {
            printf("string format is not valid\n");
        }
        return ERROR;
    }
    if (input->stsGpsIndex == 1) {
        portNum = SCC1PORT;
        fgExist = &flagexist_scc1;
        pIdx = &devIdxScc1;
    }
    else if (input->stsGpsIndex == 2) {
        portNum = SCC3PORT;
        fgExist = &flagexist_scc3;
        pIdx = &devIdxScc3;
    }
    else {
        if (markdebug == 1) {
            printf("no Index input\n");
        }
        return ERROR;
    }
    if (*fgExist == 0) {
        result = gpsdevquery(portNum);
        if (result != NO_GPS_DEVICE) {
            *fgExist = 1;
            gpsModule[*pIdx].gpsmoduleset(gpsModule[*pIdx].
                                         gpsmoduleparameter, portNum,
                                         input);
        }
        else {
            if (markdebug == 1) {
                printf("no device\n");
            }
            return ERROR;
        }
    }
    else {                      /* *fgExist ==1 */
        err =
            gpsModule[*pIdx].gpsmoduleset(gpsModule[*pIdx].
                                         gpsmoduleparameter, portNum,
                                         input);
        if (err == -2) {
            *fgExist = 0;
            return ERROR;
        }
    }
    return OK;
}


/******************************************************************************
function:
gpsSetGT - the task to get the driver response from the gps device
              
paramenter: 
 stsGpsEntry_t *entry - the struct to store response data for gps query response
 serialGpsGpwav_t *gsv - the struct to store the signal strength
 serialGpsGpgga_t *gga - the struct to store the latitude,longitude, altitude and the UTC  
 int portNum - the port number of the device
  
 RETURNS: 0 if ok. 
*/
static int gpsSetGT(void *input, int portNum, stsGpsEntry_t * entry){
 struct module1 *GT = (struct module1 *) input;
    serialGpsGpwav_t *GTgpwav = &GT->gpwav;
    serialGpsGptst_t *GTgptst = &GT->gptst;
    serialGpsGpgga_t *GTgpgga = &GT->gpgga;
    serialGpsGpgsv_t *GTgpgsv = &GT->gpgsv;
    int back = 0;
    int no_poorsig;
    char buffer1[2] = "";
    char buffer2[2] = "";
    static char ANTD[2][20];
    static char AZEL[2][40];
    static char RSTG[2][30];
    static char BITS[2][30];
    static char PAVG[2][140];
    static char SIGQ[2][20];
    /*static char SPOS[60]; */
    static char TRMO[2][20];
    static char UTCT[2][40];
    static char VERS[2][10];
    static char DETF[2][20];
    static char EFER[2][20];
    static char ESSD[2][20];
    static char ESSN[2][20];
    static char FAGE[2][20];
    static char TIMD[2][20];
    int que;

    entry->stsGpsState = getGpsRedState(portNum);
    que = sccGPGGA(GTgpgga, portNum);
    if (que == GPS_FIX_INVALID || que == OK) {
        sccGPGSV(GTgpgsv, portNum);
        no_poorsig = sccGPGSV(GTgpgsv, portNum);
        if (no_poorsig == NO_SIGNAL) {
            entry->stsGpsStatus = D_stsGpsStatus_noSignal;
            gpsSelfTestResult(GTgptst, portNum);
            gpsSatelliteInfo(GTgpwav, portNum);
        }
        else if (no_poorsig == SIGNAL_OVER_1ST || no_poorsig == SIGNAL_OVER_2ND || no_poorsig == SIGNAL_OVER_3RD
                 || no_poorsig == SIGNAL_OVER_4TH) {
            entry->stsGpsStatus = D_stsGpsStatus_ok;
            gpsSelfTestResult(GTgptst, portNum);
            gpsSatelliteInfo(GTgpwav, portNum);
        }
        else if (no_poorsig == SIGNAL_LESS_1ST || no_poorsig == SIGNAL_LESS_2ND || no_poorsig == SIGNAL_LESS_3RD
                 || no_poorsig == SIGNAL_LESS_4TH) {
            entry->stsGpsStatus = D_stsGpsStatus_poorSignal;
            gpsSelfTestResult(GTgptst, portNum);
            gpsSatelliteInfo(GTgpwav, portNum);
        }
        else {
            back = -3;
            return back;
        }
    }
    else {
        if (markdebug == 1) {
            printf("status nonAvailable\n");
        }
        entry->stsGpsStatus = D_stsGpsStatus_nonAvailable;
        back = -2;
        return back;
    }
    if (markdebug == 1) {
        printf("active\n");
    }
    memcpy(buffer1, GTgpgga->direction1, 1);
    memcpy(buffer2, GTgpgga->direction2, 1);
    memset(ANTD[entry->stsGpsIndex-1], '-', 20);   
    ANTD[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsANTD->length = sprintf(ANTD[entry->stsGpsIndex-1], "ANTD:%s", "not supported");
    entry->stsGpsANTD->octet_ptr = ANTD[entry->stsGpsIndex-1];

    memset(AZEL[entry->stsGpsIndex-1], '-', 40);
    AZEL[entry->stsGpsIndex-1][39] = 0;
    entry->stsGpsAZEL->length =
        sprintf(AZEL[entry->stsGpsIndex-1], "GEO #:%d, elevation:%d, azimuth:%d", GTgpwav->GEO_1,
                GTgpwav->Ele_1, GTgpwav->Azi_1);
    entry->stsGpsAZEL->octet_ptr = AZEL[entry->stsGpsIndex-1];
    
    memset(RSTG, '-', 30);
    RSTG[entry->stsGpsIndex-1][29] = 0;
    entry->stsGpsRSTG->length =
        sprintf(RSTG[entry->stsGpsIndex-1], "reset:%s", "now is restarting");
    entry->stsGpsRSTG->octet_ptr = RSTG[entry->stsGpsIndex-1];
    
    memset(BITS[entry->stsGpsIndex-1], '-', 30);
    BITS[entry->stsGpsIndex-1][29] = 0;
    entry->stsGpsBITS->length =
        sprintf(BITS[entry->stsGpsIndex-1], "build-in test status:%d", GTgptst->status);
    entry->stsGpsBITS->octet_ptr = BITS[entry->stsGpsIndex-1];

    memset(PAVG[entry->stsGpsIndex-1], '-', 140);
    PAVG[entry->stsGpsIndex-1][139] = 0;
    entry->stsGpsPAVG->length =
        sprintf(PAVG[entry->stsGpsIndex-1],
                "Latitude:[%d]degree, [%d]min_int, [%d]min_frac, [%s]direction\n"
                "Longitude:[%d]degree, [%d]min_int, [%d]min_frac, [%s]direction\n"
                "Altitude:[%.1f]", GTgpgga->latitude.degree,
                GTgpgga->latitude.minuate, GTgpgga->latitude.fraction,
                buffer1, GTgpgga->longitude.degree,
                GTgpgga->longitude.minuate, GTgpgga->longitude.fraction,
                buffer2, GTgpgga->altitude);
    entry->stsGpsPAVG->octet_ptr = PAVG[entry->stsGpsIndex-1];
    
    memset(SIGQ[entry->stsGpsIndex-1], '-', 20);
    SIGQ[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsSIGQ->length = sprintf(SIGQ[entry->stsGpsIndex-1], "SIGQ:%s", "not supported");
    entry->stsGpsSIGQ->octet_ptr = SIGQ[entry->stsGpsIndex-1];

    memset(TRMO[entry->stsGpsIndex-1], '-', 20);
    TRMO[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsTRMO->length = sprintf(TRMO[entry->stsGpsIndex-1], "TRMO:%s", "not supported");
    entry->stsGpsTRMO->octet_ptr = TRMO[entry->stsGpsIndex-1];
    
    memset(UTCT[entry->stsGpsIndex-1], '-', 40);
    UTCT[entry->stsGpsIndex-1][39] = 0;
    entry->stsGpsUTCT->length =
        sprintf(UTCT[entry->stsGpsIndex-1], "[%d]hour, [%d]minutes, [%d]seconds",
                GTgpgga->utc.hh, GTgpgga->utc.mm, GTgpgga->utc.ss);
    entry->stsGpsUTCT->octet_ptr = UTCT[entry->stsGpsIndex-1];
    
    memset(VERS[entry->stsGpsIndex-1], '-', 10);
    VERS[entry->stsGpsIndex-1][9] = 0;
    entry->stsGpsVERS->length =
        sprintf(VERS[entry->stsGpsIndex-1], "version:%d", GTgptst->pvnum.ver_n);
    entry->stsGpsVERS->octet_ptr = VERS[entry->stsGpsIndex-1];
    
    memset(DETF[entry->stsGpsIndex-1], '-', 20);
    DETF[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsDETF->length = sprintf(DETF[entry->stsGpsIndex-1], "DETF:%s", "not supported");
    entry->stsGpsDETF->octet_ptr = DETF[entry->stsGpsIndex-1];
    
    memset(EFER[entry->stsGpsIndex-1], '-', 20);
    EFER[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsEFER->length = sprintf(EFER[entry->stsGpsIndex-1], "EFER:%s", "not supported");
    entry->stsGpsEFER->octet_ptr = EFER[entry->stsGpsIndex-1];

    memset(ESSD[entry->stsGpsIndex-1], '-', 20);
    ESSD[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsESSD->length = sprintf(ESSD[entry->stsGpsIndex-1], "ESSD:%s", "not supported");
    entry->stsGpsESSD->octet_ptr = ESSD[entry->stsGpsIndex-1];

    memset(ESSN[entry->stsGpsIndex-1], '-', 20);
    ESSN[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsESSN->length = sprintf(ESSN[entry->stsGpsIndex-1], "ESSN:%s", "not supported");
    entry->stsGpsESSN->octet_ptr = ESSN[entry->stsGpsIndex-1];

    memset(FAGE[entry->stsGpsIndex-1], '-', 20);
    FAGE[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsFAGE->length = sprintf(FAGE[entry->stsGpsIndex-1], "FAGE:%s", "not supported");
    entry->stsGpsFAGE->octet_ptr = FAGE[entry->stsGpsIndex-1];

    memset(TIMD[entry->stsGpsIndex-1], '-', 20);
    TIMD[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsTIMD->length = sprintf(TIMD[entry->stsGpsIndex-1], "TIMD:%s", "not supported");
    entry->stsGpsTIMD->octet_ptr = TIMD[entry->stsGpsIndex-1];

    entry->stsGpsReset = D_stsGpsReset_noAction;

    if (markdebug == 1) {
        printf
            ("\n...................done the kai testing.......................\n");
    }
    return back;
}



/******************************************************************************
function:
gpsSet30 - the task to get the driver response from the gps device
              
paramenter: 
 stsGpsEntry_t *entry - the struct to store response data for gps query response
 serialGpsGpwav_t *gsv - the struct to store the signal strength
 serialGpsGpgga_t *gga - the struct to store the latitude,longitude, altitude and the UTC  
 serialGpsXXGGA_t *xxa - the struct to store the setting latidue and longitude
 int portNum - the port number of the device
  
 RETURNS: 0 if ok. 
*/
static int gpsSet30(void *input, int portNum, stsGpsEntry_t * entry){
    struct module2 *ju30 = (struct module2 *) input;
    serialGpsGpgga_t *jugpgga = &ju30->gpgga;
    serialGpsGpgsv_t *jugpgsv = &ju30->gpgsv;
    serialGpsSPOS30_t xxgga;
    char buffer3[2] = "";
    char buffer4[2] = "";
    char SPOS[200];
    int no_poorsig;
    int que;
    que = GPGGA30(jugpgga, portNum);
    entry->stsGpsState = getGpsRedState(portNum);
    if (entry->stsGpsReset == D_stsGpsReset_reset) {
        Reset30(&xxgga, portNum);
        return 0;
    }
    if (que == 0 || que == 7) {
        GPGSV30(jugpgsv, portNum);
        no_poorsig = GPGSV30(jugpgsv, portNum);
        if (no_poorsig == 0) {
            entry->stsGpsStatus = D_stsGpsStatus_noSignal;
            XXGGA30((char *) entry->stsGpsSPOS, &xxgga, portNum);
            if (markdebug == 1) {
                printf("doing the port:%d xxggaparsing..\n", portNum);
            }
        }
        else if (no_poorsig == 5 || no_poorsig == 6 || no_poorsig == 7
                 || no_poorsig == 8) {
            entry->stsGpsStatus = D_stsGpsStatus_ok;
            XXGGA30((char *) entry->stsGpsSPOS, &xxgga, portNum);
        }
        else if (no_poorsig == 1 || no_poorsig == 2 || no_poorsig == 3
                 || no_poorsig == 4) {
            entry->stsGpsStatus = D_stsGpsStatus_poorSignal;
            XXGGA30((char *) entry->stsGpsSPOS, &xxgga, portNum);
        }
    }
    else {
        entry->stsGpsStatus = D_stsGpsStatus_nonAvailable;
        if (markdebug == 1) {
            printf("standby\n");
        }
        return -1;
    }
    memcpy(buffer3, xxgga.directNS, 1);
    memcpy(buffer4, xxgga.directEW, 1);
    sprintf(SPOS, "latitude:[%.4f],longitude:[%.4f], altitude:[%f]\n",
            xxgga.lat, xxgga.longitudef, xxgga.alt);
    (char *) entry->stsGpsSPOS = SPOS;
    if (markdebug == 1) {
        printf("length SPOS:%d\n", strlen(SPOS));
    }
    return 0;
}


/******************************************************************************
function:
gpsSetPico - the task to get the driver response from the gps device
              
paramenter: 
 stsGpsEntry_t *entry - the struct to store response data for gps query response
 serialGpsGpgga_t *gga - the struct to store the latitude,longitude, altitude and the UTC  
 int portNum - the port number of the device
  
 RETURNS: 0 if ok. 
*/
static int gpsSetPico(void *input, int portNum, stsGpsEntry_t * entry)
{

    /* struct module3 *Pico =(struct module3 *)input;
       serialGpsPicoANTD_t *Picoantd = &Pico->az;        
       serialGpsPicoAZEL_t *Picoazel = &Pico->bb;
       serialGpsGpgga_t *Picogpgga = &Pico->gpgga;
       int gpsDelay = 0;
       char buffer3[2]="";
       char buffer4[2]="";
       char SPOS[200];    
       int i, no_poorsig;  */
    return 0;
}


static int oscdevquery(int portNum)
{

    serialID_t idData;

    int err;

    int idx = 0;

    int *pIdx;

    if (portNum == SMC1PORT)

        pIdx = &devIdxSmc1;

    else if (portNum == SCC4PORT)

        pIdx = &devIdxScc4;

    while (1) {

        err = oscModule[idx].oscmodulequery(portNum, &idData, sxoDev);

        if (markdebug == 1) {

            printf("start oscdevquery\n");

        }

        if (err == OSC_FIX_INVALID || err == OK) {      /* ok */

            *pIdx = idx;

            if (portNum == SMC1PORT)

                flagexist_smc1 = 1;

            else {

                flagexist_scc4 = 1;

                if (markdebug == 1) {

                    printf("@@@@@@@@@@@@@found it@@@@@@@@@@@@@@@@\n");

                }

            }

            break;

        }

        else if (err == NO_OSC_DEVICE) {        /* query all devices in the system */

            if (markdebug == 1) {

                printf("no device\n");

            }

            break;

        }

        else {

            idx++;

            continue;

        }

    }

    return err;
}


/******************************************************************************
function:
oscGet - the task to get the mibs data information to the oscillator device
              
paramenter: 
 stsOscillatorEntry_t *input - the struct to store response data for oscillator query response  
  
 RETURNS: 
   		 OK    - if osc is detected and get the signal information
       ERROR - otherwise
*/
STATUS_t oscGet(stsOscillatorEntry_t * input)
{

    int portNum;
    int *fgExist;
    int result;
    int err;
    int *pIdx;
    static OctetString stsOscillatorEFER[2];
    static OctetString stsOscillatorESSD[2];
    static OctetString stsOscillatorESSN[2];
    static OctetString stsOscillatorFAGE[2];
    static OctetString stsOscillatorTIMD[2];
    input->stsOscillatorEFER = &stsOscillatorEFER[input->stsOscillatorIndex-1];
    input->stsOscillatorESSD = &stsOscillatorESSD[input->stsOscillatorIndex-1];
    input->stsOscillatorESSN = &stsOscillatorESSN[input->stsOscillatorIndex-1];
    input->stsOscillatorFAGE = &stsOscillatorFAGE[input->stsOscillatorIndex-1];
    input->stsOscillatorTIMD = &stsOscillatorTIMD[input->stsOscillatorIndex-1];
    if (input->stsOscillatorIndex == D_stsOscillatorIndex_oscillator1) {
        portNum = SMC1PORT;
        fgExist = &flagexist_smc1;
        pIdx = &devIdxSmc1;
    }
    else if (input->stsOscillatorIndex == D_stsOscillatorIndex_oscillator2) {
        portNum = SCC4PORT;
        fgExist = &flagexist_scc4;
        pIdx = &devIdxScc4;
    }
    else {
        if (markdebug == 1) {
            printf("no Index input\n");
        }
        return ERROR;
    }
    if (*fgExist == 0) {
        result = oscdevquery(portNum);
        if (result != NO_OSC_DEVICE) {
            *fgExist = 1;
            oscModule[*pIdx].oscmoduleget(oscModule[*pIdx].
                                          oscmoduleparameter, portNum,
                                          input);
            if (markdebug == 1) {
                printf("module get\n");
            }
        }
        else {
            if (markdebug == 1) {
                printf("no device\n");
            }
            oscnullget(input);
            return ERROR;
        }
    }
    else {                      /* *fgExist ==1 */
        err =
            oscModule[*pIdx].oscmoduleget(oscModule[*pIdx].
                                          oscmoduleparameter, portNum,
                                          input);
        if (markdebug == 1) {
            printf("exist module get\n");
        }
        if (err == -2) {
            *fgExist = 0;
            return ERROR;
        }
    }
    return OK;
}


static int oscGet1(void *input, int portNum, stsOscillatorEntry_t * entry)
{

    struct osmodule1 *OSC = (struct osmodule1 *) input;
    serialID_t *OSCid = &OSC->idData;
    serialBS_t *OSCbs = &OSC->bsData;
    int statusData;
    long snData;
    int que;
    static char oscEFER[2][20];
    static char oscESSD[2][20];
    static char oscESSN[2][20];
    static char oscFAGE[2][20];
    static char oscTIMD[2][20];
    entry->stsOscillatorRedStatus =
        oscGetRedStatus(entry->stsOscillatorIndex);
    entry->stsOscillatorSwitchState =
        oscGetSwitchState(entry->stsOscillatorIndex);
    que = rb_or_cryID(portNum, OSCid, sxoDev);
    if (markdebug == 1) {
        printf("the rb_or_cryID value is:%d\n", que);
    }
    if (strncmp(OSCid->idhead, "TNTSRO", 6) == 0) {
        if (markdebug == 1) {
            printf("rubidium here\n");
        }
        entry->stsOscillatorModuleType = D_stsOscillatorModuleType_rb;
    }
    else if (strncmp(OSCid->idhead, "TNTSXO", 6) == 0) {
        if (markdebug == 1) {
            printf("crystal here\n");
        }
        entry->stsOscillatorModuleType =
            D_stsOscillatorModuleType_crystalSt2;
    }
    if (que == 0 || que == 7) {
        rb_or_crySerNum(portNum, &snData);
        rb_or_cryStatus(portNum, &statusData);
        rb_or_cryBeatSec(portNum, OSCbs);
        rb_or_cryStopBeatSec(portNum, OSCbs);
    }
    memset(oscEFER[entry->stsOscillatorIndex-1], '-', 20);
    oscEFER[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorEFER->length =
        sprintf(oscEFER[entry->stsOscillatorIndex-1], "EFER:%s", "not supported");
    entry->stsOscillatorEFER->octet_ptr = oscEFER[entry->stsOscillatorIndex-1];

    memset(oscESSD[entry->stsOscillatorIndex-1], '-', 20);
    oscESSD[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorESSD->length =
        sprintf(oscESSD[entry->stsOscillatorIndex-1], "ESSD:%s", "not supported");
    entry->stsOscillatorESSD->octet_ptr = oscESSD[entry->stsOscillatorIndex-1];

    memset(oscESSN[entry->stsOscillatorIndex-1], '-', 20);
    oscESSN[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorESSN->length =
        sprintf(oscESSN[entry->stsOscillatorIndex-1], "ESSN:%s", "not supported");
    entry->stsOscillatorESSN->octet_ptr = oscESSN[entry->stsOscillatorIndex-1];

    memset(oscFAGE[entry->stsOscillatorIndex-1], '-', 20);
    oscFAGE[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorFAGE->length =
        sprintf(oscFAGE[entry->stsOscillatorIndex-1], "FAGE:%s", "not supported");
    entry->stsOscillatorFAGE->octet_ptr = oscFAGE[entry->stsOscillatorIndex-1];

    memset(oscTIMD[entry->stsOscillatorIndex-1], '-', 20);
    oscTIMD[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorTIMD->length =
        sprintf(oscTIMD[entry->stsOscillatorIndex-1], "Delay in 1ns:[%f]", OSCbs->d9_sppp);
    entry->stsOscillatorTIMD->octet_ptr = oscTIMD[entry->stsOscillatorIndex-1];

    if (statusData == 4 || statusData == 5 || statusData == 6) {
        if (oscStatus == 2 || oscStatus == 3) {
            entry->stsOscillatorStatus =
                D_stsOscillatorStatus_holdoversync;
        }
        else {
            entry->stsOscillatorStatus = D_stsOscillatorStatus_frngsync;
        }
    }
    else if (statusData == 1) {
        entry->stsOscillatorStatus = D_stsOscillatorStatus_aqrsync;
    }
    else if (statusData == 2 || statusData == 3) {
        entry->stsOscillatorStatus = D_stsOscillatorStatus_lkdsync;
    }
    else {
        return ERROR;
    }
    oscStatus = statusData;
    return OK;
}


/******************************************************************************
function:
oscSet - the task to set the mibs data information to the oscillator device
              
paramenter: 
 stsOscillatorEntry_t *input - the struct to store response data for oscillator query response  
  
 RETURNS:    		 
 				OK    - if osc is detected and set the signal information
        ERROR - otherwise 
*/
STATUS_t oscSet(stsOscillatorEntry_t * input)
{
    int portNum;
    int *fgExist;
    int result;
    int err;
    int *pIdx;
    static OctetString stsOscillatorEFER[2];
    static OctetString stsOscillatorESSD[2];
    static OctetString stsOscillatorESSN[2];
    static OctetString stsOscillatorFAGE[2];
    static OctetString stsOscillatorTIMD[2];
    input->stsOscillatorEFER = &stsOscillatorEFER[input->stsOscillatorIndex-1];
    input->stsOscillatorESSD = &stsOscillatorESSD[input->stsOscillatorIndex-1];
    input->stsOscillatorESSN = &stsOscillatorESSN[input->stsOscillatorIndex-1];
    input->stsOscillatorFAGE = &stsOscillatorFAGE[input->stsOscillatorIndex-1];
    input->stsOscillatorTIMD = &stsOscillatorTIMD[input->stsOscillatorIndex-1];
    if (markdebug == 1) {
        printf("oscSet start\n");
    }
    if (input->stsOscillatorIndex == D_stsOscillatorIndex_oscillator1) {
        if (markdebug == 1) {
            printf("here is D_stsOscillatorIndex_oscillator1\n");
        }
        portNum = SMC1PORT;
        fgExist = &flagexist_smc1;
        pIdx = &devIdxSmc1;
    }
    else if (input->stsOscillatorIndex == D_stsOscillatorIndex_oscillator2) {
        if (markdebug == 1) {
            printf("here is D_stsOscillatorIndex_oscillator2\n");
        }
        portNum = SCC4PORT;
        fgExist = &flagexist_scc4;
        pIdx = &devIdxScc4;
    }
    else {
        if (markdebug == 1) {
            printf("no Index input\n");
        }
        return ERROR;
    }
    if (*fgExist == 0) {
        result = oscdevquery(portNum);
        if (result != NO_OSC_DEVICE) {
            *fgExist = 1;
            oscModule[*pIdx].oscmoduleset(oscModule[*pIdx].
                                          oscmoduleparameter, portNum,
                                          input);
            if (markdebug == 1) {
                printf("module set\n");
            }
        }
        else {
            if (markdebug == 1) {
                printf("no device\n");
            }
            oscnullget(input);
            return ERROR;
        }
    }
    else {                      /* *fgExist ==1 */
        if (markdebug == 1) {
            printf("start oscmoduleset\n");
        }
        err =
            oscModule[*pIdx].oscmoduleset(oscModule[*pIdx].
                                          oscmoduleparameter, portNum,
                                          input);
        if (markdebug == 1) {
            printf("err value is:%d\n", err);
            printf("end oscmoduleset\n");
        }
        if (err == -2) {
            *fgExist = 0;
            return ERROR;
        }
    }
    if (markdebug == 1) {
        printf("finish osc set\n");
    }
    return OK;
}



static int oscSet1(void *input, int portNum, stsOscillatorEntry_t * entry)
{
    if (markdebug == 1) {

        printf
            ("\n................doing kai osc .......................\n");

    }

    if (portNum == SMC1PORT) {

        entry->stsOscillatorModuleType = D_stsOscillatorIndex_oscillator1;

        entry->stsOscillatorRedStatus =
            oscGetRedStatus(D_stsOscillatorIndex_oscillator1);

    }

    else if (portNum == SCC4PORT) {

        entry->stsOscillatorModuleType = D_stsOscillatorIndex_oscillator2;

        entry->stsOscillatorRedStatus =
            oscGetRedStatus(D_stsOscillatorIndex_oscillator2);

    }

    oscSetSwitchState(entry->stsOscillatorIndex,
                      entry->stsOscillatorSwitchState);

    return OK;
}


/******************************************************************************
function:
oscReset - the task to reset the oscillator device
              
paramenter: 
 stsOscillatorEntry_t *input - the struct to store response data for oscillator query response  
  
 RETURNS:    		 
 				OK    - if osc is detected and reset
        ERROR - otherwise 
*/

STATUS_t oscReset(stsOscillatorEntry_t * input)
{

    stsOscillatorEntry_t entry;

    int portNum = 0;

    if (input->stsOscillatorIndex == 1) {

        portNum = SMC1PORT;

        entry.stsOscillatorModuleType = 1;

        if (markdebug == 1) {
        	printf("port 1 is reset now\n");
        }	

        rb_or_cryReset(portNum);

    }

    else if (input->stsOscillatorIndex == 2) {

        portNum = SCC4PORT;

        entry.stsOscillatorModuleType = 2;

        printf("port 2 is reset now\n");

        rb_or_cryReset(portNum);

    }

    else {

        return ERROR;

    }

    return OK;
}


static int getGpsRedState(int portNum)
{

    int RedState;

    if (portNum == SCC1PORT)
    {

        RedState = getRedState(D_stsCardIndex_esr1, 0);

    }

    else if (portNum == SCC3PORT)
    {

        RedState = getRedState(D_stsCardIndex_esr2, 0);

    }

    return RedState;
}


/******************************************************************************
function:
gpsGetGT - the task to get the driver response from the gps device
              
paramenter: 
 stsGpsEntry_t *entry - the struct to store response data for gps query response
 serialGpsGpwav_t *wav - the struct to store the azimuth and elevation
 serialGpsGptst_t *tst - the struct to store the built in test status and the firmware version
 serialGpsGpgga_t *gga - the struct to store the latitude,longitude, altitude and the UTC  
 serialGpsGpgsv_t *gsv - the struct to store the signal strength
 int portNum - the port number of the device
  
 RETURNS: 0 if ok. 
*/
static int gpsGetGT(void *input, int portNum, stsGpsEntry_t * entry){
    struct module1 *GT = (struct module1 *) input;
    serialGpsGpwav_t *GTgpwav = &GT->gpwav;
    serialGpsGptst_t *GTgptst = &GT->gptst;
    serialGpsGpgga_t *GTgpgga = &GT->gpgga;
    serialGpsGpgsv_t *GTgpgsv = &GT->gpgsv;
    int back = 0;
    int no_poorsig;
    char buffer1[2] = "";
    char buffer2[2] = "";
    static char ANTD[20];
    static char AZEL[40];
    static char RSTG[30];
    static char BITS[30];
    static char PAVG[140];
    static char SIGQ[20];
    /*static char SPOS[60]; */
    static char TRMO[20];
    static char UTCT[40];
    static char VERS[10];
    static char DETF[20];
    static char EFER[20];
    static char ESSD[20];
    static char ESSN[20];
    static char FAGE[20];
    static char TIMD[20];
    int que;

    entry->stsGpsState = getGpsRedState(portNum);
    que = sccGPGGA(GTgpgga, portNum);
    if (que == GPS_FIX_INVALID || que == OK) {
        sccGPGSV(GTgpgsv, portNum);
        no_poorsig = sccGPGSV(GTgpgsv, portNum);
        if (no_poorsig == NO_SIGNAL) {
            entry->stsGpsStatus = D_stsGpsStatus_noSignal;
            gpsSelfTestResult(GTgptst, portNum);
            gpsSatelliteInfo(GTgpwav, portNum);
        }
        else if (no_poorsig == SIGNAL_OVER_1ST || no_poorsig == SIGNAL_OVER_2ND || no_poorsig == SIGNAL_OVER_3RD
                 || no_poorsig == SIGNAL_OVER_4TH) {
            entry->stsGpsStatus = D_stsGpsStatus_ok;
            gpsSelfTestResult(GTgptst, portNum);
            gpsSatelliteInfo(GTgpwav, portNum);
        }
        else if (no_poorsig == SIGNAL_LESS_1ST || no_poorsig == SIGNAL_LESS_2ND || no_poorsig == SIGNAL_LESS_3RD
                 || no_poorsig == SIGNAL_LESS_4TH) {
            entry->stsGpsStatus = D_stsGpsStatus_poorSignal;
            gpsSelfTestResult(GTgptst, portNum);
            gpsSatelliteInfo(GTgpwav, portNum);
        }
        else {
            back = -3;
            return back;
        }
    }
    else {
        if (markdebug == 1) {
            printf("status nonAvailable\n");
        }
        entry->stsGpsStatus = D_stsGpsStatus_nonAvailable;
        back = -2;
        return back;
    }
    if (markdebug == 1) {
        printf("active\n");
    }
    memcpy(buffer1, GTgpgga->direction1, 1);
    memcpy(buffer2, GTgpgga->direction2, 1);
    memset(ANTD, '-', 20);   
    ANTD[19] = 0;
    entry->stsGpsANTD->length = sprintf(ANTD, "ANTD:%s", "not supported");
    entry->stsGpsANTD->octet_ptr = ANTD;

    memset(AZEL, '-', 40);
    AZEL[39] = 0;
    entry->stsGpsAZEL->length =
        sprintf(AZEL, "GEO #:%d, elevation:%d, azimuth:%d", GTgpwav->GEO_1,
                GTgpwav->Ele_1, GTgpwav->Azi_1);
    entry->stsGpsAZEL->octet_ptr = AZEL;
    
    memset(RSTG, '-', 30);
    RSTG[29] = 0;
    entry->stsGpsRSTG->length =
        sprintf(RSTG, "reset:%s", "now is restarting");
    entry->stsGpsRSTG->octet_ptr = RSTG;
    
    memset(BITS, '-', 30);
    BITS[29] = 0;
    entry->stsGpsBITS->length =
        sprintf(BITS, "build-in test status:%d", GTgptst->status);
    entry->stsGpsBITS->octet_ptr = BITS;

    memset(PAVG, '-', 140);
    PAVG[139] = 0;
    entry->stsGpsPAVG->length =
        sprintf(PAVG,
                "Latitude:[%d]degree, [%d]min_int, [%d]min_frac, [%s]direction\n"
                "Longitude:[%d]degree, [%d]min_int, [%d]min_frac, [%s]direction\n"
                "Altitude:[%.1f]", GTgpgga->latitude.degree,
                GTgpgga->latitude.minuate, GTgpgga->latitude.fraction,
                buffer1, GTgpgga->longitude.degree,
                GTgpgga->longitude.minuate, GTgpgga->longitude.fraction,
                buffer2, GTgpgga->altitude);
    entry->stsGpsPAVG->octet_ptr = PAVG;
    
    memset(SIGQ, '-', 20);
    SIGQ[19] = 0;
    entry->stsGpsSIGQ->length = sprintf(SIGQ, "SIGQ:%s", "not supported");
    entry->stsGpsSIGQ->octet_ptr = SIGQ;

    memset(TRMO, '-', 20);
    TRMO[19] = 0;
    entry->stsGpsTRMO->length = sprintf(TRMO, "TRMO:%s", "not supported");
    entry->stsGpsTRMO->octet_ptr = TRMO;
    
    memset(UTCT, '-', 40);
    UTCT[39] = 0;
    entry->stsGpsUTCT->length =
        sprintf(UTCT, "[%d]hour, [%d]minutes, [%d]seconds",
                GTgpgga->utc.hh, GTgpgga->utc.mm, GTgpgga->utc.ss);
    entry->stsGpsUTCT->octet_ptr = UTCT;
    
    memset(VERS, '-', 10);
    VERS[9] = 0;
    entry->stsGpsVERS->length =
        sprintf(VERS, "version:%d", GTgptst->pvnum.ver_n);
    entry->stsGpsVERS->octet_ptr = VERS;
    
    memset(DETF, '-', 20);
    DETF[19] = 0;
    entry->stsGpsDETF->length = sprintf(DETF, "DETF:%s", "not supported");
    entry->stsGpsDETF->octet_ptr = DETF;
    
    memset(EFER, '-', 20);
    EFER[19] = 0;
    entry->stsGpsEFER->length = sprintf(EFER, "EFER:%s", "not supported");
    entry->stsGpsEFER->octet_ptr = EFER;

    memset(ESSD, '-', 20);
    ESSD[19] = 0;
    entry->stsGpsESSD->length = sprintf(ESSD, "ESSD:%s", "not supported");
    entry->stsGpsESSD->octet_ptr = ESSD;

    memset(ESSN, '-', 20);
    ESSN[19] = 0;
    entry->stsGpsESSN->length = sprintf(ESSN, "ESSN:%s", "not supported");
    entry->stsGpsESSN->octet_ptr = ESSN;

    memset(FAGE, '-', 20);
    FAGE[19] = 0;
    entry->stsGpsFAGE->length = sprintf(FAGE, "FAGE:%s", "not supported");
    entry->stsGpsFAGE->octet_ptr = FAGE;

    memset(TIMD, '-', 20);
    TIMD[19] = 0;
    entry->stsGpsTIMD->length = sprintf(TIMD, "TIMD:%s", "not supported");
    entry->stsGpsTIMD->octet_ptr = TIMD;

    entry->stsGpsReset = D_stsGpsReset_noAction;

    if (markdebug == 1) {
        printf
            ("\n...................done the kai testing.......................\n");
    }
    return back;
}


static int gpsnullget(stsGpsEntry_t * entry)
{
    static char nANTD[2][20];
    static char nAZEL[2][20];
    static char nRSTG[2][20];
    static char nBITS[2][20];
    static char nPAVG[2][20];
    static char nSIGQ[2][20];
    static char nSPOS[2][20];
    static char nTRMO[2][20];
    static char nUTCT[2][20];
    static char nVERS[2][20];
    static char nDETF[2][20];
    static char nEFER[2][20];
    static char nESSD[2][20];
    static char nESSN[2][20];
    static char nFAGE[2][20];
    static char nTIMD[2][20];
    memset(nANTD[entry->stsGpsIndex-1], '-', 20);
    nANTD[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsANTD->length = sprintf(nANTD[entry->stsGpsIndex-1], "ANTD:%s", "NULL");
    entry->stsGpsANTD->octet_ptr = nANTD[entry->stsGpsIndex-1];
    memset(nAZEL[entry->stsGpsIndex-1], '-', 20);
    nAZEL[entry->stsGpsIndex-1][19] = 0;

    entry->stsGpsAZEL->length = sprintf(nAZEL[entry->stsGpsIndex-1], "AZEL:%s", "NULL");
    entry->stsGpsAZEL->octet_ptr = nAZEL[entry->stsGpsIndex-1];
    memset(nRSTG[entry->stsGpsIndex-1], '-', 20);
    nRSTG[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsRSTG->length = sprintf(nRSTG[entry->stsGpsIndex-1], "RSTG:%s", "NULL");
    entry->stsGpsRSTG->octet_ptr = nRSTG[entry->stsGpsIndex-1];
    memset(nBITS[entry->stsGpsIndex-1], '-', 20);
    nBITS[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsBITS->length = sprintf(nBITS[entry->stsGpsIndex-1], "BITS:%s", "NULL");
    entry->stsGpsBITS->octet_ptr = nBITS[entry->stsGpsIndex-1];
    memset(nPAVG[entry->stsGpsIndex-1], '-', 20);
    nPAVG[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsPAVG->length = sprintf(nPAVG[entry->stsGpsIndex-1], "PAVG:%s", "NULL");
    entry->stsGpsPAVG->octet_ptr = nPAVG[entry->stsGpsIndex-1];
    memset(nSIGQ[entry->stsGpsIndex-1], '-', 20);
    nSIGQ[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsSIGQ->length = sprintf(nSIGQ[entry->stsGpsIndex-1], "SIGQ:%s", "NULL");
    entry->stsGpsSIGQ->octet_ptr = nSIGQ[entry->stsGpsIndex-1];
    memset(nSPOS[entry->stsGpsIndex-1], '-', 20);
    nSPOS[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsSPOS->length = sprintf(nSPOS[entry->stsGpsIndex-1], "SPOS:%s", "NULL");
    entry->stsGpsSPOS->octet_ptr = nSPOS[entry->stsGpsIndex-1];
    memset(nTRMO[entry->stsGpsIndex-1], '-', 20);
    nTRMO[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsTRMO->length = sprintf(nTRMO[entry->stsGpsIndex-1], "TRMO:%s", "NULL");
    entry->stsGpsTRMO->octet_ptr = nTRMO[entry->stsGpsIndex-1];
    memset(nUTCT[entry->stsGpsIndex-1], '-', 20);
    nUTCT[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsUTCT->length = sprintf(nUTCT[entry->stsGpsIndex-1], "UTCT:%s", "NULL");
    entry->stsGpsUTCT->octet_ptr = nUTCT[entry->stsGpsIndex-1];
    memset(nVERS[entry->stsGpsIndex-1], '-', 20);
    nVERS[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsVERS->length = sprintf(nVERS[entry->stsGpsIndex-1], "VERS:%s", "NULL");
    entry->stsGpsVERS->octet_ptr = nVERS[entry->stsGpsIndex-1];
    memset(nDETF[entry->stsGpsIndex-1], '-', 20);
    nDETF[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsDETF->length = sprintf(nDETF[entry->stsGpsIndex-1], "DETF:%s", "NULL");
    entry->stsGpsDETF->octet_ptr = nDETF[entry->stsGpsIndex-1];
    memset(nEFER[entry->stsGpsIndex-1], '-', 20);
    nEFER[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsEFER->length = sprintf(nEFER[entry->stsGpsIndex-1], "EFER:%s", "NULL");
    entry->stsGpsEFER->octet_ptr = nEFER[entry->stsGpsIndex-1];
    memset(nESSD[entry->stsGpsIndex-1], '-', 20);
    nESSD[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsESSD->length = sprintf(nESSD[entry->stsGpsIndex-1], "ESSD:%s", "NULL");
    entry->stsGpsESSD->octet_ptr = nESSD[entry->stsGpsIndex-1];
    memset(nESSN[entry->stsGpsIndex-1], '-', 20);
    nESSN[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsESSN->length = sprintf(nESSN[entry->stsGpsIndex-1], "ESSN:%s", "NULL");
    entry->stsGpsESSN->octet_ptr = nESSN[entry->stsGpsIndex-1];
    memset(nFAGE[entry->stsGpsIndex-1], '-', 20);
    nFAGE[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsFAGE->length = sprintf(nFAGE[entry->stsGpsIndex-1], "FAGE:%s", "NULL");
    entry->stsGpsFAGE->octet_ptr = nFAGE[entry->stsGpsIndex-1];
    memset(nTIMD[entry->stsGpsIndex-1], '-', 20);
    nTIMD[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsTIMD->length = sprintf(nTIMD[entry->stsGpsIndex-1], "TIMD:%s", "NULL");
    entry->stsGpsTIMD->octet_ptr = nTIMD[entry->stsGpsIndex-1];
    entry->stsGpsReset = D_stsGpsReset_noAction;
    if (markdebug == 1) {
        printf
            ("\n...................done the kai null setting.......................\n");
    }
    return (0);
}



static int oscnullget(stsOscillatorEntry_t * entry)
{

   static char oscEFER[2][20];
    static char oscESSD[2][20];
    static char oscESSN[2][20];
    static char oscFAGE[2][20];
    static char oscTIMD[2][20];
    memset(oscEFER[entry->stsOscillatorIndex-1], '-', 20);
    oscEFER[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorEFER->length = sprintf(oscEFER[entry->stsOscillatorIndex-1], "EFER:%s", "NULL");
    entry->stsOscillatorEFER->octet_ptr = oscEFER[entry->stsOscillatorIndex-1];
    memset(oscESSD[entry->stsOscillatorIndex-1], '-', 20);
    oscESSD[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorESSD->length = sprintf(oscESSD[entry->stsOscillatorIndex-1], "ESSD:%s", "NULL");
    entry->stsOscillatorESSD->octet_ptr = oscESSD[entry->stsOscillatorIndex-1];
    memset(oscESSN[entry->stsOscillatorIndex-1], '-', 20);
    oscESSN[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorESSN->length = sprintf(oscESSN[entry->stsOscillatorIndex-1], "ESSN:%s", "NULL");
    entry->stsOscillatorESSN->octet_ptr = oscESSN[entry->stsOscillatorIndex-1];
    memset(oscFAGE[entry->stsOscillatorIndex-1], '-', 20);
    oscFAGE[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorFAGE->length = sprintf(oscFAGE[entry->stsOscillatorIndex-1], "FAGE:%s", "NULL");
    entry->stsOscillatorFAGE->octet_ptr = oscFAGE[entry->stsOscillatorIndex-1];
    memset(oscTIMD[entry->stsOscillatorIndex-1], '-', 20);
    oscTIMD[entry->stsOscillatorIndex-1][19] = 0;
    entry->stsOscillatorTIMD->length = sprintf(oscTIMD[entry->stsOscillatorIndex-1], "TIMD:%s", "NULL");
    entry->stsOscillatorTIMD->octet_ptr = oscTIMD[entry->stsOscillatorIndex-1];
    entry->stsOscillatorRedStatus =
        oscGetRedStatus(entry->stsOscillatorIndex);
    entry->stsOscillatorSwitchState =
        oscGetSwitchState(entry->stsOscillatorIndex);
    entry->stsOscillatorStatus = 77;
    return (0);
}


/******************************************************************************
function:
gpsGet30 - the task to get the driver response from the gps device
              
paramenter: 
 stsGpsEntry_t *entry - the struct to store response data for gps query response
 serialGpsGpgll_t *gll - the struct to store the built in test status
 serialGpsGpmss_t *mss - the struct to store the signal quality
 serialGpsGpgga_t *gga - the struct to store the latitude,longitude, altitude and the UTC  
 serialGpsGpgsv_t *gsv - the struct to store the azimuth & elevation
 int portNum - the port number of the device
  
 RETURNS: 0 if ok. 
*/
static int gpsGet30(void *input, int portNum, stsGpsEntry_t * entry){
    struct module2 *ju30 = (struct module2 *) input;
    serialGpsGpgll_t *jugpgll = &ju30->gpgll;
    serialGpsGpmss_t *jugpmss = &ju30->gpmss;
    serialGpsGpgga_t *jugpgga = &ju30->gpgga;
    serialGpsGpgsv_t *jugpgsv = &ju30->gpgsv;
    int back = 0;
    int no_poorsig;
    char buffer1[2] = "";
    char buffer2[2] = "";
    static char ANTD[2][20];
    static char AZEL[2][40];
    static char RSTG[2][30];
    static char BITS[2][30];
    static char PAVG[2][140];
    static char SIGQ[2][20];
    static char SPOS[2][60];
    static char TRMO[2][20];
    static char UTCT[2][40];
    static char VERS[2][10];
    static char DETF[2][20];
    static char EFER[2][20];
    static char ESSD[2][20];
    static char ESSN[2][20];
    static char FAGE[2][20];
    static char TIMD[2][20];
    int que;
    entry->stsGpsState = getGpsRedState(portNum);
    que = GPGGA30(jugpgga, portNum);
    if (que == 0 || que == 7) {
        GPGSV30(jugpgsv, portNum);
        no_poorsig = GPGSV30(jugpgsv, portNum);
        if (no_poorsig == 0) {
            entry->stsGpsStatus = D_stsGpsStatus_noSignal;
            GPGLL30(jugpgll, portNum);
            GPMSS30(jugpmss, portNum);
        }
        else if (no_poorsig == 5 || no_poorsig == 6 || no_poorsig == 7
                 || no_poorsig == 8) {
            entry->stsGpsStatus = D_stsGpsStatus_ok;
            GPGLL30(jugpgll, portNum);
            GPMSS30(jugpmss, portNum);
        }
        else if (no_poorsig == 1 || no_poorsig == 2 || no_poorsig == 3
                 || no_poorsig == 4) {
            entry->stsGpsStatus = D_stsGpsStatus_poorSignal;
            GPGLL30(jugpgll, portNum);
            GPMSS30(jugpmss, portNum);
        }
        else {
            back = -3;
            return back;
        }
    }
    else {
        if (markdebug == 1) {
            printf("status nonAvailable\n");
        }
        entry->stsGpsStatus = D_stsGpsStatus_nonAvailable;
        back = -2;
        return back;
    }
    if (markdebug == 1) {
        printf("active\n");
    }
    memcpy(buffer1, jugpgga->direction1, 1);
    memcpy(buffer2, jugpgga->direction2, 1);
    
    memset(ANTD[entry->stsGpsIndex-1], '-', 20);
    ANTD[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsANTD->length = sprintf(ANTD[entry->stsGpsIndex-1], "ANTD:%s", "not supported");
    entry->stsGpsANTD->octet_ptr = ANTD[entry->stsGpsIndex-1];

    memset(AZEL[entry->stsGpsIndex-1], '-', 40);
    AZEL[entry->stsGpsIndex-1][39] = 0;
    entry->stsGpsAZEL->length =
        sprintf(AZEL[entry->stsGpsIndex-1], "GEO #:%d, elevation:%d, azimuth:%d",
                jugpgsv->nomssg, jugpgsv->sea1, jugpgsv->sba1);
    entry->stsGpsAZEL->octet_ptr = AZEL[entry->stsGpsIndex-1];

    memset(RSTG[entry->stsGpsIndex-1], '-', 30);
    RSTG[entry->stsGpsIndex-1][29] = 0;
    entry->stsGpsRSTG->length =
        sprintf(RSTG[entry->stsGpsIndex-1], "reset:%s", "now is restarting");
    entry->stsGpsRSTG->octet_ptr = RSTG[entry->stsGpsIndex-1];

    memset(BITS[entry->stsGpsIndex-1], '-', 30);
    BITS[entry->stsGpsIndex-1][29] = 0;
    entry->stsGpsBITS->length =
        sprintf(BITS[entry->stsGpsIndex-1], "build-in test status:%s", jugpgll->status);
    entry->stsGpsBITS->octet_ptr = BITS[entry->stsGpsIndex-1];

    memset(PAVG[entry->stsGpsIndex-1], '-', 140);
    PAVG[entry->stsGpsIndex-1][139] = 0;
    entry->stsGpsPAVG->length =
        sprintf(PAVG[entry->stsGpsIndex-1],
                "Latitude:[%d]degree, [%d]min_int, [%d]min_frac, [%s]direction\n"
                "Longitude:[%d]degree, [%d]min_int, [%d]min_frac, [%s]direction\n"
                "Altitude:[%.1f]",
                jugpgga->latitude.degree,
                jugpgga->latitude.minuate, jugpgga->latitude.fraction,
                buffer1, jugpgga->longitude.degree,
                jugpgga->longitude.minuate, jugpgga->longitude.fraction,
                buffer2, jugpgga->altitude);
    entry->stsGpsPAVG->octet_ptr = PAVG[entry->stsGpsIndex-1];

    memset(SIGQ[entry->stsGpsIndex-1], '-', 20);
    SIGQ[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsSIGQ->length =
        sprintf(SIGQ[entry->stsGpsIndex-1], "signal is:%d", jugpmss->sig_strength);
    entry->stsGpsSIGQ->octet_ptr = SIGQ[entry->stsGpsIndex-1];

    memset(SPOS[entry->stsGpsIndex-1], '-', 60);
    SPOS[entry->stsGpsIndex-1][59] = 0;
    entry->stsGpsSPOS->length = sprintf(SPOS[entry->stsGpsIndex-1], "SPOS:%s", "not supported");
    entry->stsGpsSPOS->octet_ptr = SPOS[entry->stsGpsIndex-1];

    memset(TRMO[entry->stsGpsIndex-1], '-', 20);
    TRMO[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsTRMO->length = sprintf(TRMO[entry->stsGpsIndex-1], "TRMO:%s", "not supported");
    entry->stsGpsTRMO->octet_ptr = TRMO[entry->stsGpsIndex-1];

    memset(UTCT[entry->stsGpsIndex-1], '-', 40);
    UTCT[entry->stsGpsIndex-1][39] = 0;
    entry->stsGpsUTCT->length =
        sprintf(UTCT[entry->stsGpsIndex-1], "[%d]hour, [%d]minutes, [%d]seconds",
                jugpgga->utc.hh, jugpgga->utc.mm, jugpgga->utc.ss);
    entry->stsGpsUTCT->octet_ptr = UTCT[entry->stsGpsIndex-1];

    memset(VERS[entry->stsGpsIndex-1], '-', 10);
    VERS[entry->stsGpsIndex-1][9] = 0;
    entry->stsGpsVERS->length =
        sprintf(VERS[entry->stsGpsIndex-1], "version:%s", "not supported");
    entry->stsGpsVERS->octet_ptr = VERS[entry->stsGpsIndex-1];

    memset(DETF[entry->stsGpsIndex-1], '-', 20);
    DETF[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsDETF->length = sprintf(DETF[entry->stsGpsIndex-1], "DETF:%s", "not supported");
    entry->stsGpsDETF->octet_ptr = DETF[entry->stsGpsIndex-1];

    memset(EFER[entry->stsGpsIndex-1], '-', 20);
    EFER[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsEFER->length = sprintf(EFER[entry->stsGpsIndex-1], "EFER:%s", "not supported");
    entry->stsGpsEFER->octet_ptr = EFER[entry->stsGpsIndex-1];

    memset(ESSD[entry->stsGpsIndex-1], '-', 20);
    ESSD[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsESSD->length = sprintf(ESSD[entry->stsGpsIndex-1], "ESSD:%s", "not supported");
    entry->stsGpsESSD->octet_ptr = ESSD[entry->stsGpsIndex-1];

    memset(ESSN[entry->stsGpsIndex-1], '-', 20);
    ESSN[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsESSN->length = sprintf(ESSN[entry->stsGpsIndex-1], "ESSN:%s", "not supported");
    entry->stsGpsESSN->octet_ptr = ESSN[entry->stsGpsIndex-1];

    memset(FAGE[entry->stsGpsIndex-1], '-', 20);
    FAGE[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsFAGE->length = sprintf(FAGE[entry->stsGpsIndex-1], "FAGE:%s", "not supported");
    entry->stsGpsFAGE->octet_ptr = FAGE[entry->stsGpsIndex-1];

    memset(TIMD[entry->stsGpsIndex-1], '-', 20);
    TIMD[entry->stsGpsIndex-1][19] = 0;
    entry->stsGpsTIMD->length = sprintf(TIMD[entry->stsGpsIndex-1], "TIMD:%s", "not supported");
    entry->stsGpsTIMD->octet_ptr = TIMD[entry->stsGpsIndex-1];
    entry->stsGpsReset = D_stsGpsReset_noAction;
    if (markdebug == 1) {
        printf
            ("\n...................done the kai gps jupiter 30 testing.......................\n");
    }
    return back;
}


/******************************************************************************
function:
gpsGetPico - the task to get the driver response from the gps device
              
paramenter: 
 stsGpsEntry_t *entry - the struct to store response data for gps query response
 serialGpsPicoANTD_t *az - the struct to store the attenna cable delay
 serialGpsPicoAZEL_t *bb - the struct to store the azimuth & elevation
 serialGpsGpgga_t *gga - the struct to store the latitude,longitude, altitude and the UTC  
 int portNum - the port number of the device
  
 RETURNS: 0 if ok. 
*/
static int gpsGetPico(void *input, int portNum, stsGpsEntry_t * entry)
{

    struct module3 *Pico = (struct module3 *) input;

    serialGpsPicoANTD_t *Picoantd = &Pico->az;

    serialGpsPicoAZEL_t *Picoazel = &Pico->bb;

    serialGpsGpgga_t *Picogpgga = &Pico->gpgga;

    int back = 0;

    char ANTD[30], AZEL[45], RSTG[50], BITS[50];

    char PAVG[400], SIGQ[100], SPOS[400];

    char TRMO[400];

    char UTCT[500], VERS[20], DETF[30], EFER[30];

    char ESSD[30], ESSN[30], FAGE[30], TIMD[30];

    int que;
    if (markdebug == 1) {

        printf("\n................doing kai...........................\n");

    }

    que = EApicot(Picogpgga, portNum);

    entry->stsGpsState = getGpsRedState(portNum);

    if (que == 0 || que == 7) {

        AZpicot(Picoantd, portNum);

        BBpicot(Picoazel, portNum);

    }

    else {

        entry->stsGpsStatus = D_stsGpsStatus_nonAvailable;

        back = -2;

    }

    if (entry->stsGpsState == D_stsGpsState_active) {

        sprintf(ANTD, "ANTD:%s", "not supported");

        (char *) entry->stsGpsANTD = ANTD;

        sprintf(AZEL, "GEO #:%d, elevation:%d, azimuth:%d",
                Picoazel->nosatellite, Picoazel->ele1, Picoazel->azi1);

        (char *) entry->stsGpsAZEL = AZEL;

        sprintf(RSTG, "reset:%s", "now is restarting...");

        (char *) entry->stsGpsRSTG = RSTG;

        sprintf(BITS, "build-in test status:%s", "not supported");

        (char *) entry->stsGpsBITS = BITS;

        sprintf(PAVG,
                "Latitude:[%d]degree, [%d]min_int, [%d]min_frac\n"
                "Longitude:[%d]degree, [%d]min_int, [%d]min_frac\n"
                "Altitude:[%.1f]",
                Picogpgga->latitude.degree,
                Picogpgga->latitude.minuate,
                Picogpgga->latitude.fraction,
                Picogpgga->longitude.degree,
                Picogpgga->longitude.minuate,
                Picogpgga->longitude.fraction, Picogpgga->altitude);

        (char *) entry->stsGpsPAVG = PAVG;

        sprintf(SIGQ, "SIGQ:%s", "not supported");

        (char *) entry->stsGpsSIGQ = SIGQ;

        /*sprintf(SPOS, "latitude:[%.4f],[%s],longitude:[%.4f],[%s]", xxa->lat, buffer3, xxa->longitudef, buffer4); */
        sprintf(SPOS, "SPOS:%s", "not supported");

        (char *) entry->stsGpsSPOS = SPOS;

        sprintf(TRMO, "TRMO:%s", "not supported");

        (char *) entry->stsGpsTRMO = TRMO;

        sprintf(UTCT, "[%d]hour, [%d]minutes, [%d]seconds",
                Picogpgga->utc.hh, Picogpgga->utc.mm, Picogpgga->utc.ss);

        (char *) entry->stsGpsUTCT = UTCT;

        sprintf(VERS, "version:%s", "not supported");

        (char *) entry->stsGpsVERS = VERS;

        sprintf(DETF, "DETF:%s", "not supported");

        (char *) entry->stsGpsDETF = DETF;

        sprintf(EFER, "EFER:%s", "not supported");

        (char *) entry->stsGpsEFER = EFER;

        sprintf(ESSD, "ESSD:%s", "not supported");

        (char *) entry->stsGpsESSD = ESSD;

        sprintf(ESSN, "ESSN:%s", "not supported");

        (char *) entry->stsGpsESSN = ESSN;

        sprintf(FAGE, "FAGE:%s", "not supported");

        (char *) entry->stsGpsFAGE = FAGE;

        sprintf(TIMD, "TIMD:%s", "not supported");

        (char *) entry->stsGpsTIMD = TIMD;

        entry->stsGpsReset = D_stsGpsReset_noAction;

    }

    /*      
       else if (entry->stsGpsState == D_stsGpsState_standby){
       printf("standby\n");
       printf(" alarm No gps-device respond on scc3 or scc1 port\n");
       } */
    if (markdebug == 1) {

        printf
            ("\n...................done the kai gps jupiter pico t testing.......................\n");

    }

    return back;
}


/******************************************************************************
function:
idxCalculate - to find out index of uartApp[] according to command string and device type 
               field mapping to those of the uartApp[] table.

paramenter:
  char *cmdString :    pointer to command string.
  int len : 		   length of command string.											
  int *idx :           the mapping index.   
  DEV_TYPE dev :       device type. ( gpsDev, cdmaDev, sroDev, sxoDev)
               
 RETURNS: N/A 
*/
void idxCalculate(char *cmdString, int len, int *idx, DEV_TYPE dev)
{

    int ii = 0;

    while ((strncmp(uartApp[ii].cmdName, cmdString, len) != 0)
           || (uartApp[ii].devType != dev))
    {

        ++ii;

    }

    *idx = ii;
}


/***********************************************************************************************
function: sccGPGGA
paramenter:
  serialGpsGpgga_t *my_gpggaData :		the struct to store response data for GPGGA command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int sccGPGGA(serialGpsGpgga_t * my_gpggaData, int port){
    int len;
#if 0                           /* not to compiler */
    int i = 0;
#endif 
    int idx = 0;
    int check;
    DEV_TYPE dev = gpsDev;
    char rxBuf[80];
    char *cmdString = "$PFEC,GPint,GGA00\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-1);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
#if 0
    while ((rxBuf[i] != '\0') || (i == 80))
        logMsg("rxBuf[%d]=%c\r\n", i, rxBuf[i++], 0, 0, 0, 0);
#endif 
    check = uartApp[idx].rxParseFunc(rxBuf, my_gpggaData);
    if (markdebug == 1) {
        if (check)
        {                       /* response data pasing */
            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);
        }
        else {
            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);
        }
    }
#if 0
    gpggaPrint(my_gpggaData);
#endif 
    return check;
}


/***********************************************************************************************
function: sccGPZDA
paramenter:
  serialGpsGpzda_t *my_gpzdaData :		the struct to store response data for GPZDA command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int sccGPZDA(serialGpsGpzda_t * my_gpzdaData, int port)
{

    int len;

#if 0                           /* not to compiler */
    int i = 0;

#endif                          /* 
                                 */
    int idx = 0;

    int check;

    DEV_TYPE dev = gpsDev;

    char rxBuf[80];

    char *cmdString = "$PFEC,GPint,ZDA00\r\n";

    len = strlen(cmdString);

    idxCalculate(cmdString, len, &idx, dev);

    if ((port == SCC1PORT) || (port == SCC3PORT));

    else
        return (-1);

    uartCmdAct(cmdString, len, rxBuf, 1, port);

#if 0
    while ((rxBuf[i] != '\0') || (i == 80))

        logMsg("rxBuf[%d]=%c\r\n", i, rxBuf[i++], 0, 0, 0, 0);

#endif                          /* 
                                 */
    check = uartApp[idx].rxParseFunc(rxBuf, my_gpzdaData);

    if (markdebug == 1) {

        if (check)
        {                       /* response data pasing */

            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);

        }

        else {

            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);

        }

    }
#if 0
    gpggaPrint(my_gpggaData);

#endif                          /* 
                                 */
    return check;
}


/***********************************************************************************************
function: GPZDA30                       jupiter 30
paramenter:
  serialGpsGpzda_t *my_gpzdaData :		the struct to store response data for GPZDA command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int GPZDA30(serialGpsGpzda_t * my_gpzdaData, int port)
{

    int len;

#if 0                           /* not to compiler */
    int i = 0;

#endif                          /* 
                                 */
    int idx = 0;

    int check;

    DEV_TYPE dev = gpsDev;

    char rxBuf[80];

    char *cmdString = "$PSRF103,08,01,00,01*2d\r\n";

    len = strlen(cmdString);

    idxCalculate(cmdString, len, &idx, dev);

    if ((port == SCC1PORT) || (port == SCC3PORT));

    else
        return (-1);

    uartCmdAct(cmdString, len, rxBuf, 1, port);

#if 0
    while ((rxBuf[i] != '\0') || (i == 80))

        logMsg("rxBuf[%d]=%c\r\n", i, rxBuf[i++], 0, 0, 0, 0);

#endif                          /* 
                                 */
    check = uartApp[idx].rxParseFunc(rxBuf, my_gpzdaData);

    if (markdebug == 1) {

        if (check)
        {                       /* response data pasing */

            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);

        }

        else {

            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);

        }

    }
#if 0
    gpggaPrint(my_gpzdaData);

#endif                          /* 
                                 */
    return check;
}


/***********************************************************************************************
function: sccGPGSV
paramenter:
  serialGpsGpgsv_t *my_gpgsvData :		the struct to store response data for GPGSV command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int sccGPGSV(serialGpsGpgsv_t * my_gpgsvData, int port){
    int len;
    int idx = 0;
    int check;
    DEV_TYPE dev = gpsDev;
    char rxBuf[80];
    char *cmdString = "$PFEC,GPint,GSV00\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-1);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    check = uartApp[idx].rxParseFunc(rxBuf, my_gpgsvData);
    if (markdebug == 1) {
        if (check)
        {                       /* response data pasing */
            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);
        }
        else {
            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);
        }
    }
    return check;
}


/***********************************************************************************************
function: EApicot                  jupiter pico t
paramenter:
  serialGpsGpgga_t *my_eapicotData :		the struct to store response data for GPGGA jupiter 30 command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int EApicot(serialGpsGpgga_t * my_eapicotData, int port)
{

    int len;

#if 0                           /* not to compiler */
    int i = 0;

#endif                          /* 
                                 */
    int idx = 0;

    int check;

    DEV_TYPE dev = gpsDev;

    char rxBuf[80];

    char *cmdString = "@@Ea0*54\r\n";

    len = strlen(cmdString);

    idxCalculate(cmdString, len, &idx, dev);

    if ((port == SCC1PORT) || (port == SCC3PORT));

    else
        return (-1);

    uartCmdAct(cmdString, len, rxBuf, 1, port);

#if 0
    while ((rxBuf[i] != '\0') || (i == 80))

        logMsg("rxBuf[%d]=%c\r\n", i, rxBuf[i++], 0, 0, 0, 0);

#endif                          /* 
                                 */
    check = uartApp[idx].rxParseFunc(rxBuf, my_eapicotData);

    if (markdebug == 1) {

        if (check)
        {                       /* response data pasing */

            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);

        }

        else {

            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);

        }

    }
#if 0
    gpggaPrint(my_eapicotData);

#endif                          /* 
                                 */
    return check;
}


/***********************************************************************************************
function: AZpicot                  jupiter pico t
paramenter:
  serialGpsPicoANTD_t *my_azpicotData :		the struct to store response data for antenna cable delay command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int AZpicot(serialGpsPicoANTD_t * my_azpicotData, int port)
{

    int len;

#if 0                           /* not to compiler */
    int i = 0;

#endif                          /* 
                                 */
    int idx = 0;

    int check;

    DEV_TYPE dev = gpsDev;

    char rxBuf[80];

    char *cmdString = "@@Az0*0b\r\n";

    len = strlen(cmdString);

    idxCalculate(cmdString, len, &idx, dev);

    if ((port == SCC1PORT) || (port == SCC3PORT));

    else
        return (-1);

    uartCmdAct(cmdString, len, rxBuf, 1, port);

#if 0
    while ((rxBuf[i] != '\0') || (i == 80))

        logMsg("rxBuf[%d]=%c\r\n", i, rxBuf[i++], 0, 0, 0, 0);

#endif                          /* 
                                 */
    check = uartApp[idx].rxParseFunc(rxBuf, my_azpicotData);

    if (markdebug == 1) {

        if (check)
        {                       /* response data pasing */

            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);

        }

        else {

            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);

        }

    }
#if 0
    gpggaPrint(my_azpicotData);

#endif                          /* 
                                 */
    return check;
}


/***********************************************************************************************
function: BBpicot                  jupiter pico t
paramenter:
  serialGpsPicoAZEL_t *my_bbpicotData :		the struct to store response data for azimuth & elevation command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int BBpicot(serialGpsPicoAZEL_t * my_bbpicotData, int port)
{

    int len;

#if 0                           /* not to compiler */
    int i = 0;

#endif                          /* 
                                 */
    int idx = 0;

    int check;

    DEV_TYPE dev = gpsDev;

    char rxBuf[80];

    char *cmdString = "@@Bb0*10\r\n";

    len = strlen(cmdString);

    idxCalculate(cmdString, len, &idx, dev);

    if ((port == SCC1PORT) || (port == SCC3PORT));

    else
        return (-1);

    uartCmdAct(cmdString, len, rxBuf, 1, port);

#if 0
    while ((rxBuf[i] != '\0') || (i == 80))

        logMsg("rxBuf[%d]=%c\r\n", i, rxBuf[i++], 0, 0, 0, 0);

#endif                          /* 
                                 */
    check = uartApp[idx].rxParseFunc(rxBuf, my_bbpicotData);

    if (markdebug == 1) {

        if (check)
        {                       /* response data pasing */

            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);

        }

        else {

            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);

        }

    }
#if 0
    gpggaPrint(my_bbpicotData);

#endif                          /* 
                                 */
    return check;
}


/***********************************************************************************************
function: GPGGA30                    jupiter 30
paramenter:
  serialGpsGpgga_t *my_gpgga30Data :		the struct to store response data for GPGGA jupiter 30 command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int GPGGA30(serialGpsGpgga_t * my_gpgga30Data, int port)
{

    int len;

#if 0                           /* not to compiler */
    int i = 0;

#endif                          /* 
                                 */
    int idx = 0;

    int check;

    DEV_TYPE dev = gpsDev;

    char rxBuf[80];

    char *cmdString = "$PSRF103,00,01,00,01*25\r\n";

    len = strlen(cmdString);

    idxCalculate(cmdString, len, &idx, dev);

    if ((port == SCC1PORT) || (port == SCC3PORT));

    else
        return (-1);

    uartCmdAct(cmdString, len, rxBuf, 1, port);

#if 0
    while ((rxBuf[i] != '\0') || (i == 80))

        logMsg("rxBuf[%d]=%c\r\n", i, rxBuf[i++], 0, 0, 0, 0);

#endif                          /* 
                                 */
    check = uartApp[idx].rxParseFunc(rxBuf, my_gpgga30Data);

    if (markdebug == 1) {

        if (check)
        {                       /* response data pasing */

            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);

        }

        else {

            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);

        }

    }
#if 0
    gpggaPrint(my_gpgga30Data);

#endif                          /* 
                                 */
    return check;
}


/***********************************************************************************************
function: GPGSV30                  jupiter 30
paramenter:
  serialGpsGpgsv_t *my_gpgsvData :		the struct to store response data for GPGSV command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int GPGSV30(serialGpsGpgsv_t * my_gpgsvData, int port){
    int len;
    int idx = 0;
    int check;
    DEV_TYPE dev = gpsDev;
    char rxBuf[80];
    char *cmdString = "$PSRF103,03,01,00,01*26\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-1);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    check = uartApp[idx].rxParseFunc(rxBuf, my_gpgsvData);
    if (markdebug == 1) {
        if (check)
        {                       /* response data pasing */
            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);
        }
        else {
            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);
        }
    }
    return check;
}


/***********************************************************************************************
function: GPGLL30        jupiter 30
paramenter:
  serialGpsGpgll_t *my_gpgll30Data :		the struct to store response data for GPGLL command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int GPGLL30(serialGpsGpgll_t * my_gpgllData, int port)
{

    int len;

    int idx = 0;

    int check;

    DEV_TYPE dev = gpsDev;

    char rxBuf[80];

    char *cmdString = "$PSRF103,01,01,00,01*24\r\n";

    len = strlen(cmdString);

    idxCalculate(cmdString, len, &idx, dev);

    if ((port == SCC1PORT) || (port == SCC3PORT));

    else
        return (-1);

    uartCmdAct(cmdString, len, rxBuf, 1, port);

    check = uartApp[idx].rxParseFunc(rxBuf, my_gpgllData);

    if (markdebug == 1) {

        if (check)
        {                       /* response data pasing */

            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);

        }

        else {

            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);

        }

    }

    return check;
}


/***********************************************************************************************
function: GPMSS30        jupiter 30
paramenter:
  serialGpsGpmss_t *my_gpmss30Data :		the struct to store response data for GPMSS command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int GPMSS30(serialGpsGpmss_t * my_gpmssData, int port)
{

    int len;

    int idx = 0;

    int check;

    DEV_TYPE dev = gpsDev;

    char rxBuf[80];

    char *cmdString = "$PSRF103,06,01,00,01*23\r\n";

    len = strlen(cmdString);

    idxCalculate(cmdString, len, &idx, dev);

    if ((port == SCC1PORT) || (port == SCC3PORT));

    else
        return (-1);

    uartCmdAct(cmdString, len, rxBuf, 1, port);

    check = uartApp[idx].rxParseFunc(rxBuf, my_gpmssData);

    if (markdebug == 1) {

        if (check)
        {                       /* response data pasing */

            logMsg("Parameter pasing error, check=%d\r\n", check, 0, 0, 0,
                   0, 0);

        }

        else {

            logMsg("BettaOKOKOK\r\n", 0, 0, 0, 0, 0, 0);

        }

    }

    return check;
}


/***********************************************************************************************
function: sccXXGGA
paramenter:
  serialGpsXXGGA_t *xxggaData :		the struct to store response data for XXGGA command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int sccXXGGA(char *string2Parsed, serialGpsXXGGA_t * xxggaData,
                    int port)
{

/* "latitude:4888.0770,N,longtitude:12345.4700,E"  <== must follow this kind of string */
    char *mixedtoken;

    int whichtoken = 0;

    int cmdError = 0;

    int len = 0;

    char cmdString[80];

    char buffer1[2] = "";

    char buffer2[2] = "";

    if (string2Parsed[0] == '\0')
        return 1;
    if (markdebug == 1) {

        printf("Splitting string :%s\n", string2Parsed);
    }

    mixedtoken = strtok(string2Parsed, ":,\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            if (strncmp(mixedtoken, "latitude", 8) == 0) {

                cmdError = 0;
                if (markdebug == 1) {

                    printf
                        ("Yes, this is the setting initial position command\n");
                }

            }

            else {

                cmdError = -1;

            }

            break;

        case 1:

            xxggaData->lat = atof(mixedtoken);

            break;

/* "latitude:4888.0770,N,longtitude:12345.4700,E" */
        case 2:

            switch (*(mixedtoken)) {

            case 'N':

                memcpy(xxggaData->directNS, "N", 1);

                break;

            case 'S':

                memcpy(xxggaData->directNS, "S", 1);

                break;

            default:

                cmdError = -3;

            }

            break;

        case 3:

            if (strncmp(mixedtoken, "longtitude", 10) == 0) {

                cmdError = 0;

            }

            else {

                cmdError = -4;

            }

            break;

        case 4:

            xxggaData->longitudef = atof(mixedtoken);

            break;

        case 5:

            switch (*(mixedtoken)) {

            case 'E':

                memcpy(xxggaData->directEW, "E", 1);

                break;

            case 'W':

                memcpy(xxggaData->directEW, "W", 1);

                break;

            default:

                cmdError = -5;

            }

            break;

        default:

            cmdError = -7;

            break;

        }

        mixedtoken = strtok(NULL, ":,\r\n");

        whichtoken++;

    }

    memcpy(buffer1, xxggaData->directNS, 1);

    memcpy(buffer2, xxggaData->directEW, 1);

    sprintf(cmdString, "$XXGGA,,%.4f,%s,%.4f,%s,,,,,,,,,\r\n",
            xxggaData->lat, buffer1, xxggaData->longitudef, buffer2);

    len = strlen(cmdString);

    printf("the xxgga cmdstring is:%s\n", cmdString);

    memset(buffer1, 0, 2);

    memset(buffer2, 0, 2);

    if ((port == 1) || (port == 3));

    else
        return (-1);

    uartCmdAct(cmdString, len, NULL, 0, port);
    return cmdError;
}


/***********************************************************************************************
function: XXGGA30      jupiter 30
paramenter:
  serialGpsSPOS30_t *spos30Data :		the struct to store response data for PSRF104 command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int XXGGA30(char *string2Parsed, serialGpsSPOS30_t * spos30Data,
                   int port)
{

/* "latitude:4888.0770,N,longtitude:12345.4700,E" ==> GT8031
	 "latitude:12.3875111, longtitude:121.97232 , altitude:1234, reset:2" ==> jupiter 30 */
    char *mixedtoken;

    int whichtoken = 0;

    int cmdError = 0;

    int len = 0;

    char rxBuf[80];

    char cmdString[80];

    if (string2Parsed[0] == '\0')
        return 1;
    if (markdebug == 1) {

        printf("Splitting string :%s\n", string2Parsed);
    }

    mixedtoken = strtok(string2Parsed, ":,\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            if (strncmp(mixedtoken, "latitude", 8) == 0) {

                cmdError = 0;
                if (markdebug == 1) {

                    printf
                        ("Yes, this is the setting initial position for jupiter 30 command\n");
                }

            }

            else {

                cmdError = -1;

            }

            break;

        case 1:

            spos30Data->lat = atof(mixedtoken);

            break;

/* "latitude:12.3875111, longtitude:121.97232" ==> jupiter 30 */
        case 2:

            if (strncmp(mixedtoken, "longtitude", 10) == 0) {

                cmdError = 0;

            }

            else {

                cmdError = -2;

            }

            break;

        case 3:

            spos30Data->longitudef = atof(mixedtoken);

            break;

        case 4:

            if (strncmp(mixedtoken, "altitude", 8) == 0) {

                cmdError = 0;

            }

            else {

                cmdError = -3;

            }

            break;

        case 5:

            spos30Data->alt = atof(mixedtoken);

            break;

        case 6:

            if (strncmp(mixedtoken, "reset", 5) == 0) {

                cmdError = 0;

            }

            else {

                cmdError = -4;

            }

            break;

        case 7:

            spos30Data->reset = atof(mixedtoken);

            break;

        default:

            cmdError = -5;

            break;

        }


        mixedtoken = strtok(NULL, ":,\r\n");

        whichtoken++;

    }

    sprintf(cmdString, "$PSRF104,%.7f,%.5f,%f,0,0,0,1,%d*07\r\n",
            spos30Data->lat, spos30Data->longitudef, spos30Data->alt,
            spos30Data->reset);

    sprintf(positionRead30,
            "the reference of jupiter 30latitude is:%.7f,longitude is:%.7f,altitude:%f, reset:%d\n",
            spos30Data->lat, spos30Data->longitudef, spos30Data->alt,
            spos30Data->reset);

    len = strlen(cmdString);

    printf("the $psrf104 cmdstring is:%s\n", cmdString);

    if ((port == SCC1PORT) || (port == SCC3PORT));

    else
        return (-1);


    uartCmdAct(cmdString, len, rxBuf, 1, port);

    return cmdError;
}


/***********************************************************************************************
function: sccGTReset
paramenter:
  serialGpsGpclr_t *my_gpclrData :		the struct to store gpclrcommand  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int sccGTReset(void *input, int portNum, stsGpsEntry_t * entry){
	struct module1 *GT = (struct module1 *) input;    
	serialGpsGpclr_t *GTgpclr = &GT->gpclr;
	serialGpsGpgga_t *GTgpgga = &GT->gpgga;
    int back = 0;
    int que;	
    que = sccGPGGA(GTgpgga, portNum);
    if (que == GPS_FIX_INVALID || que == OK) {
        GTReset(GTgpclr, portNum);
    }
    else {
        if (markdebug == 1) {
            printf("status nonAvailable\n");
        }
        back = -2;
        return back;
    }	
    return back;
}

/***********************************************************************************************
function: gpsReset30
paramenter:
  serialGpsGpclr_t *my_gpclrData :		the struct to store gpclrcommand  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int gpsReset30(void *input, int portNum, stsGpsEntry_t * entry){
	struct module2 *j30 = (struct module2 *) input;    
	serialGpsSPOS30_t *j30gprst = &j30->gprst;
	serialGpsGpgga_t *j30gpgga = &j30->gpgga;	
    int back = 0;
    int que;	
    que = GPGGA30(j30gpgga, portNum);
    if (que == GPS_FIX_INVALID || que == OK) {
        Reset30(j30gprst, portNum);
    }
    else {
        if (markdebug == 1) {
            printf("no device query\n");
        }
        back = -2;
        return back;
    }	
    return back;
}

/***********************************************************************************************
function: GTReset
paramenter:
  serialGpsGpclr_t *my_gpclrData :		the struct to store gpclrcommand  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	
*/
static int GTReset(serialGpsGpclr_t * my_gpclrData, int port){
    int len = 0;
    int check;
    char *cmdString;
    sprintf(cmdString, "$PFEC,GPclr,%d\r\n", my_gpclrData->mode2);
    len = strlen(cmdString);
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-1);
    uartCmdAct(cmdString, len, NULL, 0, port);
    return check;
}

/***********************************************************************************************
function: Reset30           jupiter 30
paramenter:
  serialGpsSPOS30_t *my_gpclrData :		the struct to store reset command  
	int port	:		                to which serial port to send
return:
	0: ok
	other: error code [to be continued]
	

*/
static int Reset30(serialGpsSPOS30_t * my_gpclrData, int port){
    int len = 0;
    int check;
    char *cmdString;
    sprintf(cmdString, "$PSRF104,0,0,0,0,0,0,1,%d*07\r\n",
            my_gpclrData->reset);
    len = strlen(cmdString);
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-1);
    uartCmdAct(cmdString, len, NULL, 0, port);
    return check;
}


/*************************************************************************
function: picoteaParsing                      jupiter pico t
paramenter:
  serialGpsGpgga_t *eaData : 			the struct to store jupiter pico t response data for Ea command.  
  char *string2Parsed : pointer to response data for Ea command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int picoteaParsing(char *string2Parsed, serialGpsGpgga_t * eaData)
{

    char *mixedtoken;

    char *token;

    int whichtoken = 0;

    int cmdError = 0;

    if (string2Parsed[0] == '\0')
        return 1;

    if (markdebug == 1) {

        printf("Splitting string:%s\n", string2Parsed);

    }

    mixedtoken = strtok(string2Parsed, "@\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            token = (char *) malloc(10);

            memset(token, 0, 10);

            memcpy(token, mixedtoken, 2);

            if (strncmp(token, "Ea", 2) == 0) {

                printf("token here is:%s\n", token);

                printf
                    ("Yes, this is position altitude of the jupiter pico t command\n");

                memcpy(token, mixedtoken + 2, 2);

                eaData->utc.mn = atoi(token);

                printf("1token here is:%s\n", token);

                memcpy(token, mixedtoken + 4, 2);

                eaData->utc.dy = atoi(token);

                printf("2token here is:%s\n", token);

                memcpy(token, mixedtoken + 6, 4);

                eaData->utc.yy = atoi(token);

                printf("3token here is:%s\n", token);

                memset(token, 0, 10);

                memcpy(token, mixedtoken + 10, 2);

                eaData->utc.hh = atoi(token);

                printf("4token here is:%s\n", token);

                memcpy(token, mixedtoken + 12, 2);

                eaData->utc.mm = atoi(token);

                printf("5token here is:%s\n", token);

                memcpy(token, mixedtoken + 14, 2);

                eaData->utc.ss = atoi(token);

                printf("6token here is:%s\n", token);

                memset(token, 0, 10);

                memcpy(token, mixedtoken + 16, 4);

                eaData->fraction = atoi(token);

                printf("7token here is:%s\n", token);

                memset(token, 0, 10);

                memcpy(token, mixedtoken + 20, 9);

                eaData->latitude1 = atoi(token);

                printf("8token here is:%s\n", token);

                memset(token, 0, 10);

                memcpy(token, mixedtoken + 29, 9);

                eaData->longitude1 = atoi(token);

                printf("9token here is:%s\n", token);

                memset(token, 0, 10);

                memcpy(token, mixedtoken + 38, 7);

                eaData->altitude1 = atoi(token);

                printf("10token here is:%s\n", token);

                cmdError = 0;

                free(token);

            }

            else {

                cmdError = -1;

            }

            break;

            /*Ea12  02  2006  01  55  59  1111  324000000  648000000   1800000
               1   2   3    4    5   6    7         8         9        10  */
        case 1:

            cmdError = 7;

            break;

        default:

            cmdError = -3;

            break;

        }

        mixedtoken = strtok(NULL, "@*\r\n");

        whichtoken++;

    }

    return cmdError;
}


/*************************************************************************
function: picotazParsing                      jupiter pico t
paramenter:
  serialGpsPicoANTD_t *azData : 			the struct to store jupiter pico t response data for Az command.  
  char *string2Parsed : pointer to response data for Ea command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int picotazParsing(char *string2Parsed,
                          serialGpsPicoANTD_t * azData)
{

    char *mixedtoken;

    char *token;

    int whichtoken = 0;

    int cmdError = 0;

    if (string2Parsed[0] == '\0')
        return 1;

    if (markdebug == 1) {

        printf("Splitting string:%s\n", string2Parsed);

    }

    mixedtoken = strtok(string2Parsed, "@\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            if (strncmp(mixedtoken, "@", 1) == 0) {

                cmdError = 0;

                printf
                    ("Yes, this is antenna cable delay of the jupiter pico t command\n");

                strncpy(azData->idhead1, mixedtoken, 1);

            }

            else {

                cmdError = -1;

            }

            break;

        case 1:

            if (strncmp(mixedtoken, "@", 1) == 0) {

                cmdError = 0;

                printf("the second '@' jupiter pico t command\n");

                strncpy(azData->idhead2, mixedtoken, 1);

            }

            else {

                cmdError = -2;

            }

            break;

        case 2:

            token = (char *) malloc(5);

            memset(token, 0, 5);

            memcpy(token, mixedtoken + 2, 4);

            azData->time_offset = atoi(token);

            free(token);

            cmdError = 7;

            break;

        default:

            cmdError = -3;

            break;

        }


        mixedtoken = strtok(NULL, "@\r\n");

        whichtoken++;

    }

    return cmdError;
}


/*************************************************************************
function: picotbbParsing                      jupiter pico t
paramenter:
  serialGpsPicoAZEL_t *bbData : 			the struct to store jupiter pico t response data for Bb command.  
  char *string2Parsed : pointer to response data for Bb command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int picotbbParsing(char *string2Parsed,
                          serialGpsPicoAZEL_t * bbData)
{

    char *mixedtoken;

    char *token;

    int whichtoken = 0;

    int cmdError = 0;

    if (string2Parsed[0] == '\0')
        return 1;

    if (markdebug == 1) {

        printf("Splitting string:%s\n", string2Parsed);

    }

    mixedtoken = strtok(string2Parsed, "@\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            if (strncmp(mixedtoken, "@", 1) == 0) {

                cmdError = 0;

                printf
                    ("Yes, this is azimuth & elevation of the jupiter pico t command\n");

                strncpy(bbData->idhead1, mixedtoken, 1);

            }

            else {

                cmdError = -1;

            }

            break;

        case 1:

            if (strncmp(mixedtoken, "@", 1) == 0) {

                cmdError = 0;

                printf("the second '@' jupiter pico t command\n");

                strncpy(bbData->idhead2, mixedtoken, 1);

            }

            else {

                cmdError = -2;

            }

            break;

        case 2:

            token = (char *) malloc(90);

            memset(token, 0, 90);

            memcpy(token, mixedtoken + 2, 1);

            bbData->nosatellite = atoi(token);

            memcpy(token, mixedtoken + 3, 1);

            bbData->satID1 = atoi(token);

            memcpy(token, mixedtoken + 4, 2);

            bbData->dopHz1 = atoi(token);

            memcpy(token, mixedtoken + 6, 1);

            bbData->ele1 = atoi(token);

            memcpy(token, mixedtoken + 7, 2);

            bbData->azi1 = atoi(token);

            memcpy(token, mixedtoken + 9, 1);

            bbData->sathealth1 = atoi(token);

            memcpy(token, mixedtoken + 10, 1);

            bbData->satID2 = atoi(token);

            memcpy(token, mixedtoken + 11, 2);

            bbData->dopHz2 = atoi(token);

            memcpy(token, mixedtoken + 13, 1);

            bbData->ele2 = atoi(token);

            memcpy(token, mixedtoken + 14, 2);

            bbData->azi2 = atoi(token);

            memcpy(token, mixedtoken + 16, 1);

            bbData->sathealth2 = atoi(token);

            memcpy(token, mixedtoken + 17, 1);

            bbData->satID3 = atoi(token);

            memcpy(token, mixedtoken + 18, 2);

            bbData->dopHz3 = atoi(token);

            memcpy(token, mixedtoken + 20, 1);

            bbData->ele3 = atoi(token);

            memcpy(token, mixedtoken + 21, 2);

            bbData->azi3 = atoi(token);

            memcpy(token, mixedtoken + 23, 1);

            bbData->sathealth3 = atoi(token);

            memcpy(token, mixedtoken + 24, 1);

            bbData->satID4 = atoi(token);

            memcpy(token, mixedtoken + 25, 2);

            bbData->dopHz4 = atoi(token);

            memcpy(token, mixedtoken + 27, 1);

            bbData->ele4 = atoi(token);

            memcpy(token, mixedtoken + 28, 2);

            bbData->azi4 = atoi(token);

            memcpy(token, mixedtoken + 30, 1);

            bbData->sathealth4 = atoi(token);

            memcpy(token, mixedtoken + 31, 1);

            bbData->satID5 = atoi(token);

            memcpy(token, mixedtoken + 32, 2);

            bbData->dopHz5 = atoi(token);

            memcpy(token, mixedtoken + 34, 1);

            bbData->ele5 = atoi(token);

            memcpy(token, mixedtoken + 35, 2);

            bbData->azi5 = atoi(token);

            memcpy(token, mixedtoken + 37, 1);

            bbData->sathealth5 = atoi(token);

            memcpy(token, mixedtoken + 38, 1);

            bbData->satID6 = atoi(token);

            memcpy(token, mixedtoken + 39, 2);

            bbData->dopHz6 = atoi(token);

            memcpy(token, mixedtoken + 41, 1);

            bbData->ele6 = atoi(token);

            memcpy(token, mixedtoken + 42, 2);

            bbData->azi6 = atoi(token);

            memcpy(token, mixedtoken + 44, 1);

            bbData->sathealth6 = atoi(token);

            memcpy(token, mixedtoken + 45, 1);

            bbData->satID7 = atoi(token);

            memcpy(token, mixedtoken + 46, 2);

            bbData->dopHz7 = atoi(token);

            memcpy(token, mixedtoken + 48, 1);

            bbData->ele7 = atoi(token);

            memcpy(token, mixedtoken + 49, 2);

            bbData->azi7 = atoi(token);

            memcpy(token, mixedtoken + 51, 1);

            bbData->sathealth7 = atoi(token);

            memcpy(token, mixedtoken + 52, 1);

            bbData->satID8 = atoi(token);

            memcpy(token, mixedtoken + 53, 2);

            bbData->dopHz8 = atoi(token);

            memcpy(token, mixedtoken + 55, 1);

            bbData->ele8 = atoi(token);

            memcpy(token, mixedtoken + 56, 2);

            bbData->azi8 = atoi(token);

            memcpy(token, mixedtoken + 58, 1);

            bbData->sathealth8 = atoi(token);

            memcpy(token, mixedtoken + 59, 1);

            bbData->satID9 = atoi(token);

            memcpy(token, mixedtoken + 60, 2);

            bbData->dopHz9 = atoi(token);

            memcpy(token, mixedtoken + 62, 1);

            bbData->ele9 = atoi(token);

            memcpy(token, mixedtoken + 63, 2);

            bbData->azi9 = atoi(token);

            memcpy(token, mixedtoken + 65, 1);

            bbData->sathealth9 = atoi(token);

            memcpy(token, mixedtoken + 66, 1);

            bbData->satID10 = atoi(token);

            memcpy(token, mixedtoken + 67, 2);

            bbData->dopHz10 = atoi(token);

            memcpy(token, mixedtoken + 69, 1);

            bbData->ele10 = atoi(token);

            memcpy(token, mixedtoken + 70, 2);

            bbData->azi10 = atoi(token);

            memcpy(token, mixedtoken + 72, 1);

            bbData->sathealth10 = atoi(token);

            memcpy(token, mixedtoken + 73, 1);

            bbData->satID11 = atoi(token);

            memcpy(token, mixedtoken + 74, 2);

            bbData->dopHz11 = atoi(token);

            memcpy(token, mixedtoken + 76, 1);

            bbData->ele11 = atoi(token);

            memcpy(token, mixedtoken + 77, 2);

            bbData->azi11 = atoi(token);

            memcpy(token, mixedtoken + 79, 1);

            bbData->sathealth11 = atoi(token);

            memcpy(token, mixedtoken + 80, 1);

            bbData->satID12 = atoi(token);

            memcpy(token, mixedtoken + 81, 2);

            bbData->dopHz12 = atoi(token);

            memcpy(token, mixedtoken + 83, 1);

            bbData->ele12 = atoi(token);

            memcpy(token, mixedtoken + 84, 2);

            bbData->azi12 = atoi(token);

            memcpy(token, mixedtoken + 86, 1);

            bbData->sathealth12 = atoi(token);

            free(token);

            cmdError = 7;

            break;

        default:

            cmdError = -3;

            break;

        }


        mixedtoken = strtok(NULL, "@\r\n");

        whichtoken++;

    }

    return cmdError;
}


/*************************************************************************
function: gpggaParsing
paramenter:
  serialGpsGpgga_t *gpggaData : 			the struct to store response data for GPGGA command.  
  char *string2Parsed : pointer to response data for GPGGA command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int gpggaParsing(char *string2Parsed, serialGpsGpgga_t * gpggaData){
    char *mixedtoken;
    char *token;
    int whichtoken = 0;
    int cmdError = 0;
    if (string2Parsed[0] == '\0')
        return 1;
    if (markdebug == 1) {
        printf("Splitting string:%s\n", string2Parsed);
    }
    mixedtoken = strtok(string2Parsed, ",\r\n");
    while (mixedtoken != NULL) {
#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);
#endif 
        if (cmdError != 0)
            break;
        switch (whichtoken) {
        case 0:
            if (strncmp(mixedtoken, "$GPGGA", 6) == 0) {
                cmdError = 0;
                if (markdebug == 1) {
                    printf("Yes, this is position altitude command\n");
                }
                strncpy(gpggaData->idhead, mixedtoken, 6);
            }
            else {
                cmdError = -1;
            }
            break;
        case 1:
            token = (char *) malloc(3);
            memset(token, 0, 3);
            memcpy(token, mixedtoken + 0, 2);
            gpggaData->utc.hh = atoi(token);
            memcpy(token, mixedtoken + 2, 2);
            gpggaData->utc.mm = atoi(token);
            memcpy(token, mixedtoken + 4, 2);
            gpggaData->utc.ss = atoi(token);
            free(token);
            break;
        case 2:
            token = (char *) malloc(5);
            memset(token, 0, 5);
            memcpy(token, mixedtoken + 0, 2);
            gpggaData->latitude.degree = atoi(token);
            memcpy(token, mixedtoken + 2, 2);
            gpggaData->latitude.minuate = atoi(token);
            memcpy(token, mixedtoken + 5, 4);
            gpggaData->latitude.fraction = atoi(token);
            free(token);
            if ((gpggaData->latitude.degree > 90)
                || (gpggaData->latitude.degree < 0))
                cmdError = -3;
            if ((gpggaData->latitude.minuate > 59)
                || (gpggaData->latitude.minuate < 0))
                cmdError = -3;
            if ((gpggaData->latitude.fraction > 9999)
                || (gpggaData->latitude.fraction < 0))
                cmdError = -3;
            break;
        case 3:
            memcpy(gpggaData->direction1, mixedtoken, 1);
            switch (*(mixedtoken)) {
            case 'N':
                gpggaData->latitude.direction = GPS_NORTH;
                break;
            case 'S':
                gpggaData->latitude.direction = GPS_SOUTH;
                break;
            default:
                cmdError = -4;
            }
            break;
        case 4:
            token = (char *) malloc(5);
            memset(token, 0, 5);
            memcpy(token, mixedtoken + 0, 3);
            gpggaData->longitude.degree = atoi(token);
            memcpy(token, mixedtoken + 3, 3);
            gpggaData->longitude.minuate = atoi(token);
            memcpy(token, mixedtoken + 6, 4);
            gpggaData->longitude.fraction = atoi(token);
            free(token);
            if ((gpggaData->longitude.degree > 180)
                || (gpggaData->longitude.degree < 0))
                cmdError = -5;
            if ((gpggaData->longitude.minuate > 59)
                || (gpggaData->longitude.minuate < 0))
                cmdError = -5;
            if ((gpggaData->longitude.fraction > 9999)
                || (gpggaData->longitude.fraction < 0))
                cmdError = -5;
            break;
        case 5:
            memcpy(gpggaData->direction2, mixedtoken, 1);
            switch (*(mixedtoken)) {
            case 'E':
                gpggaData->longitude.direction = GPS_EAST;
                break;
            case 'W':
                gpggaData->longitude.direction = GPS_EAST;
                break;
            default:
                cmdError = -6;
            }
            break;
        case 6:
            gpggaData->gpsQuality = atoi(mixedtoken);
            if ((gpggaData->gpsQuality < 0)
                || (gpggaData->gpsQuality > 2))
                cmdError = GPS_FIX_INVALID;
            else if (gpggaData->gpsQuality == 0)
                cmdError = GPS_FIX_INVALID;
            break;
        case 7:
            gpggaData->n_satellite = atoi(mixedtoken);
            if ((gpggaData->n_satellite < 0)
                || (gpggaData->n_satellite > 12))
                cmdError = -8;
            break;
        case 8:
            memcpy(gpggaData->dop, mixedtoken, 5);
            break;
        case 9:
            gpggaData->altitude = atof(mixedtoken);
            if ((gpggaData->altitude < -1000)
                || (gpggaData->altitude > 4000))
                cmdError = -10;
            break;
        case 10:
            memcpy(gpggaData->alt_unit, mixedtoken, 1);
            break;
        case 11:
            gpggaData->geo = atof(mixedtoken);
            if ((gpggaData->geo < -1000) || (gpggaData->geo > 10000))
                cmdError = -12;
            break;
        case 12:
            memcpy(gpggaData->geo_unit, mixedtoken, 1);
            break;
        case 13:
            gpggaData->dgpsDate = atoi(mixedtoken);
            if ((gpggaData->dgpsDate < 0) || (gpggaData->dgpsDate > 99))
                cmdError = -14;
            break;
        case 14:
            gpggaData->dgpsId = atoi(mixedtoken);
            if ((gpggaData->dgpsId < 0) || (gpggaData->dgpsId > 1023))
                cmdError = -15;
            break;
        default:
            cmdError = -16;
            break;
        }
        mixedtoken = strtok(NULL, ",\r\n");
        whichtoken++;
    }
    return cmdError;
}


/*************************************************************************
function: gpzdaParsing
paramenter:
  serialGpsGpzda_t *gpzdaData : 			the struct to store response data for GPZDA command.  
  char *string2Parsed : pointer to response data for GPZDA command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int gpzdaParsing(char *string2Parsed, serialGpsGpzda_t * gpzdaData)
{

    char *mixedtoken;

    char *token;

    int whichtoken = 0;

    int cmdError = 0;

    if (string2Parsed[0] == '\0')
        return 1;

    if (markdebug == 1) {

        printf("Splitting string:%s\n", string2Parsed);

    }

    mixedtoken = strtok(string2Parsed, ",\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            if (strncmp(mixedtoken, "$GPZDA", 6) == 0) {

                cmdError = 0;

                printf("Yes, this is the date/time command\n");

                strncpy(gpzdaData->idhead, mixedtoken, 6);

            }

            else {

                cmdError = -1;

            }

            break;

        case 1:

            token = (char *) malloc(3);

            memset(token, 0, 3);

            memcpy(token, mixedtoken + 0, 2);

            gpzdaData->utc.hh = atoi(token);

            memcpy(token, mixedtoken + 2, 2);

            gpzdaData->utc.mm = atoi(token);

            memcpy(token, mixedtoken + 4, 2);

            gpzdaData->utc.ss = atoi(token);

            free(token);

            break;

        case 2:

            token = (char *) malloc(3);

            memset(token, 0, 3);

            memcpy(token, mixedtoken + 0, 2);

            gpzdaData->utc.dy = atoi(token);

            free(token);

            break;

        case 3:

            token = (char *) malloc(3);

            memset(token, 0, 3);

            memcpy(token, mixedtoken + 0, 2);

            gpzdaData->utc.mn = atoi(token);

            free(token);

            break;

        case 4:

            token = (char *) malloc(5);

            memset(token, 0, 5);

            memcpy(token, mixedtoken + 0, 5);

            gpzdaData->utc.yy = atoi(token);

            free(token);

            break;

        case 5:

            token = (char *) malloc(3);

            memset(token, 0, 3);

            memcpy(token, mixedtoken + 0, 3);

            gpzdaData->zonehr = atoi(token);

            free(token);

            break;

        case 6:

            token = (char *) malloc(3);

            memset(token, 0, 3);

            memcpy(token, mixedtoken + 0, 3);

            gpzdaData->zonemm = atoi(token);

            free(token);

            cmdError = 7;

            break;

        default:

            cmdError = -8;

            break;

        }


        mixedtoken = strtok(NULL, ",\r\n");

        whichtoken++;

    }

    return cmdError;
}


/*************************************************************************
function: gpgsvParsing
paramenter:
  serialGpsGpgsv_t *gpgsvData : 			the struct to store response data for GPGSV command.  
  char *string2Parsed : pointer to response data for GPGSV command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int gpgsvParsing(char *string2Parsed, serialGpsGpgsv_t * gpgsvData){
    char *mixedtoken;
    int whichtoken = 0;
    int cmdError = 0;
    if (string2Parsed[0] == '\0')
        return 1;
    if (markdebug == 1) {
        printf("Splitting string:%s\n", string2Parsed);
    }
    mixedtoken = strtok(string2Parsed, ",\r\n");
    while (mixedtoken != NULL) {
#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);
#endif 
        if (cmdError != 0)
            break;
        switch (whichtoken) {
        case 0:
            if (strncmp(mixedtoken, "$GPGSV", 6) == 0) {
                cmdError = 0;
                if (markdebug == 1) {
                    printf
                        ("Yes, this is satellite details of the GT8031 or jupiter 30 command\n");
                }
                strncpy(gpgsvData->idhead, mixedtoken, 6);
            }
            else {
                cmdError = -1;
            }
            break;
        case 1:
            gpgsvData->totalnomssg = atoi(mixedtoken);
            if ((gpgsvData->totalnomssg < 1)
                || (gpgsvData->totalnomssg > 3))
                cmdError = -2;
            break;
        case 2:
            gpgsvData->nomssg = atoi(mixedtoken);
            if ((gpgsvData->nomssg < 1)
                || (gpgsvData->nomssg > 3))
                cmdError = -3;
            break;
        case 3:
            gpgsvData->nosatellite = atoi(mixedtoken);
            if ((gpgsvData->nosatellite < 0)
                || (gpgsvData->nosatellite > 12))
                cmdError = -4;
            break;
        case 4:
            gpgsvData->ssv1 = atoi(mixedtoken);
            if ((gpgsvData->ssv1 < 01)
                || (gpgsvData->ssv1 > 32))
                cmdError = -5;
            break;
        case 5:
            gpgsvData->sea1 = atoi(mixedtoken);
            if ((gpgsvData->sea1 < 05)
                || (gpgsvData->sea1 > 90))
                cmdError = -6;
            break;
        case 6:
            gpgsvData->sba1 = atoi(mixedtoken);
            if ((gpgsvData->sba1 < 0) || (gpgsvData->sba1 > 359))
                cmdError = -7;
            break;
        case 7:
            gpgsvData->ssnr1 = atoi(mixedtoken);
            if ((gpgsvData->ssnr1 < 0)
                || (gpgsvData->ssnr1 > 99))
                cmdError = -8;
            else if (gpgsvData->ssnr1 > signallimit) {
                cmdError = SIGNAL_OVER_1ST;
            }
            else {
                cmdError = SIGNAL_LESS_1ST;
            }
            break;
        case 8:
           gpgsvData->ssv2 = atoi(mixedtoken);
            if ((gpgsvData->ssv2 < 01)
                || (gpgsvData->ssv2 > 32))
                cmdError = -9;
            break;
        case 9:
            gpgsvData->sea2 = atoi(mixedtoken);
            if ((gpgsvData->sea2 < 05)
                || (gpgsvData->sea2 > 90))
                cmdError = -10;
            break;
        case 10:
            gpgsvData->sba2 = atoi(mixedtoken);
            if ((gpgsvData->sba2 < 0) || (gpgsvData->sba2 > 359))
                cmdError = -11;
            break;
        case 11:
            gpgsvData->ssnr2 = atoi(mixedtoken);
            if ((gpgsvData->ssnr2 < 0)
                || (gpgsvData->ssnr2 > 99))
                cmdError = -12;
            else if (gpgsvData->ssnr2 > signallimit) {
                cmdError = SIGNAL_OVER_2ND;
            }
            else {
                cmdError = SIGNAL_LESS_2ND;
            }
            break;
        case 12:
            gpgsvData->ssv3 = atoi(mixedtoken);
            if ((gpgsvData->ssv3 < 01)
                || (gpgsvData->ssv3 > 32))
                cmdError = -13;
            break;
        case 13:
            gpgsvData->sea3 = atoi(mixedtoken);
            if ((gpgsvData->sea3 < 05)
                || (gpgsvData->sea3 > 90))
                cmdError = -14;
            break;
        case 14:
            gpgsvData->sba3 = atoi(mixedtoken);
            if ((gpgsvData->sba3 < 0) || (gpgsvData->sba3 > 359))
                cmdError = -15;
            break;
        case 15:
            gpgsvData->ssnr3 = atoi(mixedtoken);
            if ((gpgsvData->ssnr3 < 0)
                || (gpgsvData->ssnr3 > 99))
                cmdError = -16;
            else if (gpgsvData->ssnr3 > signallimit) {
                cmdError = SIGNAL_OVER_3RD;
            }
            else {
                cmdError = SIGNAL_LESS_3RD;
            }
            break;
        case 16:
            gpgsvData->ssv4 = atoi(mixedtoken);
            if ((gpgsvData->ssv4 < 01)
                || (gpgsvData->ssv4 > 32))
                cmdError = -17;
            break;
        case 17:
            gpgsvData->sea4 = atoi(mixedtoken);
            if ((gpgsvData->sea4 < 05)
                || (gpgsvData->sea4 > 90))
                cmdError = -18;
            break;
        case 18:
            gpgsvData->sba4 = atoi(mixedtoken);
            if ((gpgsvData->sba4 < 0) || (gpgsvData->sba4 > 359))
                cmdError = -19;
            break;
        case 19:
            gpgsvData->ssnr4 = atoi(mixedtoken);
            if ((gpgsvData->ssnr4 < 0)
                || (gpgsvData->ssnr4 > 99))
                cmdError = -20;
            else if (gpgsvData->ssnr4 > signallimit) {
                cmdError = SIGNAL_OVER_4TH;
            }
            else {
                cmdError = SIGNAL_LESS_4TH;
            }
            break;
        default:
            cmdError = -21;
            break;
        }
        mixedtoken = strtok(NULL, ",\r\n");
        whichtoken++;
    }
    return cmdError;
}


/*************************************************************************
function: gpgga30Parsing           jupiter 30
paramenter:
  serialGpsGpgga_t *gpggaData : 			the struct to store response data for GPGGA command.  
  char *string2Parsed : pointer to response data for GPGGA command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int gpgga30Parsing(char *string2Parsed,
                          serialGpsGpgga_t * gpggaData)
{

    char *mixedtoken;

    char *token;

    int whichtoken = 0;

    int cmdError = 0;

    if (string2Parsed[0] == '\0')
        return 1;

    if (markdebug == 1) {

        printf("Splitting string:%s\n", string2Parsed);

    }

    mixedtoken = strtok(string2Parsed, ",\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            if (strncmp(mixedtoken, "$GPGGA", 6) == 0) {

                cmdError = 0;

                printf
                    ("Yes, this is position altitude command of the jupiter 30\n");

                strncpy(gpggaData->idhead, mixedtoken, 6);

            }

            else {

                cmdError = -1;

            }

            break;

        case 1:

            token = (char *) malloc(3);

            memset(token, 0, 3);

            memcpy(token, mixedtoken + 0, 2);

            gpggaData->utc.hh = atoi(token);

            memcpy(token, mixedtoken + 2, 2);

            gpggaData->utc.mm = atoi(token);

            memcpy(token, mixedtoken + 4, 6);

            gpggaData->utc.ss = atoi(token);

            free(token);

            break;

        case 2:

            token = (char *) malloc(5);

            memset(token, 0, 5);

            memcpy(token, mixedtoken + 0, 2);

            gpggaData->latitude.degree = atoi(token);

            memcpy(token, mixedtoken + 2, 2);

            gpggaData->latitude.minuate = atoi(token);

            memcpy(token, mixedtoken + 5, 4);

            gpggaData->latitude.fraction = atoi(token);

            free(token);

            if ((gpggaData->latitude.degree > 90)
                || (gpggaData->latitude.degree < 0))

                cmdError = -3;

            if ((gpggaData->latitude.minuate > 59)
                || (gpggaData->latitude.minuate < 0))

                cmdError = -3;

            if ((gpggaData->latitude.fraction > 9999)
                || (gpggaData->latitude.fraction < 0))

                cmdError = -3;

            break;

        case 3:

            memcpy(gpggaData->direction1, mixedtoken, 1);

            switch (*(mixedtoken)) {

            case 'N':

                gpggaData->latitude.direction = GPS_NORTH;

                break;

            case 'S':

                gpggaData->latitude.direction = GPS_SOUTH;

                break;

            default:

                cmdError = -4;

            }

            break;

        case 4:

            token = (char *) malloc(5);

            memset(token, 0, 5);

            memcpy(token, mixedtoken + 0, 3);

            gpggaData->longitude.degree = atoi(token);

            memcpy(token, mixedtoken + 3, 3);

            gpggaData->longitude.minuate = atoi(token);

            memcpy(token, mixedtoken + 6, 4);

            gpggaData->longitude.fraction = atoi(token);

            free(token);

            if ((gpggaData->longitude.degree > 180)
                || (gpggaData->longitude.degree < 0))

                cmdError = -5;

            if ((gpggaData->longitude.minuate > 59)
                || (gpggaData->longitude.minuate < 0))

                cmdError = -5;

            if ((gpggaData->longitude.fraction > 9999)
                || (gpggaData->longitude.fraction < 0))

                cmdError = -5;

            break;

        case 5:

            memcpy(gpggaData->direction2, mixedtoken, 1);

            switch (*(mixedtoken)) {

            case 'E':

                gpggaData->longitude.direction = GPS_EAST;

                break;

            case 'W':

                gpggaData->longitude.direction = GPS_EAST;

                break;

            default:

                cmdError = -6;

            }

            break;

        case 6:

            gpggaData->gpsQuality = atoi(mixedtoken);

            if ((gpggaData->gpsQuality < 0)
                || (gpggaData->gpsQuality > 2))

                cmdError = 7;

            else if (gpggaData->gpsQuality == 0)

                cmdError = 7;

            break;

        case 7:

            gpggaData->n_satellite = atoi(mixedtoken);

            if ((gpggaData->n_satellite < 0)
                || (gpggaData->n_satellite > 12))

                cmdError = -8;

            break;

        case 8:

            memcpy(gpggaData->dop, mixedtoken, 5);

            break;

        case 9:

            gpggaData->altitude = atof(mixedtoken);

            if ((gpggaData->altitude < -1000)
                || (gpggaData->altitude > 4000))

                cmdError = -10;

            break;

        case 10:

            memcpy(gpggaData->alt_unit, mixedtoken, 1);

            break;

        case 11:

            gpggaData->geo = atof(mixedtoken);

            if ((gpggaData->geo < -1000) || (gpggaData->geo > 10000))

                cmdError = -12;

            break;

        case 12:

            memcpy(gpggaData->geo_unit, mixedtoken, 1);

            break;

        case 13:

            gpggaData->dgpsDate = atoi(mixedtoken);

            if ((gpggaData->dgpsDate < 0) || (gpggaData->dgpsDate > 99))

                cmdError = -14;

            break;

        case 14:

            gpggaData->dgpsId = atoi(mixedtoken);

            if ((gpggaData->dgpsId < 0) || (gpggaData->dgpsId > 1023))

                cmdError = -15;

            break;

        default:

            cmdError = -16;

            break;

        }

        mixedtoken = strtok(NULL, ",\r\n");

        whichtoken++;

    }

    return cmdError;
}


/*************************************************************************
function: gpgll30Parsing               jupiter 30
paramenter:
  serialGpsGpgll_t *gpgllData : 			the struct to store response data for GPGLL command.  
  char *string2Parsed : pointer to response data for GPGLL command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int gpgll30Parsing(char *string2Parsed,
                          serialGpsGpgll_t * gpgllData)
{

    char *mixedtoken;

    char *token;

    int whichtoken = 0;

    int cmdError = 0;

    if (string2Parsed[0] == '\0')
        return 1;

    if (markdebug == 1) {

        printf("Splitting string:%s\n", string2Parsed);

    }

    mixedtoken = strtok(string2Parsed, ",\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            if (strncmp(mixedtoken, "$GPGLL", 6) == 0) {

                cmdError = 0;

                printf
                    ("Yes, this is geographic position status command\n");

                strncpy(gpgllData->idhead, mixedtoken, 6);

            }

            else {

                cmdError = -1;

            }

            break;

        case 1:

            token = (char *) malloc(5);

            memset(token, 0, 5);

            memcpy(token, mixedtoken + 0, 2);

            gpgllData->latitude.degree = atoi(token);

            memcpy(token, mixedtoken + 2, 2);

            gpgllData->latitude.minuate = atoi(token);

            memcpy(token, mixedtoken + 5, 4);

            gpgllData->latitude.fraction = atoi(token);

            free(token);

            if ((gpgllData->latitude.degree > 90)
                || (gpgllData->latitude.degree < 0))

                cmdError = -3;

            if ((gpgllData->latitude.minuate > 59)
                || (gpgllData->latitude.minuate < 0))

                cmdError = -3;

            if ((gpgllData->latitude.fraction > 9999)
                || (gpgllData->latitude.fraction < 0))

                cmdError = -3;

            break;

        case 2:

            memcpy(gpgllData->direction1, mixedtoken, 1);

            switch (*(mixedtoken)) {

            case 'N':

                gpgllData->latitude.direction = GPS_NORTH;

                break;

            case 'S':

                gpgllData->latitude.direction = GPS_SOUTH;

                break;

            default:

                cmdError = -4;

            }

            break;

        case 3:

            token = (char *) malloc(5);

            memset(token, 0, 5);

            memcpy(token, mixedtoken + 0, 3);

            gpgllData->longitude.degree = atoi(token);

            memcpy(token, mixedtoken + 3, 3);

            gpgllData->longitude.minuate = atoi(token);

            memcpy(token, mixedtoken + 6, 4);

            gpgllData->longitude.fraction = atoi(token);

            free(token);

            if ((gpgllData->longitude.degree > 180)
                || (gpgllData->longitude.degree < 0))

                cmdError = -5;

            if ((gpgllData->longitude.minuate > 59)
                || (gpgllData->longitude.minuate < 0))

                cmdError = -5;

            if ((gpgllData->longitude.fraction > 9999)
                || (gpgllData->longitude.fraction < 0))

                cmdError = -5;

            break;

        case 4:

            memcpy(gpgllData->direction2, mixedtoken, 1);

            switch (*(mixedtoken)) {

            case 'E':

                gpgllData->longitude.direction = GPS_EAST;

                break;

            case 'W':

                gpgllData->longitude.direction = GPS_EAST;

                break;

            default:

                cmdError = -6;

            }

            break;

        case 5:

            token = (char *) malloc(3);

            memset(token, 0, 3);

            memcpy(token, mixedtoken + 0, 2);

            gpgllData->utc.hh = atoi(token);

            memcpy(token, mixedtoken + 2, 2);

            gpgllData->utc.mm = atoi(token);

            memcpy(token, mixedtoken + 4, 2);

            gpgllData->utc.ss = atoi(token);

            free(token);

            break;

        case 6:

            memcpy(gpgllData->status, mixedtoken, 1);

            break;

        default:

            cmdError = -16;

            break;

        }


        mixedtoken = strtok(NULL, ",\r\n");

        whichtoken++;

    }

    return cmdError;
}


/*************************************************************************
function: gpmss30Parsing        jupiter 30
paramenter:
  serialGpsGpmss_t *gpmssData : 			the struct to store response data for GPMSS command.  
  char *string2Parsed : pointer to response data for GPMSS command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int gpmss30Parsing(char *string2Parsed,
                          serialGpsGpmss_t * gpmssData)
{

    char *mixedtoken;

    int whichtoken = 0;

    int cmdError = 0;

    if (string2Parsed[0] == '\0')
        return 1;

    if (markdebug == 1) {

        printf("Splitting string:%s\n", string2Parsed);

    }

    mixedtoken = strtok(string2Parsed, ",\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            if (strncmp(mixedtoken, "$GPMSS", 6) == 0) {

                cmdError = 0;

                printf("Yes, this is MSK receiver signal command\n");

                strncpy(gpmssData->idhead, mixedtoken, 6);

            }

            else {

                cmdError = -1;

            }

            break;

        case 1:

            gpmssData->sig_strength = atoi(mixedtoken);

            break;

        case 2:

            gpmssData->snr = atoi(mixedtoken);

            break;

        case 3:

            gpmssData->be_freq = atoi(mixedtoken);

            break;

        case 4:

            gpmssData->be_bitrate = atoi(mixedtoken);

            break;

        case 5:

            gpmssData->channer_no = atoi(mixedtoken);

            break;

        default:

            cmdError = -16;

            break;

        }

        mixedtoken = strtok(NULL, ",\r\n");

        whichtoken++;

    }

    return cmdError;
}


/*************************************************************************
function: xxggaParsing
paramenter:
  serialGpsXXGGA_t *xxggaData : the struct to store response data for XXGGA command.  
  char *string2Parsed : pointer to response data for XXGGA command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int xxggaParsing(char *string2Parsed, serialGpsXXGGA_t * xxggaData)
{

    char *mixedtoken;

    int whichtoken = 0;

    int cmdError = 0;

    if (string2Parsed[0] == '\0')
        return 1;

    printf("Splitting string :%s\n", string2Parsed);

    mixedtoken = strtok(string2Parsed, ":,\r\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            if (strncmp(mixedtoken, "latitude", 8) == 0) {

                cmdError = 0;

                printf
                    ("Yes, this is the setting initial position command\n");

            }

            else {

                cmdError = -1;

            }

            break;

        case 1:

            xxggaData->lat = atof(mixedtoken);

            break;

/* "latitude:4888.0770,N,longtitude:12345.4700,E" */
        case 2:

            switch (*(mixedtoken)) {

            case 'N':

                memcpy(xxggaData->directNS, "N", 1);

                break;

            case 'S':

                memcpy(xxggaData->directNS, "S", 1);

                break;

            default:

                cmdError = -3;

            }

            break;

        case 3:

            if (strncmp(mixedtoken, "longtitude", 10) == 0) {

                cmdError = 0;

            }

            else {

                cmdError = -4;

            }

            break;

        case 4:

            xxggaData->longitudef = atof(mixedtoken);

            break;

        case 5:

            switch (*(mixedtoken)) {

            case 'E':

                memcpy(xxggaData->directEW, "E", 1);

                cmdError = 7;

                break;

            case 'W':

                memcpy(xxggaData->directEW, "W", 1);

                cmdError = 7;

                break;

            default:

                cmdError = -5;

            }

            break;

        default:

            cmdError = -7;

            break;

        }

        mixedtoken = strtok(NULL, ":,\r\n");

        whichtoken++;

    }

    return cmdError;
}


/*************************************************************************
function: gpclrParsing
paramenter:
  serialGpsGpclr_t *gpclrData : the struct to store data for gpclr command.  
  char *string2Parsed : pointer to gpclr command.													
return:
	0: ok
	other: error code [to be continued]

int gpclrParsing(int string2Parsed, serialGpsGpclr_t * gpclrData)
{
    int cmdError = 0;
   
    if (string2Parsed > 4 || string2Parsed <0) return 1;
    printf("Splitting string \"%d\" in tokens:\n", string2Parsed);
    gpclrData->mode = string2Parsed;

    return cmdError;
}*/
static int CDMAtimeParsing(char *string2Parsed,
                           serialCDMAtime_t * CDMAtimeData)
{

    char *mixedtoken;

    char *token;

    int whichtoken = 0;

    int cmdError = 0;

    if (string2Parsed[0] == '\0')
        return 1;

    if (markdebug == 1) {

        printf("Splitting string \"%s\" in tokens:\n", string2Parsed);

    }

    mixedtoken = strtok(string2Parsed, " \r\n");

    printf("Yes, this is Clock Emulation Modes command\n");

    while (mixedtoken != NULL) {

#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);

#endif                          /* 
                                 */
        if (cmdError != 0)

            break;

        switch (whichtoken) {

        case 0:

            CDMAtimeData->timeFig = atoi(mixedtoken);

            if ((CDMAtimeData->timeFig < 6)
                || (CDMAtimeData->timeFig > 9))

                cmdError = -1;

            break;

        case 1:

            CDMAtimeData->year = atoi(mixedtoken);

            break;

        case 2:

            CDMAtimeData->day = atoi(mixedtoken);

            if ((CDMAtimeData->day < 0) || (CDMAtimeData->day > 366))

                cmdError = -3;

            break;

        case 3:

            token = (char *) malloc(3);

            memset(token, 0, 3);

            memcpy(token, mixedtoken + 0, 2);

            CDMAtimeData->utc.hh = atoi(token);

            memcpy(token, mixedtoken + 2, 2);

            CDMAtimeData->utc.mm = atoi(token);

            memcpy(token, mixedtoken + 4, 2);

            CDMAtimeData->utc.ss = atoi(token);

            free(token);

            if ((CDMAtimeData->utc.hh > 24)
                || (CDMAtimeData->utc.hh < 0))

                cmdError = -4;

            if ((CDMAtimeData->utc.mm > 60)
                || (CDMAtimeData->utc.mm < 0))

                cmdError = -4;

            if ((CDMAtimeData->utc.ss > 60)
                || (CDMAtimeData->utc.ss < 0))

                cmdError = -4;

            break;

        case 4:

            token = (char *) malloc(3);

            memset(token, 0, 3);

            memcpy(&CDMAtimeData->sign, mixedtoken + 0, 1);

            memcpy(token, mixedtoken + 1, 2);

            CDMAtimeData->offset = atoi(token);

            free(token);

            break;

        case 5:

            memcpy(&CDMAtimeData->tmode, mixedtoken, 1);

            if ((CDMAtimeData->tmode == 'G')
                || (CDMAtimeData->tmode == 'L')
                || (CDMAtimeData->tmode == 'U'));

            else

                cmdError = -6;

            break;

        default:

            cmdError = -7;

            break;

        }


        mixedtoken = strtok(NULL, " \r\n");

        whichtoken++;

    }

    return cmdError;
}


/*************************************************************************
function: 
  gptstParsing
paramenter:
  serialGpsGptst_t *gptstData : 	the struct to store response data for tst(self-test) command.  
  char *string2Parsed : pointer to response string for tst command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int gptstParsing(char *string2Parsed, serialGpsGptst_t * gptstData)
{
    char *mixedtoken;
    char *token;
    int whichtoken = 0;
    int cmdError = 0;
    if (string2Parsed[0] == '\0')
        return 1;
    if (markdebug == 1) {
        printf("Splitting string \"%s\" in tokens:\n", string2Parsed);
    }
    mixedtoken = strtok(string2Parsed, ",\r\n");
    while (mixedtoken != NULL) {
#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);
#endif
        if (cmdError != 0)
            break;
        switch (whichtoken) {
        case 0:
            if (strncmp(mixedtoken, "$PFEC", 5) == 0) {
                if (markdebug == 1) {
                    printf("Yes, this is seft-test results command\n");
                }
                strncpy(gptstData->idhead, mixedtoken, 6);
                cmdError = 0;
            } else {
                cmdError = -1;
            }
            break;
        case 1:
            memcpy(gptstData->comname, mixedtoken, 5);
            break;
        case 2:
            gptstData->status = atoi(mixedtoken);
            if ((gptstData->status < 0) || (gptstData->status > 1))
                cmdError = -3;
            break;
        case 3:
            token = (char *) malloc(5);
            memset(token, 0, 5);
            memcpy(token, mixedtoken + 0, 8);
            gptstData->pvnum.prog_n = atoi(token);
            memcpy(token, mixedtoken + 9, 2);
            gptstData->pvnum.ver_n = atoi(token);
            free(token);
            break;
        case 4:
            gptstData->st_result1 = atoi(mixedtoken);
            if ((gptstData->st_result1 < 0)
                || (gptstData->st_result1 > 1))
                cmdError = -5;
            break;
        case 5:
            gptstData->st_result2 = atoi(mixedtoken);
            if ((gptstData->st_result2 < 0)
                || (gptstData->st_result2 > 16))
                cmdError = -6;
            break;
        default:
            cmdError = -7;
            break;
        }
        mixedtoken = strtok(NULL, ",\r\n");
        whichtoken++;
    }
    return cmdError;
}


/*************************************************************************
function: 
  gpwavParsing
paramenter:
  serialGpsGpwav_t *gpwavData : 	the struct to store response data for SBAS satellite information command.  
  char *string2Parsed : pointer to response string for tst command.													
return:
	0: ok
	other: error code [to be continued]
*/
static int gpwavParsing(char *string2Parsed, serialGpsGpwav_t * gpwavData)
{
    char *mixedtoken;
    int whichtoken = 0;
    int cmdError = 0;
    if (string2Parsed[0] == '\0')
        return 1;
    if (markdebug == 1) {
        printf("Splitting string \"%s\" in tokens:\n", string2Parsed);
    }
    mixedtoken = strtok(string2Parsed, ",\r\n");
    while (mixedtoken != NULL) {
#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);
#endif
        if (cmdError != 0)
            break;
        switch (whichtoken) {
        case 0:
            if (strncmp(mixedtoken, "$PFEC", 5) == 0) {
                if (markdebug == 1) {
                    printf
                        ("Yes, this is the SBAS satellit information command\n");
                }
            } else {
                cmdError = -1;
            }
            break;
        case 1:
            memcpy(gpwavData->comname, mixedtoken, 5);
            break;
        case 2:
            gpwavData->GEO_num = atoi(mixedtoken);
            break;
        case 3:
            gpwavData->GEO_1 = atoi(mixedtoken);
            break;
        case 4:
            gpwavData->Ele_1 = atoi(mixedtoken);
            if ((gpwavData->Ele_1 < 4) || (gpwavData->Ele_1 > 91))
                cmdError = -2;
            break;
        case 5:
            gpwavData->Azi_1 = atoi(mixedtoken);
            if ((gpwavData->Azi_1 < 0) || (gpwavData->Azi_1 > 360))
                cmdError = -3;
            break;
        case 6:
            gpwavData->SNR_1 = atoi(mixedtoken);
            if ((gpwavData->SNR_1 < 0) || (gpwavData->SNR_1 > 100))
                cmdError = -4;
            break;
        case 7:
            gpwavData->GEO_2 = atoi(mixedtoken);
            break;
        case 8:
            gpwavData->Ele_2 = atoi(mixedtoken);
            if ((gpwavData->Ele_2 < 4) || (gpwavData->Ele_2 > 91))
                cmdError = -5;
            break;
        case 9:
            gpwavData->Azi_2 = atoi(mixedtoken);
            if ((gpwavData->Azi_2 < 0) || (gpwavData->Azi_2 > 360))
                cmdError = -6;
            break;
        case 10:
            gpwavData->SNR_2 = atoi(mixedtoken);
            if ((gpwavData->SNR_2 < 0) || (gpwavData->SNR_2 > 100))
                cmdError = -7;
            break;
        case 11:
            gpwavData->GEO_3 = atoi(mixedtoken);
            break;
        case 12:
            gpwavData->Ele_3 = atoi(mixedtoken);
            if ((gpwavData->Ele_3 < 4) || (gpwavData->Ele_3 > 91))
                cmdError = -8;
            break;
        case 13:
            gpwavData->Azi_3 = atoi(mixedtoken);
            if ((gpwavData->Azi_3 < 0) || (gpwavData->Azi_3 > 360))
                cmdError = -9;
            break;
        case 14:
            gpwavData->SNR_3 = atoi(mixedtoken);
            if ((gpwavData->SNR_3 < 0) || (gpwavData->SNR_3 > 100))
                cmdError = -10;
            break;
        case 15:
            gpwavData->GEO_4 = atoi(mixedtoken);
            break;
        case 16:
            gpwavData->Ele_4 = atoi(mixedtoken);
            if ((gpwavData->Ele_4 < 4) || (gpwavData->Ele_4 > 91))
                cmdError = -11;
            break;
        case 17:
            gpwavData->Azi_4 = atoi(mixedtoken);
            if ((gpwavData->Azi_4 < 0) || (gpwavData->Azi_4 > 360))
                cmdError = -12;
            break;
        case 18:
            gpwavData->SNR_4 = atoi(mixedtoken);
            if ((gpwavData->SNR_4 < 0) || (gpwavData->SNR_4 > 100))
                cmdError = -13;
            break;
        default:
            cmdError = -14;
            break;
        }
        mixedtoken = strtok(NULL, ",\r\n");
        whichtoken++;
    }
    return cmdError;
}


/********************************************************************************************
function: 
  gpsSelfTest - GPS self-testing
paramenter:
  serialGpsGptst_t *gptstData : 	the struct to store response data for tst(self-test) command.  
  int port :                        the serial port number (1 for scc1, and 3 for scc3)            
  												
return:
	0  - self-testing successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
	other: error code [to be continued]
*/
static int gpsSelfTestResult(serialGpsGptst_t * gptstData, int port)
{
    int len;
    int idx;
    DEV_TYPE dev = gpsDev;
    char rxBuf[31];
    char *cmdString = "$PFEC,GPint,tst00\r\n";
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-2);
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
#if 0
    printf("rxBuf[]=%s\n", rxBuf);
#endif
    return (uartApp[idx].rxParseFunc(rxBuf, gptstData));        /* response data pasing */
}


/********************************************************************************************
function: 
  gpsSatelliteInfo - GPS satellite information
paramenter:
  serialGpsGpwav_t *gpwavData : 	the struct to store response data for satellite information command.  
  int port :                        the serial port number (1 for scc1, and 3 for scc3)            
  												
return:
	0  - satellite information successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
	other: error code [to be continued]
*/
static int gpsSatelliteInfo(serialGpsGpwav_t * gpwavData, int port)
{
    int len;
    int idx;
    DEV_TYPE dev = gpsDev;
    char rxBuf[73];
    char *cmdString = "$PFEC,GPint,wav00\r\n";
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-2);
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
#if 0
    printf("rxBuf[]=%s\n", rxBuf);
#endif
    return (uartApp[idx].rxParseFunc(rxBuf, gpwavData));        /* response data pasing */
}


static int gpsConductSelfTest(int port)
{
    int len;
#if 0                           /* not to compiler */
    int i = 0;
#endif
    int idx;
    DEV_TYPE dev = gpsDev;
    char rxBuf[31];
    char *cmdString = "$PFEC,GPtrq,0\r\n";
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-1);
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
#if 0
    while ((rxBuf[i] != '\0') || (i == 31))
        logMsg("rxBuf[%d]=%c\r\n", i, rxBuf[i++], 0, 0, 0, 0);
#endif
    return 0;
}


/********************************************************************************************
function: 
  gpsSignalStrength - GPS signal strength
paramenter:
  int port :                        the serial port number (1 for scc1, and 3 for scc3)     
  int range:                       the range of it (01~99)       
  												
return:
	0  - signal strength work successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
	other: error code [to be continued]
*/
static int gpsSignalStrength(int port, int range)
{
    int len;
    int idx;
    DEV_TYPE dev = gpsDev;
    char *cmdString;
    char *cmdHead = "$PFEC,GPset,S";
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-1);
    if ((range < 1) || (range > 99));
    else
        return (-2);
    idxCalculate(cmdHead, len, &idx, dev);
    sprintf(cmdString, "$PFEC,GPset,S%d\r\n", range);
    len = strlen(cmdString);
    uartCmdAct(cmdString, len, NULL, 0, port);
    return 0;
}


/********************************************************************************************
function: 
  gpsCableDelay - GPS antenna cable length delay
paramenter:
  int port :                        the serial port number (1 for scc1, and 3 for scc3)     
  int range:                       the range of it (-999999~+999999)       
  												
return:
	0  - cable delay work successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
	other: error code [to be continued]
*/
static int gpsCableDelay(int port, int range)
{
    int len;
    char cmdString[50] = "";
    /*char *cmdHead="$PFEC,GPset,t"; */
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-1);
    if ((range < -999999) || (range > +999999));
    else
        return (-2);
    sprintf(cmdString, "$PFEC,GPset,t%d\r\n", range);
    if (markdebug == 1) {
        printf("the cable Delay cmdString:%s\n", cmdString);
    }
    len = strlen(cmdString);
    uartCmdAct(cmdString, len, NULL, 0, port);
    return 0;
}


/********************************************************************************************
function: 
  gpsRePara - GPS antenna cable length delay
paramenter:
  int port :                        the serial port number (1 for scc1, and 3 for scc3)           
  												
return:
	0  - get receiver parameters work successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
	other: error code [to be continued]

int gpsRePara(int port)
{ 
	  int len;
	  char rxBuf[80];
  	  char *cmdString="$PFEC,,GPsrq\r\n";
  	  if ((port == 1) || (port == 3)) ; 
  	  else return(-1);

  	  printf("read the cable delay parameter:%s\n", cmdString);
	  len=strlen(cmdString);
	 
	 
	  uartCmdAct(cmdString, len, rxBuf, 1, port);
	  
	  printf("**********************************\n");
	  printf("the receive buf is:%s\n", rxBuf);
	  printf("**********************************\n");	 
	  return 0;  
}
*/


/*********************************************************************************************
function: 
  close_gps_allOutputSentence
  - close OutputSentence of GGA, ZDA, GLL, GSA, GSV, VTG, RMC, tps, tlp, and tst 
    after scc driver works.
     
paramenter:
  void            
  												
return:
	N/A
*/
void close_gps_allOutputSentence(int port)
{
    char *cmdString =
        "$PFEC,GPint,wav00,tst00,ZDA00,GLL00,GSA00,GSV00,VTG00,RMC00,tps00,tlp00,GGA00\r\n\r\n";
    int len;
    char buf[100];
    len = strlen(cmdString);
    if (port == SCC1PORT || port == SCC3PORT)
        uartCmdAct(cmdString, len, buf, 1, port);       /* for scc3 */
    else if (port == 1)
        uartCmdAct(cmdString, len, buf, 1, port);       /* for scc1 */
    taskDelay(300);
}


/*********************************************************************************************
function:                    jupiter 30
  close_gps30_allOutputSentence
  - close OutputSentence of GGA, GLL, GSA, GSV, RMC, VTG, MSS, and ZDA 
    after scc driver works.
     
paramenter:
  void            
  												
return:
	N/A
*/
void close_gps30_allOutputSentence(int port)
{
    char string[20];
    char buf[100];
    int len1;
    strcpy(string, "$PSRF103,00,00,00,01*24\r\n");
    len1 = strlen(string);
    uartCmdAct(string, len1, buf, 1, port);
    strcpy(string, "$PSRF103,01,00,00,01*25\r\n");
    uartCmdAct(string, len1, buf, 1, port);
    strcpy(string, "$PSRF103,02,00,00,01*26\r\n");
    uartCmdAct(string, len1, buf, 1, port);
    strcpy(string, "$PSRF103,03,00,00,01*27\r\n");
    uartCmdAct(string, len1, buf, 1, port);
    strcpy(string, "$PSRF103,08,00,00,01*2c\r\n");
    uartCmdAct(string, len1, buf, 1, port);
    strcpy(string, "$PSRF103,04,00,00,01*20\r\n");
    uartCmdAct(string, len1, buf, 1, port);
    strcpy(string, "$PSRF103,05,00,00,01*21\r\n");
    uartCmdAct(string, len1, buf, 1, port);
    strcpy(string, "$PSRF103,06,00,00,01*22\r\n");
    uartCmdAct(string, len1, buf, 1, port);
    taskDelay(200);
}

/*********************************************************************************************
function:                      Jupiter Pico T
  close_gpspico_allOutputSentence
  - close OutputSentence of GGA, ZDA, GLL, GSA, GSV, VTG, RMC, tps, tlp, and tst 
    after scc driver works.
     
paramenter:
  void            
  												
return:
	N/A
*/
void close_gpspico_allOutputSentence(int port)
{
    char *cmdString = "@@Ea0*54\r\n";
    int len;
    len = strlen(cmdString);
    if (port == SCC3PORT)
        uartCmdAct(cmdString, len, NULL, 0, 3); /* for scc3 */
    else if (port == SCC1PORT)
        uartCmdAct(cmdString, len, NULL, 0, 1); /* for scc1 */
}


/************************************************************************************
function: 
  fltstatPasing
paramenter:
  char *respond -  store response data for  FLTSTAT COMMAND          
  												
return:
	-2 : No signal time-out 
	-1 : No gps-device respond on scc3 port
	 0 : device works normally
*/
static int fltstatPasing(char *respond)
{
    if (respond[0] == '\0')
        return 1;
    if (strncmp(respond, "0x", 2) != 0) {
        return -1;
    } else if (atoi(&respond[5]) & 0x2); /*  no signal time-out */  {
        return -2;
    }
    return 0;
}


/************************************************************************************
function: 
  cdmaStatus  
paramenter:
  int port -  port number(1 for scc1, and 3 for scc3)          
  												
return:
	-2 : No signal time-out 
	-1 : No CDMA-device respond over scc port
	 0 : device works normally.
	-3  - paramenter "port" was invalid, which should be either 1 or 3.
*/
static int cdmaStatus(int port)
{
    int len;
    int idx = 0;
    int check;
    DEV_TYPE dev = cdmaDev;
    char rxBuf[9];
    char *cmdString = "FLTSTAT\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-3);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    check = uartApp[idx].rxParseFunc(rxBuf);
    return check;
}


/************************************************************************************
function: 
  cdmaAckPasing
paramenter:
  char *rxBuf -  response string from scc          
  												
return:
   -1  - Fail to send command to scc for CDMA.
	0  - setting parameter message successfully.
*/
static int cdmaAckPasing(char *rxBuf)
{
    if (rxBuf[0] == '\0')
        return 1;
    if (strncmp(rxBuf, "OK", 2) != 0) {
        return -1;
    }
    return 0;
}


/************************************************************************************
function: 
  cdmaCTIME - close time-of-day output message.
paramenter:
  int port -  port number(1 for scc1, and 3 for scc3)          
  												
return:
   -1  - Fail to close time-of-day output message
	0  - close time-of-day output message successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
*/
int cdmaCTIME(int port)
{
    int len;
    int idx = 0;
    DEV_TYPE dev = cdmaDev;
    char *cmdString = "CTIME=OFF\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, NULL, 0, port);
    return 0;
}


/************************************************************************************
function: 
  cdmaTime - Show the current time. 
paramenter:
  int port -  port number(1 for scc1, and 3 for scc3)          
  serialCDMAtime_t * CDMAtimeData - the struct to store response data for TIME\r\n command.												
return:
	0  - Show the current time successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   others - response data is invalid.
*/
static int cdmaTime(int port, serialCDMAtime_t * CDMAtimeData)
{
    int len;
    int idx = 0;
    DEV_TYPE dev = cdmaDev;
    char rxBuf[30];
    char *cmdString = "TIME\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    if (port == SCC1PORT || port == SCC3PORT);
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    return (uartApp[idx].rxParseFunc(rxBuf, CDMAtimeData));
}


/************************************************************************************
function: 
  cdmaSetPSSWidth - Set the 1PPS pulsewidth in milliseconds.    
paramenter:
  int wid  -  pulsewidth in milliseconds, may be 1 to 999
  int port -  port number(1 for scc1, and 3 for scc3)            												
return:
   -1  - Fail to set the 1PPS pulsewidth in milliseconds.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -3  - paramenter "wid" was invalid, which should be 1 to 999.
   	0  - set the 1PPS pulsewidth in milliseconds successfully.
*/
static int cdmaSetPSSWidth(int wid, int port)
{
    int len, result;
    int idx = 0;
    DEV_TYPE dev = cdmaDev;
    char rxBuf[5], cmdString[15];
    char *cmdhead = "PPSWIDTH";
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-2);
    if ((wid < 1) || (wid > 999))
        return (-3);
    sprintf(cmdString, "%s=%d\r\n", cmdhead, wid);
    len = strlen(cmdhead);
    idxCalculate(cmdhead, len, &idx, dev);
    len = strlen(cmdString);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    result = uartApp[idx].rxParseFunc(rxBuf);
    return result;
}


/************************************************************************************
function: 
  cdmaReset - perform a software reset of the unit.     
paramenter:
   int port -  port number(1 for scc1, and 3 for scc3)        												
return:
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -1  - Fail to perform a software reset of the unit.
	0  - perform a software reset of the unit successfully.
*/
static int cdmaReset(int port)
{
    int len;
    int idx = 0;
    DEV_TYPE dev = cdmaDev;
    char rxBuf[5];
    char *cmdString = "RESET\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    return (uartApp[idx].rxParseFunc(rxBuf));
}


/************************************************************************************
function: 
   cdmaVer - Display the firmware and hardware versions.  
paramenter:
   int port -  port number(1 for scc1, and 3 for scc3)      
   char *ver -  string associatted with the firmware and hardware versions. 												
return:
   0 - ok
   
*/
static int cdmaVer(int port, char *ver)
{
    int len;
    int idx = 0;
    DEV_TYPE dev = cdmaDev;
    char rxBuf[5];
    char *cmdString = "VER\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, dev);
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-1);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    strcpy(ver, rxBuf);
    return 0;
}


/************************************************************************************
function: 
  cdmaLeap - Set current and future leap seconds. 
paramenter:
   int port -  port number(1 for scc1, and 3 for scc3)      
   int leap -  current and future leap seconds											
return:
   -1  - Fail to Set current and future leap seconds.
	0  - Set current and future leap seconds successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -3  - paramenter "curleap/futleap" value was invalid.
*/
static int cdmaLeap(int port, int curleap, int futleap)
{
    int len, result;
    int idx = 0;
    DEV_TYPE dev = cdmaDev;
    char rxBuf[5], cmdString[15];
    char *cmdhead = "LEAP";
    if ((port == SCC1PORT) || (port == SCC3PORT));
    else
        return (-2);
    if ((curleap < 0) || (curleap > 60))
        result = -3;
    if ((futleap < 0) || (futleap > 60))
        result = -3;
    sprintf(cmdString, "%s=%d,%d\r\n", cmdhead, curleap, futleap);
    len = strlen(cmdhead);
    idxCalculate(cmdhead, len, &idx, dev);
    len = strlen(cmdString);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    result = uartApp[idx].rxParseFunc(rxBuf);
    return result;
}


/*************************************************************************
function: 
  idPasing - pasing response data of "ID\r\n" command of the either Rebidium or Crystal device.
paramenter:
 serialID_t * idData - 	the struct to store response data for "ID\r\n" command.  
  char *string2Parsed - pointer to response string for "ID\r\n" command.													
return:
	0 - response data is valid.
	other - response data is invalid.  SERIALTEST
*/
static int idPasing(char *string2Parsed, serialID_t * idData)
{
    char *mixedtoken;
    int whichtoken = 0;
    int cmdError = 0;
    if (string2Parsed[0] == '\0')
        return 1;
    mixedtoken = strtok(string2Parsed, "-/\r\n");
    while (mixedtoken != NULL) {
#ifdef SERIALTEST
        printf("token #: %d  [%s]\n", whichtoken, mixedtoken);
#endif
        if (cmdError != 0)
            break;
        switch (whichtoken) {
        case 0:
            if ((strncmp(mixedtoken, "TNTSRO", 6) == 0)
                || (strncmp(mixedtoken, "TNTSXO", 6) == 0)) {
                strncpy(idData->idhead, mixedtoken, 6);
                cmdError = 0;
            } else
                cmdError = -1;
            break;
        case 1:
            idData->aaa = atoi(mixedtoken);
            if ((idData->aaa < 0) || (idData->aaa > 999))
                cmdError = -2;
            break;
        case 2:
            idData->rr = atoi(mixedtoken);
            if ((idData->rr < 0) || (idData->rr > 99))
                cmdError = -2;
            break;
        case 3:
            idData->sss = atof(mixedtoken);
            cmdError = 7;
            if ((idData->sss < 0) || (idData->sss > 9))
                cmdError = -3;
            break;
        default:
            cmdError = -4;
            break;
        }
        mixedtoken = strtok(NULL, "-/\r\n");
        whichtoken++;
    }
    return cmdError;
}


/************************************************************************************
function: 
  rb_or_cryID - Query identification of the either Rubidium or Crystal device.    
paramenter:   
   int port -  port number(2 for smc1, and 4 for scc4)      
   serialID_t *idData	-	the struct to store response data for ID\r\n command.	
   DEV_TYPE devType   -     type of serial device connected with scc1 or scc3.  											
return:
	0  - Query iditification successfully.
   -4  - paramenter "port" was invalid, which should be either 1 or 3.
   -5  - paramenter "devType" was invalid, which should be either sroDev or sxoDev here.
   others - response data in rxBuf is invalid.								

*/
static int rb_or_cryID(int port, serialID_t * idData, DEV_TYPE devType)
{
    int len;
    int idx = 0;
    char rxBuf[22];
    char *cmdString = "ID\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-4);
    if ((devType == sroDev) || (devType == sxoDev));
    else
        return (-5);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the ID rxbuf:%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, idData));
}


/*************************************************************************
function: 
  statusPasing - pasing response data of "ST\r\n" command of the either Rebidium or Crystal device.
paramenter:
  int * statusData  - 	the parameter to store response data for "ST\r\n" command.  
  char *string2Parsed - pointer to response string for "ST\r\n" command.													
return:
	0 - response data is valid.
	other - response data is invalid.
*/
static int statusPasing(char *string2Parsed, int *statusData)
{
    int cmdError = 0;
    char *mixedtoken;
    if (string2Parsed[0] == '\0') {
    	if (markdebug == 1) {
        	printf("\nFail to get status of Rb or Crystall\n");
        }
        return -1;
    }
    mixedtoken = strtok(string2Parsed, "\r\n");
#ifdef SERIALTEST
    printf("token value:[%s]\n", mixedtoken);
#endif
    *statusData = atoi(mixedtoken);
    if ((*statusData < 0) || (*statusData > 10)) {
        cmdError = -1;
        if (markdebug == 1) {
        	printf("error");
        }
    } else {
        switch (*statusData) {
        case 0:
        	if (markdebug == 1) {
            	printf("The status is warming up\n");
            }
            break;
        case 1:
        	if (markdebug == 1) {
            	printf("The status is tracking set-up\n");
            }
            break;
        case 2:
        	if (markdebug == 1) {
            	printf("The status is track to PPSREF\n");
            }
            break;
        case 3:
        	if (markdebug == 1) {
            	printf("The status is synch to PPSREF\n");
            }
            break;
        case 4:
        	if (markdebug == 1) {
            	printf("The status is Free run. Track OFF\n");
            }
            break;
        case 5:
        	if (markdebug == 1) {
            	printf("The status is PPSREF unstable(hldovr)\n");
            }
            break;
        case 6:
            if (markdebug == 1) {
            	printf("The status is No PPSREF(holdover)\n");
            }
            break;
        case 7:
            if (markdebug == 1) {
            	printf("The status is factory used\n");
            }
            break;
        case 8:
            if (markdebug == 1) {
            	printf("The status is factory used\n");
            }
            break;
        case 9:
            if (markdebug == 1) {
            	printf("The status is fault\n");
            }
            break;
        default:
            cmdError = -2;
            break;
        }
    }
    return cmdError;
}


/************************************************************************************
function: 
  rb_or_cryStatus - Query status of the either Rebidium or Crystal device.
paramenter:   
   int port -  port number(2 for smc1, and 4 for scc4)      
   int * statusData	-	the parameter to store response data for ST\r\n command.	
   DEV_TYPE devType   -  type of serial device connected with scc2 or scc4.  											
return:
	0  - Query status successfully.
   -3  - paramenter "port" was invalid, which should be either 2 or 4.
   others - response data in rxBuf is invalid.								
*/
static int rb_or_cryStatus(int port, int *statusData)
{
    int len;
    int idx = 0;
    char rxBuf[4];
    DEV_TYPE devType = sroDev;
    char *cmdString = "ST\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-3);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the rxbuf of rbcrystatus:%s", rxBuf);
    }
    if (rxBuf[0] == '\0') {
    	if (markdebug == 1) {
        	printf("\nFail to get status of Rb or Crystall\n");
        }		
        return 1;
    } else
        return (uartApp[idx].rxParseFunc(rxBuf, statusData));
}


/*************************************************************************
function: 
  snPasing - pasing response data of "SN\r\n" command of the either Rebidium or Crystal device.
paramenter:
  long * snData  - 	the parameter to store response data for "SN\r\n" command.  
  char *string2Parsed - pointer to response string for "SN\r\n" command.													
return:
	0 - response data is valid.
   -1 - response data is invalid.
*/
static int snPasing(char *string2Parsed, int snData)
{
    char *mixedtoken;
    if (string2Parsed[0] == '\0')
        return 1;
    mixedtoken = strtok(string2Parsed, "\r\n");
    snData = atoi(mixedtoken);
    if (markdebug == 1) {
        printf("serialnumber is:%d", snData);
    }
    if ((snData < 0) || (snData > 999999)) {
        return -1;
    }
    return 0;
}


/************************************************************************************
function: 
  rb_or_crySerNum - Query serial number of the either Rebidium or Crystal device.
paramenter:  
   int port -  port number(2 for smc1, and 4 for scc4)      
   int * statusData	-	the parameter to store response data for SN\r\n command.	
   DEV_TYPE devType   -  type of serial device connected with scc1 or scc3.  											
return:
	0  - Query serial number successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -1 - response data in rxBuf is invalid.								
*/
static int rb_or_crySerNum(int port, long *snData)
{
    int len;
    int idx = 0;
    char rxBuf[9];
    char *cmdString = "SN\r\n";
    DEV_TYPE devType = sroDev;
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the serial number is:%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, snData));
}


/*************************************************************************
function: 
  TR3Pasing - pasing response data of "TR3\r\n" command of the either Rebidium or Crystal device.
paramenter:
  int *trackStatus  - 	the parameter to store response data for "TR3\r\n" command.  
  char *string2Parsed - pointer to response string for "TR3\r\n" command.													
return:
	0 - response data is valid.
   -1 - response data is invalid.
*/
static int TR3Pasing(char *string2Parsed, int *trackStatus)
{
    char *mixedtoken;
    if (string2Parsed[0] == '\0')
        return 1;
    mixedtoken = strtok(string2Parsed, "\r\n");
    *trackStatus = atoi(mixedtoken);
    if (markdebug == 1) {
        printf("tr3parsing value is:%d\n", *trackStatus);
    }
    if ((*trackStatus == 0) || (*trackStatus == 1))
        return 0;
    else
        return -1;
}


/************************************************************************************
function: 
  rb_or_crySetTrack - Set tracking for the either Rebidium or Crystal device.    
paramenter:  
   int port -  port number(2 for smc1, and 4 for scc4)      
   int * trackStatus	-	the parameter to store response data for TR3\r\n command.	 											
return:
	  0  - Set tracking successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -1  - response data in rxBuf is invalid.								
*/
int rb_or_crySetTrack(int port, int *trackStatus)
{
    int len;
    int idx = 0;
    char rxBuf[4];
    char *cmdString = "TR3\r\n";
    DEV_TYPE devType = sroDev;
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));

    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the cryrbsetrack is:%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, trackStatus));
}


/************************************************************************************
function: 
  rb_or_cryTrackFreeRun - Set tracking to track never for the either Rebidium or Crystal device.  
paramenter:  
   int port -  port number(2 for smc1, and 4 for scc4)      
   int * trackStatus	-	the parameter to store response data for TR3\r\n command.	 											
return:
	  0  - Set tracking successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -1  - response data in rxBuf is invalid.								
*/
static int rb_or_cryTrackFreeRun(int port, int *trackStatus)
{
    int len;
    int idx = 0;
    char rxBuf[4];
    char *cmdString = "TR0\r\n";
    DEV_TYPE devType = sroDev;
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the cryrbsetrack is:%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, trackStatus));
}


/*************************************************************************
function: 
  SY3Pasing - pasing response data of "SY3\r\n" command of the either Rebidium or Crystal device.
paramenter:
  int *syncStatus  - 	the parameter to store response data for "SY3\r\n" command.  
  char *string2Parsed - pointer to response string for "SY3\r\n" command.													
return:
	0 - response data is valid.
   -1 - response data is invalid.
*/
static int SY3Pasing(char *string2Parsed, int *syncStatus)
{
    char *mixedtoken;
    if (string2Parsed[0] == '\0')
        return 1;
    mixedtoken = strtok(string2Parsed, "\r\n");
    *syncStatus = atoi(mixedtoken);
    if (markdebug == 1) {
        printf("sy3parsing value is:%d\n", *syncStatus);
    }
    if ((*syncStatus == 0) || (*syncStatus == 1))
        return 0;
    else
        return -1;
}


/*************************************************************************
function: 
  BT3Pasing - pasing response data of "BT3\r\n" command of the either Rubidium or Crystal device.
paramenter:
  int *beatStatus  - 	the parameter to store response data for "BT3\r\n" command.  
  char *string2Parsed - pointer to response string for "BT3\r\n" command.													
return:
	0 - response data is valid.
   -1 - response data is invalid.
*/
static int BT3Pasing(char *string2Parsed, serialBS_t * btData)
{
    char *mixedtoken;
    if (string2Parsed[0] == '\0')
        return 1;
    mixedtoken = strtok(string2Parsed, " \r\n");
    btData->ddddddddd = atol(mixedtoken);
    if (markdebug == 1) {
        printf("bt3parsing value is:%ld\n", btData->ddddddddd);
    }
    return 0;
}


/************************************************************************************
function: 
  rb_or_crySetSync - Set Synchronisation PPSOUT - PPSINT for the either Rebidium or Crystal device. 
paramenter:   
   int port -  port number(2 for smc1, and 4 for scc4)      
   int * trackStatus	-	the parameter to store response data for TR3\r\n command.												
return:
	0  - Set Synchronisation successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -1  - response data in rxBuf is invalid.								

*/
int rb_or_crySetSync(int port, int *syncStatus)
{
    int len;
    int idx = 0;
    char rxBuf[4];
    char *cmdString = "SY3\r\n";
    DEV_TYPE devType = sroDev;
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the cryrbsetsync is:%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, syncStatus));
}


/************************************************************************************
function: 
  rb_or_crySetSyncFreeRun - Set Synchronisation PPSOUT - PPSINT to free run for the either Rebidium or Crystal device.  
paramenter:   
   int port -  port number(2 for smc1, and 4 for scc4)      
   int * trackStatus	-	the parameter to store response data for TR3\r\n command.												
return:
	0  - Set Synchronisation successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -1  - response data in rxBuf is invalid.								

*/
static int rb_or_crySetSyncFreeRun(int port, int *syncStatus)
{
    int len;
    int idx = 0;
    char rxBuf[4];
    char *cmdString = "SY0\r\n";
    DEV_TYPE devType = sroDev;
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the cryrbsetsync is:%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, syncStatus));
}


/************************************************************************************
function: 
  rb_or_cryBeatSec - Beat every second on serial port for the either Rebidium or Crystal device.
 
     
paramenter:
   
   int port -  port number(2 for smc1, and 4 for scc4)      
   int * beatStatus	-	the parameter to store response data for BT3\r\n command.												
return:
	0  - Beat every second on serial port successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -1  - response data in rxBuf is invalid.								

*/
static int rb_or_cryBeatSec(int port, serialBS_t * bsData)
{
    int len;
    int idx = 0;
    char rxBuf[50];
    DEV_TYPE devType = sroDev;
    char *cmdString = "BT3\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-4);
    if ((devType == sroDev) || (devType == sxoDev));
    else
        return (-5);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the cryrbbeatsec=%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, bsData));
}


/************************************************************************************
function: 
  rb_or_cryStopBeatSec - Stop beat on serial port for the either Rebidium or Crystal device.
 
     
paramenter:
   
   int port -  port number(2 for smc1, and 4 for scc4)      
   int * beatStatus	-	the parameter to store response data for BT3\r\n command.												
return:
	0  - Beat every second on serial port successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -1  - response data in rxBuf is invalid.								

*/
static int rb_or_cryStopBeatSec(int port, serialBS_t * bsData)
{
    int len;
    int idx = 0;
    char rxBuf[50];
    DEV_TYPE devType = sroDev;
    char *cmdString = "BT0\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-4);
    if ((devType == sroDev) || (devType == sxoDev));
    else
        return (-5);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the cryrbbeatsec=%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, bsData));
}


/************************************************************************************
function: 
  rb_or_cryPPSBeatSec - Beat every second on serial port for the either Rebidium or Crystal device.    
paramenter:   
   int port -  port number(2 for smc1, and 4 for scc4)      
   int * beatStatus	-	the parameter to store response data for BT3\r\n command.												
return:
	0  - Beat every second on serial port successfully.
   -2  - paramenter "port" was invalid, which should be either 1 or 3.
   -1  - response data in rxBuf is invalid.								

*/
static int rb_or_cryPPSBeatSec(int port, serialBS_t * bsData)
{
    int len;
    int idx = 0;
    char rxBuf[50];
    DEV_TYPE devType = sroDev;
    char *cmdString = "BT1\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-4);
    if ((devType == sroDev) || (devType == sxoDev));
    else
        return (-5);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the cryrbbeatsec=%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, bsData));
}


/*************************************************************************
function: 
  VTPasing - pasing response data of "VT\r\n" command of the either Rebidium or Crystal device.
paramenter:
  long * vtData  - 	the parameter to store response data for "VT\r\n" command.  
  char *string2Parsed - pointer to response string for "VT\r\n" command.													
return:
	0 - response data is valid.
   -1 - response data is invalid.
*/
static int VTPasing(char *string2Parsed, long vtData)
{
    char *mixedtoken;
    if (string2Parsed[0] == '\0')
        return 1;
    mixedtoken = strtok(string2Parsed, "\r\n");
    vtData = atol(mixedtoken);
    if (markdebug == 1) {
        printf("view the time constant is:%ld", vtData);
    }
    if ((vtData < 0) || (vtData > 999999)) {
        return -1;
    }
    return 0;
}


/*************************************************************************
function: 
  TCPasing - pasing response data of "TC\r\n" command of the either Rebidium or Crystal device.
paramenter:
  int  tcData  - 	the parameter to store response data for "VT\r\n" command.  
  char *string2Parsed - pointer to response string for "VT\r\n" command.													
return:
	0 - response data is valid.
   -1 - response data is invalid.
*/
static int TCPasing(char *string2Parsed, int tcData)
{
    char *mixedtoken;
    if (string2Parsed[0] == '\0')
        return 1;
    mixedtoken = strtok(string2Parsed, "\r\n");
    tcData = atoi(mixedtoken);
    if (markdebug == 1) {
        printf("set the time constant to:%d", tcData);
    }
    if ((tcData < 0) || (tcData > 999999)) {
        return -1;
    }
    return 0;
}


/*************************************************************************
function: 
  TWPasing - pasing response data of "TW\r\n" command of the Rubidium device.
paramenter:
  int  twData  - 	the parameter to store response data for "TW\r\n" command.  
  char *string2Parsed - pointer to response string for "TW\r\n" command.													
return:
	0 - response data is valid.
   -1 - response data is invalid.
*/
static int TWPasing(char *string2Parsed, int twData)
{
    char *mixedtoken;
    if (string2Parsed[0] == '\0')
        return 1;
    mixedtoken = strtok(string2Parsed, "\r\n");
    twData = atoi(mixedtoken);
    if (markdebug == 1) {
        printf("set the time Window to:%d", twData);
    }
    if ((twData < 0) || (twData > 999999)) {
        return -1;
    }
    return 0;
}


/*************************************************************************
function: 
  RAPasing - pasing response data of "RAsddd\r\n" command of the either Rebidium or Crystal device.
paramenter:
  int  raData  - 	the parameter to store response data for "RAsddd\r\n" command.  
  char *string2Parsed - pointer to response string for "RAsddd\r\n" command.													
return:
	0 - response data is valid.
   -1 - response data is invalid.
*/
static int RAPasing(char *string2Parsed, int raData)
{
    char *mixedtoken;
    if (string2Parsed[0] == '\0')
        return 1;
    mixedtoken = strtok(string2Parsed, "\r\n");
    raData = atoi(mixedtoken);
    if (markdebug == 1) {
        printf("the response of rapasing is:%d", raData);
    }
    if ((raData < -129) || (raData > 128)) {
        return -1;
    }
    return 0;
}


/************************************************************************************
function: 
  rb_or_cryViewTimeConst - Query loop time constant now in use of the either Rebidium or Crystal device.
      
paramenter:   
   int port -  port number(2 for smc1, and 4 for scc4)      
   long * vtData	-	the parameter to store response data for VT\r\n command.	  											
return:								

*/
static int rb_or_cryViewTimeConst(int port, long *vtData)
{
    int len;
    int idx = 0;
    char rxBuf[9];
    char *cmdString = "VT\r\n";
    DEV_TYPE devType = sroDev;
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the time constant is:%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, vtData));
}


/*************************************************************************
function: 
  rb_or_cryReset - Reset Rebidium or Crystal device.
paramenter:
  int port -  port number(2 for smc1, and 4 for scc4)   
  												
return:
   -1 - paramenter "port" was invalid, which should be either 1 or 3.
*/
static int rb_or_cryReset(int port)
{
    int len;
    int idx = 0;
    char rxBuf[50];
    DEV_TYPE devType = sroDev;
    char *cmdString = "RESET\r\n";
    len = strlen(cmdString);
    idxCalculate(cmdString, len, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-1);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("the cryrbreset is:%s\n", rxBuf);
    }
    taskDelay(100);
    return 0;
}


/************************************************************************************
function: 
  rb_or_crySetTC - Set tracking phase loop time constant in use of the either Rebidium or Crystal device.
     
paramenter:   
   int port -  port number(2 for smc1, and 4 for scc4)      
   char cmdString	-	the parameter to input TC??????\r\n command.	  ?????? = seconds	
   long *tcData - the parameter to store the response of the tc command										
return:								

*/
static int rb_or_crySetTC(int port, char *cmdString, int *tcData)
{
    int len;
    int idx = 0;
    char rxBuf[9];
    DEV_TYPE devType = sroDev;
    len = strlen(cmdString);
    if (markdebug == 1) {
        printf("the command here is:%s\n", cmdString);
    }
    idxCalculate(cmdString, 2, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("after setting the track time constant is:%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, tcData));
}


/************************************************************************************
function: 
  rb_or_crySetTW - Set tracking window(rounded to 50ns) in use of the Rubidium device.
      
paramenter:   
   int port -  port number(2 for smc1, and 4 for scc4)      
   char cmdString	-	the parameter to input TW??????\r\n command.	  ?????? = seconds	
   long *twData - the parameter to store the response of the tc command										
return:								

*/
static int rb_or_crySetTW(int port, char *cmdString, int *twData)
{
    int len;
    int idx = 0;
    char rxBuf[9];
    DEV_TYPE devType = sroDev;
    len = strlen(cmdString);
    if (markdebug == 1) {
        printf("the command here is:%s\n", cmdString);
    }
    idxCalculate(cmdString, 2, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    if (markdebug == 1) {
        printf("after setting the track time constant is:%s\n", rxBuf);
    }
    return (uartApp[idx].rxParseFunc(rxBuf, twData));
}


/************************************************************************************
function: 
  rb_or_cryRawPA - Raw phase adjust in use of the either Rebidium or Crystal device.
     
paramenter:   
   int port -  port number(2 for smc1, and 4 for scc4)      
   char cmdString	-	the parameter to input TC??????\r\n command.	  ?????? = seconds	
   long *raData - the parameter to store the response of the tc command										
return:								

*/
static int rb_or_cryRawPA(int port, char *cmdString, int *raData)
{
    int len;
    int idx = 0;
    char rxBuf[9];
    DEV_TYPE devType = sroDev;
    len = strlen(cmdString);
    if (markdebug == 1) {
        printf("the command here is:%s\n", cmdString);
    }
    idxCalculate(cmdString, 2, &idx, devType);
    if ((port == SMC1PORT) || (port == SCC4PORT));
    else
        return (-2);
    uartCmdAct(cmdString, len, rxBuf, 1, port);
    return (uartApp[idx].rxParseFunc(rxBuf, raData));
}


/*************************************************************************
function: 
  queryGPS - Query GPS device status.
paramenter:
  int port -  port number(1 for scc1, and 3 for scc3)
  int *gpsDelay -  polling latency for device.    
  												
return:
    2 - No gps device respond over scc1 or scc3 port 
    3 - Fix not available or invalid
    0 - gps works normally 
*/
int queryGPS(int port, int *gpsDelay)
{
    int status;
    status = gpsdevquery(port);
    if (status == GPS_FIX_INVALID) {

        /* alarm "Fix not available or invalid" */
        *gpsDelay = 5;
        if (markdebug == 1) {
            printf("Fix not available or invalid\n");
        }
    } else if (status == OK) {
        /* GPS works normally */
        *gpsDelay = 0;
    } else if (status == NO_GPS_DEVICE) {
        /* alarm "No gps-device respond on scc3 or scc1 port" */
        if (markdebug == 1) {
            printf("No GPS device respond over scc%d\n", port);
        }
        *gpsDelay = 5;
    }
    return status;
}


/*************************************************************************
function: 
  queryCDMA - Query CDMA device status.
paramenter:
  int port -  port number(1 for scc1, and 3 for scc3)
  int *cdmaDelay -  polling latency for device.     
  												
return:
  -2 - No signal time-out over scc1 or scc3 port 
  1 - CDMA device was absent over scc1 or scc3 port
  0 - CDMA device works normally
*/
int queryCDMA(int port, int *cdmaDelay)
{
    int result = 0;
    switch (cdmaStatus(port)) {
    case -2:
        *cdmaDelay = 5;
        /* alarm "no signal time-out over scc1 or scc3 port" */
        result = -2;
        if (markdebug == 1) {
            printf("no signal time-out\n");
        }
        break;
    case -1:
        *cdmaDelay = 5;
        result = 1;
        if (markdebug == 1) {
            /* alarm "cdma-device was absent over scc3 or scc1 port" */
            printf("No CDMA device respond over scc%d\n", port);
        }
        break;
    case 0:                    /* CDMA works normally */
        if (markdebug == 1) {
            printf("CDMA works normally\n");
        }
        result = 0;
        break;
    default:
        result = 0;
        break;
    }
    return result;
}


/*************************************************************************
function: 
  queryRb_Cry - Query Rb/Crystall device status.
paramenter:
  int port -  port number(2 for smc1, and 4 for scc4)
  int *rb_cryDelay -   polling latency for device.    
  int *rb_cryStaData - store OSC device status. 												
return:
  0 - Oscillator works normally
  1 - No Rubidium/Crystal device respond over smc2 or scc4 port
*/
int queryRb_Cry(int port, int *rb_cryDelay, int *rb_cryStaData,
                DEV_TYPE rb_CryDev)
{
    serialID_t rb_cryID;
    int result;
    if (rb_or_cryID(port, &rb_cryID, rb_CryDev) == 0) {
        result = 0;
        rb_or_cryStatus(port, rb_cryStaData);
        if (strncmp(rb_cryID.idhead, "TNTSRO", 6) == 0) {
            if (port == SMC1PORT)
                osc1Priority = 1;
            else if (port == SCC4PORT)
                osc2Priority = 1;
        } else if (strncmp(rb_cryID.idhead, "TNTSXO", 6) == 0) {
            if (port == SMC1PORT)
                osc1Priority = 2;
            else if (port == SCC4PORT)
                osc2Priority = 2;
        }
        switch (*rb_cryStaData) {
        case 0:
            /* alarm Rb or Crystal device "warming up" */
            if (markdebug == 1) {
            	printf("warming up\n");
        	}
            break;
        case 1:
            /* alarm Rb or Crystal device "tracking set-up" */
            if (markdebug == 1) {
            	printf("tracking set-up\n");
            }
            break;
        case 2:
            /* alarm Rb or Crystal device "track to PPSREF" */
            if (markdebug == 1) {
            	printf("track to PPSREF\n");
            }
            break;
        case 3:
            /* alarm Rb or Crystal device "sync to PPSREF" */
            if (markdebug == 1) {
            	printf("sync to PPSREF\n");
            }
            break;
        case 4:
            /* alarm Rb or Crystal device "Free run. Track off" */
            if (markdebug == 1) {
            	printf("Free run. Track off\n");
            }
            break;
        case 5:

            /* alarm Rb or Crystal device "PPSREF unstable(holdover)" */
            if (markdebug == 1) {
            	printf("PPSREF unstable(holdover)\n");
            }
            break;
        case 6:
            /* alarm Rb or Crystal device "No PPSREF(holdover)"  */
            if (markdebug == 1) {
            	printf("No PPSREF(holdover)\n");
            }
            break;
        case 7:
            /* alarm Rb or Crystal device "factory used" */
            if (markdebug == 1) {
            	printf("factory used\n");
            }
            break;
        case 8:
            /* alarm Rb or Crystal device "factory used" */
            if (markdebug == 1) {
            	printf("factory used\n");
            }
            break;
        case 9:
            /* alarm Rb or Crystal device "fault" */
            if (markdebug == 1) {
            	printf("fault\n");
            }
            break;
        default:
            break;
        }
    } else {
        *rb_cryDelay = 5;
        result = 1;

        /* alarm "No Rubidium/Crystal device respond over smc2 or scc4 port" */
        if (port == SMC1PORT) {
            if (markdebug == 1) {
                printf("No Rubidium/Crystal device respond over smc1\n");
            }
        } else if (port == SCC4PORT) {
            if (markdebug == 1) {
                printf("No Rubidium/Crystal device respond over scc%d\n",
                       port);
            }
        }
    }
    return result;
}


void oscNormal()
{
    int immrVal = vxImmrGet();
    *M8260_IOP_PAPAR(immrVal) &= ~PA20;
    *M8260_IOP_PADIR(immrVal) |= PA20;  /* output */
    *M8260_IOP_PADAT(immrVal) |= PA20;
    taskDelay(10);
    *M8260_IOP_PADAT(immrVal) &= ~PA20;
    taskDelay(10);
    *M8260_IOP_PADAT(immrVal) |= PA20;
}


/*************************************************************************
function: 
  osc1PowerOnOff -  OSC1 power on/off via port-A pin 31. 
paramenter:
  int on_off - indicate that OSC1 will be powered either on or off. 
               1: powered on; 
               0: powered off; 
  												
return:
  N/A 
*/
void osc1PowerOnOff(int on_off)
{
    int immrVal = vxImmrGet();
    *M8260_IOP_PAPAR(immrVal) &= ~PA31;
    *M8260_IOP_PADIR(immrVal) |= PA31;  /* output */
    if (on_off == 1)
        *M8260_IOP_PADAT(immrVal) |= PA31;
    else
        *M8260_IOP_PADAT(immrVal) &= ~PA31;
    taskDelay(100);
}


/*************************************************************************
function: 
  osc1PowerOnOff -  OSC2 power on/off via port-B pin 31. 
paramenter:
  int on_off - indicate that OSC2 will be powered either on or off. 
               1: powered on; 
               0: powered off; 
  												
return:
  N/A 
*/
void osc2PowerOnOff(int on_off)
{
    int immrVal = vxImmrGet();
    *M8260_IOP_PBPAR(immrVal) &= ~PB31;
    *M8260_IOP_PBDIR(immrVal) |= PB31;  /* output */
    if (on_off == 1)
        *M8260_IOP_PBDAT(immrVal) |= PB31;
    else
        *M8260_IOP_PBDAT(immrVal) &= ~PB31;
    taskDelay(100);
}


/*************************************************************************
function: 
  STATUS_t getUTC_GT(int *yy,int *mn, int *dy, int *hh, int *mm, int *ss, int port)
output paramenter:
	int *hh	: hour
	int *mm	: minuate
	int *ss	: second
							
return:
  OK	
  ERROR
  printf("utc is[%d]:[%d]:[%d]", *hh, *mm ,*ss);
**************************************************************************/
STATUS_t getUTC_GT(int *yy, int *mn, int *dy, int *hh, int *mm,
                   int *ss, int port)
{
    serialGpsGpzda_t input;
    int err;
    if (markdebug == 1) {
        printf("Testing getUTC_GT(port:%d)\n", port);
    }
    err = sccGPZDA(&input, port);
    if (err == 0 || err == 7) {
        *yy = input.utc.yy;
        *mn = input.utc.mn;
        *dy = input.utc.dy;
        *hh = input.utc.hh;
        *mm = input.utc.mm;
        *ss = input.utc.ss;
        return OK;
    } else {
        return ERROR;
    }
}


STATUS_t getUTC_30(int *yy, int *mn, int *dy, int *hh, int *mm, int *ss,
                   int port)
{
    serialGpsGpzda_t input;
    int err;
    if (markdebug == 1) {
        printf("Testing getUTC_30(port:%d)\n", port);
    }
    err = GPZDA30(&input, port);
    if (err == 0 || err == 7) {
        *yy = input.utc.yy;
        *mn = input.utc.mn;
        *dy = input.utc.dy;
        *hh = input.utc.hh;
        *mm = input.utc.mm;
        *ss = input.utc.ss;
        return OK;
    } else {
        return ERROR;
    }
}


#ifdef SERIAL_DEBUG

void test_SelfTestResult()
{
    int err;
    serialGpsGptst_t gptstData;
    close_gps_allOutputSentence(3);
    err = gpsSelfTestResult(&gptstData, 3);
    if (markdebug == 1) {
    	printf("err=%d\n", err);
    }
}

void RbID1()
{
    serialID_t idData;
    rb_or_cryID(2, &idData, sxoDev);
}

void RbID2()
{
    serialID_t idData;
    rb_or_cryID(4, &idData, sroDev);
}

void RbSR1()
{
    long snData;
    rb_or_crySerNum(2, &snData);
}

void RbSR2()
{
    long snData;
    rb_or_crySerNum(4, &snData);
}

void Rb1Status()
{
    int statusData;
    rb_or_cryStatus(2, &statusData);
}

void Rb2Status()
{
    int statusData;
    rb_or_cryStatus(4, &statusData);
}

void SetTrack1()
{
    int trackStatus;
    if (rb_or_crySetTrack(2, &trackStatus)){
    	if (markdebug == 1) {
        	printf("Fail to set Tracking");
        }
    }
    else {
    	if (markdebug == 1) {
        	printf("Tracking command status\n");
        }
        if (trackStatus == 0){
            if (markdebug == 1) {
            	printf("Track OFF\n");
            }
        }
        else if (trackStatus == 1){
            if (markdebug == 1) {
            	printf("Track ON\n");
            }
        }
    }
}


void SetTrack2()
{
    int trackStatus;
    if (rb_or_crySetTrack(4, &trackStatus)){
        if (markdebug == 1) {
        	printf("Fail to set Tracking");
        }
    }
    else {
        if (markdebug == 1) {
        	printf("Tracking command status\n");
        }
        if (trackStatus == 0){
            if (markdebug == 1) {
            	printf("Track OFF\n");
            }
        }
        else if (trackStatus == 1){
            if (markdebug == 1) {
            	printf("Track ON\n");
            }
        }
    }
}


/* Set tracking mode to track never, Free Fun.(0->EEPROM) */
void SetTrackFreeRun1()
{
    int trackStatus;
    if (rb_or_cryTrackFreeRun(2, &trackStatus)){
        if (markdebug == 1) {
        	printf("Fail to set Tracking");
        }
    }
    else {
    	if (markdebug == 1) {
        	printf("Tracking command status\n");
        }
        if (trackStatus == 0){
            if (markdebug == 1) {
            	printf("Track OFF\n");
            }
        }
        else if (trackStatus == 1){
            if (markdebug == 1) {
            	printf("Track ON\n");
            }
        }
    }
}

void SetTrackFreeRun2()
{
    int trackStatus;
    if (rb_or_cryTrackFreeRun(4, &trackStatus)){
        if (markdebug == 1) {
        	printf("Fail to set Tracking");
        }
    }
    else {
    	if (markdebug == 1) {
        	printf("Tracking command status\n");
        }
        if (trackStatus == 0){
            if (markdebug == 1) {
            	printf("Track OFF\n");
            }
        }
        else if (trackStatus == 1){
            if (markdebug == 1) {
            	printf("Track ON\n");
            }
        }
    }
}


void SetSync1()
{
    int syncStatus;
    if (rb_or_crySetSync(2, &syncStatus)){
        if (markdebug == 1) {
        	printf("Fail to set  synchronization\n");
        }
    }
    else {
        if (markdebug == 1) {
        	printf("Sync command status\n");
        }
        if (syncStatus == 0){
            if (markdebug == 1) {
            	printf("Sync OFF\n");
            }
        }
        else if (syncStatus == 1){
            if (markdebug == 1) {
            	printf("Sync ON\n");
            }
        }
    }
}


void SetSync2()
{
    int syncStatus;
    if (rb_or_crySetSync(4, &syncStatus)){
        if (markdebug == 1) {
        	printf("Fail to set  synchronization\n");
        }
    }
    else {
        if (markdebug == 1) {        
        	printf("Sync command status\n");
        }
        if (syncStatus == 0){
            if (markdebug == 1) {
            	printf("Sync OFF\n");
            }
        }
        else if (syncStatus == 1){
            if (markdebug == 1) {
            	printf("Sync ON\n");
            }
        }
    }
}


/* Set tracking mode to track never(0->EEPROM) */
void SetSyncFreeRun1()
{
    int syncStatus;
    if (rb_or_crySetSyncFreeRun(2, &syncStatus)){
        if (markdebug == 1) {
        	printf("Fail to set  synchronization\n");
        }
    }
    else {
        if (markdebug == 1) {
        	printf("Sync command status\n");
        }
        if (syncStatus == 0){
            if (markdebug == 1) {
            	printf("Sync OFF\n");
            }
        }
        else if (syncStatus == 1){
            if (markdebug == 1) {
            	printf("Sync ON\n");
            }
        }
    }
}


void SetSyncFreeRun2()
{
    int syncStatus;
    if (rb_or_crySetSyncFreeRun(4, &syncStatus)){
        if (markdebug == 1) {
        	printf("Fail to set  synchronization\n");
        }
    }
    else {
    	if (markdebug == 1) {
        	printf("Sync command status\n");
        }
        if (syncStatus == 0){
            if (markdebug == 1) {
            	printf("Sync OFF\n");
            }
        }
        else if (syncStatus == 1){
            if (markdebug == 1) {
            	printf("Sync ON\n");
            }
        }
    }
}


void Rb1BeatSec()
{
    serialBS_t bsData;
    rb_or_cryBeatSec(2, &bsData);
}


void Rb2BeatSec()
{
    serialBS_t bsData;
    rb_or_cryBeatSec(4, &bsData);
}


void Rb1Stop()
{
    serialBS_t bsData;
    rb_or_cryStopBeatSec(2, &bsData);
}


void Rb2Stop()
{
    serialBS_t bsData;
    rb_or_cryStopBeatSec(4, &bsData);
}


void Rb1BeatPPS()
{
    serialBS_t bsData;
    rb_or_cryPPSBeatSec(2, &bsData);
}


void Rb2BeatPPS()
{
    serialBS_t bsData;
    rb_or_cryPPSBeatSec(4, &bsData);
}


void Rb1ViewTimeConst()
{
    long vtData;
    rb_or_cryViewTimeConst(2, &vtData);
}


void Rb2ViewTimeConst()
{
    long vtData;
    rb_or_cryViewTimeConst(4, &vtData);
}


void Rb1Reset()
{
    rb_or_cryReset(2);
}


void Rb2Reset()
{
    rb_or_cryReset(4);
}


/* set the tracking time constant */
void Rb1SetTC(int seconds)
{
    char cmdString[15];
    int tcData;
    memset(cmdString, 0, sizeof(cmdString));
    if (seconds < -1 || seconds > 999999) {
    	if (markdebug == 1) {
        	printf("the range is not correct\n");
        }
    } else {
        sprintf(cmdString, "TC%06d\r\n", seconds);
        rb_or_crySetTC(2, cmdString, &tcData);
    }
}


void Rb2SetTC(int seconds)
{
    char cmdString[15];
    int tcData;
    memset(cmdString, 0, sizeof(cmdString));
    if (seconds < -1 || seconds > 999999) {
        if (markdebug == 1) {
        	printf("the range is not correct\n");
        }
    } else {
        sprintf(cmdString, "TC%06d\r\n", seconds);
        rb_or_crySetTC(4, cmdString, &tcData);
    }
}


/* set the Rubidium device tracking time window(rounded to 50ns) */
void Rb1SetTW(int seconds)
{
    char cmdString[15];
    int twData;
    memset(cmdString, 0, sizeof(cmdString));
    if (seconds < -1 || seconds > 999) {
        if (markdebug == 1) {
        	printf("the range is not correct\n");
        }
    } else {
        sprintf(cmdString, "TW%03d\r\n", seconds);
        rb_or_crySetTW(2, cmdString, &twData);
    }
}


void Rb2SetTW(int seconds)
{
    char cmdString[15];
    int twData;
    memset(cmdString, 0, sizeof(cmdString));
    if (seconds < -1 || seconds > 999) {
        if (markdebug == 1) {
        	printf("the range is not correct\n");
        }
    } else {
        sprintf(cmdString, "TW%03d\r\n", seconds);
        rb_or_crySetTW(4, cmdString, &twData);
    }
}


/* Raw phase adjust routine */
STATUS_t Rb1RawPhaseAdjust(int range)
{
    char cmdString[15];
    int raData;
    memset(cmdString, 0, sizeof(cmdString));
    if (range < -1000 || range > 1000) {
        if (markdebug == 1) {
        	printf("the range is not correct, please try again\n");
        }
        return ERROR;
    } else if (range >= 0) {
        sprintf(cmdString, "RA+%03d\r\n", range);
    } else {
        sprintf(cmdString, "RA%04d\r\n", range);
    }
    rb_or_cryRawPA(2, cmdString, &raData);
    return OK;
}


STATUS_t Rb2RawPhaseAdjust(int range)
{
    char cmdString[15];
    int raData;
    memset(cmdString, 0, sizeof(cmdString));
    if (range < -129 || range > 128) {
        if (markdebug == 1) {
        	printf("the range is not correct, please try again\n");
        }
        return ERROR;
    } else if (range >= 0) {
        sprintf(cmdString, "RA+%03d\r\n", range);
    } else {
        sprintf(cmdString, "RA%04d\r\n", range);
    }
    rb_or_cryRawPA(4, cmdString, &raData);
    return OK;
}
#endif


#ifdef SERIAL_DEBUG
void gpsdebugSeq()
{
    close_gps_allOutputSentence(1);
}

#ifdef SERIAL_DEBUG

#define SEC_PER_YEAR            31536000
#define SEC_PER_DAY             86400
#define SEC_PER_HOUR            3600
#define SEC_PER_MINUTE          60
extern int gpsUpdate;
extern struct timespec gpsTime;
static int monthdaysl[12] =
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };
static int monthdaysn[12] =
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };


int gpsdebugGetUTC(int port, int module)
{
    int yy;
    int mn;
    int dy;
    int hh;
    int mm;
    int ss;
    if (module == 1) {
        getUTC_GT(&yy, &mn, &dy, &hh, &mm, &ss, port);
        gpsUpdate = 1;
        gpsTime.tv_sec = SEC_PER_YEAR * (yy - 1900) +   /* Year      */
            SEC_PER_DAY * ((yy - 1900 - 1) / 4) +       /* Leap days */
            SEC_PER_DAY *
            ((yy % 4) ? monthdaysn[mn - 1] : monthdaysl[mn - 1]) +
            /* Month     */
            SEC_PER_DAY * (dy - 1) +    /* Day       */
            SEC_PER_HOUR * hh + /* Hour      */
            SEC_PER_MINUTE * mm +       /* Minute    */
            ss;
        logMsg("GPS UTC: 0x%x\r\n", gpsTime.tv_sec, 0, 0, 0, 0, 0);
    } else if (module == 2) {
        getUTC_30(&yy, &mn, &dy, &hh, &mm, &ss, port);
    } else if (module == 3) {
        getUTC_Pico(&yy, &mn, &dy, &hh, &mm, &ss, port);
    } else {
        printf("please type the correct parameter\n");
    }
    printf("******************************************\n");
    printf("UTC is [year->%d]:[month->%d]:[date->%d] ,[%d]:[%d]:[%d]\n",
           yy, mn, dy, hh, mm, ss);
    printf("******************************************\n");
    return (0);
}
#endif
#endif


STATUS_t getUTC_Pico(int *yy, int *mn, int *dy, int *hh, int *mm,
                     int *ss, int port)
{
    serialGpsGpgga_t input;
    int err;
    if (markdebug == 1) {
    	printf("Testing getUTC_Pico(port:%d)\n", port);
    }
    err = EApicot(&input, port);
    if (err == 0 || err == 7) {
        *yy = input.utc.yy;
        *mn = input.utc.mn;
        *dy = input.utc.dy;
        *hh = input.utc.hh;
        *mm = input.utc.mm;
        *ss = input.utc.ss;
        return OK;
    } else {
        return ERROR;
    }
}


#ifdef SERIAL_DEBUG
int oscdebugstatusread()
{
    stsOscillatorEntry_t entry;
    rbcrymibStatus(&entry);
    if (markdebug == 1) {
    	printf("status is 1or2or3or4or5 :%d\n", entry.stsOscillatorStatus);
    }
    return (0);
}


int reset_close()
{
    uartSerialReset();
    close_gps_allOutputSentence(1);
    return 0;
}
#endif


/************************************************************************************
function: 
  rbcrymibStatus - Query status of the either Rubidium or Crystal device and store
  								 the value in MIBs    
paramenter:   
   stsOscillatorEntry_t *input - the oscillator mibs structure											
return:
	OK  
  ERROR					
*/
STATUS_t rbcrymibStatus(stsOscillatorEntry_t * input)
{
    int statusData;
    int port;
    if (input->stsOscillatorIndex == 1) {
        port = 2;
        rb_or_cryStatus(port, &statusData);
    } else if (input->stsOscillatorIndex == 2) {
        port = 4;
        rb_or_cryStatus(port, &statusData);
    } else {
        return ERROR;
    }
    if (statusData == 6) {
        if (oscStatus == 3) {
            input->stsOscillatorStatus =
                D_stsOscillatorStatus_holdoversync;
        }
    } else if (statusData == 1 || statusData == 2 || statusData == 3) {
        if (oscStatus == 0) {
            input->stsOscillatorStatus = D_stsOscillatorStatus_aqrsync;
        }
    } else if (statusData == 5) {
        input->stsOscillatorStatus = D_stsOscillatorStatus_frngsync;
    } else if (statusData == 3) {
        input->stsOscillatorStatus = D_stsOscillatorStatus_lkdsync;
    } else if (statusData == 9) {
        input->stsOscillatorStatus = D_stsOscillatorStatus_absent;
    } else {
        return ERROR;
    }
    oscStatus = statusData;
    return OK;
}


/*********************************************************************************************
function: 
  opendebug - let the debug mode to be opened or not
     
paramenter:
  input - 0 =>close the debug mode
        - 1 =>open the debug mode           
  												
return:
	N/A
*/
void opendebug(int input)
{
    markdebug = input;
}

/******************************************************************************
function:
    tlDataAccess - Access the refer gps position value via flash.
              
paramenter: 
    char *fname - file name storing reference gps position value.
    int mode - IO mode (R/W)
RETURN: 
   	OK    : Access sucessfully.
    ERROR : Fail to access.
    
    tlDataAccess("sposfile.dat", ESR_READ);
    tlDataAccess("sposfile.dat", ESR_WRITE);

STATUS_t tlDataAccess(stsGpsEntry_t *input, char *fname, int mode)
{
    FILE *fhd = NULL;
    char cmdstring[50];
    STATUS_t status = OK;
    if (fname == NULL) {
        return ERROR;
    }
    if (mode == ESR_READ) {
        if ((fhd = fopen(fname, "r")) == NULL)
            status = ERROR;
        else {
        	strcpy(cmdstring, input->stsGpsSPOS->octet_ptr);
            if (fread(cmdstring, sizeof(cmdstring), 40, fhd) != 40)
                status = ERROR;
            else {								
				status = OK;
				return status;
            }
        }
        fclose(fhd);
    } else if (mode == ESR_WRITE) {
        if ((fhd = fopen(fname, "w")) == NULL)
            status = ERROR;
        else {
        		strcpy(cmdstring, input->stsGpsSPOS->octet_ptr);
        }
        fwrite(cmdstring, sizeof(char), 40, fhd);
        fclose(fhd);
    }
    return status;
}*/

/******************************************************************************
function:
esrRest - the task to reset the esr
              
paramenter: 
  1: for ESR1
   2: for ESR2
 RETURNS:       
  OK
    ERROR - otherwise
              
*/
STATUS_t esrReset(int esrIndexToReset)
{
	stsGpsEntry_t *input;
    int portNum;
    int *fgExist;
    int result;
    int err;
    int *pIdx;
    if (esrIndexToReset == D_stsGpsIndex_esr1) {
        portNum = SCC1PORT;
        fgExist = &flagexist_scc1;
        pIdx = &devIdxScc1;
    } else if (esrIndexToReset == D_stsGpsIndex_esr2) {
        portNum = SCC3PORT;
        fgExist = &flagexist_scc3;
        pIdx = &devIdxScc3;
    } else {
        if (markdebug == 1) {
            printf("no Index input\n");
        }
        return ERROR;
    }
    if (*fgExist == NO_FLAG_EXIST) {
        result = gpsdevquery(portNum);
        if (result != NO_GPS_DEVICE) {
            *fgExist = FLAG_EXIST;
            gpsModule[*pIdx].gpsmodulereset(gpsModule[*pIdx].
                                          gpsmoduleparameter, portNum,
                                          input);
        } 
        else {
            if (markdebug == 1) {
                printf("no device\n");
            }
            return ERROR;
        }
    } 
    else {                    /* *fgExist ==1 */
        err =
            gpsModule[*pIdx].gpsmodulereset(gpsModule[*pIdx].
                                          gpsmoduleparameter, portNum,
                                          input);
        if (err == -2) {
            *fgExist = 0;
            return ERROR;
        }
    }
    return OK;
}
