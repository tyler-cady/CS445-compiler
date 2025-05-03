%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "util.h"
#include "error.h"
#include "list.h"
#include "tree.h"
#include "hash.h"
#include "semantic.h"

#define MAX_ERRORS 1

extern int yylex();       /* Declare yylex() */
extern int yylineno;
extern FILE *yyin;
char *yyfilename;
// void yyerror(const char *s); /* Declare yyerror() */
extern int error_count;
// tree_t *tree;
hash_t *symbol_tbl;
list_t *list;
message_context_t error_warning = {0};
int main(int argc, char *argv[]);
%}

%union {
    int ival;
    float rval;
    int opval;
    char *sval;
    tree_t *tval;       /* tree value */
};

/* %token PROGRAM FUNCTION PROCEDURE */
%token <opval> PLUS MINUS OR 
%token <opval> EQ NE LT GT LE GE
%token <opval> STAR SLASH DIV MOD AND NOT
%token <opval> COMMA P B SEMICOLON COLON 
%token <ival> RARRAY IARRAY
%token FUNC_CALL STMT_LIST CMPND_STMT DECLS SUBPROG_DECLS PARAM_LIST 
%token RANGE EXPR_LIST PROC_CALL ARRAY_CALL SUBPROG_DECL FOR_ARG ID_LIST
%token PARAMETER LOCAL
%token READ WRITE QQ

%token VOID
%token BEGINKW END
%token IF WHILE FOR REPEAT
%token THEN ELSE UNTIL DO 
%token DECL

%token <ival> INUM
%token <rval> RNUM
%token <sval> ID


%token <opval> RELOP
%token <opval> ADDOP
%token <opval> MULOP
%token <opval> ANDOP
%token <opval> OROP
%token ASSIGNOP

%token DOUBLEDOT
%token  VAR
%token <ival> INTEGER REAL BOOL
%token ARRAY OF

%token PROGRAM

%token FUNCTION
%token PROCEDURE 
%token SUBPROGRAM
%token PROG_DECL1 PROG_DECL2


%type <tval> start
%type <tval> program 
%type <tval> program_decl
%type <tval> declarations
%type <ival> type
%type <tval> range
%type <tval> subprogram_declarations
%type <tval> statement 
%type <tval> expression
%type <tval> or_exp and_exp rel_exp add_exp mul_exp unary_exp
%type <tval> factor
%type <tval> variable
%type <tval> expression_list
%type <tval> procedure_statement
%type <tval> compound_statement
%type <tval> identifier_list
%type <ival> standard_type
%type <tval> subprogram_declaration subprogram_header
%type <tval> arguments parameter_list
%type <tval> optional_statements statement_list 
%type <opval> UMINUS 

%nonassoc ELSEFIX
%left OROP
%left ANDOP
%nonassoc RELOP
%left ADDOP
%left MULOP
%left THEN ELSE
%right NOT 
%left ASSIGNOP
%left '['
%left '('
%right UMINUS


%start start

%%

start: program 
    { 
        tprint($1, 0);
		$$ = NULL;
    } 
    ;  
program: PROGRAM ID '(' identifier_list ')' ';' program_decl
    {
        list = hash_insert( symbol_tbl, $2);
        tree_t* program_id = tmake_id( list );
        sem_set_types( program_id, PROCEDURE, PROCEDURE );
        $$ = tmake( PROG_DECL1, program_id, $7 );
    }
    ;

program_decl: declarations subprogram_declarations compound_statement
    {
        $$ = tmake( PROG_DECL1, $1, tmake( PROG_DECL2, $2, $3 ));
    }
    ;

identifier_list: ID 
    {
        list = hash_insert( symbol_tbl, $1 );
        $$ = tmake( ID_LIST, NULL, tmake_id( list ));
    }
    | identifier_list ',' ID 
    {  

        list = hash_insert( symbol_tbl, $3 );
        $$ = tmake( ID_LIST, $1, tmake_id( list ));
    }   
    ;

