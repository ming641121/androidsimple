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

extern pCLIUMAPSTRUC_t larusCapMAP;


#define MAX_USER_NAME_LEN    10
#define MAX_USER_PSWD_LEN    10
#define MAX_USERS_PROFILE    30
#define USER_PROFILE_FILE_NAME  "userprofile"


typedef struct {
    U8_t   inUse;
    U16_t  accessLvl;
    U8_t   userName[MAX_USER_NAME_LEN+1];
    U8_t   pswd[MAX_USER_PSWD_LEN+1];
    time_t time;
    U32_t  cnt;
} USER_PROF_t;

USER_PROF_t userProfile[MAX_USERS_PROFILE];

extern S32_t accessLvl;
extern SHELL_VAR_t     *shellVarPtr;
extern U32_t menuSelected;

#define ctagBuf (shellVarPtr->ctagBuf)

initUserProfile()
{
   FILE  *fp;
   U32_t len;

   fp = fopen(USER_PROFILE_FILE_NAME, "r");
   if (fp == NULL)
   {
      bzero(userProfile, sizeof(USER_PROF_t) * MAX_USERS_PROFILE);
      strcpy(userProfile[0].userName, "LARUS");
      strcpy(userProfile[0].pswd, "LARUS");
      userProfile[0].inUse = 1;
      userProfile[0].accessLvl = 0;
      strcpy(userProfile[1].userName, "SYSADMIN");
      strcpy(userProfile[1].pswd, "-VISTA-");
      userProfile[1].inUse = 1;
      userProfile[1].accessLvl = A9;

      fp = fopen(USER_PROFILE_FILE_NAME, "w");
      fwrite(userProfile, sizeof(USER_PROF_t), MAX_USERS_PROFILE, fp);
   }
   else
   {
      len = fread(userProfile, sizeof(USER_PROF_t), MAX_USERS_PROFILE, fp);
      if (len != (MAX_USERS_PROFILE))
      {
         printf("user profile read length not correct %u\n", len);
      }
   }
   fclose(fp);
   return(OK);
}

STATUS_t
saveUserProfile()
{
   FILE  *fp;

   fp = fopen(USER_PROFILE_FILE_NAME, "w");
   if (fp == NULL)
   {
      return(ERROR);
   }

   fwrite(userProfile, sizeof(USER_PROF_t), MAX_USERS_PROFILE, fp);
   fclose(fp);
   return(OK);
}


STATUS_t
act_user(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   U32_t  i;

   if (shellVarPtr->inLogin)
   {
      if (!menuSelected)
         tl1PrintErr(ctagBuf, "ICNP");
      return(ERROR);
   }
   for (i = 0; i < MAX_USERS_PROFILE; i++)
   {
      if (userProfile[i].inUse)
      {
         if (!strmatch(dataVal[0].u.str, userProfile[i].userName)) 
         {
            if (!strmatch(dataVal[1].u.str, userProfile[i].pswd))
            {
               if (!menuSelected)
               {
                  if (userProfile[i].time)
	          {
                     struct tm   *p_today;
                     U8_t        dateTime[20];

                     p_today = (struct tm *)localtime(&userProfile[i].time);
                     strftime(dateTime, 20,"%Y-%m-%d %H:%M:%S", p_today);
                     printf("M <%s> COMPLD\n  \"<%s>[:<%s>,<%u>]\"\n;\n",
                       ctagBuf, userProfile[i].userName, dateTime, userProfile[i].cnt);
                  }
                  else
                     printf("M <%s> COMPLD\n  \"<%s>\"\n;\n",
                       ctagBuf, userProfile[i].userName);
                  userProfile[i].cnt = 0;
                  time(&userProfile[i].time);
               }
               strcpy(shellVarPtr->userName, userProfile[i].userName);
               accessLvl = userProfile[i].accessLvl;
               return(USER_LOGIN);
	    }
            else
	    {
               userProfile[i].cnt++;
               tl1PrintErr(ctagBuf, "PIUI");
               return(ERROR);
	    }
	 }
      }
   }
   if (userProfile[i].inUse)
      tl1PrintErr(ctagBuf, "PIUI");
   return(ERROR);
}


STATUS_t
canc_user(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   if (!strmatch(dataVal[0].u.str, shellVarPtr->userName)) 
   {
      shellVarPtr->userName[0] = '\0';
      if (!menuSelected)
         printf("M <%s> COMPLD\n;\n", ctagBuf);
      return(USER_LOGOUT);
   }
   tl1PrintErr(ctagBuf, "PIUC");
   return(ERROR);
}

