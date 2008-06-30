/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#ifndef __X_H
#define __X_H

#include <stdlib.h>

#define MAXSTR 512

#define NINPUTS 128
#define NOUTPUTS 128
#define NPTERMS 65536
#define NTSIZE NINPUTS+NOUTPUTS
#define NSUBSTITUTE 10

#define AND -1
#define OR -2
#define NOT -3
#define ONE -4
#define ZERO -5
#define DONTCARE -6
#define NAND -7
#define NOR -8

#define MIN_OUTPUT 512		/* inputs start at 0, outputs at 512 */
#define MIN_GNODE 1024		/* and internal nodes at 1024 */

#define INPUT 1			/* code to decide what to print in prgexpr */
#define OUTPUT 2
#define INTERNAL 3

typedef short BIT;

struct Nt {
	char *name;		/* variable name */
	int isinput;		/* variable used as input? */
	int isoutput;		/* variable used as output? */
	int inord;		/* index into inorder */
	int outord;		/* index into outorder */
};

typedef struct Pterm {
	BIT *ptand;			/* AND-plane connections */
	BIT *ptor;			/* OR-plane connections */
	struct Pterm *next;		/* link to next product term */
	long andhash;			/* hash of input connection values */
	short index;			/* number of 1's in ptand */
	short cv;			/* "covered" flag */
} PTERM;

typedef struct Btreenode {
	int value;			/* operator name or leaf value */
	struct Btreenode *left;		/* link to left subtree */
	struct Btreenode *right;	/* link to right subtree */
#ifdef DEBUG
	int mark;
#endif
} BNODE;

#define NIL_BNODE ((BNODE *) 0)
#define NIL_PTERM ((PTERM *) 0)

#define or3(a,b)  (a==0?b:(b==0?a:(a==1||b==1?1:2)))
#define and3(a,b) (a==1?b:(b==1?a:(a==0||b==0?0:2)))
#define LINELEN 70

BNODE *new_bnode(int value, BNODE *left, BNODE *right);
void free_bnode(BNODE *bnode);

#endif
