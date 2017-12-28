
/*
 * Alarm/Event Menu - Set Clock Severity (M4,1)
 */

MENU_DATA_ENTRY_t setClkSevClkEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Clock]\n",
      "1) CLK1\n"
      "2) CLK2\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setClkSevAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) AQRSYNC\n"
      "2) FRNGSYNC\n"
      "3) HLDOVRSYNC\n"
      "4) LKDSYNC\n"
      "5) ACT\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setClkSevEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Severity]\n",
      "1) CR\n"
      "2) MJ\n"
	  "3) MN\n"
	  "4) NA\n"
	  "5) NR\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setClkSevParmLst[] = {
&setClkSevClkEntry, &setClkSevAlmEntry, &setClkSevEntry};
#ifdef SIMUL
#define set_attr_eqpt_clk "set_attr_eqpt_clk"
#else
extern void set_attr_eqpt_clk();
#endif

MENU_DATA_TBL_t setClkSevMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_CFG,
      1,
      (void *)set_attr_eqpt_clk,
      (void *)setClkSevParmLst,
	  A7
};

/*
 * Alarm/Event Menu - View Clock Severity (M4,2)
 */

MENU_DATA_ENTRY_t viewClkSevClkEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Clock]\n",
      "1) CLK1\n"
      "2) CLK2\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewClkSevAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) AQRSYNC\n"
      "2) FRNGSYNC\n"
      "3) HLDOVRSYNC\n"
      "4) LKDSYNC\n"
      "5) ACT\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewClkSevParmLst[] = {
&viewClkSevClkEntry, &viewClkSevAlmEntry};
#ifdef SIMUL
#define rtrv_attr_eqpt_clk "rtrv_attr_eqpt_clk"
#else
extern void rtrv_attr_eqpt_clk();
#endif

MENU_DATA_TBL_t viewClkSevMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_attr_eqpt_clk,
      (void *)viewClkSevParmLst,
	  A2
};

/*
 * Alarm/Event Menu - Set Link Severity	(M4,3)
 */

MENU_DATA_ENTRY_t setLinkSevClkEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[LINK]\n",
      "1) LINK1\n"
      "2) LINK2\n"
	  "3) LINK3\n"
	  "4) LINK4\n"
	  "5) LINK5\n"
	  "6) F5/10\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setLinkSevAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) ACT\n"
      "2) LOS\n"
      "3) AISP\n"
      "4) LOF\n"
      "5) T-BPV\n"
      "6) T-CRC\n"
      "7) T-CS\n"
      "8) T-JITTER\n"
      "9) T-TIE\n"
      "A) T-MTIE\n"
	  "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setLinkSevEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Severity]\n",
      "1) CR\n"
      "2) MJ\n"
	  "3) MN\n"
	  "4) NA\n"
	  "5) NR\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setLinkSevParmLst[] = {
&setLinkSevClkEntry, &setLinkSevAlmEntry, &setLinkSevEntry};
#ifdef SIMUL
#define set_attr_link "set_attr_link"
#else
extern void set_attr_link();
#endif

MENU_DATA_TBL_t setLinkSevMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_CFG,
      1,
      (void *)set_attr_link,
      (void *)setLinkSevParmLst,
	  A7
};

/*
 * Alarm/Event Menu - View Link Severity (M4,4)
 */

MENU_DATA_ENTRY_t viewLinkSevClkEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[LINK]\n",
      "1) LINK1\n"
      "2) LINK2\n"
	  "3) LINK3\n"
	  "4) LINK4\n"
	  "5) LINK5\n"
	  "6) F5/10\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewLinkSevAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) ACT\n"
      "2) LOS\n"
      "3) AISP\n"
      "4) LOF\n"
      "5) T-BPV\n"
      "6) T-CRC\n"
      "7) T-CS\n"
      "8) T-JITTER\n"
      "9) T-TIE\n"
      "A) T-MTIE\n"
	  "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewLinkSevParmLst[] = {
&viewLinkSevClkEntry, &viewLinkSevAlmEntry};
#ifdef SIMUL
#define rtrv_attr_link "rtrv_attr_link"
#else
extern void rtrv_attr_link();
#endif

MENU_DATA_TBL_t viewLinkSevMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_attr_link,
      (void *)viewLinkSevParmLst,
	  A2
};

/*
 * Alarm/Event Menu - Set Power Severity (M4,5)
 */

