#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

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
#include "fcntl.h"
#include "debug.h"
#include "sope.h"

#define READ 0
#define WRITE 1
#define SHA256_SIZE                   64
#define REQUEST_FIFO_PERM             0660
#define ERROR_ACCOUNT_LIMIT_EXCEEDED  -2
#define ERROR_ACCOUNT_ID              (MAX_BANK_ACCOUNTS +1)



typedef struct bank_office{         // TODO: mudar nome

    pthread_t tid;
    uint32_t id;

}bank_office_t;

RequestQueue_t requests;

bank_account_t createBankAccount(uint32_t id, char* password, uint32_t balance);
bank_account_t createAdminBankAccount(char* password);
bank_account_t findBankAccount(uint32_t id);

bank_account_t errorAccount();
int initAccounts();

int openLogFile();

int createBankOffices(unsigned int quantity);
void *bank_office_func_stub(void *stub);

int insertBankAccount(bank_account_t newAccount);
bool existsBankAccount(uint32_t id);

bool passwordIsValid(char* password);
int randomBetween(int a, int b);
char getHexChar(unsigned int a);
int generateSalt(char* saltStr);
int generateSHA256sum(char* str, char* result);

int waitForRequests();
int setupRequestFIFO();
int initSyncMechanisms(size_t thread_cnt);
int sendReply(tlv_request_t request, tlv_reply_t reply);

int handleRequest(tlv_request_t request);
int checkRequestHeader(req_header_t header);
bool passwordIsCorrect(bank_account_t account,char* pwd);

int op_createAccount(req_value_t request_value, tlv_reply_t* reply);
int op_checkBalance(req_value_t request_value, tlv_reply_t* reply);
int op_transfer(req_value_t request_value, tlv_reply_t* reply);


#endif
