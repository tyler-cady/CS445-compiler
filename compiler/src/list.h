/**
 * list.h 
 * Linked list of records for names
 */

#ifndef LIST_H
#define LIST_H


 #include "ptype.h"
 
 typedef struct list_s 
 {
     char *name;
     int class;              /* Name, Function, Procedure */
     ptype_t *type;          /* Types: integer, real, etc. */
     struct list_s *next;
 
 } list_t;
 
 /* Linkable Names */
 typedef struct lname_s
 {
     list_t *nptr;
     struct lname_s *next;
 
 } lname_t;
 
 /* Constructors */
 list_t *list_make( char *name );
 lname_t *lname_make( list_t *);
 
 /* Destructors */
 void list_free( list_t *node );
 void list_free_all( list_t *node );
 void lname_free( lname_t * );
 
 /* Functions */
 list_t *list_search( list_t *top, char *name );
 list_t *list_insert( list_t *top, char *name );
 lname_t *lname_append( lname_t *, lname_t *);
 
 #endif
 