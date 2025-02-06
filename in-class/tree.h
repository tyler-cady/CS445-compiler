#pragma once 

typedef struct tree_s{
    int type;
    int attr; 
    struct tree_s *left, *right;
} tree_t;

tree_t *make_tree(int type, int attr, tree_t *left, tree_t *right);
tree_t *make_num(int attr); 

int eval_tree(tree_t *t);

tree_t destroy_tree(tree_t *t);
void print_tree(tree_t *t, int spaces);
