/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include <stdio.h>
#include "x.h"

extern int r_flag, R_flag;

int 
reduce (pts, npts)
PTERM *pts[];
int npts;
{
	int i, changed = 1, merge (), duple (), rmcvd ();

	npts = duple (pts, npts);
	npts = rmcvd (pts, npts);

	if (!r_flag && !R_flag)
		return (npts);

	while (changed && npts > 1) {
		changed = 0;
		npts = merge (pts, i = npts);
		if (R_flag)
			cover (pts, npts);	/* doesn't change npts */
		changed |= i != npts;
		npts = rmcvd (pts, i = npts);
		changed |= i != npts;
		npts = duple (pts, i = npts);
		changed |= i != npts;
		npts = rmcvd (pts, i = npts);
		changed |= i != npts;
	}

	return (npts);
}
