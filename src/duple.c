/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include <stdio.h>
#include "x.h"

extern int ninputs;

int duple (pts, npts)
PTERM *pts[];
int npts;
{
	int cmppth (), cmpv ();
	register int i, k;

#ifdef DEBUG
	fprintf (stderr, "duple: qsort ...");
#endif
	qsort (pts, npts, sizeof (PTERM *), cmppth);
#ifdef DEBUG
	fprintf (stderr, " done. duple ...");
#endif

	for (k = 0; k < npts-1; k++)
		for (i = k+1; i < npts; i++) {
			if (pts[k]->andhash != pts[i]->andhash)
				break;
			if (cmpv (pts[i]->ptand, pts[k]->ptand, ninputs) != -1)
				continue;
			orpt (pts[k], pts[i]);
			pts[i]->cv = 1;
		}

#ifdef DEBUG
	fprintf (stderr, " done. npts = %d\n", npts);
#endif
	return (npts);
}
