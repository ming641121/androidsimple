/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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
 */

#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union {
    int val;
    char * char_ptr;
    struct tree_tag * tree_ptr;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	ACCESS	257
# define	accessible_for_notify	258
# define	AGENT_CAPABILITIES	259
# define	ANY	260
# define	APPLICATION	261
# define	CCE	262
# define	AUGMENTS	263
# define	BAR	264
# define	BEGIN_	265
# define	BINARY_NUMBER	266
# define	BIT	267
# define	BITS	268
# define	BY	269
# define	CHOICE	270
# define	COMMA	271
# define	CONTACT_INFO	272
# define	Counter	273
# define	Counter32	274
# define	Counter64	275
# define	CREATION_REQUIRES	276
# define	currrent	277
# define	DEFINED	278
# define	DEFINITIONS	279
# define	DEFVAL	280
# define	deprecated	281
# define	DESCRIPTION	282
# define	DISPLAY_HINT	283
# define	DOT	284
# define	DOT_DOT	285
# define	END	286
# define	ENTERPRISE	287
# define	EXPLICIT	288
# define	EXPORTS	289
# define	FROM	290
# define	Gauge	291
# define	Gauge32	292
# define	GROUP	293
# define	HEX_NUMBER	294
# define	IDENTIFIER	295
# define	IMPLICIT	296
# define	IMPLIED	297
# define	IMPORTS	298
# define	INCLUDES	299
# define	INDEX	300
# define	INTEGER	301
# define	Integer32	302
# define	IpAddress	303
# define	LAST_UPDATED	304
# define	LBRACE	305
# define	LBRACKET	306
# define	LOWERCASE_IDENTIFIER	307
# define	LPAREN	308
# define	mandatory	309
# define	MANDATORY_GROUPS	310
# define	MAX_ACCESS	311
# define	MAX	312
# define	MIN	313
# define	MIN_ACCESS	314
# define	MODULE	315
# define	MODULE_COMPLIANCE	316
# define	MODULE_CONFORMANCE	317
# define	MODULE_IDENTITY	318
# define	NEG_NUMBER	319
# define	not_accessible	320
# define	NOTIFICATION_GROUP	321
# define	NOTIFICATIONS	322
# define	NOTIFICATION_TYPE	323
# define	not_implemented	324
# define	NULL_	325
# define	OBJECT	326
# define	OBJECT_GROUP	327
# define	OBJECT_IDENTITY	328
# define	ObjectSyntax	329
# define	OBJECTS	330
# define	OBJECT_TYPE	331
# define	obsolete	332
# define	OCTET	333
# define	OF	334
# define	OID	335
# define	Opaque	336
# define	optional	337
# define	ORGANIZATION	338
# define	POS_NUMBER	339
# define	PRIVATE	340
# define	PRODUCT_RELEASE	341
# define	QUOTED_STRING	342
# define	RBRACE	343
# define	RBRACKET	344
# define	read_create	345
# define	read_only	346
# define	read_write	347
# define	REFERENCE	348
# define	REVISION	349
# define	RPAREN	350
# define	SEMICOLON	351
# define	SEQUENCE	352
# define	SIZE	353
# define	STATUS	354
# define	STRING	355
# define	STRUCT	356
# define	SUPPORTS	357
# define	SYNTAX	358
# define	TEXTUAL_CONVENTION	359
# define	TimeTicks	360
# define	TRAP_TYPE	361
# define	UInteger32	362
# define	UNION	363
# define	UNITS	364
# define	UNIVERSAL	365
# define	Unsigned32	366
# define	UPPERCASE_IDENTIFIER	367
# define	VARIABLES	368
# define	VARIATION	369
# define	write_only	370
# define	WRITE_SYNTAX	371


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
