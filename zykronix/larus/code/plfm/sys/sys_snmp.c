/*
 * Copyright (C) 2006, All Rights Reserved, by
 * Ahoya Networks, Inc.
 *
 * Abstract:
 *
 *    This file contains the Card Manager Task
 *
 * Author(s):
 *
 */

#include <stdio.h>
#include <string.h>
#include <ioLib.h>
#include <dirent.h>
#include <stat.h>
#include <ifLib.h>
#include <stdlib.h>
#include <routeLib.h>
#include <dhcpcLib.h>
#include <ipProto.h>
#include <dhcp\dhcpcShow.h>

#include "sr_conf.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif /* HAVE_MEMORY_H */
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif /* HAVE_STDDEF_H */
#include "sr_snmp.h"
#include "sr_trans.h"
#include "comunity.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "lookup.h"
#include "v2table.h"
#include "min_v.h"
#include "mibout.h"
#include "axss_types.h" /* joehsu */

#include "snmpsupp.h"
#include "snmpdefs.h"
#include "sys_snmp.h"
#include "sys_dir.h"
#include "rtc_if.h"
#include "smc8260Sio.h"
#include "drv\sio\m8260Sio.h"
#include "axss_types.h"
#include "sys_if.h"

FILE  *sysConfigFp;
FILE  *termConfigFp;

SYS_CONFIG_T  sysConfig;
TERM_CONFIG_T termConfig;
void * sysDhcpCookie1 ;			
void * sysDhcpCookie2 ;

int sysSnmpDbg = 0;			

/*
 *====================================================================
 *=                            include                               =
 *====================================================================
 */

/*
 *====================================================================
 *=                             definition                           =
 *====================================================================
 */
extern U32_t    menuSelected;
extern int		consoleFd;		/* fd of initial console device */

extern int      rtc_write_date(int dt, int mn, int yr, int ct);
extern int      rtc_write_time(int hh, int mm, int ss);

STATUS_t        initSysDhcp();


/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */

