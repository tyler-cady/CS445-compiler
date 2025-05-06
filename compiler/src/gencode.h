#pragma once

#include "tree.h"
#include "hash.h"
#include "error.h"
#include "reg.h"


void gfile_header(char *fname);
void gfile_footer();
void gmain_header();
void gmain_footer();
void gfunc_header(tree_t *func);
void gfunc_footer(tree_t *func);
void gread_write();
void gprintf_call(int *regs, int num_regs);
void gscanf_call(int *regs, int num_regs);
char *replace_extension(char *);
void generate_code( tree_t *root );


char *label();


