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

    // opt.init_time = start.tv_sec*1000 + start.tv_nsec/1000000;
    opt.init_time = SEC_TO_MIL(start.tv_sec) + NANO_TO_MIL(start.tv_nsec);

    if (parse_options(argc, argv, &opt) != 0)
        exit(2);

    // signal setup

    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = usr_signal_handler;
    sigaction(SIGUSR1,&sigact,NULL);
    sigaction(SIGUSR2,&sigact,NULL);

    struct stat stat_buf;
    
    if (lstat(argv[argc - 1], &stat_buf) != 0)
        return -1;
    
    if (S_ISDIR(stat_buf.st_mode)){
        scan_directory(argv[argc-1], &opt);
    } else {
        build_file_line(&stat_buf, argv[argc - 1], &opt);
    }
    
    // printf("--- DIRCNT %d | FILECNT %d \n", get_dir_cnt(),get_file_cnt());

    close(opt.output_fd);

    return 0;
}
