%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include "list.h"
#include "tree.h"
#include "ptype.h"
#include "util.h"

#define MAX_ERRORS 20

int yylex();       /* Declare yylex() */
extern int yylineno;
extern FILE *yyin;
char *yyfilename;
// void yyerror(const char *s); /* Declare yyerror() */
int error_count = 0;
tree_t *id_ptr;
hash_t *symbol_tbl;
%}


%union {
    int ival;
    float rval;
    int opval;
    char *sval;
    tree_t *tval;       /* tree value */
    ptype_t *type_val; /* type value */
    ltype_t *ltype_val; /* list of types */
    // lname_t *lname_val; /* list of names */x
};

/* %token PROGRAM FUNCTION PROCEDURE */
%token <opval> PLUS MINUS OR
%token <opval> EQ NE LT GT LE GE
%token <opval> STAR SLASH DIV MOD AND 
%token <opval> COMMA P B SEMICOLON COLON


%token <sval> BEGINKW END
%token <ival> IF WHILE FOR REPEAT
%token <sval> THEN ELSE UNTIL DO 


%token <ival> INUM
%token <rval> RNUM
%token <sval> ID

%token <opval> RELOP
%token <opval> ADDOP
%token <opval> MULOP
%token <opval> ASSIGNOP

%token <sval> DOUBLEDOT
%token <sval> VAR
%token <sval> INTEGER
%token <sval> REAL
%token <sval> ARRAY OF

%token <sval> PROGRAM
%token <ival> FUNCTION
%token <ival> PROCEDURE 

%type <tval> start
%type <tval> program
%type <tval> compound_statement
%type <tval> optional_statements
%type <tval> statement_list
%type <tval> statement
%type <tval> variable
%type <tval> procedure_statement

%type <tval> expression_list
%type <tval> expression
%type <tval> simple_expression
%type <tval> term
%type <tval> factor
%type <tval> matched_statement
%type <tval> identifier_list
%type <type_val> type
%type <type_val> standard_type
%type <tval> unmatched_statement
%type <tval> arguments
%type <tval> parameter_list
%type <tval> declarations
%type <tval> subprogram_declarations
%type <tval> subprogram_declaration
%type <tval> subprogram_header
%type <tval> range



%left ADDOP MULOP RELOP NOT
%left THEN ELSE
%start start 
%%
start 
    : program
    { 
        tree_print($1, 0); 
    }
    ;
program
    : PROGRAM ID '(' identifier_list ')' ';' declarations subprogram_declarations compound_statement '.'
    { $$ = NULL;}
    | error { yyerror("Syntax Error");}
    ;

identifier_list 
    : ID 
    { 
        $$ = tree_make_str($1);  
        hash_insert(symbol_tbl, $1);
    }

    | identifier_list ',' ID 
    { $$ = tree_make_op(COMMA, $1, tree_make_str($3)); }
    ;

declarations 
    : declarations VAR identifier_list ':' type ';' 
    {
        $$ = tree_make_op(COLON, tree_make_op(VAR, $1, $3), $5);
        
    }
    | /* empty */   { $$ = NULL;}
    ;

type 
    : standard_type { $$ = $1; }
    | ARRAY '[' range ']' OF standard_type 
    { 
        // $$ = tree_make( ARRAY, $3, $6, NULL ); 
        $$ = NULL;
    }
    ;

range 
    : INUM DOUBLEDOT INUM
    { 
        // $$ = tree_make_op(RANGE, $1, $3); 
        // $$ = tree_make_op(DOUBLEDOT, $1, $3);
    }
    ;

standard_type 
    : INTEGER 
    { 
        // $$ = tree_make(INTEGER, NULL, NULL, NULL); 
        $$ = NULL;
    }
    | REAL 
    {
        // $$ = tree_make(REAL, NULL, NULL, NULL);
        $$ = NULL;
    }
    ;

subprogram_declarations 
    : subprogram_declarations subprogram_declaration ';' 
    {
        // $$ = tree_make(ID , $1, NULL, NULL);
        $$ = NULL;
    }
    | /* empty */ { $$ = NULL; }
    ;

subprogram_declaration 
    : subprogram_header declarations subprogram_declarations compound_statement
    { 
        // $$ = tree_make(SUBPROGRAM, $1, $2, $3, $4); 
        $$ = NULL;
    } 
    ;

subprogram_header 
    : FUNCTION ID arguments ':' standard_type ';' 
    { 
        // $$ = tree_make(FUNCTION, tree_make_name($2), $3, $5, NULL);
        $$ =  NULL;    
    }
    | PROCEDURE ID arguments ';'
    { 
        // $$ = tree_make(PROCEDURE, tree_make_name($2), $3, NULL, NULL); 
        $$ = NULL;
    }
    ;

arguments 
    : '(' parameter_list ')' 
    { $$ = $2; }
    | /* empty */ { $$ = NULL; }
    ;

parameter_list 
    : identifier_list COLON type
    { 
        $$ = tree_make_op(COLON, $1, $3);
    }
    | parameter_list SEMICOLON identifier_list COLON type
    { 
        $$ = tree_make_op(SEMICOLON, $1, $3);
    }
    ;

