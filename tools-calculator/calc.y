%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
tree_t *root = NULL;
extern int yyerror(char *);
extern int yylex(void);
%}

%union {
    int ival;
    char *id;
    tree_t *node;
};

%token <ival> NUM
%token <id> ID
%token PROGRAM VAR BEGINKW
%token <ival> ASSIGNOP INTEGER MULOP ADDOP
%token END

%left ADDOP
%left MULOP

%%
program:
    PROGRAM ID ';' declarations compound_statement '.' 
    ;

declarations:
    declarations VAR identifier_list ':' type ';'
    | 
    ;

identifier_list:
    ID
    ;

type:
    standard_type
    ;

standard_type:
    INTEGER
    ;

compound_statement:
    BEGINKW { enter_scope(); } optional_statements END { exit_scope(); }
    ;

optional_statements:
    statement_list
    | 
    ;

statement_list:
    statement 
    ;

statement:
    variable ASSIGNOP expression { node = make_op(':=', $1, $2); }
    ;

variable:
    ID { make_var($1->var_name); }
    ;

expression:
    simple_expression 
    ;

simple_expression:
    simple_expression ADDOP term { node = make_op('+', $1->left, $2->right); }
    | term                 
    ;

term:
    term MULOP factor { node = make_op('*', $1, $2); }
    | factor                   
    ;

factor:
    '(' expression ')'
    | NUM { node = make_num($1); }
    ;
%%
