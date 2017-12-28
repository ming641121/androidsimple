
/*
 * Threshold Menu - Clear Log File (M5,1)
 */
#ifdef SIMUL
#define clear_log "clear_log"
#else
extern void clear_log();
#endif

MENU_DATA_TBL_t clearLogFileMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_CFG,
      0,
      (void *)clear_log,
      NULL,
	  A7
};


/*
 * Threshold Menu - Read Log File (M5,2)
 */

MENU_DATA_ENTRY_t readLogFileDateEntry = {
      MENU_DATA_PARM_SEL,
      1,
      7,
      MENU_DSP_BODY,
      "\n[Days]\n",
	  "1) 1-Day\n"
      "2) 2-Day\n"
      "3) 3-Day\n"
      "4) 4-Day\n"
      "5) 5-Day\n"
      "6) 6-Day\n"
      "7) 7-Day\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t readLogFileSevEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[Severity]\n",
      "1) CR\n"
      "2) MJ\n"
	  "3) MN\n"
	  "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *readLogFileParmLst[] = {
&readLogFileDateEntry, &readLogFileSevEntry};
#ifdef SIMUL
#define read_log "read_log"
#else
extern void read_log();
#endif

MENU_DATA_TBL_t readLogFileMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_DSP,
      0,
      (void *)read_log,
      (void *)readLogFileParmLst,
	  A2
};


MENU_DATA_TBL_t *logfileMenuLst[] = {
NULL, &clearLogFileMenu, &readLogFileMenu};

MENU_TBL_t logfileMenu = {
      MENU_ENTRY_SEL,
      0,
      2,
      "\n\n[LOGFILE MENU]\n",
      "\n"
      "1) Clear Log File\n"
      "2) Read Log File\n"
      "0) Main Menu\n",
      (void *)logfileMenuLst
};


