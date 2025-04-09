SHELL  = bash
TARGET = pc
CC     = clang
YACC   = bison
LEX    = lex

CCFL := -c -g -Wall
LDFL := -Wall -ll -ly

SRCS := error.c list.c tree.c hash.c pc.tab.c pc.c 

# Construct file lists
OBJS := $(addprefix build/obj/,$(patsubst %.c,%.o,$(SRCS)))
DEPS := $(addprefix build/dep/,$(patsubst %.c,%.d,$(SRCS)))
SRCS := $(addprefix src/,$(SRCS))

# All the make rules
.PHONY: all clean install

all: $(TARGET)

src/pc.tab.c src/pc.tab.h: src/pc.y
	$(YACC) -d -v -o src/pc.tab.c src/pc.y

src/pc.c: src/pc.l src/pc.tab.h
	$(LEX) -l -o $@ $<

# Link all objects
$(TARGET): $(OBJS) 
	$(CC) $^ $(LDFL) -o $(TARGET)

build/obj/%.o: src/%.c src/pc.tab.c src/pc.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(CCFL) $< -o $@

build/sobj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $< -o $@

# Clean rule
clean:
	rm -rf build
	rm -f $(TARGET)
	rm -f src/pc.tab.* src/pc.c src/pc.output

