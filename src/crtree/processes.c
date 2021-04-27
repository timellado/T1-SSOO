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

void worker_process(Worker *worker, Manager **managers, Worker **workers, Manager *root, int total_processes, InputFile *input_file)
{
    signal(SIGABRT, sig_handler_worker);
    signal(SIGINT, SIG_IGN);
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
    // printf("[%d] WORKER FREEING ALL\n", worker->id);
    free_all(managers, workers, root, total_processes, input_file);
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

void manager_logic(Manager *manager, Manager **managers, Worker **workers, Manager *root, int total_processes, InputFile *input_file)
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
                // printf("[%d] MANAGER WITH PID: %d\n", child_manager->id, getpid());
                manager_process(child_manager, managers, workers, root, total_processes, input_file);
            }
            else if (workers[manager->children_ids[i]] != NULL) // WORKER CHILD
            {
                Worker *child_worker = workers[manager->children_ids[i]];
                // printf("[%d] WORKER WITH PID: %d\n", child_worker->id, getpid());
                worker_process(child_worker, managers, workers, root, total_processes, input_file);
            }
        }
        else
        {
            if (managers[manager->children_ids[i]] != NULL) // MANAGER CHILD
            {
                Manager *child_manager = managers[manager->children_ids[i]];
                child_manager->pid = pid;
            }
            else if (workers[manager->children_ids[i]] != NULL) // WORKER CHILD
            {
                Worker *child_worker = workers[manager->children_ids[i]];
                child_worker->pid = pid;
            }
        }
    }
    pid_t timeout_pid = fork();
    if (timeout_pid == 0)
    {
        sleep(manager->timeout);
        free_all(managers, workers, root, total_processes, input_file);
        exit(EXIT_SUCCESS);
    }
    else
    {
        for (size_t i = 0; i < manager->children_len; i++)
        {
            if (managers[manager->children_ids[i]] != NULL) // MANAGER CHILD
            {
                Manager *child_manager = managers[manager->children_ids[i]];
                childs_pids[child_count] = child_manager->pid;
                child_count++;
            }
            else if (workers[manager->children_ids[i]] != NULL) // WORKER CHILD
            {
                Worker *child_worker = workers[manager->children_ids[i]];
                childs_pids[child_count] = child_worker->pid;
                child_count++;
            }
        }

        int exited_children = 0;
        int timeout = 0;
        while (exited_children < child_count)
        {
            pid_t exited_child = wait(&status);
            if (exited_child == timeout_pid)
            {
                kill(getpid(), SIGABRT);
                timeout = 1;
            }
            for (int i = 0; i < manager->children_len; i++)
            {
                if ((workers[manager->children_ids[i]] != NULL) && (exited_child == workers[manager->children_ids[i]]->pid))
                {
                    Worker *child_worker = workers[manager->children_ids[i]];
                    char *manager_filename = calloc(20, sizeof(char));
                    sprintf(manager_filename, "%d.txt", manager->id);
                    // printf("IM THE MANAGER %d AND MY FILENAME IS %s\n", getpid(), manager_filename);
                    char *child_filename = calloc(20, sizeof(char));
                    sprintf(child_filename, "%d.txt", child_worker->id);
                    // printf("IM THE EXITED CHILD %d FROM MANAGER %d AND MY FILENAME IS %s\n", exited_child, getpid(), child_filename);
                    manager_file_writer_worker(child_filename, manager_filename);
                    free(manager_filename);
                    free(child_filename);
                    exited_children++;
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
                    exited_children++;
                }
            }
        }
        if (!timeout)
        {
            kill(timeout_pid, SIGABRT);
            wait(NULL);
        }
    }
}

void manager_process(Manager *manager, Manager **managers, Worker **workers, Manager *root, int total_processes, InputFile *input_file)
{
    signal(SIGABRT, sig_SIGABRT_handler_manager);
    signal(SIGINT, SIG_IGN);
    manager_logic(manager, managers, workers, root, total_processes, input_file);
    // printf("[%d] MANAGER FREEING ALL\n", manager->id);
    free_all(managers, workers, root, total_processes, input_file);
    exit(EXIT_SUCCESS);
}

