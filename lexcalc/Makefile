CC = gcc 
FLAGS = -g 
YACC = bison
# YACC = yacc
LEX = lex 
LEXFLAGS = -l
YACCFLAGS = -dv
lycalc: calc.tab.o lex.yy.o
	$(CC) $(FLAGS) -o lycalc calc.tab.o lex.yy.o -ll -ly

calc.tab.o: calc.tab.c 
	$(CC) $(FLAGS) -c calc.tab.c 
lex.yy.o: lex.yy.c 
	$(CC) $(FLAGS)-c lex.yy.c 
calc.tab.c: calc.y 
	$(YACC) $(YACCFLAGS) calc.y
lex.yy.c: calc.l
	$(LEX) $(LEXFLAGS) calc.l 

clean:
	rm -f lycalc lex.yy.*  calc.tab.* calc.vcg calc.output