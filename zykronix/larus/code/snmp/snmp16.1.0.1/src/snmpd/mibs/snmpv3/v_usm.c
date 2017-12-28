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
 */


/*
 * Arguments used to create this file:
 * -f mib.cnf -o v3 -per_file_init -stubs_only -search_table (implies -row_status) 
 * -test_and_incr -parser -row_status -userpart -storage_type 
 */


/*
 * File Description:
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>


#include <string.h>


#include <stddef.h>
#include "sr_snmp.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "makevb.h"
#include "inst_lib.h"
#include "tmq.h"
#include "rowstatf.h"
#include "i_usm.h"
#include "diag.h"
SR_FILENAME

/*---------------------------------------------------------------------
 * Retrieve data from the usmStats family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
usmStats_get(OID *incoming, ObjectInfo *object, int searchType,
             ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef usmStats_DISABLE_CACHE
    usmStats_t      *data = NULL;
#else /* usmStats_DISABLE_CACHE */
    static usmStats_t      *data = NULL;
    static int             last_serialNum = 0;
    static OID             *last_incoming = NULL;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* usmStats_DISABLE_CACHE */
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and
     * the single instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    switch (searchType) {
      case EXACT:
        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
          arg = object->nominator;
        }
        break;

      case NEXT:
        if (instLength <= 0) {
          arg = object->nominator;
        }
        break;

    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_usmStats_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef usmStats_DISABLE_CACHE
     if (arg == -1) {
         data = NULL;
     }
     else {
         last_serialNum = serialNum;
         if (last_incoming != NULL) {
            FreeOID(last_incoming);
         }
         last_incoming = CloneOID(incoming);
     }
  }
  else {
     arg = object->nominator;
  }
#endif /* usmStats_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_usmStatsUnsupportedSecLevels
    case I_usmStatsUnsupportedSecLevels:
       dp = (void *) (&data->usmStatsUnsupportedSecLevels);
       break;
#endif /* I_usmStatsUnsupportedSecLevels */

#ifdef I_usmStatsNotInTimeWindows
    case I_usmStatsNotInTimeWindows:
       dp = (void *) (&data->usmStatsNotInTimeWindows);
       break;
#endif /* I_usmStatsNotInTimeWindows */

#ifdef I_usmStatsUnknownUserNames
    case I_usmStatsUnknownUserNames:
       dp = (void *) (&data->usmStatsUnknownUserNames);
       break;
#endif /* I_usmStatsUnknownUserNames */

#ifdef I_usmStatsUnknownEngineIDs
    case I_usmStatsUnknownEngineIDs:
       dp = (void *) (&data->usmStatsUnknownEngineIDs);
       break;
#endif /* I_usmStatsUnknownEngineIDs */

#ifdef I_usmStatsWrongDigests
    case I_usmStatsWrongDigests:
       dp = (void *) (&data->usmStatsWrongDigests);
       break;
#endif /* I_usmStatsWrongDigests */

#ifdef I_usmStatsDecryptionErrors
    case I_usmStatsDecryptionErrors:
       dp = (void *) (&data->usmStatsDecryptionErrors);
       break;
#endif /* I_usmStatsDecryptionErrors */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the usmUser family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
usmUser_get(OID *incoming, ObjectInfo *object, int searchType,
            ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef usmUser_DISABLE_CACHE
    usmUser_t       *data = NULL;
#else /* usmUser_DISABLE_CACHE */
    static usmUser_t       *data = NULL;
    static int             last_serialNum = 0;
    static OID             *last_incoming = NULL;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* usmUser_DISABLE_CACHE */
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and
     * the single instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    switch (searchType) {
      case EXACT:
        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
          arg = object->nominator;
        }
        break;

      case NEXT:
        if (instLength <= 0) {
          arg = object->nominator;
        }
        break;

    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_usmUser_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef usmUser_DISABLE_CACHE
     if (arg == -1) {
         data = NULL;
     }
     else {
         last_serialNum = serialNum;
         if (last_incoming != NULL) {
            FreeOID(last_incoming);
         }
         last_incoming = CloneOID(incoming);
     }
  }
  else {
     arg = object->nominator;
  }
