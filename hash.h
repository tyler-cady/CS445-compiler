#ifndef HASH_H
#define HASH_H

#include "compiler/src/tree.h"


#define HASH_SIZE 256


typedef union {
    int ival;
    float rval;
    char *sval;
} ptype_t;

typedef struct hash_node_s
{
    char *name;
    int type; // Function, Variable, Array 
    int return_type;
    int id;
    int arg_count;
    int *arg_types;
    
    int arr_begin;
    int arr_end;

    struct hash_node_s *next;
} hash_node_t;


typedef struct hash_s
{
    hash_node_t *table[HASH_SIZE];
    int id;
    int size;
    int arg_count;
    int local_count;
    int temp_offset;
    char *name;
    int table_type;
    struct hash_s *parent;
    struct hash_s *next;
} hash_t;

#endif // HASH_H

hash_t *hash_make( char* entry_name, int entry_class, ptype_t* val_ptr, int return_type, int arg_num, int* arg_types, int start_idx, int stop_idx);
hash_t *hash_push( char *name, int table_type );
hash_t *hash_pop();
hash_t *hash_top();
int hash_pjw( char *name );
void hash_make_var( char *name, ptype_t type );
void hash_make_arr( char *name, ptype_t type, int arr_begin, int arr_end );
void hash_make_function( tree_t *func, tree_t *type);
void hash_make_procedure( tree_t *proc);
void hash_free( hash_t *table );

