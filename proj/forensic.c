#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "options.h"


int main(int argc, char* argv[]){

    if (argc < 2) {
        fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
        exit(1);
    }

    struct options options;
    options.fp_mask = 0;

    if(parse_options(argc, argv, &options) == 1)
        exit(2);

    if (options.check_subdir) {
        printf("r\n");
    } 
    if (options.check_fingerprint) {
        printf("h\n");
    } 
    if (options.fp_mask & MD5_HASH)
        printf("md5\n");
    if (options.fp_mask & SHA1_HASH)
        printf("sha1\n");
    if (options.fp_mask & SHA256_HASH)
        printf("sha256\n");
    if (options.logfile)
        printf("%s\n", options.logfilename);
    if (options.output)
        printf("%s\n", options.output_file);



    exit(0);
}