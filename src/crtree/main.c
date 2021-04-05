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
  int start_line_int = *start_line - '0';
  // for para ir iterando sobre las lineas del input
  // Va con negación las condiciones, porque strcmp
  // retorna 0 cuando los strings son iguales
  for (int i = start_line_int; i < input_file->len; i++)
  {
    char *identificator = input_file->lines[i][0];
    if (!strcmp(identificator, "W"))
    {
      printf("Worker\n");
      int id = i;
      int *pid = &id;
      //armar el character de los args   
      //TODO: Preguntar por el caso que no tiene argumentos!!!!
      int args_len_integer = *input_file->lines[i][2] - '0';
      char args[200] = "";
      for (int j = 3; j < args_len_integer + 3; j++)
      {
        if (strchr(input_file->lines[i][j], '\n') != NULL)
        {
          strcat(args, input_file->lines[i][j]);
          break;
        }
        strcat(args, input_file->lines[i][j]);
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
    }
    else
    {
      printf("Root Manager\n");
    }
  };

  //free memory

    for (int i = start_line_int; i < input_file->len; i++)
  {
    char *identificator = input_file->lines[i][0];
    if (!strcmp(identificator, "W"))
    {
      // printf("Just checking worker id: %d\n", *workers[i]->pid);
      free_worker(workers[i]);
    }
  }

  free(workers);

  input_file_destroy(input_file);
  // free(test_worker);
}