MENU_DATA_ENTRY_t setPwrSevShelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[SHELF]\n",
      "1) MAIN\n"
      "2) EXPANSION1\n"
	  "3) EXPANSION2\n"
	  "4) EXPANSION3\n"
	  "5) EXPANSION4\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setPwrSevAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) OSLINK\n"
      "2) PWR_A\n"
      "3) PWR_B\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setPwrSevEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Severity]\n",
      "1) CR\n"
      "2) MJ\n"
	  "3) MN\n"
	  "4) NA\n"
	  "5) NR\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setPwrSevParmLst[] = {
&setPwrSevShelfEntry, &setPwrSevAlmEntry, &setPwrSevEntry};
#ifdef SIMUL
#define set_attr_eqpt_ospwr "set_attr_eqpt_ospwr"
#else
extern void set_attr_eqpt_ospwr();
#endif

MENU_DATA_TBL_t setPwrSevMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_CFG,
      1,
      (void *)set_attr_eqpt_ospwr,
      (void *)setPwrSevParmLst,
	  A7
};

/*
 * Alarm/Event Menu - View Power Severity (M4,6)
 */

MENU_DATA_ENTRY_t viewPwrSevShelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "/n[SHELF]\n",
      "1) MAIN\n"
      "2) EXPANSION1\n"
	  "3) EXPANSION2\n"
	  "4) EXPANSION3\n"
	  "5) EXPANSION4\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewPwrSevAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) OSLINK\n"
      "2) PWR_A\n"
      "3) PWR_B\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewPwrSevParmLst[] = {
&viewPwrSevShelfEntry, &viewPwrSevAlmEntry};
#ifdef SIMUL
#define rtrv_attr_eqpt_ospwr "rtrv_attr_eqpt_ospwr"
#else
extern void rtrv_attr_eqpt_ospwr();
#endif

MENU_DATA_TBL_t viewPwrSevMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_attr_eqpt_ospwr,
      (void *)viewPwrSevParmLst,
	  A2
};

/*
 * Alarm/Event Menu - Set Output Port Severity (M4,7)
 */

MENU_DATA_ENTRY_t setOutpSevShelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[SHELF]\n",
      "1) MAIN\n"
      "2) EXPANSION1\n"
	  "3) EXPANSION2\n"
	  "4) EXPANSION3\n"
	  "5) EXPANSION4\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setOutpSevSlotEntry = {
      MENU_DATA_PARM_SEL,
      1,
      16,
      MENU_DSP_BODY,
      "\n[Output Card]\n",
      "1) Output Card 1\n"
      "2) Output Card 2\n"
	  "3) Output Card 3\n"
	  "4) Output Card 4\n"
	  "5) Output Card 5\n"
	  "6) Output Card 6\n"
	  "7) Output Card 7\n"
	  "8) Output Card 8\n"
	  "9) Output Card 9\n"
	  "A) Output Card 10\n"
	  "B) Output Card 11\n"
	  "C) Output Card 12\n"
	  "D) Output Card 13\n"
	  "E) Output Card 14\n"
	  "F) Output Card 15\n"
	  "G) Output Card 16\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setOutpSevPortEntry = {
      MENU_DATA_PARM_SEL,
      1,
      8,
      MENU_DSP_BODY,
      "\n[Output Port]\n",
      "1) Output Port 1\n"
      "2) Output Port 2\n"
	  "3) Output Port 3\n"
	  "4) Output Port 4\n"
	  "5) Output Port 5\n"
	  "6) Output Port 6\n"
	  "7) Output Port 7\n"
	  "8) Output Port 8\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setOutpSevAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) SYNCCLK\n"
      "2) LOSSCLK\n"
      "3) TIU_ACT\n"
      "4) TIU_LOS\n"
      "5) TIU_AISP\n"
      "6) TIU_LOF\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      4,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setOutpSevEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Severity]\n",
      "1) CR\n"
      "2) MJ\n"
	  "3) MN\n"
	  "4) NA\n"
	  "5) NR\n",
      MENU_NULL_IN_ERROR,
      5,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setOutpSevParmLst[] = {
&setOutpSevShelfEntry, &setOutpSevSlotEntry, &setOutpSevPortEntry,
&setOutpSevAlmEntry, &setOutpSevEntry};
#ifdef SIMUL
#define set_attr_eqpt_output "set_attr_eqpt_output"
#else
extern void set_attr_eqpt_output();
#endif

MENU_DATA_TBL_t setOutpSevMenu = {
      MENU_DATA_TBL_SEL,
      5,
      MENU_CMD_CFG,
      1,
      (void *)set_attr_eqpt_output,
      (void *)setOutpSevParmLst,
	  A7
};

