

MENU_DATA_ENTRY_t setTermBaudEntry = {
      MENU_DATA_PARM_SEL,
      1,
      6,
      MENU_DSP_BODY,
      "\n[Baud Rate]\n",
      "1) 1200\n"
      "2) 2400\n"
      "3) 4800\n"
      "4) 9600\n"
	  "5) 19200\n"
	  "6) 38400\n"
      "<CR>) No Change\n",
      MENU_NULL_IN_NOCHG,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setTermBitEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Data Bits]\n",
      "1) 7\n"
      "2) 8\n"
      "<CR>) No Change\n",
      MENU_NULL_IN_NOCHG,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setTermParityEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[Parity]\n",
      "1) NONE\n"
      "2) ODD\n"
      "3) EVEN\n"
      "<CR>) No Change\n",
      MENU_NULL_IN_NOCHG,
      3,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};


MENU_DATA_ENTRY_t setTermStopEntry = {
      MENU_DATA_PARM_SEL,
      1,
      3,
      MENU_DSP_BODY,
      "\n[Stop Bits]\n",
      "1) 1\n"
      "2) 1.5\n"
      "3) 2\n"
      "<CR>) No Change\n",
      MENU_NULL_IN_NOCHG,
      4,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setTermParmLst[] = {
&setTermBaudEntry, &setTermBitEntry, &setTermParityEntry, &setTermStopEntry};

#ifdef SIMUL
#define ed_term   "ed_term"
#else
extern void ed_term();
#endif

MENU_DATA_TBL_t setTermMenu = {
      MENU_DATA_TBL_SEL,
      4,
      MENU_CMD_CFG,
      1,
      (void *)ed_term,
      (void *)setTermParmLst,
	  AXS_PRI_4
};

/*
 * Service Menu - Set Link (M1,2)
 */

MENU_DATA_ENTRY_t setLinkEntry = {
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

MENU_DATA_ENTRY_t setLinkIdEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[New ID]\n",
	  "<CR>) No Change\n",
      MENU_NULL_IN_NOCHG,
      2,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setLinkTypeEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Line Type]\n",
      "1) E1\n"
      "2) T1\n"
      "3) CC\n"
	  "4) F2\n"
	  "5) J1\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setLinkFramingEntry = {
      MENU_DATA_PARM_SEL,
      1,
      11,
      MENU_DSP_BODY,
      "\n[Framing]\n",
      "1) T1-SF\n"
      "2) T1-ESF\n"
      "3) J1-SF\n"
	  "4) J1-ESF\n"
	  "5) E1\n"
	  "6) E1-CRC\n"
	  "7) E1-MF\n"
	  "8) E1-CRC-MF\n"
	  "9) F2\n"
	  "A) CC\n"
	  "B) CC-400\n"
	  "<CR>) No Change\n",
	  MENU_NULL_IN_NOCHG,
      4,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setLinkCodeEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Line Code]\n",
      "1) AMI\n"
      "2) B8ZS\n"
	  "3) HDB3\n"
      "4) ZBTSI\n"
      "5) JBZS\n"
	  "<CR>) No Change\n",
	  MENU_NULL_IN_NOCHG,
      5,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setLinkParmLst[] = {
&setLinkEntry, &setLinkIdEntry, &setLinkTypeEntry, &setLinkFramingEntry, &setLinkCodeEntry};

#ifdef SIMUL
#define set_link "set_link"
#else
extern void set_link();
#endif

MENU_DATA_TBL_t setLinkMenu = {
      MENU_DATA_TBL_SEL,
      5,
      MENU_CMD_CFG,
      1,
      (void *)set_link,
      (void *)setLinkParmLst,
	  AXS_PRI_2
};

/*
 * Service Menu - View Terminal	(M1,3)
 */
#ifdef SIMUL
#define view_term "view_term"
#else
extern void view_term();
#endif

MENU_DATA_TBL_t viewTermMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_DSP,
      1,
      (void *)view_term,
      NULL,
	  AXS_PRI_6
};

/*
 * Service Menu - View Link	(M1,4)
 */

MENU_DATA_ENTRY_t viewLinkEntry = {
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

MENU_DATA_ENTRY_t *viewLinkParmLst[] = {
&viewLinkEntry};

#ifdef SIMUL
#define view_link "view_link"
#else
extern void view_link();
#endif

MENU_DATA_TBL_t viewLinkMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_DSP,
      1,
      (void *)view_link,
      (void *)viewLinkParmLst,
	  AXS_PRI_6
};

/*
 * Service Menu - Set Date/Time	(M1,5)
 */

