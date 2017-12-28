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

/*
 *  Contains functions for translating between OID descriptors and human-
 *  friendly instances to the dotted-decimal OID names and instances.
 */

#include "sr_conf.h"


#include <stdio.h>


#include <ctype.h>

#include <stdlib.h>



#include <string.h>


#ifdef WANT_SSCANF_PROTO
extern int sscanf();
#endif	/* WANT_SSCANF_PROTO */

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "mib_tree.h"
#include "oid_lib.h"
#include "diag.h"
#include "frmt_lib.h"
#include "oidtranh.h"
SR_FILENAME


int IsTableMIBObject(OID_TREE_ELE *ote);

/*
 *  IsTableMIBObject()
 *
 *  Function:  Given an OID_TREE_ELE, this function tests to see if
 *             the OID_TREE_ELE identifies a MIB object that is a
 *             table element.
 *
 *  Inputs:    ote - the OID_TREE_ELE to test.
 *
 *  Outputs:    0 - the MIB object is NOT a table element.
 *             -1 - the MIB object IS a table element.
 *
 *  Notes:     To be considered a table element:
 *
 *             -  the MIB object OID_TREE_ELE must NOT be NULL;
 *             -  the MIB object's parent OID_TREE_ELE must NOT be NULL;
#ifndef APO_LEVEL_2
 *             -  the MIB object must NOT be an aggregate type;
#endif 
 *             -  the MIB object's parent MUST be an aggregate type;
 *             -  the MIB object and it's parent must both be in
 *                the same OID subtree.
 */
int
IsTableMIBObject(OID_TREE_ELE *ote)
{
    OID           *tnoid = NULL;
    OID_TREE_ELE  *parent = NULL;

    /* --- Make sure the OID_TREE_ELE is not NULL --- */
    if (ote == NULL) {
        return 0;
    }

    /* --- Get the parent OID_TREE_ELE and make sure it is not NULL --- */
    tnoid  = MakeOIDFragFromDot(ote->oid_number_str);
    tnoid->length -= 1;
    parent = AP_GetMIBNodeFromOID(tnoid);
    FreeOID(tnoid);
    tnoid  = NULL;

    if (parent == NULL) {
        return 0;
    }

    /* --- Make sure the OID_TREE_ELE is a leaf --- */
    if ((ote->type == AGGREGATE_TYPE) || (ote->type == NON_LEAF_TYPE)) {
        FreeOTE(parent);
        return 0;
    }

    /* --- Make sure the parent is an aggregate type --- */
    if (parent->type != AGGREGATE_TYPE) {
        FreeOTE(parent);
        return 0;
    }

    /*  Make sure the OID_TREE_ELE and parent are in the same subtree  */
    if (strncmp(ote->oid_number_str,
                parent->oid_number_str,
                strlen(parent->oid_number_str))) {
        FreeOTE(parent);
        return 0;
    }

    /* --- ote is a table element --- */
    FreeOTE(parent);
    return -1;
}





/*
 *  NewToken()
 *
 *  Function:  Given an string and start/end positions, this function picks
 *             off the specified substring and converts it into an entry in
 *             a token array.
 *
 *  Inputs:    string      - the string to extract a substring from.
 *             start       - start position of the substring.
 *             end         - end position of the substring.
 *             tokens      - an array of token strings.
 *             token_count - the number of tokens in the token array.
 *
 *  Outputs:   On failure, returns -1 and does not change the token list.
 *             On success, returns 0, and extracts the specified substring 
 *             and adds it to the end of the token list.
 *
 *  Notes:     The input string is not changed.
 */
static int
NewToken(const char  *string, 
         int   start, 
         int   end, 
         char   ***tokens, 
         int    *token_count)
{
   int   len, lcount;
   char  *token = NULL, **old_tokens = NULL, **new_tokens = NULL;

   /*
    *   Error-check the arguments.
    */
   if (string == NULL) {
       return -1;
   }

   len = strlen(string);

   if ((start < 0) || (start > len)) {
       return -1;
   }

   if ((end < 0) || (end > len)) {
       return -1;
   }

   if (start > end) {
       return -1;
   }

   if (tokens == NULL) {
       return -1;
   }

   if (*token_count < 0) {
       return -1;
   }

   /* --- Increment the token count --- */
   lcount = *token_count;
   lcount++;

   /*
    *  Resize the token list.  Note the token list is always 1 greater
    *  than the number of tokens to allow for a NULL entry at the end.
    */
   old_tokens = *tokens;
   new_tokens = (char **) realloc(old_tokens, ((lcount+1) * sizeof(char *)));
   if (new_tokens == NULL) {
       return -1;
   }

   /* --- Clear the last elements of the token list --- */
   new_tokens[lcount-1] = NULL;
   new_tokens[lcount]   = NULL;

   /* --- Malloc memory for the new token --- */
   len = end - start + 1;
   token = (char *) malloc(len + 1);
   if (token == NULL) {
       return -1;
   }

   /* --- Copy the token --- */
   memcpy(token, &string[start], len);
   token[len] = '\0';

   /* --- Generate an empty token --- */
   if (start == end) {
       token[0] = '\0';
   }

   /* --- Add the token to the end of the token list --- */
   new_tokens[lcount-1] = token;

   /* --- Normal return --- */
   *tokens = new_tokens;
   *token_count = lcount;
   return 0;
}