STATUS_t systemActionFuncSet(stsSystem_t * data)
{
    int       len;
	char      tmp[100];
	STATUS_t  status;

    memset(tmp, 0, sizeof(tmp));

    sysConfig.stsSystemId.octet_ptr                  =  (unsigned char *)&(sysConfig.stsSystemIdBody);
    sysConfig.stsSystemDate.octet_ptr                =  (unsigned char *)&(sysConfig.stsSystemDateBody);
    sysConfig.stsSystemTime.octet_ptr                =  (unsigned char *)&(sysConfig.stsSystemTimeBody);
    sysConfig.stsSystemCurrentSWVersion.octet_ptr    =  (unsigned char *)&(sysConfig.stsSystemCurrentSWVersionBody);

    if(sysSnmpDbg)
        printf("systemActionFuncSet\r\n");

    if (VALID(I_stsSystemId, data->valid)) 
    {
	    if(data->stsSystemId->length >= sizeof(sysConfig.stsSystemIdBody))
		{
		    printf("stsSystemId over length: %d\n", data->stsSystemId->length);
	        return (GEN_ERROR);
		}
	         
	    memset(sysConfig.stsSystemIdBody, '\0', sizeof(sysConfig.stsSystemIdBody));

		sysConfig.stsSystemId.length = data->stsSystemId->length;
        strncpy(sysConfig.stsSystemId.octet_ptr, 
                data->stsSystemId->octet_ptr,
                data->stsSystemId->length);

    }

    if (VALID(I_stsSystemMgmtIpOption, data->valid))
    { 
        sysConfig.stsSystemMgmtIpOption = data->stsSystemMgmtIpOption; 
        if(sysConfig.stsSystemMgmtIpOption == D_stsSystemMgmtIpOption_dhcp)
	    {
		    if(sysSnmpDbg)
	            printf("MGMT IP: DHCP Mode\r\n");

	        if((status = dhcpcBind( sysDhcpCookie1, 1)) != OK)
	        {
		        printf("dhcp Bind Failed\r\n");
	        } 
	    }

    }

    if (VALID(I_stsSystemMgmtIpAddress, data->valid))
    { 
        sysConfig.stsSystemMgmtIpAddress = data->stsSystemMgmtIpAddress;
		if(sysConfig.stsSystemMgmtIpOption != D_stsSystemMgmtIpOption_dhcp)
		{
            memset(tmp, 0, sizeof(tmp));         
            sprintf(tmp, "%d.%d.%d.%d",  (sysConfig.stsSystemMgmtIpAddress >> 24) & 0xFF,
			  					         (sysConfig.stsSystemMgmtIpAddress >> 16) & 0xFF,
								         (sysConfig.stsSystemMgmtIpAddress >> 8) & 0xFF,
								         (sysConfig.stsSystemMgmtIpAddress) & 0xFF);
            ifAddrSet("motfcc0", tmp);
		}
    }

    if (VALID(I_stsSystemMgmtIpSubnetMask, data->valid))
    { 
        sysConfig.stsSystemMgmtIpSubnetMask = data->stsSystemMgmtIpSubnetMask;
		if(sysConfig.stsSystemMgmtIpOption != D_stsSystemMgmtIpOption_dhcp)
		{
            ifMaskSet("motfcc0", sysConfig.stsSystemMgmtIpSubnetMask);         
		}
    }

    if (VALID(I_stsSystemMgmtIpGatewayAddress, data->valid))
    {
        memset(tmp, 0, sizeof(tmp));
        sysConfig.stsSystemMgmtIpGatewayAddress = data->stsSystemMgmtIpGatewayAddress; 
		sprintf(tmp, "%d.%d.%d.%d", ((sysConfig.stsSystemMgmtIpGatewayAddress & 0xFF000000) >> 24),
		                                 ((sysConfig.stsSystemMgmtIpGatewayAddress & 0x00FF0000) >> 16),
										 ((sysConfig.stsSystemMgmtIpGatewayAddress & 0x0000FF00) >> 8),
										 (sysConfig.stsSystemMgmtIpGatewayAddress & 0x000000FF));
		routeDelete ("0", tmp);
		routeAdd ("0", tmp); 
    }

    if (VALID(I_stsSystemNtpIpOption, data->valid))
    { 
        sysConfig.stsSystemNtpIpOption = data->stsSystemNtpIpOption; 
        if(sysConfig.stsSystemNtpIpOption == D_stsSystemNtpIpOption_dhcp)
	    {
		    if(sysSnmpDbg)
	            printf("NTP IP: DHCP Mode\r\n");

	        if((status = dhcpcBind( sysDhcpCookie2, 1)) != OK)
	        {
		        printf("dhcp Bind Failed\r\n");
	        } 
	    }

    }

    if (VALID(I_stsSystemNtpIpAddress, data->valid))
    { 
        sysConfig.stsSystemNtpIpAddress = data->stsSystemNtpIpAddress;
        if(sysConfig.stsSystemNtpIpOption != D_stsSystemNtpIpOption_dhcp)
	    {
            memset(tmp, 0, sizeof(tmp));         
            sprintf(tmp, "%d.%d.%d.%d",  (sysConfig.stsSystemNtpIpAddress >> 24) & 0xFF,
								         (sysConfig.stsSystemNtpIpAddress >> 16) & 0xFF,
								         (sysConfig.stsSystemNtpIpAddress >> 8) & 0xFF,
								         (sysConfig.stsSystemNtpIpAddress) & 0xFF);
            ifAddrSet("motfcc1", tmp);         
		}
    }

    if (VALID(I_stsSystemNtpIpSubnetMask, data->valid))
    { 
        sysConfig.stsSystemNtpIpSubnetMask = data->stsSystemNtpIpSubnetMask;
        if(sysConfig.stsSystemNtpIpOption != D_stsSystemNtpIpOption_dhcp)
	    {
            ifMaskSet("motfcc1", sysConfig.stsSystemMgmtIpSubnetMask);                  
		}
    }

    if (VALID(I_stsSystemNtpIpGatewayAddress, data->valid))
    { 
        sysConfig.stsSystemNtpIpGatewayAddress = data->stsSystemNtpIpGatewayAddress; 
    }

    if (VALID(I_stsSystemDate, data->valid)) 
    {
	    if(data->stsSystemDate->length >= sizeof(sysConfig.stsSystemDateBody))
		{
		    printf("stsSystemDate over length: %d\n", data->stsSystemDate->length);
	        return (GEN_ERROR);
		}
	         
	    memset(sysConfig.stsSystemDateBody, '\0', sizeof(sysConfig.stsSystemDateBody));
		sysConfig.stsSystemDate.length = data->stsSystemDate->length;
        strncpy(sysConfig.stsSystemDate.octet_ptr, 
                data->stsSystemDate->octet_ptr,
                data->stsSystemDate->length);
		if(sysSnmpDbg)
		    printf("%d,%d,%d,%d,%d,%d\r\n", (sysConfig.stsSystemDate.octet_ptr[0] - '0')*10, (sysConfig.stsSystemDate.octet_ptr[1] - '0'),
		                                    (sysConfig.stsSystemDate.octet_ptr[2] - '0')*10, (sysConfig.stsSystemDate.octet_ptr[3] - '0'),
		                                    (sysConfig.stsSystemDate.octet_ptr[4] - '0')*10, (sysConfig.stsSystemDate.octet_ptr[5] - '0'));

		rtc_write_date((sysConfig.stsSystemDate.octet_ptr[4] - '0')*10 + (sysConfig.stsSystemDate.octet_ptr[5] - '0'),
		               (sysConfig.stsSystemDate.octet_ptr[2] - '0')*10 + (sysConfig.stsSystemDate.octet_ptr[3] - '0'),
		               (sysConfig.stsSystemDate.octet_ptr[0] - '0')*10 + (sysConfig.stsSystemDate.octet_ptr[1] - '0'),
		               20);
		sysClkInit();
    }

    if (VALID(I_stsSystemTime, data->valid)) 
    {
	    if(data->stsSystemTime->length >= sizeof(sysConfig.stsSystemTimeBody))
		{
		    printf("stsSystemTime over length: %d\n", data->stsSystemTime->length);
	        return (GEN_ERROR);
		}
	         
	    memset(sysConfig.stsSystemTimeBody, '\0', sizeof(sysConfig.stsSystemTimeBody));
		sysConfig.stsSystemTime.length = data->stsSystemTime->length;
        strncpy(sysConfig.stsSystemTime.octet_ptr, 
                data->stsSystemTime->octet_ptr,
                data->stsSystemTime->length);
		if(sysSnmpDbg)
		    printf("%d,%d,%d,%d,%d,%d\r\n", (sysConfig.stsSystemTime.octet_ptr[0] - '0')*10, (sysConfig.stsSystemTime.octet_ptr[1] - '0'),
		                                    (sysConfig.stsSystemTime.octet_ptr[2] - '0')*10, (sysConfig.stsSystemTime.octet_ptr[3] - '0'),
		                                    (sysConfig.stsSystemTime.octet_ptr[4] - '0')*10, (sysConfig.stsSystemTime.octet_ptr[5] - '0'));
		rtc_write_time((sysConfig.stsSystemTime.octet_ptr[0] - '0')*10 + (sysConfig.stsSystemTime.octet_ptr[1] - '0'),
		               (sysConfig.stsSystemTime.octet_ptr[2] - '0')*10 + (sysConfig.stsSystemTime.octet_ptr[3] - '0'),
		               (sysConfig.stsSystemTime.octet_ptr[4] - '0')*10 + (sysConfig.stsSystemTime.octet_ptr[5] - '0'));
		sysClkInit();
    }

    if (VALID(I_stsSystemTimeZone, data->valid))
    { 
        sysConfig.stsSystemTimeZone = data->stsSystemTimeZone; 
    }

    if (VALID(I_stsSystemAlarmAcoMode, data->valid))
    { 
        sysConfig.stsSystemAlarmAcoMode = data->stsSystemAlarmAcoMode; 
    }

    if (VALID(I_stsSystemAlarmAcoCutOff, data->valid))
    { 
        sysConfig.stsSystemAlarmAcoCutOff = data->stsSystemAlarmAcoCutOff; 
    }

    if (VALID(I_stsSystemAlarmEscalatingTime, data->valid))
    { 
        sysConfig.stsSystemAlarmEscalatingTime = data->stsSystemAlarmEscalatingTime; 
    }

    if (VALID(I_stsSystemUpgradeSWAction, data->valid))
    { 
    }

    if (VALID(I_stsSystemReset, data->valid))
    { 
    }


    if((sysConfigFp = fopen("C:config/sysConfig.bin", "wb")) == NULL)
	{
	    printf("System Config Open Failed\r\n");
	}
	len = fwrite((FUNCPTR) (&sysConfig), sizeof(SYS_CONFIG_T), 1, sysConfigFp);
	if(sysSnmpDbg)
	    printf("Read sysFw Image: len %d\r\n", len);

    fclose(sysConfigFp);
	return OK;

}

