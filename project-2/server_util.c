#include "server_util.h"
#include <semaphore.h>

bank_account_array_t accounts;
int request_fifo_fd;
int request_fifo_fd_DUMMY;

sem_t empty;
sem_t full;
pthread_mutex_t request_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t account_array_mutex = PTHREAD_MUTEX_INITIALIZER;
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

    char temp[MAX_PASSWORD_LEN + SALT_LEN];

    strcpy(temp, password);
    strcat(temp, newBankAccount.salt);

    generateSHA256sum(temp, newBankAccount.hash);

    return newBankAccount;
}

bank_account_t findBankAccount(uint32_t id)
{

    pthread_mutex_lock(&account_array_mutex);

    for (unsigned int i = 0; i < accounts.next_account_index; i++)
    {

        bank_account_t currentAccount = accounts.array[i];
        print_dbg("current account id %d\n", currentAccount.account_id);


        if (currentAccount.account_id == id){

            pthread_mutex_unlock(&account_array_mutex);        
            return currentAccount;
        }
            
    }

    pthread_mutex_unlock(&account_array_mutex);


    return errorAccount();
}

bank_account_t errorAccount(){

    bank_account_t error;

    error.account_id = ERROR_ACCOUNT_ID;

    return error;
}

int generateSHA256sum(char *str, char *result)
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

    write(fd2[WRITE], str, strlen(str));
    close(fd2[WRITE]);
    read(fd1[READ], result, SHA256_SIZE);
    close(fd1[READ]);

    result[SHA256_SIZE] = '\0';

    return 0;
}

int insertBankAccount(bank_account_t newAccount)
{
    pthread_mutex_lock(&account_array_mutex);

    if (accounts.next_account_index == MAX_BANK_ACCOUNTS){

        pthread_mutex_unlock(&account_array_mutex);
        return -1;
    }
    if (existsBankAccount(newAccount.account_id)){

        pthread_mutex_unlock(&account_array_mutex);
        return -2;
    }
        
    accounts.array[accounts.next_account_index++] = newAccount;
    
    pthread_mutex_unlock(&account_array_mutex);

    return 0;
}

bool existsBankAccount(uint32_t id)
{
    // pthread_mutex_lock(&account_array_mutex);

    for (unsigned int i = 0; i < accounts.next_account_index; i++)
    {

        if (accounts.array[i].account_id == id){

            pthread_mutex_unlock(&account_array_mutex);
            return true;
        }
            
    }

    // pthread_mutex_unlock(&account_array_mutex);

    return false;
}

int createBankOffices(unsigned int quantity)
{

    offices[0].id = MAIN_THREAD_ID;
    offices[0].tid = pthread_self();

    for (unsigned int i = 1; i <= quantity; i++)
    {

        pthread_create(&offices[i].tid, NULL, bank_office_func_stub, NULL);
        offices[i].id = i;
    }

    return 0;
}

void *bank_office_func_stub(void *stub)
{
    //int semval = 0;

    while (true)
    {   
      /*  sem_getvalue(&full, &semval);
        print_dbg("%ld before wait full %d\n", pthread_self(), semval);*/
        sem_wait(&full); 
        pthread_mutex_lock(&request_queue_mutex);

        tlv_request_t currentRequest = queue_pop(&requests);

        pthread_mutex_unlock(&request_queue_mutex);
        sem_post(&empty);
       /* sem_getvalue(&empty, &semval);
        print_dbg("%ld after post empty %d\n", pthread_self(), semval);*/
        handleRequest(currentRequest);
    }

    return stub;
}

int checkRequestHeader(req_header_t header)
{

    print_dbg("account id %d\n", header.account_id);
    bank_account_t account = findBankAccount(header.account_id);

    if (account.account_id == ERROR_ACCOUNT_ID)
        return -1;

    if (!passwordIsCorrect(account, header.password))
        return -2;

    return 0;
}

bool passwordIsCorrect(bank_account_t account, char *pwd)
{

    char temp[MAX_PASSWORD_LEN + SALT_LEN];

    strcpy(temp, pwd);
    strcat(temp, account.salt);

    generateSHA256sum(temp, temp);

    if (strcmp(temp, account.hash) == 0)
        return true;
    else
        return false;
}

int op_createAccount(req_value_t request_value, tlv_reply_t *reply)
{

    req_header_t header = request_value.header;

    reply->type = OP_CREATE_ACCOUNT;

    if (header.account_id != ADMIN_ACCOUNT_ID)
    {

        reply->value.header.ret_code = RC_OP_NALLOW;

        return -1;
    }

    bank_account_t newAccount = createBankAccount(request_value.create.account_id, request_value.create.password, request_value.create.balance);

    if (insertBankAccount(newAccount) == ERROR_ACCOUNT_LIMIT_EXCEEDED)
    {

        reply->value.header.ret_code = RC_OTHER;

        return -2;
    }

    reply->value.header.account_id = request_value.header.account_id;
    reply->value.header.ret_code = RC_OK;

    return 0;
}

