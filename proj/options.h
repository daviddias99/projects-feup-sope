#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BIT(n)              (0x01<<(n))
#define MD5_HASH            (BIT(2))
#define SHA1_HASH           (BIT(1))
#define SHA256_HASH         (BIT(0))

#define OUTPUT_OPEN_MODE    0644

struct options {
    bool check_subdir;
    bool check_fingerprint;
    bool logfile;
    char* logfilename;
    int logfilename_fd;
    bool output;
    char* output_file;
    uint8_t fp_mask;
    int output_fd;
    char* base_directory;
    double init_time;
};

extern char* optarg;

int parse_options(int argc, char* argv[], struct options* options);
//int parse_fingerprints(char* fingerprints, struct options* options);
//bool check_argument(char* name);

#endif
