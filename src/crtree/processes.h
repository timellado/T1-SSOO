#pragma once

typedef struct worker {
    int* pid;
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

Worker* new_worker(int* pid, char* executable, char* args_len, char* args);

void free_worker(Worker* worker);
