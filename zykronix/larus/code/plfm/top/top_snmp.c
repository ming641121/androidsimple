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
#include <dhcpcLib.h>
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
#include "top_snmp.h"
#include "sys_dir.h"
#include "rtc_if.h"
#include "smc8260Sio.h"
#include "drv\sio\m8260Sio.h"
#include "axss_types.h"
#include "sys_if.h"

FILE  *topConfigFp;
FILE  *topdestConfigFp;

TOP_CONFIG_T        topConfig;
TOP_DEST_CONFIG_T   topdestConfig[10];
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

/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */

STATUS_t topActionFuncSet(stsTop_t * data)
{
    int len;

    if (VALID(I_stsTopSourceIpOption, data->valid))
    {
        topConfig.stsTopSourceIpOption = data->stsTopSourceIpOption; 
    }

    if (VALID(I_stsTopSourceIpAddress, data->valid))
    {
        topConfig.stsTopSourceIpAddress = data->stsTopSourceIpAddress; 
    }

    if (VALID(I_stsTopSourceIpSubnetMask, data->valid))
    {
        topConfig.stsTopSourceIpSubnetMask = data->stsTopSourceIpSubnetMask; 
    }

    if (VALID(I_stsTopSourceIpGatewayAddress, data->valid))
    {
        topConfig.stsTopSourceIpGatewayAddress = data->stsTopSourceIpGatewayAddress; 
    }

    if (VALID(I_stsTopPacketRate, data->valid))
    {
        topConfig.stsTopPacketRate = data->stsTopPacketRate; 
    }

    if (VALID(I_stsTopInputSelect, data->valid))
    {
        topConfig.stsTopInputSelect = data->stsTopInputSelect; 
    }

    if (VALID(I_stsTopTxPackets, data->valid))
    {
        topConfig.stsTopTxPackets = data->stsTopTxPackets; 
    }

    if (VALID(I_stsTopTxBytes, data->valid))
    {
        topConfig.stsTopTxBytes = data->stsTopTxBytes; 
    }

    if((topConfigFp = fopen("C:config/topConfig.bin", "wb")) == NULL)
	{
	    printf("Top Config Open Failed\r\n");
	}

	len = fwrite((FUNCPTR) (&topConfig), sizeof(TOP_CONFIG_T), 1, topConfigFp);
	printf("Read topConfig File: len %d\r\n", len);

    fclose(topConfigFp);
	return OK;


}

STATUS_t topActionFuncGet(stsTop_t * data)
{
    data->stsTopSourceIpOption = topConfig.stsTopSourceIpOption; 
    data->stsTopSourceIpAddress = topConfig.stsTopSourceIpAddress; 
    data->stsTopSourceIpSubnetMask   = topConfig.stsTopSourceIpSubnetMask; 
    data->stsTopSourceIpGatewayAddress = topConfig.stsTopSourceIpGatewayAddress; 
    data->stsTopPacketRate = topConfig.stsTopPacketRate; 
    data->stsTopInputSelect = topConfig.stsTopInputSelect; 
    data->stsTopTxPackets = topConfig.stsTopTxPackets; 
    data->stsTopTxBytes = topConfig.stsTopTxBytes; 

	return OK;

}

STATUS_t parseTopConfig()
{
    int len;

    if((topConfigFp = fopen("C:config/topConfig.bin", "rb")) == NULL)
	{
	    printf("Top Config Open Failed\r\n");
	}
	len = fread((FUNCPTR) (&topConfig), sizeof(TOP_CONFIG_T), 1, topConfigFp);
	printf("Read topConfig File: len %d\r\n", len);

    fclose(topConfigFp);

	printf("TOP SOURCE IP OPTION         :  %d\r\n", topConfig.stsTopSourceIpOption);
	printf("TOP SOURCE IP ADDR           :  %d\r\n", topConfig.stsTopSourceIpAddress);
	printf("TOP SOURCE IP MASK           :  %d\r\n", topConfig.stsTopSourceIpSubnetMask);
	printf("TOP SOURCE IP GATEWAY        :  %d\r\n", topConfig.stsTopSourceIpGatewayAddress);
	printf("TOP PACKET RATE              :  %d\r\n", topConfig.stsTopPacketRate);
	printf("TOP INPUT SELECT             :  %d\r\n", topConfig.stsTopInputSelect);
	printf("TOP TX PACKET                :  %d\r\n", topConfig.stsTopTxPackets);
	printf("TOP TX BYTE                  :  %d\r\n", topConfig.stsTopTxBytes);

	return OK;

}

