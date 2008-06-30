/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include "x.h"
#include <stdio.h>

extern int ninputs, noutputs;

cover (pts, npts)
PTERM *pts[];
int npts;
/*
 * run through all of the product terms and if a product term is
 * properly dominated by another, mark the former as covered.
 */
 {
	PTERM *extant();
	int i, cmppth();

	qsort (pts, npts, sizeof (PTERM *), cmppth);
	for (i = 0; i < npts; i++) {
		if (pts[i]->cv)
			continue;
		if (extant (pts, npts, pts[i])) {
			pts[i]->cv = 1;
#ifdef DEBUG
			printf ("cover: covering %d\n", i);
#endif
		}
	}
	return;
}
		
PTERM *
extant (pts, npts, pt)
PTERM *pts[], *pt;
int npts;
/*
 * Try to find a product term with an and-plane which implies pt's 
 * and-plane as well as a compatible or-plane.
 * Covered product terms (which don't really exist, but have not been
 * garbage collected yet) are ignored.
 * Return a pointer to this pterm, or NIL_PTERM if not found.
 * NOTE: extant modifies pt's and-plane!! For this to work, 
 * find_pterm MUST NOT use pt's and-plane for lookups (as in
 * a simple linear search of the whole and-plane).
 */
{
	PTERM *tmp, *find_pterm();
	BIT b;
	int i;

#ifdef DEBUG
	printf ("extant? ");
	putpt (pt);
#endif
	for (i = 0; i < ninputs; i++) {
		b = pt->ptand[i];
		if (b == 2)
			continue;
		pt->ptand[i] = 2;
		tmp = find_pterm (pts, npts, pt);
		if (tmp && !tmp->cv && 
		    -1 == cmpv (tmp->ptor, pt->ptor, noutputs)) {
			pt->ptand[i] = b;
			return (tmp);
		}
		tmp = extant (pts, npts, pt);
		if (tmp && !tmp->cv && 
		    -1 == cmpv (tmp->ptor, pt->ptor, noutputs)) {
			pt->ptand[i] = b;
			return (tmp);
		}
		pt->ptand[i] = b;
	}
#ifdef DEBUG
	printf ("not extant ");
	putpt (pt);
#endif
	return (NIL_PTERM);
}
