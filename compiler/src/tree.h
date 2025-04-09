#ifndef TREE_H
#define TREE_H

#include "hash.h"
#include "list.h"
#include "pc.tab.h"



typedef struct tree_s 
{
    int type; /* INUM, RNUM, ARRAY, FUNCTION, RELOP, ... etc.*/
    
    union
    {
        int ival;       /* INUM */
        float rval;     /* RNUM */
        list_t *sval;   /* NAME */
        int opval;      /* MULOP, ADDOP, ASSIGNOP */
    }value;

    int scope_depth;
    int label;
    int caller_is_parent;

    struct tree_s *left;
    struct tree_s *right;
} tree_t;

// typedef union {
//     int ival;
//     float rval;
//     char *sval;
// } tree_value_t;

tree_t *tree_make(int type, yystype attr, tree_t *left, tree_t *right);
void tree_free(tree_t *t);
tree_t *tree_make_inum(int attr);
tree_t *tree_make_str(char *name);
tree_t *tree_make_op(int opval, tree_t *left, tree_t *right);
tree_t *tree_make_rnum(float attr);
static void tree_label(tree_t *t);
void tree_print(tree_t *t, int spaces);
void tree_set_type(tree_t *t, int type);

#endif // TREE_H