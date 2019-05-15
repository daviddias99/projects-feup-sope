#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "../constants.h"

int getLogfile();
int openLogFile();


bool passwordIsValid(char* password);
int randomBetween(int a, int b);
char getHexChar(unsigned int a);
int generateSalt(char* saltStr);
int generateSHA256sum(char* str, char* result);

#endif
