#include "forensic.h"
#include "options.h"

int main(int argc, char* argv[]){

    if (argc < 2) {
        fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
        exit(1);
    }

    struct options opt;

    if (parse_options(argc, argv, &opt) != 0)
        exit(2);

    setup_time(&opt);
    setup_signals();

    struct stat stat_buf;

    if (lstat(argv[argc - 1], &stat_buf) != 0){

        perror("lstat error");
        return -1;
    }
        

    if (S_ISDIR(stat_buf.st_mode)){

        if (scan_directory(argv[argc - 1], &opt) != 0)
            return -2;
    }
    else{

        if (build_file_line(&stat_buf, argv[argc - 1], &opt) != 0)
            return -3;
    }

    close(opt.output_fd);

    return 0;
}
