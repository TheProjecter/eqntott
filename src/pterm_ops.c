/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */
#include "hdr.h"
	
/*
 * This file contains the functions:
 * int cmppt (PTERM **a, PTERM **b)
 *	qsort comparison (for output ordering) routine
 * int cmpv (BIT *x, BIT *y, n)
 *	compares two BIT strings of length n.
 * int cmpptx (PTERM **a, PTERM **b)
 *	qsort comparison (by index values) routine
 * int cmppth (PTERM **a, PTERM **b)
 *	qsort comparison (by andhash values) routine
 * PTERM *find_pterm (PTERM *pt, PTERM *pts[], int npts)
 *	looks through the "npts" product terms "pts" and if one is found
 *	with identical and-plane to "pt", it is returned, otherwise NIL_PTERM.
 * orpt (PTERM *pt1, PTERM *pt2)
 *	BIT-wise or the or-plane connections of pt2 into those of pt1.
 * long pthash (BIT *inv, int n)
 *	hashes the n BIT's in the vector inv.
 * int ptindex (BIT *inv, int n)
 *	returns number of 1's in BIT vector inv of length n.
 */

#include <stdio.h>
#include "x.h"
extern int ninputs, noutputs;

int cmppt (a, b)
PTERM *a[], *b[];
/*
 * compare product terms indirectly pointed to by a and b.
 */
{
	register int i, aa, bb;

	for (i = 0; i < ninputs; i++) {
		aa = a[0]->ptand[i];
		bb = b[0]->ptand[i];
		if (aa == 2)
			aa = 0;
		if (bb == 2)
			bb = 0;
		if (aa != bb) {
			if (aa < bb) {
				return (-1);
			}
			else	{
				return (1);
			}
		}
	}
	return (0);
}

int cmpv (x, y, n)
register BIT *x, *y;
int n;
/*
 * return -1 if x[i] = y[i] for 0 <= i < n;
 * return j>-1 if x[i] = y[i] for all i except i != j;
 * return -2 otherwise
 */
{
	register int i;
	int j = -1;

	for (i = 0; i < n; i++, x++, y++)
		if (*x != *y)
			if (j > -1)
				return (-2);	/* 2nd discrepancy */
			else	j = i;		/* first discrepancy */

	return (j);
}

int 
cmpptx (a, b)
PTERM *a[], *b[];
/*
 * compare index of product terms indirectly pointed to by a and b.
 */
{
	if (a[0]->index == b[0]->index)
		return (0);
	else	if (a[0]->index < b[0]->index)
			return (-1);
		else	return (1);
}

int 
cmppth (a, b)
PTERM *a[], *b[];
/*
 * compare andhash of product terms indirectly pointed to by a and b.
 */
{
	register BIT *p, *q;
	register int i;

	for (i = ninputs, p = a[0]->ptand, q = b[0]->ptand; i; i--, p++, q++)
		if (*p != *q)
			if (*p < *q)
				return (-1);
			else	return (1);
	return (0);
	/*
	if (a[0]->andhash == b[0]->andhash)
		return (0);
	else	if (a[0]->andhash < b[0]->andhash)
			return (-1);
		else	return (1);
	*/
}

//PTERM *find_pterm(pts, npts, pt) 
//PTERM *pts[], *pt;
//int npts;
PTERM *find_pterm (PTERM *pt, PTERM *pts[], int npts)
/*
 * My job is to return a pointer to the first PTERM in "pts" whose 
 * ptand values are the same as those for "pt".
 * I presume that pts is sorted by ascending andhash values.
 */
{
#define H(a) (pts[a]->andhash)

	register int i, low, high;
	long hsh;

	/* MUST pthash since extant modifies and-plane!!! */
	hsh = pthash (pt->ptand, ninputs);

	/* use binary search to find first pt with right andhash */

#ifdef DEBUG
	printf ("find_pterm (real hash=%ld): ", hsh);
	putpt (pt);
#endif
	low = 0;
	high = npts-1;
	while (low <= high) {
		i = (high + low) / 2;
		if (H (i) < hsh)
			low = i + 1;
		else if (i > 0 && H (i-1) >= hsh)
			high = i - 1;
		else if (H (i) == hsh)
			break;
		else	return (NIL_PTERM); /* andhash not found */
	}

	for (; i < npts && hsh == H (i); i++) {
		register int j;
		register BIT *a, *b;

		for (j = ninputs, a = pt->ptand, b = pts[i]->ptand; j; j--)
			if (*a++ != *b++)
				break;
		if (j <= 0)
			return (pts[i]);
	}

	/* pt not found */
	return (NIL_PTERM);
}

orpt (pt1, pt2)
PTERM *pt1, *pt2;
/*
 * My job is to output BIT-wise "or" the ptor arrays of PTERMS
 * "pt1" and "pt2" and leave the result in the ptor array
 * of "pt1".
 */
{
	register int i;
	register BIT *p, *q;

	for (i = noutputs, p = pt1->ptor, q = pt2->ptor; i; i--, p++, q++)
		*p = or3 (*p, *q);

	return;
}

long
pthash (inv, n)
BIT *inv;
int n;
/*
 * form the sum of no more than the first 20 elements of inv:
 * sum inv[i] * 3 ** (i - 1);
 */
{
	register int i;
	register long sum = 0;

	if (n > 19)
		n = 19;

	for (i = n; i; i--)
		sum = sum * 3 + *inv++;
	return (sum);
}

int 
ptindex (inv, n)
BIT *inv;
int n;
/*
 * I return the number of 1's in the "n" elements of "inv".
 */
{
	register int i, sum = 0;

	for (i = n; i; i--)
		if (*inv++ == 1)
			sum++;
	return (sum);
}