declarations: declarations VAR identifier_list ':' type ';' 
    {
        sem_set_types($3, $5, LOCAL);     
        $$ = tmake( DECLS, $1, $3); 
    }
    | { $$ = NULL; } /* empty */   
    ;

type: standard_type { $$ = $1; }
    | ARRAY '[' range ']' OF standard_type
    { 
        // fprintf(stderr, "[R-ARRAY]\n");
        char *msg;
        switch( $6 ) {
            case INTEGER:
                $$ = IARRAY;
                break;
            case REAL:
                $$ = RARRAY;
                break;
            default:
                asprintf(&msg, "semantic error: invalid array type: %d", $6);
                yyerror(msg);
                free(msg);
                break;
        }
    }
   ;

range: '[' expression DOUBLEDOT expression ']' 
    {
        // fprintf(stderr, "[R-RANGE]");
        tree_t *low = $2;
        tree_t *high = $4;
        char *msg;
        if (!sem_assert_types(low, high, INTEGER)){
            asprintf(&msg, "semantic error: array indicies must be of type integer");
            yyerror(msg);
            free(msg);
        }
        if (low > high) {
            asprintf(&msg, "semantic error: array indicies must be in increasing order");
            yyerror(msg);
            free(msg);
        }
        $$ = tmake(RANGE, $2, $4);
    }
    ;

standard_type: INTEGER 
    { 
        $$ = INTEGER; 
    }
    | REAL 
    { 
        $$ = REAL; 
    }
    ;

subprogram_declarations: subprogram_declarations subprogram_declaration ';' 
    { 
        // fprintf(stderr, "[R-SUBPROGRAM_DECLARATIONS]");
        $$ = tmake(SUBPROG_DECLS, $1, $2); 
    }
    | {$$ = NULL;} /* empty */
    ;

subprogram_declaration: 
    subprogram_header 
    declarations 
    subprogram_declarations 
    compound_statement 
    { 
        // fprintf(stderr, "[R_SUBPROGRAM_DECLARATION]");
        symbol_tbl = hash_pop(symbol_tbl);
        $$ = tmake(SUBPROGRAM, $1, $2);
        $$->left = $3;
        $$->right = $4;
        $$->type = $1->type;

    }
    ;

subprogram_header: FUNCTION ID 
    { 
        fprintf(stderr, "**func id insert**");
        list = hash_insert( symbol_tbl, $2 ); 
        symbol_tbl = hash_push(symbol_tbl); 
    }
    arguments ':' standard_type ';'
    { 
        if ( is_declared_in_scope(symbol_tbl, $2) ) {
            char *msg;
            asprintf(&msg, "semantic error: function %s already declared", $2);
            yyerror(msg);
            free(msg);
        }

        list = hash_search(symbol_tbl, $2);
        tree_t *t = tmake_id( list );
        sem_set_types(t, $6, FUNCTION);

        // t->type = $6;
        $$ = tmake( FUNCTION, t , $4);
        $$->type = $6;


    } 
    | PROCEDURE ID 
    { 
        // fprintf(stderr, "[R-SUBPROGRAM_HEADER]\n");
        if ( !is_declared_in_scope(symbol_tbl, $2) ) {
            char *msg;
            asprintf(&msg, "semantic error: procedure %s already declared", $2);
            yyerror(msg);
            free(msg);
        }
        list = hash_insert(symbol_tbl, $2);
        symbol_tbl = hash_push( symbol_tbl); 
    } 
    arguments ';' 
    { 
        if ( !is_declared_in_scope(symbol_tbl, $2) ) {
            char *msg;
            asprintf(&msg, "semantic error: procedure %s already declared", $2);
            yyerror(msg);
            free(msg);
        }
        list = hash_search(symbol_tbl, $2);
        tree_t *t = tmake_id( list );
        sem_set_types(t, PROCEDURE, PROCEDURE);
        $$ = tmake( PROCEDURE, t , $4);
    }
    ;

