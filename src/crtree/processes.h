#pragma once

typedef struct worker {
    char* pid;
    char* executable;
    char* args_len;
    char* args;
    char* return_code;
    char* interrupted;
    char* time;
} Worker;

typedef struct manager {
    int* pid;
    char* children_ids;
} Manager;

typedef struct root_manager {
    int* pid;
    char* children_ids;
} RootManager;

void worker_process(Worker* worker);
Worker* new_worker(char* pid, char* executable, char* args, char* args_len);
