#ifndef SERVER_SYNC_H
#define SERVER_SYNC_H

#include "semaphore.h"
#include "pthread.h"
#include "server_util.h"
#include "sope.h"

extern sem_t empty;
extern sem_t full;

extern RequestQueue_t requests;

int initSyncMechanisms(size_t thread_cnt);

pthread_mutex_t* getRequest_Queue_Mutex();
pthread_mutex_t* getAccount_Array_Mutex();


#endif