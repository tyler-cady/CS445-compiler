#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){
    char *c;
    snprintf(&c, "hello %s", "hello");
    return 0;
}