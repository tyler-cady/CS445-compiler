#include "gencode.h"

void label_tree(tree_t *n) {
    if (n == NULL) return;

    if (n->left == NULL && n->right == NULL) {
        if (n->caller_is_parent == 1) {
            n->label = 1;
        } 
        else {
            n->label = 0;
        }
    } 
    else {
        label_tree(n->left);
        label_tree(n->right);

        int max_label = 0;
        int i = 1;

        tree_t *children[] = {n->left, n->right};
        for (int j = 0; j < 2; j++) {
            if (children[j] != NULL) {
                int child_label = children[j]->label + i - 1;
                if (child_label > max_label) {
                    max_label = child_label;
                }
                i++;
            }
        }
        n->label = max_label;
    }
}

