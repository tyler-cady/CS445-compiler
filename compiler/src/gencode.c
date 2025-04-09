#include "gencode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static int is_leftmost_leaf(tree_t *n){
    if (!n) return 0;
    if (n->left) return is_leftmost_leaf(n->left);
    if (n->right) return 0;
    return 1;
}

void push(int *stack, int *top, int val){
    if (*top < MAX_REGISTERS){
        (*top)++;
        stack[*top] = val;
    }
    else {
        fprintf(stderr, "Error: Stack overflow\n");
        exit(1);
    }
}

int pop(int *stack, int *top){
    if (*top >= 0) return stack[(*top)--];
    else {
        fprintf(stderr, "Error: Stack underflow\n");
        exit(1);
    }
}

void swap(int *stack, int *top){
    if (*top < 1){
        fprintf(stderr, "Error: Not enough elements to swap\n");
        exit(1);
    }
    int tmp = stack[*top];
    stack[*top] = stack[*top - 1];
    stack[*top - 1] = tmp;
}


static char *extension_to_asm(const char *filename) {
    const char *dot = strrchr(filename, '.');
    size_t new_filename_len = (dot ? dot - filename : strlen(filename)) + 5;  
    char *new_filename = (char *)malloc(new_filename_len);
    if (!new_filename) {
        perror("Memory allocation failed");
        return NULL;
    }
    if (dot) strncpy(new_filename, filename, dot - filename);
    else strcpy(new_filename, filename);
    strcat(new_filename, ".asm");
    return new_filename;
}

void gencode(tree_t *node, char *filename){
    filename = extension_to_asm(filename);
    printf("Generating code to %s\n", filename);

    FILE *f = fopen(filename, "w");
    if (!f){
        perror("Error opening file");
        exit(1);
    }

    fprintf(f, ".intel_syntax noprefix\n");
    fprintf(f, ".global _start\n");
    fprintf(f, "_start:\n");

    /* Case 0 */
    if (is_leftmost_leaf(node)){
        fprintf(f, "MOV rax, %d\n", name);
        
    }

}
int main(){
    tree_t *t = tree_make_inum(10);
    gencode(t, "test.c");
    return 0;

}