#include "server_operations.h"

int op_createAccount(req_value_t request_value, tlv_reply_t *reply,uint32_t officeID)
{

    req_header_t header = request_value.header;

    reply->type = OP_CREATE_ACCOUNT;

    if (header.account_id != ADMIN_ACCOUNT_ID)
    {

        reply->value.header.ret_code = RC_OP_NALLOW;

        return -1;
    }

    bank_account_t newAccount = createBankAccount(request_value.create.account_id, request_value.create.password, request_value.create.balance);

    if (insertBankAccount(newAccount,request_value.header.op_delay_ms,officeID) == ERROR_ACCOUNT_LIMIT_EXCEEDED)
    {

        reply->value.header.ret_code = RC_OTHER;

        return -2;
    }

    reply->value.header.account_id = request_value.header.account_id;
    reply->value.header.ret_code = RC_OK;

    return 0;
}

int op_checkBalance(req_value_t request_value, tlv_reply_t *reply,uint32_t officeID)
{

    req_header_t header = request_value.header;

    reply->type = OP_BALANCE;

    if (header.account_id == ADMIN_ACCOUNT_ID)
    {

        reply->value.header.ret_code = RC_OP_NALLOW;

        return -1;
    }

    bank_account_t account = *findBankAccount(request_value.header.account_id);

    reply->value.header.account_id = request_value.header.account_id;
    reply->value.header.ret_code = RC_OK;

    pthread_mutex_lock(&account_mutex[reply->value.header.account_id]);

    logDelay(getLogfile(),officeID,header.op_delay_ms);
    usleep(header.op_delay_ms);
    reply->value.balance.balance = account.balance;

    pthread_mutex_unlock(&account_mutex[reply->value.header.account_id]);

    return 0;
}

int op_transfer(req_value_t request_value, tlv_reply_t *reply,uint32_t officeID)
{

    req_header_t header = request_value.header;
    bank_account_t *source = NULL, *dest = NULL;

    UNUSED(officeID);

    reply->type = OP_TRANSFER;

    if (header.account_id == ADMIN_ACCOUNT_ID)
    {

        reply->value.header.ret_code = RC_OP_NALLOW;

        return -1;
    }

    if (header.account_id == request_value.transfer.account_id)
    {

        reply->value.header.ret_code = RC_SAME_ID;

        return -2;
    }

    if (!existsBankAccount(request_value.transfer.account_id))
    {

        reply->value.header.ret_code = RC_ID_NOT_FOUND;

        return -3;
    }

    dest = findBankAccount(request_value.transfer.account_id);
    source = findBankAccount(request_value.header.account_id);

    if (source->balance < request_value.transfer.amount)
    {

        reply->value.header.ret_code = RC_NO_FUNDS;

        return -4;
    }

    if (dest->balance + request_value.transfer.amount > MAX_BALANCE)
    {

        reply->value.header.ret_code = RC_TOO_HIGH;

        return -5;
    }

    // TODO: REMOVER DEADLOCK

    pthread_mutex_lock(&account_mutex[request_value.transfer.account_id]);
    pthread_mutex_lock(&account_mutex[request_value.header.account_id]);

    dest->balance += request_value.transfer.amount;
    source->balance -= request_value.transfer.amount;

    reply->value.transfer.balance = source->balance;

    pthread_mutex_unlock(&account_mutex[request_value.transfer.account_id]);
    pthread_mutex_unlock(&account_mutex[request_value.header.account_id]);

    reply->value.header.account_id = request_value.header.account_id;
    reply->value.header.ret_code = RC_OK;

    return 0;
}

int op_shutdown(req_value_t request_value, tlv_reply_t* reply,uint32_t officeID){

    UNUSED(officeID);

    req_header_t header = request_value.header;

    reply->type = OP_SHUTDOWN;

    if (header.account_id != ADMIN_ACCOUNT_ID)
    {

        reply->value.header.ret_code = RC_OP_NALLOW;

        return -1;
    }

    reply->value.header.ret_code = RC_OK;

    return 0;
}
