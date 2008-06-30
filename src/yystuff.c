/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

/*
 * yygetc, yyungetc, yyerror, yylex
 */

#include <stdio.h>
#include <ctype.h>
#include "x.h"
#include "y.tab.h"

int atoi();

#define NERRORS 10		/* maximum number of syntax errors allowed */
#define NINBUF 1024		/* maximum capacity of input buffer */
#define NUNBUF 16		/* maximum capacity of pushback buffer */
#define MAXFNAME 64		/* maximum file name length */
#ifndef MAXSTR
#define MAXSTR 512		/* maximum string size */
#endif

int nerrors = 0;		/* number of syntax errors */
char yytext[MAXSTR];		/* yylex's text buffer */

static char yyfname[MAXFNAME];	/* current input file name */
static char yyinbuf[NINBUF];	/* input buffer */
static int yyninbuf = 0;	/* amount currently in "yyinbuf" */
static int yycharno = 0;	/* index into "yyinbuf" */
static int yyunbuf[NUNBUF];	/* pushback buffer */
static int yynunbuf = 0;	/* current pushback buffer size */
static int yylineno = 0;	/* current input line number */

extern char *ego;		/* name program was invoked with */

static int
yygetc()
{
	if (yynunbuf)
		return (yyunbuf[yynunbuf--]);
	else	if (++yycharno >= yyninbuf)
			if (0 < (yyninbuf = read (0, yyinbuf, NINBUF)))
				return ((int) yyinbuf[yycharno=0]);
			else	return (EOF);
		else	return ((int) yyinbuf[yycharno]);
}

static 
yyungetc (yyc)
int yyc;
{
	/* where possible, just backup "yycharno" instead of save */

	if (yycharno > 0 && yyinbuf[yycharno-1] == (char) yyc)
		yycharno--;
	else	if (++yynunbuf < NUNBUF)
			yyunbuf[yynunbuf] = yyc;
		else	fprintf (stderr, 
			    "%s: yyungetc: pushback buffer full.\n", ego);
	return;
}

yyerror (yyerrmsg)
char *yyerrmsg;
{
	register int yyp, yyq, yyr;
	int yylead, yytrail, yylen;

	if (yyfname[0])
		fprintf (stderr, "%s: %s: %d: %s\n", ego, yyfname,
		    yylineno, yyerrmsg);
	else	fprintf (stderr, "%s: %d: %s\n", ego, yylineno, yyerrmsg);

	/*
	 * try to print current line with a "^" under the current
	 * character position.
	 */

	/* try to find end of last line */
	for (yyp = yycharno-1; yyp >= 0 && yyinbuf[yyp] != '\n'; yyp--);
	
	if (yylead = (yyp < 0))
		/* line started in previous buffer */
		;
	else	yyp++;

	/* try to find end of current line */
	for (yyr = yycharno; yyr < yyninbuf && yyinbuf[yyr] != '\n'; yyr++);

	yytrail = yyr-- >= yyninbuf;	/* end of line in next buffer? */

	/* measure length of line to be printed */
	for (yylen = 0, yyq = yyp; yyq <= yyr; yyq++)
		switch (yyinbuf[yyq]) {
		case '\t':
			yylen = (1 + yylen / 8) * 8;
			break;
		case '\b':
			yylen--;
			break;
		default:
			yylen++;
			break;
		}

	if (yylen >= 72)
		/*
		 * line longer than terminal width if tabs handled properly
		 * (8 bytes are reserved for header/trailer)
		 */
		if (yyr - yyp >= 71)
			/* line is really long */
			if (yyr - yycharno < yycharno - yyp)
				yylead++, yyp = yyr - 71;
			else	yytrail++, yyr = yyp + 71;

	/* if first part of line not to be printed, print leader */
	if (yylead)
		fprintf (stderr, "... ");

	/* print out as much of the current line as is in yyinbuf */
	for (yyq = yyp; yyq <= yyr; yyq++)
		if (yylen >= 72 && isspace (yyinbuf[yyq]))
			putc (' ', stderr);
		else	putc (yyinbuf[yyq], stderr);

	/* if last part of line is not to be printed, print trailer */
	if (yytrail)
		fprintf (stderr, " ...\n");
	else	putc ('\n', stderr);

	/* print white space followed by "^" under naughty line */

	/* if a leader was printed, print leading white space */
	if (yylead)
		fprintf (stderr, "    ");

	for (yyq = yyp; yyq < yycharno; yyq++)
		switch (yyinbuf[yyq]) {
		case '\t':
			if (yylen >= 72)
				putc (' ', stderr);
			else	putc ('\t', stderr);
			break;
		case '\b':
			if (yylen >= 72)
				putc (' ', stderr);
			else	putc ('\b', stderr);
			break;
		default:
			putc (' ', stderr);
		}

	putc ('^', stderr);
	putc ('\n', stderr);

	if (++nerrors >= NERRORS) {
		fprintf (stderr, "%s: Too many errors.\n", ego);
		exit (1);
	}
	return;
}

int
yylex ()
{
	int c, i = 0;
	char *index();

start:
	c = yygetc();
	switch (c) {
	case ' ':
	case '\t':
		goto start;
	case '\n':
		yylineno++;
		goto start;
	case EOF:
		return (0);
	case '#':
		goto linesynch;
	default:
		if (isalpha(c) || c == '_')
			break;
		return (c);
	}

	yytext[0] = c;
	for (i = 1, c = yygetc();
	    (isalnum (c) || index ("_.*[]<>", (char) c)) && i < MAXSTR;
	    i++, c = yygetc())
		yytext[i] = c;
	yytext[i] = '\0';
	yyungetc(c);
	if (strcmp (yytext, "INORDER") == 0)
		return (INORDER);
	else if (strcmp (yytext, "OUTORDER") == 0)
		return (OUTORDER);
	else if (strcmp (yytext, "NAME") == 0)
		return (PLANAME);
	else if (strcmp (yytext, "ONE") == 0)
		return (SYMONE);
	else if (strcmp (yytext, "ZERO") == 0)
		return (SYMZERO);
	else	return (NAME);

linesynch:

	/*
	 * the linesynch line looks like:
	 * # 24 "filename"
	 */

	while (isspace (c = yygetc()));			/* skip space after # */
	for (yylineno = 0; isdigit (c); c = yygetc())
		yylineno = 10 * yylineno + c - '0';	/* read line number */
	while ((c = yygetc()) != '"' && c != EOF);	/* skip through " */
	for (i = 0, c = yygetc(); 
	    i < MAXFNAME && c != '"' && c != EOF; 
	    i++, c = yygetc())
		yyfname[i] = c;				/* read filename */
	yyfname[i] = '\0';
	while ((c = yygetc()) != '\n' && c != EOF);	/* skip through \n */

	goto start;
}
