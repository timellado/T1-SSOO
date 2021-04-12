#include "processes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

void sig_handler_manager_parent(int signum)
{
    printf("[MANAGER Parent: %d] : Received a response signal from CHILD\n", getppid());
}

void sig_handler_manager_child(int signum)
{
    printf("[MANAGER Child: %d] : Received a signal from PARENT \n", getppid());
    sleep(1);
    kill(getppid(), SIGUSR1);
}

void sig_handler_worker(int signum)
{
    printf("[WORKER: %d] : Received a signal from PARENT \n", getppid());
    sleep(1);
    kill(getppid(), SIGUSR1);
}

void worker_process(Worker *worker, Manager **managers, Worker **workers)
{
    printf("WORKER: ME HABRÍA EJECUTADO ID: %d\n", worker->pid);
}

Worker *new_worker(int pid, char *executable, char *args_len, char *args)
{
    Worker *new_worker = malloc(sizeof(Worker));
    new_worker->pid = pid;
    new_worker->executable = executable;
    new_worker->args_len = args_len;
    new_worker->args = args;
    return new_worker;
}

void free_worker(Worker *worker)
{
    free(worker);
}

void manager_process(Manager *manager, Manager **managers, Worker **workers)
{
    for (int i = 0; i < manager->children_len; i++)
    {
        int status;
        printf("MAN_PROC: FIRST CHILD ID: %d\n", manager->children_ids[i]);
        if (managers[manager->children_ids[i]] != NULL) // MANAGER CHILD
        {
            Manager *child_manager = managers[manager->children_ids[i]];
            pid_t child_pid = fork();
            if (child_pid == 0) // Manager Child
            {
                signal(SIGUSR1, sig_handler_manager_child);
                printf("[MANAGER ID:%d PID:%d PPID:%d] IM THE CHILD MANAGER\n", child_manager->pid, getpid(), getppid());
                sleep(3);
                exit(9);
                // manager_process(child_manager, managers, workers);
            }
            else  // Manager Parent
            {
                signal(SIGUSR1, sig_handler_manager_parent);
                printf("[MANAGER ID:%d PID:%d] IM THE PARENT of a MANAGER\n", manager->pid, getpid());
                pid_t exited_child = wait(&status);
                printf("[MANAGER ID:%d PID:%d] The children MANAGER %d finished executing with code %d\n", manager->pid, getpid(), exited_child, WEXITSTATUS(status));
            }
        }
        else // WORKER CHILD
        {   
            Worker *child_worker = workers[manager->children_ids[i]];
            pid_t child_pid = fork();
            if (child_pid == 0) //Worker
            {
                signal(SIGUSR1, sig_handler_worker);
                printf("[WORKER ID:%d PID:%d PPID:%d] IM THE WORKER CHILD\n", child_worker->pid, getpid(), getppid());
                sleep(3);
                exit(9);
                // worker_process(child_worker, managers, workers);
            }
            else //Manager
            {
                signal(SIGUSR1, sig_handler_manager_parent);
                printf("[MANAGER ID:%d PID:%d] IM THE PARENT of a WORKER\n", manager->pid, getpid());
                pid_t exited_child = wait(&status);
                printf("[MANAGER ID:%d PID:%d] The WORKER %d finished executing with code %d\n", manager->pid, getpid(), exited_child, WEXITSTATUS(status));
            }
            
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

Manager *new_manager(int pid, char *timeout, char *children_len, char *children)
{
    int children_len_int = atoi(children_len);
    int timeout_int = atoi(timeout);
    int pid_int = pid;

    Manager *manager = malloc(sizeof(Manager) + children_len_int * sizeof(int));

    manager->pid = pid_int;
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

void start_processes(Manager *root, Manager **managers, Worker **workers)
{
    manager_process(root, managers, workers);
    exit(0);
}