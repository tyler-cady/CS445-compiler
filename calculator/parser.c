#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "parser.h"

/* Function Declarations */
int E(char *tokens[], int *index);
int T(char *tokens[], int *index);
int F(char *tokens[], int *index);
void match(char *expectedToken, char *tokens[], int *index);

int E(char *tokens[], int *index) {
    // printf("E: Current token: %s\n", tokens[*index]);
    T(tokens, index);
    while (strcmp(tokens[*index], "+") == 0) {
        match("+", tokens, index);
        T(tokens, index);
    }
    return 1; // Return success
}

int T(char *tokens[], int *index) {
    // printf("T: Current token: %s\n", tokens[*index]);
    F(tokens, index);
    while (strcmp(tokens[*index], "*") == 0) {
        match("*", tokens, index);
        F(tokens, index);
    }
    return 1; // Return success
}

int F(char *tokens[], int *index) {
    // printf("F: Current token: %s\n", tokens[*index]);
    if (strcmp(tokens[*index], "num") == 0) {
        match("num", tokens, index);
    } else if (strcmp(tokens[*index], "(") == 0) {
        match("(", tokens, index);
        E(tokens, index);
        match(")", tokens, index);
    } else {
        printf("Syntax Error\n");
        exit(1);
    }
    return 1; // Return success
}

void match(char *expectedToken, char *tokens[], int *index) {
    if (strcmp(tokens[*index], expectedToken) == 0) {
        // printf("Matched: %s\n", expectedToken);
        (*index)++;
    } else {
        printf("Syntax Error: Expected %s but found %s\n", expectedToken, tokens[*index]);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <expression>\n", argv[0]);
        return 1;
    }

    char *input = argv[1];
    char *tokens[100];
    int numTokens = 0;

    tokenize(input, tokens, &numTokens);

    // for (int i = 0; i < numTokens; i++) {
    //     printf("Token[%d]: %s\n", i, tokens[i]);
    // }

    int index = 0;
    E(tokens, &index);

    if (strcmp(tokens[index], "EOF") == 0) {
        printf("Parsing successful\n");
    } else {
        printf("Syntax Error\n");
    }

    for (int i = 0; i < numTokens; i++) {
        free(tokens[i]); // Free allocated memory
    }

    return 0;
}
