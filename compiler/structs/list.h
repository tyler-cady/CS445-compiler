#pragma once 
/**
 * Part of symbol table 
 * maintains a linked list of records for names
 */


 #include "ptype.h"

 typedef struct list_s
 {
    char *name;
    int class; /* Name, function, or procedure */
    int location;  /* Local or parameter */
    
    /* function or procedure */
    ltype_t *arg_list;
    int local_sze; /* size of var*/
    int in_param_sze;  /* size of parameter in */
    int out_param_sze; /* size of parameter out */
    int frame_sz; /* in + out */
    ptype_t *type; 

    /* Array bounds checks */
    int array_sze;
    int low_bound;
    int upper_bound;
    int initialized;

    struct list_s *next;
} list_t;


/* linkable names */
typedef struct lname_s
{
    list_t *nptr;
    struct lname_s *next;
} lname_t;

list_t *list_make( char *name );
void free_node( list_t *node );
void free_list( list_t * );
void lname_free( lname_t * );
void print_list( list_t *top );
void mark_initialized( list_t *var);
int check_initialized( list_t *var);

list_t *list_search( list_t *top, char *name );
list_t *list_insert( list_t *top, char *name );
lname_t *lname_append( lname_t *, lname_t *);