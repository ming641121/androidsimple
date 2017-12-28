

typedef struct {
    OID        oid;
    ObjectInfo *objPtr;
    U8_t       *opt;
    U16_t      argType;
    U16_t      mapCnt;
    void       *mapFunc;
    U8_t       *argHelpMsg;
    U8_t       *objName;
    U8_t       *objDesc;
} CLIMIBENTITY_t, *pCLIMIBENTITY_t;

typedef struct {
    OID        oid;
    ObjectInfo *objPtr;
    U8_t       *opt;
    U16_t      argType;
    U16_t      mapCnt;
    void       *mapFunc;
    U8_t       *argHelpMsg;
    U8_t       *objName;
    U8_t       *objDesc;
} CLIMIBENTITY_t, *pCLIMIBENTITY_t;

typedef struct {
    pCLIMIBENTITY_t *mibKeyList;
    pCLIMIBENTITY_t *mibList;
    U16_t           keyLen;
    U16_t           numOfMibEnt;
} CLIMIBCFGGROUP_t, *pCLIMIBCFGGROUP_t;

{
      "[Baud Rate]"
      "1) 1200"
      "2) 2400"
      "3) 4800"
      "4) 9600"
      "<CR>) No Change";
      &termBaudMib;
};

pCLIMIBENTITY *termEntryMib[] = {
    &termBaudMib, &termDatBitMib, &termParMib, &termStopMib, NULL
};


typedef struct MENU_ENTRY_s {
     U8_t   entryType;
     

} MENU_ENTRY_t;

struct {
     
}

struct menu_table {
     U8_t              menuType;    /* MENUAL SELECTION, DATA SELECTION */
     U8_t              *menuBody;
     MENU_ENTRY_t      **menuList;
} MENU_TBL_t;

struct menu_table {
     U8_t              *menuBanner;
     U8_t              *menuBody;
     MENU_ENTRY_t      **menuList;
} MENU_TBL_t;


extern MENU_TBL_t mainMenu;

MENU_ENTRY_t mainEntry = {
      MENU_SELETION;
      &mainMenu;
};

MENU_ENTRY_t loginEntry = {
      NULL;
      MENU_LOGIN;

MENU_TBL_t termMenu = {
      "[Terminal]";
      "1) Local Terminal (CID)"
      "2) Remote Terminal (OD)"
      "<CR>) This Terminal"
      
          
MENU_ENTRY_t serviceEntry = {
      MENU_SELETION;
      &serviceMenu;
};



MENU_ENTRY_t *mainEntryList[] = {
      NULL;
      
      &mainEntry;
      &serviceEntry;

      NULL};

MENU_TBL_t mainMenu = {
      "[MAIN MENU]";
      "1) Service Parameters"
      "2) Performance Monitoring"
      "3) Thresholds"
      "4) Alarm/Event"
      "5) Logfiles"
      "6) Security"
      "7) Miscellaneous"
      ""
      ""
      "0) Log Out";
      mainEntryList;
}

MENU_TBL_t serviceMenu = {
      "[SERVICE MENU]";
      "1) Set Terminal"
      "2) Set SMC"
      "3) View Terminal"
      "4) View SMC"
      "5) Set Date/Time"
      "6) Set System ID"
      "0) Main Menu";
          
MENU_ENTRY_t serviceEntry = {
      MENU_SELETION;
      &serviceMenu;
};
