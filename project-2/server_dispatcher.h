#ifndef SERVER_DISPATCHER_H
#define SERVER_DISPATCHER_H

#include "types.h"
#include "sope.h"
#include "server_office_managing.h"
#include "server_acc_managing.h"

int request_fifo_fd;
int request_fifo_fd_DUMMY;
int log_file_fd;

int waitForRequests();
int setupRequestFIFO();


#endif