/*
 * ConvertToDotFormat()
 *
 * Convert an OID with index values in brackets to a normal dotted form.
 * For example, ifDescr[1] or ifDescr[ifIndex=1] becomes ifDescr.1.
 *
 * The purpose is to get the string into the format that ParseHFString
 * expects.
 *
 * The return value is a string which is dynamically allocated and
 * therefore must be freed.
 */
static char *
ConvertToDotFormat(const char *string)
{
    char *newstring;
    int i, j;
    int start_pos = 0;

    if (string == NULL) return NULL;
    if ((newstring = strdup(string)) == NULL) {
        return NULL;
    }

    if (strchr(string, '[') == NULL) {
        return newstring;
    }
    for (i = 0, j = 0; string[i]; i++) {
        if (string[i] == '[') {
            newstring[j] = '.'; 
            j++;
            start_pos = j;
            continue;
        }
        if (string[i] == ']') {
            start_pos = 0;
            continue;
        }
        if ((string[i] == '=') && (start_pos != 0)) {
            j = start_pos;
            continue;
        }
        newstring[j] = string[i];
        j++;
    } 
    newstring[j] = '\0';
    return newstring;
}

/*
 *  Parser states.
 *
 *  ST_EXPECT_OBJ  - The starting state; no valid, non-blank characters 
 *                   have been processed.
 *  ST_OBJ_DESCR   - Indicates an alphanumeric MIB object descriptor
 *                   is being processed.
 *  ST_OBJ_NAME    - Indicates a dotted-decimal MIB object OID name
 *                   is being processed.
 *  ST_EXPECT_INST - Indicates the end of the MIB object name or
 *                   descriptor has been found.
 *  ST_ALPHA_INST  - Indicates an alphanumeric instance is being processed.
 *  ST_DOTDEC_INST - Indicates a dotted-decimal instance is being processed.
 *  ST_ERROR       - The error state.
 */
#define ST_EXPECT_OBJ    0x01
#define ST_OBJ_DESCR     0x02
#define ST_OBJ_NAME      0x03
#define ST_EXPECT_INST   0x04
#define ST_ALPHA_INST    0x05
#define ST_DOTDEC_INST   0x06
#define ST_ERROR         0x07


/*
 *  ParseHFString()
 *
 *  Function:  Parses an OID descriptor and instances into "pieces".
 *             A "piece" is either alphanumeric or dotted-decimal.
 *
 *  Inputs:    string - the string to parse.
 *
 *  Outputs:   On failure, return NULL.  On success, returns a token list.
 *
 *  Notes:     The input string is not changed.
 */
