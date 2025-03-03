#include "calc.h"

void skip_endl()
{
    while(curr_token == ENDL)
        curr_token = get_token();
}

void match(int expected)
{
    skip_endl();   
    if (curr_token == expected)
        curr_token = get_token();
    else
    {
        fprintf(stderr, "[MATCH Error: (E: %c A: %c]", expected, curr_token);
        exit(1);
    }
}

/* program -> PROGRAM ID ';' declarations compound_statement '.' */
void program()
{
    match(PROGRAM);
    match(ID);
    match(';');
    decls();
    cmpd_stmt();
    match('.');
}

/* declarations -> declarations VAR identifier_list ':' type ';' */
void decls()
{
    if (curr_token == VAR)
    {
        match(VAR);
        id_list();
        match(':');
        type();
        match(';');
    }
}

/* identifier_list -> ID | identifier_list ',' ID */
void id_list()
{
    match(ID);
    while (curr_token == ',')
    {
        match(',');
        match(ID);
    }
}

/* type -> standard_type */
/* standard_type -> INTEGER */
void type()
{
    if (curr_token == INTEGER)
        match(INTEGER);
    else
    {
        fprintf(stderr, "Error: Unsupported type [%c]", curr_token);
        exit(1);
    }
}

/* compound_statement -> BEGIN optional_statements END */
void cmpd_stmt()
{
    match(BEGIN);
    opt_stmts();
    match(END);
}

/* optional_statements -> statement_list | empty */
void opt_stmts()
{
    if (curr_token == ID)
        stmt();
}

/* statement -> variable ASSIGNOP expression */
void stmt()
{
    if (curr_token == ID)
    {
        match(ID);
        match(ASSOP);
        E();
        match(';');
    }
    else
    {
        fprintf(stderr, "Error: Invalid Statement\n");
        exit(1);
    }
}

/* Expression parsing */
int E() /* Handles addition */
{
    int val = T();
    while (curr_token == '+')
    {
        match('+');
        val += T();
    }
    return val;
}

int T() /* Handles multiplication */
{
    int val = F();
    while (curr_token == '*')
    {
        match('*');
        val *= F();
    }
    return val;
}

int F() /* Handles parentheses and numbers */
{
    int val;
    if (curr_token == '(')
    {
        match('(');
        val = E();
        match(')');
        return val;
    }
    else if (curr_token == NUM)
    {
        val = curr_attr;
        match(NUM);
        return val;
    }
    else
    {
        fprintf(stderr, "F Error: Bad token [%c]\n", curr_token);
        exit(1);
    }
}
int get_token()
{
    int c, value;
    int i = 0;
    char buff[32];
    while (1)
    {
        c = getc(fp);

        if (c == EOF)
        {
            fprintf(stderr, "[EOL]");
            return EOL;
        }
        if (isspace(c) || c == '\t')
            continue;

        if (c == '\n')
            return ENDL;

        if (isalpha(c))
        {
            buff[i++] = c;
            while (isalnum(c = getc(fp)))
                buff[i++] = c;
            buff[i] = '\0';
            ungetc(c, fp);

            if (strcmp(buff, "PROGRAM") == 0)
                return PROGRAM;
            if (strcmp(buff, "VAR") == 0)
                return VAR;
            if (strcmp(buff, "BEGIN") == 0)
                return BEGIN;
            if (strcmp(buff, "END") == 0)
                return END;
            if (strcmp(buff, "INTEGER") == 0)
                return INTEGER;
            fprintf(stderr, "[%s]", buff);
            return ID;
        }

        if (isdigit(c))
        {
            value = 0;
            do
            {
                value = 10 * value + (c - '0');
            } while (isdigit(c = fgetc(fp)));
            ungetc(c, fp);
            curr_attr = value;
            fprintf(stderr, "[NUM:%d]", curr_attr);
            return NUM;
        }
        switch (c)
        {
        case '*':
        case '+':
        case '(':
        case ')':
            fprintf(stderr, "[%c]", c);
            return c;
        case ';':
        case ',':
            return c;
        case '.':
            return c;
            break;
        case ':':
            if ((c = getc(fp)) == '=')
            {
                fprintf(stderr, "[OP::=]");
                return ASSOP;
            }
            else
                ungetc(c, fp);
            return ':';
        default:
            fprintf(stderr, "[P ERROR:%c]", c);
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    assert(argc >= 2);
    fp = fopen(argv[1], "r");

    curr_token = get_token();
    program();

    if (curr_token == EOL)
        fprintf(stderr, "\nParsing complete.\n");

    fclose(fp);
    return 0;
}
