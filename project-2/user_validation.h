#ifndef USER_VALIDATION_H
#define USER_VALIDATION_H

#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "stdbool.h"
#include "sope.h"
#include "constants.h"
#include "types.h"
#include "signal.h"
#include "string.h"
#include "time.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "debug.h"

int validUserAccount(user_command_t* command, char* accountID);

int validAccount(char* accountID);

int validPassword(user_command_t* command, char* password);

int validNewPassword(char* password);

int validDelay(user_command_t* command, char* delay);

int validOperation(user_command_t* command, char* operation);

int validBalance(char* value);

int validAmount(char* value);

int validArguments(user_command_t* command, char* arguments);

int validCreationOperation(char* arguments);

int validBalanceOperation(char* arguments);

int validTransferOperation(char* arguments);

int validShutdownOperation(char* arguments);

int checkArguments(user_command_t* command, char* accountID, char* password, char* delay, char* operation, char* arguments);

#endif