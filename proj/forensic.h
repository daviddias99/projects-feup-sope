#ifndef FORENSIC_H
#define FORENSIC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int get_cmd_output(char *args[], char* buf, size_t buf_size);

int get_file_info(char* path/* ,char* info*/);

#endif 