char **
ParseHFString(const char *string)
{
   int   i, len, state, t_start, t_end, last_dot_pos, token_count;
   char  ch, **tokens = NULL;
   char  *newstring;

   /*
    *   Error-check the arguments.
    */
   if (string == NULL) {
       return NULL;
   }

   newstring = ConvertToDotFormat(string);

   len = strlen(newstring);

   /* --- Start in the "Expect MIB Object" state --- */
   state = ST_EXPECT_OBJ;
   token_count = 0;

   /* --- Loop through the characters in the string --- */
   for (i = 0; i < len; i++) {

       /* --- Extract the test character --- */
       ch = newstring[i];

       /* --- Save the most recent position of a dot character --- */
       if (ch == '.') {
           last_dot_pos = i;
       }


       /* --- Branch based upon the current state --- */
       switch (state) {


           /* --- State "Expect MIB Object" --- */
           case ST_EXPECT_OBJ:

               /* --- Ignore leading blanks --- */
               if (ch == ' ') {
                   break;
               }

               /*
                *  If the first character is numeric, then
                *  change to the "MIB Object Name" State. 
                */
               if (isdigit((unsigned char)ch)) {
                   t_start = i;   /* Save token start position */
                   state   = ST_OBJ_NAME;
                   break;
               }

               /*
                *  If the first character is lowercase alphabetic, then 
                *  change to the "MIB Object Descriptor" State. 
                */
               if (islower((unsigned char)ch)) {
                   t_start = i;   /* Save token start position */
                   state   = ST_OBJ_DESCR;
                   break;
               }

               /* --- Otherwise, we have an error --- */
               state = ST_ERROR;
               break;


           /* --- State "MIB Object Name" --- */
           case ST_OBJ_NAME:

               /*
                *  If the character is numeric or a dot, then 
                *  stay in the "MIB Object Name" State.
                */
               if (isdigit((unsigned char)ch) || (ch == '.')) {
                   break;
               }

               /*
                *  Any other character changes state to the "Alphanumeric
                *  Instance" state.  The end of the MIB Object Name is 1 
                *  before the last dot position.  Create a new token.  
                */
               state   = ST_ALPHA_INST;
               t_end   = last_dot_pos - 1;
               NewToken(newstring, t_start, t_end, &tokens, &token_count);
               t_start = last_dot_pos + 1;  /* Save token start position */
               break;


           /* --- State "MIB Object Descriptor" --- */
           case ST_OBJ_DESCR:

               /*
                *  If the character is alphabetic or numeric, then
                *  stay in the "MIB Object Descriptor" State.
                */
               if (isalnum((unsigned char)ch)) {
                   break;
               }

               /*
                *  If the character is a dot, then change to the "Expect
                *  Instance" state.  The end of the MIB Object Descriptor 
                *  is 1 before the current position.
                */
               if (ch == '.') {
                   state = ST_EXPECT_INST;
                   t_end = i - 1;
                   NewToken(newstring, t_start, t_end, &tokens, &token_count);
                   break;
               }

               /*
                *  Any other character changes state to the "Error" state.
                */
               state = ST_ERROR;
               break;


           /* --- State "Expect Instance" --- */
           case ST_EXPECT_INST:
               t_start = i;  /* Save the token starting position */

               /*
                *  If the character is a dot, then we have an empty token.
                *  Stay in the "Expect Instance" state.  Note that if the
                *  token start and end positions are the same, the NewToken()
                *  function adds an empty token.
                */
               if (ch == '.') {
                   NewToken(newstring, t_start, t_start, &tokens, &token_count);
                   t_start = last_dot_pos + 1;  /* Save token start position */
                   break;
               }

               /*
                *  If the character is numeric, then change to the "Dotted-
                *  Decimal Instance" state.  Otherwise, change to the "Alpha-
                *  Numeric Instance" state.
                */
               if (isdigit((unsigned char)ch)) {
                   state = ST_DOTDEC_INST;
               }
               else {
                   state = ST_ALPHA_INST;
               }

               break;


           /* --- State "Alphanumeric Instance" --- */
           case ST_ALPHA_INST:

               /*
                *  If the character is a dot, then change to the "Expect
                *  Instance" state.  The end of the instance is 1 before
                *  the current position.
                */
               if (ch == '.') {
                   state = ST_EXPECT_INST;
                   t_end = i - 1;
                   NewToken(newstring, t_start, t_end, &tokens, &token_count);
                   break;
               }

               /*
                *  Stay in the "Alphanumeric Instance" state for any
                *  other character.
                */
               break;


           /* --- State "Dotted-Decimal Instance" --- */
           case ST_DOTDEC_INST:

               /*
                *  If the character is numeric or a dot, then
                *  stay in the "Dotted-Decimal Instance" State.
                */
               if (isdigit((unsigned char)ch) || (ch == '.')) {
                   break;
               }

               /*
                *  Any other character changes state to the "Alphanumeric
                *  Instance" state.  The end of the instance is 1 before 
                *  the last dot position.  Create a new token.
                */
               state   = ST_ALPHA_INST;
               t_end   = last_dot_pos - 1;
               NewToken(newstring, t_start, t_end, &tokens, &token_count);
               t_start = last_dot_pos + 1;  /* Save token start position */
               break;


           /* --- Any other state is an error --- */
           default:
               state = ST_ERROR;
               break;
       }

       /* --- Handle error state --- */
       if (state == ST_ERROR) {
           goto error_return;
       }
   }

   
   /*
    *  Generate token from remaining (non-blank) piece.
    */
   if (state != ST_EXPECT_OBJ) {
       NewToken(newstring, t_start, i, &tokens, &token_count);
   }

   /* free the copy of string */
   free(newstring);
   
   /* --- Normal return --- */   
   return tokens;


   /*
    *   Error return point.
    */
error_return:

   /* free the copy of string */
   free(newstring);
   
   /* --- Return if no tokens --- */
   if (tokens == NULL) {
       return NULL;
   }

   /* --- Free the tokens --- */
   i = 0;
   while (tokens[i] != NULL) {
       free(tokens[i]);
       tokens[i] = NULL;
       i++;
    }

    /* --- Free the token array --- */
    free(tokens);

    return NULL;
}


/*
 *  HFInstStrToDotDec()
 *
 *  Function:  Translates human-friendly instance information into
 *             dotted-decimal instance information.
 *
 *  Inputs:    inst  - Points to the instance string.  This will be converted
 *                     either from an octet string or OID descriptor into a
 *                     string consisting of only dotted-decimal numbers.  On 
 *                     input, this points to string to convert.  If this func-
 *                     tion returns successfully, upon return, this will point
 *                     to a string of dotted-decimal numbers that specifies
 *                     the instance.
 *             ote   - Points to an OID_TREE_ELE for the MIB object for which
 *                     'inst' is an instance.  This 
 *             inum  - Specifies the token number, or rather the index number
 *                     of 'inst'.  
 *
 *  Outputs:   On failure, returns -1.  On success, returns zero.  On success,
 *             the inst has been converted to its dotted-decimal form.
 */
