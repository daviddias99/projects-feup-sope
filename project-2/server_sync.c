#include "server_sync.h"

sem_t empty;
sem_t full;

int initSyncMechanisms(size_t num_threads)
{
    logSyncMechSem(getLogfile(), pthread_self(), SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, MAIN_THREAD_ID, num_threads);
    sem_init(&empty, 0, num_threads);

    logSyncMechSem(getLogfile(), pthread_self(), SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, MAIN_THREAD_ID, 0);
    sem_init(&full, 0, 0);

    return 0;
}