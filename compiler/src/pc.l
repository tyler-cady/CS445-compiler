%{
    #pragma GCC diagnostic ignored "-Wunused-function"
    #pragma GCC diagnostic ignored "-Wsign-compare"

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>
    #include "ptype.h"
    #include "error.h"
    #include "tree.h"
    #include "pc.tab.h"

    extern int yylineno;
    // extern char *yytext;
    extern char *yyfilename;
    extern int yylex();
    extern int yyparse();
    extern int yyerror(const char *s);
    
    int verbose_flag = 0;

    message_context_t error_warning;


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


number [0-9]+
floating [0-9]+\.[0-9]+ 
name [A-Za-z][A-Za-z0-9]*
whitespaces [ \t]+
yylineno = 1;
%x COMMENT
%%

"(*"            { BEGIN(COMMENT); }
<COMMENT>.      { /* Ignore comments */ }
<COMMENT>\n     { /* Ignore newlines in comments */ }
<COMMENT>"*)"   { BEGIN(INITIAL); }

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
"+"             { echo("[ADDOP: +]\n", verbose_flag); yylval.opval = PLUS; return ADDOP; }
"-"             { echo("[ADDOP: -]\n", verbose_flag); yylval.opval = MINUS; return ADDOP; }
"or"            { echo("[ADDOP: or]\n", verbose_flag); yylval.opval = OR; return ADDOP; }
"="             { echo("[RELOP: =]\n", verbose_flag); yylval.opval = EQ; return RELOP; }
"<>"            { echo("[RELOP: <>]\n", verbose_flag); yylval.opval = NE; return RELOP; }
"<"             { echo("[RELOP: <]\n", verbose_flag); yylval.opval = LT; return RELOP; }
"<="            { echo("[RELOP: <=]\n", verbose_flag); yylval.opval = LE; return RELOP; }
">"             { echo("[RELOP: >]\n", verbose_flag); yylval.opval = GT; return RELOP; }
">="            { echo("[RELOP: >=]\n", verbose_flag); yylval.opval = GE; return RELOP; }
"*"             { echo("[MULOP: *]\n", verbose_flag); yylval.opval = STAR; return MULOP; }
"/"             { echo("[MULOP: /]\n", verbose_flag); yylval.opval = SLASH; return MULOP; }
"div"           { echo("[MULOP: div]\n", verbose_flag); yylval.opval = DIV; return MULOP; }
"mod"           { echo("[MULOP: mod]\n", verbose_flag); yylval.opval = MOD; return MULOP; }
"and"           { echo("[MULOP: and]\n", verbose_flag); yylval.opval = AND; return MULOP; }
".."            { echo("[DOUBLEDOT]\n", verbose_flag); return DOUBLEDOT; }
":="            { echo("[ASSIGNOP]\n", verbose_flag); return ASSIGNOP; }
":"             { echo("[:]\n", verbose_flag); yylval.opval = COLON; return ':'; }
";"             { echo("[;]\n", verbose_flag); yylval.opval = SEMICOLON; return ';'; }
","             { echo("[,]\n", verbose_flag); yylval.opval = COMMA; return ','; }
"("             { echo("[(]\n", verbose_flag); yylval.opval = P; return '('; }
")"             { echo("[)]\n", verbose_flag); yylval.opval = P; return ')'; }
"["             { echo("[[]\n", verbose_flag); yylval.opval = B; return '['; }
"]"             { echo("[]]\n", verbose_flag); yylval.opval = B; return ']'; }
"."             { echo("[.]\n", verbose_flag); return '.'; }
{number}        { yylval.ival = atoi(yytext); echo("[%d]\n", verbose_flag, yylval.ival); return INUM; }
{floating}      { yylval.rval = atof(yytext); echo("[%.2f]\n", verbose_flag, yylval.rval); return RNUM; }
{name}          { yylval.sval = yytext; echo("[ID:%s]\n", verbose_flag, yylval.sval); return ID; }
{whitespaces}   { /* Ignore whitespaces */ }
"\n"            { yylineno++; }
"\0"            { return 0;}
.               { echo("[ERROR: %s]\n",verbose_flag, yytext); yyerror("Invalid token"); }
%%



// extern int yyerror(const char *s) {
//     static int last_error_line = -1;
//     if (last_error_line == yylineno) return 0; 
//     last_error_line = yylineno;
//     fseek(yyin, 0, SEEK_SET); /* Reset file pointer to the beginning */
//     char buffer[1024];  // Buffer to store the line
//     int current_line = 1;

//     while (fgets(buffer, sizeof(buffer), yyin) && current_line < yylineno) {
//         current_line++;
//     }

//     buffer[strcspn(buffer, "\n")] = 0;

//     int width = snprintf(NULL, 0, "%d", yylineno);
//     char *pos = strstr(buffer, yytext);
//     int error_col = (pos) ? (pos - buffer) + 1 : 1; // Default to 1 if not found
//     int error_len = strlen(yytext);  /* bad token length */
//     fprintf(stderr, "%s:%d: \033[1;31merror:\033[0m %s: '%s'\n", 
//             yyfilename ? yyfilename : "input", yylineno, s, yytext);
//     fprintf(stderr, "%*d | %s\n", width, yylineno, buffer);  
//     fprintf(stderr, "%*s |\033[0;32m", width, "");  

//     for (int i = 0; i < error_col - 1; i++) fprintf(stderr, " ");
    
//     fprintf(stderr, " ^");
//     for (int i = 1; i < error_len; i++) fprintf(stderr, "~");
//     fprintf(stderr, "\033[0m\n"); 
//     error_count++;
//     return 1;
// }

extern int yyerror( const char *s ){
    static int last_error_line = -1;
    if (last_error_line == yylineno) return 0;
    last_error_line = yylineno;
    char *error = get_error_text(yylineno, yyin, yyfilename);
    /* if s begins with 1, it is a warning */
    if (s[0] == '1') {
        // remove the first character from s
        s = ( s && *s) ? s + 1 : s;
        message_enqueue(&error_warning, 1, yyfilename, s, error, yylineno);
    } 
    else if (s[0] == '2') { /* error note */
        s = ( s && *s) ? s + 1 : s;
        message_enqueue(&error_warning, 2, yyfilename, s, error, yylineno);
    } 
    else {
        message_enqueue(&error_warning, 0, yyfilename, s, error, yylineno);
    }
    free(error);
    return 0;
}
int yywrap()
{
    return 1;
}