arguments: '(' parameter_list ')' { $$ = $2; }
    | { $$ = NULL; }
    ;

parameter_list: identifier_list ':' type 
    {
        // fprintf(stderr, "[R-PARAMETER_LIST]\n");
        sem_set_types($1, $3, PARAMETER);
        $$ = tmake( PARAM_LIST, $1, NULL );
    }
    | parameter_list ';' identifier_list ':' type 
    {
        // fprintf(stderr, "[R-PARAMETER_LIST]\n" );
        sem_set_types($3, $5, PARAMETER);     
        $$ = tmake( PARAM_LIST, $1, $3 );
    }
    ;

compound_statement: BEGINKW optional_statements END
    { 
        // fprintf(stderr, "[R-COMPOUND_STATEMENT]\n");
        $$ = tmake( CMPND_STMT, $2, NULL ); 
    }
    ;

optional_statements: statement_list 
    { 
        // fprintf(stderr, "[R-OPTIONAL_STATEMENTS]\n");
        $$ = $1; 
    }
    | /* empty */ { $$ = NULL; }
    ;

statement_list: statement_list ';' statement 
    {
        // fprintf(stderr, "[R-STATEMENT_LIST]\n");
        $$ = tmake( STMT_LIST, $1, $3 );
    }
    | statement 
    { 
        // fprintf(stderr, "[R-STATEMENT_LIST]\n");
        $$ = tmake( STMT_LIST, NULL, $1 ); 
    }
    ;

statement: variable ASSIGNOP expression 
    {
        if (sem_check_assign($1, $3)) $$ = tmake( ASSIGNOP, $1, $3 );
        else $$ = NULL;
    }
    | procedure_statement 
    {
        // fprintf(stderr, "[R-PROCEDURE_STATEMENT]");
        $$ = $1;
    }
    | compound_statement 
    {
        // fprintf(stderr, "[R-COMPOUND_STATEMENT]\n");
        $$ = $1;
    }
    | IF expression THEN statement ELSE statement 
    { 
        // fprintf(stderr, "[R-IF_STATEMENT]\n");
        if (sem_get_type($2) != BOOL) {
            char *msg;
            asprintf(&msg, "semantic error: if condition must be of type boolean");
            yyerror(msg);
            free(msg);
        }
        $$ = tmake(IF, $2, tmake(THEN, $4, tmake(ELSE, $6, NULL)));
    }
    | IF expression THEN statement %prec ELSEFIX 
    { 
        // fprintf(stderr, "[R-IF_STATEMENT]\n");
        if (sem_get_type($2) != BOOL) {
            char *msg;
            asprintf(&msg, "semantic error: if condition must be of type boolean");
            yyerror(msg);
            free(msg);
        }
        $$ = tmake(IF, $2, tmake(THEN, $4, NULL));
    }
    | WHILE expression DO statement 
    { 
        // fprintf(stderr, "[R-WHILE_STATEMENT]\n");
        if (sem_get_type($2) != BOOL) {
            char *msg;
            asprintf(&msg, "semantic error: while condition must be of type boolean");
            yyerror(msg);
            free(msg);
        }
        $$ = tmake( WHILE, $2, $4 ); 
    }
    | REPEAT statement UNTIL expression 
    { 
        // fprintf(stderr, "[R-REPEAT_STATEMENT]\n");
        $$ = tmake( REPEAT, $2, $4 ); 
    }
    | FOR ID ASSIGNOP expression range DO statement  
    { 
        // fprintf(stderr, "[R-FOR_STATEMENT\n]");
        list = hash_search(symbol_tbl, $2);
        if (is_declared_in_scope(symbol_tbl, $2)) {
            char *msg;
            asprintf(&msg, "semantic error: variable %s not declared", $2);
            yyerror(msg);
            free(msg);
        }
        $$ = tmake( FOR, tmake( FOR_ARG, tmake_id( list ), $4), $7 );
    }
    ;

