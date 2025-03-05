#pragma once 
#include <stdio.h>
#include <stdlib.h>
#include "structs/tree.h"



extern FILE *out;

void label( tree_t *n);
void gencode( tree_t *n );
