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

typedef struct bank_office{         

    pthread_t tid;
    uint32_t id;

}bank_office_t;

RequestQueue_t requests;

/**
 * @brief Launches the bank office threads with the bank_office_service_routine.
 * 
 * @param quantity      number of threads to execute
 * @return int          0 upon success, non-zero otherwise
 */
int createBankOffices(unsigned int quantity);

/**
 * @brief Routine executed by the bank office threads. It takes requests from the request-queue and handles them,responding to the user when finished. It will do
 * this until a valid shutdown request is issued, to when it will finished executing the remaining requests in the queue. (CONSUMER)
 * 
 * @param officeIDPtr   pointer to the bank office struct corresponding to the office
 */
void *bank_office_service_routine(void *officeIDPtr);

/**
 * @brief Routine that handles the request arraival from users. It puts the requests in a queue to be taken and attended by offices. (PRODUCER)
 * 
 * @return int          zero upon success, non-zero otherwise
 */
int waitForRequests();

/**
 * @brief Setups the fifos need to start comunications between the users and the server
 * 
 * @return int          zero upon success, non-zero otherwise
 */
int setupRequestFIFO();

/**
 * @brief Function used to send the reply to a given request. It does the communication from the server to the user
 * 
 * @param request       request issued by a user
 * @param reply         reply to be sent to the user
 * @param officeID      id of the office that performs the operation
 * @return int          zero upon success, non-zero otherwise
 */
int sendReply(tlv_request_t request, tlv_reply_t reply, uint32_t officeID);

/**
 * @brief Function executed by the offices on requests. It analizes the request, performs the corresponding operations and sends the reply to the user (with sendReply)
 *  
 * @param request       request issued by a user
 * @param officeID      id of the office handling the request
 * @return int          zero upon success, non-zero otherwise
 */
int handleRequest(tlv_request_t request,uint32_t officeID);

/**
 * @brief Checks the header of a request issued by a user and checks it's validity (correct permissions given the operation, correct password, valid ID)
 * 
 * @param req           request issued by a user
 * @return int          zero upon success, non-zero otherwise
 */
int checkRequestHeader(tlv_request_t req);

/**
 * @brief Does the necessary operations (hashing) to check if the given password matchs the accoun'ts password
 * 
 * @param account       user account
 * @param pwd           password to be tested
 * @return true         password is valid
 * @return false        password is not valid
 */
bool passwordIsCorrect(bank_account_t account,char* pwd);

/**
 * @brief Performs the operations need to disable user communications with the server
 * 
 * @return int          zero upon success, non-zero otherwise
 */
int shutdown_server();

/**
 * @brief Get the number of active threads, that is, threads that are performing and operation on a request
 * 
 * @return int          number of active threads
 */
int getActiveThreadCount();

/**
 * @brief Closes the bank offices
 * 
 * @return int          zero upon success, non-zero otherwise
 */
int closeOffices();

/**
 * @brief Closes communication mechanisms
 * 
 * @return int          zero upon success, non-zero otherwise
 */
int closeCommunication();

#endif