STATUS_t systemActionFuncGet(stsSystem_t * pstsSystem)
{
    serialRtc_t   rtc;

    sysConfig.stsSystemId.octet_ptr                  =  (unsigned char *)&(sysConfig.stsSystemIdBody);
    sysConfig.stsSystemDate.octet_ptr                =  (unsigned char *)&(sysConfig.stsSystemDateBody);
    sysConfig.stsSystemTime.octet_ptr                =  (unsigned char *)&(sysConfig.stsSystemTimeBody);
    sysConfig.stsSystemCurrentSWVersion.octet_ptr    =  (unsigned char *)&(sysConfig.stsSystemCurrentSWVersionBody);

    rtc_read(&rtc);
	sysConfig.stsSystemTime.length = 6;
    sysConfig.stsSystemTime.octet_ptr[0]  = rtc.hh  / 10;
    sysConfig.stsSystemTime.octet_ptr[1]  = rtc.hh  % 10;
    sysConfig.stsSystemTime.octet_ptr[2]  = rtc.mm  / 10;
    sysConfig.stsSystemTime.octet_ptr[3]  = rtc.mm  % 10;
    sysConfig.stsSystemTime.octet_ptr[4]  = rtc.ss  / 10;
    sysConfig.stsSystemTime.octet_ptr[5]  = rtc.ss  % 10;

	sysConfig.stsSystemDate.length = 6;
    sysConfig.stsSystemDate.octet_ptr[0]  = rtc.year  / 10;
    sysConfig.stsSystemDate.octet_ptr[1]  = rtc.year  % 10;
    sysConfig.stsSystemDate.octet_ptr[2]  = rtc.month / 10;
    sysConfig.stsSystemDate.octet_ptr[3]  = rtc.month % 10;
    sysConfig.stsSystemDate.octet_ptr[4]  = rtc.date  / 10;
    sysConfig.stsSystemDate.octet_ptr[5]  = rtc.date  % 10;

    if(sysSnmpDbg)
        printf("current RTC time: %d%d/%d%d/%d%d :  %d%d/%d%d/%d%d\r\n",
                    sysConfig.stsSystemDate.octet_ptr[0], sysConfig.stsSystemDate.octet_ptr[1],
                    sysConfig.stsSystemDate.octet_ptr[2], sysConfig.stsSystemDate.octet_ptr[3],
                    sysConfig.stsSystemDate.octet_ptr[4], sysConfig.stsSystemDate.octet_ptr[5],

                    sysConfig.stsSystemTime.octet_ptr[0], sysConfig.stsSystemTime.octet_ptr[1],
                    sysConfig.stsSystemTime.octet_ptr[2], sysConfig.stsSystemTime.octet_ptr[3],
                    sysConfig.stsSystemTime.octet_ptr[4], sysConfig.stsSystemTime.octet_ptr[5]);

#if 0
    if ((pstsSystem->stsSystemId = 
         CloneOctetString(&(sysConfig.stsSystemId))) == NULL) {
        strncpy(prtString, 
                sysConfig.stsSystemId.octet_ptr,
                sysConfig.stsSystemId.length);

                prtString[sysConfig.stsSystemId.length] = '\0';

        printf("Could not get stsSystemId: %s\n", prtString);
        return (GEN_ERROR);
	}
#endif
    pstsSystem->stsSystemId                    =   &(sysConfig.stsSystemId);

    pstsSystem->stsSystemMgmtIpOption          =   sysConfig.stsSystemMgmtIpOption;
    pstsSystem->stsSystemMgmtIpAddress         =   sysConfig.stsSystemMgmtIpAddress;
    pstsSystem->stsSystemMgmtIpSubnetMask      =   sysConfig.stsSystemMgmtIpSubnetMask;
    pstsSystem->stsSystemMgmtIpGatewayAddress  =   sysConfig.stsSystemMgmtIpGatewayAddress;
    pstsSystem->stsSystemNtpIpOption           =   sysConfig.stsSystemNtpIpOption;
    pstsSystem->stsSystemNtpIpAddress          =   sysConfig.stsSystemNtpIpAddress;
    pstsSystem->stsSystemNtpIpSubnetMask       =   sysConfig.stsSystemNtpIpSubnetMask;
    pstsSystem->stsSystemNtpIpGatewayAddress   =   sysConfig.stsSystemNtpIpGatewayAddress;

#if 0
    if ((pstsSystem->stsSystemDate = 
         CloneOctetString(&(sysConfig.stsSystemDate))) == NULL) {
        strncpy(prtString, 
                sysConfig.stsSystemDate.octet_ptr,
                sysConfig.stsSystemDate.length);

                prtString[sysConfig.stsSystemDate.length] = '\0';

        printf("Could not get stsSystemDate: %s\n", prtString);
        return (GEN_ERROR);
	}
#endif

    pstsSystem->stsSystemDate                  =   &(sysConfig.stsSystemDate);

#if 0
    if ((pstsSystem->stsSystemTime = 
         CloneOctetString(&(sysConfig.stsSystemTime))) == NULL) {
        strncpy(prtString, 
                sysConfig.stsSystemTime.octet_ptr,
                sysConfig.stsSystemTime.length);

                prtString[sysConfig.stsSystemTime.length] = '\0';

        printf("Could not get stsSystemTime: %s\n", prtString);
        return (GEN_ERROR);
	}
#endif

    pstsSystem->stsSystemTime                  =   &(sysConfig.stsSystemTime);

    pstsSystem->stsSystemTimeZone              =   sysConfig.stsSystemTimeZone;
    pstsSystem->stsSystemIntegratedAlarm       =   sysConfig.stsSystemIntegratedAlarm;
    pstsSystem->stsSystemAlarmAcoMode          =   sysConfig.stsSystemAlarmAcoMode;
    pstsSystem->stsSystemAlarmAcoLed           =   sysConfig.stsSystemAlarmAcoLed;
    pstsSystem->stsSystemAlarmAcoCutOff        =   sysConfig.stsSystemAlarmAcoCutOff;
    pstsSystem->stsSystemAlarmEscalatingTime   =   sysConfig.stsSystemAlarmEscalatingTime;
    pstsSystem->stsSystemMinorLED              =   sysConfig.stsSystemMinorLED;
    pstsSystem->stsSystemMajorLED              =   sysConfig.stsSystemMajorLED;
    pstsSystem->stsSystemCriticalLED           =   sysConfig.stsSystemCriticalLED;
	pstsSystem->stsSystemUpgradeSWAction       =   sysConfig.stsSystemUpgradeSWAction;

#if 0
    if ((pstsSystem->stsSystemCurrentSWVersion = 
         CloneOctetString(&(sysConfig.stsSystemCurrentSWVersion))) == NULL) {
        strncpy(prtString, 
                sysConfig.stsSystemCurrentSWVersion.octet_ptr,
                sysConfig.stsSystemCurrentSWVersion.length);

                prtString[sysConfig.stsSystemCurrentSWVersion.length] = '\0';

        printf("Could not get stsSystemCurrentSWVersion: %s\n", prtString);
        return (GEN_ERROR);
	}
#endif

    pstsSystem->stsSystemCurrentSWVersion      =   &(sysConfig.stsSystemCurrentSWVersion);

    pstsSystem->stsSystemServiceState          =   sysConfig.stsSystemServiceState;
    pstsSystem->stsSystemReset                 =   sysConfig.stsSystemReset;

    return OK;
}

