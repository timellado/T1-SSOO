#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../file_manager/manager.h"
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
  Worker **workers = malloc(sizeof(Worker *) * input_file->len);
  Manager **managers = malloc(sizeof(Manager *) * input_file->len);
  Manager *root;
  int start_line_int = *start_line - '0';
  // for para ir iterando sobre las lineas del input
  // Va con negación las condiciones, porque strcmp
  // retorna 0 cuando los strings son iguales
  for (int i = start_line_int; i < input_file->len; i++)
  {
    int *pid = &i;
    char *identificator = input_file->lines[i][0];
    if (!strcmp(identificator, "W"))
    {
      printf("Worker\n");

      //armar el character de los args
      char args[200] = "";
      for (int j = 0; j < atoi(input_file->lines[i][2]); j++)
      {
        if (strchr(input_file->lines[i][j + 3], '\n') != NULL)
        {
          strcat(args, input_file->lines[i][j + 3]);
          break;
        }
        strcat(args, input_file->lines[i][j + 3]);
        strcat(args, ",");
      }
      args[strlen(args) - 1] = '\0'; // eliminar el \n o la coma en caso que no tenga \n
      // printf("ARGS: %s\n", args);
      Worker *current_worker = new_worker(pid, input_file->lines[i][1], input_file->lines[i][2], args);
      line_writer(current_worker);
      workers[i] = current_worker;
      // free(current_worker);
    }
    else if (!strcmp(identificator, "M"))
    {
      printf("Manager\n");
      char children[200] = "";
      for (int j = 0; j < atoi(input_file->lines[i][2]); j++)
      {
        strcat(children, input_file->lines[i][j + 3]);
      }
      printf("CHILDS %s", children);
      Manager *current_manager = new_manager(pid, input_file->lines[i][1], input_file->lines[i][2], children);
      managers[i] = current_manager;
    }
    else
    {
      printf("Root Manager\n");
      char children[200] = "";
      for (int j = 0; j < atoi(input_file->lines[i][2]); j++)
      {
        strcat(children, input_file->lines[i][j + 3]);
      }
      printf("CHILDS %s", children);
      Manager *root_manager = new_manager(pid, input_file->lines[i][1], input_file->lines[i][2], children);
      root = root_manager;
    }
  };
  

  // start_processes(root, managers, workers);

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
      printf("NRO CHILDS: %d\n", managers[i]->children_len);
      for (size_t j = 0; j < managers[i]->children_len; j++)
      {
        printf("CHECKING CHILDS IDS: %d\n", managers[i]->children_ids[j]);
      }
      
      free_manager(managers[i]);
    }
  }

  free_manager(root);
  free(workers);
  free(managers);

  input_file_destroy(input_file);
  // free(test_worker);
}
