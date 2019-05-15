#ifndef SERVER_OLD_H
#define SERVER_OLD_H

#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "stdbool.h"
#include "constants.h"
#include "types.h"
#include "string.h"
#include "time.h"
#include "pthread.h"
#include "request_queue.h"
#include "sys/types.h"
#include "sys/stat.h"

#include "debug.h"
#include "sope.h"
// ---------------
#include "server_accounts.h"
#include "server_util.h"
#include "server_operations.h"


typedef struct bank_office{         // TODO: mudar nome

    pthread_t tid;
    uint32_t id;

}bank_office_t;

RequestQueue_t requests;






int createBankOffices(unsigned int quantity);
void *bank_office_func_stub(void *stub);



int waitForRequests();
int setupRequestFIFO();
int sendReply(tlv_request_t request, tlv_reply_t reply);

int handleRequest(tlv_request_t request);
int checkRequestHeader(req_header_t header);
bool passwordIsCorrect(bank_account_t account,char* pwd);


#endif