/*
 * Alarm/Event Menu - View Output Port Severity	(M4,8)
 */

MENU_DATA_ENTRY_t viewOutpSevShelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[SHELF]\n",
      "1) MAIN\n"
      "2) EXPANSION1\n"
	  "3) EXPANSION2\n"
	  "4) EXPANSION3\n"
	  "5) EXPANSION4\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewOutpSevSlotEntry = {
      MENU_DATA_PARM_SEL,
      1,
      16,
      MENU_DSP_BODY,
      "\n[Output Card]\n",
      "1) Output Card 1\n"
      "2) Output Card 2\n"
	  "3) Output Card 3\n"
	  "4) Output Card 4\n"
	  "5) Output Card 5\n"
	  "6) Output Card 6\n"
	  "7) Output Card 7\n"
	  "8) Output Card 8\n"
	  "9) Output Card 9\n"
	  "A) Output Card 10\n"
	  "B) Output Card 11\n"
	  "C) Output Card 12\n"
	  "D) Output Card 13\n"
	  "E) Output Card 14\n"
	  "F) Output Card 15\n"
	  "G) Output Card 16\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewOutpSevPortEntry = {
      MENU_DATA_PARM_SEL,
      1,
      8,
      MENU_DSP_BODY,
      "\n[Output Port]\n",
      "1) Output Port 1\n"
      "2) Output Port 2\n"
	  "3) Output Port 3\n"
	  "4) Output Port 4\n"
	  "5) Output Port 5\n"
	  "6) Output Port 6\n"
	  "7) Output Port 7\n"
	  "8) Output Port 8\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewOutpSevAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) SYNCCLK\n"
      "2) LOSSCLK\n"
      "3) TIU_ACT\n"
      "4) TIU_LOS\n"
      "5) TIU_AISP\n"
      "6) TIU_LOF\n"
      "*) All\n"
	  "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      4,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewOutpSevParmLst[] = {
&viewOutpSevShelfEntry, &viewOutpSevSlotEntry, &viewOutpSevPortEntry, 
&viewOutpSevAlmEntry};
#ifdef SIMUL
#define rtrv_attr_eqpt_output "rtrv_attr_eqpt_output"
#else
extern void rtrv_attr_eqpt_output();
#endif

MENU_DATA_TBL_t viewOutpSevMenu = {
      MENU_DATA_TBL_SEL,
      4,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_attr_eqpt_output,
      (void *)viewOutpSevParmLst,
	  A2
};

/*
 * Alarm/Event Menu - Current Clock/Link/Power Alarms (M4,9)
 */

MENU_DATA_ENTRY_t curClpAlmSourceEntry = {
      MENU_DATA_PARM_SEL,
      1,
      13,
      MENU_DSP_BODY,
      "\n[Clock/Link/Power]\n",
	  "1) CLK1\n"
      "2) CLK2\n"
      "3) PWR_A\n"
      "4) PWR_B\n"
      "5) OS\n"
      "6) F5/10\n"
      "7) LINK1\n"
      "8) LINK2\n"
      "9) LINK3\n"
      "A) LINK4\n"
      "B) LINK5\n"
      "C) ESR1\n"
      "D) ESR2\n"
	  "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curClpAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      25,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) AQRSYNC              "
      "2) FRNGSYNC  "
      "3) HLDOVRSYNC "
      "4) LKDSYNC\n"
      "5) ACT "
      "6) LOS\n"
      "7) AISP\n"
      "8) LOF\n"
      "9) T-BPV\n"
      "A) T-CRC\n"
      "B) T-CS\n"
      "C) T-JITTER\n"
      "D) T-TIE\n"
      "E) T-MTIE\n"
      "F) OSLINK\n"
	  "G) PWR_A\n"
      "H) PWR_B\n"
      "I) GPSANT\n"
      "J) GPSOSC\n"
      "K) GPSINFRQ\n"
      "L) GPS1PPS\n"
      "M) GPSWARM\n"
      "N) GPSDGRAD\n"
      "O) GPSFAIL\n"
      "P) GPSLKD\n"
	  "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curClpAlmSevEntry = {
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
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curClpAlmSerEffEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Service Affect]\n",
      "1) NSA\n"
      "2) SA\n"
	  "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      4,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *curClpAlmParmLst[] = {
&curClpAlmSourceEntry, &curClpAlmEntry, &curClpAlmSevEntry, 
&curClpAlmSerEffEntry};
#ifdef SIMUL
#define cur_clp_alm "cur_clp_alm"
#else
extern void cur_clp_alm();
#endif