variable: ID '[' expression ']' 
    { 
        // fprintf(stderr, "[R-VARIABLE]\n");
        if (is_declared(symbol_tbl, $1)) {
            char *msg;
            asprintf(&msg, "semantic error: variable %s not declared", $1);
            yyerror(msg);
            free(msg);
        }
        if ( sem_get_type($3) != INTEGER){
            char *msg;
            asprintf(&msg, "semantic error: array index must be an integer recieved %d", sem_get_type($3));
            yyerror(msg);
            free(msg);
        }
        $$ = tmake( ARRAY_CALL, tmake_id( hash_search_all( symbol_tbl, $1)), $3);
    }
    | ID 
    { 
        // fprintf(stderr, "[R-VARIABLE]\n");
       list = hash_search_all(symbol_tbl, $1);
		assert(list != NULL);
       if ( !is_declared( symbol_tbl, $1 ) ) {
            char *msg;
            asprintf(&msg, "semantic error: variable %s not declared", $1);
            yyerror(msg);
            free(msg);
        }
        // fprintf(stderr, "rule is here");
        $$ = tmake_id(list);
        

    }
    ;

procedure_statement: ID '(' expression_list ')' {
        char *msg;
        if ( !is_declared( symbol_tbl, $1 ) ) {
            asprintf(&msg, "semantic error: procedure '%s' not declared", $1);
            yyerror(msg);
            free(msg);
        }
        
        tree_t *id = tmake_id(hash_search_all(symbol_tbl, $1));
        $$ = tmake(PROC_CALL, id , $3);
    }
    | ID 
    { 
        char *msg;
        if ( !is_declared( symbol_tbl, $1 )) {
            asprintf(&msg, "semantic error: procedure '%s' not declared", $1);
            yyerror(msg);
            free(msg);
        }
        $$ = tmake_id(hash_search_all(symbol_tbl, $1));

    }
    ;

expression_list: expression_list ',' expression { $$ = tmake(EXPR_LIST, $1, $3);}
    | expression
    { 
        $$ = tmake(EXPR_LIST, $1, NULL);
    } 
    ;

expression: or_exp { $$ = $1; }
    ;

or_exp: or_exp OROP and_exp 
    { 
        char *msg;
        if ( !sem_assert_types($1, $3, BOOL)){
            asprintf(&msg, "semantic error: 'or' operands must be of the type boolean");
            yyerror(msg);
            free(msg);
        }
        $$ = tmake_addop($2, $1, $3); 
    }
    | and_exp { $$ = $1; }
    ;
and_exp: and_exp ANDOP rel_exp 
    {   
        char *msg;
        if ( !sem_assert_types($1, $3, BOOL)){
            asprintf(&msg, "semantic error: 'and' operands must be of type boolean");
            yyerror(msg);
            free(msg);
        }
        $$ = tmake_mulop($2, $1, $3); 
    }
    | rel_exp { $$ = $1; }
    ;
rel_exp: rel_exp RELOP add_exp 
    { 
        char *msg;
        if ( sem_check_types($1, $3)){
            asprintf(&msg, "semantic error: '%s' operands must be of the same type", type_to_str( $2 ));
            yyerror(msg);
            free(msg);
        }
        $$ = tmake_relop($2, $1, $3); 
    }
    | add_exp { $$ = $1; }
    ;
add_exp: add_exp ADDOP mul_exp 
    {
        char *msg;
        if ( sem_check_types($1, $3)){
            asprintf(&msg, "semantic error: '%s' operands must be of the same type", type_to_str( $2 ));
            yyerror(msg);
            free(msg);
        }
        $$ = tmake_addop($2, $1, $3); 
    }
    | mul_exp { $$ = $1; }
    ;
