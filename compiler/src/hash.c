#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

hash_t *hash_make() {
    hash_t *t = (hash_t*)malloc(sizeof(hash_t));
    if (!t) {
        fprintf(stderr, "Error: Failed to allocate memory for hashtable");
        exit(1);
    }
    t->capacity = MAX_CAPACITY;
    memset(t->table, 0, sizeof(list_t *) * MAX_CAPACITY); 
    return t;
}


void hash_free(hash_t *table) {
    if (table) {
        for (size_t i = 0; i < table->capacity; i++) {
            list_t *entry = table->table[i];
            while (entry) {
                list_t *temp = entry;
                entry = entry->next;
                free(temp->name);  
                free(temp);         
            }
        }
        free(table); 
    }
}


 
void hash_init( hash_t **top ){
   if( !top ) return;
   *top = hash_make();
}

void hash_exit( hash_t **top ){
    if( !top || !*top ) return;
    while ( *top )
    {
        hash_t *tmp = *top;
        *top = ( *top )->next;
        hash_free(tmp);
    }
}

unsigned int hash_pjw( char *s, size_t table_size )
{
    char *p; 
    unsigned h = 0, g;
    for ( p = s; *p != '\0'; p = p+1 )
    {
        h = (h << 4 ) + (*p); // multiply by 16
        if ( g = h&0xf0000000) /* If g != 0 */
        {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h % table_size; 
}

list_t *hash_insert( hash_t *table, char *name ){
    unsigned int index = hash_pjw(name, table->capacity);
    list_t *entry = (list_t *)malloc(sizeof(list_t));
    if (!entry)
    {
        fprintf(stderr, "Error: Failed to allocate memory for list entry\n");
        exit(1);
    }
    entry->name = strdup(name);
    entry->next = table->table[index];
    table->table[index] = entry;
    return entry;
}

list_t *hash_search( hash_t *table, char *name ){
    if ( !table ||  !name ) return NULL;
    unsigned int index = hash_pjw( name, table->capacity);
    return list_search(table->table[index], name);
}

list_t *hash_search_all( hash_t *table, char *name ){
    if ( !table || !name ) return NULL;
    while ( table ) /* Traverse Scope Stack */
    {
        list_t *found = hash_search( table, name); /* Search Curr Scope */
        if (found) return found;
        table = table->next; /* GOTO Parent Scope */
    }
    return NULL; 
}
list_t *hash_search_all_depth( hash_t *table, char *name, int *depth ){
    if ( !table || !name || !depth ) return NULL;
    *depth = 0;

    while ( table ){
        list_t *found = hash_search( table, name );
        if ( found ) return found;
        table = table->next;
        (*depth)++; 
    }
    return NULL;
}

/* Stack Operations */
hash_t *hash_pop( hash_t *top ){
    if( !top ) return NULL;
    hash_t *popped = top;
    top = top->next; 
    popped->next = NULL;
    return popped; 
}

hash_t *hash_push( hash_t *top, hash_t *table ){
    if (!table) return top;
    table->next = top;
    return table;
} 

void hash_print( hash_t *table)
{

    if (!table)
    {
        printf("Hash table is empty.\n");
        return;
    }

    printf("Hash Table:\n");

    for (unsigned int i = 0; i < table->capacity; i++)
    {
        list_t *entry = table->table[i];
        if (entry)
        {
            printf("Bucket %u: ", i);
            while (entry)
            {
                printf("%s -> ", entry->name);
                entry = entry->next;
            }
            printf("NULL\n");
        }
    }
}

// int main(){
//     /* test symbol table */
//     hash_t *table = hash_make();
//     hash_insert(table, "a");
//     hash_insert(table, "b");
//     hash_insert(table, "c");
//     hash_insert(table, "d");
//     hash_insert(table, "e");

//     hash_print(table);

//     list_t *found = hash_search(table, "c");
//     if (found)
//     {
//         printf("Found: %s\n", found->name);
//     }
//     else
//     {
//         printf("Not found\n");
//     }

//     hash_insert(table, "f");
//     hash_insert(table, "g");
//     hash_insert(table, "h");
//     hash_print(table);
//     list_t *result = hash_search(table, "g");
//     if (result) {
//         printf("Found: %s\n", result->name);
//     } else {
//         printf("Element not found.\n");
//     }
//     hash_free(table);
//     return 0;
// }