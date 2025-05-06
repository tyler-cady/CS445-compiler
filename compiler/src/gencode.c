#include "gencode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.tab.h"
#include "reg.h"
#include "util.h"

#include <regex.h>

/* x86 32 bit asm */
extern FILE *out;
// extern reg_t *rstack;
#define LOCALS 8
static int label_id = 0;
reg_t *rstack;
char *label()
{
    char *label = malloc(32);
    snprintf(label, 32, ".L%d", label_id++);
    return label;
}

void gfile_header(char *fname)
{
    fprintf(out, "# Tyler's Dragon Assembly \n");
    fprintf(out, "\t.file\t\"%s\"\n", fname);
    fprintf(out, "\t.text\n");
}

void gfile_footer()
{
    fprintf(out, "\t.ident\t\"GCC: (GNU) 11.5.0 20240719 (Red Hat 11.5.0-5)\"\n");
    fprintf(out, "\t.section\t.note.GNU-stack,\"\",@progbits\n");
}

void gmain_header()
{
    fprintf(out, "\t.globl\tmain\n");
    fprintf(out, "\t.type\tmain, @function\n");
    fprintf(out, "main:\n");
    fprintf(out, "%s:\n", label());
    fprintf(out, "\tpushl\t%%ebp\n");
    fprintf(out, "\tmovl\t%%esp, %%ebp\n");
}
void gmain_footer()
{
    fprintf(out, "\tmovl\t$0, %%eax\n");
    fprintf(out, "\tpopl\t%%ebp\n");
    fprintf(out, "\tret\n");
    fprintf(out, "%s:\n", label());
    fprintf(out, "\t.size\tmain, .-main\n");
}

void gread_write()
{
    if (hash_search(symbol_tbl, "write") || hash_search(symbol_tbl, "read"))
    {
        fprintf(out, "\t.section\t.rodata\n");
        char *wlabel = label();
        symbol_tbl->label = wlabel;
        fprintf(out, "%s:\n", wlabel);
        fprintf(out, "\t.string \"%%d\\n\"\n");
        fprintf(out, "\t.text\n");
    }
}
void gprintf_call(int *regs, int num_regs)
{
    int cleanup_size = (num_regs + 1) * 4;
    for (int i = num_regs - 1; i >= 0; --i)
    {
        char *regname = get_reg_name(regs[i]);
        fprintf(out, "\tpushl\t%%%s\n", regname);
        rfree(rstack, regs[i]);
    }
    fprintf(out, "\tpushl\t$%s\n", symbol_tbl->label);
    fprintf(out, "\tcall\tprintf\n");
    fprintf(out, "\taddl\t$%d, %%esp\n", cleanup_size);
}

void gscanf_call(int *regs, int num_regs)
{
    // Push arguments for scanf in reverse order
    for (int i = num_regs - 1; i >= 0; --i)
    {
        char *regname = get_reg_name(regs[i]);
        fprintf(out, "\tpushl\t%%%s\n", regname);
        rfree(rstack, regs[i]);
    }
    fprintf(out, "\tpushl\t$%s\n", symbol_tbl->label);
    fprintf(out, "\tcall\tscanf\n");
    int cleanup_size = (num_regs + 1) * 4;
    fprintf(out, "\taddl\t$%d, %%esp\n", cleanup_size);
}

char *replace_extension(char *filename)
{
    regex_t regex;
    regmatch_t pmatch[2];
    const char *pattern = "^(.*)\\.[^.]*$";
    char *result = NULL;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
    {
        return strdup(filename);
    }

    if (regexec(&regex, filename, 2, pmatch, 0) == 0)
    {
        int len = pmatch[1].rm_eo - pmatch[1].rm_so;
        result = malloc(len + 3);
        if (result)
        {
            strncpy(result, filename + pmatch[1].rm_so, len);
            result[len] = '\0';
            strcat(result, ".s");
        }
    }
    else
    {
        result = strdup(filename);
    }

    regfree(&regex);
    return result;
}


