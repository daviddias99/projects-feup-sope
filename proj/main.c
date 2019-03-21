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

    // testing for getting on line
    struct stat file_stats;

    if(stat(argv[argc-1],&file_stats) != 0)
        return -1;
    
    // allocating space for one line
    char** line = (char**) malloc(sizeof(char*) * 9);

    for(int i = 0; i < 9; i++ )
        *(line+i) = (char*) malloc(sizeof(char*) * 256);

    memset(line,0,256*9);

    // building the line
    build_file_line(line,&file_stats);

    exit(0);
}