static int
HFInstStrToDotDec(char **inst, OID_TREE_ELE *ote, int inum)
{
    int           buf_size, CanFreeAote, i, implied, index_type;
    int           sid_count, status = 0;
    char          *buffer = NULL, sid_buf[8], *token = NULL;
    char          *index_oid_descr = NULL, *index_oid_name = NULL;
    OID_TREE_ELE  *aote  = NULL;  /* ote for aggregate MIB object */
    OID_TREE_ELE  *iote  = NULL;  /* ote for index MIB object */
    MIB_INDEX     *mi    = NULL;
    OID           *tnoid = NULL;

    /* --- Make sure arguments were passed --- */
    if ((inst == NULL) || (ote == NULL) || (inum < 1)) {
        return -1;
    }

    /* --- Can't convert if we have no instance string --- */
    if (*inst == NULL) {
        return -1;
    }

    /*
     *  The OID_TREE_ELE for the MIB object for which 'inst' is instancing
     *  information was passed into this function.  Now walk through the
     *  MIB index information.  Get the OID descriptors and whether or not
     *  it is an implied-length index.
     */
    aote = ote;

    /* --- No MIB indices for this OID_TREE_ELE --- */
    if (aote->mib_index == NULL) {
        /*
         *  If this OID_TREE_ELE is a table row element, then
         *  point to the parent so we can use the MIB indices.
         */
        if (IsTableMIBObject(aote)) {

            /* - Get the parent OID_TREE_ELE and make sure it is not NULL - */
            tnoid = MakeOIDFragFromDot(aote->oid_number_str);
            tnoid->length -= 1;
            aote  = AP_GetMIBNodeFromOID(tnoid);
            CanFreeAote = 1;
            FreeOID(tnoid);
            tnoid = NULL;
            if (aote == NULL) {
                return -1;
            }
        }
    }

    /* --- Return if we cannot find the correct MIB indices --- */
    if (aote->mib_index == NULL) {
        status = -1;
        goto cleanup;
    }

    /* --- Point to the index of interest --- */
    i = 1;
    for (mi = aote->mib_index; ((mi != NULL) && (i < inum)); mi = mi->next) {
        i++;
    }

    /* --- Return if there are few MIB indices than the one desired --- */
    if (i < inum) {
        status = -1;
        goto cleanup;
    }

    /*
     *  At this point, mi should point to the correct MIB index for 'inst'.
     *  Save the human-friendly OID descriptor and implied status.
     */
    index_oid_descr = mi->name;
    implied         = mi->implied;

    /* --- Get index MIB object's OID_TREE_ELE --- */
    iote = GetMIBNodeFromDot(mi->name);
    if (iote == NULL) {
        status = -1;
        goto cleanup;
    }

    /* --- Save the MIB object type of the index --- */
    index_type     = (int) iote->type;
    index_oid_name = iote->oid_number_str;

    /*
     *  At this point, the MIB object type of the index is known.
     */
/* got_index_type: */

    /* --- Estimate the size of the return string --- */
    switch (index_type) {
        case OBJECT_ID_TYPE:

            /*
             *  The index is type OID, so we must do one more lookup to
             *  convert the actual instance string passed into this func-
             *  tion into the dotted-decimal number needed for the output
             *  OID.
             */
            FreeOTE(iote);
            iote = GetMIBNodeFromDot(*inst);
            if (iote == NULL) {
                status = -1;
                goto cleanup;
            }

            index_oid_name = iote->oid_number_str;
            buf_size = strlen(index_oid_name) + 32;
            break;
        case OCTET_PRIM_TYPE:
            buf_size = (strlen(*inst) * 4) + 32;
            break;
        /* ---  Any other type is an error --- */
        default:
            status = -1;
            goto cleanup;
    }

    /* --- Malloc space for the return string --- */
    buffer = malloc(buf_size);
    if (buffer == NULL) {
        status = -1;
        goto cleanup;
    }
    buffer[0] = '\0';

    /* --- Convert instances that are human-friendly OID descriptors --- */
    if (index_type == OBJECT_ID_TYPE) {


        /* --- Number of SIDs is number of dots + 1 --- */
        sid_count = 1;
        for (i = 0; index_oid_name[i] != '\0'; i++) {
            if (index_oid_name[i] == '.') {
                sid_count++;
            }
        }

        /* --- Returned instance needs the number of SIDs --- */
        if (implied == 0) {
            sprintf(buffer, "%d.", sid_count);
        }
         
        /* --- Concatenate the OID --- */
        strcat(buffer, index_oid_name);

        /* --- Jettison trailing dot --- */
        i = strlen(buffer);
        if (buffer[i] == '.') {
            buffer[i] = '\0';
        }
    }

    /* --- Convert instances that are human-friendly octet strings  --- */
    else {

        /* --- Number of SIDs is number of characters --- */
        token     = *inst;
        sid_count = strlen(token);

        /* --- Returned instance needs the number of SIDs --- */
        if (implied == 0) {
            sprintf(sid_buf, "%d.", sid_count);
            strcat(buffer, sid_buf);
        }
        
        /* --- Convert each character into an SID --- */
        for (i = 0; i < sid_count; i++) {
            sprintf(sid_buf, "%d.", token[i]);
            strcat(buffer, sid_buf);
        }
        
        /* --- Jettison trailing dot --- */
        i = strlen(buffer);
        buffer[i-1] = '\0';
    }

/* done: */

    /* --- Free the input instance string --- */
    free(*inst);

    /* --- Return the converted instance string --- */
    *inst = buffer;

    status = 0;

    /* --- Free OTEs and return --- */
cleanup:
    if (CanFreeAote == 1) {
        FreeOTE(aote);
    }
    FreeOTE(iote);

    return status;
}



/*
 *  MakeOIDFragFromHFDot()
 *
 *  Function:  Similar to MakeOIDFragFromDot(), this function converts
 *             a text string into an OID fragment.  The difference is
 *             this function can convert octet string and OID descriptor
 *             indices, but MakeOIDFragFromDot() cannot.
 *
 *  Inputs:    text_str - the input text string to convert.
 *
 *  Outputs:   On success, returns an OID.  On failure, returns NULL.
 *
 *  Notes:     This function is intended to be used to convert user input.
 *             It has alot of error-checking and a longer code path than
 *             does MakeOIDFragFromDot().
 *
 *             This function cannot translate OID strings that includes
 *             SIDs in hexadecimal or octal.
 */
