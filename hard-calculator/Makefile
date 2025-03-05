CC = gcc
FLAGS = -g

calc: calc.o tree.o sym_table.o
	$(CC) $(FLAGS) -o calc calc.o tree.o sym_table.o

calc.o: calc.c tree.h
	$(CC) $(FLAGS) -c calc.c

tree.o: tree.c tree.h
	$(CC) $(FLAGS) -c tree.c

sym_table.o: sym_table.c sym_table.h
	$(CC) $(FLAGS) -c sym_table.c

clean:
	rm -f calc calc.o tree.o sym_table.o
