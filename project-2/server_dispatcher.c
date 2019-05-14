#include "server_dispatcher.h"




int setupRequestFIFO()
{
    mkfifo(SERVER_FIFO_PATH, REQUEST_FIFO_PERM);
    request_fifo_fd = open(SERVER_FIFO_PATH, O_RDONLY);
    request_fifo_fd_DUMMY = open(SERVER_FIFO_PATH, O_WRONLY);   

    return 0;
}

int waitForRequests()
{
    while (true)
    {
        tlv_request_t received_request;

        read(request_fifo_fd, &received_request, sizeof(tlv_request_t));
        
        logRequest(log_file_fd,pthread_self(),&received_request);

        sem_wait(&empty);
        pthread_mutex_lock(getRequest_Queue_Mutex());

        queue_push(&requests, received_request);

        pthread_mutex_unlock(getRequest_Queue_Mutex());
        sem_post(&full);

    }

    return 0;
}