OID  *
MakeOIDFragFromHFDot(const char *text_str)
{
    int   token_count;
    char  **tokens = NULL;

    int   inum, IndexIsNumeric, ts_len, wbuflen;
    char  *clone    = NULL, *conv_buf = NULL;
    char  *temp_word  = NULL;
    char  *word = NULL;
    OID   *result = NULL, *tnoid = NULL;
    OID_TREE_ELE  *ote = NULL, *parent = NULL;
    MIB_INDEX *mi_head = NULL;

    /* --- Split the argument  into pieces --- */
    tokens = ParseHFString(text_str);

    /* --- No results --- */
    if (tokens == NULL) {
        DPRINTF((APPACKET, "%s:  Cannot parse text string.\n"));
        return NULL;
    }

    /* --- Count the number of pieces --- */
    token_count = 0;
    while (tokens[token_count] != NULL) {
        token_count++;
    }

    /*
     *  Let MakeOIDFragFromDot() handle dotted-decimal-only strings,
     *  or alphanumeric strings without instance information.
     */
    if (token_count == 1) {
        result = MakeOIDFragFromDot(text_str);
        goto done;
    }

    /*
     *  Let MakeOIDFragFromDot() handle strings with an initial alpha-
     *  betic descriptor and numeric-only instancing information.
     */
    if ((token_count == 2) && (IsDottedDecimal(tokens[1]) == -1)) {
        result = MakeOIDFragFromDot(text_str);
        goto done;
    }

    /*
     *   At this point, we have eliminated the cases where text_string
     *   is in one of the forms:
     *
     *       alpha               (alphabetic descriptor only)
     *       num.num.num.....    (dotted-decimal numeric only)
     *       alpha.num.num.....  (alphabetic descriptor, then numeric-
     *                            only instance)
     *
     *   At this point, the instancing information must contain human-
     *   friendly strings.  It must be one of the forms:
     *
     *       alpha.alpha.alpha...  (alphabetic descriptor and instances)
     *       num.num.num.....alph  (dotted-decimal descriptor and possibly
     *                              instance info, then alphas at the end.
     */

    /* --- Get the OID tree element of the MIB object --- */
    ote = GetMIBNodeFromDot(tokens[0]);
    if (ote == NULL) {
        result = NULL;
        goto done;
    }

    /*
     *  Text strings with numeric-only instance information have been elimin-
     *  ated above.  If ote is not a table MIB object, MIB_INDEX information
     *  for converting human-friendly instancing information to dotted-decimal
     *  is unavailable, so return NULL.
     */
    if (!IsTableMIBObject(ote)) { 
        result = NULL;
        goto done;
    }

    /* --- Get the parent OID_TREE_ELE and make sure it is not NULL --- */
    tnoid  = MakeOIDFragFromDot(ote->oid_number_str);
    tnoid->length -= 1;
    parent = AP_GetMIBNodeFromOID(tnoid);
    FreeOID(tnoid);
    tnoid  = NULL;
    if (parent == NULL) {
        goto done;
    }

    mi_head = parent->mib_index;
    if (mi_head == NULL) {
        result = NULL;
        goto done;
    }

    /* --- Determine the size of a work buffer --- */
    ts_len   = strlen(text_str);
    wbuflen  = (MAX_OID_SIZE * 11) + 256;

    if (wbuflen < ts_len) {
        wbuflen = ts_len;
    }

    /* --- Malloc memory for a work buffer --- */
    conv_buf = (char *) malloc(wbuflen);
    if (conv_buf == NULL) {
        goto done;
    }
    conv_buf[0] = '\0';

    /* --- Copy the dotted-decimal string to the work buffer --- */
    strcpy(conv_buf, ote->oid_number_str);

    /* --- Initialize for looping through all dot-delimited words --- */
    inum           = 0;
    IndexIsNumeric = 0;  /* Flag that indicates processing numeric index */
    word = tokens[inum+1];

    /* --- Loop through all dot-delimited words in the instance --- */
    while (word != NULL) {

        /* --- Determine if the word is dotted-decimal only --- */
        if (IsDottedDecimal(word)) {

            /* Increment index count only upon transition to numeric index */
            if (IndexIsNumeric == 0) {
                inum++;
            }

            /*
             *  Set a flag that indicates that all dot-delimited words
             *  that compose this index all must be dotted-decimal.
             */
            IndexIsNumeric = 1;

            /* --- Copy numeric-only words to conversion buffer --- */
            strcat(conv_buf, ".");
            strcat(conv_buf, word);
        }

        /* --- Convert tokens with embedded alphabetic characters --- */
        else {

            /*
             *  Reset the flag that indicates all words in this
             *  index all must be dotted-decimal.
             */
            IndexIsNumeric = 0;

            /* --- Processing a new table index --- */
            inum++;

            /* --- Clone the index word so it can be modified --- */
            temp_word = strdup(word);

            if (HFInstStrToDotDec(&temp_word, ote, inum)) {
                goto done;
            }

            /* --- Copy the converted word to the conversion buffer --- */
            strcat(conv_buf, ".");
            strcat(conv_buf, temp_word);

            /* --- Free the temp_word --- */
            free(temp_word);
            temp_word = NULL;
        }

        /* --- Get the next word --- */
        word = tokens[inum+1];
    }

    /* --- Convert the dotted-decimal into an OID --- */
    result = MakeOIDFragFromDot(conv_buf);

done:
    FreeOTE(ote);
    ote = NULL;
    FreeOTE(parent);
    parent = NULL;


    if (clone != NULL) {
        free(clone);
    }

    if (temp_word != NULL) {
        free(temp_word);
    }

    if (conv_buf != NULL) {
        free(conv_buf);
    }

    /* --- Free the tokens --- */
    token_count = 0;
    while (tokens[token_count] != NULL) {
        free(tokens[token_count]);
        tokens[token_count] = NULL;
        token_count++;
    }

    /* --- Free the token array --- */
    free(tokens);

    /* --- Return the result OID --- */
    return result;
}


