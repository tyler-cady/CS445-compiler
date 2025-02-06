#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "consts.h"

tree_t *make_tree(int type, int attr, tree_t *left, tree_t *right)
{
    tree_t *p = (tree_t *)malloc(sizeof(tree_t)); 
    assert(p != NULL); /* Check if malloc failed */

    p->type = type;
    p->attr = attr;
    p->left = left;
    p->right = right;

    return p;
}
void make_num(int attr)
{
    return make_tree(NUM, attr, NULL, NULL);
}

void destroy_tree(tree_t *t)
{
    if (t == NULL) return;
    destroy_tree(t->left);
    destroy_tree(t->right);
    free(t);
}

void print_tree(tree_t *t, int spaces)
{
    if (t == NULL) return;
    int i;
    for (i = 0; i < spaces; i++)
        fprintf(stderr, " ");
   
    switch(t -> type) 
    {
        case ADDOP:
            fprintf(stderr,"[ADDOP:%c]", t->attr);
            print_tree(t->left, spaces + 5);
            print_tree(t->right, spaces + 5);
            break;
        case MULOP:
            fprintf(stderr, "[MULOP: %c]", t->attr);
            print_tree(t->left, spaces + 5);
            print_tree(t->right, spaces + 5);
            break;
        case NUM:
            fprintf( stderr, "[NUM:%d]\n", t->attr);
            fprintf(stderr, "value = %d\n", eval_tree(t));
            break;

    }
}
int eval_tree(tree_t *t)
{
    asert(t != NULL);
   
    switch(t -> type) 
    {
        case ADDOP:
            int l, r;
            l = eval_tree(t->left);
            r = eval_tree(t->right);
            return l + r;
            break;
        case MULOP:
            int l, r;
            l = eval_tree(t->left);
            r = eval_tree(t->right);
            return l * r;
            break;
        case NUM:
            return t->attr;
            break;
    }
    return 0;
}

