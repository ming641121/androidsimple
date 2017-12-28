#ifndef _CLICMD_IF_H_
#define _CLICMD_IF_H_
#define MAPDATA_END    {NULL, 0}


#define USER_LOGIN           0x100
#define USER_LOGOUT          0x200
#define FD_ERROR             0x300


#define MENU_IN_TYPE_STR      1
#define MENU_IN_TYPE_NUM      2
#define MENU_IN_TYPE_INT      3
#define MENU_IN_TYPE_BITS     4

/*
 * input type
 */
#define TL1_IN_NUM                MENU_IN_TYPE_NUM
#define TL1_IN_STR                MENU_IN_TYPE_STR
#define TL1_IN_INT                MENU_IN_TYPE_INT
#define TL1_IN_BITS               MENU_IN_TYPE_BITS


#define INBUF_SIZE                120
#define ESC_CHAR                  0x1b
#define EOL_CHAR                  0xa
#define BK_SPACE                  0x8
#define CTRL_C                    0x3

#define CTRL_U                    21
#define CTRL_R                    18

#define A1      1 
#define A2      2
#define A3      3
#define A4      4
#define A5      5
#define A6      6
#define A7      7          
#define A8      8
#define A9      9

typedef struct {
    U8_t  *mapStr;
    U32_t val;
} CLIUMAPSTRUC_t, *pCLIUMAPSTRUC_t;

typedef pCLIUMAPSTRUC_t  u_map_t;

typedef struct {
   U8_t      type;
   U16_t     ttlVal;
   union {
      U32_t  val[32];
      U8_t   str[128];
   } u;
} MENU_DATA_VAL_t;

typedef struct {
    U8_t            userName[20];
    U8_t            ctagBuf[50];
    U32_t           inLogin;
    U32_t           telnetFlag;
    U32_t           sessionId;
} SHELL_VAR_t;

extern void 
tl1PrintErr(U8_t *ctagBuf, U8_t *errCode); 

extern U32_t strmatch(U8_t   *s1,
                U8_t   *s2);

extern U32_t strnmatch(U8_t   *s1,
                 U8_t   *s2,
                 U32_t  cnt);

#endif
