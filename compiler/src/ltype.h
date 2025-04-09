#ifndef LTYPE_H
#define LTYPE_H

#include "ptype.h"


typedef struct ltype_s
{
    ptype_t *type;          /* Types: integer, real, etc. */
    struct ltype_s *next;

} ltype_t;

#endif // LTYPE_H