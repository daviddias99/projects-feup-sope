#include "forensic.h"
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

    // testing code of getting one line
    struct stat file_stats;
    char* file_name = argv[argc-1];

    if(stat(file_name,&file_stats) != 0)
        return -1;
    
    if(S_ISDIR(file_stats.st_mode)){

        // e um diretorio

    }else{

        // ficheiro normal

    }

    char line[9][256];

    build_file_line(line,&file_stats,file_name);

    exit(0);
}