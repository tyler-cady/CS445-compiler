#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#define MAX_ERRORS 20

#define ERROR 0
#define WARNING 1
#define ERROR_NOTE 2

typedef struct message_s {
    char *filename;
    int line_no;
    int column;
    char *line_text;
    char *error_msg;
    int type;

    struct message_s *next;
} message_t;

typedef struct {
    message_t *head;
    message_t *tail;
    int count;
} message_queue_t;

typedef struct message_context_s{
    message_queue_t errors;
    message_queue_t warnings;
} message_context_t;

void message_enqueue(message_context_t *ctx, int is_warning, const char *filename, const char *error_msg, const char *line_text, int line_no, int column);
void error_print(message_queue_t *queue);
void warning_print(message_queue_t *queue);
void note_print(message_t *queue);
void message_free(message_queue_t *queue);
void error_fatal_malloc(void);
char *get_error_text(int lineno, FILE *file, char *filename);
void print_all_messages(message_context_t *ctx);
int are_there_messages(message_context_t *ctx);

void message_free_all(message_context_t *ctx);


#endif // ERROR_H
