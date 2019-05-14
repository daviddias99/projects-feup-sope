#include "server_office_managing.h"



int createBankOffices(unsigned int quantity)
{

    offices[0].id = MAIN_THREAD_ID;
    offices[0].tid = pthread_self();

    for (unsigned int i = 1; i <= quantity; i++)
    {

        pthread_create(&offices[i].tid, NULL, bank_office_func_stub, NULL);
        offices[i].id = i;

        logBankOfficeOpen(getLogfileFD(),i,offices[i].tid);
    }

    return 0;
}

void *bank_office_func_stub(void *stub)
{

    while (true)
    {   

        sem_wait(&full); 
        pthread_mutex_lock(getRequest_Queue_Mutex());

        tlv_request_t currentRequest = queue_pop(&requests);

        logRequest(getLogfileFD(),pthread_self(),&currentRequest);

        pthread_mutex_unlock(getRequest_Queue_Mutex());
        sem_post(&empty);

        handleRequest(currentRequest);
    }

    return stub;
}

int op_createAccount(req_value_t request_value, tlv_reply_t *reply)
{
    print_location();

    req_header_t header = request_value.header;

    reply->type = OP_CREATE_ACCOUNT;

    if (header.account_id != ADMIN_ACCOUNT_ID)
    {

        reply->value.header.ret_code = RC_OP_NALLOW;

        return -1;
    }

    print_location();

    bank_account_t newAccount = createBankAccount(request_value.create.account_id, request_value.create.password, request_value.create.balance);


    print_location();

    if (insertBankAccount(newAccount) == ERROR_ACCOUNT_LIMIT_EXCEEDED)
    {

        reply->value.header.ret_code = RC_OTHER;

        return -2;
    }

    reply->value.header.account_id = request_value.header.account_id;
    reply->value.header.ret_code = RC_OK;

    print_location();

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

int checkRequestHeader(req_header_t header)
{

    bank_account_t account = findBankAccount(header.account_id);

    if (account.account_id == ERROR_ACCOUNT_ID)
        return -1;

    if (!passwordIsCorrect(account, header.password))
        return -2;

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

    print_location();

    if (headerCheckStatus != 0)
    {
        
        print_location();
        if (headerCheckStatus == -1) {
            reply.value.header.ret_code = RC_ID_NOT_FOUND;
            print_location();
        }

        if (headerCheckStatus == -2)
            reply.value.header.ret_code = RC_LOGIN_FAIL;
    }
    else
    {

        switch (type)
        {
        case OP_CREATE_ACCOUNT:

            print_location();

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
    
    logReply(getLogfileFD(),pthread_self(),&reply);

    write(reply_fifo_fd,&reply,sizeof(tlv_reply_t));
    close(reply_fifo_fd);

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

