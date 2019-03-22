#ifndef FORENSIC_H
#define FORENSIC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>


int get_cmd_output(char *args[], char* buf, size_t buf_size);

int get_file_info(char* path/* ,char* info*/);

int build_file_line(char line[][256], struct stat* file_stat, char* file_name);

int parse_permissions(mode_t file_stat, char* permissions);

int build_ISO8601_date(char* date, time_t time);
#endif 