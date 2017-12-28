#include <axss_types.h>
#include <ds26502.h>

#ifndef PM_H
#define PM_H

#define ONE_SECOND_TICK       100
#define TICK_3                3
#define TICK_4                4
#define FRAMER_1              0
#define FRAMER_2              1
#define FRAMER_3              2
#define FRAMER_4              3
#define FRAMER_5              4
#define FRAMER_ALL            5
#define RAW_PHASE_BUF         32
#define DEFAULT_PM            1
#define MANUAL_PM             2
#define MANUAL_PM_OFF         0
#define MANUAL_PM_ON          1
#define PM_READY              2
#define MANUAL_PM_ABORT       2

#define ON                    1
#define OFF                   0

#define E1                    0
#define T1                    1
#define SQW                   2

#define MAX_COARSE            32768           /*2^(16-1) coarse phase data */
#define negMAX_COARSE         (-MAX_COARSE)   /*2^(8-1)   */
#define TIE_DEFAULT           6948           /* default 503 */
#define MTIE_DEFAULT          6948			 /* default 503 */
#define JITTER_DEFAULT        5
#define SLIP_DEFAULT          193
#define UI_Nsec               488
#define CS_DEFAULT            1
#define BPV_DEFAULT           16
#define CRC_DEFAULT           16
#define Bucket_100            37
#define Bucket_Hour           25
#define Bucket_Day            8
#define SEC100_PM             1
#define HOURLY_PM             2
#define DAILY_PM              3
#define PM_LINK_TYPE          8
#define PM_CRITICAL           1
#define PM_MAJOR              2
#define PM_MINOR              3
#define PM_NON_ALARM          4
#define PM_NON_REPORT         5
#define DATESTAMP_SIZE        11
#define TIMESTAMP_SIZE        9
#define LINK_TYPES            5

#define SHORT_DELAY			  sysMsDelay(1)

#define SMU_FPGA_BASE_ADDR    0x43000000


union u_ulong
{
   U32_t ul;
   struct
   {
      U8_t hh;
      U8_t hm;
      U8_t lm;
      U8_t ll;
   } ch;
};

union u_long
{
   S32_t l;
   struct
   {
      U8_t hh;
      U8_t hm;
      U8_t lm;
      U8_t ll;
   } ch;
};

union u_int
{
   S16_t i;
   struct
   {
      U8_t h;
      U8_t l;
   } ch;
};

enum
{
    T_BPV = 0,                  /* index for threshold data array */
    T_CRC,
    T_CS,
	T_ESP,
    T_JITTER,
	T_SESP,
    T_TIE,
    T_MTIE,
    THRESHOLD_MAX
};

enum
{
   /* data for timePeriod in DS1Error */
   ONE_SECOND,
   TEN_SECONDS,
   ONE_HOUR,
   ONE_DAY
};

enum
{
   /* data for timePeriod in DS1Error */
   PM_ONE_HOUR = 1,
   PM_ONE_DAY,
   PM_ONE_HUNDRED_SECOND
};

enum
{
   ONE_SECOND_INTERVAL = 1,
   TEN_SECOND_INTERVAL,
   ONE_HUNDRED_SECOND_INTERVAL,
   ONE_THOUSAND_SECOND_INTERVAL,
   TEN_THOUSAND_SECOND_INTERVAL,
   ONE_HUNDRED_THOUDSAND_SECOND_INTERVAL
};

enum
{
   CVL_THRESHOLD = 1,
   CVP_THRESHOLD,
   CS_THRESHOLD,
   JITTER_THRESHOLD,
   TIE_THRESHOLD,
   MTIE_THRESHOLD,
   WANDER_THRESHOLD
};

enum
{
   ACT_SEVERITY = 1,
   LOS_SEVERITY,
   AISP_SEVERITY,
   LOF_SEVERITY,
   CVL_SEVERITY,
   CVP_SEVERITY,
   CS_SEVERITY,
   JITTER_SEVERITY,
   TIE_SEVERITY,
   MTIE_SEVERITY,
   WANDER_SEVERITY
};

typedef struct {
   U32_t      eBPV;
   U32_t      eCRC;
   U32_t      eESP;
   U32_t      eSESP;
   S64_t      eCS;
   U32_t      eJITTER;
   S32_t      eTIE;
   S32_t      eMTIE;
#if 0
   U32_t      eWANDER;
#endif
   S32_t      clkType;
   STATUS_t   validity;
} PMPara_t;
   
typedef struct {
   U8_t     secElapsed;
   U8_t     sec100Elapsed;
   U8_t     hourElapsed;
   U8_t     TIETime;
   PMPara_t sec_100[Bucket_100];  /* 100 Seconds PM Bucket */
   PMPara_t hour[Bucket_Hour];	  /* Hourly PM Bucket */
   PMPara_t day[Bucket_Day];      /* Daily PM Bucket */
} PMRec_t;

