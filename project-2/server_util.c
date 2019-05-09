#include "server_util.h"
#include <semaphore.h>

bank_account_array_t accounts;
int request_fifo_fd;

sem_t empty;
sem_t full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

bank_account_t createAdminBankAccount(char *password)
{

    return createBankAccount(ADMIN_ACCOUNT_ID, password, 0);
}

bank_account_t createBankAccount(uint32_t id, char *password, uint32_t balance)
{

    bank_account_t newBankAccount;

    newBankAccount.account_id = id;
    newBankAccount.balance = balance;
    generateSHA256sum(password,newBankAccount.hash);
    generateSalt(newBankAccount.salt);

    return newBankAccount;
}

int generateSHA256sum(char *str, char* result)
{

    int fd1[2];
    int fd2[2];
    pipe(fd1);
    pipe(fd2);

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

    write(fd2[WRITE],str,strlen(str));
    close(fd2[WRITE]);
    read(fd1[READ],result,SHA256_SIZE);
    close(fd1[READ]);

    result[SHA256_SIZE] = '\0';

    return 0;
}

int insertBankAccount(bank_account_t newAccount)
{

    if (accounts.next_account_index == MAX_BANK_ACCOUNTS)
        return -1;

    if (existsBankAccount(newAccount.account_id))
        return -2;

    accounts.array[accounts.next_account_index] = newAccount;

    return 0;
}

bool existsBankAccount(uint32_t id)
{

    for (unsigned int i = 0; i < accounts.next_account_index; i++)
    {

        if (accounts.array[i].account_id == id)
            return true;
    }

    return false;
}

int createBankOffices(unsigned int quantity){

    offices[0].id = 0;
    offices[0].tid = pthread_self();

    for(unsigned int i = 1; i <= quantity; i++){

        pthread_create(&offices[i].tid,NULL,bank_office_func_stub,NULL);
        offices[i].id = i;
    }

    return 0;
}

void *bank_office_func_stub(void *stub){

    return stub;
}

int setupRequestFIFO(){

    mkfifo("/tmp/secure_srv",REQUEST_FIFO_PERM);
    request_fifo_fd = open("/tmp/secure_srv",O_RDWR);

    return 0;
}

int waitForRequests(){

    while(true){


        //read(request_fifo_fd)





    }



    return 0;

}

int initSyncMechanisms(size_t thread_cnt){

    sem_init(&empty,0,thread_cnt);
    sem_init(&full,0,0);
}