/*
 *  MakeOIDFromHFDot()
 *
 *  Function:  Attempts to translate a human-friendly OID descriptor (with
 *             human-friendly instance information into dotted-decimal inst-
 *             instance information.  This function checks the OID to verify
 *             the OID is in a valid subtree (subtrees 1, 2, 3) from the root.
 *
 *  Inputs:    text_str - the input text string to convert.
 *
 *  Outputs:   On success, returns an OID.  On failure, returns NULL.
 *
 *  Notes:     This function is intended to be used to convert user input.
 *             It has a lot of error-checking and a longer code path than
 *             does MakeOIDFromDot().
 *
 *             This function cannot translate OID strings that include
 *             SIDs in hexadecimal or octal.
 */
OID            *
MakeOIDFromHFDot(const char *text_str)
{
    OID *oid_ptr = NULL;

    /* --- Convert the string to an OID --- */
    oid_ptr = MakeOIDFragFromHFDot(text_str);
    if (oid_ptr == NULL) {
        DPRINTF((APPACKET, "MakeOIDFromHFDot: MakeOIDFragFromHFDot(%s) failed\n", text_str));
        return NULL;
    }

    /* --- Verify this OID is in a valid subtree from the root --- */
    if (oid_ptr->oid_ptr[0] >= 4) {     /* we have a bogus OID */
        DPRINTF((APPACKET, "MakeOIDFromHFDot, illegal OID value "UINT32_FORMAT"\n", oid_ptr->oid_ptr[0]));
        FreeOID(oid_ptr);
        return NULL;
    }

    /* --- Return the OID --- */
    return oid_ptr;
}





/*
 *   OIDInstToHFInstStr()
 *
 *   Function:  Given an OID, the start of instance info, the OID_TREE_ELE,
 *              and implied status of an index, this function converts the
 *              SIDs for a single index to a human-friendly string.
 *
 *   Inputs:   oid     - the OID to convert.
 *             pos     - points to an integer that specifies which SID in
 *                       the OID is the starting point for this instance.
 *                       The pos argument is both an input and output.  Upon
 *                       input, *pos specifies which element of the SR_UINT32
 *                       array is the first SID of the instance.  Upon success-
 *                       ful return, *pos has been updated and specifies which 
 *                       element of the SR_UINT32 array is the first SID of 
 *                       the next instance.
 *             ote     - the OID_TREE_ELE of the index node in the MIB tree.
 *             implied - whether or not the index length is implied.
 *             behavior - the desired behavior
 *
 *   Outputs:  On failure, returns NULL.  On success, returns a human-friendly
 *             string which is the converted OID.  Upon success, this function
 *             also updates the *pos argument.
 */
