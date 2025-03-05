#include "node.h"
#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define EOS '\0'

hash_t *make_hash()
{
    hash_t *p = (hash_t *)malloc(sizeof(hash_t));
    assert(p != NULL );
    for (int i = 0; i < HASH_SIZE; i++)
    {
        p->table[i] = NULL;
    }
    p->next = NULL;
    return p;
}
void unmake_hash( hash_t *);

void print_hash( hash_t *);

node_t *search_hash( hash_t *h, char *name )
{
    assert(h != NULL);
    int idx = hash_pjw( name );
    return search_node( h->table[idx], name);
}

node_t *insert_hash( hash_t *h, char *name );

/* Peter Weinberger's hashpjw function */
int hash_pjw(char *s)
{
    char *p; 
    unsigned h = 0, g;
    for (p = s; *p != EOS; p=p+1)
    {
        h = (h << 4 ) + (*p); // multiply by 16
        if ( g = h&0xf0000000) /* If g != 0 */
        {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h % HASH_SIZE; 
}