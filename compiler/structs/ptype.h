typedef enum {
    INTEGER,
    REAL,
    ARRAY,
    FUNCTION,
    PROCEDURE
} ptype_enum;

typedef struct ptype_s {
    ptype_enum type;   
    int size;          
    struct ptype_s *subtype; 
} ptype_t;