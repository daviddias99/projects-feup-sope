#include "options.h"


static int parse_fingerprints(char* fingerprints, struct options* options) {
    char* token = strtok(fingerprints, ",");

    while(token != NULL){
        
        if(strcmp(token, "md5") == 0){
            options->fp_mask |= MD5_HASH;
        } else if(strcmp(token, "sha1") == 0){
            options->fp_mask |= SHA1_HASH;
        } else if(strcmp(token, "sha256") == 0){
            options->fp_mask |= SHA256_HASH;
        } else {
            return -1;
        }

        token = strtok(NULL, ",");
    }

    return 0;
}

static bool check_argument(char* filename) {

    // Checks if string has minimum number of character to be valid ( < strlen(a.txt))
    if(strlen(filename) < 5)
        return false;

    char* name = malloc(sizeof(char)*strlen(filename) + 1);
    strcpy(name, filename);

    if(name[0] == '-'){
        free(name);
        return false;
    }

    char* token = strtok(name, ".");

    if((token = strtok(NULL, ".")) == NULL)
        return false;

    // Checks if it has a valid extension (strcmp returns 0 if equal)
    if(strcmp(token, "csv") && strcmp(token, "txt")){
        free(name);
        return false;
    }

    // Checks if file name is valid
    if((token = strtok(NULL, ".")) != NULL){
        free(name);    
        return false;
    }

    free(name);
    return true;
}


int parse_options(int argc, char* argv[], struct options* options){
    int opt;

    memset(options,0,sizeof(struct options));

    options->argv = argv;
    options->process_root_pid = getpid();

    while((opt = getopt(argc, argv, "rh:o:v")) != -1){

        switch(opt) {

            case 'r':
                options->check_subdir = true;
                break;

            case 'h':
                options->check_fingerprint = true;

                if(parse_fingerprints(optarg,options) != 0)
                    return 1;         

                break;

            case 'o':
                options->output = true;

                if(check_argument(optarg)) {
                    options->output_file = optarg;
                    options->output_fd = open(options->output_file, O_WRONLY | O_CREAT | O_EXCL, OUTPUT_OPEN_MODE);
                    if (options->output_fd == -1) {
                        perror("output file open");
                        exit(3);
                    }
                }
                else
                    return 2;
                
                break;

            case 'v':
                options->logfile = true;
                if ((options->logfilename = getenv("LOGFILENAME")) == NULL) {
                    printf("LOGFILENAME env var not set\n");
                    exit(-1);
                }

                if(check_argument(options->logfilename)) {
                    options->logfilename_fd = open(options->logfilename, O_WRONLY | O_CREAT, OUTPUT_OPEN_MODE);
                    
                    if(options->logfilename_fd == -1) {
                        perror("Log file open");
                        exit(3);
                    }
                }
                else
                    exit(3);

                break;

            default:
                return 4;
        }
    }

    if (! options->output)
        options->output_fd = STDOUT_FILENO;

    options->base_directory = argv[argc - 1];

    return 0;
}

