/**
 * list.h 
 * Linked list of records for names
 */

#ifndef LIST_H
#define LIST_H



 
 typedef struct list_s 
 {
   char *name;
   int class;              /* Name, Function, Procedure */
   int type;              /* Types: integer, real, etc. */
   int scopetype;        /* LOCAL, GLOBAL, PARAMETER */
   struct list_s *next;

   int *args;        /* For functions */
   int *arg_types;
   int *scope;       /* For functions */
   int initialized;
   int start_index;     /* For arrays */
   int end_index;       /* For arrays */
} list_t;

/* Constructors */
list_t *list_make( char *name );


/* Destructors */
void list_free( list_t *node );
void list_free_all( list_t *node );


/* Functions */
list_t *list_search( list_t *top, char *name );
list_t *list_insert( list_t *top, char *name );
void list_print( list_t *top );

#endif
 