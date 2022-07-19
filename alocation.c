#include <stdio.h>
#include <stdlib.h>



int main(int argv, char *arg[]){
    if (argv != 2){
        printf("incorrect number of arguments: %d given, 2 are required\n", argv);
        exit(0);
    }
    printf("hi\n");
    return 0;
}