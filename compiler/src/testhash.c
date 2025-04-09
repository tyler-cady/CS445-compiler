#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "hash.h"

void test_hash_basic_operations() {
    printf("Running basic hash table tests...\n");

    hash_t *table = hash_make();
    assert(table != NULL);
    assert(table->capacity == MAX_CAPACITY);

    // Insert entries
    hash_insert(table, "alpha");
    hash_insert(table, "beta");
    hash_insert(table, "gamma");

    // Search for existing entry
    list_t *found = hash_search(table, "beta");
    assert(found != NULL);
    assert(strcmp(found->name, "beta") == 0);

    // Search for non-existing entry
    list_t *not_found = hash_search(table, "delta");
    assert(not_found == NULL);

    // Print hash table
    hash_print(table);

    // Free memory
    hash_free(table);

    printf("✅ Basic hash table test passed.\n\n");
}

void test_scope_stack_push_pop() {
    printf("Running scope stack push/pop tests...\n");

    hash_t *stack = NULL;
    
    // Push valid hash_t instances onto the stack
    stack = hash_push(stack, hash_make()); // Push first scope
    stack = hash_push(stack, hash_make()); // Push second scope
    stack = hash_push(stack, hash_make()); // Push third scope

    // Ensure the stack has 3 levels
    assert(stack != NULL && stack->next != NULL && stack->next->next != NULL);

    // Pop one scope
    stack = hash_pop(stack); // pop one scope
    assert(stack != NULL && stack->next != NULL);
    assert(stack->next->next == NULL); // only two remain

    // Pop second scope
    stack = hash_pop(stack); // pop second scope
    assert(stack != NULL);  // One scope should remain

    // Pop third (last) scope
    stack = hash_pop(stack); // pop last scope
    assert(stack == NULL);  // Stack should be empty now

    hash_exit(&stack); // Free memory (not strictly necessary since stack is NULL here)
    assert(stack == NULL);

    printf("✅ Scope stack push/pop test passed.\n\n");
}


void test_hash_search_all() {
    printf("Running hash_search_all test...\n");

    hash_t *global = hash_push(NULL, 0);
    hash_insert(global, "x");

    hash_t *local = hash_push(global, 0);
    hash_insert(local, "y");

    list_t *res1 = hash_search_all(local, "x"); // should find in global
    list_t *res2 = hash_search_all(local, "y"); // should find in local
    list_t *res3 = hash_search_all(local, "z"); // not found

    assert(res1 && strcmp(res1->name, "x") == 0);
    assert(res2 && strcmp(res2->name, "y") == 0);
    assert(res3 == NULL);

    int depth = -1;
    list_t *res4 = hash_search_all_depth(local, "x", &depth);
    assert(res4 && strcmp(res4->name, "x") == 0);
    assert(depth == 1);

    hash_exit(&local);
    printf("✅ hash_search_all and depth test passed.\n\n");
}

int main() {
    test_hash_basic_operations();
    test_scope_stack_push_pop();
    test_hash_search_all();

    printf("All tests completed successfully!\n");
    return 0;
}
