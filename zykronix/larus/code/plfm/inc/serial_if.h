#ifndef		_SERIAL_IF_H_
#define   _SERIAL_IF_H_


 /*====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    interface to serial devices
 *
 * Note:
 *
 * Author: Klim and shuming
 *
 *====================================================================
 * $Id: serial_if.h,v 1.29 2006/10/05 06:27:50 klim Exp $
 * $Source: /cvsroot/larus5820/code/plfm/inc/serial_if.h,v $
 * $Log: serial_if.h,v $
 * Revision 1.29  2006/10/05 06:27:50  klim
 * mark the include header
 *
 * Revision 1.28  2006/10/05 03:40:11  betta
 * *** empty log message ***
 *
 * Revision 1.27  2006/10/05 01:26:22  klim
 * modify the extern name
 *
 * Revision 1.26  2006/10/04 06:32:28  klim
 * add gps reset action function
 *
 * Revision 1.25  2006/10/03 11:55:57  klim
 * structure type _t
 *
 * Revision 1.24  2006/10/03 11:23:31  betta
 * *** empty log message ***
 *
 * Revision 1.23  2006/10/03 11:14:04  betta
 * *** empty log message ***
 *
 * Revision 1.22  2006/10/03 08:31:08  cvsshuming
 * *** empty log message ***
 *
 * Revision 1.21  2006/10/02 10:08:47  klim
 * *** empty log message ***
 *
 * Revision 1.20  2006/10/02 01:32:13  klim
 * modify the structure
 *
 * Revision 1.19  2006/09/29 10:11:26  betta
 * *** empty log message ***
 *
 * Revision 1.18  2006/09/29 08:19:33  betta
 * *** empty log message ***
 *
 * Revision 1.17  2006/09/28 09:33:06  betta
 * *** empty log message ***
 *
 * Revision 1.16  2006/09/27 03:27:05  betta
 * *** empty log message ***
 *
 * Revision 1.15  2006/09/23 05:00:41  klim
 * extern the set synch & set track routine for the csm used
 *
 * Revision 1.14  2006/09/22 06:23:19  klim
 * extern the osc set tracking
 *
 * Revision 1.13  2006/09/20 04:12:59  betta
 * *** empty log message ***
 *
 * Revision 1.12  2006/09/20 01:45:39  betta
 * declare void close_gps30_allOutputSentence(int port) ;
 *
 * Revision 1.11  2006/09/13 10:53:42  betta
 * *** empty log message ***
 *
 * Revision 1.10  2006/09/13 07:59:30  klim
 * add extern osc routine
 *
 * Revision 1.9  2006/09/12 09:44:20  wilson
 * *** empty log message ***
 *
 * Revision 1.8  2006/09/04 07:35:53  betta
 * int osc1Priority, osc1Priority, and switchover are declared.
 *
 * Revision 1.7  2006/09/01 05:57:54  betta
 * gps/cdma definitions of errors were added.
 *
 * Revision 1.6  2006/08/30 03:27:22  betta
 * *** empty log message ***
 *
 * Revision 1.5  2006/08/29 09:52:41  betta
 * *** empty log message ***
 *
 * Revision 1.3  2006/08/22 02:24:02  klim
 * add the structure using for other gps module
 *
 * Revision 1.1  2006/08/03 09:51:06  cvsshuming
 * re-organize
 *
 *====================================================================
 * $Endlog $
 */

/*
#include <string.h>
#include <stdio.h>
#include <trapdef.h>
*/
#include <tss_types.h>


#include "sr_snmp.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpdefs.h"
#include "snmppart.h"
#include "snmpsupp.h"
#include "sys_snmp.h"
#include "snmptrap.h"
#define NO_SIGNAL 0
#define SIGNAL_LESS_1ST 1
#define SIGNAL_LESS_2ND 2
#define SIGNAL_LESS_3RD 3
#define SIGNAL_LESS_4TH 4
#define SIGNAL_OVER_1ST 5
#define SIGNAL_OVER_2ND 6
#define SIGNAL_OVER_3RD 7
#define SIGNAL_OVER_4TH 8
#define NO_FLAG_EXIST 0
#define FLAG_EXIST 1
#define NO_GPS_DEVICE -100
#define NO_OSC_DEVICE -100
#define GPS_FIX_INVALID 7
#define OSC_FIX_INVALID 7
#define ESR_READY_NOW 0
#define ESR_READY_LAST_TIME 1
#define ESR_NOT_READY_LAST_TIME 0
#define NO_SIG_TIMEOUT -2
#define NO_CDMA_DEVICE -1 
#define POWER_ON 1
#define POWER_OFF 0
#define ESR_READ              1
#define ESR_WRITE             2
extern SYS_CONFIG_T sysConfig;
extern unsigned devIdxScc1, devIdxScc3;
/*
 *====================================================================
 *=                        type declaration                          =
 *====================================================================
 */
 
