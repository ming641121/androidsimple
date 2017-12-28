
/*
 * Miscellaneous Menu - Reboot (M7,1)
 */

MENU_DATA_ENTRY_t rebootEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[Reboot]\n",
	  "1) Warm\n"
      "2) Cold\n"
      "3) Config\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *rebootParmLst[] = {
&rebootEntry};
#ifdef SIMUL
#define reset_system "reset_system"
#else
extern void reset_system();
#endif

MENU_DATA_TBL_t rebootMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      1,
      (void *)reset_system,
      (void *)rebootParmLst,
	  A8
};

/*
 * Miscellaneous Menu - Retrieve Inventory (M7,2)
 */
#ifdef SIMUL
#define rtrv_card "rtrv_card"
#else
extern void rtrv_card();
#endif

MENU_DATA_TBL_t retrieveInvMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_card,
      (void *)NULL,
	  A2
};

/*
 * Miscellaneous Menu - Set Input Clock Operation Mode (M7,3)
 */

MENU_DATA_ENTRY_t setinpclkOpEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Operation Mode]\n",
	  "1) Protected\n"
      "2) Manual\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setinpclkOpParmLst[] = {
&setinpclkOpEntry};
#ifdef SIMUL
#define ed_clockinput_mode "ed_clockinput_mode"
#else
extern void ed_clockinput_mode();
#endif

MENU_DATA_TBL_t setinpclkOpMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      1,
      (void *)ed_clockinput_mode,
      (void *)setinpclkOpParmLst,
	  A6
};

/*
 * Miscellaneous Menu - View Input Clock Operation Mode (M7,4)
 */
#ifdef SIMUL
#define rtrv_clockinput_mode "rtrv_clockinput_mode"
#else
extern void rtrv_clockinput_mode();
#endif

MENU_DATA_TBL_t viewInpOpModeMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_clockinput_mode,
      NULL,
	  A2
};

/*
 * Miscellaneous Menu - Swap Clock	(M7,5)
 */

MENU_DATA_ENTRY_t swapClockEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Clock]\n",
	  "1) CLK1\n"
      "2) CLK2\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t swapClockStaEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[State]\n",
	  "1) Standby\n"
	  "2) Active\n"
      "3) Offline\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *swapClockParmLst[] = {
&swapClockEntry, &swapClockStaEntry};
#ifdef SIMUL
#define sw_dx_osc "sw_dx_osc"
#else
extern void sw_dx_osc();
#endif

MENU_DATA_TBL_t swapClockMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_CFG,
      1,
      (void *)sw_dx_osc,
      (void *)swapClockParmLst,
	  A6
};

/*
 * Miscellaneous Menu - View Clock	(M7,6)
 */

MENU_DATA_ENTRY_t viewClockEntry = {
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

MENU_DATA_ENTRY_t *viewClockParmLst[] = {
&viewClockEntry};
#ifdef SIMUL
#define rtrv_osc "rtrv_osc"
#else
extern void rtrv_osc();
#endif

MENU_DATA_TBL_t viewClockMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      1,
      (void *)rtrv_osc,
      (void *)viewClockParmLst,
	  A2
};

/*
 * Miscellaneous Menu - Swap Input	(M7,7)
 */

MENU_DATA_ENTRY_t swapinputEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Input]\n",
	  "1) ESR1\n"
      "2) ESR2\n"
      "3) PRS\n"
      "4) LINK1\n"
      "5) LINK2\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *swapinputParmLst[] = {
&swapinputEntry};
#ifdef SIMUL
#define ed_clockinput_manualselect "ed_clockinput_manualselect"
#else
extern void ed_clockinput_manualselect();
#endif

MENU_DATA_TBL_t swapinputMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      1,
      (void *)ed_clockinput_manualselect,
      (void *)swapinputParmLst,
	  A6
};

/*
 * Miscellaneous Menu - View Input	(M7,8)
 */

#ifdef SIMUL
#define rtrv_clockinput_manualselect "rtrv_clockinput_manualselect"
#else
extern void rtrv_clockinput_manualselect();
#endif

MENU_DATA_TBL_t viewinputMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_CFG,
      1,
      (void *)rtrv_clockinput_manualselect,
      NULL,
	  A2
};

