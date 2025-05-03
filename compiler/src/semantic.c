#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "list.h"
#include "tree.h"
#include "hash.h"
#include "util.h"
#include "semantic.h"
#include "pc.tab.h"



/*
t1-1a.p: (* ERROR: variable redeclared *)
t1-1b.p: (* ERROR: variable redeclared *)
t1-2.p: (* LEGAL: local names should hide non-local names *)
t1-3.p: (* LEGAL: non-local names visible from inner scopes *)
t1-4.p: (* LEGAL: scope of the name of subprograms *)
t1-5.p: (* ERROR: local objects not defined outside their scopes *)
t2-2.p: (* ERROR: objects must be declared before used in expressions *)
t2-3.p: (* ERROR: objects of different types appear in the same expression *)
t3-2.p: (* ERROR: test expressions in IF/WHILE statements must be Boolean *)
t3-3.p: (* dangling ELSE binds to closest IF *)
t3-4.p: (* ERROR: mismatched type for index variable in FOR loops *)
t4-1.p: (* ERROR: non-integer type for array index *)
t5-2.p: (* ERROR: function missing return statement *)
t5-3.p: (* ERROR: function passed wrong number/type of arguments *)
t5-4.p: (* ERROR: function updating non-local variable *)
t6-1.p: (* ERROR: procedures don't return values *)
t6-2.p: (* ERROR: procedure passed wrong number/type of arguments *)
*/

/* Semantic Errors */

int is_declared_in_scope(hash_t *table, char *name) {
    if (!table || !name) return 0;
    list_t *found = hash_search(table, name);
    if (found) return 1;
    return 0;
}

int is_declared( hash_t *table, char *name ){
    if (!table || !name) return 0;
    list_t *found = hash_search_all(table, name);
    if (found) return 1;
    return 0;
}

void sem_set_types( tree_t *id_list, int type, int scopetype){
    if (!id_list) return;
    if (id_list->type == ID) {
        // id_list->attr.name_ptr->type = type;
        // id_list->attr.name_ptr->scopetype = scopetype;
        list_t *l = hash_search_all(symbol_tbl, id_list->attr.name_ptr->name);
        hash_set_type( symbol_tbl, l->name, type, scopetype );

        // fprintf(stderr, "[set type: %s, %d]\n", id_list->attr.name_ptr->name, id_list->attr.name_ptr->type);
    }
    else if (id_list->type == ID_LIST) {
        sem_set_types(id_list->left, type, scopetype);
        sem_set_types(id_list->right, type, scopetype);
    }
    else {
        fprintf(stderr, "Error: invalid id_list type\n");
    }
}

int sem_get_type( tree_t *t ){
     list_t *id; 
     if ( !t ) return ERROR; 
     switch ( t->type ){
        case ID:
        case ARRAY:
        case FUNCTION:
        case PROCEDURE:
            id = hash_search_all( symbol_tbl, t->attr.name_ptr->name);
        //  fprintf(stderr, "[ID TYPE FOUND: %s, %d]", id->name, id->type);
            return id->type; 
        case ADDOP:
        case MULOP:
        case OROP:
        case ANDOP:
        case RELOP:
            return t->left->type;
        default:
            return t->type;
     }

}

int sem_assert_types(tree_t *left, tree_t *right, int type_assertion){
    if (!left || !right) return 0;
    int l = sem_get_type(left);
    int r = sem_get_type(right);
    if ( l == r && r == type_assertion ) {
        return 0;
    }
    return 1;
}

int sem_check_types( tree_t *left, tree_t *right){
    if ( !left || !right ) return 0;
    int l = sem_get_type(left);
    int r = sem_get_type(right);
    if ( l == r ) {
        return 0;
    } 
    return 1;
}

int is_initialized(char *name){
    if ( hash_search(symbol_tbl, name)->initialized) return 1;
    else return 0;                         
}

int sem_check_assign(tree_t *var, tree_t *assign ){
    if (!var || !assign) return 1;
    int tvar = sem_get_type(var);
    int tassign = sem_get_type(assign);
    char *msg;
    hash_init_symbol(symbol_tbl, var->attr.name_ptr->name);

    if ( assign->type == ID){
        is_initialized(assign->attr.name_ptr->name);
        if (!is_declared_in_scope(symbol_tbl, assign->attr.name_ptr->name)){
            asprintf(&msg, "semantic error: '%s' is not declared in the scope", assign->attr.name_ptr->name);
            yyerror(msg);
            return 1;
        }
    } 
    
    if ( tassign == PROCEDURE ) {
        asprintf(&msg, "semantic error: cannot assign to var( '%s' )", var ? var->attr.name_ptr->name : NULL);
        yyerror(msg);
        free(msg);
        return 1;
    }
    else if ( tvar != tassign ) {
        asprintf(&msg, "semantic error: cannot assign '%s' to '%s'", type_to_str(tvar), type_to_str(tassign));
        yyerror(msg);
        free(msg);
        return 1;
    }
    return 0;
}
int check_local_hides_nonlocal(hash_t *table, char *name){
    if (hash_search(table, name)->scopetype == LOCAL) return 0;
    else {
        /* check if a non-local var is found in outer scopes*/
        if ( hash_search_all(table, name) ) {
            return 0;
        }
    }
    char *msg;
    asprintf(&msg, "semantic error: '%s' is declared in a non-local scope", name);
    yyerror(msg);
    free(msg);
    return 1;
}

int check_nonlocal_visibility(hash_t *table, char *name){
    /* Checks if it exists locally -- local overrides global */
    if (hash_search(table, name)) return 0; /* Symbol exists in the current scope */
    if (hash_search_all(table, name)) return 0; /* Symbol exists somewhere */
    char *msg;
    asprintf(&msg, "semantic error: '%s' is not declared", name);
    yyerror(msg);
    free(msg);
    return 1;
}

// int main(){

//    symbol_tbl = hash_make();
//    symbol_tbl = hash_push(symbol_tbl);
//    hash_insert(symbol_tbl, "F");
//    symbol_tbl = hash_push(symbol_tbl);
//    tree_t *t = tmake_id(hash_search_all(symbol_tbl, "F"));
//    fprintf(stderr, "\n%s\n", t->attr.name_ptr->name);
//    tree_t *a = tmake(ASSIGNOP, t, tmake_inum( 5 ) );
//    sem_set_types(t, INTEGER, LOCAL);
//    printf("\ntype:%d\n", sem_get_type(a->left));
// }
