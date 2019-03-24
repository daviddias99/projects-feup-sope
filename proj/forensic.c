#include "forensic.h"

#define READ    0
#define WRITE   1

#define MD5_SIZE    32
#define SHA1_SIZE   40
#define SHA256_SIZE 64

#define ISO_DATE_SIZE 19
#define PERM_SIZE     3

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

int parse_permissions(char* permissions, mode_t file_stat) {

    size_t i = 0;
    permissions[i] = '-';

    if (file_stat & S_IRUSR) {
        permissions[0] = 'r';
        i++;
    }
        
    if(file_stat & S_IWUSR){

        permissions[i] = 'w';
        i++;
    }

    if(file_stat & S_IXUSR){

        permissions[i] = 'x';
        i++;
    }
    
    i ? i : (i = 1); 
    permissions[i] = '\0';
        
    return 0;
}

int build_ISO8601_date(char* date, time_t time){

    struct tm* lTime = localtime(&time);

    // note, string is 19 chars long (+1 '/0')
    sprintf(date,"%04d-%02d-%02dT%02d:%02d:%02d",lTime->tm_year+1900,lTime->tm_mon+1,lTime->tm_mday,lTime->tm_hour,lTime->tm_min,lTime->tm_sec);

    return 0;
}

int build_file_line(const struct stat* file_stat, char* file_name, const struct options* opt) {

    char* args[3] = { "file", file_name, NULL};
    char* file_output = (char*) malloc(64 * sizeof(char));
    get_cmd_output(args, file_output, 64);
    char* file_type = strtok(file_output, " ");
    file_type = strtok(NULL, ",");

    char file_size[100];
    sprintf(file_size, "%d", (int) file_stat->st_size);



    size_t line_size = strlen(file_name) - strlen(opt->base_directory) + strlen(file_type) + strlen(file_size) + 
                       2 * ISO_DATE_SIZE + PERM_SIZE + MD5_SIZE + SHA1_SIZE + SHA256_SIZE +
                       10;
    char* line = (char*) malloc(sizeof(char) * line_size);
    line[0] = 0;

    strcat(line, file_name + strlen(opt->base_directory) + 1);
    strcat(line, ",");
    strcat(line, file_type);
    strcat(line, ",");                                  // file type
    strcat(line, file_size);    
    strcat(line, ",");   // file size
    parse_permissions(line + strlen(line), file_stat->st_mode);         // file permissions
    strcat(line, ",");    
    build_ISO8601_date(line + strlen(line),file_stat->st_atime);        // last access date
    strcat(line, ",");    
    build_ISO8601_date(line + strlen(line),file_stat->st_mtime);        // last modification date

    free(file_output);

    

    if (opt->check_fingerprint) {    

        if (opt->fp_mask & MD5_HASH) {
            args[0] = "md5sum";
            //char md5_output[MD5_SIZE + 1];
            strcat(line, ",");    
            get_cmd_output(args, line + strlen(line), MD5_SIZE + 1);
            /*
            printf("%s\n", md5_output);

            printf("%s\n", line[i]);*/

            
        }

        if (opt->fp_mask & SHA1_HASH)
        {
            args[0] = "sha1sum";
            //char md5_output[MD5_SIZE + 1];
            strcat(line, ",");    
            get_cmd_output(args, line + strlen(line), SHA1_SIZE + 1);
        }

        if (opt->fp_mask & SHA256_HASH)
        {
            args[0] = "sha256sum";
            //char md5_output[MD5_SIZE + 1];
            strcat(line, ",");    
            get_cmd_output(args, line + strlen(line), SHA256_SIZE + 1);
        }
    }

    char* action = (char*) malloc(sizeof(char) * 50 + sizeof(file_name));

    sprintf(action, "Analized %s", file_name + strlen(opt->base_directory) + 1);
    reg_execution(getpid(), action, opt);

   
    strcat(line, "\n");
    lseek(opt->output_fd, 0, SEEK_END);
    write(opt->output_fd, line, strlen(line));
    free(line);
    free(action);

    return 0;
}

int reg_execution(pid_t pid, char* act, const struct options* opt){
    
    if(!opt->logfile)
        return 0;

    char* reg = (char*) malloc(sizeof(char) * 100);

    struct timespec current;
    clock_gettime(CLOCK_REALTIME, &current);

    long double curr_time = current.tv_sec*1000 + (long double) current.tv_nsec/1000000;

    sprintf(reg, "%.2Lfms - %08d - %s\n", curr_time - opt->init_time, pid, act);
    
    lseek(opt->logfilename_fd, 0, SEEK_END);
    write(opt->logfilename_fd, reg, strlen(reg));
    free(reg);
    
    return 0;
}

int scan_directory(char* path, const struct options* opt) {
    DIR* dirp;
    struct dirent* direntp;
    char fpath[257];


    if ((dirp = opendir(path)) == NULL) {
        perror(path);
        exit(2);
    }
    while ((direntp = readdir(dirp)) != NULL) {   
        struct stat stat_buf;
        sprintf(fpath, "%s/%s", path, direntp->d_name);


        if (lstat(fpath, &stat_buf) != 0)
            return -1;

        if (S_ISDIR(stat_buf.st_mode)) {
            
            if (opt->check_subdir) { 

                if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
                    continue;

                int pid = fork();
                if (pid == -1) {
                    exit(5);
                } else if (pid == 0) {
                    return scan_directory(fpath, opt);
                }
            }
        } else {
            build_file_line(&stat_buf, fpath, opt);
        }
    }

    closedir(dirp);
    return 0;
}