STATUS_t parseSysConfig()
{
    int                len;
    STATUS_t           status = OK;
    DIR                *dd = NULL;
	char  tmp[100];

    initSysDhcp();

    /* make sure the log directory exists */ 
    if ((dd = opendir(SYS_CONFIG_DIRECTORY)) == NULL)
    {
        status = mkdir(SYS_CONFIG_DIRECTORY);
    }

    if((sysConfigFp = fopen("C:config/sysConfig.bin", "rb")) == NULL)
	{
	    printf("System Config Open Failed\r\n");
	}
	len = fread((FUNCPTR) (&sysConfig), sizeof(SYS_CONFIG_T), 1, sysConfigFp);
	printf("Read sysFw Image: len %d\r\n", len);

    fclose(sysConfigFp);

	/* Set the Craft Interface to MENU or TL1 Mode */
	menuSelected                                     = sysConfig.menuSelected;

    sysConfig.stsSystemId.octet_ptr                  =  (unsigned char *)&(sysConfig.stsSystemIdBody);
    sysConfig.stsSystemDate.octet_ptr                =  (unsigned char *)&(sysConfig.stsSystemDateBody);
    sysConfig.stsSystemTime.octet_ptr                =  (unsigned char *)&(sysConfig.stsSystemTimeBody);
    sysConfig.stsSystemCurrentSWVersion.octet_ptr    =  (unsigned char *)&(sysConfig.stsSystemCurrentSWVersionBody);


    printf("System ID            :  %s\r\n", (char *)sysConfig.stsSystemId.octet_ptr);

	printf("Mgmt  Mode           :  %d\r\n", sysConfig.stsSystemMgmtIpOption);
	printf("Mgmt  IP             :  %d.%d.%d.%d\r\n", (sysConfig.stsSystemMgmtIpAddress >> 24) & 0xFF,
	                                          (sysConfig.stsSystemMgmtIpAddress >> 16) & 0xFF,
	                                          (sysConfig.stsSystemMgmtIpAddress >> 8) & 0xFF,
	                                          (sysConfig.stsSystemMgmtIpAddress) & 0xFF);

	printf("Mgmt  Subnet         :  %d.%d.%d.%d\r\n", (sysConfig.stsSystemMgmtIpSubnetMask >> 24) & 0xFF,
	                                          (sysConfig.stsSystemMgmtIpSubnetMask >> 16) & 0xFF,
	                                          (sysConfig.stsSystemMgmtIpSubnetMask >> 8) & 0xFF,
	                                          (sysConfig.stsSystemMgmtIpSubnetMask) & 0xFF);

	printf("Mgmt  Gateway        :  %d.%d.%d.%d\r\n", (sysConfig.stsSystemMgmtIpGatewayAddress >> 24) & 0xFF,
	                                          (sysConfig.stsSystemMgmtIpGatewayAddress >> 16) & 0xFF,
	                                          (sysConfig.stsSystemMgmtIpGatewayAddress >> 8) & 0xFF,
	                                          (sysConfig.stsSystemMgmtIpGatewayAddress) & 0xFF);

	printf("NTP  Mode           :  %d\r\n", sysConfig.stsSystemNtpIpOption);
	printf("NTP  IP             :  %d.%d.%d.%d\r\n", (sysConfig.stsSystemNtpIpAddress >> 24) & 0xFF,
	                                          (sysConfig.stsSystemNtpIpAddress >> 16) & 0xFF,
	                                          (sysConfig.stsSystemNtpIpAddress >> 8) & 0xFF,
	                                          (sysConfig.stsSystemNtpIpAddress) & 0xFF);

	printf("NTP  Subnet         :  %d.%d.%d.%d\r\n", (sysConfig.stsSystemNtpIpSubnetMask >> 24) & 0xFF,
	                                          (sysConfig.stsSystemNtpIpSubnetMask >> 16) & 0xFF,
	                                          (sysConfig.stsSystemNtpIpSubnetMask >> 8) & 0xFF,
	                                          (sysConfig.stsSystemNtpIpSubnetMask) & 0xFF);

	printf("NTP  Gateway        :  %d.%d.%d.%d\r\n", (sysConfig.stsSystemNtpIpGatewayAddress >> 24) & 0xFF,
	                                          (sysConfig.stsSystemNtpIpGatewayAddress >> 16) & 0xFF,
	                                          (sysConfig.stsSystemNtpIpGatewayAddress >> 8) & 0xFF,
	                                          (sysConfig.stsSystemNtpIpGatewayAddress) & 0xFF);

    printf("Time Zone            :  %d\r\n", sysConfig.stsSystemTimeZone);
    printf("Integrated Alarm     :  %d\r\n", sysConfig.stsSystemIntegratedAlarm);
    printf("ACO Mode             :  %d\r\n", sysConfig.stsSystemAlarmAcoMode);
    printf("ACO LED              :  %d\r\n", sysConfig.stsSystemAlarmAcoLed);
    printf("ACO Cutoff           :  %d\r\n", sysConfig.stsSystemAlarmAcoCutOff);
    printf("Escalating Time      :  %d\r\n", sysConfig.stsSystemAlarmEscalatingTime);
    printf("Minor LED            :  %d\r\n", sysConfig.stsSystemMinorLED);
    printf("Major LED            :  %d\r\n", sysConfig.stsSystemMajorLED);
    printf("Critical LED         :  %d\r\n", sysConfig.stsSystemCriticalLED);
    printf("Upgrade              :  %d\r\n", sysConfig.stsSystemUpgradeSWAction);
    printf("SW Version           :  %s\r\n", sysConfig.stsSystemCurrentSWVersion.octet_ptr);
    printf("Service State        :  %d\r\n", sysConfig.stsSystemServiceState);
    printf("Sys Reset            :  %d\r\n", sysConfig.stsSystemReset);
    printf("menuSlected          :  %d\r\n", sysConfig.menuSelected);

    if(sysConfig.stsSystemMgmtIpOption == D_stsSystemMgmtIpOption_dhcp)
	{
	    printf("MGMT IP: DHCP Mode\r\n");

	    if((status = dhcpcBind( sysDhcpCookie1, 1)) != OK)
	    {
		    printf("dhcp Bind Failed\r\n");
	    } 
	}
	else
	{
	    memset(tmp,0,sizeof(tmp));

        sprintf(tmp, "%d.%d.%d.%d",  (sysConfig.stsSystemMgmtIpAddress >> 24) & 0xFF,
								     (sysConfig.stsSystemMgmtIpAddress >> 16) & 0xFF,
								     (sysConfig.stsSystemMgmtIpAddress >> 8) & 0xFF,
								     (sysConfig.stsSystemMgmtIpAddress) & 0xFF);
        ifAddrSet("motfcc0", tmp);

        ifMaskSet("motfcc0", sysConfig.stsSystemMgmtIpSubnetMask);         
	}

    if(sysConfig.stsSystemNtpIpOption == D_stsSystemNtpIpOption_dhcp)
	{
	    printf("MGMT IP: DHCP Mode\r\n");

	    if((status = dhcpcBind( sysDhcpCookie2, 1)) != OK)
	    {
		    printf("dhcp Bind Failed\r\n");
	    } 
	}
	else
	{
	    memset(tmp,0,sizeof(tmp));

        sprintf(tmp, "%d.%d.%d.%d",  (sysConfig.stsSystemNtpIpAddress >> 24) & 0xFF,
								     (sysConfig.stsSystemNtpIpAddress >> 16) & 0xFF,
								     (sysConfig.stsSystemNtpIpAddress >> 8) & 0xFF,
								     (sysConfig.stsSystemNtpIpAddress) & 0xFF);
        ifAddrSet("motfcc1", tmp);

	    ifMaskSet("motfcc1", sysConfig.stsSystemNtpIpSubnetMask);
	}

	sprintf(tmp, "%d.%d.%d.%d", ((sysConfig.stsSystemMgmtIpGatewayAddress & 0xFF000000) >> 24),
	                            ((sysConfig.stsSystemMgmtIpGatewayAddress & 0x00FF0000) >> 16),
	     						((sysConfig.stsSystemMgmtIpGatewayAddress & 0x0000FF00) >> 8),
								(sysConfig.stsSystemMgmtIpGatewayAddress & 0x000000FF));
	routeDelete ("0", tmp);
	routeAdd ("0", tmp); 

	return OK;

}

