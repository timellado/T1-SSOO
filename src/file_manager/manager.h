// Tells the compiler to compile this file once
#pragma once

// Define compile-time constants
#define MAX_SPLIT 255
#define BUFFER_SIZE 4096

#include "../crtree/processes.h"

// Define the struct
typedef struct inputfile {
  int len;
  char*** lines;  // This is an array of arrays of strings
} InputFile;

// Declare functions
InputFile* read_file(char* filename);
void input_file_destroy(InputFile* input_file);
void line_writer(Worker* worker);
void manager_file_writer_manager(char* manager_child_filename, char* manager_parent_filename);
void manager_file_writer_worker(char* child_filename, char* manager_filename);