typedef int (*CMD_STR_PARSE)();
typedef int (*RESPOND_PARSE)();

typedef enum{
  gpsDev=0,
  cdmaDev,
  sroDev,
  sxoDev
}DEV_TYPE;

typedef enum{
	mode1=1,
	mode2,
	mode3,
	mode4
}CLEAR_MODE;

typedef struct 
{
    char *         cmdName;         /*command string header */
   
    CMD_STR_PARSE  cmdParseFunc;   	/* pointer to  parse command string function*/
#if 0    
    void *         RxArg;           /* pointer to respose data sturucture */
#endif    
    RESPOND_PARSE  rxParseFunc;    /* pointer to parse respose string function*/
    DEV_TYPE       devType;        /* serial device type */
}	UART_APP_t;

extern UART_APP_t uartApp[];


/*type define - gps*/

typedef enum {
    GPS_NORTH=0,
    GPS_SOUTH,
    GPS_EAST,
    GPS_WEST
} serialGpsDirection_t;

typedef struct {
		short mn;
		short dy;
		short yy;	
    short hh;
    short mm;
    short ss;
} serialGpsUtc_t;

typedef struct {
    short degree;
    short minuate;
    short fraction;
    serialGpsDirection_t direction;
} serialGpsLatitude_t;


typedef struct {
    short degree;
    short minuate;
    short fraction;
    serialGpsDirection_t direction;
} serialGpsLongitude_t;


typedef struct {
    char idhead[6];
    serialGpsUtc_t utc;
    serialGpsLatitude_t latitude;
    serialGpsLongitude_t longitude;
    short gpsQuality;
    short n_satellite;
    char dop[5];
    double altitude;
    char alt_unit[1];
    double geo;
    char geo_unit[1];
    short dgpsDate;
    short dgpsId;
    char direction1[1];
    char direction2[1];
/* structure for the jupiter pico t */
    long fraction;
    char idhead1[1];
    char idhead2[2];
		long latitude1;
		long longitude1;
		long altitude1;      
} serialGpsGpgga_t;

typedef struct {
    char idhead[6];
    serialGpsUtc_t utc;
    short zonehr;
    short zonemm;     
} serialGpsGpzda_t;

/* structure for the jupiter pico t */

typedef struct {
    char idhead1[1];
    char idhead2[2];	
		long time_offset;
} serialGpsPicoANTD_t;

typedef struct {
    char idhead1[1];
    char idhead2[2];	
    short nosatellite;
    short satID1;
    short dopHz1;
    short ele1;
    short azi1;
    short sathealth1;
    short satID2;
    short dopHz2;
    short ele2;
    short azi2;
    short sathealth2;
    short satID3;
    short dopHz3;
    short ele3;
    short azi3;
    short sathealth3;
    short satID4;
    short dopHz4;
    short ele4;
    short azi4;
    short sathealth4;        		
    short satID5;
    short dopHz5;
    short ele5;
    short azi5;
    short sathealth5;
    short satID6;
    short dopHz6;
    short ele6;
    short azi6;
    short sathealth6;       
    short satID7;
    short dopHz7;
    short ele7;
    short azi7;
    short sathealth7;   
    short satID8;
    short dopHz8;
    short ele8;
    short azi8;
    short sathealth8;    
    short satID9;
    short dopHz9;
    short ele9;
    short azi9;
    short sathealth9;    
    short satID10;
    short dopHz10;
    short ele10;
    short azi10;
    short sathealth10;    
    short satID11;
    short dopHz11;
    short ele11;
    short azi11;
    short sathealth11;    
    short satID12;
    short dopHz12;
    short ele12;
    short azi12;
    short sathealth12;    
} serialGpsPicoAZEL_t;

typedef struct {
    char idhead[6];
    short totalnomssg;
    short nomssg;
    short nosatellite;
    short ssv1;
    short sea1;
    short sba1;
    short ssnr1;
    short ssv2;
    short sea2;
    short sba2;
    short ssnr2;
    short ssv3;
    short sea3;
    short sba3;
    short ssnr3;
    short ssv4;
    short sea4;
    short sba4;
    short ssnr4;            
} serialGpsGpgsv_t;

/* end of structure for the jupiter pico t */

typedef struct {
    char idhead[6];
    serialGpsLatitude_t latitude;
    char direction1[1];        
    serialGpsLongitude_t longitude;
    char direction2[1];
    serialGpsUtc_t utc;
    char status[1];           
} serialGpsGpgll_t;

typedef struct {
    char idhead[6];
		short sig_strength;
		short snr;
		double be_freq;
		double be_bitrate;
		short channer_no;         
} serialGpsGpmss_t;

