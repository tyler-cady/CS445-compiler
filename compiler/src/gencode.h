#pragma once
#include "structs/tree.h"
#include "structs/hash.h"

#define MAX_REGISTERS 16
/**
 * x86_64 Register names:
 * RAX, RBX, RCX, RDX
 * RSI, RDI, RBP, RSP
 * R8, R9, R10, R11
 * R12, R13, R14, R15
 */

int rstack[MAX_REGISTERS];
int tstack[MAX_REGISTERS];

int rtop = -1;
int ttop = -1;

static int is_leaf(tree_t *n);
void push(int *stack, int *top, int val);
int pop(int *stack, int *top);
void swap(int *stack, int *top);
void gencode(tree_t *n, char *filename);
static char *extension_to_asm(const char *filename);