STATUS_t defaultSysConfig()
{
    int len;

    memset((char *)&sysConfig, 0, sizeof(sysConfig));

    sprintf((char *)&(sysConfig.stsSystemIdBody), "TIEMPO");
	sysConfig.stsSystemId.length = strlen("TIEMPO");

    sysConfig.stsSystemMgmtIpOption             =  IP_MODE_STATIC;
    sysConfig.stsSystemMgmtIpAddress            =  0xC009C8DF;
    sysConfig.stsSystemMgmtIpSubnetMask         =  0xFFFFFF00;
    sysConfig.stsSystemMgmtIpGatewayAddress     =  0xC009C801;


    sysConfig.stsSystemNtpIpOption              =  IP_MODE_STATIC;
    sysConfig.stsSystemNtpIpAddress             =  0xC00AC8DF;
    sysConfig.stsSystemNtpIpSubnetMask          =  0xFFFFFF00;
    sysConfig.stsSystemNtpIpGatewayAddress      =  0xC00AC801;

    sysConfig.stsSystemTimeZone                 =  8;


    sprintf((char *)&(sysConfig.stsSystemCurrentSWVersionBody), "1.1.1.1");
	sysConfig.stsSystemCurrentSWVersion.length = strlen("1.1.1.1");

    if((sysConfigFp = fopen("C:config/sysConfig.bin", "wb")) == NULL)
	{
	    printf("System Config Open Failed\r\n");
	}
	len = fwrite((FUNCPTR) (&sysConfig), sizeof(SYS_CONFIG_T), 1, sysConfigFp);
	printf("Read sysFw Image: len %d\r\n", len);

    fclose(sysConfigFp);
	return OK;
}



