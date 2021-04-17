#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){
    printf("Sumando: %s + %s\n", argv[1], argv[2]);
    printf("...\n");
    int result = atoi(argv[1]) + atoi(argv[2]);
    int time = 3;
    if (argc == 4) {time = atoi(argv[3]);}
    sleep(time);
    printf("Resultado: %d\n", result);
    return 0;
}