MENU_DATA_TBL_t curClpAlmMenu = {
      MENU_DATA_TBL_SEL,
      4,
      MENU_CMD_DSP,
      1,
      (void *)cur_clp_alm,
      (void *)curClpAlmParmLst,
	  A2
};

/*
 * Alarm/Event Menu - Current Clock/Link/Power Events (M4, A)
 */

MENU_DATA_ENTRY_t curClpEvtSourceEntry = {
      MENU_DATA_PARM_SEL,
      1,
      13,
      MENU_DSP_BODY,
      "\n[Clock/Link/Power]\n",
	  "1) CLK1\n"
      "2) CLK2\n"
      "3) PWR_A\n"
      "4) PWR_B\n"
      "5) OS\n"
      "6) F5/10\n"
      "7) LINK1\n"
      "8) LINK2\n"
      "9) LINK3\n"
      "A) LINK4\n"
      "B) LINK5\n"
      "C) ESR1\n"
      "D) ESR2\n"
	  "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curClpEvtAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      25,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
	  "1) AQRSYNC              "
      "2) FRNGSYNC  "
      "3) HLDOVRSYNC "
      "4) LKDSYNC\n"
      "5) ACT "
      "6) LOS\n"
      "7) AISP\n"
      "8) LOF\n"
      "9) T-BPV\n"
      "A) T-CRC\n"
      "B) T-CS\n"
      "C) T-JITTER\n"
      "D) T-TIE\n"
      "E) T-MTIE\n"
      "F) OSLINK\n"
	  "G) PWR_A\n"
      "H) PWR_B\n"
      "I) GPSANT\n"
      "J) GPSOSC\n"
      "K) GPSINFRQ\n"
      "L) GPS1PPS\n"
      "M) GPSWARM\n"
      "N) GPSDGRAD\n"
      "O) GPSFAIL\n"
      "P) GPSLKD\n"
	  "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curClpEvtSerEffEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Service Affect]\n",
      "1) NSA\n"
      "2) SA\n"
	  "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *curClpEvtParmLst[] = {
&curClpEvtSourceEntry, &curClpEvtAlmEntry, &curClpEvtSerEffEntry};
#ifdef SIMUL
#define cur_clp_evt "cur_clp_evt"
#else
extern void cur_clp_evt();
#endif

MENU_DATA_TBL_t curClpEvtMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_DSP,
      1,
      (void *)cur_clp_evt,
      (void *)curClpEvtParmLst,
	  A2
};

/*
 * Alarm/Event Menu - Current Output Port Alarms (M4,B)
 */

MENU_DATA_ENTRY_t curOutpAlmShelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[SHELF]\n",
      "1) MAIN\n"
      "2) EXPANSION1\n"
	  "3) EXPANSION2\n"
	  "4) EXPANSION3\n"
	  "5) EXPANSION4\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curOutpAlmSlotEntry = {
      MENU_DATA_PARM_SEL,
      1,
      16,
      MENU_DSP_BODY,
      "\n[Output Card]\n",
      "1) Output Card 1\n"
      "2) Output Card 2\n"
	  "3) Output Card 3\n"
	  "4) Output Card 4\n"
	  "5) Output Card 5\n"
	  "6) Output Card 6\n"
	  "7) Output Card 7\n"
	  "8) Output Card 8\n"
	  "9) Output Card 9\n"
	  "A) Output Card 10\n"
	  "B) Output Card 11\n"
	  "C) Output Card 12\n"
	  "D) Output Card 13\n"
	  "E) Output Card 14\n"
	  "F) Output Card 15\n"
	  "G) Output Card 16\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curOutpAlmPortEntry = {
      MENU_DATA_PARM_SEL,
      1,
      8,
      MENU_DSP_BODY,
      "\n[Output Port]\n",
      "1) Output Port 1\n"
      "2) Output Port 2\n"
	  "3) Output Port 3\n"
	  "4) Output Port 4\n"
	  "5) Output Port 5\n"
	  "6) Output Port 6\n"
	  "7) Output Port 7\n"
	  "8) Output Port 8\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curOutpAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
  	  "1) SYNCCLK\n"
      "2) LOSSCLK\n"
      "3) TIU_ACT\n"
      "4) TIU_LOS\n"
      "5) TIU_AISP\n"
      "6) TIU_LOF\n"
	  "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      4,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curOutpAlmSevEntry = {
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
      5,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curOutpAlmSerEffEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Service Affect]\n",
      "1) NSA\n"
      "2) SA\n"
	  "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      6,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *curOutpAlmParmLst[] = {
