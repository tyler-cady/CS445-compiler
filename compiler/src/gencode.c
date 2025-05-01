#include "gencode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.tab.h"

/* x86 32 bit asm */
extern FILE *out;
extern reg_t *rstack;

void file_header(char *fname){
    fprintf( out, "\t.file\t%s\n", fname );
    fprintf( out, "\t.text\n" );
    fprintf( out, "\t.intel_syntax noprefix\n" );
}
void file_footer(){
    fprintf( out, "\t.size main, .-main\n" );
    fprintf( out, "\t.ident\t\"GCC: (GNU) 11.5.0 20240719 (Red Hat 11.5.0-5)\"\n" );
	fprintf( out, "\n\t.section\t.note.GNU-stack,\"\",@progbits\n" );
}



static char *extension_to_asm(const char *filename) {
    const char *dot = strrchr(filename, '.');
    size_t new_filename_len = (dot ? dot - filename : strlen(filename)) + 5;  
    char *new_filename = (char *)malloc(new_filename_len);
    if (!new_filename) {
        perror("Memory allocation failed");
        return NULL;
    }
    if (dot) strncpy(new_filename, filename, dot - filename);
    else strcpy(new_filename, filename);
    strcat(new_filename, ".asm");
    return new_filename;
}

void gencode(tree_t *node ){
 
}
int main(){
    out = fopen( "output.asm", "w");
    if (!out) {
        perror("Failed to open file");
        return 1;
    }

    file_header("output.c");
    write_main_header();
    return 0;
}