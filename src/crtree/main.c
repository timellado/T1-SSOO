#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "processes.h"

int main(int argc, char **argv)
{
  char *file_name = argv[1];
  char *start_line = argv[2];
  InputFile *input_file = read_file(file_name);

  //Validador de argumentos
  if (start_line == NULL || file_name == NULL)
  {
    printf("Modo de uso: ./crtree <ruta_archivo> <inicio_programa>\n");
    return 0;
  }
  Worker **workers = calloc((input_file->len + 1), sizeof(Worker *));
  Manager **managers = calloc((input_file->len + 1), sizeof(Manager *));
  Manager *root;
  int start_line_int = *start_line - '0';
  // for para ir iterando sobre las lineas del input
  // Va con negación las condiciones, porque strcmp
  // retorna 0 cuando los strings son iguales
  for (int i = start_line_int; i < input_file->len; i++)
  {
    char *identificator = input_file->lines[i][0];
    if (!strcmp(identificator, "W"))
    {
      // printf("Worker\n");

      //armar los args
      int args_len = atoi(input_file->lines[i][2]);

      char **args = calloc(args_len + 2, sizeof(char *));
      args[0] = input_file->lines[i][1];
      for (int j = 1; j <= args_len; j++)
      {
        args[j] = input_file->lines[i][j + 2];
      }
      args[args_len + 1] = NULL;
      Worker *current_worker = new_worker(i, input_file->lines[i][1], args_len, args);
      workers[i] = current_worker;
    }
    else if (!strcmp(identificator, "M"))
    {
      // printf("Manager\n");
      int children_len = atoi(input_file->lines[i][2]);
      int children[children_len];
      int child_count = 0;
      for (int j = 0; j < atoi(input_file->lines[i][2]); j++)
      {
        if (atoi(input_file->lines[i][j + 3]) >= start_line_int)
        {
          children[child_count++] = atoi(input_file->lines[i][j + 3]);
        }
        else
        {
          children_len--;
        }
      }
      Manager *current_manager = new_manager(i, atoi(input_file->lines[i][1]), children_len, children);
      managers[i] = current_manager;
    }
    else
    {
      // printf("Root Manager\n");
      int children_len = atoi(input_file->lines[i][2]);
      int children[children_len];
      int child_count = 0;
      for (int j = 0; j < atoi(input_file->lines[i][2]); j++)
      {
        if (atoi(input_file->lines[i][j + 3]) >= start_line_int)
        {
          children[child_count++] = atoi(input_file->lines[i][j + 3]);
        }
        else
        {
          children_len--;
        }
      }
      Manager *root_manager = new_manager(i, atoi(input_file->lines[i][1]), children_len, children);
      root = root_manager;
    }
  };

  start_processes(root, managers, workers, input_file->len, input_file);

  //free memory

  for (int i = start_line_int; i < input_file->len; i++)
  {
    char *identificator = input_file->lines[i][0];
    if (!strcmp(identificator, "W"))
    {
      // printf("Just checking worker id: %d\n", *workers[i]->pid);
      free_worker(workers[i]);
    }
    else if (!strcmp(identificator, "M"))
    {
      free_manager(managers[i]);
    }
  }

  free_manager(root);
  free(workers);
  free(managers);

  input_file_destroy(input_file);
  exit(EXIT_SUCCESS);
}
