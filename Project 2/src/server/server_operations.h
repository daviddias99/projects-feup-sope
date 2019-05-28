#ifndef SERVER_OPERATIONS_H
#define SERVER_OPERATIONS_H

#include "../sope.h"
#include "../constants.h"
#include "../debug.h"
#include "server_accounts.h"

/**
 * @brief Executes the operation of creating an account
 * 
 * @param request_value     user request
 * @param reply             server reply
 * @param officeID          office performing the operation
 * @return int              zero upon sucess non-zero otherwise
 */
int op_createAccount(req_value_t request_value, tlv_reply_t* reply, uint32_t officeID);

/**
 * @brief Executes the operation of checking and account's balance
 * 
 * @param request_value     user request
 * @param reply             server reply
 * @param officeID          office performing the operation
 * @return int              zero upon sucess non-zero otherwise
 */
int op_checkBalance(req_value_t request_value, tlv_reply_t* reply, uint32_t officeID);

/**
 * @brief Executes the operation of transfering funds between two accounts
 * 
 * @param request_value     user request
 * @param reply             server reply
 * @param officeID          office performing the operation
 * @return int              zero upon sucess non-zero otherwise
 */
int op_transfer(req_value_t request_value, tlv_reply_t* reply, uint32_t officeID);

/**
 * @brief Executes the operation of shutting down the server (if allowed)
 * 
 * @param request_value     user request
 * @param reply             server reply
 * @param officeID          office performing the operation
 * @return int              zero upon sucess non-zero otherwise
 */
int op_shutdown(req_value_t request_value, tlv_reply_t* reply, uint32_t officeID);

#endif
