#ifndef FORENSIC_H
#define FORENSIC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <string.h>


int get_cmd_output(char *args[], char* buf, size_t buf_size);

int get_file_info(char* path/* ,char* info*/);

int build_file_line(char* line[], struct stat* file_stat);

int parse_permissions(const mode_t* file_stat, char* permissions);


#endif 