static char *
OIDInstToHFInstStr(const OID *oid, int *pos, OID_TREE_ELE *ote, int implied,
                   int behavior)
{
    int           j, len = 0, lpos = 0, unprintable, type;
    char          *buf = NULL, *result = NULL, test_char;
    OID           *local_oid = NULL;

    /* --- Error-check the arguments --- */
    if ((oid == NULL) || (pos == NULL)) {
        return NULL;
    }
    if (ote == NULL) {
        return NULL;
    }
    if ((implied != 0) && (implied != 1)) {
        return NULL;
    }

    type = ote->type;

    /* --- Convenience argument.  Error-check it's value --- */
    lpos = *pos;
    if ((lpos < 1) || (lpos >= oid->length)) {
        return NULL;
    }

    /* --- Format the different OID types --- */
    switch(type) {

        case INTEGER_TYPE:

            /* --- Determine how many SIDs in the index --- */
            len = 1;

            /* --- Malloc space for the result string --- */
            result = malloc(16);
            if (result == NULL) {
                return NULL;
            }
            result[0] = '\0';

            sprintf(result, "%d", oid->oid_ptr[lpos]);
            break;


        case GAUGE_TYPE:

            /* --- Determine how many SIDs in the index --- */
            len = 1;

            /* --- Malloc space for the result string --- */
            result = malloc(16);
            if (result == NULL) {
                return NULL;
            }
            result[0] = '\0';

            sprintf(result, "%u", oid->oid_ptr[lpos]);
            break;


        case OBJECT_ID_TYPE:

            /* --- Determine how many SIDs in the index --- */
            if (implied == 1) {
                len = oid->length - lpos;
                len--;   /*  Fix Purify ABR  */
            }
            else {
                len = (int) oid->oid_ptr[lpos];
                lpos++;   /* Skip the length SID */
            }

            /* --- Create a local clone of the OID --- */
            local_oid = ClonePartialOID(oid, lpos, len);

            /* --- Malloc space for the result string --- */
            result = malloc(len * 11 + 256);
            if (result == NULL) {
                FreeOID(local_oid);
                return NULL;
            }
            result[0] = '\0';

            MakeDotFromOID(local_oid, result);

            FreeOID(local_oid);
            break;


        case IP_ADDR_PRIM_TYPE:
            /* --- Determine how many SIDs in the index --- */
            len = 4;

            /* --- Malloc space for the result string --- */
            result = malloc(len*11 + 32);
            if (result == NULL) {
                return NULL;
            }

            buf = result;
            for (j = 0; j < len; j++) {
                sprintf(buf, UINT32_FORMAT".", oid->oid_ptr[lpos + j]);
                buf += strlen(buf);
            }
            buf--;
            buf[0] = '\0';
            break;


        case OCTET_PRIM_TYPE:

            /* --- Determine how many SIDs in the index --- */
            if (implied == 1) {
                len = oid->length - lpos;
            }
            else {
                len = (int) oid->oid_ptr[lpos];
                lpos++;   /* Skip the length SID */
            }

            /* --- Verify all SIDs are printable --- */
            unprintable = 0;
            for (j = 0; j < len; j++) {
                test_char = (char) oid->oid_ptr[lpos + j];
                if (test_char < 0x20) {
                    unprintable = 1;
                }
            }

            if ((behavior & SR_HF_INDEX_NAME_FORM) == 0) {
                unprintable = 1;
            }

            /*
             *  Return unprintable octet strings as a character string
             *  containing a dotted-decimal number.
             */
            if (unprintable == 1) {

                /* --- If index is not implied, can't skip length byte --- */
                if (implied == 0) {
                    lpos--;
                    len = len + 1;
                }

                /* --- Malloc space for the result string --- */
                result = malloc(len*11 + 32);
                if (result == NULL) {
                    return NULL;
                }
                result[0] = '\0';

                buf = result + strlen(result);
                for (j = 0; j < len; j++) {
                    sprintf(buf, UINT32_FORMAT".", oid->oid_ptr[lpos + j]);
                    buf += strlen(buf);
                }
                buf--;
                buf[0] = '\0';
            }

            /*
             *  Render printable octet strings.
             */
            else {
                /* --- Malloc space for the result string --- */
                result = malloc(len + 1);
                if (result == NULL) {
                    return NULL;
                }
                result[len] = '\0';

                /* --- Loop through the SIDs and convert to octet string --- */
                for (j = 0; j < len; j++) {

                    /* --- Verify all SIDs are in-range --- */
                    if (oid->oid_ptr[lpos + j] > 255) {
                        free(result);
                        result = NULL;
                        goto done;
                    }

                    /* --- Convert SID to a character --- */
                    result[j] = (char) oid->oid_ptr[lpos + j];
                }

                /* --- Add the terminating NULL character --- */
                result[j] = '\0';
            }
            break;

        default:
            break;
    }

    /* --- Point to the first SID of the next instance --- */
    *pos = lpos + len;

done:
    /* --- Normal return --- */
    return result;
}


/*
 *  MakeHFDotFromOID()
 *
 *  Function:  Converts an OID into a text string, similar to MakeDotFromOID().
 *             If possible, this function converts table MIB object instances
 *             into human-friendly strings, unlike MakeDotFromOID().
 *
 *  Inputs:    oid      - the OID to convert.
 *             buffer   - points to a character buffer in which to return
 *                        the text string result.
 *             bufsize  - the number of characters in the return buffer.
 *             behavior - the desired behavior
 *
 *  Notes:     This function is intended to be used to render user output.
 *             It has a longer code path than does MakeOIDFragFromDot().
 */

