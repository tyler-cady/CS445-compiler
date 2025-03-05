CC = gcc
CFLAGS = -Wall -Wextra
LEX = flex
YACC = bison -d -v

SRC = pc.l pc.y
OBJ = lex.yy.o pc.tab.o
EXEC = parser

VERBOSE = 0  # Set this to 1 for verbose output

ifeq ($(VERBOSE), 1)
    Q =
else
    Q = @  # Suppresses output unless there's an error
endif

all: $(EXEC)

$(EXEC): $(OBJ)
	$(Q)$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

pc.tab.c pc.tab.h: pc.y
	$(Q)$(YACC) pc.y

lex.yy.c: pc.l pc.tab.h
	$(Q)$(LEX) pc.l

pc.tab.o: pc.tab.c
	$(Q)$(CC) $(CFLAGS) -c pc.tab.c

lex.yy.o: lex.yy.c
	$(Q)$(CC) $(CFLAGS) -c lex.yy.c

clean:
	$(Q)rm -f $(OBJ) pc.tab.c pc.tab.h lex.yy.c $(EXEC)
