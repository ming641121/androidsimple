/*
 *
 * Copyright (C) 2005 by Ahoya Networks Incorporated.
 *
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "time.h"
#include "axss_types.h"
#include "axss_if.h"
#include "clicmd_if.h"

extern SHELL_VAR_t     *shellVarPtr;
extern U32_t menuSelected;

#define ctagBuf (shellVarPtr->ctagBuf)

STATUS_t
rtrv_hdr(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   if (!menuSelected)
       printf("M <%s> COMPLD\n;\n", ctagBuf);
   return(OK);
}


STATUS_t
rtrv_gps_tfom(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   return(OK);
}

#if 0
STATUS_t
rtrv_osc(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   return(OK);
}

STATUS_t
rtrv_th_link(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   return(OK);
}
#endif
STATUS_t
ed_ant(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   return(OK);
}

STATUS_t
rtrv_pm_link(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   return(OK);
}

STATUS_t
ed_timing_output(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   return(OK);
}

STATUS_t rtrv_cond_com_aid(U8_t *inputstring, U32_t *input1, MENU_DATA_VAL_t *input2){

if (strncmp(inputstring, "COM", 3) == 0){
		return OK;
		
	}else{
		return ERROR;
	}
}

STATUS_t rtrv_alm_all_aid(U8_t *inputstring, U32_t *input1, MENU_DATA_VAL_t *input2){

if (strncmp(inputstring, "ALL", 3) == 0){
		return OK;
		
	}else{
		return ERROR;
	}
}