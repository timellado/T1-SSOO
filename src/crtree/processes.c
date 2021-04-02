#include "processes.h"
#include <stdlib.h>

void worker_process(Worker* worker) {
    
}

Worker* new_worker(char* pid, char* executable, char* args, char* args_len) {
    Worker* new_worker = malloc(sizeof(Worker));
    new_worker->pid = pid;
    new_worker->executable = executable;
    new_worker->args = args;
    new_worker->args_len = args_len;
    return new_worker;
}