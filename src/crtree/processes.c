#include "processes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void worker_process(Worker *worker, Manager **managers, Worker **workers)
{
    printf("WORKER: ME HABRÍA EJECUTADO ID: %d\n", *worker->pid);
}

Worker *new_worker(int *pid, char *executable, char *args_len, char *args)
{
    Worker *new_worker = malloc(sizeof(Worker));
    new_worker->pid = malloc(sizeof(int));
    memcpy(new_worker->pid, pid, sizeof(int));
    new_worker->executable = executable;
    new_worker->args_len = args_len;
    new_worker->args = args;
    return new_worker;
}

void free_worker(Worker *worker)
{
    free(worker->pid);
    free(worker);
}

void manager_process(Manager *manager, Manager **managers, Worker **workers)
{
    int status;
    for (int i = 0; i < manager->children_len; i++)
    {   
        printf("MAN_PROC: FIRST CHILD ID: %d\n", manager->children_ids[i]);
        if (managers[manager->children_ids[i]] != NULL) // check if manager or worker
        {
            // Manager *child_manager = managers[*manager->children_ids[i]];
            pid_t child_pid = fork();
            if (child_pid == 0)
            {
                printf("CHILD: IM THE CHILD MANAGER\n");
                sleep(3);
                exit(9);
                // manager_process(child_manager, managers, workers);
            }
            else
            {
                printf("PARENT: IM THE PARENT\n");
                pid_t exited_child = wait(&status);
                printf("PARENT: The children %d finished executing with code %d\n", exited_child, WEXITSTATUS(status));
            }
        }
        else
        {
            Worker *child_worker = workers[manager->children_ids[i]];
            worker_process(child_worker, managers, workers);
        }
    }

    // 0: W, 1:M, 2:W, 3:R, 4:W, 5:W etc

    // int status;
    // pid_t child_pid = fork();
    // if (child_pid == 0)
    // {
    //     printf("CHILD: IM THE CHILD\n");
    //     sleep(3);
    //     exit(9);
    // }
    // else
    // {
    //     printf("PARENT: IM THE PARENT\n");
    //     pid_t exited_child = wait(&status);
    //     printf("PARENT: The children %d finished executing with code %d\n", exited_child, WEXITSTATUS(status));
    // }
}

Manager *new_manager(int *pid, char *timeout, char *children_len, char *children)
{
    Manager *manager = malloc(sizeof(Manager));

    manager->pid = malloc(sizeof(int));
    memcpy(manager->pid, pid, sizeof(int));

    int timeout_int = atoi(timeout);
    manager->timeout = timeout_int;

    int children_len_int = atoi(children_len);
    manager->children_len = children_len_int;

    printf("MANAGER CHILDREN NUMBER: %d\n", manager->children_len);

    for (int i = 0; i < children_len_int; i++)
    {
        int n = children[i] - '0';
        manager->children_ids[i] = n;
        printf("MANAGER CHILD ID: %d\n", manager->children_ids[i]);
    }

    return manager;
}

void free_manager(Manager *manager)
{
    free(manager->pid);
    free(manager);
}

void start_processes(Manager *root, Manager **managers, Worker **workers)
{
    // manager_process(root, managers, workers);
}