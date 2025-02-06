#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"

#define EOL     256
#define OP      257
#define NUM     258

int tokenize(char *input, char *tokens[], int *numTokens)
{
    int i = 0;
    *numTokens = 0;

    while (input[i] != '\0')
    {
        if (isdigit(input[i]) || (input[i] == '.' && i > 0 && isdigit(input[i - 1])))
        {
            char buff[32];
            int j = 0;

            while (isdigit(input[i]) || (input[i] == '.' && isdigit(input[i + 1])))
            {
                buff[j++] = input[i++];
            }
            buff[j] = '\0';
            tokens[*numTokens] = malloc(strlen(buff) + 7); /* [num: ]\0 */
            sprintf(tokens[*numTokens], "num");
            (*numTokens)++;
        }
        else if (strchr("+-*/()^", input[i]))
        {
            tokens[*numTokens] = malloc(8); 
            sprintf(tokens[*numTokens], "%c", input[i]);
            (*numTokens)++;
            i++;
        }
        else if (input[i] == ' ')
        {
            // Skip spaces
            i++;
        }
        else
        {
            // Handle invalid characters
            fprintf(stderr, "Invalid character: %c\n", input[i]);
            i++;
        }
    }
    // Add EOF token
    tokens[*numTokens] = malloc(5);
    strcpy(tokens[*numTokens], "EOF");
    (*numTokens)++;
    return 1; 
}

int get_token()
{
    int c, value;
    while(1)
    {
        switch(c = fgetc(fp))
        {
            case '+':
            case '*':
            case '-':
            case '/':
                fprintf("OP:%c", c);
                return OP;
            case ' ':
            case '\t':
                continue; 
            case '(':
            case ')':
                fprintf(stderr, "[%c]", c);
            default:
                if (isdigit(c))
                {
                    value = 0;
                    do
                    {
                        value = 10*value + (c - '0');
                    }
                    while(isdigit(c=fgetc(fp)));
                    ungetc(c,fp);
                    fprintf(stderr, "[NUM: %d]", value);
                    return NUM;
                }
                else if (c == '\n' || c == EOF)
                {
                    return EOL;
                }
                else 
                {
                    fprintf(stderr, "[Error: %c]", c);
                    exit(1);
                }
        }
    }
}

void test(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <expression>\n", argv[0]);
        return;
    }

    char *input = argv[1];
    char *tokens[100];
    int numTokens = 0;

    tokenize(input, tokens, &numTokens);

    for (int i = 0; i < numTokens; i++)
    {
        printf("%s", tokens[i]);
        free(tokens[i]); 
    }
    printf("\n");
}