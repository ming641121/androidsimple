%{
/*
 *
 * Copyright (C) 1999-2003 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================== NOTES =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* 

Productions beginning with a lowercase letter may be empty.

Productions beginning with an uppercase letter may NOT be empty.

A file may contain multiple MIB modules or no MIB at all.

_ is used in tokens instead of -, yacc/bison doesn't like -.

_ is appended to some tokens to avoid confusion with
ASN.1 and C keywords.

*/

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== INCLUDES ========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#ifndef SR_WINDOWS
#include <unistd.h>
#endif /* SR_WINDOWS */
 
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== DEFINES =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

#define NUM_MIB_REPLACEMENTS    44

#define SAVE_FOR_ALL_PARSES     0
#define SINGLE_PARSE            1

#define NORMAL_PARSE            0 
#define IMPORT_PARSE            1 

/* standard child locations */
#define child_identity_0        0
#define child_syntax___1        1
#define child_status___2        2 
#define child_descript_3        3
#define child_referenc_4        4
#define child_object___5        5
#define child_range____6        6
#define child_index____7        7
#define child_display__8        8
#define child_value____9        9
#define child_default_10        10
#define child_access__11        11
#define chld_strt_rnge12        12
#define chld_end_range13        13

#define child_misc1___14        14
#define child_misc2___15        15

#define MAX_CHILDREN            16
#define MYBUFSIZE               8192

/* table size = use a prime number for the table size to minimize
 * the number of collisions, and thus maximize speed.
 */
#define TABLE_SIZE              499

#define YYERROR_VERBOSE 1 /* allow verbose bison error msgs */
/* #define YYDEBUG 1 */    /* only define during debug phase */

#ifdef SUNOS
#define strtoul(x, y, z) ((unsigned long) strtol(x, y, z))
#endif /* SUNOS */


/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== STORAGE STRUCTS  ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* node in tree or symbol table */
typedef struct tree_tag {

    unsigned long number;       /* type tells neg from pos          */

    int type;                   /* use token values e.g. currrent   */

    char * string;              /* real data storage                */
    int freeMe;

    /* PARENT */
    struct tree_tag * parent;

    /* TREE CHILDREN */
    struct tree_tag * child[MAX_CHILDREN];
 
    /* TREE SIBLING */
    struct tree_tag * next_tree_node;
    struct tree_tag * last_tree_node_in_list;

    /* SYMBOL TABLE SIBLING */
    struct tree_tag * next_symbol_node;

    /* table of enumerated items for object */
    struct tree_tag * * enumeratedItems;

    /* free lists */
    struct tree_tag * freePtr;

} tree;

typedef tree * symbol_table; 

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================= GLOBALS   ======================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* use ; as path separator on windows, : elsewhere */
#ifndef SR_WINDOWS
char * SEPARATOR = ":";
#else
char * SEPARATOR = ";";
#endif /* SR_WINDOWS */

/* ************ */
/* GLOBAL FLAGS */
/* ************ */

int globalSuppressOutputFlag        = 0; /* suppress warnings               */
int globalCppCommentFlag            = 0; /* use C++ style comments          */
int globalProduceDescrFlag          = 0; /* produce description output file */
int globalMultipleOutputFilesFlag   = 1; /* more than one output file       */
int globalRenameFlag                = 0; /* rename input file if filename doesn't match module name */
int helpFlag                        = 0; /* print help info */
int updateImportsFlag               = 0; /* replace MIB import with replacement MIB */

int openOutputFlag = 1;                  /* open the output file */

int importParseFlag                 = 0; /* import parse or normal parse    */

/* save type nodes for all parses */
/* used for AUX-MIB.dat           */
int saveForAllParsesFlag            = 1;

/* GLOBAL LISTS */
tree * globalInputFilenameList   = NULL;
tree * importFromList            = NULL;
tree * globalImportDirectoryList = NULL;

/* ********** */
/* FILE ITEMS */
/* ********** */

extern FILE * yyin;                      /* lexer */

FILE * outputFilePtr        = NULL; 
FILE * inputFilePtr         = NULL; 
FILE * descrFilePtr         = NULL;

char * lastFileParsed       = NULL;
char * lastModuleName       = NULL;
char * descrFileName        = NULL;
char * outputFileName       = NULL;
char * outputDirectoryName  = NULL;

/* ************** */
/* SEMANTIC ITEMS */
/* ************** */

/* once one of the SMIv1 or SMIv2 flags is set by a unique identifier
   that becomes the standard and any attempt to reset is an error */
int SMIv1Flag                = 0;
int SMIv2Flag                = 0;
int SMIv3Flag                = 0;  /* SMIv3 is an augmentation of SMIv2             */




/* **** NOT USED YET **** - future semantic check */
int sawObjectIdentifier      = 0;  /* saw at least one object identifier in MIB     */




/* ************** */
/* OUTPUT FORMAT  */
/* ************** */

int maxColumnWidth           = 20; /* max column width for printing to def file */

/* ************** */
/* WORK BUFFERS   */
/* ************** */

char * lineBuf               = NULL;
char * string                = NULL;
char * workBuf               = NULL;
char * stringBuf             = NULL;
char * fileNameBuffer        = NULL;
char * firstLineBuf          = NULL;

/* ************** */
/* *** LISTS **** */
/* ************** */

tree * singleParseNodeList          = NULL; /* free at end of each parse */
tree * allParsesNodeList            = NULL; /* free at end of all parses */

/* ************** */
/* *** TABLES *** */
/* ************** */

/* for each parse */
symbol_table * virtualTables        = NULL; /* listing of virtual tables, for this parse only */
symbol_table * emittedNodes         = NULL; /* types emitted, for this parse only */
symbol_table * singleParseTypeTable = NULL; /* local and imported type nodes, for this parses */ 
symbol_table * importedMIBs         = NULL; /* table of MIBs that have been imported for this parse */

/* for all parses */
symbol_table * allParsesTypeTable   = NULL; /* types from AUX-MIB.dat for all parses */ 

symbol_table * mibReplacementTable  = NULL; /* MIB supercession table */ 

/* ************** */
/* * PARSE ITEMS **/
/* ************** */

/* normal error tracking */
int line_cntr                = 1;
int col_cntr                 = 0;
int no_code_flag             = 0;

/* unterminated string error tracking */
int stringBufSize            = 0;
int stringPos                = 0;  /* for copying strings */
int string_line_cntr         = 1;
int string_col_cntr          = 0;
int stringWarnOn             = 0;

/* parse tree */
tree * root = NULL;                /* pointer to root of AST */

time_t myclock;
struct tm * currentTime;

/* old mib first, then update or replacment mib */
/* updates before replacements, multiple links on old mib */
char * mibReplacements[NUM_MIB_REPLACEMENTS][2] = {
{"RFC1065-SMI",     "RFC1155-SMI"},
{"RFC1066-MIB",     "RFC1156-MIB"}, 
{"RFC1067-SNMP",    "RFC1157-SNMP"},
{"RFC1098-SNMP",    "RFC1157-SNMP"},

{"RFC1158-MIB",     "IP-MIB"},
{"RFC1158-MIB",     "TCP-MIB"},
{"RFC1158-MIB",     "UDP-MIB"},
{"RFC1158-MIB",     "RFC1213-MIB"},

{"RFC1229-MIB",     "IF-MIB"},
{"RFC1232-MIB",     "IF-MIB"},
{"RFC1233-MIB",     "DLSW-MIB"},
{"RFC1243-MIB",     "APPLETALK-MIB"}, 
{"RFC1271-MIB",     "RMON-MIB"},
{"RFC1284-MIB",     "EtherLike-MIB"},
{"DSA-MIB",         "DIRECTORY-SERVER-MIB"},
{"RFC1289-phivMIB", "DECNET-PHIV-MIB"},
{"RFC1304-MIB",     "SIP-MIB"},
{"RFC1315-MIB",     "FRAME-RELAY-DTE-MIB"},
{"RFC1316-MIB",     "CHARACTER-MIB"},
{"RFC1317-MIB",     "RS-232-MIB"},
{"RFC1318-MIB",     "PARALLEL-MIB"},
{"RFC1354-MIB",     "IP-FORWARD-MIB"},
{"RFC1389-MIB",     "RIPv2-MIB"},
{"RFC1398-MIB",     "EtherLike-MIB"},
{"RFC1406-MIB",     "DS1-MIB"},
{"RFC1407-MIB",     "DS3-MIB"},

{"RFC1248-MIB",     "OSPF-MIB"},
{"RFC1248-MIB",     "OSPF-TRAP-MIB"},

{"RFC1252-MIB",     "OSPF-MIB"},
{"RFC1252-MIB",     "OSPF-TRAP-MIB"},

{"RFC1253-MIB",     "OSPF-MIB"},
{"RFC1253-MIB",     "OSPF-TRAP-MIB"},

{"RFC1286-MIB",     "BRIDGE-MIB"},
{"RFC1286-MIB",     "SOURCE-ROUTING-MIB"},

{"RFC1213-MIB",     "IP-MIB"},
{"RFC1213-MIB",     "TCP-MIB"},
{"RFC1213-MIB",     "UDP-MIB"},
{"RFC1213-MIB",     "RFC1213-MIB"}, 

{"RFC1285-MIB",     "FDDI-SMI73"},
{"RFC1285-MIB",     "RFC1285-MIB"},

{"RFC1231-MIB",     "TOKENRING-STATION-SR-MIB)"},
{"RFC1231-MIB",     "TOKENRING-MIB"},

{"TOKENRING-MIB",   "TOKENRING-STATION-SR-MIB"},
{"TOKENRING-MIB",   "TOKENRING-MIB"}
};

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== FUNCTION PROTOTYPES ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

int yylex(); /* just to get rid of warnings */

void check_for_range_overlap(tree * rangelist);
void check_for_range_restriction_on_type(tree * myPtr, FILE * myOutputFilePtr);
void check_for_type_definition(tree * myPtr, FILE * myOutputFilePtr, FILE * myDescrFilePtr);
void check_range_limits(tree * rangelist, int flag);
void convert_int_to_string(int n, char s[]);
void convert_ul_to_string(unsigned long  n, char s[]);
void do_imports(tree * local_importFromList, char * fileDoingImport);
void dump_table(symbol_table * table);
void dump_table1(tree * table[]);
void fill_in_max_min_values(tree * rangelist, int flag);
void free_all_nodes(tree * my_ParseNodeList);
void insert_mib_replacement_node(char * oldName, char * newName);
void insert_name(tree * node, tree * symboltable[]);
void insert_name_into_emitted_nodes(char * name, int type);
void insert_type_node(tree * node, tree * symboltable[] );
void load_mib_replacement_table();
void mosy_code_emit(tree * ptr, FILE * outPtr, FILE * descrFilePtr);
void mosy_emit_all_nodes(tree * ptr, FILE * outPtr, FILE * descrFilePtr);
void mosy_emit_node(tree * ptr, FILE * outPtr, FILE * descrFilePtr);
void mosy_print_enumerated_range(tree * ptr, tree * tmpPtr, FILE * outPtr);
void open_output_file(char * desiredOutputName);
void print_string_warning();
void print_warning(char * my_msg, char * buf);
void write_header(FILE * outPtr);
void yyerror (char * errmsg);

/* parse file returns 0 on success, 1 on failure */
int hashpjw(char *, int);
int parse_file(int importFlag, char * localInputFileName, char * partialName);
int print_value_list(tree * firstNodeInList, FILE * outPtr);

symbol_table * malloc_table(symbol_table * new_table);

tree * clone_node(tree * origNode);
tree * find_type(char * lookupName);
tree * lookup_name_in_emitted_nodes(char * name, int type);
tree * lookup_type(char * string, tree * symboltable[] );
tree * malloc_node(tree * node, int addToFreeList);
tree * lookup_name(char * variable_name, tree * symboltable[]);
tree * shallow_clone_node(tree * origNode);

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== PARSER ============================== */
/*  %right favors shifting 
    %left favors reduction 
    precedence is from lowest to highest ( with lowest first ) 
 */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

%}

%union {
    int val;
    char * char_ptr;
    struct tree_tag * tree_ptr;
}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================== TOKENS ========================== */
/*  %right favors shifting                                      */ 
/*  %left favors reduction                                      */ 
/*  precedence is from lowest to highest ( with lowest first )  */ 
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* make tokens look like keywords to enhance error reporting */

%right  ACCESS accessible_for_notify AGENT_CAPABILITIES ANY APPLICATION CCE AUGMENTS BAR
%right  BEGIN_ BINARY_NUMBER BIT BITS BY CHOICE COMMA CONTACT_INFO Counter Counter32 Counter64
%right  CREATION_REQUIRES currrent DEFINED DEFINITIONS DEFVAL deprecated DESCRIPTION DISPLAY_HINT
%right  DOT DOT_DOT END ENTERPRISE EXPLICIT EXPORTS FROM Gauge Gauge32 GROUP HEX_NUMBER IDENTIFIER
%right  IMPLICIT IMPLIED IMPORTS INCLUDES INDEX INTEGER Integer32 IpAddress LAST_UPDATED LBRACE
%right  LBRACKET LOWERCASE_IDENTIFIER LPAREN mandatory MANDATORY_GROUPS MAX_ACCESS MAX MIN
%right  MIN_ACCESS MODULE MODULE_COMPLIANCE MODULE_CONFORMANCE MODULE_IDENTITY NEG_NUMBER
%right  not_accessible NOTIFICATION_GROUP NOTIFICATIONS NOTIFICATION_TYPE not_implemented NULL_
%right  OBJECT OBJECT_GROUP OBJECT_IDENTITY ObjectSyntax OBJECTS OBJECT_TYPE obsolete OCTET
%right  OF OID Opaque optional ORGANIZATION POS_NUMBER PRIVATE PRODUCT_RELEASE QUOTED_STRING RBRACE
%right  RBRACKET read_create read_only read_write REFERENCE REVISION RPAREN SEMICOLON SEQUENCE
%right  SIZE STATUS STRING STRUCT SUPPORTS SYNTAX TEXTUAL_CONVENTION TimeTicks TRAP_TYPE UInteger32 UNION
%right  UNITS UNIVERSAL Unsigned32 UPPERCASE_IDENTIFIER VARIABLES VARIATION write_only
%right  WRITE_SYNTAX

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== PRODUCTION NAMES  ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

%type <tree_ptr>    AccessibleForNotify AgentCapabilitiesDefinition AlternativeTypeList GarbageType ApplicationSyntax
%type <tree_ptr>    assignedIdentifier Assignment AssignmentList BitsKeyword BitStringKeyword class Compliance
%type <tree_ptr>    ComplianceAccess complianceAccessPart ComplianceGroup ComplianceList ComplianceObject
%type <tree_ptr>    compliancePart complianceSyntaxPart CounterKeyword Counter32Keyword Counter64Keyword creation creationPart
%type <tree_ptr>    CurrentKeyword DefaultValue defaultValuePart DefinedValue DeprecatedKeyword descriptionPart
%type <tree_ptr>    DescriptionPart displayPart EndPoint empty exports ExtUTCTime GaugeKeyword Gauge32Keyword ImportKeyword importList
%type <tree_ptr>    imports Index indexPart IndexType IndexTypeList Integer32Keyword IntegerKeyword IpAddressKeyword
%type <tree_ptr>    LowercaseIdentifier MandatoryKeyword mandatoryPart MIB mibFile moduleName MIBList Module
%type <tree_ptr>    ModuleBody ModuleComplianceDefinition ModuleConformanceDefinition ModuleDefinition
%type <tree_ptr>    ModuleDefinitionList ModuleIdentityDefinition ModuleList moduleOID modulePart NamedBit NamedBitList
%type <tree_ptr>    NamedBitListPlugin NamedType NotAccessible NotificationGroupDefinition NotificationTypeDefinition
%type <tree_ptr>    NotImplemented Number NumericValue ObjectAccessPart ObjectAccessPart ObjectGroupDefinition
%type <tree_ptr>    ObjectIdentifier ObjectIdentifierKeyword ObjectIdentifierList ObjectIdentifierList
%type <tree_ptr>    ObjectIdentifierValue ObjectIdentityDefinition ObjectSyntaxKeyword objectPart ObjectTypeDefinition ObsoleteKeyword
%type <tree_ptr>    OctetStringKeyword Oid OpaqueKeyword OptionalKeyword QuotedString RangeList RangeListPlugin
%type <tree_ptr>    RangeRestriction ReadCreate ReadOnly ReadWrite referencePart referencePart Revision RevisionList
%type <tree_ptr>    revisionPart SequenceItem SequenceItemsList SequenceKeyword SequenceSyntax SimpleSyntax SizeKeyword SizeRestriction
%type <tree_ptr>    SMIv1Access SMIv2Access SMIv3Syntax StartBogusTC StartModuleDefinition StartModuleIdentity StartStruct
%type <tree_ptr>    StartUnion Status StatusPart Struct StructItem StructItemList Symbol SymbolFromModule SymbolList
%type <tree_ptr>    symbolsExported SymbolsFromModuleList symbolsImported Syntax SyntaxPart TextualConventionDefinition
%type <tree_ptr>    TimeTicksKeyword TrapTypeDefinition Type TypeAssignment TypeGarbage TypeTag Union unitsPart unitsPart
%type <tree_ptr>    Unsigned32Keyword UppercaseIdentifier ValueAssignment ValueRange variablePart Variation
%type <tree_ptr>    VariationAccess variationAccessPart VariationList variationPart VarType VarTypeList VarTypes
%type <tree_ptr>    VirtualTableConstructor WriteOnly writeSyntaxPart

%start  mibFile

%%

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== OVERALL ORGANIZATION ================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/*
 * One mibFile may contain multiple MIB modules.
 * It's also possible that there's no module in a file.
 */
mibFile
    :   ModuleDefinitionList { root = $1; }
    |   empty
;

ModuleDefinitionList
    :   ModuleDefinition {
            $1->last_tree_node_in_list = $1;
    }
    |    ModuleDefinitionList ModuleDefinition {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

ModuleDefinition
    :   StartModuleDefinition
        CCE
        BEGIN_ ModuleBody END {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = DEFINITIONS;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_misc1___14] = $4;
            $$ = tree_node;
    }
    ;

StartModuleDefinition
    :   UppercaseIdentifier 
        assignedIdentifier
        DEFINITIONS {

            /* reset flags for each module/mib */
            SMIv1Flag   = 0;
            SMIv2Flag   = 0;
            SMIv3Flag   = 0;
            sawObjectIdentifier = 0;

            /* set module name */
            lastModuleName = $1->string;
    }
    |   LowercaseIdentifier 
        assignedIdentifier
        DEFINITIONS {

            /* reset flags for each module/mib */
            SMIv1Flag   = 0;
            SMIv2Flag   = 0;
            SMIv3Flag   = 0;

            /* set module name */
            lastModuleName = $1->string;
            print_warning("*** WARNING ***  MIB names should begin with an uppercase letter.", lineBuf);
    }
    ;

assignedIdentifier
    :   ObjectIdentifierValue
    |   empty
    ;

imports
    :   IMPORTS symbolsImported SEMICOLON {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $2;
            $$ = tree_node; 
    }
    |   empty
    ;

symbolsImported
    :   SymbolsFromModuleList {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $1;
            $$ = tree_node;
    }
    |   empty
    ;

SymbolsFromModuleList
    :   SymbolFromModule {
            $1->last_tree_node_in_list = $1;
    }
    |   SymbolsFromModuleList SymbolFromModule {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

/* exports are not used, just parsed and tossed */
ModuleBody
    :   exports imports AssignmentList {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_misc1___14] = $3;
            $$ = tree_node; 
    }
    |   exports imports {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $2;
            $$ = tree_node; 
    }
    ;

/* EXPORTS are not used, just parsed and tossed */
exports
    :   EXPORTS symbolsExported SEMICOLON {
            $$ = $2;
    }
    |   empty
    ;

symbolsExported
    :   SymbolList 
    |   SymbolList COMMA {
            print_warning("*** WARNING ***  There is an extra COMMA at the end of the Symbol List.", lineBuf);
    } 
    |   empty
    ;

