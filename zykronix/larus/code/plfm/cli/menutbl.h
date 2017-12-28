

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "menu_objs.h"
#include "menuSvcParmTbl.h"
#include "menuPerfMonTbl.h"
#include "menuMiscTbl.h"
#include "menuLogTbl.h"
#include "menuAlmEvtTbl.h"
#include "menuSecTbl.h"
#include "menuThresTbl.h"
#include "menuLogin.h"

MENU_TBL_t *mainMenuLst[] = {
NULL, &serviceMenu, &perfMonMenu, &thresholdMenu, &almEvtMenu, &logfileMenu,
&securityMenu, &miscMenu}; 

MENU_TBL_t mainMenu = {
      MENU_ENTRY_SEL,
      0,
      7,
      "\n\n[MAIN MENU]\n",
      "1) Service Parameters\n"
      "2) Performance Monitoring\n"
      "3) Thresholds\n"
      "4) Alarm/Event\n"
      "5) Logfiles\n"
      "6) Security\n"
      "7) Miscellaneous\n"
      "\n"
      "\n"
      "0) Log Out\n",
      (void *)mainMenuLst,
};
