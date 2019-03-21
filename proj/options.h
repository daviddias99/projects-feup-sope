#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BIT(n)              (0x01<<(n))
#define MD5_HASH            (BIT(2))
#define SHA1_HASH           (BIT(1))
#define SHA256_HASH         (BIT(0))

struct options {
    bool check_subdir;
    bool check_fingerprint;
    bool logfile;
    char* logfilename;
    bool output;
    char* output_file;
    uint8_t fp_mask;
};

extern char* optarg;

int parse_options(int argc, char* argv[], struct options* options);
int parse_fingerprints(char* fingerprints, struct options* options);
bool check_argument(char* name);

#endif
