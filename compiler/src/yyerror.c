
extern int yyerror(const char *s) {
    static int last_error_line = -1;
    
    if (last_error_line == yylineno) return 0; 
    last_error_line = yylineno;
    fseek(yyin, 0, SEEK_SET); /* Reset file pointer to the beginning */
    char buffer[1024];  // Buffer to store the line
    int current_line = 1;

    while (fgets(buffer, sizeof(buffer), yyin) && current_line < yylineno) {
        current_line++;
    }

    buffer[strcspn(buffer, "\n")] = 0;

    int width = snprintf(NULL, 0, "%d", yylineno);

    // Manually find the column position of yytext
    char *pos = strstr(buffer, yytext);
    int error_col = (pos) ? (pos - buffer) + 1 : 1; // Default to 1 if not found
    int error_len = strlen(yytext);  /* bad token length */
    fprintf(stderr, "%s:%d: \033[1;31merror:\033[0m %s: '%s'\n", 
            yyfilename ? yyfilename : "input", yylineno, s, yytext);
    fprintf(stderr, "%*d | %s\n", width, yylineno, buffer);  
    fprintf(stderr, "%*s |\033[0;32m", width, "");  

    for (int i = 0; i < error_col - 1; i++) fprintf(stderr, " ");
    
    fprintf(stderr, " ^");
    for (int i = 1; i < error_len; i++) fprintf(stderr, "~");
    fprintf(stderr, "\033[0m\n"); 

    return 1;
}

int yyerror(const char *s) {

	// shortcut: 
	fprintf( stderr, "\n\033[1;31merror:\033[0m%s\n", s );
	exit(1);

	// BUGGY: the code below ignores syntax errors reported by the parser!

    /* fprintf(stderr, "Error: %s at line %d, column %d\n", s, yylineno, yycolumn); */
    if (strcmp(s, "syntax error") == 0)
        return 0;

    static int last_error_line = 0;
    static char last_error_msg[256] = "";

    if (last_error_line == yylineno && strcmp(last_error_msg, s) == 0)
        return 0;

    last_error_line = yylineno;
    strncpy(last_error_msg, s, sizeof(last_error_msg));
    last_error_msg[sizeof(last_error_msg) - 1] = '\0';

    char *line_content = get_error_text(yylineno, yyin, yyfilename);
    
    if (!line_content) {
        return 0;
    }
    message_enqueue(&error_warning, ERROR, yyfilename, s, line_content, yylineno, yycolumn);

    free(line_content);  
    error_count++;
    return 0;
}