typedef struct {
  U32_t  jitterThreshold;
  U32_t  bpvThreshold;
  U32_t  crcThreshold;
  U32_t  mtieThreshold;
  U32_t  tieThreshold;
  U32_t  csThreshold;
  U32_t  wanderThreshold;
  S8_t   tiePeriod;
  S8_t   mtiePeriod;
  S8_t   csPeriod;
  S8_t   jitterPeriod;
  S8_t   crcPeriod;
  S8_t   bpvPeriod;
  S8_t   wander5Period;
  S8_t   wander24Period;
} PM_THRESHOLD_t;

typedef struct {
  U8_t   act;
  U8_t   los;
  U8_t   aisp;
  U8_t   lof;
  U8_t   cvl;
  U8_t   cvp;
  U8_t   cs;
  U8_t   jitter;
  U8_t   tie;
  U8_t   mtie;
  U8_t   wander;
} PM_SEVERITY_t;
  
typedef struct {
  S8_t           status;
  S8_t           jitterStatus;
  S8_t           channelType;
  S8_t			 spare1;

  /* PM Alarm Threshold */
  S8_t           gv_TIEPeriod;
  S8_t           gv_MTIEPeriod;
  S8_t           gv_CSPeriod;
  S8_t           gv_JITTERPeriod;
  S8_t           gv_CRCPeriod;
  S8_t           gv_BPVPeriod;
  U32_t          jitterThreshold;
  U32_t          bpvThreshold;
  U32_t          crcThreshold;
  U32_t          mtieThreshold;
  U32_t          tieThreshold;
  U32_t          csThreshold;
  U32_t          wanderThreshold;

  /* PM Data Buckets */
  PMRec_t    	 pmRec;

  /* Alarms */
  S32_t          accumCoarseDiff;
  S32_t          alarmBPVcount;
  S32_t          alarmCRCcount;
  S32_t          alarmCScount;
  S32_t          alarmMTIEmin;
  S32_t          alarmMTIEmax;
  S32_t          alarmTIEend;
  STATUS_t       jitterAlarm;
  STATUS_t       tieAlarm;
  STATUS_t       mtieAlarm;
  STATUS_t       csAlarm;
  STATUS_t       bpvAlarm;
  STATUS_t       crcAlarm;
  STATUS_t       fsAlarm;

  /* Misc. variables   */
  union u_long   tieStartLongValue, tieEndLongValue;
  S32_t          oneSecondJitter;
  S32_t          pmJitter;
  U32_t          maxMTIE, minMTIE;
  S32_t          gv_TIEData;
  S32_t          tiePM;
} PM_FRAMER_t;

typedef struct {
  S8_t         interval;
  S8_t         status;
  S8_t         loopCount;
  S8_t         t10;
  S8_t         t100;
  S8_t         t1000;
  S8_t         t10000;
  S8_t         t100000;
  U16_t        timeElapsed;
  U16_t        rawPhaseIndex;
  U32_t        rawPhaseBuf[RAW_PHASE_BUF];
  S32_t        alarmBPVcount;
  S32_t        alarmCRCcount;

  U32_t        eBPV;
  U32_t        eCRC;
  U32_t        eESP;
  U32_t        eSESP;
  S32_t        eCS;
  U32_t        eJITTER;
  S32_t        eTIE;
  S32_t        eMTIE;
  float        eWANDER5;
  float        eWANDER24;

  union u_long tieStartLongValue, tieEndLongValue;
  U32_t        maxMTIE, minMTIE;
  S32_t        tiePM;
} PM_MANUAL_t;

extern PM_FRAMER_t   pmFramer[MAX_FRAMERS];
extern PM_SEVERITY_t pmSeverity[LINK_TYPES];
extern TE1_CONFIG_t  te1Cfg[MAX_FRAMERS];
extern INP_FRAMER_t  inpFramer[MAX_FRAMERS];
extern PM_MANUAL_t   pmManual[MAX_FRAMERS];
extern U32_t         stopTIE_Man[MAX_FRAMERS];
extern U32_t         getTIE_Man[MAX_FRAMERS];
extern U32_t         stopMTIE_Man[MAX_FRAMERS];
extern U32_t         getMTIEMax_Man[MAX_FRAMERS];
extern U32_t         getMTIEMin_Man[MAX_FRAMERS];
extern U32_t         stopJITTER_Man[MAX_FRAMERS];
extern U32_t         getJITTERMax_Man[MAX_FRAMERS];
extern U32_t         getJITTERMin_Man[MAX_FRAMERS];
extern S32_t         manSmuTestSetFlag;
extern U8_t          manSelect;
extern S32_t         manScanInterval;
extern S32_t         manNumReport;
extern S32_t         manNumObservation;

extern STATUS_t startSmuManualPM();
extern STATUS_t abortSmuManualPM(void);
extern void     readTIEData(int ch, char mode);
extern S32_t    readJitterData(int ch, char mode);
extern void     readMTIEData(int ch, char mode);
extern void     procTIEAlarm(int ch);
extern void     procMTIEAlarm(int ch);
extern void     procCSAlarm(int ch);
extern void     updateCodeThresholdAlarm(int ch);
extern void     sysMsDelay(S32_t);

#endif /* PM_H */