/*
 * Miscellaneous Menu - Swap Output Card (M7,9)
 * User Function
 */

MENU_DATA_ENTRY_t swapoutputshEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[SHELF]\n",
      "1) MAIN\n"
      "2) EXPANSION1\n"
	  "3) EXPANSION2\n"
	  "4) EXPANSION3\n"
	  "5) EXPANSION4\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t swapoutputslEntry = {
      MENU_DATA_PARM_SEL,
      1,
      16,
      MENU_DSP_BODY,
      "\n[Output Card Selection]\n",
	  "1) OUT1\n"
      "2) OUT2\n"
      "3) OUT3\n"
      "4) OUT4\n"
      "5) OUT5\n"
      "6) OUT6\n"
      "7) OUT7\n"
      "8) OUT8\n"
      "9) OUT9\n"
      "A) OUT10\n"
      "B) OUT11\n"
      "C) OUT12\n"
      "D) OUT13\n"
      "E) OUT14\n"
      "F) OUT15\n"
      "G) OUT16\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t swapoutputreEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[REDUNDANCY STATE]\n",
      "1) ACTIVE\n"
      "2) STANDBY\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *swapoutputParmLst[] = {
&swapoutputshEntry, &swapoutputslEntry, &swapoutputreEntry};
#ifdef SIMUL
#define ed_outp_red "ed_outp_red"
#else
extern void ed_outp_red();
#endif

MENU_DATA_TBL_t swapoutputMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      0,
      (void *)ed_outp_red,
      (void *)swapoutputParmLst,
	  A6
};

/*
 * Miscellaneous Menu - View GPS Data (M7,A)
 */

MENU_DATA_ENTRY_t viewGpsEsrEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[GPS]\n",
      "1) GPS1\n"
      "2) GPS2\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

#ifdef SIMUL
#define rtrv_gps_antd "rtrv_gps_antd"
#define rtrv_gps_azel "rtrv_gps_azel"
#define rtrv_gps_bits "rtrv_gps_bits"
#define rtrv_gps_pavg "rtrv_gps_pavg"
#define rtrv_gps_sigq "rtrv_gps_sigq"
#define rtrv_gps_spos "rtrv_gps_spos"
#define rtrv_gps_trmo "rtrv_gps_trmo"
#define rtrv_gps_utct "rtrv_gps_utct"
#define rtrv_gps_vers "rtrv_gps_vers"
#define rtrv_gps_detf "rtrv_gps_detf"
#define rtrv_gps_efer "rtrv_gps_efer"
#define rtrv_gps_essd "rtrv_gps_essd"
#define rtrv_gps_essn "rtrv_gps_essn"
#define rtrv_gps_fage "rtrv_gps_fage"
#define rtrv_gps_timd "rtrv_gps_timd"
#else
extern void rtrv_gps_antd();
extern void rtrv_gps_azel();
extern void rtrv_gps_bits();
extern void rtrv_gps_pavg();
extern void rtrv_gps_sigq();
extern void rtrv_gps_spos();
extern void rtrv_gps_trmo();
extern void rtrv_gps_utct();
extern void rtrv_gps_vers();
extern void rtrv_gps_detf();
extern void rtrv_gps_efer();
extern void rtrv_gps_essd();
extern void rtrv_gps_essd();
extern void rtrv_gps_fage();
extern void rtrv_gps_timd();
#endif

void *viewGpsStatDspLst[] = {
(void *)rtrv_gps_antd, (void *)rtrv_gps_azel, (void *)rtrv_gps_bits, 
(void *)rtrv_gps_pavg, (void *)rtrv_gps_sigq, (void *)rtrv_gps_spos, (void *)rtrv_gps_trmo, 
(void *)rtrv_gps_utct, (void *)rtrv_gps_vers, (void *)rtrv_gps_detf, (void *)rtrv_gps_efer, 
(void *)rtrv_gps_essd, (void *)rtrv_gps_essd, (void *)rtrv_gps_fage, (void *)rtrv_gps_timd
};