void scan(int temp_id)
{
    int addr_reg = ralloc(rstack, temp_id); // Get register for address
    fprintf(out, "\tleal\t-%d(%%ebp), %%%s\n", (temp_id + 1) * 4, get_reg_name(addr_reg));

    int scanf_regs[1] = {addr_reg};
    gscanf_call(scanf_regs, 1); // Frees addr_reg internally
}

void print(int temp_id)
{
    int val_reg = ralloc(rstack, temp_id); // Get register to load value
    fprintf(out, "\tmovl\t-%d(%%ebp), %%%s\n", (temp_id + 1) * 4, get_reg_name(val_reg));

    int printf_regs[1] = {val_reg};
    gprintf_call(printf_regs, 1); // Frees val_reg internally
}

char *x86op(int opval)
{
    switch (opval)
    {
    case PLUS:
        return "addl";
    case MINUS:
        return "sub";
    case STAR:
        return "imul";
    case SLASH:
        return "idivl";
    default:
        return "invalid";
    }
}

int gencode(tree_t *n) {
    int R, R2, T;

    /* Case 0: leaf node — grab a register */
    if (!n->left && !n->right) {
        R = pop(rstack);
        if (n->type == ID) {
            fprintf(out, "\tmovl\t%s, %%%s\n",
                    n->attr.name_ptr->name,
                    get_reg_name(R));
        } else {
            fprintf(out, "\tmovl\t$%d, %%%s\n",
                    n->attr.ival,
                    get_reg_name(R));
        }
        return R;
    }

    /* Case 1 */
    if ((n->type == MULOP || n->type == ADDOP || n->type == RELOP) && n->left && n->right && (n->right->type == ID || n->right->type == INTEGER))
    {
        R = gencode(n->left);
        if (n->right->type == ID) {
            fprintf(out, "\t%s\t%s, %%%s\n",x86op(n->attr.opval),n->right->attr.name_ptr->name,get_reg_name(R));
        } else {
            fprintf(out, "\t%s\t$%d, %%%s\n", x86op(n->attr.opval), n->right->attr.ival, get_reg_name(R));
        }
        return R;
    }

    /* Case 2 */
    if (n->left->label < n->right->label && n->right->label < REG_COUNT)
    {
        R  = gencode(n->left);
        R2 = gencode(n->right);
        fprintf(out, "\t%s\t%%%s, %%%s\n", x86op(n->attr.opval), get_reg_name(R2), get_reg_name(R));
        // free scratch R2
        push(R2, rstack);
        return R;
    }

    /* Case 3 */
    if (n->right->label <= n->left->label && n->right->label < REG_COUNT)
    {
        R  = gencode(n->left);
        R2 = gencode(n->right);
        fprintf(out, "\t%s\t%%%s, %%%s\n", x86op(n->attr.opval), get_reg_name(R), get_reg_name(R2));
        rfree(rstack, R);
        return R2;
    }
    /* Case 4 */
    R2 = gencode(n->right);
    T  = talloc(rstack);
    fprintf(out, "\tmovl\t%%%s, T%d\n",get_reg_name(R2), T);

    R = gencode(n->left);
    fprintf(out, "\t%s\tT%d, %%%s\n",x86op(n->attr.opval),T,get_reg_name(R));
    tfree(rstack, T);
    return R;
}

int main()
{
   out = fopen("test.s", "w");
   if (!out)
   {
       perror("fopen");
       exit(1);
   }
   rstack = malloc(sizeof *rstack);
   reg_init(rstack);

   symbol_tbl = hash_push(symbol_tbl);
   hash_insert(symbol_tbl, "read");
   hash_insert(symbol_tbl, "write");
   list_t *l = hash_insert(symbol_tbl, "A");
   tree_t *n = tmake_addop(STAR, tmake_inum(5), tmake_inum(4));
   gfile_header("test.s");
   gread_write();
   gmain_header();
   int result = gencode(n);
   int temp_id = talloc(rstack);
   fprintf( out, "\tmovl\t%%%s, -%d(%%ebp)\n", get_reg_name(result), 4*(temp_id + 1));
   print(temp_id); 
   gmain_footer();
   gfile_footer();

   fprintf(stderr, "written to file\n");
   fclose(out);
   free(rstack);
   return 0;
}
