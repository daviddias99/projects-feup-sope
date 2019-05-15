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
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "debug.h"
 
void alarm_handler(int signo);

void printErrorMessage(int error);

int readCommand(user_command_t* user_command);

int setupRequestFIFO();

int setupResponseFIFO();

int closeRequestFIFO();

int closeResponseFIFO();

int closeComunication();

int recordOperation(tlv_request_t* request, tlv_reply_t* reply);

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