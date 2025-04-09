#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int check_types( tree_t *left, tree_t *right ){
    if ( left->type != right->type ){
        char *msg;
        asprintf(&msg, "types")
    }
}
int assert_type(tree_t *t, int type);

int check_variable_init(int type_token, tree_t *t);

int check_function(tree_t *func, tree_t *func_call);
int check_procedure(tree_t *proc, tree_t *proc_call);
int check_operation(int op, tree_t *left, tree_t *right);
int check_if(tree_t *t);
int check_while(tree_t *t);
int check_for(tree_t *t);
int check_repeat(tree_t *t);

static int check_redeclaration(tree_t *t) {
    if (!t || t->type != ID) return 0;

    list_t *existing = hash_search_all_depth(current_scope, t->value.sval->name, &t->scope_depth);
    if (existing != NULL) {
        fprintf(stderr, "Error: redeclaration of variable '%s'\n", t->value.sval->name);
        return 1;
    }

    return 0;
}

static int check_unused(tree_t *t){
    
}
static int check_undeclared(tree_t *t) {
    if (!t || t->type != ID) return 0;

    list_t *entry = hash_search_all(current_scope, t->value.sval->name);
    if (!entry) {
        fprintf(stderr, "Error: undeclared variable '%s'\n", t->value.sval->name);
        return 1;
    }

    return 0;
}
