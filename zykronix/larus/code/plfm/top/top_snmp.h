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

#ifndef TOP_SNMP_H
#define TOP_SNMP_H

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
typedef
struct {
        SR_INT32        stsTopSourceIpOption;
        SR_UINT32       stsTopSourceIpAddress;
        SR_UINT32       stsTopSourceIpSubnetMask;
        SR_UINT32       stsTopSourceIpGatewayAddress;
        SR_INT32        stsTopPacketRate;
        SR_INT32        stsTopInputSelect;
        SR_INT32        stsTopTxPackets;
        SR_INT32        stsTopTxBytes;
        char            valid[2];
} TOP_CONFIG_T;

typedef
struct {
        SR_INT32        stsTopDestNumber;
        SR_UINT32       stsTopDestIp;
        SR_INT32        stsTopDestUdpPort;
        SR_INT32        stsTopDestVlanId;
        char            valid[1];
} TOP_DEST_CONFIG_T;
/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */

extern STATUS_t topActionFuncSet(stsTop_t * data);

extern STATUS_t topActionFuncGet(stsTop_t * data);

extern STATUS_t topdestActionFuncSet(stsTopDestEntry_t * data);

extern STATUS_t topdestActionFuncGet(stsTopDestEntry_t * data);

#endif /* TOP_SNMP_H */