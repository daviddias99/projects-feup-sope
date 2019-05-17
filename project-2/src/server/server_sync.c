#include "server_sync.h"

sem_t empty;
sem_t full;

int initSyncMechanisms(size_t num_threads)
{
    logSyncMechSem(getLogfile(), pthread_self(), SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, MAIN_THREAD_ID, num_threads);

    if(sem_init(&empty, 0, num_threads) != 0){

        perror("Semaphore 'empty' error");
        return -1;
    }
        

    logSyncMechSem(getLogfile(), pthread_self(), SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, MAIN_THREAD_ID, 0);

    if(sem_init(&full, 0, 0) != 0){

        perror("Semaphore 'full' error");
        return -2;
    }

    return 0;
}