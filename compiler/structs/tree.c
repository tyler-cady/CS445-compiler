#include "hash.h"
#include "list.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tree_t *tree_make(int type, int attr, char *name, tree_t *left, tree_t *right){
    tree_t *node = (tree_t*)malloc(sizeof(tree_t));
    if (!node){
        fprintf(stderr, "Error: Failed to allocate memory for tree node");
        exit(1);
    }
    node->type = type;
    node->scope_depth = 0;
    node->label = 0;
    node->caller_is_parent = 0;
    node->left = left;
    node->right = right;

    switch (type){
        case 1: /* INUM */
            node->value.ival = attr;
            break;
        case 2: /* RNUM */
            node->value.rval = (float)attr;
            break;
        case 3: /* NAME */
            node->value.sval = list_make(name);
            break;
        case 4: /* MULOP, ADDOP, ASSIGNOP */
            node->value.opval = attr;
            break;
        default:
            node->value.ival = 0;
            break;
    }
    return node;
}


void tree_free(tree_t *t){
    if (!t) return;
    if (t->type == 3) list_free(t->value.sval);
    tree_free(t->left);
    tree_free(t->right);
    free(t);
}
tree_t *tree_make_num(int attr){
    return tree_make(1, attr, NULL, NULL, NULL);
}
tree_t *tree_make_rnum(float attr){
    return tree_make(2, *(int *)&attr, NULL, NULL, NULL);
}
tree_t *tree_make_op(int attr, tree_t *left, tree_t *right){
    return tree_make(4, attr, NULL, left, right);
}
tree_t *tree_make_assign(tree_t *left, tree_t *right){
    return tree_make(4, 1, NULL, left, right);
}

void print_tree(tree_t *t, int spaces){
    if (!t) return;
    for (int i = 0; i < spaces; i++) printf(" ");
    switch (t->type){
        case 1:
            printf("INUM: %d\n", t->value.ival);
            break;
        case 2:
            printf("RNUM: %f\n", t->value.rval);
            break;
        case 3:
            printf("NAME: %s\n", t->value.sval->name);
            break;
        case 4:
            printf("OP: %d\n", t->value.opval);
            break;
        default:
            printf("Unknown type\n");
            break;
    }
    print_tree(t->left, spaces + 2);
    print_tree(t->right, spaces + 2);
}

int main(){
    tree_t *t = tree_make_num(5);
    tree_t *t2 = tree_make_rnum(3.14);
    tree_t *t3 = tree_make_op(1, t, t2);
    print_tree(t3, 0);
    tree_free(t3);
    return 0;
}