/* set initial position GT8031 */
typedef struct {
    serialGpsLatitude_t latitude;
    serialGpsLongitude_t longitude;
    char empty1[1];
    char empty2[1];
    char empty3[1];
    char empty4[1];
    char empty5[1];
    char empty6[1];
    char empty7[1];
    char empty8[1];
    char empty9[1];
    double lat;
    double longitudef; 
    char directNS[1];
    char directEW[1];          
} serialGpsXXGGA_t;

/* set initial position jupiter 30 */
typedef struct {
    serialGpsLatitude_t latitude;
    serialGpsLongitude_t longitude;
    char empty1[1];
    char empty2[1];
    char empty3[1];
    char empty4[1];
    char empty5[1];
    char empty6[1];
    char empty7[1];
    char empty8[1];
    char empty9[1];
    double lat;
    double longitudef;
    double alt; 
    char directNS[1];
    char directEW[1];
    short reset;          
} serialGpsSPOS30_t;

/* for GPS SBAS satellite information */
typedef struct {
    char comname[5];
    short GEO_num;
    short GEO_1;
    short Ele_1;
    short Azi_1;
    short SNR_1;
    short GEO_2;
    short Ele_2;
    short Azi_2;
    short SNR_2; 
    short GEO_3;
    short Ele_3;
    short Azi_3;
    short SNR_3;
    short GEO_4;
    short Ele_4;
    short Azi_4;
    short SNR_4;      
} serialGpsGpwav_t;


/* for GPS self-testing response (tst)*/ 

typedef struct {
    long prog_n;
    short ver_n;
} serialGpsProgVerN_t;

typedef struct {
    char idhead[6];
    serialGpsProgVerN_t pvnum;
    char comname[5];
    short status;
    short st_result1;
    short st_result2;
} serialGpsGptst_t;

typedef struct {
    short hh;
    short mm;
    short ss;
} serialCDMAUtc_t;

/* restart gps */
typedef struct {
    char comname[5];
    CLEAR_MODE mode2;
} serialGpsGpclr_t;

/* type define - CDMA */
typedef struct {
    short timeFig;
    short year;
    short day;
    serialCDMAUtc_t utc;
    char sign;
    short offset;
    char tmode;
} serialCDMAtime_t;


/* type define - Rubidium\Crystal */
typedef struct {
    char idhead[6];
    short aaa;
    short rr;
    float sss;
} serialID_t;

typedef struct {
    long ddddddddd;
    short sppp;
    double d9_sppp;
    short status;
    short hh;
    short mm;
    short ss;
    short s;
    short yyyy;
    short mn;
    short dd;
} serialBS_t;

/* gps function pointer  void *, int*/
typedef int (*OSC_QUERY)();
typedef int (*OSC_GET)(void *, int, stsOscillatorEntry_t *entry);
typedef int (*OSC_SET)(void *, int, stsOscillatorEntry_t *entry);
/* the parameter need to get & set */
struct osmodule1
{
		serialID_t				  idData;
		serialBS_t					bsData;
		DEV_TYPE 						rb_CryDev;		
}osc1getset_t;

struct osmodule2
{
		serialID_t				  idData;
		serialBS_t					bsData;
		DEV_TYPE 						rb_CryDev;						
}osc2getset_t;

typedef struct 
{
    OSC_QUERY         oscmodulequery;         /* pointer to osc query function */   
    OSC_GET  					oscmoduleget;   				/* pointer to osc get function */  
    OSC_SET         	oscmoduleset;           /* pointer to osc set function */  
    void * 				oscmoduleparameter;    /* pointer to osc parameter */  		
}	oscDevOperation_t;

extern oscDevOperation_t oscModule[];

/* gps function pointer  void *, int*/
typedef int (*GPS_QUERY)();
typedef int (*GPS_GET)(void *, int, stsGpsEntry_t *entry);
typedef int (*GPS_SET)(void *, int, stsGpsEntry_t *entry);
typedef int (*GPS_RESET)(void *, int, stsGpsEntry_t *entry);
/* the parameter need to get & set */
struct module1
{
		serialGpsGpwav_t				  gpwav;
		serialGpsGptst_t					gptst;
		serialGpsGpgga_t					gpgga;
		serialGpsGpgsv_t					gpgsv;
		serialGpsXXGGA_t          xxgga;
		serialGpsGpclr_t          gpclr;
}gpsGT8031getset_t;

struct module2
{
		serialGpsGpgll_t					gpgll;
		serialGpsGpmss_t					gpmss;
		serialGpsGpgga_t					gpgga;
		serialGpsGpgsv_t					gpgsv;	
		serialGpsGpclr_t          gpclr;		
		serialGpsSPOS30_t         gprst;		
}gps30getset_t;

