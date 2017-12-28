/*
 * Service Menu - Set SMU Manual Test  (M2,1)
 */

MENU_DATA_ENTRY_t setManLinkEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Link]\n",
      "1) LINK1\n"
      "2) LINK2\n"
      "3) LINK3\n"
      "4) LINK4\n"
      "5) LINK5\n"
	  "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setManIntvEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[Interval]\n",
      "1) 1-SEC\n"
      "2) 10-SEC\n"
      "3) 100-SEC\n"
      "4) 1000-SEC\n"
      "5) 10000-SEC\n"
      "6) 100000-SEC\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setManNumRptEntry = {
      MENU_DATA_PARM_SEL,
      -1,
      100,
      MENU_DSP_BODY,
      "\n[Number Reports]\n",
      "-1 thru 100 then <CR>\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setManNumObvrEntry = {
      MENU_DATA_PARM_SEL,
      -1,
      100,
      MENU_DSP_BODY,
      "\n[Number Observ]\n",
      "-1 thru 100 then <CR>\n",
      MENU_NULL_IN_ERROR,
      4,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setManTstParmLst[] = {
&setManLinkEntry, &setManIntvEntry, &setManNumRptEntry, &setManNumObvrEntry};

#ifdef SIMUL
#define sched_pmrept_link "sched_pmrept_link"
#else
extern void sched_pmrept_link();
#endif

MENU_DATA_TBL_t setManTstMenu = {
      MENU_DATA_TBL_SEL,
      4,
      MENU_CMD_CFG,
      1,
      (void *)sched_pmrept_link,
      (void *)setManTstParmLst,
	  AXS_PRI_5
};


/*
 * Service Menu - View SMU Manual Test Schedule	(M2,2)
 */

MENU_DATA_ENTRY_t viewManLinkEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Link]\n",
      "1) LINK1\n"
      "2) LINK2\n"
      "3) LINK3\n"
      "4) LINK4\n"
      "5) LINK5\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};


MENU_DATA_ENTRY_t *viewManTstParmLst[] = {
&viewManLinkEntry};
#ifdef SIMUL
#define rtrv_pmsched_link "rtrv_pmsched_link"
#else
extern void rtrv_pmsched_link();
#endif

MENU_DATA_TBL_t viewManTstMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_pmsched_link,
      (void *)viewManTstParmLst,
	  AXS_PRI_6
};

/*
 * Service Menu - Abort SMU Manual Test	(M2,3)
 */
#ifdef SIMUL
#define abort_smu_man_test "abort_smu_man_test"
#else
extern void abort_smu_man_test();
#endif

MENU_DATA_TBL_t abortManTstMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_CFG,
      1,
      (void *)abort_smu_man_test,
      (void *)"2",                    /* ABORT_SUM_MANUAL_TEST(\n"2\n") */
	  AXS_PRI_5
};

/*
 * Service Menu - Set Scan Channels	(M2,4)
 */

MENU_DATA_ENTRY_t setScanChanEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Link]\n",
      "1) LINK1\n"
      "2) LINK2\n"
      "3) LINK3\n"
      "4) LINK4\n"
      "5) LINK5\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setScanEnableEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Link Enable]\n",
	  "1) YES\n"
      "2) NO\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setScanParmLst[] = {
&setScanChanEntry, &setScanEnableEntry};
#ifdef SIMUL
#define set_scan_channel "set_scan_channel"
#else
extern void set_scan_channel();
#endif

MENU_DATA_TBL_t setScanChanMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_CFG,
      1,
      (void *)set_scan_channel,
      (void *)setScanParmLst,
	  AXS_PRI_5
};


/*
 * Service Menu - View Scan Channels (M2,5)
 */

MENU_DATA_ENTRY_t viewScanChanEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Link]\n",
      "1) LINK1\n"
      "2) LINK2\n"
      "3) LINK3\n"
      "4) LINK4\n"
      "5) LINK5\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewScanChanParmLst[] = {
&viewScanChanEntry};
#ifdef SIMUL
#define view_scan_channel "view_scan_channel"
#else
extern void view_scan_channel();
#endif

MENU_DATA_TBL_t viewScanChanMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_DSP,
      1,
      (void *)view_scan_channel,
      (void *)viewScanChanParmLst,
	  AXS_PRI_6
};


/*
 * Service Menu - View SMU Scan Test Data (M2,6)
 */

MENU_DATA_ENTRY_t viewScantstLinkEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Link]\n",
      "1) LINK1\n"
      "2) LINK2\n"
      "3) LINK3\n"
      "4) LINK4\n"
      "5) LINK5\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewScantstPeriodEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[Time Period]\n",
      "1) 1-DAY\n"
      "2) 1-HOUR\n"
      "3) 100-SEC\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewScantstBucketEntry = {
      MENU_DATA_PARM_SEL,
      0,
      99,
      MENU_DSP_BODY,
      "\n[Data Bucket]\n",
      "99 for all, or 0-6, or 0-23 then <CR>\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewScantstTypeEntry = {
      MENU_DATA_PARM_SEL,
      1,
      8,
      MENU_DSP_BODY,
      "\n[Monitored Type]\n",
      "1) BPV\n"
      "2) CRC\n"
      "3) CS\n"
      "4) ESP\n"
      "5) JITTER\n"
      "6) SESP\n"
      "7) TIE\n"
      "8) MTIE\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_NULL_IN_ERROR,
      4,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewScanTstParmLst[] = {
&viewScantstLinkEntry, &viewScantstPeriodEntry, &viewScantstBucketEntry,
&viewScantstTypeEntry};
#ifdef SIMUL
#define rtrv_pm_link "rtrv_pm_link"
#else
extern void rtrv_pm_link();
#endif

MENU_DATA_TBL_t viewScanTstMenu = {
      MENU_DATA_TBL_SEL,
      4,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_pm_link,
      (void *)viewScanTstParmLst,
	  AXS_PRI_6
};

MENU_DATA_TBL_t *perfMonMenuLst[] = {
NULL, &setManTstMenu, &viewManTstMenu, &abortManTstMenu, &setScanChanMenu, &viewScanChanMenu,
&viewScanTstMenu};

MENU_TBL_t perfMonMenu = {
      MENU_ENTRY_SEL,
      0,
      6,
      "\n\n[PERFORMANCE MENU]\n",
      "\n"
      "1) Set SMU Manual Test\n"
      "2) View SMU Manual Test\n"
      "3) Abort SMU Manual Test\n"
      "4) Set Scan Channels\n"
      "5) View Scan Channels\n"
      "6) View SMU Scan Test Data\n"
      "0) Main Menu\n",
      (void *)perfMonMenuLst
};
