#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
things to do:
--------------------
memory alocation algs:
-first fit
-best fit
-worst fit


*/
#define MAX_ARGS 10

typedef enum bool{false=0, true=1}bool;

typedef struct command_struct{
    char command[50];
    int pid;
    int size;
    char alg;
}command_struct;


int main(int argv, char *arg[]){
    if (argv != 2){
        printf("incorrect number of arguments: %d given, 2 are required\n", argv);
        exit(0);
    }

    int mem_size = atoi(arg[1]);
    bool end = false; 
    int inpv;
    char *inp[MAX_ARGS];
    char inp_buf[100];
    command_struct command;
    char temp;
    int i;
    int j;

    for (i=0; i<MAX_ARGS; i++){
        inp[i] = malloc(sizeof(char) * 100);
    }


    printf("Allocated %d bytes of memory\n", mem_size);


    while(!end){
        printf("command>");
        scanf("%[^\n]", inp_buf);
        scanf("%c",&temp);

        i=0;
        j=0;
        inpv=0;
        while (inp_buf[i] != '\0'){
            if (j == 0 && inp_buf[i] == ' '){
                j=0;
            }else if (inp_buf[i] == ' '){
                inp[inpv][j] = '\0';
                inpv++;
                j=0;
            }else{
                inp[inpv][j] = inp_buf[i];
                j++;
            }
            i++;
        }
        if (j != 0){
            inp[inpv][j] = '\0';
            inpv++;
        }

        
        for (i=0; i<inpv; i++){
            printf("-%s\n", inp[i]);
        }
        
        

        if (strcmp("end", inp[0]) == 0){
            end = true;
        }else if(strcmp("end", inp[0]) == 0){

        }
    }

    
    return 0;
}