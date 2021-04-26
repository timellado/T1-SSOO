#include "processes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include "../file_manager/manager.h"

int global_SIGABRT = 0;
int child_count = 0;
int childs_pids[];

void sig_SIGABRT_handler_manager(int signum)
{
    if (!global_SIGABRT)
    {
        global_SIGABRT = 1;
        for (size_t i = 0; i < child_count; i++)
        {
            kill(childs_pids[i], SIGABRT);
        }
    }
}

void sig_handler_worker(int signum)
{
    global_SIGABRT = 1;
}

void worker_process(Worker *worker, Manager **managers, Worker **workers, int total_processes)
{
    int status;
    time_t start, end;
    int pid;
    pid = fork();
    time(&start);
    if (pid == 0)
    {
        execvp(worker->executable, worker->args);
    }
    else
    {
        wait(&status);
        time(&end);
        worker->time = (int)difftime(end, start);
        worker->return_code = WEXITSTATUS(status);
        worker->interrupted = global_SIGABRT;
        line_writer(worker);
    }
    exit(WEXITSTATUS(status));
}

Worker *new_worker(int id, char *executable, int args_len, char **args)
{
    Worker *new_worker = malloc(sizeof(Worker) + (args_len + 2) * sizeof(char *));
    new_worker->id = id;
    new_worker->executable = executable;
    new_worker->args_len = args_len;
    new_worker->args = args;
    return new_worker;
}

void free_worker(Worker *worker)
{
    free(worker->args);
    free(worker);
}

void manager_logic(Manager *manager, Manager **managers, Worker **workers, int total_processes)
{
    int status;
    pid_t pid;
    for (int i = 0; i < manager->children_len; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            if (managers[manager->children_ids[i]] != NULL) // MANAGER CHILD
            {
                Manager *child_manager = managers[manager->children_ids[i]];
                manager_process(child_manager, managers, workers, total_processes);
            }
            else if (workers[manager->children_ids[i]] != NULL) // WORKER CHILD
            {
                Worker *child_worker = workers[manager->children_ids[i]];
                worker_process(child_worker, managers, workers, total_processes);
            }
        }
        else
        {
            if (managers[manager->children_ids[i]] != NULL) // MANAGER CHILD
            {
                Manager *child_manager = managers[manager->children_ids[i]];
                child_manager->pid = pid;
            }
            else if (workers[manager->children_ids[i]] != NULL)// WORKER CHILD
            {
                Worker *child_worker = workers[manager->children_ids[i]];
                child_worker->pid = pid;
            }
        }
    }
    if (pid > 0)
    {
        for (size_t i = 0; i < manager->children_len; i++)
        {
            if (managers[manager->children_ids[i]] != NULL) // MANAGER CHILD
            {
                Manager *child_manager = managers[manager->children_ids[i]];
                childs_pids[child_count] = child_manager->pid;
                child_count++;
            }
            else if (workers[manager->children_ids[i]] != NULL)// WORKER CHILD
            {
                Worker *child_worker = workers[manager->children_ids[i]];
                childs_pids[child_count] = child_worker->pid;
                child_count++;
            }
        }

        int exited_children = 0;
        while (exited_children < child_count)
        {
            pid_t exited_child = wait(&status);
            printf("[%d] CHILD %d EXITED\n", getpid(), exited_child);
            for (int i = 0; i < manager->children_len; i++)
            {
                if ((workers[manager->children_ids[i]] != NULL) && (exited_child == workers[manager->children_ids[i]]->pid))
                {
                    Worker *child_worker = workers[manager->children_ids[i]];
                    char *manager_filename = calloc(20, sizeof(char));
                    sprintf(manager_filename, "%d.txt", manager->id);
                    printf("IM THE MANAGER %d AND MY FILENAME IS %s\n", getpid(), manager_filename);
                    char *child_filename = calloc(20, sizeof(char));
                    sprintf(child_filename, "%d.txt", child_worker->id);
                    printf("IM THE EXITED CHILD %d FROM MANAGER %d AND MY FILENAME IS %s\n", exited_child, getpid(), child_filename);
                    manager_file_writer_worker(child_filename, manager_filename);
                    free(manager_filename);
                    free(child_filename);
                }
                else if ((managers[manager->children_ids[i]] != NULL) && (exited_child == managers[manager->children_ids[i]]->pid))
                {
                    Manager *child_manager = managers[manager->children_ids[i]];
                    char *manager_child_filename = calloc(20, sizeof(char));
                    sprintf(manager_child_filename, "%d.txt", child_manager->id);
                    char *manager_parent_filename = calloc(20, sizeof(char));
                    sprintf(manager_parent_filename, "%d.txt", manager->id);
                    manager_file_writer_manager(manager_child_filename, manager_parent_filename);
                    free(manager_child_filename);
                    free(manager_parent_filename);
                }
            }
            exited_children++;
        }
    }
}

void manager_process(Manager *manager, Manager **managers, Worker **workers, int total_processes)
{
    signal(SIGABRT, sig_SIGABRT_handler_manager);
    signal(SIGINT, SIG_IGN);
    manager_logic(manager, managers, workers, total_processes);
    exit(EXIT_SUCCESS);
}

void root_process(Manager *root, Manager **managers, Worker **workers, int total_processes)
{
    signal(SIGABRT, sig_SIGABRT_handler_manager);
    signal(SIGINT, sig_SIGABRT_handler_manager);
    manager_logic(root, managers, workers, total_processes);
}

Manager *new_manager(int id, char *timeout, char *children_len, char *children)
{
    int children_len_int = atoi(children_len);
    int timeout_int = atoi(timeout);
    Manager *manager = malloc(sizeof(Manager) + children_len_int * sizeof(int));

    manager->id = id;
    manager->timeout = timeout_int;
    manager->children_len = children_len_int;

    for (int i = 0; i < children_len_int; i++)
    {
        int n = children[i] - '0';
        manager->children_ids[i] = n;
    }

    return manager;
}

void free_manager(Manager *manager)
{
    free(manager);
}

void start_processes(Manager *root, Manager **managers, Worker **workers, int total_processes)
{
    root_process(root, managers, workers, total_processes);
}

// void free_all(Manager **managers, Worker **workers, int total_processes)
// {

// }