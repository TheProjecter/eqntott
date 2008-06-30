/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include <stdio.h>
#ifndef MAXSTR
#define MAXSTR 512
#endif
#define MAXARG 32
#define NFILES 32

char *ego;		/* name program was invoked with */
int Argc = 1;		/* number of flags to pass to C preprocessor */
char *Argv[MAXARG];	/* flags to pass to C preprocessor */
int nfiles = 0;		/* number of input files, if 0 use stdin */
char *files[NFILES];	/* names of input files */
char *dash = "-";
char planame[MAXSTR];	/* name for .na  */
int r_flag = 0;		/* weak reduce option */
int R_flag = 0;		/* strong reduce option */
int s_flag = 0;		/* substitute option */
int f_flag = 0;		/* feedback option */
int g_flag = 0;		/* gate array option */
int lisp = 0;		/* lisp output option */
int labels = 0;		/* labels for inputs and outputs should be generated */
int need_grind = 0;	/* need to use grind function */
int need_parse = 0;	/* need to process input */
char *key = 0;		/* output format control string */
char *defkey="iopte";	/* default output format control string */
char *defkeyx ="iolpte";/* default labeled output format control string */
char *defkeyf="iopfte";	/* default for -f option */
char *defkeyfx="iolpfte";/* default for -f with -l option */

int
procargs (argc, argv)
int argc;
char *argv[];
{
	register arg;
	char *s;

	ego = argv[0];
	for (arg = 1; arg < argc; arg++) {
		if (argv[arg][0] == '-')
			switch (argv[arg][1]) {
			case 'D':
			case 'I':
			case 'U':
				/* C preprocessor arguments */
				Argv[Argc++] = argv[arg];
				break;
			case 'r':
				need_parse = need_grind = r_flag = 1;
				break;
			case 'R':
				need_parse = need_grind = R_flag = 1;
				break;
			case 'L':
				lisp = 1;
				break;
			case 'G':
				g_flag = 1;
				break;
			case 'f':
				if (s_flag)
					goto usage;
				f_flag = 1;
				break;
			case 's':
				if (f_flag)
					goto usage;
				s_flag = 1;
				break;
			case 'l':
				labels = 1;
				break;
			case '.':
				if (key)
					goto usage;
				key = &argv[arg][2];
				break;
			default:
				goto usage;
			}
		else	files[nfiles++] = argv[arg];
	}

	if (nfiles == 0)
		files[(nfiles=1)-1] = dash;

	copyprefix( planame, files[0] );
	
	if (!key) 
		if (f_flag)
			key = (labels ? defkeyfx : defkeyf);
		else
			key = (labels ? defkeyx : defkey);
	for (s = key; *s; s++)
		switch (*s) {
		case 'e':
		case 'v':
			break;
		case 'f':
			f_flag++;
			break;
		case 's':
			s_flag++;
			break;
		case 'i':
		case 'o':
		case 'I':
		case 'O':
		case 'l':
			need_parse = 1;
			break;
		case 'n':
		case 'p':
		case 'S':
		case 'h':
		case 't':
			need_parse = need_grind = 1;
			break;
		default:
			goto usage;
		}
	return (0);

usage:
	fprintf(stderr, 
	    "usage: %s [-l] [-r] [-R] [-f|-s] [-.key] [cpp args] [file]...\n", ego);
	fprintf (stderr, 
	"\"key\" is built from I, O, S, e, f, h, i, l, n, o, p, t, and v.\n");
	return (1);
}

copyprefix( sout, sin ) char *sout, *sin; {
	while( *sout++ = *sin++ )
		if( *sin == '.' )
			break;
	*sout = 0;
}

set_planame( name ) char *name; {
	int i;
	for( i = 0; name[i] != 0; i++ ) {
		planame[i] = name[i];
	}
	planame[i] = 0;
}
