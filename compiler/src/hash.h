#pragma once
#include <stdio.h>
#include "list.h"
#include <stdlib.h>

#define MAX_CAPACITY 512


typedef struct hash_s
{
    /* Hash Table */
    list_t *table[MAX_CAPACITY];         /* Array of linked list heads ( hash table )*/
    list_t *scope_owner;
    size_t capacity;                      /* Number of buckets in the hash table */
    char *label;
    int local_ct; /* Number of local vars */
    int param_ct; /* Number of params */
    struct hash_s *next;
} hash_t;

/* Constructor */
hash_t *hash_make(); 

/* Destructor */
void hash_free( hash_t * );

void hash_init( hash_t *top );
void hash_exit( hash_t *top );
unsigned int hash_pjw( char *name, size_t table_size);

list_t *get_id_list( hash_t *t);

list_t *hash_search( hash_t *table, char *name );
list_t *hash_global_search( hash_t *table, char *name ); 
list_t *hash_search_all_depth( hash_t *table, char *name, int *depth );

list_t *hash_insert( hash_t *table, char *name );
list_t *hash_linsert( hash_t *table, list_t *name );
list_t *hash_typed_insert( hash_t *table, char *name, int type );
list_t *hash_insert_function( hash_t *table, char *name, int type, int arg_count, int *arg_types );
list_t *hash_insert_procedure( hash_t *table, char *name, int arg_count, int *arg_types );
list_t *hash_insert_array( hash_t *table, char *name, int type, int start_index, int end_index );
list_t *hash_set_type(hash_t *table, char *name, int type, int scopetype);
list_t *hash_add_bounds(hash_t *table, char *name, int start_index, int end_index);
list_t *hash_init_symbol(hash_t *table, char *name);

int hash_get_type( hash_t *table, char *name );
/* Stack Operations */
hash_t *hash_pop( hash_t *top );
hash_t *hash_push( hash_t *top ); 
hash_t *hash_top( hash_t *top );
void hash_print( hash_t *table );
char *get_list( list_t *top );