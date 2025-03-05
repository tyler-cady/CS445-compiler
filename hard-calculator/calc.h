#pragma once 
/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
/* headers */
#include "consts.h"
#include "tree.h"
#include "sym_table.h"


FILE *fp;
int curr_token;
int curr_attr; /* Current Attribute */
int tvalue;


int get_token();
void match( int );
void program();
void decls();
void id_list();
void type();
void cmpd_stmt();
void opt_stmts();
void stmt();
tree_t *E(), *T(), *F();