STATUS_t termActionFuncSet(stsTerminalEntry_t * data)
{
    int len;

    if (VALID(I_stsTerminalBaudrate, data->valid))
    {
        termConfig.stsTerminalBaudrate = data->stsTerminalBaudrate; 
    }

    if (VALID(I_stsTerminalDataBits, data->valid))
    {
        termConfig.stsTerminalDataBits = data->stsTerminalDataBits; 
    }

    if (VALID(I_stsTerminalParity, data->valid))
    {
        termConfig.stsTerminalParity = data->stsTerminalParity; 
    }

    if (VALID(I_stsTerminalStopBits, data->valid))
    {
        termConfig.stsTerminalStopBits = data->stsTerminalStopBits; 
    }

    if((termConfigFp = fopen("C:config/termConfig.bin", "wb")) == NULL)
	{
	    printf("Term Config Open Failed\r\n");
	}

    sysSetCraftOption(termConfig.stsTerminalBaudrate,
	                  termConfig.stsTerminalDataBits,
					  termConfig.stsTerminalStopBits,
					  termConfig.stsTerminalParity);

	len = fwrite((FUNCPTR) (&termConfig), sizeof(TERM_CONFIG_T), 1, termConfigFp);
	printf("Read termConfig File: len %d\r\n", len);

    fclose(termConfigFp);
	return OK;


}