struct module3
{
		serialGpsPicoANTD_t				az;
		serialGpsPicoAZEL_t				bb;
		serialGpsGpgga_t					gpgga;				
}gpsPicogetset_t;

typedef struct 
{
    GPS_QUERY           gpsmodulequery;         /* pointer to gps query function */   
    GPS_GET  			gpsmoduleget;   		/* pointer to gps get function */  
    GPS_SET         	gpsmoduleset;           /* pointer to gps set function */ 
    GPS_RESET 			gpsmodulereset;         /*pointer to gps reset function */ 
    void * 				gpsmoduleparameter;     /* pointer to gps parameter */  		
}	gpsDevOperation_t;

extern gpsDevOperation_t gpsModule[];



/* structure for querying uart device */

typedef int (*QUERY_FUNCTION)();

enum{
  SCC1PORT=1,
  SMC1PORT,
  SCC3PORT,
  SCC4PORT
};

typedef enum {
    port1 = 0,
    port2,
    port3,
    port4,
} port_t;


typedef struct
{

    int queryResult;   	       /* storing the result of querying device currently*/
    int flagWhetherDevReady;    /* flag indicates whether worked normally last time*/  
    int DelayTime;        /* querying frequency */ 
    DEV_TYPE devType;     /* serial device type */
    int        portNum;         /* uart port */
    int   formerStatus;      /* for storing the former stsGpsStatus/stsOscillatorStatus. */      
    int   stsDevIndex;            /* ex: stsGpsIndex/stsOscillatorIndex */
}	QUERY_APP_t;



/* restart gps jupiter pico t */
typedef struct {
    char idhead1[1];
    char idhead2[2];
} serialGpsPicoreset_t;

/*
 *====================================================================
 *=                        func declaration                          =
 *====================================================================
 */

extern STATUS_t gpsdevicequery(int portNum);
extern STATUS_t oscdevicequery(int portNum);
extern STATUS_t gpsGet(stsGpsEntry_t *input);
extern STATUS_t gpsSet(stsGpsEntry_t *input);
extern STATUS_t esrReset(int esrIndexToReset);

extern STATUS_t oscGet(stsOscillatorEntry_t *input);
extern STATUS_t oscSet(stsOscillatorEntry_t *input);

extern STATUS_t getUTC_GT(int *yy, int *mn, int *dy, int *hh, int *mm, int *ss, int port);
extern STATUS_t getUTC_30(int *yy, int *mn, int *dy, int *hh, int *mm, int *ss, int port);
extern STATUS_t getUTC_Pico(int *yy, int *mn, int *dy, int *hh, int *mm, int *ss, int port);

extern STATUS_t oscreadactive(int activeptr);
extern STATUS_t rbcrymibStatus(stsOscillatorEntry_t *input);
extern int queryRb_Cry(int port , int *rb_cryDelay, int *rb_cryStaData, DEV_TYPE rb_CryDev);
extern int queryCDMA(int port ,int *cdmaDelay);
extern int queryGPS(int port ,int *gpsDelay);
extern int gpsdevquery(int portNum);
extern int rb_or_crySetTrack(int port, int *trackStatus);
extern int rb_or_crySetSync(int port, int *syncStatus);
extern int cdmaCTIME(int port);
extern void close_gps_allOutputSentence(int port);
extern void close_gps30_allOutputSentence(int port);
/* API enable OSC via GPIO pins */
extern void oscNormal();
extern void osc1PowerOnOff(int on_off);
extern void osc2PowerOnOff(int on_off);

/* valuable shared by osc and esr */

extern int osc1Priority, osc2Priority;
extern int switchover;
/* API to set/get redundancy Status */
extern void oscSetSwitchState(int oscIndex ,int oscSwitchState);
extern int oscGetSwitchState(int oscIndex);
extern int oscGetRedStatus(int oscIndex);
/* API to get UTC information */
extern STATUS_t getUTC_GT(int *yy, int *mn, int *dy, int *hh, int *mm, int *ss, int port);
extern STATUS_t getUTC_30(int *yy, int *mn, int *dy, int *hh, int *mm, int *ss, int port);
typedef STATUS_t (*GET_UTC_FUNC)(int *yy, int *mn, int *dy, int *hh, int *mm, int *ss, int port);

#ifdef SERIAL_DEBUG
		/* API scc API */
		extern void sccSetBaudRate9600(int port);
		extern void sccSetBaudRate4800(int port);
		extern void test_SelfTestResult();
		extern void scc1RbID();
		extern void scc3RbID();
		extern void scc1RbStatus();
		extern void scc3RbStatus();
		extern void Scc1SetTrack();
		extern void Scc3SetTrack();
		extern void scc1SetSync();
		extern void scc3SetSync();
		extern void scc1RbReset();
		extern void scc3RbReset();
#endif



#endif