STATUS_t defaultTopConfig()
{
    int len;

    memset((char *)&topConfig, 0, sizeof(topConfig));

    topConfig.stsTopSourceIpOption				=  D_stsTopSourceIpOption_static;
    topConfig.stsTopSourceIpAddress				=  0x10101010;
    topConfig.stsTopSourceIpSubnetMask			=  0xFFFFFF00;
    topConfig.stsTopSourceIpGatewayAddress		=  0x10101001;
    topConfig.stsTopPacketRate		            =  0x20;
    topConfig.stsTopInputSelect		            =  D_stsTopInputSelect_e1;
    topConfig.stsTopTxPackets		            =  0;
    topConfig.stsTopTxBytes	                    =  0;

    if((topConfigFp = fopen("C:config/topConfig.bin", "wb")) == NULL)
	{
	    printf("Top Config Open Failed\r\n");
	}
	len = fwrite((FUNCPTR) (&topConfig), sizeof(TOP_CONFIG_T), 1, topConfigFp);
	printf("Read topConfig File: len %d\r\n", len);

    fclose(topConfigFp);
	return OK;
}


STATUS_t topdestActionFuncSet(stsTopDestEntry_t * data)
{
    int len;

    if (VALID(I_stsTopDestIp, data->valid))
    {
        topdestConfig[data->stsTopDestNumber].stsTopDestIp = data->stsTopDestIp; 
    }

    if (VALID(I_stsTopDestUdpPort, data->valid))
    {
        topdestConfig[data->stsTopDestNumber].stsTopDestUdpPort = data->stsTopDestUdpPort; 
    }

    if (VALID(I_stsTopDestVlanId, data->valid))
    {
        topdestConfig[data->stsTopDestNumber].stsTopDestVlanId = data->stsTopDestVlanId; 
    }

    if((topdestConfigFp = fopen("C:config/topdestConfig.bin", "wb")) == NULL)
	{
	    printf("Topdest Config Open Failed\r\n");
	}

	len = fwrite((FUNCPTR) (&topdestConfig), sizeof(TOP_DEST_CONFIG_T), 1, topdestConfigFp);
	printf("Read topdestConfig File: len %d\r\n", len);

    fclose(topdestConfigFp);
	return OK;


}

STATUS_t topdestActionFuncGet(stsTopDestEntry_t * data)
{
    data->stsTopDestIp      = topdestConfig[data->stsTopDestNumber].stsTopDestIp; 
    data->stsTopDestUdpPort = topdestConfig[data->stsTopDestNumber].stsTopDestUdpPort; 
    data->stsTopDestVlanId  = topdestConfig[data->stsTopDestNumber].stsTopDestVlanId; 

	return OK;

}

STATUS_t parseTopdestConfig()
{
    int len;
	int i;

    if((topdestConfigFp = fopen("C:config/topdestConfig.bin", "rb")) == NULL)
	{
	    printf("Topdest Config Open Failed\r\n");
	}
	len = fread((FUNCPTR) (&topdestConfig), sizeof(TOP_DEST_CONFIG_T)*10, 1, topdestConfigFp);
	printf("Read topdestConfig File: len %d\r\n", len);

    fclose(topdestConfigFp);

    for(i=0; i<10; i++)
	{
	    printf("TOP DEST IP ADDR          :  %d\r\n", topdestConfig[i].stsTopDestIp);
	    printf("TOP DEST IP UDP           :  %d\r\n", topdestConfig[i].stsTopDestUdpPort);
	    printf("TOP DEST IP VLAN          :  %d\r\n", topdestConfig[i].stsTopDestVlanId);
	}
	return OK;

}

STATUS_t defaultTopdestConfig()
{
    int len;
    int i;

    memset((char *)&topdestConfig, 0, sizeof(topdestConfig));

    for(i=0; i<10; i++)
	{
	    topdestConfig[i].stsTopDestIp      = 0;
	    topdestConfig[i].stsTopDestUdpPort = 0;
	    topdestConfig[i].stsTopDestVlanId  = 0;
	}

    if((topdestConfigFp = fopen("C:config/topdestConfig.bin", "wb")) == NULL)
	{
	    printf("Topdest Config Open Failed\r\n");
	}
	len = fwrite((FUNCPTR) (&topdestConfig), 10*sizeof(TOP_DEST_CONFIG_T), 1, topdestConfigFp);
	printf("Read topdestConfig File: len %d\r\n", len);

    fclose(topdestConfigFp);
	return OK;
}
