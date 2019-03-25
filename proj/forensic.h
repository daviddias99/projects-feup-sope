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
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#include "options.h"
#include <signal.h>

#define SEC_TO_MIL(a)                 (a*1000)
#define NANO_TO_MIL(a)                a/1000000;

int scan_directory(char* path, const struct options* opt);

int reg_execution(pid_t pid, char* act, const struct options* opt);

int get_cmd_output(char *args[], char* buf, size_t buf_size);

int get_file_info(char* path/* ,char* info*/);

int build_file_line(const struct stat* file_stat, char* file_name, const struct options* opt);

int print_file_line(char* line[9], int fd);

int parse_permissions(char* permissions, mode_t file_stat);

int build_ISO8601_date(char* date, time_t time);

void usr_signal_handler(int signo);

int get_file_cnt();

int get_dir_cnt();

#endif 