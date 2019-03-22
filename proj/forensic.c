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

int parse_permissions(mode_t file_stat, char* permissions){

    size_t i = 0;
    permissions[i] = '-';

    if(file_stat & S_IRUSR){

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

int build_file_line(char line[][256], struct stat* file_stat, char* file_name){

    char* argsFILE[3] = {"file",file_name,NULL};
    char* argsMD5[3] = {"md5sum",file_name,NULL};
    char* argsSHA1[3] = {"sha1sum",file_name,NULL};
    char* argsSHA256[3] = {"sha256sum",file_name,NULL};
    
    char* file_output = (char*) malloc(64 * sizeof(char));
    get_cmd_output(argsFILE,file_output,64);
    char* file_type = strtok(file_output, " ");
    file_type = strtok(NULL, ",");

    char* md5_output = (char*) malloc(33 * sizeof(char));
    get_cmd_output(argsMD5,md5_output,33);
    char* md5 = strtok(md5_output, " ");

    char* sha1_output = (char*) malloc(41 * sizeof(char));
    get_cmd_output(argsSHA1,sha1_output,41);
    char* sha1 = strtok(sha1_output, " ");
    
    char* sha256_output = (char*) malloc(65 * sizeof(char));
    get_cmd_output(argsSHA256,sha256_output,65);
    char* sha256 = strtok(sha256_output, " ");

    strcpy(line[0],file_name);                              // file name
    strcpy(line[1],file_type);                              // file type
    sprintf(line[2],"%d",(int)file_stat->st_size);          // file size
    parse_permissions(file_stat->st_mode, line[3]);         // file permissions
    build_ISO8601_date(line[4],file_stat->st_atime);        // last access date
    build_ISO8601_date(line[5],file_stat->st_mtime);        // last modification date
    strcpy(line[6],md5);                                    // md5
    strcpy(line[7],sha1);                                   // sha1
    strcpy(line[8],sha256);                                 // sha256
    printf("%s - %s - %s - %s - %s - %s - %s - %s - %s \n",line[0],line[1],line[2],line[3],line[4],line[5],line[6],line[7],line[8]);
    return 0;
}

