#include <stdio.h>
#include <stdlib.h>

/*
things to do:
--------------------
memory alocation algs:
-first fit
-best fit
-worst fit


*/


int main(int argv, char *arg[]){
    if (argv != 2){
        printf("incorrect number of arguments: %d given, 2 are required\n", argv);
        exit(0);
    }
    printf("hi\n");
    return 0;
}