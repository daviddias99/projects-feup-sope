#include "forensic.h"

#define READ    0
#define WRITE   1


int get_file_info(char* path/* ,char* info*/) {

    char* args[] = { "sha1sum", path, NULL };
    char buf[128];

    get_cmd_output(args, buf, 128);

    printf("%s\n", buf);

    args[0] = "sha256sum";

    get_cmd_output(args, buf, 128);

    printf("%s\n", buf);

    args[0] = "file";

    get_cmd_output(args, buf, 128);

    printf("%s\n", buf);

    return -1;
}


int get_cmd_output(char *args[], char* buf, size_t buf_size) {

    int pid, fd[2];

    pipe(fd);
    pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        close(fd[READ]);
        dup2(fd[WRITE], STDOUT_FILENO);
        execvp(args[0], args);
    } else if (pid > 0) {
        int status, n_read;

        close(fd[WRITE]);

        n_read = read(fd[READ], buf, buf_size);
        buf[n_read - 1] = 0;
       
        wait(&status);
        if (status)
            return -1;
    }

    return 0;
}