MENU_DATA_ENTRY_t setDateEntry = {
      MENU_DATA_PARM_SEL,
      13,
      13,
      MENU_DSP_BODY,
      "\n[Date]\n",
      "YYMMDD (or blank for no change) then <CR>\n",
      MENU_NULL_IN_NOCHG,
      1,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setTimeEntry = {
      MENU_DATA_PARM_SEL,
      13,
      13,
      MENU_DSP_BODY,
      "\n[Time]\n",
      "HHMMSS (or blank for no change) then <CR>\n",
      MENU_NULL_IN_NOCHG,
      2,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setZoneEntry = {
      MENU_DATA_PARM_SEL,
      -48,
      52,
      MENU_DSP_BODY,
      "\n[Time Zone]\n",
      "-48 thru 52 (or blank for OFF) then <CR>\n",
      MENU_NULL_IN_NOCHG,
      2,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setTimeParmLst[] = {
&setDateEntry, &setTimeEntry, &setZoneEntry};
#ifdef SIMUL
#define set_dat "set_dat"
#else
extern void set_dat();
#endif

MENU_DATA_TBL_t setTimeMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_CFG,
      1,
      (void *)set_dat,
      (void *)setTimeParmLst,
	  AXS_PRI_4
};

/*
 * Service Menu - Set System ID	(M1,6)
 */

MENU_DATA_ENTRY_t setSysIdEntry = {
      MENU_DATA_PARM_SEL,
      1,
      24,
      MENU_DSP_BODY,
      "\n[New System ID]\n",
      "<CR>) No Change\n",
      MENU_NULL_IN_NOCHG,
      1,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setSysIdParmLst[] = {
&setSysIdEntry};

#ifdef SIMUL
#define set_sid "set_sid"
#else
extern void set_sid();
#endif

MENU_DATA_TBL_t setSysIdMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      1,
      (void *)set_sid,
      (void *)setSysIdParmLst,
	  AXS_PRI_4
};

/*
 * Service Menu - Set Output Port Name (M1,7)
 */

MENU_DATA_ENTRY_t setOutpNameShelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Shelf Selection]\n",
	  "1) MAIN SHELF\n"
      "2) EXPANSION 1 SHELF\n"
      "3) EXPANSION 2 SHELF\n"
      "4) EXPANSION 3 SHELF\n"
      "5) EXPANSION 4 SHELF\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setOutpNameSlotEntry = {
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
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setOutpNamePortEntry = {
      MENU_DATA_PARM_SEL,
      1,
      8,
      MENU_DSP_BODY,
      "\n[Output Port Selection]\n",
	  "1) PORT1\n"
	  "2) PORT2\n"
	  "3) PORT3\n"
	  "4) PORT4\n"
	  "5) PORT5\n"
	  "6) PORT6\n"
	  "7) PORT7\n"
	  "8) PORT8\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setOutpNameTypeEntry = {
      MENU_DATA_PARM_SEL,
      1,
      11,
      MENU_DSP_BODY,
      "\n[Port Type]\n",
	  "1) T1\n"
      "2) E1\n"
      "3) CC\n"
      "4) SQW\n"
      "5) I-RIGB\n"	 
      "6) 1PPS\n"
      "7) 1PPM\n"
      "8) 1PPH\n"
      "9) TIU\n"
      "A) F5\n"
      "B) F10\n",
      MENU_NULL_IN_ERROR,
      4,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setOutpNameEntry = {
      MENU_DATA_PARM_SEL,
      0,
      20,
      MENU_DSP_BODY,
      "\n[Port Name]\n",
      "Enter the Port Name (Up to 20 characters), then <CR>\n",
      MENU_NULL_IN_ERROR,
      5,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setOutpNameParmLst[] = {
&setOutpNameShelfEntry, &setOutpNameSlotEntry, &setOutpNamePortEntry,
&setOutpNameTypeEntry, &setOutpNameEntry};
#ifdef SIMUL
#define set_outp_name "set_outp_name"
#else
extern void set_outp_name();
#endif

MENU_DATA_TBL_t setOutpNameMenu = {
      MENU_DATA_TBL_SEL,
      5,
      MENU_CMD_CFG,
      1,
      (void *)set_outp_name,
      (void *)setOutpNameParmLst,
	  AXS_PRI_4
};

/*
 * Service Menu - View Output Port Name	(M1,8)
 */

MENU_DATA_ENTRY_t viewOutpNameShelfEntry = {
      MENU_DATA_PARM_SEL,
      1,
      5,
      MENU_DSP_BODY,
      "\n[Shelf Selection]\n",
	  "1) MAIN SHELF\n"
      "2) EXPANSION 1 SHELF\n"
      "3) EXPANSION 2 SHELF\n"
      "4) EXPANSION 3 SHELF\n"
      "5) EXPANSION 4 SHELF\n"
      "*) All\n",
      MENU_ALL_AST | MENU_MULTI_IN | MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewOutpNameSlotEntry = {
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
      MENU_ALL_AST | MENU_MULTI_IN | MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t viewOutpNamePortEntry = {
      MENU_DATA_PARM_SEL,
      1,
      8,
      MENU_DSP_BODY,
      "\n[Output Port Selection]\n",
	  "1) PORT1\n"
	  "2) PORT2\n"
	  "3) PORT3\n"
	  "4) PORT4\n"
	  "5) PORT5\n"
	  "6) PORT6\n"
	  "7) PORT7\n"
	  "8) PORT8\n"
	  "*) All\n",
      MENU_ALL_AST | MENU_MULTI_IN | MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewOutpNameParmLst[] = {
&viewOutpNameShelfEntry, &viewOutpNameSlotEntry, &viewOutpNamePortEntry};

#ifdef SIMUL
#define view_outp_name "view_outp_name"
#else
extern void view_outp_name();
#endif

MENU_DATA_TBL_t viewOutpNameMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_DSP,
      1,
      (void *)view_outp_name,
      (void *)viewOutpNameParmLst,
	  AXS_PRI_6
};

/*
 * Service Menu - Set Management IP	(M1,9)
 */

MENU_DATA_ENTRY_t setMgmtIpOptionEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[IP Option]\n",
	  "1) STATIC\n"
      "2) DHCP\n"
      "<CR>) No Change\n",
      MENU_NULL_IN_NOCHG,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setMgmtIpAddrEntry = {
      MENU_DATA_PARM_SEL,
      7,
      19,
      MENU_DSP_BODY,
      "\n[IP Address]\n",
	  "xxxx.xxxx.xxxx.xxxx (or blank for DHCP) then <CR>\n",
      MENU_NULL_IN_NOCHG,
      2,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setMgmtIpSnetEntry = {
      MENU_DATA_PARM_SEL,
      7,
      19,
      MENU_DSP_BODY,
      "\n[IP Subnet Mask]\n",
	  "xxxx.xxxx.xxxx.xxxx (or blank for DHCP) then <CR>\n",
      MENU_NULL_IN_NOCHG,
      3,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setMgmtIpGWayEntry = {
      MENU_DATA_PARM_SEL,
      7,
      19,
      MENU_DSP_BODY,
      "\n[Gateway IP Address]\n",
	  "xxxx.xxxx.xxxx.xxxx (or blank for DHCP) then <CR>\n",
      MENU_NULL_IN_NOCHG,
      4,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setMgmtIpParmLst[] = {
&setMgmtIpOptionEntry, &setMgmtIpAddrEntry, &setMgmtIpSnetEntry, &setMgmtIpGWayEntry};

#ifdef SIMUL
#define set_mgmtip "set_mgmtip"
#else
extern void set_mgmtip();
#endif

MENU_DATA_TBL_t setMgmtIpMenu = {
      MENU_DATA_TBL_SEL,
      4,
      MENU_CMD_CFG,
      1,
      (void *)set_mgmtip,
      (void *)setMgmtIpParmLst,
	  AXS_PRI_4
};

/*
 * Service Menu - View Management IP (M1, A)
 */
#ifdef SIMUL
#define rtrv_mgmtip "rtrv_mgmtip"
#else
extern void rtrv_mgmtip();
#endif

MENU_DATA_TBL_t viewMgmtIpMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_mgmtip,
      NULL,
	  AXS_PRI_6
};

/*
 * Service Menu - Set NTP IP (M1,B)
 */

MENU_DATA_ENTRY_t setNtpIpOptionEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[IP Option]\n",
	  "1) STATIC\n"
      "2) DHCP\n"
      "<CR>) No Change\n",
      MENU_NULL_IN_NOCHG,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setNtpIpAddrEntry = {
      MENU_DATA_PARM_SEL,
      7,
      19,
      MENU_DSP_BODY,
      "\n[IP Address]\n",
	  "xxxx.xxxx.xxxx.xxxx (or blank for DHCP) then <CR>\n",
      MENU_NULL_IN_NOCHG,
      2,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setNtpIpSnetEntry = {
      MENU_DATA_PARM_SEL,
      7,
      19,
      MENU_DSP_BODY,
      "\n[IP Subnet Mask]\n",
	  "xxxx.xxxx.xxxx.xxxx (or blank for DHCP) then <CR>\n",
      MENU_NULL_IN_NOCHG,
      3,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t setNtpIpGWayEntry = {
      MENU_DATA_PARM_SEL,
      7,
      19,
      MENU_DSP_BODY,
      "\n[Gateway IP Address]\n",
	  "xxxx.xxxx.xxxx.xxxx (or blank for DHCP) then <CR>\n",
      MENU_NULL_IN_NOCHG,
      4,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setNtpIpParmLst[] = {
&setNtpIpOptionEntry, &setNtpIpAddrEntry, &setNtpIpSnetEntry, &setNtpIpGWayEntry};
#ifdef SIMUL
#define set_ntpip "set_ntpip"
#else
extern void set_ntpip();
#endif

MENU_DATA_TBL_t setNtpIpMenu = {
      MENU_DATA_TBL_SEL,
      4,
      MENU_CMD_CFG,
      1,
      (void *)set_ntpip,
      (void *)setNtpIpParmLst,
	  AXS_PRI_4
};

/*
 * Service Menu - View NTP IP (M1,C)
 */
#ifdef SIMUL
#define rtrv_ntpip "rtrv_ntpip"
#else
extern void rtrv_ntpip();
#endif

MENU_DATA_TBL_t viewNtpIpMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_ntpip,
      NULL,
	  AXS_PRI_6
};

/*
 * Service Menu - Set Alarm Escalation Time	(M1,D)
 */

MENU_DATA_ENTRY_t setAlmEscTmEntry = {
      MENU_DATA_PARM_SEL,
      1,
      24,
      MENU_DSP_BODY,
      "\n[Escalation Time]\n",
      "(1 thru 24 Hours) then <CR>\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setAlmEscTmParmLst[] = {
&setAlmEscTmEntry};
#ifdef SIMUL
#define ed_alarmescalating_time "ed_alarmescalating_time"
#else
extern void ed_alarmescalating_time();
#endif

MENU_DATA_TBL_t setAlmEscTmMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      1,
      (void *)ed_alarmescalating_time,
      (void *)setAlmEscTmParmLst,
	  AXS_PRI_2
};

/*
 * Service Menu - Set System Software Upgrade (M1,E)
 */

MENU_DATA_ENTRY_t setSwUpgEntry = {
      MENU_DATA_PARM_SEL,
      1,
      2,
      MENU_DSP_BODY,
      "\n[Upgrade]\n",
	  "1) NO ACTION\n"
      "2) UPGRADE\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_NUM,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *setSwUpgParmLst[] = {
&setSwUpgEntry};
#ifdef SIMUL
#define set_syssw_upgrade "set_syssw_upgrade"
#else
extern void set_syssw_upgrade();
#endif

MENU_DATA_TBL_t setSwUpgMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      1,
      (void *)set_syssw_upgrade,
      (void *)setSwUpgParmLst,
	  AXS_PRI_4
};

/*
 * Service Menu - View System Software Revision	(M1,F)
 */
#ifdef SIMUL
#define rtrv_syssw_version "rtrv_syssw_version"
#else
extern void rtrv_syssw_version();
#endif

MENU_DATA_TBL_t viewSwRevMenu = {
      MENU_DATA_TBL_SEL,
      0,
      MENU_CMD_DSP,
      1,
      (void *)rtrv_syssw_version,
      NULL,
	  AXS_PRI_6
};

MENU_DATA_TBL_t *serviceMenuLst[] = {
NULL, &setTermMenu, &setLinkMenu, &viewTermMenu, &viewLinkMenu, &setTimeMenu,
&setSysIdMenu, &setOutpNameMenu, &viewOutpNameMenu, &setMgmtIpMenu, &viewMgmtIpMenu, 
&setNtpIpMenu, &viewNtpIpMenu, &setAlmEscTmMenu, &setSwUpgMenu, &viewSwRevMenu};

MENU_TBL_t serviceMenu = {
      MENU_ENTRY_SEL,
      0,
      15,
      "\n\n[SERVICE MENU]\n",
	  "\n"
      "1) Set Terminal\n"
      "2) Set Link\n"
      "3) View Terminal\n"
      "4) View Link\n"
      "5) Set Date/Time\n"
      "6) Set System ID\n"
	  "7) Set Output Port Name\n"
	  "8) View Output Port Name\n"
	  "9) Set Management IP\n"
	  "A) View Management IP\n"
	  "B) Set NTP IP\n"
	  "C) View NTP IP\n"
	  "D) Set Alarm Escalation Time\n"
	  "E) Set System Software Upgrade\n"
      "F) View System Software Revision\n"
      "0) Main Menu\n",
      (void *)serviceMenuLst
};
