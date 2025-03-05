#include "tree.h"


tree_t *make_tree(int type, int attr, char *name, tree_t *left, tree_t *right) {
    tree_t *t = (tree_t *)malloc(sizeof(tree_t));
    assert(t != NULL);
    t->var_name = name;
    t->type = type;
    t->attr = attr;
    t->left = left;
    t->right = right;

    /* DEBUG
    fprintf(stderr, "Created node of type %d, with name %s, left = %p, right = %p\n", 
            type, name ? name : "NULL", (void*)left, (void*)right);
    */

    return t;
}

void destroy_tree(tree_t *t)
{
    if(!t) return;
    destroy_tree(t->left);
    destroy_tree(t->right);
    free(t);
}

tree_t *make_num(int attr) 
{
    return make_tree(NUM, attr, NULL, NULL, NULL);
}


int eval_tree(tree_t *t) {
    assert(t != NULL);

    switch (t->type) {
        case VAR:
            return *lookup(t->var_name); // Get variable value

        case NUM:
            return t->attr; // Return numeric value

        case ASSOP: {
            int val = eval_tree(t->right);
            *lookup(t->left->var_name) = val; // Assign value
            return val;
        }

        case ADDOP:
            return eval_tree(t->left) + eval_tree(t->right);

        case MULOP:
            return eval_tree(t->left) * eval_tree(t->right);

        default:
            fprintf(stderr, "Error: Unknown node type %d\n", t->type);
            exit(1);
    }
    return 0;
}

void print_tree(tree_t *t, int spaces)
{
    if (t == NULL) return;

    // Indent for the current node level
    for (int i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }

    switch (t->type) 
    {
        case ADDOP:
            fprintf(stderr, "[ADDOP: %c]\n", t->attr);
            print_tree(t->left, spaces + 4);
            print_tree(t->right, spaces + 4);
            break;
            
        case MULOP:
            fprintf(stderr, "[MULOP: %c]\n", t->attr);
            print_tree(t->left, spaces + 4);
            print_tree(t->right, spaces + 4);
            break;

        case NUM:
            fprintf(stderr, "[NUM: %d]\n", t->attr);
            break;

        case VAR:
            fprintf(stderr, "[VAR: %s]\n", t->var_name);
            break;

        default:
            fprintf(stderr, "[UNKNOWN NODE TYPE: %d]\n", t->type);
            break;
    }
}

