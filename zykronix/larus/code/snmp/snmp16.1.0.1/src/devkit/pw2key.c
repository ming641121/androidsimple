/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include <string.h>

#include "sr_snmp.h"

#ifndef SR_UNSECURABLE

#include "pw2key.h"
#include "global.h"

#ifdef SR_MD5_HASH_ALGORITHM
#include "md5key.h"
#endif /* SR_MD5_HASH_ALGORITHM */

#ifdef SR_SHA_HASH_ALGORITHM
#include "shakey.h"
#endif /* SR_SHA_HASH_ALGORITHM */

OctetString *
SrOctetStringPasswordToKey(
    SR_INT32 algorithm,
    const OctetString *password)
{
    switch (algorithm) {
#ifdef SR_MD5_HASH_ALGORITHM
        case SR_PW2KEY_ALGORITHM_MD5:
            return MakeMD5PasswordKey(password);
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
        case SR_PW2KEY_ALGORITHM_SHA:
            return MakeSHAPasswordKey(password);
#endif /* SR_SHA_HASH_ALGORITHM */
    }
    return NULL;
}

OctetString *
SrTextPasswordToKey(
    SR_INT32 algorithm,
    const char *password)
{
    OctetString pw_os;
    pw_os.octet_ptr = (unsigned char *)password;
    pw_os.length = strlen(password);
    return SrOctetStringPasswordToKey(algorithm, &pw_os);
}

OctetString *
SrLocalizeKey(
    SR_INT32 algorithm,
    const OctetString *snmpEngineID,
    const OctetString *key)
{
    switch (algorithm) {
#ifdef SR_MD5_HASH_ALGORITHM
        case SR_PW2KEY_ALGORITHM_MD5:
            return MD5LocalizeKey(snmpEngineID, key);
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
        case SR_PW2KEY_ALGORITHM_SHA:
            return SHALocalizeKey(snmpEngineID, key);
#endif /* SR_SHA_HASH_ALGORITHM */
    }
    return NULL;
}

OctetString *
SrOctetStringPasswordToLocalizedKey(
    SR_INT32 algorithm,
    const OctetString *snmpEngineID,
    const OctetString *password)
{
    OctetString *k1, *k2;

    k1 = SrOctetStringPasswordToKey(algorithm, password);
    if (k1 == NULL) {
        return NULL;
    }
    k2 = SrLocalizeKey(algorithm, snmpEngineID, k1);
    FreeOctetString(k1);
    return k2;
}

OctetString *
SrTextPasswordToLocalizedKey(
    SR_INT32 algorithm,
    const OctetString *snmpEngineID,
    const char *password)
{
    OctetString pw_os;
    pw_os.octet_ptr = (unsigned char *)password;
    pw_os.length = strlen(password);
    return SrOctetStringPasswordToLocalizedKey(algorithm, snmpEngineID, &pw_os);
}

#ifdef SR_MD5_HASH_ALGORITHM
#include "md5key.c"
#endif /* SR_MD5_HASH_ALGORITHM */

#ifdef SR_SHA_HASH_ALGORITHM
#include "shakey.c"
#endif /* SR_SHA_HASH_ALGORITHM */

#endif /* SR_UNSECURABLE */