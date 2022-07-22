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

typedef struct chunk{
    int pid;    //if pid is -1 it is a free space
    int start;
    int size;
    struct chunk *next;
}chunk;

typedef struct memory{
    int size;
    int chunks;
    int alocated;
    int free;
    chunk *root;
}memory;


int init_memory(memory **mem, int size);
chunk *new_chunk(int pid, int start, int size, chunk *next);
int first_fit(memory *mem, int pid, int size);
int first_fit(memory *mem, int pid, int size);
void pr_status(memory *mem);


int init_memory(memory **mem, int size){
    (*mem) = (memory*)malloc(sizeof(memory));
    (*mem)->size = size;
    (*mem)->chunks = 1;
    (*mem)->alocated = 0;
    (*mem)->free = size;
    (*mem)->root = new_chunk(-1, 0, size, NULL);
    return 0;
}

chunk *new_chunk(int pid, int start, int size, chunk *next){
    chunk *ch = (chunk*)malloc(sizeof(chunk));
    ch->pid = pid;
    ch->start = start;
    ch->size = size;
    ch->next = next;
    return ch;
}

int first_fit(memory *mem, int pid, int size){
    //returns 1 if successfully 0 if fail
    bool success = false;
    if (mem->free > size){
        if (mem->root->pid == -1 && mem->root->size >= size){
            printf("9\n");
            chunk *add = new_chunk(pid, 0, size, mem->root);
            mem->root->start += size;
            mem->root->size -= size;
            mem->root = add;
            success = true;
        }else{
            chunk *pre = mem->root;
            chunk *cur = pre->next;

            while (cur != NULL && !success){
                if (cur->pid == -1 && cur->size >= size){
                    chunk *add = new_chunk(pid, cur->start, size, cur);
                    pre->next = add;
                    cur->start += size;
                    cur->size -= size;
                    success = true;
                }
                pre = cur;
                cur = cur->next;
            }
        }
    }
    if (success){
        mem->alocated += size;
        mem->free -= size;
        mem->chunks++;
    }

    return success;
}

void pr_status(memory *mem){
    chunk *cur = mem->root;

    printf("Partitions [Alocated memory = %d]\n", mem->alocated);
    while (cur != NULL){
        if (cur->pid != -1){
            printf("Address [%d:%d] Process P%d\n", cur->start, cur->start+cur->size-1, cur->pid);
        }
        cur = cur->next;
    }
    printf("\n");
    printf("Holes [Free memory = %d ]:\n", mem->free);
    cur = mem->root;
    while (cur != NULL){
        if (cur->pid == -1){
            printf("Address [%d:%d] len = %d\n", cur->start, cur->start+cur->size-1, cur->size);
        }
        cur = cur->next;
    }

}




int main(int argv, char *arg[]){
    if (argv != 2){
        printf("incorrect number of arguments: %d given, 2 are required\n", argv);
        exit(0);
    }
    bool end = false;

    memory *mem;
    init_memory(&mem, atoi(arg[1]));

    
    //command input vars
    int inpv;
    char *inp[MAX_ARGS];
    char inp_buf[100];
    char temp;
    int i;
    int j;

    int pid;
    int size;
    char alg;

    for (i=0; i<MAX_ARGS; i++){
        inp[i] = malloc(sizeof(char) * 100);
    }


    printf("Allocated %d bytes of memory\n", mem->size);

    pr_status(mem);
    first_fit(mem, 1, 100);
    pr_status(mem);


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
                inp[inpv][j] = tolower(inp_buf[i]);
                j++;
            }
            i++;
        }
        if (j != 0){
            inp[inpv][j] = '\0';
            inpv++;
        }        
        

        if (strcmp("rq", inp[0]) == 0){
            pid = atoi(&inp[1][1]);
            size = atoi(inp[2]);
            alg = inp[3][0];
            
            printf("%d\n", size);
        }else if(strcmp("rl", inp[0]) == 0){
            pid = atoi(&inp[1][1]);

        }else if(strcmp("c", inp[0]) == 0){

        }else if(strcmp("status", inp[0]) == 0){

        }else if(strcmp("exit", inp[0]) == 0){
            end = true;
        }else{
            printf("invalid command\n");
        }
    }

    
    return 0;
}