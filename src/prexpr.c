/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include <stdio.h>
#include "x.h"

prexpr (exp)
BNODE *exp;
{
	extern struct Nt nts[];
	extern struct Nt *inorder[];

	switch (exp->value) {
	case ZERO:
		fprintf (stderr, "ZERO");
		break;

	case ONE:
		fprintf (stderr, "ONE");
		break;

	case DONTCARE:
		fprintf (stderr, "?");
		break;

	case NOT:
		fprintf (stderr, "!");
		prexpr (exp->left);
		break;

	case AND:
		fprintf (stderr, "(");
		prexpr (exp->left);
		fprintf (stderr, "&");
		prexpr (exp->right);
		fprintf (stderr, ")");
		break;

	case OR:
		fprintf (stderr, "(");
		prexpr (exp->left);
		fprintf (stderr, "|");
		prexpr (exp->right);
		fprintf (stderr, ")");
		break;

	default:
		fprintf (stderr, "%s", inorder[exp->value]->name);
		break;
	}

	return;
}

prlexpr (exp)
BNODE *exp;
{
	extern struct Nt nts[];
	extern struct Nt *inorder[];

	switch (exp->value) {
	case ZERO:
		printf ("ZERO");
		break;

	case ONE:
		printf ("ONE");
		break;

	case DONTCARE:
		printf ("DONT-CARE");
		break;

	case NOT:
		printf ("(NOT ");
		prlexpr (exp->left);
		printf (")");
		break;

	case AND:
		printf ("(AND ");
		prlexpr (exp->left);
		printf (" ");
		prlexpr (exp->right);
		printf (")");
		break;

	case OR:
		printf ("(OR ");
		prlexpr (exp->left);
		printf (" ");
		prlexpr (exp->right);
		printf (")");
		break;

	default:
		printf ("'%s", inorder[exp->value]->name);
		break;
	}

	return;
}

prgexpr (exp, node, kind)
BNODE *exp;
{
	extern struct Nt nts[];
	extern struct Nt *inorder[], *outorder[];

	int lnum, rnum, atom_left, atom_right;

	switch (exp->value) {
	case ZERO:
		printf ("0");
		break;

	case ONE:
		printf ("1");
		break;

	case DONTCARE:
		printf ("DONT-CARE");
		break;

	case NOT:
		printf("NOT");
		prnode( node, kind );
		if( exp->left->left == NIL_BNODE ) {
			prnode( exp->left->value, INPUT );
		}
		else {
			node = new_num();
			prnode( node, INTERNAL );
			prgexpr( exp->left, node, INTERNAL );
		}
		printf("\n");
		break;

	case AND:
	case OR:
	case NAND:
	case NOR:
		switch (exp->value) {
		case AND:
			printf("AND");
			break;
		case OR:
			printf("OR");
			break;
		case NAND:
			printf("NAND");
			break;
		case NOR:
			printf("NOR");
			break;
		}
		
		atom_left = atom_right = 0;
		if( exp->left->left == NIL_BNODE ) {
			lnum = exp->left->value;
			atom_left++;
		}
		else
			lnum = new_num();
		if( exp->right->left == NIL_BNODE ) {
			rnum = exp->right->value;
			atom_right++;
		}
		else
			rnum = new_num();
		
		prnode( node, kind );
		prnode( lnum, atom_left ? INPUT : INTERNAL );
		prnode( rnum, atom_right ? INPUT : INTERNAL );
		printf( "\n" );
		if( !atom_left )
			prgexpr( exp->left, lnum, INTERNAL );
		if( !atom_right )
			prgexpr( exp->right, rnum, INTERNAL );
		break;
	default:
		printf ("'%s", inorder[exp->value]->name);
		break;
	}

	return;
}

prnode( num, kind )
{
  extern struct Nt *inorder[], *outorder[];
	switch( kind ) {
	case INPUT:
		switch( num ) {
		case ZERO:
			printf( " ZERO" );
			break;
		case ONE:
			printf( " ONE" );
			break;
		case DONTCARE:
			printf( " DONTCARE");
			break;
		default:
			printf( " %s", inorder[num]->name );
			break;
		}
		break;
	case OUTPUT:
		printf( " %s", outorder[num]->name );
		break;
	case INTERNAL:
		printf( " %d", num );
		break;
	default:
		printf( "internal error: unknown kind in prnode\n" );
	}
}

static int c_node = MIN_GNODE;

new_num() {
	return( ++c_node );
}
