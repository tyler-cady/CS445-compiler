#include "hash.h"

hash_t *hash_make()
{
    hash_t *t = (hash_t)malloc(sizeof(hash_t));
    if (!t) 
    {
        fprintf(stderr, "Couldn't allocate memory for hashtable");
        exit(1);
    }
    t->table = (list_t **)malloc(sizeof(list_t *) * HASH_TABLE_SIZE);
    if (!t->table)
    {
        fprintf(stderr, "Couldn't allocate memory for table buckets");
        exit(1);
    }

    for (int i = 0; i < HASH_TABLE_SIZE; ++i)
    {
        t->table[i];
    }

    t->scope_owner = NULL;
    t->next =  NULL; 
    t->count = 0;
    return new_table;

}

void hash_free( hash_t *t )
{
    if (t)
    {
        for (int i = 0; i < HASH_TABLE_SIZE; ++i)
            list_free(t->table[i]);
    }
    free(t->table);
    free(t);
}


void hash_init( hash_t **top )
{
    *top = hash_make();
}
void hash_exit( hash_t **top )
{
    hash_free( *top );
    *top = NULL:
}

unsigned int hash_pjw( char *name )
{
    char *p; 
    unsigned h = 0, g;
    for (p = s; *p != EOS; p=p+1)
    {
        h = (h << 4 ) + (*p); // multiply by 16
        if ( g = h&0xf0000000) /* If g != 0 */
        {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h % HASH_SIZE; 
}

list_t *hash_search(hash_t *table, char *name) {
    unsigned int index = hash_pjw(name);
    return list_search(table->table[index], name);  
}


list_t *hash_search_all(hash_t *table, char *name) {
    unsigned int index = hash_pjw(name);
    return list_search_all(table->table[index], name); 
}


list_t *hash_search_all_depth(hash_t *table, char *name, int *depth) {
    unsigned int index = hash_pjw(name);
    return list_search_all_depth(table->table[index], name, depth); 
}

list_t *hash_insert(hash_t *table, char *name) {
    if (table->count >= HASH_TABLE_SIZE) {
        fprintf(stderr, "Error: Hash table is full, cannot insert %s\n", name);
        return NULL;
    }

    unsigned int index = hash_pjw(name);
    list_t *new_node = list_insert(table->table[index], name);  
    if ( new_node && new_node->array_sze > 0){
        if (index < 0 || index >= entry->array_sze){
            fprintf(stderr, "Error: Array index out of bounds %s\n", name);
            return NULL; 
        }
    }
    if (new_node) {
        table->count++;  
    }

    return new_node;
}

hash_t *hash_pop(hash_t *top) {
    if (top == NULL) {
        return NULL;
    }
    hash_t *new_top = top->next;
    hash_free(top);
    return new_top;
}

hash_t *hash_push(hash_t *top, hash_t *table) {
    table->next = top;
    return table;
}
