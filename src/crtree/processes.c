#include "processes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void worker_process(Worker* worker) {
    
}

Worker* new_worker(int* pid, char* executable, char* args_len, char* args) {
    Worker* new_worker = malloc(sizeof(Worker));
    new_worker->pid = malloc(sizeof(int));
    memcpy(new_worker->pid, pid, sizeof(int));
    new_worker->executable = executable;
    new_worker->args_len = args_len;
    new_worker->args = args;
    return new_worker;
}

void free_worker(Worker* worker){
    free(worker->pid);
    free(worker);
}