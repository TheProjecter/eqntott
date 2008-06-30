/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

/*
 * This file contains functions for maintaining the product term free
 * space.  Space for product terms is obtained dynamically in chunks of
 * CHUNK terms at a time.  Returned space is never returned to the
 * system, but is placed on a free list for later use.
 *
 * static get_free ()
 *	gets a bunch of product terms and adds them to the free list.
 *	space for and-plane and or-plane vectors is also obtained.
 * PTERM *new_pterm (PTERM *next)
 *	picks a pterm from the free list, and returns a pointer to it.
 *	the pterm is initialized with a next of "next", and dontcare's in
 *	the and-plane, and 0's in the or-plane.
 * PTERM *copy_pterm (PTERM *pterm)
 *	picks a pterm from the free list and copies components from "pterm"
 * PTERM *free_pterm (PTERM *pterm)
 *	puts pterm back on the free list and returns pterm's next value
 *	which is appropriated for the free list.
 */

#include <stdio.h>
#include "x.h"
#define CHUNK 1024
static PTERM *freePtr = 0;
extern int ninputs, noutputs;

static
get_free ()
{
	register PTERM *pterms;
	register int i;
	register BIT *or, *and;
	char *sbrk();

	freePtr = pterms = (PTERM *) malloc (CHUNK * sizeof (PTERM));
	and = (BIT *) malloc (ninputs * CHUNK * sizeof (BIT));
	or = (BIT *) malloc (noutputs * CHUNK * sizeof (BIT));
	/*
	fprintf( stderr, "debug: ninputs=%d, noutputs=%d, CHUNK=%d\n",
	ninputs, noutputs, CHUNK);
	*/
	if ((pterms == NULL) || (and == NULL) || (or == NULL)) {
		fprintf (stderr,
		"can't malloc for Pterm: ninputs=%d, noutputs=%d\n",
		ninputs,noutputs);
		exit (1);
	}
	for (i = 0; i < CHUNK; i++) {
		pterms[i].ptand = and;
		and += ninputs;
		pterms[i].ptor = or;
		or += noutputs;
		pterms[i].next = &pterms[i+1];
	}
	pterms[CHUNK-1].next = 0;
	return;
}

PTERM *
copy_pterm (pterm)
PTERM *pterm;
{
	register BIT *b, *c;
	register int i;
	PTERM *copy;

	if (!freePtr)
		get_free ();
	copy = freePtr;
	freePtr = freePtr->next;
	copy->next = pterm->next;
	copy->andhash = pterm->andhash;
	copy->index = pterm->index;
	copy->cv = pterm->cv;
	for (b = pterm->ptand, c = copy->ptand, i = ninputs; i; i--)
		*c++ = *b++;
	for (b = pterm->ptor, c = copy->ptor, i = noutputs; i; i--)
		*c++ = *b++;
	return (copy);
}

PTERM *
new_pterm (next)
PTERM *next;
{
	register PTERM *pterm;
	register BIT *b;
	register int i;

	if (!freePtr)
		get_free ();
	pterm = freePtr;
	freePtr = freePtr->next;
	pterm->next = next;
	for (b = pterm->ptand, i = ninputs; i; i--)
		*b++ = 2;
	for (b = pterm->ptor, i = noutputs; i; i--)
		*b++ = 0;
	return (pterm);
}

PTERM *
free_pterm (pterm)
PTERM *pterm;
{
	PTERM *next;
/* added 12/10/84 by Richard Rudell per R. Newton's suggestion */
	pterm->cv = 0;
	next = pterm->next;
	pterm->next = freePtr;
	freePtr = pterm;
	return (next);
}
