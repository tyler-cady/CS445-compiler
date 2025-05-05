#include <stdio.h>
#include <stdlib.h>


int main(){
    int *argtypes;

    argtypes = {318, 318, 318};  

    fprintf(stderr, "%d\n", argtypes[0]);
}