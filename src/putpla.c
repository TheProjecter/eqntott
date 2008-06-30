/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include <stdio.h>
#include "x.h"

extern int ninputs, noutputs;
extern int f_flag;
extern struct Nt nts[], *inorder[], *outorder[];

putpla (pts, npts)
PTERM *pts[];
int npts;
/*
 * Print and- and or-plane matrix values.
 */
{
	register int i;
	int cmppt ();

	qsort (pts, npts, sizeof (PTERM *), cmppt);

	for (i = 0; i < npts; i++)
		putpt (pts[i]);

	return;
}

putpt (pt)
register PTERM *pt;
{
	register int j;
	int spaceout;
	static char inc[3] = {'0', '1', '-'};
	static char outc[3] = {'0', '1', 'x'};

	spaceout = ninputs + 2 + 2 * noutputs <= LINELEN;
	
	for (j = 0; j < ninputs; j++)
		printf ("%c", inc[pt->ptand[j]]);
	printf ("  ");
	for (j = 0; j < noutputs; j++)
		if (spaceout)
			printf ("%c ", outc[pt->ptor[j]]);
		else	printf ("%c", outc[pt->ptor[j]]);
#ifdef DEBUG
	printf ("  hash=%ld, index=%d, cv=%d", (long) pt->andhash,
	    (int) pt->index, (int) pt->cv);
#endif
	printf ("\n");
	return;
}

putrpla (pts, npts)
PTERM *pts[];
int npts;
/*
 * Print and- and or-plane matrix values in human readable form.
 */
{
	register int i, o, n;
	int cmppt ();

	printf ("%d inputs, %d outputs, %d product terms.\n",
		ninputs, noutputs, npts);
	if (f_flag) {
		printf ("feedback:");
		for (i = 0; i < ninputs; i++)
			if (inorder[i]->isoutput)
				printf (" %s", inorder[i]->name);
		printf ("\n");
	}

#ifdef DEBUG
	fprintf (stderr, "putrpla: qsort ...");
#endif
	qsort (pts, npts, sizeof (PTERM *), cmppt);
#ifdef DEBUG
	fprintf (stderr, " done. putrpla npts = %d ...", npts);
#endif

	for (o = 0; o < noutputs; o++) {
		for (n = 0; n < npts; n++) {
			if (pts[n]->ptor[o] == 0)
				continue;
			if (pts[n]->ptor[o] == 2)
				printf ("(%s)", outorder[o]->name);
			else	printf ("%s", outorder[o]->name);
			for (i = 0; i < ninputs; i++)
				switch (pts[n]->ptand[i]) {
				case 0:
					printf ("\t!%s", inorder[i]->name);
					break;

				case 1:
					printf ("\t%s", inorder[i]->name);
					break;

				}
			printf ("\n");
		}
	}

#ifdef DEBUG
	fprintf (stderr, " done.\n");
#endif
	return;
}

putSpla (pts, npts)
PTERM *pts[];
int npts;
/*
 * print summary of pla product terms
 */
{
	FILE *fp = stdout, *popen();
	register int i, o, n;
	int count;

	printf ("PLA Connectivity Summary\n");
	printf ("\n#pterms\tinput\n");
	if (!(fp = popen ("sort -nr", "w"))) {
		fprintf (stderr, "can't write to sort\n");
		return;
	}
	fflush (stdout);
	for (i = 0; i < ninputs; i++) {
		for (count = n = 0; n < npts; n++)
			count += pts[n]->ptand[i] != 2;
		fprintf (fp, "%d\t%s\n", count, inorder[i]->name);
	}
	pclose (fp);
	printf ("\n#pterms\toutput\n");
	if (!(fp = popen ("sort -nr", "w"))) {
		fprintf (stderr, "can't write to sort\n");
		return;
	}
	fflush (stdout);
	for (o = 0; o < noutputs; o++) {
		for (count = n = 0; n < npts; n++)
			count += pts[n]->ptor[o] != 0;
		fprintf (fp, "%d\t%s\n", count, outorder[o]->name);
	}
	pclose (fp);
	printf ("\n#out\t#in\tproduct term expression\n");
	if (!(fp = popen ("sort -nr", "w"))) {
		fprintf (stderr, "can't write to sort\n");
		return;
	}
	fflush (stdout);
	for (n = 0; n < npts; n++) {
		for (count = o = 0; o < noutputs; o++)
			count += pts[n]->ptor[o] != 0;
		fprintf (fp, "%d\t", count);
		for (count = i = 0; i < ninputs; i++)
			count += pts[n]->ptand[i] != 2;
		fprintf (fp, "%d\t", count);
		for (i = 0; i < ninputs; i++) {
			switch (pts[n]->ptand[i]) {
			case 0:
				fprintf (fp, "!%s", inorder[i]->name);
				if (--count > 0)
					fprintf (fp, " & ");
				break;
			case 1:
				fprintf (fp, "%s", inorder[i]->name);
				if (--count > 0)
					fprintf (fp, " & ");
				break;
			}
		}
		fprintf (fp, "\n");
	}
	pclose (fp);
	return;
}
