#include "forensic.h"
#include "options.h"

int main(int argc, char* argv[]){

    if (argc < 2) {
        fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
        exit(1);
    }

    struct options opt;
    opt.fp_mask = 0;

    // TODO: Possivelmente fazer uma função dedicada a inicializar o tempo
    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);

    opt.init_time = start.tv_sec*1000 + start.tv_nsec/1000000;

    if (parse_options(argc, argv, &opt) != 0)
        exit(2);

    struct stat stat_buf;
    
    if (lstat(argv[argc - 1], &stat_buf) != 0)
        return -1;
    
    if (S_ISDIR(stat_buf.st_mode)){
        scan_directory(argv[argc-1], &opt);
    } else {
        build_file_line(&stat_buf, argv[argc - 1], &opt);
    }
    
    close(opt.output_fd);

    return 0;
}
