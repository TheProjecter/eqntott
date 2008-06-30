/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

/*
 * "canon" rewrites an expression tree by applying a few Boolean
 * rules so that the "!" operator only is applied to (leaf) variables.
 */

#include "x.h"

BNODE *
canon (exp)
BNODE *exp;
{
	BNODE *tBnode;

	switch (exp->value) {
	case AND:
	case OR:
		exp->left = canon (exp->left);
		exp->right = canon (exp->right);
		return (exp);

	case NOT:
		/*
		 * we assume that
		 * 1) free_bnode does not recursively free left and right
	 	 * 2) free_bnode doesn't clobber the left ptr.
		 */

		switch (exp->left->value) {
		case NOT:
			/* Double Negation */
			tBnode = exp->left->left;
			free_bnode (exp);
			free_bnode (exp->left);
			tBnode = canon( tBnode );
			return (tBnode);

		case ONE:
			/* !ONE == ZERO */
			free_bnode (exp);
			exp->left->value = ZERO;
			return (exp->left);

		case ZERO:
			/* !ZERO == ONE */
			free_bnode (exp);
			exp->left->value = ONE;
			return (exp->left);

		case DONTCARE:
			/* !DONTCARE == DONTCARE */
			/* read as ZERO for now */
			free_bnode (exp);
			exp->left->value = ZERO;
			return (exp->left);

		case AND:
			/* DeMorgan's Theorem */
			exp->value = OR;
			exp->left->value = NOT;
			exp->right = new_bnode (NOT, exp->left->right, 
				NIL_BNODE);
			exp->left = canon (exp->left);
			exp->right = canon (exp->right);
			return (exp);

		case OR:
			/* DeMorgan's Theorem */
			exp->value = AND;
			exp->left->value = NOT;
			exp->right = new_bnode (NOT, exp->left->right, 
				NIL_BNODE);
			exp->left = canon (exp->left);
			exp->right = canon (exp->right);
			return (exp);

		default:
			return (exp);
		}

	default:
		return (exp);
	}
}

#define ZERO_BNODE new_bnode( 0, NIL_BNODE, NIL_BNODE )

BNODE *
ecanon (exp)
BNODE *exp;
{
	BNODE *tBnode;

	switch (exp->value) {
	case OR:
	case NOR:
		exp->left = ecanon (exp->left);
		exp->right = ecanon (exp->right);
		return (exp);

	case AND:
		/* change (and a b) to (nor !a !b) */
		exp->value = NOR;
		exp->left = new_bnode( NOT, exp->left, NIL_BNODE );
		exp->right = new_bnode( NOT, exp->right, NIL_BNODE );
		exp->left = ecanon(exp->left);
		exp->right = ecanon(exp->right);
		return(exp);
	
	case NOT:
		/*
		 * we assume that
		 * 1) free_bnode does not recursively free left and right
	 	 * 2) free_bnode doesn't clobber the left ptr.
		 */

		switch (exp->left->value) {
		case NOT:
			/* Double Negation */
			tBnode = exp->left->left;
			free_bnode (exp);
			free_bnode (exp->left);
			tBnode = ecanon(tBnode);
			return( tBnode );

		case ONE:
			/* !ONE == ZERO */
			free_bnode (exp);
			exp->left->value = ZERO;
			return (exp->left);

		case ZERO:
			/* !ZERO == ONE */
			free_bnode (exp);
			exp->left->value = ONE;
			return (exp->left);

		case DONTCARE:
			/* !DONTCARE == DONTCARE */
			/* read as ZERO for now */
			free_bnode (exp);
			exp->left->value = ZERO;
			return (exp->left);

		case AND:
			/* DeMorgan's Theorem */
			exp->value = OR;
			exp->left->value = NOT;
			exp->right = new_bnode (NOT, exp->left->right, 
				NIL_BNODE);
			exp->left = ecanon (exp->left);
			exp->right = ecanon (exp->right);
			return (exp);

		case OR:
			exp->left->value = NOR;
			exp->left = ecanon( exp->left );
			free_bnode (exp);
			return (exp->left);
		default:
			return (exp);
		}

	default:
		return (exp);
	}
}
