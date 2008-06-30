/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

/*
 * This file contains functions for maintaining the binary tree
 * bnode space.  Space for bnodes is obtained dynamically in chunks of
 * CHUNK bnodes at a time.  Returned space is never returned to the
 * system, but is placed on a free list for later use.
 *
 * static get_free ()
 *	gets a bunch of bnodes and adds them to the free list.
 * BNODE *new_bnode(int value, BNODE *left, BNODE *right)
 *	picks a bnode from the free list, and returns a pointer to it.
 *	the new bnode is initialized from the parameters.
 * free_bnode (BNODE *bnode)
 *	puts bnode back on the free list and (recursively) its descendants.
 */

#include <stdio.h>
#include <stdlib.h>
#include "x.h"
#include "hdr.h"
#define CHUNK 1024
static BNODE *freePtr = 0;

static
get_free ()
{
	register BNODE *bnodes;
	register int i;

	freePtr = bnodes = (BNODE *) malloc (CHUNK * sizeof (BNODE));
	if (bnodes == NULL) {
		fprintf (stderr, "can't malloc for Btreenode\n");
		exit (1);
	}
	for (i = 0; i < CHUNK-1; i++)
		bnodes[i].right = &bnodes[i+1];
	bnodes[CHUNK-1].right = 0;
	return;
}

BNODE *
new_bnode (value, left, right)
int value;
BNODE *left, *right;
{
	register BNODE *bnode;

	if (!freePtr)
		get_free ();
	bnode = freePtr;
	freePtr = freePtr->right;
	bnode->value = value;
	bnode->left = left;
	bnode->right = right;
	return (bnode);
}

void free_bnode (bnode)
register BNODE *bnode;
{
	if (!bnode)
		return;
	bnode->right = freePtr;
	freePtr = bnode;
	return;
}

BNODE *
copy_bnode (old)
register BNODE *old;
{
	register BNODE *left, *right;

#ifdef DEBUG
	check_bnode (old);
#endif
	left = old->left ? copy_bnode (old->left) : NIL_BNODE;
	right = old->right ? copy_bnode (old->right) : NIL_BNODE;
	return (new_bnode (old->value, left, right));
}

#ifdef DEBUG
check_bnode (b)
BNODE *b;
{
	static int mark = 0, depth = 0;

	if (!depth++)
		mark++;

	if (b->mark == mark) {
		fprintf (stderr, "CYCLIC BNODE GRAPH\n");
		exit (1);
	}
	b->mark = mark;
	switch (b->value) {
	case ZERO:
	case ONE:
	case DONTCARE:
		break;
	case AND:
	case OR:
		check_bnode (b->left);
		check_bnode (b->right);
		break;
	case NOT:
		check_bnode (b->left);
		break;
	default:
		break;
	}
	depth--;
	return;
}
#endif