&curOutpAlmShelfEntry, &curOutpAlmSlotEntry, &curOutpAlmPortEntry,
&curOutpAlmEntry, &curOutpAlmSevEntry, &curOutpAlmSerEffEntry};
#ifdef SIMUL
#define cur_outp_alm "cur_outp_alm"
#else
extern void cur_outp_alm();
#endif

MENU_DATA_TBL_t curOutpAlmMenu = {
      MENU_DATA_TBL_SEL,
      6,
      MENU_CMD_DSP,
      1,
      (void *)cur_outp_alm,
      (void *)curOutpAlmParmLst,
	  A2
};

/*
 * Alarm/Event Menu - Current Output Port Events (M4,C)
 */

MENU_DATA_ENTRY_t curOutpEvtShelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[SHELF]\n",
      "1) MAIN\n"
      "2) EXPANSION1\n"
	  "3) EXPANSION2\n"
	  "4) EXPANSION3\n"
	  "5) EXPANSION4\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curOutpEvtSlotEntry = {
      MENU_DATA_PARM_SEL,
      1,
      16,
      MENU_DSP_BODY,
      "\n[Output Card]\n",
      "1) Output Card 1\n"
      "2) Output Card 2\n"
	  "3) Output Card 3\n"
	  "4) Output Card 4\n"
	  "5) Output Card 5\n"
	  "6) Output Card 6\n"
	  "7) Output Card 7\n"
	  "8) Output Card 8\n"
	  "9) Output Card 9\n"
	  "A) Output Card 10\n"
	  "B) Output Card 11\n"
	  "C) Output Card 12\n"
	  "D) Output Card 13\n"
	  "E) Output Card 14\n"
	  "F) Output Card 15\n"
	  "G) Output Card 16\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curOutpEvtPortEntry = {
      MENU_DATA_PARM_SEL,
      1,
      8,
      MENU_DSP_BODY,
      "\n[Output Port]\n",
      "1) Output Port 1\n"
      "2) Output Port 2\n"
	  "3) Output Port 3\n"
	  "4) Output Port 4\n"
	  "5) Output Port 5\n"
	  "6) Output Port 6\n"
	  "7) Output Port 7\n"
	  "8) Output Port 8\n"
      "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curOutpEvtAlmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[Alarms]\n",
  	  "1) SYNCCLK\n"
      "2) LOSSCLK\n"
      "3) TIU_ACT\n"
      "4) TIU_LOS\n"
      "5) TIU_AISP\n"
      "6) TIU_LOF\n"
	  "*) All\n"
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      4,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t curOutpEvtSerEffEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Service Affect]\n",
      "1) NSA\n"
      "2) SA\n"
	  "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      5,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *curOutpEvtParmLst[] = {
&curOutpEvtShelfEntry, &curOutpEvtSlotEntry, &curOutpEvtPortEntry,
&curOutpEvtAlmEntry, &curOutpEvtSerEffEntry};
#ifdef SIMUL
#define cur_outp_evt "cur_outp_evt"
#else
extern void cur_outp_evt();
#endif

MENU_DATA_TBL_t curOutpEvtMenu = {
      MENU_DATA_TBL_SEL,
      5,
      MENU_CMD_DSP,
      1,
      (void *)cur_outp_evt,
      (void *)curOutpEvtParmLst,
	  A2
};

MENU_DATA_TBL_t *almEvtMenuLst[] = {
NULL, &setClkSevMenu, &viewClkSevMenu, &setLinkSevMenu, &viewLinkSevMenu, &setPwrSevMenu,
&viewPwrSevMenu, &setOutpSevMenu, &viewOutpSevMenu, &curClpAlmMenu, &curClpEvtMenu,
&curOutpAlmMenu, &curOutpEvtMenu};

MENU_TBL_t almEvtMenu = {
      MENU_ENTRY_SEL,
      0,
      12,
      "\n\n[ALARM/EVENT MENU]\n",
      "\n"
      "1) Set Clock Severity\n"
      "2) View Clock Severity\n"
      "3) Set Link Severity\n"
      "4) View Link Severity\n"
      "5) Set Power Severity\n"
      "6) View Power Severity\n"
	  "7) Set Output Port Severity\n"
	  "8) View Output Port Severity\n"
	  "9) Current Clock/Link/Power Alarms\n"
	  "A) Current Clock/Link/Power Events\n"
	  "B) Current Output Port Alarms\n"
	  "C) Current Output Port Events\n"
      "0) Main Menu\n",
      (void *)almEvtMenuLst
};
