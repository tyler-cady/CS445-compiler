#pragma once 

#include "node.h"
#define HASH_SIZE  211 


typedef struct hash_s
{
    node_t *table[HASH_SIZE];
    struct hash_s *next;
} hash_t;

hash_t *make_hash();
void unmake_hash( hash_t *);

void print_hash( hash_t *);

node_t *search_hash( hash_t *h, char *name );
node_t *insert_hash( hash_t *h, char *name );
