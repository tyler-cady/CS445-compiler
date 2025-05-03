%{

    #pragma GCC diagnostic ignored "-Wunused-function"
    #pragma GCC diagnostic ignored "-Wsign-compare"

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>
    #include "error.h"
    #include "tree.h"
    #include "pc.tab.h"
    int yylineno;
    int yycolumn = 0;
    int tok_start;

    // #define YY_USER_ACTION                    \
    // tok_start  = yylineno;            \
    //                                         \
    // if (yytext[0] == '\n')                  \
    //     yycolumn = 1;                         \
    // else                                    \
    //     yycolumn += strlen(yytext);          \
                                            

    // extern char *yytext;
    extern char *yyfilename;
    extern int yylex();
    extern int yyparse();
    extern int yyerror(const char *s);
    int error_count = 0;
    extern message_context_t error_warning;
    
    
    int verbose_flag = 1;

    void echo(const char *s, int verbose_flag, ...) {
        if (verbose_flag) {
            va_list args;
                va_start(args, verbose_flag);
                if (strstr(s, "%") != NULL) {
                    vfprintf(stderr, s, args);
                } else {
                    fprintf(stderr, "%s", s);
                }

                va_end(args);
        }
    }
%}
%option caseless

number [0-9]+
floating [0-9]+\.[0-9]+ 
name [A-Za-z][A-Za-z0-9]*
yylineno = 1;
yycolumn = 1;

commentbraces (\{.*\})
commentparens (\(\*.*\*\))


%%


{commentbraces} { /* Ignore comment */ }
{commentparens} { /* Ignore comment */ }

"program"       { echo("[PROGRAM]", verbose_flag); return PROGRAM; }
"procedure"     { echo("[PROCEDURE]", verbose_flag); return PROCEDURE; }
"var"           { echo("[VAR]", verbose_flag); return VAR; }
"array"         { echo("[ARRAY]", verbose_flag); return ARRAY; }
"integer"       { echo("[INTEGER]", verbose_flag); return INTEGER; }
"real"          { echo("[REAL]", verbose_flag); return REAL; }
"function"      { echo("[FUNCTION]", verbose_flag); return FUNCTION; }
"end"           { echo("[END]", verbose_flag); return END; }
"if"            { echo("[IF]", verbose_flag); return IF; }
"of"            { echo("[OF]", verbose_flag); return OF; }
"then"          { echo("[THEN]", verbose_flag); return THEN; }
"while"         { echo("[WHILE]", verbose_flag); return WHILE; }
"do"            { echo("[DO]", verbose_flag); return DO; }
"repeat"        { echo("[REPEAT]", verbose_flag); return REPEAT; }
"until"         { echo("[UNTIL]", verbose_flag); return UNTIL; }
"begin"         { echo("[BEGIN]", verbose_flag); return BEGINKW; }
"else"          { echo("[ELSE]", verbose_flag); return ELSE; }
"for"           { echo("[FOR]", verbose_flag); return FOR; }
"not"           { echo("[NOT]", verbose_flag); return NOT; }
"read"          { echo("[READ]", verbose_flag); return READ; }
"write"         { echo("[WRITE]", verbose_flag); return WRITE; }
"+"             { echo("[ADDOP: +]", verbose_flag); yylval.opval = PLUS; return ADDOP; }
"-"             { echo("[ADDOP: -]", verbose_flag); yylval.opval = MINUS; return ADDOP; }
"or"            { echo("[ADDOP: or]", verbose_flag); return OROP; }
"="             { echo("[RELOP: =]", verbose_flag); yylval.opval = EQ; return RELOP; }
"<>"            { echo("[RELOP: <", verbose_flag); yylval.opval = NE; return RELOP; }
"<"             { echo("[RELOP: <]", verbose_flag); yylval.opval = LT; return RELOP; }
"<="            { echo("[RELOP: <=]", verbose_flag); yylval.opval = LE; return RELOP; }
">"             { echo("[RELOP: >]", verbose_flag); yylval.opval = GT; return RELOP; }
">="            { echo("[RELOP: >=]", verbose_flag); yylval.opval = GE; return RELOP; }
"*"             { echo("[MULOP: *]", verbose_flag); yylval.opval = STAR; return MULOP; }
"/"             { echo("[MULOP: /]", verbose_flag); yylval.opval = SLASH; return MULOP; }
"div"           { echo("[MULOP: div]", verbose_flag); yylval.opval = DIV; return MULOP; }
"mod"           { echo("[MULOP: mod]", verbose_flag); yylval.opval = MOD; return MULOP; }
"and"           { echo("[MULOP: and]", verbose_flag); yylval.opval = ANDOP; return ANDOP; }
".."            { echo("[DOUBLEDOT]", verbose_flag); return DOUBLEDOT; }
"to"            { echo("[DOUBLEDOT]", verbose_flag); return DOUBLEDOT; }
":="            { echo("[ASSIGNOP]", verbose_flag); return ASSIGNOP; }
":"             { echo("[:]", verbose_flag); yylval.opval = COLON; return ':'; }
";"             { echo("[;]", verbose_flag); yylval.opval = SEMICOLON; return ';'; }
","             { echo("[,]", verbose_flag); yylval.opval = COMMA; return ','; }
"("             { echo("[(]", verbose_flag); yylval.opval = P; return '('; }
")"             { echo("[)]", verbose_flag); yylval.opval = P; return ')'; }
"["             { echo("[[]", verbose_flag); yylval.opval = B; return '['; }
"]"             { echo("[]]", verbose_flag); yylval.opval = B; return ']'; }
"\""            { echo("[\"]", verbose_flag); return QQ; }
"."             { echo("[.]", verbose_flag); return '.'; }
{number}        { yylval.ival = atoi(yytext); echo("[%d]", verbose_flag, yylval.ival); return INUM; }
{floating}      { yylval.rval = atof(yytext); echo("[%.2f]", verbose_flag, yylval.rval); return RNUM; }
{name}          { yylval.sval = strdup(yytext); echo("[ID:%s]", verbose_flag, yylval.sval); return ID; }
" "             { echo(" ", verbose_flag);}
"\t"            { echo("\t", verbose_flag);}
"\n"            { fprintf(stderr, "\n"); yycolumn = 1;}
"\0"            { return 0;}
.               { echo("[ERROR: %s]",verbose_flag, yytext); yyerror("Invalid token"); }
%%

int yyerror(const char *s) {

	// shortcut: 
	/* fprintf( stderr, "ERROR:%s\n", s );
	exit(1); */

	// BUGGY: the code below ignores syntax errors reported by the parser!

    /* fprintf(stderr, "Error: %s at line %d, column %d\n", s, yylineno, yycolumn); */
    if (strcmp(s, "syntax error") == 0)
        return 0;

    static int last_error_line = 0;
    static char last_error_msg[256] = "";

    if (last_error_line == yylineno && strcmp(last_error_msg, s) == 0)
        return 0;

    last_error_line = yylineno;
    strncpy(last_error_msg, s, sizeof(last_error_msg));
    last_error_msg[sizeof(last_error_msg) - 1] = '\0';

    char *line_content = get_error_text(yylineno, yyin, yyfilename);
    
    if (!line_content) {
        return 0;
    }
    message_enqueue(&error_warning, ERROR, yyfilename, s, line_content, yylineno, yycolumn);

    free(line_content);  
    error_count++;
    return 0;
}

int yywrap()
{
    return 1;
}