AssignmentList
    :   Assignment {
            $1->last_tree_node_in_list = $1;
    }
    |   AssignmentList Assignment {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

importList
    :   SymbolList
    |   SymbolList COMMA {
            print_warning("*** WARNING ***  There is an extra COMMA at the end of the Symbol List.", lineBuf);

    }
    |   empty
    ;

SymbolList
    :   Symbol {
            $1->last_tree_node_in_list = $1;
    }
    |   SymbolList COMMA Symbol {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   SymbolList Symbol {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** WARNING ***  There is a missing COMMA in the Symbol List.", lineBuf);
    }
    ;

SymbolFromModule
    :   importList FROM UppercaseIdentifier {
            tree * importFromNode = NULL;
            tree * importItem     = NULL;
            tree * treePtr        = NULL;
            tree * tree_node      = NULL;

            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_misc1___14] = $3;
            $$ = tree_node; 

            /* create and insert FROM node at beginning of global import list */
            importFromNode = malloc_node(importFromNode, 1);
            importFromNode->string = strdup($3->string);
            importFromNode->freeMe = 1;

            treePtr = $1;
            while(treePtr) {

                /* -1 is ignore keyword type */
                if((treePtr->string) && (treePtr->type != -1)){

                    /* import FROM nodes are linked using the next_tree_node              */
                    /* items are hung from the FROM nodes using the next_symbol_node link */

                    /* create and insert item into FROM node's list of importItems */
                    importItem = malloc_node(importItem, 1);
                    importItem->string = strdup(treePtr->string); 
                    importItem->freeMe = 1;

                    importItem->next_symbol_node = importFromNode->next_symbol_node;
                    importFromNode->next_symbol_node = importItem;

                } 

                treePtr = treePtr->next_tree_node;
            }

            /* don't import the file unless there is something to import */
            if(importFromNode->next_symbol_node) {

                /* insert node at end of list */
                if(importFromList == NULL) {
                    importFromList = importFromNode;
                    importFromList->last_tree_node_in_list = importFromNode;
                } else {
                    importFromList->last_tree_node_in_list->next_tree_node = importFromNode;
                    importFromList->last_tree_node_in_list = importFromNode;
                }

            }

    }
    |   importList FROM LowercaseIdentifier {
            tree * importFromNode = NULL;
            tree * importItem     = NULL;
            tree * treePtr        = NULL;
            tree * tree_node      = NULL;

            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_misc1___14] = $3;
            $$ = tree_node; 

            /* create and insert FROM node at beginning of global import list */
            importFromNode = malloc_node(importFromNode, 1);
            importFromNode->string = strdup($3->string);
            importFromNode->freeMe = 1;

            print_warning("*** WARNING ***  The names of imported MIBs should be UppercaseIdentifiers.", lineBuf);

            treePtr = $1;
            while(treePtr) {

                /* -1 is ignore keyword type */
                if((treePtr->string) && (treePtr->type != -1)){

                    /* import FROM nodes are linked using the next_tree_node              */
                    /* items are hung from the FROM nodes using the next_symbol_node link */

                    /* create and insert item into FROM node's list of importItems */
                    importItem = malloc_node(importItem, 1);
                    importItem->string = strdup(treePtr->string); 
                    importItem->freeMe = 1;

                    importItem->next_symbol_node = importFromNode->next_symbol_node;
                    importFromNode->next_symbol_node = importItem;

                } 

                treePtr = treePtr->next_tree_node;
            }

            /* don't import the file unless there is something to import */
            if(importFromNode->next_symbol_node) {

                /* insert node at end of list */
                if(importFromList == NULL) {
                    importFromList = importFromNode;
                    importFromList->last_tree_node_in_list = importFromNode;
                } else {
                    importFromList->last_tree_node_in_list->next_tree_node = importFromNode;
                    importFromList->last_tree_node_in_list = importFromNode;
                }

            }
    }
    ;

Symbol
    :   UppercaseIdentifier
    |   LowercaseIdentifier
    |   ImportKeyword
    ;

