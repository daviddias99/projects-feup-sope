#ifndef SERVER_SYNC_H
#define SERVER_SYNC_H

#include <semaphore.h>

#include "../sope.h"
#include "../constants.h"
#include "server_util.h"

extern sem_t empty;
extern sem_t full;

/**
 * @brief Initializes the full and empt semaphores used to direct the request "producing and consuming"
 * 
 * @param thread_cnt    number of threads in the server (excluding the main thread)
 * @return int          zero upon sucess, non-zero otherwise
 */
int initSyncMechanisms(size_t thread_cnt);

#endif
