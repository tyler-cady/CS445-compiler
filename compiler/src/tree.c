#include "hash.h"
#include "list.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
tree_t *tree_make(int type, yystype attr, tree_t *left, tree_t *right) {
    tree_t *node = (tree_t *)malloc(sizeof(tree_t));
    if (!node) {
        error_fatal_malloc();
        exit(1);
    }

    node->type = type;
    node->scope_depth = 0;
    node->label = 0;
    node->caller_is_parent = 0;
    node->left = left;
    node->right = right;

    tree_label(node);
    return node;
}


void tree_free(tree_t *t){
    if (!t) return;
    if (t->type == 3) list_free(t->value.sval);
    tree_free(t->left);
    tree_free(t->right);
    free(t);
}

tree_t *tree_make_str(char *name){
    yystype v;
    v.sval = name;
    return tree_make(3, v, NULL, NULL);
}

tree_t *tree_make_inum(int attr){
    yystype v;
    v.ival = attr;
    return tree_make(1, v, NULL, NULL);
}

tree_t *tree_make_rnum(float attr){
    yystype v;
    v.rval = attr;
    return tree_make(2, v, NULL, NULL);
}

tree_t *tree_make_op(int opval, tree_t *left, tree_t *right){
    yystype v;
    v.ival = opval;
    return tree_make(4, v, left, right);
}

char *lookup_type(tree_t *t){
    switch (t->type){
        case 1:
            return "INUM";
        case 2:
            return "RNUM";
        case 3:
            return "NAME";
        case 4:
            return "OP";
        default:
            return "Unknown type";
    }
}

void print_tree(tree_t *t, int spaces){
    if (!t) return;
    for (int i = 0; i < spaces; i++) printf(" ");
    switch (t->type){
        case 1:
            printf("%d:INUM: %d\n",t->label, t->value.ival);
            break;
        case 2:
            printf("%d:RNUM: %f\n", t->label, t->value.rval);
            break;
        case 3:
            printf("%d:NAME: %s\n", t->label, t->value.sval->name);
            break;
        case 4:
            printf("%d:OP: %d\n", t->label, t->value.opval);
            break;
        default:
            printf("Unknown type\n");
            break;
    }
    print_tree(t->left, spaces + 2);
    print_tree(t->right, spaces + 2);
}

static void tree_label( tree_t *n ){
    /**
     * if n is a leaf: 
     *  if n is leftmost child of its parent:
     *      label(n) = 1
     *  else label(n) = 0
     * else 
     *  let n1,n2,..n, nk be children of n ordered by label label(n1) >= label(n2)...>= label(nk)
     *  label(n) = max(label(ni) + i - 1)
     */
    if (!n) return; /* Base case n is null */
    if (!n->left && !n->right){
        n->label = (n->caller_is_parent) ? 1 : 0;
        return;
    }
    if (n->left) tree_label(n->left);
    if (n->right) tree_label(n->right);

    /* Label leftmost leaf 1 */
    if (n->left && !n->left->left && !n->left->right){
        n->left->label = 1;
    }
    /* Label rightmost leaf 0 */
    if (n->right && !n->right->left && !n->right->right){
        n->right->label = 0;
    }
    /* Label internal nodes */
    if (n->left && n->right){
        int max = 0;
        tree_t *c = n->left;
        while (c){
            if (c->label > max) max = c->label;
            c = c->right;
        }
        n->label = max + 1;
    }
}

void tree_set_type(tree_t *t, int type){
    t->type = type;
}
// int main(){
//     tree_t *t = tree_make_num(5);
//     tree_t *t2 = tree_make_rnum(3.14);
//     tree_t *t3 = tree_make_op(1, t, t2);
//     tree_t *a = tree_make_name("a");
//     tree_t *b = tree_make_assign(a, t3);
    
//     print_tree(b, 0);
//     tree_free(b);
//     return 0;
// }