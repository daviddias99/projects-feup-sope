#include "server_old.h"
#include <semaphore.h>


int request_fifo_fd;
int request_fifo_fd_DUMMY;

sem_t empty;
sem_t full;
pthread_mutex_t request_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t log_file_mutex = PTHREAD_MUTEX_INITIALIZER;
bank_office_t offices[MAX_BANK_OFFICES];

int createBankOffices(unsigned int quantity)
{

    offices[0].id = MAIN_THREAD_ID;
    offices[0].tid = pthread_self();

    for (unsigned int i = 1; i <= quantity; i++)
    {
        offices[i].id = i;
        pthread_create(&offices[i].tid, NULL, bank_office_func_stub, (void *)&offices[i].id);
        logBankOfficeOpen(getLogfile(), i, offices[i].tid);
    }

    return 0;
}

void *bank_office_func_stub(void *stub)
{
    uint32_t officeID = *((uint32_t *)stub);
    int semValue;

    while (true)
    {

        sem_getvalue(&full, &semValue);
        logSyncMechSem(getLogfile(), pthread_self(), SYNC_OP_SEM_WAIT, SYNC_ROLE_CONSUMER, officeID, semValue);

        sem_wait(&full);

        pthread_mutex_lock(&request_queue_mutex);

        tlv_request_t currentRequest = queue_pop(&requests);

        logRequest(getLogfile(), pthread_self(), &currentRequest);

        pthread_mutex_unlock(&request_queue_mutex);
        sem_post(&empty);

        handleRequest(currentRequest);
    }

    return stub;
}

int checkRequestHeader(req_header_t header)
{
    print_location();

    if (!existsBankAccount(header.account_id))
        return -1;

    if (!passwordIsCorrect(*findBankAccount(header.account_id), header.password))
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
        {
            print_location();
            reply.value.header.ret_code = RC_ID_NOT_FOUND;
        }

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

    sendReply(request, reply);

    return 0;
}

int sendReply(tlv_request_t request, tlv_reply_t reply)
{

    char user_id[WIDTH_ID + 1];
    char reply_fifo_name[USER_FIFO_PATH_LEN] = USER_FIFO_PATH_PREFIX;
    sprintf(user_id, "%05d", request.value.header.pid);
    strcat(reply_fifo_name, user_id);

    int reply_fifo_fd = open(reply_fifo_name, O_WRONLY);

    if (reply_fifo_fd == -1)
    {

        return -1;
    }

    logReply(getLogfile(), pthread_self(), &reply);

    write(reply_fifo_fd, &reply, sizeof(tlv_reply_t));
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

    int semValue;
    tlv_request_t received_request;

    while (true)
    {

        read(request_fifo_fd, &received_request, sizeof(tlv_request_t));

        logRequest(getLogfile(), pthread_self(), &received_request);
        sem_getvalue(&empty, &semValue);
        logSyncMechSem(getLogfile(), pthread_self(), SYNC_OP_SEM_WAIT, SYNC_ROLE_PRODUCER, MAIN_THREAD_ID, semValue);
        sem_wait(&empty);

        pthread_mutex_lock(&request_queue_mutex);

        queue_push(&requests, received_request);

        pthread_mutex_unlock(&request_queue_mutex);

        sem_post(&full);
        sem_getvalue(&full, &semValue);
        logSyncMechSem(getLogfile(), pthread_self(), SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, MAIN_THREAD_ID, semValue);
    }

    return 0;
}


