%{
#include <stdio.h>
#include <stdlib.h>
#include "calc.tab.h"  
%%

"PROGRAM"       { return PROGRAM; }
"BEGIN"         { return BEGINKW; }
"END"           { return END; }
"VAR"           { return VAR; }
"INTEGER"       { return INTEGER; }

":="            { return ASSIGNOP; }
":"             { return ':'; }
";"             { return ';'; }
","             { return ','; }
"+"             { return ADDOP; }
"*"             { return MULOP; }
"("             { return '('; }
")"             { return ')'; }
"."             { return '.'; }

[0-9]+          { yylval.ival = atoi(yytext); return NUM; }
[A-Za-z][A-Za-z0-9]* { return ID; }

[ \t\n]         { ; }


%%

int main() {
    yylex();  
    int res = eval_tree(root);
    return 0;
}

int yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
    return 0;
}