STATUS_t
ent_user_secu(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   U32_t i;
   U8_t  *str;

   str = dataVal[0].u.str;
   if (!strmatch("all", str))
   {
      tl1PrintErr(ctagBuf, "PIUC");
      return(ERROR);
   }
   while (*str)
   {
      if (*str == ' ' || *str == ',' || *str == ':' ||
          *str == ';' || *str == '<' || *str == '>' || 
          *str == '[' || *str == ']' || *str == '{' || 
          *str == '}' || *str == '|' || *str == '+' ||
          *str == '&' || *str == '=' || *str == '^' ||
          *str == '/' || *str == '\\')
      {
         tl1PrintErr(ctagBuf, "PIUC");
         return(ERROR);

      }
      str++;
   }
   if ((str -  dataVal[0].u.str) > MAX_USER_NAME_LEN)
   {
      tl1PrintErr(ctagBuf, "PIUC");
      return(ERROR);
   }
   str = dataVal[1].u.str;
   if (!strmatch("all", str))
   {
      tl1PrintErr(ctagBuf, "PIPW");
      return(ERROR);
   }
   while (*str)
   {
      if (*str == ' ' || *str == ',' || *str == ':' ||
          *str == ';' || *str == '<' || *str == '>' || 
          *str == '[' || *str == ']' || *str == '{' || 
          *str == '}' || *str == '|' || *str == '+' ||
          *str == '&' || *str == '=' || *str == '^' ||
          *str == '/' || *str == '\\')
      {
         tl1PrintErr(ctagBuf, "PIPW");
         return(ERROR);
      }
      str++;
   }
   if ((str -  dataVal[1].u.str) > MAX_USER_PSWD_LEN)
   {
      tl1PrintErr(ctagBuf, "PIPW");
      return(ERROR);
   }

   for (i = 0; i < MAX_USERS_PROFILE; i++)
   /* is that user exist */
   {
      if (userProfile[i].inUse)
      {
         if (!strmatch(dataVal[0].u.str, userProfile[i].userName)) 
         {
            tl1PrintErr(ctagBuf, "SAAS");
            return(ERROR);
         }
      }
   }

   for (i = 0; i < MAX_USERS_PROFILE; i++)
   {
      if (!userProfile[i].inUse)
      {
         strcpy(userProfile[i].userName, dataVal[0].u.str);
         strcpy(userProfile[i].pswd, dataVal[1].u.str);
         userProfile[i].inUse = 1;
         userProfile[i].accessLvl = dataVal[2].u.val[0];
         userProfile[i].cnt = 0;
         userProfile[i].time = 0;
         saveUserProfile();
         if (!menuSelected)
            printf("M <%s> COMPLD\n;\n", ctagBuf);
         return(OK);
      }
   }
   tl1PrintErr(ctagBuf, "SROF");
   return(ERROR);
}

STATUS_t
ed_user_secu(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   U8_t  *str;
   U32_t i;
   U32_t inx;

   if (!dataVal[1].ttlVal && !dataVal[2].ttlVal && !dataVal[1].ttlVal)
   /* nothing to change */
   {
      tl1PrintErr(ctagBuf, "IPMS");
      return(ERROR);
   }


   if (dataVal[0].ttlVal > 1 && dataVal[1].ttlVal)
   /* can not change two user to same name */
   {
      tl1PrintErr(ctagBuf, "IIPG");
      return(ERROR);
   }
      

   if (dataVal[1].ttlVal)
   {
      str = dataVal[1].u.str;
      while (*str)
      {
         if (*str == ' ' || *str == ',' || *str == ':' ||
          *str == ';' || *str == '<' || *str == '>' || 
          *str == '[' || *str == ']' || *str == '{' || 
          *str == '}' || *str == '|' || *str == '+' ||
          *str == '&' || *str == '=' || *str == '^' ||
          *str == '/' || *str == '\\')
         {
            tl1PrintErr(ctagBuf, "PIUC");
            return(ERROR);

         }
         str++;
      }
      if ((str -  dataVal[1].u.str) > MAX_USER_NAME_LEN)
      {
         tl1PrintErr(ctagBuf, "PIUC");
         return(ERROR);
      }
      if (strmatch(dataVal[0].u.str, dataVal[1].u.str))
      {
         for (i = 0; i < MAX_USERS_PROFILE; i++)
         /* is that user exist */
         {
            if (userProfile[i].inUse)
            {
               if (!strmatch(dataVal[1].u.str, userProfile[i].userName)) 
               {
                  tl1PrintErr(ctagBuf, "PIUC");
                  return(ERROR);
               }
            }
         }
      }
   }
   if (dataVal[2].ttlVal)
   {

      str = dataVal[2].u.str;
      while (*str)
      {
         if (*str == ' ' || *str == ',' || *str == ':' ||
          *str == ';' || *str == '<' || *str == '>' || 
          *str == '[' || *str == ']' || *str == '{' || 
          *str == '}' || *str == '|' || *str == '+' ||
          *str == '&' || *str == '=' || *str == '^' ||
          *str == '/' || *str == '\\')
         {
            tl1PrintErr(ctagBuf, "PIPW");
            return(ERROR);
         }
         str++;
      }
      if ((str -  dataVal[2].u.str) > MAX_USER_PSWD_LEN)
      {
         tl1PrintErr(ctagBuf, "PIPW");
         return(ERROR);
      }
   }

 
   if (!menuSelected)
      printf("M <%s> RTRV\n", ctagBuf);
   for (i = 0; i < dataVal[0].ttlVal; i++)
   {
      inx = dataVal[0].u.val[i];
      if (inx < 2)
      {
         if (dataVal[1].ttlVal || dataVal[2].ttlVal)
	     /* can not change default user & password */
	 {
            if (!menuSelected)
               tl1PrintErr(ctagBuf, "IPNV");
            return(ERROR);
         }
      }
  
      if (userProfile[inx].inUse)
      {
         if (dataVal[1].ttlVal)
            strcpy(userProfile[inx].userName, dataVal[1].u.str);
         if (dataVal[2].ttlVal)
            strcpy(userProfile[inx].pswd, dataVal[2].u.str);
         if (dataVal[3].ttlVal)
            userProfile[inx].accessLvl = dataVal[3].u.val[0];
      }
   }
   if (!menuSelected)
      printf("M <%s> COMPLD\n;\n", ctagBuf);
   return(OK);
}

