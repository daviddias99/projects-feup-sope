#ifndef USER_UTIL_H
#define USER_UTIL_H

#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "stdbool.h"
#include "sope.h"
#include "constants.h"
#include "types.h"
#include "signal.h"
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
#define CANCEL_ALARM                  0
#define UNUSED(x) (void)(x)

#define OK                  0
#define SRV_DOWN            1
#define SRV_TIMEOUT         2
#define USR_DOWN            3
#define LOGIN_FAIL          4
#define OP_NALLOW           5
#define ID_IN_USE           6
#define ID_NOT_FOUND        7
#define SAME_ID             8
#define NO_FUNDS            9
#define TOO_HIGH            10
#define OTHER               11

typedef struct user_command {
    uint32_t accountID;
    char password[MAX_PASSWORD_LEN + 1];
    uint32_t delay;
    op_type_t operation;
    char* arguments;
} user_command_t;
 
void alarm_handler(int signo);

int setupRequestFIFO();

int setupResponseFIFO();

int closeRequestFIFO();

int closeResponseFIFO();

int closeComunication();

int recordOperation(tlv_request_t* request, tlv_reply_t* reply);

bool validAccount(char* accountID);

bool validPassword(char* password);

bool validDelay(char* delay);

bool validOperation(char* operation);

bool validArguments(char* arguments);

bool validCreationOperation(char* arguments);

bool validBalanceOperation(char* arguments);

bool validTransferOperation(char* arguments);

bool validShutdownOperation(char* arguments);

int checkArguments(char* accountID, char* password, char* delay, char* operation, char* arguments);

int formatCreateAccount(req_value_t* request_value);

int formatTranfer(req_value_t* request_value);

int formatReqHeader(req_header_t* header);

int formatReqValue(req_value_t* request_value);

int formatRequest(tlv_request_t* request);

int sendRequest(tlv_request_t* request);

int formatRepHeader(rep_header_t* header, int ret_code);

int formatRepValue(rep_value_t* request_value, int ret_code);

int formatReply(tlv_reply_t* reply, int ret_code);

int recordError(int ret_code);

int waitResponse(tlv_request_t* request, tlv_reply_t* reply);

#endif