mul_exp: mul_exp MULOP factor 
    { 
        char *msg;
        if ( sem_check_types($1, $3)){
            asprintf(&msg, "semantic error: '%s' operands must be of the same type", type_to_str( $2 ));
            yyerror(msg);
            free(msg);
        } 
        $$ = tmake_mulop($2, $1, $3); 
    }
    | unary_exp { $$ = $1; }
    ;
unary_exp: ADDOP factor %prec UMINUS
    { 
        if ($2->type == INTEGER || $2->type == REAL) {
            $$ = tmake_addop($1, $2, NULL);
        } 
        else {
            char *msg;
            asprintf(&msg, "semantic error: unary operator %s not applicable to type %s", type_to_str( $1 ), type_to_str($2->type));
            yyerror(msg);
            free(msg);
        }
        $$ = tmake_addop($1, $2, NULL); 
    }
    | NOT factor %prec NOT 
    { 
        if ($2->type == BOOL) {
            $$ = tmake( NOT, $2, NULL );
        } 
        else {
            char *msg;
            asprintf(&msg, "semantic error: unary operator %s not applicable to type %s", type_to_str( $1 ), type_to_str($2->type));
            yyerror(msg);
            free(msg);
        }
        $$ = tmake( NOT, $2, NULL ); 
    }
    | factor { $$ = $1; }
    ;


factor:ID 
    {
        list_t *entry = hash_search_all(symbol_tbl, $1);
        if (!entry) {
            char *msg;
            asprintf(&msg, "semantic error: ID %s not declared", $1);
            yyerror(msg);
            free(msg);
        }
        $$ = tmake_id(entry);
    }
    | ID '(' expression_list ')' 
    {
        list_t *entry = hash_search_all(symbol_tbl, $1);
        if (!entry || entry->type != FUNCTION) {
            char *msg;
            asprintf(&msg, "semantic error: %s is not a function", $1);
            yyerror(msg);
            free(msg);
        }
        tree_t *id = tmake_id(entry);
        $$ = tmake(FUNC_CALL, id, $3);
    }
    | ID '[' expression ']' 
    {
        list_t *entry = hash_search_all(symbol_tbl, $1);
        if (!entry || (entry->type != IARRAY && entry->type != RARRAY)) {
            char *msg;
            asprintf(&msg, "semantic error: %s is not an array", $1);
            yyerror(msg);
            free(msg);
        }
        tree_t *id = tmake_id(entry);
        $$ = tmake(ARRAY_CALL, id, $3);
    }
    | INUM 
    {
        tree_t *t = tmake_inum($1); 
        fprintf(stderr, "Inum->%d\n", t->type);
        $$ = t;
    }
    | RNUM 
    {
        tree_t *t = tmake_rnum($1); 
        fprintf(stderr, "RNum->%d\n", t->type);
        $$ = t;
    }
    | '(' expression ')' 
    {
        $$ = $2;
    }
	;

/* in_list: ID
    | in_list ',' ID
    ;
out_list: out_token
    | out_list ',' out_token
    ;
out_token: ID
    | INUM
    | RNUM
    | QQ ID QQ
    ;
io_rule: WRITE '(' out_list ')'
    | READ '(' in_list ')'
    ; */


%%

int main(int argc, char *argv[]) {
    symbol_tbl = hash_push( symbol_tbl );
    /* hash_insert_procedure("read", symbol_tbl);
    hash_insert_procedure("write", symbol_tbl); */
    fprintf(stderr, "%d", argc);
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
    /* Parse the input */
    echo("\n\n****************TOKENS*****************\n\n", verbose_flag);
    //do {
        yyparse();
    //} while ( !feof(yyin) );
    if (error_count > 0) {
        fprintf(stderr, "Parsing failed with %d errors\n", error_count);
        print_all_messages(&error_warning);
        exit(1);
    }
    fprintf( stderr, "Parsing succeeded\n" );

	symbol_tbl = hash_pop( symbol_tbl );
    return 0;
}