#endif /* usmUser_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_usmUserSpinLock
    case I_usmUserSpinLock:
       dp = (void *) (&data->usmUserSpinLock);
       break;
#endif /* I_usmUserSpinLock */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the usmUser data object.
 *---------------------------------------------------------------------*/
void
usmUser_free(usmUser_t *data)
{
   if (data != NULL) {
#ifdef U_usmUser
       /* free user-defined data */
       k_usmUserFreeUserpartData (data);
#endif /* U_usmUser */


       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after usmUser set/undo
 *---------------------------------------------------------------------*/
static int usmUser_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
usmUser_cleanup(doList_t *trash)
{
   usmUser_free((usmUser_t *) trash->data);
   usmUser_free((usmUser_t *) trash->undodata);
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the usmUser family
 *---------------------------------------------------------------------*/
usmUser_t *
Clone_usmUser(usmUser_t *usmUser)
{
    usmUser_t *data;

    if (usmUser == NULL) {
        return NULL;
    }

    if ((data = (usmUser_t *) malloc (sizeof(usmUser_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (usmUser), sizeof(usmUser_t));

#ifdef U_usmUser
    k_usmUserCloneUserpartData(data, usmUser);
#endif /* U_usmUser */

    return data;
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int 
usmUser_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
             doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    int            instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    usmUser_t     *usmUser;

   /*
    * Validate the object instance: 1) It must be of length 1  2) and the
    * instance must be 0.
    */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
        return (NO_CREATION_ERROR); 
    }

    usmUser = k_usmUser_get(-1, contextInfo, -1);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == usmUser_set) &&
            (((usmUser_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ((dp->data = (void *) Clone_usmUser(usmUser)) == NULL) {
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        if ((dp->undodata = (void *) Clone_usmUser(usmUser)) == NULL) {
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            error_status = RESOURCE_UNAVAILABLE_ERROR;
        }

        dp->setMethod = usmUser_set;
        dp->cleanupMethod = usmUser_cleanup;
#ifdef SR_usmUser_UNDO
        dp->undoMethod = usmUser_undo;
#else /* SR_usmUser_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_usmUser_UNDO */
        dp->state = SR_UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_usmUserSpinLock
   case I_usmUserSpinLock:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     /* since this is a TestAndIncr variable, make sure that it is being set to
      * its current value, if one exists */
     if (dp->undodata != NULL) {
         /* compare against the cached value.  Use the undodata value,
          * in case this object is set in multiple varbinds. */
         if(value->sl_value != ((usmUser_t *) (dp->undodata))->usmUserSpinLock) {
             /* value doesn't match, return an error */
             return INCONSISTENT_VALUE_ERROR;
         } else {
             /* update the value */
             if(value->sl_value != 2147483647) {
                 /* increment the value */
                 value->sl_value++;
             } else {
                 /* wrap the value */
                 value->sl_value = 0;
             }
         }
     }

     ((usmUser_t *) (dp->data))->usmUserSpinLock = value->sl_value;
     break;
#endif /* I_usmUserSpinLock */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in usmUser_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_usmUser_test */
   error_status = k_usmUser_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((usmUser_t *) (dp->data))->valid);
        error_status = k_usmUser_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
usmUser_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_usmUser_set((usmUser_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the usmUserEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
usmUserEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                 ContextInfo *contextInfo, int serialNum)
{
    int             arg = object->nominator;
    void            *dp;
    int             carry;
    int             i;
    OctetString *   usmUserEngineID = NULL;
    int             usmUserEngineID_offset;
    OctetString *   usmUserName = NULL;
    int             usmUserName_offset;
    int             index;
    int             final_index;

#ifdef usmUserEntry_DISABLE_CACHE
    usmUserEntry_t  *data = NULL;
    SR_UINT32   buffer[MAX_OID_SIZE];
    OID             inst;
#else /* usmUserEntry_DISABLE_CACHE */
    static usmUserEntry_t  *data = NULL;
    static SR_UINT32   buffer[MAX_OID_SIZE];
    static OID             inst;
    static int             last_serialNum = 0;
    static OID             *last_incoming = NULL;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
         (!NEXT_ROW(data->valid)) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* usmUserEntry_DISABLE_CACHE */
    usmUserEngineID_offset = object->oid.length;
    usmUserName_offset = usmUserEngineID_offset + GetVariableIndexLength(incoming, usmUserEngineID_offset);
    final_index = usmUserName_offset + GetVariableIndexLength(incoming, usmUserName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, usmUserName_offset, &usmUserName, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, usmUserEngineID_offset, &usmUserEngineID, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_usmUserEntry_get(serialNum, contextInfo, arg ,searchType, usmUserEngineID, usmUserName)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->usmUserEngineID->length;
        for(i = 0; i < data->usmUserEngineID->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->usmUserEngineID->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = data->usmUserName->length;
        for(i = 0; i < data->usmUserName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->usmUserName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(usmUserEngineID);
        FreeOctetString(usmUserName);

#ifndef usmUserEntry_DISABLE_CACHE
     if (arg == -1) {
         data = NULL;
     }
     else {
         last_serialNum = serialNum;
         if (last_incoming != NULL) {
            FreeOID(last_incoming);
         }
         last_incoming = CloneOID(incoming);
     }
  }
#endif /* usmUserEntry_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_usmUserSecurityName
    case I_usmUserSecurityName:
       dp = (void *) (CloneOctetString(data->usmUserName));
       break;
#endif /* I_usmUserSecurityName */

#ifdef I_usmUserCloneFrom
    case I_usmUserCloneFrom:
       dp = (void *) (CloneOID(data->usmUserCloneFrom));
       break;
#endif /* I_usmUserCloneFrom */

#ifdef I_usmUserAuthProtocol
    case I_usmUserAuthProtocol:
       dp = (void *) (CloneOID(data->usmUserAuthProtocol));
       break;
#endif /* I_usmUserAuthProtocol */

#ifdef I_usmUserAuthKeyChange
    case I_usmUserAuthKeyChange:
       dp = (void *) (CloneOctetString(data->usmUserAuthKeyChange));
       break;
#endif /* I_usmUserAuthKeyChange */

#ifdef I_usmUserOwnAuthKeyChange
    case I_usmUserOwnAuthKeyChange:
       dp = (void *) (CloneOctetString(data->usmUserOwnAuthKeyChange));
       break;
#endif /* I_usmUserOwnAuthKeyChange */

#ifdef I_usmUserPrivProtocol
    case I_usmUserPrivProtocol:
       dp = (void *) (CloneOID(data->usmUserPrivProtocol));
       break;
#endif /* I_usmUserPrivProtocol */

#ifdef I_usmUserPrivKeyChange
    case I_usmUserPrivKeyChange:
       dp = (void *) (CloneOctetString(data->usmUserPrivKeyChange));
       break;
#endif /* I_usmUserPrivKeyChange */

#ifdef I_usmUserOwnPrivKeyChange
    case I_usmUserOwnPrivKeyChange:
       dp = (void *) (CloneOctetString(data->usmUserOwnPrivKeyChange));
       break;
#endif /* I_usmUserOwnPrivKeyChange */

#ifdef I_usmUserPublic
    case I_usmUserPublic:
       dp = (void *) (CloneOctetString(data->usmUserPublic));
       break;
#endif /* I_usmUserPublic */

#ifdef I_usmUserStorageType
    case I_usmUserStorageType:
       dp = (void *) (&data->usmUserStorageType);
       break;
#endif /* I_usmUserStorageType */

#ifdef I_usmUserStatus
    case I_usmUserStatus:
       dp = (void *) (&data->usmUserStatus);
       break;
#endif /* I_usmUserStatus */

#ifdef I_usmTargetTag
    case I_usmTargetTag:
       dp = (void *) (CloneOctetString(data->usmTargetTag));
       break;
#endif /* I_usmTargetTag */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * cleanup after usmUserEntry set/undo
 *---------------------------------------------------------------------*/
static int usmUserEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
usmUserEntry_cleanup(doList_t *trash)
{
    FreeEntries(usmUserEntryTypeTable, trash->data);
    if (trash->data != NULL) {
        free(trash->data);
    }
    FreeEntries(usmUserEntryTypeTable, trash->undodata);
    if (trash->undodata != NULL) {
        free(trash->undodata);
    }
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the usmUserEntry family
 *---------------------------------------------------------------------*/
usmUserEntry_t *
Clone_usmUserEntry(usmUserEntry_t *usmUserEntry)
{
    usmUserEntry_t *data;

    if (usmUserEntry == NULL) {
        return NULL;
    }

    if ((data = (usmUserEntry_t *) malloc (sizeof(usmUserEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (usmUserEntry), sizeof(usmUserEntry_t));

    data->usmUserEngineID = CloneOctetString(usmUserEntry->usmUserEngineID);
    data->usmUserName = CloneOctetString(usmUserEntry->usmUserName);
    data->usmUserSecurityName = CloneOctetString(usmUserEntry->usmUserSecurityName);
    data->usmUserCloneFrom = CloneOID(usmUserEntry->usmUserCloneFrom);
    data->usmUserAuthProtocol = CloneOID(usmUserEntry->usmUserAuthProtocol);
    data->usmUserAuthKeyChange = CloneOctetString(usmUserEntry->usmUserAuthKeyChange);
    data->usmUserOwnAuthKeyChange = CloneOctetString(usmUserEntry->usmUserOwnAuthKeyChange);
    data->usmUserPrivProtocol = CloneOID(usmUserEntry->usmUserPrivProtocol);
    data->usmUserPrivKeyChange = CloneOctetString(usmUserEntry->usmUserPrivKeyChange);
    data->usmUserOwnPrivKeyChange = CloneOctetString(usmUserEntry->usmUserOwnPrivKeyChange);
    data->usmUserPublic = CloneOctetString(usmUserEntry->usmUserPublic);
    data->usmTargetTag = CloneOctetString(usmUserEntry->usmTargetTag);
#ifdef U_usmUserEntry
    k_usmUserEntryCloneUserpartData(data, usmUserEntry);
#endif /* U_usmUserEntry */

    return data;
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int 
usmUserEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                  doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    int            carry = 0;
    usmUserEntry_t     *usmUserEntry;
    OctetString *  usmUserEngineID = NULL;
    int            usmUserEngineID_offset;
    OctetString *  usmUserName = NULL;
    int            usmUserName_offset;
    int            final_index;

   /*
    * Validate the object instance
    *
    */
    usmUserEngineID_offset = object->oid.length;
    usmUserName_offset = usmUserEngineID_offset + GetVariableIndexLength(incoming, usmUserEngineID_offset);
    final_index = usmUserName_offset + GetVariableIndexLength(incoming, usmUserName_offset);

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, usmUserEngineID_offset, &usmUserEngineID, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToVariableOctetString(incoming, usmUserName_offset, &usmUserName, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status == NO_ERROR) {
        if ((usmUserEngineID->length > 32) ||
            (usmUserEngineID->length < 5) ||
            (usmUserName->length > 32) ||
            (usmUserName->length < 1)) {
            error_status = NO_CREATION_ERROR;
        }
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(usmUserEngineID);
        FreeOctetString(usmUserName);
        return error_status;
    }

    usmUserEntry = k_usmUserEntry_get(-1, contextInfo, -1, EXACT, usmUserEngineID, usmUserName);
    if (usmUserEntry != NULL) {
        if (usmUserEntry->usmUserStorageType == D_usmUserStorageType_readOnly) {
        FreeOctetString(usmUserEngineID);
        FreeOctetString(usmUserName);
            return NOT_WRITABLE_ERROR;
        }
    }

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == usmUserEntry_set) &&
            (((usmUserEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((usmUserEntry_t *) (dp->data))->usmUserEngineID, usmUserEngineID) == 0) &&
            (CmpOctetStrings(((usmUserEntry_t *) (dp->data))->usmUserName, usmUserName) == 0) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = usmUserEntry_set;
        dp->cleanupMethod = usmUserEntry_cleanup;
#ifdef SR_usmUserEntry_UNDO
        dp->undoMethod = usmUserEntry_undo;
#else /* SR_usmUserEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_usmUserEntry_UNDO */
        dp->state = SR_UNKNOWN;

        if (usmUserEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_usmUserEntry(usmUserEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_usmUserEntry(usmUserEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = (void *) malloc (sizeof(usmUserEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
#ifdef SR_CLEAR_MALLOC
                memset(dp->data, 0, sizeof(usmUserEntry_t));
#endif /* SR_CLEAR_MALLOC */
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((usmUserEntry_t *) (dp->data))->usmUserEngineID = CloneOctetString(usmUserEngineID);
                SET_VALID(I_usmUserEngineID, ((usmUserEntry_t *) (dp->data))->valid);

                ((usmUserEntry_t *) (dp->data))->usmUserName = CloneOctetString(usmUserName);
                SET_VALID(I_usmUserName, ((usmUserEntry_t *) (dp->data))->valid);

                error_status = k_usmUserEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(usmUserEngineID);
        FreeOctetString(usmUserName);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_usmUserCloneFrom
   case I_usmUserCloneFrom:

     if (((usmUserEntry_t *) (dp->data))->usmUserCloneFrom != NULL) {
        FreeOID(((usmUserEntry_t *) (dp->data))->usmUserCloneFrom);
     }

     ((usmUserEntry_t *) (dp->data))->usmUserCloneFrom = 
            CloneOID(value->oid_value);

     break;
#endif /* I_usmUserCloneFrom */

#ifdef I_usmUserAuthProtocol
   case I_usmUserAuthProtocol:

     if (((usmUserEntry_t *) (dp->data))->usmUserAuthProtocol != NULL) {
        FreeOID(((usmUserEntry_t *) (dp->data))->usmUserAuthProtocol);
     }

     ((usmUserEntry_t *) (dp->data))->usmUserAuthProtocol = 
            CloneOID(value->oid_value);

     break;
#endif /* I_usmUserAuthProtocol */

#ifdef I_usmUserAuthKeyChange
   case I_usmUserAuthKeyChange:

     if (((usmUserEntry_t *) (dp->data))->usmUserAuthKeyChange != NULL) {
        FreeOctetString(((usmUserEntry_t *) (dp->data))->usmUserAuthKeyChange);
     }

     ((usmUserEntry_t *) (dp->data))->usmUserAuthKeyChange = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_usmUserAuthKeyChange */

#ifdef I_usmUserOwnAuthKeyChange
   case I_usmUserOwnAuthKeyChange:
     if (((usmUserEntry_t *) (dp->data))->usmUserOwnAuthKeyChange != NULL) {
        FreeOctetString(((usmUserEntry_t *) (dp->data))->usmUserOwnAuthKeyChange);
     }

     ((usmUserEntry_t *) (dp->data))->usmUserOwnAuthKeyChange = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_usmUserOwnAuthKeyChange */

#ifdef I_usmUserPrivProtocol
   case I_usmUserPrivProtocol:

     if (((usmUserEntry_t *) (dp->data))->usmUserPrivProtocol != NULL) {
        FreeOID(((usmUserEntry_t *) (dp->data))->usmUserPrivProtocol);
     }

     ((usmUserEntry_t *) (dp->data))->usmUserPrivProtocol = 
            CloneOID(value->oid_value);

     break;
#endif /* I_usmUserPrivProtocol */

#ifdef I_usmUserPrivKeyChange
   case I_usmUserPrivKeyChange:

     if (((usmUserEntry_t *) (dp->data))->usmUserPrivKeyChange != NULL) {
        FreeOctetString(((usmUserEntry_t *) (dp->data))->usmUserPrivKeyChange);
     }

     ((usmUserEntry_t *) (dp->data))->usmUserPrivKeyChange = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_usmUserPrivKeyChange */

#ifdef I_usmUserOwnPrivKeyChange
   case I_usmUserOwnPrivKeyChange:

     if (((usmUserEntry_t *) (dp->data))->usmUserOwnPrivKeyChange != NULL) {
        FreeOctetString(((usmUserEntry_t *) (dp->data))->usmUserOwnPrivKeyChange);
     }

     ((usmUserEntry_t *) (dp->data))->usmUserOwnPrivKeyChange = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_usmUserOwnPrivKeyChange */

#ifdef I_usmUserPublic
   case I_usmUserPublic:

     if ( ((value->os_value->length < 0) || (value->os_value->length > 32)) ) {
         return WRONG_LENGTH_ERROR;
     }

     if (((usmUserEntry_t *) (dp->data))->usmUserPublic != NULL) {
        FreeOctetString(((usmUserEntry_t *) (dp->data))->usmUserPublic);
     }

     ((usmUserEntry_t *) (dp->data))->usmUserPublic = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_usmUserPublic */

#ifdef I_usmUserStorageType
   case I_usmUserStorageType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return WRONG_VALUE_ERROR;
     }

     if (usmUserEntry != NULL) {
         /* check for attempts to change 'permanent' to other value */
         if ((usmUserEntry->usmUserStorageType == D_usmUserStorageType_permanent) &&
             (value->sl_value != D_usmUserStorageType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
         /* check for attempts to change 'permanent' to other value */
         if ((usmUserEntry->usmUserStorageType < D_usmUserStorageType_permanent) &&
             (value->sl_value >= D_usmUserStorageType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
     }
     ((usmUserEntry_t *) (dp->data))->usmUserStorageType = value->sl_value;
     break;
#endif /* I_usmUserStorageType */

#ifdef I_usmUserStatus
   case I_usmUserStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     error_status = CheckRowStatus(value->sl_value,
         usmUserEntry == NULL ? 0L : usmUserEntry->usmUserStatus);
     if (error_status != 0) return(error_status);

     /* check for attempts to delete 'permanent' rows */
     if(usmUserEntry != NULL
     && usmUserEntry->usmUserStorageType == D_usmUserStorageType_permanent
     && value->sl_value == D_usmUserStatus_destroy) {
         /* permanent rows can be changed but not deleted */
         return INCONSISTENT_VALUE_ERROR;
     }

     ((usmUserEntry_t *) (dp->data))->usmUserStatus = value->sl_value;
     break;
#endif /* I_usmUserStatus */

#ifdef I_usmTargetTag
   case I_usmTargetTag:

     if ( ((value->os_value->length < 0) || (value->os_value->length > 255)) ) {
         return WRONG_LENGTH_ERROR;
     }

     if (((usmUserEntry_t *) (dp->data))->usmTargetTag != NULL) {
        FreeOctetString(((usmUserEntry_t *) (dp->data))->usmTargetTag);
     }

     ((usmUserEntry_t *) (dp->data))->usmTargetTag = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_usmTargetTag */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in usmUserEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_usmUserEntry_test */
   error_status = k_usmUserEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((usmUserEntry_t *) (dp->data))->valid);
        error_status = k_usmUserEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
usmUserEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_usmUserEntry_set((usmUserEntry_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */
