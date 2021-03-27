#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../file_manager/manager.h"



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
        printf("Worker\n");
      }
      else if (!strcmp(identificator, "M"))
      {
        printf("Manager\n");
      }
      else
      {
        printf("Root Manager\n");
      }
    }

    // Ejemplo del line_writer
    line_writer(start_line, "./sum", "ARG_1,ARG_2,ARG_3", "10", "0", "0");
    
    //free memory para el input_file
    input_file_destroy(input_file);
}