STATUS_t termActionFuncGet(stsTerminalEntry_t * data)
{
    data->stsTerminalBaudrate = termConfig.stsTerminalBaudrate; 
    data->stsTerminalDataBits = termConfig.stsTerminalDataBits; 
    data->stsTerminalParity   = termConfig.stsTerminalParity; 
    data->stsTerminalStopBits = termConfig.stsTerminalStopBits; 
	return OK;

}

STATUS_t parseTermConfig()
{
    int len;

    if((termConfigFp = fopen("C:config/termConfig.bin", "rb")) == NULL)
	{
	    printf("Term Config Open Failed\r\n");
	}
	len = fread((FUNCPTR) (&termConfig), sizeof(TERM_CONFIG_T), 1, termConfigFp);
	printf("Read termConfig File: len %d\r\n", len);

    fclose(termConfigFp);

	printf("BAUD RATE            :  %d\r\n", termConfig.stsTerminalBaudrate);
	printf("DATA BITS            :  %d\r\n", termConfig.stsTerminalDataBits);
	printf("PARITY               :  %d\r\n", termConfig.stsTerminalParity);
	printf("STOP BITS            :  %d\r\n", termConfig.stsTerminalStopBits);

	return OK;

}

STATUS_t defaultTermConfig()
{
    int len;

    memset((char *)&termConfig, 0, sizeof(termConfig));

    termConfig.stsTerminalBaudrate				=  D_stsTerminalBaudrate_b38400;
    termConfig.stsTerminalDataBits				=  D_stsTerminalDataBits_eight;
    termConfig.stsTerminalParity				=  D_stsTerminalParity_none;
    termConfig.stsTerminalStopBits				=  D_stsTerminalStopBits_one;

    if((termConfigFp = fopen("C:config/termConfig.bin", "wb")) == NULL)
	{
	    printf("Term Config Open Failed\r\n");
	}
	len = fwrite((FUNCPTR) (&termConfig), sizeof(TERM_CONFIG_T), 1, termConfigFp);
	printf("Read termConfig File: len %d\r\n", len);

    fclose(termConfigFp);
	return OK;
}

