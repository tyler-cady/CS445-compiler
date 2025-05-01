#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg.h"

reg_t *rstack;

/* Initialize the register stack */
void reg_init( reg_t *stack ){
    memset(stack->rUsed, 0, sizeof(stack->rUsed));
    memset(stack->rContents, -1, sizeof(stack->rContents));
    memset(stack->tUsed, 0, sizeof(stack->tUsed));
    for ( int i =  0; i < REG_COUNT; i++ ){
        stack->stack[i] = 0;
    }
    stack->top = -1;
    for (int i = REG_COUNT -1; i >= 0; i--){
        stack->stack[stack->top++] = i;
    }
}

int ralloc( reg_t *stack, int id){
    int reg = pop(stack);
    if ( reg == -1 ) return -1; 
    stack->rContents[reg] = id;
    stack->tLoc[id] = reg;
    return reg;
}

int rfree( reg_t *stack, int reg){
    if ( stack->rUsed[reg] == 0 ) return -1;
    stack->rUsed[reg] = 0;
    stack->rContents[reg] = -1;
    push(reg, stack);
    return 0;
}

int talloc( reg_t *stack ){
    for ( int i = 0; i < MAX_TEMP; i++ ){
        if ( stack->tUsed[i] == 0 ){
            stack->tUsed[i] = 1;
            return i;
        }
    }
    return -1;
}
int tfree( reg_t *stack, int temp){
    if ( stack->tUsed[temp] == 0 ) return -1;
    stack->tUsed[temp] = 0;
    stack->tLoc[temp] = -1;
    return 0;
}

int top( reg_t *stack ){
    if ( stack->top == -1 ) return -1;
    return stack->stack[stack->top];
}

int pop( reg_t *stack ){
    if ( stack->top == -1 ) return -1;
    int reg = stack->stack[stack->top--];
    stack->rUsed[reg] = 1;
    return reg;
}

int push( int reg, reg_t *stack ){
    if ( stack->top == REG_COUNT - 1 ) return -1;
    stack->stack[++stack->top] = reg;
    stack->rUsed[reg] = 0;
    return 0;
}
char *get_reg_name( int reg ){
    switch ( reg ){
        case EAX: return "EAX";
        case EBX: return "EBX";
        case ECX: return "ECX";
        case EDX: return "EDX";
        case ESI: return "ESI";
        case EDI: return "EDI";
        case ESP: return "ESP";
        case EBP: return "EBP";
    }
    return NULL;
}

void print_reg_stack( reg_t *stack ){
    printf("Register Stack: ");
    for ( int i = 0; i < REG_COUNT; i++ ){
        if ( stack->rUsed[i] == 1 ){
            printf("%s ", get_reg_name(i));
        }
    }
    printf("\n");
}


// int main() {
//     rstack = (reg_t *)malloc(sizeof(reg_t));
//     reg_init(rstack);
//     printf("Register stack initialized.\n");
    
//     int reg1 = ralloc(rstack, 1);
//     printf("Allocated register: %s\n", get_reg_name(reg1));
    
//     int reg2 = ralloc(rstack, 2);
//     printf("Allocated register: %s\n", get_reg_name(reg2));
    
//     print_reg_stack(rstack);
    
//     rfree(rstack, reg1);
//     printf("Freed register: %s\n", get_reg_name(reg1));
    
//     print_reg_stack(rstack);
    
//     free(rstack);
//     return 0;
// }