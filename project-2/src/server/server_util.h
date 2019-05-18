#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <wait.h>

#include "../constants.h"
#include "../types.h"

/**
 * @brief Get the log file file descriptor
 * 
 * @return int      fd of the server log file
 */
int getLogfile();

/**
 * @brief Open the server log file
 *  
 * @return int      zero upon sucess non-zero otherwise
 */
int openLogFile();

/**
 * @brief Checks if the password is valid
 * 
 * @param password  password to check
 * @return true     password is valid
 * @return false    password is not valid
 */
bool passwordIsValid(char* password);

/**
 * @brief Get a random integer between two numbers
 * 
 * @param a         lower bound
 * @param b         upper bound
 * @return int      integer between a and b
 */
int randomBetween(int a, int b);

/**
 * @brief Get the hex char corresponding to the given digit  (1 to 15)
 * 
 * @param a         digit to convert (1 to 15)
 * @return char     corresponding hex char
 */
char getHexChar(unsigned int a);

/**
 * @brief Generate and hex salt
 * 
 * @param saltStr   string where the salt will be stored
 * @return int      zero upon sucess nonzero otherwise
 */
int generateSalt(char* saltStr);

/**
 * @brief Executes the sha256 co-process to generate the salt of a given string
 * 
 * @param str       string to hash
 * @param result    sha256 hash of str
 * @return int      zero upon sucess, non-zero otherwise
 */
int generateSHA256sum(char* str, char* result);


#endif
