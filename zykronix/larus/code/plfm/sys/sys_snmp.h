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

#ifndef SYS_SNMP_H
#define SYS_SNMP_H

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
#define  IP_MODE_STATIC 1
#define  IP_MODE_DHCP   2

typedef struct  {
        int             signature;
		int             version;
		OctetString     stsSystemId;
        SR_INT32        stsSystemMgmtIpOption;
        SR_UINT32       stsSystemMgmtIpAddress;
        SR_UINT32       stsSystemMgmtIpSubnetMask;
        SR_UINT32       stsSystemMgmtIpGatewayAddress;
        SR_INT32        stsSystemNtpIpOption;
        SR_UINT32       stsSystemNtpIpAddress;
        SR_UINT32       stsSystemNtpIpSubnetMask;
        SR_UINT32       stsSystemNtpIpGatewayAddress;
        OctetString     stsSystemDate;
        OctetString     stsSystemTime;
        SR_INT32        stsSystemTimeZone;
        SR_INT32        stsSystemIntegratedAlarm;
        SR_INT32        stsSystemAlarmAcoMode;
        SR_INT32        stsSystemAlarmAcoLed;
        SR_INT32        stsSystemAlarmAcoCutOff;
        SR_INT32        stsSystemAlarmEscalatingTime;
        SR_INT32        stsSystemMinorLED;
        SR_INT32        stsSystemMajorLED;
        SR_INT32        stsSystemCriticalLED;
        SR_INT32        stsSystemUpgradeSWAction;
        OctetString     stsSystemCurrentSWVersion;
        SR_INT32        stsSystemServiceState;
        SR_INT32        stsSystemReset;

        SR_INT32        stsSystemIdBody[32];
        SR_INT32        stsSystemDateBody[6];
        SR_INT32        stsSystemTimeBody[6];
        SR_INT32        stsSystemCurrentSWVersionBody[8];
        SR_INT32        menuSelected;

        char            valid[4];

        int  checksum;

} SYS_CONFIG_T;

typedef struct  {
        SR_INT32        stsTerminalIndex;
        SR_INT32        stsTerminalBaudrate;
        SR_INT32        stsTerminalDataBits;
        SR_INT32        stsTerminalParity;
        SR_INT32        stsTerminalStopBits;
        char            valid[1];
} TERM_CONFIG_T;

/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */

extern STATUS_t systemActionFuncSet(stsSystem_t * data);

extern STATUS_t systemActionFuncGet(stsSystem_t * pstsSystem);

extern STATUS_t termActionFuncSet(stsTerminalEntry_t * data);

extern STATUS_t termActionFuncGet(stsTerminalEntry_t * data);

extern STATUS_t sysSetCraftOption(int baud, int dbits, int sbits, int parity);

#endif /* SYS_SNMP_H */