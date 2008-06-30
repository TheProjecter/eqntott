/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include <stdio.h>
#include "x.h"

extern int ninputs, noutputs, nerrors;
extern int f_flag, s_flag, need_parse, need_grind;
extern char *ego, *key;
extern struct Nt nts[], *outorder[], *inorder[];

long pthash();
BNODE *exprs[NOUTPUTS], *canon(), *ecanon();
PTERM *pts[NPTERMS], *read_ones();

extern char planame[];

main (argc, argv)
int argc; 
char *argv[];
{
	int i, npts;

	if (procargs (argc, argv) || preprocess())
		exit (1);
	nt_init();
	if (need_parse && yyparse())
			nerrors++;
	if (need_grind) {
		if (nerrors)
			exit (1);
		npts = grind (pts, npts);
	}
	for (; *key; key++)
		switch (*key) {
		case 'I':
			for (i = 0; i < ninputs; i++)
				printf (".I %s\n", inorder[i]->name);
			break;
		case 'O':
			for (i = 0; i < noutputs; i++)
				printf (".O %s\n", outorder[i]->name);
			break;
		case 'S':
			putSpla (pts, npts);
			break;
		case 'e':
			printf (".e\n");
			break;
		case 'f':
			for (i = 0; i < ninputs; i++)
				if (f_flag && inorder[i]->isoutput)
					printf (".f %d %d\n", 
					    inorder[i]->outord+1, i+1);
			break;
		case 'h':
			putrpla (pts, npts);
			break;
		case 'i':
			printf (".i %d\n", ninputs);
			break;
		case 'n':
			printf (".n %d\n", npts);
			break;
		case 'o':
			printf (".o %d\n", noutputs);
			break;
		case 'l':
			if( strcmp( planame, "-" ) != 0 ) {
				printf(".na %s\n", planame );
			}
			printf(".ilb ");
			for( i = 0; i < ninputs; i++ ) {
				printf( " %s", inorder[i]->name );
			}
			printf("\n");
			printf(".ob ");
			for( i = 0; i < noutputs; i++ ) {
				printf( " %s", outorder[i]->name );
			}
			printf("\n");
			break;
		case 'p':
			printf (".p %d\n", npts);
			break;
		case 't':
			putpla (pts, npts);
			break;
		case 'g':
			for( i = 0; i < ninputs; i++ )
			{
				ecanon( exprs[i] );
			}
			for( i = 0; i < noutputs; i++ )
			{
				prgexpr( exprs[i], i, OUTPUT );
			}
			break;
		case 'x':
			printf ("(INPUTS ");
			for( i = 0; i < ninputs; i++ ) {
				printf ("'%s ", inorder[i]->name);
			}
			printf (")\n(OUTPUTS ");
			for( i = 0; i < noutputs; i++ ) {
				printf ("'%s ", outorder[i]->name);
			}
			printf (")\n");
			for( i = 0; i < noutputs; i++ ) {
				printf ("(CONNECT '%s ", outorder[i]->name);
				prlexpr (exprs[i]);
				printf (")\n");
			}
			break;
		case 'v':
			version();
			break;
		default:
			break;
		}
	
	exit (0);
}

int
grind (pts, npts)
PTERM *pts[];
int npts;
{
	PTERM *pt, *ptexprs[NOUTPUTS];
	int pass, changed, o, last;

	for (o = 0; o < noutputs; o++) {
		if (!exprs[o]) {
			fprintf (stderr, "%s: no expression for %s\n",
				ego, outorder[o]->name);
			exit (1);
		}
	}
	pass = 0;
	do {
		for (last = (changed = o = 0) - 1; o < noutputs; o++) {
			exprs[o] = canon (exprs[o]);
			if (s_flag && substitute (exprs[o], &exprs[o])) {
				last = o;
				changed++;
				exprs[o] = canon (exprs[o]);
			}
		}
	} while (changed && ++pass < NSUBSTITUTE);

	if (changed) {
		fprintf (stderr, "%s: infinite substitution loop: %s?\n", 
			ego, outorder[last]->name);
		return (0);
	}

	for (o = 0; o < noutputs; o++)
		ptexprs[o] = read_ones (exprs[o], o);

	/*
	 * grind shares "pts" with read_ones (compress) so
	 * DON'T merge the previous and next for loops.
	 */

	for (npts = o = 0; o < noutputs; o++) {
		pt = ptexprs[o];
		for (; pt; pt = pt->next) {
			/*
			 * we depend on read_ones (compress) for:
			 * pt->andhash = pthash (pt->ptand, ninputs);
			 * pt->cv = 0;
			 */
			pt->index = ptindex (pt->ptand, ninputs);
			if (npts < NPTERMS)
				pts[npts++] = pt;
		}
	}
	npts = reduce (pts, npts);
	return (npts);
}
