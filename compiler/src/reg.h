#ifndef REG_H
#define REG_H

/* To be used for gencode */

#define REG_COUNT 4
#define MAX_TEMP 128

typedef enum {
    EDX = 0, ECX = 1, EBX = 2, EAX = 3
} Reg;

/* Struct for the register allocator */
typedef struct reg_s
{
    int rUsed[REG_COUNT]; /* 1 if used, 0 if free */
    int rContents[REG_COUNT]; /* temp id in each register */
    int tUsed[MAX_TEMP]; /* 1 if used, 0 if free */
    int tLoc[MAX_TEMP]; /* location of temp in register */
   
    int stack[REG_COUNT];
    int top; /* top of the stack */
} reg_t; 


int top( reg_t *stack );
int pop( reg_t *stack );
void push( int reg, reg_t *stack );
void swap( reg_t *stack);
char *get_reg_name( int reg );
void print_reg_stack( reg_t *stack );
void reg_init( reg_t *stack );
/* Temps */
int talloc( reg_t *stack );
int tfree( reg_t *stack, int temp );

/* Registers */
int ralloc( reg_t *stack, int id );
int rfree( reg_t *stack, int reg );

#endif // REG_H