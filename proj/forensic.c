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

#define BIT(n)              (0x01<<(n))
#define MD5_HASH            (BIT(2))
#define SHA1_HASH           (BIT(1))
#define SHA256_HASH         (BIT(0))

extern char *optarg;

struct OPTIONS {
    bool check_subdir;
    bool check_fingerprint;
    int8_t fp_mask; // mask = bit(2) = md5 ; bit(1) = sha1 ; bit(0) = sha256
    bool logfile;
    char* logfilename;
    bool output;
    char* output_file;

} options;

bool check_output(char* name){
    //char* extension;

    if(name[0] == '-')
        return false;

    //extension = strstr(name, ".csv");

    //TODO: Further extension verification

    return true;
}

int parse_fingerprints(char* fingerprints){
    char* token = strtok(fingerprints, ",");

    while(token != NULL){
        
        if(strcmp(token, "md5") == 0){
            options.fp_mask |= MD5_HASH;
        } else if(strcmp(token, "sha1") == 0){
            options.fp_mask |= SHA1_HASH;
        } else if(strcmp(token, "sha256") == 0){
            options.fp_mask |= SHA256_HASH;
        } else {
            return -1;
        }

        token = strtok(NULL, ",");
    }

    return 0;
}

int parse_options(int argc, char* argv[]){
    int opt;

    while((opt = getopt(argc, argv, "rh:o:v")) != -1){

        switch(opt) {

            case 'r':
                options.check_subdir = true;
                break;

            case 'h':
                options.check_fingerprint = true;
                parse_fingerprints(optarg);          
                break;

            case 'o':
                options.output = true;
                if(check_output(optarg))
                    options.output_file = optarg;
                else
                    return 1;
                
                break;

            case 'v':
                options.logfile = true;
                options.logfilename = getenv("LOGFILENAME");
                break;

            default:
                exit(69);
        }
    }

    return 0;
}



int parse_permissions(const mode_t* file_stat, char* permissions){

    size_t i = 0;
    permissions[i] = '-';

    if(*file_stat & S_IRUSR){

        permissions[0] = 'r';
        i++;
    }
        
    if(*file_stat & S_IWUSR){

        permissions[i] = 'w';
        i++;
    }

    if(*file_stat & S_IXUSR){

        permissions[i] = 'x';
        i++;
    }
        permissions[i] = '\0';
        
    return 0;
}

int main(int argc, char* argv[], char* envp[]){

    if (argc < 2)
    {
        fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
        exit(1);
    }

    if(parse_options(argc, argv) == 1)
        exit(2);

/*
    if(fork() == 0){
        execlp("file", "file", argv[1], NULL);
        exit(2);
    }

*/

    exit(0);
}