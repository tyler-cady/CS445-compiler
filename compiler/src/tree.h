#ifndef TREE_H
#define TREE_H


#include "list.h"

typedef struct tree_s 
{
    int type; /* INUM, RNUM, ARRAY, FUNCTION, RELOP, ... etc.*/

    union
    {
        int ival;       /* INUM */
        float rval;     /* RNUM */
        list_t *name_ptr;   /* NAME */
        int opval;      /* MULOP, ADDOP, ASSIGNOP */
    } attr;
 
    int scope_depth;
    int label;
    int caller_is_parent;
    int reg;

    struct tree_s *left;
    struct tree_s *right;
} tree_t;

void tree_free(tree_t *t);

/* Makes */
tree_t *tmake(int type, tree_t *left, tree_t *right);

/* Type makes */
tree_t *tmake_program(list_t *name, tree_t *args, tree_t *decls, tree_t *subprograms, tree_t *body);

tree_t *tmake_inum(int ival);
tree_t *tmake_rnum(float rval);
tree_t *tmake_id(list_t *name_ptr);
tree_t *tmake_type(int type);

/* Operator makes */
tree_t *tmake_mulop(int opval, tree_t *left, tree_t *right);
tree_t *tmake_addop(int opval, tree_t *left, tree_t *right);
tree_t *tmake_relop(int opval, tree_t *left, tree_t *right);

const char *type_to_str(int type);
void tprint(tree_t *t, int spaces);

#endif // TREE_H
