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

static bool check_argument(char* name) {
    //char* extension;

    if(name[0] == '-')
        return false;

    //extension = strstr(name, ".csv");

    //TODO: Further extension verification

    return true;
}


int parse_options(int argc, char* argv[], struct options* options){
    int opt;

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
                    options->output_fd = open(optarg, O_WRONLY | O_CREAT | O_EXCL, OUTPUT_OPEN_MODE);
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

                options->logfilename = getenv("LOGFILENAME");

                if(options->logfilename == NULL)
                    return 3;
                else
                    options->logfilename_fd = open(options->logfilename, O_WRONLY | O_CREAT | O_EXCL, OUTPUT_OPEN_MODE);
                
                if(options->logfilename_fd == -1) {
                        perror("Log file open");
                        exit(3);
                }

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

