/* lparse.y */

%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern int yyerror(char *);
%}
%union {
    int ival; /* attribute of num */
}
%token <ival> NUM

%left '+' 
%left '*'

%%
boo: expr '\n'
    ;
expr: expr '+' expr { $$ = $1 + $3; }
    | expr '*' expr { $$ = $1 * $3; }
    | '(' expr ')'  { $$ = $2; }
    | NUM { $$ = $1; }
    ;
%%

int main()
{
    yyparse();
}