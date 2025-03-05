%{
#include <stdio.h>
#include <stdlib.h>
int yylex();       /* Declare yylex() */
extern int yylineno;
void yyerror(const char *s); /* Declare yyerror() */

%}

%token PROGRAM ID VAR ARRAY OF INTEGER REAL FUNCTION PROCEDURE
%token BEGINKW END IF THEN ELSE WHILE DO REPEAT UNTIL FOR ASSIGNOP
%token INUM RELOP ADDOP MULOP DOUBLEDOT NUM NOT 
%union {
    int ival;
    float fval;
}
%left ADDOP MULOP RELOP NOT

%%
program
    : PROGRAM ID '(' identifier_list ')' ';' declarations subprogram_declarations compound_statement '.'
    | error { yyerror("Syntax Error:"); }
;


identifier_list 
    : ID 
    | identifier_list ',' ID
    ;

declarations 
    : declarations VAR identifier_list ':' type ';' 
    | /* empty */
    ;

type 
    : standard_type 
    | ARRAY '[' range ']' OF standard_type
    ;

range 
    : INUM DOUBLEDOT INUM
    ;

standard_type 
    : INTEGER 
    | REAL
    ;

subprogram_declarations 
    : subprogram_declarations subprogram_declaration ';' 
    | /* empty */
    ;

subprogram_declaration 
    : subprogram_header declarations subprogram_declarations compound_statement
    ;

subprogram_header 
    : FUNCTION ID arguments ':' standard_type ';' 
    | PROCEDURE ID arguments ';'
    ;

arguments 
    : '(' parameter_list ')' 
    | /* empty */
    ;

parameter_list 
    : identifier_list ':' type
    | parameter_list ';' identifier_list ':' type
    ;

compound_statement 
    : BEGINKW optional_statements END
    ;

optional_statements 
    : statement_list
    | /* empty */
    ;

statement_list 
    : statement
    | statement_list ';' statement
    ;

statement
    : variable ASSIGNOP expression
    | procedure_statement
    | compound_statement
    | IF expression THEN matched_statement ELSE matched_statement
    | IF expression THEN unmatched_statement
    | WHILE expression DO statement
    | REPEAT statement UNTIL expression
    | FOR ID ASSIGNOP range DO statement
    ;

matched_statement
    : IF expression THEN matched_statement ELSE matched_statement
    | statement  /* other non-conditional statements */
;

unmatched_statement
    : IF expression THEN statement
;


variable 
    : ID
    | ID '[' expression ']'
    ;

procedure_statement 
    : ID 
    | ID '(' expression_list ')'
    ;

expression_list 
    : expression
    | expression_list ',' expression
    ;

expression 
    : simple_expression
    | simple_expression RELOP simple_expression
    ;

simple_expression 
    : term
    | ADDOP term
    | simple_expression ADDOP term
    ;

term 
    : factor
    | term MULOP factor
    ;

factor 
    : ID
    | ID '(' expression_list ')'
    | ID '[' expression ']'
    | INUM
    | NUM
    | '(' expression ')'
    | NOT factor
    ;
%%

int main() {
    
    if (yyparse() == 0) {
        printf("Parsing Successful!\n");
    } else {
        printf("Parsing Failed!\n");
    }
    return 0;
}

