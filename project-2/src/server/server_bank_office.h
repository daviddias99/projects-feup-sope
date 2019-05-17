#ifndef SERVER_BANK_OFFICE_H
#define SERVER_BANK_OFFICE_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "server_accounts.h"
#include "server_util.h"
#include "server_operations.h"
#include "../constants.h"
#include "../types.h"
#include "../debug.h"
#include "../sope.h"
#include "../request_queue.h"

typedef struct bank_office{         // TODO: mudar nome

    pthread_t tid;
    uint32_t id;

}bank_office_t;

RequestQueue_t requests;


int createBankOffices(unsigned int quantity);
void *bank_office_service_routine(void *officeIDPtr);


int waitForRequests();
int setupRequestFIFO();
int sendReply(tlv_request_t request, tlv_reply_t reply, uint32_t officeID);

int handleRequest(tlv_request_t request,uint32_t officeID);
int checkRequestHeader(tlv_request_t req);
bool passwordIsCorrect(bank_account_t account,char* pwd);

int shutdown_server();
int getActiveThreadCount();
int closeOffices();

#endif
