CC = gcc 
FLAGS = -g 
YACC = bison
LEX = lex 
LEXFLAGS = -l
YACCFLAGS = -dv

lycalc: calc.tab.o lex.yy.o sym_table.o tree.o
	$(CC) $(FLAGS) -o lycalc calc.tab.o lex.yy.o sym_table.o tree.o -ll -ly

calc.tab.o: calc.tab.c 
	$(CC) $(FLAGS) -c calc.tab.c 

lex.yy.o: lex.yy.c calc.tab.h
	$(CC) $(FLAGS) -c lex.yy.c 

sym_table.o: sym_table.c sym_table.h
	$(CC) $(FLAGS) -c sym_table.c

tree.o: tree.c tree.h
	$(CC) $(FLAGS) -c tree.c

calc.tab.c calc.tab.h: calc.y
	$(YACC) $(YACCFLAGS) calc.y

lex.yy.c: calc.l
	$(LEX) $(LEXFLAGS) calc.l 

clean:
	rm -f lycalc lex.yy.* calc.tab.* calc.vcg calc.output sym_table.o tree.o
