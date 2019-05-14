#include "server_util.h"

int log_file_fd;

bool passwordIsValid(char *password)
{

    int passwordLength = strlen(password);

    if (passwordLength > MAX_PASSWORD_LEN)
        return false;

    if (passwordLength < MIN_PASSWORD_LEN)
        return false;

    return true;
}

int randomBetween(int a, int b)
{

    return rand() % (b - a) + a;
}

char getHexChar(unsigned int a)
{

    if (a > 15)
        return '\0';

    switch (a)
    {
    case 10:
        return 'a';
    case 11:
        return 'b';
    case 12:
        return 'c';
    case 13:
        return 'd';
    case 14:
        return 'e';
    case 15:
        return 'f';
    default:
        return a + '0';
    }
}

int generateSalt(char *saltStr)
{

    for (int i = 0; i < SALT_LEN; i++)
        saltStr[i] = getHexChar(randomBetween(0, 15));

    saltStr[SALT_LEN] = '\0';

    return 0;
}

int generateSHA256sum(char *str, char *result)
{
    int fd1[2];
    int fd2[2];
    
    if(pipe(fd1) < 0){
        perror("Pipe 1");
        return 1;
    }

    if(pipe(fd2) < 0){
        perror("Pipe 2");
        return 2;
    }

    pid_t PID = fork();

    if (PID == 0)
    {
        close(fd1[READ]);
        close(fd2[WRITE]);
        dup2(fd1[WRITE], STDOUT_FILENO);
        dup2(fd2[READ], STDIN_FILENO);
        execlp("sha256sum", "sha256sum", NULL);
    }

    close(fd1[WRITE]);
    close(fd2[READ]);

    write(fd2[WRITE], str, strlen(str));
    close(fd2[WRITE]);
    read(fd1[READ], result, SHA256_SIZE);
    close(fd1[READ]);

    result[SHA256_SIZE] = '\0';

    return 0;
}

int openLogfile(){


    log_file_fd = open(SERVER_LOGFILE,O_WRONLY | O_APPEND,0660);

    return 0;
}

int getLogfileFD(){

    return log_file_fd;
}
