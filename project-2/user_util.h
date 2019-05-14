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

#define NEG_ACCOUNT_ID      1
#define LRG_ACCOUNT_ID      2
#define SRT_PASSWORD        3
#define LNG_PASSWORD        4
#define BAD_PASSWORD        5
#define NEG_DELAY           6
#define LNG_DELAY           7
#define INV_OPERATION       8
#define INV_NEW_ACCOUNT_ID  9
#define SRT_BALANCE         10
#define LRG_BALANCE         11
#define INV_NEW_PASSWORD    12
#define INV_TRS_ACCOUNT     13
#define SML_TRSF_AMOUNT     14
#define LRG_TRSF_AMOUNT     15
#define INV_OP_ARGUMENTS    16
#define LOG_REQUEST_ERROR   17
#define LOG_REPLY_ERROR     18

static const char* ERROR_MESSAGES[] = {
    [OK] = "Operation successful",
    [NEG_ACCOUNT_ID] = "Account ID can't be negative.",
    [LRG_ACCOUNT_ID] = "Account ID too large.",
    [SRT_PASSWORD] = "Password was too short.",
    [LNG_PASSWORD] = "Password was too long.",
    [BAD_PASSWORD] = "Password can't contain blank spaces.",
    [NEG_DELAY] = "Operation delay can't be negative or zero.",
    [LNG_DELAY] = "Operation delay was too long.",
    [INV_OPERATION] = "Choosen operation doesn't exist.",
    [INV_NEW_ACCOUNT_ID] = "New account ID is not accepted.",
    [SRT_BALANCE] = "New account balance is too small.",
    [LRG_BALANCE] = "New account balance is too large.",
    [INV_NEW_PASSWORD] = "New account password is invalid.",
    [INV_TRS_ACCOUNT] = "Recipient account ID is not accepted.",
    [SML_TRSF_AMOUNT] = "Transfering amount is too small.",
    [LRG_TRSF_AMOUNT] = "Transfering amount is too large.",
    [INV_OP_ARGUMENTS] = "Operation doesn't accept those arguments.",
    [LOG_REQUEST_ERROR] = "Failed to record request on user log file.",
    [LOG_REPLY_ERROR] = "Failed to record reply on user log file.",
};

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

int validAccount(char* accountID);

int validPassword(char* password);

int validDelay(char* delay);

int validOperation(char* operation);

int validArguments(char* arguments);

int validCreationOperation(char* arguments);

int validBalanceOperation(char* arguments);

int validTransferOperation(char* arguments);

int validShutdownOperation(char* arguments);

int checkArguments(char* accountID, char* password, char* delay, char* operation, char* arguments);

int formatReqCreateAccount(req_value_t* request_value);

int formatReqTransfer(req_value_t* request_value);

int formatReqHeader(req_header_t* header);

int formatReqValue(req_value_t* request_value);

int formatRequest(tlv_request_t* request);

int sendRequest(tlv_request_t* request);

int formatRepBalanceAccount(rep_value_t* reply_value);

int formatRepTransfer(rep_value_t* reply_value);

int formatRepShutdownTransfer(rep_value_t* reply_value);

int formatRepHeader(rep_header_t* header, int ret_code);

int formatRepValue(rep_value_t* reply_value, int ret_code);

int formatReply(tlv_reply_t* reply, int ret_code);

int recordError(int ret_code);

int waitResponse(tlv_request_t* request, tlv_reply_t* reply);

#endif