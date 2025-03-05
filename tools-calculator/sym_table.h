#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "consts.h"



typedef struct var_entry
{
    char *key;
    int value;
    struct var_entry *next;
} var_entry_t;

#define TABLE_SIZE 100
typedef struct hashmap
{
    var_entry_t *table[TABLE_SIZE];
} hashmap_t;

typedef struct scope_stack
{
    hashmap_t *scope[100];
    int top;
} scope_stack_t;



unsigned int hash_pjw(char *);
hashmap_t *create_hashmap();
void enter_scope();
void exit_scope();
int *lookup(char *);
void set_var(char *, int);
