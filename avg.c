#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){
    printf("Calculando promedio de ");
    
    int total = 0;
    for (int i = 1; i < argc; i++)
    {
        printf("%s ", argv[i]);
        int curent_value = atoi(argv[i]);
        total += curent_value;
    }
    printf("...\n");
    float total_d = (float) total;
    float nro_args = (float) (argc - 1);
    float result = total / nro_args;
    int time = 1;
    sleep(time);
    printf("Resultado: %f\n", result);
    return 0;
}