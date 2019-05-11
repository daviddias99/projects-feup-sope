#ifndef USER_UTIL_H
#define USER_UTIL_H

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

#define READ 0
#define WRITE 1
#define SHA256_SIZE                   64
#define REQUEST_FIFO_PERM             0660
#define RESPONSE_FIFO_PERM            0660
#define ERROR_ACCOUNT_LIMIT_EXCEEDED  -2
#define ERROR_ACCOUNT_ID              (MAX_BANK_ACCOUNTS +1)

#define VALID_OPERATION               0
#define ERROR_INVALID_ACCOUNT         1
#define ERROR_INVALID_PASSWORD        2
#define ERROR_INVALID_DELAY           3
#define ERROR_INVALID_OPERATION       4
#define ERROR_INVALID_ARGUMENTS       5

int setupRequestFIFO();

int setupResponseFIFO();

bool validAccount(char* accountID);

bool validPassword(char* password);

bool validDelay(char* delay);

bool validOperation(char* operation);

bool validArguments(char* accountID, char* operation, char* arguments);

bool validCreationOperation(char* accountID, char* arguments);

bool validBalanceOperation(char* accountID, char* arguments);

bool validTransferOperation(char* accountID, char* arguments);

bool validShutdownOperation(char* accountID, char* arguments);

int checkArguments(char* accountID, char* password, char* delay, char* operation, char* arguments);

int formatCreateAccount(req_value_t* request_value, char* arguments);

int formatTranfer(req_value_t* request_value, char* arguments);

int formatHeader(req_header_t* header, char* accountID, char* password, char* op_delay);

int formatValue(req_value_t* request_value, char* accountID, char* password, char* delay, char* operation, char* arguments);

int formatRequest(tlv_request_t* request, char* accountID, char* password, char* delay, char* operation, char* arguments);

int sendRequest(tlv_request_t* request);

int waitResponse(tlv_reply_t* reply);

#endif