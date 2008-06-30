/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "x.h"
#define X(a) pts[a]->index

extern int ninputs, noutputs;
extern char *ego;

#include "hdr.h"

find_index (pts, npts, x, start, finish)
PTERM *pts[];
int npts, x, *start, *finish;
/*
 * This function searches the index components of "pts" looking
 * for the first and last pterms with index level "x".
 * These are stashed into *start and *finish, respectively.
 * It is presumed that pts is sorted by ascending index level.
 * If no pterms with the desired index level exist, *finish
 * is set lower than *start.
 */
{
	register int high, low, current;

	*start = (*finish = npts) + 1;
	if (x < X (0) || x > X (npts-1))
		return;

	/* use binary search to find where index level starts */

	low = 0;
	high = npts-1;
	while (low <= high) {
		current = (high + low) / 2;
		if (X (current) < x)
			low = current + 1;
		else if (current > 0 && X (current-1) >= x)
			high = current - 1;
		else if (X (current) == x) {
			*start = current;
			break;
		}
		else	return; /* not found */
		if (X (high) > x)
			*finish = high;
	}

	/* use binary search to find where index level ends */

	low = *start;
	high = *finish - 1;
	while (low <= high) {
		current = (high + low) / 2;
		if (current < npts-1 && X (current+1) <= x)
			low = current + 1;
		else if (X (current) > x)
			high = current - 1;
		else {
			*finish = current;
			return;
		}
	}
	assert (low <= high);
	return;
}


int
trymerge (pts, npts, i, j)
register PTERM *pts[];
register int i, j, npts;
{
	int m;

	if (cmpv (pts[i]->ptor, pts[j]->ptor, noutputs) != -1)
		/* different or-planes */
		return (npts);

	m = cmpv (pts[i]->ptand, pts[j]->ptand, ninputs);
	if (m < 0)
		/* very different and-planes */
		return (npts);

	if (npts >= NPTERMS) {
		fprintf (stderr, "%s: merge: No space.\n", ego);
		exit (1);
	}
#ifdef DEBUG
	printf ("merging %d and %d\n", i, j);
#endif
	if (pts[i]->ptand[m] == 2) {
		/* i dominates j */
		pts[j]->cv = 1;
		return (npts);
	}
	if (pts[j]->ptand[m] == 2) {
		/* j dominates i */
		pts[i]->cv = 1;
		return (npts);
	}
	/* neither dominates the other */
	pts[npts] = (PTERM *)copy_pterm (pts[i]);
	pts[npts]->ptand[m] = 2;
	pts[npts]->andhash = pthash (pts[npts]->ptand, ninputs);
	pts[npts]->index = ptindex (pts[npts]->ptand, ninputs);
	pts[npts]->cv = 0;
	npts++;
	pts[i]->cv = pts[j]->cv = 1;
	return (npts);
}

int 
merge (pts, npts)
PTERM *pts[];
int npts;
/*
 * My job is to find pairs of adjacent PTERM's with identical
 * outputs, add a PTERM on the end with those same outputs
 * but with an extra "don't care" for that pair, and mark the
 * pair "covered". I return the new number of PTERM's. 
 */
{
	register int i, j, k;
	int strt, fin, strtnext, finnext, old_npts;
	long pthash(BIT *inv, int n);

	if ((old_npts = npts) < 2)
		return (npts);
#ifdef DEBUG
	fprintf (stderr, "merge: qsort ...");
#endif
	/* sort pts by ascending index (number of 1's)*/
	qsort (pts, npts, sizeof (PTERM *), cmpptx);

#ifdef DEBUG
	fprintf (stderr, " done. merge npts = %d ...", npts);
#endif

    for (k = 0; k < ninputs; k++) {
	find_index (pts, old_npts, k, &strt, &fin);
#ifdef DEBUG
	printf ("index %d: start = %d, finish = %d\n", k, strt, fin);
#endif
	if (fin >= strt) {
		find_index (pts, old_npts, k+1, &strtnext, &finnext);
#ifdef DEBUG
	printf ("index %d: start = %d, finish = %d\n", k+1, strtnext, finnext);
#endif
	}
	else	continue;

	for (i = strt; i <= fin; i++) {
		for (j = i+1; j <= fin; j++)
			/* same index level */
			npts = trymerge (pts, npts, i, j);
		for (j = strtnext; j <= finnext; j++)
			/* adjacent index level */
			npts = trymerge (pts, npts, i, j);
	}
    }

#ifdef DEBUG
	fprintf (stderr, " done. npts = %d.\n", npts);
#endif
	return (npts);
}
