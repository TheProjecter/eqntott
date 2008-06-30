/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

/*
 * nt_init, nt_update, nt_input, nt_output, nt_compar, nt_open
 */

#include <stdio.h>
#include "x.h"

struct Nt nts[NTSIZE];
struct Nt *inorder[NINPUTS];
struct Nt *outorder[NINPUTS];
int ninputs = 0;
int noutputs = 0;
extern char *ego;
extern int f_flag, s_flag;

/*
 * initialize name table
 */

nt_init ()
{
	register struct Nt *p = nts;
	register int i = NTSIZE;

	for (; i > 0; i--, p++)
		p->name = (char *) 0;

	return;
}

/*
 * add name to name table if not present
 */

struct Nt *
nt_update (name)
char *name;
{
	struct Nt *nt, *slot;
	int nt_compar(), nt_open();
	char *store(), *hsearch();

	nt = (struct Nt *) hsearch (name, (char *) nts, NTSIZE, 
		sizeof (struct Nt), nt_compar, nt_open, (char **) &slot);
	if (nt)
		return (nt);

	if (!slot) {
		yyerror ("name table full");
		return ((struct Nt *) 0);
	}
	if (!(slot->name = store (name))) {
		yyerror ("can't store name");
		return ((struct Nt *) 0);
	}
	slot->isinput = slot->isoutput = 0;
	slot->outord = slot->inord = -1;
#ifdef DEBUG
	fprintf (stderr, "nt_update: %4d (%d)\t%s\n", 
		slot-nts, hash (name, NTSIZE), name);
#endif
	return (slot);
}

/*
 * find/add new input variable
 */

int
nt_input (name)
char *name;
{
	struct Nt *nt;

	nt = nt_update (name);
	if (!nt)
		return (-1);
	if (nt->isoutput && !(f_flag || s_flag)) {
		yyerror ("input is output");
		return (-1);
	}
	if (nt->isinput)
		return (nt->inord);
	if (ninputs >= NINPUTS) {
		yyerror ("too many inputs");
		return (-1);
	}
	inorder[ninputs] = nt;
	nt->isinput = 1;
	nt->inord = ninputs;
	return (ninputs++);
}

/*
 * find/add new output variable
 */

int
nt_output (name)
char *name;
{
	struct Nt *nt;

	nt = nt_update (name);
	if (!nt)
		return (-1);
	if (nt->isinput && !(f_flag || s_flag)) {
		yyerror ("output is input");
		return (-1);
	}
	if (nt->isoutput)
		return (nt->outord);
	if (noutputs >= NOUTPUTS) {
		yyerror ("too many outputs");
		return (-1);
	}
	outorder[noutputs] = nt;
	nt->isoutput = 1;
	nt->outord = noutputs;
	return (noutputs++);
}

int
nt_compar (nt1, nt2)
char *nt1;
struct Nt *nt2;
{
	return (strcmp (nt1, nt2->name));
}

int
nt_open (nt)
struct Nt *nt;
{
	return (!nt->name);
}