void root_process(Manager *root, Manager **managers, Worker **workers, int total_processes, InputFile *input_file)
{
    signal(SIGABRT, sig_SIGABRT_handler_manager);
    signal(SIGINT, sig_SIGABRT_handler_manager);
    manager_logic(root, managers, workers, root, total_processes, input_file);
}

Manager *new_manager(int id, int timeout, int children_len, int *children)
{
    int children_len_int = children_len;
    int timeout_int = timeout;
    Manager *manager = malloc(sizeof(Manager) + children_len_int * sizeof(int));

    manager->id = id;
    manager->timeout = timeout_int;
    manager->children_len = children_len_int;

    for (int i = 0; i < children_len_int; i++)
    {
        manager->children_ids[i] = children[i];
    }

    return manager;
}

void free_manager(Manager *manager)
{
    free(manager);
}

void start_processes(Manager *root, Manager **managers, Worker **workers, int total_processes, InputFile *input_file)
{
    root_process(root, managers, workers, total_processes, input_file);
}

void free_all(Manager **managers, Worker **workers, Manager *root, int total_processes, InputFile *input_file)
{
    for (int i = 0; i < total_processes; i++)
    {
        if (managers[i] != NULL) // MANAGER
        {
            // printf("FREEING MANAGER %d\n", managers[i]->id);
            free_manager(managers[i]);
        }
        else if (workers[i] != NULL) // WORKER
        {
            // printf("FREEING WORKER %d\n", workers[i]->id);
            free_worker(workers[i]);
        }
    }
    free(workers);
    free(managers);
    free_manager(root);
    input_file_destroy(input_file);
}

// Encuentra o crea el archvo y escribe una lina de output.
// Para los argumentos se debe pasar un char con todos los argumentos separados por una ","
void line_writer(Worker *worker)
{
  char *filename = calloc(20, sizeof(char));
  sprintf(filename, "%d.txt", worker->id);
  // printf("%s\n", filename);
  FILE *output_file = fopen(filename, "w");
  // TODO: Definir cuando se ejecute el proceso

  if (worker->args_len == 0)
  {
    fprintf(output_file, "%s,%d,%d,%d\n", worker->executable, worker->time, worker->return_code, worker->interrupted);
  }
  else
  {
    char str[200];
    strcpy(str, "");
    for (int i = 0; i < worker->args_len; i++)
    {
      strcat(str, worker->args[i + 1]);
      strcat(str, ",");
    }
    if(strstr(str, "\n") != NULL) {
      str[strlen(str) - 2] = '\0'; //quita el \n y la última coma
    }
    else{
      str[strlen(str) - 1] = '\0';
    }
    
    fprintf(output_file, "%s,%s,%d,%d,%d\n", worker->executable, str, worker->time, worker->return_code, worker->interrupted);
  }

  fclose(output_file);
  free(filename);
}

void manager_file_writer_manager(char* manager_child_filename, char* manager_parent_filename)
{
  FILE *child_file = fopen(manager_child_filename, "r");
  FILE *manager_output_file = fopen(manager_parent_filename, "a");
  char buffer[BUFFER_SIZE];
  while (fgets(buffer, BUFFER_SIZE, child_file))
  {
    fprintf(manager_output_file, "%s", buffer);
  }
  fclose(manager_output_file);
  fclose(child_file);
}

void manager_file_writer_worker(char* worker_filename, char* manager_filename)
{
  FILE *child_file = fopen(worker_filename, "r");
  FILE *manager_output_file = fopen(manager_filename, "a");
  char buffer[BUFFER_SIZE];
  fgets(buffer, BUFFER_SIZE, child_file);
  fprintf(manager_output_file, "%s", buffer);
  fclose(manager_output_file);
  fclose(child_file);
}