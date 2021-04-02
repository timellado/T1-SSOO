#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../file_manager/manager.h"
#include "processes.h"

int main(int argc, char **argv)
{
    char* file_name = argv[1];
    char* start_line = argv[2];
    InputFile* input_file = read_file(file_name);

    //Validador de argumentos
    if (start_line == NULL || file_name == NULL)
    {
      printf("Modo de uso: ./crtree <ruta_archivo> <inicio_programa>\n");
      return 0;
    }

    // for para ir iterando sobre las lineas del input
    // Va con negación las condiciones, porque strcmp 
    // retorna 0 cuando los strings son iguales
    for (int i = 0; i < input_file->len; i++)
    {
      char* identificator = input_file->lines[i][0];
      if (!strcmp(identificator, "W"))
      {
        char id = i + '0';
        char* pid = &id;
        // char args_len_c = *input_file->lines[i][3];
        // printf("%d\n", args_len_c);
        printf("%s\n", pid);
        Worker* current_worker = new_worker(pid, input_file->lines[i][1], "ARG_1,ARG_2,ARG_3", input_file->lines[i][2]);
        current_worker->time = "10";
        current_worker->return_code = "0";
        current_worker->interrupted = "0";
        line_writer(current_worker);
        free(current_worker);
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

    
    
    // Ejemplo del line_writer
    // line_writer(test_worker);
    
    //free memory
    input_file_destroy(input_file);
    // free(test_worker);

}
