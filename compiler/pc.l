%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "pc.tab.h"
    extern int yylineno;
    extern int yylex();
    extern int yyparse();
    extern int yyerror(const char *s);
%}

%x COMMENT

number [0-9]+
floating [0-9]+\.[0-9]+ 
name [A-Za-z][A-Za-z0-9]*
whitespaces [ \t]+
yylineno = 1;
%%

"(*"            { BEGIN(COMMENT); }
<COMMENT>.      { /* Ignore comments */ }
<COMMENT>\n     { /* Ignore newlines in comments */ }
<COMMENT>"*)"   { BEGIN(INITIAL); }

"program"       { fprintf(stderr, "[PROGRAM]"); return PROGRAM; }
"procedure"     { fprintf(stderr, "[PROCEDURE]"); return PROCEDURE; }
"var"           { fprintf(stderr, "[VAR]"); return VAR; }
"array"         { fprintf(stderr, "[ARRAY]"); return ARRAY; }
"integer"       { fprintf(stderr, "[INTEGER]"); return INTEGER; }
"real"          { fprintf(stderr, "[REAL]"); return REAL; }
"function"      { fprintf(stderr, "[FUNCTION]"); return FUNCTION; }
"end"           { fprintf(stderr, "[END]"); return END; }
"if"            { fprintf(stderr, "[IF]"); return IF; }
"of"            { fprintf(stderr, "[OF]"); return OF; }
"then"          { fprintf(stderr, "[THEN]"); return THEN; }
"while"         { fprintf(stderr, "[WHILE]"); return WHILE; }
"do"            { fprintf(stderr, "[DO]"); return DO; }
"repeat"        { fprintf(stderr, "[REPEAT]"); return REPEAT; }
"until"         { fprintf(stderr, "[UNTIL]"); return UNTIL; }
"begin"         { fprintf(stderr, "[BEGIN]"); return BEGINKW; }
"else"          { fprintf(stderr, "[ELSE]"); return ELSE; }
"for"           { fprintf(stderr, "[FOR]"); return FOR; }
"not"           { fprintf(stderr, "[NOT]"); return NOT; }
"+"             { fprintf(stderr, "[ADDOP: +]\n"); return ADDOP; }
"-"             { fprintf(stderr, "[ADDOP: -]\n"); return ADDOP; }
"or"            { fprintf(stderr, "[ADDOP: or]\n"); return ADDOP; }
"="             { fprintf(stderr, "[RELOP: =]\n"); return RELOP; }
"<>"            { fprintf(stderr, "[RELOP: <>]\n"); return RELOP; }
"<"             { fprintf(stderr, "[RELOP: <]\n"); return RELOP; }
"<="            { fprintf(stderr, "[RELOP: <=]\n"); return RELOP; }
">"             { fprintf(stderr, "[RELOP: >]\n"); return RELOP; }
">="            { fprintf(stderr, "[RELOP: >=]\n"); return RELOP; }
"*"             { fprintf(stderr, "[MULOP: *]\n"); return MULOP; }
"/"             { fprintf(stderr, "[MULOP: /]\n"); return MULOP; }
"div"           { fprintf(stderr, "[MULOP: div]\n"); return MULOP; }
"mod"           { fprintf(stderr, "[MULOP: mod]\n"); return MULOP; }
"and"           { fprintf(stderr, "[MULOP: and]\n"); return MULOP; }
".."            { fprintf(stderr, "[DOUBLEDOT]\n"); return DOUBLEDOT; }
":="            { fprintf(stderr, "[ASSIGNOP]\n"); return ASSIGNOP; }
":"             { fprintf(stderr, "[:]\n"); return ':'; }
";"             { fprintf(stderr, "[;]\n"); return ';'; }
","             { fprintf(stderr, "[,]\n"); return ','; }
"("             { fprintf(stderr, "[(]\n"); return '('; }
")"             { fprintf(stderr, "[)]\n"); return ')'; }
"["             { fprintf(stderr, "[[]\n"); return '['; }
"]"             { fprintf(stderr, "[]]\n"); return ']'; }
"."             { fprintf(stderr, "[.]\n"); return '.'; }
{number}        { yylval.ival = atoi(yytext); fprintf(stderr, "[%d]", atoi(yytext)); return INUM; }
{floating}      { yylval.fval = atof(yytext); fprintf(stderr, "[%.2f]", atof(yytext)); return NUM; }
{name}          { yylval.ival = atoi(yytext); return ID; }
{whitespaces}   { /* Ignore whitespaces */ }
"\n"            { yylineno++; }
%%

extern int yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s Line: %d\n", s, yylineno);
    exit(1);
}


int yywrap()
{
    return 1;
}

