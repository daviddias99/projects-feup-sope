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

static bool check_output(char* name) {
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


int parse_options(int argc, char* argv[], struct options* options) {
    int opt;

    while((opt = getopt(argc, argv, "rh:o:v")) != -1){

        switch(opt) {

            case 'r':
                options->check_subdir = true;
                break;

            case 'h':
                options->check_fingerprint = true;
                parse_fingerprints(optarg, options);          
                break;

            case 'o':
                options->output = true;
                if(check_output(optarg)) {
                    printf("%s\n", optarg);
                    options->output_file = optarg;
                }
                else
                    return -1;

                break;

            case 'v':
                options->logfile = true;
                if ((options->logfilename = getenv("LOGFILENAME")) == NULL)
                    options->logfile = false;

                

                break;

            default:
                return -1;
        }
    }

    return 0;
}
