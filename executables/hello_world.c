#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){
    printf("Printeando hello_world");   
    printf("...\n");
    int time = 3;
    if (argc == 2) {time = atoi(argv[3]);}
    sleep(time);
    printf("Hello World\n");
    return 0;
}