int  
MakeHFDotFromOID(const OID  *oid,
                 char       *buffer,
                 int        bufsize,
                 int        behavior)
{
    FNAME("MakeHFDotFromOID")

    int           blen, olen, tolen, status = 0;
    int           i;
    char          *buf = NULL, *HFString = NULL;
    MIB_INDEX     *mi  = NULL;
    OID_TREE_ELE  *iote = NULL, *ote = NULL, *parent = NULL;
    OID           *tnoid = NULL;

    /* --- Make sure arguments were specified --- */
    if ((oid == NULL) || (oid->length == 0) || (oid->oid_ptr == NULL)) {
        DPRINTF((APPACKET, "%s:  Invalid OID specified.\n", Fname));
        return -1;
    }

    if ((buffer == NULL) || (bufsize < 1)) {
        DPRINTF((APPACKET, "%s:  Invalid string buffer specified.\n", Fname));
        return -1;
    }

    memset(buffer, 0x00, bufsize);
    buffer[0] = '\0';
    olen = 0;

    /* --- Get the OID tree element of the input OID --- */
    ote   = AP_GetMIBNodeFromOID(oid);

    /* --- If no OID_TREE_ELE, then render as a dotted-decimal string --- */
    if (ote == NULL || ((behavior & SR_HF_NUMBER_FORM) == SR_HF_NUMBER_FORM)) {
        if (ote == NULL) {
           DPRINTF((APPACKET, "%s:  Unable to find OID in MIB tree.\n", Fname));
        }

        /* --- Estimate the length of the result text string --- */
        blen = oid->length * 11 + 32;  
        if (bufsize < blen) {
            DPRINTF((APPACKET, "%s:  String buffer too small.\n", Fname));
            FreeOTE(ote);
            ote = NULL;
            status = -1;
            goto cleanup;
        }

        /* --- Render the OID and return --- */
        SPrintOID(oid, buffer);
        FreeOTE(ote);
        ote = NULL;
        status = 0;
        goto cleanup;
    }

    /* --- Set olen to the number of SIDS in the descriptor of the OID --- */
    tnoid = MakeOIDFragFromDot(ote->oid_number_str);
    olen  = tnoid->length;
    FreeOID(tnoid);
    tnoid  = NULL;


    /* --- Verify the output buffer is large enough for MIB object descr --- */
    if (bufsize < (strlen(ote->oid_name)+2)) {
        DPRINTF((APPACKET, "%s:  String buffer too small.\n", Fname));
        FreeOTE(ote);
        ote = NULL;
        status = -1;
        goto cleanup;
    }

    /* --- Render the OID descriptor into the string buffer --- */
    if (ote != NULL) {
        sprintf(buffer, "%s", ote->oid_name);
    } else {
        olen = 0;
    }

    /* --- If object is not tabular, then render SIDs as dotted-decimal --- */
    if (!IsTableMIBObject(ote)) {
        goto render_remaining_sids;
    }
    /* --- Back up over the trailing dot since --- */
    buf = buffer + strlen(buffer);
    buf[0] = '\0';

    /* --- Get the parent OID_TREE_ELE --- */
    tnoid  = MakeOIDFragFromDot(ote->oid_number_str);
    tnoid->length -= 1;
    parent = AP_GetMIBNodeFromOID(tnoid);
    FreeOID(tnoid);
    tnoid  = NULL;

    if ((behavior & SR_HF_ABBREVIATE_NAME) == SR_HF_ABBREVIATE_NAME) {
        /* skip the common prefix */
        for (i = 0; ote->oid_name[i] == parent->oid_name[i]; i++);
        sprintf(buffer, "%s", &(ote->oid_name[i]));
    }

    /* --- Free the initial OTE --- */
    FreeOTE(ote);
    ote = NULL;

    /* --- If no parent ote, then render SIDs as dotted-decimal --- */
    ote    = parent;
    parent = NULL;
    if (ote == NULL) {
        goto render_remaining_sids;
    }

    /* --- If no MIB indices, then render SIDs as dotted-decimal --- */
    mi = ote->mib_index;
    if (mi == NULL) {
        goto render_remaining_sids;
    }

    /* 
     *  Verify there is sufficient information to render the table
     *  indices in human-friendly form.
     */
    blen = 0;
    tolen = olen;
    while (mi != NULL) {
        /* --- If no MIB object name, render SIDs as dotted-decimal --- */
        if (mi->name == NULL) {
            goto render_remaining_sids;
        }

        /* --- If no ote for index, render SIDs as dotted-decimal --- */
        iote = GetMIBNodeFromDot(mi->name);
        if (iote == NULL) {
            goto render_remaining_sids;
        }

        /* --- If HF rendering failure, render SIDs as dotted-decimal --- */
        HFString = OIDInstToHFInstStr(oid, &tolen, iote, mi->implied, 0);
        FreeOTE(iote);
        iote = NULL;
        if (HFString == NULL) {
            goto render_remaining_sids;
        }

        /* --- Accumulate lengths of index strings --- */
        blen = blen + strlen(HFString);
        free(HFString);

        /* --- Next table index --- */
        mi = mi->next;
    }

    /* --- Verify the output buffer is large enough for MIB object descr --- */
    if (bufsize < (strlen(buffer) + blen)) {
        DPRINTF((APPACKET, "%s:  String buffer too small.\n", Fname));
        status = -1;
        goto cleanup;
    }

    /*
     *  Render indices in human-friendly form.
     */
    mi = ote->mib_index;
    while (mi != NULL) {

        /* --- Get the OID_TREE_ELE of the MIB object index --- */
        iote = GetMIBNodeFromDot(mi->name);

        HFString = OIDInstToHFInstStr(oid, &olen, iote, mi->implied, behavior);

        /* --- Concatenate human-friendly instance info --- */
        if (HFString != NULL) {
            if ((behavior & SR_HF_BRACKETS) == SR_HF_BRACKETS) {
                strcat(buffer, "[");
            } else {
                strcat(buffer, ".");
            }
            if ((behavior & SR_HF_VERBOSE_INDEX) == SR_HF_VERBOSE_INDEX) {
                strcat(buffer, iote->oid_name);
                strcat(buffer, "=");
            }
            strcat(buffer, HFString);
            if ((behavior & SR_HF_BRACKETS) == SR_HF_BRACKETS) {
                strcat(buffer, "]");
            }
            free(HFString);
            HFString = NULL;
        }

        FreeOTE(iote);
        iote = NULL;

        /* --- Next table index --- */
        mi = mi->next;
    }


    /* 
     *  Render the remaining SIDs.
     */
render_remaining_sids:

    /* --- Estimate the length of the result text string --- */
    blen = (oid->length - olen) * 11 + 32;
    if (bufsize < blen) {
        DPRINTF((APPACKET, "%s:  String buffer too small.\n", Fname));
        status = -1;
        goto cleanup;
    }

    /* --- Render the SIDs --- */
    if (olen < oid->length) {
        buf = buffer + strlen(buffer);
        for ( ; olen < oid->length ; olen++) {
            sprintf(buf, "."UINT32_FORMAT, oid->oid_ptr[olen]);
            buf += strlen(buf);
        }
        buf[0] = '\0';
    }

cleanup:
    /* --- Normal return --- */
    FreeOTE(ote);
    ote = NULL;

    FreeOTE(parent);
    parent = NULL;
    return 0;
}

