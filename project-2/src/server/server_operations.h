#include "../sope.h"
#include "../constants.h"
#include "server_accounts.h"

int op_createAccount(req_value_t request_value, tlv_reply_t* reply);
int op_checkBalance(req_value_t request_value, tlv_reply_t* reply);
int op_transfer(req_value_t request_value, tlv_reply_t* reply);
