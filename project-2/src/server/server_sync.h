#ifndef SERVER_SYNC_H
#define SERVER_SYNC_H

#include <semaphore.h>

#include "../sope.h"
#include "../constants.h"
#include "server_util.h"

extern sem_t empty;
extern sem_t full;

int initSyncMechanisms(size_t thread_cnt);

#endif
