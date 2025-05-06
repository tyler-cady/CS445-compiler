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
    list_t *found = hash_global_search(table, name);
    if (found) return 1;
    return 0;
}

void sem_set_types( tree_t *id_list, int type, int scopetype){
    if (!id_list) return;
    if (id_list->type == ID) {
        // id_list->attr.name_ptr->type = type;
        // id_list->attr.name_ptr->scopetype = scopetype;
        list_t *l = hash_global_search(symbol_tbl, id_list->attr.name_ptr->name);
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

int sem_get_type( tree_t* t ) {
    list_t *id;
	if(t == NULL) return ERROR;
	int left_type, right_type;

	switch( t->type ) {
	case ID:
        id = hash_global_search(symbol_tbl, t->attr.name_ptr->name);
        if (!id) return ERROR;
        return id->type;
	case INTEGER:
	case INUM:
		return INTEGER;
	case REAL:
	case RNUM:
		return REAL;
	case MULOP:
	case ADDOP:
		left_type = sem_get_type( t->left );
		right_type = sem_get_type( t->right );

		if( left_type != right_type ) {
			fprintf(stderr, "ERROR: type mismatch in %s, recieved %d and %d\n", ((t->type) == ADDOP ? "ADDOP" : "MULOP"), left_type, right_type );
			exit(1);
		}

		return left_type;
	case RELOP:
	case AND:
	case OR:
		return BOOL;
	case IARRAY:
		return INTEGER;
	case RARRAY:
		return REAL;
	case ARRAY_CALL:
		int array_type = t->left->attr.opval;
		if(array_type == IARRAY)	return INTEGER;
		else if(array_type == RARRAY)	return REAL;
		return array_type;
	case FUNC_CALL:
		return sem_get_type( t->left );
	default:
		return ERROR;
	}
}


int sem_assert_types(tree_t *left, tree_t *right, int type_assertion){
    if (!left || !right) return 0;
    int l = sem_get_type(left);
    int r = sem_get_type(right);
    if ( l == r && r == type_assertion ) {
        return 1;
    }
    return 0;
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
    if ( hash_global_search(symbol_tbl, name)->initialized) return 1;
    else return 0;                         
}

int sem_check_assign(tree_t *var, tree_t *assign ){
    if (!var || !assign) return 1;
    int tvar = sem_get_type(var);
    int tassign =  sem_get_type(assign);
    char *msg;
    /* Initialize the var id */
    char *name;
    if (var->type == ID){
        name = var->attr.name_ptr->name;
        hash_init_symbol(symbol_tbl, name);
    }
    if (tvar == IARRAY || tvar == RARRAY){
        name = var->left->attr.name_ptr->name;
        hash_init_symbol(symbol_tbl, name );

        /* Set types to integer and real for type checking */
        if( tvar == IARRAY ) tvar = INTEGER;
        if( tvar == RARRAY ) tvar = REAL;
       
        if( tassign == IARRAY ) tassign = INTEGER;
        if( tassign == RARRAY ) tassign = REAL;
    }

    if ( assign->type == ID || assign->type == IARRAY || assign->type == RARRAY){

        if ( !is_initialized( name )){
            asprintf(&msg, "semantic error: '%s' is not initialized", name);
            yyerror(msg);
            return 1;
        }
        list_t *list = hash_global_search(symbol_tbl, name);
        if (list){
            if (!is_declared_in_scope(symbol_tbl, name) && list->scopetype != FUNCTION){
                asprintf(&msg, "semantic error: '%s' is not declared in the scope", name);
                yyerror(msg);
                return 1;
            }
            if (list->scopetype == FUNCTION){
                fprintf(stderr, "{initializing...%s}", list->name);
                hash_init_symbol(symbol_tbl, list->name);
            }
        }
    } 
    
    if ( tassign == PROCEDURE ) {
        asprintf(&msg, "semantic error: cannot assign to var( '%s' )", var ? var->attr.name_ptr->name : NULL);
        yyerror(msg);
        free(msg);
        return 1;
    }

    else if ( tvar != tassign) {
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
        if ( hash_global_search(table, name) ) {
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
    if (hash_global_search(table, name)) return 0; /* Symbol exists somewhere */
    char *msg;
    asprintf(&msg, "semantic error: '%s' is not declared", name);
    yyerror(msg);
    free(msg);
    return 1;
}
int count_tree_ids(tree_t *t) {
    if (!t) return 0;
    int count = (t->type == ID) ? 1 : 0;
    if(t->left) count += count_tree_ids(t->left);
    if (t->right) count += count_tree_ids(t->right);
    return count;
}

// int fill_argtypes(tree_t *t, int *types, int index) {
//     if (!t) return index;
//     if (t->type == ID) {
//         types[index++] = hash_search(symbol_tbl, t->attr.name_ptr->name)->type;
//     }
//     if(t->left) index = fill_argtypes(t->left, types, index);
//     if(t->right) index = fill_argtypes(t->right, types, index);
//     return index;
// }
int fill_argtypes(tree_t *t, int *types, int index, int max) {
    if (!t || index >= max) return index;

    if (t->type == ID) {
        list_t *sym = hash_search(symbol_tbl, t->attr.name_ptr->name);
        if (!sym) {
            fprintf(stderr, "semantic error: undeclared identifier '%s'\n", t->attr.name_ptr->name);
            exit(1);
        }
        types[index++] = sym->type;
    }

    if (t->left) index = fill_argtypes(t->left, types, index, max);
    if (t->right) index = fill_argtypes(t->right, types, index, max);
    return index;
}

// int *sem_get_argtype(tree_t *args) {
//     int count = count_tree_ids(args);
//     int *types = malloc(count * sizeof(int));
//     if (!types) {
//         perror("malloc failed");
//         exit(1);
//     }
//     fill_argtypes(args, types, 0);
//     return types;
// }
int *sem_get_argtype(tree_t *args, int *count_out) {
    int count = count_tree_ids(args);
    int *types = malloc(count * sizeof(int));
    if (!types) {
        perror("malloc failed");
        exit(1);
    }

    int filled = fill_argtypes(args, types, 0, count);
    if (filled != count) {
        fprintf(stderr, "internal error: mismatched arg count (%d != %d)\n", filled, count);
        exit(1);
    }

    *count_out = count;
    return types;
}

// int sem_comp_args(int *a, int *b, int size){
//     for( int i = 0; i < )
// }
void sem_print_argtypes(int *args, int count) {
    for (int i = 0; i < count; i++) {
        printf("arg[%d] = %d\n", i, args[i]);
    }
}


// int main(){
//     list_t *list = list_make("x");
//     tree_t *addop = tmake(ADDOP, tmake_inum(5), tmake_inum(10));
//     tree_t *f = tmake(ASSIGNOP, tmake_id(list), addop);
//     tprint(f, 0);
//     fprintf(stderr, "TYPE: %d\n", sem_get_type(f));
// }

