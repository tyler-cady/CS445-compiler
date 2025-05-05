#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "hash.h"
#include "list.h"
#include "tree.h"
#include "pc.tab.h"
#include "semantic.h"
#include "util.h"

static void tree_label(tree_t *t);

tree_t *tmake(int type, tree_t *left, tree_t *right) {
    tree_t *node = (tree_t *)malloc(sizeof(tree_t));
    if (!node) {
        error_fatal_malloc();
        exit(1);
    }
  
    
    node->type = type;
    node->scope_depth = 0;
    node->label = 0;
    node->caller_is_parent = 0;
    node->left = left ? left : NULL;
    node->right = right ? right : NULL;

    tree_label(node);
    return node;
}

void tree_free(tree_t *t) {
    if (!t) return; 
    if (t->left) tree_free(t->left);
    if (t->right) tree_free(t->right); 
    free(t);
}

tree_t *tmake_id(list_t *name_ptr) {
    if (!name_ptr) return NULL;
    
    tree_t *id = tmake(ID, NULL, NULL); 
    id->attr.name_ptr = name_ptr;
    id->type = ID;
    // THIS SEEMS TO BE THE BUG (WHY CLOBBER INTO VOID?): name_ptr->type = VOID;
    return id;
}

tree_t *tmake_inum(int ival) {
    tree_t *v = tmake(INUM, NULL, NULL);
    v->attr.ival = ival;
    v->type = INTEGER;
    return v;
}

tree_t *tmake_rnum(float rval) {
    tree_t *v = tmake(RNUM, NULL, NULL);
    v->attr.rval = rval;
    v->type = REAL;
    return v;
}

tree_t *tmake_mulop(int opval, tree_t *left, tree_t *right) {
    tree_t *v = tmake(MULOP, left, right);
    v->attr.opval = opval;
    v->type = sem_get_type(left);
    return v;
}

tree_t *tmake_addop(int opval, tree_t *left, tree_t *right) {
    tree_t *v = tmake(ADDOP, left, right);
    v->attr.opval = opval;
    v->type = sem_get_type(left);
    return v;
}

tree_t *tmake_relop(int opval, tree_t *left, tree_t *right) {
    tree_t *v = tmake(RELOP, left, right);
    v->type = sem_get_type(left);
    v->attr.opval = opval;
    return v;
}

tree_t *tmake_program(list_t *name, tree_t *args, tree_t *decls, tree_t *subprograms, tree_t *body) {
    if (!name || !decls || !body) return NULL;
    tree_t *v = tmake(PROG_DECL1, args, decls);
    v->attr.name_ptr = name;
    v->left->left = body;
    v->left->right = subprograms;
    return v;
}

const char *type_to_str(int type){
    switch(type){
        case PLUS:     return "+";
        case MINUS:    return "-";
        case SLASH:    return "/";
        case STAR:     return "*";
        case DIV:      return "DIV";
        case MOD:      return "MOD";
        case AND:      return "AND";
        case OR:       return "OR";
        case EQ:       return "=";
        case ASSIGNOP: return ":=";
        case NE:       return "<>";
        case LT:       return "<";
        case GT:       return ">";
        case LE:       return "<=";
        case GE:       return ">=";
        case COMMA:    return ",";
        case SEMICOLON:return ";";
        case COLON:    return ":";
        case DOUBLEDOT:return "..";
        case INTEGER: return "INTEGER";
        case REAL:    return "REAL";
        case BOOL:    return "BOOL";
        case VOID:    return "VOID";
        case ID_LIST: return "ID_LIST";
        case IARRAY: return "IARRAY";
        case RARRAY: return "RARRAY";
        default: {
            static char buffer[32];
            snprintf(buffer, sizeof(buffer), "UNKNOWN(%d)", type);
            return buffer;
        }
    }
}



