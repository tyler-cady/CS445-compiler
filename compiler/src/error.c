#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "ctype.h"
void error_fatal_malloc(void) {
    fprintf(stderr, " \033[1;31mfatal error:\033[0m memory allocation failed\n");
    exit(1);
}

void message_enqueue(message_context_t *ctx, int is_warning, const char *filename, const char *error_msg, const char *line_text, int line_no, int column){
    message_queue_t *queue;

    if (is_warning == ERROR || is_warning == ERROR_NOTE) {
        queue = &ctx->errors;
    } else if (is_warning == WARNING) {
        queue = &ctx->warnings;
    } else {
        fprintf(stderr, " \033[1;31mfatal error:\033[0m invalid message value\n");
        return;
    }
    message_t *check = queue->head;
    while (check) {
        if (check->line_no == line_no && strcmp(check->error_msg, error_msg) == 0) {
            return;
        }
        check = check->next;
    }

    if (is_warning == ERROR && queue->count >= MAX_ERRORS) {
        print_all_messages(ctx);
        fprintf(stderr, "\033[1;31mfatal error:\033[0m too many errors emitted, stopping now [-ferror-limit=]\n");
        fprintf(stderr, "%d errors generated.\n", queue->count);
        exit(1);
    }

    message_t *new_node = malloc(sizeof(message_t));
    if (!new_node) error_fatal_malloc();

    new_node->line_no = line_no;
    new_node->column = column;
    new_node->type = is_warning;
    new_node->line_text = strdup(line_text);
    new_node->error_msg = strdup(error_msg);
    new_node->filename = strdup(filename);
    new_node->next = NULL;

    if (!new_node->line_text || !new_node->error_msg || !new_node->filename)
        error_fatal_malloc();

    if (queue->tail)
        queue->tail->next = new_node;
    else
        queue->head = new_node;

    queue->tail = new_node;
    queue->count++;
}


void error_print(message_queue_t *queue) {
    message_t *current = queue->head;
    while (current) {
        if (current->type == ERROR) {
            int width = snprintf(NULL, 0, "%d", current->line_no);
            // char *pos = strstr(current->line_text, current->error_msg);
            // int error_col = current->column;
            // int error_len = strlen(current->error_msg);
            fprintf(stderr, "%s:%d: \033[1;31merror:\033[0m %s\n",current->filename, current->line_no, current->error_msg);
            fprintf(stderr, "%*d | %s\n", width, current->line_no, current->line_text);
            fprintf(stderr, "%*s |\n", width, "");
            // for (int i = 0; i < error_col - 1; i++) fprintf(stderr, " ");
            // fprintf(stderr, "^");
            // fprintf(stderr, "\033[0m\n");
        } else if (current->type == ERROR_NOTE) {
            note_print(current);
            break;  
        }
        current = current->next;
    }
}

void note_print(message_t *note) {
    // This function handles printing only one note at a time
    int width = snprintf(NULL, 0, "%d", note->line_no);
    fprintf(stderr, "%s:%d: \033[38;5;8mnote:\033[0m %s\n",
            note->filename, note->line_no, note->error_msg);
    fprintf(stderr, "%*d | %s\n", width, note->line_no, note->line_text);
}

void warning_print(message_queue_t *queue) {
    message_t *current = queue->head;
    while (current) {
        int width = snprintf(NULL, 0, "%d", current->line_no);
        // char *pos = strstr(current->line_text, current->error_msg);
        int error_col = current->column-1;
        // int error_len = strlen(current->error_msg);
        fprintf(stderr, "%s:%d: \033[1;35mwarning:\033[0m %s\n",
            current->filename, current->line_no, current->error_msg);
        fprintf(stderr, "%*d | %s\n", width, current->line_no, current->line_text);
        fprintf(stderr, "%*s |\033[0;32m", width, "");
        for (int i = 0; i < error_col - 1; i++) fprintf(stderr, " ");
        fprintf(stderr, "^");
        // for (int i = 1; i < error_len; i++) fprintf(stderr, "~");
        fprintf(stderr, "\033[0m\n");
        current = current->next;
    }
}

void message_free(message_queue_t *queue) {
    message_t *current = queue->head;
    while (current) {
        message_t *next = current->next;
        free(current->line_text);
        free(current->error_msg);
        free(current->filename);
        free(current);
        current = next;
    }
    queue->head = queue->tail = NULL;
    queue->count = 0;
}

char *get_error_text(int lineno , FILE *file, char *filename) {
    (void)filename; // Not used, but kept for future compatibility
    fseek(file, 0, SEEK_SET);
    char buffer[1024];
    int current_line = 1;
    while (fgets(buffer, sizeof(buffer), file) && current_line < lineno) {
        current_line++;
    }
    buffer[strcspn(buffer, "\n")] = 0;
    return strdup(buffer);
}

void print_all_messages(message_context_t *ctx) {
    warning_print(&ctx->warnings);
    error_print(&ctx->errors);
}
void message_free_all(message_context_t *ctx) {
    message_free(&ctx->errors);
    message_free(&ctx->warnings);
}

int are_there_messages(message_context_t *ctx) {
    return (ctx->errors.count > 0 || ctx->warnings.count > 0);
}

// int main(void) {
//     message_context_t ctx = {0}; // cleaner

//     message_enqueue(&ctx, 0, "main.c", "undeclared variable", "    int x = y;", 42);
//     message_enqueue(&ctx, 2, "main.c", "unused function", "    void foo() {}", 43);
//     message_enqueue(&ctx, 0, "main.c", "syntax error", "    if (x = 5)", 43);
//     message_enqueue(&ctx, 1, "main.c", "unused variable", "    int z;", 44);
//     message_enqueue(&ctx, 1, "main.c", "deprecated function", "    old_function();", 45);
//     message_enqueue(&ctx, 0, "main.c", "memory leak", "    malloc(10);", 46);

//     print_all_messages(&ctx);
//     return 0;
// }



