#include "server_util.h"
#include <semaphore.h>

bank_account_array_t accounts;
int request_fifo_fd;

sem_t empty;
sem_t full;
pthread_mutex_t request_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t account_array_mutex = PTHREAD_MUTEX_INITIALIZER;
RequestQueue_t requests;
bank_office_t offices[MAX_BANK_OFFICES];

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
    generateSalt(newBankAccount.salt);

    char temp[MAX_PASSWORD_LEN+SALT_LEN];

    strcpy(temp,password);
    strcat(temp,newBankAccount.salt);

    generateSHA256sum(temp,newBankAccount.hash);

    return newBankAccount;
}

bank_account_t* findBankAccount(uint32_t id){

    pthread_mutex_lock(&account_array_mutex);

    for(unsigned int i = 0; i < accounts.next_account_index; i++){

        if(accounts.array[i].account_id == id)
            return &accounts.array[i];
    }

    pthread_mutex_unlock(&account_array_mutex);

    return NULL;
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
    pthread_mutex_lock(&account_array_mutex);

    for (unsigned int i = 0; i < accounts.next_account_index; i++)
    {
        
        if (accounts.array[i].account_id == id)
            return true;
        
    }

    pthread_mutex_unlock(&account_array_mutex);

    return false;
}

int createBankOffices(unsigned int quantity){

    offices[0].id = MAIN_THREAD_ID;
    offices[0].tid = pthread_self();

    for(unsigned int i = 1; i <= quantity; i++){

        pthread_create(&offices[i].tid,NULL,bank_office_func_stub,NULL);
        offices[i].id = i;
    }

    return 0;
}

void *bank_office_func_stub(void *stub){


    while(true){

        sem_wait(&full);
        pthread_mutex_lock(&request_queue_mutex);

        tlv_request_t currentRequest = queue_pop(&requests);

        pthread_mutex_unlock(&request_queue_mutex);
        sem_post(&empty);

        handleRequest(currentRequest);
    }


    return stub;
}

int checkRequestHeader(req_header_t header){

    bank_account_t* account = findBankAccount(header.account_id);

    if(account == NULL)
        return -1;

    if(!passwordIsCorrect(account,header.password))
        return -2;

    return 0;
}

bool passwordIsCorrect(bank_account_t* account,char* pwd){

    char temp[MAX_PASSWORD_LEN+SALT_LEN];

    strcpy(temp,pwd);
    strcat(temp,account->salt);

    generateSHA256sum(temp,temp);

    if(strcmp(temp,account->hash) == 0)
        return true;
    else
        return false;

}

int handleRequest(tlv_request_t request){

    enum op_type type = request.type;
    // uint32_t size = request.length;
    req_header_t header = request.value.header;
    // tlv_reply_t reply;

    if(checkRequestHeader(header) != 0)
        return -1;

    switch (type)
    {
    case OP_CREATE_ACCOUNT:

        break;

    case OP_BALANCE:
    
        break;

    case OP_TRANSFER:
    
        break;

    case OP_SHUTDOWN:
    
        break;
    
    default:
        break;
    }

    // enviar resposta

    return 0;
}


int setupRequestFIFO(){

    mkfifo("/tmp/secure_srv",REQUEST_FIFO_PERM);
    request_fifo_fd = open("/tmp/secure_srv",O_RDWR);

    return 0;
}

int waitForRequests(){

    while(true){

        tlv_request_t received_request;
        read(request_fifo_fd,&received_request,sizeof(tlv_reply_t));

        sem_wait(&empty);
        pthread_mutex_lock(&request_queue_mutex);

        queue_push(&requests,received_request);

        pthread_mutex_unlock(&request_queue_mutex);
        sem_post(&full);

        handleRequest(received_request);
    }

    return 0;

}

int initSyncMechanisms(size_t thread_cnt){

    sem_init(&empty,0,thread_cnt);
    sem_init(&full,0,0);

    return 0;
}
