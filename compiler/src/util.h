#ifndef YACC_H
#define YACC_H
#include <stdio.h>


void yyerror(const char *s);
extern int yylineno;
extern int yylineno;
extern char *yyfilename;

extern FILE *yyin;
extern FILE *yyout;
#endif // YACC_H