STATUS_t
ed_pid(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   U32_t i;
   U8_t  *str;

   str = dataVal[1].u.str;
   while (*str)
   {
      if (*str == ' ' || *str == ',' || *str == ':' ||
       *str == ';' || *str == '<' || *str == '>' || 
       *str == '[' || *str == ']' || *str == '{' || 
       *str == '}' || *str == '|' || *str == '+' ||
       *str == '&' || *str == '=' || *str == '^' ||
       *str == '/' || *str == '\\')
      {
         tl1PrintErr(ctagBuf, "PIPW");
         return(ERROR);
      }
      str++;
   }
   if ((str -  dataVal[1].u.str) > MAX_USER_PSWD_LEN)
   {
      tl1PrintErr(ctagBuf, "PIPW");
      return(ERROR);
   }

   for (i = 0; i < MAX_USERS_PROFILE; i++)
   {
      if (userProfile[i].inUse)
      {
         if (!strmatch(userProfile[i].userName, shellVarPtr->userName)) 
         {
            if (!strmatch(userProfile[i].pswd, dataVal[0].u.str))
	    {
               strcpy(userProfile[i].pswd, dataVal[1].u.str);
               if (!menuSelected)
                  printf("M <%s> COMPLD\n;\n", ctagBuf);
               return(OK);
	    }
            tl1PrintErr(ctagBuf, "PIPW");
            return(ERROR);
	 }
      }
   }
   tl1PrintErr(ctagBuf, "PIUC");
   return(ERROR);
}

STATUS_t
rtrv_user_secu(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   U32_t i;
   U32_t inx;
   U8_t  uapStr[100];

   if (!menuSelected)
      printf("M <%s> RTRV(", ctagBuf);
   
   for (i = 0; i < dataVal[0].ttlVal; i++)
   {
      inx = dataVal[0].u.val[i];
      if (userProfile[inx].inUse)
      {
         sprintf(uapStr, "%s", larusCapMAP[userProfile[inx].accessLvl-1].mapStr);
         printf("\n  \"<%s>:,<%s>\"", userProfile[inx].userName, uapStr);
      }
   }
   if (!menuSelected)
      printf(")+\n;\n");
   return(OK);
}

STATUS_t
rtrv_user(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   U32_t i;
   U8_t  uapStr[100];

 
   if (!menuSelected)
      printf("M <%s> RTRV\n", ctagBuf);
   for (i = 1; i < MAX_USERS_PROFILE; i++)
   {
      if (userProfile[i].inUse)
      {
         if (!strmatch(shellVarPtr->userName, userProfile[i].userName)) 
         {
            sprintf(uapStr, "%s", larusCapMAP[userProfile[i].accessLvl-1].mapStr);
            printf("  \"<%s>:,<%s>\n;\n", userProfile[i].userName, uapStr);
	 }
      }
   }
   return(OK);
}

STATUS_t
dlt_user_secu(U32_t ttlFld, MENU_DATA_VAL_t *dataVal)
{
   U32_t i;
   U32_t inx;
 
   for (i = 0; i < dataVal[0].ttlVal; i++)
   {
      inx = dataVal[0].u.val[i];
      if (inx < 2)
      {
         tl1PrintErr(ctagBuf, "IIAC");
         return(ERROR);
      }
      if (userProfile[inx].inUse)
      {
         userProfile[inx].inUse = 0;
      }
   }

   if (!menuSelected)
      printf("M <%s> COMPLD\n;\n", ctagBuf);
   return(OK);
}


STATUS_t
getSecuUser(U32_t allSet, U8_t  *user, MENU_DATA_VAL_t *dataVal)
{
   U32_t  i, j = 0;

   if (allSet)
   {
      for (i = 1; i < MAX_USERS_PROFILE; i++)
      {
         if (userProfile[i].inUse)
	 {
            dataVal->u.val[j++] = i;
	 }
      }
      dataVal->ttlVal = j;
      dataVal->type = TL1_IN_NUM;
      return(OK);
   }

   for (i = 1; i < MAX_USERS_PROFILE; i++)
   {
      if (userProfile[i].inUse)
      {
         if (!strmatch(userProfile[i].userName, user)) 
	 {
            dataVal->u.val[0] = i;
            return(OK);
	 }
      }
   }
   return(ERROR);
}

