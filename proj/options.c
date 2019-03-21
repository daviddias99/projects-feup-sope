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
    
    i ? i : (i = 1); 
    permissions[i] = '\0';
        
    return 0;
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

                if(check_argument(optarg))
                    options->output_file = optarg;
                else
                    return 2;
                
                break;

            case 'v':
                options->logfile = true;

                options->logfilename = getenv("LOGFILENAME");

                if(options->logfilename == NULL)
                    return 3;

                break;

            default:
                return 4;
        }
    }

    return 0;
}