MENU_DSP_TBL_t viewGpsStatusMenu = {
      MENU_DSP_PARM_SEL,
      1,
      15,
      MENU_DSP_BODY,
      "\n[Status Info]\n",
      "1) ANTD\n"
      "2) AZEL\n"
      "3) BITS\n"
      "4) PAVG\n"
      "5) SIGQ\n"
      "6) SPOS\n"
      "7) TRMO\n"
      "8) UTCT\n"
      "9) VERS\n"
      "A) DETF\n"
      "B) EFER\n"
      "C) ESSD\n"
      "D) ESSN\n"
      "E) FAGE\n"
      "F) TIMD\n"
      "*) All\n"      
      "Enter Group (use '-' for range), then <CR>\n",
      MENU_MULTI_IN | MENU_ALL_AST | MENU_NULL_IN_ERROR,
      0,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      (void *)viewGpsStatDspLst
};

MENU_DATA_ENTRY_t *viewGpsParmLst[] = {
&viewGpsEsrEntry, (MENU_DATA_ENTRY_t *)&viewGpsStatusMenu};

MENU_DATA_TBL_t viewgpsdataMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_DSP,
      1,
      NULL,
      (void *)viewGpsParmLst,
	  A2
};

/*
 * Miscellaneous Menu - Set Input Clock	(M7,B)
 */

MENU_DATA_ENTRY_t setinpclkselEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[Input Clocks]\n",
	  "1) ESR\n"
 	  "2) INPUT\n"
      "3) PRS\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setinpclkpriEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[Priority]\n",
	  "1) priority-1\n"
	  "2) priority-2\n"
      "3) priority-3\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setinpclkParmLst[] = {
&setinpclkselEntry, &setinpclkpriEntry};
#ifdef SIMUL
#define ed_clockinput_priority "ed_clockinput_priority"
#else
extern void ed_clockinput_priority();
#endif

MENU_DATA_TBL_t setinpclkMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_CFG,
      1,
      (void *)ed_clockinput_priority,
      (void *)setinpclkParmLst,
	  A6
};

/*
 * Miscellaneous Menu - view Input Clock	(M7,C)
 */

#ifdef SIMUL
#define rtrv_clockinput_priority "rtrv_clockinput_priority"
#else
extern void rtrv_clockinput_priority();
#endif

MENU_DATA_TBL_t viewinpclkMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_CFG,
      1,
      (void *)rtrv_clockinput_priority,
      NULL,
	  A2
};

/*
 * Miscellaneous Menu - View Input SSM	(M7,D)
 */

MENU_DATA_ENTRY_t viewinpssmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Input Clocks]\n",
	  "1) LINK1\n"
      "2) LINK2\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewinpssmParmLst[] = {
&viewinpssmEntry};
#ifdef SIMUL
#define rtrv_clockinput_ssm "rtrv_clockinput_ssm"
#else
extern void rtrv_clockinput_ssm();
#endif

MENU_DATA_TBL_t viewinpssmMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      1,
      (void *)rtrv_clockinput_ssm,
      (void *)viewinpssmParmLst,
	  A2
};

/*
 * Miscellaneous Menu - Set Output SSM	(M7,E)
 */

MENU_DATA_ENTRY_t setoutssmshEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Shelf]\n",
	  "1) MAIN\n"
      "2) EXP1\n"
	  "3) EXP2\n"
	  "4) EXP3\n"
	  "5) EXP4\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setoutssmslEntry = {
      MENU_DATA_PARM_SEL,
      1,
      16,
      MENU_DSP_BODY,
      "\n[Output Slot Selection]\n",
	  "1) OUT1\n"
      "2) OUT2\n"
      "3) OUT3\n"
      "4) OUT4\n"
      "5) OUT5\n"
      "6) OUT6\n"
      "7) OUT7\n"
      "8) OUT8\n"
      "9) OUT9\n"
      "A) OUT10\n"
      "B) OUT11\n"
      "C) OUT12\n"
      "D) OUT13\n"
      "E) OUT14\n"
      "F) OUT15\n"
      "G) OUT16\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setoutssmvalEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[SSM Value]\n",
	  "1) PRS\n"
      "2) STU\n"
      "3) Stratum 2\n"
      "4) Stratum3\n"
      "5) SIC\n"
      "6) DUC\n",
	  MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setoutssmParmLst[] = {
&setoutssmshEntry, &setoutssmslEntry, &setoutssmvalEntry};
#ifdef SIMUL
#define ed_outp_ssm "ed_outp_ssm"
#else
extern void ed_outp_ssm();
#endif

