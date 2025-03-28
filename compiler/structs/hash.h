#pragma once
#include <stdio.h>
#include "list.h"
#include <stdlib.h>

#define MAX_CAPACITY 512


typedef struct hash_s
{
    /* Hash Table */
    list_t **table[MAX_CAPACITY];         /* Array of linked list heads ( hash table )*/
    list_t *scop_owner;
    size_t capacity;                      /* Number of buckets in the hash table */
    struct hash_s *next;
} hash_t;

/* Constructor */
hash_t *hash_make(); 
// void hash_resize(hash_t *table, size_t new_capacity); // Assistant to the regional manager 

/* Destructor */
void hash_free( hash_t * );

void hash_init( hash_t **top );
void hash_exit( hash_t **top );
unsigned int hash_pjw( char *name, size_t table_size);

list_t *hash_search( hash_t *table, char *name );
list_t *hash_search_all( hash_t *table, char *name ); 
list_t *hash_search_all_depth( hash_t *table, char *name, int *depth );
list_t *hash_insert( hash_t *table, char *name );

/* Stack Operations */
hash_t *hash_pop( hash_t *top );
hash_t *hash_push( hash_t *top, hash_t *table ); 

void hash_print( hash_t *table );