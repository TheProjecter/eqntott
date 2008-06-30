/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

%{
#include "x.h"
#include <stdio.h>
#include <ctype.h>
BNODE *new_bnode(), *copy_bnode();
extern char yytext[];
extern BNODE *exprs[];
extern int noutputs;
%}

%union {
	BNODE *bnodeval;
	int intval;
}

%type <intval> input output;
%type <bnodeval> expr
%left '|'
%left '&' 
%token NAME INORDER OUTORDER PLANAME SYMONE SYMZERO
%start prog

%%

prog	:	stat
	|	stat prog
	;

stat	:	error ';'
	|	INORDER '=' inlist ';'
	|	OUTORDER '=' outlist ';'
	|	PLANAME '=' planame ';'
	|	output '=' expr ';'
			{
			  if (exprs[$1])
				exprs[$1] = new_bnode (OR, $3, exprs[$1]);
			  else	exprs[$1] = $3;
			}
	;

inlist	:	input
	|	inlist input
	;

outlist	:	output
	|	outlist output
	;

expr	:	expr '|' expr
			{ $$ = new_bnode (OR, $1, $3); }
	|	expr '&' expr
			{ $$ = new_bnode (AND, $1, $3); }
	|	'!' expr	%prec '&'
			{ $$ = new_bnode (NOT, $2, NIL_BNODE); }
	|	input
			{ $$ = new_bnode ($1, NIL_BNODE, NIL_BNODE); }
	|	SYMZERO
			{ $$ = new_bnode (ZERO, NIL_BNODE, NIL_BNODE); }
	|	'0'
			{ $$ = new_bnode (ZERO, NIL_BNODE, NIL_BNODE); }
	|	SYMONE
			{ $$ = new_bnode (ONE, NIL_BNODE, NIL_BNODE); }
	|	'1'
			{ $$ = new_bnode (ONE, NIL_BNODE, NIL_BNODE); }
	|	'?'
			{ $$ = new_bnode (DONTCARE, NIL_BNODE, NIL_BNODE); }
	|	'(' expr ')'
			{ $$ = $2; }
	;

output	:	NAME
			{ $$ = nt_output (yytext); }
	;

input	:	NAME
			{ $$ = nt_input (yytext); }
	;

planame	:	NAME
			{ set_planame (yytext); }
	;

%%
