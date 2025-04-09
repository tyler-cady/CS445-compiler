#ifndef PTYPE_H
#define PTYPE_H


typedef enum {
    INTEGER_T,
    REAL_T,
    ARRAY_T,
    FUNCTION_T,
    PROCEDURE_T
} ptype_enum;

typedef struct ptype_s {
    ptype_enum type;   
    int size;          
    struct ptype_s *subtype; 
} ptype_t;

#endif // PTYPE_H