/* -1 for type we want to ignore */
ImportKeyword
    :   ApplicationSyntax {
            $1->type = -1;
    }
    |   GaugeKeyword {
            $1->type = -1;
    }
    |   CounterKeyword {
            $1->type = -1;
    }
    |   IntegerKeyword {
            $1->type = -1;
    }
    |   BitsKeyword {
            $1->type = -1;
    }
    |   AGENT_CAPABILITIES { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   NOTIFICATION_GROUP {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   NOTIFICATION_TYPE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   TRAP_TYPE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   MANDATORY_GROUPS {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   MODULE_COMPLIANCE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   MODULE_CONFORMANCE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node; 
    }
    |   MODULE_IDENTITY {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   OBJECT_GROUP {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   OBJECT_IDENTITY {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   OBJECT_TYPE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   TEXTUAL_CONVENTION {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    ;

TypeAssignment
    :   UppercaseIdentifier CCE Type {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = TEXTUAL_CONVENTION;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_syntax___1] = $3;

            /* if saving for all parses, you must clone, otherwise
               the children would go bye-bye at the end of the parse */

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;
    }
    |   LowercaseIdentifier CCE Type {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = TEXTUAL_CONVENTION;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_syntax___1] = $3;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;

            print_warning("*** WARNING ***  You should not use LowercaseIdentifiers in type assignments.", lineBuf);

    }
        /* garbage collector */
    |   ImportKeyword CCE Type {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;

            print_warning("*** WARNING ***  You should not attempt to redefine basic types.", lineBuf);

    }
    ;    

Assignment
    :   AgentCapabilitiesDefinition
    |   ModuleComplianceDefinition
    |   ModuleConformanceDefinition
    |   ModuleIdentityDefinition
    |   NotificationGroupDefinition
    |   NotificationTypeDefinition
    |   ObjectGroupDefinition
    |   ObjectIdentityDefinition
    |   ObjectTypeDefinition
    |   TextualConventionDefinition
    |   TrapTypeDefinition
    |   TypeAssignment
    |   ValueAssignment

        /* SMIv3 extensions */
    |   Struct
    |   Union
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============== GRAY AREA - GRAMMAR ILL-DEFINED ============= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ValueAssignment
    :   LowercaseIdentifier 
        OBJECT IDENTIFIER 
        CCE ObjectIdentifierValue  {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = IDENTIFIER;
            tree_node->string                  = "object-id";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_value____9] = $5;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;

            sawObjectIdentifier = 1;

    }
    |   UppercaseIdentifier 
        OBJECT IDENTIFIER 
        CCE ObjectIdentifierValue  {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = IDENTIFIER;
            tree_node->string                  = "object-id";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_value____9] = $5;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;

            sawObjectIdentifier = 1;

            print_warning("*** WARNING ***  Object identifiers should begin with a lowercase letter.", lineBuf);

    }
    ;

ObjectIdentifierValue
    :   LowercaseIdentifier
    |   UppercaseIdentifier {
        print_warning("*** WARNING ***  Object identifiers should begin with a lowercase letter.", lineBuf);
    }
    |   LBRACE ObjectIdentifierList RBRACE {
            $$ = $2;
    }
    ;

ObjectIdentifierList
    :   ObjectIdentifier {
            $1->last_tree_node_in_list = $1;
    }
    |   ObjectIdentifierList ObjectIdentifier {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

Index
    :   LowercaseIdentifier
    |   UppercaseIdentifier
    |   ApplicationSyntax
    |   IntegerKeyword
    |   OctetStringKeyword
    |   ObjectIdentifierKeyword
    ;

/* may also be a textual convention (or its refinement)*/
Syntax
    :   Type
    ;

Type
    :   ApplicationSyntax

    |   Integer32Keyword  RangeRestriction { 
            $1->child[child_range____6] = $2; 
            fill_in_max_min_values($2, INTEGER);
            check_range_limits($2, INTEGER);
            check_for_range_overlap($2);
    }
    |   Gauge32Keyword    RangeRestriction { 
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }
    |   Unsigned32Keyword RangeRestriction { 
            $1->child[child_range____6] = $2; 
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }
    |   OpaqueKeyword     SizeRestriction  { $1->child[child_range____6] = $2; }
    |   SimpleSyntax
    |   VirtualTableConstructor
    |   SequenceKeyword LBRACE SequenceItemsList RBRACE {
            $1->child[child_identity_0] = $3;
    }

    |   SMIv3Syntax

    /* *** GARBAGE COLLECTORS *** */

    |   TimeTicksKeyword  RangeRestriction { 
            print_warning("*** WARNING ***  Per rfc2578, TimeTicks may not have range restriction, ignoring range restriction.", lineBuf);
    }
    |   Counter64Keyword  RangeRestriction { 
            print_warning("*** WARNING ***  Per rfc2578, Counter64 may not have range restriction, ignoring range restriction.", lineBuf);
    }
    |   Counter32Keyword  RangeRestriction { 
            print_warning("*** WARNING ***  Per rfc2578, Counter32 may not have range restriction, ignoring range restriction.", lineBuf);
    }
    |   SequenceKeyword LBRACE SequenceItemsList COMMA RBRACE {
            $1->child[child_identity_0] = $3;
            print_warning("*** WARNING ***  There is an extra COMMA at the end of the Sequence Item List.", lineBuf);
    }
    ;

DefinedValue
    :   UppercaseIdentifier
    |   LowercaseIdentifier
    |   UppercaseIdentifier DOT UppercaseIdentifier
    ;

ApplicationSyntax
    :   Integer32Keyword
    |   Gauge32Keyword
    |   Unsigned32Keyword
    |   TimeTicksKeyword

    |   ObjectSyntaxKeyword

    |   IpAddressKeyword
    |   Counter32Keyword
    |   OpaqueKeyword
    |   Counter64Keyword
    ;

SimpleSyntax
    :   ObjectIdentifierKeyword
    |   ObjectIdentifierKeyword SizeRestriction {
            $1->child[child_range____6] = $2;
    }

    |   IntegerKeyword
    |   IntegerKeyword RangeRestriction {
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, INTEGER);
            check_range_limits($2, INTEGER);
            check_for_range_overlap($2);
    }
    |   IntegerKeyword LBRACE  NamedBitListPlugin RBRACE  {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

    |   OctetStringKeyword {
            print_warning("*** WARNING ***  OCTET STRING on previous line should have SIZE specification.", lineBuf);
    }
    |   OctetStringKeyword SizeRestriction  { $1->child[child_range____6] = $2; }
    |   OctetStringKeyword LBRACE NamedBitListPlugin RBRACE {
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }        

    |   GaugeKeyword
    |   GaugeKeyword RangeRestriction {
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }

    |   CounterKeyword


    |   BitStringKeyword
    |   BitStringKeyword LBRACE NamedBitListPlugin RBRACE  {
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

    |   BitsKeyword LBRACE NamedBitListPlugin RBRACE  { 
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

    |   UppercaseIdentifier             /* e.g. the uppercase name of a row or an imported type */
    |   LowercaseIdentifier {
            print_warning("*** WARNING ***  UppercaseIdentifiers should be used for types, row names, or imported types, NOT LowercaseIdentifiers.", lineBuf);
    }
    |   UppercaseIdentifier LBRACE NamedBitListPlugin RBRACE  {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

    |   UppercaseIdentifier RangeRestriction { $1->child[child_range____6] = $2; }
    |   UppercaseIdentifier SizeRestriction  { $1->child[child_range____6] = $2; }
    |   UppercaseIdentifier DOT UppercaseIdentifier LBRACE NamedBitListPlugin RBRACE  {

            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->enumeratedItems         = $5->enumeratedItems;
            $5->enumeratedItems                = NULL;
            tree_node->type                    = BITS;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_misc1___14] = $3;
            tree_node->child[child_status___2] = $5;
            $$ = tree_node; 
    }
    |   UppercaseIdentifier DOT UppercaseIdentifier RangeRestriction {

            $1->child[child_identity_0] = $3;
            $1->child[child_range____6] = $4;
            $1->type = DOT;
    }
    |    UppercaseIdentifier DOT UppercaseIdentifier SizeRestriction {

            $1->child[child_identity_0] = $3;
            $1->child[child_range____6] = $4;
            $1->type = DOT;
    }

    |    NULL_ { /* rfc1065, rfc1155 */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node; 
    }

    /* *** GARBAGE COLLECTORS *** */

    |   IntegerKeyword SizeRestriction { 
            /* this isn't really legal, should be range restriction */
            print_warning("*** WARNING ***  This should be a range restriction, NOT a size restriction.", lineBuf);
            $1->child[child_range____6] = $2;
    }
    |   OctetStringKeyword RangeRestriction { 
            $1->child[child_range____6] = $2; 
            print_warning("*** WARNING ***  OCTET STRING is missing SIZE specification.", lineBuf);
    }
    |   TypeGarbage
    ;

/* FUTURE CHECK **** must be valid for the type specified in SYNTAX clause of same OBJECT-TYPE macro */
DefaultValue
    :   Number
    |   LowercaseIdentifier
    |   UppercaseIdentifier
    |   QuotedString
    |   Oid
    |   LBRACE RBRACE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "";
            $$ = tree_node;
    }

    |   LBRACE Number              RBRACE { $$ = $2; }
    |   LBRACE NamedBit            RBRACE { $$ = $2; }
    |   LBRACE LowercaseIdentifier RBRACE { $$ = $2; }
    |   LBRACE Oid                 RBRACE { $$ = $2; }

    |   LBRACE LowercaseIdentifier COMMA VarTypeList RBRACE {
            $2->last_tree_node_in_list = $4->last_tree_node_in_list;
            $2->next_tree_node = $4;
            $$ = $2;
    }

    |   LBRACE ObjectIdentifier ObjectIdentifierList RBRACE {
            $2->last_tree_node_in_list = $3->last_tree_node_in_list;
            $2->next_tree_node = $3;
            $$ = $2;
    }
    ;

SequenceItemsList
    :   SequenceItem {
            $1->last_tree_node_in_list = $1; 
    }
    |   SequenceItemsList COMMA SequenceItem {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   SequenceItemsList SequenceItem {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** WARNING ***  Missing COMMA in Sequence Item List.", lineBuf);
    }
    ;

SequenceItem
    :   LowercaseIdentifier SequenceSyntax

    |   LowercaseIdentifier TypeTag SequenceSyntax /* garbage collector */

    |   LowercaseIdentifier VirtualTableConstructor {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_warning("*** NOTE ***  VirtualTableConstructors inside a sequence (SEQUENCE OF) are not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }
    }
    |   LowercaseIdentifier SMIv3Syntax {
            $2->child[child_identity_0] = $1;
            $$ = $2;
    }

    |   UppercaseIdentifier SequenceSyntax {
            print_warning("*** WARNING ***  Object identifiers should begin with a lowercase letter.", lineBuf);
    }
    |   UppercaseIdentifier TypeTag SequenceSyntax {
            print_warning("*** WARNING ***  Object identifiers should begin with a lowercase letter.", lineBuf);
    }

    |   UppercaseIdentifier VirtualTableConstructor {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_warning("*** NOTE ***  VirtualTableConstructors inside a sequence (SEQUENCE OF) are not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }
            print_warning("*** WARNING ***  Object identifiers should begin with a lowercase letter.", lineBuf);

    }
    |   UppercaseIdentifier SMIv3Syntax {
            $2->child[child_identity_0] = $1;
            $$ = $2;
            print_warning("*** WARNING ***  Object identifiers should begin with a lowercase letter.", lineBuf);
    }
    ;

/* SEQUENCE is used to generate lists or tables */
SequenceSyntax
    :   ObjectIdentifierKeyword
    |   ObjectIdentifierKeyword SizeRestriction {
            $1->child[child_range____6] = $2;
    }

    |   GaugeKeyword
    |   CounterKeyword

    |   BitStringKeyword
    |   BitStringKeyword LBRACE NamedBitListPlugin RBRACE  {
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }                

    |   IntegerKeyword
    |   IntegerKeyword RangeRestriction { 
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, INTEGER);
            check_range_limits($2, INTEGER);
            check_for_range_overlap($2);
    }      
    |   IntegerKeyword SizeRestriction  { 
            /* this isn't really legal, should be range restriction */
            $1->child[child_range____6] = $2;
            print_warning("*** WARNING ***  This should be a range restriction, NOT a size restriction.", lineBuf);
    }
    |   IntegerKeyword LBRACE NamedBitListPlugin RBRACE  {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }                

    |   OctetStringKeyword
    |   OctetStringKeyword SizeRestriction {
            $1->child[child_range____6] = $2;
    }        
    |   OctetStringKeyword RangeRestriction {
            $1->child[child_range____6] = $2;
            print_warning("*** WARNING ***  OCTET STRING is missing SIZE specification.", lineBuf);
    }        
    |   OctetStringKeyword LBRACE NamedBitListPlugin RBRACE {
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }        

    |   UppercaseIdentifier
    |   LowercaseIdentifier {
            print_warning("*** WARNING ***  LowercaseIdentifiers should not be used for SequenceSyntax.", lineBuf);
    }
    |   UppercaseIdentifier RangeRestriction {
            $1->child[child_range____6] = $2;
    } 
    |   UppercaseIdentifier SizeRestriction {
            $1->child[child_range____6] = $2;
            $1->type = SIZE;
    } 
    |   UppercaseIdentifier LBRACE NamedBitListPlugin RBRACE {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    } 

    |   BitsKeyword

    |   ApplicationSyntax
    |   Integer32Keyword  RangeRestriction    { 
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, INTEGER);
            check_range_limits($2, INTEGER);
            check_for_range_overlap($2);
    }
    |   Gauge32Keyword    RangeRestriction    { 
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }
    |   Unsigned32Keyword RangeRestriction    { 
            $1->child[child_range____6] = $2; 
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }

    |   Integer32Keyword LBRACE NamedBitListPlugin RBRACE  {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }           
    |   Gauge32Keyword LBRACE  NamedBitListPlugin RBRACE  {                /* 0 - 4294967295         */
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }
    |   Unsigned32Keyword LBRACE NamedBitListPlugin RBRACE  {          /* 0 - 4294967295         */
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

        /* garbage collector */
    |   GarbageType

    ;

ObjectIdentifier
    :   Number
    |   LowercaseIdentifier
    |   UppercaseIdentifier {
            print_warning("*** WARNING ***  Object identifiers should begin with a lowercase letter.", lineBuf);
    }
    |   NamedBit
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================= TRAP TYPE MACRO ==================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

TrapTypeDefinition
    :   LowercaseIdentifier TRAP_TYPE ENTERPRISE ObjectIdentifierValue 
        variablePart 
        descriptionPart
        referencePart 
        CCE Number {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = TRAP_TYPE;
            tree_node->child[child_identity_0] = $1;

            tree_node->child[child_object___5] = $9;
            tree_node->child[child_value____9] = $4;

            tree_node->child[child_descript_3] = $6;
            tree_node->child[child_misc1___14] = $5;

            $6->parent = tree_node;

            $$ = tree_node;

            if(!$6->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** WARNING *** \"DESCRIPTION\" is required in SMIv2 and SMIv3.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_warning("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }
 
    }
    |   UppercaseIdentifier TRAP_TYPE ENTERPRISE ObjectIdentifierValue 
        variablePart 
        descriptionPart
        referencePart 
        CCE Number {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = TRAP_TYPE;
            tree_node->child[child_identity_0] = $1;

            tree_node->child[child_object___5] = $9;
            tree_node->child[child_value____9] = $4;

            tree_node->child[child_descript_3] = $6;
            tree_node->child[child_misc1___14] = $5;

            $6->parent = tree_node;

            $$ = tree_node;
 
            print_warning("*** WARNING ***  You should use a LowercaseIdentifier to name a trap type.", lineBuf);

            if(!$6->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** WARNING *** \"DESCRIPTION\" is required in SMIv2 and SMIv3.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_warning("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    ;

variablePart
    :   VARIABLES LBRACE VarTypeList RBRACE { 
            $$ = $3;
    }
    |   empty
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================= MODULE CONFORMANCE MACRO ================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ModuleConformanceDefinition
    :   LowercaseIdentifier MODULE_CONFORMANCE
        LAST_UPDATED        ExtUTCTime
        PRODUCT_RELEASE     QuotedString
        DescriptionPart
        modulePart
        CCE  ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_CONFORMANCE;
            tree_node->string                  = "module-conformance";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_descript_3] = $7;
            tree_node->child[child_value____9] = $10;

            tree_node->child[child_misc1___14] = $8;

            $7->parent = tree_node;

            $$ = tree_node; 
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== MODULE COMPLIANCE MACRO ================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ModuleComplianceDefinition
    :   LowercaseIdentifier MODULE_COMPLIANCE
        StatusPart
        DescriptionPart
        referencePart 
        MIBList 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_COMPLIANCE;
            tree_node->string                  = "module-compliance";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $3;
            tree_node->child[child_descript_3] = $4;
            tree_node->child[child_value____9] = $8;

            tree_node->child[child_misc1___14] = $6;

            $4->parent = tree_node;

            $$ = tree_node; 
    }
    |   UppercaseIdentifier MODULE_COMPLIANCE
        StatusPart
        DescriptionPart
        referencePart 
        MIBList 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_COMPLIANCE;
            tree_node->string                  = "module-compliance";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $3;
            tree_node->child[child_descript_3] = $4;
            tree_node->child[child_value____9] = $8;

            tree_node->child[child_misc1___14] = $6;

            $4->parent = tree_node;

            $$ = tree_node;

            print_warning("*** WARNING ***  \"MODULE-COMPLIANCE\" names should begin with an uppercase letter.", lineBuf);
 
    }
    ;

MIBList
    :   MIB { 
            $1->last_tree_node_in_list = $1; 
    }
    |   MIBList MIB {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

MIB
    :   MODULE moduleName
        mandatoryPart 
        compliancePart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_misc1___14] = $3;
            tree_node->child[child_misc2___15] = $4;

            $$ = tree_node; 
    }
    ;

mandatoryPart
    :   MANDATORY_GROUPS LBRACE VarTypeList RBRACE {
            $$ = $3;
    }
    |   empty
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== AGENT CAPABILITIES MACRO ================ */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

AgentCapabilitiesDefinition
    :   LowercaseIdentifier AGENT_CAPABILITIES 
        PRODUCT_RELEASE QuotedString 
        StatusPart
        DescriptionPart
        referencePart 
        modulePart 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = AGENT_CAPABILITIES;
            tree_node->string                  = "agent-capabilities";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $5;
            tree_node->child[child_descript_3] = $6;
            tree_node->child[child_misc1___14] = $8;
            tree_node->child[child_value____9] = $10;

            $6->parent = tree_node;

            $$ = tree_node; 

    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== NOTIFICATION GROUP MACRO ================ */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

NotificationGroupDefinition
    :   LowercaseIdentifier NOTIFICATION_GROUP 
        NOTIFICATIONS LBRACE VarTypeList RBRACE
        StatusPart
        DescriptionPart
        referencePart 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = NOTIFICATION_GROUP;
            tree_node->string                  = "notification-group";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $7;
            tree_node->child[child_descript_3] = $8;
            tree_node->child[child_value____9] = $11;

            tree_node->child[child_misc1___14] = $5;

            $8->parent = tree_node;

            $$ = tree_node; 
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== OBJECT GROUP MACRO =================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ObjectGroupDefinition
    :   LowercaseIdentifier OBJECT_GROUP 
        OBJECTS LBRACE VarTypeList RBRACE         
        StatusPart
        DescriptionPart
        referencePart 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = OBJECT_GROUP;
            tree_node->string                  = "object-group";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $7;
            tree_node->child[child_descript_3] = $8;
            tree_node->child[child_value____9] = $11;

            tree_node->child[child_misc1___14] = $5;

            $8->parent = tree_node;

            $$ = tree_node; 
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== NOTIFICATION TYPE MACRO ================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

NotificationTypeDefinition
    :   LowercaseIdentifier NOTIFICATION_TYPE 
        objectPart
        StatusPart
        DescriptionPart
        referencePart 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = NOTIFICATION_TYPE;
            tree_node->string                  = "notification";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_object___5] = $3;
            tree_node->child[child_status___2] = $4;
            tree_node->child[child_descript_3] = $5;
            tree_node->child[child_value____9] = $8;

            $5->parent = tree_node;

            $$ = tree_node; 
    }
    ;

objectPart
    :   OBJECTS LBRACE VarTypeList RBRACE {
            $$ = $3;
    }
    |   empty
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== OBJECT TYPE MACRO =================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ObjectTypeDefinition
    :   LowercaseIdentifier OBJECT_TYPE 
        SyntaxPart
        unitsPart 
        ObjectAccessPart
        StatusPart
        descriptionPart
        referencePart 
        indexPart 
        defaultValuePart
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = OBJECT_TYPE;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $6;
            tree_node->child[child_descript_3] = $7;
            tree_node->child[child_index____7] = $9;
            tree_node->child[child_value____9] = $12;
            tree_node->child[child_default_10] = $10;
            tree_node->child[child_access__11] = $5;
            tree_node->child[child_syntax___1] = $3;

            $7->parent  = tree_node;
            $10->parent = tree_node;

            $$ = tree_node; 

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            if($10->child[child_identity_0]) {
                if( ($3->type == Counter32) || ($3->type == Counter64) ) {
                    print_warning("*** WARNING ***  DEFVAL is not allowed with Counter32 or Counter64, ignoring.", lineBuf);
                    tree_node->child[child_default_10] = NULL;
                }
            }

            if(!$7->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** WARNING *** \"DESCRIPTION\" is required in SMIv2 and SMIv3.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_warning("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    |   UppercaseIdentifier OBJECT_TYPE 
        SyntaxPart
        unitsPart 
        ObjectAccessPart
        StatusPart
        descriptionPart
        referencePart 
        indexPart 
        defaultValuePart
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = OBJECT_TYPE;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $6;
            tree_node->child[child_descript_3] = $7;
            tree_node->child[child_index____7] = $9;
            tree_node->child[child_value____9] = $12;
            tree_node->child[child_default_10] = $10;
            tree_node->child[child_access__11] = $5;
            tree_node->child[child_syntax___1] = $3;

            $7->parent  = tree_node;
            $10->parent = tree_node;

            $$ = tree_node; 

            /* *** SAVE TYPE *** */

            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            /* *** SEMANTIC CHECKS *** */

            print_warning("*** WARNING ***  Object types should begin with a lowercase letter.", lineBuf);

            if($10->child[child_identity_0]) {
                if( ($3->type == Counter32) || ($3->type == Counter64) ) {
                    print_warning("*** WARNING ***  DEFVAL is not allowed with Counter32 or Counter64, ignoring.", lineBuf);
                    tree_node->child[child_default_10] = NULL;
                }
            }

            if(!$7->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** WARNING *** \"DESCRIPTION\" is required in SMIv2 and SMIv3.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_warning("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    ;

ObjectAccessPart
    :   MAX_ACCESS  SMIv2Access {
            $$ = $2;

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"MAX-ACCESS\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"MAX-ACCESS\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    } 
    |   ACCESS SMIv1Access{
            $$ = $2;

            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) {
                print_warning("*** WARNING *** \"ACCESS\" in ObjectAccessPart is only used in SMIv1.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_warning("*** NOTE *** \"ACCESS\" in ObjectAccessPart is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    ;

SMIv1Access
    :   NotAccessible
    |   ReadOnly
    |   ReadWrite
    |   WriteOnly

    |   AccessibleForNotify {
             print_warning("*** WARNING *** \"accessible-for-notify\" should only be used in an SMIv2 MIB.", lineBuf);
    }
    |   ReadCreate {
             print_warning("*** WARNING *** \"read-create\" should only be used in an SMIv2 MIB.", lineBuf);
    }
    ;

unitsPart
    :   UNITS QuotedString { 
            $$ = $2;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"UNITS\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"UNITS\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    |   empty {
    }
    ;

indexPart
    :   INDEX LBRACE IndexTypeList RBRACE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = INDEX;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;
    }
    |   INDEX LBRACE IndexTypeList COMMA RBRACE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = INDEX;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;

            print_warning("*** WARNING ***  There is an extra comma at the end of the index list.", lineBuf);
    }

    |   AUGMENTS LBRACE LowercaseIdentifier RBRACE {
            $3->type = AUGMENTS;
            $$ = $3;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"AUGMENTS\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"AUGMENTS\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    |   empty
    ;

IndexTypeList
    :   IndexType {
            $1->last_tree_node_in_list = $1; 
    }
    |   IndexTypeList COMMA IndexType {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   IndexTypeList IndexType {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** WARNING ***  There is a missing COMMA in the Index List.", lineBuf);
    }
    ;

IndexType
    :   IMPLIED Index { 
            $2->type = IMPLIED;
            $$ = $2;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"IMPLIED\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"IMPLIED\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
 
    }
    |   Index
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================= SMIv3 EXTENSIONS =================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* struct may have only one item */
Struct
    :   StartStruct
        MAX_ACCESS  SMIv2Access
        StatusPart
        descriptionPart
        LBRACE        
        StructItemList
        RBRACE
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = STRUCT;
            tree_node->string                  = "Struct";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $4;
            tree_node->child[child_descript_3] = $5;
            tree_node->child[child_value____9] = $10;
            tree_node->child[child_access__11] = $3;

            tree_node->child[child_misc1___14] = $7;

            $5->parent = tree_node;

            $$ = tree_node;

            if(!$5->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** WARNING *** \"DESCRIPTION\" is required in SMIv2 and SMIv3.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_warning("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    ;

StartStruct
    :   LowercaseIdentifier STRUCT {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_warning("*** NOTE ***  STRUCT is not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }   
    }
    ;

StartUnion
    :   LowercaseIdentifier UNION {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_warning("*** NOTE ***  UNION is not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }
    }
    ; 

/* union must have at least two items */
Union
    :   StartUnion 
        MAX_ACCESS  SMIv2Access
        StatusPart
        descriptionPart
        LBRACE        
        StructItem
        StructItemList
        RBRACE
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = UNION;
            tree_node->string                  = "Union";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $4;
            tree_node->child[child_descript_3] = $5;
            tree_node->child[child_value____9] = $11;
            tree_node->child[child_access__11] = $3;

            tree_node->child[child_misc1___14] = $7;
            tree_node->child[child_misc2___15] = $8;

            $5->parent = tree_node;

            $$ = tree_node;

            if(!$5->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** WARNING *** \"DESCRIPTION\" is required in SMIv2 and SMIv3.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_warning("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    ;

StructItemList
    :   StructItem {
            $1->last_tree_node_in_list = $1;
    }
    |   StructItemList StructItem {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;
    
StructItem
    :   ObjectTypeDefinition
    |   Struct
    |   Union
    ;

SMIv3Syntax
    :   STRUCT {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   UNION {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================= TEXTUAL CONVENTION MACRO ================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

TextualConventionDefinition
    :   UppercaseIdentifier 
        CCE TEXTUAL_CONVENTION 
        displayPart 
        StatusPart
        DescriptionPart
        referencePart 
        SyntaxPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = TEXTUAL_CONVENTION;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_display__8] = $4;
            tree_node->child[child_status___2] = $5;
            tree_node->child[child_descript_3] = $6;
            tree_node->child[child_syntax___1] = $8;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node; 
    }
    |   StartBogusTC
        displayPart 
        StatusPart
        DescriptionPart
        referencePart 
        SyntaxPart {

            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;

            /*
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = TEXTUAL_CONVENTION;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_display__8] = $4;
            tree_node->child[child_status___2] = $5;
            tree_node->child[child_descript_3] = $6;
            tree_node->child[child_syntax___1] = $8;

            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;
            */ 
    }
    ;

StartBogusTC
    :   ImportKeyword CCE TEXTUAL_CONVENTION { 
            print_warning("*** WARNING ***  Ignoring re-definition of pre-defined type.", lineBuf);
    }
    ;

displayPart
    :   DISPLAY_HINT QuotedString { 
            $$ = $2; 
    }
    |   empty
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============== MODULE IDENTITY MACRO ======================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ModuleIdentityDefinition
    :   StartModuleIdentity 
        LAST_UPDATED ExtUTCTime
        ORGANIZATION QuotedString 
        CONTACT_INFO QuotedString 
        DescriptionPart
        revisionPart 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_IDENTITY;
            tree_node->string                  = "module-identity";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_descript_3] = $8;
            tree_node->child[child_referenc_4] = $9;
            tree_node->child[child_value____9] = $11;

            /* revision contains descriptions */
            tree_node->child[child_misc1___14] = $9;

            $8->parent = tree_node;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node; 

    }
    ;

StartModuleIdentity
    :   LowercaseIdentifier MODULE_IDENTITY {

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"MODULE-IDENTITY\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"MODULE-IDENTITY\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    |   UppercaseIdentifier MODULE_IDENTITY {

            print_warning("*** WARNING ***  \"MODULE-IDENTITY\" names should begin with an uppercase letter.", lineBuf);

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"MODULE-IDENTITY\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"MODULE-IDENTITY\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    ;

revisionPart
    :   RevisionList
    |   empty
    ;

RevisionList
    :   Revision { 
            $1->last_tree_node_in_list = $1; 
    }
    |   RevisionList Revision {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

Revision
    :   REVISION ExtUTCTime DescriptionPart {
            $$ = $3;
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============== OBJECT IDENTITY MACRO ======================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ObjectIdentityDefinition
    :   LowercaseIdentifier OBJECT_IDENTITY
        StatusPart
        DescriptionPart
        referencePart 
        CCE ObjectIdentifierValue  {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = OBJECT_IDENTITY;
            tree_node->string                  = "object-id";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $3;
            tree_node->child[child_descript_3] = $4;
            tree_node->child[child_value____9] = $7;

            $4->parent = tree_node;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;

    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================= COMMON =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ValueRange
    :   EndPoint {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = DOT_DOT;
            tree_node->child[chld_end_range13] = $1;
            $$ = tree_node;
    }
    |   EndPoint DOT_DOT EndPoint {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = DOT_DOT;
            tree_node->child[chld_strt_rnge12] = $1;
            tree_node->child[chld_end_range13] = $3;
            $$ = tree_node;
    }
    ;

EndPoint
    :   Number
    |   LowercaseIdentifier
    |   UppercaseIdentifier
    ;

empty
    :   { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    ;

VirtualTableConstructor
    :   SEQUENCE OF UppercaseIdentifier {
        /* uppercase name of row */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = OF;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;

            /* insert name into symbol table */
            insert_name($3, virtualTables);

    }
    |   SEQUENCE OF LowercaseIdentifier {
        /* uppercase name of row */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = OF;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;

            /* insert name into symbol table */
            insert_name($3, virtualTables);

            print_warning("*** WARNING *** \"SEQUENCE OF\" should be followed by an UppercaseIdentifier.", lineBuf);

    }
    ;

/* description is mandatory in SMIv2, but optional in SMIv1 */
descriptionPart
    :   DescriptionPart
    |   empty {
            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) { 
                print_warning("*** WARNING *** \"DESCRIPTION\" is required in SMIv2 and SMIv3.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_warning("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    ;

DescriptionPart
    :   DESCRIPTION QuotedString {
            $2->type = DESCRIPTION;
            $$ = $2;
    }  
    ; 

compliancePart
    :   ComplianceList
    |   empty
    ;

ComplianceList
    :   Compliance { 
            $1->last_tree_node_in_list = $1;
    }
    |   ComplianceList Compliance { 
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

Compliance
    :   ComplianceGroup
    |   ComplianceObject
    ;

ComplianceGroup
    :   GROUP LowercaseIdentifier DescriptionPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_descript_3] = $3;

            $$ = tree_node; 
    }
    ;

ComplianceObject
    :   OBJECT LowercaseIdentifier 
        complianceSyntaxPart 
        writeSyntaxPart
        complianceAccessPart
        DescriptionPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_descript_3] = $6;

            tree_node->child[child_misc1___14] = $4;

            tree_node->child[child_access__11] = $5;

            tree_node->child[child_misc2___15]  = $3;

            $$ = tree_node; 
    }
    ;

complianceAccessPart
    :   MIN_ACCESS ComplianceAccess { $$ = $2; }
    |   empty
    ;

ComplianceAccess
    :   NotAccessible
    |   AccessibleForNotify
    |   ReadOnly
    |   ReadWrite
    |   ReadCreate
    ;

creationPart
    :   CREATION_REQUIRES LBRACE creation RBRACE {
            $$ = $3;
    }
    |   empty 
    ;

/* can this part be empty ?? */
creation
    :   VarTypeList
    |   empty
    ;

Variation 
    :   VARIATION LowercaseIdentifier
        complianceSyntaxPart 
        writeSyntaxPart
        variationAccessPart
        creationPart 
        defaultValuePart 
        DescriptionPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_descript_3] = $8;
            tree_node->child[child_referenc_4] = $6;

            tree_node->child[child_misc1___14] = $4;

            tree_node->child[child_default_10] = $7;
            tree_node->child[child_access__11] = $5;

            tree_node->child[child_misc2___15] = $3;

            if($7->child[child_identity_0]) {

                if( ($3->type == Counter32) || ($3->type == Counter64) ) {
                    print_warning("*** WARNING ***  DEFVAL is not allowed with Counter32 or Counter64, ignoring.", lineBuf);
                    tree_node->child[child_default_10] = NULL;
                }
            }

            $$ = tree_node; 
    }
    ;

variationAccessPart
    :   ACCESS VariationAccess { $$ = $2; }
    |   empty
    ;

VariationAccess 
    :   NotImplemented
    |   AccessibleForNotify
    |   ReadOnly
    |   ReadWrite
    |   ReadCreate
    |   WriteOnly
    |   NotAccessible
    ;

/* must be a refinement for object's SYNTAX clause */
complianceSyntaxPart
    :   SyntaxPart
    |   empty
    ;

/* must be a refinement for object's SYNTAX clause */
writeSyntaxPart
    :   WRITE_SYNTAX Syntax { $$ = $2; }
    |   empty
    ;

moduleName
    :   UppercaseIdentifier moduleOID {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $1;

            tree_node->child[1] = $2;

            $$ = tree_node; 
    }
    |   empty { /* only if contained in MIB module */ }
    ;

moduleOID
    :   ObjectIdentifierValue
    |   empty
    ;

modulePart
    :   ModuleList
    |   empty
    ;

ModuleList
    :   Module { 
            $1->last_tree_node_in_list = $1;
    }
    |   ModuleList Module { 
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

Module
    :   SUPPORTS moduleName INCLUDES LBRACE VarTypeList RBRACE variationPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[1]          = $5;
            tree_node->child[5]          = $7;

            $$ = tree_node; 
    }
    ;

variationPart
    :   VariationList
    |   empty
    ;

VariationList
    :   Variation { 
            $1->last_tree_node_in_list = $1;
    }
    |   VariationList Variation {  
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

VarTypeList
    :   VarTypes
    |   VarTypes COMMA {
        print_warning("*** WARNING ***  There is an extra comma at the end of the variable type list.", lineBuf);
    }
    ;

VarTypes
    :   VarType { 
            $1->last_tree_node_in_list = $1; 
    }
    |   VarTypes COMMA VarType {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   VarTypes VarType {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** WARNING ***  There is a missing COMMA in the Variable Type List.", lineBuf);
    }
    ;

VarType
    :   LowercaseIdentifier
    |   UppercaseIdentifier {
            print_warning("*** WARNING ***  Variable types should be LowercaseIdentifiers.", lineBuf);
    }
    ;

SyntaxPart
    :   SYNTAX Syntax {
            $$ = $2;
    }
    ;

/*
   rfc1212 - The DEFVAL clause, which need not be present in SMIv1, defines an acceptable
   default value which may be used when an object instance is created at
   the discretion of the agent acting in conformance with the third
   paradigm described in Section 4.2 above.

*/
defaultValuePart
    :   DEFVAL LBRACE DefaultValue RBRACE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = DEFVAL;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;
    }
    |   empty 
    ;

/* number is non-negative */
NamedBit
    :   LowercaseIdentifier LPAREN Number RPAREN  {
            $1->child[child_identity_0] = $3;
    }
    |   UppercaseIdentifier LPAREN Number RPAREN  {
            $1->child[child_identity_0] = $3;
             print_warning("*** WARNING ***  Named items should begin with a lowercase letter.", lineBuf);
    }
    |   Number LPAREN Number RPAREN  {
            $1->child[child_identity_0] = $3;
             print_warning("*** WARNING ***  Numbers should not be used for named items.", lineBuf);
    }
    ;

NamedBitListPlugin
    :   NamedBitList
    |   NamedBitList COMMA {
            print_warning("*** WARNING ***  There is an extra COMMA at then end of the Named Item List.", lineBuf);
    }
    ;

NamedBitList
    :   NamedBit {
            symbol_table * enumerationTable = NULL;

            /* create enumeration table */
            enumerationTable = malloc_table(enumerationTable);
 
            /* insert name into symbol table */
            insert_name($1, enumerationTable);

            $1->enumeratedItems = enumerationTable;

            $1->last_tree_node_in_list = $1;

    }
    |   NamedBitList COMMA NamedBit {

            /* insert name into symbol table */
            insert_name($3, $1->enumeratedItems);

            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   NamedBitList NamedBit {

            insert_name($2, $1->enumeratedItems);

            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** WARNING ***  There is a missing COMMA in the Named Item List.", lineBuf);
    }
    ;

SMIv2Access
    :   NotAccessible
    |   AccessibleForNotify
    |   ReadOnly
    |   ReadWrite
    |   ReadCreate
    ;

/*
-- Extended UTCTime, to allow dates with four-digit years
-- (Note that this definition of ExtUTCTime is not to be IMPORTed
--  by MIB modules.)
ExtUTCTime ::= OCTET STRING(SIZE(11 | 13))
    -- format is YYMMDDHHMMZ or YYYYMMDDHHMMZ
    --   where: YY   - last two digits of year (only years
    --                 between 1900-1999)
    --          YYYY - last four digits of the year (any year)
    --          MM   - month (01 through 12)
    --          DD   - day of month (01 through 31)
    --          HH   - hours (00 through 23)
    --          MM   - minutes (00 through 59)
    --          Z    - denotes GMT (the ASCII character Z)
    --
    -- For example, "9502192015Z" and "199502192015Z" represent
    -- 8:15pm GMT on 19 February 1995. Years after 1999 must use
    -- the four digit year format. Years 1900-1999 may use the
    -- two or four digit format.

*/
ExtUTCTime
    :   QuotedString
    ;

StatusPart
    :   STATUS Status {
            $$ = $2;
    }
    ;

Status
    :   CurrentKeyword     /* SMIv2 */
    |   DeprecatedKeyword  /* both */
    |   ObsoleteKeyword    /* both */
    |   MandatoryKeyword   /* SMIv1 */
    |   OptionalKeyword    /* SMIv1 */
    ;

referencePart
    :   REFERENCE QuotedString { 
            $$ = $2;
    }
    |   empty {
    }
    ;

RangeRestriction
    :   LPAREN RangeListPlugin RPAREN {
        $$ = $2;
    }
    ; 

SizeRestriction
    :   LPAREN SizeKeyword RangeRestriction RPAREN {
        $$ = $3;
    }
    ; 

RangeListPlugin
    :   RangeList
    |   RangeList BAR {
            print_warning("*** WARNING ***  There is an extra BAR at the end of the range list.", lineBuf);
    }
    ;

RangeList
    :   ValueRange {
            $1->last_tree_node_in_list = $1;
    }
    |   RangeList BAR ValueRange {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== TOKEN HOLDERS ======================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/*
rfc2578 3.1.1.  Textual Values and Strings

   Some clauses in a macro invocation may take a character string as a
   textual value (e.g., the DESCRIPTION clause).  Other clauses take
   binary or hexadecimal strings (in any position where a non-negative
   number is allowed).

   A character string is preceded and followed by the quote character
   ("), and consists of an arbitrary number (possibly zero) of:

      - any 7-bit displayable ASCII characters except quote ("),
      - tab characters,
      - spaces, and
      - line terminator characters (\n or \r\n).

   The value of a character string is interpreted as ASCII.

   A binary string consists of a number (possibly zero) of zeros and
   ones preceded by a single (') and followed by either the pair ('B) or
   ('b), where the number is a multiple of eight.

   A hexadecimal string consists of an even number (possibly zero) of
   hexadecimal digits, preceded by a single (') and followed by either
   the pair ('H) or ('h).  Digits specified via letters can be in upper
   or lower case.

   Note that ASN.1 comments can not be enclosed inside any of these
   types of strings.
*/

QuotedString
    :   QUOTED_STRING {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            /* stringBuf was zeroed by the lexer before being filled */
            /* so there is a null after the string */ 
            tree_node->string = (char *)malloc( (stringPos + 1)*sizeof(char) ); 
            memcpy(tree_node->string, stringBuf, stringPos);
            tree_node->string[stringPos] = '\0';

            tree_node->freeMe = 1;

            $$ = tree_node; 
    }
    ;

LowercaseIdentifier
    :   LOWERCASE_IDENTIFIER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = strdup(yylval.char_ptr);

            tree_node->freeMe = 1;

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node; 
    }
    |   OptionalKeyword { 
            print_warning("*** WARNING ***  Don't use the \"optional\" keyword as a lowercase identifier.", lineBuf);
    }
    |   ReadOnly { 
            print_warning("*** WARNING ***  Don't use the \"read-only\" keyword as a lowercase identifier.", lineBuf);
    }
    |   ReadWrite { 
            print_warning("*** WARNING ***  Don't use the \"read-write\" keyword as a lowercase identifier.", lineBuf);
    }
    |   WriteOnly {
            print_warning("*** WARNING ***  Don't use the \"write-only\" keyword as a lowercase identifier.", lineBuf);
    }
    |   NotAccessible {
            print_warning("*** WARNING ***  Don't use the \"not-accessible\" keyword as a lowercase identifier.", lineBuf);
    }
    |   NotImplemented {
            print_warning("*** WARNING ***  Don't use the \"not-implemented\" keyword as a lowercase identifier.", lineBuf);
    }
    |   AccessibleForNotify {
            print_warning("*** WARNING ***  Don't use the \"acccessible_for_notifify\" keyword as a lowercase identifier.", lineBuf);
    }
    |   ReadCreate {
            print_warning("*** WARNING ***  Don't use the \"read-create\" keyword as a lowercase identifier.", lineBuf);
    }
    |   DeprecatedKeyword {
            print_warning("*** WARNING ***  Don't use the \"deprecated\" keyword as a lowercase identifier.", lineBuf);
    }
    |   CurrentKeyword {
            print_warning("*** WARNING ***  Don't use the \"current\" keyword as a lowercase identifier.", lineBuf);
    }
    |   MandatoryKeyword {
            print_warning("*** WARNING ***  Don't use the \"mandatory\" keyword as a lowercase identifier.", lineBuf);
    }
    |   ObsoleteKeyword {
            print_warning("*** WARNING ***  Don't use the \"obsolete\" keyword as a lowercase identifier.", lineBuf);
    }
    ;

UppercaseIdentifier
    :   UPPERCASE_IDENTIFIER  { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node;
    }
    ;

Oid
    :   OID {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = strdup(yylval.char_ptr);

            tree_node->freeMe = 1;

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node; 
    }
    ;

Number
    :   POS_NUMBER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = POS_NUMBER;
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            /* convert string to decimal number */
            tree_node->number = strtoul(tree_node->string, (char **)NULL, 10);

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node; 
    }
    |   MAX {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = MAX;
            $$ = tree_node; 
    }
    |   MIN {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = MIN;
            $$ = tree_node; 
    }
    |   NEG_NUMBER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = NEG_NUMBER;
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            /* convert string to decimal number */
            tree_node->number = strtoul(tree_node->string, (char **)NULL, 10);

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node; 
    }
    |   BINARY_NUMBER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = POS_NUMBER;
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            /* convert string to decimal number */
            tree_node->number = strtoul(tree_node->string+1, (char **)NULL, 2);

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node; 
    }
    |   HEX_NUMBER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = POS_NUMBER;
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            /* convert string to decimal number */
            tree_node->number = strtoul(tree_node->string+1, (char **)NULL, 16);

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node; 
    }
    ;

ReadOnly
    :   read_only { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "read-only"; 
            $$ = tree_node;
    }
    ;

ReadWrite
    :   read_write { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "read-write"; 
            $$ = tree_node;
    }
    ;

WriteOnly
    :   write_only { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "write-only"; 
            $$ = tree_node;

            if(SMIv2Flag == 1) { 
                print_warning("*** WARNING *** \"write-only\" is not used in SMIv2.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_warning("*** NOTE *** \"write-only\" is not used in SMIv2, setting SMIv1 flag.", lineBuf);
            }

    }
    ;

NotAccessible
    :   not_accessible { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "not-accessible"; 
            $$ = tree_node;
    }
    ;

NotImplemented
    :   not_implemented { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "not-implemented"; 
            $$ = tree_node;

            if(SMIv1Flag == 1) { 
                print_warning("*** WARNING *** \"not-implemented\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"not-implemented\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

AccessibleForNotify
    :   accessible_for_notify { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "accessible-for-notify"; 
            $$ = tree_node;

            if(SMIv1Flag == 1) { 
                print_warning("*** WARNING *** \"accessible-for-notify\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"accessible-for-notify\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

ReadCreate
    :   read_create { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "read-create"; 
            $$ = tree_node;

            if(SMIv1Flag == 1) { 
                print_warning("*** WARNING *** \"read-create\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"read-create\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

BitsKeyword
    :   BITS {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = BITS;
            tree_node->string = "Bits";
            $$ = tree_node;

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"BITS\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"BITS\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    ;

SequenceKeyword
    :   SEQUENCE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    ;

CurrentKeyword
    :   currrent { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "current";
            $$ = tree_node;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) { 
                print_warning("*** WARNING *** \"current\" should not be used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"current\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    ;

DeprecatedKeyword
    :   deprecated { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "deprecated";
            $$ = tree_node;
    }
    ;

ObsoleteKeyword
    :   obsolete { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "obsolete"; 
            $$ = tree_node;
    }
    ;

MandatoryKeyword
    :   mandatory { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "mandatory";
            $$ = tree_node;
            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) { 
                print_warning("*** WARNING *** \"mandatory\" should only be used in SMIv1 (perhaps you want to use \"current\").", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_warning("*** NOTE *** \"mandatory\" is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    ;

OptionalKeyword
    :   optional { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "optional"; 
            $$ = tree_node;
            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) { 
                print_warning("*** WARNING *** \"optional\" should only be used in SMIv1.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_warning("*** NOTE *** \"optional\" is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    ;

OctetStringKeyword
    :   OCTET STRING {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "OctetString";
            tree_node->type = OCTET;
            $$ = tree_node;
    }
    ;

Integer32Keyword
    :    Integer32 {        /* (-2147483648..2147483647) */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Integer32";
            $$ = tree_node; 

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"Integer32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"Integer32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

/*
   This application-wide type represents a 32-bit internet address.  It
   is represented as an OCTET STRING of length 4, in network byte-order.
*/
IpAddressKeyword
    :   IpAddress {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "IpAddress";
            $$ = tree_node; 
    }
    ;

/*
   This application-wide type represents a non-negative integer which
   monotonically increases until it reaches a maximum value, then it
   wraps around and starts increasing again from zero.  This memo
   specifies a maximum value of 2^32-1 (4294967295 decimal) for
   counters.
*/
Counter32Keyword
    :   Counter32 { /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Counter32";
            tree_node->type = Counter32;

            $$ = tree_node; 
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"Counter32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"Counter32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

/*
   This application-wide type represents a non-negative integer, which
   may increase or decrease, but which latches at a maximum value.  This
   memo specifies a maximum value of 2^32-1 (4294967295 decimal) for
   gauges.
*/
Gauge32Keyword
    :   Gauge32 { /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Gauge32";
            $$ = tree_node; 
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"Gauge32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"Gauge32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

GaugeKeyword
    :   Gauge { /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Gauge";
            $$ = tree_node; 

            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) {
                print_warning("*** WARNING *** \"Gauge\" is only used in SMIv1.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_warning("*** NOTE *** \"Gauge\" is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }

    }
    ;

CounterKeyword
    :   Counter {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Counter";
            $$ = tree_node; 

            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) {
                print_warning("*** WARNING *** \"Counter\" is only used in SMIv1.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_warning("*** NOTE *** \"Counter\" is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    ;

Unsigned32Keyword
    :   Unsigned32 {        /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Unsigned32";
            $$ = tree_node;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"Unsigned32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"Unsigned32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    |   UInteger32 {        /* (0..4294967295)         */
            /* this is an outdated version not in the language anymore */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Unsigned32";
            $$ = tree_node;

            print_warning("*** WARNING *** \"UInteger32\" is not used anymore, use \"Unsigned32\".", lineBuf);

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"Unsigned32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"Unsigned32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

/*
   This application-wide type represents a non-negative integer which
   counts the time in hundredths of a second since some epoch.  When
   object types are defined in the MIB which use this ASN.1 type, the
   description of the object type identifies the reference epoch.
*/
TimeTicksKeyword
    :   TimeTicks {        /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "TimeTicks";
            $$ = tree_node; 
    }
    ;

/*
   This application-wide type supports the capability to pass arbitrary
   ASN.1 syntax.  A value is encoded using the ASN.1 basic rules into a
   string of octets.  This, in turn, is encoded as an OCTET STRING, in
   effect "double-wrapping" the original ASN.1 value.

   Note that a conforming implementation need only be able to accept and
   recognize opaquely-encoded data.  It need not be able to unwrap the
   data and then interpret its contents.

   Further note that by use of the ASN.1 EXTERNAL type, encodings other
   than ASN.1 may be used in opaquely-encoded data.
*/
OpaqueKeyword
    :   Opaque {            /* IMPLICIT OCTET STRING     */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Opaque";
            $$ = tree_node; 
    }
    ;

Counter64Keyword    
    :   Counter64 {            /* (0..18446744073709551615) */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Counter64";
            tree_node->type = Counter64;

            $$ = tree_node; 
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** WARNING *** \"Counter64\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_warning("*** NOTE *** \"Counter64\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    ;

IntegerKeyword
    :   INTEGER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "INTEGER";
            $$ = tree_node;
    }  
    ;

BitStringKeyword  
    :    BIT STRING {        /* (SIZE (0..65535))         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = BIT;
            tree_node->string = "BIT STRING";
            $$ = tree_node;

            print_warning("*** WARNING *** \"BIT STRING\" is no longer a legal type.", lineBuf);
    }
    ;

ObjectIdentifierKeyword
    :   OBJECT IDENTIFIER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "ObjectID";
            tree_node->type   = IDENTIFIER;
            $$ = tree_node;
    }
    ;

SizeKeyword
    :   SIZE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = SIZE;
            tree_node->string = "SIZE";
            $$ = tree_node; 

    }
    ;

ObjectSyntaxKeyword
    :   ObjectSyntax {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "ObjectSyntax";
            $$ = tree_node; 

    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* =================== GARBAGE CATCHERS ======================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* used to catch garbage in bad mibs so they still parse */
/* this stuff is not really in the language */

TypeGarbage
    :   TypeTag Type { $$ = $2; }
    |   TypeTag IMPLICIT Type { $$ = $3; }
    |   TypeTag EXPLICIT Type { $$ = $3; }
    |   CHOICE LBRACE AlternativeTypeList RBRACE       { $$ = $3; }
    |   CHOICE LBRACE AlternativeTypeList COMMA RBRACE { $$ = $3; }
    |   GarbageType
    ;

AlternativeTypeList
    :   NamedType
    |   AlternativeTypeList COMMA NamedType
    ;

NamedType
    :   LowercaseIdentifier Type
    |   Type
    ;

TypeTag
    :   LBRACKET class NumericValue RBRACKET { $$ = $3; }
    ;

class
    :   UNIVERSAL { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   APPLICATION { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   PRIVATE { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   empty
    ;

NumericValue
    :   Number
    |   DefinedValue
    ;

GarbageType
    :   ANY {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   ANY DEFINED BY LowercaseIdentifier { $$ = $4; }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================= MAIN ============================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

%%

int main(int argc, char * argv[])
{
    int i;
    int parseReturnValue;

    tree * tmpTreePtr  = NULL;
    tree * defLoc      = NULL, * thisLoc = NULL;

    char * inputFileName             = NULL;
    char * tmpCharPtr                = NULL;
    char * aux_dat_filename          = NULL;
    char * mibLocEnvironmentVariable = NULL;


    /* *********************** */
    /* MALLOC BUFFERS & TABLES */
    /* *********************** */

    lineBuf = (char *)malloc(MYBUFSIZE*sizeof(char));
    if(lineBuf) {
        memset((void *)lineBuf,        0, MYBUFSIZE );
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for lineBuf!\n");
        _exit(1);
    }

    stringBuf = (char *)malloc(2*MYBUFSIZE*sizeof(char));
    if(stringBuf) {
        memset((void *)stringBuf,      0, stringBufSize );
        stringBufSize = 2*MYBUFSIZE;
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for stringBuf!\n");
        _exit(1);
    }

    workBuf = (char *)malloc(MYBUFSIZE*sizeof(char));
    if(workBuf) {
        memset((void *)workBuf,        0, MYBUFSIZE );
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for workBuf!\n");
        _exit(1);
    }

    fileNameBuffer = (char *)malloc(MYBUFSIZE*sizeof(char));
    if(fileNameBuffer) {
        memset((void *)fileNameBuffer, 0, MYBUFSIZE );
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for fileNameBuffer!\n");
        _exit(1);
    }

    firstLineBuf   = (char *)malloc(MYBUFSIZE*sizeof(char));
    if(firstLineBuf) {
        memset((void *)firstLineBuf, 0, MYBUFSIZE );
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for firstLineBuf!\n");
        _exit(1);
    }

    /* for all parses */
    allParsesTypeTable  = malloc_table(allParsesTypeTable);

    mibReplacementTable = malloc_table(mibReplacementTable);

    /* single parse only */
    virtualTables = malloc_table(virtualTables);
    emittedNodes  = malloc_table(emittedNodes);
    singleParseTypeTable     = malloc_table(singleParseTypeTable);
    importedMIBs  = malloc_table(importedMIBs);


    /* ************************** */
    /* GET COMMAND LINE ARGUMENTS */
    /* ************************** */

    /*
    for(i = 0 ; i < argc ; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    */

    /* get command-line arguments */
    if(argc > 1) {
        
        for(i = 1 ; i < argc ; i++) {

            /* scan for option */
            if(argv[i][0] == '-') {

                /* check for output filename option*/
                if( (argv[i][1] =='o' || argv[i][1] =='O' ) && (argc > (i+1))  ) {

                    outputFileName = strdup(argv[i+1]);
                    globalMultipleOutputFilesFlag = 0;
                    i++;

                /* check for c++ comment flag */
                } else if( (argv[i][1] =='c' ) || (argv[i][1] =='C' )) {
                    globalCppCommentFlag = 1;

                /* check for description flag */
                } else if( (argv[i][1] =='d') || (argv[i][1] =='D')) {
                    globalProduceDescrFlag = 1;

                } else if( (argv[i][1] =='f'  || argv[i][1] =='F') && (argc > (i+1))  ) {

                    outputDirectoryName = strdup(argv[i+1]);
                    i++;

                /* check for help flag */
                } else if( (argv[i][1] =='h') || (argv[i][1] =='H')) {
                    helpFlag = 1;

                /* check for help flag */
                } else if( (argv[i][1] =='-')  && ((argv[i][2] =='h') || (argv[i][2] =='H') ) ) {

                    helpFlag = 1;

                /* check for rename input file option */
                } else if( (argv[i][1] =='r' ) || ( argv[i][1] =='R' )) {
                    globalRenameFlag = 1;

                /* check for suppress output option */
                } else if( (argv[i][1] =='s' ) || ( argv[i][1] =='S' )) {
                    globalSuppressOutputFlag = 1;

                /* check for automatic update option */
                } else if( (argv[i][1] =='u' ) || ( argv[i][1] =='U' )) {
                    updateImportsFlag = 1;

                /* check for mib import search directories */
                } else if( ((argv[i][1] =='m') || (argv[i][1] =='M')  )  && (argc > (i+1))  ) {

                    memset(lineBuf, 0, MYBUFSIZE );
                    strcpy(lineBuf, argv[i+1]);

                    tmpCharPtr = strtok(lineBuf, SEPARATOR);

                    while(tmpCharPtr) {

                        tree * tree_node = NULL;
                        tree_node = malloc_node(tree_node, SAVE_FOR_ALL_PARSES);
                        tree_node->string = strdup(tmpCharPtr);
                        tree_node->freeMe = 1;

                        /* insert node at end of list */
                        if(globalImportDirectoryList == NULL) {
                            globalImportDirectoryList = tree_node;
                            globalImportDirectoryList->last_tree_node_in_list = tree_node;
                        } else {
                            globalImportDirectoryList->last_tree_node_in_list->next_tree_node = tree_node;
                            globalImportDirectoryList->last_tree_node_in_list = tree_node;
                        }

                        tmpCharPtr = strtok(NULL, SEPARATOR);
                    }    

                    i++;

                } else {
                    fprintf(stderr, "ignoring meaningless option: %s\n", argv[i]);
                }

            /* any name after argv[0] that is not an option must be an input filename */
            } else {

                tree * tree_node = NULL;
                tree_node = malloc_node(tree_node, SAVE_FOR_ALL_PARSES);
                tree_node->string = strdup(argv[i]);
                tree_node->freeMe = 1;

                /* insert node at end of list */
                if(globalInputFilenameList == NULL) {
                    globalInputFilenameList = tree_node;
                    globalInputFilenameList->last_tree_node_in_list = tree_node;
                } else {
                    globalInputFilenameList->last_tree_node_in_list->next_tree_node = tree_node;
                    globalInputFilenameList->last_tree_node_in_list = tree_node;
                }

            }

        }

    }

    /* print usage if no arguments or helpFlag set */
    if((argc == 1) || helpFlag) {

        fprintf(stderr, "\nmibgc compiles SNMP SMIv1, SMIv2, and SMIv3 MIBs to produce mosy-compatible .def files.\n\n");
#ifndef SR_WINDOWS
        fprintf(stderr, "USAGE: mibgc [-c|C] [-d|D] [-f|F outputdirectory] [-m|M mibFileDir:mibFileDir:...] [-o|O outputfilename]\n");
        fprintf(stderr, "           [-r|R] [-s|S] inputFileNames (e.g. mibgc *.my -o mud)\n\n");
#else
        fprintf(stderr, "USAGE: mibgc [-c|C] [-d|D] [-f|F outputdirectory] [-m|M mibFileDir;mibFileDir;...] [-o|O outputfilename]\n");
        fprintf(stderr, "           [-r|R] [-s|S] inputFileNames (e.g. mibgc *.my -o mud)\n\n");
#endif /* SR_WINDOWS */
        fprintf(stderr, "   FLAGS (use either upper or lower case:\n");
        fprintf(stderr, "   ~~~~~\n");
        fprintf(stderr, "   -c -C   comment     - treat comments like C++ comments i.e. only end-of-line ends comment\n");
        fprintf(stderr, "   -d -D   description - produce description file\n");

#ifndef SR_WINDOWS
        fprintf(stderr, "   -f -F   files       - directory to dump output files (e.g. ../mibgc *.my -s -f ~/dump)\n");
#else
        fprintf(stderr, "   -f -F   files       - directory to dump output files (e.g. ../mibgc *.my -s -f C:\\dump)\n");
#endif
        fprintf(stderr, "   -h -H   help        - show help information (also --h or --H)\n");
#ifndef SR_WINDOWS
        fprintf(stderr, "   -m -M   MIB         - directories (separated by colons) to search for AUX-MIB.dat and imported MIBs\n");
#else
        fprintf(stderr, "   -m -M   MIB         - directories (separated by semicolons) to search for AUX-MIB.dat and imported MIBs\n");
#endif /* SR_WINDOWS */
        fprintf(stderr, "   -o -O   output      - produce specified output file\n");
        fprintf(stderr, "                         if used, all compiled MIBs placed in single file\n");
        fprintf(stderr, "                         if NOT used, base of input name plus .def used for each MIB\n");
        fprintf(stderr, "   -r -R   rename      - rename root of input filename to match module name\n");
        fprintf(stderr, "   -s -S   suppress    - suppress warning output\n");
        fprintf(stderr, "   -u -U   update      - automatically replace/update obsolete imports e.g. RFC1065-SMI --> RFC1155-SMI\n\n");

        fprintf(stderr, "   COMMAND LINE ARGUMENTS:\n");
        fprintf(stderr, "   ~~~~~~~~~~~~~~~~~~~~~~\n");
        fprintf(stderr, "      order      - order of command-line arguments is irrelevant.\n");
#ifndef SR_WINDOWS
        fprintf(stderr, "      spaces     - enclose argument with quotes if using spaces e.g. -m \"test dir/mibs\".\n");
#else
        fprintf(stderr, "      spaces     - enclose path with quotes if using spaces e.g. -m \"C:\\Program Files\\mibs\"\n");
#endif /* SR_WINDOWS */
        fprintf(stderr, "      wildcards  - (*,?) are expanded e.g. \"*.my\" becomes all files that end in \"my\"\n\n");

        fprintf(stderr, "   SEARCH ORDER for AUX-MIB.dat and imported MIBs:\n");
        fprintf(stderr, "   ~~~~~~~~~~~~\n");
        fprintf(stderr, "       (1) command-line paths         (first to last - set with -m or -M)\n");
        fprintf(stderr, "       (2) environment-variable paths (first to last - set with SR_MIBS_MIBGC)\n");
#ifndef SR_WINDOWS
        fprintf(stderr, "       (3) /usr/local/mibs (default mib location)\n");
        fprintf(stderr, "       (4) ./\n\n");
#else
        fprintf(stderr, "       (3) C:\\mibs (default mib location)\n");
        fprintf(stderr, "       (4) current directory\n\n");
#endif /* SR_WINDOWS */
        fprintf(stderr, "   PRECEDENCE of definitions:\n");
        fprintf(stderr, "   ~~~~~~~~~~\n");
        fprintf(stderr, "       (1) AUX-MIB.dat,\n"); 
        fprintf(stderr, "       (2) current MIB,\n");
        fprintf(stderr, "       (3) imported MIBs. (The precedence within imported MIBs is first-come, first-served.)\n\n"); 

        fprintf(stderr, "   OVERWRITE PROTECTION:\n");
        fprintf(stderr, "   ~~~~~~~~~~~~~~~~~~~~\n");
        fprintf(stderr, "   If the intended output file already exists then an incrementing three-digit number is used\n"); 
        fprintf(stderr, "   for file history, i.e. .def always contains the newest compile, .def_000 contains the oldest compile\n\n");

    }

    /* do parsing if needed*/
    if((argc > 1) && !helpFlag) {

        /* is there an environment variable set for import mib directories */
        if( (mibLocEnvironmentVariable = (char *)getenv("SR_MIBGC_MIBS")) != NULL) {

            memset(lineBuf, 0, MYBUFSIZE );
            strcpy(lineBuf, mibLocEnvironmentVariable);

            tmpCharPtr = strtok(lineBuf, SEPARATOR);

            while(tmpCharPtr) {

                tree * tree_node = NULL;
                tree_node = malloc_node(tree_node, SAVE_FOR_ALL_PARSES);
                tree_node->string = strdup(tmpCharPtr);
                tree_node->freeMe = 1;

                /* insert node at end of list */
                if(globalImportDirectoryList == NULL) {
                    globalImportDirectoryList = tree_node;
                    globalImportDirectoryList->last_tree_node_in_list = tree_node;
                } else {
                    globalImportDirectoryList->last_tree_node_in_list->next_tree_node = tree_node;
                    globalImportDirectoryList->last_tree_node_in_list = tree_node;
                }

                tmpCharPtr = strtok(NULL, SEPARATOR);
            }    

        }


        /* load replacement MIBs for imports */
        load_mib_replacement_table();

        /*
        dump_table1(mibReplacementTable);
        */

        /* add default import mib location, then ./ to import path */
        defLoc = malloc_node(defLoc, 0);

#ifndef SR_WINDOWS
        defLoc->string = strdup("/usr/local/mibs");
#else
        defLoc->string = strdup("C:\\mibs");
#endif /* SR_WINDOWS */

        defLoc->freeMe = 1;

        /* insert node at end of list */
        if(globalImportDirectoryList == NULL) {
            globalImportDirectoryList = defLoc;
            globalImportDirectoryList->last_tree_node_in_list = defLoc;
        } else {
            globalImportDirectoryList->last_tree_node_in_list->next_tree_node = defLoc;
            globalImportDirectoryList->last_tree_node_in_list = defLoc;
        }

        thisLoc = malloc_node(thisLoc, 0);
        thisLoc->string = strdup(".");
        thisLoc->freeMe = 1;

        /* insert node at end of list */
        globalImportDirectoryList->last_tree_node_in_list->next_tree_node = thisLoc;
        globalImportDirectoryList->last_tree_node_in_list = thisLoc;


        /* print out list of import directories */
        if(!globalSuppressOutputFlag) {
            fprintf(stderr, "import directories to search:\n");

            tmpTreePtr = globalImportDirectoryList;
            while(tmpTreePtr && tmpTreePtr->string) {
                printf("%s\n", tmpTreePtr->string);
                tmpTreePtr = tmpTreePtr->next_tree_node;
            }
        }

        /* do import parse of aux definitions */
        /* an import parse doesn't produce any output code,
           just puts the data into the type table */
        /* try each of the import paths in turn */

        tmpTreePtr = globalImportDirectoryList;
        while(tmpTreePtr && tmpTreePtr->string) {

            /* import the file */
            memset(lineBuf, 0, MYBUFSIZE );
            strcpy(lineBuf, tmpTreePtr->string);

#ifndef SR_WINDOWS
            strcat(lineBuf, "/");
#else
            strcat(lineBuf, "\\");
#endif /* SR_WINDOWS */
            strcat(lineBuf, "AUX-MIB.dat");

            /* ************************** */
            aux_dat_filename = strdup(lineBuf);

            /* do import parse, but save type nodes for all parses */
            saveForAllParsesFlag = 1;
            openOutputFlag = 1;
            parseReturnValue = parse_file(IMPORT_PARSE, aux_dat_filename, "AUX-MIB.dat");
            saveForAllParsesFlag = 0;

            if(aux_dat_filename) free(aux_dat_filename);
            aux_dat_filename = NULL;
            /* ************************** */

            /* if we have successfully parsed the file, then quit looking */
            if(!parseReturnValue) {
                break;
            }

            tmpTreePtr = tmpTreePtr->next_tree_node;
        }


        /* ******************************* */
        /* PROCESS EACH INPUT FILE IN TURN */
        /* ******************************* */

        tmpTreePtr = globalInputFilenameList;
        while(tmpTreePtr && tmpTreePtr->string) {

            tree * name_node = NULL, * tree_node = NULL;

            /* ************************** */
            inputFileName = tmpTreePtr->string;

            /* do full parse, not import parse, of next file in list to parse */
            parseReturnValue = parse_file(NORMAL_PARSE, inputFileName, inputFileName);

            /* ************************** */

            /* add self to list of imported MIBs, so
               this file is not imported during this parse */
            name_node = malloc_node(name_node, SINGLE_PARSE);
            name_node->string = strdup(inputFileName);
            name_node->freeMe = 1;

            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[0] = name_node;

            insert_type_node( tree_node, importedMIBs);

            /* **************************************** */
            /* CLOSE ONE GROUP OF MULTIPLE OUTPUT FILES */
            /* **************************************** */

            if(globalMultipleOutputFilesFlag) {

                /* ********************** */
                /* close description file */
                /* ********************** */

                if(globalProduceDescrFlag) {

                    if(descrFilePtr) fclose(descrFilePtr);
                    if(descrFileName) free(descrFileName);
                    descrFileName = NULL;

                }

                /* ********************************* */
                /* close mosy compatible output file */ 
                /* ********************************* */

                if(outputFilePtr) fclose(outputFilePtr);
                if(outputFileName) free(outputFileName);
                outputFileName = NULL;

            }


            /* RESET FOR EACH INPUT FILE PROCESSED */

            maxColumnWidth = 20;

            /* reset imports for next parse to NULL */
            importFromList = NULL;

            /* free old tree for new parse */
            if(singleParseNodeList) {
                free_all_nodes(singleParseNodeList);
                singleParseNodeList = NULL;
            }

            /* reset tables and emitted nodes for each new parse */
            memset((void *)virtualTables,  0, TABLE_SIZE*sizeof(tree *));
            memset((void *)emittedNodes, 0, TABLE_SIZE*sizeof(tree *));
            memset((void *)singleParseTypeTable, 0, TABLE_SIZE*sizeof(tree *));
            memset((void *)importedMIBs, 0, TABLE_SIZE*sizeof(tree *));

            tmpTreePtr = tmpTreePtr->next_tree_node;
        }


        /* ************************* */
        /* CLOSE SINGLE OUTPUT FILE  */
        /* ************************* */

        /* if there is a single output file, then close it */
        if(!globalMultipleOutputFilesFlag) {

            /* ********************** */
            /* close description file */
            /* ********************** */

            if(globalProduceDescrFlag) {
                if(descrFilePtr) fclose(descrFilePtr);
                if(descrFileName) free(descrFileName);
                descrFileName = NULL;
            }

            /* ********************************* */
            /* close mosy compatible output file */ 
            /* ********************************* */

            if(outputFilePtr) fclose(outputFilePtr);
            if(outputFileName) free(outputFileName);
            outputFileName = NULL;

            /* **************************** */
            /* FREE TREE NODES IN FREE LIST */
            /* **************************** */
            if(singleParseNodeList) {
                free_all_nodes(singleParseNodeList);
                singleParseNodeList = NULL;
            }

        }

        /* ************************ */
        /* FREE BUFFERS AND TABLES */
        /* ************************ */

        free(lineBuf);
        free(stringBuf);
        free(workBuf);
        free(fileNameBuffer);
        free(firstLineBuf);

        /* the nodes in these tables are freed at end of each parse */
        free(virtualTables);
        free(emittedNodes);
        free(singleParseTypeTable);
        free(importedMIBs);

        /* free all the nodes in the allParsesTypeTable plus lists */
        if(allParsesNodeList) {
            free_all_nodes(allParsesNodeList);
            allParsesNodeList = NULL;
        }

        free(allParsesTypeTable);

        if(lastFileParsed) {
            free(lastFileParsed);
        }

    }

    return(0);

}

/* ============================================================ */

int 
parse_file(int importFlag, char * myInputFileName, char * partialFileName)
{
    FILE * tmpFilePtr;

    int i;
    int parserReturnValue = 0;

    int fileCounter = 0;

    char * ptr;
    char * localInputFileName  = NULL;
    char * inputPrefix         = NULL;

    tree * mibAlreadyImportedPtr;

    /* SAVE STATE for things needed when output produced */
    tree * temp_root                  = root;
    int temp_SMIv1Flag                = SMIv1Flag;
    int temp_SMIv2Flag                = SMIv2Flag;
    int temp_SMIv3Flag                = SMIv3Flag;
    int temp_sawObjectIdentifier      = sawObjectIdentifier;

    tree * temp_importFromList;



    /* reset for error reporting */
    lastModuleName   = NULL;

    /* tell the parsing routines if this is an import,
       so don't display messages */
    importParseFlag = importFlag;

    /* set defaults for each new parse */
    line_cntr                = 1;
    col_cntr                 = 0;
    no_code_flag             = 0;
    string_line_cntr         = 0;
    stringWarnOn             = 0;
    root                     = NULL;

    if(lastFileParsed) {
        free(lastFileParsed);
    }

    /* for tracking errors */
    lastFileParsed = strdup(myInputFileName); 

    /* clear all the buffers */
    memset((void *)lineBuf,        0, MYBUFSIZE );
    memset((void *)stringBuf,      0, stringBufSize );
    memset((void *)workBuf,        0, MYBUFSIZE );
    memset((void *)fileNameBuffer, 0, MYBUFSIZE );
    memset((void *)firstLineBuf,   0, MYBUFSIZE );


    /* have we already parsed this file */
    mibAlreadyImportedPtr = lookup_type(myInputFileName, importedMIBs);

    if(mibAlreadyImportedPtr) {
        parserReturnValue = 1;
        goto restore_point;
    }


    /* OPEN INPUT FILE */
    if(  !(yyin = fopen(myInputFileName, "r"))  ) {

        if(!globalSuppressOutputFlag) {
            fprintf(stderr, "\n*** WARNING ***  Could not open file for parsing: \"%s\".\n\n", myInputFileName);
        }

        parserReturnValue = 1;
        goto restore_point;
    } 

    if(!globalSuppressOutputFlag) {
        fprintf(stderr, "*** NOTE ***  Opening file for parsing: \"%s\".\n", myInputFileName);
    }

    /* do the actual parse */
    parserReturnValue = yyparse();

    /* *** EXIT*** */
    if(parserReturnValue) {
        _exit(parserReturnValue);

        /* printf("parserreturnValue: %d\n", parserReturnValue); */

    }

    /* CLOSE INPUT FILE */
    fclose(yyin);

    /* GENERATE OUTPUT CODE */
    if(!root || no_code_flag) {
        fprintf(stderr, "\n* Warning - errors *\n");
        fprintf(stderr, "* No output code generated for: %s\n\n", outputFileName);
    } else {

        /* ************* */
        /* WRITE OUTPUT  */
        /* ************* */

        /* only open output file if not import */
        if(!importFlag) {

            /* 
             * If we don't have an output file name, but we do
             * have an input filename, then create the output filename
             * by adding .def to the root of the input filename.
             */

            if( myInputFileName && !outputFileName) {

                memset((void *)fileNameBuffer, 0, MYBUFSIZE);

                /* preface with outputDirectoryName if it exists */
                if(outputDirectoryName) {
                    strncpy(fileNameBuffer, outputDirectoryName, MYBUFSIZE/2);
#ifndef SR_WINDOWS
                    strcat(fileNameBuffer, "/");
#else
                    strcat(fileNameBuffer, "\\");
#endif
                    strcat(fileNameBuffer, myInputFileName);
                } else {
                    strcpy(fileNameBuffer, myInputFileName);
                }

                /* start at end of string to get root of input filename */
                i = strlen(fileNameBuffer) - 1;
                ptr = (char *)(fileNameBuffer+i);
                while( (*ptr != '.') && (i > 0) ) {
                    i--;
                    ptr--;
                }

                if(i > 0) {
                    *ptr = '\0';
                }

                strcat(fileNameBuffer, ".def");

                outputFileName = strdup(fileNameBuffer);

                /* check for description flag */
                if( globalProduceDescrFlag) {
                    fileNameBuffer[strlen(outputFileName)-1] = 's';
                    descrFileName  = strdup(fileNameBuffer); 
                }

            }

            if(globalMultipleOutputFilesFlag) {

                open_output_file(outputFileName);

                /* write header if not import */
                write_header(outputFilePtr);

            } else if(openOutputFlag){

                open_output_file(outputFileName);

                /* write header if not import */
                write_header(outputFilePtr);

                openOutputFlag = 0;

            }

        }        

        /* OPEN DESCRIPTION FILE */
        if(!importFlag && globalProduceDescrFlag) {
            /* open the description output file */
            if((descrFilePtr = fopen(descrFileName, "a")) == NULL) {
                fprintf(stderr, "\n*** ERROR ***  Can't open \"%s\" for writing.\n", descrFileName);
                _exit(-1);
            }

        }

        /* ***************************** */
        /* RENAME INPUT FILES IF NEEDED  */
        /* ***************************** */

        /* only rename if not import */
        if(!importFlag && globalRenameFlag) {

            memset(lineBuf, 0, MYBUFSIZE );
            strcpy(lineBuf, myInputFileName);

            /* start at end of string to get prefix of input filename */
            i  = strlen(lineBuf) - 1;
            ptr = (char *)(lineBuf+i);
            while( (*ptr != '/') && (i >= 0) ) {
                i--;
                *ptr = '\0';
                ptr--;
            }

            inputPrefix = strdup(lineBuf);

            strcat(lineBuf, lastModuleName);
            strcat(lineBuf, ".my");

            /* module name doesn't match inputfilename, so rename */
            if( strcmp(lineBuf, myInputFileName) ) {

                localInputFileName = strdup(lineBuf);

                memset(fileNameBuffer, 0, MYBUFSIZE);
                sprintf(fileNameBuffer, "%s%s", inputPrefix, localInputFileName);

                /* does the output file already exist? */
	            while( (tmpFilePtr = fopen(fileNameBuffer, "r")) != NULL) {

                    if(!globalSuppressOutputFlag) {
                        printf("\n*** WARNING ***  Output file \"%s\" already exists\n\n", fileNameBuffer);
                    }

                    memset(fileNameBuffer, 0, MYBUFSIZE);
                    sprintf(fileNameBuffer, "%s%s_%03d", inputPrefix, localInputFileName, fileCounter++);

                    fclose(tmpFilePtr);

	            }

                /* rename to next free _??? counter */

                memset(lineBuf, 0, MYBUFSIZE );
#ifndef SR_WINDOWS
                strcpy(lineBuf, "mv ");
#else
                strcpy(lineBuf, "rename ");
#endif /* SR_WINDOWS */
                strcat(lineBuf, myInputFileName);
                strcat(lineBuf, " ");
                strcat(lineBuf, fileNameBuffer);

                system(lineBuf);


                myInputFileName = localInputFileName;

            }

        }

        /* DEAL WITH IMPORTS FOR THIS FILE */
        temp_importFromList = importFromList;
        importFromList = NULL;

        if(!globalSuppressOutputFlag) {
            fprintf(stderr,"\n*** NOTE ***  Resolving imports for %s.\n\n", partialFileName);
        }

        do_imports(temp_importFromList, partialFileName );

        /* print mosy code for tree */
        if(!importFlag) {
            mosy_emit_all_nodes(root, outputFilePtr, descrFilePtr);

            fprintf(outputFilePtr, "\n-- ");
            for(i = 0 ; i < ((4*maxColumnWidth)+8) ; i++) {
                fprintf(outputFilePtr, "=");
            }   
            fprintf(outputFilePtr, "\n");

        }

    }

restore_point:

    /* RESTORE STATE */
    root                     = temp_root;
    SMIv1Flag                = temp_SMIv1Flag;
    SMIv2Flag                = temp_SMIv2Flag;
    SMIv3Flag                = temp_SMIv3Flag;
    sawObjectIdentifier      = temp_sawObjectIdentifier;

    /* free local mallocs */
    if(localInputFileName) free(localInputFileName);
    localInputFileName = NULL;

    if(inputPrefix) free(inputPrefix);
    inputPrefix = NULL;

    return parserReturnValue;
}

/* ============================================================ */

void 
do_imports(tree * local_importFromList, char * fileDoingImport)
{
    int parseReturnValue         = 0;

    tree * mibToImport           = NULL;
    tree * mibAlreadyImportedPtr = NULL;
    tree * directoryListPtr      = NULL;
    char * fileToImportPtr       = NULL;
    tree * nextSymbolToImportPtr = NULL;
    tree * nextImportListPtr     = NULL;
    tree * typeExistsPtr         = NULL;
    tree * alreadyEmittedPtr     = NULL;
    tree * tmpTreePtr            = NULL;
    char * importString          = NULL;


    mibToImport = local_importFromList;

    while(mibToImport) {

        importString = mibToImport->string;

        /* if automatic update/replace feature turned on */
        if(updateImportsFlag) {

            /* does an entry already exist */
            tmpTreePtr = lookup_name(importString, mibReplacementTable);
            if(tmpTreePtr) {

                if(!globalSuppressOutputFlag) {
                    fprintf(stderr, "\n*** NOTE ***  Automatically replacing import of %s with import of %s.\n\n", importString, tmpTreePtr->next_tree_node->string);
                }

                importString = tmpTreePtr->next_tree_node->string;

            }

        }

        if(tmpTreePtr) {
            tmpTreePtr = tmpTreePtr->next_tree_node;
        }

        while(importString) {

            /* check whether already imported */
            mibAlreadyImportedPtr = lookup_type(importString, importedMIBs);

            if(!mibAlreadyImportedPtr) {

                tree * name_node = NULL, * tree_node = NULL;

                name_node = malloc_node(name_node, SINGLE_PARSE);
                name_node->string = strdup(importString);
                name_node->freeMe = 1;

                tree_node = malloc_node(tree_node, SINGLE_PARSE);
                tree_node->child[0] = name_node;

                insert_type_node( tree_node, importedMIBs);


                /* try each of the import paths in turn to find the import file */
                directoryListPtr = globalImportDirectoryList;
                while(directoryListPtr && directoryListPtr->string) {

                    /* import the file */
                    memset(lineBuf, 0, MYBUFSIZE );
                    strcpy(lineBuf, directoryListPtr->string);
#ifndef SR_WINDOWS
                    strcat(lineBuf, "/");
#else
                    strcat(lineBuf, "\\");
#endif /* SR_WINDOWS */
                    strcat(lineBuf, importString);
                    strcat(lineBuf, ".my");


                    /* ************************** */
                    fileToImportPtr = strdup(lineBuf);

                    /* do import parse */
                    parseReturnValue = parse_file(IMPORT_PARSE, fileToImportPtr, importString);


/* IMPORTS EVERY DEF IN IMPORTED MIB */


                    if(fileToImportPtr) free(fileToImportPtr);
                    fileToImportPtr = NULL;
                    /* ************************** */


                    /* if we have successfully parsed the file, then quit looking */
                    if(!parseReturnValue) {
                        break;
                    }

                    directoryListPtr = directoryListPtr->next_tree_node;
                }

            } 


            /* look at the symbols we need to import from this MIB */
            /* point to symbol import list */
            nextSymbolToImportPtr = mibToImport->next_symbol_node;
        
            /* don't bother if no symbols to import */
            if(nextSymbolToImportPtr) {

                fprintf(outputFilePtr, "\n-- %s importing FROM %s\n", fileDoingImport, importString);
                fprintf(outputFilePtr, "-- looking for: ");

                /* for each of the imported symbols */
                nextImportListPtr = mibToImport->next_symbol_node;
                while(nextImportListPtr) {
                    fprintf(outputFilePtr, "%s, ", nextImportListPtr->string);
                    nextImportListPtr = nextImportListPtr->next_symbol_node;
                }
                fprintf(outputFilePtr, "\n");

                /* for each of the imported symbols */
                while(nextSymbolToImportPtr) {

                    typeExistsPtr = find_type(nextSymbolToImportPtr->string);

                    /* type exists in type table */
                    if(typeExistsPtr) {

                        /* check whether already emitted for this parse */
                        alreadyEmittedPtr = lookup_name_in_emitted_nodes(nextSymbolToImportPtr->string, typeExistsPtr->type);

                        /* printf("4150 checking %s\n", nextSymbolToImportPtr->string); */

                        if(alreadyEmittedPtr) {

                            fprintf(outputFilePtr, "-- %s already emitted\n", nextSymbolToImportPtr->string);
                            /* printf("4154 %s already emitted\n", nextSymbolToImportPtr->string); */

                        /* emit */
                        } else {

                            /* emit type and all parents back to root to output file */
                            while(typeExistsPtr) {

                                /* trace def back to root */
                                if(typeExistsPtr && typeExistsPtr->child[child_value____9] && typeExistsPtr->child[child_value____9]->string) {

                                    typeExistsPtr = find_type(typeExistsPtr->child[child_value____9]->string);
                                } else {
                                    typeExistsPtr = NULL;
                                }

                                alreadyEmittedPtr = NULL;
                                /* check whether already emitted for this parse */
                                if(typeExistsPtr && typeExistsPtr->child[0] && typeExistsPtr->child[0]->string) {
                                    alreadyEmittedPtr = lookup_name_in_emitted_nodes(typeExistsPtr->child[0]->string, typeExistsPtr->type);
                                }

                                if(alreadyEmittedPtr) {
                                    typeExistsPtr = NULL;
                                } 

                            }

                        }

                    } else {
                        fprintf(outputFilePtr, "-- %s not found\n", nextSymbolToImportPtr->string);
                    }

                    nextSymbolToImportPtr = nextSymbolToImportPtr->next_symbol_node;
                }

        
            } /* end next symbol to import */

            /* if automatic update/replace feature turned on */
            if(updateImportsFlag) {
                if(tmpTreePtr) {
                    tmpTreePtr = tmpTreePtr->next_tree_node;
                    if(tmpTreePtr) {
                        importString = tmpTreePtr->string;
                    } else {
                        importString = NULL;
                    }

                } else {
                    importString = NULL;
                }
            } else {
                importString = NULL;
            }

        } /* end while(importString) */

        mibToImport = mibToImport->next_tree_node;


    } /* end while(mibToImport) */

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================= FREE =============================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* frees list of tree nodes that we have allocated */

void free_all_nodes(tree * my_singleParseNodeList)
{
    tree * nextNode, * ptr;

    /* safety catch */
    if(!my_singleParseNodeList) return;

    ptr = my_singleParseNodeList;

    while(ptr) {

        nextNode = ptr->freePtr;

        if( ptr->string && ptr->freeMe) {
            free(ptr->string);
            ptr->string = NULL;
        }

        /* free enumerationTable if there is one */
        if(ptr->enumeratedItems) {
            free(ptr->enumeratedItems);
            ptr->enumeratedItems = NULL;
        }

        if(ptr) {
            ptr->freePtr = NULL;
            free(ptr);
            ptr = NULL;
        }

        ptr = nextNode;

    }

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* =================== ERROR HANDLING ========================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* YYERROR:  yyerror is required by bison/yacc for reporting errors */
void 
yyerror (char * errmsg)
{
    int temp_importParseFlag;

    no_code_flag = 1;

    /* temporarily turn off importParseFlag if on */
    temp_importParseFlag = importParseFlag;
    importParseFlag = 0;

    print_warning(errmsg, lineBuf);

    if(stringWarnOn) {
        print_string_warning();
        stringWarnOn = 0;
    }

    importParseFlag = temp_importParseFlag;

}

/* ============================================================ */

/* 
 * This prints the error message, the line number and 
 * contents, then the ^ pointer to the error.
 */

void 
print_warning(char * my_msg, char * mybuf)
{
    int i, skipDis = 0;
    char * ptr;


    /* if import parse don't show msgs */
    if(importParseFlag) return;

    /* if suppressOutput turned on don't print */
    if(globalSuppressOutputFlag && !no_code_flag) return;

    if(line_cntr < 10) {
        skipDis = 1;
    } else if(line_cntr < 100) {
        skipDis = 2;
    } else if(line_cntr < 1000) {
        skipDis = 3;
    } else if(line_cntr < 10000) {
        skipDis = 4;
    } else if(line_cntr < 100000) {
        skipDis = 5;
    }

    /* print message */
    fprintf(stderr, "\nReference module \"%s\" in file \"%s\"\n", lastModuleName, lastFileParsed);
    fprintf(stderr, "%s\n", my_msg);

    /* print line number */
    fprintf(stderr, "line: %d ", line_cntr);

    /* skip newlines at beginning of text */

    ptr = mybuf;
    while(*ptr == '\n' || *ptr == '\r') {
        ptr++;
    }

    /* print contents of line */
    fprintf(stderr, "%s\n", ptr);

    /* point to error with ^ */
    for(i = 0 ; i < (col_cntr + 6 + skipDis) ; i++){
        fprintf(stderr, " ");
    }
    fprintf(stderr, "^\n");

}

/* ============================================================ */

/* 
 * This prints the error message, the line number and 
 * contents, then the ^ pointer to the error.
 */

void print_string_warning()
{
    int i, skipDis = 0;
    char * ptr;

    /* if suppressOutput turned on don't print */
    if(globalSuppressOutputFlag) return;

    if(string_line_cntr < 10) {
        skipDis = 1;
    } else if(string_line_cntr < 100) {
        skipDis = 2;
    } else if(string_line_cntr < 1000) {
        skipDis = 3;
    } else if(string_line_cntr < 10000) {
        skipDis = 4;
    } else if(string_line_cntr < 100000) {
        skipDis = 5;
    }

    /* print message */
    /* fprintf(stderr, "\nReference module \"%s\" in file \"%s\"\n", lastModuleName, inputFileName); */
    fprintf(stderr, "Possible beginning of unterminated string.\n");

    /* print line number */
    fprintf(stderr, "line: %d ", string_line_cntr);

    /* skip newlines at beginning of text */
    ptr = firstLineBuf;
    while(*ptr == '\n' || *ptr == '\r') {
        ptr++;
    }

    /* print contents of line */
    fprintf(stderr, "%s\n", ptr);

    /* point to error with ^ */
    for(i = 0 ; i < (col_cntr + 6 + skipDis) ; i++){
        fprintf(stderr, " ");
    }
    fprintf(stderr, "^\n");

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== MOSY EMIT ============================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

void 
write_header(FILE * outputFilePtr)
{
    int i;

    /* get current time */
    myclock = time(0);

    fprintf(outputFilePtr, "-- ");
    for(i = 0 ; i < ((4*maxColumnWidth)+8) ; i++) {
        fprintf(outputFilePtr, "=");
    }   
    fprintf(outputFilePtr, "\n");

    fprintf(outputFilePtr, "-- automatically generated by mibgc\n");
    fprintf(outputFilePtr, "-- ");
    fprintf(outputFilePtr, "%s", (char *)ctime(&myclock));
    fprintf(outputFilePtr, "-- do not edit!\n");

    fprintf(outputFilePtr, "-- ");
    for(i = 0 ; i < ((4*maxColumnWidth)+8) ; i++) {
        fprintf(outputFilePtr, "=");
    }   
    fprintf(outputFilePtr, "\n");
    fprintf(outputFilePtr, "--\n");
    fprintf(outputFilePtr, "-- DIRECTIVES\n");
    fprintf(outputFilePtr, "-- %%defval  - default value, multiple BITS values are ORed together\n");
    fprintf(outputFilePtr, "-- %%ea      - augmentation\n");
    fprintf(outputFilePtr, "-- %%ei      - index clause, IMPLIED values are prefixed with *\n");
    fprintf(outputFilePtr, "-- %%er      - range for INTEGER or OCTET STRING\n");
    fprintf(outputFilePtr, "-- %%es      - syntax - multiple types in SYNTAX clause\n");
    fprintf(outputFilePtr, "-- %%ev      - enumerated values\n");
    fprintf(outputFilePtr, "-- %%n0      - OBJECT IDENTIFIER registration\n");
    fprintf(outputFilePtr, "-- %%tc      - Textual Convention definition\n");
    fprintf(outputFilePtr, "-- %%trap    - traps and notifications\n");
    fprintf(outputFilePtr, "--\n");

    fprintf(outputFilePtr, "-- FIELDS\n");
    fprintf(outputFilePtr, "-- Object Descriptor - name of the object\n");
    fprintf(outputFilePtr, "-- Group Entry       - MIB object group and the location of the MIB variable in the group\n");
    fprintf(outputFilePtr, "-- Syntax            - type of the object - \n");
    fprintf(outputFilePtr, "--                     INTEGER, OctetString, DisplayString, object-id, Aggregate\n");
    fprintf(outputFilePtr, "--                     IpAddress, Counter, Gauge, TimeTicks, Opaque\n");
    fprintf(outputFilePtr, "-- Access            - access permissions of the object \n");
    fprintf(outputFilePtr, "--                     read-only, read-write, write-only, not-accessible\n");
    fprintf(outputFilePtr, "-- Status            - RFC1213 compliance status of the object\n");
    fprintf(outputFilePtr, "--                     mandatory, optional, deprecated, obsolete\n");
    fprintf(outputFilePtr, "--\n");

    /*
    fprintf(outputFilePtr, "-- Note that mosy (unlike mibgc) doesn't handle BITS.\n");
    fprintf(outputFilePtr, "-- Mosy also fails to include all SIZE restrictions.\n\n");
    */

    fprintf(outputFilePtr, "-- ");
    for(i = 0 ; i < ((4*maxColumnWidth)+8) ; i++) {
        fprintf(outputFilePtr, "=");
    }   

    fprintf(outputFilePtr, "\n-- ");
    fprintf(outputFilePtr, "Object Descriptor ");
    for(i = 0 ; i < (maxColumnWidth - 20) ; i++) {
        fprintf(outputFilePtr, " ");
    }   

    fprintf(outputFilePtr, "Group Entry           ");
    for(i = 0 ; i < (maxColumnWidth - 21) ; i++) {
        fprintf(outputFilePtr, " ");
    }   

    fprintf(outputFilePtr, "Syntax ");
    for(i = 0 ; i < (maxColumnWidth - 6) ; i++) {
        fprintf(outputFilePtr, " ");
    }   

    fprintf(outputFilePtr, "Access ");
    for(i = 0 ; i < (maxColumnWidth - 6) ; i++) {
        fprintf(outputFilePtr, " ");
    }   

    fprintf(outputFilePtr, " Status\n");

    fprintf(outputFilePtr, "-- ");
    for(i = 0 ; i < ((4*maxColumnWidth)+8) ; i++) {
        fprintf(outputFilePtr, "=");
    }   

    fprintf(outputFilePtr, "\n");

}

/* ============================================================ */

void 
mosy_print_enumerated_range(tree * ptr, tree * rangelistPtr, FILE * outPtr) {

    int i;

    /* safety catch */
    if(!ptr || !ptr->child[child_identity_0] || !ptr->child[child_identity_0]->string) return; 

    while(rangelistPtr) {
        if(rangelistPtr->type == DOT_DOT) {

            /* column 1 */
            fprintf(outPtr, "%%er ");

            for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                fprintf(outPtr, " ");
            }   

            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string){

                /* column 2 */
                fprintf(outPtr, "%s ", ptr->child[child_identity_0]->string);

                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                    fprintf(outPtr, " ");
                }   

            }

            /* is there a start range child */
            if(rangelistPtr->child[chld_strt_rnge12] && rangelistPtr->child[chld_strt_rnge12]->string){

                memset((void *)fileNameBuffer, 0, MYBUFSIZE );
                convert_ul_to_string(rangelistPtr->child[chld_strt_rnge12]->number, fileNameBuffer);

                if(rangelistPtr->child[chld_strt_rnge12]->type == NEG_NUMBER) {

                    /* column 3 */
                    fprintf(outPtr, "-%lu ", rangelistPtr->child[chld_strt_rnge12]->number);

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(fileNameBuffer) - 1) ; i++) {
                        fprintf(outPtr, " ");
                    }

                } else {

                    /* column 3 */
                    fprintf(outPtr, "%lu ", rangelistPtr->child[chld_strt_rnge12]->number);

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(fileNameBuffer)) ; i++) {
                        fprintf(outPtr, " ");
                    }

                }

   
            /* if there is only one number for the range it is printed twice */
            } else {

                if(rangelistPtr->child[chld_end_range13] && rangelistPtr->child[chld_end_range13]->string){

                    memset((void *)fileNameBuffer, 0, MYBUFSIZE );
                    convert_ul_to_string(rangelistPtr->child[chld_end_range13]->number, fileNameBuffer);

                    if(rangelistPtr->child[chld_end_range13]->type == NEG_NUMBER) {

                        /* column 3 */
                        fprintf(outPtr, "-%lu ", rangelistPtr->child[chld_end_range13]->number);

                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(fileNameBuffer) - 1) ; i++) {
                            fprintf(outPtr, " ");
                        }

                    } else {
                        
                        /* column 3 */
                        fprintf(outPtr, "%lu ", rangelistPtr->child[chld_end_range13]->number);

                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(fileNameBuffer)) ; i++) {
                            fprintf(outPtr, " ");
                        }

                    }


                }


            }

            /* is there an end range child */
            if(rangelistPtr->child[chld_end_range13] && rangelistPtr->child[chld_end_range13]->string){

                /* column 4 */
                if(rangelistPtr->child[chld_end_range13]->type == NEG_NUMBER) {
                    fprintf(outPtr, " -%lu\n", rangelistPtr->child[chld_end_range13]->number);
                } else {
                    fprintf(outPtr, " %lu\n", rangelistPtr->child[chld_end_range13]->number);
                }

            }

        }

        /* get next rangelist */
        rangelistPtr = rangelistPtr->next_tree_node;
    }

}

/* ============================================================ */

void 
mosy_emit_all_nodes(tree * ptr, FILE * outputFilePtr, FILE * descrFilePtr)
{
    int i;

    /* safety catch */
    if(!ptr)         return;

    mosy_emit_node(ptr, outputFilePtr,descrFilePtr);

    for(i = 0 ; i < MAX_CHILDREN ; i++)
        if(ptr->child[i])
            mosy_emit_all_nodes(ptr->child[i], outputFilePtr, descrFilePtr);

    if(ptr->next_tree_node) 
        mosy_emit_all_nodes(ptr->next_tree_node, outputFilePtr, descrFilePtr);
}

/* ============================================================ */

void 
mosy_emit_node(tree * ptr, FILE * outputFilePtr, FILE * descrFilePtr)
{
    int i = 0, jmpDis = 0;
    int tmpValue = 0;

    char * tmpCharPtr = NULL;

    tree * tmpPtr = NULL;
    tree * lookupResult = NULL;
    tree * alreadyEmittedPtr;


    /* safety catch */
    if(!ptr) return;


    /* check whether already emitted for this parse */
    if(ptr->child[0] && ptr->child[0]->string) {

        alreadyEmittedPtr = lookup_name_in_emitted_nodes(ptr->child[0]->string, ptr->type);

        if(!alreadyEmittedPtr) {
            insert_name_into_emitted_nodes(ptr->child[0]->string, ptr->type);
        } else {
            return;

        }

    }


    /* trace type definitions back to root */
    /* check_for_type_definition(ptr , outputFilePtr, descrFilePtr); */

    if(ptr->child[child_syntax___1]) {
        check_for_type_definition(ptr->child[child_syntax___1] , outputFilePtr, descrFilePtr);
    }

    if(ptr->parent && ptr->parent->child[child_syntax___1]) {
        check_for_type_definition(ptr->child[child_syntax___1] , outputFilePtr, descrFilePtr);
    }


    switch(ptr->type) {

        case DEFVAL:

            /* DEFVAL*/
            if( ptr->parent && ptr->parent->child[child_syntax___1] && ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                /* BITS values must be ORed, lookup values in enumeratedItems table and OR */
                if(ptr->parent->child[child_syntax___1]->type == BITS) {

                    tmpValue = 0;

                    tmpPtr = ptr->child[child_identity_0];
                    while(tmpPtr) {
            
                        if(tmpPtr->string) {

                            if(ptr->parent->child[child_syntax___1]->enumeratedItems) {

                                lookupResult = lookup_name(tmpPtr->string, ptr->parent->child[child_syntax___1]->enumeratedItems);

                                /* bitwise OR converted number with previous tmpValue */
                                if(lookupResult) {
                                    tmpValue = tmpValue | atoi(lookupResult->child[child_identity_0]->string);
                                }

                            }
                        }

                        tmpPtr = tmpPtr->next_tree_node;
                    }


                    /* column 1 */
                    fprintf(outputFilePtr, "%%defval ");

                    for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){

                        /* column 2 */
                        fprintf(outputFilePtr, "%s ", ptr->parent->child[child_identity_0]->string);

                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }

                    }   

                    /* column 4 */
                    fprintf(outputFilePtr, "%d", tmpValue);


                /* OCTET STRING type */
                } else if(ptr->parent->child[child_syntax___1]->type == OCTET) {

                    /* column 1 */
                    fprintf(outputFilePtr, "%%defval ");

                    for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){
            
                        /* column 2 */
                        fprintf(outputFilePtr, "%s", ptr->parent->child[child_identity_0]->string);
                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }   

                    }

                    /* column 3 */
                    fprintf(outputFilePtr, " %s", ptr->child[child_identity_0]->string);
                    jmpDis = strlen(ptr->child[child_identity_0]->string);

                } else {

                    /* column 1 */
                    fprintf(outputFilePtr, "%%defval ");

                    for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){

                        /* column 2 */
                        if(ptr->parent->child[child_identity_0]->type == NEG_NUMBER) {
                            fprintf(outputFilePtr, "-%s ", ptr->parent->child[child_identity_0]->string);
                            for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)-1) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   
                        } else {
                            fprintf(outputFilePtr, "%s ", ptr->parent->child[child_identity_0]->string);
                            for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   
                        }

                    }

                    /* column 3 */
                    jmpDis = print_value_list(ptr->child[child_identity_0], outputFilePtr);

                }

                fprintf(outputFilePtr, "\n");

                check_for_type_definition(ptr->child[child_identity_0] , outputFilePtr, descrFilePtr);
            } 

            break;

        case DESCRIPTION:

            /* do we need to produce a description file */
            if(globalProduceDescrFlag && ptr->parent && ptr->parent->child[child_value____9] && ptr->parent->child[child_value____9]->string && ptr->string) {

                jmpDis = print_value_list(ptr->parent->child[child_value____9], descrFilePtr);

                memset(lineBuf, 0, MYBUFSIZE );
                strcpy(lineBuf, ptr->string);

                fprintf(descrFilePtr, "   \"");
                    
                tmpCharPtr = strtok(lineBuf, " \t\n\f\r");

                while(tmpCharPtr) {

                    /* print it, find next token */ 
                    fprintf(descrFilePtr, "%s ", tmpCharPtr);

                    tmpCharPtr = strtok(NULL, " \t\n\f\r");

                }    

                fprintf(descrFilePtr, "\"\n");

            }

            break;

        /* module definition */
        case DEFINITIONS:

            /* child 0 */
            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {
                fprintf(outputFilePtr, "\n-- object definitions compiled from %s\n\n", ptr->child[child_identity_0]->string);
            }

            break;

        /* this should be SMIv1 */
        case TRAP_TYPE:

            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                fprintf(outputFilePtr, "%%trap ");

                /* print name */
                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                /* print object part */
                if(ptr->child[child_object___5] && ptr->child[child_object___5]->string) {

                    if(ptr->child[child_object___5]->type == NEG_NUMBER) {
                        fprintf(outputFilePtr, "-%s", ptr->child[child_object___5]->string);
                    } else {
                        fprintf(outputFilePtr, "%s", ptr->child[child_object___5]->string);
                    }

                    tmpPtr = ptr->child[child_object___5]->next_tree_node;
                    while(tmpPtr) {
                        if(tmpPtr->string) fprintf(outputFilePtr, ".%s", tmpPtr->string);
                        tmpPtr = tmpPtr->next_tree_node;
                    }

                }

                /* print value */
                if(ptr->child[child_value____9] && ptr->child[child_value____9]->string) {

                    /* child 1 and siblings */

                    tmpPtr = ptr->child[child_value____9];
                    while(tmpPtr->next_tree_node) {
                        if(tmpPtr->string) fprintf(outputFilePtr, " %s", tmpPtr->string);
                        tmpPtr = tmpPtr->next_tree_node;
                        fprintf(outputFilePtr, ",");
                    }

                    if(tmpPtr->string) fprintf(outputFilePtr, " %s", tmpPtr->string);

                }

                fprintf(outputFilePtr, "\n");

            }

            break;

        case STRUCT:
        case UNION:

            /* print name and value */
            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                /* column 1 */
                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                /* print value */
                if(ptr->child[child_value____9]) {

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    jmpDis = print_value_list(ptr->child[child_value____9], outputFilePtr);

                }

                /* column 2 */
                /* print Struct or Union */
                if(ptr->string) {

                    for(i = 0 ; i < (maxColumnWidth - jmpDis + 1) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    fprintf(outputFilePtr, "%s ", ptr->string);

                }

                /* column 3 */
                /* print access */
                if(ptr->child[child_access__11] && ptr->child[child_access__11]->string){

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_access__11]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    fprintf(outputFilePtr, " %s ", ptr->child[child_access__11]->string);

                }

                /* column 4 */
                /* print status */
                if(ptr->child[child_status___2] && ptr->child[child_status___2]->string){

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_status___2]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    fprintf(outputFilePtr, "%s\n", ptr->child[child_status___2]->string);
                }

                check_for_type_definition(ptr->child[child_value____9] , outputFilePtr, descrFilePtr);

            }

            break;

        /* this should be SMIv2 */
        case NOTIFICATION_TYPE:

            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                fprintf(outputFilePtr, "%%trap ");

                /* print name */
                fprintf(outputFilePtr, "%s -1 ", ptr->child[child_identity_0]->string);

                /* print value */
                if(ptr->child[child_value____9] && ptr->child[child_value____9]->string) {

                    if(ptr->child[child_value____9]->type == NEG_NUMBER) {
                        fprintf(outputFilePtr, "-%s", ptr->child[child_value____9]->string);
                    } else {
                        fprintf(outputFilePtr, "%s", ptr->child[child_value____9]->string);
                    }

                    tmpPtr = ptr->child[child_value____9]->next_tree_node;
                    while(tmpPtr) {
                        if(tmpPtr->string) fprintf(outputFilePtr, ".%s", tmpPtr->string);
                        tmpPtr = tmpPtr->next_tree_node;
                    }

                }

                /* is there an objectPart */
                if(ptr->child[child_object___5] && ptr->child[child_object___5]->string) {

                    /* child 1 and siblings */
                    fprintf(outputFilePtr, " { ");

                    tmpPtr = ptr->child[child_object___5];
                    while(tmpPtr->next_tree_node) {
                        if(tmpPtr->string) fprintf(outputFilePtr, " %s", tmpPtr->string);
                        tmpPtr = tmpPtr->next_tree_node;
                        fprintf(outputFilePtr, ",");
                    }

                    if(tmpPtr->string) fprintf(outputFilePtr, " %s", tmpPtr->string);

                    fprintf(outputFilePtr, " }");

                }

                fprintf(outputFilePtr, "\n");

            }
           
        case AGENT_CAPABILITIES:
        case MODULE_IDENTITY:       /* module identification definition */
        case IDENTIFIER:            /* OBJECT IDENTIFIER */
        case MODULE_COMPLIANCE:
        case MODULE_CONFORMANCE:
        case NOTIFICATION_GROUP:
        case OBJECT_GROUP:
        case OBJECT_IDENTITY:

            /* print name and value */
            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string && ptr->child[child_value____9] && ptr->child[child_value____9]->string) {

                /* column 1 */
                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                /* column 2 */
                /* print value */
                if(ptr->child[child_value____9]) {

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    jmpDis = print_value_list(ptr->child[child_value____9], outputFilePtr);

                }

                if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                    /* column 1 */
                    fprintf(outputFilePtr, "\n%%n0 ");

                    for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    /* column 2 */
                    fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    /* column 3 */
                    if(ptr->string) {
                        fprintf(outputFilePtr, "%s\n", ptr->string);
                    }

                }

                check_for_type_definition(ptr->child[child_value____9] , outputFilePtr, descrFilePtr);

            }


            break;

        case OBJECT_TYPE:

            check_for_range_restriction_on_type(ptr, outputFilePtr);

            if(ptr->child[child_value____9] && ptr->child[child_value____9]->string) {
                check_for_type_definition(ptr->child[child_value____9] , outputFilePtr, descrFilePtr);
            }

            lookupResult = NULL;

            /* print name */
            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                /* print value */
                if(ptr->child[child_value____9] && ptr->child[child_value____9]->string) {

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    jmpDis = print_value_list(ptr->child[child_value____9], outputFilePtr);

                }

                for(i = 0 ; i < (maxColumnWidth - jmpDis) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   



            }

            /* SEQUENCE OF, virtual table */
            if(ptr->child[child_syntax___1] && ptr->child[child_syntax___1]->type == OF) {

                fprintf(outputFilePtr, " Aggregate ");

                jmpDis = 9;

                for(i = 0 ; i < (maxColumnWidth - jmpDis) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   

            } else {

                /* is this a new named aggregate type */
                if(ptr->child[child_syntax___1] && ptr->child[child_syntax___1]->string) {

                    lookupResult = lookup_type(ptr->child[child_syntax___1]->string, virtualTables);

                    if(lookupResult) {

                        fprintf(outputFilePtr, "%s", " Aggregate ");
                        jmpDis = 9;

                        /* reset */
                        lookupResult = NULL;

                    } else {

                        /* if it is an imported type, type is 0, so lookup in table */
                        if(!ptr->child[child_syntax___1]->type) {

                                lookupResult = lookup_type(ptr->child[child_syntax___1]->string, singleParseTypeTable);

                                if(lookupResult) {

                                    if(lookupResult->child[child_identity_0] && lookupResult->child[child_identity_0]->string) {

                                        if(lookupResult->child[child_identity_0]->type == NEG_NUMBER) {
                                            fprintf(outputFilePtr, " -%s ", lookupResult->child[child_identity_0]->string);
                                            jmpDis = (int)strlen(lookupResult->child[child_identity_0]->string)+1;
                                        } else {
                                            fprintf(outputFilePtr, " %s ", lookupResult->child[child_identity_0]->string);
                                            jmpDis = (int)strlen(lookupResult->child[child_identity_0]->string);
                                        }

                                    }

                                } else {

                                    if(ptr->child[child_syntax___1]->string) {
                                        fprintf(outputFilePtr, " %s ", ptr->child[child_syntax___1]->string);
                                        jmpDis = (int)strlen(ptr->child[child_syntax___1]->string);
                                    }

                                } 

                        } else {

                            fprintf(outputFilePtr, " %s ", ptr->child[child_syntax___1]->string);
                            jmpDis = (int)strlen(ptr->child[child_syntax___1]->string);


                        }

                    }

                    for(i = 0 ; i < (maxColumnWidth - jmpDis) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                }

            }

            /* access */
            if(ptr->child[child_access__11] && ptr->child[child_access__11]->string){

                fprintf(outputFilePtr, " %s", ptr->child[child_access__11]->string);

                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_access__11]->string)) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   
            
            }

            if(ptr->child[child_status___2] && ptr->child[child_status___2]->string){
                fprintf(outputFilePtr, " %s\n", ptr->child[child_status___2]->string);
            }




/* ======================================= */

/* enumerated range, RangeList is always child[child_range____6]  */
if( lookupResult && lookupResult->child[child_identity_0] && lookupResult->child[child_identity_0]->child[child_range____6] ) {
    mosy_print_enumerated_range(ptr, lookupResult->child[child_identity_0]->child[child_range____6], outputFilePtr);
}

/* ======================================= */

            /* INDEX or AUGMENTS*/
            if( ptr->child[child_index____7] ) {

                /* INDEX */
                if( ptr->child[child_index____7]->type == INDEX) {
                    fprintf(outputFilePtr, "%%ei ");

                    for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    /* print name */
                    if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                        fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                        tmpPtr = ptr->child[child_index____7]->child[child_identity_0];

                        /* do we have at least one item? */
                        if(tmpPtr) {

                            for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   

                            /* print opening quote */
                            fprintf(outputFilePtr, "\"");

                            while(tmpPtr->next_tree_node && tmpPtr->string) {
                                /* print each item with a space after it */
                
                                if(tmpPtr->type == IMPLIED){
                                    fprintf(outputFilePtr, "*%s ", tmpPtr->string);
                                } else {
                                    fprintf(outputFilePtr, "%s ", tmpPtr->string);
                                }

                                tmpPtr = tmpPtr->next_tree_node;
                            }

                            /* print last item without a space after it */
                            if(tmpPtr->string){
                                if(tmpPtr->type == IMPLIED){
                                    fprintf(outputFilePtr, "*%s", tmpPtr->string);
                                } else {
                                    fprintf(outputFilePtr, "%s", tmpPtr->string);
                                }
                            }

                            /* print closing quote */
                            fprintf(outputFilePtr, "\"");

                        }

                    }

                    fprintf(outputFilePtr, "\n");

                /* AUGMENTS */
                } else if( ptr->child[child_index____7]->type == AUGMENTS) {

                    fprintf(outputFilePtr, "%%ea ");

                    for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    /* print name */
                    if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {
                        fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);
            
                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }   

                    }

                    /* print name */
                    if(ptr->child[child_index____7]->string) {
                        fprintf(outputFilePtr, "%s ", ptr->child[child_index____7]->string);

                    }

                    fprintf(outputFilePtr, "\n");

                }

            }

/* ======================================= */


            if( ptr->child[child_identity_0] && ptr->child[child_identity_0]->string && ptr->child[child_syntax___1] &&  ptr->child[child_syntax___1]->child[child_status___2] ) {

                /* enumerated values , NameItemList is always child[child_status___2] */

                if( (ptr->child[child_syntax___1]->type == BITS) || (ptr->child[child_syntax___1]->type == BIT) ) {

                    /* named item list */
                    tmpPtr = ptr->child[child_syntax___1]->child[child_status___2];
                    while(tmpPtr) {
                        if(tmpPtr->child[child_identity_0]) {

                            fprintf(outputFilePtr, "%%ev ");

                            for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   

                            if(ptr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            } else {
                                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            }

                            if(tmpPtr->string) {

                                if(tmpPtr->type == NEG_NUMBER) {
                                    fprintf(outputFilePtr, "-%s ", tmpPtr->string);
                                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)-1) ; i++) {
                                        fprintf(outputFilePtr, " ");
                                    }   
                                } else {
                                    fprintf(outputFilePtr, "%s ", tmpPtr->string);
                                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)) ; i++) {
                                        fprintf(outputFilePtr, " ");
                                    }   
                                }

                            }

                            if(tmpPtr->child[child_identity_0]->string) {
                                fprintf(outputFilePtr, " %s\n", tmpPtr->child[child_identity_0]->string);
                            }


                        }
                        tmpPtr = tmpPtr->next_tree_node;
                    }


                /* enumerated syntax, NamedBitListPlugin is always child[child_status___2] */
                } else {

                    tmpPtr = ptr->child[child_syntax___1]->child[child_status___2];
                    while(tmpPtr) {
                        if(tmpPtr->child[child_identity_0] && tmpPtr->child[child_identity_0]->string && tmpPtr->string)

                            fprintf(outputFilePtr, "%%es ");

                            for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }
   
                            if(ptr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            } else {
                                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            }

                            if(tmpPtr->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", tmpPtr->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            } else {
                                fprintf(outputFilePtr, "%s ", tmpPtr->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            }


                            if(tmpPtr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, " -%s\n", tmpPtr->child[child_identity_0]->string);
                            } else {
                                fprintf(outputFilePtr, " %s\n", tmpPtr->child[child_identity_0]->string);
                            }

                        tmpPtr = tmpPtr->next_tree_node;
                    }

                }

            }

/* ======================================= */

/* enumerated range, RangeList is always child[child_range____6]  */
if( ptr->child[child_syntax___1] &&  ptr->child[child_syntax___1]->child[child_range____6] ) {
    mosy_print_enumerated_range(ptr, ptr->child[child_syntax___1]->child[child_range____6], outputFilePtr);
}

/* ======================================= */


/* ======================================= */

            break;


        case TEXTUAL_CONVENTION:

            /* child 0 */
            if(ptr->child[child_identity_0] && ptr->child[child_syntax___1] && ptr->child[child_syntax___1]->string && ptr->child[child_identity_0]->string) {

                fprintf(outputFilePtr, "%%tc ");

                for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   

                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   

                if(ptr->child[child_syntax___1] && ptr->child[child_syntax___1]->string) {

                    fprintf(outputFilePtr, "%s ", ptr->child[child_syntax___1]->string);

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_syntax___1]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    } 

                    fprintf(outputFilePtr, " \"");


                }

                /* display hint */
                if(ptr->child[child_display__8] && ptr->child[child_display__8]->string)
                    fprintf(outputFilePtr, "%s", ptr->child[child_display__8]->string);

                fprintf(outputFilePtr, "\"\n");

            }

            /* enumerated range , RangeList is always child[child_range____6] */
            if( ptr->child[child_syntax___1] &&  ptr->child[child_syntax___1]->child[child_range____6] ) {
                mosy_print_enumerated_range(ptr, ptr->child[child_syntax___1]->child[child_range____6], outputFilePtr);
            }

            /* enumerated syntax, NamedBitListPlugin is always child[child_status___2] */
            if( ptr->child[child_identity_0] && ptr->child[child_identity_0]->string && ptr->child[child_syntax___1] &&  ptr->child[child_syntax___1]->child[child_status___2] ) {

                tmpPtr = ptr->child[child_syntax___1]->child[child_status___2];
                while(tmpPtr) {

                    if(tmpPtr && tmpPtr->child[child_identity_0] && tmpPtr->child[child_identity_0]->string && tmpPtr->string) {

                        fprintf(outputFilePtr, "%%es ");
                        for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }   

                        if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                            if(ptr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }
                            } else {
                                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }
                            }

                        }   

                        if(tmpPtr->string) {

                            if(tmpPtr->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", tmpPtr->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            } else {
                                fprintf(outputFilePtr, "%s ", tmpPtr->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            }

                        }

                        if(tmpPtr->child[child_identity_0] && tmpPtr->child[child_identity_0]->string) {

                            if(tmpPtr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, " -%s\n", tmpPtr->child[child_identity_0]->string);
                            } else {
                                fprintf(outputFilePtr, " %s\n", tmpPtr->child[child_identity_0]->string);
                            }

                        }

                    }

                    tmpPtr = tmpPtr->next_tree_node;
                }

            }

            break;

        default:
            break;

    }

}

/* ============================================================ */

void 
check_for_type_definition(tree * myPtr, FILE * myOutputFilePtr, FILE * myDescrFilePtr)
{
    tree * typeExistsPtr     = NULL;
    tree * alreadyEmittedPtr = NULL;

    /* safety catch */
    if(!myPtr || !myPtr->string) return;

    /* check whether already emitted for this parse */
    if(myPtr->child[0] && myPtr->child[0]->string) {
        alreadyEmittedPtr = lookup_name_in_emitted_nodes(myPtr->child[0]->string, myPtr->type);
    }

    if(!alreadyEmittedPtr) {

        /* find type */
        typeExistsPtr = find_type(myPtr->string);

        /* trace type back to root */
        while(typeExistsPtr && !alreadyEmittedPtr) {

            /* insert node into list of emitted nodes */
            mosy_emit_node(typeExistsPtr, myOutputFilePtr, myDescrFilePtr);

            /* trace def back to root */
            if(typeExistsPtr && typeExistsPtr->child[child_syntax___1] && typeExistsPtr->child[child_syntax___1]->string) {

                typeExistsPtr = find_type(typeExistsPtr->child[child_syntax___1]->string);

            } else {
                typeExistsPtr = NULL;
            }

            if(typeExistsPtr) {

                alreadyEmittedPtr = lookup_name_in_emitted_nodes(typeExistsPtr->child[child_syntax___1]->string, typeExistsPtr->type);

                if(alreadyEmittedPtr) {
                    typeExistsPtr = NULL;  
                }

            }

        }

    }    

}

/* ============================================================ */

void 
check_for_range_restriction_on_type(tree * myPtr, FILE * myOutputFilePtr)
{
    tree * tmpPtr;

    /* is there a range restriction associated with the type of the object */
    if(myPtr && myPtr->child[child_syntax___1] && myPtr->child[child_syntax___1]->string) {

        /* check for type definition in type table */
        tmpPtr = lookup_type(myPtr->child[child_syntax___1]->string, allParsesTypeTable);

        if(!tmpPtr) {
            tmpPtr = lookup_type(myPtr->child[child_syntax___1]->string, singleParseTypeTable);
        }

        if( tmpPtr && tmpPtr->child[0] && tmpPtr->child[0]->child[0] && tmpPtr->child[0]->child[0]->child[child_range____6] ) {
            /* enumerated range, RangeList is always child[child_range____6]  */
            mosy_print_enumerated_range(myPtr, tmpPtr->child[0]->child[0]->child[child_range____6], myOutputFilePtr);

        }

    }

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* =================== HASH TABLE ============================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* INSERT: insert node into symbol table */
/* lookup has already been used to make sure not already in table */
void 
insert_name(tree * node, tree * symboltable[] )
{
    register int location;

    /* safety catch */
    if(!node || !symboltable) return;

    /* find location to insert into symbol table */
    location  = hashpjw(node->string, TABLE_SIZE);

    /* insert at head of linked list chained at that location */
    node->next_symbol_node       = symboltable[location];
    symboltable[location] = node;

}

/* ============================================================ */

/* LOOKUP: lookup string name in symbol table */
/* will always find latest first */

tree * 
lookup_name(char * string, tree * symboltable[] )
{
    register int location, compare = 1;
    tree * entry_ptr = NULL;

    /* safety catch */
    if(!string || !symboltable) return NULL;

    location = hashpjw(string, TABLE_SIZE);
    entry_ptr = symboltable[location];

    /* traverse list to find */
    while( entry_ptr &&  entry_ptr->string && (compare = strcmp(entry_ptr->string,string)) )
        entry_ptr = entry_ptr->next_symbol_node;

    /* if not found return NULL */
    if(compare) {
        return(NULL);
    /* otherwise, return location */
    } else {
        return(entry_ptr);
    }  
}

/* ============================================================ */

tree * 
lookup_type(char * string, tree * symboltable[] )
{
    register int location, compare = 1;
    tree * entry_ptr = NULL;

    /* safety catch */
    if(!string || !symboltable || !strlen(string)) {
        return NULL;
    }

    location = hashpjw(string, TABLE_SIZE);
    entry_ptr = symboltable[location];

    /* traverse list to find */
    while( entry_ptr &&  entry_ptr->child[0] && entry_ptr->child[0]->string && (compare = strcmp(entry_ptr->child[0]->string,string)) )
        entry_ptr = entry_ptr->next_symbol_node;

    /* if not found return NULL */
    if(compare) {
        return(NULL);
    /* otherwise, return location */
    } else {
        return(entry_ptr);
    }  
}

/* ============================================================ */

void 
insert_type_node(tree * node, tree * symboltable[] )
{
    register int location;

    /* safety catch */
    if(!node || !symboltable || !node->child[0] || !node->child[0]->string ) return;

    /* if already in table, do nothing, don't overwrite */
    /* we don't want to overwrite because we load our own
       aux defs first, we don't want them overwritten by
       someone else's bogus definitions */

    if( lookup_type(node->child[0]->string, symboltable) ) {

        if(!globalSuppressOutputFlag) {
            print_warning("*** WARNING ***  Ignoring re-definition of type.", lineBuf);
        }

        return;
    }

    /* find location to insert into symbol table */
    location  = hashpjw(node->child[0]->string, TABLE_SIZE);

    /* insert at head of linked list chained at that location */
    node->next_symbol_node       = symboltable[location];
    symboltable[location] = node;

}

/* ============================================================ */

/*
 * HASHPJW FUNCTION
 *   Adds the characters in the key "s" then mods
 *   with the number of entries in 
 *   "num_entries" to produce a number that is used
 *   to index into a hash table
 *
 *   taken from page 436 of "Compilers, Principles, Techniques, and Tools",
 *   by Aho, Sethi, Ullman
 */
int 
hashpjw(char * s, int num_entries)
{
    register char * p;
    register unsigned h = 0;
    register unsigned g;

    for(p = s ; *p != '\0' ; p++) {

        h = (h << 4) + (*p);

        if((g = h&0xf0000000)) {

            h = h ^ (g >> 24);
            h = h ^ g;

        }

    }

  return h % num_entries;

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== UTILITY ============================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

tree *
shallow_clone_node(tree * origNode)
{
    tree * copyNode = NULL;
    tree * copyNodeChild = NULL;

    if(!origNode) return NULL;

    copyNode = malloc_node(copyNode, SINGLE_PARSE);
    copyNode->number = origNode->number;
    copyNode->type   = origNode->type;
    if(origNode->string) {
        copyNode->string = strdup(origNode->string);
        copyNode->freeMe = 1;
    }

    copyNodeChild = malloc_node(copyNodeChild, 1);
    copyNodeChild->number = origNode->child[0]->number;
    copyNodeChild->type   = origNode->child[0]->type;
    if(origNode->child[0]->string) {
        copyNodeChild->string = strdup(origNode->child[0]->string);
        copyNodeChild->freeMe = 1;
    }

    copyNode->child[0] = copyNodeChild;

    return copyNode;

}

/* ============================================================ */

tree *
clone_node(tree * origNode)
{
    int i;
    tree * copyNode = NULL;

    if(!origNode) return NULL;

    copyNode = malloc_node(copyNode, SAVE_FOR_ALL_PARSES);
    copyNode->number = origNode->number;
    copyNode->type   = origNode->type;
    if(origNode->string) {
        copyNode->string = strdup(origNode->string);
        copyNode->freeMe = 1;
    }

    for(i = 0 ; i < MAX_CHILDREN ; i++)
        if(origNode->child[i])
            copyNode->child[i] = clone_node(origNode->child[i]);

    if(origNode->next_tree_node) 
        copyNode->next_tree_node = clone_node(origNode->next_tree_node);

    /* table of enumerated items for object */
    /* struct tree_tag * * enumeratedItems; */

    return copyNode;

}

/* ============================================================ */

/* 
 * INT_TO_STRING: Converts a number into a string.
 * The size of s limits the size of the number to be converted.
 *
 * Adapted from page 64 of Kernighan and Ritchie
 * "The C Programming Language"
 */
void 
convert_int_to_string(int n, char s[])
{
    int i, sign, j, counter, c;

    if((sign = n) < 0) {
        n = -n;
    }

    i = 0;
    counter = 0;

    /* generate the digits in reverse order
         and place in the string s */
    do {     
        s[i++] = n % 10 + '0';
        counter++;
    } while((n/=10) > 0);

    if(sign < 0) {

        s[i++] = '-';
        counter++;
    }

    /* null terminate */
    s[i] = '\0';

    /* now reverse the string s in place */
    for(i = 0, j = counter-1; i< j;i++, j--) {

        c = s[i];
        s[i] = s[j];
        s[j] = c;

    }

}

/* ============================================================ */

void 
convert_ul_to_string(unsigned long  n, char s[])
{
    int i, j, counter, c;

    i = 0;
    counter = 0;

    /* generate the digits in reverse order
         and place in the string s */
    do {     
        s[i++] = n % 10 + '0';
        counter++;
    } while((n/=10) > 0);


    /* null terminate */
    s[i] = '\0';

    /* now reverse the string s in place */
    for(i = 0, j = counter-1; i< j;i++, j--) {

        c = s[i];
        s[i] = s[j];
        s[j] = c;

    }

}

/* ============================================================ */

int
print_value_list(tree * tmpPtr, FILE * outPtr) {

    int jmpDis = 0;

    /* first item in list */
    /* dotted identifier */
    if(tmpPtr->type == DOT) {

        if(tmpPtr->child[1] && tmpPtr->child[1]->string) {

            fprintf(outPtr, "%s", tmpPtr->child[1]->string);
            jmpDis += (int)strlen(tmpPtr->child[1]->string);

        }

    } else if(tmpPtr->type == DEFVAL) {

        if(tmpPtr->child[child_identity_0] && tmpPtr->child[child_identity_0]->string) {

            fprintf(outPtr, " %s", tmpPtr->child[child_identity_0]->string);
            jmpDis += (int)strlen(tmpPtr->child[child_identity_0]->string);

        }

    /* named item ? */
    } else if(tmpPtr->child[child_identity_0]) {

        if(tmpPtr->child[child_identity_0]->string) {

            fprintf(outPtr, "%s", tmpPtr->child[child_identity_0]->string);
            jmpDis += (int)strlen(tmpPtr->child[child_identity_0]->string);

        }
    /* normal string */
    } else if(tmpPtr->string) {

        if(tmpPtr->type == NEG_NUMBER) {

            fprintf(outPtr, "-%s", tmpPtr->string);
            jmpDis += (int)strlen(tmpPtr->string)+1;
 
        } else {

            fprintf(outPtr, "%s", tmpPtr->string);
            jmpDis += (int)strlen(tmpPtr->string);

        }

    }

    /* all other items in list */
    tmpPtr = tmpPtr->next_tree_node;

    while(tmpPtr) {

        if(tmpPtr->type == DOT) {

            if(tmpPtr->child[1] && tmpPtr->child[1]->string) {

                fprintf(outPtr, ".%s", tmpPtr->child[1]->string);
                jmpDis += (int)strlen(tmpPtr->child[1]->string) + 1;

            }

        /* named item ? */
        } else if(tmpPtr->child[child_identity_0]) {

            if(tmpPtr->child[child_identity_0]->string) {

                fprintf(outPtr, ".%s", tmpPtr->child[child_identity_0]->string);
                jmpDis += (int)strlen(tmpPtr->child[child_identity_0]->string);

            }
        } else if(tmpPtr->string) {

                fprintf(outPtr, ".%s", tmpPtr->string);
                jmpDis += (int)strlen(tmpPtr->string) + 1;

        }

        tmpPtr = tmpPtr->next_tree_node;

    }


    return jmpDis;

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== MALLOC ============================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* mallocs space, checks return, clears */
tree * 
malloc_node(tree * node, int singleParseOnly)
{
    node = (tree *)malloc(sizeof(tree));
    if(node) {
        memset((void *)node, 0, sizeof(tree));
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for new tree node!\n");
        _exit(1);
    }

    /* trees are freed at end of parse, aux-types at end of all processing */
    if(singleParseOnly) {
        node->freePtr = singleParseNodeList;
        singleParseNodeList = node;
    } else {
        node->freePtr = allParsesNodeList;
        allParsesNodeList = node;
    }

    return(node);

}

/* ============================================================ */
void
dump_table(tree * table[]) {

    int i, haveOne;
    tree * entry_ptr;

    for(i = 0 ; i < TABLE_SIZE ; i++) {

        entry_ptr = table[i];
    
        if(entry_ptr) {
            haveOne = 1;
        } else {
            haveOne = 0;
        }

        while( entry_ptr ) {

            if(entry_ptr->child[0] && entry_ptr->child[0]->string) {

                printf("%s ", entry_ptr->child[0]->string);

            }

            entry_ptr = entry_ptr->next_symbol_node;
        }

        if(haveOne) printf("\n");

    }

}

/* ============================================================ */

symbol_table * 
malloc_table(symbol_table * new_table)
{
    new_table = (symbol_table *)malloc(TABLE_SIZE*sizeof(tree *));

    if(!new_table) {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for new symbol table!\n");
        exit(1);
    }

    /* clear table */
    memset((void *)new_table, 0, TABLE_SIZE*sizeof(tree *));

    return(new_table);
    
}

/* ============================================================ */

/* reuses fileNameBuffer,
   clears old outputFileName, sets new
   sets outputFilePtr
*/

void
open_output_file(char * desiredOutputName)
{

    FILE * tmpFilePtr;
    int fileCounter = 0, rename = 0;


    memset(fileNameBuffer, 0, MYBUFSIZE);
    sprintf(fileNameBuffer, "%s", desiredOutputName);

    /* does the output file already exist? */
	while( (tmpFilePtr = fopen(fileNameBuffer, "r")) != NULL) {

        if(!globalSuppressOutputFlag) {
            printf("\n*** WARNING ***  Output file \"%s\" already exists\n\n", fileNameBuffer);
        }

        memset(fileNameBuffer, 0, MYBUFSIZE);
        sprintf(fileNameBuffer, "%s_%03d", desiredOutputName, fileCounter++);

        fclose(tmpFilePtr);
        rename = 1;

	}

    if(rename) {

        /* rename .def file to next free _??? counter */

        memset(lineBuf, 0, MYBUFSIZE );
#ifndef SR_WINDOWS
        strcpy(lineBuf, "mv ");
#else
        strcpy(lineBuf, "rename ");
#endif /* SR_WINDOWS */
        strcat(lineBuf, desiredOutputName);
        strcat(lineBuf, " ");
        strcat(lineBuf, fileNameBuffer);

        system(lineBuf);

    }


    /* now you can use the output filename you wanted */
    /* OPEN OUTPUT FILE */
    if((outputFilePtr = fopen(outputFileName, "w")) == NULL) {
        fprintf(stderr, "\n*** ERROR ***  Can't open \"%s\" for writing.\n", outputFileName);
        _exit(-1);
    } 

    if(!globalSuppressOutputFlag) {
        printf("\n*** NOTE ***  Opening output file: \"%s\".\n", outputFileName);
    }

}


/* ============================================================ */

/* first try to find type node in AUX-MIB data from allParsesTypeTable,
   if not found, look in singleParseTypeTable - where the current
   MIB has precedence over any imported MIB
*/

tree * 
find_type(char * lookupName)
{
    tree * typeExistsPtr = NULL;

    if(!lookupName) return NULL;

    /* check if aux mib has type */
    typeExistsPtr = lookup_type(lookupName, allParsesTypeTable);

    /* if not, check regular types */
    if(!typeExistsPtr) {
        typeExistsPtr = lookup_type(lookupName, singleParseTypeTable);
    }

    return typeExistsPtr;
}

/* ============================================================ */

void
insert_name_into_emitted_nodes(char * name, int type)
{
    tree * name_node = NULL, * tree_node = NULL;

    if(!name) return;

    memset(fileNameBuffer, 0, MYBUFSIZE);
    sprintf(fileNameBuffer, "%s%d", name, type);

    name_node = malloc_node(name_node, SINGLE_PARSE);
    name_node->string = strdup(fileNameBuffer);
    name_node->freeMe = 1;

    tree_node = malloc_node(tree_node, SINGLE_PARSE);
    tree_node->child[0] = name_node;

    insert_type_node(tree_node, emittedNodes);

}

/* ============================================================ */

tree *
lookup_name_in_emitted_nodes(char * name, int type)
{
    if(!name) return NULL;

    memset(fileNameBuffer, 0, MYBUFSIZE);
    sprintf(fileNameBuffer, "%s%d", name, type);
    return(lookup_type(fileNameBuffer, emittedNodes));
}


/* ============================================================ */

void
load_mib_replacement_table()
{
    int i;

    for(i = 0 ; i < NUM_MIB_REPLACEMENTS ; i++) {
        insert_mib_replacement_node(mibReplacements[i][0], mibReplacements[i][1]);
    }

}

/* ============================================================ */

void
insert_mib_replacement_node(char * oldName, char * newName)
{
    tree * tree_node  = NULL;
    tree * data_node  = NULL;
    tree * tmpTreePtr = NULL;

    /* does an entry already exist */
    tmpTreePtr = lookup_name(oldName, mibReplacementTable);

    /* node exists, add data at end */
    if(tmpTreePtr) {

        data_node = malloc_node(data_node, SAVE_FOR_ALL_PARSES);
        data_node->string = strdup(newName);
        data_node->freeMe = 1;

        tmpTreePtr->next_tree_node->last_tree_node_in_list->next_tree_node = data_node;
        tmpTreePtr->next_tree_node->last_tree_node_in_list = data_node;

    /* no node, new insert */
    } else {

        tree_node = malloc_node(tree_node, SAVE_FOR_ALL_PARSES);
        tree_node->string = strdup(oldName);
        tree_node->freeMe = 1;

        data_node = malloc_node(data_node, SAVE_FOR_ALL_PARSES);
        data_node->string = strdup(newName);
        data_node->freeMe = 1;

        tree_node->next_tree_node = data_node;

        data_node->last_tree_node_in_list = data_node;

        insert_name(tree_node, mibReplacementTable);

    }

}

/* ============================================================ */
void
dump_table1(tree * table[]) {

    int i, haveOne;
    tree * entry_ptr, * tmpPtr;

    for(i = 0 ; i < TABLE_SIZE ; i++) {

        entry_ptr = table[i];
    
        if(entry_ptr) {
            haveOne = 1;
        } else {
            haveOne = 0;
        }

        while( entry_ptr ) {

            if(entry_ptr && entry_ptr->string) {

                printf("%s: ", entry_ptr->string);

            }

            tmpPtr = entry_ptr->next_tree_node;
            while(tmpPtr ) {

                if(tmpPtr && tmpPtr->string) {

                    printf("%s: ", tmpPtr->string);

                }
                tmpPtr = tmpPtr->next_tree_node;
            }


            entry_ptr = entry_ptr->next_symbol_node;
        }

        if(haveOne) printf("\n");

    }

}

/* ============================================================ */
void
check_for_range_overlap(tree * rangelist) {

    tree * rangeNode1,  * rangeNode2;
    tree * startRange1, * startRange2;
    tree * endRange1,   * endRange2;
    tree * prevPtr;

    int retValue = 0;


    rangeNode1     = rangelist;

    while(rangeNode1) {

        startRange1 = rangeNode1->child[chld_strt_rnge12];
        endRange1   = rangeNode1->child[chld_end_range13];

        if(!startRange1) {
            startRange1 = endRange1;
        }

        rangeNode2 = rangeNode1->next_tree_node;
        while(rangeNode2) {

            startRange2 = rangeNode2->child[chld_strt_rnge12];
            endRange2   = rangeNode2->child[chld_end_range13];

            if(!startRange2) {
                startRange2 = endRange2;
            }

            /* startRange1 is positive */
            if(startRange1->type == POS_NUMBER) {

                /* endRange1 is positive */
                if(endRange1->type == POS_NUMBER) {

                    /* startRange2 is positive */
                    if(startRange2->type == POS_NUMBER) {

                        /* ++ ++ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if((startRange1->number >= startRange2->number) && (startRange1->number <= (endRange2->number+1))) {

                                /* is 1 completely contained in 2 */
                                if(endRange1->number <= endRange2->number) {

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } else if((startRange2->number >= startRange1->number) && (startRange2->number <= (endRange1->number+1))) {

                                /* is 2 completely contained in 1 */
                                if(endRange2->number <= endRange1->number) {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } 

                        } 
                        /* ++ +- endRange2 is negative - CAN'T HAPPEN*/

                    /* startRange2 is negative */
                    } else {

                        /* ++ -+ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if( startRange1->number <= (endRange2->number+1)) {

                                /* is 1 completely contained in 2 */
                                if(endRange1->number <= endRange2->number) {

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } 

                        }
                        /* ++ -- endRange2 is negative - no overlap possible*/


                    }

                }
                /* endRange1 is negative - CAN'T HAPPEN*/


            /* startRange1 is negative */
            } else {

                /* endRange1 is positive */
                if(endRange1->type == POS_NUMBER) {

                    /* startRange2 is positive */
                    if(startRange2->type == POS_NUMBER) {

                        /* -+ ++ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if( startRange2->number <= (endRange1->number+1) ) {

                                /* is 2 completely contained in 1 */
                                if(endRange2->number <= endRange1->number) {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;


                                }

                                retValue = 1;

                            } 

                        }
                        /* -+ +- endRange2 is negative  - CAN'T HAPPEN */


                    /* startRange2 is negative */
                    } else {

                        /* -+ -+ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if( startRange1->number <= startRange2->number) {

                                /* is 1 completely contained in 2 */
                                if(endRange1->number <= endRange2->number) {

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } else if( startRange2->number <= startRange1->number ) {

                                /* is 2 completely contained in 1 */
                                if(endRange2->number <= endRange1->number) {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } 


                        /* -+ -- endRange2 is negative */
                        } else {

                            if((startRange1->number <= startRange2->number) && (startRange1->number >= (endRange2->number-1))) {

                                endRange2->number = endRange1->number;
                                if(endRange2->string) {
                                    free(endRange2->string);
                                }
                                endRange2->string = strdup(endRange1->string);
                                endRange2->type = endRange1->type;

                                /* set type to null so not printed */
                                rangeNode1->type = 0;

                                retValue = 1;

                            } else if(startRange2->number <= startRange1->number ) {

                                /* 2 completely contained in 1 */

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;



                                retValue = 1;

                            } 

                        }

                    }

                /* endRange1 is negative */
                } else {

                    /* startRange2 is positive - no cases to consider */
                        /* -- ++ endRange2 is positive - no overlap possible*/
                        /* -- +- endRange2 is negative - not possible */

                    /* startRange2 is negative */
                    if(startRange2->type == NEG_NUMBER) {

                        /* -- -+ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if( startRange1->number <= startRange2->number) {

                                /* 1 completely contained in 2 */

                                /* set type to null so not printed */
                                rangeNode1->type = 0;

                                retValue = 1;

                            } else if((startRange2->number <= startRange1->number) && (startRange2->number >= (endRange1->number-1))) {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;


                                retValue = 1;

                            } 

                        /* -- -- endRange2 is negative */
                        } else {

                            if((startRange1->number <= startRange2->number) && (startRange1->number >= (endRange2->number-1))) {

                                /* is 1 completely contained in 2 */
                                if(endRange1->number >= endRange2->number) {

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } else if((startRange2->number <= startRange1->number) && (startRange2->number >= (endRange1->number-1))) {

                                /* is 2 completely contained in 1 */
                                if(endRange2->number >= endRange1->number) {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;



                                } else {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;


                                }

                                retValue = 1;

                            } 


                        }

                    }

                }

            }

            rangeNode2 = rangeNode2->next_tree_node;
        }


        rangeNode1 = rangeNode1->next_tree_node;
    }

    if(retValue && !globalSuppressOutputFlag && !importParseFlag) {
        print_warning("*** WARNING ***  Range overlaps automatically adjusted.", lineBuf);

        rangeNode1 = rangelist;

        fprintf(stderr, "range reset to: ");
    
        while(rangeNode1) {

            if(rangeNode1->type) {

                startRange1 = rangeNode1->child[chld_strt_rnge12];
                endRange1   = rangeNode1->child[chld_end_range13];

                if(!startRange1) {
                    startRange1 = endRange1;
                }

                if(startRange1->type == NEG_NUMBER) {
                    fprintf(stderr, "-%lu..", startRange1->number);
                } else {
                    fprintf(stderr, "%lu..", startRange1->number);
                }

                if(endRange1->type == NEG_NUMBER) {
                    fprintf(stderr, "-%lu", endRange1->number);
                } else {
                    fprintf(stderr, "%lu", endRange1->number);
                }

            }

            prevPtr = rangeNode1;

            rangeNode1 = rangeNode1->next_tree_node;

            if(prevPtr->type && rangeNode1 && rangeNode1->type) fprintf(stderr, "|");

        }

        fprintf(stderr, "\n");

    }

}

/* ============================================================ */
void
check_range_limits(tree * rangelist, int flag) {

    tree * rangeNode1 = rangelist;
    tree * startRange1 = NULL;
    tree * endRange1   = NULL;
    tree * prevPtr;

    int retValue = 0;

    unsigned long minValue = 0;
    unsigned long maxValue = 0;

    char * minValueString = NULL;
    char * maxValueString = NULL;


    /* INTEGER and Integer32 */
    if(flag == INTEGER) {
        minValue       = 2147483648U;
        maxValue       = 2147483647U;
        minValueString = "2147483648";
        maxValueString = "2147483647";

    /* Unsigned32, Gauge32, Gauge,  UInteger32, TimeTicks  */
    } else {
        minValue       = 0U;
        maxValue       = 4294967295U;
        minValueString = "0";
        maxValueString = "4294967295";
    }
    
    while(rangeNode1) {

        startRange1 = rangeNode1->child[chld_strt_rnge12];
        endRange1   = rangeNode1->child[chld_end_range13];

        if(!startRange1) {
            startRange1 = endRange1;
        }


        if(startRange1->type == NEG_NUMBER) {

            if(endRange1->type == NEG_NUMBER) {

                if(endRange1->number > minValue) {

                    endRange1->number = minValue;
                    if(endRange1->string) {
                        free(endRange1->string);
                    }
                    endRange1->string = strdup(minValueString);

                    retValue = 1;
                }


                /* -- start is actually greater than end */
                if(startRange1->number < endRange1->number) {

                    startRange1->number = endRange1->number;
                    if(startRange1->string) {
                        free(startRange1->string);
                    }
                    startRange1->string = strdup(endRange1->string);

                    retValue = 1;
                }                    


            } else {

                if(endRange1->number > maxValue) {

                    endRange1->number = maxValue;
                    if(endRange1->string) {
                        free(endRange1->string);
                    }
                    endRange1->string = strdup(maxValueString);

                    retValue = 1;

                }

                /* -+ start is always less than end */

            }


            if(startRange1->number > minValue) {

                startRange1->number = minValue;
                if(startRange1->string) {
                    free(startRange1->string);
                }
                startRange1->string = strdup(minValueString);

                retValue = 1;
            }


        } else {

            if(endRange1->type == NEG_NUMBER) {

                /* +- start is always greater than end */
                retValue = 1;

                startRange1->number = endRange1->number;
                if(startRange1->string) {
                    free(startRange1->string);
                }
                startRange1->string = strdup(endRange1->string);
                startRange1->type = endRange1->type;

            } else {

                if(endRange1->number > maxValue) {

                    endRange1->number = maxValue;
                    if(endRange1->string) {
                        free(endRange1->string);
                    }
                    endRange1->string = strdup(maxValueString);

                    retValue = 1;

                }

                /* ++ if start is greater than end */
                if(startRange1->number > endRange1->number) {
                    retValue = 1;

                    startRange1->number = endRange1->number;
                    if(startRange1->string) {
                        free(startRange1->string);
                    }
                    startRange1->string = strdup(endRange1->string);

                }                    

            }

            if(startRange1->number > maxValue) {

                startRange1->number = maxValue;
                if(startRange1->string) {
                    free(startRange1->string);
                }
                startRange1->string = strdup(maxValueString);

                retValue = 1;

            }


        }


        if(startRange1 && (startRange1->number == 0)) {
            startRange1->type = POS_NUMBER;
        }

        if(endRange1 && (endRange1->number == 0)) {
            endRange1->type = POS_NUMBER;
        }

        rangeNode1 = rangeNode1->next_tree_node;
    }

    if(retValue && !globalSuppressOutputFlag && !importParseFlag) {
        print_warning("*** WARNING ***  Range limits automatically adjusted.", lineBuf);

        rangeNode1 = rangelist;

        fprintf(stderr, "range reset to: ");
    
        while(rangeNode1) {

            if(rangeNode1->type) {

                startRange1 = rangeNode1->child[chld_strt_rnge12];
                endRange1   = rangeNode1->child[chld_end_range13];

                if(!startRange1) {
                    startRange1 = endRange1;
                }

                if(startRange1->type == NEG_NUMBER) {
                    fprintf(stderr, "-%lu..", startRange1->number);
                } else {
                    fprintf(stderr, "%lu..", startRange1->number);
                }

                if(endRange1->type == NEG_NUMBER) {
                    fprintf(stderr, "-%lu", endRange1->number);
                } else {
                    fprintf(stderr, "%lu", endRange1->number);
                }

            }

            prevPtr = rangeNode1;

            rangeNode1 = rangeNode1->next_tree_node;

            if(prevPtr->type && rangeNode1 && rangeNode1->type) fprintf(stderr, "|");

        }

        fprintf(stderr, "\n");

    }

}

/* ============================================================ */
void
fill_in_max_min_values(tree * rangelist, int flag) {

    tree * rangeNode1 = rangelist;
    tree * startRange1, * endRange1;
    tree * prevPtr;

    int retValue = 0;

    int minType;

    unsigned long minValue;
    unsigned long maxValue;
    char * minValueString;
    char * maxValueString;

    /* INTEGER and Integer32 */
    if(flag == INTEGER) {
        minValue       = 2147483648U;
        maxValue       = 2147483647U;
        minValueString = "2147483648";
        maxValueString = "2147483647";
        minType        = NEG_NUMBER;

    /* Unsigned32, Gauge32, Gauge,  UInteger32, TimeTicks  */
    } else {
        minValue       = 0U;
        maxValue       = 4294967295U;
        minValueString = "0";
        maxValueString = "4294967295";
        minType        = POS_NUMBER;
    }
    
    while(rangeNode1) {

        startRange1 = rangeNode1->child[chld_strt_rnge12];
        endRange1   = rangeNode1->child[chld_end_range13];

        if(startRange1) {
            if(startRange1->type == MAX) {
                startRange1->number = maxValue;
                startRange1->string = maxValueString;
                startRange1->type   = POS_NUMBER;
                retValue = 1;
            } else if(startRange1->type == MIN) {
                startRange1->number = minValue;
                startRange1->string = minValueString;
                startRange1->type   = minType;
                retValue = 1;
            }
        }

        if(endRange1->type == MAX) {
            endRange1->number = maxValue;
            endRange1->string = maxValueString;
            endRange1->type   = POS_NUMBER;
            retValue = 1;
        } else if(endRange1->type == MIN) {
            endRange1->number = minValue;
            endRange1->string = minValueString;
            endRange1->type   = minType;
            retValue = 1;
        }

        rangeNode1 = rangeNode1->next_tree_node;
    }

    if(retValue && !globalSuppressOutputFlag && !importParseFlag) {
        print_warning("*** NOTE ***  MAX and MIN substitutions made (technically illegal).", lineBuf);

        rangeNode1 = rangelist;
    
        fprintf(stderr, "range reset to: ");

        while(rangeNode1) {

            if(rangeNode1->type) {

                startRange1 = rangeNode1->child[chld_strt_rnge12];
                endRange1   = rangeNode1->child[chld_end_range13];

                if(!startRange1) {
                    startRange1 = endRange1;
                }

                if(startRange1->type == NEG_NUMBER) {
                    fprintf(stderr, "-%lu..", startRange1->number);
                } else {
                    fprintf(stderr, "%lu..", startRange1->number);
                }

                if(endRange1->type == NEG_NUMBER) {
                    fprintf(stderr, "-%lu", endRange1->number);
                } else {
                    fprintf(stderr, "%lu", endRange1->number);
                }

            }

            prevPtr = rangeNode1;

            rangeNode1 = rangeNode1->next_tree_node;

            if(prevPtr->type && rangeNode1 && rangeNode1->type) fprintf(stderr, "|");

        }

        fprintf(stderr, "\n");

    }

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============================================================ */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