void tprint(tree_t *t, int spaces) {
    if (!t) {
        if (verbose_flag) {
            for (int i = 0; i < spaces; i++) echo(" ", verbose_flag);
            echo("[NULL]\n", verbose_flag);
        }
        return;
    }

    if (verbose_flag) {
        for (int i = 0; i < spaces; i++) echo(" ", verbose_flag);
        switch (t->type){
            case PROG_DECL1:
                echo("\n\n****************TREE*******************\n", verbose_flag);
                echo("\n[PROGRAM DECLARATION]\n", verbose_flag);
                break;
            case PROG_DECL2:
                echo("[PROGRAM DECLARATION 2]\n", verbose_flag);
                break;
            case SUBPROG_DECLS:
                echo("[SUBPROGRAM DECLARATION LIST]\n", verbose_flag);
                break;
            case DECLS:
                echo("[DECLARATIONS]\n", verbose_flag);
                break;
            case SUBPROGRAM:
                echo("[SUBPROGRAM]\n", verbose_flag);
                break;
            case SUBPROG_DECL:
                echo("[SUBPROGRAM_DECLARE]\n", verbose_flag);
                break;
            case ADDOP:
                echo("[ADDOP:%d]\n", verbose_flag, t->attr.opval);
                break;
            case MULOP:
                echo("[MULOP:%d]\n", verbose_flag, t->attr.opval);
                break;
            case ASSIGNOP:
                echo("[ASSIGNOP]\n", verbose_flag);
                break;
            case ID_LIST:
                echo("[ID LIST]\n", verbose_flag);
                break;
            case ID:
                echo("[ID]\n", verbose_flag);
                break;
            case RNUM:
                echo("[RNUM:%f]\n", verbose_flag, t->attr.rval);
                break;
            case INUM:
                echo("[INUM:%d]\n", verbose_flag, t->attr.ival);
                break;
            case INTEGER:
                echo("[INTEGER]\n", verbose_flag);
                break;
            case REAL:
                echo("[REAL]\n", verbose_flag);
                break;
            case RANGE:
                echo("[RANGE]\n", verbose_flag);
                break;
            case CMPND_STMT:
                echo("[COMPOUND STATEMENT]\n", verbose_flag);
                break;
            case STMT_LIST:
                echo("[STATEMENT LIST]\n", verbose_flag);
                break;
            case IF:
                echo("[IF]\n", verbose_flag);
                break;
            case THEN:
                echo("[THEN]\n", verbose_flag);
                break;
            case REPEAT:
                echo("[REPEAT]\n", verbose_flag);
                break;
            case WHILE:
                echo("[WHILE]\n", verbose_flag);
                break;
            case FOR:
                echo("[FOR]\n", verbose_flag);
                break;
            case FOR_ARG:
                echo("[FOR ARGUMENT]\n", verbose_flag);
                break;
            case ARRAY_CALL:
                echo("[ARRAY ACCESS]\n", verbose_flag);
                break;
            case PROC_CALL:
                echo("[PROCEDURE CALL]\n", verbose_flag);
                break;
            case EXPR_LIST:
                echo("[EXPR LIST]\n", verbose_flag);
                break;
            case RELOP:
                echo("[RELOP:%s]\n", verbose_flag, type_to_str(t->attr.opval));
                break;
            case FUNC_CALL:
                echo("[FUNCTION CALL]\n", verbose_flag);
                break;
            case NOT:
                echo("[NOT]\n", verbose_flag);
                break;
            case PARAM_LIST:
                echo("[PARAM LIST]\n", verbose_flag);
                break;
            case PROCEDURE:
                echo("[PROCEDURE]\n", verbose_flag);
                break;
            default:
                echo("[TOKEN(%d)]\n", verbose_flag, t->type);
                break; 
        }
    }

    if (t->left || t->right) {
            tprint(t->left, spaces + 4);
            tprint(t->right, spaces + 4);
    }
}


static void tree_label(tree_t *n) {
    if (!n) return;

    // Leaf node
    if (!n->left && !n->right) {
        n->label = n->caller_is_parent ? 1 : 0;
        return;
    }
    if (n->left) {
        n->left->caller_is_parent = 1;
        tree_label(n->left);
    }
    if (n->right) {
        n->right->caller_is_parent = 0;
        tree_label(n->right);
    }
    int max_label = 0;
    if (n->left)  max_label = n->left->label;
    if (n->right) {
        int val = n->right->label + 1;
        if (val > max_label) max_label = val;
    }

    n->label = max_label;
}


// int main(){
//     /* Test Program
//     program main(input, output);
//     var
//         a: integer;
//         b: real;
//         c: boolean;
//     begin 
//     end.
//     */
    
//     list_t *name = list_insert(NULL, "main");
//     tree_t *args = tmake(ID_LIST, tmake_id(list_insert(NULL, "input")), tmake_id(list_insert(NULL, "output")));
//     tree_t *decls = tmake(DECLS, tmake_id(list_insert(NULL, "a")), tmake_id(list_insert(NULL, "b")));
//     tree_t *body = tmake(CMPND_STMT, NULL, NULL);
//     tree_t *program = tmake_program(name, args, decls, NULL, body);
//     tprint(program, 0);
//     tree_free(program);
//     return 0;
// }
