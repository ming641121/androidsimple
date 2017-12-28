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


#ifndef SR_MIBOUT_H
#define SR_MIBOUT_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef MIBDEFS
#ifdef SR_QUOTE
#include SR_QUOTE(MIBDEFS)
#else /* SR_QUOTE */
#include MIBDEFS
#endif /* SR_QUOTE */
#else /* MIBDEFS */
#include "snmpdefs.h"
#endif /* MIBDEFS */

#ifdef MIBSUPP
#ifdef SR_QUOTE
#include SR_QUOTE(MIBSUPP)
#else /* SR_QUOTE */
#include MIBSUPP
#endif /* SR_QUOTE */
#else /* MIBSUPP */
#include "snmpsupp.h"
#endif /* MIBSUPP */

#ifdef MIBPART
#ifdef SR_QUOTE
#include SR_QUOTE(MIBPART)
#else /* SR_QUOTE */
#include MIBPART
#endif /* SR_QUOTE */
#else /* MIBPART */
#include "snmppart.h"
#endif /* MIBPART */

#ifdef MIBTYPE
#ifdef SR_QUOTE
#include SR_QUOTE(MIBTYPE)
#else /* SR_QUOTE */
#include MIBTYPE
#endif /* SR_QUOTE */
#else /* MIBTYPE */
#include "snmptype.h"
#endif /* MIBTYPE */

#ifdef MIBPROT
#ifdef SR_QUOTE
#include SR_QUOTE(MIBPROT)
#else /* SR_QUOTE */
#include MIBPROT
#endif /* SR_QUOTE */
#endif /* MIBPROT */



#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_MIBOUT_H */