MENU_DATA_TBL_t setoutssmMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_CFG,
      1,
      (void *)ed_outp_ssm,
      (void *)setoutssmParmLst,
	  A7
};

/*
 * Miscellaneous Menu - View Output SSM	(M7,F)
 */

MENU_DATA_ENTRY_t viewoutssmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Shelf]\n",
	  "1) MAIN SHELF\n"
      "2) EXPANSION 1 SHELF\n"
      "3) EXPANSION 2 SHELF\n"
      "4) EXPANSION 3 SHELF\n"
      "5) EXPANSION 4 SHELF\n"
      "*) All\n",      
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewoutssmslEntry = {
      MENU_DATA_PARM_SEL,
      1,
      16,
      MENU_DSP_BODY,
      "\n[Output Slot Selection]\n",
	  "1) OUT1\n"
      "2) OUT2\n"
      "3) OUT3\n"
      "4) OUT4\n"
      "5) OUT5\n"
      "6) OUT6\n"
      "7) OUT7\n"
      "8) OUT8\n"
      "9) OUT9\n"
      "A) OUT10\n"
      "B) OUT11\n"
      "C) OUT12\n"
      "D) OUT13\n"
      "E) OUT14\n"
      "F) OUT15\n"
      "G) OUT16\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewoutssmParmLst[] = {
&viewoutssmEntry, &viewoutssmslEntry};
#ifdef SIMUL
#define rtrv_outp_ssm "rtrv_outp_ssm"
#else
extern void rtrv_outp_ssm();
#endif

MENU_DATA_TBL_t viewoutssmMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_CFG,
      1,
      (void *)rtrv_outp_ssm,
      (void *)viewoutssmParmLst,
	  A2
};

/*
 * Miscellaneous Menu - Reset Shelf	(M7,G)
 */

MENU_DATA_ENTRY_t resetshelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Shelf]\n",
	  "1) MAIN\n"
      "2) EXP1\n"
	  "3) EXP2\n"
	  "4) EXP3\n"
	  "5) EXP4\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t resetshelfrstEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Reset]\n",
	  "1) NO ACTION\n"
      "2) RESET\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *resetshelfParmLst[] = {
&resetshelfEntry, &resetshelfrstEntry};
#ifdef SIMUL
#define reset_shelf "reset_shelf"
#else
extern void reset_shelf();
#endif

MENU_DATA_TBL_t resetshelfMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_CFG,
      1,
      (void *)reset_shelf,
      (void *)resetshelfParmLst,
	  A7
};

/*
 * Miscellaneous Menu - View Shelf Information	(M7,H)
 */

MENU_DATA_ENTRY_t viewselfinfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Shelf]\n",
	  "1) MAIN\n"
      "2) EXP1\n"
	  "3) EXP2\n"
	  "4) EXP3\n"
	  "5) EXP4\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewselfinfParmLst[] = {
&viewselfinfEntry};
#ifdef SIMUL
#define rtrv_shelf "rtrv_shelf"
#else
extern void rtrv_shelf();
#endif

MENU_DATA_TBL_t viewselfinfMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      1,
      (void *)rtrv_shelf,
      (void *)viewselfinfParmLst,
	  A2
};

/*
 * Miscellaneous Menu - Set Output Redundancy	(M7,I)
 */

