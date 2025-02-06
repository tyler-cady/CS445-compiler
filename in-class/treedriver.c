#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
#include "consts.h"

int main()
{
    tree_t *p, *q, *r;
    print_tree(MULOP, '*', make_tree((ADDOP, '+', make_num(2), make_num(3)), make_num(4)));
}