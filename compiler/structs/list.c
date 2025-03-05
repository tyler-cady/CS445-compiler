#include "list.h"

list_t *list_make( char *name )
{
    

}
void free_node( list_t *node );
void free_list( list_t * );
void lname_free( lname_t * );

void mark_initialized( list_t *var)
{
    var->initialized = 1; 
}
int check_initialized( list_t *var)
{
    return var->initialized;
}

list_t *list_search( list_t *top, char *name )
{
    for (list_t *p = top; p != NULL; p = p->next)
    {
        if (strcmp(p->name, name) == 0)
        {
            return p;
        }
    }
    return NULL;

}

list_t *list_insert( list_t *top, char *name )
{
    list_t *new_node = list_make(name);
    new_node->next = top;
    return new_node;
}
lname_t *lname_append( lname_t *, lname_t *)
{
    return NULL;
}

void print_list( list_t *top )
{
    for (list_t *p = top; p != NULL; p = p->next)
    {
        printf("%s\n", p->name);
    }
}