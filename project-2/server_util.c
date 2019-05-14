#include "server_util.h"
#include <semaphore.h>

bank_account_t accounts[MAX_BANK_ACCOUNTS];
pthread_mutex_t account_mutex[MAX_BANK_ACCOUNTS];
int request_fifo_fd;
int request_fifo_fd_DUMMY;
int log_file_fd;

sem_t empty;
sem_t full;
pthread_mutex_t request_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t account_array_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_file_mutex = PTHREAD_MUTEX_INITIALIZER;
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
    return accounts[id];
}


int initAccounts(){

    for(size_t i = 0; i < MAX_BANK_ACCOUNTS;i++){

        pthread_mutex_init(&account_mutex[i],NULL);
        accounts[i] = errorAccount();

    }

    return 0;
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

    if (newAccount.account_id >= MAX_BANK_ACCOUNTS){

        pthread_mutex_unlock(&account_array_mutex);
        return -1;
    }
    if (existsBankAccount(newAccount.account_id)){

        pthread_mutex_unlock(&account_array_mutex);
        return -2;
    }
        
    accounts[newAccount.account_id] = newAccount;

    pthread_mutex_lock(&log_file_mutex);
    logAccountCreation(log_file_fd,pthread_self(),&accounts[newAccount.account_id]);
    pthread_mutex_unlock(&log_file_mutex);
    
    pthread_mutex_unlock(&account_array_mutex);

    return 0;
}

bool existsBankAccount(uint32_t id)
{

    if(id >= MAX_BANK_ACCOUNTS)
        return false;

    if (accounts[id].account_id != ERROR_ACCOUNT_ID)
        return true;
        
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

        pthread_mutex_lock(&log_file_mutex);
        logBankOfficeOpen(log_file_fd,i,offices[i].tid);
        pthread_mutex_unlock(&log_file_mutex);
    }

    return 0;
}

void *bank_office_func_stub(void *stub)
{

    while (true)
    {   

        sem_wait(&full); 
        pthread_mutex_lock(&request_queue_mutex);

        tlv_request_t currentRequest = queue_pop(&requests);

        pthread_mutex_lock(&log_file_mutex);
        logRequest(log_file_fd,pthread_self(),&currentRequest);
        pthread_mutex_unlock(&log_file_mutex);

        pthread_mutex_unlock(&request_queue_mutex);
        sem_post(&empty);

        handleRequest(currentRequest);
    }

    return stub;
}

int checkRequestHeader(req_header_t header)
{

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


    if(!existsBankAccount(request_value.transfer.account_id)){

        reply->value.header.ret_code = RC_ID_NOT_FOUND;

        return -3;
    }

    dest = &accounts[request_value.transfer.account_id];
    source = &accounts[request_value.header.account_id];


    if(source->balance < request_value.transfer.amount){

        reply->value.header.ret_code = RC_NO_FUNDS;

        return -4;
    }

        
    if(dest->balance + request_value.transfer.amount > MAX_BALANCE){

        reply->value.header.ret_code = RC_TOO_HIGH;

        return -5;
    }

    // TODO: REMOVER DEADLOCK

    pthread_mutex_lock(&account_mutex[request_value.transfer.account_id]);
    pthread_mutex_lock(&account_mutex[request_value.header.account_id]);

    dest->balance += request_value.transfer.amount;
    source->balance -= request_value.transfer.amount;

    pthread_mutex_unlock(&account_mutex[request_value.transfer.account_id]);
    pthread_mutex_unlock(&account_mutex[request_value.header.account_id]);


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

    sendReply(request,reply);    

    return 0;
}


int sendReply(tlv_request_t request, tlv_reply_t reply){

    char user_id[WIDTH_ID+1];
    char reply_fifo_name[USER_FIFO_PATH_LEN] = USER_FIFO_PATH_PREFIX;
    sprintf(user_id,"%05d", request.value.header.pid);
    strcat(reply_fifo_name,user_id);

    int reply_fifo_fd = open(reply_fifo_name,O_WRONLY);

    if(reply_fifo_fd == -1){

        return -1;
    }
    
    pthread_mutex_lock(&log_file_mutex);
    logReply(log_file_fd,pthread_self(),&reply);
    pthread_mutex_unlock(&log_file_mutex);

    write(reply_fifo_fd,&reply,sizeof(tlv_reply_t));
    close(reply_fifo_fd);

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
        tlv_request_t received_request;

        read(request_fifo_fd, &received_request, sizeof(tlv_request_t));
        
        pthread_mutex_lock(&log_file_mutex);
        logRequest(log_file_fd,pthread_self(),&received_request);
        pthread_mutex_unlock(&log_file_mutex);

        sem_wait(&empty);
        pthread_mutex_lock(&request_queue_mutex);

        queue_push(&requests, received_request);

        pthread_mutex_unlock(&request_queue_mutex);
        sem_post(&full);

    }

    return 0;
}

int initSyncMechanisms(size_t thread_cnt)
{

    logSyncMechSem(log_file_fd,pthread_self(),SYNC_OP_SEM_INIT,SYNC_ROLE_PRODUCER,offices[MAIN_THREAD_ID].id,thread_cnt);
    sem_init(&empty, 0, thread_cnt);
    logSyncMechSem(log_file_fd,pthread_self(),SYNC_OP_SEM_INIT,SYNC_ROLE_PRODUCER,offices[MAIN_THREAD_ID].id,0);
    sem_init(&full, 0, 0);

    return 0;
}


int openLogFile(){


    log_file_fd = open(SERVER_LOGFILE,O_WRONLY | O_APPEND,0660);

    return 0;
}
