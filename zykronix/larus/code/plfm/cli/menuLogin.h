
MENU_DATA_ENTRY_t loginUserEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "Login\n",
      "Username: ",
      MENU_NULL_IN_ERROR,
      1,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};

MENU_DATA_ENTRY_t loginPswdEntry = {
      MENU_DATA_PARM_SEL,
      1,
      10,
      MENU_DSP_BODY,
      "",
      "Password: ",
      MENU_NULL_IN_ERROR | MENU_SECUR_ECHO,
      2,
      MENU_IN_TYPE_STR,
      MAP_ARG_NOMAP,
      NULL,
      NULL
};


MENU_DATA_ENTRY_t *usrLoginParmLst[] = {
&loginUserEntry, &loginPswdEntry};

extern void act_user();

MENU_DATA_TBL_t usrLoginMenu = {
      MENU_DATA_TBL_SEL,
      2,
      MENU_CMD_CFG,
      0,
      (void *)act_user,
      (void *)usrLoginParmLst
};


MENU_DATA_TBL_t *loginMenuLst[] = {
    &usrLoginMenu};


MENU_TBL_t loginMenu = {
      MENU_ENTRY_SEL,
      0,
      0,
      "[MAIN MENU]\n",
      "0) Log In\n\n",
      (void *)loginMenuLst,
};
