
/*
 * Threshold Menu - Set SMU Threshold (M3,1)
 */

MENU_DATA_ENTRY_t setSmuThresLinkEntry = {
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
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setSmuThresEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[Threshold]\n",
      "1) CVL\n"
      "2) CVP\n"
      "3) CS\n"
      "4) JITTER\n"
      "5) TIE\n"
      "6) MTIE\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setSmuThresLevelEntry = {
      MENU_DATA_PARM_SEL,
      0,
      10000,
      MENU_DSP_BODY,
      "\n[Threshold Level]\n",
      "0-10000, then <CR>\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setSmuThresParmLst[] = {
&setSmuThresLinkEntry, &setSmuThresEntry, &setSmuThresLevelEntry};
#ifdef SIMUL
#define set_smu_thres "set_smu_thres"
#else
extern void set_smu_thres();
#endif

MENU_DATA_TBL_t setSmuThresMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_CFG,
      1,
      (void *)set_smu_thres,
      (void *)setSmuThresParmLst,
	  AXS_PRI_3
};

/*
 * Threshold Menu - View SMU Threshold (M3,2)
 */

MENU_DATA_ENTRY_t viewSmuThresLinkEntry = {
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
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewSmuThresEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[Threshold]\n",
      "1) CVL\n"
      "2) CVP\n"
      "3) CS\n"
      "4) JITTER\n"
      "5) TIE\n"
      "6) MTIE\n"
	  "*) All\n"
	  "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewSmuThresParmLst[] = {
&viewSmuThresLinkEntry, &viewSmuThresEntry};
#ifdef SIMUL
#define rtrv_th_link "rtrv_th_link"
#else
extern void rtrv_th_link();
#endif

MENU_DATA_TBL_t viewSmuThresMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_CFG,
      1,
      (void *)rtrv_th_link,
      (void *)viewSmuThresParmLst,
	  AXS_PRI_6
};

MENU_DATA_TBL_t *thresholdMenuLst[] = {
NULL, &setSmuThresMenu, &viewSmuThresMenu};

MENU_TBL_t thresholdMenu = {
      MENU_ENTRY_SEL,
      0,
      2,
      "\n\n[THRESHOLD MENU]\n",
      "\n"
      "1) Set SMU Threshold\n"
      "2) View SMU Threshold\n"
      "0) Main Menu\n",
      (void *)thresholdMenuLst
};


