#pragma once
#include <sys/types.h>
#include "../file_manager/manager.h"
typedef struct worker
{
    int id;
    pid_t pid;
    char *executable;
    int args_len;
    char **args;
    int return_code;
    int interrupted;
    int time;
} Worker;

typedef struct manager
{
    int id;
    pid_t pid;
    int timeout;
    int children_len;
    int children_ids[];
} Manager;

Worker *new_worker(int id, char *executable, int args_len, char **args);

void worker_process(Worker *worker, Manager **managers, Worker **workers, Manager *root, int total_processes, InputFile *input_file);

void free_worker(Worker *worker);

Manager *new_manager(int id, int timeout, int children_len, int *children);

void manager_process(Manager *manager, Manager **managers, Worker **workers, Manager *root, int total_processes, InputFile *input_file);

void manager_logic(Manager *manager, Manager **managers, Worker **workers, Manager *root, int total_processes, InputFile *input_file);

void free_manager(Manager *manager);

void root_process(Manager *root, Manager **managers, Worker **workers, int total_processes, InputFile *input_file);

void start_processes(Manager *root, Manager **managers, Worker **workers, int total_processes, InputFile *input_file);

void free_all(Manager **managers, Worker **workers, Manager *root, int total_processes, InputFile *input_file);

void sig_SIGABRT_handler_manager(int signum);

void sig_handler_worker(int signum);

void line_writer(Worker *worker);

void manager_file_writer_manager(char *manager_child_filename, char *manager_parent_filename);

void manager_file_writer_worker(char *child_filename, char *manager_filename);