#include "list.h"

#define HASH_TABLE_SIZE 300

typedef struct hash_s
{
    list_t **table;
    list_t *scope_owner;
    struct hash_s *next;
} hash_t;

hash_t *hash_make();
void hash_free( hash_t * );

void hash_init( hash_t **top );
void hash_exit( hash_t **top );
unsigned int hash_pjw( char *name );

list_t *hash_search( hash_t *table, char *name );
list_t *hash_search_all( hash_t *table, char *name ); 
list_t *hash_search_all_depth( hash_t *table, char *name, int *depth );
list_t *hash_insert( hash_t *table, char *name );


hash_t *hash_pop( hash_t *top );
hash_t *hash_push( hash_t *top, hash_t *table ); 
