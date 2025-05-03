#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "util.h"
#include "error.h"
#include "list.h"
#include "hash.h"
#include "semantic.h"
#include "pc.tab.h"

hash_t *hash_make() {
    hash_t *t = (hash_t*)malloc(sizeof(hash_t));
    if (!t) {
        fprintf(stderr, "Error: Failed to allocate memory for hashtable");
        exit(1);
    }
    t->capacity = MAX_CAPACITY;
    t->next = NULL;
    memset(t->table, 0, sizeof(list_t *) * MAX_CAPACITY); 
    return t;
}

void hash_free(hash_t *table) {
    if (table) {
        for (size_t i = 0; i < table->capacity; i++) {
            list_t *entry = table->table[i];
            while (entry) {
                list_t *next = entry->next;
                free(entry->name);
                free(entry);
                entry = next;
            }
        }
        free(table);
    }
}


unsigned int hash_pjw( char *s, size_t table_size )
{
    char *p; 
    unsigned h = 0, g;
    for ( p = s; *p != '\0'; p = p+1 )
    {
        h = (h << 4 ) + (*p); // multiply by 16
        if (( g = h&0xf0000000)) /* If g != 0 */
        {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h % table_size; 
}

list_t *hash_insert( hash_t *table, char *name ){
    if ( !table || !name ) return NULL;
    if (is_declared_in_scope(table, name)) {
        char *msg;
        asprintf(&msg, "semantic error: '%s' is already declared in scope", name);
        yyerror(msg);
        free(msg);
        return NULL;
    }

    unsigned int index = hash_pjw(name, table->capacity);
    list_t *l;

    // if (strcmp(name, "input") == 0) {
    //     l = list_insert(table->table[index], "read");
    //     l->class = PROCEDURE;
    //     table->table[index] = l;
    //     return l;
    // } else if (strcmp(name, "output") == 0) {
    //     l = list_insert(table->table[index], "write");
    //     l->class = PROCEDURE;
    //     l->arg_types = malloc(sizeof(int));
    //     l->arg_types[0] = INTEGER;
    //     table->table[index] = l;
    //     return l;
    // } else {
        l = list_insert(table->table[index], name);
        table->table[index] = l;
        return l;
    // }
}




list_t *get_id_list(hash_t *t) {
    if (!t) return NULL;
    list_t *l = NULL;

    for (int i = 0; i < t->capacity; i++) {
        list_t *bucket = t->table[i];
        while (bucket != NULL) {
            l = list_insert(l, bucket->name); 
            l->type = bucket->type;
            l->start_index = bucket->start_index;
            l->end_index = bucket->end_index;
            bucket = bucket->next;
        }
    }

    return l;
}



list_t *hash_search( hash_t *table, char *name ){
    if ( (table == NULL) ||  (name == NULL) ) return NULL;
    unsigned int index = hash_pjw( name, table->capacity);
    list_t *l  = list_search( table->table[index], name );
    return l;
}

list_t *hash_search_all( hash_t *table, char *name ){
    if ( !table || !name ) return NULL;
    while ( table ) /* Traverse Scope Stack */
    {
        list_t *found = hash_search( table, name); /* Search Curr Scope */
        if (found != NULL) return found;
        table = table->next; /* GOTO Parent Scope */
    }
    return NULL; 
}

int hash_get_type(hash_t *table, char *name) {
    if (!table || !name) return -1;
    list_t *found = hash_search(table, name);
    if (found) return found->type;
    return -1;
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

list_t *hash_set_type(hash_t *table, char *name, int type, int scopetype) {
    list_t *entry = hash_search_all(table, name);
    if (!entry) {
        fprintf(stderr, "hash_set_type: symbol '%s' not found\n", name);
        return NULL;
    }
    entry->type = type;
    entry->scopetype = scopetype;
    return entry;
}

list_t *hash_add_bounds(hash_t *table, char *name, int start_index, int end_index) {
    list_t *entry = hash_search_all(table, name);
    if (!entry) {
        fprintf(stderr, "hash_add_bounds: symbol '%s' not found\n", name);
        return NULL;
    }
    entry->start_index = start_index;
    entry->end_index = end_index;
    return entry;
}

list_t *hash_init_symbol(hash_t *table, char *name){
    list_t *entry = hash_search(table, name);
    if (!entry) {
        fprintf(stderr, "hash init: symbol '%s' not found\n", name);
        return NULL;
    }
    entry->initialized = 1;
    return entry;
}

hash_t *hash_pop( hash_t *top ){
    assert( top != NULL );
    hash_t *local = top->next;
    hash_free( top );
    return local;
}

hash_t *hash_push( hash_t *top ) {
    hash_t *local = hash_make();
    local->next = top;
    return local;
}

void hash_print( hash_t *table)
{
    assert( table != NULL );
    for ( int i = 0; i < MAX_CAPACITY; i++ ){
        fprintf( stderr, "%d: %s", i, get_list(table->table[i]) );
        // list_print( table->table[i] );
        fprintf( stderr, "\n" );
    }
}


//  int main(){
//      hash_t *top = NULL;
//      list_t *p = NULL;
//      char buff[ 100 ];
//      int choice;
//      while (1){
//          fprintf(stderr, "(0) Search (1) Global Search (2) Insert (3) Push (4) Pop (5) Print\n");
//          scanf("%d", &choice);
//          switch ( choice ){
//              case 0:
//                  fprintf(stderr, "Enter name: ");
//                  scanf("%s", buff);
//                  p = hash_search( top, buff );
//                  if ( p ){
//                      fprintf(stderr, "Found: %s\n", p->name);
//                  } else {
//                      fprintf(stderr, "Not Found\n");
//                  }
//                  break;
//              case 1:
//                  fprintf(stderr, "Enter name: ");
//                  scanf("%s", buff);
//                  p = hash_search_all( top, buff );
//                  if ( p ){
//                      fprintf(stderr, "Found: %s\n", p->name);
//                  } else {
//                      fprintf(stderr, "Not Found\n");
//                  }
//                  break;
//              case 2:
//                  fprintf(stderr, "Enter name: ");
//                  scanf("%s", buff);
//                  p = hash_insert( top, buff );
//                  if ( p ){
//                      fprintf(stderr, "Inserted: %s\n", p->name);
//                  } else {
//                      fprintf(stderr, "Insert Failed\n");
//                  }
//                  break;
//              case 3:
//                  top = hash_push( top );
//                  fprintf(stderr, "Pushed new scope\n");
//                  break;
//              case 4:
//                  top = hash_pop( top );
//                  fprintf(stderr, "Popped scope\n");
//                  break;
//              case 5:
//                  hash_print( top );
//                  break;
//              case 6:
//                  fprintf(stderr, "Enter name: ");
//                  scanf("%s", buff);
//                  hash_set_type(top, buff, INTEGER, LOCAL);
//                  list_t *l = hash_search_all(top, buff);
//                  fprintf(stderr, "%d\n", l->type);
//                  break;
//              default:
//                  fprintf(stderr, "Invalid choice\n");
//                  break;
//              }
//      }
//  }
