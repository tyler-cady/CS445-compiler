#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "consts.h"
#include "tree.h"

/* globals */
FILE *fp;
int current_token;		/* token type */
int current_attribute;	/* token attribute */

/* prototypes*/
int get_token();
void match( int );
int E(), T(), F();
tree_t *Expr(), *Term(), *Factor();
void error(char *);
void aux_print_tree(tree_t *t, int spaces);


/* main driver */
int main( int argc, char *argv[] )
{
	// int value;
	tree_t *tvalue = NULL;
	
	/* open and read input file */
	assert( argc >= 2 );
	fp = fopen( argv[1], "r" );

	/* initialize first token */
	current_token = get_token();

	/* start the parser */
	tvalue = E();	/* calls the start variable in the grammar */

	if ( current_token == EOL ) 
		fprintf( stderr, "\nValue = %d\n", tvalue );

	/* close input file */
	fclose( fp );
}

/* recursive-descent parser */
/* ambiguous grammar:
 *	E -> E '+' E | E '*' E | '(' E ')' | NUM
 * predence-based grammar:
 *  E -> E '+' T | T
 *  T -> T '*' F | F
 *  F -> '(' E ')' | NUM
 * left-recursion removed:
 *  E -> T E1 
 *  E1 -> '+' T E1 | empty
 *  T -> F T1
 *  T1 -> '*' F T1 | empty
 *  F -> '(' E ')' | NUM 
 */

int E()
{
	int value = T();
	while ( current_token == '+' ) {
		match( '+' );
		value += T();
	}
	return value;
}

int T()
{
	int value = F();
	while ( current_token == '*' ) {
		match( '*' );
		value *= F();
	}
	return value;
}

int F()
{
	int value;
	if ( current_token == '(' ) {
		match( '(' );
		value = E();
		match( ')' );
		return value;
	}
	else if ( current_token == NUM ) {
		value = current_attribute;
		match( NUM );
		return value;
	}
	else {
		fprintf( stderr, "ERROR in F(): unexpected token %d\n", current_token );
		exit(1);
	}
}


/* tokenizer */
int get_token()
{
	int c, value;
	while (1) {
		switch( c = fgetc( fp ) ) {	
		/* operators */
		case '+': 
			fprintf( stderr, "[OP:%c]", c );
			return c;
		case '*': 
			fprintf( stderr, "[OP:%c]", c );
			return c;
		/* whitespaces */
		case ' ': case '\t': 
			continue;
		/* brackets */
		case '(': case ')':
			fprintf( stderr, "[%c]", c );
			return c;
		default:
			/* numbers */
			if ( isdigit(c) ) {
				value = 0;
				do {
					value = 10*value + (c - '0');
				} 
				while( isdigit( c = fgetc( fp )));
				ungetc( c, fp );
				current_attribute = value;
				fprintf( stderr, "[NUM:%d]", current_attribute );
				return NUM;
			}
			/* terminators */
			else if ( c == '\n' || c == EOF ) {
				return EOL;
			}
			/* unknown */
			else {
				fprintf( stderr, "[ERROR:%c]", c );
				exit(1);
			}
		}
	}
}


/* match */
void match( int expected_token ) 
{
	if ( current_token == expected_token ) {
		current_token = get_token();
	}
	else {
		fprintf( stderr, "ERROR in match: unexpected token %d\n", current_token );
		exit(1);
	}
}



tree_t *Expr()
{

}
tree_t *Term()
{
	int c;
	tree_t *value = NULL;

	while (current_token == MULOP)
	{
		c = current_attribute;
		match(MULOP);
		value = make_tree(MULOP, c, value, Factor());
	}
	return value;
}
tree_t *Factor()
{
	

}