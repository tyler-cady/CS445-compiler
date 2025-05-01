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
    l->type = 0;
    l->scopetype = 0;
    l->args = NULL;
    l->arg_types = NULL;
    l->scope = NULL;
    l->start_index = 0;
    l->end_index = 0;
    l->next = NULL;
    return l;
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

void list_print( list_t *top ){
    while ( top ){
        printf("%s -> ", top->name);
        top = top->next;
    }
}

char *get_list(list_t *top) {
    char *result = NULL;
    char *temp = NULL;

    while (top) {
        if (result == NULL) {
            asprintf(&result, "%s", top->name);
        } else {
            asprintf(&temp, "%s -> %s", result, top->name);
            free(result);
            result = temp;
        }
        top = top->next;
    }
    return result ? result : strdup(""); 
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