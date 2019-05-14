#ifndef SERVER_OFFICE_MANAGING_H
#define SERVER_OFFICE_MANAGING_H

#include "types.h"
#include "pthread.h"
#include "stdbool.h"
#include "server_util.h"
#include "server_sync.h"
#include "server_acc_managing.h"

typedef struct bank_office{         // TODO: mudar nome

    pthread_t tid;
    uint32_t id;

}bank_office_t;

bank_office_t offices[MAX_BANK_OFFICES];


int createBankOffices(unsigned int quantity);
void *bank_office_func_stub(void *stub);
int op_createAccount(req_value_t request_value, tlv_reply_t* reply);
int op_checkBalance(req_value_t request_value, tlv_reply_t* reply);
int op_transfer(req_value_t request_value, tlv_reply_t* reply);

int sendReply(tlv_request_t request, tlv_reply_t reply);
int handleRequest(tlv_request_t request);
int checkRequestHeader(req_header_t header);
bool passwordIsCorrect(bank_account_t account,char* pwd);


#endif