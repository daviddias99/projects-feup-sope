#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
#include "request_queue.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "debug.h"
#include "pthread.h"
#include "semaphore.h"


int openLogfile();
int getLogfileFD();

bool passwordIsValid(char* password);
int randomBetween(int a, int b);
char getHexChar(unsigned int a);
int generateSalt(char* saltStr);
int generateSHA256sum(char* str, char* result);



#endif
