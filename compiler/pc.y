%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX_ERRORS 20

int yylex();       /* Declare yylex() */
extern int yylineno;
extern FILE *yyin;
char *yyfilename;
void yyerror(const char *s); /* Declare yyerror() */
int error_count = 0;

%}

%token PROGRAM ID VAR ARRAY OF INTEGER REAL FUNCTION PROCEDURE
%token BEGINKW END IF THEN ELSE WHILE DO REPEAT UNTIL FOR ASSIGNOP
%token INUM RELOP ADDOP MULOP DOUBLEDOT NUM NOT 
%token ERROR_TOKEN

%union {
    int ival;
    float fval;
}
%left ADDOP MULOP RELOP NOT
%left THEN ELSE
%%
program
    : PROGRAM ID '(' identifier_list ')' ';' declarations subprogram_declarations compound_statement '.'
    | error { yyerror("Syntax Error");}
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
    : IF expression THEN statement ELSE statement
    | IF expression THEN matched_statement  /* other non-conditional statements */
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



int main(int argc, char *argv[]) {
    if (argc > 1) {
        yyfilename = argv[1];  // Set filename from command-line argument
        char *valid_extensions[] = {".pas", ".p", ".pp", ".inc", ".pascal"};
        int valid = 0;

        for (int i = 0; i < 5; i++) {
            if (strstr(yyfilename, valid_extensions[i]) != NULL) {
                yyin = fopen(yyfilename, "r");
                valid = 1;
                break;
            }
        }

        if (!valid) {
            fprintf(stderr, "Invalid file extension: %s\n", yyfilename);
            return 1;
        }

        if (!yyin) {
            fprintf(stderr,"Error opening file\n");
            return 1;
        }
    } 
    else {
        yyfilename = "input"; 
        yyin = stdin;  
    }

    if (yyparse() == 0) {
        printf("Parsing Finished.\n");
        if (error_count > 0) {
            fprintf(stderr, "Parsing Completed with %d errors.\n", error_count);
        }
    } 
    else {
        fprintf(stderr, "Parsing Failed.\n");
    }
    
    return 0;
}