compound_statement 
    : BEGINKW optional_statements END
    { 
        tree_t *t = tree_make_str("begin-end");
        t->left = $2;
        t->right = NULL;
        $$ = t;  
    }
    ;

optional_statements 
    : statement_list 
    { $$ = $1; }
    | /* empty */ 
    { $$ = NULL; }
    ;

statement_list
    : statement
    { $$ = $1; }
    | statement_list ';' statement
    { 
        $$ = tree_make_op(SEMICOLON, $1, $3);
    }
    ;

statement
    : variable ASSIGNOP expression
    { 
        $$ = tree_make_op( ASSIGNOP, $1, $3 );
    }
    | procedure_statement
    { 
        $$ = $1;
    }
    | compound_statement
    { $$ = $1; }
    | IF expression THEN matched_statement ELSE matched_statement
    {  
        /*TODO*/
        tree_t *t = tree_make_str("if-else");
        t->left = $2;  /* Conditional */
        t->right = tree_make_op(IF, $4, $6);  /* branches */
        $$ = t;
    }
    | IF expression THEN unmatched_statement
    { 
        tree_t *t = tree_make_str("if");
        t->left = $2;  /* condition */
        t->right = $4; /* then branch */
        $$ = t;
    }
    | WHILE expression DO statement
    { 
        tree_t *t = tree_make_str("while-do");
        t->left = $2;
        t->right = $4;
        $$ = t;
    }
    | REPEAT statement UNTIL expression
    { 
        tree_t *t = tree_make_str("repeat-until");
        t->left = $2;
        t->right = $4;
        $$ = t;
    }
    | FOR ID ASSIGNOP range DO statement
    { 
        tree_t *t = tree_make_str("for");
        t->left = tree_make_op($3, $2, $4);
        t->right = tree_make_str("do");
        t->right->right = $6;
        // t->right->left = $9;
        $$ = t;
    }
    ;
    
matched_statement
    : IF expression THEN statement ELSE statement
    {
        tree_t *t = tree_make_str("if-else");
        t->left = $2;  
        t->right = tree_make_op(THEN, $4, $6); 
        $$ = t;
    }
    | IF expression THEN matched_statement
    {
        tree_t *t = tree_make_str("if");
        t->left = $2;  
        t->right = $4; 
        $$ = t;
    }
    ;

unmatched_statement
    : IF expression THEN statement
    { 
        tree_t *t = tree_make_str("if");
        t->left = $2;  
        t->right = $4; 
        $$ = t;
    }
    ;


variable 
    : ID
    { $$ = $1; }
    | ID '[' expression ']'
    {
        $$ = tree_make_op(B, $1, $3); 
    }
    ;

procedure_statement 
    : ID 
    {
        $$ = $1;
    }
    | ID '(' expression_list ')'
    {
        $$ = tree_make_op(P, $1, $3);
    }
    ;

expression_list 
    : expression
    { $$ = tree_make_op(COMMA, NULL, $1); }
    | expression_list ',' expression
    { $$ = tree_make_op(COMMA, $1, $3); }
    ;

expression 
    : simple_expression
    { $$ = $1; }
    | simple_expression EQ simple_expression
    { $$ = tree_make_op( EQ, $1, $3 ); }
    | simple_expression NE simple_expression
    { $$ = tree_make_op( NE, $1, $3 ); }
    | simple_expression LT simple_expression
    { $$ = tree_make_op( LT, $1, $3 ); }
    | simple_expression LE simple_expression
    { $$ = tree_make_op( LE, $1, $3 ); }
    | simple_expression GT simple_expression
    { $$ = tree_make_op( GT, $1, $3 ); }
    | simple_expression GE simple_expression
    { $$ = tree_make_op( GE, $1, $3 ); }
    ;

simple_expression
    : term
    { $$ = $1; }
    | PLUS term
    { $$ = tree_make_op(PLUS, tree_make_inum(0), $2); }
    | MINUS term
    { $$ = tree_make_op(MINUS, tree_make_inum(0), $2); }
    | simple_expression PLUS term
    { $$ = tree_make_op(PLUS, $1, $3); }
    | simple_expression MINUS term
    { $$ = tree_make_op(MINUS, $1, $3); }
    | simple_expression OR term
    { $$ = tree_make_op(OR, $1, $3); } 
;


term 
    : factor
    { $$ = $1; }
    | term STAR factor
    { $$ = tree_make_op(STAR, $1, $3); }
    | term SLASH factor
    { $$ = tree_make_op(SLASH, $1, $3); }
    | term DIV factor 
    { $$ = tree_make_op(DIV, $1, $3); }
    | term MOD factor 
    { $$ = tree_make_op(MOD, $1, $3); }
    | term AND factor
    { $$ = tree_make_op(AND, $1, $3); }
    ;

factor 
    : ID 
    { $$ = $1; }
    | ID '(' expression_list ')'
    {
        $$ = tree_make_op( P, $1, $3 );
    }
    | ID '[' expression ']'
    {
        $$ = tree_make_op( B, $1, $3 );
    }
    | INUM
    { $$ = tree_make_inum( $1 ); }
    | RNUM
    { $$ = tree_make_inum( $1 );}
    | '(' expression ')' { $$ = $2;}
    | NOT factor { $$ = tree_make_op(NOT, $2, NULL); }
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