STATUS_t sysSetCraftOption(int baud, int dbits, int sbits, int parity)
{
	int SMC_CLEN = 0;
	int SMC_SBITS = 0;
	int SMC_PARITY_MODE = 0;
	int SMC_PARITY_ENABLE = 0;
    
    if(sbits == D_stsTerminalStopBits_one)
	{
        SMC_SBITS = SMCMR_SL_1_BIT;
        if(dbits == D_stsTerminalDataBits_seven)
	    {
            SMC_CLEN = SMCMR_CLEN_8_BITS;
        }
	    else
	    if(dbits == D_stsTerminalDataBits_eight)
	    {
		    SMC_CLEN = SMCMR_CLEN_9_BITS;
    	}
	    else
	    {
	    }
    }
	else
	if(sbits == D_stsTerminalStopBits_two)
	{
		SMC_SBITS = SMCMR_SL_2_BITS;
        if(dbits == D_stsTerminalDataBits_seven)
	    {
            SMC_CLEN = SMCMR_CLEN_9_BITS;
        }
	    else
	    if(dbits == D_stsTerminalDataBits_eight)
	    {
		    SMC_CLEN = SMCMR_CLEN_10_BITS;
    	}
	    else
	    {
	    }
	}
	else
	{
	}

    if(parity ==  D_stsTerminalParity_none)
	{
	    SMC_PARITY_ENABLE = 0;
	}
	else
	if(parity ==  D_stsTerminalParity_odd)
	{
	    SMC_PARITY_ENABLE = SMCMR_PEN;
		SMC_PARITY_MODE   = SMCMR_PM_ODD;
	}
	else
	if(parity ==  D_stsTerminalParity_even)
	{
	    SMC_PARITY_ENABLE = SMCMR_PEN;
		SMC_PARITY_MODE   = SMCMR_PM_EVEN;
	}
    else
	{
	}

    SMCMR_STD_MODE    =	(SMC_CLEN          | \
				         SMC_SBITS         | \
						 SMC_PARITY_MODE   | \
						 SMC_PARITY_ENABLE | \
				         SMCMR_SM_UART	   | \
				         SMCMR_DM_NORMAL);

    switch(baud)
	{
	    case D_stsTerminalBaudrate_b1200:
		    sysCraftBaudRate = 1200;
		break;

	    case D_stsTerminalBaudrate_b2400:
		    sysCraftBaudRate = 2400;
		break;

	    case D_stsTerminalBaudrate_b4800:
		    sysCraftBaudRate = 4800;
		break;

	    case D_stsTerminalBaudrate_b9600:
		    sysCraftBaudRate = 9600;
		break;

	    case D_stsTerminalBaudrate_b19200:
		    sysCraftBaudRate = 19200;
		break;

	    case D_stsTerminalBaudrate_b38400:
		    sysCraftBaudRate = 38400;
		break;

        default:
		    sysCraftBaudRate = 38400;
		break;
	}

#if 0
    eepromWrite(EE_TERM_BAUD_ADDR, (char *)&sysCraftBaudRate, EE_TERM_BAUD_SIZE);
    eepromWrite(EE_TERM_OPTION_ADDR, (char *)&SMCMR_STD_MODE, EE_TERM_OPTION_SIZE);
#endif
        
	printf("Baud Rate : %d\n", sysCraftBaudRate);
	printf("SMC option: %d\n", SMCMR_STD_MODE);

	(void) ioctl (consoleFd, FIOBAUDRATE, sysCraftBaudRate);
    smc8260ChangeOption(&(ppcChn[0]));
	return OK;
}

#if 0
STATUS_t sysTermInit()
{
#if 0
    eepromRead(EE_TERM_BAUD_ADDR, (char *)&sysCraftBaudRate, EE_TERM_BAUD_SIZE);
    eepromRead(EE_TERM_OPTION_ADDR, (char *)&SMCMR_STD_MODE, EE_TERM_OPTION_SIZE);
#endif
    smc8260DevInit ( &ppcChn[0] ) ;
	smc8260ResetChannel(&(ppcChn[0]));
	smc8260Ioctl(&(ppcChn[0]), SIO_MODE_SET, SIO_MODE_INT);
	sysSerialHwInit2();
    return OK;    
}
#endif


STATUS_t initSysDhcp()
{
    struct ifnet *      pIf;            /* pointer to network interface data */
	void * pCookie;

    pIf = ifunit ("motfcc0");
    sysDhcpCookie1 = pCookie = dhcpcInit(pIf, TRUE);

	dhcpcParamsShow( pCookie);
	printf("pIf: %p, pCookie: %p\r\n", pIf,  pCookie);


    ipAttach(1, "motfcc");

    pIf = ifunit ("motfcc1");
    sysDhcpCookie2 = pCookie = dhcpcInit(pIf, TRUE);

	dhcpcParamsShow( pCookie);
	printf("pIf: %p, pCookie: %p\r\n", pIf,  pCookie);

	return OK;
}


STATUS_t printHdr()
{
    struct timespec  tp;
	struct tm        brokenTime;

    clock_gettime(CLOCK_REALTIME, &tp);

    localtime_r(&(tp.tv_sec), 	&brokenTime);


    printf(" %s %2d-%2d-%2d %2d:%2d:%2d\r\n",(char *)sysConfig.stsSystemId.octet_ptr,
	                                   brokenTime.tm_year + 1900 - 2000,
	                                   brokenTime.tm_mon + 1,
	                                   brokenTime.tm_mday,
	                                   brokenTime.tm_hour,
	                                   brokenTime.tm_min,
	                                   brokenTime.tm_sec);
    return OK;

}

STATUS_t menuEnable()
{
    int len;

    menuSelected = 1;
	sysConfig.menuSelected = menuSelected;
    if((sysConfigFp = fopen("C:config/sysConfig.bin", "wb")) == NULL)
	{
	    printf("System Config Open Failed\r\n");
	}
	len = fwrite((FUNCPTR) (&sysConfig), sizeof(SYS_CONFIG_T), 1, sysConfigFp);
	printf("Read sysFw Image: len %d\r\n", len);

    fclose(sysConfigFp);

	return OK;
}

STATUS_t tl1Enable()
{
    int len;

    menuSelected = 0;
	sysConfig.menuSelected = menuSelected;
    if((sysConfigFp = fopen("C:config/sysConfig.bin", "wb")) == NULL)
	{
	    printf("System Config Open Failed\r\n");
	}
	len = fwrite((FUNCPTR) (&sysConfig), sizeof(SYS_CONFIG_T), 1, sysConfigFp);
	printf("Read sysFw Image: len %d\r\n", len);

    fclose(sysConfigFp);

	return OK;
}
