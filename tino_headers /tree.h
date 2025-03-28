#include "hash.h"
#include "list.h"

typedef struct tree_s 
{
    int type; /* INUM, RNUM, ARRAY, FUNCTION, RELOP, ... etc.*/
    
    union
    {
        int ival;       /* INUM */
        float rval;     /* RNUM */
        list_t *sval;   /* NAME */
        int opval;      /* MULOP, ADDOP, ASSIGNOP */
    }
    value;

    int scope_depth;
    int label;
    int caller_is_parent;

    struct tree_s *left;
    struct tree_s *right;
} tree_t;

tree_t *tree_make(int type, int attr, char *name, tree_t *left, tree_t *right);
void tree_free(tree_t *t);
tree_t *tree_make_num(int attr);
tree_t *tree_make_name(char *name);
tree_t *tree_make_op(int attr, tree_t *left, tree_t *right);
tree_t *tree_make_assign(tree_t *left, tree_t *right);
void tree_print(tree_t *t, int spaces);

