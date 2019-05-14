#include "server_sync.h"

pthread_mutex_t request_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t account_array_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t empty;
sem_t full;
RequestQueue_t requests;


int initSyncMechanisms(size_t thread_cnt)
{

    logSyncMechSem(getLogfileFD(),pthread_self(),SYNC_OP_SEM_INIT,SYNC_ROLE_PRODUCER,MAIN_THREAD_ID,thread_cnt);
    sem_init(&empty, 0, thread_cnt);
    logSyncMechSem(getLogfileFD(),pthread_self(),SYNC_OP_SEM_INIT,SYNC_ROLE_PRODUCER,MAIN_THREAD_ID,0);
    sem_init(&full, 0, 0);


    return 0;
}

pthread_mutex_t* getRequest_Queue_Mutex(){

    return &request_queue_mutex;
}
pthread_mutex_t* getAccount_Array_Mutex(){

    return &account_array_mutex;
}
