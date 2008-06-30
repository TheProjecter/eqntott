/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include <stdio.h>
#include "x.h"

int rmcvd (pts, npts)
PTERM *pts[];
int npts;
/*
 * I remove PTERM's with the "covered" bit set, and return new
 * number of PTERMS in "pts".
 */
{
	register int i;
	PTERM *free_pterm();

#ifdef DEBUG
	fprintf (stderr, "rmcvd: npts = %d ...", npts);
#endif
	for (i = 0; i < npts; i++)
		if (pts[i]->cv) {
			free_pterm (pts[i]);
			npts--;
			pts[i] = pts[npts];
			i--;
	}

#ifdef DEBUG
	fprintf (stderr, " done. npts = %d.\n", npts);
#endif
	return (npts);
}
