#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg.h"

/* Initialize the register stack */
void reg_init(reg_t *stack) {
    memset(stack->rUsed, 0, sizeof(stack->rUsed));
    memset(stack->rContents, -1, sizeof(stack->rContents));
    memset(stack->tUsed, 0, sizeof(stack->tUsed));
    memset(stack->tLoc, -1, sizeof(stack->tLoc));

    stack->top = 0;
    for (int i = REG_COUNT - 1; i >= 0; i--) {
        stack->stack[stack->top++] = i;
    }
}

int ralloc(reg_t *stack, int id) {
    int reg = pop(stack);
    if (reg == -1) return -1;
    stack->rContents[reg] = id;
    stack->tLoc[id] = reg;
    return reg;
}

int rfree(reg_t *stack, int reg) {
    if (stack->rUsed[reg] == 0) return -1;
    stack->rUsed[reg] = 0;
    stack->rContents[reg] = -1;
    push(reg, stack);
    return 0;
}

int talloc(reg_t *stack) {
    for (int i = 0; i < MAX_TEMP; i++) {
        if (stack->tUsed[i] == 0) {
            stack->tUsed[i] = 1;
            return i;
        }
    }
    return -1;
}

int tfree(reg_t *stack, int temp) {
    if (stack->tUsed[temp] == 0) return -1;
    stack->tUsed[temp] = 0;
    stack->tLoc[temp] = -1;
    return 0;
}

int top(reg_t *stack) {
    if (stack->top == 0) return -1;
    return stack->stack[stack->top - 1];
}

int pop(reg_t *stack) {
    if (stack->top == 0) return -1;
    int reg = stack->stack[--stack->top];
    stack->rUsed[reg] = 1;
    return reg;
}

void push(int reg, reg_t *stack) {
    if (stack->top >= REG_COUNT) return;
    stack->stack[stack->top++] = reg;
    stack->rUsed[reg] = 0;
}

void swap(reg_t *stack) {
    if (stack->top < 2) return;
    int tmp = stack->stack[stack->top - 1];
    stack->stack[stack->top - 1] = stack->stack[stack->top - 2];
    stack->stack[stack->top - 2] = tmp;
}

char *get_reg_name(int reg) {
    switch (reg) {
        case EDX: return "edx";
        case ECX: return "ecx";
        case EBX: return "ebx";
        case EAX: return "eax";
        default: return "Unknown Register";
    }
}

void print_reg_stack(reg_t *stack) {
    printf("Register Stack: ");
    for (int i = 0; i < REG_COUNT; i++) {
        if (stack->rUsed[i] == 1) {
            printf("%s ", get_reg_name(i));
        }
    }
    printf("\n");
}
