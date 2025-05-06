#ifndef YACC_H
#define YACC_H
#include <stdio.h>
#include "error.h"
#include "hash.h"
#include "reg.h"


extern int yyerror(const char *s);
extern int yylineno;
extern char *yyfilename;
extern message_context_t error_warning;
extern int verbose_flag;
extern int error_count;
extern void echo(const char *s, int verbose_flag, ...);
extern hash_t *symbol_tbl;
extern char *yyfilename; 
// extern reg_t *rstack;


#endif // YACC_H
