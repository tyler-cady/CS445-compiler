#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"

void tokenize(char *input, char *tokens[], int *numTokens)
{
    int i = 0;
    *numTokens = 0;

    while (input[i] != '\0')
    {
        if (isdigit(input[i]) || (input[i] == '.' && i > 0 && isdigit(input[i - 1])))
        {
            // Handle numbers (including floating-point numbers)
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
            // Handle operators and parentheses
            tokens[*numTokens] = malloc(8); /* [op: ]\0 */
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
        free(tokens[i]); // Free allocated memory
    }
    printf("\n");
}

// int main(int argc, char *argv[])
// {
//     test(argc, argv);
//     return 0;
// }