#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Constructors */
list_t *list_make(char *name) {
    list_t *l = (list_t *)malloc(sizeof(list_t));
    if (!l) {
        fprintf(stderr, "Error: Failed to allocate memory for list.\n");
        exit(1);
    }
    
    l->name = strdup(name); // Duplicate string to avoid modification issues
    if (!l->name) {
        fprintf(stderr, "Error: Failed to allocate memory for name.\n");
        free(l);
        exit(1);
    }
    
    l->class = 0;
    l->type = NULL;
    l->next = NULL;
    return l;
}


lname_t *lname_make(list_t *nptr) {
    lname_t *ln = (lname_t *)malloc(sizeof(lname_t));
    if (!ln) {
        fprintf(stderr, "Error: Failed to allocate memory for lname.\n");
        exit(1);
    }
    
    ln->nptr = nptr;
    ln->next = NULL;
    return ln;
}


/* Destructors */
void list_free( list_t *node ){
    if ( node ){
        free( node->name );
        free( node );
    }
}
void list_free_all( list_t *node ){
    while ( node ){
        list_t *tmp = node;
        node = node->next;
        list_free(tmp);
    }
}
void lname_free(lname_t *node) {
    while (node) {
        lname_t *temp = node;
        node = node->next;
        free(temp);
    }
}

/* Functions */
list_t *list_search(list_t *top, char *name) {
    while (top) {
        if (strcmp(top->name, name) == 0)
            return top;
        top = top->next;
    }
    return NULL; 
}

list_t *list_insert( list_t *top, char *name ){
    list_t *l = list_make(name);
    l->next = top;
    return l;
}
lname_t *lname_append( lname_t *appendee, lname_t *to_append){ /* Concatenate to linked lists */
    if ( !appendee ) return to_append;
    lname_t *tmp = appendee;
    while ( tmp->next ){
        tmp = tmp->next;
    }
    tmp->next = to_append;
    return appendee;
}


/* Test Driver */
// int main(){
//     list_t *symbol1 = list_make("varA");
//     list_t *symbol2 = list_make("varB");

//     lname_t *list1 = lname_make(symbol1);
//     lname_t *list2 = lname_make(symbol2);

//     list1 = lname_append(list1, list2);

//     lname_t *temp = list1;
//     while (temp) {
//         printf("Symbol: %s\n", temp->nptr->name);
//         temp = temp->next;
//     }

//     lname_free(list1);
//     return 0;
// }
