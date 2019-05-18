#include "server_operations.h"

int op_createAccount(req_value_t request_value, tlv_reply_t *reply,uint32_t officeID)
{

    reply->type = OP_CREATE_ACCOUNT;
    reply->length = sizeof(rep_header_t);


    bank_account_t newAccount = createBankAccount(request_value.create.account_id, request_value.create.password, request_value.create.balance);

    if (insertBankAccount(newAccount,request_value.header.op_delay_ms,officeID) == RC_ID_IN_USE)
    {
        reply->value.header.ret_code = RC_ID_IN_USE;

        return -1;
    }

    reply->value.header.account_id = request_value.header.account_id;
    reply->value.header.ret_code = RC_OK;

    return 0;
}

int op_checkBalance(req_value_t request_value, tlv_reply_t *reply,uint32_t officeID)
{

    req_header_t header = request_value.header;
    reply->length = sizeof(rep_header_t) + sizeof(rep_balance_t);

    reply->type = OP_BALANCE;

    bank_account_t account = *findBankAccount(request_value.header.account_id);

    reply->value.header.account_id = request_value.header.account_id;
    reply->value.header.ret_code = RC_OK;

    
    pthread_mutex_lock(&account_mutex[reply->value.header.account_id]);
    logSyncMech(getLogfile(),officeID,SYNC_OP_MUTEX_LOCK,SYNC_ROLE_ACCOUNT,request_value.header.account_id);

    logSyncDelay(getLogfile(),officeID, account.account_id, header.op_delay_ms);
    usleep(MS_TO_US(header.op_delay_ms));
    reply->value.balance.balance = account.balance;

    pthread_mutex_unlock(&account_mutex[reply->value.header.account_id]);
    logSyncMech(getLogfile(),officeID,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,request_value.header.account_id);



    return 0;
}

int op_transfer(req_value_t request_value, tlv_reply_t *reply,uint32_t officeID)
{
    bank_account_t *source = NULL, *dest = NULL;

    UNUSED(officeID);

    reply->type = OP_TRANSFER;
    reply->length = sizeof(rep_header_t) + sizeof(rep_transfer_t);
    
    dest = findBankAccount(request_value.transfer.account_id);
    source = findBankAccount(request_value.header.account_id);

    uint32_t first_lock_id, second_lock_id;

    if (request_value.transfer.account_id > request_value.header.account_id) {
        first_lock_id = request_value.transfer.account_id;
        second_lock_id = request_value.header.account_id;
    }
    else {
        first_lock_id = request_value.header.account_id;
        second_lock_id = request_value.transfer.account_id;    
    }


    pthread_mutex_lock(&account_mutex[first_lock_id]);
    logSyncMech(getLogfile(),officeID,SYNC_OP_MUTEX_LOCK,SYNC_ROLE_ACCOUNT,first_lock_id);
    logSyncDelay(getLogfile(), officeID, first_lock_id, request_value.header.op_delay_ms);

    pthread_mutex_lock(&account_mutex[second_lock_id]);
    logSyncMech(getLogfile(),officeID,SYNC_OP_MUTEX_LOCK,SYNC_ROLE_ACCOUNT,second_lock_id);
    logSyncDelay(getLogfile(), officeID, second_lock_id, request_value.header.op_delay_ms);
    usleep(MS_TO_US(request_value.header.op_delay_ms));

    if (!existsBankAccount(request_value.transfer.account_id))
        reply->value.header.ret_code = RC_ID_NOT_FOUND;
    else if (source->balance < request_value.transfer.amount)
        reply->value.header.ret_code = RC_NO_FUNDS;
    else if (dest->balance + request_value.transfer.amount > MAX_BALANCE)
        reply->value.header.ret_code = RC_TOO_HIGH;
    else {    
        dest->balance += request_value.transfer.amount;
        source->balance -= request_value.transfer.amount;
    
        reply->value.header.ret_code = RC_OK;
    }

    reply->value.transfer.balance = source->balance;
    
    pthread_mutex_unlock(&account_mutex[first_lock_id]);
    logSyncMech(getLogfile(),officeID,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,first_lock_id);
    pthread_mutex_unlock(&account_mutex[second_lock_id]);
    logSyncMech(getLogfile(),officeID,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,second_lock_id);

    reply->value.header.account_id = request_value.header.account_id;

    return 0;
}

int op_shutdown(req_value_t request_value, tlv_reply_t* reply,uint32_t officeID){

    UNUSED(officeID);
    UNUSED(request_value);

    reply->type = OP_SHUTDOWN;
    reply->value.header.ret_code = RC_OK;
    reply->length = sizeof(rep_header_t) + sizeof(rep_shutdown_t);

    return 0;
}
