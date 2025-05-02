#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "list.h"
#include "tree.h"
#include "hash.h"


int main(){
    hash_t *table = hash_make();
    // hash_insert(table, "a");
    tree_t *n = tree_make_id(hash_search_all(table, "a"), NULL, NULL);
    tree_print(n, 0);
    tree_free(n);
    hash_free(table);
    return 0;
}