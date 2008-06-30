/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

/*
 * read_ones (exp, o) returns a list of (all) product terms whose and-plane
 * connections are such as to yield a non-false value for the expression
 * "exp".  The or-plane connections for output "o" are initialized to 
 * 1's or x's.
 */

#include <stdio.h>
#include "x.h"
#include "hdr.h"
extern int ninputs;
extern struct Nt nts[], *outorder[];
extern char *ego;

PTERM *
read_ones  (exp, o)
BNODE *exp;
int o;
{
	register PTERM *L, *R;
	register int i;
	PTERM *left, *right, *pt, *new_pterm(), *free_pterm(), *compress();
	BIT *land, *mand, *rand;

	switch (exp->value) {
	case ZERO:
		return (NIL_PTERM);

	case ONE:
		pt = new_pterm (NIL_PTERM);
		pt->ptor[o] = 1;
		return (pt);

	case DONTCARE:
		pt = new_pterm (NIL_PTERM);
		pt->ptor[o] = 2;
		return (pt);

	case AND:
		left = read_ones (exp->left, o);
		right = read_ones (exp->right, o);
		pt = NIL_PTERM;
		for (L = left; L; L = L->next) {
			land = L->ptand;
			for (R = right; R; R = R->next) {
				rand = R->ptand;
				pt = new_pterm (pt);
				pt->ptor[o] = and3 (L->ptor[o], R->ptor[o]);
				mand = pt->ptand;
				for (i = 0; i < ninputs; i++)
					if (land[i] == rand[i])
						mand[i] = land[i];
					else if (rand[i] == 2)
						mand[i] = land[i];
					else if (land[i] == 2)
						mand[i] = rand[i];
					else if (R->ptor[o] == 2) {
						pt->ptor[o] = 1;
						mand[i] = land[i];
					}
					else if (L->ptor[o] == 2) {
						pt->ptor[o] = 1;
						mand[i] = rand[i];
					}
					else {
						/* logical contadiction */
						pt = free_pterm (pt);
						/* contradiction (exp, o); */
						break;
					}
			}
		}
		for (L = left; L; L = free_pterm (L)) ;
		for (R = right; R; R = free_pterm (R)) ;
		return (compress(pt));

	case OR:
		/* cat and return lists from left and right operands */
		L = left = read_ones (exp->left, o);
		right = read_ones (exp->right, o);
		if (!left)
			return (compress(right));
		if (!right)
			return (compress(left));
		while (L->next)
			L = L->next;
		L->next = right;
		return (compress(left));

	case NOT:
		pt = new_pterm (NIL_PTERM);
		pt->ptand[exp->left->value] = 0;
		pt->ptor[o] = 1;
		return (pt);

	default:
		pt = new_pterm (NIL_PTERM);
		pt->ptand[exp->value] = 1;
		pt->ptor[o] = 1;
		return (pt);
	}
}

contradiction (exp, o)
BNODE *exp;
int o;
/*
 * a logical contradiction has been found.
 */
{
	fprintf (stderr, "%s: logical contradiction in eqn for %s:\n",
		ego, outorder[o]->name);
	prexpr (exp);
	fprintf (stderr, "\nWill try to continue... \n");
	return;
}

PTERM *
compress (pt)
register PTERM *pt;
{
	extern PTERM *pts[];
	register int i;
	int npts;

	for (i = 0; pt && i < NPTERMS; pt=pt->next, i++) {
		pt->andhash = pthash (pt->ptand, ninputs);
		pt->cv = 0;
		pts[i] = pt;
	}
	npts = i;
	if (npts > NPTERMS) {
		fprintf (stderr, "%s: compress: too many pterms\n");
		return (pts[0]);
	}
	npts = duple (pts, npts);
	npts = rmcvd (pts, npts);
	for (i = 0; i <= npts-2; i++)
		pts[i]->next = pts[i+1];
	if (npts) {
		pts[npts-1]->next = NIL_PTERM;
		pt = pts[0];
	}
	else	pt = NIL_PTERM;
	return (pt);
}
