
/*
 * Security Menu - Add User	(M6,1)
 */

MENU_DATA_ENTRY_t addUserUidEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "\n[New User ID]\n",
      "Enter 4 to 10 characters, then <CR>\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t addUserPidEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "\n[New Password]\n",
      "Enter 4 to 10 characters (2 non-alphabetic with 1 symbolic), then <CR>\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t addUserPriLvlEntry = {
      MENU_DATA_PARM_SEL,
      1,
      9,
      MENU_DSP_BODY,
      "\n[Privilege Level]\n",
	  "Enter 1 to 9 (9 has the highest privilege), then <CR>\n",
      MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_INT,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *addUserParmLst[] = {
&addUserUidEntry, &addUserPidEntry, &addUserPriLvlEntry};
#ifdef SIMUL
#define ent_user_secu "ent_user_secu"
#else
extern void ent_user_secu();
#endif

MENU_DATA_TBL_t addUserMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_CFG,
      0,
      (void *)ent_user_secu,
      (void *)addUserParmLst,
	  A7
};

/*
 * Security Menu - Edit User (M6,2)
 */

MENU_DATA_ENTRY_t editUserOidEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "\n[Existing User ID]\n",
	  "Enter 4 to 10 characters, then <CR>\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t editUserNidEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "\n[New User IDs]\n",
      "Enter 4 to 10 characters, then <CR>\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t editUserPidEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "\n[New Password]\n",
      "Enter 4 to 10 characters (2 non-alphabetic with 1 symbolic) with <CR>\n"
	  "<CR>) for no change\n",
      MENU_NULL_IN_ERROR,
      2,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t editUserPriLvlEntry = {
      MENU_DATA_PARM_SEL,
      1,
      9,
      MENU_DSP_BODY,
      "\n[New Privilege Level]\n",
	  "Enter 1 to 9 (9 has the highest privilege), then <CR>\n",
	  MENU_NULL_IN_ERROR,
      3,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *editUserParmLst[] = {
&editUserOidEntry, &editUserNidEntry, &editUserPidEntry, &editUserPriLvlEntry};
#ifdef SIMUL
#define ed_user_secu "ed_user_secu"
#else
extern void ed_user_secu();
#endif

MENU_DATA_TBL_t editUserMenu = {
      MENU_DATA_TBL_SEL,
      3,
      MENU_CMD_CFG,
      0,
      (void *)ed_user_secu,
      (void *)editUserParmLst,
	  A7
};

/*
 * Security Menu - View User (M6,3)
 */

MENU_DATA_ENTRY_t viewUserIdEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "\n[User ID]\n",
      "Enter User ID or All, then <CR>\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *viewUserParmLst[] = {
&viewUserIdEntry};
#ifdef SIMUL
#define rtrv_user_secu "rtrv_user_secu"
#else
extern void rtrv_user_secu();
#endif

MENU_DATA_TBL_t viewUserMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_DSP,
      0,
      (void *)rtrv_user_secu,
      (void *)viewUserParmLst,
	  A2
};

/*
 * Security Menu - Delete User (M6,4)
 */

MENU_DATA_ENTRY_t delUserIdEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "\n[User IDs]\n",
      "Enter Existing User ID, then <CR>\n",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t *delUserParmLst[] = {
&delUserIdEntry};
#ifdef SIMUL
#define dlt_user_secu "dlt_user_secu"
#else
extern void dlt_user_secu();
#endif

MENU_DATA_TBL_t delUserMenu = {
      MENU_DATA_TBL_SEL,
      1,
      MENU_CMD_CFG,
      0,
      (void *)dlt_user_secu,
      (void *)delUserParmLst,
	  A7
};




MENU_DATA_TBL_t *securityMenuLst[] = {
NULL, &addUserMenu, &editUserMenu, &viewUserMenu, &delUserMenu};

MENU_TBL_t securityMenu = {
      MENU_ENTRY_SEL,
      0,
      6,
      "\n\n[SECURITY MENU]\n",
      "\n"
      "1) Add User\n"
      "2) Edit User\n"
      "3) View User\n"
      "4) Delete User\n"
      "0) Main Menu\n",
      (void *)securityMenuLst
};
