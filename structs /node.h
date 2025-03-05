#pragma once 

typedef struct node_s
{
    char *name;
    struct node_s *next;
} node_t;


/* Constructor */
node_t *make_node( char * );
void unmake_node( node_t * );

/* Visual debugger */
void print_node( node_t *p );

/* Support Functions */
node_t *search_node( char *name, node_t *top );
node_t *insert_node( char *name, node_t *top ); /* Insert at the top */

/* node.h */



