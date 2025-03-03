/* calc.l*/

%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "y.tab.h"
%}

number [0-9]+
whitespace [ \t]+
%%
"+"                     {printf("[ADDOP:%c]\n", yytext[0]);     return yytext[0];}
"*"                     {printf("[MULOP:%c]\n", yytext[0]);     return yytext[0];}
"("                     {printf("[%c]\n", yytext[0]);           return yytext[0];}
")"                     {printf("[%c]\n", yytext[0]);           return yytext[0];}
{number}                {printf("[NUM:%d]\n", yylval.ival = atoi(yytext));    return NUM;}
{whitespace}            { ; }
\n                      {fprintf(stderr, "[%c]", yytext[0]);    return yytext[0];}
.                       {fprintf(stderr, "[%c]", yytext[0]);    return yytext[0];}   
%%
int yerror (char *msg)
{
    fprintf(stderr,"Error:%s\n",*msg);
    exit(1);
}
/* int main()
{
    while(1)
    {
        int curr = yylex();
        fprintf(stderr, "curr = %d\n", yylex());

    }
    return 0;

} */