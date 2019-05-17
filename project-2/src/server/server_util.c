#include "server_util.h"

static int log_file_fd;

int getLogfile()
{
    return log_file_fd;
}

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

int openLogFile()
{
    log_file_fd = open(SERVER_LOGFILE, O_WRONLY | O_APPEND, 0660);

    if(log_file_fd == -1){

        perror("Log file opening error");
        return -1;
    }

    return 0;
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

    if (pipe(fd1) < 0)
    {
        perror("Pipe 1");
        return 1;
    }

    if (pipe(fd2) < 0)
    {
        perror("Pipe 2");
        return 2;
    }

    pid_t PID = fork();

    if (PID == 0)
    {
        if(close(fd1[READ]) != 0){

            perror("SHA256 co-process error");
            return -1;
        }

        if(close(fd2[WRITE])){

            perror("SHA256 co-process error");
            return -1;
        }

        if(dup2(fd1[WRITE], STDOUT_FILENO) == -1){

            perror("SHA256 co-process error");
            return -1;
        }

        if(dup2(fd2[READ], STDIN_FILENO)){

            perror("SHA256 co-process error");
            return -1;
        }

        execlp("sha256sum", "sha256sum", NULL);

        return -2;
    }

    if (close(fd1[WRITE]) == -1){

        perror("SHA256 co-process error");
        return -1;
    }

    if (close(fd2[READ]) == -1){

        perror("SHA256 co-process error");
        return -1;
    }

    if(write(fd2[WRITE], str, strlen(str)) == -1){

        perror("SHA256 co-process error");
        return -1;
    }

    if (close(fd2[WRITE]) == -1){

        perror("SHA256 co-process error");
        return -1;
    }

    if(read(fd1[READ], result, SHA256_SIZE) == -1){

        perror("SHA256 co-process error");
        return -1;
    }

    if (close(fd1[READ]) == -1){

        perror("SHA256 co-process error");
        return -1;
    }

    result[SHA256_SIZE] = '\0';
    
    wait(NULL);

    return 0;
}


