#include "sym_table.h"
scope_stack_t sym_table = {.top = -1};

unsigned int hash_pjw(char *s)
{
    unsigned int h = 0, g;
    for (; *s; s++)
    {
        h = (h << 4) + *s;
        if ((g = h & 0xf0000000))
        {
            h ^= g >> 24;
            h ^= g;
        }
    }
    return h % TABLE_SIZE;
}

hashmap_t *create_hashmap()
{
    hashmap_t *new_hashmap = (hashmap_t *)malloc(sizeof(hashmap_t));
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        new_hashmap->table[i] = NULL;
    }
    return new_hashmap;
}

void enter_scope()
{
    sym_table.top++;
    sym_table.scope[sym_table.top] = create_hashmap();
}

void exit_scope()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        var_entry_t *entry = sym_table.scope[sym_table.top]->table[i];
        while (entry)
        {
            var_entry_t *temp = entry;
            entry = entry->next;
            free(temp->key);
            free(temp);
        }
    }
    free(sym_table.scope[sym_table.top]);
    sym_table.top--;
}

int *lookup(char *key)
{
    unsigned int index = hash_pjw(key);
    var_entry_t *entry = sym_table.scope[sym_table.top]->table[index];
    while (entry)
    {
        if (strcmp(entry->key, key) == 0)
        {
            return &entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}


void set_var(char *key, int value)
{
    unsigned int index = hash_pjw(key);
    var_entry_t *entry = sym_table.scope[sym_table.top]->table[index];
    while (entry)
    {
        if (strcmp(entry->key, key) == 0)
        {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }
    var_entry_t *new_entry = (var_entry_t *)malloc(sizeof(var_entry_t));
    new_entry->key = (char *)malloc(strlen(key) + 1);
    strcpy(new_entry->key, key);
    new_entry->value = value;
    new_entry->next = sym_table.scope[sym_table.top]->table[index];
    sym_table.scope[sym_table.top]->table[index] = new_entry;
}

