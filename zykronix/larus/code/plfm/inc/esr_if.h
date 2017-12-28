#ifndef	  _ESR_IF_H_
#define   _ESR_IF_H_


 /*====================================================================
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *    interface to GPS devices
 *
 * Note:
 *
 * Author: Betta
 *
 *====================================================================
 */


/*
 *====================================================================
 *=                        type declaration                          =
 *====================================================================
 */
          /* declaration of routine */
extern STATUS_t esrTask(void);
extern STATUS_t antGet(stsAntEntry_t *antEntry);
extern STATUS_t antSet(stsAntEntry_t *antEntry);

          /* Globle valuables */
          
/* gps_cdmaAPP[0] :  information for GPS connected to SCC1.
   gps_cdmaAPP[1] :  information for GPS connected to SCC3.
   gps_cdmaAPP[2] :  information for CDMA connected to SCC1.
   gps_cdmaAPP[3] :  information for CDMA connected to SCC3.
*/
extern QUERY_APP_t gps_cdmaAPP[4];

#endif