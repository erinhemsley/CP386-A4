#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
int best_fit(memory *mem, int pid, int size);
int worst_fit(memory *mem, int pid, int size);
int compact(memory *mem);
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

int best_fit(memory *mem, int pid, int size){
    //returns 1 if successfully 0 if fail
    bool success = false;
    if (mem->free > size){
        if (mem->root->pid == -1 && mem->root->size >= size && mem->chunks == 1){
            chunk *add = new_chunk(pid, 0, size, mem->root);
            mem->root->start += size;
            mem->root->size -= size;
            mem->root = add;
            success = true;
        }else{
            chunk *pre = mem->root;
            chunk *cur = pre->next;
            chunk *best = NULL;
            bool found_first = false;

            while (cur != NULL){
                if (cur->pid == -1 && cur->size >= size){
                    if (found_first && cur->size < best->next->size){
                        best = pre;
                    }else{
                        best = pre;
                        found_first = true;
                    }
                    
                }
                pre = cur;
                cur = cur->next;
            }
            if (found_first){
                chunk *add = new_chunk(pid, best->next->start, size, best->next);
                best->next = add;
                add->next->start += size;
                add->next->size -= size;
                success = true;
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

int worst_fit(memory *mem, int pid, int size){
    //returns 1 if successfully 0 if fail
    bool success = false;
    if (mem->free > size){
        if (mem->root->pid == -1 && mem->root->size >= size && mem->chunks == 1){
            chunk *add = new_chunk(pid, 0, size, mem->root);
            mem->root->start += size;
            mem->root->size -= size;
            mem->root = add;
            success = true;
        }else{
            chunk *pre = mem->root;
            chunk *cur = pre->next;
            chunk *worst = NULL;
            bool found_first = false;

            while (cur != NULL){
                if (cur->pid == -1 && cur->size >= size){
                    if (found_first && cur->size > worst->next->size){
                        worst = pre;
                    }else{
                        worst = pre;
                        found_first = true;
                    }
                    
                }
                pre = cur;
                cur = cur->next;
            }
            if (found_first){
                chunk *add = new_chunk(pid, worst->next->start, size, worst->next);
                worst->next = add;
                add->next->start += size;
                add->next->size -= size;
                success = true;
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

int release(memory *mem, int pid){
    bool success = false;

    chunk *remove;
    chunk *prev_remove;
    
    if (mem->root->pid == pid){
        remove = mem->root;
        if (remove->next->pid == -1){
            //combine with folowing free chunk
            remove->next->start = remove->start;
            remove->next->size += remove->start;
            mem->root = mem->root->next;

            mem->chunks -= 1;
            mem->alocated -= remove->size;
            mem->free += remove->size;

            free(remove);
        }else{
            //make chunk free;
            remove->pid = -1;
            mem->alocated -= remove->size;
            mem->free += remove->size;
        }
        success = true;
    }else{
        bool found = false;
        prev_remove = mem->root;
        remove = prev_remove->next;
        while (remove != NULL && !found){
            if (remove->pid == pid){
                found = true;
            }else{
                prev_remove = remove;
                remove = remove->next;
            }
        }
        if (found){
            mem->alocated -= remove->size;
            mem->free += remove->size;
            if (prev_remove->pid != -1 && remove->next->pid != -1){
                //convert to free
                remove->pid = -1;
            }else{
                if(remove->next->pid == -1){
                    //combine with next
                    remove->next->start = remove->start;
                    remove->next->size += remove->start;
                    prev_remove->next = remove->next;
                    
                    mem->chunks -= 1;
                    free(remove);
                    remove = prev_remove->next;
                }
                if (prev_remove->pid == -1){
                    //combine with prev
                    prev_remove->next = remove->next;
                    prev_remove->size += remove->size;

                    mem->chunks -= 1;
                    free(remove);
                }     
            }
            success = true;
        }
    }

    return success;
}

int compact(memory *mem){
    if (mem->chunks > 1){
        chunk *cur = mem->root;
        chunk *start = NULL;
        chunk *end;
        chunk *to_free;
        bool added_first = false;

        mem->chunks = 1;

        while(cur != NULL){
            if (cur->pid != -1){
                mem->chunks++;
                if (!added_first){
                    start = cur;
                    end = cur;
                    start->start = 0;
                    added_first = true;
                }else{
                    cur->start = end->start + end->size;
                    end->next = cur;
                    end = end->next;
                }
                cur = cur->next;
            }else{
                to_free = cur;
                cur = cur->next;
                free(to_free);
            }        
        }
        //add final empty area
        end->next = new_chunk(-1, end->start+end->size, mem->free, NULL);
        mem->root = start;
    }
    return true;
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
    printf("Holes [Free memory = %d]:\n", mem->free);
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
    bool success;

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

            if (alg == 'f'){
                success = first_fit(mem, pid, size);
            }else if (alg == 'b'){
                success = best_fit(mem, pid, size);
            }else if (alg == 'w'){
                success = worst_fit(mem, pid, size);
            }

            if (success){
                printf("Successfully added %d to P%d\n", size, pid);
            }else{
                printf("No hole of sufficient size\n");
            }
        }else if(strcmp("rl", inp[0]) == 0){
            pid = atoi(&inp[1][1]);

            printf("Releasing memory for process P%d\n", pid);
            if(release(mem, pid)){
                printf("Successfully releaced memory for process P%d\n", pid);
            }else{
                printf("Failed to releace memory for process P%d\n", pid);
            }
        }else if(strcmp("c", inp[0]) == 0){
            if(compact(mem)){
                printf("compact process successfull\n");
            }else{
                printf("compact process not successfull\n");
            }
        }else if(strcmp("status", inp[0]) == 0){
            pr_status(mem);
        }else if(strcmp("exit", inp[0]) == 0){
            end = true;
        }else{
            printf("invalid command '%s'\n", inp_buf);
        }
    }

    
    return 0;
}