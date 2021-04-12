#pragma once

typedef struct worker
{
    int pid;
    char *executable;
    char *args_len;
    char *args;
    char *return_code;
    char *interrupted;
    char *time;
} Worker;

typedef struct manager
{
    int pid;
    int timeout;
    int children_len;
    int children_ids[];
} Manager;

void worker_process(Worker *worker, Manager **managers, Worker **workers);

Worker *new_worker(int pid, char *executable, char *args_len, char *args);

void free_worker(Worker *worker);

void manager_process(Manager *manager, Manager **managers, Worker **workers);

Manager *new_manager(int pid, char *timeout, char *children_len, char *children);

void free_manager(Manager *manager);

void start_processes(Manager *root, Manager **managers, Worker **workers);

void sig_handler_manager_parent(int signum);

void sig_handler_manager_child(int signum);

void sig_handler_worker(int signum);