int op_checkBalance(req_value_t request_value, tlv_reply_t *reply)
{

    req_header_t header = request_value.header;

    reply->type = OP_BALANCE;

    if (header.account_id == ADMIN_ACCOUNT_ID)
    {

        reply->value.header.ret_code = RC_OP_NALLOW;

        return -1;
    }

    bank_account_t account = findBankAccount(request_value.header.account_id);


    reply->value.header.account_id = request_value.header.account_id;
    reply->value.header.ret_code = RC_OK;
    reply->value.balance.balance = account.balance;

    return 0;
}

int op_transfer(req_value_t request_value, tlv_reply_t *reply)
{

    req_header_t header = request_value.header;
    bank_account_t* source = NULL,*dest = NULL;

    reply->type = OP_TRANSFER;

    if (header.account_id == ADMIN_ACCOUNT_ID)
    {

        reply->value.header.ret_code = RC_OP_NALLOW;

        return -1;
    }

    if(header.account_id == request_value.transfer.account_id){

        reply->value.header.ret_code = RC_SAME_ID;

        return -2;
    }

    pthread_mutex_lock(&account_array_mutex);

    for (unsigned int i = 0; i < accounts.next_account_index; i++)
    {

        if (accounts.array[i].account_id == request_value.transfer.account_id){

            dest = &accounts.array[i]; 
        }

        if (accounts.array[i].account_id == request_value.header.account_id){

            source = &accounts.array[i]; 
        }
            
    }

    if(dest == NULL){

        pthread_mutex_unlock(&account_array_mutex);
        reply->value.header.ret_code = RC_ID_NOT_FOUND;

        return -3;
    }

    if(source->balance < request_value.transfer.amount){

        pthread_mutex_unlock(&account_array_mutex);
        reply->value.header.ret_code = RC_NO_FUNDS;

        return -4;
    }

        
    if(dest->balance + request_value.transfer.amount > MAX_BALANCE){

        pthread_mutex_unlock(&account_array_mutex);
        reply->value.header.ret_code = RC_TOO_HIGH;

        return -5;
    }

    dest->balance += request_value.transfer.amount;
    source->balance -= request_value.transfer.amount;

    pthread_mutex_unlock(&account_array_mutex);


    reply->value.header.account_id = request_value.header.account_id;
    reply->value.header.ret_code = RC_OK;

    return 0;
}

int handleRequest(tlv_request_t request)
{
    enum op_type type = request.type;
    req_header_t header = request.value.header;

    tlv_reply_t reply;
    reply.length = sizeof(tlv_reply_t);
    reply.value.header.account_id = request.value.header.account_id;

    int headerCheckStatus = checkRequestHeader(header);
/*
    print_dbg("pthread_self %ld\n", pthread_self());*/
    print_dbg("header status 1: %d\n", headerCheckStatus);

    if (headerCheckStatus != 0)
    {

        if (headerCheckStatus == -1)
            reply.value.header.ret_code = RC_ID_NOT_FOUND;

        if (headerCheckStatus == -2)
            reply.value.header.ret_code = RC_LOGIN_FAIL;
    }
    else
    {
        switch (type)
        {
        case OP_CREATE_ACCOUNT:

            op_createAccount(request.value, &reply);

            break;

        case OP_BALANCE:

            op_checkBalance(request.value, &reply);

            break;

        case OP_TRANSFER:

            op_transfer(request.value, &reply);

            break;

        case OP_SHUTDOWN:

            break;

        default:
            break;
        }
    }

    print_dbg("header status 2: %d\n", headerCheckStatus);


    // enviar resposta

    char user_id[WIDTH_ID+1];
    char fifo_name[USER_FIFO_PATH_LEN] = USER_FIFO_PATH_PREFIX;
    sprintf(user_id,"%05d", request.value.header.pid);
    strcat(fifo_name,user_id);


    int reply_fifo_status = open(fifo_name,O_WRONLY | O_NONBLOCK);

    if(reply_fifo_status == ENXIO)
        return -1;


    write(fifo_name,&reply,sizeof(tlv_reply_t));

    return 0;
}

int setupRequestFIFO()
{
    mkfifo(SERVER_FIFO_PATH, REQUEST_FIFO_PERM);
    request_fifo_fd = open(SERVER_FIFO_PATH, O_RDONLY);

    request_fifo_fd_DUMMY = open(SERVER_FIFO_PATH, O_WRONLY);   

    return 0;
}

int waitForRequests()
{
    while (true)
    {
       // int semval = 0;

        tlv_request_t received_request;
        read(request_fifo_fd, &received_request, sizeof(tlv_request_t));
/*
        print_dbg("--- %s \n",received_request.value.header.password);
        sem_getvalue(&empty, &semval);
        print_dbg("%ld before wait empty %d\n", pthread_self(), semval);*/
        sem_wait(&empty);
        pthread_mutex_lock(&request_queue_mutex);

        queue_push(&requests, received_request);

        pthread_mutex_unlock(&request_queue_mutex);
        sem_post(&full);
       /* sem_getvalue(&full, &semval);
        
        print_dbg("%ld after post full %d\n", pthread_self(), semval);*/
    }

    return 0;
}

int initSyncMechanisms(size_t thread_cnt)
{

    sem_init(&empty, 0, thread_cnt);
    sem_init(&full, 0, 0);

    return 0;
}
