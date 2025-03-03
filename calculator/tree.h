#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "consts.h"
#include "sym_table.h"

typedef struct tree_s
{
    int type;
    int attr;
    struct tree_s *right, *left;
    char *var_name;

} tree_t;

tree_t *make_tree(int type, int attr, char *name, tree_t *left, tree_t *right);
tree_t *make_num(int attr); 
tree_t *make_operator_node(int op_type, tree_t *left, tree_t *right);

int eval_tree(tree_t *t);

void destroy_tree(tree_t *t);
void print_tree(tree_t *t, int spaces);