MENU_DATA_ENTRY_t setoutpredshelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Shelf]\n",
	  "1) MAIN SHELF\n"
      "2) EXPANSION 1 SHELF\n"
      "3) EXPANSION 2 SHELF\n"
      "4) EXPANSION 3 SHELF\n"
      "5) EXPANSION 4 SHELF\n",      
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setoutpredslotEntry = {
      MENU_DATA_PARM_SEL,
      1,
      16,
      MENU_DSP_BODY,
      "\n[Output Slot Selection]\n",
	  "1) OUT1\n"
      "2) OUT2\n"
      "3) OUT3\n"
      "4) OUT4\n"
      "5) OUT5\n"
      "6) OUT6\n"
      "7) OUT7\n"
      "8) OUT8\n"
      "9) OUT9\n"
      "A) OUT10\n"
      "B) OUT11\n"
      "C) OUT12\n"
      "D) OUT13\n"
      "E) OUT14\n"
      "F) OUT15\n"
      "G) OUT16\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setoutpredstatEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Redundancy State]\n",
	  "1) ACTIVE\n"
      "2) STANDBY\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setoutpredParmLst[] = {
&setoutpredshelfEntry, &setoutpredslotEntry, &setoutpredstatEntry};
#ifdef SIMUL
#define ed_outp_red "ed_outp_red"
#else
extern void ed_outp_red();
#endif

MENU_DATA_TBL_t setOutpRedMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_CFG,
      1,
      (void *)ed_outp_red,
      (void *)setoutpredParmLst,
	  A6
};

/*
 * Miscellaneous Menu - View Output Redundancy	(M7,J)
 */

MENU_DATA_ENTRY_t viewoutpredshelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Shelf]\n",
	  "1) MAIN SHELF\n"
      "2) EXPANSION 1 SHELF\n"
      "3) EXPANSION 2 SHELF\n"
      "4) EXPANSION 3 SHELF\n"
      "5) EXPANSION 4 SHELF\n"
      "*) All\n",      
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewoutpredslotEntry = {
      MENU_DATA_PARM_SEL,
      1,
      16,
      MENU_DSP_BODY,
      "\n[Output Slot Selection]\n",
	  "1) OUT1\n"
      "2) OUT2\n"
      "3) OUT3\n"
      "4) OUT4\n"
      "5) OUT5\n"
      "6) OUT6\n"
      "7) OUT7\n"
      "8) OUT8\n"
      "9) OUT9\n"
      "A) OUT10\n"
      "B) OUT11\n"
      "C) OUT12\n"
      "D) OUT13\n"
      "E) OUT14\n"
      "F) OUT15\n"
      "G) OUT16\n"
      "*) All\n",
      MENU_ALL_AST | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewoutpredParmLst[] = {
&viewoutpredshelfEntry, &viewoutpredslotEntry};
#ifdef SIMUL
#define rtrv_outp_red "rtrv_outp_red"
#else
extern void rtrv_outp_red();
#endif

MENU_DATA_TBL_t viewOutpRedMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_CFG,
      1,
      (void *)rtrv_outp_red,
      (void *)viewoutpredParmLst,
	  A2
};


MENU_DATA_TBL_t *miscMenuLst[] = {
NULL, &rebootMenu, &retrieveInvMenu, &setinpclkOpMenu, &viewInpOpModeMenu, &swapClockMenu, 
&viewClockMenu,	&swapinputMenu, &viewinputMenu, &swapoutputMenu, &viewgpsdataMenu, &setinpclkMenu, 
&viewinpclkMenu, &viewinpssmMenu, &setoutssmMenu, &viewoutssmMenu, &resetshelfMenu, &viewselfinfMenu,
&setOutpRedMenu, &viewOutpRedMenu};

MENU_TBL_t miscMenu = {
      MENU_ENTRY_SEL,
      0,
      19,
      "\n\n[MISCELLANEOUS MENU]\n",
      "\n"
      "1) Reboot\n"
      "2) Retrieve Inventory\n"
      "3) Set Input Clock Operation Mode\n"
	  "4) View Input Clock Operation Mode\n"
      "5) Switch Clock\n"
	  "6) View Clock\n"
      "7) Switch Input\n"
	  "8) View Input\n"
	  "9) Switch Output Card\n"
      "A) View GPS Data\n"
	  "B) Set Input Clock Priority\n"
      "C) View Input Clock Priority\n"
      "D) View Input SSM\n"
      "E) Set Output SSM\n"
      "F) View Output SSM\n"
      "G) Reset Shelf\n"
      "H) View Shelf Information\n"
	  "I) Set Output Redundancy\n"
	  "J) View Output Redundancy\n"
      "0) Main Menu\n",
      (void *)miscMenuLst
};



