#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* Con/De/structor */
node_t *make_node( char *name )
{
    node_t *t = ( node_t * )malloc(sizeof(node_t));
    assert( t != NULL );
    t->name = strdup(name);
    t->next = NULL; 
    return t; 
}

void unmake_node( node_t *n )
{
    assert(n != NULL);
    free(n->name);
    free(n);
}

/* visual debugger */
void print_node( node_t *top)
{
    node_t *p = top;  
    while (p != NULL) 
    {
        fprintf(stderr, "[%s]->", p->name);
        p = p->next;
    }
    fprintf(stderr, "\n");

}

node_t *search_node( char *name, node_t *top )
{
    node_t *p = top;
    while( p != NULL )
    {
        if (strcmp(p->name, name) == 0) return p; 
        else break;
    }
    return NULL;
}

node_t *insert_node( char *name, node_t *top )
{
    node_t *p = make_node( name );
    p->next = top;
    return p; /* new top of the list */
}
#define BUFFER_SZ  32 

int main()
{
    node_t *top = NULL;
    char buff[BUFFER_SZ];
    char choice;
    node_t *p = NULL;

    while(1)
    {
        fprintf(stderr, "(s)earch\n(i)nsert\n(p)rint\n");
        scanf( "%c", &choice );
        switch(choice)
        {
            case 's':
            case 'S':
                fprintf(stderr, "Enter name: " );
                scanf("%s", buff);
                p = search_node( buff, top );
                if (p != NULL) fprintf(stderr, "[found: %s]", p->name);
                else fprintf(stderr, "[%s not found]", buff);
                break;
            case 'i':
            case 'I':
                fprintf(stderr, "Enter name: " );
                scanf("%s", buff);
                p = insert_node( buff, top);
                
                break;
            case 'p':
            case 'P':
                print_node( top );
                break;
            default:
                fprintf(stderr, "What are you trying to pull? Illegal input ya big silly.\n");
        }
    }
    unmake_node(top);
}