#pragma once

#include "tree.h"
#include "hash.h"
#include "error.h"
#include "reg.h"


char *Label = ".LL";
char *EndLabel = ".LLfe";
char *ReadIntLabel = ".LLC0";
char *WriteIntLabel = ".LLC1";
char *ReadRealLabel = ".LLD0";
char *WriteRealLabel = ".LLD1";
char *TrueLabel = ".LLt";
char *FalseLabel = ".LLf";

void file_header(char *fname);
void file_footer();
void write_main_header();
void write_io();
void write_main_footer();
void func_header(tree_t *t);
void func_footer(tree_t *t);
char* opval_to_x86(int opval);