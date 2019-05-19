#ifndef USER_VALIDATION_H
#define USER_VALIDATION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../types.h"
#include "../debug.h"
#include "../sope.h"
#include "../constants.h"

/**
 * @brief Checks if the given acccountID is a valid ID and if sets
 * the user_command_t struct accordingly.
 * 
 * @param command       Struct to be altered with the accountID
 * @param accountID     Account ID to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int validUserAccount(user_command_t* command, char* accountID);

/**
 * @brief Checks if the given acccountID is a valid ID.
 * 
 * @param accountID     Account ID to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int validAccount(char* accountID);

/**
 * @brief Checks if the given password has a valid length and no
 * blanck spaces. Sets the command passed as argument accordingly.
 * 
 * @param command       Struct to be altered with the password
 * @param password      Password to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int validPassword(user_command_t* command, char* password);

/**
 * @brief Checks if the given password has a valid length and no
 * blanck spaces.
 * 
 * @param password      Password to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int validNewPassword(char* password);

/**
 * @brief Checks if the given delay is neither negative or zero nor to
 * long. Sets the command passed as argument accordingly
 * 
 * @param command       Struct to be altered with the delay
 * @param delay         Delay to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int validDelay(user_command_t* command, char* delay);

/**
 * @brief Checks if the given operation exists and if so, sets the 
 * command given accordingly.
 * 
 * @param command       Struct to be altered with the operation type
 * @param operation     Operation type to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int validOperation(user_command_t* command, char* operation);

/**
 * @brief Checks if the given balance is neither to small nor to 
 * large to be accepted.
 * 
 * @param value         Balance to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int validBalance(char* value);

/**
 * @brief Checks if the given tranfer amount is neither to small 
 * nor to large to be accepted.
 * 
 * @param value         Amount tranfered to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int validAmount(char* value);

/**
 * @brief Checks if the given arguments are valid given the choosen
 * operation. Sets the command struct given accordingly.
 * 
 * @param command       Struct to be changed with the arguments  
 * @param arguments     Arguments to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int validArguments(user_command_t* command, char* arguments);

/**
 * @brief Checks if the number of arguments passed are valid for
 * a create account operation.
 * 
 * @param arguments     Arguments to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise 
 */
int validCreationOperation(char* arguments);

/**
 * @brief Checks if the number of arguments passed are valid for
 * a balance check operation.
 * 
 * @param arguments     Arguments to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise 
 */
int validBalanceOperation(char* arguments);

/**
 * @brief Checks if the number of arguments passed are valid for
 * a tranfering operation.
 * 
 * @param arguments     Arguments to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise 
 */
int validTransferOperation(char* arguments);

/**
 * @brief Checks if the number of arguments passed are valid for
 * a shutdown operation.
 * 
 * @param arguments     Arguments to be checked
 * 
 * @return int          Returns 0 if valid, non 0 otherwise 
 */
int validShutdownOperation(char* arguments);

/**
 * @brief Checks if the command read from the command line is a valid
 * operation request. Fills the command struct given accordingly.
 * 
 * @param command       Struct to be filled
 * @param accountID     Account ID read from the console
 * @param password      Password read from the console
 * @param delay         Delay read from the console
 * @param operation     Operation type read from the console
 * @param arguments     Arguments read from the console
 * 
 * @return int          Returns 0 if valid, non 0 otherwise
 */
int checkArguments(user_command_t* command, char* accountID, char* password, char* delay, char* operation, char* arguments);

#endif
